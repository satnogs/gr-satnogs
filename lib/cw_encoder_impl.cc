/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017
 *  Libre Space Foundation <http://librespacefoundation.org/>
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
#include "cw_encoder_impl.h"

namespace gr {
  namespace satnogs {

    const std::vector<char> cw_encoder_impl::cw_chars (
      { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
          'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2',
          '3', '4', '5', '6', '7', '8', '9', '0' });
    const std::vector<std::string> cw_encoder_impl::cw_symbols (
          { ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
              ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
              "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", ".----",
              "..---", "...--", "....-", ".....", "-....", "--...", "---..",
              "----.", "-----" });

    cw_encoder::sptr
    cw_encoder::make(double samp_rate, double cw_freq, size_t wpm)
    {
      return gnuradio::get_initial_sptr
        (new cw_encoder_impl(samp_rate, cw_freq, wpm));
    }

    /*
     * The private constructor
     */
    cw_encoder_impl::cw_encoder_impl(double samp_rate, double cw_freq,
                                     size_t wpm)
      : gr::sync_block("cw_encoder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_samp_rate (samp_rate),
              d_cw_freq (cw_freq),
              d_wpm (wpm),
              d_nco (),
              d_word (new uint8_t[2048]),
              d_remaining (0)
    {
      message_port_register_in(pmt::mp("word"));
      d_nco.set_freq ((2 * M_PI * cw_freq) / samp_rate);
    }

    /*
     * Our virtual destructor.
     */
    cw_encoder_impl::~cw_encoder_impl()
    {
      delete [] d_word;
    }

    static inline size_t
    find_char_idx(const char* characters, size_t len, char c)
    {
      size_t i;
      for(i = 0; i < len; i++) {
        if(characters[i] == c){
          return i;
        }
      }
      return len;
    }

    std::string
    cw_encoder_impl::get_cw_symbol (char c)
    {
      size_t i;
      for(i = 0; i < cw_chars.size(); i++) {
        if(cw_chars[i] == c) {
          return cw_symbols[i];
        }
      }
      return "";
    }

    int
    cw_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

