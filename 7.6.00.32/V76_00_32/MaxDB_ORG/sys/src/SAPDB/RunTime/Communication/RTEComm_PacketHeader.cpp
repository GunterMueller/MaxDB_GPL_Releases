/*!**************************************************************************

  module      : RTEComm_PacketHeader.cpp

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/Communication/RTEComm_Swapping.h"
#include "RunTime/Communication/RTEComm_PacketHeader.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

extern SAPDBTrace_Topic Runtime_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTECOMM_MIN(a,b)            (((a) < (b)) ? (a) : (b))


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                        class RTEComm_PacketHeader                         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


SAPDB_UInt4 const  RTEComm_PacketHeader::PacketHeaderBufferSize()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::PacketHeaderBufferSize", Runtime_Trace, 5);

  return sizeof(RTEComm_PacketHeader::RTEComm_PacketHeaderRecord);
}


/*---------------------------------------------------------------------------*/

SAPDB_UInt4 const  RTEComm_PacketHeader::Set( SAPDB_UInt1 const                           ReqRespType, 
                                              SAPDB_UInt4 const                           SenderRef,
                                              SAPDB_UInt4 const                           ReceiverRef,
                                              SAPDB_UInt4 const                           DataLen,
                                              RTEComm_ProtocolID const                    ProtocolID,
                                              SAPDB_UInt1 const                           ProtocolVersion,
                                              SAPDB_UInt4 const                           MaxPacketLen,
                                              SAPDB_UInt4 const                           SequenceNumber )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::Set", Runtime_Trace, 5);

  m_SaveRestoreBufferPos = 0;

  m_PacketHeaderBuffer->ProtocolID      = (SAPDB_UInt1)ProtocolID;
  m_PacketHeaderBuffer->ProtocolVersion = ProtocolVersion;
  m_PacketHeaderBuffer->ReqRespType     = ReqRespType;
  m_PacketHeaderBuffer->SenderRef       = SenderRef;
  m_PacketHeaderBuffer->ReceiverRef     = ReceiverRef;
  m_PacketHeaderBuffer->SequenceNumber  = SequenceNumber;
  m_PacketHeaderBuffer->SwapType        = m_Swap.GetLocalSwapType();
  m_PacketHeaderBuffer->Filler1         = 0;
  m_PacketHeaderBuffer->Filler2         = 0;
  m_PacketHeaderBuffer->Filler3         = 0;

  if ( MaxPacketLen != (SAPDB_UInt4)-1 && 0 != DataLen )
  {
    m_MaxDataLen = MaxPacketLen - PacketHeaderBufferSize();

    SAPDB_UInt4    CurrDataLen = RTECOMM_MIN( m_MaxDataLen, DataLen );

    m_PacketHeaderBuffer->CurrPacketSegLen   = CurrDataLen + PacketHeaderBufferSize();
    m_PacketHeaderBuffer->MaxSendLen         = DataLen     + PacketHeaderBufferSize();
    m_PacketHeaderBuffer->ResidualPacketSegs = (SAPDB_UInt1)((DataLen - 1) / m_MaxDataLen);
    SAPDBERR_ASSERT_STATE ( ((DataLen - 1) / m_MaxDataLen) < 256 );

    m_DataRemaining = DataLen - CurrDataLen;
  }
  else
  {
    m_PacketHeaderBuffer->CurrPacketSegLen   = DataLen + PacketHeaderBufferSize();
    m_PacketHeaderBuffer->MaxSendLen         = m_PacketHeaderBuffer->CurrPacketSegLen;
    m_PacketHeaderBuffer->ResidualPacketSegs = 0;
    
    m_DataRemaining = 0;
  }

  return m_PacketHeaderBuffer->CurrPacketSegLen;
}

/*---------------------------------------------------------------------------*/

bool RTEComm_PacketHeader::Get ( SAPDB_UInt1 * const                           pReqRespType, 
                                 SAPDB_UInt4 * const                           pSenderRef,
                                 SAPDB_UInt4 * const                           pReceiverRef,
                                 SAPDB_UInt4 * const                           pDataLen,
                                 SAPDB_UInt4 * const                           pActDataLen,
                                 SAPDB_UInt4  * const                          pSequenceNumber,
                                 RTEComm_ProtocolID * const                    pProtocolID,
                                 SAPDB_UInt1 * const                           pProtocolVersion,
                                 SAPDBErr_MessageList                          &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::Get", Runtime_Trace, 5);
  bool              Ok;

  // --- swap header if needed
  Ok = SwapHeader( MessageList );
    
  if ( Ok )
  {
    if ( pReqRespType != NULL )  
      *pReqRespType = m_PacketHeaderBuffer->ReqRespType;  

    if ( pProtocolID != NULL ) 
      *pProtocolID = (RTEComm_ProtocolID) m_PacketHeaderBuffer->ProtocolID;

    if ( pProtocolVersion != NULL ) 
      *pProtocolVersion = m_PacketHeaderBuffer->ProtocolVersion;

    if ( pSenderRef != NULL ) 
      *pSenderRef = m_PacketHeaderBuffer->SenderRef;

    if ( pReceiverRef  != NULL ) 
      *pReceiverRef = m_PacketHeaderBuffer->ReceiverRef;

    if ( pSequenceNumber != NULL )  
      *pSequenceNumber = m_PacketHeaderBuffer->SequenceNumber;

    if ( pActDataLen != NULL ) 
      *pActDataLen = m_PacketHeaderBuffer->CurrPacketSegLen - PacketHeaderBufferSize();

    if ( pDataLen != NULL ) 
      *pDataLen = m_PacketHeaderBuffer->MaxSendLen - PacketHeaderBufferSize();
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 const RTEComm_PacketHeader::NextSendPacketSeg ()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::NextSendPacketSeg", Runtime_Trace, 5);

  if ( m_DataRemaining == 0 )
    return 0;

  SAPDB_UInt4    CurrDataLen = RTECOMM_MIN( m_MaxDataLen, m_DataRemaining );

  m_PacketHeaderBuffer->CurrPacketSegLen = CurrDataLen + PacketHeaderBufferSize();
  m_DataRemaining                        = m_DataRemaining - CurrDataLen;

  m_PacketHeaderBuffer->ResidualPacketSegs--;

  return m_PacketHeaderBuffer->CurrPacketSegLen;
}

/*---------------------------------------------------------------------------*/

bool RTEComm_PacketHeader::CurrPacketSegLen( SAPDB_UInt4               &PacketSegLength,
                                             SAPDBErr_MessageList      &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::CurrPacketSegLen", Runtime_Trace, 5);
  bool Ok = true;

  // --- swap header if needed
  Ok = SwapHeader( MessageList );
    
  if ( Ok )
    PacketSegLength = m_PacketHeaderBuffer->CurrPacketSegLen;

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool RTEComm_PacketHeader::SwapHeader ( SAPDBErr_MessageList  &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_PacketHeader::SwapHeader", Runtime_Trace, 5);
  bool Ok = true;

  if ( m_Swap.PeerSwapTypeSet() == false )
    Ok = m_Swap.SetPeerSwapType ( m_PacketHeaderBuffer->SwapType, MessageList );

  if ( Ok ) 
  {
    // --- check if swapping is required 
    if ( m_Swap.SwappingRequired( m_PacketHeaderBuffer->SwapType ) == true )
    {
      // --- set header swap type to local swap type to avoid swapping twice
      m_PacketHeaderBuffer->SwapType = m_Swap.GetLocalSwapType();

      // --- swap all value regardless of types
                m_Swap.SwapValue( m_PacketHeaderBuffer->CurrPacketSegLen, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->ProtocolID, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->ReqRespType, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->ProtocolVersion, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->ResidualPacketSegs, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->SenderRef, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->ReceiverRef, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->SequenceNumber, MessageList );
      if ( Ok ) m_Swap.SwapValue( m_PacketHeaderBuffer->MaxSendLen, MessageList );    
    }
  }

  return Ok;
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/