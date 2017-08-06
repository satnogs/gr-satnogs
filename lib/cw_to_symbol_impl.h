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

      typedef enum
      {
        NO_SYNC, SEARCH_DOT, SEARCH_DASH, SEARCH_SPACE
      } cw_dec_state_t;

    private:
      const double d_sampling_rate;
      float d_act_thrshld;
      const float d_confidence_level;
      size_t d_dot_samples;
      size_t d_window_size;
      size_t d_window_cnt;
      size_t d_dot_windows_num;
      size_t d_dash_windows_num;
      size_t d_short_pause_windows_num;
      size_t d_long_pause_windows_num;
      cw_dec_state_t d_dec_state;
      bool d_prev_space_symbol;
      float *d_const_val;
      float *d_tmp;
      int32_t *d_out;

      inline void
      set_idle ();

      inline void
      set_short_on ();

      inline void
      set_long_on ();

      inline void
      set_search_space ();

      inline int32_t
      hadd (const int32_t *in, size_t len);

      inline void
      clamp_input (int32_t *out, const float *in, size_t len);

      inline bool
      is_triggered (const float *in, size_t len);

      inline void
      send_symbol_msg (morse_symbol_t s);

      inline bool
      check_conf_level(size_t cnt, size_t target);

      void
      set_act_threshold_msg_handler (pmt::pmt_t msg);

    public:
      cw_to_symbol_impl (double sampling_rate, float threshold,
                         float conf_level, size_t wpm);
      ~cw_to_symbol_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

      void
      set_act_threshold (float thrhld);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CW_TO_SYMBOL_IMPL_H */

