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

#ifndef INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_H
#define INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief <+description of block+>
     * \ingroup satnogs
     *
     */
    class SATNOGS_API power_spectral_density_cf : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<power_spectral_density_cf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of satnogs::power_spectral_density_cf.
       *
       * To avoid accidental use of raw pointers, satnogs::power_spectral_density_cf's
       * constructor is in a private implementation
       * class. satnogs::power_spectral_density_cf::make is the public interface for
       * creating new instances.
       */
      static sptr
      make (const float normalization_factor, const float rbw,
            const size_t vlen);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_POWER_SPECTRAL_DENSITY_CF_H */

