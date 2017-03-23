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

#include "ogg_source_impl.h"

namespace gr {
  namespace satnogs {

    ogg_source::sptr
    ogg_source::make(double samp_rate, const std::string& filename)
    {
      return gnuradio::get_initial_sptr
        (new ogg_source_impl(samp_rate, filename));
    }

    /*
     * The private constructor
     */
    ogg_source_impl::ogg_source_impl(double samp_rate, const std::string& filename)
      : gr::sync_block("ogg_source",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(float)))
    {
        if(ov_fopen(filename.c_str(), &d_ogvorb_f) < 0) {
          throw std::invalid_argument("Invalid .ogg file");
        }
    }

    /*
     * Our virtual destructor.
     */
    ogg_source_impl::~ogg_source_impl()
    {
    }

    int
    ogg_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      float *out = (float *) output_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

