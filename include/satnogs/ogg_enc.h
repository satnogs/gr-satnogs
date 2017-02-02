/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_OGG_ENCODER_H
#define INCLUDED_SATNOGS_OGG_ENCODER_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief <+description of block+>
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ogg_encoder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ogg_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of satnogs::ogg_encoder.
       *
       * To avoid accidental use of raw pointers, satnogs::ogg_encoder's
       * constructor is in a private implementation
       * class. satnogs::ogg_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(char* filename, double samp_rate, float quality);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_OGG_ENCODER_H */
