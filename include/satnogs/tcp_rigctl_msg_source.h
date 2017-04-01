/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_H
#define INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Block that accepts TCP messages with rigctl commands. Depending
     * the command contents this block produces an appropriate PMT message
     * to control other blocks in the flowgraph
     * \ingroup satnogs
     *
     */
    class SATNOGS_API tcp_rigctl_msg_source : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<tcp_rigctl_msg_source> sptr;


      /**
       * Rigctl TCP command accepter
       *
       * @param addr the address of the interface to listen at
       * @param port the TCP port to listen or connect
       * @param serve_mode If set to yes this block, act as a rigctl server.
       * Otherwise as a rigctl client
       * @param interval_ms The interval in milliseconds at which the client
       * request the frequency from the rigctl
       * @param mtu the maximum MTU
       * @return
       */
      static sptr
      make (const std::string& addr, uint16_t port, bool server_mode,
	    size_t interval_ms = 1000, size_t mtu = 1500);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_H */

