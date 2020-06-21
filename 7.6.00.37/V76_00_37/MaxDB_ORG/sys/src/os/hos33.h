/*!
  @file           hos33.h
  @author         RaymondR
  @brief          local communication
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



\endif
*/




#ifndef HOS33_H
#define HOS33_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

//
// --- VOS33C INTERNAL USE ONLY!
//
typedef struct shm_connect_info_record
  {
  PID                               pidClientPID;
  SQL_NODEIDC                       szClientNode;
  SQL_DBNAMEC                       szServerDB;
  SQL_NODEIDC                       szServerNode;
  ULONG                             ulServerRef;
  ULONG                             ulClientRef;
  ULONG                             ulServiceType;

  ULONG                             ulCurrentPacket;
  ULONG                             ulCmdDataLen;
  ULONG                             ulPacketCnt;
  struct teo003_CommPacketRecord   *pCommPacketList[MAX_SQL_PACKETS];

  ULONG                             ulPacketSize;
  ULONG                             ulMaxDataLen;
  ULONG                             ulMinReplySize;

  HANDLE                            hCS;             // - NT only
  struct comm_seg_header_record     *pCSHeader;
  ULONG                             ulCSSize;
  HANDLE                            hFCS;
  PFLAG_COMM_SEG                    pFCS;
  HANDLE                            hQueMailSlot;
  HANDLE                            hClientSem;
  ULONG                             ulNumOfServerSems;
  HANDLE                            *phServerSems;
  HANDLE                            hServerProcess;

  ULONG                             ulCommFlagNo;
  PULONG                            pulCommFlag;
  PULONG                            pulSemaFlag;
  PIPC_BASE_OBJ_NAME                pSemaNames;
  PCHAR                             pszSemaPath;
  SAPDB_UInt1                       encryption;
 } SHM_CONNECT_INFO_REC;

typedef SHM_CONNECT_INFO_REC        *PSHM_CONNECT_INFO_REC;


// --- VOS33C - ( Local communication ) --------------------------------------

externC ULONG           sql33c_info_request   ( PSZ                             pszServerDB,
                                                ULONG                           ulServiceType,
                                                ULONG                           ulPacketCnt,
                                                SAPDB_Bool                      omitReplyPart,
                                                PULONG                          pulPacketSize,
                                                PULONG                          pulMaxDataLen,
                                                PULONG                          pulMinReplySize,
                                                PCHAR                           authenticationAllow,
                                                ULONG                           authenticationAllowBufSize,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_connect        ( PCONNECT_PARAM_REC              pConnParam,
                                                PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_connect_xserver( PCONNECT_PARAM_REC              pConnParam,
                                                PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_request        ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                ULONG                           ulCmdPacket,
                                                ULONG                           ulCmdPacketLen,
                                                tsp00_Uint1                     RTEFlags,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_replyavailable ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_receive        ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                struct teo003_CommPacketRecord  **ppResPacket,
                                                ULONG VAR_VALUE_REF             ulResPacketLen,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_cancel_dump    ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                ULONG                           ulReqType,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
externC ULONG           sql33c_release        ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
#if defined (KERNEL)
 externC ULONG          sql33c_crash_cleanup  ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
 externC ULONG          sql33c_reply          ( PSHM_CONNECT_INFO_REC           pShmConnInfo,
                                                struct  task_ctrl_record        *pTaskCtrl,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );
#endif

#endif
