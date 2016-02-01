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
#include "cw_matched_filter_ff_impl.h"

#include <volk/volk.h>
#include <gnuradio/fxpt_nco.h>

namespace gr {
  namespace satnogs {

    cw_matched_filter_ff::sptr
    cw_matched_filter_ff::make(double sampling_rate, double carrier_freq,
			       size_t wpm, bool energy_out)
    {
      return gnuradio::get_initial_sptr
        (new cw_matched_filter_ff_impl(sampling_rate, carrier_freq,
				       wpm, energy_out));
    }

    /*
     * The private constructor
     */
    cw_matched_filter_ff_impl::cw_matched_filter_ff_impl (double sampling_rate,
							  double carrier_freq,
							  size_t wpm,
							  bool energy_out) :
	gr::sync_block ("cw_matched_filter_ff",
			gr::io_signature::make (1, 1, sizeof(float)),
			gr::io_signature::make (1, 1, sizeof(float))),
			d_dot_duration(1.2/wpm),
			d_produce_enrg(energy_out),
			d_dot_samples(d_dot_duration / (1.0 / sampling_rate))
    {
      const int alignment_multiple = volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1,alignment_multiple));
      set_history(d_dot_samples);

      d_sin_wave = (float *)volk_malloc(d_dot_samples * sizeof(float), 32);
      if(!d_sin_wave){
	  throw std::runtime_error("Could not allocate sine wave buffer");
      }

      /* Now fill the buffer with the appropriate sine wave */
      gr::fxpt_nco nco;
      nco.set_freq(2 * M_PI * carrier_freq / sampling_rate);
      nco.sin(d_sin_wave, d_dot_samples, 1.0);
    }

    /*
     * Our virtual destructor.
     */
    cw_matched_filter_ff_impl::~cw_matched_filter_ff_impl()
    {
      volk_free(d_sin_wave);
    }

    int
    cw_matched_filter_ff_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];

        for(int i = 0; i < noutput_items; i++ ){
            volk_32f_x2_dot_prod_32f(out + i, in + i, d_sin_wave,
				     d_dot_samples);
        }
        if(d_produce_enrg){
          volk_32f_s32f_power_32f(out, out, 2, noutput_items);
        }

        return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

