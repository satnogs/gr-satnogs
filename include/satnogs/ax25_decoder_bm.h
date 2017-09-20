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

#ifndef INCLUDED_SATNOGS_AX25_DECODER_BM_H
#define INCLUDED_SATNOGS_AX25_DECODER_BM_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief AX.25 decoder that supports the legacy hardware radios.
     *
     * This block takes as input a quadrature demodulated bit stream.
     * Each byte should contains only one bit of information at the LSB.
     *
     * The block will try to find an AX.25 frame. If the frame pass the
     * CRC check then a blob PMT message is produced at the message output
     * indicated with name 'out'. Otherwise if the frame did not pass the
     * CRC check or the size was invalid, a blob PMT message is generated at
     * the output port with the name 'fail'. This will help to recover at least
     * some bytes from a corrupted message.
     *
     * The block also supports destination callsign check. Only frames with
     * the right destination Callsign will be accepted. This feature can be
     * disabled using the promisc parameter.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ax25_decoder_bm : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<ax25_decoder_bm> sptr;

      /**
       * This block takes as input a quadrature demodulated bit stream.
       * Each byte should contains only one bit of information at the LSB.
       *
       * The block will try to find an AX.25 frame. If the frame pass the
       * CRC check then a blob PMT message is produced at the message output
       * indicated with name 'pdu'. Otherwise if the frame did not pass the
       * CRC check or the size was invalid, a blob PMT message is generated at
       * the output port with the name 'failed_pdu'.
       * This will help to recover at least some bytes from a corrupted message.
       *
       * The block also supports destination callsign check. Only frames with
       * the right destination Callsign will be accepted. This feature can be
       * disabled using the promisc parameter.
       * @param addr the Callsign of the receiver
       * @param ssid the SSID of the receiver
       * @param promisc if set to yes, the Callsign check is disabled
       * @param descramble if set to yes, the data will be descrambled prior
       * decoding using the G3RUH self-synchronizing descrambler.
       * @param max_frame_len the maximum allowed frame length
       * @param n_sync_flags the number of the leading AX.25 SYNC flag
       * repetitions. Normally the SYNC flag is repeated multiple times
       * prior the payload transmission. Increasing this parameter
       * reduces significantly the false alarms
       * @return
       */
      static sptr
      make (const std::string& addr, uint8_t ssid, bool promisc = false,
	    bool descramble = true,
	    size_t max_frame_len = 512,
	    size_t n_sync_flags = 2);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AX25_DECODER_BM_H */

