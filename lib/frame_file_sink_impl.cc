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
#include "frame_file_sink_impl.h"

namespace gr
{
  namespace satnogs
  {

    frame_file_sink::sptr
    frame_file_sink::make (const std::string& prefix_name, int output_type)
    {
      return gnuradio::get_initial_sptr (
	  new frame_file_sink_impl (prefix_name, output_type));
    }

    /*
     * The private constructor
     */
    frame_file_sink_impl::frame_file_sink_impl (const std::string& prefix_name,
						int output_type) :
	    gr::block ("frame_file_sink", gr::io_signature::make (0, 0, 0),
		       gr::io_signature::make (0, 0, 0)),
	    d_prefix_name (prefix_name),
	    d_output_type (output_type)
    {
      message_port_register_in (pmt::mp ("frame"));
      set_msg_handler (
	  pmt::mp ("frame"),
	  boost::bind (&frame_file_sink_impl::msg_handler_frame, this, _1));

    }

    /*
     * Our virtual destructor.
     */
    frame_file_sink_impl::~frame_file_sink_impl ()
    {
    }

    void
    frame_file_sink_impl::msg_handler_frame (pmt::pmt_t msg)
    {

      /* check for the current UTC time */
      std::stringstream tmp_time;
      std::time_t t = std::time (nullptr);
      tmp_time << std::put_time (std::gmtime (&t), "%FT%H-%M-%S");
      std::string time2string = tmp_time.str ();

      /* create name of the file according prefix and timestamp */
      std::string filename;
      filename.append (d_prefix_name);
      filename.append ("_");
      filename.append (time2string);

      uint8_t *su;

      switch (d_output_type)
	{
	case 0:
	  {
	    /* add .txt to filename */
	    filename.append (".txt");
	    FILE *fd = fopen (filename.c_str (), "w");
	    if (fd == NULL) {
	      throw std::invalid_argument ("Error opening file");

	    }
	    fwrite ((const char *) pmt::blob_data (msg), pmt::blob_length (msg),
		    1, fd);
	    fclose (fd);
	    break;
	  }
	case 1:
	  {
	    /* add .hex to filename */
	    filename.append (".hex");
	    FILE *fd = fopen (filename.c_str (), "w");
	    if (fd == NULL) {
	      throw std::invalid_argument ("Error opening file");

	    }
	    su = (uint8_t *) pmt::blob_data (msg);
	    std::stringstream tmp_hex;
	    for (size_t i = 0; i < pmt::blob_length (msg); i++) {
	      tmp_hex << std::hex << std::showbase << std::setw (4)
		  << (uint32_t) su[i] << " ";
	    }
	    std::string str_hex = tmp_hex.str ();
	    fprintf (fd, "%s", str_hex.c_str ());
	    fclose (fd);
	    break;
	  }
	case 2:
	  {
	    /* add .bin to filename */
	    filename.append (".bin");
	    FILE *fd = fopen (filename.c_str (), "w");
	    if (fd == NULL) {
	      throw std::invalid_argument ("Error opening file");

	    }
	    su = (uint8_t *) pmt::blob_data (msg);
	    std::stringstream tmp_bin;
	    for (size_t i = 0; i < pmt::blob_length (msg); i++) {
	      tmp_bin << "0b" << std::bitset<8> (su[i]) << " ";
	    }
	    std::string str_bin = tmp_bin.str ();
	    fprintf (fd, "%s", str_bin.c_str ());
	    fclose (fd);
	    break;
	  }
	default:
	  throw std::invalid_argument ("Invalid format");
	}

    }

  } /* namespace satnogs */
} /* namespace gr */

