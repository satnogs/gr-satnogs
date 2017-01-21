/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_WATERFALL_SINK_IMPL_H
#define INCLUDED_SATNOGS_WATERFALL_SINK_IMPL_H

#include <satnogs/waterfall_sink.h>
#include <volk/volk.h>
#include <gnuradio/fft/fft.h>
#include <iostream>
#include <fstream>

namespace gr
{
  namespace satnogs
  {

    class waterfall_sink_impl : public waterfall_sink
    {
    private:
      /**
       * The different types of operation of the waterfall
       */
      typedef enum {
        WATERFALL_MODE_DECIMATION = 0,//!< WATERFALL_MODE_DECIMATION Performs just a decimation and computes the energy only
        WATERFALL_MODE_MAX_HOLD = 1,  //!< WATERFALL_MODE_MAX_HOLD compute the max hold energy of all the FFT snapshots between two consecutive pixel rows
        WATERFALL_MODE_MEAN = 2       //!< WATERFALL_MODE_MEAN compute the mean energy of all the FFT snapshots between two consecutive pixel rows
      } wf_mode_t;

      const double d_samp_rate;
      double d_pps;
      const size_t d_fft_size;
      wf_mode_t d_mode;
      size_t d_refresh;
      size_t d_fft_cnt;
      size_t d_fft_shift;
      size_t d_samples_cnt;
      fft::fft_complex d_fft;
      gr_complex *d_shift_buffer;
      float *d_hold_buffer;
      std::ofstream d_fos;

    public:
      waterfall_sink_impl (double samp_rate, double center_freq,
                           double pps, size_t fft_size,
                           const std::string& filename, int mode);
      ~waterfall_sink_impl ();


      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

      void
      compute_decimation(const gr_complex *in, size_t n_fft);

      void
      compute_max_hold(const gr_complex *in, size_t n_fft);

      void
      compute_mean(const gr_complex *in, size_t n_fft);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_WATERFALL_SINK_IMPL_H */

