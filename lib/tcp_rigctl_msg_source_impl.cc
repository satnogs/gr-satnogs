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
#include "tcp_rigctl_msg_source_impl.h"
#include <satnogs/log.h>
#include <ifaddrs.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


namespace gr
{
  namespace satnogs
  {

    tcp_rigctl_msg_source::sptr
    tcp_rigctl_msg_source::make (const std::string& addr, uint16_t port,
				 size_t mtu)
    {
      return gnuradio::get_initial_sptr (
	  new tcp_rigctl_msg_source_impl (addr, port, mtu));
    }

    /*
     * The private constructor
     */
    tcp_rigctl_msg_source_impl::tcp_rigctl_msg_source_impl (
	const std::string& addr, uint16_t port, size_t mtu) :
	    gr::block ("tcp_rigctl_msg_source",
		       gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
	    d_iface_addr (addr),
	    d_port (port),
	    d_mtu (mtu),
	    d_running (true)
    {
      message_port_register_out (pmt::mp ("freq"));
      boost::shared_ptr<boost::thread> (
	  new boost::thread (
	      boost::bind (&tcp_rigctl_msg_source_impl::tcp_msg_accepter,
			   this)));
    }

    static inline void
    send_freq(int sock, uint64_t freq)
    {
      static char buf[512];
      snprintf(buf, 512, "%llu\n", freq);
      send(sock, buf, strnlen(buf, 512), 0);
    }

    static inline void
    send_report_code(int sock, int code)
    {
      static char buf[512];
      snprintf(buf, 512, "RPRT %d\n", code);
      send(sock, buf, strnlen(buf, 512), 0);
    }

    void
    tcp_rigctl_msg_source_impl::tcp_msg_accepter ()
    {
      int sock;
      int listen_sock;
      struct sockaddr_in sin;
      struct sockaddr client_addr;
      socklen_t client_addr_len;
      ssize_t ret;
      uint8_t *buf;
      double freq = 0.0;
      uint64_t reported_freq = 0;
      int error_code = 0;
      int optval = 1;

      if ((listen_sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
	perror ("opening UDP socket");
	exit (EXIT_FAILURE);
      }

      memset (&client_addr, 0, sizeof(struct sockaddr));
      memset (&sin, 0, sizeof(struct sockaddr_in));
      sin.sin_family = AF_INET;
      sin.sin_port = htons (d_port);
      sin.sin_addr.s_addr = INADDR_ANY;

      if (inet_aton (d_iface_addr.c_str (), &(sin.sin_addr)) == 0) {
	LOG_ERROR("Wrong IP address");
	close (listen_sock);
	exit (EXIT_FAILURE);
      }

      if (bind (listen_sock, (struct sockaddr *) &sin,
		sizeof(struct sockaddr_in)) == -1) {
	perror ("TCP bind");
	close (listen_sock);
	exit (EXIT_FAILURE);
      }

      if (listen (listen_sock, 1000) == -1) {
	perror ("TCP listen");
	close (listen_sock);
	exit (EXIT_FAILURE);
      }

      /* All good until now. Allocate buffer memory and proceed */
      buf = new uint8_t[d_mtu];

      while (d_running) {
	sock = accept (listen_sock, &client_addr, &client_addr_len);
	if (sock <= 0) {
	  perror ("TCP accept");
	  exit (EXIT_FAILURE);
	}

	/* Apply the TCP_NODELAY option at the accepted socket */
	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int)) < 0){
	  perror ("TCP setsockopt");
	  exit (EXIT_FAILURE);
	}

	while ((ret = recv (sock, buf, d_mtu, 0)) > 0 && d_running) {
	  switch (buf[0])
	    {
	    case 'F':
	      freq = get_freq_from_buf (buf + 2);
	      /*
	       * If the frequency is different than 0, then the parsed value
	       * is valid and an appropriate message can be generated
	       *
	       * NOTE: Comparison for equality in floats is a bit tricky.
	       * But here the get_freq_from_buf() will assign a 0.0 explicitly
	       * if something goes wrong. For this reason it is safe to compare
	       * the in-equality against 0.0.
	       */
	      if (freq != 0.0) {
		reported_freq = freq;
		message_port_pub (pmt::mp ("freq"), pmt::from_double (freq));
		error_code = 0;
	      }
	      else{
		error_code = -11;
	      }
	      /* Send the report code */
	      send_report_code(sock, error_code);
	      break;
	    case 'f':
	      send_freq(sock, reported_freq);
	      break;
	    /* Terminate the connection and exit */
	    case 'q':
	      send_report_code(sock, 0);
	      d_running = false;
	      break;
	    default:
	      LOG_WARN("Unsupported rigctl command");
	      send_report_code(sock, -11);
	    }
	}
	shutdown (sock, SHUT_RDWR);
	close (sock);
      }
      close (listen_sock);
      delete [] buf;
      exit (EXIT_SUCCESS);
    }

    /**
     * Parses the string in the \p buf trying to extract a frequency reading
     * until the newline character
     * @param buf the buffer containing a line with the target frequency
     * @return the frequency converted into double
     */
    double
    tcp_rigctl_msg_source_impl::get_freq_from_buf (const uint8_t* buf)
    {
      long int f;
      char *end;
      f = strtol ((char *) buf, &end, 10);

      /* Check for various possible errors */
      if ((errno == ERANGE && (f == LONG_MAX || f == LONG_MIN))
	  || (errno != 0 && f == 0)) {
	LOG_WARN("Invalid rigctl command");
	f = 0;
      }

      if ((char *) buf == end) {
	LOG_WARN("Invalid rigctl command");
	f = 0;
      }

      return (double) f;
    }

    /*
     * Our virtual destructor.
     */
    tcp_rigctl_msg_source_impl::~tcp_rigctl_msg_source_impl ()
    {
      d_running = false;
      d_thread->join ();
    }

  } /* namespace satnogs */
} /* namespace gr */

