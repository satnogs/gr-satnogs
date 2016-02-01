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

#ifndef INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_H
#define INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief This block implements a matched filter to reduce the noise
     * level of the received CW signal.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API cw_matched_filter_ff : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<cw_matched_filter_ff> sptr;

      /*!
       * \brief Matched filter for CW noise reduction
       *
       * @param sampling_rate the sampling rate of the signal
       * @param carrier_freq the audio frequency of the CW signal
       * @param wpm Morse code Words per Minute
       * @param energy_out if true, the filter produces the energy of the
       * resulting filtered samples, otherwise the raw filter samples are
       * produced. This is handy, in order to save the flowgraph from
       * am additional signal energy block.
       */
      static sptr make(double sampling_rate, double carrier_freq = 500,
		       size_t wpm = 20,
		       bool energy_out = false);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_MATCHED_FILTER_FF_H */

