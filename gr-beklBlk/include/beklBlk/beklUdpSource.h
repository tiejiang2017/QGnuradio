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

#ifndef INCLUDED_BEKLBLOCK_BEKLUDPSOURCE_H
#define INCLUDED_BEKLBLOCK_BEKLUDPSOURCE_H

#include <beklBlk/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace beklBlock {

    /*!
     * \brief <+description of block+>
     * \ingroup beklBlock
     *
     */
    class BEKLBLK_API beklUdpSource : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<beklUdpSource> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of beklBlock::beklUdpSource.
       *
       * To avoid accidental use of raw pointers, beklBlock::beklUdpSource's
       * constructor is in a private implementation
       * class. beklBlock::beklUdpSource::make is the public interface for
       * creating new instances.
       */
      static sptr make(int itemsize,const std::string& host, int port, int payload_size, bool eof);
//      virtual void connect(const std::string& host, int port) = 0;

//      /*! \brief Cut the connection if we have one set up.
//       */
//      virtual void disconnect() = 0;

//      /*! \brief return the PAYLOAD_SIZE of the socket */
//      virtual int payload_size() = 0;

//      /*! \brief return the port number of the socket */
//      virtual int get_port() = 0;
    };

  } // namespace beklBlock
} // namespace gr

#endif /* INCLUDED_BEKLBLOCK_BEKLUDPSOURCE_H */

