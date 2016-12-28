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

#ifndef INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_IMPL_H
#define INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_IMPL_H

#include <satnogs/power_spectral_density_cf.h>

namespace gr
{
  namespace satnogs
  {

    class power_spectral_density_cf_impl : public power_spectral_density_cf
    {
    private:
      const float d_norm;
      const float d_rbw;
      const size_t d_vlen;

    public:
      power_spectral_density_cf_impl (const float normalization_factor,
                                      const float rbw, const size_t vlen);
      ~power_spectral_density_cf_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_IMPL_H */

