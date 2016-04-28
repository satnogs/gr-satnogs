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

#ifndef INCLUDED_SATNOGS_UDP_MSG_SINK_IMPL_H
#define INCLUDED_SATNOGS_UDP_MSG_SINK_IMPL_H

#include <satnogs/udp_msg_sink.h>
#include <satnogs/log.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace gr
{
  namespace satnogs
  {

    class udp_msg_sink_impl : public udp_msg_sink
    {
    private:
      const std::string d_iface_addr;
      const uint16_t d_udp_port;
      const size_t d_mtu;
      int d_sock;
      struct sockaddr_in d_sin;

      void msg_handler(pmt::pmt_t msg);

    public:
      udp_msg_sink_impl (const std::string& addr, uint16_t port, size_t mtu);
      ~udp_msg_sink_impl ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_UDP_MSG_SINK_IMPL_H */

