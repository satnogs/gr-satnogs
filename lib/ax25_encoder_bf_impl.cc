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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ax25_encoder_bf_impl.h"
#include <satnogs/log.h>

namespace gr
{
  namespace satnogs
  {

    ax25_encoder_bf::sptr
    ax25_encoder_bf::make (std::string dest_addr,
			   uint8_t dest_ssid, std::string src_addr,
			   uint8_t src_ssid)
    {
      return gnuradio::get_initial_sptr (new ax25_encoder_bf_impl (AX25_I_FRAME,
								   dest_addr,
								   dest_ssid,
								   src_addr,
								   src_ssid));
    }

    /*
     * The private constructor
     */
    ax25_encoder_bf_impl::ax25_encoder_bf_impl (ax25_frame_type_t type,
						std::string dest_addr,
						uint8_t dest_ssid,
						std::string src_addr,
						uint8_t src_ssid) :
	gr::sync_block ("ax25_encoder_bf", gr::io_signature::make (0, 0, 0),
			gr::io_signature::make (1, 1, sizeof(float))),
			d_type (type),
			d_remaining (0),
			d_produced(0),
			d_endoded_frame(new float[(AX25_MAX_FRAME_LEN * 2)]),
			d_tmp_buf(new uint8_t[AX25_MAX_FRAME_LEN * 2]),
			d_addr_field(new uint8_t[AX25_MAX_ADDR_LEN])
    {
      /* Input is a key-value pair containing the info field data */
      message_port_register_in (pmt::mp ("info"));
      d_addr_len = ax25_create_addr_field(d_addr_field, dest_addr, dest_ssid,
					  src_addr, src_ssid);
    }

    /*
     * Our virtual destructor.
     */
    ax25_encoder_bf_impl::~ax25_encoder_bf_impl ()
    {
      delete[] d_endoded_frame;
      delete[] d_tmp_buf;
      delete[] d_addr_field;
    }

    int
    ax25_encoder_bf_impl::work (int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const uint8_t *info;
      size_t info_len;
      size_t max_avail;
      size_t len;
      size_t i;
      pmt::pmt_t msg;
      ax25_encode_status_t status;

      float *out = (float *) output_items[0];

      /* If all the frame samples have already be sent, wait for a new frame */
      if (d_remaining == 0) {
	boost::mutex::scoped_lock lock (d_mutex);
	d_produced = 0;

	/* Block waiting from a new message from users */
	msg = delete_head_blocking (pmt::mp ("info"), 0);
	info = (const uint8_t *)pmt::blob_data(msg);
	info_len = pmt::blob_length(msg);

	/* Prepare and encode the AX.25 frame */
	len = ax25_prepare_frame (d_tmp_buf, info, info_len, AX25_I_FRAME,
				  d_addr_field, d_addr_len, 0, 1);
	status = ax25_nrz_encode(d_endoded_frame, &d_remaining, d_tmp_buf,
				 len);
	if(status != AX25_ENC_OK){
	  LOG_ERROR("NRZ Encoding failed");
	  d_remaining = 0;
	  return 0;
	}
      }

      /* If this is the first part of the frame add the start of burst tag*/
      if(d_produced == 0) {
	add_sob(nitems_written(0));
      }
      max_avail = std::min(d_remaining, (size_t) noutput_items);
      memcpy(out, d_endoded_frame + d_produced, max_avail * sizeof(float));
      d_remaining -= max_avail;
      d_produced += max_avail;
      if(d_remaining == 0){
	add_eob(nitems_written(0) + max_avail);
      }
      return (int) max_avail;
    }

    /**
     * Updates the source and destination callsigns and SSIDs
     * @param dest_addr the destination callsign address
     * @param dest_ssid the destination SSID
     * @param src_addr the callsign of the source
     * @param src_ssid the source SSID
     */
    void
    ax25_encoder_bf_impl::set_address_field (std::string dest_addr,
					     uint8_t dest_ssid,
					     std::string src_addr,
					     uint8_t src_ssid)
    {
      boost::mutex::scoped_lock lock (d_mutex);
      d_addr_len = ax25_create_addr_field(d_addr_field, dest_addr, dest_ssid,
					  src_addr, src_ssid);
    }

    void
    ax25_encoder_bf_impl::add_sob (uint64_t item)
    {
      static const pmt::pmt_t sob_key = pmt::string_to_symbol ("tx_sob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, sob_key, value, srcid);
    }

    void
    ax25_encoder_bf_impl::add_eob (uint64_t item)
    {
      static const pmt::pmt_t eob_key = pmt::string_to_symbol ("tx_eob");
      static const pmt::pmt_t value = pmt::PMT_T;
      static const pmt::pmt_t srcid = pmt::string_to_symbol (alias ());
      add_item_tag (0, item, eob_key, value, srcid);
    }

  } /* namespace satnogs */
} /* namespace gr */

