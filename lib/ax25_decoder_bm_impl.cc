/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, 2017, 2018
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
                           bool descramble, size_t max_frame_len)
    {
      return gnuradio::get_initial_sptr (
          new ax25_decoder_bm_impl (addr, ssid, promisc, descramble,
                                    max_frame_len));
    }

    /*
     * The private constructor
     */
    ax25_decoder_bm_impl::ax25_decoder_bm_impl (const std::string& addr,
                                                uint8_t ssid, bool promisc,
                                                bool descramble,
                                                size_t max_frame_len) :
            gr::sync_block ("ax25_decoder_bm",
                            gr::io_signature::make (1, 1, sizeof(uint8_t)),
                            gr::io_signature::make (0, 0, 0)),
            d_promisc (promisc),
            d_descramble (descramble),
            d_max_frame_len (max_frame_len),
            d_state (NO_SYNC),
            d_shift_reg (0x0),
            d_dec_b (0x0),
            d_prev_bit_nrzi (0),
            d_received_bytes (0),
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

    size_t
    ax25_decoder_bm_impl::descramble_and_decode (const uint8_t* in,
                                                 size_t nitems)
    {
      size_t i;
      uint8_t descr_bit;
      uint8_t dec_bit;

      switch (d_state)
        {
        case NO_SYNC:
          for (i = 0; i < nitems; i++) {
            descramble_and_decode_1b (in[i]);
            if (d_shift_reg == AX25_SYNC_FLAG) {
              enter_sync_state ();
              return i + 1;
            }
          }
          return nitems;
        case IN_SYNC:
          /*
           * Most of the transmitters repeat several times the AX.25 SYNC
           * In case of G3RUH this is mandatory to allow the self synchronizing
           * scrambler to settle
           */
          for (i = 0; i < nitems; i++) {
            descramble_and_decode_1b (in[i]);
            d_decoded_bits++;
            if(d_decoded_bits == 8) {
              /* Perhaps we are in frame! */
              if(d_shift_reg != AX25_SYNC_FLAG) {
                enter_decoding_state();
                return i+1;
              }
              d_decoded_bits = 0;
            }
          }
          return nitems;
        case DECODING:
          for (i = 0; i < nitems; i++) {
            descramble_and_decode_1b (in[i]);
            if (d_shift_reg == AX25_SYNC_FLAG) {
              LOG_DEBUG("Found frame end");
              enter_frame_end();
              return i+1;
            }
            else if ((d_shift_reg & 0xfc) == 0x7c) {
              /*This was a stuffed bit */
              d_dec_b <<= 1;
            }
            else if((d_shift_reg & 0xfe) == 0xfe) {
              LOG_DEBUG("Invalid shift register value %u", d_received_bytes);
              reset_state ();
              return i+1;
            }
            else{
              d_decoded_bits++;
              if(d_decoded_bits == 8) {
                d_frame_buffer[d_received_bytes] = d_dec_b;
                d_received_bytes++;
                d_decoded_bits = 0;

                /*Check if the frame limit was reached */
                if (d_received_bytes >= d_max_frame_len) {
                  LOG_DEBUG("Wrong size");
                  message_port_pub (
                      pmt::mp ("failed_pdu"),
                      pmt::make_blob (d_frame_buffer, d_max_frame_len));
                  reset_state ();
                  return i+1;
                }
              }
            }
          }
          return nitems;
        case FRAME_END:
          for (i = 0; i < nitems; i++) {
            descramble_and_decode_1b (in[i]);
            d_decoded_bits++;
            if (d_decoded_bits == 8) {
              /* Repetitions of the trailing SYNC flag finished */
              if (d_shift_reg != AX25_SYNC_FLAG) {
                reset_state();
                return i + 1;
              }
              d_decoded_bits = 0;
            }
          }
          return nitems;
        default:
          LOG_ERROR("Invalid decoding state");
          reset_state();
          return nitems;
        }
    }

    size_t
    ax25_decoder_bm_impl::decode (const uint8_t* in, size_t nitems)
    {
      size_t i;
      uint8_t descr_bit;
      uint8_t dec_bit;
      switch (d_state)
        {
        case NO_SYNC:
          for (i = 0; i < nitems; i++) {
            decode_1b (in[i]);
            if (d_shift_reg == AX25_SYNC_FLAG) {
              enter_sync_state ();
              return i + 1;
            }
          }
          return nitems;
        case IN_SYNC:
          /*
           * Most of the transmitters repeat several times the AX.25 SYNC
           * In case of G3RUH this is mandatory to allow the self synchronizing
           * scrambler to settle
           */
          for (i = 0; i < nitems; i++) {
            decode_1b (in[i]);
            d_decoded_bits++;
            if (d_decoded_bits == 8) {
              /* Perhaps we are in frame! */
              if (d_shift_reg != AX25_SYNC_FLAG) {
                enter_decoding_state ();
                return i + 1;
              }
              d_decoded_bits = 0;
            }
          }
          return nitems;
        case DECODING:
          for (i = 0; i < nitems; i++) {
            decode_1b (in[i]);
            if (d_shift_reg == AX25_SYNC_FLAG) {
              enter_frame_end ();
              return i + 1;
            }
            else if ((d_shift_reg & 0xfc) == 0x7c) {
              /*This was a stuffed bit */
              d_dec_b <<= 1;
            }
            else if ((d_shift_reg & 0xfe) == 0xfe) {
              LOG_DEBUG("Invalid shift register value");
              reset_state ();
              return i + 1;
            }
            else {
              d_decoded_bits++;
              if (d_decoded_bits == 8) {
                d_frame_buffer[d_received_bytes] = d_dec_b;
                d_received_bytes++;
                d_decoded_bits = 0;

                /*Check if the frame limit was reached */
                if (d_received_bytes >= d_max_frame_len) {
                  LOG_DEBUG("Wrong size");
                  message_port_pub (
                      pmt::mp ("failed_pdu"),
                      pmt::make_blob (d_frame_buffer, d_max_frame_len));
                  reset_state ();
                  return i + 1;
                }
              }
            }
          }
          return nitems;
        case FRAME_END:
          for (i = 0; i < nitems; i++) {
            decode_1b (in[i]);
            d_decoded_bits++;
            if (d_decoded_bits == 8) {
              /* Repetitions of the trailing SYNC flag finished */
              if (d_shift_reg != AX25_SYNC_FLAG) {
                reset_state ();
                return i + 1;
              }
              d_decoded_bits = 0;
            }
          }
          return nitems;
        default:
          LOG_ERROR("Invalid decoding state");
          reset_state ();
          return nitems;
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
    }

    void
    ax25_decoder_bm_impl::enter_sync_state ()
    {
      d_state = IN_SYNC;
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
    }

    void
    ax25_decoder_bm_impl::enter_decoding_state ()
    {
      uint8_t tmp;
      d_state = DECODING;
      d_decoded_bits = 0;
      d_shift_reg = 0x0;

      /*
       * Due to the possibility of bit stuffing on the first byte some special
       * handling is necessary
       */
      tmp = d_dec_b;
      d_dec_b = 0x0;
      for (size_t i = 0; i < 8; i++) {
        d_shift_reg = (d_shift_reg >> 1) | (((tmp >> i) & 0x1) << 7);
        d_dec_b = (d_dec_b >> 1) | (((tmp >> i) & 0x1) << 7);
        if ((d_shift_reg & 0xfc) == 0x7c) {
          /*This was a stuffed bit */
          d_dec_b <<= 1;
        }
        else{
          d_decoded_bits++;
        }
      }

      if(d_decoded_bits == 8) {
        d_frame_buffer[0] = d_dec_b;
        d_decoded_bits = 0;
        d_received_bytes = 1;
      }
      else {
        d_received_bytes = 0;
      }

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
        LOG_DEBUG("Wrong crc");
      }
      d_dec_b = 0x0;
      d_shift_reg = 0x0;
      d_decoded_bits = 0;
      d_received_bytes = 0;
      d_state = FRAME_END;
    }

    /**
     * Performs descrambling and NRZI decoding of an input bit.
     * The decoded bit is then shifted in front of the d_shift_reg and the d_dec_b
     * variables. This shift in front is due to the LS bit first transmission
     * of the Ax.25 protocol.
     *
     * @param in input bit
     */
    inline void
    ax25_decoder_bm_impl::descramble_and_decode_1b (uint8_t in)
    {
      uint8_t descr_bit;
      uint8_t dec_bit;

      in &= 0x1;

      /* Perform NRZI decoding */
      dec_bit = (~((in - d_prev_bit_nrzi) % 2)) & 0x1;
      d_prev_bit_nrzi = in;


      /* Descramble the input bit */
      descr_bit = d_lfsr.next_bit_descramble (dec_bit);

      /* In AX.25 the LS bit is sent first */
      d_shift_reg = (d_shift_reg >> 1) | (descr_bit << 7);
      d_dec_b = (d_dec_b >> 1) | (descr_bit << 7);
    }

    inline void
    ax25_decoder_bm_impl::decode_1b (uint8_t in)
    {
      uint8_t dec_bit;
      in &= 0x1;

      /* Perform NRZI decoding */
      dec_bit = (~((in - d_prev_bit_nrzi) % 2)) & 0x1;
      d_prev_bit_nrzi = in;

      /* In AX.25 the LS bit is sent first */
      d_shift_reg = (d_shift_reg >> 1) | (dec_bit << 7);
      d_dec_b = (d_dec_b >> 1) | (dec_bit << 7);
    }

    int
    ax25_decoder_bm_impl::work (int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      int ret;
      const uint8_t *in = (const uint8_t *) input_items[0];


      if(noutput_items < 1) {
        return noutput_items;
      }
      if (d_descramble) {
        ret = descramble_and_decode (in, noutput_items);
      }
      else {
        ret = decode (in, noutput_items);
      }
      return ret;
    }

  } /* namespace satnogs */
} /* namespace gr */

