/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017
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

#ifndef INCLUDED_SATNOGS_CW_ENCODER_H
#define INCLUDED_SATNOGS_CW_ENCODER_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief CW encoder block, mainly for debugging and testing purposes.
     * It accepts a CW word via a message source port and transmits the
     * corresponding CW symbols.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API cw_encoder : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<cw_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of satnogs::cw_encoder.
       * @param samp_rate the sampling rate
       * @param cw_freq the CW tone frequency
       * @param wpm words per minute (WPM)
       */
      static sptr
      make (double samp_rate, double cw_freq = 700, size_t wpm = 20);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_ENCODER_H */

