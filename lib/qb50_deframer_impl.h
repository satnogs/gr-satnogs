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

#ifndef INCLUDED_SATNOGS_QB50_DEFRAMER_IMPL_H
#define INCLUDED_SATNOGS_QB50_DEFRAMER_IMPL_H

#include <satnogs/qb50_deframer.h>

namespace gr
{
  namespace satnogs
  {

    class qb50_deframer_impl : public qb50_deframer
    {
    private:
      const uint8_t d_wod_ssid;

      void msg_handler(pmt::pmt_t msg);

    public:
      qb50_deframer_impl (uint8_t wod_ssid);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_QB50_DEFRAMER_IMPL_H */

