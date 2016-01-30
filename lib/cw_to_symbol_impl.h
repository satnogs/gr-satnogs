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

#ifndef INCLUDED_SATNOGS_CW_TO_SYMBOL_IMPL_H
#define INCLUDED_SATNOGS_CW_TO_SYMBOL_IMPL_H

#include <satnogs/morse.h>
#include <satnogs/cw_to_symbol.h>

namespace gr
{
  namespace satnogs
  {

    class cw_to_symbol_impl : public cw_to_symbol
    {
      typedef enum {
	IDLE,
	SHORT_ON_PERIOD,
	LONG_ON_PERIOD,
	SHORT_OFF_PERIOD,
	LONG_OFF_PERIOD
      } cw_state_t;
    private:
      const double d_sampling_rate;
      const float d_act_thrshld;
      const float d_confidence_level;
      const size_t d_dot_samples;
      const size_t d_dash_samples;
      const size_t d_short_pause_samples;
      const size_t d_long_pause_samples;
      cw_state_t d_state;
      size_t d_state_cnt;
      size_t d_pause_cnt;
      bool d_seq_started;

      inline void
      set_idle ();

      inline void
      set_short_on ();

      inline void
      set_long_on ();

      inline void
      set_short_off ();

      inline void
      set_long_off ();

      inline void
      send_symbol_msg (morse_symbol_t s);

    public:
      cw_to_symbol_impl (double sampling_rate, float threshold,
			 float conf_level, size_t wpm);
      ~cw_to_symbol_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_TO_SYMBOL_IMPL_H */

