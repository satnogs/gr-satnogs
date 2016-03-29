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

#ifndef INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_H
#define INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block corrects the doppler effect between the ground
     * station and the satellite. It takes the imput stream in baseband
     * and applies proper corrections to keep the carrier at the desired
     * frequency. To achieve that it uses messages containing the absolute
     * predicted frequency of the satellite from software like Gpredict.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API doppler_correction_cc : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<doppler_correction_cc> sptr;

      /*!
       * The doppler correction block. The input is the complex signal at
       * baseband as it comes from the SDR device. The message input \p freq
       * received periodically messages containing the predicted absolute
       * frequency of the satellite at the moment of the
       * @param target_freq the absolute frequency of the satellite
       * @param sampling_rate the sampling rate of the signal
       * @param corrections_per_sec the number of the corrections every second
       * that the block should perform
       */
      static sptr
      make (double target_freq, double sampling_rate,
	    size_t corrections_per_sec = 1000);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DOPPLER_CORRECTION_CC_H */

