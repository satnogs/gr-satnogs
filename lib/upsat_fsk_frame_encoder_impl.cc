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
#include <satnogs/utils.h>

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_encoder::sptr
    upsat_fsk_frame_encoder::make (const std::vector<uint8_t>& preamble,
				   const std::vector<uint8_t>& sync_word,
				   bool append_crc, bool whitening,
				   bool manchester,
				   bool msb_first)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_encoder_impl (preamble, sync_word, append_crc,
					    whitening, manchester, msb_first));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_encoder_impl::upsat_fsk_frame_encoder_impl (
	const std::vector<uint8_t>& preamble,
	const std::vector<uint8_t>& sync_word, bool append_crc, bool whitening,
	bool manchester, bool msb_first) :
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
	    d_msb_first(msb_first),
	    d_max_pdu_len(d_preamble_len + d_sync_word_len + sizeof(uint8_t)
	      + UPSAT_MAX_FRAME_LEN + sizeof(uint16_t)),
	    d_encoded(0),
	    d_pdu_len(0)
    {
      /* Simplify the logic of the output samples handling */
      set_output_multiple(8);
      message_port_register_in (pmt::mp ("pdu"));
      /*
       * Allocate memory for the maximum possible frame WITH all headers and
       * tail fields
       * +-------------+----------+--------+--------------------------+------+
       * | Preamble    |  SFD     | LENGTH |   PDU                    | CRC  |
       * +-------------+----------+--------+--------------------------+------+
       *   User def.     User def.   1B         1-255 B                  2 B
       */
      d_pdu = new uint8_t[d_max_pdu_len];

      if(!d_pdu) {
	throw std::runtime_error("Could not allocate memory");
      }

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

    inline void
    upsat_fsk_frame_encoder_impl::map_msb_first(float *out, size_t nsamples_out)
    {
      size_t i;
      register uint8_t b;
      for(i = 0; i < nsamples_out; i+=8)
      {
	b = d_pdu[d_encoded + i/8];
	out[i] = ((b >> 7 ) * 2.0f) - 1.0f;
	out[i + 1] = (( (b >> 6 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 2] = (( (b >> 5 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 3] = (( (b >> 4 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 4] = (( (b >> 3 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 5] = (( (b >> 2 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 6] = (( (b >> 1 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 7] = ((b & 0x1) * 2.0f) - 1.0f;
      }
    }

    inline void
    upsat_fsk_frame_encoder_impl::map_lsb_first(float *out, size_t nsamples_out)
    {
      size_t i;
      register uint8_t b;
      for(i = 0; i < nsamples_out; i+=8)
      {
	b = d_pdu[d_encoded + i/8];
	out[i + 7] = ((b >> 7 ) * 2.0f) - 1.0f;
	out[i + 6] = (( (b >> 6 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 5] = (( (b >> 5 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 4] = (( (b >> 4 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 3] = (( (b >> 3 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 2] = (( (b >> 2 ) & 0x1) * 2.0f) - 1.0f;
	out[i + 1] = (( (b >> 1 ) & 0x1) * 2.0f) - 1.0f;
	out[i] = ((b & 0x1) * 2.0f) - 1.0f;
      }
    }

    int
    upsat_fsk_frame_encoder_impl::work (int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
    {
      uint16_t crc;
      size_t min_available;
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
	/*
	 * Set the frame length at the corresponding field.
	 * NOTE: The frame length is calculated taking consideration only
	 * the address field (if exists) and the payload. Length and CRC fields
	 * are NOT included.
	 */
	d_pdu[d_preamble_len + d_sync_word_len] = (uint8_t) d_pdu_len;

	/* Append the variable length PDU */
	memcpy (d_pdu + d_preamble_len + d_sync_word_len + 1,
		pmt::blob_data (pdu), d_pdu_len);

	/* If it is necessary calculate and append the CRC */
	if(d_append_crc) {
	  crc = crc16_ccitt(d_pdu + d_preamble_len + d_sync_word_len,
			    d_pdu_len + 1);
	  memcpy(d_pdu + d_preamble_len + d_sync_word_len + 1 + d_pdu_len,
		 &crc, sizeof(uint16_t));
	  d_pdu_len += sizeof(uint16_t);
	}

	d_pdu_len += d_preamble_len + d_sync_word_len + 1;
      }

      noutput_items = std::max (0, noutput_items);
      min_available = std::min ((size_t) noutput_items,
				(d_pdu_len - d_encoded) * 8);

      if(d_msb_first){
	map_msb_first(out, min_available*8);
      }
      else{
	map_lsb_first(out, min_available*8);
      }
      d_encoded += min_available/8;

      /* End of the frame reached */
      if(d_encoded == d_pdu_len){
	d_encoded = 0;
      }

      return min_available;
    }

  } /* namespace satnogs */
} /* namespace gr */

