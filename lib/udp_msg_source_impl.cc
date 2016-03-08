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
#include "udp_msg_source_impl.h"
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

    udp_msg_source::sptr
    udp_msg_source::make (const std::string& addr, uint16_t port, size_t mtu)
    {
      return gnuradio::get_initial_sptr (
	  new udp_msg_source_impl (addr, port, mtu));
    }

    /*
     * The private constructor
     */
    udp_msg_source_impl::udp_msg_source_impl (const std::string& addr,
					      uint16_t port,
					      size_t mtu) :
	    gr::block ("udp_msg_source",
		       gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
	    d_iface_addr (addr),
	    d_udp_port (port),
	    d_mtu(mtu),
	    d_running (true)
    {
      message_port_register_out(pmt::mp("msg"));
      boost::shared_ptr<boost::thread> (
	  new boost::thread (
	      boost::bind (&udp_msg_source_impl::udp_msg_accepter, this)));
    }

    void
    udp_msg_source_impl::udp_msg_accepter ()
    {
      int sock;
      struct sockaddr_in sin;
      struct sockaddr client_addr;
      socklen_t client_addr_len;
      ssize_t ret;
      uint8_t *buf;

      if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror ("opening UDP socket");
	exit (EXIT_FAILURE);
      }

      memset (&client_addr, 0, sizeof(struct sockaddr));
      memset (&sin, 0, sizeof(struct sockaddr_in));
      sin.sin_family = AF_INET;
      sin.sin_port = htons (d_udp_port);

      if( inet_aton(d_iface_addr.c_str(), &(sin.sin_addr)) == 0){
	LOG_ERROR("Wrong IP address");
	close(sock);
	exit (EXIT_FAILURE);
      }

      if (bind (sock, (struct sockaddr *) &sin, sizeof(struct sockaddr_in))
	  == -1) {
	perror ("UDP bind");
	close(sock);
	exit (EXIT_FAILURE);
      }

      /* All good until now. Allocate buffer memory and proceed */
      buf = new uint8_t(d_mtu);

      while(d_running){
	ret = recvfrom(sock, buf, d_mtu, 0, &client_addr, &client_addr_len);
	if(ret > 0) {
	  message_port_pub(pmt::mp("msg"), pmt::make_blob(buf, ret));
	}
	else{
	  perror("UDP recvfrom");
	  close(sock);
	  delete buf;
	  exit(EXIT_FAILURE);
	}
      }
      close(sock);
      delete buf;
      exit (EXIT_SUCCESS);
    }

    /*
     * Our virtual destructor.
     */
    udp_msg_source_impl::~udp_msg_source_impl ()
    {
      d_running = false;
      d_thread->join ();
    }

  } /* namespace satnogs */
} /* namespace gr */

