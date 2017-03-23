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

#ifndef INCLUDED_SATNOGS_OGG_SOURCE_H
#define INCLUDED_SATNOGS_OGG_SOURCE_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief OGG source block. Reads a file with an OGG audio and
     * convert it to float samples
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API ogg_source : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<ogg_source> sptr;

      /*!
       *
       * @param samp_rate the sampling rate
       * @param filename the OGG audio file path
       */
      static sptr
      make (double samp_rate, const std::string& filename);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_OGG_SOURCE_H */

