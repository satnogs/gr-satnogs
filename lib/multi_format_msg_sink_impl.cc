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
#include "multi_format_msg_sink_impl.h"

namespace gr {
  namespace satnogs {

    multi_format_msg_sink::sptr
    multi_format_msg_sink::make(size_t format)
    {
      return gnuradio::get_initial_sptr
        (new multi_format_msg_sink_impl(format));
    }

    void
    multi_format_msg_sink_impl::msg_handler (pmt::pmt_t msg)
    {
      uint8_t *su;
      std::string s((const char *)pmt::blob_data(msg), pmt::blob_length(msg));
      switch(d_format){
        case 0:
          std::cout << "Received text sequence:" << s << " " << std::endl;
          break;
        case 1:
          su = (uint8_t *)pmt::blob_data(msg);
	  for (size_t i = 0; i < pmt::blob_length (msg); i++) {
	    printf ("0x%02x ", su[i]);
	  }
	  std::cout << std::endl;
          break;
        case 2:
          su = (uint8_t *)pmt::blob_data(msg);
	  for (size_t i = 0; i < pmt::blob_length (msg); i++) {
	    std::cout << "0b" << std::bitset<8> (su[i]) << " ";
	  }
	  std::cout << std::endl;
          break;
        default:
          printf("Invalid format");
      }
    }

    /*
     * The private constructor
     */
    multi_format_msg_sink_impl::multi_format_msg_sink_impl(size_t format)
      : gr::block("multi_format_msg_sink",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
			  d_format(format)
    {
      message_port_register_in(pmt::mp("in"));
      set_msg_handler (
	  pmt::mp ("in"),
	  boost::bind (&multi_format_msg_sink_impl::msg_handler, this, _1));
    }

  } /* namespace satnogs */
} /* namespace gr */

