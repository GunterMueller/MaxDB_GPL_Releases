/*!
  @file           geo42.h
  @author         RaymondR
  @special area   communication base classes
  @brief          This module contains the common communication (base) classes.
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


                                        
#ifndef GEO42_H
#define GEO42_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo003.h"
#include "geo60.h"
#include "geo200.h"
#include "geo007_1.h"
#include "gsp00.h"
#include "geo41.h"
#include "RunTime/Communication/RTEComm_PacketHeader.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// --- Use the following define for nested private structs or unions declared in classes.
//     It's a workaround for a scope error "private member ... cannot be accessed."
#if defined (_WIN32)    
# define  PRIVATE_NESTED_STRUCTS private
#else
# define  PRIVATE_NESTED_STRUCTS public
#endif


#define  MIN_PACKET_BUFFER_SIZE                8196

// --- request/response types ( 1 - 32 ) reserved by geo42.h  
#define REQ_RESP_TYPE_RAW_DATA                 1
#define REQ_RESP_TYPE_EVENTLIST                2


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------
   Declaration:  teo42_ProtocolID (obsolete, see: RTEComm_Header.h)
   Description:  Some type need be the following interfaces
*/
typedef RTEComm_ProtocolID teo42_ProtocolID;

#define Undefined_ee042   Prot_Undefined
#define DBManager_eeo42   Prot_DBManager
#define ConsoleSM_ee042   Prot_ConsoleSM


/* EndDeclaration */

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class teo42_TransferUnitBase                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_TransferUnitBase
   @Description    Send and receive base class.


                This base class is used to implement classes with uniform 
                send and receive interface providing a connection-based 
                byte-stream service.
                teo42_TransferUnitBase does not have a base class.

 */

class teo42_TransferUnitBase
{
public:

  /*!
     @Description    Buffer size of the transfer unit 
              (e.g. socket buffer size, pipe buffer size).


                This method returns the the internal buffer size of the 
                transfer unit. This is typically the maximum length of 
                data which should be transfered per send operation.
                maximum size of the data 
                which should be transfered per send operation.
                This is typically the internal buffer of the 
                transfer unit (socket buffer size, pipe buffer size). 
     @Return value   maximum packet size

   */

  virtual tsp00_Uint4 const  eo42_TransferUnitBufferSize () const { return (tsp00_Uint4)UNDEF_SP00; };

  /*!
     @Description    Send data to a specific destination.
     @param          SendBuffer [in] - A buffer containing the data to be transmitted.
     @param          SendLen [in] - The length of the data in SendBuffer.
     @param          BytesSend [out] - Number of bytes send.
     @param          pEventList [inout] - The event list to be used,                                       if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_RawSend            ( void const * const            SendBuffer,
                                              size_t const                  SendLen,
                                              size_t                        &BytesSend,
                                              teo200_EventList * const      pEventList = NULL ) = 0;

  /*!
     @Description    Receive data from a specific destination.
     @param          RecvBuffer [inout] - A buffer for the incoming data.              RecvBufferSize[in]      - The size of RecvBuffer.
     @param          BytesReceived [out] - Number of bytes received.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */


  virtual bool      eo42_RawReceive         ( void * const                  RecvBuffer,
                                              size_t const                  RecvBufferSize,
                                              size_t                        &BytesReceived,
                                              teo200_EventList * const      pEventList = NULL ) = 0;


  /*!
     @Description    Sends a request/response to a specific destination asyncronously to
              'eo42_RawSend'. The request/response can only be received with 
              'eo42_RecvAsyncReqResp'. The methode is usefull for cancel, 
              kill and dump resquest implementations.
     @param          SendBuffer [in] - A buffer containing the data to be transmitted.
     @param          SendLen [in] - The length of the data in SendBuffer.
     @param          BytesSend [out] - Number of bytes send.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_SendAsyncReqResp   ( void const * const            SendBuffer,
                                              size_t const                  SendLen,
                                              size_t                        &BytesSend,
                                              teo200_EventList * const      pEventList = NULL ) = 0;

  /*!
     @Description    Receives a request/response from a specific destination, which
              was send with 'eo42_SendAsyncReqResp'.
     @param          RecvBuffer [inout] - A buffer for the incoming data.              RecvBufferSize[in]      - The size of RecvBuffer.
     @param          BytesReceived [out] - Number of bytes received.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_RecvAsyncReqResp   ( void * const                  RecvBuffer,
                                              size_t const                  RecvBufferSize,
                                              size_t                        &BytesReceived,
                                              teo200_EventList * const      pEventList = NULL ) = 0;
protected:
                    teo42_TransferUnitBase()  {;};
  virtual           ~teo42_TransferUnitBase() {;};
};




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo42_PacketHeader                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_PacketHeader
   @Description    Interface class to create, read and modify the 
              packet header. (obsolete, see: RTEComm_Header.h)


                The 'PacketHeader is the first part of each packet indicating
                the kind of packet data. It also contains length information
                to reduce the chance of data truncation going undetected.
                Each transfered packet should look like this:                   
                                                                                &lt;code>&lt;pre>
                 PacketBuffer/      ->+----------------+
                 PacketHeaderBuffer   |                |
                                      |  PacketHeader  |
                                      |                |
                 PacketDataBuffer   ->+----------------+
                                      .                .
                                      .                .
                                      |   PacketData   |
                                      .                .
                                      .                .
                                      +----------------+
                                                                                    &lt;br>
                                                                                &lt;/code>&lt;/pre>
                The PacketHeader class hides the local and peer swap type. 
                To swap values outside of this header class use the method 
                'eo42_SwapObj'. 
                The PacketBuffer/PacketHeaderBuffer should have at least 
                a 16 Byte alignment!
                teo42_PacketHeader is derived (private) from teo41_Swap.

 */

class teo42_PacketHeader :  private teo41_Swap
{
private:
  typedef struct 
  {
    tsp00_Uint4                           CurrPacketSegLen;
    tsp00_Uint1                           ProtocolID;
    tsp00_Uint1                           ReqRespType; 
    tsp00_Uint1                           ProtocolVersion;
    tsp00_Uint1                           ResidualPacketSegs;
    tsp00_Uint4                           SenderRef;
    tsp00_Uint4                           ReceiverRef;
    tsp00_Uint2                           CommState; // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
    tsp00_Uint1                           SwapType;
    tsp00_Uint1                           Filler;
    tsp00_Uint4                           MaxSendLen;

    // - Use the following define only for static buffer allocations,
    //   otherwise use the 'eo42_PacketHeaderBufferSize' methode.
#   define PACKET_HEADER_BUFFER_SIZE_EO42     24

    // Should be 8-byte aligned to avoid alignment computations.
    //
    // CurrPacketSegLen    Current packet segment length includes the PacketHeader.
    // MaxSendLen          Includes one PacketHeader regardless of the number
    //                     of packet segments. This means, that the sum of all
    //                     segment's CurrPacketSegLen is greater than
    //                     MaxSendLen unless the number of segments is 1.
    // ProtocolID          Type of protocol ( e.g. TCP/IP, Pipe ... )
    // ReqRespType         This Identifier specifies the packet contents.
    //                     ( e.g. connect, connect response, cancel, release, data, data response ...)
    // ResidualPacketSegs  Number of packet segments  remaining.
    // SenderRef           Sender reference 
    // ReceiverRef         Receiver reference
    // CommState           Communication state
    
  } teo42_PacketHeaderRecord; 

public:
  /*!
     @Description    Create a PacketHeader interface object.
     @param          PacketHeaderBuffer [in] - Pointer to a buffer receiving the PacketHeader.
     @Return value   none

   */

                    teo42_PacketHeader         ( void * const                  PacketHeaderBuffer = NULL )
                                               : m_PacketHeaderBuffer((teo42_PacketHeaderRecord *)PacketHeaderBuffer),
                                                 m_MaxDataLen   (0),
                                                 m_DataRemaining(0) { ; };

  /*!
     @Description    Set the PacketHeader buffer.
     @param          PacketHeaderBuffer [in] - Pointer to a buffer receiving the PacketHeader.
     @Return value   none

   */

  void              teo42_SetPacketHeaderBuffer( void * const                  PacketHeaderBuffer )
                                               { m_PacketHeaderBuffer = (teo42_PacketHeaderRecord *)PacketHeaderBuffer; }

  /*!
     @Description    Copy the PacketHeader buffer.
     @param          PacketHeader [in] - Source PacketHeader
     @Return value   none

   */

  void              teo42_CopyPacketHeader     ( teo42_PacketHeader const         &PacketHeader );
                                            


  /*!
     @Description    Returns the buffer size needed to hold the PacketHeader.
     @Return value   The buffer size needed to hold the PacketHeader.

   */

  static tsp00_Uint4 const  eo42_PacketHeaderBufferSize();

  


  /*!
     @Description    Build a new PacketHeader.


                If a packet must be transfered with more than one send 
                operation you have to set the header values with 'eo42_Set'
                only once. After each send you must call 'eo42_NextSendPacketSeg'
                which decreases the number of residual packet segments and 
                calculates the send length for the next packet segment.
                The 'eo42_ResidualPacketSegs' methode can be used to get the
                number of residual packet segments.
     @param          ReqRespType [in] - This Identifier specifies the packet contents.                                         ( e.g. connect, connect response, cancel, release, data, data response ...)
     @param          SenderRef [in] - Sender reference.
     @param          ReceiverRef [in] - Receiver reference.
     @param          DataLen [in] - Total length of data.
     @param          MaxPacketLen [in] - Maximum packet length per send/receive  (including PacketHeader).
     @param          CommState [in] - Communication state.
     @param          ProtocolID [in] - Type of protocol ( e.g. TCP/IP, Pipe ... )
     @param          ProtocolVersion [in] - Version of Protocol
     @Return value   The send length for the next packet segment ( including PacketHeader space )

   */

  tsp00_Uint4 const eo42_Set                   ( tsp00_Uint1  const                          ReqRespType,
                                                 tsp00_Uint4  const                          SenderRef,
                                                 tsp00_Uint4  const                          ReceiverRef,
                                                 tsp00_Uint4  const                          DataLen,
                                                 teo42_ProtocolID const                      ProtocolID,
                                                 tsp00_Uint1 const                           ProtocolVersion,
                                                 tsp00_Uint4 const                           MaxPacketLen  = (tsp00_Uint4)UNDEF_SP00,
                                                 tsp01_CommErr_Enum const                    CommState     = commErrOk_esp01 );  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!

  /*!
     @Description    Get current PacketHeader information.
     @param          ReqRespType [out] - This Identifier specifies the packet contents.                                         ( e.g. connect, connect response, cancel, release, data, data response ...)
     @param          SenderRef [out] - Sender reference.
     @param          ReceiverRef [out] - Receiver reference.
     @param          DataLen [out] - Total length of data.
     @param          ActDataLen [out] - Data length of the current packet.
     @param          CommState [out] - Communication state.
     @param          ProtocolID [out] - Type of protocol ( e.g. TCP/IP, Pipe ... )
     @param          ProtocolVersion [out] - Protocol Version
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool               eo42_Get                  ( tsp00_Uint1 * const                           pReqRespType,
                                                 tsp00_Uint4 * const                           pSenderRef,
                                                 tsp00_Uint4 * const                           pReceiverRef,
                                                 tsp00_Uint4 * const                           pDataLen,
                                                 tsp00_Uint4 * const                           pActDataLen,
                                                 tsp01_CommErr_Enum * const                    pCommState,    // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
                                                 teo42_ProtocolID * const                      pProtocolID,
                                                 tsp00_Uint1 * const                           pProtocolVersion,
                                                 teo200_EventList * const                      pEventList = NULL );

  /*!
     @Description    Get number of remainig packet segments.
     @Return value   Number of packet segments remaining.

   */

  tsp00_Uint1 const eo42_ResidualPacketSegs    () const
                                               { return m_PacketHeaderBuffer->ResidualPacketSegs; } ;

  /*!
     @Description    Get the total length of the current packet segment ( including PacketHeader space ).
     @param          PacketSegLength [out] - The total length of the current                                          packet segment ( including PacketHeader ).
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool             eo42_CurrPacketSegLen       ( tsp00_Uint4                    &PacketSegLength,
                                                 teo200_EventList * const       pEventList = NULL );

  /*!
     @Description    Update header for the next send operation.


                If a packet must be transfered with more than one send 
                operation you have to set the header values with 'eo42_Set'
                only once. After each send you must call 'eo42_NextSendPacketSeg'
                which decreases the number of residual packet segments and 
                calculates the actual send length for the next packet.
                The 'eo42_ResidualPacketSegs' methode can be used to get the
                number of residual packet segments.
     @Return value   The send length for the next packet ( including PacketHeader space ).

   */

  tsp00_Uint4 const eo42_NextSendPacketSeg     ();

  /*!
     @Description    Save the content of the current PacketHeader buffer.
     @Return value   none.

   */

  void              eo42_SavePacketHeaderBuffer() 
                                               { SAPDB_MemCopyNoCheck(SaveRestoreBuffer, m_PacketHeaderBuffer, sizeof(SaveRestoreBuffer)); };

  /*!
     @Description    Restore the old content of the PacketHeader buffer.
     @Return value   none.

   */

  void              eo42_RestorePacketHeaderBuffer() const
                                               { SAPDB_MemCopyNoCheck(m_PacketHeaderBuffer, SaveRestoreBuffer, sizeof(SaveRestoreBuffer)); };

  /*!
     @Description    Get a 'teo41_Swap' object to swap values outside of this class.


                The PacketHeader class hides the local and peer swap type. 
                To swap values outside of this header class use this method.
                which returns a 'teo41_Swap' object ('swap_object'). 
                A swap can be done by calling:                            &lt;br>
                                                                          &lt;br>
                 'swap_object' = 'header_object'.eo42_SwapObj();         &lt;br>
                                                                          &lt;br>
                 'swap_object'.eo42_SwapValue('my_value1');               &lt;br>
                 'swap_object'.eo42_SwapValue('my_value2');               &lt;br>
                 'swap_object'.eo42_SwapValue('my_value3');               &lt;br>
                                                                          &lt;br>
     @Return value   A 'teo41_Swap' object (&lt;swap_object>).

   */

  teo41_Swap        eo42_SwapObj               () const
                                               { return *this; };
private:
  bool              eo42_SwapHeader            ( teo200_EventList * const       pEventList = NULL );

private:
  teo42_PacketHeaderRecord *     m_PacketHeaderBuffer;

  tsp00_Uint4                    m_MaxDataLen;
  tsp00_Uint4                    m_DataRemaining;

  char                           SaveRestoreBuffer[PACKET_HEADER_BUFFER_SIZE_EO42];

};




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                      class teo42_RTEReqRespVarPart                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_RTEReqRespVarPart
   @Description    Interface class to insert and read values of the 
              RTE request/response packet VarPart.


                The 'VarPart is the last part of a RTE request/response packet.
                It can contain several information and is structured 
                as follows:                                     
                                                                                &lt;code>&lt;pre>
                   VarPart: [argument][argument]...                             
                                                                                &lt;br>
                   Each argument is structured as follows:
                                                                                &lt;br>
                   Argument: |id|length|value|
                                                                                &lt;br>
                             'id'          is a one byte character
                             'length'      is a two byte integer value length.
                                           ( network byte order )
                             'value'       is coded argument dependent
                                                                                &lt;br>
                   VarPart: 50 00 02 1b 58 49 00 04 31 32 33 00 (hex)
                            ^  ^     ^     ^  ^     ^
                            |  |     |     |  |     |
                            |  |     |     |  |     Zero terminated 
                            |  |     |     |  |     string ("123")
                            |  |     |     |  Value length 
                            |  |     |     |  (4 bytes: string + \0)
                            |  |     |     Argument id 'I' for remote pid
                            |  |     TCP/IP port number (0x1b58)
                            |  Value length (2 bytes: port number)
                            Argument id 'P' for TCP/IP port number
                                                                                &lt;/code>&lt;/pre>
                   There is no terminator for the VarPart since the length of the
                   request/response packet includes the VarPart and thus specifies its length.

 */


class teo42_RTEReqRespVarPart
{
public:
  /*!
     @Description    Create a teo42_RTEReqRespVarPart interface object.
     @param          VarPart [in] - Pointer to the VarPart buffer.
     @param          VarPartSize [in] - VarPart buffer size.
     @param          VarPartLen [in] - Length of VarPart content. Must be 0                                 VarPart has no content.
     @Return value   none

   */

                      teo42_RTEReqRespVarPart ( void * const       VarPart,
                                                tsp00_Uint2 const  VarPartSize,
                                                tsp00_Uint2 const  VarPartLen = 0,
                                                bool const         OldVarPart = false ) // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
                                      : VarPart_eo42((char*)VarPart),
                                        VarPartSize_eo42(VarPartSize),
                                        VarPartLen_eo42(VarPartLen),
                                        OldVarPart_eo42(OldVarPart) {;};


                      ~teo42_RTEReqRespVarPart() {;};

  /*!
     @Description    Returns the actual VarPart length.
     @Return value   VarPart length.

   */

  tsp00_Uint2         eo42_VarPartLen        ( )  const
                                             { return VarPartLen_eo42; };

  /*!
     @Description    Put 'VarPart' value


                 Here, we have some functions to write VarPart values.
                 They append each value to the end of the value list.
     @param          ID [in] - Unique VarPart identfier.
     @param          Value [in] - Value.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint1 const           Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int2 const            Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint2 const           Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int4 const            Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint4 const           Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int8 const            Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint8 const           Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};

  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       char const * const          Value,
                                       teo200_EventList * const    pEventList  = NULL );
#if defined (_WIN32)
  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       RTE_OSPid const             Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { return PutVarPartValue(ID,&Value,sizeof(Value),pEventList);};
#endif

#if !defined (CPP_BOOL_NOT_IMPLEMENTED)
  bool                eo42_PutValue  ( tsp00_Uint1 const           ID,
                                       bool const                  Value,
                                       teo200_EventList * const    pEventList  = NULL )
                                     { 
                                       tsp00_Uint1 TmpValue = Value ? 1 : 0;
                                       return PutVarPartValue(ID,&TmpValue,sizeof(TmpValue),pEventList);
                                     };
#endif

  /*!
     @Description    Get 'VarPart' values


                 Here, we have some functions to read VarPart values.
     @param          ID [in] - Unique VarPart identfier.
     @param          Value [in/out] - Value.
     @param          Optional [in] - true if the value is optional.                                        no error is returned!
     @param          MaxValueLen [in] - Maximum value length.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint1                 &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int2                  &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint2                 &Value,
                                       bool const                  Optional,
                                      teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int4                  &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint4                 &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Int8                  &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1                 ID,
                                       tsp00_Uint8                 &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };

  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       tsp00_Uint2 const           MaxValueLen,
                                       char *                      Value,
                                       bool const                  Optional,
                                       teo200_EventList *  const   pEventList ) const
                                     { return GetVarPartValue(ID,0,MaxValueLen,Value,
                                                              Optional,pEventList);};
#if defined (_WIN32)
  bool                eo42_GetValue  ( tsp00_Uint1 const           ID,
                                       RTE_OSPid                   &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const
                                     { return GetVarPartValue(ID,sizeof(Value),sizeof(Value),
                                                              &Value,Optional,pEventList); };
#endif

#if !defined (CPP_BOOL_NOT_IMPLEMENTED)
  inline bool         eo42_GetValue  ( tsp00_Uint1                 ID,
                                       bool                        &Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const;
#endif
  /*!
     @Description    Swaps the complete 'VarPart'
     @param          Swap [in] - Swap object
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool                eo42_Swap      ( teo41_Swap const            &Swap,
                                       teo200_EventList * const    pEventList ) const;

private:
  bool                PutVarPartValue( tsp00_Uint1 const           ID,
                                       void const * const          Value,
                                       tsp00_Uint2 const           ValueLength,
                                       teo200_EventList * const    pEventList );

  bool                GetVarPartValue( tsp00_Uint1 const           ID,
                                       tsp00_Uint2 const           MinValueLength,
                                       tsp00_Uint2 const           MaxValueLength,
                                       void * const                Value,
                                       bool const                  Optional,
                                       teo200_EventList * const    pEventList ) const;
private:
  bool                                                       OldVarPart_eo42;      // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!  
  char *                                                     VarPart_eo42;         // - Pointer to the VarPart buffer.
  tsp00_Uint2                                                VarPartSize_eo42;     // - VarPart buffer size.     
  tsp00_Uint2                                                VarPartLen_eo42;      // - Length of VarPart content.
}; 



/*---------------------------------------------------------------------------*/


#if !defined (CPP_BOOL_NOT_IMPLEMENTED)
inline bool teo42_RTEReqRespVarPart::eo42_GetValue( tsp00_Uint1                 ID,
                                                    bool                        &Value,
                                                    bool const                  Optional,
                                                    teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespVarPart::eo42_GetValue"));

  tsp00_Uint1 TmpValue;
  bool        Ok;
  
  Ok = GetVarPartValue(ID,sizeof(TmpValue),sizeof(TmpValue),
                       &TmpValue,Optional,pEventList); 
  Value = (TmpValue != 0);
  
  return Ok;
}
#endif



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class teo42_RTEReqRespBase                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_RTEReqRespBase
   @Description    Build or extract request/response packets.


                This base class was designed to create request/response classes 
                to exchange different informations on connection-based 
                services. Each request and response information is part of 
                the packet data area.
                A complete requests/response packet should look like this:                   
                                                                                 &lt;code>&lt;pre>
                   PacketBuffer       ->+--------------------+
                                        |                    |
                                        |    PacketHeader    |
                                        |                    |
                   PacketDataBuffer   ->++------------------++
                   (RTEReqRespBase)     ||                  ||
                                        ||    Fixed Part    ||
                                        ||                  ||
                                        |+------------------+|
                                        ..                  ..
                                        ||                  ||
                                        ||  Variable Part   ||
                                        ||    (varpart)     ||
                                        ||                  ||
                                        ..                  ..
                                        ++------------------++
                                                                                 &lt;br>
                The PacketBuffer/PacketHeaderBuffer should have at least 
                a 16 Byte alignment.
                You have to derive a class from teo42_RTEReqRespBase 
                which implements the following methods:                          &lt;code>&lt;pre>
                                                                                 &lt;br>
                    - eo42_FillVarPart
                    - eo42_ExtractVarPart
                                                                                 &lt;br>
                  and your parameter structure must be derived from
                                                                                 &lt;br>
                    - teo42_FixedPartParams   
                                                                                 &lt;br>&lt;/code>&lt;/pre>
                You don't have to create your own fixed part because 
                teo42_RTEReqRespBase creates a default fixed part. 
                But if you do so, implement the following methods:                   &lt;code>&lt;pre>
                                                                                 &lt;br>   
                    - eo42_CalcVarPartBuffer
                    - eo42_CalcVarPartBufferSize
                    - eo42_FillFixedPart
                    - eo42_ExtractFixedPart
                                                                                 &lt;br>
                and the following structure must be your own:
                                                                                 &lt;br>
                    - teo42_FixedPartParams   
                    - teo42_RTEReqRespData
                                                                                 &lt;br>&lt;/code>&lt;/pre>
                teo42_RTEReqRespBase has no base class.

 */


class teo42_RTEReqRespBase
{
public:
  struct teo42_FixedPartParams { 
                                 tsp00_Uint2          RequestVersion_eo42; // - in/out
                                 tsp00_Uint1          OSType_eo42;         // - out
                               };

public:

  /*!
     @Description    Create a packet containing the request/response information.
     @param          PacketDataBuffer [in] - Pointer to the data buffer receiving                                             the request/response information.
     @param          PacketDataBufferSize [in] - Data buffer size.
     @param          DataLen [out] - Length of data inserted.
     @param          pEventList [inout] - The event list to be used,                                             if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool                eo42_FillPacketData           ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           PacketDataBufferSize,
                                                      tsp00_Uint4                 &DataLen,
                                                      teo200_EventList * const    pEventList = NULL );
                   
  /*!
     @Description    Swaps the request/response data.


                This operation must be done before you extract the data content
                of the packet. Do a data swapping only once!!!
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          request/response information.
     @param          DataLen [in] - Length of data.
     @param          Swap [in] - Swap object
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo42_SwapPacketData           ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );

  /*!
     @Description    Exctract request/response from Packet.
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          request/response information.
     @param          DataLen [in] - Length of data.
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  bool                eo42_ExtractPacketData        ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      teo200_EventList * const    pEventList = NULL );
                   

  /*!
     @Description    Swap and extract the request/response data.


                This call this operation only once because of 
                a data swapping is done!!!
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          request/response information.
     @param          DataLen [in] - Length of data.
     @param          Swap [in] - Swap object
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  inline bool         eo42_SwapAndExtractPacketData ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );

  /*!
     @Description    Returns the minmum buffer space needed to hold the
              request/response data.
     @Return value   Minmum data buffer space.

   */

  static tsp00_Uint4  eo42_MinPacketDataBufferSize  () { return MIN_PACKET_BUFFER_SIZE - 
                                                                teo42_PacketHeader::eo42_PacketHeaderBufferSize(); };

  /*!
     @Description    Returns the response type of a packet containing raw data.
     @Return value   Response type of a packet containing raw data.

   */

  static tsp00_Uint1  eo42_RawDataReqRespType       () { return REQ_RESP_TYPE_RAW_DATA; };

protected:
  inline void         eo42_InitFixedPartParams      ();

  virtual teo42_FixedPartParams& eo42_FixedPartParams() = 0;


  // --- varpart methods
  virtual bool        eo42_FillVarPart              ( teo42_RTEReqRespVarPart        &VarPart,
                                                      teo200_EventList * const       pEventList = NULL ) { return true; };
  virtual bool        eo42_ExtractVarPart           ( teo42_RTEReqRespVarPart const  &VarPart,
                                                      teo200_EventList * const       pEventList = NULL ) { return true; };

protected:
                      teo42_RTEReqRespBase          () {;};
  virtual             ~teo42_RTEReqRespBase         () {;};

protected:
  // --- if you create your own request packet with a different fixed part,
  //     you have to implement the following virtual functions in your 
  //     derived class
  //
  virtual void *      eo42_CalcVarPartBuffer        ( void * const                PacketDataBuffer ) const;
  virtual tsp00_Uint2 eo42_CalcVarPartBufferSize    ( tsp00_Uint2                 PacketDataBufferSize ) const;

  virtual bool        eo42_FillFixedPart            ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           PacketDataBufferSize,
                                                      teo00_Uint2 const           VarPartLen,
                                                      teo00_Uint4                 &DataLen,
                                                      teo200_EventList * const    pEventList = NULL );

  virtual bool        eo42_ExtractFixedPart         ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      tsp00_Uint2                 &VarPartLen,
                                                      teo200_EventList * const    pEventList = NULL );

  virtual bool        eo42_SwapFixedPart            ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      tsp00_Uint2                 &VarPartLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );

private:
  struct teo42_RTEReqRespData
  {
    tsp00_Uint1                           OSType_eo42;
    tsp00_Uint1                           Filler1_eo42;
    tsp00_Uint2                           RequestVersion_eo42; 

    tsp00_Uint4                           VarPartLen_eo42;
    char                                  VarPart_eo42[1];
  };
}; 


/*---------------------------------------------------------------------------*/

inline void  teo42_RTEReqRespBase::eo42_InitFixedPartParams ()
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_InitFixedPartParams"));

  eo42_FixedPartParams().OSType_eo42          = (tsp00_Uint1)UNDEF_SP00;
  eo42_FixedPartParams().RequestVersion_eo42  = 0;
}

/*---------------------------------------------------------------------------*/

inline bool teo42_RTEReqRespBase::
              eo42_SwapAndExtractPacketData ( void * const                PacketDataBuffer,
                                              tsp00_Uint4 const           DataLen,
                                              teo41_Swap const            &Swap,
                                              teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_SwapAndExtractPacketData"));

  bool Ok;

  Ok = eo42_SwapPacketData  ( PacketDataBuffer, DataLen, Swap, pEventList );

  if ( Ok )
    Ok = eo42_ExtractPacketData ( PacketDataBuffer, DataLen, pEventList );

  return Ok;
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo42_RTEErrorResp                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_RTEErrorResp
   @Description    Build up and/or extract an error packet.


                This base class is used to build up and/or extract error
                packets. 
                teo42_RTEErrorResp has no base class

 */


class teo42_RTEErrorResp
{
public:
                      teo42_RTEErrorResp            () {;}
  virtual             ~teo42_RTEErrorResp           () {;};

  /*!
     @Description    Create a packet containing an error.
     @param          PacketDataBuffer [in] - Pointer to the data buffer receiving the                                              error data.
     @param          PacketDataBufferSize [in] - Data buffer size.
     @param          CommState [in] - Communication state.
     @param          EventList [in] - Pointer to an EventList.
     @param          DataLen [out] - Length of data inserted.
     @param          pEventList [inout] - The event list to be used,                                             if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool        eo42_FillPacketData           ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           PacketDataBufferSize,
                                                      tsp01_CommErr_Enum          CommState,
                                                      teo200_EventList const      &EventList,
                                                      tsp00_Uint4                 &DataLen,
                                                      teo200_EventList * const    pEventList = NULL ) const;


  /*!
     @Description    Swaps the error response data.


                This operation must be done before you extract the dat content
                of the packet!!
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          error data.
     @param          DataLen [in] - Length of data.
     @param          Swap [in] - Swap object
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  virtual bool        eo42_SwapPacketData           ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      teo41_Swap const            &Swap,
                                                      teo200_EventList * const    pEventList = NULL );

  /*!
     @Description    Exctract an error from a Packet.
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          error data.
     @param          DataLen [in] - Length of data.
     @param          CommState [out] - Communication state.
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  virtual bool        eo42_ExtractPacketData        ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      tsp01_CommErr_Enum          &CommState,
                                                      teo200_EventList * const    pEventList );

  /*!
     @Description    Swap and extract the request/response data.


                This call this operation only once because of 
                a data swapping is done!!!
     @param          PacketDataBuffer [in] - Pointer to the data buffer containing the                                          request/response information.
     @param          DataLen [in] - Length of data.
     @param          Swap [in] - Swap object
     @param          pEventList [inout] - The event list to be used,                                         if NULL throw EventList on error.
     @Return value   true if ok

   */

  inline bool         eo42_SwapAndExtractPacketData ( void * const                PacketDataBuffer,
                                                      tsp00_Uint4 const           DataLen,
                                                      teo41_Swap const            &Swap,
                                                      tsp01_CommErr_Enum          &CommState,
                                                      teo200_EventList * const    pEventList = NULL );

  /*!
     @Description    Returns the minmum buffer space needed to hold the error response.
     @Return value   Minmum buffer space needed.

   */

  static tsp00_Uint4  eo42_MinPacketDataBufferSize  () { return MIN_PACKET_BUFFER_SIZE - 
                                                                teo42_PacketHeader::eo42_PacketHeaderBufferSize(); };

  /*!
     @Description    Returns the response type of this packet data.
     @Return value   Response type.

   */

  static tsp00_Uint1  eo42_ReqRespType              () { return REQ_RESP_TYPE_EVENTLIST; };

private:
  struct teo42_RTEErrorRespData
  {
    tsp00_Uint1                           OSType_eo42;
    tsp00_Uint1                           Filler1_eo42;
    tsp00_Uint2                           RequestVersion_eo42;

    tsp00_Uint2                           CommState_eo42;
    tsp00_Uint2                           Filler2_eo42;

    char                                  EvtLstBuffer_eo42[1];
  };
};


/*---------------------------------------------------------------------------*/

inline bool teo42_RTEErrorResp::
              eo42_SwapAndExtractPacketData ( void * const                PacketDataBuffer,
                                              tsp00_Uint4 const           DataLen,
                                              teo41_Swap const            &Swap,
                                              tsp01_CommErr_Enum          &CommState,
                                              teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEErrorResp::eo42_SwapAndExtractPacketData"));

  bool Ok;

  Ok = eo42_SwapPacketData  ( PacketDataBuffer, DataLen, Swap, pEventList );

  if ( Ok )
    Ok = eo42_ExtractPacketData ( PacketDataBuffer,DataLen, CommState, pEventList );

  return Ok;
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo42_EmptyReqResp                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo42_EmptyReqResp
   @Description    Build up and/or extract an empty request/response packet.


                teo42_EmptyReqResp is derived from 
                teo42_RTEReqRespBase

 */

class teo42_EmptyReqResp : public teo42_RTEReqRespBase
{
public:
  typedef teo42_RTEReqRespBase::teo42_FixedPartParams   teo42_Params;

public:
                      teo42_EmptyReqResp              () { eo42_InitParams();};
  virtual             ~teo42_EmptyReqResp             () {;}

  teo42_Params&       eo42_Params                     () { return m_Params; };
  void                eo42_InitParams                 () { eo42_InitFixedPartParams(); };

protected:
  virtual teo42_FixedPartParams& eo42_FixedPartParams () { return m_Params; };

private:
  teo42_Params  m_Params;
}; 



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/


#endif  /* GEO42_H */
