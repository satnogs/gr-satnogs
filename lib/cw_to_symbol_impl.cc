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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits>
#include <gnuradio/io_signature.h>
#include <satnogs/log.h>
#include <satnogs/utils.h>
#include <boost/math/common_factor.hpp>
#include "cw_to_symbol_impl.h"
#include <volk/volk.h>

namespace gr
{
  namespace satnogs
  {

    cw_to_symbol::sptr
    cw_to_symbol::make (double sampling_rate, float threshold, float conf_level,
                        size_t wpm)
    {
      return gnuradio::get_initial_sptr (
          new cw_to_symbol_impl (sampling_rate, threshold, conf_level, wpm));
    }

    /*
     * The private constructor
     */
    cw_to_symbol_impl::cw_to_symbol_impl (double sampling_rate, float threshold,
                                          float conf_level, size_t wpm) :
            gr::sync_block ("cw_to_symbol",
                            gr::io_signature::make (1, 1, sizeof(float)),
                            gr::io_signature::make (0, 0, 0)),
            d_sampling_rate (sampling_rate),
            d_act_thrshld (threshold),
            d_confidence_level (conf_level),
            d_dot_samples ((1.2 / wpm) / (1.0 / sampling_rate)),
            d_window_size(0),
            d_window_cnt(0),
            d_dot_windows_num(0),
            d_dec_state (NO_SYNC),
            d_prev_space_symbol (true)
    {
      if (wpm < MIN_WPM) {
        throw std::invalid_argument (
            "Decoder can not handle such low WPM setting");
      }

      if (wpm > MAX_WPM) {
        throw std::invalid_argument (
            "Decoder can not handle such high WPM setting");
      }

      if (conf_level > 1.0 || conf_level < 0.5) {
        throw std::invalid_argument (
            "Confidence level should be in the range [0.5, 1.0]");
      }

      message_port_register_in (pmt::mp ("act_threshold"));
      message_port_register_out (pmt::mp ("out"));

      /* Register the message handlers */
      set_msg_handler (
          pmt::mp ("act_threshold"),
          boost::bind (&cw_to_symbol_impl::set_act_threshold_msg_handler, this,
                       _1));

      /*
       * Try to split the CW pulses in smaller windows for detecting faster
       * a false alarm. As we use the window size for setting the history
       * it should have a reasonably size.
       */
      size_t i = 1;
      d_window_size = d_dot_samples / i;
      while(d_window_size > 256) {
        i++;
        d_window_size = d_dot_samples / i;
      }

      /* NOTE: The dot duration should be a perfect multiple of the window */
      while(d_dot_samples % d_window_size != 0) {
        d_window_size++;
      }

      LOG_DEBUG("Dot samples: %lu", d_dot_samples);
      LOG_DEBUG("Window size: %lu", d_window_size);

      /* Set the duration of each symbol in multiples of the window size */
      d_dot_windows_num = d_dot_samples / d_window_size;
      d_dash_windows_num = 3 * d_dot_windows_num;
      d_short_pause_windows_num = d_dash_windows_num;
      d_long_pause_windows_num = 7 * d_dot_windows_num;

      const int alignment_multiple = volk_get_alignment ()
          / (d_window_size * sizeof(float));
      set_alignment (std::max (1, alignment_multiple));

      d_const_val = (float *) volk_malloc(d_window_size * sizeof(float),
                                          volk_get_alignment ());
      d_tmp = (float *) volk_malloc(d_window_size * sizeof(float),
                                    volk_get_alignment ());
      d_out = (int32_t *) volk_malloc (d_window_size * sizeof(int32_t),
                                       volk_get_alignment ());

      if(!d_const_val || !d_tmp || !d_out) {
        throw std::runtime_error("cw_to_symbol: Could not allocate memory");
      }

      for(i = 0; i < d_window_size; i++) {
        d_const_val[i] = threshold;
      }
      set_history(d_window_size);
    }

    inline void
    cw_to_symbol_impl::send_symbol_msg (morse_symbol_t s)
    {
      if(s == MORSE_S_SPACE || s == MORSE_L_SPACE) {
        d_prev_space_symbol = true;
      }
      else{
        d_prev_space_symbol = false;
      }
      message_port_pub (pmt::mp ("out"), pmt::from_long (s));
    }

    inline bool
    cw_to_symbol_impl::check_conf_level(size_t cnt, size_t target)
    {
      return ((float)cnt > target * d_confidence_level);
    }

    /*
     * Our virtual destructor.
     */
    cw_to_symbol_impl::~cw_to_symbol_impl ()
    {
      volk_free (d_const_val);
      volk_free (d_tmp);
      volk_free (d_out);
    }

    inline void
    cw_to_symbol_impl::set_idle ()
    {
      d_dec_state = NO_SYNC;
      d_window_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_short_on ()
    {
      d_dec_state = SEARCH_DOT;
      d_window_cnt = 1;
    }

    inline void
    cw_to_symbol_impl::set_long_on ()
    {
      d_dec_state = SEARCH_DASH;
    }

    inline void
    cw_to_symbol_impl::set_search_space ()
    {
      d_dec_state = SEARCH_SPACE;
      d_window_cnt = 1;
    }

    void
    cw_to_symbol_impl::set_act_threshold_msg_handler (pmt::pmt_t msg)
    {
      if (pmt::is_pair (msg)) {
        set_act_threshold (pmt::to_double (pmt::cdr (msg)));
      }
    }

    int
    cw_to_symbol_impl::work (int noutput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items)
    {
      bool triggered;
      int i;
      const float *in_old = (const float *) input_items[0];
      const float *in = in_old + history() - 1;

      /* During idle state search for a possible trigger */
      if(d_dec_state == NO_SYNC) {
        for(i = 0; i < noutput_items; i++) {
          /*
           * Clamp the input so the window mean is not affected by strong spikes
           * Good luck understanding this black magic shit!
           */
          triggered = is_triggered(in_old + i, d_window_size);
          if(triggered) {
            LOG_DEBUG("Triggered!");
            set_short_on();
            return i+1;
          }
        }
        return noutput_items;
      }

      /* From now one, we handle the input in multiples of a window */
      for (i = 0; i < noutput_items / d_window_size; i++) {
        triggered = is_triggered(in + i * d_window_size, d_window_size);
        switch(d_dec_state) {
          case SEARCH_DOT:
            if(triggered) {
              d_window_cnt++;
              if(d_window_cnt > d_dot_windows_num) {
                set_long_on();
              }
            }
            else {
              if(check_conf_level(d_window_cnt, d_dot_windows_num)) {
                LOG_DEBUG("DOT");
                send_symbol_msg(MORSE_DOT);
              }
              set_search_space ();
            }
            break;
          case SEARCH_DASH:
            if(triggered) {
              d_window_cnt++;
            }
            else{
              if(check_conf_level(d_window_cnt, d_dash_windows_num)) {
                LOG_DEBUG("DASH");
                send_symbol_msg(MORSE_DASH);
              }
              else{
                LOG_DEBUG("DOT");
                send_symbol_msg(MORSE_DOT);
              }
              set_search_space ();
            }
            break;
          case SEARCH_SPACE:
            if (triggered) {
              if(check_conf_level(d_window_cnt, d_long_pause_windows_num)) {
                LOG_DEBUG("LONG SPACE");
                send_symbol_msg(MORSE_L_SPACE);
              }
              else if(check_conf_level(d_window_cnt, d_short_pause_windows_num)){
                LOG_DEBUG("SHORT SPACE");
                send_symbol_msg(MORSE_S_SPACE);
              }
              set_short_on();
            }
            else{
              d_window_cnt++;
              if(d_window_cnt > d_long_pause_windows_num) {
                LOG_DEBUG("LONG SPACE");
                send_symbol_msg(MORSE_L_SPACE);
                set_idle();
                return (i + 1) * d_window_size;
              }
            }
            break;
          default:
            LOG_ERROR("Invalid decoder state");
        }
      }
      return i * d_window_size;
    }

    /**
     * Sets a new activation threshold.
     * @param thrhld the new threshold.
     */
    void
    cw_to_symbol_impl::set_act_threshold (float thrhld)
    {
      d_act_thrshld = thrhld;
    }

    /**
     * Clamps the input and performs at the same time binary slicing.
     * With this way, a decision based on moving average is not affected
     * by strong peaks.
     * @param out the output buffer with the binary sliced output
     * @param in the input signal
     * @param len number of samples to process
     */
    inline void
    cw_to_symbol_impl::clamp_input (int32_t* out, const float* in, size_t len)
    {
      volk_32f_x2_subtract_32f(d_tmp, in, d_const_val, len);
      volk_32f_binary_slicer_32i(d_out, d_tmp, len);
    }

    inline int32_t
    cw_to_symbol_impl::hadd (const int32_t* in, size_t len)
    {
      size_t i;
      int32_t cnt = 0;
      for(i = 0; i < len; i++) {
        cnt += in[i];
      }
      return cnt;
    }

    inline bool
    cw_to_symbol_impl::is_triggered (const float* in, size_t len)
    {
      int32_t cnt;
      clamp_input(d_out, in, len);
      cnt = hadd(d_out, len);
      return (cnt >= (int32_t)(d_window_size * d_confidence_level)) ? true : false;
    }

  } /* namespace satnogs */
} /* namespace gr */

