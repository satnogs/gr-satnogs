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

#ifndef INCLUDED_SATNOGS_CLEAR_TEXT_MSG_SINK_H
#define INCLUDED_SATNOGS_CLEAR_TEXT_MSG_SINK_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * \brief Block accepting clear text messages from various decoders.
     * Its purpose is to forward these messages at other services, programs,
     * stdout, etc,
     *
     * \ingroup satnogs
     *
     */
    class SATNOGS_API clear_text_msg_sink : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<clear_text_msg_sink> sptr;

      /*!
       * \brief Block accepting clear text messages from various decoders.
       * Its purpose is to forward these messages at other services, programs,
       * stdout, etc,
       *
       */
      static sptr
      make ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_CLEAR_TEXT_MSG_SINK_H */

