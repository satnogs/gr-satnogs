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

#ifndef INCLUDED_SATNOGS_NOAA_APT_SINK_IMPL_H
#define INCLUDED_SATNOGS_NOAA_APT_SINK_IMPL_H

#include <satnogs/noaa_apt_sink.h>
#define PNG_DEBUG 3
#include <png.h>
#include <chrono>

namespace gr
{
  namespace satnogs
  {

    class noaa_apt_sink_impl : public noaa_apt_sink
    {
    private:
      uint32_t d_sync_word;
      uint32_t d_constructed_word;
      float d_slicer_threshold;
      bool d_sync_found;
      float d_max_value;
      float d_min_value;
      size_t d_norm_window;
      size_t d_sample_counter;
      const char* d_filename_png;
      size_t d_width;
      size_t d_height;
      bool d_split;
      size_t d_history_length;
      bool d_synchronize_opt;
      png_structp* d_png_ptr;
      png_infop* d_info_ptr;
      uint8_t* d_row_buffer;
      png_byte d_color_type;
      png_byte d_bit_depth;
      FILE** d_png_fd;
      size_t d_images_per_frame;
      size_t d_row_counter;
      size_t d_num_images;
      size_t d_current_buffered_samples;
      std::vector<std::string> d_png_fn;
      bool d_flip;

    public:
      noaa_apt_sink_impl (const char *filename_png, size_t width, size_t height,
                          bool split, bool sync, bool flip);
      ~noaa_apt_sink_impl ();
      void
      write_png_row ();
      void
      init_png ();
      void
      flip_image ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_NOAA_APT_SINK_IMPL_H */

