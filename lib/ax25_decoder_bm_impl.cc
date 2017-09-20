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
#include <satnogs/log.h>
#include <satnogs/ax25.h>
#include "ax25_decoder_bm_impl.h"

namespace gr
{
  namespace satnogs
  {

    ax25_decoder_bm::sptr
    ax25_decoder_bm::make (const std::string& addr, uint8_t ssid, bool promisc,
                           bool descramble, size_t max_frame_len,
                           size_t n_sync_flags)
    {
      return gnuradio::get_initial_sptr (
          new ax25_decoder_bm_impl (addr, ssid, promisc, descramble,
                                    max_frame_len, n_sync_flags));
    }

    /*
     * The private constructor
     */
    ax25_decoder_bm_impl::ax25_decoder_bm_impl (const std::string& addr,
                                                uint8_t ssid, bool promisc,
                                                bool descramble,
                                                size_t max_frame_len,
                                                size_t n_sync_flags) :
            gr::sync_block ("ax25_decoder_bm",
                            gr::io_signature::make (1, 1, sizeof(uint8_t)),
                            gr::io_signature::make (0, 0, 0)),
            d_promisc (promisc),
            d_descramble (descramble),
            d_max_frame_len (max_frame_len),
            d_sync_flags_thr (n_sync_flags - 1),
            d_state (NO_SYNC),
            d_shift_reg (0x0),
            d_dec_b (0x0),
            d_prev_bit_nrzi (0),
            d_received_bytes (0),
            d_sync_received (0),
            d_decoded_bits (0),
            d_lfsr (0x21, 0x0, 16),
            d_frame_buffer (
                new uint8_t[max_frame_len + AX25_MAX_ADDR_LEN
                    + AX25_MAX_CTRL_LEN + sizeof(uint16_t)])
    {
      /* Valid PDUs output message port */
      message_port_register_out (pmt::mp ("pdu"));
      /*
       * Valid invalid (wrong CRC, different destination Callsign/SSID,
       * wrong frame size)PDUs output message port
       */
      message_port_register_out (pmt::mp ("failed_pdu"));
    }

    void
    ax25_decoder_bm_impl::descramble_and_decode (const uint8_t* in,
                                                 size_t nitems)
    {
      size_t i;
      uint8_t descr_bit;
      uint8_t dec_bit;

      for (i = 0; i < nitems; i++) {
        /* Descramble the input bit */
        descr_bit = d_lfsr.next_bit_descramble (in[i]);

        /* Perform NRZI decoding */
        dec_bit = (~((descr_bit - d_prev_bit_nrzi) % 2)) & 0x1;
        d_prev_bit_nrzi = descr_bit;

        /* In AX.25 the LS bit is sent first */
        d_shift_reg = (d_shift_reg >> 1) | (dec_bit << 7);
        d_dec_b = (d_dec_b >> 1) | (dec_bit << 7);

        switch (d_state)
          {
          case NO_SYNC:
            if (d_shift_reg == AX25_SYNC_FLAG) {
              enter_sync_state ();
            }
            break;
          case IN_SYNC:
            d_decoded_bits++;
            if (d_decoded_bits == 8) {
              d_received_bytes++;
              d_decoded_bits = 0;
              if (d_shift_reg == AX25_SYNC_FLAG) {
                d_sync_received++;
                if (d_sync_received > d_sync_flags_thr) {
                  enter_decoding_state ();
                }
              }
              if (d_received_bytes > 3) {
                reset_state ();
              }
            }
            break;
          case DECODING:
            /*
             * If the received byte was an AX.25 sync flag, there are two
             * possibilities. Either it was the end of frame or just a repeat of the
             * preamble.
             *
             * Also in case in error at the preamble, the G3RUH polynomial should
             * re-sync after 3 repetitions of the SYNC flag. For this reason we demand
             * that the distance between the last SYNC flag is greater than 3 bytes
             */
            if (d_shift_reg == AX25_SYNC_FLAG) {
              if (d_received_bytes < 3) {
                enter_sync_state ();
              }
              else {
                /* Frame end */
                enter_frame_end ();
              }
            }
            else if ((d_shift_reg & 0xfc) == 0x7c) {
              /*This was a stuffed bit */
              d_dec_b <<= 1;
            }
            else if ((d_shift_reg & 0xfe) == 0xfe) {
              /*This can never happen... Error! */
              if (d_received_bytes > AX25_MIN_ADDR_LEN) {
                message_port_pub (
                    pmt::mp ("failed_pdu"),
                    pmt::make_blob (d_frame_buffer, d_received_bytes));
              }
              reset_state ();
            }
            else {
              d_decoded_bits++;
              if (d_decoded_bits == 8) {
                d_frame_buffer[d_received_bytes++] = d_dec_b;
                d_decoded_bits = 0;

                /*Check if the frame limit was reached */
                if (d_received_bytes >= d_max_frame_len) {
                  LOG_WARN("Wrong size");
                  message_port_pub (
                      pmt::mp ("failed_pdu"),
                      pmt::make_blob (d_frame_buffer, d_max_frame_len));
                  reset_state ();
                }
              }
            }
            break;
          case FRAME_END:
            if (d_shift_reg == AX25_SYNC_FLAG) {
              d_dec_b = 0;
              d_decoded_bits = 0;
              d_received_bytes = 0;
              d_state = FRAME_END;
            }
            else {
              d_decoded_bits++;
              if (d_decoded_bits / 8 > 3) {
                reset_state ();
              }
            }
            break;
          default:
            LOG_ERROR("Invalid decoding state");
          }
      }
    }

    void
    ax25_decoder_bm_impl::decode (const uint8_t* in, size_t nitems)
    {
      size_t i;
      uint8_t dec_bit;

      for (i = 0; i < nitems; i++) {

        /* Perform NRZI decoding */
        dec_bit = (~(((in[i] & 0x1) - d_prev_bit_nrzi) % 2)) & 0x1;
        d_prev_bit_nrzi = in[i] & 0x1;

        /* In AX.25 the LS bit is sent first */
        d_shift_reg = (d_shift_reg >> 1) | (dec_bit << 7);
        d_dec_b = (d_dec_b >> 1) | (dec_bit << 7);

        switch (d_state)
          {
          case NO_SYNC:
            if (d_shift_reg == AX25_SYNC_FLAG) {
              enter_sync_state ();
            }
            break;
          case IN_SYNC:
            d_decoded_bits++;
            if (d_decoded_bits == 8) {
              d_received_bytes++;
              d_decoded_bits = 0;
              if (d_shift_reg == AX25_SYNC_FLAG) {
                d_sync_received++;
                if (d_sync_received > d_sync_flags_thr) {
                  enter_decoding_state ();
                }
              }
              if (d_received_bytes > 3) {
                reset_state ();
              }
            }
            break;
          case DECODING:
            /*
             * If the received byte was an AX.25 sync flag, there are two
             * possibilities. Either it was the end of frame or just a repeat of the
             * preamble.
             *
             * Also in case in error at the preamble, the G3RUH polynomial should
             * re-sync after 3 repetitions of the SYNC flag. For this reason we demand
             * that the distance between the last SYNC flag is greater than 3 bytes
             */
            if (d_shift_reg == AX25_SYNC_FLAG) {
              if (d_received_bytes < 3) {
                enter_sync_state ();
              }
              else {
                /* Frame end */
                enter_frame_end ();
              }
            }
            else if ((d_shift_reg & 0xfc) == 0x7c) {
              /*This was a stuffed bit */
              d_dec_b <<= 1;
            }
            else if ((d_shift_reg & 0xfe) == 0xfe) {
              /*This can never happen... Error! */
              if (d_received_bytes > AX25_MIN_ADDR_LEN) {
                message_port_pub (
                    pmt::mp ("failed_pdu"),
                    pmt::make_blob (d_frame_buffer, d_received_bytes));
              }
              reset_state ();
            }
            else {
              d_decoded_bits++;
              if (d_decoded_bits == 8) {
                d_frame_buffer[d_received_bytes++] = d_dec_b;
                d_decoded_bits = 0;

                /*Check if the frame limit was reached */
                if (d_received_bytes >= d_max_frame_len) {
                  LOG_WARN("Wrong size");
                  message_port_pub (
                      pmt::mp ("failed_pdu"),
                      pmt::make_blob (d_frame_buffer, d_max_frame_len));
                  reset_state ();
                }
              }
            }
            break;
          case FRAME_END:
            if (d_shift_reg == AX25_SYNC_FLAG) {
              d_dec_b = 0x0;
              d_shift_reg = 0x0;
              d_decoded_bits = 0;
              d_received_bytes = 0;
              d_state = FRAME_END;
            }
            else {
              d_decoded_bits++;
              if (d_decoded_bits / 8 > 3) {
                reset_state ();
              }
            }
            break;
          default:
            LOG_ERROR("Invalid decoding state");
          }
      }
    }

    /*
     * Our virtual destructor.
     */
    ax25_decoder_bm_impl::~ax25_decoder_bm_impl ()
    {
      delete [] d_frame_buffer;
    }

    void
    ax25_decoder_bm_impl::reset_state ()
    {
      d_state = NO_SYNC;
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_prev_bit_nrzi = 0;
      d_sync_received = 0;
    }

    void
    ax25_decoder_bm_impl::enter_sync_state ()
    {
      d_state = IN_SYNC;
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_sync_received = 0;
    }

    void
    ax25_decoder_bm_impl::enter_decoding_state ()
    {
      d_state = DECODING;
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
    }

    void
    ax25_decoder_bm_impl::enter_frame_end ()
    {
      uint16_t fcs;
      uint16_t recv_fcs = 0x0;

      /* First check if the size of the frame is valid */
      if (d_received_bytes < AX25_MIN_ADDR_LEN + sizeof(uint16_t)) {
        message_port_pub (pmt::mp ("failed_pdu"),
                          pmt::make_blob (d_frame_buffer, d_received_bytes));
        d_dec_b = 0x0;
        d_shift_reg = 0x0;
        d_decoded_bits = 0;
        d_received_bytes = 0;
        d_sync_received = 0;
        d_state = FRAME_END;
        return;
      }

      /* Check if the frame is correct using the FCS field */
      fcs = ax25_fcs (d_frame_buffer, d_received_bytes - sizeof(uint16_t));
      recv_fcs = (((uint16_t) d_frame_buffer[d_received_bytes - 1]) << 8)
          | d_frame_buffer[d_received_bytes - 2];

      if (fcs == recv_fcs) {
        message_port_pub (
            pmt::mp ("pdu"),
            pmt::make_blob (d_frame_buffer,
                            d_received_bytes - sizeof(uint16_t)));
      }
      else {
        message_port_pub (
            pmt::mp ("failed_pdu"),
            pmt::make_blob (d_frame_buffer,
                            d_received_bytes - sizeof(uint16_t)));
        LOG_WARN("Wrong crc");
      }
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_sync_received = 0;
      d_state = FRAME_END;
    }


    int
    ax25_decoder_bm_impl::work (int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];

      if (d_descramble) {
        descramble_and_decode (in, noutput_items);
      }
      else {
        decode (in, noutput_items);
      }
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */

