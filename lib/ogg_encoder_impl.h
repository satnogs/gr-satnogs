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


#ifndef INCLUDED_SATNOGS_OGG_ENCODER_IMPL_H
#define INCLUDED_SATNOGS_OGG_ENCODER_IMPL_H

#include <satnogs/ogg_encoder.h>
#include <vorbis/vorbisenc.h>

namespace gr {
  namespace satnogs {

    class ogg_encoder_impl : public ogg_encoder
    {
     private:
      // Nothing to declare in this block.
        ogg_stream_state d_os;
        ogg_page         d_og;
        ogg_packet       d_op;

        vorbis_info      d_vi;
        vorbis_comment   d_vc;

        vorbis_dsp_state d_vd;
        vorbis_block     d_vb;
        FILE* d_out;
        double d_samp_rate;
        float d_quality;

     public:
      ogg_encoder_impl(char* filename, double samp_rate, float quality);
      ~ogg_encoder_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_OGG_ENCODER_IMPL_H */

