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

#ifndef INCLUDED_SATNOGS_SINE_MATCHED_FILTER_FF_H
#define INCLUDED_SATNOGS_SINE_MATCHED_FILTER_FF_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Matched filter for signals that use sinusoidal transmissions,
     * like FSK, AFSK, e.t.c.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API sine_matched_filter_ff : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<sine_matched_filter_ff> sptr;

      /*!
       * A matched filter that produces the filtered signal or its energy
       * @param sampling_rate the sampling rate
       * @param sine_freq the frequency of the sine wave
       * @param baudrate the Baud-rate (aka symbols-per-second) of the
       * telecommunication protocol
       * @param compute_energy id set to true the filter computes the energy
       * of the filtered signal. Otherwise, the filtered signal itself is
       * produced
       */
      static sptr
      make (double sampling_rate, double sine_freq, double baudrate,
	    bool compute_energy = false);

      virtual void
      set_new_freq (double freq) = 0;

      virtual void
      set_new_freq_locked (double freq) = 0;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_SINE_MATCHED_FILTER_FF_H */

