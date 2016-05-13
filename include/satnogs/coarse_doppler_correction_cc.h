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

#ifndef INCLUDED_SATNOGS_COARSE_DOPPLER_CORRECTION_CC_H
#define INCLUDED_SATNOGS_COARSE_DOPPLER_CORRECTION_CC_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block corrects the doppler effect between the ground
     * station and the satellite in a coarse and very simplified way.
     * Instead of changing the hardware center frequency, we use an NCO
     * to digitally compensate the doppler effect.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API coarse_doppler_correction_cc : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<coarse_doppler_correction_cc> sptr;

      /**
       * The doppler correction block. The input is the complex signal at
       * baseband as it comes from the SDR device.
       *
       * The message input \p freq receives periodically messages containing
       * the predicted absolute frequency of the satellite at that specific time.
       * @param target_freq the absolute frequency of the satellite
       * @param sampling_rate the sampling rate of the signal
       */
      static sptr
      make (double target_freq, double sampling_rate);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_COARSE_DOPPLER_CORRECTION_CC_H */

