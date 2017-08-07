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
#include "iq_sink_impl.h"
#include <volk/volk.h>
#include <stdexcept>

namespace gr
{
  namespace satnogs
  {

    iq_sink::sptr
    iq_sink::make (const float scale, const char *filename, bool append,
                   const int status)
    {
      return gnuradio::get_initial_sptr (
          new iq_sink_impl (scale, filename, append, status));
    }

    /*
     * The private constructor
     */
    iq_sink_impl::iq_sink_impl (const float scale, const char *filename,
                                bool append, const int status) :
            gr::sync_block ("iq_sink",
                            gr::io_signature::make (1, 1, sizeof(gr_complex)),
                            gr::io_signature::make (0, 0, 0)),
            file_sink_base (filename, true, append),
            d_scale (scale),
            d_num_points (16384),
            d_status ((iq_sink_status_t) status)
    {
      set_max_noutput_items (d_num_points);
      unsigned int alignment = volk_get_alignment ();
      d_out = (int16_t*) volk_malloc (sizeof(int16_t) * d_num_points * 2,
                                      alignment);
    }

    /*
     * Our virtual destructor.
     */
    iq_sink_impl::~iq_sink_impl ()
    {
      volk_free (d_out);
    }

    int
    iq_sink_impl::work (int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      gr_complex *inbuf = (gr_complex*) input_items[0];
      int nwritten = 0;
      switch (d_status)
        {
        case IQ_SINK_STATUS_NULL:
          {
            return noutput_items;
            break;
          }

        case IQ_SINK_STATUS_ACTIVE:
          {
            /* update d_fp is required */
            do_update ();

            if (!d_fp)
              /* drop output on the floor */
              return noutput_items;

            volk_32f_s32f_convert_16i (d_out, (float*) inbuf, d_scale,
                                       noutput_items * 2);

            while (nwritten < noutput_items) {
              int count = fwrite (d_out, 2 * sizeof(int16_t),
                                  noutput_items - nwritten, d_fp);
              if (count == 0) {
                if (ferror (d_fp)) {
                  std::cout << count << std::endl;
                  std::stringstream s;
                  s << "file_sink write failed with error " << fileno (d_fp)
                      << std::endl;
                  throw std::runtime_error (s.str ());
                }
                /* if EOF */
                else {
                  break;
                }
              }
              nwritten += count;
            }

            return nwritten;
            break;
          }
        }
    }

  } /* namespace satnogs */
} /* namespace gr */

