/*!
  @file           geo45.h
  @author         RaymondR
  @special area   communication protocol
  @brief          This module contains the communication protocol
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



#ifndef GEO45_H
#define GEO45_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo42.h"
#include "geo44.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                        class teo45_RTEProtocolBase                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo45_RTEProtocolBase
   @Description    This base class is used to handle request,response and 
              raw data packets.


                Each packet has the following areas:
                                                                               &lt;code>&lt;pre>
                PacketBuffer       -> +----------------+ +
                PacketHeaderBuffer    |                | |
                                      |  PacketHeader  | PacketHeaderBufferSize
                                      |                | |
                PacketDataBuffer   -> +----------------+ +  PacketBufferSize
                                      .                . |
                                      .                . |
                                      |   PacketData   | |
                                      |  ( Req./Resp.) | PacketDataBufferSize
                                      |                | |
                                      .                . |
                                      +----------------+ + 
                                                                               &lt;br>&lt;/code>&lt;/pre>
               The packet data area contains raw/application, request
               or response data. Each request or response has a special
               data format and must be filled or extracted with its
               own class derived from 'teo42_RTEReqRespBase'. These
               classes should have a methode to get a corresponding
               request/response type information (ReqRespType). The type 
               information is inserted into the packet header and can be
               used by the peer to determine the correct request/response 
               handling.
               The 'teo45_RTEProtocolBase' class has some methods to handle 
               standard requests e.g. eo45_QueryPeerBufferSizes or
               eo45_SetPeerBufferSizes. These methods are sending a request 
               and receiving its response. A send request starts a
               eo45_OnQueryBufferSize/eo45_OnSetBufferSize  call on the
               peer if the peer protocol class was derived from 
               'teo45_RTEProtocolBase' and is waiting for data via
               'eo45_ReceiveRawDataPacket'. 
               Too handle these requests you have to implement the following 
               methods in your derive class:
                                                                               &lt;code>&lt;pre>
                   - eo45_QueryBufferSizeReqSupp
                   - eo45_OnQueryBufferSize        
                   - eo45_SetBufferSizeReqSupp  
                   - eo45_OnSetBufferSize          
                                                                               &lt;/code>&lt;/pre>
               If you not do so, these requests are not supported 
               by this side and will be respond with an error.
               To extend your protocol with your own requests the
               derived class should implement the 
               'eo45_HandleDerivedClassRequests' methode. This 
               methode should handle unknown requests by calling
               'eo45_HandleUnknownReq'. If an error occurs you
               can send an error response via 'eo45_SendRTEErrorResp'.
               For your implementation use 'eo45_HandleRequests' 
               as design pattern.
               The buffer containig the packets is names transfer 
               buffer. The transfer buffer holds one or two packets
               depending on whether the send and receive data 
               should hold the same buffer space or not. 
               Keep in mind that the PacketBuffer/PacketHeaderBuffer 
               should have at least a 16 Byte alignment!!!!!!!!!!
               teo45_RTEProtocolBase has no base class.

 */

class teo45_RTEProtocolBase
{
public:
  /*!
     @Description    Set the transfer unit used to send/receive packets to/from peer.


                This methode must be called to allow this class a packet transfer
                via a transfer unit (pipe,socket,NI...). The transfer unit class
                must be derived from 'teo42_TransferUnitBase' which supports a raw
                send/receive functionality.
     @param          Transfer [in] - Class derived from 'teo42_TransferUnitBase'
     @Return value   none

   */

  void                eo45_SetTransferUnit          (  teo42_TransferUnitBase     &Transfer )
                                                    { m_Transfer = &Transfer;};

  /*!
     @Description    Returns the minmum buffer space needed to hold a packet.
     @Return value   Minmum buffer space needed.

   */

  tsp00_Uint4         eo45_MinPacketBufferSize      () const
                                                    { return teo42_RTEReqRespBase::eo42_MinPacketDataBufferSize() +
                                                             teo42_PacketHeader::eo42_PacketHeaderBufferSize(); };

  /*!
     @Description    Calculate the data buffer area of a packet.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @Return value   The packets data buffer (PacketDataBuffer).

   */

  void * const        eo45_CalcPacketDataBuffer     ( void * const                PacketBuffer ) const
                                                    { return (void*)((char*)PacketBuffer + 
                                                                     teo42_PacketHeader::eo42_PacketHeaderBufferSize()); };

  /*!
     @Description    Calculate the maximum data buffer size of a packet.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          HeaderCount      [in] - Number of packet header per packet buffer.
     @Return value   Maximum size of the data packet area.

   */

  tsp00_Uint4         eo45_CalcPacketDataBufferSize ( tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           HeaderCount = 1  ) const
                                                    { return PacketBufferSize - 
                                                             ( teo42_PacketHeader::eo42_PacketHeaderBufferSize() * HeaderCount ); };


  /*!
     @Description    Calculate the packet length using the data length.
     @param          DataLen [in] - The length of the packet data.
     @Return value   Length of a packet

   */

  tsp00_Uint4         eo45_CalcPacketLength         ( tsp00_Uint4  const          DataLen ) const
                                                    { return DataLen + 
                                                             teo42_PacketHeader::eo42_PacketHeaderBufferSize(); };

  /*!
     @Description    Send a packet containg raw/application data.


                The packet contains no request/reponse information.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          DataLen [in] - The length of the packet data.
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_SendRawDataPacket        ( void * const                PacketBuffer,
                                                      tsp00_Uint4  const          DataLen,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_SendPacket( PacketBuffer, DataLen, 
                                                                              teo42_RTEReqRespBase::eo42_RawDataReqRespType(), 
                                                                              pEventList );
                                                    };

  /*!
     @Description    Receive a packet containg raw/application data.


                Any packet containing request/reponse information
                will be dispatched via the eo45_HandleRequests methode
                of this class. 'eo45_ReceiveRawDataPacket' only returns
                if a raw data packet arrived.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          DataLen [out] - The length of the packet data.                                         If the data length is zero, the                                         peer has send disconnect request                                         and closed the communication unit.
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_ReceiveRawDataPacket     ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize, 
                                                      tsp00_Uint4                 &DataLen,
                                                      teo200_EventList * const    pEventList = NULL );


  /*!
     @Description    Query the peer transfer buffer size and its minimum
              response packet size.


                This query triggers a eo45_OnQueryBufferSize call on the
                peer if the peer protocol class was derived from 
                'teo45_RTEProtocolBase'.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          MaxTransferBufferSize [in] - Maximum transfer buffer size                                                of this site.
     @param          PeerTransferBufferSize [out] - The peer transfer buffer
     @param          PeerMinResponsePacketSize [out] - The peer minimum response                                                packet size. This meens that                                                this side should only send packets                                                smaller than 'PeerTransferBufferSize'                                                minus 'PeerMinResponsePacketSize'.
     @param          pEventList [inout] - The event list to be used,                                                if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_QueryPeerBufferSizes     ( void *  const               PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize, 
                                                      tsp00_Uint4                 &PeerTransferBufferSize,
                                                      tsp00_Uint4                 &PeerMinResponsePacketSize,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_QueryPeerBufferSizes ( PacketBuffer, PacketBufferSize, 
                                                                                        (tsp00_Uint4)UNDEF_SP00, 
                                                                                         PeerTransferBufferSize,
                                                                                         PeerMinResponsePacketSize, 
                                                                                         pEventList );
                                                    }

  bool                eo45_QueryPeerBufferSizes     ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize, 
                                                      tsp00_Uint4 const           MaxTransferBufferSize,
                                                      tsp00_Uint4                 &PeerTransferBufferSize,
                                                      tsp00_Uint4                 &PeerMinResponsePacketSize,
                                                      teo200_EventList * const    pEventList = NULL );


  /*!
     @Description    Set the transfer buffer size of peer and this site.


                This methode triggers a eo45_OnSetBufferSize call on the
                peer if the peer protocol class was derived from 
                'teo45_RTEProtocolBase'.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          TransferBufferSize [in] - Transfer buffer size                                           of this site.
     @param          pEventList [inout] - The event list to be used,                                           if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_SetPeerBufferSizes       ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize, 
                                                      tsp00_Uint4 const           TransferBufferSize,
                                                      teo200_EventList * const    pEventList = NULL );


  /*!
     @Description    Send disconnect to peer.


                This methode triggers a eo45_OnDisconnect call on the
                peer if the peer protocol class was derived from 
                'teo45_RTEProtocolBase'.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          pEventList [inout] - The event list to be used,                                           if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_DisconnectFromPeer       ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      teo200_EventList * const    pEventList );


  /*!
     @Description    Send a asyncronus cancel request to peer.


                This methode trigges the peer thread/proccess which
                is waiting for a cancel request via 'eo45_AsyncWaitForCancel'.
     @param          pEventList [inout] - The event list to be used,                                           if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_AsyncCancel              ( teo200_EventList * const    pEventList = NULL );
  bool                eo45_AsyncWaitForCancel       ( teo200_EventList * const    pEventList = NULL );

protected:
  /*!
     @Description    Send/Receive a packet.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          Length [in/out] - The length of the packet.
     @param          ReqRespType [in/out] - The request or reponse type                                      which defines the content                                      of the data part.
     @param          Swap [out] - Swap object which can be used                                      to swap swap the received data                                       requests/responds.
     @param          DataLen [in] - The length of the packet data.
     @param          PacketLen [out] - The length of the packet.
     @param          pEventList [inout] - The event list to be used,                                      if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_SendPacket               ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           Length,
                                                      tsp00_Uint1 const           ReqRespType,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_Send ( false, PacketBuffer,Length, ReqRespType, 
                                                                         commErrOk_esp01, (tsp00_Uint4)UNDEF_SP00, 
                                                                         (tsp00_Uint4)UNDEF_SP00, pEventList );
                                                    };
  bool                eo45_SendPacket               ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           Length,
                                                      tsp00_Uint1 const           ReqRespType,
                                                      tsp00_Uint4 const           SenderRef,
                                                      tsp00_Uint4 const           ReceiverRef,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_Send ( false, PacketBuffer, Length, ReqRespType, 
                                                                         commErrOk_esp01, SenderRef, ReceiverRef, 
                                                                         pEventList );
                                                    };
  bool                eo45_ReceivePacket            ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      teo41_Swap                  &Swap,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( false, PacketBuffer, PacketBufferSize, Length, 
                                                                             ReqRespType, NULL, NULL, NULL, &Swap, pEventList );
                                                    };
  bool                eo45_ReceivePacket            ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      tsp00_Uint4                 &SenderRef,
                                                      tsp00_Uint4                 &ReceiverRef,
                                                      teo41_Swap                  &Swap,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( false, PacketBuffer, PacketBufferSize, Length, 
                                                                             ReqRespType, NULL, &SenderRef, &ReceiverRef, &Swap, pEventList );
                                                    };
  bool                eo45_ReceivePacket            ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( false, PacketBuffer, PacketBufferSize, 
                                                                             Length, ReqRespType, NULL, NULL, NULL, NULL, pEventList );
                                                    };
  bool                eo45_ReceivePacket            ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      tsp00_Uint4                 &SenderRef,
                                                      tsp00_Uint4                 &ReceiverRef,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( false, PacketBuffer, PacketBufferSize, 
                                                                             Length, ReqRespType, NULL, &SenderRef, &ReceiverRef, 
                                                                             NULL, pEventList );
                                                    };


  bool                eo45_XferPacket               ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           SendLength,
                                                      tsp00_Uint1 const           SendReqRespType,
                                                      tsp00_Uint4                 &RecvLength,
                                                      tsp00_Uint1                 &RecvReqRespType,
                                                      teo41_Swap                  &Swap,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      bool Ok = eo45_Send ( false, PacketBuffer,SendLength, SendReqRespType, 
                                                                            commErrOk_esp01, (tsp00_Uint4)UNDEF_SP00, 
                                                                            (tsp00_Uint4)UNDEF_SP00, pEventList );
                                                      if ( Ok )
                                                        Ok = eo45_Receive ( false, PacketBuffer, PacketBufferSize, RecvLength, 
                                                                            RecvReqRespType, NULL, NULL, NULL, &Swap, pEventList );
                                                      return Ok;
                                                    };

  /*!
     @Description    Send/Receive a request/response packet asyncronously to
              eo45_SendPacket/eo45_ReceivePacket.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          Length [in/out] - The length of the packet.
     @param          ReqRespType [in/out] - The request or reponse type                                      which defines the content                                      of the data part.
     @param          Swap [out] - Swap object which can be used                                      to swap swap the received data                                       requests/responds.
     @param          DataLen [in] - The length of the packet data.
     @param          PacketLen [out] - The length of the packet.
     @param          pEventList [inout] - The event list to be used,                                      if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_SendAsyncReqRespPacket   ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           Length,
                                                      tsp00_Uint1 const           ReqRespType,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_Send ( true, PacketBuffer, Length, ReqRespType, 
                                                                         commErrOk_esp01, (tsp00_Uint4)UNDEF_SP00, 
                                                                         (tsp00_Uint4)UNDEF_SP00, pEventList );
                                                    };
  bool                eo45_SendAsyncReqRespPacket   ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           Length,
                                                      tsp00_Uint1 const           ReqRespType,
                                                      tsp00_Uint4 const           SenderRef,
                                                      tsp00_Uint4 const           ReceiverRef,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_Send ( true, PacketBuffer, Length, ReqRespType, 
                                                                         commErrOk_esp01, SenderRef, ReceiverRef, 
                                                                         pEventList );
                                                    };
  bool                eo45_ReceiveAsyncReqRespPacket( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      teo41_Swap                  &Swap,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( true, PacketBuffer, PacketBufferSize, 
                                                                             Length, ReqRespType, NULL, NULL, NULL, &Swap, 
                                                                             pEventList );
                                                    };
  bool                eo45_ReceiveAsyncReqRespPacket( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( true, PacketBuffer, PacketBufferSize, 
                                                                             Length, ReqRespType, NULL, NULL, NULL, NULL, 
                                                                             pEventList );
                                                    };
  bool                eo45_ReceiveAsyncReqRespPacket( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      tsp00_Uint4                 &SenderRef,
                                                      tsp00_Uint4                 &ReceiverRef,
                                                      teo41_Swap                  &Swap,
                                                      teo200_EventList * const    pEventList = NULL )

                                                    {
                                                      return  eo45_Receive ( true, PacketBuffer, PacketBufferSize, 
                                                                             Length, ReqRespType, NULL, &SenderRef, &ReceiverRef, 
                                                                             &Swap, pEventList );
                                                    };
protected:                                                   

  /*!
     @Description    On request functions


                A eo45_QueryPeerBufferSizes or eo45_SetPeerBufferSizes 
                call of the peer is triggering a
                eo45_OnQueryBufferSize/eo45_OnSetBufferSize call 
                on this side if its waiting for data via
                'eo45_ReceiveRawDataPacket'.

   */

  virtual bool        eo45_QueryBufferSizeReqSupp   () const { return false; };
  virtual bool        eo45_OnQueryBufferSize        ( tsp00_Uint4  const          MaxTransferBufferSize,
                                                      tsp00_Uint4                 &TransferBufferSize,
                                                      tsp00_Uint4                 &MinResponsePacketSize,
                                                      teo200_EventList * const    pEventList = NULL ) { return false; };
                                                             

  virtual bool        eo45_SetBufferSizeReqSupp     () const { return false; };
  virtual bool        eo45_OnSetBufferSize          ( tsp00_Uint4 const           TransferBufferSize,
                                                      teo200_EventList * const    pEventList = NULL ) { return false; };

  /*!
     @Description    Handle disconnect request


                A eo45_DisconnectFromPeer call of the peer is triggering a
                eo45_OnDisconnect call on this side if its waiting for 
                data via 'eo45_ReceiveRawDataPacket'.

   */

  virtual bool        eo45_OnDisconnect             ( teo200_EventList * const    pEventList = NULL ) { return true; };



  /*!
     @Description    Handle the requests of the derived class


                To extend your protocol with your own requests the
                derived class should implement the 
                'eo45_HandleDerivedClassRequests' methode. This 
                methode should handle unknown or not allowed requests 
                by calling 'eo45_HandleUnknownReq'.
                For your implementation use 'eo45_HandleRequests' 
                as design pattern.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          ReqDataLen [in] - The length of the request data.
     @param          ReqType [in] - The request type which defines t                                      the content of the data part.
     @param          Swap [in] - Swap object which can be used                                      to swap swap the received data                                       requests/responds.
     @param          pEventList [inout] - The event list to be used,                                      if NULL throw EventList on error.
     @Return value   true if ok

   */

  virtual bool        eo45_HandleDerivedClassRequests(void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      tsp00_Uint1 const           ReqType,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL )
                                                    {
                                                      return eo45_HandleUnknownReq ( PacketBuffer, PacketBufferSize, 
                                                                                     ReqType, pEventList );
                                                    };

  /*!
     @Description    Send an error response


                If an error occurs send an error response packet
                which is filled with an event list.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          EventList [in] - Event list holding the error                                      which should be send to peer.
     @param          CommState [in] - Communication state.
     @Return value   none

   */


  void                eo45_SendRTEErrorResp         ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      teo200_EventList const      &EventList,
                                                      tsp01_CommErr_Enum  const   &CommState );

  void                eo45_SendRTEErrorResp         ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      teo200_EventList const      &EventList )
                                                    {
                                                      eo45_SendRTEErrorResp ( PacketBuffer, PacketBufferSize, EventList,
                                                                              commErrNotOk_esp01 );
                                                    };

  /*!
     @Description    Handle unknown or not supported requests.


                This function should be called by 
                'eo45_HandleDerivedClassRequests' if a request is 
                unknown or not supported.
     @param          PacketBuffer [in] - Pointer to the packet buffer.
     @param          PacketBufferSize [in] - Packet buffer size.
     @param          ReqType [in] - The request type which defines t                                      the content of the data part.
     @param          pEventList [inout] - The event list to be used,                                      if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo45_HandleUnknownReq         ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint1 const           ReqType,
                                                      teo200_EventList * const    pEventList = NULL );

protected:
  /*!
     @Description    Set the protocol id an its version.
     @param          ProtocolID [in] - Protocol ID like TCP, NI, Pipe ...
     @param          ProtocolVersion [in] - Version number
     @Return value   none

   */

  virtual void        eo45_SetProtocolID            ( teo42_ProtocolID const      ProtocolID      )
                                                    { m_ProtocolID = ProtocolID; };
                                                    
  virtual void        eo45_SetProtocolVersion       ( tsp00_Uint1 const           ProtocolVersion )
                                                    { m_ProtocolVersion = ProtocolVersion; };

  /*!
     @Description    Get the protocol id an its version of the peer.
     @Return value   ProtocolID       - Protocol ID like TCP, NI, Pipe ...
              ProtocolVersion  - Version number

   */

  virtual teo42_ProtocolID eo45_GetPeerProtocolID   () const
                                                    { return m_PeerProtocolID; };

  virtual tsp00_Uint1 eo45_GetPeerProtocolVersion   () const
                                                    { return m_PeerProtocolVersion; };

protected:                                                   
  bool                eo45_HandleRequests           ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      tsp00_Uint1 const           ReqType,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );

  bool                eo45_ItsARawDataReqResp       ( tsp00_Uint1 const           RespType ) const
                                                    { return RespType == teo42_RTEReqRespBase::eo42_RawDataReqRespType();};

  bool                eo45_ItsASetBufferSizeReq     ( tsp00_Uint1 const           ReqType ) const
                                                    { return ReqType == teo44_SetTransferBufferSizeReq::eo42_ReqRespType();};
                                                             
  bool                eo45_ItsAQueryBufferSizeReq   ( tsp00_Uint1 const           ReqType ) const
                                                    { return ReqType == teo44_QueryTransferBufferSizeReq::eo42_ReqRespType();};
                                                             
  bool                eo45_ItsADisconnectReq        ( tsp00_Uint1 const           ReqType ) const
                                                    { return ReqType == teo44_DisconnectReq::eo42_ReqRespType();};

  bool                eo45_ItsARTEErrorResp         ( tsp00_Uint1 const           RespType ) const
                                                    { return RespType == teo42_RTEErrorResp::eo42_ReqRespType();};

  bool                eo45_HandleSetBufferSizeReq   ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );
                                                             
  bool                eo45_HandleQueryBufferSizeReq ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );
                                                             
  bool                eo45_HandleDisconnectReq      ( void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4 const           ReqDataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );
                                                             
  void                eo45_ExtractRTEErrorRespData  ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           RespDataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL ) const
                                                    {
                                                      eo45_ExtractRTEErrorRespData( PacketDataBuffer, RespDataLen, Swap, NULL, pEventList );
                                                    };

  void                eo45_ExtractRTEErrorRespData  ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           RespDataLen,
                                                      teo41_Swap const            &Swap,
                                                      tsp01_CommErr_Enum * const  pCommState,
                                                      teo200_EventList * const    pEventList = NULL ) const;

  bool                eo45_HandleUnknownResponses   ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           RespDataLen,
                                                      tsp00_Uint1 const           RespType,
                                                      tsp00_Uint1 const           ExpectedRespType,
                                                      teo41_Swap const            &Swap,
                                                      tsp01_CommErr_Enum * const  pCommState,
                                                      teo200_EventList * const    pEventList = NULL ) const;

private:
  bool                eo45_Send                     ( bool const                  AsyncReqResp,
                                                      void * const                PacketBuffer,
                                                      tsp00_Uint4 const           Length,
                                                      tsp00_Uint1 const           ReqRespType,
                                                      tsp01_CommErr_Enum const    CommState,
                                                      tsp00_Uint4 const           SenderRef,
                                                      tsp00_Uint4 const           ReceiverRef,
                                                      teo200_EventList * const    pEventList = NULL );

  bool                eo45_Receive                  ( bool const                  AsyncReqResp,
                                                      void * const                PacketBuffer,
                                                      tsp00_Uint4 const           PacketBufferSize,
                                                      tsp00_Uint4                 &Length,
                                                      tsp00_Uint1                 &ReqRespType,
                                                      tsp01_CommErr_Enum * const  CommState,
                                                      tsp00_Uint4 * const         pSenderRef,
                                                      tsp00_Uint4 * const         pReceiverRef,
                                                      teo41_Swap * const          pSwap,
                                                      teo200_EventList * const    pEventList );

  bool                eo45_SendPacketSeg            ( bool const                  AsyncReqResp,
                                                      void * const                SegBuffer,
                                                      tsp00_Uint4 const           SegLength,
                                                      teo200_EventList * const    pEventList );

  bool                eo45_ReceivePacketSeg         ( bool const                  AsyncReqResp,
                                                      void * const                SegBuffer,
                                                      tsp00_Uint4 const           SegBufferSize,
                                                      teo42_PacketHeader          &PacketHeader,
                                                      tsp00_Uint4                 &SegLength,
                                                      teo200_EventList * const    pEventList );

protected:
                      teo45_RTEProtocolBase         ()
                        : m_ProtocolID( Undefined_ee042 ),
                          m_ProtocolVersion( (tsp00_Uint1)UNDEF_SP00 ),
                          m_PeerProtocolID( Undefined_ee042 ),
                          m_PeerProtocolVersion( (tsp00_Uint1)UNDEF_SP00 ),
                          m_Transfer(NULL)             {;};

  virtual             ~teo45_RTEProtocolBase        () {;};


private:
  teo42_ProtocolID                m_ProtocolID;
  tsp00_Uint1                     m_ProtocolVersion;
                             
  teo42_ProtocolID                m_PeerProtocolID;
  tsp00_Uint1                     m_PeerProtocolVersion;

  teo42_TransferUnitBase *        m_Transfer;
};




/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GEO45_H */
