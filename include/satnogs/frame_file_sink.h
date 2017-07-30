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

#ifndef INCLUDED_SATNOGS_FRAME_FILE_SINK_H
#define INCLUDED_SATNOGS_FRAME_FILE_SINK_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief <+description of block+>
     * \ingroup satnogs
     *
     */
    class SATNOGS_API frame_file_sink : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<frame_file_sink> sptr;

      /*!
       * Frame to file, sink block
       * @param prefix_name Prefix of the file name, including the directory path
       * @param output_type Format type of the output file, txt, hex, bin
       */
      static sptr
      make (const std::string& prefix_name, int output_type);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FRAME_FILE_SINK_H */

