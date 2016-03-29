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
	    d_est_thrhld(7),
	    d_freq_diff (0.0),
	    d_have_est(false),
	    d_freq_est_num(0)
    {
      message_port_register_in (pmt::mp ("freq"));
      message_port_register_in (pmt::mp ("reset"));
      /*
       * Set the maximum number of samples to be equivalent of half a second.
       * With this way we are sure that at least one frequency message
       * per second will be processed.
       *
       * This is taken into consideration due to the fact that the work()
       * and the input message handler are not reentrant.
       */
      set_max_noutput_items (d_samp_rate / 2.0);

      set_msg_handler(pmt::mp("freq"),
		      boost::bind(&doppler_correction_cc_impl::new_freq,
				  this, _1));
      set_msg_handler(pmt::mp("reset"),
		      boost::bind(&doppler_correction_cc_impl::reset,
				  this, _1));
    }

    void
    doppler_correction_cc_impl::new_freq (pmt::pmt_t msg)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      double new_freq;
      new_freq = pmt::to_double(msg);
      d_freq_diff = d_target_freq - new_freq;
      if(!d_have_est){
	d_freq_est_num++;
	if(d_freq_est_num > d_est_thrhld - 1){
	  d_have_est = true;
	}
	d_doppler_freqs.push_back(d_freq_diff);
      }
      else{
	d_doppler_freqs.pop_front();
	d_doppler_freqs.push_back(d_freq_diff);
      }
    }

    void
    doppler_correction_cc_impl::reset (pmt::pmt_t msg)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      d_doppler_freqs.clear();
      d_freq_est_num = 0;
      d_have_est = false;
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

      /*
       * If we do not have an estimation yet, just copy the input to the output.
       * Otherwise perform Doppler correction, using the fitted curve indicating
       * the frequency drift.
       */
      if(d_have_est){
      }
      else{
	memcpy(out, in, noutput_items * sizeof(gr_complex));
      }

      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

