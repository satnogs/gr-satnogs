/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <http://librespacefoundation.org/>
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
#include "quad_demod_filter_ff_impl.h"

namespace gr {
  namespace satnogs {

    quad_demod_filter_ff::sptr
    quad_demod_filter_ff::make(float gain, int window)
    {
      return gnuradio::get_initial_sptr
        (new quad_demod_filter_ff_impl(gain, window));
    }

    /*
     * The private constructor
     */
    quad_demod_filter_ff_impl::quad_demod_filter_ff_impl (float gain,
                                                          int window) :
            gr::block ("quad_demod_filter_ff",
                       gr::io_signature::make (1, 1, sizeof(float)),
                       gr::io_signature::make (1, 1, sizeof(float))),
            d_gain (gain),
            d_norm (1.0f / window),
            d_win (window),
            d_sum (0.0f),
            d_sum_sq (0.0f),
            d_remaining (0)
    {
      if(window < 1) {
        throw std::invalid_argument ("Window must be a positive");
      }
      set_history (2 * window);
    }

    /*
     * Our virtual destructor.
     */
    quad_demod_filter_ff_impl::~quad_demod_filter_ff_impl()
    {
    }

    /**
     * Fast approximation of the inverse of the square root.
     * Thank you OpenArena!
     * @param number the number to be computed
     * @return the inverse of the square root of the number
     */
    static inline float
    inv_sqrt (float number)
    {
      long i;
      float x2, y;
      const float threehalfs = 1.5f;

      x2 = number * 0.5f;
      y = number;
      i = *(long *) &y;
      i = 0x5f3759df - (i >> 1);               /* what the fuck? */
      y = *(float *) &i;
      y = y * (threehalfs - (x2 * y * y));
      return y;
    }

    int
    quad_demod_filter_ff_impl::general_work (
        int noutput_items, gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];
      int available = std::min(ninput_items[0], noutput_items);
      int produced = 0;
      float m;
      float m_sq;
      float snr;
      float in_old;
      float in_new;

      float diff;
      for(int i = 0; i < available; i++) {
        in_old = std::abs(in[i + d_win]);
        in_new = std::abs(in[i + 2*d_win - 1]);
        d_sum -= in_old;
        d_sum += in_new;
        d_sum_sq -= (in_old * in_old);
        d_sum_sq += (in_new * in_new);

        m = (d_sum * d_norm);
        m_sq = (d_sum_sq * d_norm);
        snr = m  * inv_sqrt(m_sq - m*m);

        /*
         * If the SNR is high enough start passing the data to the output.
         * In order not to loose any samples due to the settling period, start
         * from the buffered and let a window of samples to pass after the
         * trigger is off
         */
        if(snr > d_gain * 1.8) {
          d_remaining = 2*d_win;
        }

        if(d_remaining) {
          out[produced++] = in[i];
          d_remaining--;
        }
      }

      // Tell runtime system how many output items we produced.
      consume_each(available);
      return produced;
    }

  } /* namespace satnogs */
} /* namespace gr */

