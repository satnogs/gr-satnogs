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
       * @param filename the OGG audio file path
       * @param number of channels of the OGG stream. If the actual OGG stream
       * contains a different number of channels than specified an exception
       * is raised
       * @param repeat if set to true, when EOF is reached the block
       * will continue to output samples from the beginning of the OGG file.
       */
      static sptr
      make (const std::string& filename, size_t channels = 1,
            bool repeat = false);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_OGG_SOURCE_H */

