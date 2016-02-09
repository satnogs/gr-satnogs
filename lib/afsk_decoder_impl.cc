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
#include "afsk_decoder_impl.h"

namespace gr
{
  namespace satnogs
  {

    afsk_decoder::sptr
    afsk_decoder::make (double sampling_rate, size_t samples_per_sym,
		       size_t fft_size,
		       bool auto_carrier_tracking,
		       double mark_freq,
		       double space_freq)
    {
      return gnuradio::get_initial_sptr (
	  new afsk_decoder_impl (sampling_rate, samples_per_sym, fft_size,
				 auto_carrier_tracking,
				 mark_freq, space_freq));
    }
    /*
     * The private constructor
     */
    afsk_decoder_impl::afsk_decoder_impl (double sampling_rate, size_t samples_per_sym,
					  size_t fft_size,
					  bool auto_carrier_tracking,
					  double mark_freq, double space_freq) :
	gr::block ("afsk_decoder",
		   gr::io_signature::make (1, 1, fft_size * sizeof(float)),
		   gr::io_signature::make (1, 1, sizeof(char))),
		   d_samp_rate(sampling_rate),
		   d_samples_per_sym(samples_per_sym),
		   d_fft_size(fft_size),
		   d_carriers_found(!auto_carrier_tracking),
		   d_mark_freq(mark_freq),
		   d_space_freq(space_freq)
    {
      if(auto_carrier_tracking){
	d_mark_freq = d_space_freq = 0.0;
      }
    }

    /*
     * Our virtual destructor.
     */
    afsk_decoder_impl::~afsk_decoder_impl ()
    {
    }

    void
    afsk_decoder_impl::forecast (int noutput_items,
				 gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    afsk_decoder_impl::general_work (int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];

      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

