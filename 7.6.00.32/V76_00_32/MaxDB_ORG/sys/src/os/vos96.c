/*!
  @file           vos96.c
  @author         RaymondR
  @brief          Server - common routines for servers
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
#include "gos00.h"
#include "gos96.h"
#include "geo007_1.h"
#include "geo43_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MAXTCPIPCONNECTIONS       32768
#define LINGER_TIME               30

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



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void _System os96TCPIPCoordinator ( TCPIP_COORD_CTRL_REC *pTCPIPCoord );


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void
os96InitConPktParamRec ( teo003_ConPktParamRecord *pConPktParamRec )
{
  #undef  MF__
  #define MF__ MOD__"os96InitConPktParamRec"

  pConPktParamRec->ulMessClass           = undef_ceo00;
  pConPktParamRec->ulSenderRef           = undef_ceo00;
  pConPktParamRec->ulReceiverRef         = undef_ceo00;
  pConPktParamRec->ulServiceType         = undef_ceo00;
  pConPktParamRec->ulMaxSegmentSize      = undef_ceo00;
  pConPktParamRec->ulPacketSize          = undef_ceo00;
  pConPktParamRec->ulMaxDataLen          = undef_ceo00;
  pConPktParamRec->ulMinReplySize        = undef_ceo00;
  pConPktParamRec->ulSwapType            = undef_ceo00;
  pConPktParamRec->szSenderServerDB[0]   = '\0';
  pConPktParamRec->szReceiverServerDB[0] = '\0';
  pConPktParamRec->pidSenderPID          = undef_ceo00;
  pConPktParamRec->usServicePort         = undef_ceo00;
  pConPktParamRec->ulCommState           = commErrOk_esp01;
  pConPktParamRec->fAcknowledge          = FALSE;
  pConPktParamRec->fOmitReplyPart        = false;
}

/*------------------------------*/
void os96InitLocalConnectParamRec(teo003_ConnectParamRecord    *pLocConnParam,
                                  teo003_ConPktParamRecord     *pConPktParamRec,
                                  TCPIP_SERV_CONNECT_INFO_REC  *pConnInfo )
{                                  
  memset ( pLocConnParam, 0, sizeof(CONNECT_PARAM_REC) );
  pLocConnParam->ulPacketCnt       = 1;
  pLocConnParam->ulClientRef       = pConPktParamRec->ulSenderRef;
  pLocConnParam->ulServiceType     = pConPktParamRec->ulServiceType;
  pLocConnParam->pWorldSA          = pConnInfo->pWorldSA;
  pLocConnParam->pLockSA           = pConnInfo->pLockSA;
  pLocConnParam->ulConnTimeout     = LOCAL_CONN_TIMEOUT;
  pLocConnParam->pszServerDB       = pConPktParamRec->szReceiverServerDB;
  pLocConnParam->pszClientNode     = pConnInfo->szSenderNodeID;
  pLocConnParam->ulPacketSize      = pConPktParamRec->ulPacketSize;
  pLocConnParam->ulMaxDataLen      = pConPktParamRec->ulMaxDataLen;
  pLocConnParam->ulMinReplySize    = pConPktParamRec->ulMinReplySize;
  pLocConnParam->pidClientPID      = pConPktParamRec->pidSenderPID;
  pLocConnParam->ulCommFlagNo      = undef_ceo00;
  pLocConnParam->ulServerRef       = undef_ceo00;
  pLocConnParam->encryption        = RTE_NO_ENCRYPTION;
}

/*------------------------------*/

tsp00_Uint     
os96OpenListeningSocket ( char           *pszService,
                          tsp00_Uint2    *pusServicePort,
                          teo00_Socket   *sd,
                          tsp00_Uint     *pulMaxSegmentSize )
  {
  #undef  MF__
  #define MF__ MOD__"os96OpenListeningSocket"
  LONG                           rc;
  ULONG                          ulMinSegmentSize;
  ULONG                          ulCommState = commErrOk_esp01;
  DBGIN;

  *sd = INVALID_SOCKET;

  // --- create server socket as overlapped due to problems at shutdown () otherwise
  rc = sql40c_stream_socket2 ( sd, WSA_FLAG_OVERLAPPED);

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_LIS_SOCK_OPEN, rc ));
    DBGOUT;
    return ( rc );
    }

  SetHandleInformation ( (HANDLE)*sd, HANDLE_FLAG_INHERIT, 0 );

  rc =  sql40c_keep_alive ( *sd );

  if ( rc != NO_ERROR )
    MSGCD (( ERR_XSER_KEEP_ALIV_ERROR, rc ))

  MSG6 (( INFO_XSER_OP_LISEN_SOCKET, *sd ));


  *pulMaxSegmentSize = sizeof(RTE_HEADER_REC) +
                       sizeof(RTE_CONNECT_PACKET_REC);
  ulMinSegmentSize   = sizeof(RTE_HEADER_REC);

  ulCommState = os96SetSockBufSizes( *sd,
                                      ulMinSegmentSize,
                                      pulMaxSegmentSize );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql40c_socket_close ( *sd );
    *sd                = INVALID_SOCKET;
    *pulMaxSegmentSize = 0 ;
    DBGOUT;
    return ( ulCommState );
    }

  if ( false == gxr.fPortableInstallation )
  {
      // --- bind the socket to the server address
      rc = sql40c_bind ( *sd, *pusServicePort );
  }
  else
  {
      // --- bind the socket to the server address
      rc = sql40c_bind_on_port_and_inet_addr ( *sd, *pusServicePort, "127.0.0.1" );
  }

  if ( rc != NO_ERROR )
    {
    if ( rc == SOCEADDRINUSE )
      {
      MSGALL (( ERR_XSER_SOCK_ADDR_IN_USE ));
      }
    else
      MSGALL (( ERR_XSER_BIND_ERROR, rc ));

    sql40c_socket_close ( *sd );
    *sd = INVALID_SOCKET;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  rc = sql40c_listen ( *sd, SOMAXCONN );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_LISTEN_ERROR, rc ));

    sql40c_socket_close ( *sd );
    *sd = INVALID_SOCKET;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  MSG6 (( INFO_XSER_LISTEN_ON_SOCKET, *sd ));

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

tsp00_Uint    
os96SetSockBufSizes( teo00_Socket    sd,
                     tsp00_Uint      ulMinSegmentSize,
                     tsp00_Uint      *pulMaxSegmentSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql96t_set_sock_buf_sizes"
  LONG                           rc;

  DBGIN;

  //
  // --- set send buffer size
  //
  rc = sql40c_set_send_buffer_size ( sd, pulMaxSegmentSize,
                                     ulMinSegmentSize );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_BUF_SIZE, "SNDBUF", rc ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- set receive buffer size
  //
  rc = sql40c_set_recv_buffer_size ( sd, pulMaxSegmentSize,
                                     ulMinSegmentSize );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_BUF_SIZE, "RCVBUF", rc ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  MSG6 (( INFO_XSER_SEGMENT_SIZES, sd, ulMinSegmentSize, *pulMaxSegmentSize ));

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

tsp00_Int    
os96StartTCPIPCoord ( TCPIP_COORD_CTRL_REC *pTCPIPCoord )
{
    LONG                    rc        = NO_ERROR;

    pTCPIPCoord->ThrdCtrlHeader.Tid         = (TID)undef_ceo00;
    pTCPIPCoord->ThrdCtrlHeader.hThrd       = (HANDLE)INVALID_HANDLE_VALUE;
    pTCPIPCoord->ThrdCtrlHeader.lThrdPrio   = SERVER_LOOP_THRD_PRIO;
    pTCPIPCoord->ThrdCtrlHeader.ThrdState   = THRD_INITIALIZED;
    pTCPIPCoord->ThrdCtrlHeader.ulStartTime = (ULONG)undef_ceo00;

    rc = CREATE_THREAD( &pTCPIPCoord->ThrdCtrlHeader.Tid,
                        &pTCPIPCoord->ThrdCtrlHeader.hThrd,
                        os96TCPIPCoordinator,
                        pTCPIPCoord,
                        CREATE_THRD_SUSPENDED,
                        pTCPIPCoord->ThrdCtrlHeader.StackSize );

    if ( rc == NO_ERROR )
      {
      rc = RESUME_THREAD( pTCPIPCoord->ThrdCtrlHeader.Tid,
                          pTCPIPCoord->ThrdCtrlHeader.hThrd );

      if ( rc != NO_ERROR )
        MSGALL (( ERR_XSER_RESUMING_THREAD, "COORDINATOR", rc));
      }
    else
      MSGALL (( ERR_XSER_CREATING_THREAD, "COORDINATOR", rc));

    return ( rc ) ;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static void _System 
os96TCPIPCoordinator ( TCPIP_COORD_CTRL_REC *pTCPIPCoord )
  {
  #undef  MF__
  #define MF__ MOD__"os96TCPIPCoordinator"
  sockaddr_type                  SockAddr;
  SQL_NODEIDC                    szSenderNodeID;
  ULONG                          ulConnInfoHandle;
  APIRET                         rc        = NO_ERROR;
  ULONG                          ulErrCnt  = 0;
  INT                            AddrLen   = sizeof(SockAddr);
  teo00_Socket                   sd        = UNDEF;
  PTCPIP_SERV_CONNECT_INFO_REC   pConnInfo = NULL;

  DBGIN;

  __try
  {
  // --- set thread control info
  pTCPIPCoord->ThrdCtrlHeader.ulStartTime = time (NULL);

  //
  //
  // --- set the thread priority
  //
  //
  rc = SET_THREAD_PRIO ( pTCPIPCoord->ThrdCtrlHeader.lThrdPrio,
                         pTCPIPCoord->ThrdCtrlHeader.Tid,
                         pTCPIPCoord->ThrdCtrlHeader.hThrd );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_THRD_PRIO_ERROR,
             pTCPIPCoord->ThrdCtrlHeader.Tid, rc ));
    pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
    gxr.ulCrashRc                           = rc;
    ABORT();
    }

  if( !sql09c_init_handle_dir( &pTCPIPCoord->pHandles, MAXTCPIPCONNECTIONS ))
    {
    MSGALL (( ERR_RESERVE_HANDLE_VALUES ));
    pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
    gxr.ulCrashRc                           = ERROR_INVALID_HANDLE;
    ABORT();
    }

  rc = os96OpenListeningSocket ( pTCPIPCoord->pServiceStr,
                                &pTCPIPCoord->usServicePort, 
                                &pTCPIPCoord->sd, 
                                &pTCPIPCoord->ulMaxSegmentSize );

  if ( rc != NO_ERROR )
    {
    pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
    gxr.ulCrashRc                           = rc;
    ABORT();
    }


  for (;;)
    {
    MSG6 (( INFO_XSER_ACCEPTING ));

    pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_SLEEPING;

    rc = sql40c_accept_conn_request ( pTCPIPCoord->sd, &sd,
                                      &SockAddr, &AddrLen );

    pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_RUNNING;

    if ( (rc != NO_ERROR) || (sd == INVALID_SOCKET) )
      {
      if ( rc == SOCEINTR ) continue;

      MSGALL (( ERR_XSER_ACCEPT_ERROR, rc ));

      if ( ( (++ulErrCnt) > 9 ) || ( rc == SOCENOTSOCK ) )
        {
        //
        // --- try to reopen the listen socket
        //
        sql40c_socket_close ( pTCPIPCoord->sd );
        pTCPIPCoord->sd = INVALID_SOCKET;
        ulErrCnt          = 0;
        AddrLen           = sizeof(SockAddr);

        SLEEP( 30000 );

        rc = os96OpenListeningSocket ( pTCPIPCoord->pServiceStr,
                                      &pTCPIPCoord->usServicePort, 
                                      &pTCPIPCoord->sd, 
                                      &pTCPIPCoord->ulMaxSegmentSize );

        if ( rc != NO_ERROR )
          break;
        }
      continue;
      }
    else
      ulErrCnt = 0;

    MSG7 (( INFO_XSER_ACCEPT_CONNECTION ));

    if ( gxr.fNoDNSReverseLookup == FALSE )
      {
      sql43_get_host_by_address ( (PUCHAR)&SockAddr.sa_data[2],
                                   szSenderNodeID, sizeof(szSenderNodeID));
      }
    else
      {
      char* ptr = (char*) &SockAddr.sa_data[2];

      _snprintf ( szSenderNodeID, sizeof(szSenderNodeID), "%d.%d.%d.%d" ,
                  ptr[0]&0xff , ptr[1]&0xff , ptr[2]&0xff , ptr[3]&0xff );
      }

    MSG7 (( INFO_XSER_ACCEPT_CONNECTION_FROM_CLIENT, sd, szSenderNodeID ));

    SetHandleInformation ( (HANDLE)sd, HANDLE_FLAG_INHERIT, 0 );

    rc =  sql40c_keep_alive ( sd );

    if ( rc != NO_ERROR )
      MSGCD (( ERR_XSER_KEEP_ALIV_ERROR, rc ))

    rc =  sql40c_linger ( sd, TRUE, LINGER_TIME );

    if ( rc != NO_ERROR )
      MSGCD (( ERR_LINGER, rc ))

    // --- allocate space for the internal communication structure
    rc = ALLOC_MEM((PPVOID)&pConnInfo, sizeof(TCPIP_SERV_CONNECT_INFO_REC));

    if ( rc != NO_ERROR )
      {
      sql40c_socket_close ( sd );
      continue;
      }

    // --- Get a new handle and combine it with the pointer to the
    //     internal communication structure.
    //
    if ( !sql09c_reserve_handle ( pTCPIPCoord->pHandles, &ulConnInfoHandle) )
      {
      MSGALL (( ERR_XSER_MAX_CONNECTIONS ));
      sql40c_socket_close ( sd );
      FREE_MEM ( pConnInfo );
      continue;
      }

    if ( !sql09c_put_handle_data( pTCPIPCoord->pHandles, ulConnInfoHandle,
                                  pConnInfo, (ULONG)UNDEF, (ULONG)UNDEF))
      {
      MSGALL (( ERR_XSER_GET_SET_HANDLE_DATA ));
      gxr.ulCrashRc = ERROR_INVALID_HANDLE;
      ABORT();
      }

    pConnInfo->ThrdCtrlHeader.Tid         = (TID)UNDEF;
    pConnInfo->ThrdCtrlHeader.hThrd       = (HANDLE)INVALID_HANDLE_VALUE;
    pConnInfo->ThrdCtrlHeader.lThrdPrio   = SERVER_THRD_PRIO;
    pConnInfo->ThrdCtrlHeader.ThrdState   = THRD_INITIALIZED;
    pConnInfo->ThrdCtrlHeader.ulStartTime = (ULONG)UNDEF;

    pConnInfo->ulConnInfoHandle           = ulConnInfoHandle;
    pConnInfo->sd                         = sd;
    strcpy ( pConnInfo->szSenderNodeID, szSenderNodeID );

    pConnInfo->ulConnStatus               = CON_INITIALIZED;
    pConnInfo->ulServerRef                = (ULONG)UNDEF;
    pConnInfo->ulMaxCmdDataLen            = (ULONG)UNDEF;
    pConnInfo->pCommPacket                = NULL;

    pConnInfo->fAcknowledge               = FALSE;
    pConnInfo->ulSwapType                 = (ULONG)UNDEF;
    pConnInfo->ulMaxSegmentSize           = (ULONG)UNDEF;

    #if defined(_WIN32)
     pConnInfo->pWorldSA                  = &gxr.WorldSA;
     pConnInfo->pLockSA                   = &gxr.LockSA;
    #endif

    rc = CREATE_THREAD( &pConnInfo->ThrdCtrlHeader.Tid,
                        &pConnInfo->ThrdCtrlHeader.hThrd,
                         pTCPIPCoord->pTCPIPWorkerThreadFkt,
                         (tsp00_Longuint)ulConnInfoHandle,
                         CREATE_THRD_SUSPENDED,
                         SERVER_STACK_SIZE);

    if (rc != NO_ERROR)
      {
      MSGALL (( ERR_XSER_CREATING_THREAD, gxr.ServerSpecification.ServerName, rc));

      sql40c_socket_close ( sd );
      sql09c_free_handle  ( pTCPIPCoord->pHandles, ulConnInfoHandle );
      FREE_MEM ( pConnInfo );
      }
    else
      {
      rc = RESUME_THREAD(pConnInfo->ThrdCtrlHeader.Tid,
                         pConnInfo->ThrdCtrlHeader.hThrd);

      if (rc != NO_ERROR)
        {
        MSGALL (( ERR_XSER_RESUMING_THREAD, gxr.ServerSpecification.ServerName, rc));

        KILL_THREAD (pConnInfo->ThrdCtrlHeader.Tid,
                     pConnInfo->ThrdCtrlHeader.hThrd);
        sql40c_socket_close ( sd );
        sql09c_free_handle  ( pTCPIPCoord->pHandles, ulConnInfoHandle );
        FREE_MEM ( pConnInfo );
        }
      }
    }

  pTCPIPCoord->ThrdCtrlHeader.ThrdState = THRD_TERMINATED;
  gxr.ulCrashRc                           = rc;
  ABORT();

   } 
  __except( os968ExceptionHandler(GetExceptionInformation()) ){;}

  DBGOUT;
  EXITTHREAD( rc, pTCPIPCoord->ThrdCtrlHeader.hThrd );
  return;
  }

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
