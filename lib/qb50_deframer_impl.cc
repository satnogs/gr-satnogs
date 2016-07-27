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
#include "qb50_deframer_impl.h"
#include <satnogs/ax25.h>

namespace gr
{
  namespace satnogs
  {

    qb50_deframer::sptr
    qb50_deframer::make (uint8_t wod_ssid)
    {
      return gnuradio::get_initial_sptr (new qb50_deframer_impl (wod_ssid));
    }

    void
    qb50_deframer_impl::msg_handler (pmt::pmt_t msg)
    {
      const uint8_t *frame;
      uint8_t dest_ssid;
      size_t frame_len;

      frame = (const uint8_t *) pmt::blob_data(msg);
      frame_len = pmt::blob_length(msg);

      if(frame_len < AX25_MIN_ADDR_LEN){
	LOG_ERROR("Invalid AX.25 frame size");
	return;
      }

      /*
       * Get the destination SSID and decide in which port the frame should
       * be forwarded. Also skip the leading headers and leave only the payload
       */
      dest_ssid = ax25_get_dest_ssid(frame);
      if(dest_ssid == d_wod_ssid){
	message_port_pub (
	    pmt::mp ("wod"),
	    pmt::make_blob (frame + AX25_MIN_ADDR_LEN + 2,
			    frame_len - AX25_MIN_ADDR_LEN - 2));
      }
      else{
	message_port_pub (
	    pmt::mp ("out"),
	    pmt::make_blob (frame + AX25_MIN_ADDR_LEN + 2,
			    frame_len - AX25_MIN_ADDR_LEN - 2));
      }
    }

    /*
     * The private constructor
     */
    qb50_deframer_impl::qb50_deframer_impl(uint8_t wod_ssid)
    : gr::block("qb50_deframer",
	gr::io_signature::make(0, 0, 0),
	gr::io_signature::make(0, 0, 0)),
	d_wod_ssid(wod_ssid)
    {
      message_port_register_in(pmt::mp("in"));
      message_port_register_out(pmt::mp("out"));
      message_port_register_out(pmt::mp("wod"));

      set_msg_handler (
	  pmt::mp ("in"),
	  boost::bind (&qb50_deframer_impl::msg_handler, this, _1));
    }


  } /* namespace satnogs */
} /* namespace gr */

