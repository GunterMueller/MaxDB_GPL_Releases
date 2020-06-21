/*!
  @file           vos964.c
  @author         JoergM/RaymondR
  @brief          NISSL support > 6.1 applications
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

#include           "hos42.h"
#include           "geo40.h"
#include           "heo46.h"
#include           "heo420.h"
#include		   "geo43_0.h"
#include		   "gos96.h"
#include		   "geo007_1.h"
#include		   "gos962.h"
#include		   "gos964.h"
#include		   "geo908.h"
#include           "hsp100.h"

#include           "RunTime/Security/RTESec_SSL.h"
#include           "RunTime/Security/RTESec_SAPSSL.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__  "VOS964.C : "
#define MF__   MOD__"UNDEFINED"

#define MAXTCPIPCONNECTIONS       32768
#define LINGER_TIME               30

#  define DEBUGGER                       "msdev "

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

# if defined (DEBUGGER)
   typedef WINBASEAPI BOOL (WINAPI tIsDbgPresent)( VOID );
# endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

VOID _System sql96t_NISSL_serv_thread ( ULONG                        ulConnInfoHandle );
ULONG        sql96t_cancel_dump       ( ULONG                        ulServerRef,
                                        PSZ                          pszServerDB,
                                        BOOL                         DBMConnect,
                                        ULONG                        ulReqType );

static ULONG os964NiSSLGetRequest       ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo );

static ULONG os964NiSSLTransfer         ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo );

void _System os964NiSSLCoordinator      ( void );

static ULONG os964OpenListeningSocket  ( VOID );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


tsp00_Int    
os964StartNISSLCoord ( NI_COORD_CTRL_REC *pNICoord )
{
  #undef  MF__
  #define MF__ MOD__"os964StartNISSLCoord"
  LONG                    rc        = NO_ERROR;

  if (RTESec_SAPSSLIsInitialized ())
  {
    pNICoord->ThrdCtrlHeader.Tid         = (TID)UNDEF;
    pNICoord->ThrdCtrlHeader.hThrd       = (HANDLE)INVALID_HANDLE_VALUE;
    pNICoord->ThrdCtrlHeader.lThrdPrio   = SERVER_LOOP_THRD_PRIO;
    pNICoord->ThrdCtrlHeader.ThrdState   = THRD_INITIALIZED;
    pNICoord->ThrdCtrlHeader.ulStartTime = (ULONG)UNDEF;

    rc = CREATE_THREAD( &pNICoord->ThrdCtrlHeader.Tid,
                        &pNICoord->ThrdCtrlHeader.hThrd,
                         os964NiSSLCoordinator,
                         0,
                         CREATE_THRD_SUSPENDED,
                         TCPIP_COORD_STACK_SIZE );

    if ( rc == NO_ERROR )
    {
      rc = RESUME_THREAD( pNICoord->ThrdCtrlHeader.Tid,
                          pNICoord->ThrdCtrlHeader.hThrd );

     if ( rc != NO_ERROR )
        MSGD (( ERR_XSER_RESUMING_THREAD, "NISSLCOORDINATOR", rc));
    }
    else
      MSGD (( ERR_XSER_CREATING_THREAD, "NISSLCOORDINATOR", rc));
  }
  return ( rc ) ;
}

/*------------------------------*/

VOID _System os964NiSSLCoordinator ( VOID )
{
  #undef  MF__
  #define MF__ MOD__"os964NiSSLCoordinator"
  SQL_NODEIDC                    szSenderNodeID;
  ULONG                          ulConnInfoHandle;
  APIRET                         rc        = NO_ERROR;
  ULONG                          ulErrCnt  = 0;
  void                          *NiHandle;
  void                          *pSSLHandle = 0;
  PTCPIP_SERV_CONNECT_INFO_REC   pConnInfo = NULL;
  ULONG                          ulCommState = commErrOk_esp01;  /* PTS 1112818 */

  DBGIN;

  __try
  {
  // --- set thread control info
  gxr.NISSLCoord.ThrdCtrlHeader.ulStartTime = time (NULL);

  //
  //
  // --- set the thread priority
  //
  //
  rc = SET_THREAD_PRIO ( gxr.NISSLCoord.ThrdCtrlHeader.lThrdPrio,
                         gxr.NISSLCoord.ThrdCtrlHeader.Tid,
                         gxr.NISSLCoord.ThrdCtrlHeader.hThrd );

  if ( rc != NO_ERROR )
  {
      MSGD (( ERR_XSER_SET_THRD_PRIO_ERROR,
               gxr.NISSLCoord.ThrdCtrlHeader.Tid, rc ));
      gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
      gxr.ulCrashRc                           = rc;
      ABORT();
  }

  if( !sql09c_init_handle_dir( &gxr.NISSLCoord.pHandles, MAXTCPIPCONNECTIONS ))
  {
      MSGD (( ERR_RESERVE_HANDLE_VALUES ));
      gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
      gxr.ulCrashRc                           = ERROR_INVALID_HANDLE;
      ABORT();
  }

  rc = os964OpenListeningSocket ();

  if ( rc != commErrOk_esp01 )
  {
      gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
      gxr.ulCrashRc                           = rc;
      ABORT();
  }


  for (;;)
  {
    tsp00_ErrTextc          pErrText ;

    MSG6 (( INFO_NISSL_SERV_ACCEPTING ));
    gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_SLEEPING;

    /* PTS 1112818 */
    ulCommState = eo40NiAccept ( gxr.NISSLCoord.NiHandle, &NiHandle, szSenderNodeID, 
                                 sizeof(szSenderNodeID), pErrText );

    gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_RUNNING;

    if ( ulCommState == commErrOk_esp01 && eo40NiIsValidHandle(NiHandle) )
    {
        ulCommState = RTESec_SAPSSLStartSession (NiHandle,
                                                 0,
                                                 false,
                                                 &pSSLHandle,
                                                 pErrText); 
    }

    /* PTS 1112818 */
    if ( (ulCommState != commErrOk_esp01) || !eo40NiIsValidHandle(NiHandle) )
    {
      
        MSGD (( ERR_NISSL_SERV_ACCEPT_ERROR, (char *)pErrText ));

        if (++ulErrCnt > 9)
        {
        //
        // --- try to reopen the listen socket
        //
            eo40NiClose ( &gxr.NISSLCoord.NiHandle );
            ulErrCnt = 0;

            SLEEP( 30000 );

            ulCommState = os964OpenListeningSocket ();

            if ( ulCommState != commErrOk_esp01 )
                break;
        }

        continue;
    }
    else
    {
      ulErrCnt = 0;
    }

    MSG7 (( INFO_NISSL_SERV_ACCEPT_CONNECTION_FROM_CLIENT, *((SAPDB_Int *)NiHandle), szSenderNodeID ));

    // --- allocate space for the internal communication structure
    rc = ALLOC_MEM((PPVOID)&pConnInfo, sizeof(TCPIP_SERV_CONNECT_INFO_REC));

    if ( rc != NO_ERROR )
    {
        RTESec_SAPSSLSessionClose (pSSLHandle);
        eo40NiClose ( &NiHandle );
        continue;
    }

    // --- Get a new handle and combine it with the pointer to the
    //     internal communication structure.
    //
    if ( !sql09c_reserve_handle ( gxr.NISSLCoord.pHandles, &ulConnInfoHandle) )
    {
        MSGD (( ERR_XSER_MAX_CONNECTIONS ));
        RTESec_SAPSSLSessionClose (pSSLHandle);
        eo40NiClose ( &NiHandle );
        FREE_MEM ( pConnInfo );
        continue;
    }

    if ( !sql09c_put_handle_data( gxr.NISSLCoord.pHandles, ulConnInfoHandle,
                                  pConnInfo, (ULONG)UNDEF, (ULONG)UNDEF))
    {
        MSGD (( ERR_XSER_GET_SET_HANDLE_DATA ));
        gxr.ulCrashRc = ERROR_INVALID_HANDLE;
        ABORT();
    }

    pConnInfo->ThrdCtrlHeader.Tid         = (TID)UNDEF;
    pConnInfo->ThrdCtrlHeader.hThrd       = (HANDLE)INVALID_HANDLE_VALUE;
    pConnInfo->ThrdCtrlHeader.lThrdPrio   = SERVER_THRD_PRIO;
    pConnInfo->ThrdCtrlHeader.ThrdState   = THRD_INITIALIZED;
    pConnInfo->ThrdCtrlHeader.ulStartTime = (ULONG)UNDEF;

    pConnInfo->ulConnInfoHandle           = ulConnInfoHandle;
    pConnInfo->NiHandle                   = NiHandle;
    pConnInfo->pSSLHandle                 = pSSLHandle;
    strcpy ( pConnInfo->szSenderNodeID, szSenderNodeID );

    pConnInfo->ulConnStatus               = CON_INITIALIZED_EO003;
    pConnInfo->ulServerRef                = (ULONG)UNDEF;
    pConnInfo->ulMaxCmdDataLen            = (ULONG)UNDEF;
    pConnInfo->pCommPacket                = NULL;

    pConnInfo->fAcknowledge               = FALSE;
    pConnInfo->ulSwapType                 = (ULONG)UNDEF;
    pConnInfo->ulMaxSegmentSize           = (ULONG)UNDEF;

    pConnInfo->pWorldSA                  = &gxr.WorldSA;
    pConnInfo->pLockSA                   = &gxr.LockSA;

    rc = CREATE_THREAD( &pConnInfo->ThrdCtrlHeader.Tid,
                        &pConnInfo->ThrdCtrlHeader.hThrd,
                        sql96t_NISSL_serv_thread,
                        (tsp00_Longuint)ulConnInfoHandle,
                        CREATE_THRD_SUSPENDED,
                        SERVER_STACK_SIZE);

    if (rc != NO_ERROR)
    {
        MSGD (( ERR_NISSL_SERV_CREATING_THREAD, "SERVER", rc));

        RTESec_SAPSSLSessionClose (pSSLHandle);
        eo40NiClose ( &NiHandle );
        sql09c_free_handle  ( gxr.NISSLCoord.pHandles, ulConnInfoHandle );
        FREE_MEM ( pConnInfo );
    }
    else
    {
        rc = RESUME_THREAD(pConnInfo->ThrdCtrlHeader.Tid,
                           pConnInfo->ThrdCtrlHeader.hThrd);

        if (rc != NO_ERROR)
        {
            MSGD (( ERR_NISSL_SERV_RESUMING_THREAD, "SERVER", rc));

            KILL_THREAD (pConnInfo->ThrdCtrlHeader.Tid,
                         pConnInfo->ThrdCtrlHeader.hThrd);
            RTESec_SAPSSLSessionClose (pSSLHandle);
            eo40NiClose ( &NiHandle );
            sql09c_free_handle  ( gxr.NISSLCoord.pHandles, ulConnInfoHandle );
            FREE_MEM ( pConnInfo );
        }
    }
  }

  gxr.NISSLCoord.ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
  gxr.ulCrashRc                           = rc;
  ABORT();
  } 
  __except( os968ExceptionHandler(GetExceptionInformation()) ){;}

  DBGOUT;
  EXITTHREAD( rc, gxr.NISSLCoord.ThrdCtrlHeader.hThrd );
  return;
}

/*------------------------------*/

VOID os964NISSLFinish ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os964NISSLFinish"
  ULONG                           ulCommState = commErrOk_esp01;
  PTCPIP_SERV_CONNECT_INFO_REC    pConnInfo;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulTmp;
  ULONG                           ulConnInfoHandle = 0;
  TID                             tidTID           = THREADID;

  DBGPAS;

  if ( tidTID != gxr.NISSLCoord.ThrdCtrlHeader.Tid)
    {
    KILL_THREAD ( gxr.NISSLCoord.ThrdCtrlHeader.Tid,
                  gxr.NISSLCoord.ThrdCtrlHeader.hThrd );
    }

  while ( sql09c_find_next_handle( gxr.NISSLCoord.pHandles, &ulConnInfoHandle ))
    {
    if ( !sql09c_lock_handle ( gxr.NISSLCoord.pHandles, ulConnInfoHandle ))
      continue;

    if ( sql09c_get_handle_data  ( gxr.NISSLCoord.pHandles,
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
          ( pConnInfo->ulConnStatus != CON_INITIALIZED_EO003 ) &&
          ( pConnInfo->ulConnStatus != CON_CANCELLED_EO003   ) &&
          ( pConnInfo->ulConnStatus != CON_RELEASED_EO003    ))
        {
        MSGD (( INFO_NISSL_SERV_CONN_KILLED,
                pConnInfo->szServerDB, pConnInfo->ulServerRef ));

        RTESec_SAPSSLSessionClose (pConnInfo->pSSLHandle);

        if ( pConnInfo->pDBM != NULL )
          {
          eo908_Release ( pConnInfo->pDBM );
          pConnInfo->pDBM = NULL;
          }
        else
          sql33c_release ( &pConnInfo->Shm, pErrText );

        pConnInfo->ulConnStatus = CON_RELEASED_EO003;
        }
      }
    }

  RTESec_SAPSSLUtilClose ();
  RTESec_SAPSSLClose ();

  return;
  }

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

//****************************************************************************
tsp00_Int os964InitializeNISSLSupport ( )
//****************************************************************************
{
    tsp00_ErrTextc  pErrText;
    tsp00_Pathc     niTraceFile ;
    tsp01_RteError  RteError;

    ROUTINE_DBG_MEO60 (_T("os964InitializeNISSLSupport"));

    if (!RTEListener_BuildNiTraceFilename(niTraceFile, &RteError))
    {

        MSGD (( ERR_XSERVER_NI_TRACEFILE, RteError.RteErrText));
        return (1);
    }


    if ( RTESec_SSLInit (niTraceFile, pErrText) != commErrOk_esp01 )
    {
        MSGD (( WRN_NISSL_INIT, pErrText));
	    return (1);
    }

    if ( RTESec_SSLUtilInit (niTraceFile, pErrText) != commErrOk_esp01 )
    {
        MSGD (( ERR_NISSL_UTIL_INIT, pErrText));
	    return (1);
    }

    return(NO_ERROR); 
}

/*------------------------------*/

static ULONG os964OpenListeningSocket ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os964OpenListeningSocket"
  ULONG                          ulCommState = commErrOk_esp01;
  tsp00_ErrTextc                 ErrText ;

  DBGIN;

  gxr.NISSLCoord.ulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
                                    sizeof(RTE_CONNECT_PACKET_REC);

  ulCommState = eo40NiListen ( &gxr.NISSLCoord.NiHandle, &gxr.NISSLCoord.usServicePort,
                               ErrText) ;

  if ( ulCommState != commErrOk_esp01 )
  {
    MSGD (( ERR_NISSL_SERV_LISTEN_ERROR, (char *)ErrText ));
  }
  else
  {
    MSG6 (( INFO_XSER_LISTEN_ON_NI_HANDLE, *((SAPDB_Int *)(gxr.NISSLCoord.NiHandle)) ));
    /* set socket options */
  }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static VOID _System sql96t_NISSL_serv_thread ( ULONG ulConnInfoHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql96t_NISSL_serv_thread"
  PTCPIP_SERV_CONNECT_INFO_REC   pConnInfo = NULL;
  ULONG                          ulCommState;
  HANDLE                         hServerThread;
  ULONG                          ulTmp;
  tsp00_ErrText                  pErrText;
  ULONG                          ulMessClass;
  LONG                           rc;

  DBGIN;

  __try
  {
    if ( !sql09c_get_handle_data  ( gxr.NISSLCoord.pHandles,
                                  ulConnInfoHandle,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSGD (( ERR_XSER_GET_SET_HANDLE_DATA ));
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
    MSGD (( ERR_XSER_SET_THRD_PRIO_ERROR,
             pConnInfo->ThrdCtrlHeader.Tid, rc ));
    gxr.ulCrashRc  = rc;
    ABORT();
    return;
    }

  MSG4 (( INFO_NISSL_SERV_SERVER_STARTED, pConnInfo->ThrdCtrlHeader.Tid ))

  pConnInfo->ThrdCtrlHeader.ThrdState = THRD_RUNNING;

  do 
  {
    ulCommState = os964NiSSLGetRequest ( pConnInfo );
  }
  while (( ulCommState             == commErrOk_esp01 )  &&
         ( pConnInfo->ulConnStatus == CON_KEEP_ALIVE_EO003 ));
  
  if ( pConnInfo->ulConnStatus == CON_CONNECTED_EO003 )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      if ( pConnInfo->szServerDB[0] != '\0' )
        {
        MSGD (( INFO_NISSL_SERV_APPL_CONNECTED,
                pConnInfo->szSenderNodeID,
                pConnInfo->szServerDB,
                pConnInfo->ulServerRef ));
        }
      else
        {
        MSGD (( INFO_NISSL_SERV_CONNECTED,
                pConnInfo->szSenderNodeID,
                pConnInfo->ulServerRef ));
        }


      MSG3 (( INFO_NISSL_SERV_ENTER_DATA_TRANS,
              pConnInfo->ThrdCtrlHeader.Tid ));

      ulCommState = os964NiSSLTransfer ( pConnInfo );

      MSG3 (( INFO_NISSL_SERV_END_OF_DATA_TRANS,
              pConnInfo->ThrdCtrlHeader.Tid ));

      if ( pConnInfo->szServerDB[0] != '\0' )
        {
        MSGD (( INFO_NISSL_SERV_APPL_DISCONNECT,
                pConnInfo->szSenderNodeID,
                pConnInfo->szServerDB,
                pConnInfo->ulServerRef ));
        }
      else
        {
        MSGD (( INFO_NISSL_SERV_DISCONNECT,
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

    pConnInfo->ulConnStatus = CON_RELEASED_EO003;

    if ( eo40NiIsValidHandle(pConnInfo->NiHandle) )
      {
      ulMessClass = RSQL_USER_RELEASE_REQUEST_EO003;

      eo420SendReleasePacket( pConnInfo->pSSLHandle, (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend,
                              pConnInfo->ulSenderRef,
                              pConnInfo->ulServerRef,
                              ulMessClass,
                              ulCommState );
      }
    }

  MSG4 (( INFO_NISSL_SERV_SERVER_EXITING, pConnInfo->ThrdCtrlHeader.Tid ));

  if ( eo40NiIsValidHandle(pConnInfo->NiHandle) )
  {
      RTESec_SAPSSLSessionClose (pConnInfo->pSSLHandle);
      eo40NiClose ( &pConnInfo->NiHandle );
  }
  sql09c_free_handle  ( gxr.NISSLCoord.pHandles, ulConnInfoHandle );

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

static ULONG os964NiSSLGetRequest ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo )
  {
  #undef  MF__
  #define MF__ MOD__"os964NiSSLGetRequest"
  teo003_ConPktParamRecord        ConPktParamRec;
  teo003_ConnectParamRecord       LocConnParam;
  ULONG                           ulCommState;
  tsp00_ErrTextc                  pErrText;
  ULONG                           ulMinSegmentSize;
  ULONG                           ulMaxSegmentSize;
  tsp00_Uint4                     TransferBufferSize;

  DBGIN;


  os96InitConPktParamRec( &ConPktParamRec) ;


  //
  // ---  receive server reply
  //
  ulCommState = eo420ReceiveConnectPacket ( pConnInfo->pSSLHandle, 
                                            (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive,
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


      strcpy ( ConPktParamRec.szSenderServerDB,
               ConPktParamRec.szReceiverServerDB );
      ConPktParamRec.szReceiverServerDB[0] = '\0';
      ConPktParamRec.ulReceiverRef         = ConPktParamRec.ulSenderRef;
      ConPktParamRec.ulSenderRef           = (ULONG)UNDEF;
      ConPktParamRec.usServicePort         = UNDEF; /* unused */
      ConPktParamRec.ulMaxSegmentSize      = gxr.NISSLCoord.ulMaxSegmentSize;
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
      {
        pConnInfo->ulConnStatus = CON_KEEP_ALIVE_EO003;
        MSG8 (( INFO_XSER_REPLY_INFO_REQUEST, ConPktParamRec.szReceiverServerDB,
                ConPktParamRec.ulPacketSize,  ConPktParamRec.ulMaxDataLen,
                ConPktParamRec.ulMinReplySize, gxr.NISSLCoord.usServicePort ));
      }
      break;

    case RSQL_USER_CONN_REQUEST_EO003:
    case RSQL_KERN_CONN_REQUEST_EO003:
      MSG8 (( INFO_NISSL_SERV_REC_CONN_REQ, ConPktParamRec.szReceiverServerDB,
              ConPktParamRec.ulSenderRef, ConPktParamRec.ulPacketSize,
              ConPktParamRec.ulMaxDataLen ));

      os96InitLocalConnectParamRec( &LocConnParam, &ConPktParamRec, pConnInfo ) ;

      LocConnParam.encryption = RTE_NI_SSL_ENCRYPTION;
      ulCommState = sql33c_connect( &LocConnParam, &pConnInfo->Shm,
                                    pErrText );

      if ( ulCommState != commErrOk_esp01 )
        {
        MSGD (( ERR_NISSL_SERV_CONN_REQ, pErrText ))
        }
      else
        {
        pConnInfo->ulConnStatus = CON_CONNECTED_EO003;
        ulMinSegmentSize        = 1024;
        ulMaxSegmentSize        = LocConnParam.ulMaxDataLen +
                                  sizeof(RTE_HEADER_REC);

        if ( ulMaxSegmentSize < ConPktParamRec.ulMaxSegmentSize )
          ConPktParamRec.ulMaxSegmentSize = ulMaxSegmentSize;

        if ( ConPktParamRec.ulMaxSegmentSize < ulMinSegmentSize )
          ulMinSegmentSize = ConPktParamRec.ulMaxSegmentSize;

        ulCommState = eo420SetNiSocketBufferSize ( pConnInfo->NiHandle, ulMinSegmentSize,
                                                  &ConPktParamRec.ulMaxSegmentSize,
                                                   pErrText) ;
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
          if ( !sql09c_put_handle_data ( gxr.NISSLCoord.pHandles,
                                         pConnInfo->ulConnInfoHandle,
                                         pConnInfo,
                                         (ULONG)UNDEF,
                                         pConnInfo->ulServerRef ))
            {
            MSGD (( ERR_XSER_GET_SET_HANDLE_DATA ));
            gxr.ulCrashRc = ERROR_INVALID_HANDLE;
            ABORT();
            }

          MSG8 (( INFO_NISSL_SERV_CONN_REQ,
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
      MSG5 (( INFO_NISSL_SERV_CANCEL_REQUESTED, ConPktParamRec.ulReceiverRef  ));

      ulCommState = sql96t_cancel_dump ( ConPktParamRec.ulReceiverRef,
                                         ConPktParamRec.szReceiverServerDB,
                                         FALSE,
                                         SQL_RTE_CANCEL );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED_EO003;

      return ( ulCommState );

    case RSQL_DUMP_REQUEST_EO003:
      MSGD (( INFO_NISSL_SERV_DUMP_REQUESTED, ConPktParamRec.ulSenderRef  ));

      ulCommState = sql96t_cancel_dump ( ConPktParamRec.ulReceiverRef,
                                         ConPktParamRec.szReceiverServerDB,
                                         FALSE,
                                         SQL_RTE_DUMP );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED_EO003;

      return ( ulCommState );

    case RSQL_CTRL_CANCEL_REQUEST_EO003:
      MSGD (( INFO_NISSL_SERV_CANCEL_REQUESTED, ConPktParamRec.ulReceiverRef  ));

      ulCommState = sql96t_cancel_dump ( ConPktParamRec.ulReceiverRef,
                                         ConPktParamRec.szReceiverServerDB,
                                         TRUE,
                                         SQL_RTE_CANCEL );

      if ( ulCommState == commErrOk_esp01 )
        pConnInfo->ulConnStatus = CON_CANCELLED;

      return ( ulCommState );

    case RSQL_CTRL_CONN_REQUEST_EO003:
      MSG8 (( INFO_NISSL_SERV_REC_CONN_REQ, ConPktParamRec.szReceiverServerDB,
              ConPktParamRec.ulSenderRef, ConPktParamRec.ulPacketSize,
              ConPktParamRec.ulMaxDataLen ));

      ulCommState = eo908_Connect( "dbmsrv.exe",
                                   ConPktParamRec.szServerDBRoot,
                                   ConPktParamRec.szReceiverServerDB,
                                   &pConnInfo->pDBM,
                                   &pConnInfo->pCommPacket,
                                   &pConnInfo->ulMaxCmdDataLen,
                                   &TransferBufferSize,
                                   &pConnInfo->ulServerRef, 
                                   (SAPDB_Char const *)&pConnInfo->szSenderNodeID,
                                   ConPktParamRec.pidSenderPID);


      if ( ulCommState == commErrOk_esp01 )
        {
/*
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
*/
          {
          pConnInfo->ulSenderRef      = ConPktParamRec.ulSenderRef;
          pConnInfo->ulMaxSegmentSize = ConPktParamRec.ulMaxSegmentSize;
          pConnInfo->fAcknowledge     = ConPktParamRec.fAcknowledge;
          pConnInfo->ulSwapType       = ConPktParamRec.ulSwapType;
          pConnInfo->ulServiceType    = ConPktParamRec.ulServiceType;
          pConnInfo->ulConnStatus     = CON_CONNECTED;
         
          // - store server reference
          if ( !sql09c_put_handle_data ( gxr.NISSLCoord.pHandles,
                                         pConnInfo->ulConnInfoHandle,
                                         pConnInfo,
                                         (ULONG)UNDEF,
                                         pConnInfo->ulServerRef ))
            {
            MSGD (( ERR_XSER_GET_SET_HANDLE_DATA ));
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
      ConPktParamRec.ulMessClass           = RSQL_PING_REPLY_EO003;
      SAPDB_memcpy(ConPktParamRec.szVersion, gxr.RTEVersionString, sizeof(ConPktParamRec.szVersion) );
      MSG8(( INFO_NISSL_SERV_PING_FROM, ConPktParamRec.szVersion ));
#ifdef HAS_CONVERT_TO_PING_REPlY_VERSION_STRING
      sp100_ConvertToPingReplyVersionString(ConPktParamRec.szVersion);
#endif
      ConPktParamRec.ulCommState           = commErrOk_esp01;
      break;

    default :
      MSGD (( ERR_NISSL_SERV_ILL_FRST_MESS_CLASS,
                ConPktParamRec.ulMessClass ));
      ulCommState                = commErrNotOk_esp01;
      ConPktParamRec.ulCommState = commErrNotOk_esp01;
      break;
    }

    if ( ulCommState != commErrOk_esp01 )            // - do not overwrite 'ulCommState'
        eo420SendConnectPacket ( pConnInfo->pSSLHandle, (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend,
        &ConPktParamRec,  pErrText );
    else
    {
        ulCommState = eo420SendConnectPacket(  pConnInfo->pSSLHandle, (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend,
            &ConPktParamRec, pErrText );

        if ( ulCommState != commErrOk_esp01 )
            MSGD (( ERR_NISSL_SERV_SEND_REQ_PACKET_ERR ));
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG os964NiSSLTransfer ( PTCPIP_SERV_CONNECT_INFO_REC pConnInfo )
  {
  #undef  MF__
  #define MF__ MOD__"os964NiSSLTransfer"
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

    ulCommState = eo420ReceiveCommPacket ( pConnInfo->pSSLHandle,
                                           (RECV_PKT_FUNC_PTR_EO003)RTESec_SAPSSLReceive,
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

      MSGD (( ERR_NISSL_SERV_RECV_PACKET, pConnInfo->ulServerRef, pErrText ));
      break;
      }
    else if ( ! ulDataLen )
      break;
    else if (( ulMessClass != RSQL_KERN_DATA_REQUEST_EO003 ) &&
             ( ulMessClass != RSQL_USER_DATA_REQUEST_EO003 ))
      {
      MSGD ((ERR_NISSL_SERV_ILL_MESS_CLASS, ulMessClass, pConnInfo->ulServerRef));
      break;
      }

    MSG8 (( INFO_NISSL_SERV_READ_REQ_PACKET, ulDataLen, pConnInfo->ulServerRef ));

    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_COMMAND_SEND;

    if ( pConnInfo->pDBM != NULL )
    {
      ulCommState = eo908_Request( pConnInfo->pDBM, ulDataLen );

      if ( ulCommState != commErrOk_esp01 )
      {
        if ( ulCommState == commErrReleased_esp01 )
          ulCommState = commErrOk_esp01;
        break;
      }
      else if ( ! ulDataLen )
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

                ulCommState = eo420SendCommPacket ( pConnInfo->pSSLHandle, (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend,
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
                    MSGD (( ERR_NISSL_SERV_SEND_PACKET, pConnInfo->ulServerRef, pErrText ));
                    break;
                }
                MSG8 (( INFO_NISSL_SERV_SEND_REPLY_PACKET, ulResDataLen, pConnInfo->ulServerRef ));

                continue;
            }

            ulCommState = sql33c_request( &pConnInfo->Shm, 0, ulDataLen, RTEFlags, pErrText );

            if ( ulCommState != commErrOk_esp01 )
            {
                MSGD (( ERR_NISSL_SERV_REQUEST, pConnInfo->ulServerRef, pErrText ));
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
  
      if ( ulCommState != commErrOk_esp01 )
      {
        MSGD (( ERR_NISSL_SERV_REQUEST, pConnInfo->ulServerRef, pErrText ));
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
        break;
    }
    else
    {
      ulCommState = sql33c_receive( &pConnInfo->Shm, &pResCommPacket,
                                    &ulResDataLen, pErrText );

      if ( ulCommState != commErrOk_esp01 )
        {
        MSGD (( ERR_NISSL_SERV_RECEIVE, pConnInfo->ulServerRef, pErrText ));
        break;
        }
    }

    MSG8 (( INFO_NISSL_SERV_RECEIVE_RETURNED,
            pConnInfo->ulServerRef, ulCommState ));

    pConnInfo->ThrdCtrlHeader.ThrdState = THRD_DATA_SEND;

    ulCommState = eo420SendCommPacket ( pConnInfo->pSSLHandle, (SEND_PKT_FUNC_PTR_EO003)RTESec_SAPSSLSend,
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
      MSGD (( ERR_NISSL_SERV_SEND_PACKET, pConnInfo->ulServerRef, pErrText ));
      break;
      }

    MSG8 (( INFO_NISSL_SERV_SEND_REPLY_PACKET,
            ulResDataLen, pConnInfo->ulServerRef ));
    }
  while (ulCommState == commErrOk_esp01) ;

  pConnInfo->ThrdCtrlHeader.ThrdState = THRD_RUNNING;

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG sql96t_cancel_dump ( ULONG                        ulServerRef,
                                  PSZ                          pszServerDB,
                                  BOOL                         DBMConnect,
                                  ULONG                        ulReqType )
  {
  #undef  MF__
  #define MF__ MOD__"sql96t_cancel_dump"
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
    if (( !sql09c_find_handle_by_id ( gxr.NISSLCoord.pHandles,
                                      ulServerRef, &ulConnInfoHandle )) ||
        ( !sql09c_lock_handle ( gxr.NISSLCoord.pHandles, ulConnInfoHandle )) )
      {
      return ( commErrOk_esp01 );
      }

    if ( sql09c_get_handle_data  ( gxr.NISSLCoord.pHandles,
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

    if ( !sql09c_unlock_handle ( gxr.NISSLCoord.pHandles, ulConnInfoHandle ))
      {
      MSGD (( ERR_XSER_LOCK_UNLOCK_HANDLE ));
      gxr.ulCrashRc = ERROR_INVALID_HANDLE;
      ABORT();
      }

    break;
    }

  MSG5 (( INFO_NISSL_SERV_CANCEL_RETURNING, ulCommState ))

  return ( ulCommState );
  }

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
