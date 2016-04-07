/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_FRAME_ENCODER_IMPL_H
#define INCLUDED_SATNOGS_FRAME_ENCODER_IMPL_H

#include <satnogs/frame_encoder.h>

namespace gr
{
  namespace satnogs
  {

    class frame_encoder_impl : public frame_encoder
    {
    private:
      // Nothing to declare in this block.

    public:
      frame_encoder_impl (bool append_preamble, bool ecss_encap,
			  const std::string& dest_addr, uint8_t dest_ssid,
			  const std::string& src_addr, uint8_t src_ssid);
      ~frame_encoder_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FRAME_ENCODER_IMPL_H */

