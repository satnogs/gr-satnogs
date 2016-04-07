/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_AX25_DECODER_B_IMPL_H
#define INCLUDED_SATNOGS_AX25_DECODER_B_IMPL_H

#include <satnogs/ax25_decoder_b.h>
#include <satnogs/ax25.h>

namespace gr
{
  namespace satnogs
  {

    class ax25_decoder_b_impl : public ax25_decoder_b
    {
    private:
      typedef enum {
	NO_SYNC,
	IN_SYNC
      } decoding_state_t;

      /**
       * If this flag is set, the decoder operates in promiscuous mode and
       * forwards all successfully decoded frames
       */
      const bool d_promisc;
      decoding_state_t d_state;
      uint8_t d_dec_b;
      uint8_t d_prev_bit;
      size_t d_received_bytes;
      size_t d_decoded_bits;
      size_t d_cont_1;

      uint8_t *d_frame_buffer;

      void reset_state();
      void enter_sync_state();
      void update_recv_frame(uint8_t byte);

    public:
      ax25_decoder_b_impl (std::string addr, uint8_t ssid, bool promisc);
      ~ax25_decoder_b_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_DECODER_B_IMPL_H */

