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

#ifndef INCLUDED_SATNOGS_DECODER_8B10B_H
#define INCLUDED_SATNOGS_DECODER_8B10B_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

  /*!
   * \brief <+description of block+>
   * \ingroup satnogs
   *
   */
  class SATNOGS_API decoder_8b10b : virtual public gr::sync_block
  {
  public:
    typedef boost::shared_ptr<decoder_8b10b> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of satnogs::decoder_8b10b.
     *
     * To avoid accidental use of raw pointers, satnogs::decoder_8b10b's
     * constructor is in a private implementation
     * class. satnogs::decoder_8b10b::make is the public interface for
     * creating new instances.
     */
    static sptr make(const std::string& control_symbol,size_t max_frame_len);
  };

}
 // namespace satnogs
}// namespace gr

#endif /* INCLUDED_SATNOGS_DECODER_8B10B_H */

