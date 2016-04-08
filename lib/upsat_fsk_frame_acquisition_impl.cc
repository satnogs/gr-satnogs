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
#include <satnogs/utils.h>

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_acquisition::sptr
    upsat_fsk_frame_acquisition::make (const std::vector<uint8_t> &preamble,
				       const std::vector<uint8_t> &sync_word,
				       bool whitening, bool manchester,
				       bool check_crc)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_acquisition_impl (preamble, sync_word, whitening,
						manchester, check_crc));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_acquisition_impl::upsat_fsk_frame_acquisition_impl (
	const std::vector<uint8_t> &preamble,
	const std::vector<uint8_t> &sync_word, bool whitening, bool manchester,
	bool check_crc) :
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
	    d_state (SEARCHING),
	    d_shifting_byte (0x0),
	    d_decoded_bytes (0),
	    d_decoded_bits (0),
	    d_frame_len (0)
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
    }

    /*
     * Our virtual destructor.
     */
    upsat_fsk_frame_acquisition_impl::~upsat_fsk_frame_acquisition_impl ()
    {
      delete[] d_pdu;
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

    int
    upsat_fsk_frame_acquisition_impl::work (
	int noutput_items, gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
    {
      int i;
      uint16_t crc_received;
      uint16_t crc_calc;
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
	      /* Frame length field is needed for the CRC calculation */
	      d_pdu[0] = d_shifting_byte;
	      /* CRC is not included in the frame length field, but we want it */
	      d_frame_len = 1 + d_shifting_byte + sizeof(uint16_t);
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
		crc_calc = crc16_ccitt(d_pdu, d_frame_len - sizeof(uint16_t));
		if(crc_calc == crc_received) {
		  message_port_pub (
		      pmt::mp ("pdu"),
		      pmt::make_blob (d_pdu + 1,
				      d_frame_len - 1 - sizeof(uint16_t)));
		}
		else{
		  LOG_WARN("Frame with wrong CRC");
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

