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

#ifndef INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_IMPL_H
#define INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_IMPL_H

#include <satnogs/upsat_fsk_frame_acquisition.h>

namespace gr
{
  namespace satnogs
  {

    class upsat_fsk_frame_acquisition_impl : public upsat_fsk_frame_acquisition
    {
    private:
      // Nothing to declare in this block.

    public:
      upsat_fsk_frame_acquisition_impl (const std::vector<uint8_t> &preamble,
					const std::vector<uint8_t> &sync_word,
					bool whitening, bool manchester);
      ~upsat_fsk_frame_acquisition_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_UPSAT_FSK_FRAME_ACQUISITION_IMPL_H */

