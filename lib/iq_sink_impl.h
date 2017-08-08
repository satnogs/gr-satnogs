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

#ifndef INCLUDED_SATNOGS_IQ_SINK_IMPL_H
#define INCLUDED_SATNOGS_IQ_SINK_IMPL_H

#include <satnogs/iq_sink.h>
#include <chrono>
#include <fstream>

namespace gr
{
  namespace satnogs
  {

    class iq_sink_impl : public iq_sink
    {
    private:
      /**
       * The different values for iq sink status
       */
      typedef enum
      {
        IQ_SINK_STATUS_NULL = 0, //!< IQ_SINK_STATUS_NULL IQ sink block behaves just like a null sink
        IQ_SINK_STATUS_ACTIVE = 1, //!< IQ_SINK_STATUS_ACTIVE IQ sink block is active
      } iq_sink_status_t;

      iq_sink_status_t d_status;
      size_t d_num_points;
      float d_scale;
      int16_t *d_out;
      const char *d_filename;

    public:
      iq_sink_impl (const float scale, const char *filename, bool append,
                    const int status);
      ~iq_sink_impl ();

      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_IQ_SINK_IMPL_H */

