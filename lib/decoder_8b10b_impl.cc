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
#include <satnogs/log.h>

#include <gnuradio/blocks/count_bits.h>

namespace gr
{
  namespace satnogs
  {

    decoder_8b10b::sptr
    decoder_8b10b::make (const std::string& control_symbol,
                         size_t max_frame_len)
    {
      return gnuradio::get_initial_sptr (
          new decoder_8b10b_impl (control_symbol, max_frame_len));
    }

    /*
     * The private constructor
     */
    decoder_8b10b_impl::decoder_8b10b_impl (const std::string& control_symbol,
                                            size_t max_frame_len) :
            gr::sync_block ("decoder_8b10b",
                            gr::io_signature::make (1, 1, sizeof(char)),
                            gr::io_signature::make (0, 0, 0)),
            d_max_frame_len (max_frame_len),
            d_erasure_cnt (0),
            d_control_symbol_pos (0),
            d_control_symbol_neg (0),
            d_data_reg (0),
            d_wrong_bits (0),
            d_wrong_bits_neg (0),
            d_nwrong (0),
            d_nwrong_neg (0),
            d_word (0),
            d_word_cnt (0),
            d_state (IN_SYNC)
    {
      message_port_register_out (pmt::mp ("pdu"));
      set_output_multiple(10);

      if (!set_access_code (control_symbol)) {
        throw std::out_of_range ("control_symbol is not 10 bits");
      }

      d_8b_words = new uint8_t [d_max_frame_len / 10];
      d_erasures_indexes = new int [d_max_frame_len / 10];
    }

    /*
     * Our virtual destructor.
     */
    decoder_8b10b_impl::~decoder_8b10b_impl ()
    {
      delete [] d_8b_words;
      delete [] d_erasures_indexes;
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
      d_control_symbol_neg = (~d_control_symbol_pos) & 0x3FF;

      return true;
    }

    uint8_t
    decoder_8b10b_impl::process_10b (int write_pos)
    {
      uint16_t diff_bits = 0;
      uint8_t min_pos = 0, temp_min_pos_rd = 0, min_pos_rd = 0;
      uint8_t min_dist = 11;
      uint8_t curr_dist = 0;
      size_t i = 0;

      while ((i < 256) || (min_dist > 0)) {

        diff_bits = (d_word ^ (d_lookup_8b10b[0][i])) & 0x3FF;
        curr_dist = gr::blocks::count_bits16 (diff_bits);

        if (curr_dist > 5) {
          curr_dist = 10 - curr_dist;
          temp_min_pos_rd = 1;
        }
        else {
          temp_min_pos_rd = 0;
        }

        if (curr_dist < min_dist) {
          min_dist = curr_dist;
          min_pos = i;
          min_pos_rd = temp_min_pos_rd;
        }

        i++;
      }

      /* report that there is erasure to this 10 bits */
      d_8b_words[write_pos] = min_pos;

      /* If we did not found a perfect match, mark this index as erasure */
      if(min_dist != 0) {
        d_erasures_indexes[d_erasure_cnt++] = write_pos;
      }
    }

    static inline uint16_t
    pack_10b_word(const uint8_t *in)
    {
      return ((in[0] & 0x1) << 9) | ((in[1] & 0x1) << 8)
          | ((in[2] & 0x1) << 7) | ((in[3] & 0x1) << 6)
          | ((in[4] & 0x1) << 5) | ((in[5] & 0x1) << 4)
          | ((in[6] & 0x1) << 3) | ((in[7] & 0x1) << 2)
          | ((in[8] & 0x1) << 1) | (in[9] & 0x1);
    }

    int
    decoder_8b10b_impl::work (int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      int i;
      const uint8_t *in = (const uint8_t *) input_items[0];

      /* Search for the Comma symbol */
      if(d_state == IN_SYNC) {
        for (i = 0; i < noutput_items; i++) {
          d_data_reg = (d_data_reg << 1) | (in[i] & 0x1);
          d_wrong_bits = (d_data_reg ^ d_control_symbol_pos) & 0x3FF;
          d_wrong_bits_neg = (d_data_reg ^ d_control_symbol_neg) & 0x3FF;
          d_nwrong = gr::blocks::count_bits16 (d_wrong_bits);
          d_nwrong_neg = gr::blocks::count_bits16 (d_wrong_bits_neg);

          /* we found the controls symbol */
          if ((d_nwrong == 0) || (d_nwrong_neg == 0)) {
            d_erasure_cnt = 0;
            d_word_cnt = 0;
            d_state = DECODING;
            return i + 1;
          }
        }
        return noutput_items;
      }

      /*
       * From now one, we have a SYNC so we process the data
       * in chunks of 10 bits
       */
      for(i = 0; i < noutput_items / 10; i++) {
        d_word = pack_10b_word(&in[i * 10]);

        /* Revert 10b to 8b and accumulate! */
        process_10b (d_word_cnt);
        d_word_cnt++;


        if(d_word_cnt == d_max_frame_len / 10) {
          d_state = IN_SYNC;
          pmt::pmt_t data = pmt::init_u8vector (d_max_frame_len / 10,
                                                d_8b_words);

          pmt::pmt_t out = pmt::make_dict();
          if (d_erasure_cnt > 0) {
            pmt::pmt_t erasures = pmt::init_s32vector (d_erasure_cnt,
                                                       d_erasures_indexes);
            out = pmt::dict_add (out, pmt::intern ("erasures"), erasures);
          }
          out = pmt::dict_add(out, pmt::intern("data"), data);


          message_port_pub (pmt::mp ("pdu"), out);
          return (i+1) * 10;
        }
      }
      return noutput_items;
    }

  }
/* namespace satnogs */
} /* namespace gr */

