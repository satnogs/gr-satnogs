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
#include "decoder_8b10b_impl.h"
#include <gnuradio/blocks/count_bits.h>

namespace gr
{
  namespace satnogs
  {

    decoder_8b10b::sptr
    decoder_8b10b::make (const std::string& control_symbol,
                         size_t max_frame_len, int comp_type)
    {
      return gnuradio::get_initial_sptr (
          new decoder_8b10b_impl (control_symbol, max_frame_len, comp_type));
    }

    /*
     * The private constructor
     */
    decoder_8b10b_impl::decoder_8b10b_impl (const std::string& control_symbol,
                                            size_t max_frame_len, int comp_type) :
            gr::sync_block ("decoder_8b10b",
                            gr::io_signature::make (1, 1, sizeof(char)),
                            gr::io_signature::make (0, 0, 0)),
            d_max_frame_len (max_frame_len),
            d_comp_type (comp_type),
            d_control_symbol_pos (0),
            d_control_symbol_neg (0),
            d_data_reg (0)
    {
      message_port_register_out (pmt::mp ("pdu"));
      if (!set_access_code (control_symbol)) {
        GR_LOG_ERROR(d_logger, "control_symbol is not 10 bits");
        throw std::out_of_range ("control_symbol is not 10 bits");
      }
    }

    /*
     * Our virtual destructor.
     */
    decoder_8b10b_impl::~decoder_8b10b_impl ()
    {
    }

    bool
    decoder_8b10b_impl::set_access_code (const std::string &control_symbol)
    {
      unsigned len = control_symbol.length ();      // # of bytes in string

      /* if the control sequence is not 10-bit then throw exception */
      if (len != 10) {
        return false;
      }

      for (size_t i = 0; i < len; i++) {
        d_control_symbol_pos = (d_control_symbol_pos << 1)
            | (control_symbol[i] & 0x1);
      }
      d_control_symbol_neg = ~d_control_symbol_pos;

      return true;
    }

    int
    decoder_8b10b_impl::work (int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      for (int i = 0; i < noutput_items; i++) {

        d_data_reg = (d_data_reg << 1) | (in[i] & 0x1);

        unsigned long long wrong_bits = (d_data_reg ^ d_control_symbol_pos)
            & 0x3FF;
        unsigned int nwrong = gr::blocks::count_bits64 (wrong_bits);

        if (nwrong <= 1) {
          d_data_reg = 0;
          printf ("Packet found \n");
        }

        //GR_LOG_DEBUG(d_logger, boost::format ("Wrong number: %u") % wrong_bits);

      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  }
/* namespace satnogs */
} /* namespace gr */

