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
#include <png++/png.hpp>
#include <chrono>



namespace gr
{
  namespace satnogs
  {

    class noaa_apt_sink_impl : public noaa_apt_sink
    {
    private:
      std::string d_filename_png;
      size_t d_width;
      size_t d_height;
      bool d_split;
      bool d_synchronize_opt;
      bool d_flip;
      size_t d_history_length;

      size_t d_num_images;
      png::image<png::gray_pixel> d_full_image;
      png::image<png::gray_pixel> d_left_image;
      png::image<png::gray_pixel> d_right_image;
      std::string d_full_filename;
      std::string d_left_filename;
      std::string d_right_filename;

      size_t d_current_x;
      size_t d_current_y;

      float f_max_level;
      float f_min_level;

    public:
      noaa_apt_sink_impl (const char *filename_png, size_t width, size_t height,
                          bool split, bool sync, bool flip);
      ~noaa_apt_sink_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);

    private:
        void
        init_images ();

        void
        set_pixel (float sample);

        void
        end_line ();

        void
        write_images ();

        void
        write_image (png::image<png::gray_pixel> image, std::string filename);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_NOAA_APT_SINK_IMPL_H */
