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
#include <arpa/inet.h>

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_encoder::sptr
    upsat_fsk_frame_encoder::make (const std::vector<uint8_t>& preamble,
				   const std::vector<uint8_t>& sync_word,
				   bool append_crc, bool whitening,
				   bool manchester, bool msb_first,
				   bool ax25_format,
				   const std::string& ax25_dest_addr,
				   uint8_t ax25_dest_ssid,
				   const std::string& ax25_src_addr,
				   uint8_t ax25_src_ssid,
				   size_t settling_samples)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_encoder_impl (preamble, sync_word,
					    append_crc,
					    whitening, manchester,
					    msb_first, ax25_format,
					    ax25_src_addr, ax25_src_ssid,
					    ax25_dest_addr, ax25_dest_ssid,
					    settling_samples));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_encoder_impl::upsat_fsk_frame_encoder_impl (
	const std::vector<uint8_t>& preamble,
	const std::vector<uint8_t>& sync_word,
	bool append_crc, bool whitening,
	bool manchester, bool msb_first,
	bool ax25_format,
	const std::string& ax25_dest_addr,
	uint8_t ax25_dest_ssid,
	const std::string& ax25_src_addr,
	uint8_t ax25_src_ssid,
	size_t settling_samples) :
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
	    d_is_ax25(ax25_format),
	    d_max_pdu_len(d_preamble_len + d_sync_word_len + sizeof(uint8_t)
	      + UPSAT_MAX_FRAME_LEN + sizeof(uint16_t)),
	    d_settling_samples(settling_samples),
	    d_encoded(0),
	    d_pdu_len(0),
	    d_scrambler(0x21, 0x1FF, 9)
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


      if(d_is_ax25){
	d_ax25_addr = new uint8_t[AX25_MAX_ADDR_LEN];
	/* One useful bit per byte for the AX.25 buffer */
	d_ax25_pdu = new uint8_t[2 * d_max_pdu_len * 8];
	d_ax25_tmp_buf = new uint8_t[d_max_pdu_len * 2];
	d_ax25_addr_len = ax25_create_addr_field (d_ax25_addr, ax25_dest_addr,
						  ax25_dest_ssid, ax25_src_addr,
						  ax25_src_ssid);
      }

      d_pdu_encoded = new float[d_max_pdu_len*8 + d_settling_samples];

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
      delete [] d_pdu_encoded;
      if(d_is_ax25) {
	delete [] d_ax25_addr;
	delete [] d_ax25_pdu;
	delete [] d_ax25_tmp_buf;
      }
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

    inline void
    upsat_fsk_frame_encoder_impl::add_sob (uint64_t item)
    {
      static const pmt::pmt_t sob_key = pmt::string_to_symbol ("tx_sob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, sob_key, value, srcid);
    }

    inline void
    upsat_fsk_frame_encoder_impl::add_eob (uint64_t item)
    {
      static const pmt::pmt_t eob_key = pmt::string_to_symbol ("tx_eob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, eob_key, value, srcid);
    }

    inline int
    upsat_fsk_frame_encoder_impl::raw_frame_handling (float* out,
						      int noutput_items)
    {
      uint16_t crc;
      size_t min_available;
      pmt::pmt_t pdu;

      /*
       * If the whole previous frame has been successfully sent, block waiting
       * for a new one
       */
      if (d_encoded == 0) {
	pdu = delete_head_blocking (pmt::mp ("pdu"));
	d_pdu_len = pmt::blob_length (pdu);

	if (d_pdu_len > UPSAT_MAX_FRAME_LEN) {
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
	if (d_append_crc) {
	  crc = crc16_ccitt (d_pdu + d_preamble_len + d_sync_word_len,
			     d_pdu_len + 1);
	  /* CRC must be transmitted MSB first */
	  crc = htons (crc);
	  memcpy (d_pdu + d_preamble_len + d_sync_word_len + 1 + d_pdu_len,
		  &crc, sizeof(uint16_t));
	  d_pdu_len += sizeof(uint16_t);
	}

	/*
	 * Whitening is performed on all bytes except preamble and SYNC fields
	 */
	if (d_whitening) {
	  d_scrambler.reset ();
	  d_scrambler.scramble (d_pdu + d_preamble_len + d_sync_word_len,
				d_pdu + d_preamble_len + d_sync_word_len,
				d_pdu_len + 1);
	}

	d_pdu_len += d_preamble_len + d_sync_word_len + 1;

	/*
	 * NRZ Encode the whole frame
	 */
	if (d_msb_first) {
	  map_msb_first (d_pdu_encoded, d_pdu_len * 8);
	}
	else {
	  map_lsb_first (d_pdu_encoded, d_pdu_len * 8);
	}

	/* Reset the settling trailing samples */
	memset (d_pdu_encoded + d_pdu_len * 8, 0,
		d_settling_samples * sizeof(float));

	/* The new frame now has a bigger size of course*/
	d_pdu_len += d_settling_samples / 8;

	/* Start of burst */
	add_sob (nitems_written (0));
      }

      noutput_items = std::max (0, noutput_items);
      min_available = std::min ((size_t) noutput_items,
				(d_pdu_len - d_encoded) * 8);

      memcpy (out, d_pdu_encoded + d_encoded * 8,
	      min_available * sizeof(float));
      d_encoded += min_available / 8;

      /* End of the frame reached */
      if (d_encoded == d_pdu_len) {
	add_eob (nitems_written (0) + min_available - 1);
	d_encoded = 0;
      }

      return min_available;
    }

    inline int
    upsat_fsk_frame_encoder_impl::ax25_frame_handling (float* out,
						       int noutput_items)
    {
      uint16_t crc;
      size_t min_available;
      pmt::pmt_t pdu;
      size_t len;
      size_t encoded_len;
      ax25_encode_status_t status;
      size_t extra_bits;
      size_t i;

      /*
       * If the whole previous frame has been successfully sent, block waiting
       * for a new one
       */
      if (d_encoded == 0) {
	/* Reset the buffer */
	memset(d_ax25_pdu, 0, 2 * d_max_pdu_len * 8);
	memset(d_pdu_encoded, 0, d_max_pdu_len * 8 * sizeof(float));
	pdu = delete_head_blocking (pmt::mp ("pdu"));
	d_pdu_len = pmt::blob_length (pdu);

	if (d_pdu_len > UPSAT_MAX_FRAME_LEN) {
	  LOG_ERROR("PDU is greater than the supported. Dropping the PDU");
	  return 0;
	}

	len = ax25_prepare_frame(d_ax25_tmp_buf, (uint8_t *) pmt::blob_data (pdu),
				 d_pdu_len, AX25_UI_FRAME, d_ax25_addr,
				 d_ax25_addr_len, 0x03, 1, 1, 1);
	status = ax25_bit_stuffing(d_ax25_pdu, &encoded_len,
				   d_ax25_tmp_buf, len, 1, 1);
	if(status != AX25_ENC_OK) {
	  LOG_WARN("Failed to properly encode into AX.25 frame");
	  return 0;
	}

	/* Due to bit stuffing the resulting bit number may not be
	 * a multiple of 8. Thus we append proper number of zeros
	 * after the SYNC flag. So neither the AX.25 or the FSK
	 * framing are affected
	 */
	extra_bits = 8 - (encoded_len % 8);
	encoded_len += extra_bits;

	/*
	 * Set the frame length at the corresponding field.
	 * NOTE: The frame length is calculated taking consideration only
	 * the address field (if exists) and the payload. Length and CRC fields
	 * are NOT included.
	 */
	d_pdu[d_preamble_len + d_sync_word_len] = (uint8_t) (encoded_len/8);

	/* If it is necessary calculate and append the CRC */
	if (d_append_crc) {
	  LOG_WARN("AX.25 has its own CRC-16 field. Skipping...");
	}

	/*
	 * Whitening can not be applied in the AX.25 because it will alter
	 * the SYNC flag of the stack
	 */
	if (d_whitening) {
	  LOG_WARN("AX.25 and whitening are not compatible."
	      " No whitening will be performed");
	}

	d_pdu_len = d_preamble_len + d_sync_word_len + 1 + encoded_len/8;

	/* NRZ encoding the FSK preamble with the standard method */
	map_msb_first (d_pdu_encoded, (d_preamble_len + d_sync_word_len + 1) * 8);

	/* NRZ encode the AX.25 part of the FSK frame */
	for(i = 0; i < encoded_len - extra_bits; i++){
	  d_pdu_encoded[i + (d_preamble_len + d_sync_word_len + 1)*8]
			= (d_ax25_pdu[i] * 2.0f) - 1.0f;
	}

	/* Reset the settling trailing samples */
	memset (&d_pdu_encoded[d_pdu_len * 8], 0,
		d_settling_samples * sizeof(float));

	/* The new frame now has a bigger size of course*/
	d_pdu_len += d_settling_samples/8;

	/* Start of burst */
	add_sob (nitems_written (0));
      }

      noutput_items = std::max (0, noutput_items);
      min_available = std::min ((size_t) noutput_items,
				(d_pdu_len - d_encoded) * 8);

      memcpy (out, d_pdu_encoded + d_encoded * 8,
	      min_available * sizeof(float));
      d_encoded += min_available / 8;

      /* End of the frame reached */
      if (d_encoded == d_pdu_len) {
	add_eob (nitems_written (0) + min_available - 1);
	d_encoded = 0;
      }

      return min_available;
    }

    int
    upsat_fsk_frame_encoder_impl::work (int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
    {
      float *out = (float *) output_items[0];

      if(d_is_ax25) {
	return ax25_frame_handling(out, noutput_items);
      }
      else{
	return raw_frame_handling(out, noutput_items);
      }
    }

  } /* namespace satnogs */
} /* namespace gr */

