/*!
  @file           heo420.h
  @author         JoergM
  @special area   communication ...
  @brief          description ...
  @see            example.html ...

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



#ifndef HEO420_H
#define HEO420_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo003.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


externC tsp00_Uint1 
eo420NewSwapType ();

externC tsp00_Uint    
eo420ServiceTypeToMessClass ( tsp00_Uint     ulServiceType ) ;

externC void
eo420InitConPktParamRec     ( teo003_ConPktParamRecord *pConPktParamRec );
  
externC void
eo420UnpackRteHeader        ( tsp00_Uint                    ulSwapType,
                              teo003_RteHeaderRecord       *pRTEHeader,
                              tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC tsp00_Uint 
eo420UnpackSAPUCString      (tsp00_Uint                    ulSwapTyp,
                             SAPDB_Byte *                  pSourceString,
                             SAPDB_Byte *                  pDestString,
                             SAPDB_UInt4                   numBytes,
                             tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC tsp00_Uint    
eo420GetDBRootFromConPkt    ( teo003_RteConnectPacketRec *pConnectPacket,
                              char                       *szDBRoot );
externC tsp00_Uint    
eo420GetServerPgmFromConPkt ( teo003_RteConnectPacketRec *pConnectPacket,
                              char                       *szServerPgm ) ;

externC tsp00_Uint    
eo420SendRteHeader     ( void                         *SendHandle,
                         SEND_PKT_FUNC_PTR_EO003       SendFunc,
                         tsp00_Uint                    ulSenderRef,
                         tsp00_Uint                    ulReceiverRef,
                         tsp00_Uint                    ulMessClass,
                         tsp00_Uint                    ulCommState,
                         tsp00_Uint1                   RTEFlags,
                         tsp00_ErrTextc  VAR_ARRAY_REF pErrText ) ;

externC tsp00_Uint     
eo420ReceiveConnectPacket( void                         *RecvHandle,
                           RECV_PKT_FUNC_PTR_EO003       RecvFunc,
                           teo003_ConPktParamRecord     *pConPktParamRec,
                           tsp00_ErrTextc VAR_ARRAY_REF  ErrText );

externC tsp00_Uint    
eo420SendConnectPacket ( void                          *SendHandle, 
                         SEND_PKT_FUNC_PTR_EO003        SendFunc,
                         teo003_ConPktParamRecord      *pConPktParamRec,
                         tsp00_ErrTextc  VAR_ARRAY_REF  pErrText );

externC tsp00_Uint 
eo420SendCertificatePacket (void *                        pHdl, 
                            SEND_PKT_FUNC_PTR_EO003       pSendFunc, 
                            teo003_ConPktParamRecord *    pConPktParamRec,
                            SAPDB_Byte VAR_VALUE_REF      pServerCertificate,
                            SAPDB_UInt4                   lenCertificate,
                            tsp00_ErrTextc VAR_ARRAY_REF  pErrText);

externC tsp00_Uint 
eo420ReceiveCertificatePacket (void *                         pHdl, 
                               RECV_PKT_FUNC_PTR_EO003        pRecvFunc, 
                               teo003_ConPktParamRecord     *pConPktParamRec,
                               SAPDB_Byte VAR_VALUE_REF       pServerCertificate,
                               SAPDB_UInt4                    maxLenCertificate,
                               tsp00_ErrTextc VAR_ARRAY_REF   pErrText);

externC tsp00_Uint    
eo420SendCommPacket    ( void                         *SendHandle,
                         SEND_PKT_FUNC_PTR_EO003       SendFunc,
                         tsp00_Uint                    ulMaxSegmentSize,
                         teo003_CommPacketRecord      *pCommPacket,
                         tsp00_Uint                    ulDataLen,
                         tsp00_Uint                    ulSenderRef,
                         tsp00_Uint                    ulReceiverRef,
                         tsp00_Uint                    ulMessClass,
                         tsp00_Uint                    ulCommState,
                         tsp00_Uint1                   RTEFlags,
                         tsp00_ErrTextc  VAR_ARRAY_REF pErrText ) ;

externC tsp00_Uint    
eo420SendReleasePacket ( void                      *SendHandle,
                         SEND_PKT_FUNC_PTR_EO003    SendFunc,
                         tsp00_Uint                 ulSenderRef,
                         tsp00_Uint                 ulReceiverRef,
                         tsp00_Uint                 ulMessClass,
                         tsp00_Uint                 ulCommState) ;

externC tsp00_Uint    
eo420ReceiveCommPacket ( void                         *RecvHandle,
                         RECV_PKT_FUNC_PTR_EO003       RecvFunc,
                         tsp00_Uint                    ulSwapType,
                         teo003_CommPacketRecord      *pCommPacket,
                         tsp00_Uint                    ulMaxDataLen,
                         tsp00_Uint                   *pulDataLen,
                         tsp00_Uint                   *pulSenderRef,
                         tsp00_Uint                   *pulReceiverRef,
                         tsp00_Uint                   *pulMessClass,
                         tsp00_Uint1                  *pRTEFlags,
                         tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

#if defined (RTE_USE_SOCK)
externC tsp00_Uint    
eo420SetSocketBufferSize ( teo00_Socket                  SocketHandle,
                           tsp00_Uint                    ulMinSendLen,
                           tsp00_Uint                   *ulMaxSendLen,
                           tsp00_ErrTextc VAR_ARRAY_REF  pErrText );
#endif

externC tsp00_Uint    
eo420SetNiSocketBufferSize ( void                         *NiHandle,
                             tsp00_Uint                    ulMinSendLen,
                             tsp00_Uint                   *ulMaxSendLen,
                             tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC tsp00_Uint
eo420FillPingPacket        ( char*                        pData,
                             tsp00_Uint                   maxLength,
                             tsp00_Uint1                  hops,
                             char*                        peerName,
                             char*                        versionString,
                             char*                        fillPattern );

externC tsp00_Bool 
eo420ExtractPingPacket     ( char*                        pData,
                             tsp00_Uint                   length,
                             tsp00_Uint1*                 hops,
                             char**                       peerName,
                             char**                       versionString );
#endif  /* HEO420_H */
