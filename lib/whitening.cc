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
	    d_lut_len (std::pow (2, order)),
	    d_lut_idx (0)
    {
      size_t i;
      size_t j;
      uint32_t cnt;
      uint32_t shift_reg = seed;

      if (order > 32) {
	throw std::invalid_argument ("The maximum allowed order is 32");
      }

      d_lut = new uint8_t[d_lut_len];

      for (i = 0; i < d_lut_len; i++) {
	d_lut[i] = shift_reg & 0xFF;
	for (j = 0; j < 8; j++) {
	  cnt = bit_count (shift_reg & mask) % 2;
	  shift_reg = shift_reg >> 1;
	  shift_reg |= cnt << (order - 1);
	}
      }
    }

    whitening::~whitening()
    {
      delete [] d_lut;
    }

    /**
     * Resets the scrambler (or the descrambler) to the initial stage and
     * the initial seed.
     */
    void
    whitening::reset ()
    {
      d_lut_idx = 0;
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
      for(i = 0; i < len; i++){
	out[i] = in[i] ^ d_lut[ (d_lut_idx + i ) % d_lut_len];
      }
      d_lut_idx = (d_lut_idx + len ) % d_lut_len;
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

  } /* namespace satnogs */
} /* namespace gr */

