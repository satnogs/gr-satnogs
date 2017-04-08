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

#include <limits>
#include <gnuradio/io_signature.h>
#include <satnogs/log.h>
#include <satnogs/utils.h>
#include "cw_to_symbol_impl.h"

namespace gr
{
  namespace satnogs
  {

    cw_to_symbol::sptr
    cw_to_symbol::make (double sampling_rate, float threshold, float conf_level,
                        size_t wpm, bool auto_config)
    {
      return gnuradio::get_initial_sptr (
          new cw_to_symbol_impl (sampling_rate, threshold, conf_level, wpm,
                                 auto_config));
    }

    /*
     * The private constructor
     */
    cw_to_symbol_impl::cw_to_symbol_impl (double sampling_rate, float threshold,
                                          float conf_level, size_t wpm,
                                          bool auto_config) :
            gr::sync_block ("cw_to_symbol",
                            gr::io_signature::make (1, 1, sizeof(float)),
                            gr::io_signature::make (0, 0, 0)),
            d_sampling_rate (sampling_rate),
            d_act_thrshld (threshold),
            d_confidence_level (conf_level),
            d_sync_limit (15),
            d_auto_sync (auto_config),
            d_dot_samples ((1.2 / wpm) / (1.0 / sampling_rate)),
            d_dash_samples (3 * d_dot_samples),
            d_short_pause_samples (3 * d_dot_samples),
            d_long_pause_samples (7 * d_dot_samples),
            d_state (IDLE),
            d_state_cnt (0),
            d_pause_cnt (0),
            d_est_cnt (0),
            d_mean_cnt (0),
            d_have_sync (!auto_config),
            d_seq_started (false),
            d_sync_state (SYNC_TRIGGER_OFF)
    {
      message_port_register_in (pmt::mp ("act_threshold"));
      message_port_register_in (pmt::mp ("sync"));
      message_port_register_out (pmt::mp ("out"));

      /* Register the message handlers */
      set_msg_handler (
          pmt::mp ("act_threshold"),
          boost::bind (&cw_to_symbol_impl::set_act_threshold_msg_handler, this,
                       _1));
      set_msg_handler (
          pmt::mp ("sync"),
          boost::bind (&cw_to_symbol_impl::sync_msg_handler, this, _1));

      if (auto_config) {
        d_dot_samples = (1.2 / MIN_WPM) / (1.0 / d_sampling_rate);
      }
    }

    inline void
    cw_to_symbol_impl::send_symbol_msg (morse_symbol_t s)
    {
      message_port_pub (pmt::mp ("out"), pmt::from_long (s));
    }

    /*
     * Our virtual destructor.
     */
    cw_to_symbol_impl::~cw_to_symbol_impl ()
    {
    }

    inline void
    cw_to_symbol_impl::set_idle ()
    {
      d_state = IDLE;
      d_state_cnt = 0;
      d_pause_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_short_on ()
    {
      d_state = SHORT_ON_PERIOD;
      d_state_cnt = 1;
      d_pause_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_long_on ()
    {
      d_state = LONG_ON_PERIOD;
    }

    inline void
    cw_to_symbol_impl::set_short_off ()
    {
      d_state = SHORT_OFF_PERIOD;
      d_state_cnt = 0;
      d_pause_cnt = 1;
    }

    inline void
    cw_to_symbol_impl::set_long_off ()
    {
      d_state = LONG_OFF_PERIOD;
    }

    void
    cw_to_symbol_impl::set_act_threshold_msg_handler (pmt::pmt_t msg)
    {
      if (pmt::is_pair (msg)) {
        set_act_threshold (pmt::to_double (pmt::cdr (msg)));
      }
    }

    void
    cw_to_symbol_impl::sync_msg_handler (pmt::pmt_t msg)
    {
      if (pmt::is_pair (msg)) {
        if (pmt::to_bool (pmt::cdr (msg))) {
          start_timing_recovery ();
        }
      }
    }

    int
    cw_to_symbol_impl::work (int noutput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items)
    {
      int i;
      float conf_lvl;
      const float *in = (const float *) input_items[0];

      /* The estimation for the WPM has not yet been computed */
      if (!d_have_sync) {
        for (i = 0; i < noutput_items; i++) {
          switch (d_sync_state)
            {
            case SYNC_TRIGGER_OFF:
              if (in[i] > d_act_thrshld) {
                d_sync_state = SYNC_TRIGGER_ON;
              }
              break;
            case SYNC_TRIGGER_ON:
              if (in[i] > d_act_thrshld) {
                d_state_cnt++;
              }
              else {
                /* Trigger is OFF. Extract the best timing information available */
                d_sync_state = SYNC_TRIGGER_OFF;
                estimate_dot_duration (d_state_cnt);
                d_state_cnt = 0;

                /*
                 * If the sync process was over set the state to idle and
                 * return for proper decoding
                 */
                if (d_have_sync) {
                  set_idle ();
                  return i + 1;
                }
              }
              break;
            }
        }

        return noutput_items;
      }

      for (i = 0; i < noutput_items; i++) {
        switch (d_state)
          {
          case IDLE:
            if (in[i] > d_act_thrshld) {
              set_short_on ();
            }
            break;

          case SHORT_ON_PERIOD:
            if (in[i] > d_act_thrshld) {
              d_state_cnt++;

              if (d_state_cnt > d_dot_samples) {
                set_long_on ();
              }
            }
            else {
              /*
               * Before going to short pause, check the confidence level.
               *  Perhaps a short symbol should be produced.
               *
               *  Otherwise, it was a false alarm.
               */
              conf_lvl = ((float) d_state_cnt) / d_dot_samples;
              if (conf_lvl > d_confidence_level) {
                LOG_DEBUG("DOT");
                send_symbol_msg (MORSE_DOT);
              }

              /* Go find a possible short pause symbol */
              set_short_off ();
            }
            break;

          case LONG_ON_PERIOD:
            if (in[i] > d_act_thrshld) {
              d_state_cnt++;
            }
            else {
              conf_lvl = ((float) d_state_cnt) / d_dash_samples;
              if (conf_lvl > d_confidence_level) {
                LOG_DEBUG("DASH");
                send_symbol_msg (MORSE_DASH);
                set_short_off ();
                break;
              }

              /* Perhaps this was a short on symbol */
              conf_lvl = ((float) d_state_cnt) / d_dot_samples;
              if (conf_lvl > d_confidence_level) {
                LOG_DEBUG("DOT");
                send_symbol_msg (MORSE_DOT);
                set_short_off ();
              }

            }
            break;

          case SHORT_OFF_PERIOD:
            if (in[i] > d_act_thrshld) {
              /*
               * Before going to ON state, again check if a short pause symbol
               * should be produced
               */
              conf_lvl = ((float) d_pause_cnt) / d_short_pause_samples;
              if (conf_lvl > d_confidence_level) {
                LOG_DEBUG("Short space");
                send_symbol_msg (MORSE_S_SPACE);
              }
              set_short_on ();
            }
            else {
              d_pause_cnt++;
              if (d_pause_cnt > d_short_pause_samples) {
                set_long_off ();
              }
            }
            break;

          case LONG_OFF_PERIOD:
            if (in[i] > d_act_thrshld) {
              conf_lvl = ((float) d_pause_cnt) / d_long_pause_samples;
              if (conf_lvl > d_confidence_level) {
                LOG_DEBUG("Long space");
                send_symbol_msg (MORSE_L_SPACE);
                set_idle ();
                break;
              }
              else {
                LOG_DEBUG("Short space");
                send_symbol_msg (MORSE_S_SPACE);
              }
              set_short_on ();
            }
            else {
              d_pause_cnt++;
              /*
               * If the pause duration is greater than the long pause symbol,
               * definitely a long pause symbol should be produced
               */
              if (d_pause_cnt > d_long_pause_samples) {
                LOG_DEBUG("Long space");
                send_symbol_msg (MORSE_L_SPACE);
                d_seq_started = false;
                set_idle ();
              }
            }
            break;
          default:
            LOG_ERROR("Invalid State.");
            d_state = IDLE;
          }
      }
      return noutput_items;
    }

    void
    cw_to_symbol_impl::estimate_dot_duration (size_t estimate)
    {
      /*
       * The estimations should be in a logical range of
       * [MIN_WPM - MAX_WPM] WPM. Otherwise it is considered a false alarm.
       */
      if (estimate < (1.2 / MAX_WPM) / (1.0 / d_sampling_rate)
          || estimate >= (1.2 / MIN_WPM) / (1.0 / d_sampling_rate)) {
        return;
      }

      /*
       * Keep the minimum dot duration. This is only for the sync process.
       * At the end the resulting dot duration will be a mean value of
       * possible dot durations
       */
      if (estimate < d_dot_samples) {
        d_dot_samples = estimate;
        d_mean_cnt += estimate;
        d_est_cnt++;
      }
      else if (mape (d_dot_samples, estimate) < 1 - d_confidence_level) {
        d_mean_cnt += estimate;
        d_est_cnt++;
      }
      /*
       * Perhaps the synchronization process was triggered by a dash.
       * Check this possibility and use MAPE and confidence level to
       * decide or not to take into consideration the new estimation
       */
      else if (mape (d_dot_samples, estimate / 3.0) < 1 - d_confidence_level) {
        d_mean_cnt += estimate / 3.0;
        d_est_cnt++;
      }

      /*
       * If the synchronization process is over update the dot duration
       * with an average estimation
       */
      if (d_est_cnt == d_sync_limit) {
        d_have_sync = true;
        d_dot_samples = d_mean_cnt / d_sync_limit;
        set_symbols_duration ();
        std::cout << "Have sync" << d_dot_samples << std::endl;
      }
    }

    /**
     * Sets the dash, short and long pause durations based on the dot estimated
     * duration.
     */
    void
    cw_to_symbol_impl::set_symbols_duration ()
    {
      d_dash_samples = 3 * d_dot_samples;
      d_short_pause_samples = d_dash_samples;
      d_long_pause_samples = 7 * d_dot_samples;
    }

    /**
     * Resets the estimated dot  duration. After the call of this method
     * the caller MUST initiate a timing recovery procedure.
     */
    void
    cw_to_symbol_impl::reset_sync ()
    {
      d_sync_state = SYNC_TRIGGER_OFF;
      d_est_cnt = 0;
      set_idle ();
      d_have_sync = false;
      d_dot_samples = (1.2 / MIN_WPM) / (1.0 / d_sampling_rate);
    }

    /**
     * Sets a new activation threshold. If auto timing recovery is enabled,
     * the process of finding automatically the WPM is initiated after the
     * configuration of the new threshold.
     * @param thrhld the new threshold.
     */
    void
    cw_to_symbol_impl::set_act_threshold (float thrhld)
    {
      d_act_thrshld = thrhld;
      /* When a new activation threshold is set and automatic synchronization
       * is set, the automatic timing recovery should be again performed
       */
      if (d_auto_sync) {
        reset_sync ();
      }
    }

    /**
     * Starts the timing recovery process fro automatically retrieving the best
     * WPM estimation.
     */
    void
    cw_to_symbol_impl::start_timing_recovery ()
    {
      reset_sync ();
    }

  } /* namespace satnogs */
} /* namespace gr */

