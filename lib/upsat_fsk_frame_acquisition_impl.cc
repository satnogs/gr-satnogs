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
#include "upsat_fsk_frame_acquisition_impl.h"

namespace gr
{
  namespace satnogs
  {

    upsat_fsk_frame_acquisition::sptr
    upsat_fsk_frame_acquisition::make (const std::vector<uint8_t> &preamble,
				       const std::vector<uint8_t> &sync_word,
				       bool whitening, bool manchester)
    {
      return gnuradio::get_initial_sptr (
	  new upsat_fsk_frame_acquisition_impl (preamble, sync_word, whitening,
						manchester));
    }

    /*
     * The private constructor
     */
    upsat_fsk_frame_acquisition_impl::upsat_fsk_frame_acquisition_impl (
	const std::vector<uint8_t> &preamble,
	const std::vector<uint8_t> &sync_word, bool whitening, bool manchester) :
	    gr::sync_block ("upsat_fsk_frame_acquisition",
			    gr::io_signature::make (1, 1, sizeof(float)),
			    gr::io_signature::make (0, 0, 0))
    {
    }

    /*
     * Our virtual destructor.
     */
    upsat_fsk_frame_acquisition_impl::~upsat_fsk_frame_acquisition_impl ()
    {
    }

    int
    upsat_fsk_frame_acquisition_impl::work (
	int noutput_items, gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

