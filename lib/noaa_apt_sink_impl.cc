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
#include "noaa_apt_sink_impl.h"

#include <cmath>

namespace gr
{
  namespace satnogs
  {

    noaa_apt_sink::sptr
    noaa_apt_sink::make (const char *filename_png, size_t width, size_t height,
                         bool split, bool sync, bool flip)
    {
      return gnuradio::get_initial_sptr (
          new noaa_apt_sink_impl (filename_png, width, height, split, sync,
                                  flip));
    }

    /*
     * The private constructor
     */
    noaa_apt_sink_impl::noaa_apt_sink_impl (const char *filename_png,
                                            size_t width, size_t height,
                                            bool split, bool sync, bool flip) :
            gr::sync_block ("noaa_apt_sink",
                            gr::io_signature::make (1, 1, sizeof(float)),
                            gr::io_signature::make (0, 0, 0)),
            d_filename_png (filename_png),
            d_width (width),
            d_height (height),
            d_split (split),
            d_synchronize_opt (sync),
            d_flip (flip),
            d_history_length (40),
            d_current_x (0),
            d_current_y (0),
            d_num_images (0),
            f_max_level(0.0),
            f_min_level(1.0)
    {
      set_history(d_history_length);
      init_images();
    }

    void
    noaa_apt_sink_impl::init_images () {
        size_t len = d_filename_png.size();
        size_t pos = d_filename_png.rfind('.');
        std::string base_filename = d_filename_png.substr(0, pos);
        std::string extension = d_filename_png.substr(pos+1, len-1);

        d_full_filename = base_filename + std::to_string(d_num_images) + "." + extension;
        d_full_image = png::image<png::gray_pixel>(d_width, d_height);

        if(d_split) {
            d_left_filename = base_filename + "_left" + std::to_string(d_num_images) + "." + extension;
            d_right_filename = base_filename + "_right" + std::to_string(d_num_images)+ "." + extension;

            d_left_image = png::image<png::gray_pixel>(d_width/2, d_height);
            d_right_image = png::image<png::gray_pixel>(d_width/2, d_height);
        }
    }

    void
    noaa_apt_sink_impl::write_image (png::image<png::gray_pixel> image, std::string filename) {
        if(d_flip) {
            size_t width = image.get_width();
            size_t height = image.get_height();

            png::image<png::gray_pixel> flipped(width, height);

            for(size_t y = 0; y < height; y++) {
                for(size_t x = 0; x < width; x++) {
                    auto pixel = image.get_pixel(x, height - y - 1);
                    flipped.set_pixel(x, y, pixel);
                }
            }
            flipped.write(filename);
        }
        else {
            image.write(filename);
        }
    }

    void
    noaa_apt_sink_impl::write_images () {
        write_image(d_full_image, d_full_filename);

        if(d_split) {
            write_image(d_left_image, d_left_filename);
            write_image(d_right_image, d_right_filename);
        }
    }

    noaa_apt_sink_impl::~noaa_apt_sink_impl () {
        write_images();
    }

    void noaa_apt_sink_impl::end_line () {
        d_current_y += 1;
        d_current_x = 0;

        if(d_current_y % 100 == 0) {
            write_images();
        }

        if(d_current_y >= d_height) {
            d_current_y = 0;
            d_num_images += 1;
            write_images();
            init_images();
        }
    }

    void noaa_apt_sink_impl::set_pixel (float sample) {
        sample = (sample - f_min_level) / (f_max_level - f_min_level) * 255;
        d_full_image.set_pixel(d_current_x, d_current_y, sample);

        if(d_split) {
            if(d_current_x < d_width / 2) {
                d_left_image.set_pixel(d_current_x, d_current_y, sample);
            }
            else {
                d_right_image.set_pixel(d_current_x - d_width / 2, d_current_y, sample);
            }
        }
    }

    int
    noaa_apt_sink_impl::work (int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];

        for (size_t i = d_history_length - 1; i < noutput_items + d_history_length - 1; i++) {
            float sample = in[i];

            f_min_level = std::fmin(f_min_level, sample);
            f_max_level = std::fmax(f_max_level, sample);

            set_pixel(sample);

            d_current_x += 1;
            if(d_current_x >= d_width) {
                end_line();
            }
        }


        return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */
