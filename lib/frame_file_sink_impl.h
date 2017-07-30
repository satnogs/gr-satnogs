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

#ifndef INCLUDED_SATNOGS_FRAME_FILE_SINK_IMPL_H
#define INCLUDED_SATNOGS_FRAME_FILE_SINK_IMPL_H

#include <satnogs/frame_file_sink.h>

namespace gr
{
  namespace satnogs
  {

    class frame_file_sink_impl : public frame_file_sink
    {
    private:
      const std::string d_prefix_name;
      int d_output_type;

    public:
      frame_file_sink_impl (const std::string& prefix_name, int output_type);
      ~frame_file_sink_impl ();

      void
      msg_handler_frame (pmt::pmt_t msg);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FRAME_FILE_SINK_IMPL_H */

