/*!**************************************************************************

  module      : RTEComm_PacketHeader.hpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: communication base classes
  description : This module contains the common communication (base) classes.


  last changed: 2002-11-15  16:20
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/


#ifndef RTECOMM_PACKETHEADER_HPP
#define RTECOMM_PACKETHEADER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/Communication/RTEComm_Swapping.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     RTE_COMM_UNDEF_REF  SAPDB_MAX_UINT4

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class        RTEComm_PacketHeader  
  @brief        Interface class to create, read and modify the 
                packet header.

                The 'PacketHeader is the first part of each packet indicating
                the kind of packet data. It also contains length information
                to reduce the chance of data truncation going undetected.

                Each transfered packet should look like this:                   
                                                                                <code><pre>
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
                                                                                <br>
                                                                                </code></pre>
                The PacketHeader class hides the local and peer swap type. 
                To swap values outside of this header class use the method 
                'SwapObj'. 

                The PacketBuffer/PacketHeaderBuffer should have at least 
                a 16 Byte alignment!

                RTEComm_PacketHeader does not have a base class.
*/
class RTEComm_PacketHeader
{
private:
  typedef struct 
  {
    SAPDB_UInt4                           CurrPacketSegLen;
    SAPDB_UInt1                           ProtocolID;
    SAPDB_UInt1                           ReqRespType; 
    SAPDB_UInt1                           ProtocolVersion;
    SAPDB_UInt1                           ResidualPacketSegs;
    SAPDB_UInt4                           SenderRef;
    SAPDB_UInt4                           ReceiverRef;
    SAPDB_UInt2                           Filler1;
    SAPDB_UInt1                           SwapType;
    SAPDB_UInt1                           Filler2;
    SAPDB_UInt4                           MaxSendLen;
    SAPDB_UInt4                           SequenceNumber;
    SAPDB_UInt4                           Filler3;

    // - Use the following define only for static buffer allocations,
    //   otherwise use the 'PacketHeaderBufferSize' methode.
#   define PACKET_HEADER_BUFFER_SIZE     32

    // Should be 16-byte aligned to avoid alignment computations.
    //
    // CurrPacketSegLen    Current packet segment length includes the PacketHeader.
    // MaxSendLen          Includes one PacketHeader regardless of the number
    //                     of packet segments. This means, that the sum of all
    //                     segment's CurrPacketSegLen is greater than
    //                     MaxSendLen unless the number of segments is 1.
    // ProtocolID          Type of protocol ( e.g. TCP/IP, Pipe ... )
    // ProtocolVersion     Version of Protocol
    // ReqRespType         This Identifier specifies the packet contents.
    //                     ( e.g. connect, connect response, cancel, release, data, data response ...)
    // ResidualPacketSegs  Number of packet segments  remaining.
    // SenderRef           Sender reference 
    // ReceiverRef         Receiver reference
    // SequenceNumber      Sequence number
    
  } RTEComm_PacketHeaderRecord; 

public:
  /*!
     @brief Create a PacketHeader interface object.

     Arguments:    PacketHeaderBuffer [in] - Pointer to a buffer receiving the PacketHeader.

     @return none
  */
                    RTEComm_PacketHeader ( void * const                  PacketHeaderBuffer = NULL )
                                         : m_PacketHeaderBuffer((RTEComm_PacketHeaderRecord *)PacketHeaderBuffer),
                                           m_MaxDataLen   (0),
                                           m_DataRemaining(0),
                                           m_SaveRestoreBufferPos(0) { ; }

  /*!
     @brief Set the PacketHeader buffer.

     Arguments:    PacketHeaderBuffer [in] - Pointer to a buffer receiving the PacketHeader.

     @return none
  */
  void              SetPacketHeaderBuffer( void * const                  PacketHeaderBuffer )
                                         {   m_SaveRestoreBufferPos = 0;
                                             m_PacketHeaderBuffer   = (RTEComm_PacketHeaderRecord *)PacketHeaderBuffer; }


  /*!
     @brief Returns the buffer size needed to hold the PacketHeader.

     @return The buffer size needed to hold the PacketHeader.

  */
  static SAPDB_UInt4 const  PacketHeaderBufferSize();


  /*!
     @brief Build a new PacketHeader.

       If a packet must be transfered with more than one send 
       operation you have to set the header values with 'Set'
       only once. After each send you must call 'NextSendPacketSeg'
       which decreases the number of residual packet segments and 
       calculates the send length for the next packet segment.
       The 'ResidualPacketSegs' methode can be used to get the
       number of residual packet segments.


     Arguments:    ReqRespType         [in] - This Identifier specifies the packet contents.
                                              ( e.g. connect, connect response, cancel, release, data, data response ...)
                   SenderRef           [in] - Sender reference.
                   ReceiverRef         [in] - Receiver reference.
                   DataLen             [in] - Total length of data.
                   ProtocolID          [in] - Type of protocol ( e.g. TCP/IP, Pipe ... )
                   ProtocolVersion     [in] - Version of Protocol
                   SequenceNumber      [in] - Sequence number.
                   MaxPacketLen        [in] - Maximum packet length per send/receive  (including PacketHeader).

     @return The send length for the next packet segment ( including PacketHeader space )
  */
  SAPDB_UInt4 const Set                   ( SAPDB_UInt1  const                          ReqRespType,
                                            SAPDB_UInt4  const                          SenderRef,
                                            SAPDB_UInt4  const                          ReceiverRef,
                                            SAPDB_UInt4  const                          DataLen,
                                            RTEComm_ProtocolID const                    ProtocolID,
                                            SAPDB_UInt1 const                           ProtocolVersion,
                                            SAPDB_UInt4 const                           MaxPacketLen   = (SAPDB_UInt4)-1, 
                                            SAPDB_UInt4 const                           SequenceNumber = 0); 

  /*!
     @brief Get current PacketHeader information.

     Arguments:    ReqRespType      [out]   - This Identifier specifies the packet contents.
                                              ( e.g. connect, connect response, cancel, release, data, data response ...)
                   SenderRef        [out]   - Sender reference.
                   ReceiverRef      [out]   - Receiver reference.
                   DataLen          [out]   - Total length of data.
                   ActDataLen       [out]   - Data length of the current packet.
                   SequenceNumber   [out]   - Sequence number.
                   ProtocolID       [out]   - Type of protocol ( e.g. TCP/IP, Pipe ... )
                   ProtocolVersion  [out]   - Protocol Version
                   MessageList    [inout]   - The message list to be used

     @return true if ok
  */
  bool               Get                  ( SAPDB_UInt1 * const                           pReqRespType,
                                            SAPDB_UInt4 * const                           pSenderRef,
                                            SAPDB_UInt4 * const                           pReceiverRef,
                                            SAPDB_UInt4 * const                           pDataLen,
                                            SAPDB_UInt4 * const                           pActDataLen,
                                            SAPDB_UInt4 * const                           pSequenceNumber,
                                            RTEComm_ProtocolID * const                    pProtocolID,
                                            SAPDB_UInt1 * const                           pProtocolVersion,
                                            SAPDBErr_MessageList                          &MessageList );

  /*!
     @brief Get current PacketHeader information.

     Arguments:    ReqRespType      [out]   - This Identifier specifies the packet contents.
                                              ( e.g. connect, connect response, cancel, release, data, data response ...)
                   SenderRef        [out]   - Sender reference.
                   ReceiverRef      [out]   - Receiver reference.
                   DataLen          [out]   - Total length of data.
                   ActDataLen       [out]   - Data length of the current packet.
                   ProtocolID       [out]   - Type of protocol ( e.g. TCP/IP, Pipe ... )
                   ProtocolVersion  [out]   - Protocol Version
                   MessageList    [inout]   - The message list to be used

     @return true if ok
  */
  bool               Get                  ( SAPDB_UInt1 * const                           pReqRespType,
                                            SAPDB_UInt4 * const                           pSenderRef,
                                            SAPDB_UInt4 * const                           pReceiverRef,
                                            SAPDB_UInt4 * const                           pDataLen,
                                            SAPDB_UInt4 * const                           pActDataLen,
                                            RTEComm_ProtocolID * const                    pProtocolID,
                                            SAPDB_UInt1 * const                           pProtocolVersion,
                                            SAPDBErr_MessageList                          &MessageList )
                                          {
                                           SAPDB_UInt4 DummySequenceNumber;
                                           return  Get ( pReqRespType, pSenderRef, pReceiverRef,
                                                         pDataLen, pActDataLen, &DummySequenceNumber, pProtocolID,
                                                         pProtocolVersion, MessageList );
                                          }

  /*!
     @brief Get number of remainig packet segments.

     Arguments:    none

     @return Number of packet segments remaining.
  */
  SAPDB_UInt1 const ResidualPacketSegs    () const
                                          { return m_PacketHeaderBuffer->ResidualPacketSegs; } ;

  /*!
     @brief Get the total length of the current packet segment ( including PacketHeader space ).

     Arguments:    PacketSegLength  [out]   - The total length of the current 
                                              packet segment ( including PacketHeader ).
                   MessageList      [inout] - The event list to be used,

     @return true if ok
  */
  bool             CurrPacketSegLen       ( SAPDB_UInt4                    &PacketSegLength,
                                            SAPDBErr_MessageList           &MessageList );

  /*!
     @brief Update header for the next send operation.

                   If a packet must be transfered with more than one send 
                   operation you have to set the header values with 'Set'
                   only once. After each send you must call 'NextSendPacketSeg'
                   which decreases the number of residual packet segments and 
                   calculates the actual send length for the next packet.
                   The 'ResidualPacketSegs' methode can be used to get the
                   number of residual packet segments.


     Arguments:    none

     @return The send length for the next packet ( including PacketHeader space ).
  */
  SAPDB_UInt4 const NextSendPacketSeg     ();

  /*!
     @brief Save the content of a given buffer so it's ready to receive a new packet buffer.
                   This methode is normaly used before cloning a packet header into a data area.

     Arguments:    Buffer [in] - Pointer to a buffer to be saved.
     @return none.
  */
  void              SaveBufferContent( void * const                  Buffer ) 
                                          {  m_SaveRestoreBufferPos = Buffer; 
                                             SAPDB_MemCopyNoCheck(m_SaveRestoreBuffer, Buffer, sizeof(m_SaveRestoreBuffer)); };

  /*!
     @brief Restore the old content of a previously saved buffer ( refer to SaveBufferContent ) 

     @return none.
  */
  void              RestoreBufferContent() 
                                          {   if ( 0 != m_SaveRestoreBufferPos )
                                              {
                                                  SAPDB_MemCopyNoCheck(m_SaveRestoreBufferPos, m_SaveRestoreBuffer, sizeof(m_SaveRestoreBuffer));
                                                  m_SaveRestoreBufferPos = 0;
                                              }
                                          };

  /*!
     @brief Clone the packet header buffer content.

     Arguments:    Buffer [in] - Pointer to a buffer where the orignal packet 
                                 header buffer has to be written to.

     @return none
  */
  void              Clone  ( void * const                  Buffer )
                                          {
                                             SAPDB_MemCopyNoCheck(m_SaveRestoreBuffer, Buffer, sizeof(m_SaveRestoreBuffer)); 
                                          }
                                            

  /*!
     @brief Get a 'RTEComm_Swapping' object to swap values outside of this class.

                   The PacketHeader class hides the local and peer swap type. 
                   To swap values outside of this header class use this method.
                   which returns a 'RTEComm_Swapping' object ('swap_object'). 

                   A swap can be done by calling:                       <br>
                                                                        <br>
                    'swap_object' = 'header_object'.SwapObj();          <br>
                                                                        <br>
                    'swap_object'.SwapValue('my_value1');               <br>
                    'swap_object'.SwapValue('my_value2');               <br>
                    'swap_object'.SwapValue('my_value3');               <br>
                                                                        <br>

     @return A 'RTEComm_Swapping' object (<swap_object>). 
  */
  RTEComm_Swapping const & SwapObj        () const
                                          { return m_Swap; };
private:
  bool                     SwapHeader     ( SAPDBErr_MessageList   &MessageList );

private:
  RTEComm_PacketHeaderRecord *   m_PacketHeaderBuffer;

  SAPDB_UInt4                    m_MaxDataLen;
  SAPDB_UInt4                    m_DataRemaining;

  char                           m_SaveRestoreBuffer[PACKET_HEADER_BUFFER_SIZE];
  void*                          m_SaveRestoreBufferPos;
  RTEComm_Swapping               m_Swap;

};

/*! EndClass: RTEComm_PacketHeader */

#endif  /* RTECOMM_PACKETHEADER_HPP */
