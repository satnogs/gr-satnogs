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
#include <volk/volk.h>
#include <gnuradio/fxpt_nco.h>

#include "sine_matched_filter_ff_impl.h"

namespace gr {
  namespace satnogs {

    sine_matched_filter_ff::sptr
    sine_matched_filter_ff::make(double sampling_rate, double sine_freq,
				 double baudrate, bool compute_energy)
    {
      return gnuradio::get_initial_sptr
        (new sine_matched_filter_ff_impl(sampling_rate, sine_freq,
					 baudrate, compute_energy));
    }

    /*
     * The private constructor
     */
    sine_matched_filter_ff_impl::sine_matched_filter_ff_impl(double sampling_rate,
							     double sine_freq,
							     double baudrate,
							     bool compute_energy)
      : gr::sync_block("sine_matched_filter_ff",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float))),
	      d_samp_rate(sampling_rate),
	      d_baud_rate(baudrate),
	      d_produce_enrg(compute_energy),
	      d_filter_taps(2 * std::ceil(sampling_rate / baudrate))
    {
      const int alignment_multiple = volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1,alignment_multiple));
      set_history(d_filter_taps);

      /*
       * TODO: Perhaps is better that now phase offset exists at the end
       * of the buffer
       */
      d_sin_wave = (float *)volk_malloc(d_filter_taps * sizeof(float), 32);
      if(!d_sin_wave){
	  throw std::runtime_error("Could not allocate sine wave buffer");
      }

      /* Register the input port for frequency change messages */
      message_port_register_in(pmt::mp("freq"));
      set_msg_handler(pmt::mp("freq"),
		      boost::bind(&sine_matched_filter_ff_impl::new_freq_msg_handler,
				  this, _1));

      /* Now fill the buffer with the appropriate sine wave */
      gr::fxpt_nco nco;
      nco.set_freq(2 * M_PI * sine_freq / sampling_rate);
      nco.sin(d_sin_wave, d_filter_taps, 1.0);
    }

    void
    sine_matched_filter_ff_impl::new_freq_msg_handler (pmt::pmt_t msg)
    {
      if (pmt::is_pair (msg)) {
	set_new_freq (pmt::to_double (pmt::cdr (msg)));
      }
    }

    /*
     * Our virtual destructor.
     */
    sine_matched_filter_ff_impl::~sine_matched_filter_ff_impl()
    {
      volk_free(d_sin_wave);
    }

    int
    sine_matched_filter_ff_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
	volk_32f_x2_dot_prod_32f (out + i, in + i, d_sin_wave, d_filter_taps);
      }
      if (d_produce_enrg) {
	volk_32f_s32f_power_32f (out, out, 2, noutput_items);
      }

      return noutput_items;
    }

    void
    sine_matched_filter_ff_impl::set_new_freq (double freq)
    {
      gr::fxpt_nco nco;
      nco.set_freq (2 * M_PI * freq / d_samp_rate);
      nco.sin (d_sin_wave, d_filter_taps, 1.0);
    }

    void
    sine_matched_filter_ff_impl::set_new_freq_locked (double freq)
    {
      boost::mutex::scoped_lock lock(d_mutex);
      set_new_freq(freq);
    }

  } /* namespace satnogs */
} /* namespace gr */

