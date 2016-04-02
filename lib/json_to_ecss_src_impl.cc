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
#include "json_to_ecss_src_impl.h"

#include <sstream>
#include <map>

#define BUFFER_SIZE 2048


namespace gr {
  namespace satnogs {

    json_to_ecss_src::sptr
    json_to_ecss_src::make()
    {
      return gnuradio::get_initial_sptr
        (new json_to_ecss_src_impl());
    }

    /*
     * The private constructor
     */
    json_to_ecss_src_impl::json_to_ecss_src_impl()
      : gr::block("json_to_ecss_src",
		       gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
			   is_running(true),
			   d_in_port(pmt::mp("in")),
			   d_out_port(pmt::mp("out"))
    {
    	message_port_register_out(d_out_port);
    	message_port_register_in(d_in_port);

    	d_buf = (uint8_t*)malloc(BUFFER_SIZE * sizeof(uint8_t));

    	new boost::thread (
    		      boost::bind (&json_to_ecss_src_impl::json_accepter, this));
    }

    /*
     * Our virtual destructor.
     */
    json_to_ecss_src_impl::~json_to_ecss_src_impl()
    {
    	is_running = false;
    }
    void
	json_to_ecss_src_impl::json_accepter(){
    	pmt::pmt_t message;
    	size_t length;

    	while(is_running){
    		message = delete_head_blocking(d_in_port,0);
    		length = blob_length(message);
    		d_buf = (uint8_t*)blob_data(message);
    		std::istringstream ss(std::string(d_buf, d_buf + length));
    		ptree tree;
    		read_json(ss,tree);
    		BOOST_FOREACH(const ptree::value_type &v, tree) {
    			std::cout<<"First = "<<v.first << "Second = "<<v.second.data()<<std::endl ;
    		}
    	}
    }

  } /* namespace satnogs */
} /* namespace gr */

