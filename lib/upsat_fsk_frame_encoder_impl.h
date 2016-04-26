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

#ifndef INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_IMPL_H
#define INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_IMPL_H

#include <satnogs/upsat_fsk_frame_encoder.h>
#include <satnogs/whitening.h>

namespace gr
{
  namespace satnogs
  {

    class upsat_fsk_frame_encoder_impl : public upsat_fsk_frame_encoder
    {
    private:
      const std::vector<uint8_t> d_preamble;
      const size_t d_preamble_len;
      const std::vector<uint8_t> d_sync_word;
      const size_t d_sync_word_len;
      const bool d_append_crc;
      const bool d_whitening;
      const bool d_manchester;
      const bool d_msb_first;
      const bool d_is_ax25;
      const size_t d_max_pdu_len;
      const size_t d_settling_samples;
      size_t d_encoded;
      size_t d_pdu_len;
      whitening d_scrambler;
      uint8_t *d_pdu;
      float *d_pdu_encoded;

      inline void
      map_msb_first (float *out, size_t nsamples_out);
      inline void
      map_lsb_first (float *out, size_t nsamples_out);
      inline void
      add_sob (uint64_t item);
      inline void
      add_eob (uint64_t item);

    public:
      upsat_fsk_frame_encoder_impl (const std::vector<uint8_t>& preamble,
				    const std::vector<uint8_t>& sync_word,
				    bool append_crc, bool whitening,
				    bool manchester,
				    bool msb_first,
				    bool ax25_format,
				    size_t settling_samples);
      ~upsat_fsk_frame_encoder_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_IMPL_H */

