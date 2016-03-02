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

#ifndef INCLUDED_SATNOGS_AX25_ENCODER_BF_IMPL_H
#define INCLUDED_SATNOGS_AX25_ENCODER_BF_IMPL_H

#include <satnogs/ax25_encoder_bf.h>
#include <boost/thread/mutex.hpp>

namespace gr {
  namespace satnogs {

    class ax25_encoder_bf_impl : public ax25_encoder_bf
    {
     private:
      const ax25_frame_type_t d_type;
      size_t d_remaining;
      size_t d_produced;
      /* Twice the maximum frame length is enough to hold all possible input data*/
      float d_endoded_frame[(AX25_MAX_FRAME_LEN * 2)];
      uint8_t d_tmp_buf[AX25_MAX_FRAME_LEN * 2];
      uint8_t d_addr_field[AX25_MAX_ADDR_LEN];
      size_t d_addr_len;
      boost::mutex d_mutex;

      void add_sob(uint64_t item);
      void add_eob(uint64_t item);

     public:
      ax25_encoder_bf_impl (ax25_frame_type_t type, std::string dest_addr,
			    uint8_t dest_ssid, std::string src_addr,
			    uint8_t src_ssid);
      ~ax25_encoder_bf_impl();

      void
      set_address_field (std::string dest_addr, uint8_t dest_ssid,
			 std::string src_addr, uint8_t src_ssid);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_ENCODER_BF_IMPL_H */

