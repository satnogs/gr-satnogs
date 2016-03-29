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

#ifndef INCLUDED_SATNOGS_DOPPLER_FIT_H
#define INCLUDED_SATNOGS_DOPPLER_FIT_H

#include <satnogs/api.h>
#include <satnogs/freq_drift.h>
#include <deque>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Doppler frequency polynomial fitting tool
     * \ingroup satnogs
     */
    class SATNOGS_API doppler_fit
    {
    public:
      doppler_fit (size_t degree);
      ~doppler_fit ();

      void fit(std::deque<freq_drift> drifts);

    private:
      const size_t d_degree;
      std::vector<double> d_coeff;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DOPPLER_FIT_H */

