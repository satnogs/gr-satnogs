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
#include <volk/volk.h>

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
	    d_est_thrhld (7),
	    d_corrections_per_sec (corrections_per_sec),
	    d_nco (),
	    /* A 3-rd order polynomial curve fitting is more than enough */
	    d_doppler_fit_engine (3),
	    d_freq_diff (0.0),
	    d_have_est (false),
	    d_freq_est_num (0),
	    d_corrections (0),
	    d_corrected_samples (0)
    {
      message_port_register_in (pmt::mp ("freq"));
      message_port_register_in (pmt::mp ("reset"));

      /*
       * NOTE:
       * Set the maximum number of samples to be equivalent of half a second.
       * With this way we are sure that at least one frequency message
       * per second will be processed.
       *
       * This is taken into consideration due to the fact that the work()
       * and the input message handler are NOT reentrant.
       */
      set_max_noutput_items (d_samp_rate / 2.0);
      set_alignment(8);

      set_msg_handler (
	  pmt::mp ("freq"),
	  boost::bind (&doppler_correction_cc_impl::new_freq, this, _1));
      set_msg_handler (
	  pmt::mp ("reset"),
	  boost::bind (&doppler_correction_cc_impl::reset, this, _1));

      /* Allocate the buffer that will hold the predicted frequency differences */
      d_predicted_freqs = new double[d_corrections_per_sec];

      /* Allocate aligned memory for the NCO */
      d_nco_buff = (gr_complex *) volk_malloc (
	  d_update_period * sizeof(gr_complex), 32);
      if (!d_nco_buff) {
	throw std::runtime_error ("Could not allocate NCO memory");
      }
    }

    void
    doppler_correction_cc_impl::new_freq (pmt::pmt_t msg)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      double new_freq;
      new_freq = pmt::to_double (msg);
      d_freq_diff = d_target_freq - new_freq;
      if (!d_have_est) {
	d_freq_est_num++;
	if (d_freq_est_num > d_est_thrhld - 1) {
	  d_have_est = true;
	}
	d_doppler_freqs.push_back (
	    freq_drift (nitems_written (0), d_freq_diff));
      }
      else {
	d_doppler_freqs.pop_front ();
	d_doppler_freqs.push_back (
	    freq_drift (nitems_written (0), d_freq_diff));

	/* Fit the doppler drift based on the new estimated frequency */
	d_doppler_fit_engine.fit (d_doppler_freqs);
	/* Predict the frequency differences for the near future */
	d_doppler_fit_engine.predict_freqs (d_predicted_freqs,
					    d_corrections_per_sec,
					    d_update_period);
      }
    }

    void
    doppler_correction_cc_impl::reset (pmt::pmt_t msg)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      d_doppler_freqs.clear ();
      d_freq_est_num = 0;
      d_corrections = 0;
      d_have_est = false;
    }

    /*
     * Our virtual destructor.
     */
    doppler_correction_cc_impl::~doppler_correction_cc_impl ()
    {
      delete[] d_predicted_freqs;
      volk_free (d_nco_buff);
    }

    int
    doppler_correction_cc_impl::work (int noutput_items,
				      gr_vector_const_void_star &input_items,
				      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int produced = 0;
      size_t cnt;

      /*
       * If we do not have an estimation yet, just copy the input to the output.
       * Otherwise perform Doppler correction, using the fitted curve indicating
       * the frequency drift.
       */
      if (d_have_est) {
	while (produced < noutput_items) {
	  /*
	   * If no samples have been corrected from the current correction step
	   * compute and store the NCO buffer with the corresponding frequency
	   */
	  if (d_corrected_samples == 0) {
	    d_nco.set_freq (
		2 * M_PI * (-d_predicted_freqs[d_corrections]) / d_samp_rate);
	    d_nco.sincos (d_nco_buff, d_update_period, 1.0);
	    d_corrections++;

	    /*
	     * The doppler estimation may fail/delay. In such a case the block
	     * should continue using the predicted frequencies
	     */
	    if (d_corrections == d_corrections_per_sec) {
	      d_doppler_fit_engine.predict_freqs (d_predicted_freqs,
						  d_corrections_per_sec,
						  d_update_period);
	      d_corrections = 0;
	    }

	    cnt = std::min(d_update_period - d_corrected_samples,
			   (size_t) (noutput_items - produced));
	    /* Perform the doppler shift correction */
	    volk_32fc_x2_multiply_32fc (out + produced, in + produced,
					d_nco_buff + d_corrected_samples, cnt);

	    /* Make the proper advances */
	    produced += (int) cnt;
	    d_corrected_samples += cnt;

	    if(d_corrected_samples == d_update_period) {
	      d_corrected_samples = 0;
	    }
	  }
	}
      }
      else {
	memcpy (out, in, noutput_items * sizeof(gr_complex));
      }

      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

