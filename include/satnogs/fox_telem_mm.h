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

#ifndef INCLUDED_SATNOGS_FOX_TELEM_MM_H
#define INCLUDED_SATNOGS_FOX_TELEM_MM_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief This block takes a AMSAT FOX telemetry frame from the CCSDS
     * RS decoder, discards the pad bytes, checks if the frame is originating
     * from a valid spacecraft and outputs the data to another message port.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API fox_telem_mm : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<fox_telem_mm> sptr;

      /*!
       * \brief This block takes a AMSAT FOX telemetry frame from the CCSDS
       * RS decoder, discards the pad bytes, checks if the frame is originating
       * from a valid spacecraft and outputs the data to another message port.
       */
      static sptr
      make ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FOX_TELEM_MM_H */

