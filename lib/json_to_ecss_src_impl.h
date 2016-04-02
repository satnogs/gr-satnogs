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

#ifndef INCLUDED_SATNOGS_JSON_TO_ECSS_SRC_IMPL_H
#define INCLUDED_SATNOGS_JSON_TO_ECSS_SRC_IMPL_H

#include <satnogs/json_to_ecss_src.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;



namespace gr {
  namespace satnogs {

    class json_to_ecss_src_impl : public json_to_ecss_src
    {
     private:
      // Nothing to declare in this block.
    	bool is_running;
    	pmt::pmt_t d_in_port;
    	pmt::pmt_t d_out_port;

    	uint8_t*	d_buf;

    	void json_accepter();

     public:
      json_to_ecss_src_impl();
      ~json_to_ecss_src_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_JSON_TO_ECSS_SRC_IMPL_H */

