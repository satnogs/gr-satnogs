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

#ifndef INCLUDED_SATNOGS_AFSK_DECODER_IMPL_H
#define INCLUDED_SATNOGS_AFSK_DECODER_IMPL_H

#include <satnogs/afsk_decoder.h>

namespace gr
{
  namespace satnogs
  {

    class afsk_decoder_impl : public afsk_decoder
    {
    private:
      const double d_samp_rate;
      const size_t d_samples_per_sym;
      const size_t d_fft_size;
      bool d_carriers_found;
      double d_mark_freq;
      double d_space_freq;


    public:
      afsk_decoder_impl (double sampling_rate, size_t baudrate, size_t fft_size,
			 bool auto_carrier_tracking, double mark_freq,
			 double space_freq);
      ~afsk_decoder_impl ();

      // Where all the action really happens
      void
      forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int
      general_work (int noutput_items, gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AFSK_DECODER_IMPL_H */

