/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H
#define INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H

#include <gnuradio/api.h>
#include <gnuradio/messages/msg_accepter1.h>
#include <pmt/pmt.h>

namespace gr {

/*!
 * \brief Accepts messages and inserts them into a message queue,
 * then notifies subclass gr::basic_block there is a message pending.
 */
class GR_RUNTIME_API msg_accepter : public gr::messages::msg_accepter
{
public:
    msg_accepter();
    ~msg_accepter();

    void post(pmt::pmt_t which_port, pmt::pmt_t msg);
};

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H */
