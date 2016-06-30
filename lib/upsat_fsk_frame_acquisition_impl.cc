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
#include "upsat_fsk_frame_acquisition_impl.h"
#include <satnogs/log.h>
#include <satnogs/ax25.h>
#include <satnogs/utils.h>
#include <arpa/inet.h>

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_acquisition::sptr
    upsat_fsk_frame_acquisition::make (const std::vector<uint8_t> &preamble,
				       const std::vector<uint8_t> &sync_word,
				       bool whitening, bool manchester,
				       bool check_crc,
				       bool ax25_format)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_acquisition_impl (preamble, sync_word, whitening,
						manchester, check_crc,
						ax25_format));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_acquisition_impl::upsat_fsk_frame_acquisition_impl (
	const std::vector<uint8_t> &preamble,
	const std::vector<uint8_t> &sync_word, bool whitening, bool manchester,
	bool check_crc, bool ax25_format) :
	    gr::sync_block ("upsat_fsk_frame_acquisition",
			    gr::io_signature::make (1, 1, sizeof(float)),
			    gr::io_signature::make (0, 0, 0)),
	    d_preamble (preamble),
	    d_preamble_len (preamble.size ()),
	    d_sync_word (sync_word),
	    d_sync_word_len (sync_word.size ()),
	    /*
	     * Preamble is used only for AGC. The true synchronization is
	     * performed using the SYNC word. For this reason if some preamble
	     * symbols are retrieved, the algorithm should immediately start
	     * searching for the SYNC word.
	     */
	    d_search_for_sync_thrhld(d_preamble_len / 3),
	    d_whitening(whitening),
	    d_manchester(manchester),
	    d_check_crc(check_crc),
	    d_is_ax25(ax25_format),
	    d_state (SEARCHING),
	    d_shifting_byte (0x0),
	    d_decoded_bytes (0),
	    d_decoded_bits (0),
	    d_frame_len (0),
	    d_descrambler(0x1001, 0x1FF, 17)
    {
      size_t i;
      message_port_register_out (pmt::mp ("pdu"));
      if (d_preamble_len < 3) {
	throw std::invalid_argument ("Preamble must be at least 2 bytes long");
      }

      if (d_sync_word_len < 1) {
	throw std::invalid_argument (
	    "Synchronization word must be at least 1 byte long");
      }

      for(i = 1; i < d_preamble_len; i++){
	if(d_preamble[i] != d_preamble[0]) {
	  throw std::invalid_argument (
	      "The preamble should contain the same bytes");
	}
      }

      d_pdu = new uint8_t[UPSAT_MAX_FRAME_LEN];
      d_ax25_tmp_buf = new uint8_t[2 * UPSAT_MAX_FRAME_LEN * 8];
      d_ax25_buf = new uint8_t[2 * UPSAT_MAX_FRAME_LEN];
    }

    /*
     * Our virtual destructor.
     */
    upsat_fsk_frame_acquisition_impl::~upsat_fsk_frame_acquisition_impl ()
    {
      delete[] d_pdu;
      delete[] d_ax25_tmp_buf;
      delete[] d_ax25_buf;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::slice_and_shift (float in)
    {
      uint8_t tmp;
      /* Slice the input into 0 and 1 bits */
      tmp = in > 0 ? 1 : 0;
      d_shifting_byte = d_shifting_byte << 1;
      d_shifting_byte |= tmp;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::reset_state ()
    {
      LOG_DEBUG("Enter reset");
      d_state = SEARCHING;
      d_decoded_bytes = 0;
      d_decoded_bits = 0;
      d_shifting_byte = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_preamble ()
    {
      LOG_DEBUG("Enter have preamble");
      d_state = HAVE_PREAMBLE;
      d_decoded_bytes = 1;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::searching_sync_word ()
    {
      LOG_DEBUG("Enter searching sync");
      d_state = SEARCHING_SYNC_WORD;
      d_decoded_bytes = 0;
      d_decoded_bits = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_sync ()
    {
      LOG_DEBUG("Enter have sync");
      d_state = HAVE_SYNC_WORD;
      /* The first SYNC byte have already been found */
      d_decoded_bytes = 1;
      d_decoded_bits = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_frame_len ()
    {
      LOG_DEBUG("Enter frame len");
      d_descrambler.reset();
      d_state = HAVE_FRAME_LEN;
      d_decoded_bytes = 0;
      d_decoded_bits = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_payload ()
    {
      LOG_DEBUG("Enter have payload");
      d_state = HAVE_PAYLOAD;
      d_decoded_bytes = 1;
      d_decoded_bits = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::unpack_ax25_bytes (size_t len_bytes)
    {
      size_t i;
      uint8_t *in = d_pdu + 1;

      for(i = 0; i < len_bytes; i++){
	d_ax25_tmp_buf[8*i] = (in[i] >> 7) & 0x1;
	d_ax25_tmp_buf[8*i + 1] = (in[i] >> 6) & 0x1;
	d_ax25_tmp_buf[8*i + 2] = (in[i] >> 5) & 0x1;
	d_ax25_tmp_buf[8*i + 3] = (in[i] >> 4) & 0x1;
	d_ax25_tmp_buf[8*i + 4] = (in[i] >> 3) & 0x1;
	d_ax25_tmp_buf[8*i + 5] = (in[i] >> 2) & 0x1;
	d_ax25_tmp_buf[8*i + 6] = (in[i] >> 1) & 0x1;
	d_ax25_tmp_buf[8*i + 7] = in[i]  & 0x1;
      }

      /* De-white the data if necessary */
      if (d_whitening) {
	d_descrambler.descramble_one_bit_per_byte (d_ax25_tmp_buf,
						   d_ax25_tmp_buf,
						   len_bytes * 8);
      }
    }

    int
    upsat_fsk_frame_acquisition_impl::work (
	int noutput_items, gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
    {
      int i;
      uint16_t crc_received;
      uint16_t crc_calc;
      size_t ax25_frame_len = 0;
      ax25_decode_status_t status;
      const float *in = (const float *) input_items[0];
      for (i = 0; i < noutput_items; i++) {
	slice_and_shift (in[i]);

	switch (d_state)
	  {
	  case SEARCHING:
	    if (d_shifting_byte == d_preamble[0]) {
	      have_preamble ();
	    }
	    break;
	  case HAVE_PREAMBLE:
	    d_decoded_bits++;

	    if(d_decoded_bits == 8) {
	      d_decoded_bits = 0;
	      if(d_shifting_byte == d_preamble[d_decoded_bytes]){
		d_decoded_bytes++;
		if(d_decoded_bytes == d_search_for_sync_thrhld){
		  /* End of the preamble. It's time for the sync word */
		  searching_sync_word();
		}
	      }
	      else{
		/* Reset the preamble detection */
		reset_state();
	      }
	    }
	    break;
	  case SEARCHING_SYNC_WORD:
	    d_decoded_bits++;
	    if(d_shifting_byte == d_sync_word[0]){
	      have_sync();
	      break;
	    }

	    if(d_decoded_bits == 8) {
	      d_decoded_bits = 0;
	      d_decoded_bytes++;
	      /*
	       * If we decoded bytes have length greater than the preamble and
	       * the SYNC word, we lost the frame...
	       */
	      if (d_decoded_bytes > d_preamble_len
		  - d_search_for_sync_thrhld + d_sync_word_len) {
		reset_state ();
	      }
	    }
	    break;
	  case HAVE_SYNC_WORD:
	    d_decoded_bits++;
	    if(d_decoded_bits == 8) {
	      d_decoded_bits = 0;
	      if(d_shifting_byte == d_sync_word[d_decoded_bytes]) {
		d_decoded_bytes++;
		if(d_decoded_bytes == d_sync_word_len){
		  have_frame_len();
		}
	      }
	      else{
		reset_state();
	      }
	    }
	    break;
	  case HAVE_FRAME_LEN:
	    d_decoded_bits++;
	    if(d_decoded_bits == 8) {

	      /* Length field has been whitened if the option is enabled */
	      if(d_whitening){
		/* Frame length field is needed for the CRC calculation */
		d_descrambler.descramble(d_pdu, &d_shifting_byte, 1);
		/* CRC is not included in the frame length field, but we want it */
		d_frame_len = 1 + d_pdu[0] + sizeof(uint16_t);
	      }
	      else{
		/* Frame length field is needed for the CRC calculation */
		d_pdu[0] = d_shifting_byte;
		/* CRC is not included in the frame length field, but we want it */
		d_frame_len = 1 + d_shifting_byte + sizeof(uint16_t);
	      }
	      have_payload();
	    }
	    break;
	  case HAVE_PAYLOAD:
	    d_decoded_bits++;
	    if (d_decoded_bits == 8) {
	      d_decoded_bits = 0;
	      d_pdu[d_decoded_bytes] = d_shifting_byte;
	      d_decoded_bytes++;

	      if (d_decoded_bytes == d_frame_len) {
		if(d_is_ax25) {

		  unpack_ax25_bytes(d_frame_len - 1);
		  status = ax25_decode(d_ax25_buf, &ax25_frame_len,
				       d_ax25_tmp_buf, (d_frame_len - 1)*8);
		  if(status == AX25_DEC_OK){
		    /* Skip the AX.25 header */
		    message_port_pub (
			pmt::mp ("pdu"),
			pmt::make_blob (d_ax25_buf + AX25_MIN_ADDR_LEN + 2,
					ax25_frame_len - AX25_MIN_ADDR_LEN - 2));
		  }

		  /*
		   * We are done here. Whitening and FSK CRC is not supported
		   * when transmitting/receiving AX.25 frames
		   */
		  reset_state ();
		  break;
		}

		if(d_whitening){
		  d_descrambler.descramble(d_pdu+1, d_pdu+1, d_frame_len - 1);
		}

		if(!d_check_crc){
		  message_port_pub (
		      pmt::mp ("pdu"),
		      pmt::make_blob (d_pdu + 1,
				      d_frame_len - 1 - sizeof(uint16_t)));
		  reset_state ();
		  break;
		}

		/* Retrieve and check the CRC */
		memcpy(&crc_received, d_pdu + d_frame_len - sizeof(uint16_t),
		       sizeof(uint16_t));
		/* The CRC is transmitted in network byte order */
		crc_received = ntohs(crc_received);
		crc_calc = crc16_ccitt(d_pdu, d_frame_len - sizeof(uint16_t));
		if(crc_calc == crc_received) {
		  message_port_pub (
		      pmt::mp ("pdu"),
		      pmt::make_blob (d_pdu + 1,
				      d_frame_len - 1 - sizeof(uint16_t)));
		}
		else{
		  LOG_WARN("Frame with wrong CRC got 0x%x calc 0x%x",
			   crc_received, crc_calc);
		}
		reset_state ();
	      }
	    }
	    break;
	  default:
	    LOG_WARN("Unknown decoding state");
	  }
      }

      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

