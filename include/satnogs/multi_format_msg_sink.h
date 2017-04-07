/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2017, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_MULTI_FORMAT_MSG_SINK_H
#define INCLUDED_SATNOGS_MULTI_FORMAT_MSG_SINK_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Block accepting clear text messages from various decoders.
     * Its purpose is to forward these messages at other services, programs,
     * stdout, etc,
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API multi_format_msg_sink : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<multi_format_msg_sink> sptr;

      /*!
       * \brief Block accepting clear text messages from various decoders.
       * Its purpose is to either print these messages to stdout or save them
       * in text format in a file.
       *
       * Depending on format parameter, the contents of each message are
       * converted to hexademical, binary or ASCII format.
       *
       * @param format the format that will used to display the messages.
       * 0: Clear Text 1: Hexademical 2: Binary
       * @param timestamp if set, a ISO 8601 timestamp is inserted in front of
       * each message
       * @param out_stdout if set, the messages are displayed in the stdout.
       * Otherwise messages are saved in a text file
       * @param filepath specifies the file path of the text file
       */
      static sptr
      make (size_t format, bool timestamp = true,
            bool out_stdout = true,
            const std::string& filepath = "");
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_multi_format_MSG_SINK_H */

