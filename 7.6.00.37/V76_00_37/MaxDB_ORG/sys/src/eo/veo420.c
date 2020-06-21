/*!
  @file           veo420.c
  @author         RaymondR
  @special area   remote communication
  @brief          
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
#define RTE_USE_SOCK

#include <stdlib.h>

#include "gsp00.h"
#include "geo003.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo60.h"
#include "heo46.h"
#include "heo47.h"
#include "heo420.h"
#include "geo40.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/RTE_Types.h"
#if defined(_WIN32)
#include "gos42.h" /* nocheck */ /* _WIN32 */
#endif /* _WIN32 */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__ "veo420 :"


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define MIN_EO420(a,b)            (((a) < (b)) ? (a) : (b))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef struct teo40_OldRteConnectPacketRec
  {
  tsp00_C2                              sMessCode;
  tsp00_Int2                            ConnectLength;
  tsp00_Uint1                           ServiceType;
  tsp00_Uint1                           OSType;
  tsp00_Uint1                           Filler1;
  tsp00_Uint1                           Filler2;
  tsp00_Int4                            MaxSegmentSize;
  tsp00_Int4                            MaxDataLen;
  tsp00_Int4                            PacketSize;
  tsp00_Int4                            MinReplySize;
  tsp00_C8                              ReceiverServerDB;
  tsp00_C8                              SenderServerDB;
  tsp00_C256                            VarPart;
  } teo40_OldRteConnectPacketRec;


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
static /* inline */ tsp00_Uint     
eo420ReceivePacket ( void                        *RecvHandle,
                     RECV_PKT_FUNC_PTR_EO003      RecvFunc,
                     tsp00_Uint                   ulSwapType,
                     teo003_RteHeaderRecord      *pRTEHeader,
                     char                        *pData,
                     tsp00_Uint                   ulMaxDataLen,
                     tsp00_ErrTextc VAR_ARRAY_REF pErrText ) ;

static /* inline */ tsp00_Uint    
eo420UnpackInt2        ( tsp00_Uint                   ulSwapTyp,
                         tsp00_Int2                   Src,
                         tsp00_Int2                  *pDst,
                         tsp00_ErrTextc VAR_ARRAY_REF pErrText ) ;

static /* inline */ tsp00_Uint    
eo420UnpackInt4        ( tsp00_Uint                   ulSwapTyp,
                         tsp00_Int4                   Src,
                         tsp00_Int4                  *pDst,
                         tsp00_ErrTextc VAR_ARRAY_REF pErrText );

static /* inline */ tsp00_Uint     
eo420_GetPortNo    ( teo003_RteConnectPacketRec *pConnectPacket,
                     tsp00_Uint2                *pusServicePort );

static /* inline */ tsp00_Uint     
eo420_GetRemotePid ( teo003_RteConnectPacketRec *pConnectPacket,
                     teo001_Pid                 *pPID );

static /* inline */ tsp00_Uint     
eo420_GetAcknowledge( teo003_RteConnectPacketRec *pConnectPacket,
                      tsp00_Bool                 *pfAcknowledge );

static /* inline */ tsp00_Uint     
eo420_GetOmitReplyPart (teo003_RteConnectPacketRec *pConnectPacket,
                        tsp00_Bool                 *pOmitReplyPart );

static /* inline */ tsp00_Uint     
eo420_GetStringFromVarPart ( teo003_RteConnectPacketRec *pConnectPacket,
                             char                        StringID,
                             char                       *szString,
                             tsp00_Uint                  MaxStringLen) ;

static /* inline */ tsp00_Int4  
eo420_PutRemotePid           ( char                       *VarPart,
                               teo001_Pid                  Pid );

static /* inline */ tsp00_Int4  
eo420_PutPortNo              ( char                       *VarPart,
                               tsp00_Uint2                 usServicePort );

static /* inline */ tsp00_Int4
eo420_PutAcknowledge         ( char                       *VarPart,
                               tsp00_Bool                 fAcknowledge );

static /* inline */ tsp00_Int4 
eo420_PutOmitReplyPart       ( char                       *VarPart,
                               tsp00_Bool                  omitReplyPart );

static /* inline */ tsp00_Int4 
eo420_PutStringToVarPart  ( char                       *VarPart,
                                 char                        StingId, 
                                 char                       *pszString );

static /* inline */ tsp00_Int4
eo420CreateRteConnectPacket ( teo003_ConPktParamRecord   *pConPktParamRec,
                              teo003_RteConnectPacketRec *pConnectPacket );

static /* inline */ void
eo420CreateRteConnectPacketHeader  ( teo003_ConPktParamRecord   *pConPktParamRec,
                                     teo003_RteHeaderRecord     *pRTEHeader );

static /* inline */ tsp00_Int4 
eo420CreateRteConnectPacketVarPart( teo003_ConPktParamRecord   *pConPktParamRec,
                                    char                       *VarPart);
static tsp00_Uint 
eo420ExtractRTEHeader        (tsp00_Int4                    ulSwapType,
                              teo003_ConPktParamRecord *    pConPktParamRec,
                              teo003_RteHeaderRecord *      pRTEHeader,
                              tsp00_ErrTextc                pErrText);

static /* inline */ tsp00_Uint    
eo420ExtractRteConnectPacket( teo003_ConPktParamRecord    *pConPktParamRec,
                              teo003_RteHeaderRecord      *pRTEHeader,
                              teo003_RteConnectPacketRec  *pConnectPacket,
                              tsp00_ErrTextc               pErrText );

static SAPDB_UInt1 
eo420OldSwapType ();


static tsp00_Uint1 LoadUint1  ( tsp00_Uint1 const *   pIn );
static tsp00_Uint2 LoadUint2  ( tsp00_Uint2 const *   pIn );
static tsp00_Uint4 LoadUint4  ( tsp00_Uint4 const *   pIn );
static tsp00_Uint8 LoadUint8  ( tsp00_Uint8 const *   pIn );

static void        StoreUint1 ( tsp00_Uint1 * const   pOut,
                                tsp00_Uint1 const     value );
static void        StoreUint2 ( tsp00_Uint2 * const   pOut,
                                tsp00_Uint2 const     value );
static void        StoreUint4 ( tsp00_Uint4 * const   pOut,
                                tsp00_Uint4 const     value );
static void        StoreUint8 ( tsp00_Uint8 * const   pOut,
                                tsp00_Uint8 const     value );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC tsp00_Uint eo420FillPingPacket ( char*       pData,
                                         tsp00_Uint  maxLength,
                                         tsp00_Uint1 hops,
                                         char*       peerName,
                                         char*       versionString,
                                         char*       fillPattern )
{
    teo003_PingRecord*  pPingRecord = (teo003_PingRecord*)pData;
    tsp00_Uint2         len;
    tsp00_Uint2         dataLen = 0;

    if ( sizeof(teo003_PingRecord) + 3 <= maxLength )
    {
      pData = (char*)pPingRecord + sizeof(teo003_PingRecord);

      pPingRecord->RecordVersion = 0;
      StoreUint1 ( &pPingRecord->Hops, hops);
      StoreUint2 ( &pPingRecord->DataOffset, sizeof(teo003_PingRecord) );

      maxLength -= sizeof(teo003_PingRecord) + 3; /* subtract record and end marker */

      if ( peerName )
      {
          len = 3 + strlen(peerName) + 1;
          if  ( len <= maxLength )
          {
              pData[0] = 's'; /* server name identifier */
              StoreUint2 ( (tsp00_Uint2*)(pData + 1), len - 3 );
              strcpy ( pData + 3,  peerName );
              pData                += len;
              maxLength            -= len;
              dataLen              += len;
          }
      }


      if ( versionString ) 
      {
          len = 3 + strlen(versionString) + 1;
          if ( len <= maxLength )
          {
              pData[0] = 'v'; /* verion string identifier */
              StoreUint2 ( (tsp00_Uint2*)(pData + 1), len - 3 );
              strcpy ( pData + 3, versionString );
              pData                += len;
              maxLength            -= len;
              dataLen              += len;
          }
      }

      if ( fillPattern )
      {
          len = 3 + strlen(fillPattern) + 1;

          while ( len <= maxLength )
          {
              pData[0] = '#'; /* fill pattern string identifier */
              StoreUint2 ( (tsp00_Uint2*)(pData + 1), len - 3 );
              strcpy ( pData + 3, fillPattern );
              pData                += len;
              maxLength            -= len;
              dataLen              += len;
          }
      }

      pData[0] = 0;  /* add end marker, maxlength correction already considered! */
      StoreUint2 ( (tsp00_Uint2*)(pData + 1), 0 );
      pData   += 3;
      dataLen += 3;

      if ( fillPattern )
      {
          /* fill up with blanks */
          memset ( pData, 0, maxLength );
          dataLen += maxLength;
      }

      /* now we know the data length, store it! */
      StoreUint4 ( &pPingRecord->DataLen, dataLen );

      return dataLen + sizeof(teo003_PingRecord);
    }

    return 0;   /* nothing inserted! */
}

/*------------------------------*/

externC tsp00_Bool eo420ExtractPingPacket ( char*         pData,
                                            tsp00_Uint    length,
                                            tsp00_Uint1*  hops,
                                            char**        peerName,
                                            char**        versionString )
{
    tsp00_Bool          ok          = true;
    teo003_PingRecord*  pPingRecord = (teo003_PingRecord*)pData;
    tsp00_Uint2         dataLen     = LoadUint4( &pPingRecord->DataLen );
    tsp00_Uint2         valueLength;

    if ( peerName )     *peerName      = 0;
    if ( versionString )*versionString = 0;
    if (hops)           *hops          = 0;

    if ( length >= sizeof(teo003_PingRecord) + dataLen )
    {
        if (hops) 
            *hops  = LoadUint1( &pPingRecord->Hops);

        pData  = (char*)pPingRecord + LoadUint2( &pPingRecord->DataOffset );
        length = dataLen;

        valueLength = LoadUint2( (tsp00_Uint2*)(pData + 1) );

        while ( length && valueLength )
        {
            switch (pData[0])
            {
            case 's' :
                if (peerName) *peerName = pData + 3;
                break;
            case 'v' :
                if (versionString) *versionString = pData + 3;
                break;
            }

            length      -= valueLength + 3;
            pData       += valueLength + 3;
            valueLength  = LoadUint2( (tsp00_Uint2*)(pData + 1) );
        }
    }
    else
        ok = false;

    return ok;
}

/*------------------------------*/

externC tsp00_Uint1  eo420NewSwapType ()
{ 
  static  tsp00_Uint1               SwapType = (tsp00_Uint1)UNDEF_SP00;

  if ( SwapType == (tsp00_Uint1)UNDEF_SP00 )
  {
    union { tsp00_Int8         int8;
            tsp00_C8           c8; }  SwapCheck;
    
    /* --- set int8 to 0x100000000 */
    SwapCheck.int8   = 0x01;
    SwapCheck.int8 <<= 32;
    
    for ( SwapType = 0;
          ( SwapType < 8 ) && ( SwapCheck.c8[ SwapType ] != 1 ); 
          SwapType++ ) { ; }
  }
  
  return SwapType;
}

/*------------------------------*/

#if !defined (KERNEL_LZU)

/*------------------------------*/

externC tsp00_Uint eo420ServiceTypeToMessClass ( tsp00_Uint     ulServiceType )
{
  #undef  MF__
  #define MF__ MOD__"eo420ServiceTypeToMessClass"

  tsp00_Uint     ulMessClass ;

  DBGPAS;

  switch ( ulServiceType )
    {
    case srvDistribution_esp01 :
      ulMessClass  = RSQL_KERN_CONN_REQUEST_EO003;
      break;

    case srvControl_esp01 :
      ulMessClass  = RSQL_CTRL_CONN_REQUEST_EO003;
      break;

    default:
      ulMessClass  = RSQL_USER_CONN_REQUEST_EO003;
      break;
    }

  return ( ulMessClass ) ;
}

/*------------------------------*/

externC void eo420InitConPktParamRec ( teo003_ConPktParamRecord *pConPktParamRec )
{
  #undef  MF__
  #define MF__ MOD__"eo420InitConPktParamRec"

  DBGPAS;

  pConPktParamRec->ulMessClass            = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulSenderRef            = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulReceiverRef          = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulServiceType          = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulMaxSegmentSize       = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulPacketSize           = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulMaxDataLen           = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulMinReplySize         = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->ulSwapType             = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->szSenderServerDB[0]    = '\0';
  pConPktParamRec->szReceiverServerDB[0]  = '\0';
  pConPktParamRec->pidSenderPID           = RTE_UNDEF_OSPID;;
  pConPktParamRec->usServicePort          = (tsp00_Uint2 )undef_ceo00;
  pConPktParamRec->ulCommState            = commErrOk_esp01;
  pConPktParamRec->fAcknowledge           = false;
  pConPktParamRec->fOmitReplyPart         = false;
  pConPktParamRec->szOptPeerNode[0]       = '\0';
  pConPktParamRec->szVersion[0]           = '\0';
  pConPktParamRec->szServerDBRoot[0]      = '\0';
  pConPktParamRec->szServerPgm[0]         = '\0';
  pConPktParamRec->authenticationAllow[0] = '\0';
  /* UNIX local connect only */
  pConPktParamRec->szOptFiFo[0]           = '\0';
  pConPktParamRec->ShmId                  = (tsp00_Int4 )undef_ceo00;
  pConPktParamRec->SemId                  = (tsp00_Int4 )undef_ceo00;
  pConPktParamRec->PacketCnt              = (tsp00_Int4 )undef_ceo00;
  pConPktParamRec->BigComsegOffset        = (tsp00_Uint     )undef_ceo00;
  pConPktParamRec->LocalApplPID           = (teo001_Pid )undef_ceo00;

}


/*------------------------------*/

externC void eo420UnpackRteHeader ( tsp00_Uint                    ulSwapType,
                                    teo003_RteHeaderRecord       *pRTEHeader,
                                    tsp00_ErrTextc  VAR_ARRAY_REF pErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo420UnpackRteHeader"

  DBGPAS;

  eo420UnpackInt4 ( ulSwapType, pRTEHeader->ActSendLen,
                   &pRTEHeader->ActSendLen, pErrText );

  eo420UnpackInt4 ( ulSwapType, pRTEHeader->MaxSendLen,
                   &pRTEHeader->MaxSendLen, pErrText );

  eo420UnpackInt2 ( ulSwapType, pRTEHeader->RTEReturnCode,
                   &pRTEHeader->RTEReturnCode, pErrText );

  eo420UnpackInt4 ( ulSwapType, pRTEHeader->ReceiverRef,
                   &pRTEHeader->ReceiverRef, pErrText );

  eo420UnpackInt4 ( ulSwapType, pRTEHeader->SenderRef,
                   &pRTEHeader->SenderRef, pErrText );
  return ;
}

/*------------------------------*/

/*
    SAP Unicode is realized as UTF16. So a single unicode character needs two or four bytes to be represented.
    But even if a character is has a four byte representation it can be seen as two shorts. Therefore we can
    swap a SAP Unicode string by swapping a short one after another.
*/

externC tsp00_Uint eo420UnpackSAPUCString (tsp00_Uint                    ulSwapTyp,
                                           SAPDB_Byte *                  pSourceString,
                                           SAPDB_Byte *                  pDestString,
                                           SAPDB_UInt4                   numBytes,
                                           tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    #undef  MF__
    #define MF__ MOD__"eo420UnpackSAPUCString"

    SAPDB_UInt4     idx;

    switch ( ulSwapTyp )
    {
    case 1:
        /* --- no swap: hi-lo */
        for (idx = 0; idx <= numBytes; idx += 2)
        {
            *((SAPDB_Int2 *)(pDestString + idx)) = ( pSourceString[idx] << 8 ) | ( pSourceString[idx + 1] << 0 );
        }
        break;
    case 2:
        /* --- full swap: lo-hi */
    case 3:
        /* --- half swap: lo-hi */
        for (idx = 0; idx <= numBytes; idx += 2)
        {

            *((SAPDB_Int2 *)(pDestString + idx)) = ( pSourceString[idx] << 0 ) | ( pSourceString[idx + 1] << 8 );
        }
        break;
    default:
        MSGD (( ERR_ILL_SWAP_TYPE, ulSwapTyp ));
        strcpy( pErrText, ERRMSG_ILL_SWAP_TYPE );
        *pDestString = 0;
        return ( commErrNotOk_esp01 );
    }

    return ( commErrOk_esp01 );
}

/*------------------------------*/

externC tsp00_Uint eo420GetDBRootFromConPkt( teo003_RteConnectPacketRec *pConnectPacket,
                                              char                       *szDBRoot )
{
  return eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_DBROOT_EO003,
                                      szDBRoot, sizeof ( tsp00_Path ) );
}

/*------------------------------*/

externC tsp00_Uint eo420GetServerPgmFromConPkt ( teo003_RteConnectPacketRec *pConnectPacket,
                                                 char                       *szServerPgm )
{
  return eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_SERVERPGM_EO003,
                               szServerPgm, sizeof ( tsp00_Path ) );
}

/*------------------------------*/

externC tsp00_Uint eo420SendConnectPacket ( void                         *SendHandle, 
                                            SEND_PKT_FUNC_PTR_EO003       SendFunc,
                                            teo003_ConPktParamRecord     *pConPktParamRec,
                                            tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo420SendConnectPacket"
  tsp00_Uint                      CommState;
  int                             Len;
  teo003_RteHeaderRecord         *pRTEHeader;
  teo003_RteConnectPacketRec     *pConnectPacket;
  char                            ucPacket[sizeof(teo003_RteHeaderRecord) +
                                           sizeof(teo003_RteConnectPacketRec)];
  DBGPAS;
  /*
   * - Structure of the remote connect packet:
   *
   *      +------------------------------------+
   *      |     rte_header_record              |
   *      +------------------------------------+
   *      |     rte_connect_packet_record      |
   *      +------------------------------------+
   *
   */
  pRTEHeader     = (teo003_RteHeaderRecord *)&ucPacket[0];
  pConnectPacket = (teo003_RteConnectPacketRec *) &ucPacket[sizeof(teo003_RteHeaderRecord)];

  eo420CreateRteConnectPacketHeader ( pConPktParamRec, pRTEHeader ) ;
  DBG3 (( MF__, "Len of Header = %d", pRTEHeader->ActSendLen ));

  Len = eo420CreateRteConnectPacket ( pConPktParamRec, pConnectPacket);
  DBG3 (( MF__, "Len of ConnectPacket excl. VarPart = %d", Len ));

  Len += eo420CreateRteConnectPacketVarPart( pConPktParamRec, (char *)pConnectPacket->VarPart ) ;
  DBG3 (( MF__, "Len of ConnectPacket incl. VarPart = %d", Len ));

  pConnectPacket->ConnectLength   = Len;
  pRTEHeader->ActSendLen         += Len;
  pRTEHeader->MaxSendLen         += Len;

  Len = pRTEHeader->ActSendLen;

  DBG3 (( MF__, "SendLen = %d", Len ));
  CommState = (* SendFunc) ( SendHandle, (char *)&ucPacket, Len, pErrText );

  return ( CommState );
}

/*------------------------------*/

externC tsp00_Uint eo420ReceiveConnectPacket( void                        *RecvHandle,
                                              RECV_PKT_FUNC_PTR_EO003      RecvFunc,
                                              teo003_ConPktParamRecord    *pConPktParamRec,
                                              tsp00_ErrTextc VAR_ARRAY_REF ErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo420ReceiveConnectPacket"
  tsp00_Uint                      BytesRec = 0;
  tsp00_Uint                      CommState = commErrOk_esp01;
  teo003_RteHeaderRecord         *pRTEHeader;
  teo003_RteConnectPacketRec     *pConnectPacket;
  char                            ucPacket[sizeof(teo003_RteHeaderRecord) +
                                           sizeof(teo003_RteConnectPacketRec)];

  DBGIN;
  /*
   * - Structure of the remote connect packet:
   *
   *      +------------------------------------+
   *      |     rte_header_record              |
   *      +------------------------------------+
   *      |     rte_connect_packet_record      |
   *      +------------------------------------+
   *
   */
  eo420InitConPktParamRec( pConPktParamRec) ;
  pRTEHeader     = (teo003_RteHeaderRecord *)&ucPacket[0];
  pConnectPacket = (teo003_RteConnectPacketRec *) &ucPacket[sizeof(teo003_RteHeaderRecord)];

  DBG3 (( MF__, "calling RecvFunc to receive rte_connect_packet" ));


  CommState = RecvFunc(RecvHandle, (char *) &ucPacket, sizeof(ucPacket), 
                       &BytesRec, ErrText);


  DBG3 (( MF__, "%d Bytes received", BytesRec ));

  if (CommState != commErrOk_esp01 || BytesRec == 0 )
    {
      MSGD (( ERR_CONN_SERV_REJECT )) ;
      strcpy(ErrText, ERRMSG_COM_CONN_SERV_REJECT );
      CommState = commErrNotOk_esp01 ;
    }
  else
  if ( BytesRec <  sizeof (ucPacket) - sizeof (pConnectPacket->VarPart))
    {
      MSGD (( ERR_CONN_PACKET_GARBLED, BytesRec )) ;
      strcpy( ErrText, ERRMSG_COM_CONN_PACKET_GARBLED );
      CommState = commErrNotOk_esp01 ;
    }
  else
    CommState = eo420ExtractRteConnectPacket ( pConPktParamRec, pRTEHeader,
                                               pConnectPacket, ErrText );

  if ( CommState == commErrOk_esp01 )
  {
    CommState = pConPktParamRec->ulCommState ;
    switch ( CommState )
    {
    case commErrTasklimit_esp01 :
       MSGD (( ERR_TO_MANY_SESSIONS )); 
       strcpy( ErrText, ERRMSG_COM_TO_MANY_DB_SESSIONS );
       break ;
  
    case commErrStartRequired_esp01:
       MSGD (( ERR_COM_SERVER_OR_DB_NOT_ACC )); 
       strcpy( ErrText, ERRMSG_COM_DATABASE_NOT_STARTED );
       break ;

    case commErrServerOrDBUnknown_esp01:
       MSGD (( ERR_COMM_SERVER_OR_DB_NOT_FOUND )); 
       strcpy( ErrText, ERRMSG_COM_SERVER_OR_DB_NOT_FOUND );
       break ;

    case commErrOk_esp01:
       break;

    default:
       MSGD (( ERR_CONN_REFUSED )) ;
       strcpy( ErrText, ERRMSG_COM_CONN_REFUSED );
    }

    if ( CommState == commErrOk_esp01 )
      if (( pRTEHeader->ActSendLen       != pRTEHeader->MaxSendLen ) ||
          ( pRTEHeader->ResidualPackets  != 0 ) ||
          ( pRTEHeader->ActSendLen       >  sizeof (ucPacket) ))
      {
        MSGD (( ERR_CONN_PACKET_GARBLED, BytesRec )) 
        strcpy( ErrText, ERRMSG_COM_PACKET_GARBLED );
        CommState = commErrNotOk_esp01 ;
      }
  }
  return ( CommState );
}

/*------------------------------*/

externC tsp00_Uint eo420SendCertificatePacket (void *                        pHdl, 
                                               SEND_PKT_FUNC_PTR_EO003       pSendFunc, 
                                               teo003_ConPktParamRecord *    pConPktParamRec,
                                               SAPDB_Byte *                  pServerCertificate,
                                               SAPDB_UInt4                   lenCertificate,
                                               tsp00_ErrTextc                pErrText)
{
  #undef  MF__
  #define MF__ MOD__"eo420SendCertificatePacket"
  tsp00_Uint                      commState;
  SAPDB_Char *                    pPacket;
  teo003_RteHeaderRecord *        pRTEHeader;
  RTESec_CertificateReplyPacket * pCertPacket;
  SAPDB_UInt4                     packetSize;

  DBGIN;
  /*
   * - Structure of the remote connect packet:
   *
   *      +------------------------------------+
   *      |     rte_header_record              |
   *      +------------------------------------+
   *      |     RTESec_CertificateReplyPacket  |
   *      |     (has dynamic length)           |
   *      +------------------------------------+
   *
   */

  packetSize = sizeof (*pRTEHeader) + sizeof (RTESec_CertificateReplyPacket) - 1 + lenCertificate;

  pPacket = (SAPDB_Char *) alloca (packetSize);  

  memset (pPacket, 0, packetSize);

  pRTEHeader     = (teo003_RteHeaderRecord *)pPacket;
  pCertPacket    = (RTESec_CertificateReplyPacket *)(pPacket + sizeof(*pRTEHeader));

  eo420CreateRteConnectPacketHeader ( pConPktParamRec, pRTEHeader ) ;
  DBG3 (( MF__, "Len of Header = %d", pRTEHeader->ActSendLen ));

  pCertPacket->swapType = eo420OldSwapType ();

  if (0 != lenCertificate)
  {
    memcpy ((SAPDB_Char *)(pCertPacket->pCertificate), (SAPDB_Char *)pServerCertificate, lenCertificate);
    DBG3 (( MF__, "Len of Certificate = %d", lenCertificate));
  }

  pRTEHeader->ActSendLen         += lenCertificate;
  pRTEHeader->MaxSendLen         += lenCertificate;

  DBG3 (( MF__, "SendLen = %d", pRTEHeader->ActSendLen ));
  commState = (* pSendFunc) ( pHdl, (char *)pPacket, pRTEHeader->ActSendLen, pErrText );

  return (commState);
}

/*------------------------------*/

externC tsp00_Uint eo420ReceiveCertificatePacket (void *                         pHdl,
                                                  RECV_PKT_FUNC_PTR_EO003        pRecvFunc,
                                                  teo003_ConPktParamRecord *     pConPktParamRec,
                                                  SAPDB_Byte *                   pServerCertificate,
                                                  SAPDB_UInt4                    maxLenCertificate,
                                                  tsp00_ErrTextc                 pErrText)
{
  #undef  MF__
  #define MF__ MOD__"eo420ReceiveCertificatePacket"
  tsp00_Uint                      bytesRec;
  tsp00_Uint                      ulCommState = commErrOk_esp01;
  teo003_RteHeaderRecord *        pRTEHeader;
  tsp00_Int                       bytesRemaining;
  RTESec_CertificateReplyPacket * pCertPacket;
  char                            ucPacket[sizeof(*pRTEHeader) +
                                           sizeof(*pCertPacket) - 1];


  DBGIN;
  /*
   * - Structure of the remote connect packet:
   *
   *      +------------------------------------+
   *      |     rte_header_record              |
   *      +------------------------------------+
   *      |     RTESec_CertificateReplyPacket  |
   *      |     (has dynamic length)           |
   *      +------------------------------------+
   *
   */

  pRTEHeader     = (teo003_RteHeaderRecord *)&ucPacket[0];
  pCertPacket    = (RTESec_CertificateReplyPacket *) &ucPacket[sizeof(*pRTEHeader)];

  ulCommState = pRecvFunc (pHdl, ucPacket + 0, sizeof(ucPacket), &bytesRec, pErrText);

  DBG3 (( MF__, "%d Bytes received", bytesRec ));

  if (bytesRec < sizeof(ucPacket))
  {
      MSGD (( ERR_CONN_SERV_REJECT )) ;
      strcpy (pErrText, ERRMSG_COM_CONN_SERV_REJECT);
      ulCommState = commErrNotOk_esp01 ;
  }
  else
  {
      ulCommState = eo420ExtractRTEHeader (pCertPacket->swapType, pConPktParamRec, pRTEHeader, pErrText);
  }

  if (commErrOk_esp01 == ulCommState)
  {
      ulCommState = pConPktParamRec->ulCommState ;
      switch ( ulCommState )
      {
      case commErrStartRequired_esp01:
          MSGD (( ERR_COM_SERVER_OR_DB_NOT_ACC ));
          strcpy( pErrText, ERRMSG_COM_DATABASE_NOT_STARTED );
          break ;

      case commErrServerOrDBUnknown_esp01:
          MSGD (( ERR_COMM_SERVER_OR_DB_NOT_FOUND ));
          strcpy( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_FOUND );
          break ;

      case commErrOk_esp01:
          break;

      default:
          MSGD (( ERR_CONN_REFUSED )) ;
          strcpy( pErrText, ERRMSG_COM_CONN_REFUSED );
      }


      if (commErrOk_esp01 == ulCommState)
      {
          SAPDB_Byte *  pData = pServerCertificate;

          if (maxLenCertificate < pRTEHeader->ActSendLen - sizeof(ucPacket))
          {
              MSGD (( ERR_COM_CERT_BUFFER_TOO_SMALL, maxLenCertificate, pRTEHeader->ActSendLen - sizeof(ucPacket))) ;
              strcpy (pErrText, ERRMSG_COM_CERT_BUFFER_TOO_SMALL);
              ulCommState = commErrNotOk_esp01 ;
          }

          for( bytesRec = 0, bytesRemaining = pRTEHeader->ActSendLen - sizeof(ucPacket);
              bytesRemaining > 0 && ulCommState == commErrOk_esp01 ;
              pData += bytesRec, bytesRemaining -= bytesRec )
          {
              ulCommState = pRecvFunc (pHdl, (char *) pData, bytesRemaining, &bytesRec, pErrText);

              DBG3 (( MF__, "%d bytes received, CommState = %d", bytesRec, ulCommState));
              if ( ulCommState == commErrOk_esp01  )
              {
                  if ( bytesRec == 0 )
                  {
                      MSGD (( ERR_CONN_BROKEN ));
                      strcpy( pErrText, ERRMSG_COM_CONN_BROKEN );
                      ulCommState = commErrNotOk_esp01 ;
                  }
              }
          }
      }

      if (commErrOk_esp01 == ulCommState)
      {
          ulCommState = eo420UnpackSAPUCString (pCertPacket->swapType, 
                                                pServerCertificate, 
                                                pServerCertificate, 
                                                pRTEHeader->ActSendLen - sizeof(ucPacket),
                                                pErrText);
          if (ulCommState != commErrOk_esp01)
          {
              MSGD (( ERR_COM_CERT_UNPACK_CERT ));
          }
      }
  }

  DBGOUT;
  return ( ulCommState );
}

/*------------------------------*/

externC tsp00_Uint eo420SendCommPacket  ( void                          *SendHandle,
                                          SEND_PKT_FUNC_PTR_EO003        SendFunc,
                                          tsp00_Uint                     ulMaxSegmentSize,
                                          teo003_CommPacketRecord       *pCommPacket,
                                          tsp00_Uint                     ulDataLen,
                                          tsp00_Uint                     ulSenderRef,
                                          tsp00_Uint                     ulReceiverRef,
                                          tsp00_Uint                     ulMessClass,
                                          tsp00_Uint                     ulCommState,
                                          tsp00_Uint1                    RTEFlags,
                                          tsp00_ErrTextc  VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420SendCommPacket"
  tsp00_Uint                 ulCurrCommState;
  tsp00_Int                  lDataRemaining;
  tsp00_Int                  ulMaxDataLen;
  char                      *pchSendBuff;
  teo003_RteHeaderRecord     RTEHeader;
  char                       chDataSave[RTE_HEADER_SIZE_EO003];

  DBGIN;

  if ( ulMaxSegmentSize <= RTE_HEADER_SIZE_EO003 )
    {
    MSGD (( ERR_ILL_MAXSEGMENTSIZE, ulMaxSegmentSize ))
    strcpy( pErrText, ERRMSG_COM_ILL_MAXSEGMENTSIZE );
    return ( commErrNotOk_esp01 );
    }

  /**/
  /* --- Update RTE header. */
  /**/
  pCommPacket->RTEHeader.ProtocolID      = RSQL_RTE_PROT_TCP_EO003;
  pCommPacket->RTEHeader.MessClass       = (teo00_Int1)ulMessClass;
  pCommPacket->RTEHeader.RTEFlags        = RTEFlags;
  pCommPacket->RTEHeader.ResidualPackets = 0;
  pCommPacket->RTEHeader.SenderRef       = (tsp00_Int4)ulSenderRef;
  pCommPacket->RTEHeader.ReceiverRef     = (tsp00_Int4)ulReceiverRef;
  pCommPacket->RTEHeader.RTEReturnCode   = (tsp00_Int2)ulCommState;
  pCommPacket->RTEHeader.NewSwapType     = eo420NewSwapType();
  pCommPacket->RTEHeader.Filler1         = 0;
  pCommPacket->RTEHeader.MaxSendLen      = (tsp00_Int4)ulDataLen + RTE_HEADER_SIZE_EO003;
  pCommPacket->RTEHeader.ResidualPackets = 0;
  pCommPacket->RTEHeader.ActSendLen      = pCommPacket->RTEHeader.MaxSendLen;

  if ( (tsp00_Uint    )pCommPacket->RTEHeader.MaxSendLen <= ulMaxSegmentSize )
    {
    DBG3 (( MF__, "calling SendFunc to send %d bytes", pCommPacket->RTEHeader.ActSendLen ));

    ulCurrCommState = (* SendFunc) ( SendHandle, (char *)pCommPacket,
                                     pCommPacket->RTEHeader.ActSendLen,
                                     pErrText );

    DBG3 (( MF__, "ulCurrCommState returned drom SendFunc %d", ulCurrCommState ));
    }
  else
    {
    /* --- save the header informations.*/
    RTEHeader                 = pCommPacket->RTEHeader;

    ulMaxDataLen              = ulMaxSegmentSize - RTE_HEADER_SIZE_EO003;
    RTEHeader.ResidualPackets = (teo00_Int1)((ulDataLen - 1) / ulMaxDataLen);
    pchSendBuff               = (char *)pCommPacket;


    for ( lDataRemaining  = ulDataLen;
          lDataRemaining;
          lDataRemaining -= ulDataLen )
      {
      ulDataLen            = MIN_EO420 ( ulMaxDataLen, lDataRemaining );
      RTEHeader.ActSendLen = (tsp00_Int4)ulDataLen + RTE_HEADER_SIZE_EO003;

      /* --- Save a part of data to replace it with the current header */
      /*     information. */
      SAPDB_memcpy ( chDataSave,  pchSendBuff, RTE_HEADER_SIZE_EO003 );
      SAPDB_memcpy ( pchSendBuff, &RTEHeader,  RTE_HEADER_SIZE_EO003 );

      DBG3 (( MF__, "calling SendFunc to send %d bytes", pCommPacket->RTEHeader.ActSendLen ));

      ulCurrCommState = (* SendFunc) ( SendHandle, pchSendBuff, 
                                       RTEHeader.ActSendLen, pErrText );

      DBG3 (( MF__, "ulCurrCommState returned drom SendFunc %d", ulCurrCommState ));

      /* --- restore saved data part.*/
      SAPDB_memcpy ( pchSendBuff, chDataSave, RTE_HEADER_SIZE_EO003 );

      if ( ulCurrCommState != commErrOk_esp01 )
        break;

      RTEHeader.ResidualPackets--;
      pchSendBuff += ulDataLen;
      }
    }

  DBGOUT;
  return ( ulCurrCommState ) ;
  }

/*------------------------------*/

externC tsp00_Uint eo420SendRteHeader ( void                          *SendHandle,
                                        SEND_PKT_FUNC_PTR_EO003        SendFunc,
                                        tsp00_Uint                     ulSenderRef,
                                        tsp00_Uint                     ulReceiverRef,
                                        tsp00_Uint                     ulMessClass,
                                        tsp00_Uint                     ulCommState,
                                        tsp00_Uint1                    RTEFlags,
                                        tsp00_ErrTextc  VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420SendRteHeader"
  tsp00_Uint                      ulCurrCommState;
  teo003_RteHeaderRecord          RTEHeader;

  DBGIN;

  /*  --- Create RTE header. */
  RTEHeader.ProtocolID      = RSQL_RTE_PROT_TCP_EO003;
  RTEHeader.MessClass       = (teo00_Int1)ulMessClass;
  RTEHeader.RTEFlags        = RTEFlags;
  RTEHeader.ResidualPackets = 0;
  RTEHeader.SenderRef       = (tsp00_Int4)ulSenderRef;
  RTEHeader.ReceiverRef     = (tsp00_Int4)ulReceiverRef;
  RTEHeader.RTEReturnCode   = (tsp00_Int2)ulCommState;
  RTEHeader.NewSwapType     = eo420NewSwapType();
  RTEHeader.Filler1         = 0;
  RTEHeader.MaxSendLen      = (tsp00_Int4)RTE_HEADER_SIZE_EO003;
  RTEHeader.ResidualPackets = 0;
  RTEHeader.ActSendLen      = RTEHeader.MaxSendLen;

  DBG3 (( MF__, "calling SendFunc to send %d bytes", RTEHeader.ActSendLen ));

  ulCurrCommState = (* SendFunc) ( SendHandle, (char *)&RTEHeader,
                                   RTEHeader.ActSendLen, pErrText );

  DBG3 (( MF__, "ulCurrCommState returned drom SendFunc %d", ulCurrCommState ));

  DBGOUT;
  return ( ulCurrCommState ) ;
  }

/*------------------------------*/

externC tsp00_Uint eo420SendReleasePacket ( void                      *SendHandle,
                                            SEND_PKT_FUNC_PTR_EO003    SendFunc,
                                            tsp00_Uint                 ulSenderRef,
                                            tsp00_Uint                 ulReceiverRef,
                                            tsp00_Uint                 ulMessClass,
                                            tsp00_Uint                 ulCommState)
  {
  #undef  MF__
  #define MF__ MOD__"eo420SendReleasePacket"
  tsp00_Uint                      ulCurrCommState;
  teo003_RteHeaderRecord          RTEHeader;
  tsp00_ErrTextc                  ErrText;

  DBGIN;

  RTEHeader.ProtocolID      = RSQL_RTE_PROT_TCP_EO003;
  RTEHeader.MessClass       = (teo00_Int1)ulMessClass;
  RTEHeader.RTEFlags        = RSQL_NORMAL_EO003;
  RTEHeader.ResidualPackets = 0;
  RTEHeader.SenderRef       = (tsp00_Int4)ulSenderRef;;
  RTEHeader.ReceiverRef     = ulReceiverRef;
  RTEHeader.RTEReturnCode   = (tsp00_Int2)ulCommState;
  RTEHeader.NewSwapType     = eo420NewSwapType();
  RTEHeader.Filler1         = 0;
  RTEHeader.MaxSendLen      = (tsp00_Int4)RTE_HEADER_SIZE_EO003;
  RTEHeader.ResidualPackets = 0;
  RTEHeader.ActSendLen      = RTEHeader.MaxSendLen;


  DBG3 (( MF__, "calling SendFunc to send %d bytes", RTEHeader.ActSendLen ));

  ulCurrCommState = (* SendFunc) ( SendHandle, (char *)&RTEHeader,
                                   RTEHeader.ActSendLen, ErrText );

  DBG3 (( MF__, "ulCurrCommState returned drom SendFunc %d", ulCurrCommState ));
  DBGOUT;
  return ( ulCurrCommState ) ;
  }

/*------------------------------*/

externC tsp00_Uint eo420ReceiveCommPacket ( void                         *RecvHandle,
                                            RECV_PKT_FUNC_PTR_EO003       RecvFunc,
                                            tsp00_Uint                    ulSwapType,
                                            teo003_CommPacketRecord      *pCommPacket,
                                            tsp00_Uint                    ulMaxDataLen,
                                            tsp00_Uint                   *pulDataLen,
                                            tsp00_Uint                   *pulSenderRef,
                                            tsp00_Uint                   *pulReceiverRef,
                                            tsp00_Uint                   *pulMessClass,
                                            tsp00_Uint1                  *pRTEFlags,
                                            tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420ReceiveCommPacket"
  tsp00_Uint                           ulCommState;
  char                                 *pDst;
  tsp00_Int                            lDataRemaining;
  tsp00_Int4                           MaxSendLen;

  DBGIN;

  ulCommState = eo420ReceivePacket( RecvHandle, RecvFunc, 
                                    ulSwapType, &pCommPacket->RTEHeader,
                                    pCommPacket->pDataPart, ulMaxDataLen, pErrText );

  DBG3 (( MF__,"ulMaxDataLen %d,  bytes, got %d,  ulCommState = %d", 
                ulMaxDataLen, pCommPacket->RTEHeader.MaxSendLen, ulCommState ));

  if ( ulCommState != commErrOk_esp01 )
    return ( ulCommState );

  MaxSendLen = (tsp00_Int4)ulMaxDataLen + RTE_HEADER_SIZE_EO003;

  if (( pCommPacket->RTEHeader.MaxSendLen < RTE_HEADER_SIZE_EO003  ) ||
      ( pCommPacket->RTEHeader.MaxSendLen > MaxSendLen ))
    {
    MSGD (( ERR_PACKET_GARBLED, pCommPacket->RTEHeader.MaxSendLen ))
    strcpy(pErrText, ERRMSG_COM_PACKET_GARBLED );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( pCommPacket->RTEHeader.ResidualPackets )
    {
    pDst           = pCommPacket->pDataPart +
                     pCommPacket->RTEHeader.ActSendLen - RTE_HEADER_SIZE_EO003;
    MaxSendLen     = pCommPacket->RTEHeader.MaxSendLen;
    lDataRemaining = MaxSendLen - pCommPacket->RTEHeader.ActSendLen;

    for( ; (pCommPacket->RTEHeader.ResidualPackets > 0) &&
           (lDataRemaining                         > 0); )
      {
      ulCommState = eo420ReceivePacket( RecvHandle, RecvFunc, 
                                        ulSwapType, &pCommPacket->RTEHeader,
                                        pDst, lDataRemaining, pErrText );

      DBG3 (( MF__,"lDataRemaining %d,  bytes, got %d,  ulCommState = %d", 
                    lDataRemaining, pCommPacket->RTEHeader.MaxSendLen, ulCommState ));

      if ( ulCommState != commErrOk_esp01 )
        {
        DBGOUT;
        return ( commErrNotOk_esp01 );
        }


      /* --- New 'MaxSendLen' received? */
      if ( pCommPacket->RTEHeader.MaxSendLen != MaxSendLen )
        {
        MSGD (( ERR_REC_NEW_MAXSENDLEN ))
        strcpy( pErrText, ERRMSG_COM_PACKET_GARBLED );

        DBGOUT;
        return ( commErrNotOk_esp01 );
        }

      pDst           += pCommPacket->RTEHeader.ActSendLen - RTE_HEADER_SIZE_EO003;
      lDataRemaining -= pCommPacket->RTEHeader.ActSendLen - RTE_HEADER_SIZE_EO003;
      }

    if (( lDataRemaining >  0 ) ||
        ( pCommPacket->RTEHeader.ResidualPackets >  0 ))
      {
      MSGD (( ERR_REC_MISSING_BYTES, lDataRemaining, pCommPacket->RTEHeader.ResidualPackets ));
      strcpy(pErrText, ERRMSG_COM_PACKET_GARBLED );

      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    pCommPacket->RTEHeader.ActSendLen = pCommPacket->RTEHeader.MaxSendLen;
    }

  *pulDataLen     = pCommPacket->RTEHeader.MaxSendLen - RTE_HEADER_SIZE_EO003;
  *pulSenderRef   = pCommPacket->RTEHeader.SenderRef;
  *pulReceiverRef = pCommPacket->RTEHeader.ReceiverRef;
  *pulMessClass   = (tsp00_Uint    )pCommPacket->RTEHeader.MessClass;
  *pRTEFlags      = pCommPacket->RTEHeader.RTEFlags;

  if ((  *pulDataLen == 0 ) &&
      (( *pulMessClass == RSQL_USER_RELEASE_REQUEST_EO003 ) ||
       ( *pulMessClass == RSQL_KERN_RELEASE_REQUEST_EO003  )))
    {
    strcpy( pErrText, ERRMSG_COM_CONN_CLOSED_BY_COM_PART );

    DBGOUT;
    return ( commErrReleased_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }


/*------------------------------*/

externC tsp00_Uint eo420SetSocketBufferSize ( teo00_Socket                  SocketHandle,
                                              tsp00_Uint                    ulMinSendLen,
                                              tsp00_Uint                   *ulMaxSendLen,
                                              tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  tsp00_Uint              ulCommState = commErrOk_esp01;

  DBG1 ((FUNCTION_NAME_MEO60,"try to set max socket buffer to: %d", *ulMaxSendLen ));
/*********************/
#ifdef WIN32
/*********************/
  ulCommState = os42SocketSetBuf (SocketHandle, ulMinSendLen, 
                                  ulMaxSendLen, pErrText );
/*********************/
#else
/*********************/
  {
  extern int sql42_socket_buffer( int Sd, long *ulMaxSendLen );
  ulCommState = sql42_socket_buffer ( SocketHandle, (long *) ulMaxSendLen );
  }
/*********************/
#endif
/*********************/

  DBG1 ((FUNCTION_NAME_MEO60,"socket buffer set to : %d",  *ulMaxSendLen)) ;

  return ( ulCommState );
}

/*------------------------------*/

externC tsp00_Uint eo420SetNiSocketBufferSize ( void                         *NiHandle,
                                                tsp00_Uint                    ulMinSendLen,
                                                tsp00_Uint                   *ulMaxSendLen,
                                                tsp00_ErrTextc VAR_ARRAY_REF  ErrText )
{
  int SocketHandle ;
  tsp00_Uint      ulCommState = eo40NiHandleToSocket ( NiHandle , &SocketHandle, ErrText );

  DBG1 ((FUNCTION_NAME_MEO60,"eo40NiHandleToSocket: Sd = %d, ulCommState = %d", 
                              SocketHandle, ulCommState ));

  if ( ulCommState == commErrOk_esp01 )
  {
    eo420SetSocketBufferSize ( SocketHandle, ulMinSendLen, ulMaxSendLen, ErrText ) ;
  }

  return ( ulCommState );
}

#endif

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static tsp00_Uint1 LoadUint1 ( tsp00_Uint1 const * pIn )
{
    return pIn[0];  /* This function does not make much sense,
                       but it helps to keep in mind the value 
                       swapping if the value type changes */
}

/*---------------------------------------------------------------------------*/

static void StoreUint1 ( tsp00_Uint1 * const   pOut,
                         tsp00_Uint1 const     value )
{
    pOut[0] = value; /* This function does not make much sense,
                        but it helps to keep in mind the value 
                        swapping if the value type changes */
}

/*---------------------------------------------------------------------------*/

static tsp00_Uint2 LoadUint2 ( tsp00_Uint2 const * pIn )
{
    return ( (tsp00_Uint2)(((char*)pIn)[ 0 ]) << 8 ) | 
           ( (tsp00_Uint2)(((char*)pIn)[ 1 ]) );
}

/*---------------------------------------------------------------------------*/

static void StoreUint2 ( tsp00_Uint2 * const   pOut,
                         tsp00_Uint2 const     value )
{
    ((unsigned char*)pOut)[0] = (unsigned char)(value >> 8);
    ((unsigned char*)pOut)[1] = (unsigned char) value;
}

/*---------------------------------------------------------------------------*/

static tsp00_Uint4 LoadUint4 ( tsp00_Uint4 const * pIn )
{
    return ( (tsp00_Int4)(((unsigned char*)pIn)[ 0 ]) << 24 ) |
           ( (tsp00_Int4)(((unsigned char*)pIn)[ 1 ]) << 16 ) |
           ( (tsp00_Int4)(((unsigned char*)pIn)[ 2 ]) <<  8 ) |
           ( (tsp00_Int4) ((unsigned char*)pIn)[ 3 ]);
}

/*---------------------------------------------------------------------------*/

static void StoreUint4 ( tsp00_Uint4 * const   pOut,
                         tsp00_Uint4 const     value )
{
    ((unsigned char*)pOut)[0] = (unsigned char)(value >> 24);
    ((unsigned char*)pOut)[1] = (unsigned char)(value >> 16);
    ((unsigned char*)pOut)[2] = (unsigned char)(value >>  8);
    ((unsigned char*)pOut)[3] = (unsigned char) value;
}

/*---------------------------------------------------------------------------*/

static tsp00_Uint8 LoadUint8 ( tsp00_Uint8 const * pIn )
{
    return ( (tsp00_Int8)((unsigned char*)pIn[ 0 ]) << 56 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 1 ]) << 48 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 2 ]) << 40 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 3 ]) << 32 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 4 ]) << 24 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 5 ]) << 16 ) |
           ( (tsp00_Int8)((unsigned char*)pIn[ 6 ]) <<  8 ) |
           ( (tsp00_Int8) (unsigned char*)pIn[ 7 ]);
}

/*---------------------------------------------------------------------------*/

static void StoreUint8 ( tsp00_Uint8 * const      pOut,
                         tsp00_Uint8 const        value )
{
    ((unsigned char*)pOut)[0] = (unsigned char)(value >> 56);
    ((unsigned char*)pOut)[1] = (unsigned char)(value >> 48);
    ((unsigned char*)pOut)[2] = (unsigned char)(value >> 40);
    ((unsigned char*)pOut)[3] = (unsigned char)(value >> 32);
    ((unsigned char*)pOut)[4] = (unsigned char)(value >> 24);
    ((unsigned char*)pOut)[5] = (unsigned char)(value >> 16);
    ((unsigned char*)pOut)[6] = (unsigned char)(value >>  8);
    ((unsigned char*)pOut)[7] = (unsigned char) value;
}

/*---------------------------------------------------------------------------*/

#if !defined (KERNEL_LZU)

static tsp00_Uint eo420ReceivePacket ( void                        *RecvHandle,
                                       RECV_PKT_FUNC_PTR_EO003      RecvFunc,
                                       tsp00_Uint                   ulSwapType,
                                       teo003_RteHeaderRecord      *pRTEHeader,
                                       char                        *pData,
                                       tsp00_Uint                   ulMaxDataLen,
                                       tsp00_ErrTextc VAR_ARRAY_REF pErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo420ReceivePacket"
 
  tsp00_Uint              CommState = commErrOk_esp01;
  tsp00_Uint              BytesRec;
  tsp00_Uint              BytesRemaining;
  char                   *pRecvPos;

  /* --- Receive the RTE Header first!   */

  for ( BytesRec       = 0,
        pRecvPos       = (char *)pRTEHeader,
        BytesRemaining = RTE_HEADER_SIZE_EO003;
        BytesRemaining > 0 && CommState == commErrOk_esp01 ;
        pRecvPos += BytesRec, BytesRemaining -= BytesRec )
  {
    DBG3 (( MF__, "calling RecvFunc to receive rte_header" ));

    CommState = RecvFunc(RecvHandle, pRecvPos, BytesRemaining, &BytesRec, pErrText);

    DBG3 (( MF__, "%d bytes received, CommState = %d", BytesRec, CommState));
    if ( CommState == commErrOk_esp01  )
    {
      if ( BytesRec == 0 )
      {
        CommState = commErrNotOk_esp01 ;
        if ( BytesRemaining != RTE_HEADER_SIZE_EO003 )
        {
          /* --- We have already received some data. */
          MSGD (( ERR_CONN_BROKEN ));
          strcpy( pErrText, ERRMSG_COM_CONN_BROKEN );
        }
      else
        strcpy( pErrText, ERRMSG_COM_CONN_CLOSED_BY_COM_PART );
      }
    }
  }


  if ( CommState == commErrOk_esp01 )
  {
    /* --- After the rte header is complete,
     *     we know exactly which size to receive.
     */

    eo420UnpackRteHeader (ulSwapType, pRTEHeader, pErrText );

    DBG3((MF__, "total data to receive %d ", pRTEHeader->ActSendLen ));
 
    if ( (tsp00_Uint    )pRTEHeader->ActSendLen > ulMaxDataLen + RTE_HEADER_SIZE_EO003 )
    {
      MSGD (( ERR_ILL_PACKET_SIZE, pRTEHeader->ActSendLen )); 
      strcpy( pErrText, ERRMSG_COM_PACKET_GARBLED );
      CommState = commErrNotOk_esp01 ;
    }
    else
    {
      for( BytesRec = 0, BytesRemaining = pRTEHeader->ActSendLen - RTE_HEADER_SIZE_EO003;
           BytesRemaining > 0 && CommState == commErrOk_esp01 ;
           pData += BytesRec, BytesRemaining -= BytesRec )
      {

        CommState = RecvFunc(RecvHandle, pData, BytesRemaining, &BytesRec, pErrText);

        DBG3 (( MF__, "%d bytes received, CommState = %d", BytesRec, CommState));
        if ( CommState == commErrOk_esp01  )
        {
          if ( BytesRec == 0 )
          {
            MSGD (( ERR_CONN_BROKEN ));
            strcpy( pErrText, ERRMSG_COM_CONN_BROKEN );
            CommState = commErrNotOk_esp01 ;
          }
        }
      }

      if (( CommState == commErrOk_esp01 ) && ( pRTEHeader->RTEReturnCode ))
      {
        char*  pszErrText = NULL;
        
        switch ( pRTEHeader->RTEReturnCode )
        {
        case commErrNotOk_esp01:
          pszErrText = ERRMSG_COM_NOT_OK;
          break;
        case commErrTasklimit_esp01:
          pszErrText = ERRMSG_COM_TO_MANY_DB_SESSIONS;
          break;
        case commErrTimeout_esp01:
          pszErrText = ERRMSG_COM_TIMEOUT;
          break;
        case commErrCrash_esp01:
          pszErrText = ERRMSG_COM_CRASH;
          break;
        case commErrStartRequired_esp01:
          pszErrText = ERRMSG_COM_DATABASE_NOT_STARTED;
          break;
        case commErrPacketLimit_esp01:
          pszErrText = ERRMSG_COM_PACKET_LIMIT;
          break;
        case commErrReleased_esp01:
          pszErrText = ERRMSG_COM_RELEASED;
          break;
        case commErrUnknownRequest_esp01:
          pszErrText = ERRMSG_COM_UNKNOWN_REQUEST;
          break;
        case commErrServerOrDBUnknown_esp01:
          pszErrText = ERRMSG_COM_SERVER_OR_DB_NOT_FOUND;
          break;
        case commErrShutdown_esp01:
          pszErrText = ERRMSG_COM_SHUTDOWN;
          break;
        default :
          strcpy( pErrText, ERRMSG_COM_BAD_RTE_RETURN_CODE );
          break ;
        }
        if ( pszErrText != NULL )
          strcpy( pErrText, pszErrText );
        
        CommState = pRTEHeader->RTEReturnCode ;
      }
    }
  }
  return ( CommState );
}

/*------------------------------*/

/*
 *  Here, we have some functions which use the var_part of the connect
 *  info packet. The var_part can contain several information and is
 *  structured as follows:
 *
 *  var_part: [argument][argument]...
 *
 *  Each argument is structured as follows:
 *
 *  argument: |length|id|information|
 *
 *            'length'      is a one byte integer.
 *                          It includes the length and id-bytes.
 *            'id'          is a one byte character
 *            'information' is coded argument dependent
 *
 *  Example:
 *
 *      var_part: 04 50 1b 58 06 49 31 32 33 00 (hexadecimal)
 *                ^  ^  ^     ^  ^  ^
 *                |  |  |     |  |  |
 *                |  |  |     |  |  NUL terminated string ("123")
 *                |  |  |     |  Argument id 'I' for remote pid
 *                |  |  |     Argument length (6 bytes: length,id,string,\0)
 *                |  |  |
 *                |  |  TCP/IP port number (0x1b58)
 *                |  Argument id 'P' for TCP/IP port number
 *                Argument length (4 bytes: length,id,port number)
 *
 *  There is no terminator for the VarPart since the length of the
 *  connect packet includes the VarPart and thus specifies its length.
 */

static tsp00_Uint eo420_GetPortNo ( teo003_RteConnectPacketRec  *pConnectPacket,
                                    tsp00_Uint2                 *pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_GetPortNo"
  tsp00_Uint               ulLength;
  tsp00_Uint               ulPos;

  DBGIN;

  ulLength = pConnectPacket->ConnectLength  -
             ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );

  ulLength = MIN_EO420 ( ulLength, sizeof (pConnectPacket->VarPart) );

  for ( ulPos =  0;
        ulPos <  ulLength;
        ulPos += pConnectPacket->VarPart[ulPos] & 0xff )
    {
    if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) < 2 )
      {
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    if ( pConnectPacket->VarPart[ulPos + 1] == RSQL_OPT_PORT_NO_EO003 )
      {
      if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) != 4 )
        {
        MSGD (( WRN_ILL_ARG_LENGTH, pConnectPacket->VarPart[ulPos] & 0xff ));
        DBGOUT;
        return ( commErrNotOk_esp01 );
        }
      break;
      }
    }

  if ( ulPos >= ulLength )
    {
    /* --- no port number found! */
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *pusServicePort  = (tsp00_Uint2)(pConnectPacket->VarPart[ulPos + 2] & 0xff) << 8;
  *pusServicePort |= (tsp00_Uint2)(pConnectPacket->VarPart[ulPos + 3] & 0xff);

  DBG3 (( MF__, "ServicePort = %d", *pusServicePort));
  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420_GetRemotePid ( teo003_RteConnectPacketRec *pConnectPacket,
                                       teo001_Pid                 *pPID )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_GetRemotePid"
  tsp00_Uint               ulLength;
  tsp00_Uint               ulPos;
  tsp00_Uint               ulStrLen;

  DBGIN;

  ulLength = pConnectPacket->ConnectLength  -
             ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );

  ulLength = MIN_EO420 ( ulLength, sizeof (pConnectPacket->VarPart) );

  for ( ulPos =  0;
        ulPos <  ulLength;
        ulPos += pConnectPacket->VarPart[ulPos] & 0xff )
    {
    if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) < 2 )
      {
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    if ( pConnectPacket->VarPart[ulPos + 1] == RSQL_OPT_REM_PID_EO003 )
      {
      ulStrLen = pConnectPacket->VarPart[ulPos] & 0xff;

      if ( ( ulStrLen < 4 ) ||
           ( pConnectPacket->VarPart[ulPos + ulStrLen - 1 ] != '\0' ))
        {
        MSGD (( WRN_ILL_ARG_LENGTH, pConnectPacket->VarPart[ulPos] & 0xff ));
        DBGOUT;
        return ( commErrNotOk_esp01 );
        }
      break;
      }
    }

  if ( ulPos >= ulLength )
    {
    /* --- no pid found! */
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *pPID = (teo001_Pid)atol( (const char *)(pConnectPacket->VarPart + ulPos + 2) );
  DBG3 (( MF__, "remote pid = %d", *pPID));

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420_GetAcknowledge( teo003_RteConnectPacketRec *pConnectPacket,
                                        tsp00_Bool                 *pfAcknowledge )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_GetAcknowledge"
  tsp00_Uint               ulLength;
  tsp00_Uint               ulPos;

  DBGIN;

  *pfAcknowledge = true;

  ulLength = pConnectPacket->ConnectLength  -
             ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );

  ulLength = MIN_EO420 ( ulLength, sizeof (pConnectPacket->VarPart) );

  for ( ulPos =  0;
        ulPos <  ulLength;
        ulPos += pConnectPacket->VarPart[ulPos] & 0xff )
    {
    if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) < 2 )
      {
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    if ( pConnectPacket->VarPart[ulPos + 1] == RSQL_OPT_ACKNOWLEDGE_EO003 )
      {
      if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) != 3 )
        {
        MSGD (( WRN_ILL_ARG_LENGTH, pConnectPacket->VarPart[ulPos] & 0xff ));
        DBGOUT;
        return ( commErrNotOk_esp01 );
        }
      break;
      }
    }

  if ( ulPos >= ulLength )
    {
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *pfAcknowledge = ((pConnectPacket->VarPart[ulPos + 2]) == 0);

  DBG3 (( MF__, "acknowledge = %d", *pfAcknowledge));
  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420_GetOmitReplyPart(teo003_RteConnectPacketRec *pConnectPacket,
                                         tsp00_Bool                 *pOmitReplyPart )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_GetOmitReplyPart"
  tsp00_Uint               ulLength;
  tsp00_Uint               ulPos;

  DBGIN;

  *pOmitReplyPart = false;

  ulLength = pConnectPacket->ConnectLength  -
             ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );

  ulLength = MIN_EO420 ( ulLength, sizeof (pConnectPacket->VarPart) );

  for ( ulPos =  0;
        ulPos <  ulLength;
        ulPos += pConnectPacket->VarPart[ulPos] & 0xff )
    {
    if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) < 2 )
      {
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    if ( pConnectPacket->VarPart[ulPos + 1] == RSQL_OPT_OMIT_REPLY_PART_EO003 )
      {
      if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) != 3 )
        {
        MSGD (( WRN_ILL_ARG_LENGTH, pConnectPacket->VarPart[ulPos] & 0xff ));
        DBGOUT;
        return ( commErrNotOk_esp01 );
        }
      break;
      }
    }

  if ( ulPos >= ulLength )
    {
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *pOmitReplyPart = ((pConnectPacket->VarPart[ulPos + 2]) == 1);

  DBG3 (( MF__, "omit reply part = %d", *pOmitReplyPart));
  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420_GetStringFromVarPart ( teo003_RteConnectPacketRec *pConnectPacket,
                                               char                        StringID,
                                               char                       *szString,
                                               tsp00_Uint                  MaxStringLen )
{
#undef  MF__
#define MF__ MOD__"eo420_GetStringFromVarPart"
    tsp00_Uint               ulLength;
    tsp00_Uint               ulPos;
    tsp00_Uint               ulArglen = 0;

    DBGIN;

    *szString = '\0';

    ulLength = pConnectPacket->ConnectLength  -
               ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );

    ulLength = MIN_EO420 ( ulLength, sizeof (pConnectPacket->VarPart) );

    for ( ulPos =  0;
          ulPos <  ulLength;
          ulPos += pConnectPacket->VarPart[ulPos] & 0xff )
    {
        if ( ( pConnectPacket->VarPart[ulPos] & 0xff ) < 2 )
        {
            DBGOUT;
            return ( commErrNotOk_esp01 );
        }

        if ( pConnectPacket->VarPart[ulPos + 1] == StringID )
        {
            ulArglen = pConnectPacket->VarPart[ulPos] & 0xff;
            if ( ( ulArglen > MaxStringLen)
            /* PTS 1124003 length check incomplete */
              || ( ulArglen < 3 ) /* strlen() + 3 (len + tag + zerobyte) */
              || ( (ulPos + ulArglen) > ulLength ) )
            {
                MSGD (( WRN_ILL_STRING_ARG, ulArglen, StringID ));
                DBGOUT;
                return ( commErrNotOk_esp01 );
            }

            /* PTS 1124003 buffer overflow attack possible */
            if ( pConnectPacket->VarPart[ulPos + ulArglen - 1] )
            {
                MSGD (( WRN_LONG_STRING_ARG, ulArglen, StringID ));
                DBGOUT;
                return ( commErrNotOk_esp01 );
            }
            if ( strlen((char *)pConnectPacket->VarPart + ulPos + 2) != (ulArglen - 3) )
            {
                MSGD (( WRN_SHORT_STRING_ARG, ulArglen, StringID ));
                DBGOUT;
                return ( commErrNotOk_esp01 );
            }
            break;
        }
    }

    if ( ulPos >= ulLength )
    {
        DBGOUT;
        return ( commErrNotOk_esp01 );
    }

    /* PTS 1124003 buffer overflow attack possible */
    SAPDB_memcpy( szString, pConnectPacket->VarPart + ulPos + 2, ulArglen - 2);

    DBG3 (( MF__, "szString = %s", (char *) szString));
    DBGOUT;
    return ( commErrOk_esp01 );
}


/*------------------------------*/

static tsp00_Int4 eo420_PutPortNo ( char                       *VarPart,
                                    tsp00_Uint2                 usServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_PutPortNo"
  tsp00_Int4               Length=4;

  DBGPAS;

  DBG3 (( MF__, "ServicePort = %d", usServicePort));

  VarPart[0] = (unsigned char)4;
  VarPart[1] = RSQL_OPT_PORT_NO_EO003;
  VarPart[2] = ( usServicePort >> 8 & 0xFF ); /* - high byte */
  VarPart[3] = ( usServicePort      & 0xFF ); /* - low  byte */

  return(Length);
  }

/*------------------------------*/

static tsp00_Int4 eo420_PutAcknowledge ( char                       *VarPart,
                                         tsp00_Bool                  fAcknowledge )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_PutAcknowledge"
  tsp00_Int4               Length=3;

  DBGPAS;

  DBG3 (( MF__, "acknowledge = %d", fAcknowledge));

  VarPart[0]     = (unsigned char)Length;
  VarPart[1]     = RSQL_OPT_ACKNOWLEDGE_EO003;
  VarPart[2]     = (tsp00_Uint1) (fAcknowledge == false);

  return(Length);
  }

/*------------------------------*/

static tsp00_Int4 eo420_PutOmitReplyPart ( char                       *VarPart,
                                           tsp00_Bool                  omitReplyPart )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_PutOmitReplyPart"
  tsp00_Int4               Length=3;

  DBGPAS;

  DBG3 (( MF__, "omit reply part = %d", omitReplyPart));

  VarPart[0]     = (unsigned char)Length;
  VarPart[1]     = RSQL_OPT_OMIT_REPLY_PART_EO003;
  VarPart[2]     = (tsp00_Uint1) (omitReplyPart);

  return(Length);
  }

/*------------------------------*/

static tsp00_Int4 eo420_PutRemotePid ( char                       *VarPart,
                                       teo001_Pid                  pid )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_PutRemotePid"
  tsp00_Int4               Length;

  DBGPAS;

  DBG3 (( MF__, "pid = %d", pid));

  sql47_ltoa((long )pid, VarPart + 2, 10);
  Length = (tsp00_Int4)strlen ( VarPart + 2 ) + 3;

  VarPart[0]  = (unsigned char)Length;
  VarPart[1]  = RSQL_OPT_REM_PID_EO003;

  return(Length);
  }

/*------------------------------*/

static tsp00_Int4 eo420_PutStringToVarPart ( char                       *VarPart,
                                                  char                        StingId, 
                                                  char                       *pszString )
  {
  #undef  MF__
  #define MF__ MOD__"eo420_PutStringToVarPart"
  tsp00_Int4     Length = 0 ;

  DBGPAS;

  if ((pszString != NULL) && (pszString [0] != '\0'))
  {
    Length = (tsp00_Int4)strlen ( pszString ) + 3; /* len, tag, NUL */
    DBG3 (( MF__, "string = %s, total Length %d", (char *)pszString, Length));

    VarPart[0]  = (unsigned char) Length;
    VarPart[1]  = StingId;
    strcpy ( VarPart + 2, pszString);
  }

  return(Length);
  }

/*------------------------------*/

static void eo420CreateRteConnectPacketHeader  ( teo003_ConPktParamRecord   *pConPktParamRec,
                                     teo003_RteHeaderRecord     *pRTEHeader )
{
  #undef  MF__
  #define MF__ MOD__"eo420CreateRteConnectPacketHeader"

  DBGPAS;

  pRTEHeader->ProtocolID       = RSQL_RTE_PROT_TCP_EO003;
  pRTEHeader->RTEFlags         = RSQL_NORMAL_EO003;
  pRTEHeader->MessClass        = (teo00_Int1)pConPktParamRec->ulMessClass;
  pRTEHeader->ResidualPackets  = 0;
  pRTEHeader->SenderRef        = (tsp00_Int4)pConPktParamRec->ulSenderRef;
  pRTEHeader->ReceiverRef      = (tsp00_Int4)pConPktParamRec->ulReceiverRef;
  pRTEHeader->RTEReturnCode    = (tsp00_Int2)pConPktParamRec->ulCommState;
  pRTEHeader->NewSwapType      = eo420NewSwapType();
  pRTEHeader->Filler1          = 0;
  pRTEHeader->ActSendLen       = sizeof (*pRTEHeader) ;
  pRTEHeader->MaxSendLen       = pRTEHeader->ActSendLen ;
}

/*------------------------------*/

static tsp00_Int4 eo420CreateRteConnectPacketVarPart( teo003_ConPktParamRecord   *pConPktParamRec,
                                                      char                       *VarPart)
{
  #undef  MF__
  #define MF__ MOD__"eo420CreateRteConnectPacketVarPart"

  tsp00_Int4 ActVarPartPos = 0 ;

  ActVarPartPos  = eo420_PutRemotePid   ( &VarPart[ActVarPartPos], 
                                          pConPktParamRec->pidSenderPID );

  ActVarPartPos += eo420_PutPortNo      ( &VarPart[ActVarPartPos], 
                                          pConPktParamRec->usServicePort );

  ActVarPartPos += eo420_PutAcknowledge ( &VarPart[ActVarPartPos], 
                                          pConPktParamRec->fAcknowledge );

  if (pConPktParamRec->fOmitReplyPart)
  {
    ActVarPartPos += eo420_PutOmitReplyPart (&VarPart[ActVarPartPos], 
                                             pConPktParamRec->fOmitReplyPart);
  }

  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos],
                                              RSQL_OPT_DBROOT_EO003, pConPktParamRec->szServerDBRoot );

  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos], 
                                              RSQL_OPT_SERVERPGM_EO003, pConPktParamRec->szServerPgm );

  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos], 
                                              RSQL_OPT_NODE_EO003, pConPktParamRec->szOptPeerNode );

  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos], 
                                              RSQL_OPT_FIFO_EO003, pConPktParamRec->szOptFiFo );

  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos], 
                                              RSQL_OPT_AUTH_ALLOW_EO003, pConPktParamRec->authenticationAllow );
  /* PTS 1105295 */
  ActVarPartPos += eo420_PutStringToVarPart ( &VarPart[ActVarPartPos], 
                                              RSQL_OPT_VERSION_EO003, pConPktParamRec->szVersion );

  return(ActVarPartPos);
}

/*------------------------------*/

static tsp00_Int4 eo420CreateRteConnectPacket( teo003_ConPktParamRecord   *pConPktParamRec,
                                               teo003_RteConnectPacketRec *pConnectPacket)
  {
  #undef  MF__
  #define MF__ MOD__"eo420CreateRteConnectPacket"
  tsp00_Int4                           i;

  DBGIN;

  i = ' ';
  if ( i == 32 )
    pConnectPacket->sMessCode[ 0 ] = 0;
  else
    pConnectPacket->sMessCode[ 0 ] = 1;

  pConnectPacket->sMessCode[ 1 ] = eo420OldSwapType ();
  pConnectPacket->ConnectLength  = 0;
  pConnectPacket->ServiceType    = (teo00_Int1)pConPktParamRec->ulServiceType;
  pConnectPacket->OSType         = RSQL_OS_TYPE_EO003;
  pConnectPacket->MaxSegmentSize = (tsp00_Int4)pConPktParamRec->ulMaxSegmentSize;
  pConnectPacket->PacketSize     = (tsp00_Int4)pConPktParamRec->ulPacketSize;
  pConnectPacket->MaxDataLen     = (tsp00_Int4)pConPktParamRec->ulMaxDataLen;
  pConnectPacket->MinReplySize   = (tsp00_Int4)pConPktParamRec->ulMinReplySize;
  pConnectPacket->Filler1        = 0;
  pConnectPacket->Filler2        = 0;

  strcpy((char *)pConnectPacket->ReceiverServerDB, pConPktParamRec->szReceiverServerDB );
  strcpy((char *)pConnectPacket->SenderServerDB, pConPktParamRec->szSenderServerDB );

  DBGOUT;
  return ( sizeof (*pConnectPacket) - sizeof (pConnectPacket->VarPart) );
  }

/*------------------------------*/

static tsp00_Uint eo420ExtractRTEHeader        (tsp00_Int4                   ulSwapType,
                                                teo003_ConPktParamRecord     *pConPktParamRec,
                                                teo003_RteHeaderRecord       *pRTEHeader,
                                                tsp00_ErrTextc               pErrText)
{
#undef  MF__
#define MF__ MOD__"eo420ExtractRTEHeader"

    DBGPAS;

    if ( eo420UnpackInt4 ( ulSwapType, pRTEHeader->ActSendLen,
        &pRTEHeader->ActSendLen,
        pErrText ) != commErrOk_esp01 )
        return ( commErrNotOk_esp01 );

    if ( eo420UnpackInt2 ( ulSwapType, pRTEHeader->RTEReturnCode,
        &pRTEHeader->RTEReturnCode,
        pErrText ) != commErrOk_esp01 )
        return ( commErrNotOk_esp01 );

    if ( eo420UnpackInt4 ( ulSwapType, pRTEHeader->MaxSendLen,
        &pRTEHeader->MaxSendLen,
        pErrText ) != commErrOk_esp01 )
        return ( commErrNotOk_esp01 );

    if ( eo420UnpackInt4 ( ulSwapType, pRTEHeader->ReceiverRef,
        &pRTEHeader->ReceiverRef,
        pErrText ) != commErrOk_esp01 )
        return ( commErrNotOk_esp01 );

    if ( eo420UnpackInt4 ( ulSwapType, pRTEHeader->SenderRef,
        &pRTEHeader->SenderRef,
        pErrText ) != commErrOk_esp01 )
        return ( commErrNotOk_esp01 );

    pConPktParamRec->ulSwapType          = ulSwapType;
    pConPktParamRec->ulCommState         = pRTEHeader->RTEReturnCode;
    pConPktParamRec->ulSenderRef         = pRTEHeader->SenderRef;
    pConPktParamRec->ulReceiverRef       = pRTEHeader->ReceiverRef;
    pConPktParamRec->ulMessClass         = pRTEHeader->MessClass;

    return ( commErrOk_esp01 );
}

/*------------------------------*/

static tsp00_Uint eo420ExtractRteConnectPacket ( teo003_ConPktParamRecord     *pConPktParamRec,
                                                 teo003_RteHeaderRecord       *pRTEHeader,
                                                 teo003_RteConnectPacketRec   *pConnectPacket,
                                                 tsp00_ErrTextc                pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420ExtractRteConnectPacket"

  tsp00_Int4       ulSwapType    = 0;
  teo001_Pid       pidSenderPID  = RTE_UNDEF_OSPID;
  tsp00_Uint2      usServicePort = 0;
  tsp00_Bool       fAcknowledge;
  tsp00_Bool       fOmitReplyPart = false;

  DBGPAS;

  ulSwapType  = pConnectPacket->sMessCode[ 1 ];

  if (commErrOk_esp01 != eo420ExtractRTEHeader (ulSwapType, pConPktParamRec, pRTEHeader, pErrText))
  {
      return commErrNotOk_esp01;
  }

  if ( eo420UnpackInt2 ( ulSwapType, pConnectPacket->ConnectLength,
      &pConnectPacket->ConnectLength,
      pErrText ) != commErrOk_esp01 )
      return ( commErrNotOk_esp01 );

  if ( eo420UnpackInt4 ( ulSwapType, pConnectPacket->MaxSegmentSize,
      &pConnectPacket->MaxSegmentSize,
      pErrText ) != commErrOk_esp01 )
      return ( commErrNotOk_esp01 );

  if ( eo420UnpackInt4 ( ulSwapType, pConnectPacket->MaxDataLen,
      &pConnectPacket->MaxDataLen,
      pErrText ) != commErrOk_esp01 )
      return ( commErrNotOk_esp01 );

  if ( eo420UnpackInt4 ( ulSwapType, pConnectPacket->PacketSize,
      &pConnectPacket->PacketSize,
      pErrText ) != commErrOk_esp01 )
      return ( commErrNotOk_esp01 );

  if ( eo420UnpackInt4 ( ulSwapType, pConnectPacket->MinReplySize,
      &pConnectPacket->MinReplySize,
      pErrText ) != commErrOk_esp01 )
      return ( commErrNotOk_esp01 );

  eo420_GetRemotePid    ( pConnectPacket, &pidSenderPID );
  eo420_GetPortNo       ( pConnectPacket, &usServicePort );
  eo420_GetAcknowledge  ( pConnectPacket, &fAcknowledge );
  eo420_GetOmitReplyPart( pConnectPacket, &fOmitReplyPart );


  /* PTS 1105295 */
  
  eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_DBROOT_EO003,
                               pConPktParamRec->szServerDBRoot, sizeof(pConPktParamRec->szServerDBRoot) );

  eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_SERVERPGM_EO003,
                               pConPktParamRec->szServerPgm, sizeof(pConPktParamRec->szServerPgm) );

  eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_VERSION_EO003,
                               pConPktParamRec->szVersion, sizeof(pConPktParamRec->szVersion) );

  eo420_GetStringFromVarPart ( pConnectPacket, RSQL_OPT_AUTH_ALLOW_EO003,
                               pConPktParamRec->authenticationAllow, sizeof(pConPktParamRec->authenticationAllow) );

  pConPktParamRec->pidSenderPID        = pidSenderPID;
  pConPktParamRec->usServicePort       = usServicePort;
  pConPktParamRec->fAcknowledge        = fAcknowledge;
  pConPktParamRec->fOmitReplyPart      = fOmitReplyPart;
  
  pConPktParamRec->ulServiceType       = pConnectPacket->ServiceType;
  pConPktParamRec->ulMaxSegmentSize    = pConnectPacket->MaxSegmentSize;
  pConPktParamRec->ulPacketSize        = pConnectPacket->PacketSize;
  pConPktParamRec->ulMaxDataLen        = pConnectPacket->MaxDataLen;
  pConPktParamRec->ulMinReplySize      = pConnectPacket->MinReplySize;

  /*
   * Size of CString and PString differ! C8 != C18. If fixed replace by .p2c member function...
   */
  eo46PtoC ( pConPktParamRec->szSenderServerDB,
             (tsp00_Byte *)(char *)pConnectPacket->SenderServerDB,
             sizeof( pConnectPacket->SenderServerDB ));
  eo46PtoC ( pConPktParamRec->szReceiverServerDB,
             (tsp00_Byte *)(char *)pConnectPacket->ReceiverServerDB,
             sizeof( pConnectPacket->ReceiverServerDB ));

  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420UnpackInt2 ( tsp00_Uint                    ulSwapTyp,
                                    tsp00_Int2                    Src,
                                    tsp00_Int2                   *pDst,
                                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420UnpackInt2"
  register unsigned char *puc;

  DBGPAS;

  puc = (unsigned char *) &Src;

  switch ( ulSwapTyp )
    {
    case 1:
          /* --- no swap: hi-lo */
          *pDst = ( puc[ 0 ] << 8 ) | ( puc[ 1 ] << 0 );
          break;
    case 2:
          /* --- full swap: lo-hi */
    case 3:
          /* --- half swap: lo-hi */
          *pDst = ( puc[ 0 ] << 0 ) | ( puc[ 1 ] << 8 );
          break;
    default:
          MSGD (( ERR_ILL_SWAP_TYPE, ulSwapTyp ));
          strcpy( pErrText, ERRMSG_ILL_SWAP_TYPE );
          *pDst = 0;
          return ( commErrNotOk_esp01 );
    }

  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp00_Uint eo420UnpackInt4 ( tsp00_Uint                    ulSwapTyp,
                                    tsp00_Int4                    Src,
                                    tsp00_Int4                   *pDst,
                                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"eo420UnpackInt4"
  register unsigned char *puc;

  DBGPAS;

  puc = (unsigned char *) &Src;

  switch ( ulSwapTyp )
    {
    case 1:
          /* --- no swap */
          *pDst = ( puc[ 0 ] << 24 ) |
                  ( puc[ 1 ] << 16 ) |
                  ( puc[ 2 ] <<  8 ) |
                  ( puc[ 3 ] <<  0 );
          break;
    case 2:
          /* --- full swap */
          *pDst = ( puc[ 0 ] <<  0 ) |
                  ( puc[ 1 ] <<  8 ) |
                  ( puc[ 2 ] << 16 ) |
                  ( puc[ 3 ] << 24 );
          break;
    case 3:
          /* --- half swap */
          *pDst = ( puc[ 0 ] <<  8 ) |
                  ( puc[ 1 ] <<  0 ) |
                  ( puc[ 2 ] << 24 ) |
                  ( puc[ 3 ] << 16 );
          break;
    default:
          MSGD (( ERR_ILL_SWAP_TYPE, ulSwapTyp ));
          strcpy( pErrText, ERRMSG_ILL_SWAP_TYPE );
          *pDst = 0;
          return ( commErrNotOk_esp01 );
    }

  return ( commErrOk_esp01 );
  }
#endif

/*------------------------------*/

static SAPDB_UInt1 eo420OldSwapType ()
{
  SAPDB_UInt1    i;

  union map_check_i4_b4         MapCheck;

  DBGIN;

  MapCheck.int4 = 65536;
  for ( i = 0; (i < 4) && ( MapCheck.c4[ i ] != 1 ); i++ ) { ; }

  return i;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
