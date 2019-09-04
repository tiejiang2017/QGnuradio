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

#ifndef INCLUDED_BEKLBLK_BEKLAMPANALYSIS_IMPL_H
#define INCLUDED_BEKLBLK_BEKLAMPANALYSIS_IMPL_H

#include <beklBlk/beklAmpAnalysis.h>
#include<beklBlk/RngLib.h>

#pragma pack(1)
typedef struct
{
    uint frameCnt;
    uint head;
    uint reserves;
    uint frameType;

    uint timeStampH;
    uint timeStampL;
    uint channelFreqH;
    uint channelFreqL;
    uint channelDbH;
    uint channelDbL;//10
    uint ampFrame[236];//118*2
    uint a124[2];//
    uint a125[2];
    uint a126[2];
    uint a127[2];
    uint a128;
    uint tail;
}AMP_STRUCT;
#pragma pack()
namespace gr {
  namespace beklBlk {

    class beklAmpAnalysis_impl : public beklAmpAnalysis
    {
     private:
      // Nothing to declare in this block.
    beklRngLib rngBuf;
     public:
      beklAmpAnalysis_impl(int frameType);
      ~beklAmpAnalysis_impl();
    void dealUdpMsg();
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace beklBlk
} // namespace gr

#endif /* INCLUDED_BEKLBLK_BEKLAMPANALYSIS_IMPL_H */

