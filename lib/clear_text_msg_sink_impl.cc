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
#include "clear_text_msg_sink_impl.h"

namespace gr {
  namespace satnogs {

    clear_text_msg_sink::sptr
    clear_text_msg_sink::make()
    {
      return gnuradio::get_initial_sptr
        (new clear_text_msg_sink_impl());
    }

    void
    clear_text_msg_sink_impl::msg_handler (pmt::pmt_t msg)
    {
      std::string s((const char *)pmt::blob_data(msg), pmt::blob_length(msg));
      std::cout << s << " " << std::endl;
    }

    /*
     * The private constructor
     */
    clear_text_msg_sink_impl::clear_text_msg_sink_impl()
      : gr::block("clear_text_msg_sink",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("in"));
      set_msg_handler (
	  pmt::mp ("in"),
	  boost::bind (&clear_text_msg_sink_impl::msg_handler, this, _1));
    }

  } /* namespace satnogs */
} /* namespace gr */

