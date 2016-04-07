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

#ifndef INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_H
#define INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block implements a FSK frame encoder for the UPSAT satellite.
     * It takes as input a message containing the PDU and performs the NRZ
     * encoding. The resulting float samples can be passed from a FM modulation
     * block and then to the SDR device.
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API upsat_fsk_frame_encoder : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<upsat_fsk_frame_encoder> sptr;

      /*!
       * Creates an FSK encoding block. Note that this block does NOT perform
       * the frequency modulation. You can use the existing frequency modulation
       * block shipped with the GNU Radio.
       *
       * @param preamble the bytes that consist the preamble of the frame
       *
       * @param sync_word the byte synchronization word
       *
       * @param append_crc if set to true the encoder will append a two byte
       * CRC field at the end of the frame. The CRC algorithm is compatible
       * with the CC1120 chip.
       *
       * @param whitening true if the transmitted data have been processed by
       * the whitening algorithm of the CC1120 chip. False otherwise.
       *
       * @param manchester true if the transmitted data have been processed by
       * the Manchester algorithm of the CC1120 chip. False otherwise.
       */
      static sptr
      make (const std::vector<uint8_t>& preamble,
	    const std::vector<uint8_t>& sync_word, bool append_crc = true,
	    bool whitening = false, bool manchester = false);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ENCODER_H */

