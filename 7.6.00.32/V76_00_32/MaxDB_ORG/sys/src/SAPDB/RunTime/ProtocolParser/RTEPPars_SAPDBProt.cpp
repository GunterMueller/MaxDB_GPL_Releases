/*!**************************************************************************

  module      : RTEPPars_SAPDBProt.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: SAPDB Protocol Parser
  description : 


  last changed: 2002-08-16  10:51
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

#include "RunTime/ProtocolParser/RTEPPars_SAPDBProt.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define  BASE_HEX                           16
#define  BASE_DEC                           10

#define FORMAT_STRING_LEN                   120
#define NEGATIVE_TCP_ACKNOWLEDGE_OFFSET     12

#define PROT_GARBELED_SUMMARY_LABEL         "Request/reply garbled"
#define PROT_SUMMARY_LABEL                  "Request/reply"
#define PROT_MESS_CLASS_LABEL               "MessClass"
#define PROT_SENDER_REF_LABEL               "SenderRef"
#define PROT_RECV_REF_LABEL                 "ReceiverRef"
#define PROT_ACT_SND_LEN_LABEL              "ActSendLen"
#define PROT_MAX_SND_LEN_LABEL              "MaxSendLen"
#define PROT_PROT_ID_LABEL                  "ProtocolID"
#define PROT_SWAP_TYPE_LABEL                "SwapType"
#define PROT_RESIDUAL_PACK_LABEL            "ResidualPackets"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define OFFSET_OFF(_strct,_elem)   ((size_t)&(((_strct *)0)->_elem))
#define SIZE_OFF(_strct,_elem)     sizeof(((_strct *)0)->_elem)


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

RTEPPars_SAPDBProt * RTEPPars_SAPDBProt::m_Instance = NULL;


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTEPPars_SAPDBProt::RTEPPars_SAPDBProt () 
                   : m_Attached( 0 ), 
                     m_hProtocol( NULL ),
                     m_ProtocolName( "SAPDB" ),
                     m_ProtocolInfo( "SAPDB protocol" ),
                     m_Port( 7210 ) ,
                     m_NumParsers ( 1 ),
                     m_NumFollows ( 0 ),
                     m_NumHandoffs( 1 ),
                     m_PropertyTable( NULL ),
                     m_NumProperties( 0 )
{
  m_EntryPoints.AttachProperties = reinterpret_cast <ATTACHPROPERTIES> (_AttachProperties);
  m_EntryPoints.Register         = _Register;
  m_EntryPoints.Deregister       = _Deregister;
  m_EntryPoints.FormatProperties = reinterpret_cast <FORMATPROPERTIES> (_FormatProperties);
  m_EntryPoints.RecognizeFrame   = reinterpret_cast <RECOGNIZEFRAME> (_RecognizeFrame);
}

/*---------------------------------------------------------------------------*/

RTEPPars_SAPDBProt & RTEPPars_SAPDBProt::Instance( )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEPPars_SAPDBProt::Instance", Memory_Trace, 5);

  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEPPars_SAPDBProt));

  if ( m_Instance == NULL )
    m_Instance = new (Space) RTEPPars_SAPDBProt();

  return *m_Instance;
}

/*---------------------------------------------------------------------------*/

void RTEPPars_SAPDBProt::CreateProtocol()
{
  if( m_Attached == 0 )
    m_hProtocol = ::CreateProtocol( m_ProtocolName, &m_EntryPoints, sizeof (m_EntryPoints) );

  m_Attached++;
}

/*---------------------------------------------------------------------------*/

void RTEPPars_SAPDBProt::DestroyProtocol()
{
  m_Attached--;

  if( m_Attached == 0 )
  {
    ::DestroyProtocol( m_hProtocol );
    m_hProtocol = NULL;
  }
}

/*---------------------------------------------------------------------------*/

DWORD BHAPI RTEPPars_SAPDBProt::Register( HPROTOCOL hProtocol )
{
  // Create property database.
  DWORD             NumProperties   = 100;
  DWORD             MaxEntries      = 20;
  DWORD             rc              = NO_ERROR;
  DWORD             Size            = NumProperties * sizeof(RTEPPars_SAPDBPropEntry); 

  m_PropertyTable = (RTEPPars_SAPDBPropEntry*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, Size );

  if ( m_PropertyTable == NULL )
  {
    rc = GetLastError();
    return rc;
  }

  if ( CreatePropertyDatabase ( m_hProtocol, NumProperties ) != NMERR_SUCCESS )
    rc = GetLastError();


  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 0, hProtocol, -1, 0, 0, 0,
                      PROT_GARBELED_SUMMARY_LABEL,  "Summary of protocol packet", PROP_TYPE_SUMMARY, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 0, hProtocol, -1, 0, 0, 0,
                      PROT_SUMMARY_LABEL,  "Summary of protocol packet", PROP_TYPE_SUMMARY, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 1, hProtocol, RTE_HEADER_SIZE, 0, 0, 0,
                      "RTEHeader",  "RTEHeader", PROP_TYPE_SUMMARY, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,ActSendLen),
                      OFFSET_OFF(teo003_RteHeaderRecord, ActSendLen), 0, 0,
                      PROT_ACT_SND_LEN_LABEL,  "Actual send length", PROP_TYPE_DWORD, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,ProtocolID),
                      OFFSET_OFF(teo003_RteHeaderRecord, ProtocolID), 0, 0,
                      PROT_PROT_ID_LABEL,  "Protocol identifier", PROP_TYPE_BYTE, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,MessClass),
                      OFFSET_OFF(teo003_RteHeaderRecord, MessClass), 0, 0,
                      PROT_MESS_CLASS_LABEL, "Message class", PROP_TYPE_BYTE, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,RTEFlags),
                      OFFSET_OFF(teo003_RteHeaderRecord, RTEFlags), 0, 0,
                      "RTEFlags",  "RTE Flags", PROP_TYPE_BYTE, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,ResidualPackets),
                      OFFSET_OFF(teo003_RteHeaderRecord, ResidualPackets), 0, 0,
                      PROT_RESIDUAL_PACK_LABEL,  "Residual packets", PROP_TYPE_BYTE, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,SenderRef),
                      OFFSET_OFF(teo003_RteHeaderRecord, SenderRef), 0, 0,
                      PROT_SENDER_REF_LABEL,  "Sender reference", PROP_TYPE_DWORD, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,ReceiverRef),
                      OFFSET_OFF(teo003_RteHeaderRecord, ReceiverRef), 0, 0,
                      PROT_RECV_REF_LABEL,  "Receiver reference", PROP_TYPE_DWORD, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,RTEReturnCode),
                      OFFSET_OFF(teo003_RteHeaderRecord, RTEReturnCode), 0, 0,
                      "RTEReturnCode",  "RTE return code", PROP_TYPE_WORD, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,NewSwapType),
                      OFFSET_OFF(teo003_RteHeaderRecord, NewSwapType), 0, 0,
                      PROT_SWAP_TYPE_LABEL,  "Swap type", PROP_TYPE_BYTE, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  {
    rc = AddProperty( m_NumProperties++, 2, hProtocol, SIZE_OFF(teo003_RteHeaderRecord,MaxSendLen),
                      OFFSET_OFF(teo003_RteHeaderRecord, MaxSendLen), 0, 0,
                      PROT_MAX_SND_LEN_LABEL,  "Maximum send length", PROP_TYPE_DWORD, PROP_QUAL_NONE );
  }

  if (( rc == NO_ERROR ) && ( m_NumProperties < NumProperties ))
  { 
    WORD Len = (WORD)sizeof(teo003_RteHeaderRecord) + 1;
    rc = AddProperty( m_NumProperties++, 1, hProtocol,  -Len,
                     sizeof(teo003_RteHeaderRecord), 0, 0,
                     "Data Part",  "Data", PROP_TYPE_RAW_DATA, PROP_QUAL_NONE );
  }

  // Create handoff table.
  if ( rc == NO_ERROR )
  {
    if ( CreateHandoffTable("HandoffTable", "SAPDBProt.INI", &m_hTable, MaxEntries, BASE_HEX ) == 0 )
      rc = GetLastError();
  }

  return rc;
}

/*---------------------------------------------------------------------------*/

DWORD BHAPI RTEPPars_SAPDBProt::Deregister( HPROTOCOL hProtocol )
{

  if ( m_PropertyTable )
  {
    HeapFree( GetProcessHeap(), HEAP_NO_SERIALIZE, m_PropertyTable );
    m_PropertyTable = NULL;
  }

  if ( m_hProtocol )
    DestroyPropertyDatabase( m_hProtocol );

  return 0;
}

/*---------------------------------------------------------------------------*/

LPBYTE BHAPI RTEPPars_SAPDBProt::AttachProperties( HFRAME     hFrame,
                                                   LPBYTE     pMacFrame,
                                                   LPBYTE     pFrame,
                                                   DWORD      MacType,
                                                   DWORD      BytesLeft,
                                                   HPROTOCOL  hPreviousProtocol,
                                                   DWORD      nPrevProtocolOffset,
                                                   DWORD      InstData )
{
  DWORD                    EntryNo;
  DWORD                    StartNo;
  DWORD                    StopNo;
  SAPDB_Int4               ActSendLen       = 0;
  SAPDB_Int4               MaxSendLen       = 0;
  SAPDB_UInt1              ProtocolID       = (SAPDB_UInt1)-1;
  SAPDB_UInt1              SwapType         = (SAPDB_UInt1)-1;
  SAPDB_UInt1              ResidualPackets  = (SAPDB_UInt1)-1;
  PCHAR                    pMessClass;

  for ( SAPDB_UInt4 i = 0; i < m_NumProperties; i++ )
  {
    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_ACT_SND_LEN_LABEL ))
      ActSendLen = *(SAPDB_Int4*)(pFrame + m_PropertyTable[i].DataOffset);

    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_MAX_SND_LEN_LABEL ))
      MaxSendLen = *(SAPDB_Int4*)(pFrame + m_PropertyTable[i].DataOffset);

    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_MESS_CLASS_LABEL ))
      pMessClass = GetMessClassName ( pFrame[m_PropertyTable[i].DataOffset] );

    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_PROT_ID_LABEL ))
      ProtocolID = *(SAPDB_UInt1*)(pFrame + m_PropertyTable[i].DataOffset);

    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_SWAP_TYPE_LABEL ))
      SwapType = *(SAPDB_UInt1*)(pFrame + m_PropertyTable[i].DataOffset);

    if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_RESIDUAL_PACK_LABEL ))
      ResidualPackets = *(SAPDB_UInt1*)(pFrame + m_PropertyTable[i].DataOffset);
    
  }

  if (( SwapType > 7 ) || 
      ( ProtocolID > 128 ) || 
      ( ActSendLen > MaxSendLen ) ||
      (( ActSendLen == MaxSendLen ) && ( ResidualPackets > 1 )))
  {
    StartNo = 0;
    StopNo  = 1;
  }
  else
  {
    StartNo = 1;
    StopNo  = m_NumProperties;
  }

  for ( EntryNo = StartNo; EntryNo < StopNo; EntryNo++ )
  {
    WORD Length;

    if ( m_PropertyTable[EntryNo].Length < 0 )
      Length = (WORD)BytesLeft + m_PropertyTable[EntryNo].Length + 1;
    else
      Length = (WORD)m_PropertyTable[EntryNo].Length;

    AttachPropertyInstance( hFrame,
                             m_PropertyTable[EntryNo].Info.hProperty,
                             Length,
                             (LPBYTE)pFrame + m_PropertyTable[EntryNo].DataOffset,
                             m_PropertyTable[EntryNo].HelpID,
                             m_PropertyTable[EntryNo].IdentLevel,
                             m_PropertyTable[EntryNo].IFlags );
  }


  return NULL;
}

/*---------------------------------------------------------------------------*/

DWORD BHAPI RTEPPars_SAPDBProt::FormatProperties ( HFRAME         hFrame,
                                                   LPBYTE         pMacFrame,
                                                   LPBYTE         pFrame,
                                                   DWORD          nPropertyInsts,
                                                   PROPERTYINST   *p )
{
  while( nPropertyInsts-- > 0)
  {
    if (( !strcmp ( p->lpPropertyInfo->Label, PROT_GARBELED_SUMMARY_LABEL )) ||
        ( !strcmp ( p->lpPropertyInfo->Label, PROT_SUMMARY_LABEL )))
      FormatSummaryProperty (pFrame, pMacFrame, p);
    else
      ( (FORMAT) p->lpPropertyInfo->InstanceData )( p );

    p++;
  }
  return NMERR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

void RTEPPars_SAPDBProt::FormatSummaryProperty ( LPBYTE         pFrame,
                                                 LPBYTE         pMacFrame,
                                                 PROPERTYINST   *p )
{
  PROPERTYINFO* pTmp = p->lpPropertyInfo;
  PROPERTYINFO  Tmp  = *p->lpPropertyInfo;
  CHAR*         pRequestReply = Tmp.Label;
  SAPDB_Int4    SenderRef   = -1;
  SAPDB_Int4    ReceiverRef = -1;
  SAPDB_Int4    ActSendLen  = 0;
  char          NewLabel[FORMAT_STRING_LEN];
  char          Garbled[FORMAT_STRING_LEN];

  if ( !strcmp ( p->lpPropertyInfo->Label, PROT_GARBELED_SUMMARY_LABEL ))
  {
    SAPDB_UInt4 TCPAckNum = TCPAcknowledgeNum ( pFrame,  pMacFrame );

    if ( TCPAckNum > 0 )
      sprintf( NewLabel, "%s [TCPAck: 0x%X, Len: %d]", Tmp.Label, TCPAckNum, p->DataLength );
    else
      sprintf( NewLabel, "%s, [Len: %d bytes]", Tmp.Label, p->DataLength );
  }
  else
  {
    for ( SAPDB_UInt4 i = 0; i < m_NumProperties; i++ )
    {
      if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_MESS_CLASS_LABEL ))
      {
        pRequestReply = GetMessClassName ( pFrame[m_PropertyTable[i].DataOffset] );
        break;
      }
    }

    for ( i = 0; i < m_NumProperties; i++ )
    {
      if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_SENDER_REF_LABEL ))
        SenderRef = *(SAPDB_UInt4*)(pFrame + m_PropertyTable[i].DataOffset);

      if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_RECV_REF_LABEL ))
        ReceiverRef = *(SAPDB_UInt4*)(pFrame + m_PropertyTable[i].DataOffset);

      if ( !strcmp( m_PropertyTable[i].Info.Label, PROT_ACT_SND_LEN_LABEL ))
        ActSendLen = *(SAPDB_Int4*)(pFrame + m_PropertyTable[i].DataOffset);
    }

    if ( p->DataLength != ActSendLen )
    {
      SAPDB_UInt4 TCPAckNum = TCPAcknowledgeNum ( pFrame,  pMacFrame );

      if ( TCPAckNum > 0 )
        sprintf( Garbled, " [TCPAck: 0x%X, Garbled packet, %d bytes left]", 
                  TCPAckNum, ActSendLen - p->DataLength );
      else
        sprintf( Garbled, " [Garbled packet, %d bytes left]", ActSendLen - p->DataLength );
    }
    else
      Garbled[0] = '\0';

    if (( SenderRef == -1 ) && ( ReceiverRef == -1 ))
      sprintf( NewLabel, "%s%s", pRequestReply, Garbled );
    else if ( ReceiverRef == -1 )
      sprintf( NewLabel, "%s%s ( Sender: %d )", pRequestReply, Garbled, SenderRef  );
    else if ( SenderRef == -1 )
      sprintf( NewLabel, "%s%s ( Receiver: %d )", pRequestReply, Garbled, ReceiverRef );
    else 
      sprintf( NewLabel, "%s%s ( Sender: %d, Receiver: %d )", pRequestReply, Garbled, SenderRef, ReceiverRef );
  }

  Tmp.Label = NewLabel;

  p->lpPropertyInfo = &Tmp;
  ( (FORMAT) p->lpPropertyInfo->InstanceData )( p );
  p->lpPropertyInfo = pTmp;

}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEPPars_SAPDBProt::TCPAcknowledgeNum ( LPBYTE                    pFrame, 
                                                    LPBYTE                    pMacFrame )
{
  SAPDB_UInt4 TCPAckNum;

  if ( pFrame - pMacFrame > NEGATIVE_TCP_ACKNOWLEDGE_OFFSET )
  {
    // NASTY HACK - we are looking into the TCP part to get the acknowledgement
    //              number 
    TCPAckNum = *(pFrame - NEGATIVE_TCP_ACKNOWLEDGE_OFFSET + 0) << 24 |
                *(pFrame - NEGATIVE_TCP_ACKNOWLEDGE_OFFSET + 1) << 16 |
                *(pFrame - NEGATIVE_TCP_ACKNOWLEDGE_OFFSET + 2) << 8 |
                *(pFrame - NEGATIVE_TCP_ACKNOWLEDGE_OFFSET + 3);
  }
  else
  {
    TCPAckNum = 0;
  }

  return TCPAckNum;
}

/*---------------------------------------------------------------------------*/

PCHAR const RTEPPars_SAPDBProt::GetMessClassName ( SAPDB_UInt1   MessClass )
{
  PCHAR pMessClass;

  switch (  MessClass )
  {
  case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003:
    pMessClass = "Keep alive request";
    break;
  case RSQL_INFO_REQUEST_EO003:
    pMessClass = "Info request";
    break;
  case RSQL_INFO_REPLY_EO003:
    pMessClass = "Info reply";
    break;
  case RSQL_PING_REQUEST_EO003:
    pMessClass = "Ping request";
    break;
  case RSQL_PING_REPLY_EO003:
    pMessClass = "Ping reply";
    break;
  case RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003:
    pMessClass = "New debug level request";
    break;
  case RSQL_NEW_DEBUG_LEVEL_REPLY_EO003:  
    pMessClass = "New debug level reply";
    break;
  case RSQL_USER_CONN_REQUEST_EO003:
    pMessClass = "User connect request";
    break;
  case RSQL_USER_CONN_REPLY_EO003:
    pMessClass = "User connect reply";
    break;
  case RSQL_USER_DATA_REQUEST_EO003:
    pMessClass = "User data request";
    break;
  case RSQL_USER_DATA_REPLY_EO003:
    pMessClass = "User data reply";
    break;
  case RSQL_USER_CANCEL_REQUEST_EO003:
    pMessClass = "Cancel request";
    break;
  case RSQL_USER_RELEASE_REQUEST_EO003:
    pMessClass = "User release request";
    break;
  case RSQL_KERN_CONN_REQUEST_EO003:
    pMessClass = "Kernel connect request";
    break;
  case RSQL_KERN_CONN_REPLY_EO003:
    pMessClass = "Kernel connect reply";
    break;
  case RSQL_KERN_DATA_REQUEST_EO003:
    pMessClass = "Kernel data request";
    break;
  case RSQL_KERN_DATA_REPLY_EO003:
    pMessClass = "Kernel data reply";
    break;
  case RSQL_KERN_RELEASE_REQUEST_EO003:
    pMessClass = "Kernel release request";
    break;
  case RSQL_DUMP_REQUEST_EO003:
    pMessClass = "Dump request";
    break;
  case RSQL_CTRL_CONN_REQUEST_EO003:
    pMessClass = "Control connect request";
    break;
  case RSQL_CTRL_CONN_REPLY_EO003:
    pMessClass = "Control connect reply";
    break;
  case RSQL_CTRL_CANCEL_REQUEST_EO003:
    pMessClass = "Control cancel request";
    break;
  case RSQL_NEW_USER_CONN_REQUEST_EO003:
    pMessClass = "New user connect request";
    break;
  default:
    pMessClass = NULL;
    break;
  }

  return pMessClass;
}

/*---------------------------------------------------------------------------*/

LPBYTE BHAPI RTEPPars_SAPDBProt::RecognizeFrame ( HFRAME     hFrame,
                                                  LPBYTE     pMacFrame,
                                                  LPBYTE     pProtocol,
                                                  DWORD      MacType,
                                                  DWORD      BytesLeft,
                                                  HPROTOCOL  hPrevProtocol,
                                                  DWORD      nPreviuosProtOffset,
                                                  LPDWORD    pProtocolStatus,
                                                  HPROTOCOL  *phNextProtocol,
                                                  LPDWORD    InstData )
{
  // If unclaimed data is recognized but you do not know what follows.
  *pProtocolStatus =  PROTOCOL_STATUS_RECOGNIZED;

  return  pProtocol + RTE_HEADER_SIZE;

#if 0  
  DWORD     ItemToFind = 0;
  LPDWORD   lpInstData;

  // If unclaimed data is recognized and you know what follows.
  *pProtocolStatus =  PROTOCOL_STATUS_NEXT_PROTOCOL;

  *phNextProtocol = GetProtocolFromTable( m_hTable, ItemToFind, lpInstData );

  return  pProtocol + RTE_HEADER_SIZE;
  
  // If unclaimed data is recognized and the protocol is the last 
  // protocol in the frame.
  *pProtocolStatus =  PROTOCOL_STATUS_CLAIMED;
  return NULL;
  
  // If the unclaimed data is not recognized.
  *pProtocolStatus =  PROTOCOL_STATUS_NOT_RECOGNIZED;

  return pProtocol;
#endif
}
/*---------------------------------------------------------------------------*/

DWORD RTEPPars_SAPDBProt::AddProperty ( DWORD                     EntryNo,
                                        DWORD                     IdentLevel,
                                        HPROTOCOL                 hProtocol,
                                        SAPDB_Int4                Length,
                                        DWORD                     DataOffset,
                                        DWORD                     HelpID,
                                        DWORD                     IFlags,
                                        LPSTR const               Label,
                                        LPSTR const               Comment,
                                        BYTE                      DataType,
                                        BYTE                      DataQualifier )
{
  RTEPPars_SAPDBPropEntry   *pEntry = &m_PropertyTable[EntryNo];

  memset ( pEntry, 0, sizeof(*pEntry) );

  
  pEntry->Length                = Length;
  pEntry->DataOffset            = DataOffset;
  pEntry->IdentLevel            = IdentLevel;
  pEntry->HelpID                = HelpID;
  pEntry->IFlags                = IFlags;
  pEntry->Info.Label            = Label;
  pEntry->Info.Comment          = Comment;
  pEntry->Info.DataType         = DataType;
  pEntry->Info.DataQualifier    = DataQualifier;
  pEntry->Info.FormatStringSize = FORMAT_STRING_LEN;
  pEntry->Info.InstanceData     = FormatPropertyInstance;

  if ( ::AddProperty( hProtocol, &pEntry->Info ) == NULL )
    return GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

PPF_PARSERDLLINFO RTEPPars_SAPDBProt::ParserAutoInstallInfo() 
{
  // Allocate memory for PF_PARSERDLLINFO structure.
  PPF_PARSERDLLINFO pParserDllInfo; 
  PPF_PARSERINFO    pParserInfo;
  DWORD             Size  = sizeof( PF_PARSERDLLINFO ) + m_NumParsers * sizeof( PF_PARSERINFO);
  
  pParserDllInfo = (PPF_PARSERDLLINFO)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, Size );

  if( pParserDllInfo == NULL)
    return NULL;
    
  // Specify the number of parsers in the DLL.
  pParserDllInfo->nParsers = m_NumParsers;

  pParserInfo = &(pParserDllInfo->ParserInfo[0]); // Specify the name, comment, and Help file for each protocol.
  sprintf( pParserInfo->szProtocolName, m_ProtocolName );
  sprintf( pParserInfo->szComment,      m_ProtocolInfo );
  sprintf( pParserInfo->szHelpFile,     "");
  
  // Specify preceding protocols.
  PPF_HANDOFFSET    pHandoffSet;
  PPF_HANDOFFENTRY  pHandoffEntry;
  
  // Allocate PF_HANDOFFSET structure.
  Size = sizeof( PF_HANDOFFSET ) + m_NumHandoffs * sizeof( PF_HANDOFFENTRY);

  pHandoffSet = (PPF_HANDOFFSET)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, Size );

  if( pHandoffSet == NULL )
     return pParserDllInfo;

  // Fill in handoff set
  pParserInfo->pWhoHandsOffToMe = pHandoffSet;
  pHandoffSet->nEntries         = m_NumHandoffs;

  // set TCP PORT
  pHandoffEntry = &(pHandoffSet->Entry[0]);
  sprintf( pHandoffEntry->szIniFile,    "TCPIP.INI" );
  sprintf( pHandoffEntry->szIniSection, "TCP_HandoffSet" );
  sprintf( pHandoffEntry->szProtocol,   m_ProtocolName );
  pHandoffEntry->dwHandOffValue =        m_Port;
  pHandoffEntry->ValueFormatBase =       HANDOFF_VALUE_FORMAT_BASE_DECIMAL;    
  
  pParserInfo->pWhoCanFollowMe = NULL;

  return pParserDllInfo;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/