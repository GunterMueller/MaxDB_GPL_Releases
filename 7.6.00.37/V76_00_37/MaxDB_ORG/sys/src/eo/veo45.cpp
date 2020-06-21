/*!
  @file           veo45.cpp
  @author         RaymondR
  @special area   communication
  @brief          This module contains the communication protocol base
              base classes.
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo42.h"
#include "geo44.h"
#include "geo45.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                        class teo45_RTEProtocolBase                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo45_RTEProtocolBase::eo45_QueryPeerBufferSizes( void * const                PacketBuffer,
                                                       tsp00_Uint4 const           PacketBufferSize,
                                                       tsp00_Uint4 const           MaxTransferBufferSize,
                                                       tsp00_Uint4                 &PeerTransferBufferSize,
                                                       tsp00_Uint4                 &PeerMinResponsePacketSize,
                                                       teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_QueryPeerBufferSizes"));

  tsp00_Uint4                 ReqDataLen;
  tsp00_Uint4                 RespDataLen;
  tsp00_Uint1                 RespType;
  bool                        Ok;
  teo41_Swap                  Swap;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  //
  // --- Create and send a buffer size request
  //
  teo44_QueryTransferBufferSizeReq    QueryTransferBufferSizeReq;

  if ( MaxTransferBufferSize != (tsp00_Uint4)UNDEF_SP00 )
    QueryTransferBufferSizeReq.eo44_Params().MaxTransferBufferSize_eo44 = MaxTransferBufferSize;

  Ok = QueryTransferBufferSizeReq.eo42_FillPacketData ( PacketDataBuffer, PacketDataBufferSize,
                                                        ReqDataLen, pEventList );

  if ( Ok )
  { // --- send request and receive response!
    Ok = eo45_XferPacket ( PacketBuffer, PacketBufferSize,
                           ReqDataLen, QueryTransferBufferSizeReq.eo42_ReqRespType(),
                           RespDataLen, RespType, Swap, pEventList );
  }

  if ( Ok )
  {
    if ( RespType == teo44_QueryTransferBufferSizeResp::eo42_ReqRespType() )
    {
      // --- we received a valid buffer size response!
      //
      teo44_QueryTransferBufferSizeResp    QueryTransferBufferSizeResp;

      // --- swap and extract the buffer size response
      Ok = QueryTransferBufferSizeResp.eo42_SwapAndExtractPacketData( PacketDataBuffer,RespDataLen, Swap, pEventList );

      if ( Ok )
      {
        PeerTransferBufferSize    = QueryTransferBufferSizeResp.eo44_Params().TransferBufferSize_eo44;
        PeerMinResponsePacketSize = QueryTransferBufferSizeResp.eo44_Params().MinResponsePacketSize_eo44;
      }
    }
    else
    {
      Ok = eo45_HandleUnknownResponses ( PacketDataBuffer, RespDataLen, RespType,
                                         teo44_QueryTransferBufferSizeResp::eo42_ReqRespType(),
                                         Swap, NULL,  pEventList ) ;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_SetPeerBufferSizes ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           TransferBufferSize,
                                                      teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_SetPeerBufferSizes"));

  tsp00_Uint4                 ReqDataLen;
  tsp00_Uint4                 RespDataLen;
  tsp00_Uint1                 RespType;
  bool                        Ok;
  teo41_Swap                  Swap;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  //
  // --- Create and send a set buffer size request
  //
  teo44_SetTransferBufferSizeReq     SetTransferBufferSizeReq;

  SetTransferBufferSizeReq.eo44_Params().TransferBufferSize_eo44 = TransferBufferSize;

  Ok = SetTransferBufferSizeReq.eo42_FillPacketData ( PacketDataBuffer, PacketDataBufferSize,
                                                      ReqDataLen, pEventList );

  if ( Ok )
  { // --- send request and receive response!
    Ok = eo45_XferPacket ( PacketBuffer, PacketBufferSize,
                           ReqDataLen, SetTransferBufferSizeReq.eo42_ReqRespType(),
                           RespDataLen, RespType, Swap, pEventList );
  }

  if (( Ok ) && ( RespType != teo44_SetTransferBufferSizeResp::eo42_ReqRespType() ))
  {
    Ok = eo45_HandleUnknownResponses ( PacketDataBuffer, RespDataLen, RespType,
                                       teo44_SetTransferBufferSizeResp::eo42_ReqRespType(),
                                       Swap, NULL,  pEventList ) ;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_DisconnectFromPeer( void * const                PacketBuffer,
                                                     tsp00_Uint4 const           PacketBufferSize,
                                                     teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_DisconnectFromPeer"));

  tsp00_Uint4                 ReqDataLen;
  tsp00_Uint4                 RespDataLen;
  tsp00_Uint1                 RespType;
  bool                        Ok;
  teo41_Swap                  Swap;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  //
  // --- Create and send a disconnect request
  //
  teo44_DisconnectReq    DisconnectReq;

  Ok = DisconnectReq.eo42_FillPacketData ( PacketDataBuffer, PacketDataBufferSize,
                                           ReqDataLen, pEventList );

  if ( Ok )
  { // --- send request and receive response!
    Ok = eo45_XferPacket ( PacketBuffer, PacketBufferSize, ReqDataLen,
                           DisconnectReq.eo42_ReqRespType(),
                           RespDataLen, RespType, Swap, pEventList );
  }

  if (( Ok ) && ( RespType != teo44_DisconnectResp::eo42_ReqRespType() ))
  {
    Ok = eo45_HandleUnknownResponses ( PacketDataBuffer, RespDataLen, RespType,
                                       teo44_DisconnectResp::eo42_ReqRespType(),
                                       Swap, NULL,  pEventList ) ;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_AsyncCancel( teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_AsyncCancel"));

  tsp00_Uint4                 ReqDataLen;
  bool                        Ok;
  char *                      PacketBuffer[MIN_PACKET_BUFFER_SIZE];

  // --- Create and send a buffer size request
  //
  teo44_CancelReq    CancelReq;

  Ok = CancelReq.eo42_FillPacketData ( eo45_CalcPacketDataBuffer( PacketBuffer ),
                                       eo45_CalcPacketDataBufferSize( MIN_PACKET_BUFFER_SIZE ),
                                       ReqDataLen, pEventList );
  if ( Ok )
  { // --- send request!
    Ok = eo45_SendAsyncReqRespPacket ( PacketBuffer, ReqDataLen,
                                       CancelReq.eo42_ReqRespType(),  pEventList );
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_AsyncWaitForCancel( teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_AsyncWaitForCancel"));

  tsp00_Uint4                 ReqDataLen;
  bool                        Ok;
  tsp00_Uint1                 ReqType;
  char *                      PacketBuffer[MIN_PACKET_BUFFER_SIZE];
  tsp00_Uint4                 PacketBufferSize = MIN_PACKET_BUFFER_SIZE;

  do
  {
    Ok = eo45_ReceiveAsyncReqRespPacket ( PacketBuffer, PacketBufferSize,
                                          ReqDataLen, ReqType,  pEventList );

  }
  while (( Ok ) && ( ReqType != teo44_CancelReq::eo42_ReqRespType() ));

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_ReceiveRawDataPacket( void * const                PacketBuffer,
                                                       tsp00_Uint4 const           PacketBufferSize,
                                                       tsp00_Uint4                 &DataLen,
                                                       teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_ReceiveRawDataPacket"));

  bool              Ok;
  tsp00_Uint1       ReqType;
  teo41_Swap        Swap;

  Ok = eo45_ReceivePacket( PacketBuffer, PacketBufferSize,
                           DataLen, ReqType, Swap, pEventList );

  while ( Ok && !eo45_ItsARawDataReqResp( ReqType ) )
  {
    // --- handle none data requests!
    Ok = eo45_HandleRequests( PacketBuffer, PacketBufferSize,
                              DataLen, ReqType, Swap, pEventList );

    if ( Ok )
    {
      if ( eo45_ItsADisconnectReq( ReqType ) )
      {
        DataLen = 0;
        break;
      }
      // --- receive the next packet!
      Ok = eo45_ReceivePacket( PacketBuffer, PacketBufferSize,
                               DataLen, ReqType, Swap, pEventList );
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleUnknownResponses( void * const                PacketDataBuffer,
                                                         tsp00_Uint4 const           RespDataLen,
                                                         tsp00_Uint1 const           RespType,
                                                         tsp00_Uint1 const           ExpectedRespType,
                                                         teo41_Swap const            &Swap,
                                                         tsp01_CommErr_Enum * const  pCommState,
                                                         teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleUnknownResponses"));

  if ( eo45_ItsARTEErrorResp( RespType ) )
  {
    // --- extract an  eventlist from packet
    eo45_ExtractRTEErrorRespData ( PacketDataBuffer, RespDataLen, Swap, pCommState, pEventList );
  }
  else
  {
    // --- unknown reponse type
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_INVALID_RESP_TYPE,
                             RespType, ExpectedRespType );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
  }

  return false;
}

/*---------------------------------------------------------------------------*/

void teo45_RTEProtocolBase::eo45_ExtractRTEErrorRespData( void * const                PacketDataBuffer,
                                                          tsp00_Uint4 const           RespDataLen,
                                                          teo41_Swap const            &Swap,
                                                          tsp01_CommErr_Enum * const  pCommState,
                                                          teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_ExtractRTEErrorRespData"));

  bool                      Ok;
  tsp01_CommErr_Enum        CommState;
  teo42_RTEErrorResp        ErrorResponse;

  Ok = ErrorResponse.eo42_SwapAndExtractPacketData( PacketDataBuffer, RespDataLen, Swap, CommState, pEventList );

  if ( pCommState != NULL )
    *pCommState = CommState;

  return;
}

/*---------------------------------------------------------------------------*/

void teo45_RTEProtocolBase::eo45_SendRTEErrorResp( void * const                PacketBuffer,
                                                   tsp00_Uint4 const           PacketBufferSize,
                                                   teo200_EventList const      &EventList,
                                                   tsp01_CommErr_Enum  const   &CommState )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_SendRTEErrorResp"));
  bool              Ok;
  tsp00_Uint1       RespType;
  tsp00_Uint4       RespDataLen;
  teo200_EventList  TmpEvtLst;

  // --- send an error!
  teo42_RTEErrorResp ErrorResponse;

  RespType = ErrorResponse.eo42_ReqRespType();

  Ok = ErrorResponse.eo42_FillPacketData( eo45_CalcPacketDataBuffer( PacketBuffer ),
                                          eo45_CalcPacketDataBufferSize( PacketBufferSize ),
                                          CommState, EventList, RespDataLen, &TmpEvtLst );

  if ( Ok )
    eo45_SendPacket( PacketBuffer, RespDataLen, RespType, &TmpEvtLst );

  return;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleRequests( void * const                PacketBuffer,
                                                 tsp00_Uint4 const           PacketBufferSize,
                                                 tsp00_Uint4 const           ReqDataLen,
                                                 tsp00_Uint1 const           ReqType,
                                                 teo41_Swap const            &Swap,
                                                 teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleRequests"));

  bool                        Ok;

  if ( eo45_ItsAQueryBufferSizeReq ( ReqType ) && eo45_QueryBufferSizeReqSupp() )
  {
    Ok = eo45_HandleQueryBufferSizeReq ( PacketBuffer, PacketBufferSize,
                                         ReqDataLen, Swap, pEventList );
  }
  else if ( eo45_ItsASetBufferSizeReq( ReqType ) && eo45_SetBufferSizeReqSupp() )
  {
    Ok = eo45_HandleSetBufferSizeReq ( PacketBuffer, PacketBufferSize,
                                       ReqDataLen, Swap, pEventList );
  }
  else if ( eo45_ItsADisconnectReq( ReqType ) )
  {
    Ok = eo45_HandleDisconnectReq ( PacketBuffer, PacketBufferSize,
                                    ReqDataLen, Swap, pEventList );
  }
  else if ( eo45_ItsARTEErrorResp( ReqType ) )
  {
    // --- extract an  eventlist from packet
    eo45_ExtractRTEErrorRespData ( eo45_CalcPacketDataBuffer( PacketBuffer ),
                                   ReqDataLen, Swap, pEventList );
    Ok = false;
  }
  else
    Ok = eo45_HandleDerivedClassRequests ( PacketBuffer, PacketBufferSize, ReqDataLen,
                                           ReqType, Swap, pEventList );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleQueryBufferSizeReq( void * const                PacketBuffer,
                                                           tsp00_Uint4 const           PacketBufferSize,
                                                           tsp00_Uint4 const           ReqDataLen,
                                                           teo41_Swap const            &Swap,
                                                           teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleQueryBufferSizeReq"));

  bool                        Ok;
  tsp00_Uint4                 MaxTransferBufferSize;
  tsp00_Uint4                 TransferBufferSize;
  tsp00_Uint4                 MinResponsePacketSize;
  teo200_EventList            EventList;
  tsp00_Uint4                 RespDataLen;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  teo44_QueryTransferBufferSizeReq    QueryTransferBufferSizeReq;

  // --- swap and extract the request
  Ok = QueryTransferBufferSizeReq.eo42_SwapAndExtractPacketData( PacketDataBuffer, ReqDataLen, Swap, pEventList );

  if ( Ok )
    MaxTransferBufferSize = QueryTransferBufferSizeReq.eo44_Params().MaxTransferBufferSize_eo44;

  Ok = eo45_OnQueryBufferSize( MaxTransferBufferSize, TransferBufferSize, MinResponsePacketSize, &EventList );

  if ( Ok )
  {
     teo44_QueryTransferBufferSizeResp    QueryTransferBufferSizeResp;

     QueryTransferBufferSizeResp.eo44_Params().TransferBufferSize_eo44    = TransferBufferSize;
     QueryTransferBufferSizeResp.eo44_Params().MinResponsePacketSize_eo44 = MinResponsePacketSize;

     Ok = QueryTransferBufferSizeResp.eo42_FillPacketData( PacketDataBuffer, PacketDataBufferSize,
                                                           RespDataLen, pEventList );
     if ( Ok )
       Ok = eo45_SendPacket( PacketBuffer, RespDataLen,
                             teo44_QueryTransferBufferSizeResp::eo42_ReqRespType(),
                             pEventList );
  }
  else   // --- send an error resonse !
  {
    eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize, EventList );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EventList );
  }


  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleSetBufferSizeReq( void * const                PacketBuffer,
                                                         tsp00_Uint4 const           PacketBufferSize,
                                                         tsp00_Uint4 const           ReqDataLen,
                                                         teo41_Swap const            &Swap,
                                                         teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleSetBufferSizeReq"));

  bool                        Ok;
  tsp00_Uint4                 TransferBufferSize;
  teo200_EventList            EventList;
  tsp00_Uint4                 RespDataLen;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  // --- we received a request
  //
  teo44_SetTransferBufferSizeReq     SetTransferBufferSizeReq;

  // --- swap and extract the request
  Ok = SetTransferBufferSizeReq.eo42_SwapAndExtractPacketData( PacketDataBuffer, ReqDataLen, Swap, pEventList );

  if ( Ok )
    TransferBufferSize = SetTransferBufferSizeReq.eo44_Params().TransferBufferSize_eo44;

  Ok = eo45_OnSetBufferSize( TransferBufferSize, &EventList );

  if ( Ok == true )
  {
     teo44_SetTransferBufferSizeResp    SetTransferBufferSizeResp;

     Ok = SetTransferBufferSizeResp.eo42_FillPacketData( PacketDataBuffer, PacketDataBufferSize,
                                                         RespDataLen, pEventList );
     if ( Ok )
       Ok = eo45_SendPacket( PacketBuffer, RespDataLen,
                             teo44_SetTransferBufferSizeResp::eo42_ReqRespType(),
                             pEventList );
  }
  else   // --- send an error resonse !
  {
    eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize, EventList );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EventList );
  }


  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleDisconnectReq( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleDisconnectReq"));

  bool                        Ok;
  teo200_EventList            EventList;


  Ok = eo45_OnDisconnect( &EventList );

  if ( Ok == true )
  {
     tsp00_Uint4             RespDataLen;
     void *                  PacketDataBuffer;
     tsp00_Uint4             PacketDataBufferSize;
     teo44_DisconnectResp    DisconnectResp;
     teo200_EventList        TmpEventList;

     PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
     PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

     Ok = DisconnectResp.eo42_FillPacketData( PacketDataBuffer, PacketDataBufferSize,
                                              RespDataLen, pEventList );
     if ( Ok )
     {
       // --- try to send response but ignore any errors, because
       //     the peer might already be down
       eo45_SendPacket( PacketBuffer, RespDataLen,
                        teo44_DisconnectResp::eo42_ReqRespType(),
                        &TmpEventList );
     }
  }
  else   // --- send an error resonse !
  {
    eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize, EventList );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EventList );
  }

  m_Transfer = NULL; // - We can not send or receive any data from peer!!!!

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo45_RTEProtocolBase::eo45_HandleUnknownReq ( void * const                PacketBuffer,
                                                    tsp00_Uint4 const           PacketBufferSize,
                                                    tsp00_Uint1 const           ReqType,
                                                    teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_HandleUnknownReq"));

  teo200_EventList  EventList( FUNCTION_NAME_MEO60, ERR_COMM_UNKNOWN_REQ_TYPE, ReqType );

  // --- send an error!
  eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize,
                         EventList, commErrUnknownRequest_esp01 );

  return true;
}

/*---------------------------------------------------------------------------*/

bool  teo45_RTEProtocolBase::eo45_Send ( bool const                  AsyncReqResp,
                                         void * const                PacketBuffer,
                                         tsp00_Uint4 const           Length,
                                         tsp00_Uint1 const           ReqRespType,
                                         tsp01_CommErr_Enum const    CommState,    // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
                                         tsp00_Uint4 const           SenderRef,
                                         tsp00_Uint4 const           ReceiverRef,
                                         teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_Send"));

  tsp00_Uint4                           SegLength;
  bool                                  Ok;
  teo42_PacketHeader                    PacketHeader ( PacketBuffer );


  if ( m_Transfer == NULL )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_NOT_CONNECTED );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // -- Fill the PacketHeader which is the first part of a transfered packet or packet segment.
  SegLength = PacketHeader.eo42_Set( ReqRespType, SenderRef, ReceiverRef, Length,
                                     m_ProtocolID, m_ProtocolVersion,
                                     m_Transfer->eo42_TransferUnitBufferSize(),
                                     CommState );

  Ok = eo45_SendPacketSeg ( AsyncReqResp, PacketBuffer, SegLength, pEventList );

  // --- If the transfer unit buffer is to small to hold the whole data,
  //     we have to send the data in multiple segements
  if (( Ok == true ) && ( PacketHeader.eo42_ResidualPacketSegs() > 0 ))
  {
    teo42_PacketHeader  SegPacketHeader;
    char *              SegBuffer = (char*)PacketBuffer;

    do
    {
      SegBuffer += SegLength - PacketHeader.eo42_PacketHeaderBufferSize();

      SegLength = PacketHeader.eo42_NextSendPacketSeg(); // - update send packet counter etc.

      // --- Copy the PacketHeader into the data part in front of the next packet segment.
      //     Save a small data part to hold the header.
      SegPacketHeader.teo42_SetPacketHeaderBuffer ( SegBuffer );
      SegPacketHeader.eo42_SavePacketHeaderBuffer();
      SegPacketHeader.teo42_CopyPacketHeader ( PacketHeader );

      Ok = eo45_SendPacketSeg ( AsyncReqResp, SegBuffer, SegLength, pEventList );

      // - restore the saved data part
      SegPacketHeader.eo42_RestorePacketHeaderBuffer();
    }
    while (( Ok == true ) && ( PacketHeader.eo42_ResidualPacketSegs() > 0 ));
 }

  return Ok;
}


/*---------------------------------------------------------------------------*/

bool  teo45_RTEProtocolBase::eo45_SendPacketSeg( bool const                  AsyncReqResp,
                                                 void * const                SegBuffer,
                                                 tsp00_Uint4 const           SegLength,
                                                 teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_SendPacketSeg"));

  bool             Ok;
  char const *     Buffer     = (char const *)SegBuffer;
  size_t           Len        = SegLength;
  size_t           BytesSend;

  do
  {
    if ( AsyncReqResp )
      Ok = m_Transfer->eo42_SendAsyncReqResp ( Buffer, Len, BytesSend, pEventList );
    else
      Ok = m_Transfer->eo42_RawSend ( Buffer, Len, BytesSend, pEventList );

    Len       -= BytesSend;
    Buffer    += BytesSend;
  }
  while ( Ok && ( Len > 0 ) );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  teo45_RTEProtocolBase::eo45_Receive( bool const                              AsyncReqResp,
                                           void * const                            PacketBuffer,
                                           tsp00_Uint4 const                       PacketBufferSize,
                                           tsp00_Uint4                             &Length,
                                           tsp00_Uint1                             &ReqRespType,
                                           tsp01_CommErr_Enum * const              CommState, // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
                                           tsp00_Uint4 * const                     pSenderRef,
                                           tsp00_Uint4 * const                     pReceiverRef,
                                           teo41_Swap * const                      pSwap,
                                           teo200_EventList * const                pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_Receive"));

  tsp00_Uint4                           SegLength;
  bool                                  Ok;
  teo42_PacketHeader                    PacketHeader( PacketBuffer );

  if ( m_Transfer == NULL )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_NOT_CONNECTED );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  Ok = eo45_ReceivePacketSeg( AsyncReqResp, PacketBuffer, PacketBufferSize,
                              PacketHeader, SegLength, pEventList );
  if ( Ok == false )
    return false;

  // --- do we have to receive more than one packet segment?
  if ( PacketHeader.eo42_ResidualPacketSegs() > 0 )
  {
    teo42_PacketHeader  SegPacketHeader;
    char *           SegBuffer     = (char*)PacketBuffer;
    tsp00_Uint4      SegBufferSize = PacketBufferSize;
    tsp00_Uint4      DataLength;

    do
    {
      DataLength     = SegLength - PacketHeader.eo42_PacketHeaderBufferSize();
      SegBuffer     += DataLength;
      SegBufferSize -= DataLength;

      SegPacketHeader.teo42_SetPacketHeaderBuffer ( SegBuffer );
      SegPacketHeader.eo42_SavePacketHeaderBuffer();

      Ok = eo45_ReceivePacketSeg( AsyncReqResp, SegBuffer, SegBufferSize,
                                  SegPacketHeader, SegLength, pEventList );

      if ( Ok == false )
        return false;

      // - restore the saved data part
      PacketHeader.teo42_CopyPacketHeader ( SegPacketHeader );
      SegPacketHeader.eo42_RestorePacketHeaderBuffer();
    }
    while ( PacketHeader.eo42_ResidualPacketSegs() > 0 );
  }

  // --- evaluate packet header information!
  //
  teo42_ProtocolID    ProtocolID;
  tsp00_Uint1         ProtocolVersion;

  Ok = PacketHeader.eo42_Get ( &ReqRespType, pSenderRef, pReceiverRef, &Length, NULL, CommState,
                               &ProtocolID, &ProtocolVersion, pEventList );

  if ( Ok )
  {
    if ( pSwap != NULL )
      *pSwap = PacketHeader.eo42_SwapObj();

    m_PeerProtocolID      = ProtocolID;
    m_PeerProtocolVersion = ProtocolVersion;
  }

  return Ok;
}


/*---------------------------------------------------------------------------*/

bool  teo45_RTEProtocolBase::eo45_ReceivePacketSeg( bool const                  AsyncReqResp,
                                                    void * const                SegBuffer,
                                                    tsp00_Uint4 const           SegBufferSize,
                                                    teo42_PacketHeader          &PacketHeader,
                                                    tsp00_Uint4                 &SegLength,
                                                    teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo45_RTEProtocolBase::eo45_Receive"));

  bool                                  Ok;
  char *                                TmpSegBuffer;
  tsp00_Uint4                           PacketSegLen;
  size_t                                ReceivedJustNow;
  tsp00_Uint4                           Received;
  tsp00_Uint4                           Remaining;

  ReceivedJustNow = 0;
  Received        = 0;
  PacketSegLen    = (tsp00_Uint4)UNDEF_SP00;
  TmpSegBuffer    = (char*)SegBuffer;
  Remaining       = teo42_PacketHeader::eo42_PacketHeaderBufferSize();

  do
  {
    if ( AsyncReqResp )
      Ok = m_Transfer->eo42_RecvAsyncReqResp ( TmpSegBuffer, (size_t)Remaining, ReceivedJustNow, pEventList );
    else
      Ok = m_Transfer->eo42_RawReceive ( TmpSegBuffer, (size_t)Remaining, ReceivedJustNow, pEventList );

    if ( Ok == true )
    {
      if ( ReceivedJustNow == 0 )
      {
        if ( Received != 0 )
        {
          // We have already received some data.
          teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_CONN_BROKEN );
          COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
        }
        else { /* Connection closed by communication partner. */ }

        return false;
      }

      Received     += (tsp00_Uint4)ReceivedJustNow;
      TmpSegBuffer += (tsp00_Uint4)ReceivedJustNow;

      // --- if we haven't received a complete RTE Header continue read!
      if ( Received  < teo42_PacketHeader::eo42_PacketHeaderBufferSize() )
      {
        Remaining -= (tsp00_Uint4)ReceivedJustNow;
        continue;
      }

      if ( PacketSegLen == (tsp00_Uint4)UNDEF_SP00 )
      {
        Ok = PacketHeader.eo42_CurrPacketSegLen( PacketSegLen, pEventList );

        if (( Ok == true ) && ( SegBufferSize < PacketSegLen ))
        {
          teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_ILL_PACKET_SIZE, PacketSegLen );
          COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );

          return false;
        }
      }

      Remaining = PacketSegLen - Received;
    }
  }
  while (( Ok == true ) && ( Remaining != 0 ));

  if ( Ok )
    SegLength = PacketSegLen;

  return Ok;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/