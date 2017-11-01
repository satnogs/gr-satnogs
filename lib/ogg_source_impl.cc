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
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <volk/volk.h>

#include <satnogs/log.h>
#include "ogg_source_impl.h"

#define PCM_BUF_SIZE 4096

namespace gr {
  namespace satnogs {

    ogg_source::sptr
    ogg_source::make (const std::string& filename, size_t channels, bool repeat)
    {
      return gnuradio::get_initial_sptr (
          new ogg_source_impl (filename, channels, repeat));
    }

    /*
     * The private constructor
     */
    ogg_source_impl::ogg_source_impl (const std::string& filename,
                                      size_t channels, bool repeat) :
            gr::sync_block (
                "ogg_source", gr::io_signature::make (0, 0, 0),
                gr::io_signature::make (channels, channels, sizeof(float))),
            d_channels (channels),
            d_repeat (repeat)
    {
      if (channels < 1) {
        throw std::invalid_argument ("At least one output channels should"
                                     " be specified");
      }

      if (ov_fopen (filename.c_str (), &d_ogvorb_f) < 0) {
        throw std::invalid_argument ("Invalid .ogg file");
      }

      vorbis_info *vi = ov_info (&d_ogvorb_f, -1);
      if (vi->channels != (int) channels) {
        throw std::invalid_argument (
            std::string ("Channels number specified (")
                + std::to_string (channels)
                + ") does not match the channels of "
                    "the ogg stream (" + std::to_string (vi->channels) + ")");
      }

      const int alignment_multiple = volk_get_alignment () / sizeof(float);
      set_alignment (std::max (1, alignment_multiple));
      set_max_noutput_items (PCM_BUF_SIZE);

      d_in_buffer = (int16_t *) volk_malloc (PCM_BUF_SIZE * sizeof(int16_t),
                                             volk_get_alignment ());
      d_out_buffer = (float *) volk_malloc (PCM_BUF_SIZE * sizeof(float),
                                            volk_get_alignment ());
      if(!d_in_buffer || !d_out_buffer) {
        throw std::runtime_error("Could not allocate memory");
      }
    }

    /*
     * Our virtual destructor.
     */
    ogg_source_impl::~ogg_source_impl()
    {
      ov_clear(&d_ogvorb_f);
      volk_free(d_in_buffer);
      volk_free(d_out_buffer);
    }

    int
    ogg_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      long int ret;
      int section = 0;
      int available = (noutput_items / d_channels);
      int produced = 0;

      ret = ov_read (&d_ogvorb_f, (char *)d_in_buffer,
                     available * sizeof(int16_t),
                     0, sizeof(int16_t), 1, &section);
      if(ret < sizeof(int16_t)) {
        /*
         * If return value is EOF and the repeat mode is set seek back to the
         * start of the ogg stream
         */
        if(ret == 0 && d_repeat) {
          if(ov_seekable(&d_ogvorb_f)){
            ov_time_seek(&d_ogvorb_f, 0);
            return 0;
          }
          LOG_WARN("File is not seakable.");
        }
        return WORK_DONE;
      }

      /* Convert to float the signed-short audio samples */
      volk_16i_s32f_convert_32f (d_out_buffer, d_in_buffer, 2 << 15,
                                 ret / sizeof(int16_t));

      /* De-interleave the available channels */
      for(int i = 0; i < ret / sizeof(int16_t); i += d_channels, produced++) {
        for(int chan = 0; chan < d_channels; chan++){
          ((float *)output_items[chan])[produced] = d_out_buffer[i * d_channels + chan];
        }
      }

      return produced;
    }

  } /* namespace satnogs */
} /* namespace gr */

