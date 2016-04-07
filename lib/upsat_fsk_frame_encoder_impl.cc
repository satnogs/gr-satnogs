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
#include "upsat_fsk_frame_encoder_impl.h"
#include <satnogs/log.h>

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_encoder::sptr
    upsat_fsk_frame_encoder::make (const std::vector<uint8_t>& preamble,
				   const std::vector<uint8_t>& sync_word,
				   bool append_crc, bool whitening,
				   bool manchester)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_encoder_impl (preamble, sync_word, append_crc,
					    whitening, manchester));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_encoder_impl::upsat_fsk_frame_encoder_impl (
	const std::vector<uint8_t>& preamble,
	const std::vector<uint8_t>& sync_word, bool append_crc, bool whitening,
	bool manchester) :
	    gr::sync_block ("upsat_fsk_frame_encoder",
			    gr::io_signature::make (0, 0, 0),
			    gr::io_signature::make (1, 1, sizeof(float))),
	    d_preamble (preamble),
	    d_preamble_len (preamble.size ()),
	    d_sync_word (sync_word),
	    d_sync_word_len (sync_word.size ()),
	    d_append_crc(append_crc),
	    d_whitening(whitening),
	    d_manchester(manchester),
	    d_encoded(0),
	    d_pdu_len(0)
    {
      message_port_register_in (pmt::mp ("pdu"));
      /*
       * Allocate memory for the maximum possible frame WITH all headers and
       * tail fields
       * +-------------+----------+--------+--------------------------+------+
       * | Preamble    |  SFD     | LENGTH |   PDU                    | CRC  |
       * +-------------+----------+--------+--------------------------+------+
       *   User def.     User def.   1B         1-255 B                  2 B
       */
      d_pdu = new uint8_t[d_preamble_len + d_sync_word_len + sizeof(uint8_t)
	  + UPSAT_MAX_FRAME_LEN + sizeof(uint16_t)];

      /* Copy the preamble at the start of the pdu */
      memcpy(d_pdu, d_preamble.data(), d_preamble_len);
      memcpy(d_pdu + d_preamble_len, d_sync_word.data(), d_sync_word_len);
    }

    /*
     * Our virtual destructor.
     */
    upsat_fsk_frame_encoder_impl::~upsat_fsk_frame_encoder_impl ()
    {
      delete [] d_pdu;
    }

    int
    upsat_fsk_frame_encoder_impl::work (int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
    {
      pmt::pmt_t pdu;
      float *out = (float *) output_items[0];

      /*
       * If the whole previous frame has been successfully sent, block waiting
       * for a new one
       */
      if(d_encoded == 0){
	pdu = delete_head_blocking(pmt::mp("pdu"));
	d_pdu_len = pmt::blob_length (pdu);

	if(d_pdu_len > UPSAT_MAX_FRAME_LEN){
	  LOG_ERROR("PDU is greater than the supported. Dropping the PDU");
	  return 0;
	}
	/* Set the frame length at the corresponding field */
	d_pdu[d_preamble_len + d_sync_word_len] = (uint8_t) d_pdu_len;
	/* Append the variable length PDU */
	memcpy (d_pdu + d_preamble_len + d_sync_word_len + sizeof(uint8_t),
		pmt::blob_data (pdu),
		d_pdu_len * sizeof(uint8_t));
	d_pdu_len += d_preamble_len + d_sync_word_len + sizeof(uint8_t);
	/* If it is necessary calculate and append the CRC */
	if(d_append_crc) {
	  /* TODO: Calc CRC */
	  d_pdu_len += sizeof(uint16_t);
	}
      }

      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

