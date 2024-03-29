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

#ifndef INCLUDED_BEKLBLK_BEKLUDPSOURECE_IMPL_H
#define INCLUDED_BEKLBLK_BEKLUDPSOURECE_IMPL_H

#include <beklBlk/beklUdpSourece.h>
#include <gnuradio/thread/thread.h>
#include <boost/asio.hpp>
#include <boost/format.hpp>
namespace gr {
  namespace beklBlk {

  class work_impl
  {
  public:
      work_impl(){}
      ~work_impl(){}

      void woke_handle(){

//          int* s = 0;
//           (*s) = 1;
          printf("generate error\n");
          fflush(0);

          int a=1/0;
      }
      void woke_handle2(){
          printf("new work handle\n");
          fflush(0);
      }
  };


    class beklUdpSourece_impl : public beklUdpSourece
    {
     private:
      // Nothing to declare in this block.
        size_t d_itemsize;
        int d_payload_size; // maximum transmission unit (packet length)
        bool d_eof;         // look for an EOF signal
        bool d_connected;   // are we connected?
        char* d_rxbuf;      // get UDP buffer items
        char* d_residbuf;   // hold buffer between calls
        ssize_t d_residual; // hold information about number of bytes stored in residbuf
        ssize_t d_sent;     // track how much of d_residbuf we've outputted

        static const int
            BUF_SIZE_PAYLOADS; //!< The d_residbuf size in multiples of d_payload_size

        work_impl *_workImpl;
        std::string d_host;
        unsigned short d_port;

        boost::asio::ip::udp::socket* d_socket;
        boost::asio::ip::udp::endpoint d_endpoint;
        boost::asio::ip::udp::endpoint d_endpoint_rcvd;
        boost::asio::io_service d_io_service;

        gr::thread::condition_variable d_cond_wait;
        gr::thread::mutex d_udp_mutex;
        gr::thread::thread d_udp_thread;

        void start_receive();
        void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
        void run_io_service() { d_io_service.run(); }
     public:
      beklUdpSourece_impl(int itemsize,const std::string& host, int port, int payload_size, bool eof);
      ~beklUdpSourece_impl();
      void connect(const std::string& host, int port);
      void disconnect();

      int payload_size() { return d_payload_size; }
      int get_port();
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

//      int error_handle(int noutput_items,
//                       gr_vector_int& ninput_items,
//                       gr_vector_const_void_star& input_items,
//                       gr_vector_void_star& output_items);

      void getThreadName();
    };

  } // namespace beklBlk
} // namespace gr

#endif /* INCLUDED_BEKLBLK_BEKLUDPSOURECE_IMPL_H */

