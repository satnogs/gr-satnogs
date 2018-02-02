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

#ifndef INCLUDED_SATNOGS_FOX_TELEM_MM_IMPL_H
#define INCLUDED_SATNOGS_FOX_TELEM_MM_IMPL_H

#include <satnogs/fox_telem_mm.h>

namespace gr
{
  namespace satnogs
  {

    class fox_telem_mm_impl : public fox_telem_mm
    {
    private:
      const size_t              d_frame_size;

      void
      process_frame (pmt::pmt_t msg);

    public:
      static const uint8_t      fox_id[];

      fox_telem_mm_impl ();
      ~fox_telem_mm_impl ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_FOX_TELEM_MM_IMPL_H */

