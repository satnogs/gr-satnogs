/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017
 *  Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_CW_ENCODER_IMPL_H
#define INCLUDED_SATNOGS_CW_ENCODER_IMPL_H

#include <vector>
#include <string>
#include <satnogs/cw_encoder.h>
#include <gnuradio/fxpt_nco.h>

namespace gr
{
  namespace satnogs
  {

    class cw_encoder_impl : public cw_encoder
    {
    private:
      const double d_samp_rate;
      const double d_cw_freq;
      const size_t d_wpm;
      gr::fxpt_nco d_nco;
      uint8_t *d_word;
      size_t d_remaining;

      std::string
      get_cw_symbol(char c);

    public:
      static const std::vector<char> cw_chars;
      static const std::vector<std::string> cw_symbols;
      cw_encoder_impl (double samp_rate, double cw_freq, size_t wpm);
      ~cw_encoder_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_ENCODER_IMPL_H */

