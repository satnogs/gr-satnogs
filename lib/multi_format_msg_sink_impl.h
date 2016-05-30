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

#ifndef INCLUDED_SATNOGS_MULTI_FORMAT_MSG_SINK_IMPL_H
#define INCLUDED_SATNOGS_MULTI_FORMAT_MSG_SINK_IMPL_H

#include <satnogs/multi_format_msg_sink.h>

namespace gr
{
  namespace satnogs
  {

    class multi_format_msg_sink_impl : public multi_format_msg_sink
    {
    private:
      void
      msg_handler(pmt::pmt_t msg);

      size_t d_format;

    public:
      multi_format_msg_sink_impl (size_t format);

    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_multi_format_MSG_SINK_IMPL_H */

