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

#ifndef INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_IMPL_H
#define INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_IMPL_H

#include <satnogs/cw_matched_filter_ff.h>


namespace gr {
  namespace satnogs {

    class cw_matched_filter_ff_impl : public cw_matched_filter_ff
    {
     private:
	    /**
	     * The duration of the dot in seconds
	     */
	    const double d_dot_duration;
	    /**
	     * If set to true, this block produces the energy of the filtered
	     * samples, rather the samples themselves
	     */
	    const bool d_produce_enrg;
	    /**
	     * The duration of the dot in number of samples
	     */
	    const size_t d_dot_samples;

	    float *d_sin_wave;

     public:
      cw_matched_filter_ff_impl(double sampling_rate, double carrier_freq,
                                size_t wpm, bool energy_out);
      ~cw_matched_filter_ff_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_IMPL_H */

