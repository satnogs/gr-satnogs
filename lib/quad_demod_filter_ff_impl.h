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

#ifndef INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_IMPL_H
#define INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_IMPL_H

#include <satnogs/quad_demod_filter_ff.h>

namespace gr {
  namespace satnogs {

    class quad_demod_filter_ff_impl : public quad_demod_filter_ff
    {
     private:
      const float d_gain;
      const float d_norm;
      const int d_win;
      float d_sum;
      float d_sum_sq;
      int d_remaining;

     public:
      quad_demod_filter_ff_impl(float gain, int window);
      ~quad_demod_filter_ff_impl();

      // Where all the action really happens
      int
      general_work (int noutput_items, gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_IMPL_H */

