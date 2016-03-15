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
#include <satnogs/tc_tm.h>

namespace gr
{
  namespace satnogs
  {

    uint8_t
    checkSum (const uint8_t *data, uint16_t size)
    {
      uint8_t CRC = 0;
      for (int i = 0; i <= size; i++) {
	CRC = CRC ^ data[i];
      }
      return CRC;
    }

    /*Must check for endianess*/
    OBC_ret_state_t
    unpack_pkt (const uint8_t *buf, tc_tm_pkt *pkt, const uint16_t size)
    {
      union _cnv cnv;
      uint8_t tmp_crc[2];

      uint8_t ver, dfield_hdr, ccsds_sec_hdr, tc_pus;

      tmp_crc[0] = buf[size - 1];
      tmp_crc[1] = checkSum (buf, size - 2);

      ver = buf[0] >> 5;

      pkt->type = (buf[0] >> 4) & 0x01;
      dfield_hdr = (buf[0] >> 3) & 0x01;

      cnv.cnv8[0] = buf[1];
      cnv.cnv8[1] = 0x07 & buf[0];
      pkt->app_id = cnv.cnv16[0];

      pkt->seq_flags = buf[2] >> 6;

      cnv.cnv8[0] = buf[3];
      cnv.cnv8[1] = buf[2] & 0x3F;
      pkt->seq_count = cnv.cnv16[0];

      cnv.cnv8[0] = buf[4];
      cnv.cnv8[1] = buf[5];
      pkt->len = cnv.cnv16[0];

      ccsds_sec_hdr = buf[6] >> 7;

      tc_pus = buf[6] >> 4;

      pkt->ack = 0x04 & buf[6];

      pkt->ser_type = buf[7];
      pkt->ser_subtype = buf[8];
      pkt->dest_id = buf[9];

      if (app_id_verification[pkt->app_id] != 1) {
	return R_OBC_PKT_ILLEGAL_APPID;
      }

      if (pkt->len != size - 7) {
	return R_OBC_PKT_INV_LEN;
      }

      if (tmp_crc[0] != tmp_crc[1]) {
	return R_OBC_PKT_INC_CRC;
      }

      if (services_verification_TC_TM[pkt->ser_type][pkt->ser_subtype][pkt->type]
	  != 1) {
	return R_OBC_PKT_ILLEGAL_PKT_TP;
      }

      if (ver != 0) {
	return R_OBC_ERROR;
      }

      if (tc_pus != 1) {
	return R_OBC_ERROR;
      }

      if (ccsds_sec_hdr != 0) {
	return R_OBC_ERROR;
      }

      if (pkt->type != TC && pkt->type != TM) {
	return R_OBC_ERROR;
      }

      if (dfield_hdr != 1) {
	return R_OBC_ERROR;
      }

      if (pkt->ack != TC_ACK_NO || pkt->ack != TC_ACK_ACC
	  || pkt->ack != TC_ACK_EXE_COMP) {
	return R_OBC_ERROR;
      }

      for (int i = 0; i < pkt->len - 4; i++) {
	pkt->data[i] = buf[10 + i];
      }

      return R_OBC_OK;
    }

    /**
     * Packs a TC packet into a byte buffer
     * @param buf buffer to store the data to be sent
     * @param pkt the data to be stored in the buffer
     * @param size size of the array
     * @return appropriate error code or R_OBC_OK if all operation succeed
     */
    OBC_ret_state_t
    pack_pkt (uint8_t *buf, tc_tm_pkt *pkt, uint16_t *size)
    {

      union _cnv cnv;
      uint8_t buf_pointer;

      cnv.cnv16[0] = pkt->app_id;

      buf[0] = ( ECSS_VER_NUMBER << 5 | pkt->type << 4
	  | ECSS_DATA_FIELD_HDR_FLG << 3 | cnv.cnv8[1]);
      buf[1] = cnv.cnv8[0];

      cnv.cnv16[0] = pkt->seq_count;
      buf[2] = (pkt->seq_flags << 6 | cnv.cnv8[1]);
      buf[3] = cnv.cnv8[0];

      /* TYPE = 0 TM, TYPE = 1 TC*/
      if (pkt->type == TM) {
	buf[6] = ECSS_PUS_VER << 4;
      }
      else if (pkt->type == TC) {
	buf[6] = ( ECSS_SEC_HDR_FIELD_FLG << 7 | ECSS_PUS_VER << 4 | pkt->ack);
      }
      else {
	return R_OBC_ERROR;
      }

      buf[7] = pkt->ser_type;
      buf[8] = pkt->ser_subtype;
      buf[9] = pkt->dest_id; /*source or destination*/

      buf_pointer = 10;

      if (pkt->ser_type == TC_VERIFICATION_SERVICE) {
	//cnv.cnv16[0] = tc_pkt_id;
	//cnv.cnv16[1] = tc_pkt_seq_ctrl;

	/*verification_pack_pkt_api (buf, pkt, &buf_pointer);*/

      }
      else if (pkt->ser_type == TC_HOUSEKEEPING_SERVICE) {

	/*hk_pack_pkt_api (buf, pkt, &buf_pointer);*/

      }
      else if (pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE
	  && pkt->ser_subtype == 1) {

	buf[10] = pkt->data[0];

	buf[11] = pkt->data[1];
	buf[12] = pkt->data[2];
	buf[13] = pkt->data[3];
	buf[14] = pkt->data[4];

	buf_pointer += 5;

      }
      else {
	return R_OBC_ERROR;
      }

      /*check if this is correct*/
      cnv.cnv16[0] = buf_pointer - 6;
      buf[4] = cnv.cnv8[0];
      buf[5] = cnv.cnv8[1];

      buf[buf_pointer] = checkSum (buf, buf_pointer - 1);
      *size = buf_pointer;
      return R_OBC_OK;
    }

    OBC_ret_state_t
    crt_pkt (tc_tm_pkt *pkt, uint16_t app_id, uint8_t type, uint8_t ack,
	     uint8_t ser_type, uint8_t ser_subtype, uint16_t dest_id)
    {

      pkt->type = type;
      pkt->app_id = app_id;
      pkt->dest_id = dest_id;

      pkt->ser_type = ser_type;
      pkt->ser_subtype = ser_subtype;

      return R_OBC_OK;
    }

  } /* namespace satnogs */
} /* namespace gr */

