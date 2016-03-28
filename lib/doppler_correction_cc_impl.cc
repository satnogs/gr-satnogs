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
#include "doppler_correction_cc_impl.h"

namespace gr
{
  namespace satnogs
  {

    doppler_correction_cc::sptr
    doppler_correction_cc::make (double target_freq, double sampling_rate,
				 size_t corrections_per_sec)
    {
      return gnuradio::get_initial_sptr (
	  new doppler_correction_cc_impl (target_freq, sampling_rate,
					  corrections_per_sec));
    }

    /*
     * The private constructor
     */
    doppler_correction_cc_impl::doppler_correction_cc_impl (
	double target_freq, double sampling_rate, size_t corrections_per_sec) :
	    gr::sync_block ("doppler_correction_cc",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (1, 1, sizeof(gr_complex))),
	    d_target_freq (target_freq),
	    d_samp_rate (sampling_rate),
	    d_update_period (sampling_rate / (double) corrections_per_sec),
	    d_freq_diff (0.0)
    {
      message_port_register_in (pmt::mp ("freq"));
      /*
       * Set the maximum number of samples to be equivalent of half a second.
       * With this way we are sure that at least one frequency message
       * per second will be processed.
       */
      set_max_noutput_items (d_samp_rate / 2.0);
    }

    /*
     * Our virtual destructor.
     */
    doppler_correction_cc_impl::~doppler_correction_cc_impl ()
    {
    }

    int
    doppler_correction_cc_impl::work (int noutput_items,
				      gr_vector_const_void_star &input_items,
				      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

