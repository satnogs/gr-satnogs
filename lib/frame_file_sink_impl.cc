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
            d_output_type (output_type),
            d_filename_prev(""),
            d_counter(0)
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
      std::chrono::system_clock::time_point p2 =
          std::chrono::system_clock::now ();

      char buffer[30];
      std::time_t t2 = std::chrono::system_clock::to_time_t (p2);
      struct tm * timeinfo;
      timeinfo = std::gmtime (&t2);

      std::strftime (buffer, 30, "%FT%H-%M-%S", timeinfo);
      //puts (buffer);

      /* create name of the file according prefix and timestamp */
      std::string filename;
      filename.append (d_prefix_name);
      filename.append ("_");
      filename.append (buffer);


      if (!filename.compare(d_filename_prev))
      {
        filename.append ("_");
        d_counter++;
        filename.append(std::to_string(d_counter));
        d_filename_prev.assign(filename);
      }
      else
      {
        d_filename_prev.assign(filename);
        d_counter=0;
      }

      uint8_t *su;

      switch (d_output_type)
        {
        case 0:
          {
            /* add .txt to filename */
            filename.append (".txt");
            std::ofstream fd (filename.c_str ());
            fd.write ((const char *) pmt::blob_data (msg),
                      pmt::blob_length (msg));
            fd.close ();
            break;
          }
        case 1:
          {
            /* add .hex to filename */
            filename.append (".hex");
            std::ofstream fd (filename.c_str ());
            su = (uint8_t *) pmt::blob_data (msg);
            for (size_t i = 0; i < pmt::blob_length (msg); i++) {
              fd << std::hex << std::showbase << std::setw (4)
                  << (uint32_t) su[i] << " ";
            }
            fd.close ();
            break;
          }
        case 2:
          {
            /* add .bin to filename */
            filename.append (".bin");
            std::ofstream fd (filename.c_str ());
            su = (uint8_t *) pmt::blob_data (msg);
            for (size_t i = 0; i < pmt::blob_length (msg); i++) {
              fd << "0b" << std::bitset<8> (su[i]) << " ";
            }
            fd.close ();
            break;
          }
        default:
          throw std::invalid_argument ("Invalid format");
        }

    }

  } /* namespace satnogs */
} /* namespace gr */

