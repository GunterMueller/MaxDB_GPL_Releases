/*!
  @file           hos23.h
  @author         RaymondR
  @brief          communication (TCP/IP)
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef HOS23_H
#define HOS23_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"
#include "geo003.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (RTE_USE_SOCK)

//
// --- VOS23C INTERNAL USE ONLY!
//
typedef struct tcpip_connect_info_record
  {
  PID                               pidClientPID;
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

  teo00_Socket                      sd;
  INT                               SwapType;
  UCHAR                             ucName[12];
  USHORT                            usServicePort;
  ULONG                             ulSwapType;
  ULONG                             ulMinSegmentSize;
  ULONG                             ulMaxSegmentSize;
  } TCPIP_CONNECT_INFO_REC;

typedef TCPIP_CONNECT_INFO_REC      *PTCPIP_CONNECT_INFO_REC;

// --- VOS23C - ( TCPIP communication ) ---------------------------------------

externC ULONG           sql23c_connect        ( PCONNECT_PARAM_REC              pConnParam,
                                                PTCPIP_CONNECT_INFO_REC         pTCPIPConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );

externC ULONG           sql23c_request        ( PTCPIP_CONNECT_INFO_REC         pTCPIPConnInfo,
                                                ULONG                           ulCmdPacket,
                                                ULONG                           ulCmdDataLen,
                                                tsp00_Uint1                     RTEFlags,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );

externC ULONG           sql23c_replyavailable ( PTCPIP_CONNECT_INFO_REC         pTCPIPConnInfo,
                                                tsp00_ErrTextc VAR_ARRAY_REF    pErrText );

externC ULONG           sql23c_receive       ( PTCPIP_CONNECT_INFO_REC          pTCPIPConnInfo,
                                               struct teo003_CommPacketRecord **ppResPacket,
                                               ULONG VAR_VALUE_REF              ulResDataLen,
                                               tsp00_ErrTextc VAR_ARRAY_REF     pErrText );

externC ULONG           sql23c_receive_ackn  ( PTCPIP_CONNECT_INFO_REC          pTCPIPConnInfo,
                                               tsp00_ErrTextc VAR_ARRAY_REF     pErrText );

externC ULONG           sql23c_cancel_dump   ( PTCPIP_CONNECT_INFO_REC          pTCPIPConnInfo,
                                               ULONG                            ulReqType,
                                               tsp00_ErrTextc VAR_ARRAY_REF     pErrText );

externC ULONG           sql23c_release       ( PTCPIP_CONNECT_INFO_REC          pTCPIPConnInfo,
                                               tsp00_ErrTextc VAR_ARRAY_REF     pErrText );


#endif // RTE_USE_SOCK
externC ULONG           sql23c_GetServicePort (char *                           pszServerNode,
                                               tsp00_Uint2 VAR_VALUE_REF        pusServicePort,
                                               SAPDB_Char *                     defaultService,
                                               tsp00_ErrTextc VAR_ARRAY_REF     pErrText);


/* PTS 1105295 */
externC tsp01_CommErr_Enum 
os23XServerPing ( char *                         pszServerNode,     /* '\0' terminated node or saprouterstring */
                  tsp00_Versionc VAR_ARRAY_REF   szServerVersion,   /* '\0' terminated version from answering server */
                  SAPDB_Byte VAR_VALUE_REF       pServerCertificate,
                  SAPDB_UInt4                    lenCertificate,
                  SAPDB_Bool                     captureServerCert,  
                  tsp00_ErrTextc VAR_ARRAY_REF   ErrTextC);         /* '\0' terminated error string   */

/* PTS 1111208 */
externC tsp01_CommErr_Enum 
os23NewDebugLevel ( char                  *pszServerNode,     /* '\0' terminated node or saprouterstring */
				    int                    newDebugLevel,     /* new debug level for x_server */
                    tsp00_ErrTextc VAR_ARRAY_REF        ErrTextC);          /* '\0' terminated error string   */
#endif
