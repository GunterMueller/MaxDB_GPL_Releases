/*!
  @file           vos960.c
  @author         RaymondR
  @brief          Xserver support > 6.1 applications
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

#include          <winerror.h>

#include         "heo46.h"
#include	     "geo007_1.h"
#include         "hos42.h"
#include         "heo420.h"
#include	  	 "gos96.h"
#include		 "gos960.h"
#include		 "geo908.h"
#include         "hsp100.h"
#include         "RunTime/Security/RTESec_SAPSSL.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define MOD__  "VOS960.C: "
#define MF__   MOD__"UNDEFINED"

#if defined(_WIN32)
#  define DEBUGGER                       "msdev "
#endif /* _WIN32 */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef struct sockaddr          sockaddr_type;


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

extern  GLOBAL_XSERVER_REC   gxr;


/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

#if defined(_WIN32)
# if defined (DEBUGGER)
   typedef WINBASEAPI BOOL (WINAPI tIsDbgPresent)( VOID );
# endif
#endif /* _WIN32 */

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


static ULONG os960CancelDump      ( ULONG                        ulServerRef,
                                    PSZ                          pszServerDB,
                                    BOOL                         DBMConnect,
                                    ULONG                        ulReqType );
static ULONG os960Transfer        ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo );

static ULONG os960Request         ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo );

extern VOID os968UpdateDebugLevelFromRegistry( VOID );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


VOID os960Finish ( TCPIP_COORD_CTRL_REC *pTCPIPCoord )
  {
  #undef  MF__
  #define MF__ MOD__"os960Finish"
  ULONG                           ulCommState = commErrOk_esp01;
  PTCPIP_SERV_CONNECT_INFO_REC    pConnInfo;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulTmp;
  ULONG                           ulConnInfoHandle = 0;
  TID                             tidTID           = THREADID;

  DBGPAS;

  if ( tidTID != pTCPIPCoord->ThrdCtrlHeader.Tid)
    {
    KILL_THREAD ( pTCPIPCoord->ThrdCtrlHeader.Tid,
                  pTCPIPCoord->ThrdCtrlHeader.hThrd );
    }

  while ( sql09c_find_next_handle( pTCPIPCoord->pHandles, &ulConnInfoHandle ))
    {
    if ( !sql09c_lock_handle ( pTCPIPCoord->pHandles, ulConnInfoHandle ))
      continue;

    if ( sql09c_get_handle_data  ( pTCPIPCoord->pHandles,
                                   ulConnInfoHandle,
                                   (PVOID*)&pConnInfo,
                                   &ulTmp, &ulTmp ))
      {
      if ( tidTID != pConnInfo->ThrdCtrlHeader.Tid)
        {
        KILL_THREAD ( pConnInfo->ThrdCtrlHeader.Tid,
                      pConnInfo->ThrdCtrlHeader.hThrd );
        }

      if (( pConnInfo->ulConnStatus != 0               ) &&
          ( pConnInfo->ulConnStatus != CON_INITIALIZED ) &&
          ( pConnInfo->ulConnStatus != CON_CANCELLED   ) &&
          ( pConnInfo->ulConnStatus != CON_RELEASED    ))
        {
        MSGD (( INFO_XSER_CONN_KILLED,
                pConnInfo->szServerDB, pConnInfo->ulServerRef ));

        if ( pConnInfo->pDBM != NULL )
          {
          eo908_Release ( pConnInfo->pDBM );
          pConnInfo->pDBM = NULL;
          }
        else
          sql33c_release ( &pConnInfo->Shm, pErrText );

        pConnInfo->ulConnStatus = CON_RELEASED;
        }

      if ( pConnInfo->sd != INVALID_SOCKET )
        {
        sql40c_socket_close ( pConnInfo->sd );
        pConnInfo->sd = INVALID_SOCKET;
        }
      }
    }

  if (pTCPIPCoord->sd != INVALID_SOCKET)
    {
    sql40c_socket_close ( pTCPIPCoord->sd );
    pTCPIPCoord->sd = INVALID_SOCKET;
    }

  return;
  }

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

VOID _System os960TCPIP_ServerThread ( ULONG ulConnInfoHandle )
  {
  #undef  MF__
  #define MF__ MOD__"os960TCPIP_ServerThread"
  PTCPIP_SERV_CONNECT_INFO_REC   pConnInfo = NULL;
  ULONG                          ulCommState;
  HANDLE                         hServerThread;
  ULONG                          ulTmp;
  tsp00_ErrTextc                 pErrText;
  ULONG                          ulMessClass;
  LONG                           rc;

  DBGIN;

  __try
  {
  if ( !sql09c_get_handle_data  ( gxr.TCPIPCoord.pHandles,
                                  ulConnInfoHandle,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSGALL (( ERR_XSER_GET_SET_HANDLE_DATA ));
    gxr.ulCrashRc  = ERROR_INVALID_HANDLE;
    ABORT();
    return;
    }

  // --- set thread control info
  pConnInfo->ThrdCtrlHeader.ulStartTime = time (NULL);

  //
  //
  // --- set the thread priority
  //
  //
  rc = SET_THREAD_PRIO ( pConnInfo->ThrdCtrlHeader.lThrdPrio,
                         pConnInfo->ThrdCtrlHeader.Tid,
                         pConnInfo->ThrdCtrlHeader.hThrd );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_THRD_PRIO_ERROR,
             pConnInfo->ThrdCtrlHeader.Tid, rc ));
    gxr.ulCrashRc  = rc;
    ABORT();
    return;
    }

  MSG4 (( INFO_XSER_SERVER_STARTED, pConnInfo->ThrdCtrlHeader.Tid ))

  pConnInfo->ThrdCtrlHeader.ThrdState = THRD_RUNNING;

  do 
  {
    ulCommState = os960Request ( pConnInfo );
  }
  while (( ulCommState             == commErrOk_esp01 ) &&
         ( pConnInfo->ulConnStatus == CON_KEEP_ALIVE_EO003 ));

  if ( pConnInfo->ulConnStatus == CON_CONNECTED )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      if ( pConnInfo->szServerDB[0] != '\0' )
        {
        MSGD (( INFO_XSER_APPL_CONNECTED,
                pConnInfo->szSenderNodeID,
                pConnInfo->szServerDB,
                pConnInfo->ulServerRef ));
        }
      else
        {
        MSGD (( INFO_XSER_CONNECTED,
                pConnInfo->szSenderNodeID,
                pConnInfo->ulServerRef ));
        }

      MSG3 (( INFO_XSER_ENTER_DATA_TRANS,
              pConnInfo->ThrdCtrlHeader.Tid ));

      ulCommState = os960Transfer ( pConnInfo );

      MSG3 (( INFO_XSER_END_OF_DATA_TRANS,
              pConnInfo->ThrdCtrlHeader.Tid ));

      if ( pConnInfo->szServerDB[0] != '\0' )
        {
        MSGD (( INFO_XSER_APPL_DISCONNECT,
                pConnInfo->szSenderNodeID,
                pConnInfo->szServerDB,
                pConnInfo->ulServerRef ));
        }
      else
        {
        MSGD (( INFO_XSER_DISCONNECT,
                pConnInfo->szSenderNodeID,
                pConnInfo->ulServerRef ));
        }
      }

    if ( pConnInfo->pDBM != NULL )
      {
      eo908_Release ( pConnInfo->pDBM );
      pConnInfo->pDBM = NULL;
      }
    else
      sql33c_release ( &pConnInfo->Shm, pErrText );

    pConnInfo->ulConnStatus = CON_RELEASED;

    if ( pConnInfo->sd != INVALID_SOCKET )
      {
      ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;
      eo420SendReleasePacket ((void *)&pConnInfo->sd, os42SocketSend, 
                              pConnInfo->ulSenderRef,
                              pConnInfo->ulServerRef,
                              ulMessClass,
                              ulCommState );
      }
    }

  MSG4 (( INFO_XSER_SERVER_EXITING, pConnInfo->ThrdCtrlHeader.Tid ));

  if ( pConnInfo->sd != INVALID_SOCKET )
    {
    sql40c_socket_close ( pConnInfo->sd );
    pConnInfo->sd = INVALID_SOCKET;
    }

  sql09c_free_handle  ( gxr.TCPIPCoord.pHandles, ulConnInfoHandle );

  pConnInfo->ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
  hServerThread                       = pConnInfo->ThrdCtrlHeader.hThrd;
  FREE_MEM ( pConnInfo );
  } 
  __except( os968ExceptionHandler(GetExceptionInformation()) ){;}


  DBGOUT;
  EXITTHREAD( ulCommState, hServerThread );
  return;
  }

/*------------------------------*/

static ULONG os960Request ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo )
  {
  #undef  MF__
  #define MF__ MOD__"os960Request"
  teo003_ConPktParamRecord        ConPktParamRec;
  teo003_ConnectParamRecord       LocConnParam;
  ULONG                           ulCommState;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulMinSegmentSize;
  ULONG                           ulMaxSegmentSize;
  tsp00_Uint4                     TransferBufferSize;
  SAPDB_Byte *                    pServerCert = NULL;
  SAPDB_UInt4                     lenCert = 0;


  DBGIN;

  os96InitConPktParamRec( &ConPktParamRec) ;

  //
  // ---  receive server reply
  //
  ulCommState = eo420ReceiveConnectPacket ( (void *) &pConnInfo->sd, 
                                            os42SocketReceive,
                                            &ConPktParamRec, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    MSGD (( ERR_XSER_REC_REQ_PACKET_ERR ));
    return ( ulCommState );
    }
  
  pConnInfo->pDBM = NULL;

  MSG2 (( INFO_XSER_REC_REQ_PACKET,
          ConPktParamRec.ulMessClass,
          ConPktParamRec.ulReceiverRef,
          ConPktParamRec.ulSenderRef,
          ConPktParamRec.ulServiceType,
          ConPktParamRec.szReceiverServerDB,
          ConPktParamRec.szSenderServerDB ));

  switch ( ConPktParamRec.ulMessClass )
    {
    case RSQL_INFO_REQUEST_EO003:
    case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003:

      MSG8 (( INFO_XSER_REC_INFO_REQUEST, ConPktParamRec.szReceiverServerDB ));
      ulCommState = sql33c_info_request( ConPktParamRec.szReceiverServerDB,
                                         ConPktParamRec.ulServiceType,
                                         1,
                                         ConPktParamRec.fOmitReplyPart,
                                         &ConPktParamRec.ulPacketSize,
                                         &ConPktParamRec.ulMaxDataLen,
                                         &ConPktParamRec.ulMinReplySize,
                                         ConPktParamRec.authenticationAllow,
                                         sizeof(ConPktParamRec.authenticationAllow),
                                         pErrText );

      if (( ConPktParamRec.ulMessClass   == RSQL_INFO_REQUEST_KEEP_ALIVE_EO003 ) &&
          ( ConPktParamRec.usServicePort == gxr.TCPIPCoord.usServicePort ))
      {
        pConnInfo->ulConnStatus = CON_KEEP_ALIVE_EO003;
      }

      strcpy ( ConPktParamRec.szSenderServerDB,
               ConPktParamRec.szReceiverServerDB );
      ConPktParamRec.szReceiverServerDB[0] = '\0';
      ConPktParamRec.ulReceiverRef         = ConPktParamRec.ulSenderRef;
      ConPktParamRec.ulSenderRef           = (ULONG)UNDEF;
      ConPktParamRec.usServicePort         = gxr.TCPIPCoord.usServicePort;
      ConPktParamRec.ulMaxSegmentSize      = gxr.TCPIPCoord.ulMaxSegmentSize;
      ConPktParamRec.ulMessClass           = RSQL_INFO_REPLY_EO003;
      ConPktParamRec.ulCommState           = ulCommState;

      if ( ulCommState != commErrOk_esp01 )
        {
        if ( ulCommState != commErrStartRequired_esp01 )
          {
          MSGD (( ERR_XSER_INFO_REQUEST, pErrText ))
          }
        }
      else
        MSG8 (( INFO_XSER_REPLY_INFO_REQUEST, ConPktParamRec.szReceiverServerDB,
                ConPktParamRec.ulPacketSize,  ConPktParamRec.ulMaxDataLen,
                ConPktParamRec.ulMinReplySize, gxr.TCPIPCoord.usServicePort ));
      break;

    case RSQL_USER_CONN_REQUEST_EO003:
    case RSQL_KERN_CONN_REQUEST_EO003:

      MSG8 (( INFO_XSER_REC_CONN_REQ, ConPktParamRec.szReceiverServerDB,
              ConPktParamRec.ulSenderRef, ConPktParamRec.ulPacketSize,
              ConPktParamRec.ulMaxDataLen ));

      os96InitLocalConnectParamRec( &LocConnParam, &ConPktParamRec, pConnInfo ) ;

      ulCommState = sql33c_connect( &LocConnParam, &pConnInfo->Shm, pErrText );

      if ( ulCommState != commErrOk_esp01 )
        {
        MSGD (( ERR_XSER_CONN_REQ, pErrText ))
        }
      else
        {
        pConnInfo->ulConnStatus = CON_CONNECTED;
        ulMinSegmentSize        = 1024;
        ulMaxSegmentSize        = LocConnParam.ulMaxDataLen +
                                  sizeof(RTE_HEADER_REC);

        if ( ulMaxSegmentSize < ConPktParamRec.ulMaxSegmentSize )
          ConPktParamRec.ulMaxSegmentSize = ulMaxSegmentSize;

        if ( ConPktParamRec.ulMaxSegmentSize < ulMinSegmentSize )
          ulMinSegmentSize = ConPktParamRec.ulMaxSegmentSize;

        ulCommState = os96SetSockBufSizes( pConnInfo->sd,
                                           ulMinSegmentSize,
                                          &ConPktParamRec.ulMaxSegmentSize );

        if ( ulCommState == commErrOk_esp01 )
          {
          pConnInfo->ulServerRef      = LocConnParam.ulServerRef;
          pConnInfo->ulMinReplySize   = LocConnParam.ulMinReplySize;
          pConnInfo->ulMaxCmdDataLen  = LocConnParam.ulMaxDataLen -
                                        LocConnParam.ulMinReplySize;
          pConnInfo->pCommPacket      = LocConnParam.pCommPacketList[0];
          pConnInfo->ulSenderRef      = ConPktParamRec.ulSenderRef;
          pConnInfo->ulMaxSegmentSize = ConPktParamRec.ulMaxSegmentSize;
          pConnInfo->fAcknowledge     = ConPktParamRec.fAcknowledge;
          pConnInfo->ulSwapType       = ConPktParamRec.ulSwapType;
          pConnInfo->ulServiceType    = ConPktParamRec.ulServiceType;
          strcpy ( pConnInfo->szServerDB, ConPktParamRec.szReceiverServerDB );

          // - store server reference
          if ( !sql09c_put_handle_data ( gxr.TCPIPCoord.pHandles,
                                         pConnInfo->ulConnInfoHandle,
                                         pConnInfo,
                                         (ULONG)UNDEF,
                                         pConnInfo->ulServerRef ))
            {
            MSGALL (( ERR_XSER_GET_SET_HANDLE_DATA ));
            gxr.ulCrashRc = ERROR_INVALID_HANDLE;
            ABORT();
            }

          MSG8 (( INFO_XSER_CONN_REQ,
                  ConPktParamRec.szReceiverServerDB,
                  pConnInfo->ulServerRef,
                  pConnInfo->ulMaxCmdDataLen,
                  pConnInfo->ulConnInfoHandle ));
          }
        }

      strcpy ( ConPktParamRec.szSenderServerDB,
               ConPktParamRec.szReceiverServerDB );
      ConPktParamRec.szReceiverServerDB[0]  = '\0';
      ConPktParamRec.ulReceiverRef          = ConPktParamRec.ulSenderRef;
      ConPktParamRec.ulSenderRef            = LocConnParam.ulServerRef;
      ConPktParamRec.ulPacketSize           = LocConnParam.ulPacketSize;
      ConPktParamRec.ulMaxDataLen           = LocConnParam.ulMaxDataLen;
      ConPktParamRec.ulMinReplySize         = LocConnParam.ulMinReplySize;
      ConPktParamRec.ulMessClass            = RSQL_USER_CONN_REPLY_EO003;
      ConPktParamRec.ulCommState            = ulCommState;

      strncpy ( ConPktParamRec.authenticationAllow, LocConnParam.authenticationAllow,
                sizeof(ConPktParamRec.authenticationAllow) - 1 );
      ConPktParamRec.authenticationAllow[sizeof(ConPktParamRec.authenticationAllow) - 1] = '\0';
      break;

    case RSQL_USER_CANCEL_REQUEST_EO003:

      MSG5 (( INFO_XSER_CANCEL_REQUESTED, ConPktParamRec.ulReceiverRef  ));

      ulCommState = os960CancelDump ( ConPktParamRec.ulReceiverRef,
                                      ConPktParamRec.szReceiverServerDB,
                                      FALSE,
                                      SQL_RTE_CANCEL );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED;

      return ( ulCommState );

    case RSQL_DUMP_REQUEST_EO003:

      MSGALL (( INFO_XSER_DUMP_REQUESTED, ConPktParamRec.ulSenderRef  ));

      ulCommState = os960CancelDump ( ConPktParamRec.ulReceiverRef,
                                      ConPktParamRec.szReceiverServerDB,
                                      FALSE,
                                      SQL_RTE_DUMP );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED;

      return ( ulCommState );

    case RSQL_CTRL_CANCEL_REQUEST_EO003:

      MSGALL (( INFO_XSER_CANCEL_REQUESTED, ConPktParamRec.ulReceiverRef  ));

      ulCommState = os960CancelDump ( ConPktParamRec.ulReceiverRef,
                                      ConPktParamRec.szReceiverServerDB,
                                      TRUE,
                                      SQL_RTE_CANCEL );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED;

      return ( ulCommState );

    case RSQL_CTRL_CONN_REQUEST_EO003:

      MSG8 (( INFO_XSER_REC_CONN_REQ, ConPktParamRec.szReceiverServerDB,
              ConPktParamRec.ulSenderRef, ConPktParamRec.ulPacketSize,
              ConPktParamRec.ulMaxDataLen ));

      {
          char *serverPgm = ConPktParamRec.szServerPgm;
          if ( stricmp(serverPgm, "dbmsrv.exe")
            && stricmp(serverPgm, "dbmsrv")
            && stricmp(serverPgm, "lserver.exe")
            && stricmp(serverPgm, "lserver") )
          {
              serverPgm = (char *)"dbmsrv.exe";
          }
          ulCommState = eo908_Connect( serverPgm,
                                       ConPktParamRec.szServerDBRoot,
                                       ConPktParamRec.szReceiverServerDB,
                                       &pConnInfo->pDBM,
                                       &pConnInfo->pCommPacket,
                                       &pConnInfo->ulMaxCmdDataLen,
                                       &TransferBufferSize,
                                       &pConnInfo->ulServerRef, 
                                       (SAPDB_Char const *)&pConnInfo->szSenderNodeID,
                                       ConPktParamRec.pidSenderPID);
      }

      if ( ulCommState == commErrOk_esp01 )
        {
        ulMinSegmentSize        = 1024;
        ulMaxSegmentSize        = TransferBufferSize;

        if ( ulMaxSegmentSize < ConPktParamRec.ulMaxSegmentSize )
          ConPktParamRec.ulMaxSegmentSize = ulMaxSegmentSize;

        if ( ConPktParamRec.ulMaxSegmentSize < ulMinSegmentSize )
          ulMinSegmentSize = ConPktParamRec.ulMaxSegmentSize;

        ulCommState = os96SetSockBufSizes( pConnInfo->sd,
                                           ulMinSegmentSize,
                                           &ConPktParamRec.ulMaxSegmentSize );

        if ( ulCommState == commErrOk_esp01 )
          {
          pConnInfo->ulSenderRef      = ConPktParamRec.ulSenderRef;
          pConnInfo->ulMaxSegmentSize = ConPktParamRec.ulMaxSegmentSize;
          pConnInfo->fAcknowledge     = ConPktParamRec.fAcknowledge;
          pConnInfo->ulSwapType       = ConPktParamRec.ulSwapType;
          pConnInfo->ulServiceType    = ConPktParamRec.ulServiceType;
          pConnInfo->ulConnStatus     = CON_CONNECTED;
         
          // - store server reference
          if ( !sql09c_put_handle_data ( gxr.TCPIPCoord.pHandles,
                                         pConnInfo->ulConnInfoHandle,
                                         pConnInfo,
                                         (ULONG)UNDEF,
                                         pConnInfo->ulServerRef ))
            {
            MSGALL (( ERR_XSER_GET_SET_HANDLE_DATA ));
            gxr.ulCrashRc = ERROR_INVALID_HANDLE;
            ABORT();
            }
          }
        }

      strcpy ( ConPktParamRec.szSenderServerDB,
               ConPktParamRec.szReceiverServerDB );
      ConPktParamRec.ulCommState    = ulCommState;
      ConPktParamRec.ulMessClass    = RSQL_CTRL_CONN_REPLY_EO003;
      ConPktParamRec.ulReceiverRef  = pConnInfo->ulSenderRef;
      ConPktParamRec.ulSenderRef    = pConnInfo->ulServerRef;
      break;

    /* PTS 1105295 */
    case RSQL_PING_REQUEST_EO003:
      MSG8(( INFO_XSER_PING_FROM, ConPktParamRec.szVersion ));
      ConPktParamRec.ulMessClass           = RSQL_PING_REPLY_EO003;
      SAPDB_memcpy(ConPktParamRec.szVersion, gxr.RTEVersionString, sizeof(ConPktParamRec.szVersion) );
      ConPktParamRec.ulCommState           = commErrOk_esp01;
      break;

    case RSQL_CERT_REQUEST_EO003:
      MSGD(( INFO_XSER_CERT_REQUEST ));
      ulCommState = RTESec_SAPSSLExportOwnCertificate (&pServerCert, &lenCert, pErrText);   
      ConPktParamRec.ulCommState           = ulCommState;
      break;
 
    case RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003:  /* PTS 1111208 */
      os968UpdateDebugLevelFromRegistry();
	  MSGD(( INFO_XSERVER_NEW_DEBUG_LEVEL, gxr.usDebugLevel ));
      ConPktParamRec.ulMessClass           = RSQL_NEW_DEBUG_LEVEL_REPLY_EO003;
      ConPktParamRec.ulCommState           = commErrOk_esp01;
      break;

    default :
      MSGALL (( ERR_XSER_ILL_FRST_MESS_CLASS,
                ConPktParamRec.ulMessClass ));
      ulCommState                = commErrNotOk_esp01;
      ConPktParamRec.ulCommState = commErrNotOk_esp01;
      break;
    }


    if (RSQL_CERT_REQUEST_EO003 == ConPktParamRec.ulMessClass)
    {
        ConPktParamRec.ulMessClass           = RSQL_CERT_REPLY_EO003;
        if ( ulCommState != commErrOk_esp01 )            // - do not overwrite 'ulCommState'
        {
            eo420SendCertificatePacket ((void *) &pConnInfo->sd, 
                                        os42SocketSend,
                                        &ConPktParamRec, 
                                        pServerCert, 
                                        lenCert,
                                        pErrText );
        }
        else
        {
            ulCommState = eo420SendCertificatePacket ((void *) &pConnInfo->sd, 
                                                      os42SocketSend,
                                                      &ConPktParamRec,
                                                      pServerCert,
                                                      lenCert,
                                                      pErrText );
            if ( ulCommState != commErrOk_esp01 )
            {
                MSGD (( ERR_XSER_SEND_CERT_PACKET ));
            }
        }

        RTESec_SAPSSLAuxFreeString (&pServerCert);      
    }
    else
    {
        if ( ulCommState != commErrOk_esp01 )            // - do not overwrite 'ulCommState'
            eo420SendConnectPacket( (void *) &pConnInfo->sd,
                                    os42SocketSend,
                                    &ConPktParamRec, pErrText );
        else
        {
            ulCommState = eo420SendConnectPacket( (void *) &pConnInfo->sd, 
                os42SocketSend,
                &ConPktParamRec, pErrText );

            if ( ulCommState != commErrOk_esp01 )
                MSGD (( ERR_XSER_SEND_REQ_PACKET_ERR ));
        }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG os960Transfer ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo )
  {
  #undef  MF__
  #define MF__ MOD__"os960Transfer"
  ULONG                           ulCommState;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulResDataLen;
  PCOMM_PACKET_REC                pResCommPacket;
  ULONG                           ulDataLen;
  ULONG                           ulSenderRef;
  ULONG                           ulReceiverRef;
  ULONG                           ulMessClass;
  tsp00_Uint1                     RTEFlags;


  do
    {
    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_COMMAND_WAIT;

    ulCommState = eo420ReceiveCommPacket ( (VOID *)&pConnInfo->sd, 
                                           os42SocketReceive,
                                           pConnInfo->ulSwapType,
                                           pConnInfo->pCommPacket,
                                           pConnInfo->ulMaxCmdDataLen,
                                           &ulDataLen,
                                           &ulSenderRef,
                                           &ulReceiverRef,
                                           &ulMessClass,
                                           &RTEFlags,
                                           pErrText );

    if ( ulCommState != commErrOk_esp01 )
      {
      if ( ulCommState == commErrReleased_esp01 )
        {
        ulCommState = commErrOk_esp01;
        break;
        }

      MSGD (( ERR_XSER_RECV_PACKET, pConnInfo->ulServerRef, pErrText ));
      break;
      }
    else if ( ! ulDataLen )
      break;
    else if (( ulMessClass != RSQL_KERN_DATA_REQUEST_EO003 ) &&
             ( ulMessClass != RSQL_USER_DATA_REQUEST_EO003 ))
      {
      MSGALL ((ERR_XSER_ILL_MESS_CLASS, ulMessClass, pConnInfo->ulServerRef));
      break;
      }

    MSG8 (( INFO_XSER_READ_REQ_PACKET, ulDataLen, pConnInfo->ulServerRef ));

    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_COMMAND_SEND;

    if ( pConnInfo->pDBM != NULL )
    {
      ulCommState = eo908_Request( pConnInfo->pDBM, ulDataLen );

      if ( ulCommState != commErrOk_esp01 )
        break;
    }
    else if ( pConnInfo->ulServiceType == srvPing_esp01 )
    {
        SAPDB_UInt1     hops;

        if ( eo420ExtractPingPacket( (char*)pConnInfo->pCommPacket->pDataPart, ulDataLen, &hops, 0, 0 ) )
        {
            if ( hops-- == 0 )
            {   // No more hops!
                pResCommPacket  = (PCOMM_PACKET_REC)(PCHAR)pConnInfo->pCommPacket;

                ulResDataLen =  eo420FillPingPacket ( (char*) pResCommPacket->pDataPart,
                                                      pConnInfo->ulMaxCmdDataLen - ulDataLen + 
                                                      pConnInfo->ulMinReplySize,
                                                      0, // hops equal to 0 means that the final destination was not reached
                                                      XSERV_TITLE,
                                                      gxr.RTEVersionString, 0 );

                pConnInfo->ThrdCtrlHeader.ThrdState = THRD_DATA_SEND;

                ulCommState = eo420SendCommPacket ( (void *)&pConnInfo->sd, os42SocketSend,
                                                    pConnInfo->ulMaxSegmentSize,
                                                    pResCommPacket,
                                                    ulResDataLen,
                                                    ulSenderRef,
                                                    ulReceiverRef,
                                                    RSQL_USER_DATA_REPLY_EO003,
                                                    commErrOk_esp01,
                                                    RSQL_NORMAL_EO003,
                                                    pErrText );

                if ( ulCommState != commErrOk_esp01 )
                {
                    MSGD (( ERR_XSER_SEND_PACKET, pConnInfo->ulServerRef, pErrText ));
                    break;
                }
                MSG8 (( INFO_XSER_SEND_REPLY_PACKET, ulResDataLen, pConnInfo->ulServerRef ));
                continue;
            }
    
            ulCommState = sql33c_request( &pConnInfo->Shm, 0, ulDataLen, RTEFlags, pErrText );

            if ( ulCommState != commErrOk_esp01 )
            {
                MSGD (( ERR_XSER_REQUEST, pConnInfo->ulServerRef, pErrText ));
                break;
            }
        }
        else
        {
            ulCommState = commErrNotOk_esp01;
            break;
        }
    }
    else
    {
      ulCommState = sql33c_request( &pConnInfo->Shm, 0, ulDataLen, RTEFlags, pErrText );

      if ( ulCommState != commErrOk_esp01 )      {

        MSGD (( ERR_XSER_REQUEST, pConnInfo->ulServerRef, pErrText ));
        break;
      }
    }

    MSG8(( INFO_XSER_REQUEST_RETURNED, pConnInfo->ulServerRef, ulCommState ));

    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_DATA_RECEIVE;

    if ( pConnInfo->pDBM != NULL )
    {
      ulCommState = eo908_Receive( pConnInfo->pDBM, &pResCommPacket,
                                   &ulResDataLen );

      if ( ulCommState != commErrOk_esp01 )
      {
        if ( ulCommState == commErrReleased_esp01 )
          ulCommState = commErrOk_esp01;
        break;
      }
      else if ( ! ulDataLen )
        break;
    }
    else
    {
      ulCommState = sql33c_receive( &pConnInfo->Shm, &pResCommPacket,
                                    &ulResDataLen, pErrText );

      if ( ulCommState != commErrOk_esp01 )
        {
        MSGD (( ERR_XSER_RECEIVE, pConnInfo->ulServerRef, pErrText ));
        break;
        }
    }


    MSG8 (( INFO_XSER_RECEIVE_RETURNED,
            pConnInfo->ulServerRef, ulCommState ));

    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_DATA_SEND;

    ulCommState = eo420SendCommPacket ( (void *)&pConnInfo->sd, os42SocketSend,
                                        pConnInfo->ulMaxSegmentSize,
                                        pResCommPacket,
                                        ulResDataLen,
                                        ulSenderRef,
                                        ulReceiverRef,
                                        RSQL_USER_DATA_REPLY_EO003,
                                        commErrOk_esp01,
                                        RSQL_NORMAL_EO003,
                                        pErrText );

    if ( ulCommState != commErrOk_esp01 )
      {
      MSGD (( ERR_XSER_SEND_PACKET, pConnInfo->ulServerRef, pErrText ));
      break;
      }

    MSG8 (( INFO_XSER_SEND_REPLY_PACKET,
            ulResDataLen, pConnInfo->ulServerRef ));
    }
  while (ulCommState == commErrOk_esp01) ;

  pConnInfo->ThrdCtrlHeader.ThrdState = THRD_RUNNING;

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG os960CancelDump ( ULONG                        ulServerRef,
                               PSZ                          pszServerDB,
                               BOOL                         DBMConnect,
                               ULONG                        ulReqType )
  {
  #undef  MF__
  #define MF__ MOD__"os960CancelDump"
  ULONG                           ulCommState = commErrOk_esp01;
  PTCPIP_SERV_CONNECT_INFO_REC    pConnInfo;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulTmp;
  ULONG                           ulConnInfoHandle = 0;


  DBGPAS;

  // --- Find the connect info handle of the command to be cancelled and try
  //     to lock it.
  //
  for ( ;; )
    {
    if (( !sql09c_find_handle_by_id ( gxr.TCPIPCoord.pHandles,
                                      ulServerRef, &ulConnInfoHandle )) ||
        ( !sql09c_lock_handle ( gxr.TCPIPCoord.pHandles, ulConnInfoHandle )) )
      {
      return ( commErrOk_esp01 );
      }

    if ( sql09c_get_handle_data  ( gxr.TCPIPCoord.pHandles,
                                  ulConnInfoHandle,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
      {
      if ( strcmp (pConnInfo->szServerDB, pszServerDB) )
        continue;

      if (( pConnInfo->pDBM != NULL ) && ( DBMConnect == TRUE ))
        ulCommState = eo908_Cancel ( pConnInfo->pDBM );
      else
        ulCommState = sql33c_cancel_dump ( &pConnInfo->Shm, ulReqType, pErrText );
      }

    if ( !sql09c_unlock_handle ( gxr.TCPIPCoord.pHandles, ulConnInfoHandle ))
      {
      MSGALL (( ERR_XSER_LOCK_UNLOCK_HANDLE ));
      gxr.ulCrashRc = ERROR_INVALID_HANDLE;
      ABORT();
      }

    break;
    }


  MSG5 (( INFO_XSER_CANCEL_RETURNING, ulCommState ))

  return ( ulCommState );
  }


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
