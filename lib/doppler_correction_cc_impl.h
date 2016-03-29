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

#ifndef INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_IMPL_H
#define INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_IMPL_H

#include <satnogs/doppler_correction_cc.h>
#include <deque>

namespace gr
{
  namespace satnogs
  {

    class doppler_correction_cc_impl : public doppler_correction_cc
    {
    private:
      const double d_target_freq;
      const double d_samp_rate;
      const double d_update_period;
      const size_t d_est_thrhld;

      double d_freq_diff;
      bool d_have_est;
      size_t d_freq_est_num;
      std::deque<double> d_doppler_freqs;
      boost::mutex d_mutex;

      void
      new_freq (pmt::pmt_t msg);

      void
      reset (pmt::pmt_t msg);

    public:
      doppler_correction_cc_impl (double target_freq, double sampling_rate,
				  size_t corrections_per_sec);
      ~doppler_correction_cc_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_IMPL_H */

