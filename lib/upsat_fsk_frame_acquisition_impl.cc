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

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_acquisition::sptr
    upsat_fsk_frame_acquisition::make (const std::vector<uint8_t> &preamble,
				       const std::vector<uint8_t> &sync_word,
				       bool whitening, bool manchester)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_acquisition_impl (preamble, sync_word, whitening,
						manchester));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_acquisition_impl::upsat_fsk_frame_acquisition_impl (
	const std::vector<uint8_t> &preamble,
	const std::vector<uint8_t> &sync_word, bool whitening, bool manchester) :
	    gr::sync_block ("upsat_fsk_frame_acquisition",
			    gr::io_signature::make (1, 1, sizeof(float)),
			    gr::io_signature::make (0, 0, 0)),
	    d_preamble (preamble),
	    d_preamble_len (preamble.size ()),
	    d_sync_word (sync_word),
	    d_sync_word_len (sync_word.size ()),
	    d_state (SEARCHING),
	    d_shifting_byte (0x0),
	    d_decoded_bytes (0),
	    d_frame_len (0)
    {
      message_port_register_out (pmt::mp ("pdu"));
      if (d_preamble_len < 2) {
	throw std::invalid_argument ("Preamble must be at least 2 bytes long");
      }

      if (d_sync_word_len < 1) {
	throw std::invalid_argument (
	    "Synchronization word must be at least 1 byte long");
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
      d_state = SEARCHING;
      d_decoded_bytes = 0;
      d_shifting_byte = 0;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_preamble ()
    {
      d_state = HAVE_PREAMBLE;
      d_decoded_bytes = 1;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_sync ()
    {
      d_state = HAVE_SYNC_WORD;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_frame_len ()
    {
      d_state = HAVE_FRAME_LEN;
    }

    inline void
    upsat_fsk_frame_acquisition_impl::have_payload ()
    {
      d_state = HAVE_PAYLOAD;
    }

    int
    upsat_fsk_frame_acquisition_impl::work (
	int noutput_items, gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
    {
      int i;
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
	    break;
	  case HAVE_SYNC_WORD:
	    break;
	  case HAVE_PAYLOAD:
	    if (d_decoded_bytes == d_frame_len) {
	      message_port_pub (pmt::mp ("pdu"),
				pmt::make_blob (d_pdu, d_frame_len));
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

