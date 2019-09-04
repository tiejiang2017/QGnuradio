/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "beklUdpSourece_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/prefs.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <setjmp.h>
#include<signal.h>
#include<threadexceptionhandle.h>
#include<linux/prctl.h>
#include <sys/prctl.h>
#include <execinfo.h>
extern void errorDump();
extern uint  getCurrentFuncReturnAddr();
extern void recvSignal(int sig);
    extern jmp_buf env_buf;
namespace gr {
  namespace beklBlk {

  const int beklUdpSourece_impl::BUF_SIZE_PAYLOADS =
      gr::prefs::singleton()->get_long("udp_blocks", "buf_size_payloads", 50);
    beklUdpSourece::sptr
    beklUdpSourece::make(int itemsize,const std::string& host, int port, int payload_size, bool eof)
    {
      return gnuradio::get_initial_sptr
        (new beklUdpSourece_impl(itemsize, host, port, payload_size, eof));
    }

    /*
     * The private constructor
     */
    beklUdpSourece_impl::beklUdpSourece_impl(int itemsize,const std::string& host, int port, int payload_size, bool eof)
      : gr::sync_block("beklUdpSourece",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, 1)),
        d_itemsize(itemsize),
        d_payload_size(payload_size),
        d_eof(eof),
        d_connected(false),
        d_residual(0),
        d_sent(0)
    {
        d_rxbuf = new char[4 * d_payload_size];
        d_residbuf = new char[BUF_SIZE_PAYLOADS * d_payload_size];

        _workImpl = new work_impl();
        connect(host, port);
    }

    /*
     * Our virtual destructor.
     */
    beklUdpSourece_impl::~beklUdpSourece_impl()
    {
//        printf("~beklUdpSourece_impl\n");
//        fflush(0);
        if (d_connected)
            disconnect();

        delete[] d_rxbuf;
        delete[] d_residbuf;
    }

    void beklUdpSourece_impl::connect(const std::string& host, int port)
    {
        if (d_connected)
            disconnect();

        d_host = host;
        d_port = static_cast<unsigned short>(port);

        std::string s_port;
        s_port = (boost::format("%d") % d_port).str();

        if (host.size() > 0) {
            boost::asio::ip::udp::resolver resolver(d_io_service);
            boost::asio::ip::udp::resolver::query query(
                d_host, s_port, boost::asio::ip::resolver_query_base::passive);
            d_endpoint = *resolver.resolve(query);

            d_socket = new boost::asio::ip::udp::socket(d_io_service);
            d_socket->open(d_endpoint.protocol());

            boost::asio::socket_base::reuse_address roption(true);
            d_socket->set_option(roption);

            boost::asio::socket_base::receive_buffer_size recv_option(1048576*20);
            d_socket->set_option(recv_option);

            d_socket->bind(d_endpoint);

            start_receive();
            d_udp_thread =
                gr::thread::thread(boost::bind(&beklUdpSourece_impl::run_io_service, this));
            d_connected = true;
        }
    }

    void beklUdpSourece_impl::disconnect()
    {
        gr::thread::scoped_lock lock(d_setlock);

        if (!d_connected)
            return;

        d_io_service.reset();
        d_io_service.stop();
        d_udp_thread.join();

        d_socket->close();
        delete d_socket;

        d_connected = false;
    }

    // Return port number of d_socket
    int beklUdpSourece_impl::get_port(void)
    {
        // return d_endpoint.port();
        return d_socket->local_endpoint().port();
    }

    void beklUdpSourece_impl::start_receive()
    {
        d_socket->async_receive_from(
            boost::asio::buffer((void*)d_rxbuf, d_payload_size),
            d_endpoint_rcvd,
            boost::bind(&beklUdpSourece_impl::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void beklUdpSourece_impl::handle_read(const boost::system::error_code& error,
                                      size_t bytes_transferred)
    {
        if (!error) {
            {
//                printf("udp read:%ld\n",bytes_transferred);
                boost::lock_guard<gr::thread::mutex> lock(d_udp_mutex);
                if (d_eof && (bytes_transferred == 0)) {
                    // If we are using EOF notification, test for it and don't
                    // add anything to the output.
                    d_residual = WORK_DONE;
                    d_cond_wait.notify_one();
                    return;
                } else {
                    // Make sure we never go beyond the boundary of the
                    // residual buffer.  This will just drop the last bit of
                    // data in the buffer if we've run out of room.
                    if ((int)(d_residual + bytes_transferred) >=
                        (BUF_SIZE_PAYLOADS * d_payload_size)) {
                        GR_LOG_WARN(d_logger, "Too much data; dropping packet.");
                        printf("BUF_SIZE_PAYLOADS :%d,recvBuf size:%d\n",BUF_SIZE_PAYLOADS,BUF_SIZE_PAYLOADS * d_payload_size);
                        fflush(NULL);
                    } else {
                        // otherwise, copy received data into local buffer for
                        // copying later.
                        memcpy(d_residbuf + d_residual, d_rxbuf, bytes_transferred);
                        d_residual += bytes_transferred;
                    }
                }
                d_cond_wait.notify_one();
            }
        }
        start_receive();
    }

//#define OUTPUT
#define TRY_IF  char name[32];\
    prctl(PR_GET_NAME, (unsigned long)name);\
    uint id = pthread_self(); \
    sigjmp_buf env;\
    int r = sigsetjmp(env,1);\
    if(r  == 0)\
    {\
       threadExceptionHandle::pushOne(name,env,id);\
    }\
    if(r==0)

#define IF_ERR else
//typedef int (beklUdpSourece_impl::*pFunType)(int noutput_items,
//                                              gr_vector_const_void_star &input_items,
//                                              gr_vector_void_star &output_items);
    typedef void (beklUdpSourece_impl::*pFunType)();

    template<typename dst_type,typename src_type>
    dst_type pointer_cast(src_type src)
    {
        return *static_cast<dst_type*>(static_cast<void*>(&src));
    }
static unsigned long int oldSize=0;
    int
    beklUdpSourece_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        //uint addr = getCurrentFuncReturnAddr();


        static unsigned long int size =0;
        //const char *in = (const <+ITYPE+> *) input_items[0];
        char*out = (char *) output_items[0];
        //TRY_IF
        {
            //初次执行 ，那么可以执行 可能会发生错误的代码
            printf("excute this code!!");
//            printf(" threadName:%s\n", name);
            fflush(0);
            //        void* p1 =  pointer_cast<void*>(&beklUdpSourece_impl::work);
            //        pFunType f= &gr::beklBlk::beklUdpSourece_impl::getThreadName;
            //        //void * p1 = static_cast<void*>(gr::beklBlk::beklUdpSourece_impl::work);
            //        std::cout<<"getThreadName:"<<f<<std::endl;
            //        uint *a =  static_cast<uint*>(static_cast<void *>(&f));
            //        uint b = *a;

            if(oldSize==0)
            {
                oldSize=9;
                _workImpl->woke_handle();
            }



        }
//        IF_ERR
//        {
//            //是由longjmp 跳转回来的
//            printf("jump this code !!");
//            printf(" threadName:%s\n", name);
//            delete _workImpl;
//            _workImpl = new work_impl();
//            _workImpl->woke_handle2();
//        }

  printf("work func quit!!\n");
  fflush(0);
    // Do <+signal processing+>
    boost::unique_lock<boost::mutex> lock(d_udp_mutex);

    // use timed_wait to avoid permanent blocking in the work function
    //unique_lock相对lock_guard更灵活的地方在于在等待中的线程如果在等待期间需要解锁mutex，并在之后重新将其锁定
    d_cond_wait.timed_wait(lock, boost::posix_time::milliseconds(10));

    if (d_residual < 0) {
        return d_residual;
    }


    int bytes_left_in_buffer = (int)(d_residual - d_sent);
#ifdef OUTPUT
    int bytes_to_send = std::min<int>(d_itemsize * noutput_items, bytes_left_in_buffer);
#else
    int bytes_to_send =bytes_left_in_buffer;
#endif
    size += bytes_to_send;
    if(oldSize != size)
    {
       // oldSize = size;
        printf("processed:%lu,%d\n",size,noutput_items);
        fflush(0);
    }


   #ifdef OUTPUT
    // Copy the received data in the residual buffer to the output stream
    memcpy(out, d_residbuf + d_sent, bytes_to_send);

    //int nitems = bytes_to_send / d_itemsize;
    int nitems = bytes_to_send / d_itemsize;
    #else
    int nitems =1;
#endif
    //size+=bytes_to_send;
    //printf("recv:%fMB\n",size/(float)(1024*1024));
    //fflush(0);
    // Keep track of where we are if we don't have enough output
    // space to send all the data in the residbuf.
    if (bytes_to_send == bytes_left_in_buffer) {
        d_residual = 0;
        d_sent = 0;
    } else {
        d_sent += bytes_to_send;
    }
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
//      return noutput_items;//
    return nitems;
    }

//    int beklUdpSourece_impl::error_handle(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
//    {

//        char name[32];
//        prctl(PR_GET_NAME, (unsigned long)name);
//        printf("error occured: %s-->work\n",name);
//        fflush(0);
//        return 0;
//    }

    void beklUdpSourece_impl::getThreadName()
    {
        char name[32];
        prctl(PR_GET_NAME, (unsigned long)name);
        printf("name:%s\n",name);
    }

  } /* namespace beklBlk */
} /* namespace gr */

