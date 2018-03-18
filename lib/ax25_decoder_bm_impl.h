/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2018
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

#ifndef INCLUDED_SATNOGS_AX25_DECODER_BM_IMPL_H
#define INCLUDED_SATNOGS_AX25_DECODER_BM_IMPL_H

#include <satnogs/ax25_decoder_bm.h>
#include <gnuradio/digital/lfsr.h>

namespace gr
{
  namespace satnogs
  {

    class ax25_decoder_bm_impl : public ax25_decoder_bm
    {
    private:
      typedef enum
      {
        NO_SYNC, IN_SYNC, DECODING, FRAME_END
      } decoding_state_t;

      /**
       * If this flag is set, the decoder operates in promiscuous mode and
       * forwards all successfully decoded frames
       */
      const bool d_promisc;
      const bool d_descramble;
      const size_t d_max_frame_len;
      decoding_state_t d_state;
      uint8_t d_shift_reg;
      uint8_t d_dec_b;
      uint8_t d_prev_bit_nrzi;
      size_t d_received_bytes;
      size_t d_decoded_bits;
      digital::lfsr d_lfsr;
      uint8_t *d_frame_buffer;

      void
      reset_state ();
      void
      enter_sync_state ();
      void
      enter_decoding_state ();
      void
      enter_frame_end ();

      size_t
      descramble_and_decode (const uint8_t *in, size_t nitems);
      size_t
      decode (const uint8_t *in, size_t nitems);

      inline void
      descramble_and_decode_1b (uint8_t in);
      inline void
      decode_1b (uint8_t in);


    public:
      ax25_decoder_bm_impl (const std::string& addr, uint8_t ssid, bool promisc,
                            bool descramble, size_t max_frame_len);
      ~ax25_decoder_bm_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_DECODER_BM_IMPL_H */

