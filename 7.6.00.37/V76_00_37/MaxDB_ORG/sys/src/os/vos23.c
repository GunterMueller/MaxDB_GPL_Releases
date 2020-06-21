/*!
  @file           vos23.c
  @author         RaymondR
  @brief          TCP/IP connectivity
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*
 * INCLUDE FILES
 */
#define RTE_USE_SOCK

#include "gos00.h"
#include "heo46.h"
#include "gsp09.h"
#include "heo420.h"
#include "gos003.h"
#include "hos23.h"
#include "hos42.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo43_0.h"

#if defined(_WIN32)
 #include          <winerror.h>
#else
 #include          <nerrno.h>
#endif


/*
 *  DEFINES
 */
#define MOD__  "VOS23C : "
#define MF__   MOD__"UNDEFINED"

#define LINGER_TIME  15

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */
ULONG sql23c_connect_server  ( PTCPIP_CONNECT_INFO_REC pTCPIPConnInfo,
                               tsp00_ErrTextc          pErrText );

ULONG sql23c_get_server_addr ( PTCPIP_CONNECT_INFO_REC pTCPIPConnInfo,
                               tsp00_ErrTextc          pErrText );

ULONG sql23c_info_request    ( PTCPIP_CONNECT_INFO_REC pTCPIPConnInfo,
                               tsp00_ErrTextc          pErrText );

VOID sql23c_init_conn_info   ( PTCPIP_CONNECT_INFO_REC pTCPIPConnInfo,
                               PCONNECT_PARAM_REC      pConnParam );

static VOID os23InitRteConnectParam ( PTCPIP_CONNECT_INFO_REC   pTCPIPConnInfo,
                                      teo003_ConPktParamRecord *pConPktParamRec ,
                                      ULONG                     ulMessClass );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

/* PTS 1105295 */
/*------------------------------*/
tsp01_CommErr_Enum 
os23XServerPing ( char                  *pszServerNode,     /* '\0' terminated node or saprouterstring */
                  tsp00_Versionc         szServerVersion,   /* '\0' terminated version from answering server */
                  SAPDB_Byte *           pServerCertificate,
                  SAPDB_UInt4            lenCertificate,
                  SAPDB_Bool             captureServerCert,  
                  tsp00_ErrTextc         pErrText)          /* '\0' terminated error string   */
{
#undef  MF__
#define MF__ MOD__"os23NiPing"

    TCPIP_CONNECT_INFO_REC          TmpTCPIPConnInfo;
    teo003_ConPktParamRecord        ConPktParamRec;
    tsp01_CommErr_Enum              ulCommState ;

    DBGIN;

    //
    // ---  get address of the server socket
    //
    strcpy ( TmpTCPIPConnInfo.szServerNode, pszServerNode );
    ulCommState =  sql23c_get_server_addr ( &TmpTCPIPConnInfo, pErrText );

    if ( ulCommState != commErrOk_esp01 )
        return ( ulCommState );
    //
    // --- connect to server socket
    //

    TmpTCPIPConnInfo.sd               = UNDEF;
    TmpTCPIPConnInfo.ulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
        sizeof(RTE_CONNECT_PACKET_REC);
    TmpTCPIPConnInfo.ulMinSegmentSize = sizeof(RTE_HEADER_REC);

    ulCommState = sql23c_connect_server ( &TmpTCPIPConnInfo, pErrText );

    if ( ulCommState != commErrOk_esp01 )
        return ( ulCommState );

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

    ulCommState = eo420SendConnectPacket( (void *) &TmpTCPIPConnInfo.sd,
            os42SocketSend,
            &ConPktParamRec, pErrText );


    if ( ulCommState != commErrOk_esp01 )
    {
        sql40c_socket_close ( TmpTCPIPConnInfo.sd );

        DBGOUT;
        return ( ulCommState );
    }

  //
  // ---  receive server reply
    //
    if (captureServerCert)
    {
        ulCommState = eo420ReceiveCertificatePacket ((void *)&TmpTCPIPConnInfo.sd, 
                                                     os42SocketReceive,
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

        ulCommState = eo420ReceiveConnectPacket ((void *)&TmpTCPIPConnInfo.sd, 
                                                 os42SocketReceive,
                                                 &ConPktParamRec, 
                                                 pErrText );

        if ( ulCommState == commErrOk_esp01 )
        {
            if ( ConPktParamRec.ulMessClass != RSQL_PING_REPLY_EO003 )
            {
                strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
                ulCommState = commErrNotOk_esp01 ;
                MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
            }
            else
            {
                SAPDB_memcpy (szServerVersion, ConPktParamRec.szVersion, sizeof(tsp00_Versionc) -1 ) ;
                szServerVersion[sizeof(tsp00_Versionc) -1] = '\0' ;
            }
        }
    }

    sql40c_socket_close ( TmpTCPIPConnInfo.sd );

    DBGOUT;
    return ( ulCommState );
}

#ifdef SERVER
/* PTS 1111208 */
/*------------------------------*/
tsp01_CommErr_Enum 
os23NewDebugLevel ( char                  *pszServerNode,     /* '\0' terminated node or saprouterstring */
				    int                    newDebugLevel,     /* new debug level for x_server */
                    tsp00_ErrTextc         pErrText)          /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"os23NewDebugLevel"

  TCPIP_CONNECT_INFO_REC          TmpTCPIPConnInfo;
  teo003_ConPktParamRecord        ConPktParamRec;
  tsp01_CommErr_Enum              ulCommState ;

  DBGIN;

  //
  // ---  get address of the server socket
  //
  strcpy ( TmpTCPIPConnInfo.szServerNode, pszServerNode );
  ulCommState =  sql23c_get_server_addr ( &TmpTCPIPConnInfo, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    return ( ulCommState );
  //
  // --- connect to server socket
  //

  TmpTCPIPConnInfo.sd               = UNDEF;
  TmpTCPIPConnInfo.ulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
                                      sizeof(RTE_CONNECT_PACKET_REC);
  TmpTCPIPConnInfo.ulMinSegmentSize = sizeof(RTE_HEADER_REC);

  ulCommState = sql23c_connect_server ( &TmpTCPIPConnInfo, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    return ( ulCommState );

  memset(&ConPktParamRec, 0, sizeof(teo003_ConPktParamRecord) ) ;
  ConPktParamRec.ulSenderRef = newDebugLevel;

  // to prevent a 'denial of service attack' by setting debug level.... it is only settable via registry
  {
      DWORD         dwDebugLevel = (DWORD)newDebugLevel;
      REG_ENTRY_REC RegistryEntries[1];
      PATHNAME      szKey;
      LONG          rc;

      RegistryEntries[0].pszValueName = REG_VN_DEBUG_LEVEL;
      RegistryEntries[0].pValue       = &dwDebugLevel;
      RegistryEntries[0].ulValueSize  = sizeof(dwDebugLevel);
      RegistryEntries[0].ulValueType  = REG_DWORD;

      strcpy ( szKey, XSERV_TITLE "\\" REG_SK_SERVICE_PARAM );

      rc = sql50_reg_put_service_values ( NULL, szKey, 1, RegistryEntries );
      if ( rc != NO_ERROR )
      {
          strcpy(pErrText, "Cant update registry");
          return ( commErrNotOk_esp01 );
      }
  }
  /* send NEW_DEBUG_LEVEL request */
  ConPktParamRec.ulMessClass         = RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003;

  ulCommState = eo420SendConnectPacket( (void *) &TmpTCPIPConnInfo.sd,
                                         os42SocketSend,
                                        &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( TmpTCPIPConnInfo.sd );

    DBGOUT;
    return ( ulCommState );
    }

  //
  // ---  receive server reply
  //
  ulCommState = eo420ReceiveConnectPacket ( (void *) &TmpTCPIPConnInfo.sd, 
                                            os42SocketReceive,
                                            &ConPktParamRec, pErrText );

  if ( ulCommState == commErrOk_esp01 )
    if ( ConPktParamRec.ulMessClass != RSQL_NEW_DEBUG_LEVEL_REPLY_EO003 )
    {
      strcpy(pErrText, ERRMSG_PROTOCOL_ERROR);
      ulCommState = commErrNotOk_esp01 ;
      MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
    }

  sql40c_socket_close ( TmpTCPIPConnInfo.sd );

  DBGOUT;
  return ( ulCommState );
}
#endif /* SERVER */

/*------------------------------*/

ULONG sql23c_connect ( PCONNECT_PARAM_REC       pConnParam,
                       PTCPIP_CONNECT_INFO_REC  pTCPIPConnInfo,
                       tsp00_ErrTextc           pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_connect"
  LONG                            rc = NO_ERROR;
  teo003_ConPktParamRecord        ConPktParamRec;
  ULONG                           ulCommState;
  ULONG                           ulCnt;
  PCHAR                           pCommPackets;

  DBGIN;

  // --- initialize the connect information record
  sql23c_init_conn_info ( pTCPIPConnInfo, pConnParam );

  //
  // ---  get address of the server socket
  //
  rc =  sql23c_get_server_addr ( pTCPIPConnInfo, pErrText );

  if ( rc != NO_ERROR )
  {
    return ( commErrNotOk_esp01 );
  }

  // --- Get a socket address for the requested session, and the
  //     packet sizes!
  //
  if (pConnParam->ulServiceType != srvControl_esp01)
    {
    ulCommState = sql23c_info_request ( pTCPIPConnInfo, pErrText );

    if ( ulCommState != commErrOk_esp01 )
      return ( ulCommState );
    }
  else
    {
    // packet sizes are fixed, no info request necessary
    pTCPIPConnInfo->ulPacketSize   = csp9_ctrl_packet_size;
    pTCPIPConnInfo->ulMinReplySize = ALIGN( 100, ALIGNMENT_VALUE );
    pTCPIPConnInfo->ulMaxDataLen   = pTCPIPConnInfo->ulPacketSize
                                     - (2 * RTE_HEADER_SIZE);
    }

  //
  // ---  connect to server socket
  //
  pTCPIPConnInfo->ulMaxSegmentSize = pTCPIPConnInfo->ulPacketSize;
  pTCPIPConnInfo->ulMinSegmentSize = 1024;

  ulCommState = sql23c_connect_server ( pTCPIPConnInfo, pErrText );

  if ( ulCommState  != commErrOk_esp01 )
  {
    return ( ulCommState );
  }
  /*
   *  send connect packet
   */
  os23InitRteConnectParam ( pTCPIPConnInfo, &ConPktParamRec,
                            pTCPIPConnInfo->ulServiceType == srvControl_esp01 
                            ? RSQL_CTRL_CONN_REQUEST_EO003 : RSQL_USER_CONN_REQUEST_EO003 ) ;

  if (ConPktParamRec.ulServiceType == srvControl_esp01)
    {
    if (pConnParam->pszServerPgm != NULL) 
      {
      strcpy ( ConPktParamRec.szServerPgm, pConnParam->pszServerPgm);
      }
    if (pConnParam->pszServerDBRoot != NULL) 
      {
      strcpy ( ConPktParamRec.szServerDBRoot, pConnParam->pszServerDBRoot);
      }
    }

  ulCommState = eo420SendConnectPacket( (void *) &pTCPIPConnInfo->sd,
                                         os42SocketSend,
                                        &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( ulCommState );
    }

  //
  // ---  receive server reply
  //
  ulCommState = eo420ReceiveConnectPacket ( (void *) &pTCPIPConnInfo->sd, 
                                            os42SocketReceive,
                                            &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( ulCommState );
    }

  if ( ConPktParamRec.ulMaxSegmentSize > pTCPIPConnInfo->ulMaxSegmentSize )
    {
    MSGD (( ERR_INCORRECT_MAXSEGMENTSIZE, ConPktParamRec.ulMaxSegmentSize ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_ILL_MAXSEGMENTSIZE, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  pTCPIPConnInfo->ulMaxSegmentSize = ConPktParamRec.ulMaxSegmentSize;
  pTCPIPConnInfo->ulServerRef      = ConPktParamRec.ulSenderRef;
  pTCPIPConnInfo->ulSwapType       = ConPktParamRec.ulSwapType;

  rc = ALLOC_MEM( (PPVOID)&pCommPackets,
                  pTCPIPConnInfo->ulPacketCnt * pTCPIPConnInfo->ulPacketSize );

  if ( rc != NO_ERROR )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    eo46BuildErrorStringRC ( pErrText, ERRMSG_ALLOC_MEMORY, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // --- set output parameter
  //
  pConnParam->ulPacketSize      = pTCPIPConnInfo->ulPacketSize;
  pConnParam->ulMaxDataLen      = pTCPIPConnInfo->ulMaxDataLen;
  pConnParam->ulMinReplySize    = pTCPIPConnInfo->ulMinReplySize;
  pConnParam->ulServerRef       = pTCPIPConnInfo->ulServerRef;

  strncpy ( pConnParam->authenticationAllow, 
            ConPktParamRec.authenticationAllow, 
            sizeof(pConnParam->authenticationAllow) - 1 );
  pConnParam->authenticationAllow[sizeof(pConnParam->authenticationAllow) - 1] = '\0';


  for ( ulCnt = 0; ulCnt < pTCPIPConnInfo->ulPacketCnt; ulCnt++ )
    {
    pTCPIPConnInfo->pCommPacketList[ulCnt] = (PCOMM_PACKET_REC)pCommPackets;
    pConnParam->pCommPacketList[ulCnt]     = (PCOMM_PACKET_REC)pCommPackets;

    pCommPackets += pTCPIPConnInfo->ulPacketSize;
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

ULONG sql23c_release ( PTCPIP_CONNECT_INFO_REC    pTCPIPConnInfo,
                       tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_release"
  APIRET rc;
  ULONG  ulMessClass;

  DBGPAS;

  if ( pTCPIPConnInfo->sd != UNDEF )
    {
    switch ( pTCPIPConnInfo->ulServiceType )
      {
      case srvControl_esp01 :
        ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;
        break;

      default:
        ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;
        break;
      }

    eo420SendReleasePacket ((void *)&pTCPIPConnInfo->sd, os42SocketSend, 
                             pTCPIPConnInfo->ulClientRef,
                             pTCPIPConnInfo->ulServerRef,
                             ulMessClass,
                             commErrOk_esp01 );

    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;
    }

  if ( pTCPIPConnInfo->pCommPacketList[0] != NULL ) 
    {
    rc = FREE_MEM ( pTCPIPConnInfo->pCommPacketList[0] );

    if ( rc != NO_ERROR )
      {
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_FREE_MEM, rc );
      return ( commErrNotOk_esp01 );
      }

    pTCPIPConnInfo->pCommPacketList[0] = NULL;
    }

  return ( commErrOk_esp01 );
  }

/*------------------------------*/

ULONG sql23c_request ( PTCPIP_CONNECT_INFO_REC    pTCPIPConnInfo,
                       ULONG                      ulCmdPacket,
                       ULONG                      ulCmdDataLen,
                       tsp00_Uint1                RTEFlags,
                       tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_request"
  ULONG                           ulCommState;
  PCOMM_PACKET_REC                pCommPacket = NULL;
  ULONG                           ulMessClass;

  DBGIN;

  ulMessClass = RSQL_USER_DATA_REQUEST_EO003;

  pCommPacket                     = pTCPIPConnInfo->pCommPacketList[ulCmdPacket];
  pTCPIPConnInfo->ulCurrentPacket = ulCmdPacket;
  pTCPIPConnInfo->ulCmdDataLen    = ulCmdDataLen;

  ulCommState = eo420SendCommPacket( (void *)&pTCPIPConnInfo->sd, os42SocketSend,
                                     pTCPIPConnInfo->ulMaxSegmentSize,
                                     pCommPacket,
                                     ulCmdDataLen,
                                     pTCPIPConnInfo->ulClientRef,
                                     pTCPIPConnInfo->ulServerRef,
                                     ulMessClass,
                                     commErrOk_esp01,
                                     RTEFlags,
                                     pErrText );

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

ULONG sql23c_replyavailable ( PTCPIP_CONNECT_INFO_REC    pTCPIPConnInfo,
                              tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_replyavailable"
  fd_set                         SDMask;
  APIRET                         rc   = NO_ERROR;
  INT                            SDCount;

  DBGIN;

  FD_ZERO ( &SDMask );
  FD_SET  ( (teo00_Socket)pTCPIPConnInfo->sd, &SDMask );

  for ( ;; )
    {
    SDCount = (INT)pTCPIPConnInfo->sd + 1;

    rc = sql40c_select ( &SDCount, (PVOID)&SDMask,
                         NULL, NULL, 0 );

    if ( rc == ERROR_TIMEOUT )
      {
      DBGOUT;
      return ( commErrWouldBlock_esp01 );
      }
    else if ( rc == SOCEINTR )
      {
      continue;
      }
    else if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SOCKET_SELECT_ERR, rc ));
      eo46BuildErrorStringRC ( pErrText,
                                  ERRMSG_COM_SOCKET_SELECT_ERROR, rc );
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    break;
    }


  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

ULONG sql23c_receive ( PTCPIP_CONNECT_INFO_REC    pTCPIPConnInfo,
                       PCOMM_PACKET_REC           *ppResPacket,
                       PULONG                     pulResDataLen,
                       tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_receive"
  ULONG                          ulCommState;
  PCOMM_PACKET_REC               pCmdCommPacket;
  ULONG                          ulCurrentPacket;
  ULONG                          ulSenderRef;
  ULONG                          ulReceiverRef;
  ULONG                          ulMessClass;
  tsp00_Uint1                    RTEFlags;
  ULONG                          maxReceiveLength;

  DBGIN;

  ulCurrentPacket = pTCPIPConnInfo->ulCurrentPacket;
  pCmdCommPacket  = pTCPIPConnInfo->pCommPacketList[ulCurrentPacket];
  if (0 == pTCPIPConnInfo->ulMinReplySize)
  {
    *ppResPacket      = pCmdCommPacket;
    maxReceiveLength  = pTCPIPConnInfo->ulMaxDataLen;
  }
  else
  {
    *ppResPacket     = (PCOMM_PACKET_REC) ((PCHAR)pCmdCommPacket->pDataPart +
                                           pTCPIPConnInfo->ulCmdDataLen);
    maxReceiveLength = pTCPIPConnInfo->ulMaxDataLen - pTCPIPConnInfo->ulCmdDataLen;
  }

  ulCommState = eo420ReceiveCommPacket ( (void *)&pTCPIPConnInfo->sd, 
                                         os42SocketReceive, 
                                         pTCPIPConnInfo->ulSwapType,
                                        *ppResPacket,
                                         maxReceiveLength,
                                         pulResDataLen,
                                        &ulSenderRef,
                                        &ulReceiverRef,
                                        &ulMessClass,
                                        &RTEFlags,
                                         pErrText );

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/


ULONG sql23c_cancel_dump ( PTCPIP_CONNECT_INFO_REC    pTCPIPConnInfo,
                           ULONG                      ulReqType,
                           tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_cancel_dump"
  TCPIP_CONNECT_INFO_REC          TmpTCPIPConnInfo;
  teo003_ConPktParamRecord        ConPktParamRec;
  ULONG                           ulMessClass;
  ULONG                           ulCommState;

  DBGIN;

  TmpTCPIPConnInfo = *pTCPIPConnInfo;

   switch ( ulReqType )
     {
     case SQL_RTE_CANCEL:
       if (pTCPIPConnInfo->ulServiceType == srvControl_esp01)
         ulMessClass = RSQL_CTRL_CANCEL_REQUEST_EO003;
       else
         ulMessClass = RSQL_USER_CANCEL_REQUEST_EO003;
       break;

     case SQL_RTE_DUMP:
       ulMessClass = RSQL_DUMP_REQUEST_EO003;
       break;

     default:
       MSGD (( ERR_PROTOCOL_ERROR, "REQUEST TYPE" ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );
       return ( commErrNotOk_esp01 );
     }


  //
  // ---  get address of the server socket
  //
  ulCommState =  sql23c_get_server_addr ( &TmpTCPIPConnInfo, pErrText );

  if ( ulCommState != commErrOk_esp01 )
  {
    return ( ulCommState );
  }
  //
  // --- connect to server socket
  //
  TmpTCPIPConnInfo.sd               = UNDEF;
  TmpTCPIPConnInfo.ulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
                                      sizeof(RTE_CONNECT_PACKET_REC);
  TmpTCPIPConnInfo.ulMinSegmentSize = sizeof(RTE_HEADER_REC);

  ulCommState = sql23c_connect_server ( &TmpTCPIPConnInfo, pErrText );

  if ( ulCommState != commErrOk_esp01 )
  {
    return ( ulCommState );
  }

  //
  // --- send cancel request
  //

  os23InitRteConnectParam ( &TmpTCPIPConnInfo, &ConPktParamRec, ulMessClass ) ;
  ConPktParamRec.ulReceiverRef       = TmpTCPIPConnInfo.ulServerRef;

  ulCommState = eo420SendConnectPacket( (void *) &TmpTCPIPConnInfo.sd,
                                         os42SocketSend,
                                        &ConPktParamRec, pErrText );

  sql40c_socket_close ( TmpTCPIPConnInfo.sd );

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

ULONG sql23c_GetServicePort (char *         pszServerNode,
                            tsp00_Uint2 *   pusServicePort,
                            SAPDB_Char *    defaultService,
                            tsp00_ErrTextc  pErrText)
{
#undef  MF__
#define MF__ MOD__"sql23c_GetServicePort"
    LONG                            rc;
    char                           *ServiceString = NULL ;
    char                           *pServiceInfo = NULL;


    DBGIN;

    pServiceInfo = strchr(pszServerNode, ':');

    if ( NULL != pServiceInfo )
    {
        
        if ( pServiceInfo[1] )
        {
            switch( pServiceInfo[1] )
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                *pusServicePort = atoi(&pServiceInfo[1]);
                ServiceString = 0;
                break;
            default:
                ServiceString = &pServiceInfo[1];
                break;
            }
        }
        else
        {
            ServiceString = pServiceInfo;
        }

        //port is removed from pszServerNode 
        memset (pServiceInfo, 0, 1);
    }
    else
    {
        ServiceString = defaultService;
    }

    if ( NULL != ServiceString )
    {
        rc = sql43_get_service_by_name (ServiceString,
                                        pusServicePort);

        if ( rc != NO_ERROR )
        {
            strcpy  ( pErrText, ERRMSG_MISSING_TCP_SERVICE_ENTRY );
            strncat ( pErrText, ServiceString,
                sizeof(tsp00_ErrTextc) -
                strlen(pErrText) - 1 );
            pErrText[strlen(pErrText)] = '\0';

            MSGD (( ERR_SERV_BY_NAME_ERROR, ServiceString ));

            DBGOUT;
            return commErrNotOk_esp01;
        }
    }

    DBGOUT;
    return commErrOk_esp01;
}


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static ULONG sql23c_info_request ( PTCPIP_CONNECT_INFO_REC  pTCPIPConnInfo,
                                   tsp00_ErrTextc           pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_info_request"
  ULONG                           ulCommState;
  teo003_ConPktParamRecord        ConPktParamRec;
  ULONG                           ulMyHeaderSize;
  ULONG                           ulPeerHeaderSize;

  DBGIN;

  //
  // ---  connect to server socket
  //
  pTCPIPConnInfo->ulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
                                     sizeof(RTE_CONNECT_PACKET_REC);
  pTCPIPConnInfo->ulMinSegmentSize = sizeof(RTE_HEADER_REC);

  ulCommState = sql23c_connect_server ( pTCPIPConnInfo, pErrText );

  if ( ulCommState != commErrOk_esp01 )
  {
    return ( ulCommState );
  }
  /*
   *  send connect packet
   */
  os23InitRteConnectParam ( pTCPIPConnInfo, &ConPktParamRec, RSQL_INFO_REQUEST_EO003 ) ;

  ulCommState = eo420SendConnectPacket( (void *) &pTCPIPConnInfo->sd,
                                         os42SocketSend,
                                        &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( ulCommState );
    }

  //
  // ---  receive server reply
  //
  ulCommState = eo420ReceiveConnectPacket ( (void *) &pTCPIPConnInfo->sd, 
                                            os42SocketReceive,
                                            &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( ulCommState );
    }

  sql40c_socket_close ( pTCPIPConnInfo->sd );
  pTCPIPConnInfo->sd = UNDEF;

  if ( ConPktParamRec.ulMessClass != RSQL_INFO_REPLY_EO003 )
    {
    MSGD (( ERR_WRONG_MESSCLASS_REPL, ConPktParamRec.ulMessClass ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( ConPktParamRec.ulServiceType != pTCPIPConnInfo->ulServiceType )
    {
    MSGD (( ERR_PROTOCOL_ERROR, "SERVICE" ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( strcmp ( pTCPIPConnInfo->szServerDB,
                ConPktParamRec.szSenderServerDB ) )
    {
    MSGD (( ERR_PROTOCOL_ERROR, "SERVERDB" ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  ulPeerHeaderSize = ConPktParamRec.ulPacketSize -
                     ConPktParamRec.ulMaxDataLen;
  ulMyHeaderSize   = (sizeof(RTE_HEADER_REC) * 2);

  if ( ulPeerHeaderSize < ulMyHeaderSize )
    {
    ConPktParamRec.ulMaxDataLen -= ulMyHeaderSize - ulPeerHeaderSize;
    }

  if ( ConPktParamRec.usServicePort != 0 )
    pTCPIPConnInfo->usServicePort = ConPktParamRec.usServicePort;

  pTCPIPConnInfo->ulPacketSize    = ConPktParamRec.ulPacketSize;
  pTCPIPConnInfo->ulMaxDataLen    = ConPktParamRec.ulMaxDataLen;
  pTCPIPConnInfo->ulMinReplySize  = ConPktParamRec.ulMinReplySize;

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG sql23c_connect_server ( PTCPIP_CONNECT_INFO_REC  pTCPIPConnInfo,
                                     tsp00_ErrTextc           pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_connect_server"
  LONG                            rc;

  DBGIN;

  //
  // --- create active socket to connect to server
  //
  rc = sql40c_stream_socket ( &pTCPIPConnInfo->sd );

  if ( rc != NO_ERROR )
    {
    MSGD (( ERR_CANT_CREATE_SOCKET, rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_CANT_CREATE_SOCKET, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }


  //
  // --- set send buffer size
  //
  rc = sql40c_set_send_buffer_size ( pTCPIPConnInfo->sd,
                                     &pTCPIPConnInfo->ulMaxSegmentSize,
                                     pTCPIPConnInfo->ulMinSegmentSize );

  if ( rc != NO_ERROR )
    {
    if ( rc == SOCENOBUFS )
      {
      MSGD (( ERR_OUT_OF_SOCK_BUF_MEM, "SNDBUF" ));
      strcpy( pErrText, ERRMSG_OUT_OF_SOCK_BUF_MEM );
      }
    else
      {
      MSGD (( ERR_CANT_SET_BUF_SIZE, "SNDBUF", rc ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_CANT_SET_BUF_SIZE, rc );
      }

    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- set receive buffer size
  //
  rc = sql40c_set_recv_buffer_size ( pTCPIPConnInfo->sd,
                                     &pTCPIPConnInfo->ulMaxSegmentSize,
                                     pTCPIPConnInfo->ulMinSegmentSize );

  if ( rc != NO_ERROR )
    {
    if ( rc == SOCENOBUFS )
      {
      MSGD (( ERR_OUT_OF_SOCK_BUF_MEM, "RCVBUF" ));
      strcpy( pErrText, ERRMSG_OUT_OF_SOCK_BUF_MEM );
      }
    else
      {
      MSGD (( ERR_CANT_SET_BUF_SIZE, "RCVBUF", rc ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_CANT_SET_BUF_SIZE, rc );
      }

    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- Connect to server socket.
  //
  rc = sql40c_connect ( pTCPIPConnInfo->sd,
                        pTCPIPConnInfo->usServicePort,
                        pTCPIPConnInfo->ucName,
                        sizeof (pTCPIPConnInfo->ucName));

  if ( rc != NO_ERROR )
    {
    sql40c_socket_close ( pTCPIPConnInfo->sd );
    pTCPIPConnInfo->sd = UNDEF;

    if ( rc == SOCECONNREFUSED )
      {
      MSGD (( ERR_XSERVER_NOT_ACTIVE ));
      strcpy ( pErrText, ERRMSG_XSERVER_NOT_ACTIVE );
      DBGOUT;
      return ( commErrStartRequired_esp01 );
      }
    else
      {
      MSGD (( ERR_REQ_SOCKET_CONNECT, rc ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_REQ_SOCKET_CONNECT, rc );
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  rc =  sql40c_keep_alive ( pTCPIPConnInfo->sd );

  if ( rc != NO_ERROR )
    {
    MSGD (( ERR_KEEP_ALIVE, rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_KEEP_ALIVE, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  rc =  sql40c_linger ( pTCPIPConnInfo->sd, TRUE, LINGER_TIME );

  if ( rc != NO_ERROR )
    {
    MSGCD (( ERR_LINGER, rc ))
    eo46BuildErrorStringRC ( pErrText, ERRMSG_LINGER, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }


  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/
static VOID os23InitRteConnectParam ( PTCPIP_CONNECT_INFO_REC   pTCPIPConnInfo,
                                      teo003_ConPktParamRecord *pConPktParamRec ,
                                      ULONG                     ulMessClass )
  {
  #undef  MF__
  #define MF__ MOD__"os23InitRteConnectParam"
  DBGPAS;

  eo420InitConPktParamRec ( pConPktParamRec );
  pConPktParamRec->ulMessClass         = ulMessClass;
  pConPktParamRec->ulSenderRef         = pTCPIPConnInfo->ulClientRef;
  pConPktParamRec->ulServiceType       = pTCPIPConnInfo->ulServiceType;
  pConPktParamRec->ulMaxSegmentSize    = pTCPIPConnInfo->ulMaxSegmentSize;
  pConPktParamRec->ulPacketSize        = pTCPIPConnInfo->ulPacketSize;
  pConPktParamRec->ulMaxDataLen        = pTCPIPConnInfo->ulMaxDataLen;
  pConPktParamRec->ulMinReplySize      = pTCPIPConnInfo->ulMinReplySize;
  pConPktParamRec->pidSenderPID        = pTCPIPConnInfo->pidClientPID;
  pConPktParamRec->usServicePort       = pTCPIPConnInfo->usServicePort;
  pConPktParamRec->fOmitReplyPart      = true; 
  strcpy ( pConPktParamRec->szReceiverServerDB, pTCPIPConnInfo->szServerDB );

  return;
  }

/*------------------------------*/

static ULONG sql23c_get_server_addr ( PTCPIP_CONNECT_INFO_REC  pTCPIPConnInfo,
                                      tsp00_ErrTextc           pErrText )
{
#undef  MF__
#define MF__ MOD__"sql23c_get_server_addr"
    LONG                            rc;
    tsp00_NodeIdc                   szServerNode;
    tsp00_Uint                      commState = commErrOk_esp01;

    DBGIN;

    
    strcpy (szServerNode, pTCPIPConnInfo->szServerNode);

    commState = sql23c_GetServicePort (szServerNode,
                                       &pTCPIPConnInfo->usServicePort,
                                       SOCKET_SERVICE_SQL6_EO003,
                                       pErrText);
    if (commErrOk_esp01 == commState)
    {
        memset ( pTCPIPConnInfo->ucName, 0, sizeof ( pTCPIPConnInfo->ucName ));

        rc = sql43_get_host_by_name ( szServerNode,
            pTCPIPConnInfo->ucName,
            sizeof( pTCPIPConnInfo->ucName ));

        if ( rc != NO_ERROR )
        {
            strcpy(pErrText, ERRMSG_GET_HOST_BY_NAME_ERR);
            commState = commErrNotOk_esp01;
        }
    }

    return (commState);
}

/*------------------------------*/

static VOID sql23c_init_conn_info ( PTCPIP_CONNECT_INFO_REC  pTCPIPConnInfo,
                                    PCONNECT_PARAM_REC       pConnParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql23c_init_conn_info"
  DBGPAS;

  pTCPIPConnInfo->sd                 = UNDEF;
  pTCPIPConnInfo->SwapType           = UNDEF;
  pTCPIPConnInfo->ucName[0]          = '\0';
  pTCPIPConnInfo->usServicePort      = (USHORT)UNDEF;

  if ( pConnParam == NULL )
    {
    pTCPIPConnInfo->pidClientPID     = (PID)UNDEF;
    pTCPIPConnInfo->szServerDB[0]    = '\0';
    pTCPIPConnInfo->szServerNode[0]  = '\0';
    pTCPIPConnInfo->ulPacketCnt      = (ULONG)UNDEF;
    pTCPIPConnInfo->ulServerRef      = (ULONG)UNDEF;
    pTCPIPConnInfo->ulClientRef      = (ULONG)UNDEF;
    pTCPIPConnInfo->ulServiceType    = (ULONG)UNDEF;

    pTCPIPConnInfo->ulCurrentPacket  = (ULONG)UNDEF;
    pTCPIPConnInfo->ulCmdDataLen     = (ULONG)UNDEF;
    pTCPIPConnInfo->ulPacketCnt      = 0;

    pTCPIPConnInfo->ulPacketSize     = 0;
    pTCPIPConnInfo->ulMaxDataLen     = 0;
    pTCPIPConnInfo->ulMinReplySize   = (ULONG)UNDEF;

    pTCPIPConnInfo->ulMaxSegmentSize = (ULONG)UNDEF;
    }
  else
    {
    pTCPIPConnInfo->pidClientPID     = pConnParam->pidClientPID;
    pTCPIPConnInfo->ulServerRef      = (ULONG)UNDEF;
    pTCPIPConnInfo->ulClientRef      = pConnParam->ulClientRef;
    pTCPIPConnInfo->ulServiceType    = pConnParam->ulServiceType;

    pTCPIPConnInfo->ulCurrentPacket  = (ULONG)UNDEF;
    pTCPIPConnInfo->ulCmdDataLen     = (ULONG)UNDEF;
    pTCPIPConnInfo->ulPacketCnt      = pConnParam->ulPacketCnt;

    pTCPIPConnInfo->ulPacketSize     = pConnParam->ulPacketSize;
    pTCPIPConnInfo->ulMaxDataLen     = pConnParam->ulMaxDataLen;
    pTCPIPConnInfo->ulMinReplySize   = (ULONG)UNDEF;

    pTCPIPConnInfo->ulMaxSegmentSize = (ULONG)UNDEF;

    if ( pConnParam->pszServerNode )
      strcpy ( pTCPIPConnInfo->szServerNode, pConnParam->pszServerNode );
    else
      pTCPIPConnInfo->szServerNode[0]  = '\0';

    strcpy ( pTCPIPConnInfo->szServerDB,   pConnParam->pszServerDB );
    }

  return;
  }


/*
 * =============================== END ========================================
 */
