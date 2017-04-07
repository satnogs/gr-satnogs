/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2017 Libre Space Foundation <http://librespacefoundation.org/>
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
#include "multi_format_msg_sink_impl.h"
#include <ctime>
#include <iostream>
#include <iomanip>

namespace gr
{
  namespace satnogs
  {

    multi_format_msg_sink::sptr
    multi_format_msg_sink::make (size_t format,
                                 bool timestamp,
                                 bool out_stdout,
                                 const std::string& filepath)
    {
      return gnuradio::get_initial_sptr (
          new multi_format_msg_sink_impl (format, timestamp,
                                          out_stdout, filepath));
    }

    void
    multi_format_msg_sink_impl::msg_handler_file (pmt::pmt_t msg)
    {
      uint8_t *su;
      std::string s ((const char *) pmt::blob_data (msg),
                     pmt::blob_length (msg));

      if(d_timestamp) {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        d_fos << "[" << std::put_time(&tm, "%F %T %z") << "] ";
      }

      switch (d_format)
        {
        case 0:
          d_fos << s << std::endl;
          break;
        case 1:
          su = (uint8_t *) pmt::blob_data (msg);
          for (size_t i = 0; i < pmt::blob_length (msg); i++) {
            d_fos << std::hex << std::showbase << std::setw (4)
                << (uint32_t) su[i] << " ";
          }
          d_fos << std::endl;
          break;
        case 2:
          su = (uint8_t *) pmt::blob_data (msg);
          for (size_t i = 0; i < pmt::blob_length (msg); i++) {
            d_fos << "0b" << std::bitset<8> (su[i]) << " ";
          }
          d_fos << std::endl;
          break;
        default:
          throw std::invalid_argument("Invalid format");
        }
    }

    void
    multi_format_msg_sink_impl::msg_handler_stdout (pmt::pmt_t msg)
    {
      uint8_t *su;
      std::string s ((const char *) pmt::blob_data (msg),
                     pmt::blob_length (msg));

      if(d_timestamp) {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::cout << "[" << std::put_time(&tm, "%F %T %z") << "] ";
      }

      switch (d_format)
        {
        case 0:
          std::cout << s << std::endl;
          break;
        case 1:
          su = (uint8_t *) pmt::blob_data (msg);
          for (size_t i = 0; i < pmt::blob_length (msg); i++) {
            std::cout << std::hex << std::showbase << std::setw (4)
                << (uint32_t) su[i] << " ";
          }
          std::cout << std::endl;
          break;
        case 2:
          su = (uint8_t *) pmt::blob_data (msg);
          for (size_t i = 0; i < pmt::blob_length (msg); i++) {
            std::cout << "0b" << std::bitset<8> (su[i]) << " ";
          }
          std::cout << std::endl;
          break;
        default:
          throw std::invalid_argument("Invalid format");
        }
    }

    /*
     * The private constructor
     */
    multi_format_msg_sink_impl::multi_format_msg_sink_impl (
        size_t format, bool timestamp, bool out_stdout,
        const std::string& filepath) :
            gr::block ("multi_format_msg_sink",
                       gr::io_signature::make (0, 0, 0),
                       gr::io_signature::make (0, 0, 0)),
            d_format (format),
            d_timestamp (timestamp),
            d_stdout (out_stdout)
    {
      message_port_register_in (pmt::mp ("in"));
      if(out_stdout) {
        set_msg_handler (
            pmt::mp ("in"),
            boost::bind (&multi_format_msg_sink_impl::msg_handler_stdout,
                         this, _1));
      }
      else{
        d_fos.open(filepath);
        set_msg_handler (
            pmt::mp ("in"),
            boost::bind (&multi_format_msg_sink_impl::msg_handler_file,
                         this, _1));
      }
    }

    multi_format_msg_sink_impl::~multi_format_msg_sink_impl ()
    {
      if(!d_stdout) {
        d_fos.close();
      }
    }

  } /* namespace satnogs */
} /* namespace gr */

