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
#include "ax25_decoder_b_impl.h"
#include <satnogs/log.h>

namespace gr {
  namespace satnogs {

    ax25_decoder_b::sptr
    ax25_decoder_b::make(std::string addr, uint8_t ssid, bool promisc)
    {
      return gnuradio::get_initial_sptr
        (new ax25_decoder_b_impl(addr, ssid, promisc));
    }

    /*
     * The private constructor
     */
    ax25_decoder_b_impl::ax25_decoder_b_impl (std::string addr, uint8_t ssid,
					      bool promisc) :
	gr::sync_block ("ax25_decoder_b",
			gr::io_signature::make (1, 1, sizeof(uint8_t)),
			gr::io_signature::make (0, 0, 0)),
			d_promisc(promisc),
			d_state(NO_SYNC),
			d_dec_b(0),
			d_prev_bit(0),
			d_received_bytes(0),
			d_decoded_bits(0),
			d_cont_1(0),
			d_frame_buffer(new uint8_t[AX25_MAX_FRAME_LEN
						   + AX25_MAX_ADDR_LEN
						   + AX25_MAX_CTRL_LEN
						   + sizeof(uint16_t)])
    {
      message_port_register_out (pmt::mp ("pdu"));
    }

    void
    ax25_decoder_b_impl::reset_state ()
    {
      d_state = NO_SYNC;
      d_dec_b = 0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_cont_1 = 0;
    }

    void
    ax25_decoder_b_impl::enter_sync_state ()
    {
      d_state = IN_SYNC;
      d_dec_b = 0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_prev_bit = 0;
      d_cont_1 = 0;
    }

    void
    ax25_decoder_b_impl::update_recv_frame (uint8_t byte)
    {
      uint16_t fcs;
      uint16_t recv_fcs = 0x0;
      if( d_decoded_bits ) {
	return;
      }

      if(byte == AX25_SYNC_FLAG) {
	/* First check if the size of the frame is valid */
	if(d_received_bytes < AX25_MIN_ADDR_LEN + sizeof(uint16_t)){
	  reset_state();
	  return;
	}

	/* Check if the frame is correct using the FCS field */
	fcs = ax25_fcs (d_frame_buffer, d_received_bytes - sizeof(uint16_t));
	recv_fcs = (((uint16_t) d_frame_buffer[d_received_bytes - 2]) << 8)
	    | d_frame_buffer[d_received_bytes - 1];

	if( fcs == recv_fcs ) {
	  message_port_pub (
	      pmt::mp ("pdu"),
	      pmt::make_blob (d_frame_buffer,
			      d_received_bytes - sizeof(uint16_t)));
	}
	else {
	  LOG_WARN("Wrong FCS: Should be 0%x, computed as 0%x", recv_fcs, fcs);
	}

	LOG_WARN("Frame size %lu", d_received_bytes);
	reset_state();
      }
      else{
	d_frame_buffer[d_received_bytes++] = byte;

	if(d_received_bytes > AX25_MAX_FRAME_LEN){
	  LOG_WARN("Maximum frame size reached. Abort decoding this frame");
	  reset_state();
	}
      }
      d_dec_b = 0;
    }

    /*
     * Our virtual destructor.
     */
    ax25_decoder_b_impl::~ax25_decoder_b_impl()
    {
      delete[] d_frame_buffer;
    }

    int
    ax25_decoder_b_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      int i;
      const uint8_t *in = (const uint8_t *) input_items[0];

      for (i = 0; i < noutput_items; i++) {
	switch (d_state)
	  {
	  case NO_SYNC:
	    d_dec_b = (d_dec_b << 1) | in[i];
	    /*
	     * If the decoder find the sync flag of the AX.25 frame start the
	     * decoding process
	     */
	    if(d_dec_b == AX25_SYNC_FLAG){
	      enter_sync_state();
	    }
	    break;
	  case IN_SYNC:
	    if(in[i]){
	      if(d_prev_bit) {
		d_cont_1++;
	      }
	      else {
		d_cont_1 = 1;
	      }
	      d_dec_b |= 1 << d_decoded_bits;
	      d_decoded_bits = (d_decoded_bits + 1) % 8;
	      update_recv_frame(d_dec_b);
	      d_prev_bit = 1;
	    }
	    else{
	      /* Check if this a stuffed zero bit */
	      if(d_cont_1 < 5) {
		d_decoded_bits = (d_decoded_bits + 1) % 8;
		update_recv_frame(d_dec_b);
	      }
	      d_prev_bit = 0;
	      d_cont_1 = 0;
	    }
	    break;
	  default:
	    LOG_ERROR("Invalid decoding state");
	  }
      }
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

