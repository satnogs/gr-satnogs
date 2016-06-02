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
#include "debug_msg_source_raw_impl.h"

namespace gr
{
  namespace satnogs
  {

    debug_msg_source_raw::sptr
    debug_msg_source_raw::make (const std::vector<uint8_t> &msg, double delay,
				bool repeat)
    {
      return gnuradio::get_initial_sptr (
	  new debug_msg_source_raw_impl (msg, delay, repeat));
    }

    /*
     * The private constructor
     */
    debug_msg_source_raw_impl::debug_msg_source_raw_impl (
	const std::vector<uint8_t> &msg, double delay, bool repeat) :
	    gr::block ("debug_msg_source_raw", gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
	    d_buf_len (msg.size ()),
	    d_delay (delay),
	    d_repeat (repeat),
	    d_running (true)
    {
      d_buf = new uint8_t[msg.size()];
      memcpy (d_buf, msg.data(), msg.size());
      message_port_register_out (pmt::mp ("msg"));
      boost::shared_ptr<boost::thread> (
	  new boost::thread (
	      boost::bind (&debug_msg_source_raw_impl::msg_sender, this)));
    }

    void
    debug_msg_source_raw_impl::msg_sender ()
    {
      pmt::pmt_t msg = pmt::make_blob (d_buf, d_buf_len);
      if (d_repeat) {
	while (d_running) {
	  boost::this_thread::sleep_for (
	      boost::chrono::milliseconds ((size_t) (d_delay * 1e3)));
	  message_port_pub (pmt::mp ("msg"), msg);
	}
      }
      else {
	boost::this_thread::sleep_for (
	    boost::chrono::milliseconds ((size_t) (d_delay * 1e3)));
	message_port_pub (pmt::mp ("msg"), msg);
      }
    }

    /*
     * Our virtual destructor.
     */
    debug_msg_source_raw_impl::~debug_msg_source_raw_impl ()
    {
      d_running = false;
      d_thread->join ();
      delete[] d_buf;
    }

  } /* namespace satnogs */
} /* namespace gr */

