/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2017
 *  Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_H
#define INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief A Morse debug source block that supports injection of random
     * errors based on a Bernulli distribution with probability p.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API morse_debug_source : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<morse_debug_source> sptr;

      /*!
       * \brief A Morse debug source block that produces messages corresponding
       * to Morse symbols, based on the given debug_seq string.
       * This block can also inject random errors, based on a Bernoulli
       * distribution.
       *
       * @param wpm words per minute
       * @param debug_seq A string containing the debug sentence
       * @param inject_errors if set the debug source block will produce
       * errors that follow a Bernoulli distribution
       * @param error_prob the probability p of error for the Bernulli distribution
       * @param seq_pause_ms pause in milliseconds between concecutive debug
       * sequences. For simplicity the pause duration will be scaled to multiple
       * dot durations.
       */
      static sptr
      make (const size_t wpm,
            const std::string& debug_seq,
            bool inject_errors = false,
            float error_prob = 0.1,
            size_t seq_pause_ms = 1000);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_H */

