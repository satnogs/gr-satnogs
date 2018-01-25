/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_DECODER_8B10B_IMPL_H
#define INCLUDED_SATNOGS_DECODER_8B10B_IMPL_H

#include <satnogs/decoder_8b10b.h>

namespace gr
{
  namespace satnogs
  {

    class decoder_8b10b_impl : public decoder_8b10b
    {
    private:
      size_t d_max_frame_len;
      int d_comp_type;
      uint16_t d_control_symbol_pos;
      uint16_t d_control_symbol_neg;
      uint16_t d_data_reg;

    public:
      decoder_8b10b_impl (const std::string& control_symbol,
                          size_t max_frame_len, int comp_type);
      ~decoder_8b10b_impl ();

      bool
      set_access_code (const std::string &access_code);

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  }
// namespace satnogs
}// namespace gr

#endif /* INCLUDED_SATNOGS_DECODER_8B10B_IMPL_H */

