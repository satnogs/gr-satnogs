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

#ifndef INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_H
#define INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief A block for debug reasons producing specific messages.
     * The input message can be anything, opposed to the \p debug_msg_source()
     * block that can accept only string messages.
     * \ingroup satnogs
     *
     */
    class SATNOGS_API debug_msg_source_raw : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<debug_msg_source_raw> sptr;

      /**
       * Debug message source block.
       * @param msg the message
       * @param delay delay in seconds between consecutive messages
       * @param repeat if set to yes the block will produce a message every
       * \p delay seconds
       */
      static sptr
      make (const std::vector<uint8_t> &msg, double delay, bool repeat);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DEBUG_MSG_SOURCE_RAW_H */

