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

#ifndef INCLUDED_SATNOGS_OGG_SOURCE_IMPL_H
#define INCLUDED_SATNOGS_OGG_SOURCE_IMPL_H

#include <satnogs/ogg_source.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace gr
{
  namespace satnogs
  {

    class ogg_source_impl : public ogg_source
    {
    private:
      const size_t              d_channels;
      const bool                d_repeat;
      OggVorbis_File            d_ogvorb_f;

      int16_t                   *d_in_buffer;
      float                     *d_out_buffer;

    public:
      ogg_source_impl (const std::string& filename, size_t channels,
                       bool repeat);
      ~ogg_source_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_OGG_SOURCE_IMPL_H */

