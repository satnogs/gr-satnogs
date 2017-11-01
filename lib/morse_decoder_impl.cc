/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, 2017
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
#include "morse_decoder_impl.h"
#include <satnogs/log.h>
namespace gr
{
  namespace satnogs
  {

    morse_decoder::sptr
    morse_decoder::make (char unrecognized_char)
    {
      return gnuradio::get_initial_sptr (
          new morse_decoder_impl (unrecognized_char));
    }

    void
    morse_decoder_impl::symbol_msg_handler (pmt::pmt_t msg)
    {
      bool res = false;
      std::string str;
      morse_symbol_t s;
      s = (morse_symbol_t) pmt::to_long (msg);

      switch (s)
        {
        case MORSE_DOT:
        case MORSE_DASH:
        case MORSE_S_SPACE:
          res = d_morse_tree.received_symbol (s);
          break;
          /*
           * If a word separator occurs it is a good time to retrieve the decoded
           * word
           */
        case MORSE_L_SPACE:
          /*
           * Inject a character separator, for the morse decoder to commit
           * the outstanding character
           */
          res = d_morse_tree.received_symbol (MORSE_S_SPACE);
          /* Just ignore the word separator if no word is yet decoded */
          if (d_morse_tree.get_word_len () == 0) {
            res = true;
            break;
          }
          str = d_morse_tree.get_word ();
          d_morse_tree.reset ();
          message_port_pub (pmt::mp ("out"),
                            pmt::make_blob (str.c_str (), str.length ()));
          break;
        case MORSE_INTRA_SPACE:
          /*Ignore it */
          break;
        default:
          LOG_ERROR("Unknown Morse symbol");
          return;
        }

      /*
       * If the decoding return false, it means that either an non decode-able
       * character situation occurred or the maximum word limit reached
       */
      if (!res) {
        if (d_morse_tree.get_max_word_len () == d_morse_tree.get_word_len ()) {
          str = d_morse_tree.get_word ();
          d_morse_tree.reset ();
          message_port_pub (pmt::mp ("out"),
                            pmt::make_blob (str.c_str (), str.length ()));
        }
      }
    }

    /*
     * The private constructor
     */
    morse_decoder_impl::morse_decoder_impl (char unrecognized_char) :
            gr::block ("morse_decoder", gr::io_signature::make (0, 0, 0),
                       gr::io_signature::make (0, 0, 0)),
            d_morse_tree (unrecognized_char)
    {
      /* Register the input and output msg handler */
      message_port_register_in (pmt::mp ("in"));
      message_port_register_out (pmt::mp ("out"));
      set_msg_handler (
          pmt::mp ("in"),
          boost::bind (&morse_decoder_impl::symbol_msg_handler, this, _1));
    }

  } /* namespace satnogs */
} /* namespace gr */

