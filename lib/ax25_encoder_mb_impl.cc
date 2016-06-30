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
#include "ax25_encoder_mb_impl.h"
#include <satnogs/log.h>
#include <satnogs/ax25.h>

namespace gr
{
  namespace satnogs
  {

    ax25_encoder_mb::sptr
    ax25_encoder_mb::make (const std::string& dest_addr, uint8_t dest_ssid,
			   const std::string& src_addr, uint8_t src_ssid,
			   size_t preamble_len, size_t postamble_len,
			   bool scramble)
    {
      return gnuradio::get_initial_sptr (
	  new ax25_encoder_mb_impl (dest_addr, dest_ssid, src_addr, src_ssid,
				    preamble_len, postamble_len, scramble));
    }

    /*
     * The private constructor
     */
    ax25_encoder_mb_impl::ax25_encoder_mb_impl (const std::string& dest_addr,
						uint8_t dest_ssid,
						const std::string& src_addr,
						uint8_t src_ssid,
						size_t preamble_len,
						size_t postabmle_len,
						bool scramble) :
	    gr::sync_block ("ax25_encoder_mb", gr::io_signature::make (0, 0, 0),
			    gr::io_signature::make (1, 1, sizeof(uint8_t))),
	    d_preamble_len (preamble_len),
	    d_postamble_len (postabmle_len),
	    d_scramble(scramble),
	    d_remaining (0),
	    d_produced (0),
	    d_prev_bit(0x0),
	    d_encoded_frame (
		new uint8_t[preamble_len + postabmle_len
		    + (AX25_MAX_FRAME_LEN * 2)]),
	    d_tmp_buf (
		new uint8_t[preamble_len + postabmle_len
		    + (AX25_MAX_FRAME_LEN * 2)]),
	    d_addr_field (new uint8_t[AX25_MAX_ADDR_LEN]),
	    d_lfsr(0x21, 0x0, 16)
    {
      /* Input is a blob message containing the info field data */
      message_port_register_in (pmt::mp ("info"));
      d_addr_len = ax25_create_addr_field(d_addr_field, dest_addr, dest_ssid,
					  src_addr, src_ssid);
    }

    /*
     * Our virtual destructor.
     */
    ax25_encoder_mb_impl::~ax25_encoder_mb_impl ()
    {
      delete [] d_encoded_frame;
      delete [] d_tmp_buf;
      delete [] d_addr_field;
    }

    int
    ax25_encoder_mb_impl::work (int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const uint8_t *info;
      size_t info_len;
      size_t max_avail;
      size_t len;
      size_t i;
      pmt::pmt_t msg;
      ax25_encode_status_t status;

      uint8_t *out = (uint8_t *) output_items[0];

      /* If all the frame samples have already be sent, wait for a new frame */
      if (d_remaining == 0) {
	boost::mutex::scoped_lock lock (d_mutex);
	d_produced = 0;
	d_prev_bit = 0x0;
	d_lfsr.reset();

	/* Block waiting from a new message from users */
	msg = delete_head_blocking (pmt::mp ("info"));
	info = (const uint8_t *)pmt::blob_data(msg);
	info_len = pmt::blob_length(msg);

	/* Prepare and encode the AX.25 frame */
	len = ax25_prepare_frame (d_tmp_buf, info, info_len, AX25_I_FRAME,
				  d_addr_field, d_addr_len, 0, 1,
				  d_preamble_len, d_postamble_len);

	/* Perform bit stuffing */
	status = ax25_bit_stuffing (d_encoded_frame, &d_remaining, d_tmp_buf,
				    len, d_preamble_len, d_postamble_len);
	if (status != AX25_ENC_OK) {
	  LOG_ERROR ("AX.25 encoding failed");
	  d_remaining = 0;
	  return 0;
	}

	/*Perform scrambling if the user asked for it */
	if(d_scramble){
	  for(i = 0; i < d_remaining; i++){
	    d_encoded_frame[i] = d_lfsr.next_bit_scramble(d_encoded_frame[i]);
	  }
	}

	 /* If this is the first part of the frame add the start of burst tag*/
	if (d_produced == 0) {
	  add_sob (nitems_written (0));
	}
	max_avail = std::min (d_remaining, (size_t) noutput_items);

	/* Perform NRZI encoding */
	for(i = 0; i < max_avail; i++) {
	  out[i] = ( (0x1 & ~d_encoded_frame[i + d_produced]) + d_prev_bit) % 2;
	  d_prev_bit = out[i];
	}

	d_remaining -= max_avail;
	d_produced += max_avail;
	if (d_remaining == 0) {
	  add_eob (nitems_written (0) + max_avail);
	}
	return (int) max_avail;
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }


    void
    ax25_encoder_mb_impl::add_sob (uint64_t item)
    {
      static const pmt::pmt_t sob_key = pmt::string_to_symbol ("tx_sob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, sob_key, value, srcid);
    }

    void
    ax25_encoder_mb_impl::add_eob (uint64_t item)
    {
      static const pmt::pmt_t eob_key = pmt::string_to_symbol ("tx_eob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, eob_key, value, srcid);
    }

  } /* namespace satnogs */
} /* namespace gr */

