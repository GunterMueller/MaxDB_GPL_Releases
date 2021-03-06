/*
        ========== licence begin  GPL
        Copyright (C) 2005 SAP AG

        This program is free software; you can redistribute it and/or
        modify it under the terms of the GNU General Public License
        as published by the Free Software Foundation; either version 2
        of the License, or (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
        ========== licence end
 */
#ifndef RTE_COMM_MESSAGES_HPP
#define RTE_COMM_MESSAGES_HPP

// Generated Header file RTE_Comm_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RTEERR_COMM_ILL_SWAP_TYPE                      SAPDBErr_MessageList::Error,14000,"Illegal swap type: %s",1
#define RTEERR_COMM_ILL_SWAP_TYPE_ID 14000
#define RTEERR_COMM_OUT_OF_MEMORY                      SAPDBErr_MessageList::Error,14001,"Out of memory error",0
#define RTEERR_COMM_OUT_OF_MEMORY_ID 14001
#define RTEERR_COMM_WRONG_OR_MISSING_LOCATION          SAPDBErr_MessageList::Error,14002,"Could not determine the server location",0
#define RTEERR_COMM_WRONG_OR_MISSING_LOCATION_ID 14002
#define RTEERR_COMM_UNEXPECTED_PROTOCOL                SAPDBErr_MessageList::Error,14003,"Unexpected protocol name: %s",1
#define RTEERR_COMM_UNEXPECTED_PROTOCOL_ID 14003
#define RTEERR_COMM_WRONG_LOCATION_STRING              SAPDBErr_MessageList::Error,14004,"Wrong location string, expecting: %s",1
#define RTEERR_COMM_WRONG_LOCATION_STRING_ID 14004
#define RTEERR_COMM_PATH_CONTAINS_WRONG_CHARS          SAPDBErr_MessageList::Error,14005,"Path part contains characters which must be escaped",0
#define RTEERR_COMM_PATH_CONTAINS_WRONG_CHARS_ID 14005
#define RTEERR_COMM_WRONG_OR_MISSING_PATH              SAPDBErr_MessageList::Error,14006,"Wrong or missing path part",0
#define RTEERR_COMM_WRONG_OR_MISSING_PATH_ID 14006
#define RTEERR_COMM_QUERY_QUESTION_MARK_EXP            SAPDBErr_MessageList::Error,14007,"Question mark not found or is not on the correct postion",0
#define RTEERR_COMM_QUERY_QUESTION_MARK_EXP_ID 14007
#define RTEERR_COMM_QUERY_CONTAINS_WRONG_CHARS         SAPDBErr_MessageList::Error,14008,"Query part contains characters which must be escaped",0
#define RTEERR_COMM_QUERY_CONTAINS_WRONG_CHARS_ID 14008
#define RTEERR_COMM_IPLOC_CONTAINS_WRONG_CHARS         SAPDBErr_MessageList::Error,14009,"Location part contains characters which must be escaped",0
#define RTEERR_COMM_IPLOC_CONTAINS_WRONG_CHARS_ID 14009
#define RTEERR_COMM_IPLOC_CONTAINS_WRONG_PORTNO        SAPDBErr_MessageList::Error,14010,"Location part contains invalid port number",0
#define RTEERR_COMM_IPLOC_CONTAINS_WRONG_PORTNO_ID 14010
#define RTEERR_COMM_NILOC_DELIMITER_MISSING            SAPDBErr_MessageList::Error,14011,"Delimiter %s missing in SAPNI location part",1
#define RTEERR_COMM_NILOC_DELIMITER_MISSING_ID 14011
#define RTEERR_COMM_NILOC_HOST_SEP_MISSING             SAPDBErr_MessageList::Error,14012,"Host separator %s missing in SAPNI location part",1
#define RTEERR_COMM_NILOC_HOST_SEP_MISSING_ID 14012
#define RTEERR_COMM_NILOC_CONTAINS_WRONG_PORTNO        SAPDBErr_MessageList::Error,14013,"SAPNI location part contains invalid port number",0
#define RTEERR_COMM_NILOC_CONTAINS_WRONG_PORTNO_ID 14013
#define RTEERR_COMM_DBPATH_MISSING_DBNAME              SAPDBErr_MessageList::Error,14014,"Database name is missing",0
#define RTEERR_COMM_DBPATH_MISSING_DBNAME_ID 14014
#define RTEERR_COMM_REQ_WRONG_BUFFER_SIZE              SAPDBErr_MessageList::Error,14015,"Wrong packet buffer size: %s, minimum expected: %s",2
#define RTEERR_COMM_REQ_WRONG_BUFFER_SIZE_ID 14015
#define RTEERR_COMM_REQ_ILL_DATA_LEN                   SAPDBErr_MessageList::Error,14016,"Illegal packet data length: %s, minimum expected: %s",2
#define RTEERR_COMM_REQ_ILL_DATA_LEN_ID 14016
#define RTEERR_COMM_VAR_PART_EXHAUSTED                 SAPDBErr_MessageList::Error,14017,"Variable part (varpart) exhausted",0
#define RTEERR_COMM_VAR_PART_EXHAUSTED_ID 14017
#define RTEERR_COMM_VAR_PART_VALUE_OUT_OF_RANGE        SAPDBErr_MessageList::Error,14018,"Variable part value out of range: %s ( min: %s, max: %s )",3
#define RTEERR_COMM_VAR_PART_VALUE_OUT_OF_RANGE_ID 14018
#define RTEERR_COMM_UNEXPECTED_VALUE_LENGTH            SAPDBErr_MessageList::Error,14019,"Unexpected value length: %s",1
#define RTEERR_COMM_UNEXPECTED_VALUE_LENGTH_ID 14019
#define RTEERR_COMM_VARPART_VALUE_NOT_FOUND            SAPDBErr_MessageList::Error,14020,"Value not found, value id: %s",1
#define RTEERR_COMM_VARPART_VALUE_NOT_FOUND_ID 14020
#define RTEERR_COMM_PACKET_LEN_EXC_BUF_SIZE            SAPDBErr_MessageList::Error,14021,"Packet length (%s) exceeds buffer size (%s)",2
#define RTEERR_COMM_PACKET_LEN_EXC_BUF_SIZE_ID 14021
#define RTEERR_COMM_SURPRISING_PEER_REF                SAPDBErr_MessageList::Error,14022,"Surprising peer reference (%s, expected: %s)",2
#define RTEERR_COMM_SURPRISING_PEER_REF_ID 14022
#define RTEERR_COMM_SURPRISING_RECEIVER_REF            SAPDBErr_MessageList::Error,14023,"Surprising receiver reference (%s, expected: %s)",2
#define RTEERR_COMM_SURPRISING_RECEIVER_REF_ID 14023
#define RTEERR_COMM_UNEXPECTED_SEQ_NUMBER              SAPDBErr_MessageList::Error,14024,"Unexpected sequence number (%s, expected: %s)",2
#define RTEERR_COMM_UNEXPECTED_SEQ_NUMBER_ID 14024
#define RTEERR_COMM_INVALID_DATAL_LEN                  SAPDBErr_MessageList::Error,14025,"Unexpected data length ( maximum: %s, total: %s, actual: %s)",3
#define RTEERR_COMM_INVALID_DATAL_LEN_ID 14025
#define RTEERR_COMM_SURPRISING_PROTO                   SAPDBErr_MessageList::Error,14026,"Surprising protocol id or version (id: %s, version: %s)",2
#define RTEERR_COMM_SURPRISING_PROTO_ID 14026
#define RTEERR_COMM_PACKET_HEADER_INCOMPLETE           SAPDBErr_MessageList::Error,14027,"Incomplete packet header (receive length: %s, header size %s)",2
#define RTEERR_COMM_PACKET_HEADER_INCOMPLETE_ID 14027

#endif /* RTE_COMM_MESSAGES_HPP */
