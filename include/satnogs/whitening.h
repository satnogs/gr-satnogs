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

#ifndef INCLUDED_SATNOGS_WHITENING_H
#define INCLUDED_SATNOGS_WHITENING_H

#include <satnogs/api.h>
#include <gnuradio/digital/lfsr.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Performs data whitening and de-whitening
     *
     */
    class SATNOGS_API whitening
    {
    public:
      whitening (uint32_t mask, uint32_t seed, uint32_t order);

      void
      reset();

      void scramble(uint8_t *out, const uint8_t *in, size_t len);
      void descramble(uint8_t *out, const uint8_t *in, size_t len);

      void
      scramble_one_bit_per_byte (uint8_t *out, const uint8_t *in,
				 size_t bits_num);
      void
      descramble_one_bit_per_byte (uint8_t *out, const uint8_t *in,
				   size_t bits_num);

    private:
      digital::lfsr d_lfsr;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_WHITENING_H */

