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
#include "ccsds_rs_decoder_mm_impl.h"

#include <satnogs/log.h>
extern "C" {
  #include <fec.h>
}

namespace gr {
  namespace satnogs {

    ccsds_rs_decoder_mm::sptr
    ccsds_rs_decoder_mm::make()
    {
      return gnuradio::get_initial_sptr
        (new ccsds_rs_decoder_mm_impl());
    }

    /*
     * The private constructor
     */
    ccsds_rs_decoder_mm_impl::ccsds_rs_decoder_mm_impl()
      : gr::block("ccsds_rs_decoder_mm",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("in"));
      message_port_register_out(pmt::mp("pdu"));

      set_msg_handler (
          pmt::mp ("in"),
          boost::bind (&ccsds_rs_decoder_mm_impl::message_handler, this, _1));
    }

    void
    ccsds_rs_decoder_mm_impl::message_handler (pmt::pmt_t m)
    {
      uint8_t data[255];
      int erasures[255];
      const uint8_t *data_ref;
      size_t data_len;
      const int *erasures_ref;
      size_t erasures_len;

      pmt::pmt_t pmt_data = pmt::dict_ref(m, pmt::intern("data"), pmt::PMT_NIL);
      pmt::pmt_t pmt_erasures = pmt::dict_ref(m, pmt::intern("erasures"),
                                              pmt::PMT_NIL);
      if (pmt::equal (pmt::PMT_NIL, pmt_data)) {
        LOG_ERROR("Invalid message format.");
      }

      data_ref = pmt::u8vector_elements(pmt_data, data_len);
      memcpy(data, data_ref, data_len);

      if( pmt::equal (pmt::PMT_NIL, pmt_erasures)) {
        decode_rs_ccsds(data, NULL, 0, (int)(255 - data_len));
      }
      else {
        erasures_len = pmt::blob_length(pmt_erasures);
        erasures_ref = pmt::s32vector_elements(pmt_erasures, erasures_len);
        memcpy(erasures, erasures_ref, erasures_len * sizeof(int));
        decode_rs_ccsds(data, erasures, (int)erasures_len,
                        (int)(255 - data_len));
      }
      message_port_pub(pmt::mp("pdu"), pmt::make_blob(data, 223));
    }

    /*
     * Our virtual destructor.
     */
    ccsds_rs_decoder_mm_impl::~ccsds_rs_decoder_mm_impl()
    {
    }

  } /* namespace satnogs */
} /* namespace gr */

