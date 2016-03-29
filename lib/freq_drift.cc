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
#include <satnogs/freq_drift.h>

namespace gr
{
  namespace satnogs
  {

    freq_drift::freq_drift (uint64_t x, double y) :
	    d_x (x),
	    d_freq_drift (y)
    {
    }

    freq_drift::~freq_drift ()
    {
    }

    double
    freq_drift::get_freq_drift () const
    {
      return d_freq_drift;
    }

    void
    freq_drift::set_freq_drift (double freq_drift)
    {
      d_freq_drift = freq_drift;
    }

    uint64_t
    freq_drift::get_x () const
    {
      return d_x;
    }

    void
    freq_drift::set_x (uint64_t x)
    {
      d_x = x;
    }

  } /* namespace satnogs */
} /* namespace gr */

