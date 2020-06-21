/*!
  @file           veo42.cpp
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/



#include "geo200.h"
#include "geo007_1.h"
#include "geo60.h"
#include "geo42.h"
#include "gsp00.h"
#include "gsp01.h"
#include "geo41.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

#include <stddef.h>
#include <stdlib.h>

#if defined(_WIN32)
# include <crtdbg.h>
#else /* UNIX */
# include <assert.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif
#endif

#define OSTYPE_WIN32_EO42                  1
#define OSTYPE_UNIX_OS_EO42                2


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MIN_EO42(a,b)            (((a) < (b)) ? (a) : (b))


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo42_PacketHeader                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


tsp00_Uint4 const  teo42_PacketHeader::eo42_PacketHeaderBufferSize()
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_PacketHeaderBufferSize"));

  return sizeof(teo42_PacketHeader::teo42_PacketHeaderRecord);
}


/*---------------------------------------------------------------------------*/

tsp00_Uint4 const  teo42_PacketHeader::eo42_Set( tsp00_Uint1 const                           ReqRespType, 
                                                 tsp00_Uint4 const                           SenderRef,
                                                 tsp00_Uint4 const                           ReceiverRef,
                                                 tsp00_Uint4 const                           DataLen,
                                                 teo42_ProtocolID const                      ProtocolID,
                                                 tsp00_Uint1 const                           ProtocolVersion,
                                                 tsp00_Uint4 const                           MaxPacketLen,
                                                 tsp01_CommErr_Enum const                    CommState )   // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_Set"));

  m_PacketHeaderBuffer->ProtocolID      = (tsp00_Uint1)ProtocolID;
  m_PacketHeaderBuffer->ProtocolVersion = ProtocolVersion;
  m_PacketHeaderBuffer->ReqRespType     = ReqRespType;
  m_PacketHeaderBuffer->SenderRef       = SenderRef;
  m_PacketHeaderBuffer->ReceiverRef     = ReceiverRef;
  m_PacketHeaderBuffer->CommState       = (tsp00_Uint2)CommState;      // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
  m_PacketHeaderBuffer->SwapType        = teo41_Swap::eo41_GetLocalSwapType();
  m_PacketHeaderBuffer->Filler          = 0;

  if ( MaxPacketLen != (tsp00_Uint4)UNDEF_SP00 )
  {
    m_MaxDataLen = MaxPacketLen - eo42_PacketHeaderBufferSize();

    tsp00_Uint4    CurrDataLen = MIN_EO42( m_MaxDataLen, DataLen );

    m_PacketHeaderBuffer->CurrPacketSegLen   = CurrDataLen + eo42_PacketHeaderBufferSize();
    m_PacketHeaderBuffer->MaxSendLen         = DataLen     + eo42_PacketHeaderBufferSize();
    m_PacketHeaderBuffer->ResidualPacketSegs = (tsp00_Uint1)((DataLen - 1) / m_MaxDataLen);
    _ASSERT ( ((DataLen - 1) / m_MaxDataLen) < 256 );

    m_DataRemaining = DataLen - CurrDataLen;
  }
  else
  {
    m_PacketHeaderBuffer->CurrPacketSegLen   = DataLen + eo42_PacketHeaderBufferSize();
    m_PacketHeaderBuffer->MaxSendLen         = m_PacketHeaderBuffer->CurrPacketSegLen;
    m_PacketHeaderBuffer->ResidualPacketSegs = 0;
    
    m_DataRemaining = 0;
  }

  return m_PacketHeaderBuffer->CurrPacketSegLen;
}

/*---------------------------------------------------------------------------*/

bool teo42_PacketHeader::eo42_Get ( tsp00_Uint1 * const                           pReqRespType, 
                                    tsp00_Uint4 * const                           pSenderRef,
                                    tsp00_Uint4 * const                           pReceiverRef,
                                    tsp00_Uint4 * const                           pDataLen,
                                    tsp00_Uint4 * const                           pActDataLen,
                                    tsp01_CommErr_Enum * const                    pCommState,  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
                                    teo42_ProtocolID * const                      pProtocolID,
                                    tsp00_Uint1 * const                           pProtocolVersion,
                                    teo200_EventList * const                      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_Get"));
  bool              Ok;

  // --- swap header if needed
  Ok = eo42_SwapHeader( pEventList );
    
  if ( Ok )
  {
    if ( pReqRespType != NULL )  
      *pReqRespType = m_PacketHeaderBuffer->ReqRespType;  

    if ( pProtocolID != NULL ) 
      *pProtocolID = (teo42_ProtocolID) m_PacketHeaderBuffer->ProtocolID;

    if ( pProtocolVersion != NULL ) 
      *pProtocolVersion = m_PacketHeaderBuffer->ProtocolVersion;

    if ( pSenderRef != NULL ) 
      *pSenderRef = m_PacketHeaderBuffer->SenderRef;

    if ( pReceiverRef  != NULL ) 
      *pReceiverRef = m_PacketHeaderBuffer->ReceiverRef;

    if ( pCommState != NULL )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
      *pCommState = (tsp01_CommErr_Enum)m_PacketHeaderBuffer->CommState;

    if ( pActDataLen != NULL ) 
      *pActDataLen = m_PacketHeaderBuffer->CurrPacketSegLen - eo42_PacketHeaderBufferSize();

    if ( pDataLen != NULL ) 
      *pDataLen = m_PacketHeaderBuffer->MaxSendLen - eo42_PacketHeaderBufferSize();
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

tsp00_Uint4 const teo42_PacketHeader::eo42_NextSendPacketSeg ()
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_NextSendPacketSeg"));

  if ( m_DataRemaining == 0 )
    return 0;

  tsp00_Uint4    CurrDataLen = MIN_EO42( m_MaxDataLen, m_DataRemaining );

  m_PacketHeaderBuffer->CurrPacketSegLen = CurrDataLen + eo42_PacketHeaderBufferSize();
  m_DataRemaining                        = m_DataRemaining - CurrDataLen;

  m_PacketHeaderBuffer->ResidualPacketSegs--;

  return m_PacketHeaderBuffer->CurrPacketSegLen;
}

/*---------------------------------------------------------------------------*/

bool teo42_PacketHeader::eo42_CurrPacketSegLen( tsp00_Uint4               &PacketSegLength,
                                                teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_CurrPacketSegLen"));
  bool Ok = true;

  // --- swap header if needed
  Ok = eo42_SwapHeader( pEventList );
    
  if ( Ok )
    PacketSegLength = m_PacketHeaderBuffer->CurrPacketSegLen;

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo42_PacketHeader::eo42_SwapHeader ( teo200_EventList * const   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::eo42_SwapHeader"));
  bool Ok = true;

  if ( eo41_GetPeerSwapType() == SWAP_TYPE_UDEFINED_EO41 )
    Ok = eo41_SetPeerSwapType ( m_PacketHeaderBuffer->SwapType, pEventList );

  if ( Ok ) 
  {
    // --- check if swapping is required 
    if ( eo41_SwappingRequired( m_PacketHeaderBuffer->SwapType ) == true )
    {
      // --- swap all value regardless of types
      eo41_SwapValue( m_PacketHeaderBuffer->CurrPacketSegLen );
      eo41_SwapValue( m_PacketHeaderBuffer->ProtocolID );
      eo41_SwapValue( m_PacketHeaderBuffer->ReqRespType );
      eo41_SwapValue( m_PacketHeaderBuffer->ProtocolVersion );
      eo41_SwapValue( m_PacketHeaderBuffer->ResidualPacketSegs );
      eo41_SwapValue( m_PacketHeaderBuffer->SenderRef );
      eo41_SwapValue( m_PacketHeaderBuffer->ReceiverRef );
      eo41_SwapValue( m_PacketHeaderBuffer->CommState );       // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
      eo41_SwapValue( m_PacketHeaderBuffer->MaxSendLen );    

      // --- set header swap type to local swap type to avoid swapping
      //     twice
      m_PacketHeaderBuffer->SwapType = teo41_Swap::eo41_GetLocalSwapType();
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

void teo42_PacketHeader::teo42_CopyPacketHeader ( teo42_PacketHeader  const        &PacketHeader )
{
  ROUTINE_DBG_MEO60 (_T("teo42_PacketHeader::teo42_CopyPacketHeader"));

  m_MaxDataLen          = PacketHeader.m_MaxDataLen;
  m_DataRemaining       = PacketHeader.m_DataRemaining;
  *m_PacketHeaderBuffer = *PacketHeader.m_PacketHeaderBuffer;

  return;
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class teo42_RTEReqRespVarPart                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

bool teo42_RTEReqRespVarPart::eo42_PutValue( tsp00_Uint1 const           ID,
                                             char const * const          Value,
                                             teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespVarPart::eo42_PutValue"));

  tsp00_Uint2   StrLen = (tsp00_Uint2)strlen(Value);
  bool          Ok     = true;

  if ( StrLen != 0 )
    Ok = PutVarPartValue( ID, Value, StrLen + 1, pEventList );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  teo42_RTEReqRespVarPart::PutVarPartValue( tsp00_Uint1 const         ID,
                                                void const * const        Value,
                                                tsp00_Uint2 const         ValueLength,
                                                teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("RTEReqRespPacket::PutVarPartValue"));
  tsp00_Uint2               HeaderLen = 3;
  char const * const        ValuePtr   = (char*) Value;
  char  *                   VarPartPtr = (char*) VarPart_eo42 + VarPartLen_eo42;
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!!!  ALWAYS USE CHARACTER POINTER FOR UNALIGNED VARPART ACCESS   !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if ( OldVarPart_eo42 )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
    HeaderLen = 2;

  if ( VarPartLen_eo42 + HeaderLen + ValueLength > VarPartSize_eo42 )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_VAR_PART_EXHAUSTED );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( OldVarPart_eo42 ) // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!!!
  {
    VarPartPtr[0] = (unsigned char)(ValueLength + HeaderLen);
    VarPartPtr[1] = ID;
  }
  else // --- new stuff !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  {
    VarPartPtr[0] = ID;
    VarPartPtr[1] = (unsigned char)(ValueLength >> 8);
    VarPartPtr[2] = (unsigned char)ValueLength & 0xFF;
  }

  //
  // --- use character pointer for unaligned VarPart access!!!!!!!!!!
  //
  VarPartPtr += HeaderLen;

  switch ( ValueLength )
  {
  case 1:
    VarPartPtr[0] = ValuePtr[0];
    break;
  case 2:
    VarPartPtr[0] = ValuePtr[0];
    VarPartPtr[1] = ValuePtr[1];
    break;
  case 4:
    VarPartPtr[0] = ValuePtr[0];
    VarPartPtr[1] = ValuePtr[1];
    VarPartPtr[2] = ValuePtr[2];
    VarPartPtr[3] = ValuePtr[3];
    break;
  case 8:
    VarPartPtr[0] = ValuePtr[0];
    VarPartPtr[1] = ValuePtr[1];
    VarPartPtr[2] = ValuePtr[2];
    VarPartPtr[3] = ValuePtr[3];
    VarPartPtr[4] = ValuePtr[4];
    VarPartPtr[5] = ValuePtr[5];
    VarPartPtr[6] = ValuePtr[6];
    VarPartPtr[7] = ValuePtr[7];
    break;
  default:
    SAPDB_MemCopyNoCheck ( VarPartPtr, ValuePtr, ValueLength );
    break;
  }

  VarPartLen_eo42 += ValueLength + HeaderLen;

  return true;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEReqRespVarPart::GetVarPartValue ( tsp00_Uint1 const           ID,
                                                tsp00_Uint2 const           MinValueLength,
                                                tsp00_Uint2 const           MaxValueLength,
                                                void * const                Value,
                                                bool const                  Optional,
                                                teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("RTEReqRespPacket::GetVarPartValue"));
  tsp00_Uint2              Pos;
  char *                   ValuePtr;
  char *                   VarPartValue;
  tsp00_Uint2              VarPartValueLength = 0;
  tsp00_Uint2              HeaderLen          = 3;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!!!  ALWAYS USE CHARACTER POINTER FOR UNALIGNED VARPART ACCESS   !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if ( OldVarPart_eo42 )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
    HeaderLen = 2;

  // --- searching for the ID
  //
  for ( Pos = 0; Pos < VarPartLen_eo42; Pos += VarPartValueLength + HeaderLen )
  {
    if ( OldVarPart_eo42 )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
    {
      VarPartValueLength = HeaderLen < VarPart_eo42[Pos] ? VarPart_eo42[Pos] - HeaderLen : 0;
     
      if ( VarPart_eo42[Pos + 1] == ID )
        break;
    }
    else // --- new stuff !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
      VarPartValueLength = ( VarPart_eo42[ Pos + 1 ] << 8 ) | ( VarPart_eo42[ Pos + 2 ] );
      
      if ( VarPart_eo42[Pos] == ID )
        break;
    }
  }

  if ( Pos < VarPartLen_eo42 ) // End of list reached?
  {
    if (( VarPartValueLength < MinValueLength ) || ( VarPartValueLength > MaxValueLength ))
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_ILL_VARPART_VALUE_LENGTH, VarPartValueLength );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    if ( Pos + HeaderLen + VarPartValueLength > VarPartSize_eo42 )
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INVALID_VAR_PART_LENGTH, 
                               VarPartValueLength );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    //
    // --- use character pointer for unaligned VarPart access!!!!!!
    //
    VarPartValue = &VarPart_eo42[Pos + HeaderLen];
    ValuePtr     = (char*)Value;

    switch ( VarPartValueLength )
    {
    case 1:
      ValuePtr[0] = VarPartValue[0];
      break;
    case 2:
      ValuePtr[0] = VarPartValue[0];
      ValuePtr[1] = VarPartValue[1];
      break;
    case 4:
      ValuePtr[0] = VarPartValue[0];
      ValuePtr[1] = VarPartValue[1];
      ValuePtr[2] = VarPartValue[2];
      ValuePtr[3] = VarPartValue[3];
      break;
    case 8:
      ValuePtr[0] = VarPartValue[0];
      ValuePtr[1] = VarPartValue[1];
      ValuePtr[2] = VarPartValue[2];
      ValuePtr[3] = VarPartValue[3];
      ValuePtr[4] = VarPartValue[4];
      ValuePtr[5] = VarPartValue[5];
      ValuePtr[6] = VarPartValue[6];
      ValuePtr[7] = VarPartValue[7];
      break;
    default:
      SAPDB_MemCopyNoCheck ( ValuePtr, VarPartValue, VarPartValueLength );
      break;
    }
  }
  else if ( Optional == false )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_VARPART_VALUE_NOT_FOUND, ID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  return true;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEReqRespVarPart::eo42_Swap( teo41_Swap const            &Swap,
                                         teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("RTEReqRespPacket::eo42_Swap"));
  tsp00_Uint2              Pos;
  char *                   ValuePtr;
  char *                   VarPartValue;
  tsp00_Uint2              VarPartValueLength = 0;
  tsp00_Uint2              HeaderLen          = 3;
  tsp00_Uint2              TmpUint2Value;
  tsp00_Uint4              TmpUint4Value;
  tsp00_Uint8              TmpUint8Value;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!!!  ALWAYS USE CHARACTER POINTER FOR UNALIGNED VARPART ACCESS   !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if ( OldVarPart_eo42 )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
    HeaderLen = 2;

  // --- Swap all values
  for ( Pos = 0; Pos < VarPartLen_eo42;
        Pos += VarPartValueLength + HeaderLen )
  {
    if ( OldVarPart_eo42 )  // - old communication only, 6.2 - 7.2 !!!!!!!!!!!!
      VarPartValueLength = VarPart_eo42[Pos];
    else // --- new stuff !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      VarPartValueLength = ( VarPart_eo42[ Pos + 1 ] << 8 ) | ( VarPart_eo42[ Pos + 2 ] );

    if ( Pos + HeaderLen + VarPartValueLength > VarPartSize_eo42 )
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INVALID_VAR_PART_LENGTH, VarPartValueLength );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    VarPartValue  = &VarPart_eo42[Pos + HeaderLen];

    switch ( VarPartValueLength )
    {
    case 1:
      break;
    case 2:
      ValuePtr        = (char*)&TmpUint2Value;
      ValuePtr[0]     = VarPartValue[0];
      ValuePtr[1]     = VarPartValue[1];

      Swap.eo41_SwapValue( TmpUint2Value );

      VarPartValue[0] = ValuePtr[0];
      VarPartValue[1] = ValuePtr[1]; 
      break;
    case 4:
      ValuePtr        = (char*)&TmpUint4Value;
      ValuePtr[0]     = VarPartValue[0];
      ValuePtr[1]     = VarPartValue[1];
      ValuePtr[2]     = VarPartValue[2];
      ValuePtr[3]     = VarPartValue[3];

      Swap.eo41_SwapValue( TmpUint4Value );

      VarPartValue[0] = ValuePtr[0];
      VarPartValue[1] = ValuePtr[1]; 
      VarPartValue[2] = ValuePtr[2]; 
      VarPartValue[3] = ValuePtr[3]; 
      break;
    case 8:
      ValuePtr        = (char*)&TmpUint8Value;
      ValuePtr[0]     = VarPartValue[0];
      ValuePtr[1]     = VarPartValue[1];
      ValuePtr[2]     = VarPartValue[2];
      ValuePtr[3]     = VarPartValue[3];
      ValuePtr[4]     = VarPartValue[4];
      ValuePtr[5]     = VarPartValue[5];
      ValuePtr[6]     = VarPartValue[6];
      ValuePtr[7]     = VarPartValue[7];

      Swap.eo41_SwapValue( TmpUint8Value );

      VarPartValue[0] = ValuePtr[0];
      VarPartValue[1] = ValuePtr[1]; 
      VarPartValue[2] = ValuePtr[2]; 
      VarPartValue[3] = ValuePtr[3]; 
      VarPartValue[4] = ValuePtr[4]; 
      VarPartValue[5] = ValuePtr[5]; 
      VarPartValue[6] = ValuePtr[6]; 
      VarPartValue[7] = ValuePtr[7]; 
      break;
    default:
      break;
    }
  }

  return true;
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class teo42_RTEErrorResp                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

bool teo42_RTEErrorResp::eo42_FillPacketData ( void * const                PacketDataBuffer,
                                               tsp00_Uint4 const           PacketDataBufferSize,
                                               tsp01_CommErr_Enum          CommState,
                                               teo200_EventList const      &EventList,
                                               tsp00_Uint4                 &DataLen,
                                               teo200_EventList * const    pEventList ) const
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEErrorResp::eo42_FillPacketData"));


  bool                        Ok;
  tsp00_Uint4                 TruncBufferSizeReq;
  tsp00_Uint4                 EvtLstBufferSize;
  tsp00_Uint4                 EvtLstLen;
  teo42_RTEErrorRespData *    pRTEErrorRespData;

# if defined (_WIN32)
   teo00_Int1 OSType = OSTYPE_WIN32_EO42;
# else
   teo00_Int1 OSType = OSTYPE_UNIX_OS_EO42;
# endif
  pRTEErrorRespData  = (teo42_RTEErrorRespData*)PacketDataBuffer;

  pRTEErrorRespData->OSType_eo42         = OSType;
  pRTEErrorRespData->Filler1_eo42        = 0; 
  pRTEErrorRespData->CommState_eo42      = (tsp00_Uint2)CommState;
  pRTEErrorRespData->RequestVersion_eo42 = 0;
  pRTEErrorRespData->Filler2_eo42        = 0; 

  EvtLstBufferSize = PacketDataBufferSize - offsetof(teo42_RTEErrorRespData, EvtLstBuffer_eo42);

  // --- get the buffer size required to store at least on event of the event list
  EventList.eo200_DataBufferSize( &TruncBufferSizeReq );

  if ( EvtLstBufferSize >= TruncBufferSizeReq )
  {
    Ok = EventList.eo200_StoreEventList( pRTEErrorRespData->EvtLstBuffer_eo42, EvtLstBufferSize, 
                                         EvtLstLen, true, pEventList );
  }
  else
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_EVTLST_DROPPED );
    
    Ok = EventList.eo200_StoreEventList( pRTEErrorRespData->EvtLstBuffer_eo42, EvtLstBufferSize,
                                         EvtLstLen, false, pEventList );
  }

  DataLen = EvtLstLen + offsetof(teo42_RTEErrorRespData, EvtLstBuffer_eo42);

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEErrorResp::eo42_SwapPacketData ( void * const                PacketDataBuffer,
                                               tsp00_Uint4 const           DataLen,
                                               teo41_Swap const            &Swap,
                                               teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEErrorResp::eo42_SwapPacketData"));

  teo42_RTEErrorRespData *  pRTEErrorRespData = (teo42_RTEErrorRespData*)PacketDataBuffer;

  Swap.eo41_SwapValue(pRTEErrorRespData->OSType_eo42);
  Swap.eo41_SwapValue(pRTEErrorRespData->RequestVersion_eo42);
  Swap.eo41_SwapValue(pRTEErrorRespData->CommState_eo42);

  return true;
}
/*---------------------------------------------------------------------------*/

bool teo42_RTEErrorResp::eo42_ExtractPacketData ( void * const                PacketDataBuffer,
                                                  tsp00_Uint4 const           DataLen,
                                                  tsp01_CommErr_Enum          &CommState,
                                                  teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEErrorResp::eo42_ExtractPacketData"));
  tsp00_Uint4               EvtLstLen;
  teo42_RTEErrorRespData *  pRTEErrorRespData;
  bool                      Ok;

  EvtLstLen         = DataLen - offsetof(teo42_RTEErrorRespData, EvtLstBuffer_eo42);
  pRTEErrorRespData = (teo42_RTEErrorRespData*)PacketDataBuffer;

  CommState = (tsp01_CommErr_Enum)pRTEErrorRespData->CommState_eo42;

  if ( pEventList != NULL )
    Ok = pEventList->eo200_LoadEventList( pRTEErrorRespData->EvtLstBuffer_eo42, EvtLstLen, pEventList );
  else
  {
    teo200_EventList EvtLst;

    EvtLst.eo200_LoadEventList( pRTEErrorRespData->EvtLstBuffer_eo42, EvtLstLen, pEventList );

    throw EvtLst;
  }

  return Ok;
}



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class teo42_RTEReqRespBase                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo42_RTEReqRespBase::eo42_FillPacketData ( void * const                PacketDataBuffer,
                                                 tsp00_Uint4 const           PacketDataBufferSize,
                                                 tsp00_Uint4                 &DataLen,
                                                 teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_FillPacketData"));

  bool                              Ok;
  void *                            VarPartBuffer;
  tsp00_Uint2                       VarPartSize;

  VarPartBuffer = eo42_CalcVarPartBuffer     ( PacketDataBuffer );
  VarPartSize   = eo42_CalcVarPartBufferSize ( PacketDataBufferSize );

  // --- create varpart object
  teo42_RTEReqRespVarPart   VarPart( VarPartBuffer, VarPartSize );

  Ok = eo42_FillVarPart ( VarPart, pEventList );

  if ( Ok ) 
    Ok = eo42_FillFixedPart( PacketDataBuffer, PacketDataBufferSize, VarPart.eo42_VarPartLen(), DataLen, pEventList );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEReqRespBase::eo42_ExtractPacketData ( void * const                PacketDataBuffer,
                                                    tsp00_Uint4 const           DataLen,
                                                    teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_ExtractPacketData"));

  bool                              Ok;
  void *                            VarPartBuffer;
  tsp00_Uint2                       VarPartSize     = (tsp00_Uint2)UNDEF_SP00;
  tsp00_Uint2                       VarPartLen      = 0;

  Ok = eo42_ExtractFixedPart ( PacketDataBuffer, DataLen, VarPartLen, pEventList );
   
  if (( Ok ) && ( VarPartLen > 0 ))
  {
    VarPartBuffer = eo42_CalcVarPartBuffer ( PacketDataBuffer );

    // --- create varpart object
    teo42_RTEReqRespVarPart VarPart( VarPartBuffer, VarPartSize, VarPartLen );

    Ok = eo42_ExtractVarPart( VarPart, pEventList );
  }
  
  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEReqRespBase::eo42_SwapPacketData ( void * const                PacketDataBuffer,
                                                 tsp00_Uint4 const           DataLen,
                                                 teo41_Swap const            &Swap,
                                                 teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_SwapPacketData"));

  bool    Ok = true;

  if ( Swap.eo41_SwappingRequired() == true )
  {
    void *         VarPartBuffer;
    tsp00_Uint2    VarPartSize     = (tsp00_Uint2)UNDEF_SP00;
    tsp00_Uint2    VarPartLen      = 0;
    
    Ok = eo42_SwapFixedPart ( PacketDataBuffer, DataLen, VarPartLen, Swap, pEventList );
    
    if (( Ok ) && ( VarPartLen > 0 ))
    {
      VarPartBuffer = eo42_CalcVarPartBuffer ( PacketDataBuffer );
    
      // --- create varpart object
      teo42_RTEReqRespVarPart VarPart( VarPartBuffer, VarPartSize, VarPartLen );

      Ok = VarPart.eo42_Swap( Swap, pEventList );
    }
  }
   
  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo42_RTEReqRespBase::eo42_FillFixedPart ( void * const                PacketDataBuffer,
                                                tsp00_Uint4 const           PacketDataBufferSize,
                                                teo00_Uint2 const           VarPartLen,
                                                teo00_Uint4                 &DataLen,
                                                teo200_EventList *  const   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_FillFixedPart"));

  teo42_RTEReqRespData * pReqRespData;

  if ( PacketDataBufferSize < eo42_MinPacketDataBufferSize() )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_WRONG_BUFFER_SIZE, 
                             eo42_MinPacketDataBufferSize() );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

# if defined (_WIN32)
   teo00_Int1 OSType = OSTYPE_WIN32_EO42;
# else
   teo00_Int1 OSType = OSTYPE_UNIX_OS_EO42;
# endif

  pReqRespData = (teo42_RTEReqRespData*)PacketDataBuffer;

  // - distinguish different versions of the same request type ( for future use ) 
  pReqRespData->RequestVersion_eo42  = eo42_FixedPartParams().RequestVersion_eo42; 

  pReqRespData->Filler1_eo42         = 0;
  pReqRespData->OSType_eo42          = OSType;
  pReqRespData->VarPartLen_eo42      = VarPartLen;

  DataLen = offsetof(teo42_RTEReqRespData, VarPart_eo42) + VarPartLen;

  return true;
}

/*---------------------------------------------------------------------------*/

bool  teo42_RTEReqRespBase::eo42_SwapFixedPart ( void * const                PacketDataBuffer,
                                                 tsp00_Uint4 const           DataLen,
                                                 tsp00_Uint2                 &VarPartLen,
                                                 teo41_Swap const            &Swap,
                                                 teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_SwapFixedPart"));


  if ( DataLen < offsetof(teo42_RTEReqRespData, VarPart_eo42) )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_DATA_LEN, 
                             DataLen, offsetof(teo42_RTEReqRespData, VarPart_eo42) );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  
  teo42_RTEReqRespData *   pReqRespData;

  pReqRespData = (teo42_RTEReqRespData*)PacketDataBuffer;

  Swap.eo41_SwapValue(pReqRespData->RequestVersion_eo42);
  Swap.eo41_SwapValue(pReqRespData->OSType_eo42);
  Swap.eo41_SwapValue(pReqRespData->VarPartLen_eo42);

  VarPartLen = pReqRespData->VarPartLen_eo42;

  if ( VarPartLen != DataLen - offsetof(teo42_RTEReqRespData, VarPart_eo42) )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_VARPART_LEN,
                             VarPartLen, DataLen - offsetof(teo42_RTEReqRespData, VarPart_eo42) );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool  teo42_RTEReqRespBase::eo42_ExtractFixedPart ( void * const                PacketDataBuffer,
                                                    tsp00_Uint4 const           DataLen,
                                                    tsp00_Uint2                 &VarPartLen,
                                                    teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_ExtractFixedPart"));

  if ( DataLen < offsetof(teo42_RTEReqRespData, VarPart_eo42) )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_DATA_LEN, 
                             DataLen, offsetof(teo42_RTEReqRespData, VarPart_eo42) );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  teo42_RTEReqRespData *   pReqRespData;

  pReqRespData = (teo42_RTEReqRespData*)PacketDataBuffer;

  eo42_FixedPartParams().OSType_eo42          = pReqRespData->OSType_eo42;
  eo42_FixedPartParams().RequestVersion_eo42  = pReqRespData->RequestVersion_eo42;

  VarPartLen = pReqRespData->VarPartLen_eo42;

  if ( VarPartLen != DataLen - offsetof(teo42_RTEReqRespData, VarPart_eo42) )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_VARPART_LEN,
                             VarPartLen, DataLen - offsetof(teo42_RTEReqRespData, VarPart_eo42) );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

void * teo42_RTEReqRespBase::eo42_CalcVarPartBuffer ( void * const PacketDataBuffer ) const
{     
  ROUTINE_DBG_MEO60 (_T("teo42_RTEReqRespBase::eo42_CalcVarPartBuffer"));

  return ((teo42_RTEReqRespData*)PacketDataBuffer)->VarPart_eo42;
};

/*---------------------------------------------------------------------------*/

tsp00_Uint2 teo42_RTEReqRespBase::eo42_CalcVarPartBufferSize ( tsp00_Uint2  PacketDataBufferSize ) const
{     
  return PacketDataBufferSize - offsetof(teo42_RTEReqRespData, VarPart_eo42);
};


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
