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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "udp_msg_sink_impl.h"
#include <satnogs/log.h>

namespace gr {
  namespace satnogs {

    udp_msg_sink::sptr
    udp_msg_sink::make(const std::string& addr, uint16_t port, size_t mtu)
    {
      return gnuradio::get_initial_sptr
        (new udp_msg_sink_impl(addr, port, mtu));
    }

    /*
     * The private constructor
     */
    udp_msg_sink_impl::udp_msg_sink_impl (const std::string& addr,
					  uint16_t port, size_t mtu) :
	    gr::block ("udp_msg_sink",
		       gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
		       d_iface_addr(addr),
		       d_udp_port(port),
		       d_mtu(mtu)
    {
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"),
      		      boost::bind(&udp_msg_sink_impl::msg_handler,
      				  this, _1));

      /* Open the socket */
      if ((d_sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror ("opening UDP socket");
	throw std::runtime_error("Could not open UDP socket");
      }

      memset (&d_sin, 0, sizeof(struct sockaddr_in));
      d_sin.sin_family = AF_INET;
      d_sin.sin_port = htons (d_udp_port);

      if( inet_aton(d_iface_addr.c_str(), &(d_sin.sin_addr)) == 0){
	LOG_ERROR("Wrong IP address");
	close(d_sock);
	throw std::runtime_error("Wrong IP address");
      }
    }

    void
    udp_msg_sink_impl::msg_handler (pmt::pmt_t msg)
    {
      const void *buf = pmt::blob_data(msg);
      size_t len = pmt::blob_length(msg);

      if(len < d_mtu){
	sendto(d_sock, buf, len, 0, (sockaddr *) &d_sin, sizeof(sockaddr_in));
      }
    }

    /*
     * Our virtual destructor.
     */
    udp_msg_sink_impl::~udp_msg_sink_impl()
    {
      close(d_sock);
    }

  } /* namespace satnogs */
} /* namespace gr */

