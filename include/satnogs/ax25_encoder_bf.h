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

#ifndef INCLUDED_SATNOGS_AX25_ENCODER_BF_H
#define INCLUDED_SATNOGS_AX25_ENCODER_BF_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>
#include <satnogs/ax25.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief AX.25 packet encoder
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ax25_encoder_bf : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ax25_encoder_bf> sptr;


      /**
       * AX.25 packet encoder. This block receives messages containing
       * the packet information, contracts a AX.25 packet end encodes it
       * using NRZ.
       *
       * The block also creates suitable burst tags for proper
       * burst transmission in SDR devices that support them.
       *
       * @param dest_addr the destination callsign address
       * @param dest_ssid the destination SSID
       * @param src_addr the callsign of the source
       * @param src_ssid the source SSID
       */
      static sptr make(std::string dest_addr,
		       uint8_t dest_ssid,
		       std::string src_addr,
		       uint8_t src_ssid);

      virtual void
      set_address_field (std::string dest_addr, uint8_t dest_ssid,
			 std::string src_addr, uint8_t src_ssid) = 0;
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_ENCODER_BF_H */

