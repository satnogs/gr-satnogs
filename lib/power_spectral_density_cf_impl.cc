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
#include "power_spectral_density_cf_impl.h"

namespace gr
{
  namespace satnogs
  {

    power_spectral_density_cf::sptr
    power_spectral_density_cf::make (const float normalization_factor,
                                     const float rbw, const size_t vlen)
    {
      return gnuradio::get_initial_sptr (
          new power_spectral_density_cf_impl (normalization_factor, rbw, vlen));
    }

    /*
     * The private constructor
     */
    power_spectral_density_cf_impl::power_spectral_density_cf_impl (
        const float normalization_factor, const float rbw, const size_t vlen) :
            gr::sync_block (
                "power_spectral_density_cf",
                gr::io_signature::make (1, 1, vlen * sizeof(gr_complex)),
                gr::io_signature::make (1, 1, sizeof(float))),
            d_norm (normalization_factor),
            d_rbw (rbw),
            d_vlen (vlen)
    {
      const int alignment_multiple = volk_get_alignment ()
          / (d_vlen * sizeof(gr_complex));
      set_alignment (std::max (1, alignment_multiple));
    }

    /*
     * Our virtual destructor.
     */
    power_spectral_density_cf_impl::~power_spectral_density_cf_impl ()
    {
    }

    int
    power_spectral_density_cf_impl::work (
        int noutput_items, gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];

      volk_32fc_s32f_x2_power_spectral_density_32f (out, in, d_norm, d_rbw,
                                                    d_vlen * noutput_items);
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

