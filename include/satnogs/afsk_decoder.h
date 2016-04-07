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

#ifndef INCLUDED_SATNOGS_AFSK_DECODER_H
#define INCLUDED_SATNOGS_AFSK_DECODER_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief AFSK decoder. This particular decoder uses a Frequency Domain
     * approach to retrieve the message.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API afsk_decoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<afsk_decoder> sptr;


      /*!
       * An AFSK decoder.
       * @param sampling_rate the input sampling rate
       * @param baudrate the baudrate (symbols per second)
       * @param fft_size the FFT size that the decoder will use.
       * @param auto_carrier_tracking if set to yes, the decoder will try
       * to automatically find the mark and space frequencies
       * @param mark_freq the frequency of the mark (1)
       * @param mark_space the frequency of the space (0)
       */
      static sptr make(double sampling_rate, size_t baudrate = 1200,
		       size_t fft_size = 128,
		       bool auto_carrier_tracking = false,
		       double mark_freq = 2400.0,
		       double space_freq = 1200.0);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_AFSK_DECODER_H */

