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

#ifndef INCLUDED_SATNOGS_AX25_ENCODER_MB_H
#define INCLUDED_SATNOGS_AX25_ENCODER_MB_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief AX.25 encoder block that supports the legacy hardware radios.
     *
     * The block takes as inputs blob PMT messages and generates a byte stream.
     * Each output byte contains only one LSB, thus the output can be directly
     * used for FM modulation.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ax25_encoder_mb : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<ax25_encoder_mb> sptr;


      /**
       * AX.25 encoder block that supports the legacy hardware radios.
       *
       * The block takes as inputs blob PMT messages and generates a byte stream.
       * Each output byte contains only one LSB, thus the output can be directly
       * used for FM modulation.
       *
       * @param dest_addr the destination callsign
       * @param dest_ssid the destination SSID
       * @param src_addr the source callsign
       * @param src_ssid the source SSID
       * @param preamble_len the number of times that the AX.25 synchronization flags
       * should be repeated in front of the frame.
       * @param postamble_len the number of times that the AX.25 synchronization flags
       * should be repeated at the end of the frame.
       * @param scramble if set to true, G3RUH scrambling will be performed
       * after bit stuffing
       */
      static sptr
      make (const std::string& dest_addr, uint8_t dest_ssid,
	    const std::string& src_addr,
	    uint8_t src_ssid, size_t preamble_len =16,
	    size_t postamble_len = 16,
	    bool scramble = true);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_ENCODER_MB_H */

