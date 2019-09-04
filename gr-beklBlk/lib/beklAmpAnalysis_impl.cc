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
#include "beklAmpAnalysis_impl.h"

namespace gr {
  namespace beklBlk {

    beklAmpAnalysis::sptr
    beklAmpAnalysis::make(int frameType)
    {
      return gnuradio::get_initial_sptr
        (new beklAmpAnalysis_impl(frameType));
    }

    /*
     * The private constructor
     */
    beklAmpAnalysis_impl::beklAmpAnalysis_impl(int frameType)
      : gr::sync_block("beklAmpAnalysis",
              gr::io_signature::make(1, 1, 1),
              gr::io_signature::make(1, 1, 1))
    {
        bool r = rngBuf.rngCreate(1024*1024*2);
        if(r==false)
        {
            printf("rngCreate error \n");
            fflush(stdout);
        }
    }

    /*
     * Our virtual destructor.
     */
    beklAmpAnalysis_impl::~beklAmpAnalysis_impl()
    {
        rngBuf.rngDelete();
//        rngDelete(rngBuf);
    }

    int
    beklAmpAnalysis_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];

        // Do <+signal processing+>
        uint r = rngBuf.rngBufPut((char*)in,noutput_items);
        if(r!=noutput_items)
        {
            printf("rngBufPut err\n");
        }
        //dealUdpMsg();

        boost::this_thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        for (int i = 0; i < noutput_items; ++i)
        {
          out[i] = in[i]  ;
        }

      // Tell runtime system how many output items we produced.
      return noutput_items;
//        return 0;
    }
    void beklAmpAnalysis_impl::dealUdpMsg()
    {
        long long recvLen;
        //char *buf=(char*)malloc(1024*1024);
        char tmp[1024*2] = {0};

        AMP_STRUCT *h = (AMP_STRUCT*)tmp;
    //    qDebug()<<"UDP recv  ....";
        uint head = (h->head);


        while(1)
        {
            if(rngBuf.rngNBytes()< sizeof(AMP_STRUCT))
                break;

            rngBuf.rngBufGetNoDel(&tmp[0],sizeof(AMP_STRUCT));

            head = h->head;
            if (head != 0x55444648)
            {
                rngBuf.rngBufGet(&tmp[0],1);
                //printf(" %02X ",(unsigned char)tmp[0]);
            }
            else
            {
                int need = 1024;
                if(h->reserves == 2024)
                {
                    need = 2048;
                }else if (h->reserves == 1000)
                {
                    need = 1024;
                }
                if((uint)rngBuf.rngNBytes()>=need)
                {


                    int n = rngBuf.rngBufGet(tmp,need);
                    if(n!=need)
                    {
                        printf("err: rngBufGet,n!=need");
                    }
                    printf("type:0x%08x,%d,frameCnt:%d\n",h->frameType,h->reserves,h->frameCnt);
                    fflush(0);

                }
                else
                {
                    break;
                }
            }

        }

         printf("dealUdpMsg done\n");
        fflush(stdout);
    }

  } /* namespace beklBlk */
} /* namespace gr */

