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

#ifndef INCLUDED_SATNOGS_CW_TO_SYMBOL_H
#define INCLUDED_SATNOGS_CW_TO_SYMBOL_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief The CW to Symbol block tries to translate the received signal
     * power time-series into Morse symbols.
     * \ingroup satnogs
     */
    class SATNOGS_API cw_to_symbol : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<cw_to_symbol> sptr;

      /*!
       * The CW to Symbol block tries to translate the received signal
       * power time-series into Morse symbols.
       *
       * The input stream is the energy of the signal in the time domain.
       * For best results, a proper moving average filter should be applied
       * before.
       *
       * Upon a successful recovery of a symbol it produces an appropriate
       * asynchronous message that can directly be used by the Morse decoder
       * block.
       *
       * @param sampling_rate the sampling rate of the signal
       * @param threshold the activation threshold
       * @param conf_level the confidence level, for the decisions made by
       * the decoder. Higher values, means that the decoder would be more
       * conservative, whereas lower may help in noisy environments but may
       * trigger false alarms too, especially for the case of short pauses
       * symbols
       *
       * @param wpm Morse code Words per Minute
       *
       * @param auto_config if set to true, the block will try first to
       * automatically adjust the WPM in order to extract the dot and dash
       * durations. If set to false, the given WPM is used.
       */
      static cw_to_symbol::sptr
      make (double sampling_rate, float threshold, float conf_level = 0.9,
	    size_t wpm = 20, bool auto_config = false);

      virtual void set_act_threshold(float thrld) = 0;

      virtual void start_timing_recovery() = 0;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_TO_SYMBOL_H */

