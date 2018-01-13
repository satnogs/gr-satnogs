/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <http://librespacefoundation.org/>
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


#ifndef INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_H
#define INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief The goal of this block is to act as a valve for a signal
     * commonly originating from a clock recovered quadrature demodulated stream.
     *
     * A very common problem in such cases, is that after the quadrature
     * demodulation the signal is on the phase domain and the detection
     * of signal presence or absence cannot be performed with energy detection.
     * Why do we need to identify signal presence? Most of the amateur satellites
     * uses quite poor and short training sequences, especially in the AX.25
     * AFSK1200 mode. Thus, the decoders have too many false alarms and due to
     * this, they miss frames.
     *
     * Some may argue that you can perform energy detection before the quadrature
     * demodulation, but this implies a 'magic' threshold should be set.
     * In the SatNOGS case, this is not possible. Every ground station is unique
     * in terms of performance and RF characteristics. On the other hand the
     * phase domain has the nice property that the signal level is known. This
     * level can be +-pi. This block tries to find an SNR-like metric based
     * on the variance and the mean of the signal.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API quad_demod_filter_ff : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<quad_demod_filter_ff> sptr;


      /**
       * Creates a Quadrature Demodulate filter. The block will output samples
       * only in signal presence acting as a valve to reduce false alarms of
       * the FSK decoders.
       *
       * @param gain this MUST be the gain set on the quadrature demodulate
       * gain or an appropriate value if the amplitude of the quadrature
       * demodulated signal somehow is scaled. By default if the signal is
       * on the +-pi region, the gain should be one. If it is on the +-2pi, the
       * gain should be set to 2.0 etc.
       *
       * @param window the window size to calculate the moving average and
       * variance.
       *
       * @return shared pointer of the block
       */
      static sptr make(float gain, int window = 80);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_QUAD_DEMOD_FILTER_FF_H */

