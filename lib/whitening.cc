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
#include <satnogs/whitening.h>
#include <satnogs/utils.h>

namespace gr {
  namespace satnogs {

    /**
     * Data whitening and de-whitening class
     * @param mask the polynomial mask
     * @param seed the initial seed
     * @param order the order of the shift register. This is equal to the
     * number of memory stages.
     */
    whitening::whitening (uint32_t mask, uint32_t seed, uint32_t order) :
	    d_lfsr(mask, seed, order)
    {
    }

    /**
     * Resets the scrambler (or the descrambler) to the initial stage and
     * the initial seed.
     */
    void
    whitening::reset ()
    {
      d_lfsr.reset();
    }

    /**
     * Performs data scrambling
     * @param out the output buffer
     * @param in the input buffer
     * @param len the number of the bytes to be scrambled
     */
    void
    whitening::scramble (uint8_t* out, const uint8_t* in, size_t len)
    {
      size_t i;
      uint8_t b;
      for(i = 0; i < len; i++){
	b = d_lfsr.next_bit();
	b |= d_lfsr.next_bit() << 1;
	b |= d_lfsr.next_bit() << 2;
	b |= d_lfsr.next_bit() << 3;
	b |= d_lfsr.next_bit() << 4;
	b |= d_lfsr.next_bit() << 5;
	b |= d_lfsr.next_bit() << 6;
	b |= d_lfsr.next_bit() << 7;
	out[i] = in[i] ^ b;
      }
    }

    /**
     * Performs data de-scrambling
     * @param out the output buffer
     * @param in the input buffer
     * @param len the number of the bytes to be de-scrambled
     */
    void
    whitening::descramble (uint8_t* out, const uint8_t* in, size_t len)
    {
      scramble(out, in, len);
    }

    /**
     * Performs data scrambling. The input and output buffer
     * contain one bit per byte
     * @param out the output buffer
     * @param in the input buffer
     * @param bits_num the number of bits to be scrambled
     */
    void
    whitening::scramble_one_bit_per_byte (uint8_t* out, const uint8_t* in,
					  size_t bits_num)
    {
      size_t i;
      for(i = 0; i < bits_num; i++){
	out[i] = in[i] ^ d_lfsr.next_bit();
      }
    }

    /**
     * Performs data descrambling. The input and output buffer
     * contain one bit per byte
     * @param out the output buffer
     * @param in the input buffer
     * @param bits_num the number of bits to be descrambled
     */
    void
    whitening::descramble_one_bit_per_byte (uint8_t* out, const uint8_t* in,
					    size_t bits_num)
    {
      scramble_one_bit_per_byte(out, in, bits_num);
    }

  } /* namespace satnogs */
} /* namespace gr */
