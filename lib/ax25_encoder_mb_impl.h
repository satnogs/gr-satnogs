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

#ifndef INCLUDED_SATNOGS_AX25_ENCODER_MB_IMPL_H
#define INCLUDED_SATNOGS_AX25_ENCODER_MB_IMPL_H

#include <satnogs/ax25_encoder_mb.h>
#include <gnuradio/digital/lfsr.h>

namespace gr
{
  namespace satnogs
  {

    class ax25_encoder_mb_impl : public ax25_encoder_mb
    {
    private:
      const size_t d_preamble_len;
      const size_t d_postamble_len;
      const bool d_scramble;
      size_t d_remaining;
      size_t d_produced;
      uint8_t d_prev_bit;
      uint8_t *d_encoded_frame;
      uint8_t *d_tmp_buf;
      uint8_t *d_addr_field;
      size_t d_addr_len;
      digital::lfsr d_lfsr;
      boost::mutex d_mutex;

      void
      add_sob (uint64_t item);
      void
      add_eob (uint64_t item);

    public:
      ax25_encoder_mb_impl (const std::string& dest_addr, uint8_t dest_ssid,
			    const std::string& src_addr, uint8_t src_ssid,
			    size_t preamble_len, size_t postamble_len,
			    bool scramble);
      ~ax25_encoder_mb_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_ENCODER_MB_IMPL_H */

