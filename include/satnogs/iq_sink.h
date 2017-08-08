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

#ifndef INCLUDED_SATNOGS_IQ_SINK_H
#define INCLUDED_SATNOGS_IQ_SINK_H

#include <satnogs/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/blocks/file_sink_base.h>

namespace gr
{
  namespace satnogs
  {

  /*!
   * \brief This block converts a complex float input stream to short and stores 
   * it to a file. If the value of status argument is zero the block behaves
   * as a null sink block.
   *
   * \ingroup satnogs
   *
   */
  class SATNOGS_API iq_sink : virtual public gr::sync_block,
  virtual public gr::blocks::file_sink_base
  {
  public:
    typedef boost::shared_ptr<iq_sink> sptr;

    /**
     * This block converts a complex float input stream to short and stores 
     * it to a file. If the value of status argument is zero the block behaves
     * as a null sink block.
     *
     * @param scale the value multiplied against each point in the input stream
     * @param filename name of the file to open and write output to.
     * @param append if true, data is appended to the file instead of
     *        overwriting the initial content.
     * @param status the status of the block.
     * - 0: Block acts as a null sink
     * - 1: Active
     *
     * @return a shared_ptr to a new instance of satnogs::iq_sink.
     */
    static sptr make(const float scale,
        const char *filename, bool append=false,
        const int status = 0);
  };

}
 // namespace satnogs
}// namespace gr

#endif /* INCLUDED_SATNOGS_IQ_SINK_H */

