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

#ifndef INCLUDED_SATNOGS_MORSE_DECODER_IMPL_H
#define INCLUDED_SATNOGS_MORSE_DECODER_IMPL_H

#include <satnogs/morse_decoder.h>
#include <satnogs/morse_tree.h>

namespace gr {
  namespace satnogs {

    class morse_decoder_impl : public morse_decoder
    {
     private:
      morse_tree d_morse_tree;
      size_t d_min_frame_len;

      void
      symbol_msg_handler(pmt::pmt_t msg);

     public:
      morse_decoder_impl(char unrecognized_char, size_t min_frame_len);

    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_DECODER_IMPL_H */

