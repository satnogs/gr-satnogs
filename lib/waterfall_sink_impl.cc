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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "waterfall_sink_impl.h"
#include <satnogs/log.h>
namespace gr
{
  namespace satnogs
  {

    waterfall_sink::sptr
    waterfall_sink::make (double samp_rate, double center_freq,
                          double fps, size_t fft_size,
                          const std::string& filename, int mode)
    {
      return gnuradio::get_initial_sptr (
          new waterfall_sink_impl (samp_rate, center_freq,
                                   fps, fft_size, filename, mode));
    }

    /*
     * The private constructor
     */
    waterfall_sink_impl::waterfall_sink_impl (double samp_rate,
                                              double center_freq,
                                              double pps,
                                              size_t fft_size,
                                              const std::string& filename,
                                              int mode) :
            gr::sync_block ("waterfall_sink",
                            gr::io_signature::make (1, 1, sizeof(gr_complex)),
                            gr::io_signature::make (0, 0, 0)),
            d_samp_rate (samp_rate),
            d_pps (pps),
            d_fft_size (fft_size),
            d_mode ((wf_mode_t)mode),
            d_refresh( (d_samp_rate / fft_size) / pps),
            d_fft_cnt(0),
            d_fft_shift((size_t)(ceil(fft_size/2.0))),
            d_samples_cnt(0),
            d_fft (fft_size)
    {
      float r = 0.0;
      const int alignment_multiple = volk_get_alignment ()
          / (fft_size * sizeof(gr_complex));
      set_alignment (std::max (1, alignment_multiple));
      set_output_multiple (fft_size);

      d_shift_buffer = (gr_complex *) volk_malloc (
          fft_size * sizeof(gr_complex), volk_get_alignment());
      if(!d_shift_buffer){
        LOG_ERROR("Could not allocate aligned memory");
        throw std::runtime_error("Could not allocate aligned memory");
      }

      d_hold_buffer = (float *)volk_malloc(fft_size * sizeof(float),
                                           volk_get_alignment());
      if(!d_hold_buffer){
        LOG_ERROR("Could not allocate aligned memory");
        throw std::runtime_error("Could not allocate aligned memory");
      }

      d_fos.open(filename, std::ios::binary | std::ios::trunc);

      /* Append header for proper plotting  */
      r = fft_size;
      d_fos.write((char *)&r, sizeof(float));
      for(size_t i = 0; i < fft_size; i++) {
        r = (samp_rate/fft_size * i ) - samp_rate/2.0 + center_freq;
        d_fos.write((char *)&r, sizeof(float));
      }
    }

    /*
     * Our virtual destructor.
     */
    waterfall_sink_impl::~waterfall_sink_impl ()
    {
      d_fos.close();
      volk_free(d_shift_buffer);
      volk_free(d_hold_buffer);
    }

    int
    waterfall_sink_impl::work (int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      size_t n_fft = ((size_t) noutput_items / d_fft_size);

      switch (d_mode)
        {
        case WATERFALL_MODE_DECIMATION:
          compute_decimation (in, n_fft);
          break;
        case WATERFALL_MODE_MAX_HOLD:
          compute_max_hold (in, n_fft);
          break;
        case WATERFALL_MODE_MEAN:
          compute_mean (in, n_fft);
          break;
        default:
          LOG_ERROR("Wrong waterfall mode");
          throw std::runtime_error ("Wrong waterfall mode");
          return -1;
        }

      return n_fft * d_fft_size;
    }

    void
    waterfall_sink_impl::compute_decimation (const gr_complex* in, size_t n_fft)
    {
      size_t i;
      float t;
      gr_complex *fft_in;
      for(i = 0; i < n_fft; i++){
        d_fft_cnt++;
        if(d_fft_cnt > d_refresh){
          fft_in = d_fft.get_inbuf();
          memcpy(fft_in, in + i*d_fft_size, d_fft_size*sizeof(gr_complex));
          d_fft.execute();
          /* Perform FFT shift */
          memcpy (d_shift_buffer, &d_fft.get_outbuf ()[d_fft_shift],
                  sizeof(gr_complex) * (d_fft_size - d_fft_shift));
          memcpy (&d_shift_buffer[d_fft_size - d_fft_shift],
                  &d_fft.get_outbuf ()[0], sizeof(gr_complex) * d_fft_shift);

          /* Compute the energy in dB */
          volk_32fc_s32f_x2_power_spectral_density_32f (d_hold_buffer,
                                                        d_shift_buffer,
                                                        (float) d_fft_size, 1.0,
                                                        d_fft_size);
          /* Write the result to the file */
          t = (float)(d_samples_cnt / d_samp_rate);
          d_fos.write((char *) &t, sizeof(float));
          d_fos.write((char *) d_hold_buffer, d_fft_size * sizeof(float));
          d_fft_cnt = 0;
        }
        d_samples_cnt += d_fft_size;
      }
    }

    void
    waterfall_sink_impl::compute_max_hold (const gr_complex* in, size_t n_fft)
    {
    }

    void
    waterfall_sink_impl::compute_mean (const gr_complex* in, size_t n_fft)
    {
    }

  } /* namespace satnogs */
} /* namespace gr */

