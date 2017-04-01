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
#include "coarse_doppler_correction_cc_impl.h"
#include <volk/volk.h>
#include <satnogs/log.h>

namespace gr
{
  namespace satnogs
  {

    coarse_doppler_correction_cc::sptr
    coarse_doppler_correction_cc::make (double target_freq,
					double sampling_rate)
    {
      return gnuradio::get_initial_sptr (
	  new coarse_doppler_correction_cc_impl (target_freq, sampling_rate));
    }

    /*
     * The private constructor
     */
    coarse_doppler_correction_cc_impl::coarse_doppler_correction_cc_impl (
	double target_freq, double sampling_rate) :
	    gr::sync_block ("coarse_doppler_correction_cc",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (1, 1, sizeof(gr_complex))),
	    d_target_freq (target_freq),
	    d_samp_rate (sampling_rate),
	    d_buf_items (std::min ((size_t)8192UL, (size_t) (d_samp_rate / 4))),
	    d_freq_diff (0),
	    d_nco ()
    {
      message_port_register_in (pmt::mp ("freq"));

      /*
       * NOTE:
       * Set the maximum number of samples to be equivalent of half a second.
       * With this way we are sure that at least one frequency message
       * per second will be processed.
       *
       * This is taken into consideration due to the fact that the work()
       * and the input message handler are NOT reentrant.
       */
      set_max_noutput_items (d_samp_rate / 4.0);
      set_alignment (8);

      set_msg_handler (
	  pmt::mp ("freq"),
	  boost::bind (&coarse_doppler_correction_cc_impl::new_freq, this, _1));

      d_nco.set_freq (0);
      /* Allocate aligned memory for the NCO */
      d_nco_buff = (gr_complex *) volk_malloc (
	  (d_samp_rate / 4) * sizeof(gr_complex), 32);
      if (!d_nco_buff) {
	throw std::runtime_error ("Could not allocate NCO memory");
      }
    }

    void
    coarse_doppler_correction_cc_impl::new_freq (pmt::pmt_t msg)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      double new_freq;
      new_freq = pmt::to_double (msg);
      d_freq_diff = new_freq - d_target_freq;
      d_nco.set_freq ((2 * M_PI * (-d_freq_diff)) / d_samp_rate);
    }

    /*
     * Our virtual destructor.
     */
    coarse_doppler_correction_cc_impl::~coarse_doppler_correction_cc_impl ()
    {
      volk_free (d_nco_buff);
    }

    int
    coarse_doppler_correction_cc_impl::work (
	int noutput_items, gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      /* Perform the correction */
      d_nco.sincos (d_nco_buff, noutput_items, 1.0);
      volk_32fc_x2_multiply_32fc (out, in, d_nco_buff, noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void
    coarse_doppler_correction_cc_impl::set_target_freq (double freq)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      d_target_freq = freq;
      d_freq_diff = 0.0;
      d_nco.set_freq (0);
    }

  } /* namespace satnogs */
} /* namespace gr */

