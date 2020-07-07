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
#ifndef RTE_XCONS_MESSAGES_HPP
#define RTE_XCONS_MESSAGES_HPP

// Generated Header file RTE_XCons_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RTEERR_XCONS_SECURITY_DESCRIPTOR               SAPDBErr_MessageList::Error,13400,"XCons: Allocation of security descriptor failed, rc = %s",1
#define RTEERR_XCONS_SECURITY_DESCRIPTOR_ID 13400
#define RTEERR_XCONS_GUID                              SAPDBErr_MessageList::Error,13402,"XCons: Getting GUID for %s failed",1
#define RTEERR_XCONS_GUID_ID 13402
#define RTEERR_XCONS_INIT_DATA_COMMUNICATION           SAPDBErr_MessageList::Error,13403,"XCons: Initialization of data communication failed",0
#define RTEERR_XCONS_INIT_DATA_COMMUNICATION_ID 13403
#define RTEERR_XCONS_CONNECT                           SAPDBErr_MessageList::Error,13404,"XCons: Connect failed",0
#define RTEERR_XCONS_CONNECT_ID 13404
#define RTEERR_XCONS_DISCONNECT                        SAPDBErr_MessageList::Error,13405,"XCons: Disconnect failed",0
#define RTEERR_XCONS_DISCONNECT_ID 13405
#define RTEERR_XCONS_DB_NOT_STARTED                    SAPDBErr_MessageList::Error,13407,"XCons: '%s' has not been started",1
#define RTEERR_XCONS_DB_NOT_STARTED_ID 13407
#define RTEERR_XCONS_PUT_REQUEST                       SAPDBErr_MessageList::Error,13408,"XCons: Sending request to server DB %s failed",1
#define RTEERR_XCONS_PUT_REQUEST_ID 13408
#define RTEERR_XCONS_SEND_DATA                         SAPDBErr_MessageList::Error,13409,"XCons: Sending data to server DB %s failed",1
#define RTEERR_XCONS_SEND_DATA_ID 13409
#define RTEERR_XCONS_GET_DATA                          SAPDBErr_MessageList::Error,13410,"XCons: Getting data from server DB %s failed",1
#define RTEERR_XCONS_GET_DATA_ID 13410
#define RTEERR_XCONS_GET_DATA_READ                     SAPDBErr_MessageList::Error,13411,"XCons: Reading data from server DB %s: bytes read: %s, bytes to read: %s",3
#define RTEERR_XCONS_GET_DATA_READ_ID 13411
#define RTEERR_XCONS_ACCESS_DENIED                     SAPDBErr_MessageList::Error,13412,"XCons: Access to server DB %s denied",1
#define RTEERR_XCONS_ACCESS_DENIED_ID 13412
#define RTEERR_XCONS_REQUEST_PROCESSING                SAPDBErr_MessageList::Error,13413,"XCons: Error while processing request command %s",1
#define RTEERR_XCONS_REQUEST_PROCESSING_ID 13413
#define RTEERR_XCONS_UNKNOWN_REQUEST                   SAPDBErr_MessageList::Error,13414,"XCons: Unknown XCons request command %s",1
#define RTEERR_XCONS_UNKNOWN_REQUEST_ID 13414
#define RTEERR_XCONS_INIT_REQ_COMM                     SAPDBErr_MessageList::Error,13415,"XCons: Initialization of request communication failed",0
#define RTEERR_XCONS_INIT_REQ_COMM_ID 13415
#define RTEERR_XCONS_ALLOC_EVENT                       SAPDBErr_MessageList::Error,13416,"XCons: Allocation of %s bytes for event data failed",1
#define RTEERR_XCONS_ALLOC_EVENT_ID 13416
#define RTEERR_XCONS_GET_ERROR                         SAPDBErr_MessageList::Error,13417,"XCons: Getting error data from server DB %s failed",1
#define RTEERR_XCONS_GET_ERROR_ID 13417
#define RTEERR_XCONS_LOAD_EVENT                        SAPDBErr_MessageList::Error,13418,"XCons: Loading event list failed",0
#define RTEERR_XCONS_LOAD_EVENT_ID 13418
#define RTEERR_XCONS_GET_DATA_NEXT_READ                SAPDBErr_MessageList::Error,13419,"XCons: Reading next data from server DB %s: residual sends: %s, bytes read: %s, bytes to read: %s",4
#define RTEERR_XCONS_GET_DATA_NEXT_READ_ID 13419
#define RTEERR_XCONS_PROTOCOL_ERROR                    SAPDBErr_MessageList::Error,13421,"XCons: Protocol error",0
#define RTEERR_XCONS_PROTOCOL_ERROR_ID 13421
#define RTEERR_XCONS_GET_DATA_LEN                      SAPDBErr_MessageList::Error,13422,"XCons: Wrong reply size from server DB %s, expected: %s, send: %s",3
#define RTEERR_XCONS_GET_DATA_LEN_ID 13422
#define RTEERR_XCONS_UNKNOWN_REQUEST_TYPE              SAPDBErr_MessageList::Error,13423,"XCons: Unknown request type: %s",1
#define RTEERR_XCONS_UNKNOWN_REQUEST_TYPE_ID 13423
#define RTEERR_XCONS_WRONG_DB_VERSION                  SAPDBErr_MessageList::Error,13425,"XCons: Incompatible versions: Kernel RTE: %s, XCons: %s",2
#define RTEERR_XCONS_WRONG_DB_VERSION_ID 13425
#define RTEERR_XCONS_OPEN_KSS                          SAPDBErr_MessageList::Error,13426,"XCons: Opening Kernel Shared Segment of Server DB %s failed, rc = %s",2
#define RTEERR_XCONS_OPEN_KSS_ID 13426
#define RTEERR_XCONS_RELEASE_KSS                       SAPDBErr_MessageList::Error,13427,"XCons: Releasing Kernel Shared Segment of Server DB %s failed, rc = %s",2
#define RTEERR_XCONS_RELEASE_KSS_ID 13427
#define RTEERR_XCONS_SEND_REQUEST                      SAPDBErr_MessageList::Error,13428,"XCons: Send request %s to server DB %s failed",2
#define RTEERR_XCONS_SEND_REQUEST_ID 13428
#define RTEERR_XCONS_GET_HEADER                        SAPDBErr_MessageList::Error,13429,"XCons: Get data header for request %s from server DB %s failed",2
#define RTEERR_XCONS_GET_HEADER_ID 13429
#define RTEERR_XCONS_ALLOC_DATA                        SAPDBErr_MessageList::Error,13430,"XCons: Allocation of %s bytes for processing data reply from server DB %s to command %s failed",3
#define RTEERR_XCONS_ALLOC_DATA_ID 13430
#define RTEERR_XCONS_WRONG_TASK                        SAPDBErr_MessageList::Error,13431,"XCons: Wrong task index",0
#define RTEERR_XCONS_WRONG_TASK_ID 13431
#define RTEERR_XCONS_WRONG_TASK_TYPE                   SAPDBErr_MessageList::Error,13432,"XCons: Could not cancel/kill task",0
#define RTEERR_XCONS_WRONG_TASK_TYPE_ID 13432
#define RTEERR_XCONS_WRONG_UKT                         SAPDBErr_MessageList::Error,13433,"XCons: Wrong ukt index",0
#define RTEERR_XCONS_WRONG_UKT_ID 13433
#define RTEERR_XCONS_SET_UKT_AFFINITY                  SAPDBErr_MessageList::Error,13434,"XCons: Could not set affinity, (view kernel diagnostic file)",0
#define RTEERR_XCONS_SET_UKT_AFFINITY_ID 13434
#define RTEERR_XCONS_AFFIN_NO_PROCESSOR                SAPDBErr_MessageList::Error,13435,"XCons: Not enough processors",0
#define RTEERR_XCONS_AFFIN_NO_PROCESSOR_ID 13435
#define RTEERR_XCONS_INTERACT_WTH_DECTOP               SAPDBErr_MessageList::Error,13436,"XCons: Service needs 'interact with desktop' option for 'DebugBreak'",0
#define RTEERR_XCONS_INTERACT_WTH_DECTOP_ID 13436
#define RTEERR_XCONS_TASK_INACTIVE                     SAPDBErr_MessageList::Error,13437,"XCons: Task is inactive",0
#define RTEERR_XCONS_TASK_INACTIVE_ID 13437
#define RTEERR_XCONS_WRONG_VALUE                       SAPDBErr_MessageList::Error,13438,"XCons: Wrong value",0
#define RTEERR_XCONS_WRONG_VALUE_ID 13438
#define RTEERR_XCONS_UNKNOWN_RET_CODE                  SAPDBErr_MessageList::Error,13439,"XCons: Unknown return code: %s",1
#define RTEERR_XCONS_UNKNOWN_RET_CODE_ID 13439
#define RTEERR_XCONS_TIMEOUT_WORKER_THRD_START         SAPDBErr_MessageList::Error,13440,"XCons: Starting semaphore timeout thread failed with %s: %s",2
#define RTEERR_XCONS_TIMEOUT_WORKER_THRD_START_ID 13440
#define RTEERR_XCONS_INIT_MULTITHREADING               SAPDBErr_MessageList::Error,13442,"XCons: Initialization of multi threading failed with %s: %s",2
#define RTEERR_XCONS_INIT_MULTITHREADING_ID 13442
#define RTEERR_XCONS_SIGNAL_CONFIRMATION               SAPDBErr_MessageList::Error,13443,"XCons: Signal confirmation failed",0
#define RTEERR_XCONS_SIGNAL_CONFIRMATION_ID 13443
#define RTEINFO_XCONS_TIMEOUT                          SAPDBErr_MessageList::Info,13550,"XCons: Timeout for this connection on server DB %s occured",1
#define RTEINFO_XCONS_TIMEOUT_ID 13550

#endif /* RTE_XCONS_MESSAGES_HPP */