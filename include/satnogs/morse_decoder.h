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

#ifndef INCLUDED_SATNOGS_MORSE_DECODER_H
#define INCLUDED_SATNOGS_MORSE_DECODER_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace satnogs {
    /*!
     * \brief Morse code decoder block.
     *
     * This block received messages from the previous blocks
     * and try to decode the dot and dashes into clear text.
     */
    class SATNOGS_API morse_decoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<morse_decoder> sptr;

      /*!
       * Creates a Morse decoder block
       * @param unrecognized_char the character that will be placed
       * in situations where the decoder can not decide which character
       * was received.
       * @param min_frame_len the minimum frame length in order to reduce
       * false alarms
       */
      static sptr make(char unrecognized_char = '#', size_t min_frame_len = 3);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_DECODER_H */

