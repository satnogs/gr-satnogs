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

#ifndef INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_H
#define INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block takes samples after the Clock Recovery block
     * and tries to extract frames. This is performed by searching for a
     * known preamble. For the byte synchronization the block tries to
     * search for the specified synchronization word that should be present
     * before the start of the payload.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API upsat_fsk_frame_acquisition : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<upsat_fsk_frame_acquisition> sptr;

      /*!
       * Creates the FSK frame acquisition block for the UPSAT satellite.
       * @param preamble the bytes that consist the preamble of the frame
       *
       * @param sync_word the byte synchronization word
       *
       * @param whitening true if the transmitted data have been processed by
       * the whitening algorithm of the CC1120 chip. False otherwise.
       *
       * @param manchester true if the transmitted data have been processed by
       * the Manchester algorithm of the CC1120 chip. False otherwise.
       *
       * @param check_crc if set to true the decoder will push frames only if
       * their CRC field in correct.
       *
       * @param ax25_format if set to true the frame contains an AX.25
       * encoded payload. Prior producing the payload, AX.25 decoding
       * will be performed. If set to false, the payload will be pushed
       * as it is.
       */
      static sptr
      make (const std::vector<uint8_t> &preamble,
	    const std::vector<uint8_t> &sync_word, bool whitening = false,
	    bool manchester = false, bool check_crc = true,
	    bool ax25_format = false);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_H */

