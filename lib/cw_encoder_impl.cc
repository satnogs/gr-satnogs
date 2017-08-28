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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <satnogs/log.h>
#include "cw_encoder_impl.h"

namespace gr {
  namespace satnogs {

    cw_encoder::sptr
    cw_encoder::make(double samp_rate, double cw_freq, size_t wpm)
    {
      return gnuradio::get_initial_sptr
        (new cw_encoder_impl(samp_rate, cw_freq, wpm));
    }

    /*
     * The private constructor
     */
    cw_encoder_impl::cw_encoder_impl(double samp_rate, double cw_freq,
                                     size_t wpm)
      : gr::sync_block("cw_encoder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_samp_rate (samp_rate),
              d_cw_freq (cw_freq),
              d_wpm (wpm),
              d_dot_samples ((1.2 / wpm) / (1.0 / samp_rate)),
              d_window_size (0),
              d_windows_remaining (0),
              d_cw_symbol (MORSE_L_SPACE),
              d_nco ()
    {
      message_port_register_in(pmt::mp("symbol"));

      /*
       * Try to split the CW pulses in smaller windows for dealing efficiently
       * with the available buffer size
       */
      size_t i = 10;
      d_window_size = d_dot_samples / i;
      while(d_window_size > 200) {
        i += 10;
        d_window_size = d_dot_samples / i;
      }

      /* NOTE: The dot duration should be a perfect multiple of the window */
      while(d_dot_samples % d_window_size != 0) {
        d_window_size++;
      }

      set_output_multiple(d_window_size);
      d_nco.set_freq ((2 * M_PI * cw_freq) / samp_rate);
    }

    /*
     * Our virtual destructor.
     */
    cw_encoder_impl::~cw_encoder_impl()
    {
    }

    int
    cw_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      size_t available;
      size_t i;
      gr_complex *out = (gr_complex *) output_items[0];

      if(noutput_items < 0) {
        return noutput_items;
      }

      if(d_windows_remaining == 0) {
        pmt::pmt_t symbol = delete_head_blocking(pmt::mp("symbol"));
        d_cw_symbol = (morse_symbol_t) pmt::to_long(symbol);
        /* Reset NCO so the amplitude starts from zero */
        d_nco.set_freq ((2 * M_PI * d_cw_freq) / d_samp_rate);
        switch(d_cw_symbol) {
          case MORSE_DOT:
          case MORSE_INTRA_SPACE:
            d_windows_remaining = d_dot_samples / d_window_size;
            break;
          case MORSE_DASH:
          case MORSE_S_SPACE:
            d_windows_remaining = (d_dot_samples / d_window_size) * 3;
            break;
          case MORSE_L_SPACE:
            d_windows_remaining = (d_dot_samples / d_window_size) * 7;
            break;
          default:
            LOG_WARN("Unrecognized CW symbol");
            return 0;
        }
      }

      for(i = 0; i < (size_t)noutput_items / d_window_size; i++) {
        switch(d_cw_symbol){
          case MORSE_S_SPACE:
          case MORSE_L_SPACE:
          case MORSE_INTRA_SPACE:
            memset (out + i * d_window_size, 0,
                    d_window_size * sizeof(gr_complex));
            break;
          case MORSE_DOT:
          case MORSE_DASH:
            d_nco.sincos(out + i * d_window_size, d_window_size, 1.0);
            break;
        }
        d_windows_remaining--;
        if(d_windows_remaining == 0) {
          return (i + 1) * d_window_size;
        }
      }
      return i * d_window_size;
    }

  } /* namespace satnogs */
} /* namespace gr */

