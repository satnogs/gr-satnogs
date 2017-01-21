/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_WATERFALL_SINK_H
#define INCLUDED_SATNOGS_WATERFALL_SINK_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block computes the waterfall of the incoming signal
     * and stores the result to a file.
     *
     * The file has a special header, so that the satnogs_waterfall Gnuplot
     * script to be able to plot it properly.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API waterfall_sink : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<waterfall_sink> sptr;

      /**
       * This block computes the waterfall of the incoming signal
       * and stores the result to a file.
       *
       * The file has a special header, so that the satnogs_waterfall Gnuplot
       * script to be able to plot it properly.
       *
       * @param samp_rate the sampling rate
       * @param center_freq the observation center frequency. Used only for
       * plotting reasons. For a normalized frequency x-axis set it to 0.
       * @param pps pixels per second
       * @param fft_size FFT size
       * @param filename the name of the output file
       * @param mode the mode that the waterfall.
       * - 0: Simple decimation
       * - 1: Max hold
       * - 2: Mean energy
       *
       * @return shared pointer to the object
       */
      static sptr
      make (double samp_rate, double center_freq,
            double pps, size_t fft_size,
            const std::string& filename, int mode = 0);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_WATERFALL_SINK_H */

