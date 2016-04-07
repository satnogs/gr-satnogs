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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "frame_encoder_impl.h"

namespace gr
{
  namespace satnogs
  {

    frame_encoder::sptr
    frame_encoder::make (bool append_preamble, bool ecss_encap,
			 const std::string& dest_addr, uint8_t dest_ssid,
			 const std::string& src_addr, uint8_t src_ssid)
    {
      return gnuradio::get_initial_sptr (
	  new frame_encoder_impl (append_preamble, ecss_encap, dest_addr,
				  dest_ssid, src_addr, src_ssid));
    }

    /*
     * The private constructor
     */
    frame_encoder_impl::frame_encoder_impl (bool append_preamble,
					    bool ecss_encap,
					    const std::string& dest_addr,
					    uint8_t dest_ssid,
					    const std::string& src_addr,
					    uint8_t src_ssid) :
	    gr::sync_block ("frame_encoder", gr::io_signature::make (0, 0, 0),
			    gr::io_signature::make (0, 0, 0))
    {
    }

    /*
     * Our virtual destructor.
     */
    frame_encoder_impl::~frame_encoder_impl ()
    {
    }

    int
    frame_encoder_impl::work (int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

