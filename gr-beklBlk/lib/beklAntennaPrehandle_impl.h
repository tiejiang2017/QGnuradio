/* -*- c++ -*- */
/*
 * Copyright 2019 gr-beklBlock author.
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

#ifndef INCLUDED_BEKLBLOCK_BEKLANTENNAPREHANDLE_IMPL_H
#define INCLUDED_BEKLBLOCK_BEKLANTENNAPREHANDLE_IMPL_H

#include <beklBlk/beklAntennaPrehandle.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define LEN_FRAME_HEAD 16
#define LEN_RLTEX 2048
#define LEN_ALL 512 * 10000 * 4
#define INIT_j  j0 = 0; \
        j1 = 0; \
        j2 = 0; \
        j3 = 0; \
        j4 = 0;

namespace gr {
  namespace beklBlock {

    class beklAntennaPrehandle_impl : public beklAntennaPrehandle
    {
     private:
      // Nothing to declare in this block.
        std::string  CHECK_GPS_INI;// "package.ini"
        std::string  magdecINI;
        char     recvbuf[LEN_RLTEX];
        char     comBuf[LEN_RLTEX * 5];
        int      frameNumber ;
        int      nframeNumber, pframeNumber;
        int      udfh;
        int      udfe;
        int      frameLen;
        long int frequency;
        char     ch;
        int      j0, j1, j2, j3, j4;
        int      ndataNum;
        int      dataNum;
        char*    sumCorr;
        uint outputLen;
        int GetIniKeyString(char *dst, const char *title, const char *key, const char *filename);
        int GetIniKeyFloatArray(const char *title, const char *key, float *array, int arraySize, const char *filename);
        long long GetIniKeyInt(const char *title, const char *key, const char *filename);
        float GetIniKeyFloat(const char *title, const char *key, const char *filename);
        int correct_gps(char *mybuf);
        int correct_magdec(char *mybuf);
        int func_run(const char *inputbuf, char *outputBuf, uint *outputLen);
    public:
      beklAntennaPrehandle_impl(const std::string& packageIniPath,const std::string& magdecIniPath);
      ~beklAntennaPrehandle_impl();

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items);
      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_int& ninput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace beklBlock
} // namespace gr

#endif /* INCLUDED_BEKLBLOCK_BEKLANTENNAPREHANDLE_IMPL_H */

