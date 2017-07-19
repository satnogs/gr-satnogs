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
            d_sync_word (0x0ccccccc),
            d_constructed_word (0),
            d_slicer_threshold (0.5),
            d_sync_found (false),
            d_norm_window (2048),
            d_sample_counter (0),
            d_filename_png (filename_png),
            d_width (width),
            d_height (height),
            d_split (split),
            d_history_length (28),
            d_synchronize_opt (sync),
            d_row_counter (0),
            d_num_images (0),
            d_current_buffered_samples (0),
            d_flip (flip)

    {
      set_history (d_history_length);
      if (sync) {
        d_max_value = -1;
        d_min_value = 100;
      }
      else {
        d_max_value = 1;
        d_min_value = 0;
      }
      d_color_type = PNG_COLOR_TYPE_GRAY;
      d_bit_depth = 8;
      d_row_buffer = new uint8_t[d_width];
      if (d_split) {
        d_png_fn.push_back (std::string (""));
        d_png_fn.push_back (std::string (""));
      }
      else {
        d_png_fn.push_back (std::string (""));
      }
      init_png ();
    }
    void
    noaa_apt_sink_impl::init_png ()
    {
      if (d_split) {
        d_images_per_frame = 2;
        d_png_fd = new FILE*[2];
        d_png_ptr = new png_structp[2];
        d_info_ptr = new png_infop[2];
        std::string fn (d_filename_png);
        std::string fn_left (d_filename_png);
        std::string fn_right (d_filename_png);
        std::size_t found = fn.find_last_of (".");
        if (d_num_images == 0) {
          if (found == std::string::npos) {
            fn_left.append ("_left");
            fn_right.append ("_right");
          }
          else {
            std::size_t found_dir = fn.substr (found).find ("/");
            if (found_dir == std::string::npos) {
              fn_left.insert (found, "_left");
              fn_right.insert (found, "_right");
            }
            else {
              fn_left.append ("_left");
              fn_right.append ("_right");
            }
          }
        }
        else {
          if (found == std::string::npos) {
            fn_left.append (std::to_string (d_num_images).append ("_left"));
            fn_right.append (std::to_string (d_num_images).append ("_right"));
          }
          else {
            std::size_t found_dir = fn.substr (found).find ("/");
            if (found_dir == std::string::npos) {
              fn_left.insert (found,
                              std::to_string (d_num_images).append ("_left"));
              fn_right.insert (found,
                               std::to_string (d_num_images).append ("_right"));
            }
            else {
              fn_left.append (std::to_string (d_num_images).append ("_left"));
              fn_right.append (std::to_string (d_num_images).append ("_right"));
            }
          }
        }
        d_png_fn[0] = fn_left;
        d_png_fn[1] = fn_right;
        d_png_fd[0] = fopen (fn_right.c_str (), "wb");
        d_png_fd[1] = fopen (fn_left.c_str (), "wb");
        for (size_t i = 0; i < d_images_per_frame; i++) {
          d_png_ptr[i] = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
          NULL,
                                                  NULL);
          d_info_ptr[i] = png_create_info_struct (d_png_ptr[i]);
          png_init_io (d_png_ptr[i], d_png_fd[i]);
          png_set_IHDR (d_png_ptr[i], d_info_ptr[i], d_width / 2, d_height,
                        d_bit_depth, d_color_type,
                        PNG_INTERLACE_NONE,
                        PNG_COMPRESSION_TYPE_BASE,
                        PNG_FILTER_TYPE_BASE);

          png_write_info (d_png_ptr[i], d_info_ptr[i]);
        }
      }
      else {
        d_images_per_frame = 1;
        d_png_fd = new FILE*[1];
        d_png_ptr = new png_structp[2];
        d_info_ptr = new png_infop[2];
        if (d_num_images == 0) {
          d_png_fd[0] = fopen (d_filename_png, "wb");
          d_png_fn[0] = std::string (d_filename_png);
        }
        else {
          std::string fn (d_filename_png);
          std::size_t found = fn.find (".");
          if (found == std::string::npos) {
            fn.append (std::to_string (d_num_images));
          }
          else {
            fn.insert (found, std::to_string (d_num_images));
          }
          d_png_fd[0] = fopen (fn.c_str (), "wb");
          d_png_fn[0] = fn;
        }

        d_png_ptr[0] = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
        NULL,
                                                NULL);
        d_info_ptr[0] = png_create_info_struct (d_png_ptr[0]);
        png_init_io (d_png_ptr[0], d_png_fd[0]);
        png_set_IHDR (d_png_ptr[0], d_info_ptr[0], d_width, d_height,
                      d_bit_depth, d_color_type,
                      PNG_INTERLACE_NONE,
                      PNG_COMPRESSION_TYPE_BASE,
                      PNG_FILTER_TYPE_BASE);
        png_write_info (d_png_ptr[0], d_info_ptr[0]);
      }
    }

    void
    noaa_apt_sink_impl::write_png_row ()
    {
      if (d_row_counter == d_height) {
        for (size_t i = 0; i < d_images_per_frame; i++) {
          png_write_end (d_png_ptr[i], NULL);
          fclose (d_png_fd[i]);
        }
        if (d_flip) {
          flip_image ();
        }
        d_row_counter = 0;
        d_num_images++;
        init_png ();
      }
      if (d_split) {
        for (size_t i = 0; i < d_images_per_frame; i++) {
          png_write_row (d_png_ptr[i], d_row_buffer + i * d_width / 2);
        }
      }
      else {
        png_write_row (d_png_ptr[0], d_row_buffer);
      }
      d_row_counter++;
    }
    noaa_apt_sink_impl::~noaa_apt_sink_impl ()
    {
      if (d_current_buffered_samples < d_width) {
        memset (d_row_buffer + d_current_buffered_samples, 0,
                (d_width - d_current_buffered_samples) * sizeof(uint8_t));
      }
      write_png_row ();
      if (d_row_counter < d_height) {
        size_t row_count = d_row_counter;
        for (size_t i = 0; i < d_height - row_count; i++) {
          memset (d_row_buffer, 0, d_width * sizeof(uint8_t));
          write_png_row ();
        }
      }

      for (size_t i = 0; i < d_images_per_frame; i++) {
        png_write_end (d_png_ptr[i], NULL);
        fclose (d_png_fd[i]);
      }
      if (d_flip) {
        flip_image ();
      }
      delete[] d_row_buffer;
      delete[] d_png_fd;
      delete[] d_png_ptr;
      delete[] d_info_ptr;
    }

    void
    noaa_apt_sink_impl::flip_image ()
    {
      int height;
      png_byte color_type;
      png_byte bit_depth;
      png_structp png_ptr;
      png_infop info_ptr;
      int number_of_passes;
      png_bytep* row_pointers;
      size_t width;
      if (d_split) {
        width = d_width / 2;
      }
      else {
        width = d_width;
      }
      for (size_t i = 0; i < d_images_per_frame; i++) {
        char header[8];    // 8 is the maximum size that can be checked
        d_png_fd[i] = fopen (d_png_fn[i].c_str (), "rb");
        fread (header, 1, 8, d_png_fd[i]);

        /* initialize stuff */
        png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL,
        NULL);

        info_ptr = png_create_info_struct (png_ptr);

        png_init_io (png_ptr, d_png_fd[i]);
        png_set_sig_bytes (png_ptr, 8);
        png_read_info (png_ptr, info_ptr);

        png_read_update_info (png_ptr, info_ptr);

        row_pointers = new png_bytep[d_height];
        for (size_t y = 0; y < d_height; y++) {
          row_pointers[y] = new png_byte[png_get_rowbytes (png_ptr, info_ptr)];
        }
        png_read_image (png_ptr, row_pointers);

        fclose (d_png_fd[i]);
        d_png_fd[i] = fopen (d_png_fn[i].c_str (), "wb");
        d_png_ptr[i] = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL,
        NULL,
                                                NULL);
        d_info_ptr[i] = png_create_info_struct (d_png_ptr[i]);
        png_init_io (d_png_ptr[i], d_png_fd[i]);
        png_set_IHDR (d_png_ptr[i], d_info_ptr[i], width, d_height, d_bit_depth,
                      d_color_type,
                      PNG_INTERLACE_NONE,
                      PNG_COMPRESSION_TYPE_BASE,
                      PNG_FILTER_TYPE_BASE);

        png_write_info (d_png_ptr[i], d_info_ptr[i]);
        for (int j = d_height - 1; j >= 0; j--) {
          uint8_t *istart = row_pointers[j];
          uint8_t *iend = istart + width;
          std::reverse (row_pointers[j], iend);
          png_write_row (d_png_ptr[i], row_pointers[j]);
        }
        png_write_end (d_png_ptr[i], NULL);
        fclose (d_png_fd[i]);
      }
      for (size_t y = 0; y < d_height; y++) {
        delete[] row_pointers[y];
      }
      delete[] row_pointers;

    }

    int
    noaa_apt_sink_impl::work (int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      uint8_t b = 0;
      float sample;
      long int r;

      for (size_t i = d_history_length - 1;
          i < noutput_items + d_history_length - 1; i++) {
        if (!d_synchronize_opt) {
          sample = 255 * in[i];
          r = (long int) rint (sample);
          if (r < 0)
            r = 0;
          else if (r > 255)
            r = 255;
          d_row_buffer[d_current_buffered_samples] = (uint8_t) r;
          d_current_buffered_samples++;
          if (d_current_buffered_samples == d_width) {
            write_png_row ();
            d_current_buffered_samples = 0;
          }
        }
        else {
          if (d_sync_found) {
            if (d_sample_counter < d_norm_window) {
              if (in[i] < d_min_value) {
                d_min_value = in[i];
              }
              if (in[i] > d_max_value) {
                d_max_value = in[i];
              }
              d_sample_counter++;
            }
            sample = ((in[i] - d_min_value) / (d_max_value - d_min_value));
            sample = 255 * sample;
            r = (long int) rint (sample);
            if (r < 0) {
              r = 0;
            }
            else if (r > 255) {
              r = 255;
            }
            d_row_buffer[d_current_buffered_samples] = (uint8_t) r;
            d_current_buffered_samples++;
            if (d_current_buffered_samples == d_width) {
              write_png_row ();
              d_current_buffered_samples = 0;
            }
          }
          else {
            b = in[i] > d_slicer_threshold ? 1 : 0;
            d_constructed_word = (d_constructed_word << 1) | b;
            if (d_constructed_word == d_sync_word) {
              d_sync_found = true;
              for (size_t j = i - (d_history_length - 1); j <= i; j++) {
                if (in[j] < d_min_value) {
                  d_min_value = in[i];
                }
                if (in[j] > d_max_value) {
                  d_max_value = in[j];
                }
              }
              for (size_t j = i - (d_history_length - 1); j <= i; j++) {
                sample = ((in[i] - d_min_value) / (d_max_value - d_min_value));
                sample = 255 * sample;
                r = (long int) rint (sample);
                if (r < 0) {
                  r = 0;
                }
                else if (r > 255) {
                  r = 255;
                }
                d_row_buffer[d_current_buffered_samples] = (uint8_t) r;
                d_current_buffered_samples++;
                if (d_current_buffered_samples == d_width) {
                  write_png_row ();
                  d_current_buffered_samples = 0;
                }
              }
            }
          }
        }
      }
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

