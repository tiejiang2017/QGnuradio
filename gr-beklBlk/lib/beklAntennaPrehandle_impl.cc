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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "beklAntennaPrehandle_impl.h"
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
namespace gr {
  namespace beklBlock {

    beklAntennaPrehandle::sptr
    beklAntennaPrehandle::make(const std::string& packageIniPath,const std::string& magdecIniPath)
    {
      return gnuradio::get_initial_sptr
        (new beklAntennaPrehandle_impl(packageIniPath, magdecIniPath));
    }

    /*
       读取字符类型的值
     */
    int beklAntennaPrehandle_impl::GetIniKeyString(char* dst, const char* title, const char* key, const char* filename)
    {

        FILE*       fp = NULL;
        int         flag = 0;
        char        sTitle[32], *wTmp;
        static char sLine[1024];

        sprintf(sTitle, "[%s]", title);
        if (NULL == (fp = fopen(filename, "r")))
        {
            // fclose(fp);
            printf("filename:%s\n",filename);
            perror("fopen");
            return -1;
        }
        else
        {
            while (NULL != fgets(sLine, 1024, fp))
            {
                // 这是注释行
                if (0 == strncmp("//", sLine, 2))
                    continue;
                if ('#' == sLine[0])
                    continue;

                wTmp = strchr(sLine, '=');
                if ((NULL != wTmp) && (1 == flag))
                {
                    if (0 == strncmp(key, sLine, strlen(key)))
                    {  // 长度依文件读取的为准

                        if (sLine[strlen(sLine) - 2] == '\r')
                        {
                            sLine[strlen(sLine) - 2] = '\0';
                        }
                        else
                        {
                            sLine[strlen(sLine) - 1] = '\0';
                        }
                        fclose(fp);
                        strcpy(dst, wTmp + 1);
                        return 1;
                    }
                }
                else
                {
                    if (0 == strncmp(sTitle, sLine, strlen(sTitle)))
                    {              // 长度依文件读取的为准
                        flag = 1;  // 找到标题位置
                    }
                }
            }
        }
        fclose(fp);
        return 0;
    }

    /*
       读取数组类型的值
     */
    int beklAntennaPrehandle_impl::GetIniKeyFloatArray(const char* title, const char* key, float* array, int arraySize, const char* filename)
    {
        FILE*       fp = NULL;
        int         flag = 0;
        char        sTitle[32];
        static char sLine[1024];
        char*       p;
        int         i = 0;

        sprintf(sTitle, "[%s]", title);
        if (NULL == (fp = fopen(filename, "r")))
        {
            //  fclose(fp);
            printf("filename:%s\n",filename);
            perror("fopen");
            return -1;
        }
        else
        {
            while (NULL != fgets(sLine, 1024, fp))
            {
                // 这是注释行
                if (0 == strncmp("//", sLine, 2))
                    continue;
                if ('#' == sLine[0])
                    continue;

                if ((1 == flag) && 0 == strncmp(sLine, key, strlen(key)))  //找到键值位置
                {
                    p = strtok(sLine + strlen(key) + 1, ",");
                    while (i < arraySize)
                    {
                        if (p != NULL && *p != '\n' && *p != '\r')
                        {
                            array[i] = atof(p);
                            p = strtok(NULL, ",");
                            i++;
                        }
                        else
                        {
                            if (NULL == fgets(sLine, 1024, fp))
                            {
                                fclose(fp);
                                return -1;
                            }
                            else
                                p = strtok(sLine, ",");
                        }
                    }
                    fclose(fp);
                    return 1;
                }
                else
                {
                    if (0 == strncmp(sTitle, sLine, strlen(sTitle)))
                    {              // 长度依文件读取的为准
                        flag = 1;  // 找到标题位置
                    }
                }
            }
        }
        fclose(fp);
        return 0;
    }

    /*
       读取整数类型的值
     */
    long long beklAntennaPrehandle_impl::GetIniKeyInt(const char* title, const char* key, const char* filename)
    {
        char str[1024];
        if (GetIniKeyString(str, title, key, filename))
            return strtoll(str, NULL, 0);
        else
            return 0;
    }

    /*
       读取浮点型的值
     */
    float beklAntennaPrehandle_impl::GetIniKeyFloat(const char* title, const char* key, const char* filename)
    {
        char str[1024];
        if (GetIniKeyString(str, title, key, filename))
            return atof(str);
        else
            return 0;
    }

    int beklAntennaPrehandle_impl::correct_magdec(char* mybuf)
    {
        char   monitor_buf[100] = { 0 };
        int    ret;
        double lati = 0.0, loni = 0.0;
        FILE*  fp = fopen(magdecINI.data(), "r");
        if (fp == NULL)
        {
            // send_monitor(4, "open magedc.ini error.");
            perror("open magedc.ini");
        }
        char           buf[50];
        unsigned short old_angle = 0;
        int            year = -1, lat = -1, lon = -1;
        float          angle = -1000;

        int ctl_mag = GetIniKeyInt("GPS", "UseMaDe", CHECK_GPS_INI.data());

        old_angle = *(unsigned short*)(mybuf + 2048 - 16);

        if (ctl_mag == 0)
        {
            fclose(fp);
            return 0;
        }
        else if (ctl_mag == 1)
        {
            angle = GetIniKeyFloat("GPS", "MagneticDeclination", CHECK_GPS_INI.data());
            if (angle < -180 || angle > 180)
            {
                memset(monitor_buf, 0, 100);
                sprintf(monitor_buf, "the angle(%f) of package.ini -> MagneticDeclination error.", angle);
                // send_monitor(5, monitor_buf);
                printf("the angle of package.ini -> MagneticDeclination error\n");
                return -1;
            }
            (old_angle + (short)(angle * 100) >= 0) ? (*(unsigned short*)(mybuf + 2048 - 16) = old_angle + (short)(angle * 100))
                                                    : (*(unsigned short*)(mybuf + 2048 - 16) = old_angle + (short)(angle * 100) + 360 * 100);
        }

        else if (ctl_mag == 2)
        {
            unsigned char lo_j;

            int            latitude2;
            unsigned char  l_buf[20];
            int            longitude2;
            double         longitude3, latitude3;
            unsigned short longitude_d;
            unsigned char  latitude_d;
            unsigned char* l_num;
            unsigned char* l_num2;
            int            num_j = 0x80;

            l_num = (unsigned char*)(mybuf + 2048 - 24 + 4);

            lo_j = (*l_num) & (num_j);

            longitude_d = *(short*)(mybuf + 2048 - 24 + 3);
            longitude_d = longitude_d << 1;
            longitude_d = longitude_d >> 1;

            longitude2 = (*((int*)(mybuf + 2048 - 24)));
            longitude2 = longitude2 << 8;
            longitude2 = longitude2 >> 8;
            longitude3 = (double)(longitude2);
            if (lo_j == 0x80)
            {
                loni = -(longitude_d + (longitude3 / 100000) / 60);
            }

            else if (lo_j == 0)
            {

                loni = (longitude_d + (longitude3 / 100000) / 60);
            }

            latitude_d = (*((unsigned char*)(mybuf + 2048 - 16 + 4 + 3)));
            lo_j = latitude_d & num_j;

            latitude_d = latitude_d << 1;
            latitude_d = latitude_d >> 1;

            latitude2 = (*((int*)(mybuf + 2048 - 16 + 4)));
            latitude2 = latitude2 << 8;
            latitude2 = latitude2 >> 8;
            latitude3 = (double)(latitude2);

            if (lo_j == 0x80)
            {
                lati = (latitude_d + ((latitude3 / 100000) / 60));
            }

            else if (lo_j == 0)
            {
                lati = -(latitude_d + ((latitude3 / 100000) / 60));
            }

            while (1)
            {

                year = lat = lon = -1;
                angle = -1000.0;
                memset(buf, 0, 50);
                if (fgets(buf, 50, fp) == NULL)
                    break;
                if (strcmp(buf, "#START") == 0 || strcmp(buf, "#END") == 0)
                    continue;
                // printf("buf = %s",buf);
                sscanf(buf, "{%d, %d, %d, %f},", &year, &lat, &lon, &angle);
                if (lat == (int)(lati + 0.5) && lon == (int)(loni + 0.5))
                    break;
                // printf("year:%d lan:%d lon:%d angle:%f\n\n",year,lan,lon,angle);
            }

            fclose(fp);
            if (angle < -180.0 || angle > 180.0)
            {
                memset(monitor_buf, 0, 100);
                sprintf(monitor_buf, "magdec.ini MagneticDeclination(%f) error.", angle);
                // send_monitor(5, monitor_buf);
                printf("the angle of MagneticDeclination error\n");
                return -1;
            }

            *(unsigned char*)(mybuf + 2048 - 16 + 2) = 0x55;
            //*(unsigned short *)(mybuf + 2048 - 16 ) = old_angle + (unsigned short)(angle * 100);
            (old_angle + (short)(angle * 100) >= 0) ? (*(unsigned short*)(mybuf + 2048 - 16) = old_angle + (short)(angle * 100))
                                                    : (*(unsigned short*)(mybuf + 2048 - 16) = old_angle + (short)(angle * 100) + 360 * 100);
        }
        else
        {
            memset(monitor_buf, 0, 100);
            sprintf(monitor_buf, "package.ini UseMaDe(%d) error.", ctl_mag);
            // send_monitor(5, monitor_buf);
        }

        if (*(unsigned char*)(mybuf + 2048 - 16 + 2) == 0x55)
        {
            *(unsigned char*)(mybuf + 2048 - 16 + 2) = 0x56;
        }
        else if (*(unsigned char*)(mybuf + 2048 - 16 + 2) == 0x66)
        {
            *(unsigned char*)(mybuf + 2048 - 16 + 2) = 0x67;
        }

        else
        {
            memset(monitor_buf, 0, 100);
            sprintf(monitor_buf, "angle_info_valid(%x) error.", *(unsigned char*)(mybuf + 2048 - 16 + 2));
            // send_monitor(5, monitor_buf);
            printf("%s\n", monitor_buf);
        }
        return 0;
    }

    int beklAntennaPrehandle_impl::correct_gps(char* mybuf)
    {
        unsigned char gps_valid;
        unsigned char angle_info_valid;

        int ctl_gps = GetIniKeyInt("GPS", "UseFile", CHECK_GPS_INI.data());           //控制经纬度and time
        int ctl_azi = GetIniKeyInt("GPS", "UseSystemAzimuth", CHECK_GPS_INI.data());  //控制角度
        int time_zone = 0;
        int ctl_zone = GetIniKeyInt("GPS", "UseTimeZone", CHECK_GPS_INI.data());
        int my_zone = GetIniKeyInt("GPS", "TimeZone", CHECK_GPS_INI.data());

        angle_info_valid = *(unsigned char*)(mybuf + 2048 - 16 + 2);
        gps_valid = *(unsigned char*)(mybuf + 2048 - 24 + 5);
        struct timeval timep;
        struct tm*     p = NULL;
        struct tm*     p_tm_time;
        time_t         time_utc = 0;

        p_tm_time = localtime(&time_utc);
        time_zone = (p_tm_time->tm_hour > 12) ? (p_tm_time->tm_hour -= 24) : p_tm_time->tm_hour;

        gettimeofday(&timep, NULL);
        p = localtime(&timep.tv_sec);
        unsigned char num_j = 0;
        unsigned char num_j2 = 0;
        float         my_longitude = GetIniKeyFloat("GPS", "longitude", CHECK_GPS_INI.data());
        float         my_latitude = GetIniKeyFloat("GPS", "latitude", CHECK_GPS_INI.data());
        float         my_SystemAzimuth = GetIniKeyFloat("GPS", "SystemAzimuth", CHECK_GPS_INI.data());

        if ((ctl_azi == 0) || (ctl_azi == 2 && angle_info_valid != 0x55))  //使用文件罗盘角
        {
            *(unsigned char*)(mybuf + 2048 - 16 + 2) = 0x66;
            *(unsigned short*)(mybuf + 2048 - 16) = (short)(my_SystemAzimuth * 100);
        }

        if ((ctl_gps == 0) || (ctl_gps == 2 && gps_valid != 0x55))  //使用文件GPS
        {
            if ((ctl_zone == 0) && (my_zone != time_zone))  //年月日暂未处理
            {
                p->tm_hour = p->tm_hour + (my_zone - time_zone);

                if (p->tm_hour < 0)
                {
                    p->tm_hour = p->tm_hour + 24;
                }
                else if (p->tm_hour > 24)
                {
                    p->tm_hour = p->tm_hour - 24;
                }
            }

            *(unsigned char*)(mybuf + 22) = p->tm_hour;
            *(unsigned char*)(mybuf + 21) = p->tm_min;
            *(unsigned char*)(mybuf + 20) = p->tm_sec;
            *(unsigned int*)(mybuf + 16) = timep.tv_usec * 1000;
            *(unsigned char*)(mybuf + 2048 - 24 + 5) = 0x66;

            *(unsigned int*)(mybuf + 2048 - 24) = (int)((my_longitude - (int)(my_longitude)) * 100000 * 60);
            *(unsigned short*)(mybuf + 2048 - 24 + 3) = (short)(my_longitude);

            *(unsigned int*)(mybuf + 2048 - 16 + 4) = (int)((my_latitude - (int)(my_latitude)) * 100000 * 60);
            *(unsigned char*)(mybuf + 2048 - 16 + 4 + 3) = (short)(my_latitude);

            num_j2 = *(unsigned char*)(mybuf + 2048 - 24 + 4);
            if (my_longitude < 0)
            {
                num_j = 0x80;
                *(unsigned char*)(mybuf + 2048 - 24 + 4) = num_j2 | num_j;
            }
            else
            {
                num_j = 0x7F;
                *(unsigned char*)(mybuf + 2048 - 24 + 4) = num_j2 & num_j;
            }

            num_j2 = *(unsigned char*)(mybuf + 2048 - 16 + 4 + 3);
            if (my_latitude > 0)
            {
                num_j = 0x80;
                *((unsigned char*)(mybuf + 2048 - 16 + 4 + 3)) = num_j2 | num_j;
            }
            else
            {
                num_j = 0x7F;
                *((unsigned char*)(mybuf + 2048 - 16 + 4 + 3)) = num_j2 & num_j;
            }
        }

        else
        {
            if ((ctl_zone == 0) && (my_zone != time_zone))
            {
                char myhour = *(unsigned char*)(mybuf + 22);
                *(unsigned char*)(mybuf + 22) = myhour + my_zone;
                myhour = *(unsigned char*)(mybuf + 22);
                if (myhour < 0)
                {
                    *(unsigned char*)(mybuf + 22) = myhour + 24;
                }
                else if (myhour > 24)
                {
                    *(unsigned char*)(mybuf + 22) = myhour - 24;
                }
            }

            else
            {
                char myhour = *(unsigned char*)(mybuf + 22);
                *(unsigned char*)(mybuf + 22) = myhour + time_zone;
                myhour = *(unsigned char*)(mybuf + 22);
                if (myhour < 0)
                {
                    *(unsigned char*)(mybuf + 22) = myhour + 24;
                }
                else if (myhour > 24)
                {
                    *(unsigned char*)(mybuf + 22) = myhour - 24;
                }
            }
        }
        return 0;
    }
    int beklAntennaPrehandle_impl::func_run(const char* inputbuf,char *outputBuf,uint *outputLen)
    {
        //接收
        *outputLen = 0;
        int inputSize = 4096;
        memset(recvbuf, 0, LEN_RLTEX);
        memcpy(recvbuf, inputbuf, LEN_RLTEX);
        frameNumber = *(int*)(recvbuf + 0);
        udfh = *(int*)(recvbuf + 4);
        frameLen = *(int*)(recvbuf + 8);
        udfe = *(int*)(recvbuf + LEN_FRAME_HEAD + frameLen);
        ch = *(recvbuf + 12) - '0';



        frequency = *(long int*)(recvbuf + 24);
        if (frequency == 0)
            return inputSize;


        if (udfh != 0x55444648)
        {
            printf("handle.udfh error(%x)\n", udfh);
            return inputSize;
        }


        if (udfe != 0x55444645)
        {
            printf("handle.udfe error(%x)\n", udfe);
            return inputSize;
        }

        if (ch == 1 && j0 == 0)
        {
            nframeNumber = frameNumber;
            memcpy(comBuf + 0 * LEN_RLTEX, recvbuf, LEN_RLTEX);
            j0++;
        }

        else if (ch == 2 && j1 == 0 && j0 == 1)
        {
            memcpy(comBuf + 1 * LEN_RLTEX, recvbuf, LEN_RLTEX);
            j1++;
        }

        else if (ch == 3 && j2 == 0 && j1 == 1)
        {
            memcpy(comBuf + 2 * LEN_RLTEX, recvbuf, LEN_RLTEX);
            j2++;
        }

        else if (ch == 4 && j3 == 0 && j2 == 1)
        {
            memcpy(comBuf + 3 * LEN_RLTEX, recvbuf, LEN_RLTEX);
            j3++;
        }

        else if (ch == 5 && j4 == 0 && j3 == 1)
        {
            memcpy(comBuf + 4 * LEN_RLTEX, recvbuf, LEN_RLTEX);
            j4++;
        }
        else
        {
            INIT_j;
            return inputSize;
        }

        if (j4 == 1)
        {
            INIT_j;
            if (nframeNumber - pframeNumber != 1)
            {
                memset(sumCorr, 0, LEN_ALL);
                ndataNum = 0;
            }

            memcpy(sumCorr + ndataNum * LEN_RLTEX * 5, comBuf, LEN_RLTEX * 5);
            pframeNumber = nframeNumber;
            ndataNum++;
            memset(comBuf, 0, LEN_RLTEX * 5);
            if (ndataNum == dataNum)
            {
                printf("6\n");
                fflush(0);
                correct_gps(sumCorr);
                correct_magdec(sumCorr);
                //发送
                printf("7\n");
                fflush(0);
                memcpy(outputBuf,sumCorr,LEN_ALL);//长度？
                *outputLen = LEN_ALL;
            }
        }


        return inputSize;
    }
    /*
     * The private constructor
     */
    beklAntennaPrehandle_impl::beklAntennaPrehandle_impl(const std::string& packageIniPath, const std::string& magdecIniPath)
      : gr::block("beklAntennaPrehandle",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, 20480000))
    {
        memset(recvbuf,0,LEN_RLTEX);
        memset(comBuf,0,LEN_RLTEX* 5);
        frameNumber = 0;
        nframeNumber = 0;
        pframeNumber = 0;
        udfh = 0;
        udfe = 0;
        frameLen = 0;
        frequency = 0;
        ch = 0;
        j0 = 0;
        j1 = 0;
        j2 = 0;
        j3 = 0;
        j4 = 0;
        ndataNum = 0;
        dataNum = 2000;
        sumCorr = (char*)malloc(LEN_ALL);
        CHECK_GPS_INI = packageIniPath;
        magdecINI = magdecIniPath;
        set_min_output_buffer(10*2);
        set_min_noutput_items(1);
    }

    /*
     * Our virtual destructor.
     */
    beklAntennaPrehandle_impl::~beklAntennaPrehandle_impl()
    {
        free(sumCorr);
    }

    int beklAntennaPrehandle_impl::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
    {
        //printf("beklAntennaPrehandle_impl::general_work\n");
        //fflush(0);
        int r = work(noutput_items, ninput_items,input_items, output_items);

        if (r > 0)
            consume_each(r);

        if (outputLen > 0)
        {
            printf("beklAntennaPrehandle_impl::general_work outPut:%u\n",outputLen);
            fflush(0);
        }

        return block::WORK_CALLED_PRODUCE;
    }

    int
    beklAntennaPrehandle_impl::work(int noutput_items, gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char *) output_items[0];
      int ninput = ninput_items[0];
      outputLen=0;
      int nitems = 0;
      printf("beklAntennaPrehandle_impl::work,noutput_items:%d\n",ninput);

      //printf("alias:%s",this->alias().data());


      if(ninput<4096)
          return 0;
      nitems = func_run(in,out,&outputLen);
//self.beklBlock_beklAntennaPrehandle_0.set_min_output_buffer(20480000)

      if(outputLen>0)
      {
        this->detail()->produce_each(outputLen/this->detail()->output(0)->get_sizeof_item());
        int bufsize =  this->detail()->output(0)->bufsize();
        int nitems_written =  this->detail()->output(0)->nitems_written();
        printf("bufsize:%d,nitems_written:%d\n",bufsize,nitems_written);
      }
      // Tell runtime system how many output items we produced.
      //return noutput_items;
      return nitems;
    }

  } /* namespace beklBlock */
} /* namespace gr */

