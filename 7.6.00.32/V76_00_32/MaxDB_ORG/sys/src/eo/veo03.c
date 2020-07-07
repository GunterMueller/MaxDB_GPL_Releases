/*!
  @file           veo03.c
  @author         JoergM
  @special area   communication
  @brief          RTE independent functions (USER communication)
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"
#include "geo001.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "heo46.h"
#include "geo57.h"
#include "geo60.h"
#include "heo03.h"
#include "geo03.h"
#include "geo43_0.h"
#include "heo420.h"
#include "RunTime/Security/RTESec_SAPSSL.h"
#include "SAPDBCommon/SAPDB_string.h"

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/
#include "hos23.h"      /*nocheck*/
#else                   /*&else*/
#include "gen003.h"     /*nocheck*/
#define SOCKETS
#include <sys/socket.h>
#include "gen42.h"      /*nocheck*/
#endif                  /*&endif*/


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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static tsp00_Uint     
eo03_GetServicePort ( SAPDB_Bool     isSSL,
                      SAPDB_Bool     isNISSL,
                      char          *szSapRouterString, 
                      tsp00_Uint2   *pusServicePort,
                      tsp00_ErrTextc VAR_ARRAY_REF  pErrText) ;

static tsp00_Uint     
eo03_NiUpdateConnInfoAfterConnect ( teo003_ConnectParamRecord    *pConnParam,
                                    teo40_NiConnectInfoRecord    *pNIConnInfo,
                                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText);

static tsp00_Uint     
eo03_InitNIConnInfo               ( teo003_ConnectParamRecord    *pConnParam,
                                    teo40_NiConnectInfoRecord    *pNIConnInfo,
                                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText);

static tsp00_Uint     
eo03NiSqlConnect  ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

static void 
eo03InitConnectParamRec ( teo40_NiConnectInfoRecord *pNIConnInfo,
                          teo003_ConPktParamRecord  *pConPktParamRec,
                          tsp00_Uint                 ulMessClass);

static tsp00_Uint     
eo03InfoRequest         ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                          tsp00_ErrTextc VAR_ARRAY_REF  ErrText );

static void     
eo03_NiClose (teo40_NiConnectInfoRecord    *pNIConnInfo);

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* PTS 1104899 */

externC tsp00_Bool 
SqlIsSaprouterString ( char *szServerNode )	/* '\0' terminated node or saprouterstring */
{
  return ( (tsp00_Bool ) eo40NiIsSaprouterSring ( szServerNode ) );
}

/*------------------------------*/

/* PTS 1105295 */

externC void 
eo03NiBuildTracefileName    (tsp00_Pathc VAR_ARRAY_REF  TracefileName)
{
  tsp01_RteError  RteError;

  if ( sqlGetIndependentWrkPath(TracefileName, TERM_WITH_DELIMITER_EO01, &RteError ) )
    strcat( TracefileName, NI_TRACEFILE_EO40 ) ;
  else
    strcpy( TracefileName, NI_TRACEFILE_EO40 ) ; /* use current directory ( x_ping ) */

  return ;
}

/*------------------------------*/
/*
externC tsp00_Uint     
eo03NiInit  (tsp00_ErrTextc  VAR_ARRAY_REF  pErrText)
{
    tsp00_Pathc         TracefileName;
    tsp00_Uint          CommState = commErrOk_esp01;


    if (!eo40NiIsInitialized ())
    {
        eo03NiBuildTracefileName (TracefileName);

        CommState = eo40NiInit(TracefileName, NI_TRACE_ERRORS_ONLY_EO40, pErrText);
        if (commErrOk_esp01 != CommState)
        {
            tsp00_ErrTextc  tmpErrText;
            eo40NiFinish (tmpErrText);
        }
    }

    return (CommState);
}
*/
/*------------------------------*/

externC tsp00_Uint     
eo03NiConnect ( teo003_ConnectParamRecord    *pConnParam,
                teo40_NiConnectInfoRecord    *pNIConnInfo,
                tsp00_ErrTextc VAR_ARRAY_REF  pErrText  )
{
  #undef  MF__
  #define MF__ MOD__"eo03NiConnect"

  tsp00_Uint      ulCommState;

  ulCommState = eo03_InitNIConnInfo( pConnParam, pNIConnInfo, pErrText ); 
  ulCommState = eo03NiSqlConnect (pNIConnInfo, 
                                  pErrText);
  if ( ulCommState == commErrOk_esp01 )
    ulCommState = eo03_NiUpdateConnInfoAfterConnect( pConnParam, pNIConnInfo, pErrText ); 

  if (ulCommState != commErrOk_esp01)
  {
      eo03_NiClose (pNIConnInfo);
  }

  return ( ulCommState );
}

/*------------------------------*/

externC tsp00_Uint     
eo03NiRelease ( teo40_NiConnectInfoRecord *pNIConnInfo,
                tsp00_ErrTextc  VAR_ARRAY_REF  pErrText  )
{

    tsp00_Uint              ulMessClass;
    tsp00_Uint              CommState = commErrOk_esp01;
    tsp00_Int               rc = NO_ERROR_EO001;
    SEND_PKT_FUNC_PTR_EO003 pSendFunc;
    void *                  pHdl;
    SAPDB_Bool              isSSL = pNIConnInfo->isSSL;


    switch ( pNIConnInfo->ulServiceType )
    {
    case srvControl_esp01 :
        ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;
        break;

    default:
        ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;
        break;
    }

    if (isSSL)
    {
        pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
        pHdl = pNIConnInfo->pSSLHandle;
    }
    else
    {
        pSendFunc = eo40NiSend;
        pHdl = pNIConnInfo->NiHandle;
    }

    CommState = eo420SendReleasePacket (pHdl, 
                                        pSendFunc,
                                        pNIConnInfo->ulClientRef, 
                                        pNIConnInfo->ulServerRef, 
                                        ulMessClass, 
                                        commErrOk_esp01);
    
    eo03_NiClose (pNIConnInfo);

    if ( pNIConnInfo->pCommPacketList[0] != NULL ) 
    {
        rc = FREE_MEM_EO57 ( pNIConnInfo->pCommPacketList[0] );

        if ( NO_ERROR_EO001 == rc  )
            pNIConnInfo->pCommPacketList[0] = NULL;
    }

    if ( pNIConnInfo->szSapRouterString != NULL )
    {
        if ( NO_ERROR_EO001 == rc )
            rc = FREE_MEM_EO57 ( pNIConnInfo->szSapRouterString ) ;
        else
            FREE_MEM_EO57 ( pNIConnInfo->szSapRouterString ) ; /* don't overwrite previous error */

        pNIConnInfo->szSapRouterString = NULL ;
    }

    if ( rc != NO_ERROR_EO001 )
    {
        eo46BuildErrorStringRC ( pErrText, (char *)ERRMSG_COM_CANT_FREE_MEM, rc );
        CommState = commErrNotOk_esp01 ;
    }

    return ( CommState );
}

/*------------------------------*/

externC tsp00_Uint     
eo03NiRequest  ( teo40_NiConnectInfoRecord     *pNIConnInfo,
                 tsp00_Uint                     ulCmdPacket,
                 tsp00_Uint                     ulCmdDataLen,
                 tsp00_Uint1                    RTEFlags,
                 tsp00_ErrTextc  VAR_ARRAY_REF  pErrText )
  {

  tsp00_Uint                      ulCommState;
  teo003_CommPacketRecord         *pCommPacket = NULL;
  tsp00_Uint                      ulMessClass;
  SEND_PKT_FUNC_PTR_EO003         pSendFunc;
  void *                          pHdl;
  SAPDB_Bool                      isSSL = pNIConnInfo->isSSL;



  if ( pNIConnInfo->ulServiceType == srvDistribution_esp01 )
    ulMessClass = RSQL_KERN_DATA_REQUEST_EO003;
  else
    ulMessClass = RSQL_USER_DATA_REQUEST_EO003;

  pCommPacket                  = pNIConnInfo->pCommPacketList[ulCmdPacket];
  pNIConnInfo->ulCurrentPacket = ulCmdPacket;
  pNIConnInfo->ulCmdDataLen    = ulCmdDataLen;

  if (isSSL)
  {
      pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
      pHdl = pNIConnInfo->pSSLHandle;
  }
  else
  {
      pSendFunc = eo40NiSend;
      pHdl = pNIConnInfo->NiHandle;
  }

  ulCommState = eo420SendCommPacket ( pHdl, 
                                      pSendFunc,
                                      pNIConnInfo->ulMaxSegmentSize,
                                      pCommPacket,
                                      ulCmdDataLen,
                                      pNIConnInfo->ulClientRef,
                                      pNIConnInfo->ulServerRef,
                                      ulMessClass,
                                      commErrOk_esp01,
                                      RTEFlags,
                                      pErrText );

  return ( ulCommState );
  }

/*===========================================================================*/

externC tsp00_Uint     
eo03NiReceive  ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                    teo003_CommPacketRecord     **ppResPacket,
                    tsp00_Uint                   *pulResDataLen,
                    tsp00_ErrTextc  VAR_ARRAY_REF  pErrText )
{

  tsp00_Uint                              ulCommState;
  teo003_CommPacketRecord                *pCmdCommPacket;
  tsp00_Uint                              ulCurrentPacket;
  tsp00_Uint                              ulSenderRef;
  tsp00_Uint                              ulReceiverRef;
  tsp00_Uint                              ulMessClass;
  RECV_PKT_FUNC_PTR_EO003                 pReceiveFunc;
  void *                                  pHdl;
  SAPDB_Bool                              isSSL = pNIConnInfo->isSSL;
  tsp00_Uint1                             RTEFlags;
  ULONG                                   maxReceiveLength;



  ulCurrentPacket = pNIConnInfo->ulCurrentPacket;
  pCmdCommPacket  = pNIConnInfo->pCommPacketList[ulCurrentPacket];

  if (0 == pNIConnInfo->ulMinReplySize)
  {
    *ppResPacket      = pCmdCommPacket;
    maxReceiveLength  = pNIConnInfo->ulMaxDataLen;
  }
  else
  {
    *ppResPacket    = (teo003_CommPacketRecord *) ((char *)pCmdCommPacket->pDataPart +
                                        pNIConnInfo->ulCmdDataLen);
    maxReceiveLength = pNIConnInfo->ulMaxDataLen - pNIConnInfo->ulCmdDataLen;
  }

  if (isSSL)
  {
      pReceiveFunc = (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive;
      pHdl = pNIConnInfo->pSSLHandle;
  }
  else
  {
      pReceiveFunc = eo40NiReceive;
      pHdl = pNIConnInfo->NiHandle;
  }

  ulCommState = eo420ReceiveCommPacket ( pHdl, 
                                         pReceiveFunc, 
                                         pNIConnInfo->ulSwapType,
                                        *ppResPacket,
                                         maxReceiveLength,
                                         pulResDataLen,
                                        &ulSenderRef,
                                        &ulReceiverRef,
                                        &ulMessClass,
                                        &RTEFlags,
                                         pErrText );
 
  return ( ulCommState );
}
/*===========================================================================*/

/* PTS 1105295 */
externC tsp00_Uint     
eo03NiXserverPing( char                         *szSapRouterString,
                   tsp00_Versionc VAR_ARRAY_REF  szServerVersion,
                   SAPDB_Byte *                  pServerCertificate,
                   SAPDB_UInt4                   lenCertificate,
                   SAPDB_Bool                    isSSL,
                   SAPDB_Bool                    isNISSL,
                   SAPDB_Bool                    ignoreHostnameInServerCert,  
                   SAPDB_Bool                    captureServerCert,  
                   tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{

    teo40_NiConnectInfoRecord       TmpNIConnInfo;
    tsp00_Uint                      ulCommState = commErrOk_esp01 ;
    SEND_PKT_FUNC_PTR_EO003         pSendFunc;
    RECV_PKT_FUNC_PTR_EO003         pReceiveFunc;
    void *                          pHdl;

    memset (&TmpNIConnInfo, 0, sizeof (TmpNIConnInfo));

    if ( szSapRouterString )
    {
        TmpNIConnInfo.szSapRouterString = szSapRouterString ;
        TmpNIConnInfo.usServicePort     = 0 ;

        /* SAP Router String may also be a normal Hostname */
        ulCommState = eo03_GetServicePort(isSSL,
                                          isNISSL,
                                          TmpNIConnInfo.szSapRouterString,
                                          &TmpNIConnInfo.usServicePort, 
                                          pErrText);

        if ( ulCommState == commErrOk_esp01 )
            ulCommState = eo40NiConnectToServer ( &TmpNIConnInfo, pErrText ) ;


        if ( ulCommState == commErrOk_esp01 )
        {
            teo003_ConPktParamRecord         ConPktParamRec;

            /* SSL Start */
            if (isSSL)
            {
                ulCommState = RTESec_SAPSSLStartSession (TmpNIConnInfo.NiHandle,
                    TmpNIConnInfo.szSapRouterString,
                    ignoreHostnameInServerCert,
                    &TmpNIConnInfo.pSSLHandle,
                    pErrText); 
            }

            if (commErrOk_esp01 == ulCommState)
            {

                memset(&ConPktParamRec, 0, sizeof(teo003_ConPktParamRecord) ) ;

                /* send PING request */
                if (captureServerCert)
                {
                    ConPktParamRec.ulMessClass         = RSQL_CERT_REQUEST_EO003;
                }
                else
                {
                    ConPktParamRec.ulMessClass         = RSQL_PING_REQUEST_EO003;
                }

                if (isSSL)
                {
                    pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
                    pReceiveFunc = (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive;
                    pHdl = TmpNIConnInfo.pSSLHandle;
                }
                else
                {
                    pSendFunc = eo40NiSend;
                    pReceiveFunc = eo40NiReceive;
                    pHdl = TmpNIConnInfo.NiHandle;
                }

                ulCommState = eo420SendConnectPacket (pHdl, pSendFunc,
                                                          &ConPktParamRec,  pErrText);

                if ( ulCommState == commErrOk_esp01 )
                {
                    /* receive server reply */
                    if (captureServerCert)
                    {
                        ulCommState = eo420ReceiveCertificatePacket (pHdl, 
                                                                     pReceiveFunc,
                                                                     &ConPktParamRec, 
                                                                     pServerCertificate, 
                                                                     lenCertificate,
                                                                     pErrText );

                        if (ulCommState == commErrOk_esp01)
                        {  
                            if ( ConPktParamRec.ulMessClass != RSQL_CERT_REPLY_EO003 )
                            {
                                MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
                                strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
                                ulCommState = commErrNotOk_esp01 ;
                            }
                        }
                    }
                    else
                    {
                        ulCommState = eo420ReceiveConnectPacket (pHdl, 
                            pReceiveFunc,
                            &ConPktParamRec, pErrText );

                        if ( ulCommState == commErrOk_esp01 )
                        {  
                            if ( ConPktParamRec.ulMessClass != RSQL_PING_REPLY_EO003 )
                            {
                                MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
                                strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
                                ulCommState = commErrNotOk_esp01 ;
                            }
                            else
                            {
                                SAPDB_memcpy (szServerVersion, &ConPktParamRec.szVersion[0], sizeof(tsp00_Versionc) -1 ) ;
                                szServerVersion[sizeof(tsp00_Versionc) -1] = '\0' ;
                            }
                        }
                    }
                }
            }

            eo03_NiClose (&TmpNIConnInfo);
        }
    }

    return ( ulCommState );
}

/*===========================================================================*/

externC tsp00_Uint     
eo03NiSqlCancelDump       ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                            char                         *szSapRouterString,
                            tsp00_Uint                    ulReqType,
                            tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{

    teo40_NiConnectInfoRecord       TmpNIConnInfo;
    tsp00_Uint                      ulMessClass;
    tsp00_Uint                      ulCommState = commErrOk_esp01 ;
    SEND_PKT_FUNC_PTR_EO003         pSendFunc;
    void *                          pHdl;


    TmpNIConnInfo = *pNIConnInfo;

    switch ( ulReqType )
    {
    case SQL_RTE_CANCEL_EO003:
        if (pNIConnInfo->ulServiceType == srvControl_esp01)
            ulMessClass = RSQL_CTRL_CANCEL_REQUEST_EO003;
        else
            ulMessClass = RSQL_USER_CANCEL_REQUEST_EO003;
        break;

    case SQL_RTE_DUMP_EO003:
        ulMessClass = RSQL_DUMP_REQUEST_EO003;
        break;

    default:
        MSGD (( ERR_PROTOCOL_ERROR, "REQUEST TYPE" )); 
        strcpy((char *)pErrText, ERRMSG_PROTOCOL_ERROR);
        ulCommState = commErrNotOk_esp01 ;
    }

    if ( ulCommState == commErrOk_esp01 )
    {
        teo003_ConPktParamRecord         ConPktParamRec;
        /* connect to server */
        TmpNIConnInfo.ulMaxSegmentSize  = sizeof(teo003_RteHeaderRecord) +
            sizeof(teo003_RteConnectPacketRec);
        TmpNIConnInfo.ulMinSegmentSize  = sizeof(teo003_RteHeaderRecord);

        /* PTS 1105116 */
        /* Cancellation may be performed over an other SAPRouter String.                    */
        /* But we do not support cancellation and dumping over SSL with a SAPRouter String  */
        /* when the connection was initiated by SSL without SAPRouter String.               */
        if ( szSapRouterString && (!TmpNIConnInfo.isSSL || TmpNIConnInfo.isSSLoverSAPRouterString))
        {
            TmpNIConnInfo.szSapRouterString = szSapRouterString ;
        }

        ulCommState = eo40NiConnectToServer ( &TmpNIConnInfo, pErrText ) ;

        if ( ulCommState == commErrOk_esp01 )
        {
            /* SSL Start */
            if (TmpNIConnInfo.isSSL)
            {
                ulCommState = RTESec_SAPSSLStartSession (TmpNIConnInfo.NiHandle,
                    TmpNIConnInfo.szSapRouterString,
                    TmpNIConnInfo.ignoreHostnameInServerCert,
                    &TmpNIConnInfo.pSSLHandle,
                    pErrText); 
            }

            if (commErrOk_esp01 == ulCommState)
            {
                /* send cancel request */
                ConPktParamRec.ulMessClass         = ulMessClass;
                ConPktParamRec.ulSenderRef         = TmpNIConnInfo.ulClientRef;
                ConPktParamRec.ulReceiverRef       = TmpNIConnInfo.ulServerRef;
                ConPktParamRec.ulServiceType       = TmpNIConnInfo.ulServiceType;
                ConPktParamRec.ulMaxSegmentSize    = TmpNIConnInfo.ulMaxSegmentSize;
                ConPktParamRec.ulPacketSize        = TmpNIConnInfo.ulPacketSize;
                ConPktParamRec.ulMaxDataLen        = TmpNIConnInfo.ulMaxDataLen;
                ConPktParamRec.ulMinReplySize      = TmpNIConnInfo.ulMinReplySize;
                ConPktParamRec.ulSwapType          = (tsp00_Uint    )undef_ceo00;
                ConPktParamRec.szSenderServerDB[0] = '\0';
                ConPktParamRec.pidSenderPID        = TmpNIConnInfo.pidClientPID;
                ConPktParamRec.usServicePort       = TmpNIConnInfo.usServicePort;
                ConPktParamRec.ulCommState         = commErrOk_esp01;
                ConPktParamRec.fAcknowledge        = false;

                if (TmpNIConnInfo.isSSL)
                {
                    pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
                    pHdl = TmpNIConnInfo.pSSLHandle;
                }
                else
                {
                    pSendFunc = eo40NiSend;
                    pHdl = TmpNIConnInfo.NiHandle;
                }

                ulCommState = eo420SendConnectPacket ( pHdl, pSendFunc,
                    &ConPktParamRec,  pErrText );                  

            }

            eo03_NiClose (&TmpNIConnInfo);
        }

    }
    return ( ulCommState );
}

/*===========================================================================*/

externC tsp00_Uint     
eo03NiSqlReplyAvailable   ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                           tsp00_ErrTextc  VAR_ARRAY_REF  pErrText )
{
    tsp00_Uint     ulCommState = eo40NiDataAvailable(pNIConnInfo->NiHandle, pErrText);

    return(ulCommState);
}

/*===========================================================================*/

externC void     
eo03Finish ()
{
    tsp00_ErrTextc   errText;

    RTESec_SAPSSLClose ();

    eo40NiFinish (errText);
}



/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static tsp00_Uint     
eo03NiSqlConnect  ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    tsp00_Uint              CommState = commErrOk_esp01;
    SEND_PKT_FUNC_PTR_EO003 pSendFunc;
    RECV_PKT_FUNC_PTR_EO003 pReceiveFunc;
    void *                  pHdl;
    SAPDB_Bool              isSSL = pNIConnInfo->isSSL;


    pNIConnInfo->ulMaxSegmentSize = sizeof(teo003_RteHeaderRecord) +
        sizeof(teo003_RteConnectPacketRec);
    pNIConnInfo->ulMinSegmentSize = sizeof(teo003_RteHeaderRecord);

    /* connect to server */
    CommState = eo40NiConnectToServer ( pNIConnInfo, pErrText ) ;

    if ( CommState == commErrOk_esp01 )
    {
        if (isSSL)
        {
            CommState = RTESec_SAPSSLStartSession (pNIConnInfo->NiHandle,
                pNIConnInfo->szSapRouterString,
                pNIConnInfo->ignoreHostnameInServerCert,
                &pNIConnInfo->pSSLHandle,
                pErrText); 
        }

        if (commErrOk_esp01 == CommState)
        {
            if (pNIConnInfo->ulServiceType != srvControl_esp01)
            {
                /* info request to server, get packet sizes */
                CommState = eo03InfoRequest (pNIConnInfo, pErrText); 
            }
            else
            {
                /* packet sizes are fixed, no info request necessary */
                pNIConnInfo->ulPacketSize   = RSQL_CTRL_PACKET_SIZE_EO003 ;
                pNIConnInfo->ulMinReplySize = RSQL_CTRL_MIN_REPLY_SIZE_EO003;
                pNIConnInfo->ulMaxDataLen   = pNIConnInfo->ulPacketSize
                    - (2 * RTE_HEADER_SIZE_EO003);
            }
        }
    }

    if ( CommState == commErrOk_esp01 )
    {
        teo003_ConPktParamRecord   ConPktParamRec;

        pNIConnInfo->ulMaxSegmentSize = pNIConnInfo->ulPacketSize;
        pNIConnInfo->ulMinSegmentSize = 1024;

        /* Können die Buffergrößen bei SSL über NiHandle geändert werden; Und ist der Zeitpunkt korrekt (SapSSLSetNiHdl schon geschehen) */

        /* try to set socket buffers to get maximum performance */
        CommState = eo420SetNiSocketBufferSize ( pNIConnInfo->NiHandle,
            pNIConnInfo->ulMinSegmentSize,
            &pNIConnInfo->ulMaxSegmentSize,
            pErrText ) ;

        if ( CommState == commErrOk_esp01 )
        {
            eo03InitConnectParamRec(pNIConnInfo, &ConPktParamRec,
                eo420ServiceTypeToMessClass (pNIConnInfo->ulServiceType ));

            if ( ConPktParamRec.ulMessClass == RSQL_CTRL_CONN_REQUEST_EO003 )
            {
                if (pNIConnInfo->pszServerPgm != NULL) 
                    strcpy ( ConPktParamRec.szServerPgm, pNIConnInfo->pszServerPgm);

                if (pNIConnInfo->pszServerDBRoot != NULL) 
                    strcpy ( ConPktParamRec.szServerDBRoot, pNIConnInfo->pszServerDBRoot);
            }

            /* send connect packet to server */
            if (isSSL)
            {
                pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
                pReceiveFunc = (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive;
                pHdl = pNIConnInfo->pSSLHandle;
            }
            else
            {
                pSendFunc = eo40NiSend;
                pReceiveFunc = eo40NiReceive;
                pHdl = pNIConnInfo->NiHandle;
            }

            CommState = eo420SendConnectPacket ( pHdl, pSendFunc,
                &ConPktParamRec,  pErrText );

            if ( CommState == commErrOk_esp01 )
            {
                /* receive server reply */
                CommState = eo420ReceiveConnectPacket ( pHdl, 
                    pReceiveFunc,
                    &ConPktParamRec, pErrText );

                if ( CommState == commErrOk_esp01 )
                {
                    if ( ConPktParamRec.ulMaxSegmentSize > pNIConnInfo->ulMaxSegmentSize )
                    {
                        strcpy(pErrText, ERRMSG_COM_ILL_MAXSEGMENTSIZE );
                        CommState = commErrNotOk_esp01 ;
                    }
                    else
                    {

                        pNIConnInfo->ulMaxSegmentSize = ConPktParamRec.ulMaxSegmentSize;
                        pNIConnInfo->ulServerRef      = ConPktParamRec.ulSenderRef;
                        pNIConnInfo->ulSwapType       = ConPktParamRec.ulSwapType;

                        strncpy ( pNIConnInfo->authenticationAllow, 
                                  ConPktParamRec.authenticationAllow, 
                                  sizeof(pNIConnInfo->authenticationAllow) - 1 );
                        pNIConnInfo->authenticationAllow[sizeof(pNIConnInfo->authenticationAllow) - 1] = '\0';
                    }
                }
            }
        }
    }

    return ( CommState );
}
/*===========================================================================*/

static tsp00_Uint     
eo03InfoRequest( teo40_NiConnectInfoRecord    *pNIConnInfo,
                 tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{

  teo003_ConPktParamRecord ConPktParamRec;
  tsp00_Uint               CommState;
  SEND_PKT_FUNC_PTR_EO003  pSendFunc;
  RECV_PKT_FUNC_PTR_EO003  pReceiveFunc;
  void *                  pHdl;
  SAPDB_Bool              isSSL = pNIConnInfo->isSSL;



  eo03InitConnectParamRec ( pNIConnInfo, &ConPktParamRec, RSQL_INFO_REQUEST_EO003 ) ;

  /* send connect packet to server */
  if (isSSL)
  {
      pSendFunc = (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend;
      pReceiveFunc = (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive;
      pHdl = pNIConnInfo->pSSLHandle;
  }
  else
  {
      pSendFunc = eo40NiSend;
      pReceiveFunc = eo40NiReceive;
      pHdl = pNIConnInfo->NiHandle;
  }
      
  CommState = eo420SendConnectPacket ( pHdl, pSendFunc,
                                       &ConPktParamRec,  pErrText );

  if ( CommState == commErrOk_esp01 )
  {
    /* receive server reply */
    CommState = eo420ReceiveConnectPacket ( pHdl, 
                                            pReceiveFunc,
                                           &ConPktParamRec, pErrText );

    if ( CommState == commErrOk_esp01 )
    {  
      if ( ConPktParamRec.ulMessClass != RSQL_INFO_REPLY_EO003 )
      {
       MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
       strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
       CommState = commErrNotOk_esp01 ;
      }
      else
      if ( ConPktParamRec.ulServiceType != pNIConnInfo->ulServiceType )
      {
        MSGD (( ERR_PROTOCOL_ERROR, "SERVICE" ));
        strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
        CommState = commErrNotOk_esp01 ;
      }
      else
      if ( strcmp ( pNIConnInfo->szServerDB,
                    ConPktParamRec.szSenderServerDB ) )
      {
        MSGD (( ERR_PROTOCOL_ERROR, "SERVERDB" ));
        strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
        CommState = commErrNotOk_esp01 ;
      }
      else
      {
        tsp00_Uint     ulPeerHeaderSize = ConPktParamRec.ulPacketSize -
                                          ConPktParamRec.ulMaxDataLen;
        tsp00_Uint     ulMyHeaderSize   = RTE_HEADER_SIZE_EO003 * 2;

        if ( ulPeerHeaderSize < ulMyHeaderSize )
             ConPktParamRec.ulMaxDataLen -= ulMyHeaderSize - ulPeerHeaderSize;

        if ( ConPktParamRec.usServicePort != 0 )
          pNIConnInfo->usServicePort = ConPktParamRec.usServicePort;

        pNIConnInfo->ulPacketSize    = ConPktParamRec.ulPacketSize;
        pNIConnInfo->ulMaxDataLen    = ConPktParamRec.ulMaxDataLen;
        pNIConnInfo->ulMinReplySize  = ConPktParamRec.ulMinReplySize;
      }
    }
  }
  return ( CommState );
}

/*===========================================================================*/

static void 
eo03InitConnectParamRec ( teo40_NiConnectInfoRecord *pNIConnInfo,
                          teo003_ConPktParamRecord  *pConPktParamRec,
                          tsp00_Uint                 ulMessClass)
{
  eo420InitConPktParamRec ( pConPktParamRec );
  pConPktParamRec->ulMessClass         = ulMessClass;
  pConPktParamRec->ulSenderRef         = pNIConnInfo->ulClientRef;
  pConPktParamRec->ulServiceType       = pNIConnInfo->ulServiceType;
  pConPktParamRec->ulMaxSegmentSize    = pNIConnInfo->ulMaxSegmentSize;
  pConPktParamRec->ulPacketSize        = pNIConnInfo->ulPacketSize;
  pConPktParamRec->ulMaxDataLen        = pNIConnInfo->ulMaxDataLen;
  pConPktParamRec->ulMinReplySize      = pNIConnInfo->ulMinReplySize;
  pConPktParamRec->pidSenderPID        = pNIConnInfo->pidClientPID;
  pConPktParamRec->usServicePort       = pNIConnInfo->usServicePort;
  strcpy ( pConPktParamRec->szReceiverServerDB, pNIConnInfo->szServerDB );

  if ( ulMessClass != RSQL_INFO_REQUEST_EO003 )
    pConPktParamRec->fAcknowledge        = pNIConnInfo->fAcknowledge;

}


/*===========================================================================*/

static tsp00_Uint     
eo03_NiUpdateConnInfoAfterConnect ( teo003_ConnectParamRecord    *pConnParam,
                                    teo40_NiConnectInfoRecord    *pNIConnInfo,
                                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText)
{

  char                           *pCommPackets;
  tsp00_Uint                      CommState = commErrOk_esp01;
  tsp00_Int                       rc = NO_ERROR_EO001;

  rc = ALLOC_MEM_EO57 ( (void **)&pCommPackets,
                        pNIConnInfo->ulPacketCnt * pNIConnInfo->ulPacketSize );

  if ( NO_ERROR_EO001 == rc)
  {
    char *Ptr ;
    /* Saprouter string is the original pointer given as a DbNode from the application */
    /* if the NI-connect before was successfull, we have to save the saprouter string  */
    /* because we need it for canceling (or sqladump) the connection                   */
    rc = ALLOC_MEM_EO57 ( (void **)&Ptr, (tsp00_Uint)strlen ( pNIConnInfo->szSapRouterString ) + 1);
    if ( NO_ERROR_EO001 == rc )
    {
      strcpy ( Ptr, pNIConnInfo->szSapRouterString ) ;
      pNIConnInfo->szSapRouterString = Ptr ;
    }
    else
    {
      if ( pCommPackets != NULL )
        FREE_MEM_EO57 ( pCommPackets );
    }
  }

  if ( rc != NO_ERROR_EO001 )
    {
    eo46BuildErrorStringRC ( pErrText, (char *)ERRMSG_ALLOC_MEMORY, rc );
    CommState = commErrNotOk_esp01;
    }
  else
    {
      tsp00_Uint               ulCnt;

      for ( ulCnt = 0; ulCnt < pNIConnInfo->ulPacketCnt; ulCnt++ )
      {
        pNIConnInfo->pCommPacketList[ulCnt] = (teo003_CommPacketRecord *)pCommPackets;
        pConnParam->pCommPacketList[ulCnt]  = (teo003_CommPacketRecord *)pCommPackets;
        pCommPackets += pNIConnInfo->ulPacketSize;
      }
    }
  
  if ( CommState == commErrOk_esp01 )
  {
    pConnParam->ulPacketSize      = pNIConnInfo->ulPacketSize;
    pConnParam->ulMaxDataLen      = pNIConnInfo->ulMaxDataLen;
    pConnParam->ulMinReplySize    = pNIConnInfo->ulMinReplySize;
    pConnParam->ulServerRef       = pNIConnInfo->ulServerRef;

    strncpy ( pConnParam->authenticationAllow, 
              pNIConnInfo->authenticationAllow, 
              sizeof(pConnParam->authenticationAllow) - 1 );
    pConnParam->authenticationAllow[sizeof(pConnParam->authenticationAllow) - 1] = '\0';
  }

  return( CommState ) ;
}

/*------------------------------*/

/* PTS 1105116 */
static tsp00_Uint     
eo03_GetServicePort ( SAPDB_Bool    isSSL,  /* SSL connection */
                      SAPDB_Bool    isNISSL,/* SSL connection with SAP Router String */
                      char          *pHost, /* may be changed for SSL connection without (!) SAP Router String if a port is given */
                      tsp00_Uint2   *pServicePort,
                      tsp00_ErrTextc VAR_ARRAY_REF  pErrText)
{
    tsp00_Uint                      CommState = commErrOk_esp01;
    SAPDB_Char *                    defaultService;

    *pServicePort = 0;

    if (isSSL)
    {
        defaultService = SOCKET_SERVICE_NISSL76_EO003;
    }
    else
    {
        defaultService = SOCKET_SERVICE_NI72_EO003;
    }

    if (!isSSL || isNISSL) 
    {/* SAP Router String either with SSL or not */
        eo40NiExtractServiceFromSaprouterSring (pHost, pServicePort );

        if ( *pServicePort == 0 )
        {
            if ( sql43_get_service_by_name  ( (char *)defaultService, pServicePort ) != NO_ERROR_EO001 )
            {
                CommState = commErrNotOk_esp01 ;
                strcpy( pErrText, ERRMSG_SERV_BY_NAME_ERROR);
                MSGD (( ERR_GET_SERV_BY_NAME, defaultService ));
            }
        }
    }
    else 
    {/* SSL connection without SAP Router String */
#if defined (_WIN32)
        CommState = sql23c_GetServicePort (pHost,
                                           pServicePort,
                                           defaultService,
                                           pErrText);
#else
        CommState = sql42_GetServicePort (pHost,
                                          pServicePort,
                                          defaultService,
                                          pErrText);
#endif
    }

    return ( CommState ) ;
}

/*------------------------------*/

static tsp00_Uint     
eo03_InitNIConnInfo ( teo003_ConnectParamRecord    *pConnParam,
                      teo40_NiConnectInfoRecord    *pNIConnInfo,
                      tsp00_ErrTextc VAR_ARRAY_REF  pErrText)
{

    tsp00_Uint                      CommState = commErrOk_esp01;

    pNIConnInfo->pidClientPID               = pConnParam->pidClientPID;
    pNIConnInfo->ulServerRef                = (tsp00_Uint    )undef_ceo00;
    pNIConnInfo->ulClientRef                = pConnParam->ulClientRef;
    pNIConnInfo->ulServiceType              = pConnParam->ulServiceType;

    pNIConnInfo->ulCurrentPacket            = (tsp00_Uint    )undef_ceo00;
    pNIConnInfo->ulCmdDataLen               = (tsp00_Uint    )undef_ceo00;
    pNIConnInfo->ulPacketCnt                = pConnParam->ulPacketCnt;

    pNIConnInfo->ulPacketSize               = pConnParam->ulPacketSize;
    pNIConnInfo->ulMaxDataLen               = pConnParam->ulMaxDataLen;
    pNIConnInfo->ulMinReplySize             = (tsp00_Uint    )undef_ceo00;
    pNIConnInfo->ulMaxSegmentSize           = (tsp00_Uint    )undef_ceo00;
    pNIConnInfo->fAcknowledge               = pConnParam->fAcknowledge;

    pNIConnInfo->pszServerDBRoot            = pConnParam->pszServerDBRoot ;
    pNIConnInfo->pszServerPgm               = pConnParam->pszServerPgm ;

    pNIConnInfo->isSSL                      = pConnParam->isSSL;
    pNIConnInfo->isSSLoverSAPRouterString   = pConnParam->isSSLoverSAPRouterString;    
    pNIConnInfo->pSSLHandle                 = NULL;
    pNIConnInfo->ignoreHostnameInServerCert = pConnParam-> ignoreHostnameInServerCert;
    
    if (pConnParam->pszServerNode)
    {   /* Seperate SSL or NI/NISSL */
        pNIConnInfo->szSapRouterString = pConnParam->pszServerNode  ; /* normal hostname is also taken as SAPRouter string */
        if (pNIConnInfo->isSSL && !pNIConnInfo->isSSLoverSAPRouterString)
        {
            /* Servernode may still contain the port (...:<port>); see below */
            strcpy ( pNIConnInfo->szServerNode, pConnParam->pszServerNode );
        }
        else /* NI or NISSL */
        {
            eo40NiExtractDBNodeFromSaprouterSring  ( pNIConnInfo->szSapRouterString, pNIConnInfo->szServerNode );
        }
    }
    else
    {
        pNIConnInfo->szSapRouterString  = NULL;
        pNIConnInfo->szServerNode[0]    = '\0';
    }

    strcpy ( pNIConnInfo->szServerDB,   pConnParam->pszServerDB );

    /* In case of a SSL connection without SAP Router String the pNIConnInfo->szSapRouterString */
    /* is freed from the port by the following call                                             */
    CommState = eo03_GetServicePort(pNIConnInfo->isSSL,
        pNIConnInfo->isSSLoverSAPRouterString,
        pNIConnInfo->szSapRouterString, 
        &pNIConnInfo->usServicePort, 
        pErrText);
 
    return ( CommState ) ;
}

static void     
eo03_NiClose (teo40_NiConnectInfoRecord    *pNIConnInfo)
{
    RTESec_SAPSSLSessionClose (pNIConnInfo->pSSLHandle);
    eo40NiClose (&pNIConnInfo->NiHandle);
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
