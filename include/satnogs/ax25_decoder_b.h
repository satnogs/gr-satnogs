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

#ifndef INCLUDED_SATNOGS_AX25_DECODER_B_H
#define INCLUDED_SATNOGS_AX25_DECODER_B_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief AX.25 decoding block
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ax25_decoder_b : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ax25_decoder_b> sptr;

      /**
       * AX.25 decoder block.
       *
       * @param addr the Callsign of the receiver
       * @param ssid the SSID of the receiver
       * @param promisc if set to true, all the successfully decoded frames
       * are forwarded. Otherwise, only those that have as destination address
       * and SSID specified by the addr and ssid parameters respectively.
       */
      static sptr make(std::string addr, uint8_t ssid, bool promisc=true);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_DECODER_B_H */

