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
#include "leo_channel_impl.h"

namespace gr
{
  namespace satnogs
  {

    leo_channel::sptr
    leo_channel::make (const double freq, const double samp_rate,
		       const double sat_altitude, const double sat_inclination,
		       const size_t pass_duration_sec,
		       const size_t freq_shifts_per_sec)
    {
      return gnuradio::get_initial_sptr (
	  new leo_channel_impl (freq, freq_shifts_per_sec));
    }

    /*
     * The private constructor
     */
    leo_channel_impl::leo_channel_impl (const double freq,
					const size_t freq_shifts_per_sec) :
	    gr::sync_block ("leo_channel",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (1, 1, sizeof(gr_complex))),
	    d_freq (freq),
	    d_shifts_per_sec (freq_shifts_per_sec)
    {
    }

    /*
     * Our virtual destructor.
     */
    leo_channel_impl::~leo_channel_impl ()
    {
    }

    int
    leo_channel_impl::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

