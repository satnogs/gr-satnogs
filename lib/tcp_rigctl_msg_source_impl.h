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

#ifndef INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_IMPL_H
#define INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_IMPL_H

#include <satnogs/tcp_rigctl_msg_source.h>

namespace gr
{
  namespace satnogs
  {

    class tcp_rigctl_msg_source_impl : public tcp_rigctl_msg_source
    {
    private:
      const std::string d_iface_addr;
      const uint16_t d_port;
      const size_t d_mtu;
      bool d_running;
      boost::shared_ptr<boost::thread> d_thread;

      void
      tcp_msg_accepter();

      double
      get_freq_from_buf(const uint8_t *buf);

    public:
      tcp_rigctl_msg_source_impl (const std::string& addr, uint16_t port,
				  size_t mtu);
      ~tcp_rigctl_msg_source_impl ();
    };

    } // namespace satnogs
  } // namespace gr

#endif /* INCLUDED_SATNOGS_TCP_RIGCTL_MSG_SOURCE_IMPL_H */

