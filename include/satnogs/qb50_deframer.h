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

#ifndef INCLUDED_SATNOGS_QB50_DEFRAMER_H
#define INCLUDED_SATNOGS_QB50_DEFRAMER_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Parses the received AX.25 and separates the
     * telecommand and control frames from the WOD frames.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API qb50_deframer : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<qb50_deframer> sptr;

      /*!
       * \brief Parses the received AX.25 and separates the
       * telecommand and control frames from the WOD frames.
       *
       * @param wod_ssid the SSID that separates the WOD frames from the others
       */
      static sptr
      make (uint8_t wod_ssid);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_QB50_DEFRAMER_H */

