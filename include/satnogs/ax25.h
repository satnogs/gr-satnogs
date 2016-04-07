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

#ifndef INCLUDE_SATNOGS_AX25_H_
#define INCLUDE_SATNOGS_AX25_H_

#include <satnogs/utils.h>

namespace gr
{

  namespace satnogs
  {
    const size_t AX25_MIN_ADDR_LEN  = 14;
    const size_t AX25_MAX_ADDR_LEN  = 28;
    const size_t AX25_MIN_CTRL_LEN  = 1;
    const size_t AX25_MAX_CTRL_LEN  = 2;
    const size_t AX25_MAX_FRAME_LEN = 256;
    const uint8_t AX25_SYNC_FLAG = 0x7E;
    const uint8_t AX25_CALLSIGN_MAX_LEN = 6;
    const float AX25_SYNC_FLAG_MAP[8] = {-1, 1, 1, 1, 1, 1, 1, -1};
    /**
     * AX.25 Frame types
     */
    typedef enum
    {
      AX25_I_FRAME, //!< AX25_I_FRAME Information frame
      AX25_S_FRAME, //!< AX25_S_FRAME Supervisory frame
      AX25_U_FRAME //!< AX25_U_FRAME Unnumbered frame
    } ax25_frame_type_t;

    typedef enum
    {
      AX25_ENC_FAIL,
      AX25_ENC_OK
    } ax25_encode_status_t;

    typedef struct
    {
      uint8_t address[AX25_MAX_ADDR_LEN];
      size_t address_len;
      uint16_t ctrl;
      size_t ctrl_len;
      uint8_t pid;
      uint8_t info[AX25_MAX_FRAME_LEN];
      ax25_frame_type_t type;
    } ax25_frame_t;

    /**
     * Calculates the FCS of the AX25 frame
     * @param buffer data buffer
     * @param len size of the buffer
     * @return the FCS of the buffer
     */
    static inline uint16_t
    ax25_fcs (uint8_t *buffer, size_t len)
    {
      uint16_t fcs = 0xFFFF;
      while (len--) {
	fcs = (fcs >> 8) ^ crc16_ccitt_table_reverse[(fcs ^ *buffer++) & 0XFF];
      }
      return fcs ^ 0xFFFF;
    }

    /**
     * Createst the header field of the AX.25 frame
     * @param out the output buffer with enough memory to hold the address field
     * @param dest_addr the destination callsign address
     * @param dest_ssid the destination SSID
     * @param src_addr the callsign of the source
     * @param src_ssid the source SSID
     */
    static inline size_t
    ax25_create_addr_field (uint8_t *out, std::string dest_addr,
			    uint8_t dest_ssid, std::string src_addr,
			    uint8_t src_ssid)
    {
      size_t i;

      for(i = 0; i < dest_addr.length(); i++) {
	*out++ = dest_addr[i] << 1;
      }
      /*
       * Perhaps the destination callsign was smaller that the maximum allowed.
       * In this case the leftover bytes should be filled with space
       */
      for(; i < AX25_CALLSIGN_MAX_LEN; i++){
	*out++ = ' ' << 1;
      }
      /* Apply SSID, reserved and C bit */
      /* FIXME: C bit is set to 0 implicitly */
      *out++ = ((0b1111 & dest_ssid) << 1) | 0b01100000;

      for(i = 0; i < src_addr.length(); i++) {
	*out++ = dest_addr[i] << 1;
      }
      for(; i < AX25_CALLSIGN_MAX_LEN; i++){
	*out++ = ' ' << 1;
      }
      /* Apply SSID, reserved and C bit. As this is the last address field
       * the trailing bit is set to 1.
       * /
      /* FIXME: C bit is set to 0 implicitly */
      *out++ = ((0b1111 & dest_ssid) << 1) | 0b01100001;
      return AX25_MIN_ADDR_LEN;
    }

    static inline size_t
    ax25_prepare_frame (uint8_t *out, const uint8_t *info, size_t info_len,
			ax25_frame_type_t type, uint8_t *addr, size_t addr_len,
			uint16_t ctrl, size_t ctrl_len)
    {
      uint16_t fcs;
      size_t i = 1;
      if(info_len > AX25_MAX_FRAME_LEN) {
	return 0;
      }

      out[0] = AX25_SYNC_FLAG;
      /* Insert address and control fields */
      if( addr_len == AX25_MIN_ADDR_LEN || addr_len == AX25_MAX_ADDR_LEN){
	memcpy(out + i, addr, addr_len);
	i += addr_len;
      }
      else{
	return 0;
      }

      if( ctrl_len == AX25_MIN_CTRL_LEN || ctrl_len == AX25_MAX_CTRL_LEN){
	memcpy(out + i, &ctrl, ctrl_len);
	i += addr_len;
      }
      else{
	return 0;
      }

      /*
       * Set the PID depending the frame type.
       * FIXME: For now, only the "No layer 3 is implemented" information is
       * inserted
       */
      if(type == AX25_I_FRAME){
	out[i++] = 0xF0;
      }
      memcpy(out + i, info, info_len);
      i += info_len;

      /* Compute the FCS. Ignore the first flag byte */
      fcs = ax25_fcs(out + 1, i - 1);
      /* The MS bits are sent first ONLY at the FCS field */
      out[i++] = (fcs >> 8) & 0xFF;
      out[i++] = fcs & 0xFF;
      out[i++] = AX25_SYNC_FLAG;

      return i;
    }

    static inline ax25_encode_status_t
    ax25_nrz_encode(float *out, size_t *out_len,
		    const uint8_t *buffer, const size_t buffer_len)
    {
      uint8_t bit;
      uint8_t prev_bit = 0;
      size_t out_idx = 0;
      size_t bit_idx;
      size_t cont_1 = 0;
      size_t total_cont_1 = 0;
      size_t i;

      /* Leading FLAG field does not need bit stuffing */
      memcpy(out, AX25_SYNC_FLAG_MAP, 8 * sizeof(float));
      out_idx = 8;

      /* Skip the leading and trailing FLAG field */
      buffer++;
      for(i = 0; i < 8 * (buffer_len - 2); i++){
	bit = (buffer[i / 8] >> ( i % 8)) & 0x1;
	out[out_idx++] = bit ? 1.0 : -1.0;

	/* Check if bit stuffing should be applied */
	if(bit & prev_bit){
	  cont_1++;
	  total_cont_1++;
	  if(cont_1 == 4){
	    out[out_idx++] = -1.0;
	    cont_1 = 0;
	  }
	}
	else{
	  cont_1 = total_cont_1 = 0;
	}
	prev_bit = bit;

	/*
	 * If the total number of continuous 1's is 15 the the frame should be
	 * dropped
	 */
	if(total_cont_1 >= 14) {
	  return AX25_ENC_FAIL;
	}
      }

      /* Trailing FLAG field does not need bit stuffing */
      memcpy(out + out_idx, AX25_SYNC_FLAG_MAP, 8 * sizeof(float));
      out_idx += 8;

      *out_len = out_idx;
      return AX25_ENC_OK;
    }

  }  // namespace satnogs

}  // namespace gr



#endif /* INCLUDE_SATNOGS_AX25_H_ */
