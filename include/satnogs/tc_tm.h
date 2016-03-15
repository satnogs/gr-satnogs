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

#ifndef INCLUDED_SATNOGS_TC_TM_H
#define INCLUDED_SATNOGS_TC_TM_H

#include <satnogs/api.h>
#include <stdint.h>

/* TM TC services*/
#define ECSS_VER_NUMBER             0
#define ECSS_DATA_FIELD_HDR_FLG     1
#define TC_TM_SER_TC_VER            1

#define ECSS_PUS_VER            1
#define ECSS_SEC_HDR_FIELD_FLG  0
/* Minimal */
#define TC_TM_SER_TC_VER_ACC_SUCC           1
#define TC_TM_SER_TC_VER_ACC_FAIL           2
/* Additional */
#define TC_TM_SER_TC_VER_EXEC_START_SUCC    3
#define TC_TM_SER_TC_VER_EXEC_START_FAIL    4
#define TC_TM_SER_TC_VER_EXEC_PROG_SUCC     5
#define TC_TM_SER_TC_VER_EXEC_PROG_FAIL     6
#define TC_TM_SER_TC_VER_EXEC_COMP_SUCC     7
#define TC_TM_SER_TC_VER_EXEC_COMP_FAIL     8

#define TC_TM_SER_DIST      2
#define TC_TM_SER_HK        3

/* Minimal */
#define TC_TM_SER_HK_PAR    25
/* Additional */

#define TC_TM_SER_STAT      4
#define TC_TM_SER_EVENT     5
#define TC_TM_SER_MEM       6
#define TC_TM_SER_NC1       7
#define TC_TM_SER_FMAG      8
#define TC_TM_SER_TMAG      9
#define TC_TM_SER_NC2       10

#define TC_TM_SER_SCH       11
#define TC_TM_SER_MON       12
#define TC_TM_SER_LDATA     13
#define TC_TM_SER_PCKT_FWD  14
#define TC_TM_SER_STORAGE   15
#define TC_TM_SER_NC3       16
#define TC_TM_SER_TEST      17
#define TC_TM_SER_PROC      18
#define TC_TM_SER_EV_ACT    19

/*sequence definitions*/
#define TC_TM_SEQ_FPACKET 0x01
#define TC_TM_SEQ_CPACKET 0x00
#define TC_TM_SEQ_LPACKET 0x02
#define TC_TM_SEQ_SPACKET 0x03

/*services ack req*/
/*should confirm endianess*/
#define TC_ACK_NO           0x00
#define TC_ACK_ACC          0x01
#define TC_ACK_EXE_START    0x02
#define TC_ACK_EXE_PROG     0x04
#define TC_ACK_EXE_COMP     0x08
#define TC_ACK_ALL          0x0F

#define TC_TM_SER_LDATA_MAX_DATA_PKT    512

#define VER_NUMBER
#define TC_TM_TYPE
#define DATA_FIELD_HDR_FLG
#define APP_ID
#define SEQ_FLG

#define MAX_PKT_DATA    20
#define MAX_EX_PKT_DATA 512
#define MAX_APP_ID      20
#define MAX_SERVICES    20
#define MAX_SUBTYPES    26

#define TC 1
#define TM 0

#define TC_VERIFICATION_SERVICE         1
#define TC_HOUSEKEEPING_SERVICE         3
#define TC_FUNCTION_MANAGEMENT_SERVICE  8
#define TC_LARGE_DATA_SERVICE           13
#define TC_MASS_STORAGE_SERVICE         15
#define TC_TEST_SERVICE                 17

#define OBC     1
#define EPS     2
#define ADCS    3
#define COMMS   4
#define IAC     5
#define GND     6

#define SD1     1

#define TURN_OFF    0
#define TURN_ON     1
#define RESET       2

namespace gr
{
  namespace satnogs
  {
    /*!
     * \brief Telemetry and telecommands packet methods
     * \ingroup satnogs
     */

    /*!
     * Return status codes
     */
    typedef enum
    {
      R_OBC_PKT_ILLEGAL_APPID = 0,   //!< R_OBC_PKT_ILLEGAL_APPID illegal application ID
      R_OBC_PKT_INV_LEN = 1,         //!< R_OBC_PKT_INV_LEN invalid length
      R_OBC_PKT_INC_CRC = 2,         //!< R_OBC_PKT_INC_CRC incorrect CRC
      R_OBC_PKT_ILLEGAL_PKT_TP = 3,  //!< R_OBC_PKT_ILLEGAL_PKT_TP
      R_OBC_PKT_ILLEGAL_PKT_STP = 4, //!< R_OBC_PKT_ILLEGAL_PKT_STP
      R_OBC_PKT_ILLEGAL_APP_DATA = 5,//!< R_OBC_PKT_ILLEGAL_APP_DATA
      R_OBC_OK = 6,                  //!< R_OBC_OK All ok
      R_OBC_ERROR = 7,               //!< R_OBC_ERROR an error occured
      R_OBC_EOT = 8,                 //!< R_OBC_EOT End-of-transfer
    } OBC_ret_state_t;

    union _cnv
    {
      uint32_t cnv32;
      uint16_t cnv16[2];
      uint8_t cnv8[4];
    };

    typedef struct
    {
      /* packet id */
      uint8_t ver; /* 3 bits, should be equal to 0 */
      uint8_t data_field_hdr; /* 1 bit, data_field_hdr exists in data = 1 */
      uint16_t app_id; /* TM: app id = 0 for time packets, = 0xff for idle packets. */
      uint8_t type; /* 1 bit, tm = 0, tc = 1 */

      /* packet sequence control */
      uint8_t seq_flags; /* 3 bits, definition in TC_SEQ_xPACKET */
      uint16_t seq_count; /* 14 bits, packet counter, should be unique for each app id */

      uint16_t len; /* 16 bits, C = (Number of octets in packet data field) - 1 */

      uint8_t ack; /* 4 bits, definition in TC_ACK_xxxx 0 if its a TM */
      uint8_t ser_type; /* 8 bit, service type */
      uint8_t ser_subtype; /* 8 bit, service subtype */

      /*optional*/
      uint8_t pckt_sub_cnt; /* 8 bits*/
      uint16_t dest_id;

      uint8_t *data; /* variable data, this should be fixed array */
      uint8_t padding;  /* x bits, padding for word alligment */

      uint16_t crc; /* CRC or checksum, mission specific*/
    } tc_tm_pkt;

    extern const uint8_t services_verification_TC_TM[MAX_SERVICES][MAX_SUBTYPES][2];
    extern const uint8_t app_id_verification[MAX_APP_ID];
    extern const uint8_t services_verification_OBC_TC[MAX_SERVICES][MAX_SUBTYPES];

    extern OBC_ret_state_t
    verification_pack_pkt_api (uint8_t *buf, tc_tm_pkt *pkt,
			       uint16_t *buf_pointer);
    extern OBC_ret_state_t
    hk_pack_pkt_api (uint8_t *buf, tc_tm_pkt *pkt, uint16_t *buf_pointer);

    uint8_t
    checkSum (const uint8_t *data, uint16_t size);

    OBC_ret_state_t
    unpack_pkt (const uint8_t *buf, tc_tm_pkt *pkt, const uint16_t size);

    OBC_ret_state_t
    pack_pkt (uint8_t *buf, tc_tm_pkt *pkt, uint16_t *size);

    OBC_ret_state_t
    crt_pkt (tc_tm_pkt *pkt, uint16_t app_id, uint8_t type, uint8_t ack,
	     uint8_t ser_type, uint8_t ser_subtype, uint16_t dest_id);

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_TC_TM_H */

