/*!
  @file           hos42.h
  @author         RaymondR
  @brief          communication
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




#ifndef HOS42_H
#define HOS42_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define  USE_OLD_CONPKT

#include "gos00.h"
#include "geo003.h"
#include "gos42.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef struct tos42_old_conpkt_param_record
{
  teo00_Socket                    sd;
  tsp00_Uint                      ulMessClass;
  tsp00_Uint                      ulSenderRef;
  tsp00_Uint                      ulReceiverRef;
  tsp00_Uint                      ulServiceType;
  tsp00_Uint                      ulMaxSegmentSize;
  tsp00_Uint                      ulPacketSize;
  tsp00_Uint                      ulMaxDataLen;
  tsp00_Uint                      ulMinReplySize;
  tsp00_Uint                      ulSwapType;
  SQL_DBNAMEC                     szSenderServerDB;
  SQL_DBNAMEC                     szReceiverServerDB;
  PID                             pidSenderPID;
  USHORT                          usServicePort;
  tsp00_Uint                      ulCommState;
  BOOL                            fAcknowledge;
  PATHNAME                        szServerDBRoot;
  PATHNAME                        szServerPgm;
} OLD_CONPKT_PARAM_REC;

typedef OLD_CONPKT_PARAM_REC  *POLD_CONPKT_PARAM_REC;



/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

// --- VOS42C --------------------------------------------------------------

ULONG           sql42c_pipe_send_conpkt( POLD_CONPKT_PARAM_REC     pConPktParamRec,
                                         HANDLE                    hWritePipe,
                                         tsp00_ErrText             pErrText );

ULONG           sql42c_pipe_recv_conpkt( POLD_CONPKT_PARAM_REC     pConPktParamRec,
                                         HANDLE                    hReadPipe,
                                         tsp00_ErrText             pErrText );

void            sql42c_new2old_conpkt  ( POLD_CONPKT_PARAM_REC     pOldConPktParamRec,
                                         PCONPKT_PARAM_REC         pConPktParamRec );

ULONG           sql42c_recv_header( teo00_Socket              sd,
                                    ULONG                     ulSwapType,
                                    PULONG                    pulSenderRef,
                                    PULONG                    pulReceiverRef,
                                    PULONG                    pulMessClass,
                                    tsp00_ErrText             pErrText );


ULONG           sql42c_send_packet( teo00_Socket                    sd,
                                    ULONG                           ulMaxSegmentSize,
                                    struct teo003_CommPacketRecord *pCommPacket,
                                    ULONG                           ulDataLen,
                                    ULONG                           ulClientRef,
                                    ULONG                           ulServerRef,
                                    ULONG                           ulMessClass,
                                    ULONG                           ulCommState,
                                    tsp00_ErrText                   pErrText );

ULONG           sql42c_recv_packet( teo00_Socket                    sd,
                                    ULONG                           ulSwapType,
                                    struct teo003_CommPacketRecord *pCommPacket,
                                    ULONG                           ulMaxDataLen,
                                    PULONG                          pulDataLen,
                                    PULONG                          pulSenderRef,
                                    PULONG                          pulReceiverRef,
                                    PULONG                          pulMessClass,
                                    tsp00_ErrText                   pErrText );

#endif  /* HOS42_H */
