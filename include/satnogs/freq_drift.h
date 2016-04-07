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

#ifndef INCLUDED_SATNOGS_FREQ_DRIFT_H
#define INCLUDED_SATNOGS_FREQ_DRIFT_H

#include <satnogs/api.h>
#include <stdint.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Class that specifies the frequency drift at a given time.
     * The time is measured in samples.
     * \ingroup satnogs
     */
    class SATNOGS_API freq_drift
    {
    public:
      freq_drift (uint64_t x, double y);
      ~freq_drift ();
      double
      get_freq_drift () const;
      void
      set_freq_drift (double freqDrift);
      uint64_t
      get_x () const;
      void
      set_x (uint64_t x);

    private:
      uint64_t d_x;
      double d_freq_drift;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FREQ_DRIFT_H */

