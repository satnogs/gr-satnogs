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
#include <satnogs/morse_tree.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief Morse code decoder block.
     *
     * This block received messages from the previous blocks
     * and try to decode the dot and dashes into clear text.
     */
    class SATNOGS_API morse_decoder
    {
    public:
      morse_decoder();
      ~morse_decoder();
    private:
      morse_tree d_morse_tree;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_DECODER_H */

