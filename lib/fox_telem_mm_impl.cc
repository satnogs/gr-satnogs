/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <http://librespacefoundation.org/>
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
#include "fox_telem_mm_impl.h"

#include <satnogs/log.h>

namespace gr {
  namespace satnogs {

    /**
     * AMSAT FOX spacecraft IDs
     */
    const uint8_t fox_telem_mm_impl::fox_id[] = {0x1, 0x2, 0x3};

    fox_telem_mm::sptr
    fox_telem_mm::make()
    {
      return gnuradio::get_initial_sptr
        (new fox_telem_mm_impl());
    }

    /*
     * The private constructor
     */
    fox_telem_mm_impl::fox_telem_mm_impl()
      : gr::block("fox_telem_mm",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              /* 6 header, 58 payload */
              d_frame_size (6 + 58)
    {
      message_port_register_in (pmt::mp ("in"));
      message_port_register_out (pmt::mp ("raw"));

      set_msg_handler (
          pmt::mp ("in"),
          boost::bind (&fox_telem_mm_impl::process_frame, this, _1));
    }

    static inline bool
    is_spacecraft_valid(uint8_t id)
    {
      for (size_t i = 0; i < sizeof(fox_telem_mm_impl::fox_id); i++) {
        if (fox_telem_mm_impl::fox_id[i] == id) {
          return true;
        }
      }
      return false;
    }

    void
    fox_telem_mm_impl::process_frame (pmt::pmt_t msg)
    {
      uint8_t fox_id;
      const uint8_t *data = (const uint8_t *) pmt::blob_data(msg);
      size_t data_len = pmt::blob_length(msg);

      if(data_len != 223) {
        LOG_ERROR("Invalid message");
      }

      /* Check the spacecraft ID */
      fox_id = data[0] & 0x3;
      if(!is_spacecraft_valid(fox_id)) {
        return;
      }

      message_port_pub(pmt::mp("raw"), pmt::make_blob(data, d_frame_size));
    }

    /*
     * Our virtual destructor.
     */
    fox_telem_mm_impl::~fox_telem_mm_impl()
    {
    }

  } /* namespace satnogs */
} /* namespace gr */

