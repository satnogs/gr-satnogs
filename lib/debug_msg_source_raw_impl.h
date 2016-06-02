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

#ifndef INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_IMPL_H
#define INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_IMPL_H

#include <satnogs/debug_msg_source_raw.h>

namespace gr
{
  namespace satnogs
  {

    class debug_msg_source_raw_impl : public debug_msg_source_raw
    {
    private:
      const size_t d_buf_len;
      const double d_delay;
      const bool d_repeat;
      bool d_running;
      boost::shared_ptr<boost::thread> d_thread;
      uint8_t *d_buf;

      void
      msg_sender ();


    public:
      debug_msg_source_raw_impl (const std::vector<uint8_t> &msg, double delay,
				 bool repeat);
      ~debug_msg_source_raw_impl ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_IMPL_H */

