/*!
  @file           vos33.c
  @author         RaymondR
  @brief          local communication
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

#include "RunTime/System/RTESys_MemoryBarrier.h"

/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"
#include "gos003.h"
#include "hos33.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "heo420.h"
#include "gos41.h"
#include  "hsp100.h"
#include  "RunTime/RTE_KSS.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS33C : "
#define MF__   MOD__"UNDEFINED"

#define ALIVE_LOOKUP_TIME     120


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

ULONG  sql33c_open_queue_mailslot    ( PSZ                   pszQueName,
                                       BOOL                  fConnectToXServer,
                                       PHANDLE               phQueMailSlot,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_write_queue_mailslot   ( HANDLE                hQueMailSlot,
                                       PSZ                   pszClientObjName,
                                       ULONG                 ulReqType,
                                       ULONG                 ulServerRef,
                                       ULONG                 ulCSSize,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_close_queue_mailslot   ( HANDLE                hQueMailSlot,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_create_client_sem      ( PSZ                   pszSemPath,
                                       PSZ                   pszClientObjName,
                                       PHANDLE               phClientSem,
                                       PSECURITY_ATTRIBUTES  pWorldSA,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_wait_client_sem        ( HANDLE                hClientSem,
                                       ULONG                 ulTimeOut,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_close_client_sem       ( HANDLE                hClientSem,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_open_server_sem        ( PSZ                   pszSemPath,
                                       PSZ                   pszSemName,
                                       PHANDLE               phServerSem,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_post_server_sem        ( HANDLE                hServerSem,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_close_server_sem       ( HANDLE                hServerSem,
                                       tsp00_ErrTextc        pErrText );

ULONG sql33c_close_all_server_sems   ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_try_open_server_process( PID                   pidServerPID,
                                       PHANDLE               hServerProcess,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_close_server_process   ( HANDLE                hServerProcess,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_create_comm_seg        ( ULONG                 ulCSSize,
                                       PSZ                   pszClientObjName,
                                       PSECURITY_ATTRIBUTES  pWorldSA,
                                       PHANDLE               phCS,
                                       PCOMM_SEG_HEADER_REC  *ppCSHeader,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_lock_comm_seg          ( HANDLE                hCS,
                                       PSECURITY_ATTRIBUTES  pLockSA,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_free_comm_seg          ( HANDLE                hCS,
                                       PCOMM_SEG_HEADER_REC  pCSHeader,
                                       tsp00_ErrTextc        pErrText );

VOID   sql33c_init_comm_seg          ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       tsp100_VersionID2*    pKernelVersion,
                                       PSZ                   pszClientObjName,
                                       PSZ                   pszSemPath,
                                       PSZ                   pszSemName,
                                       tsp00_ErrTextc        pErrText );

BOOL  sql33c_VersionIsOlder          ( tsp100_VersionID2*    pVersion,
                                       unsigned char         majorVersion,
                                       unsigned char         minorVersion,
                                       unsigned char         corrLevel,
                                       unsigned char         buildNumberPrefix );

ULONG sql33c_get_flag_comm_seg       ( PSZ                   pszFCSName,
                                       PHANDLE               phFCS,
                                       PFLAG_COMM_SEG        *ppFCS,
                                       tsp00_ErrTextc        pErrText );

ULONG sql33c_free_flag_comm_seg      ( HANDLE                hFCS,
                                       PFLAG_COMM_SEG        pFCS,
                                       tsp00_ErrTextc        pErrText );

VOID  sql33c_calculate_sizes         ( ULONG                 ulPacketCnt,
                                       ULONG                 ulMaxPacketSize,
                                       PULONG                pulPacketSize,
                                       PULONG                pulMaxDataLen,
                                       PULONG                pulCSSize );

ULONG sql33c_eval_conn_reply         ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       PCONNECT_PARAM_REC    pConnParam,
                                       tsp00_ErrTextc        pErrText );

ULONG sql33c_eval_xserv_conn_reply   ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       PCONNECT_PARAM_REC    pConnParam,
                                       tsp00_ErrTextc        pErrText );

ULONG sql33c_connect_wait            ( HANDLE                hClientSem,
                                       ULONG                 ulServiceType,
                                       ULONG                 ulTimeOut,
                                       HANDLE                hServerProcess,
                                       PCOMM_SEG_HEADER_REC  pCSHeader,
                                       tsp00_ErrTextc        pErrText );

VOID  sql33c_connect_cleanup         ( PSHM_CONNECT_INFO_REC pShmConnInfo );
ULONG sql33c_init_cleanup_conn_info  ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       PCONNECT_PARAM_REC    pConnParam,
                                       ULONG                 ulNumOfServerSems,
                                       HANDLE                hFCS,
                                       FLAG_COMM_SEG         *pFCS,
                                       tsp00_ErrTextc        pErrText );

ULONG  sql33c_open_server_sems       ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                       tsp00_ErrTextc        pErrText );

ULONG sql33c_get_kernel_pid_and_version( PSZ                     pszServerDB,
                                         PID*                    ppidServer,
                                         tsp100_VersionID2*      pVersion,
                                         tsp00_ErrTextc          pErrText );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

#if defined(SERVER)
 ULONG  sql33c_info_request ( PSZ                     pszServerDB,
                              ULONG                   ulServiceType,
                              ULONG                   ulPacketCnt,
                              SAPDB_Bool              omitReplyPart,
                              PULONG                  pulPacketSize,
                              PULONG                  pulMaxDataLen,
                              PULONG                  pulMinReplySize,
                              PCHAR                   authenticationAllow,
                              ULONG                   authenticationAllowBufSize,
                              tsp00_ErrTextc          pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_info_request"
   ULONG                           ulCommState = commErrOk_esp01;
   HANDLE                          hFCS;
   PFLAG_COMM_SEG                  pFCS;
   SQL_DBNAMEC                     szServerDB;
   ULONG                           ulDummy = 0;
   tsp100_VersionID2               KernelVersion;
   PID                             pidServer;

   DBGIN;

   // --- leave the orignal database name as it is!
   strcpy ( szServerDB,   pszServerDB );
   strupr ( szServerDB );

   ulCommState = sql33c_get_flag_comm_seg ( szServerDB, &hFCS,
                                            &pFCS, pErrText );
   if ( ulCommState != commErrOk_esp01 )
     {
     // --- kernel might be inactive!
     DBGOUT;
     return ( ulCommState );
     }

   ulCommState = sql33c_get_kernel_pid_and_version ( szServerDB, &pidServer, &KernelVersion, pErrText );

   if ( ulCommState != commErrOk_esp01 )
     {
     DBGOUT;
     return ( ulCommState );
     }

   sql33c_calculate_sizes ( ulPacketCnt, pFCS->ulPacketSize,
                            pulPacketSize, pulMaxDataLen, &ulDummy );

   if ( ulServiceType == srvDistribution_esp01 )
   {
     *pulMinReplySize = 0;
   }
   else
   {
        if  ( sql33c_VersionIsOlder( &KernelVersion, 7,6,0,11 ))
            strcpy ( authenticationAllow, "BASIC" );
        else
        {
           strncpy ( authenticationAllow, pFCS->AuthenticationAllow, authenticationAllowBufSize - 1 );
           authenticationAllow[authenticationAllowBufSize - 1] = '\0';
        }

        if ( omitReplyPart )
        {   //Client wants to omit reply part ... 
            *pulMinReplySize = sql33c_VersionIsOlder( &KernelVersion, 7,6,0,10 ) ? pFCS->ulMinReplySize : 0;
        }
        else
        {
            *pulMinReplySize = pFCS->ulMinReplySize;
        }
   }

   ulCommState = sql33c_free_flag_comm_seg ( hFCS, pFCS, pErrText );

   if ( ulCommState != commErrOk_esp01 )
     {
     DBGOUT;
     return ( ulCommState );
     }

   if ( *pulPacketSize == 0 )
     {
     MSGD (( ERR_INVALID_PACKET_SIZE, *pulPacketSize ));
     eo46BuildErrorStringRC ( pErrText, ERRMSG_INVALID_PACKET_SIZE, 0 );
     ulCommState = commErrNotOk_esp01;
     }

   DBGOUT;
   return ( ulCommState );
   }
#endif

/*------------------------------*/

ULONG  sql33c_connect ( PCONNECT_PARAM_REC      pConnParam,
                        PSHM_CONNECT_INFO_REC   pShmConnInfo,
                        tsp00_ErrTextc          pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_connect"
  IPC_BASE_OBJ_NAME               szClientObjName;
  PSZ                             pszClientObjName = szClientObjName;
  PSZ                             pszClientSemPath;
  PSZ                             pszClientSemName;
  ULONG                           ulCommState;
  PID                             pidServer;
  HANDLE                          hFCS;
  FLAG_COMM_SEG                   *pFCS;
  SQL_DBNAMEC                     szServerDB;
  tsp100_VersionID2               KernelVersion;

  DBGIN;

  // --- leave the orignal database name as it is!
  strcpy ( szServerDB,   pConnParam->pszServerDB );
  strupr ( szServerDB );

  ulCommState = sql33c_get_flag_comm_seg ( szServerDB, &hFCS, &pFCS, pErrText );

  if ( ulCommState != commErrOk_esp01 ) // --- kernel/xserver might be inactive!
    {
    DBGOUT;
    return ( ulCommState );
    }

   ulCommState = sql33c_get_kernel_pid_and_version ( szServerDB, &pidServer, &KernelVersion, pErrText );

   if ( ulCommState != commErrOk_esp01 )
     {
     DBGOUT;
     return ( ulCommState );
     }


  // --- initialize the connect information record
  ulCommState = sql33c_init_cleanup_conn_info ( pShmConnInfo, pConnParam, pFCS->ulNumOfSemaphores, hFCS, pFCS, pErrText );

  if ( ulCommState != commErrOk_esp01 )
    {
    sql33c_free_flag_comm_seg( hFCS, pFCS, NULL );
    DBGOUT;
    return ( ulCommState );
    }

  //
  // --- calculate sizes
  //
  sql33c_calculate_sizes ( pShmConnInfo->ulPacketCnt,
                           pFCS->ulPacketSize,
                           &pShmConnInfo->ulPacketSize,
                           &pShmConnInfo->ulMaxDataLen,
                           &pShmConnInfo->ulCSSize );

  if ( pShmConnInfo->ulServiceType == srvDistribution_esp01 )
    pShmConnInfo->ulMinReplySize = 0;
  else
  {
      if  ( sql33c_VersionIsOlder( &KernelVersion, 7,6,0,11 ))
          strcpy ( pConnParam->authenticationAllow, "BASIC" );
      else
      {
          ULONG authenticationAllowBufSize = sizeof( pConnParam->authenticationAllow );
          strncpy ( pConnParam->authenticationAllow, pFCS->AuthenticationAllow, authenticationAllowBufSize - 1 );
          pConnParam->authenticationAllow[authenticationAllowBufSize -1] = '\0';
      }

#if defined(SERVER)
        if ( 0 == pConnParam->ulMinReplySize )
        {   //Client wants to omit reply part ... 
            pShmConnInfo->ulMinReplySize = sql33c_VersionIsOlder( &KernelVersion, 7,6,0,10 ) ? pFCS->ulMinReplySize : 0;
        }
        else
        {
            pShmConnInfo->ulMinReplySize = pFCS->ulMinReplySize;
        }
#else
     pShmConnInfo->ulMinReplySize = sql33c_VersionIsOlder( &KernelVersion, 7,4,4,1 ) ? pFCS->ulMinReplySize : 0;
#endif
  }

  if ( pShmConnInfo->ulPacketSize == 0 )
    {
    MSGD (( ERR_INVALID_PACKET_SIZE, pShmConnInfo->ulPacketSize ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_INVALID_PACKET_SIZE, 0 );
    sql33c_connect_cleanup  ( pShmConnInfo );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }


  ulCommState = sql33c_open_queue_mailslot ( pShmConnInfo->szServerDB,
                                             FALSE,
                                             &pShmConnInfo->hQueMailSlot,
                                             pErrText );

  if ( ulCommState != commErrOk_esp01 )
  {
      sql33c_connect_cleanup  ( pShmConnInfo );
      DBGOUT;
      return ulCommState;
  }

  sql41c_build_unique_obj_name ( pszClientObjName );

  #if defined (KERNEL)
   // --- we are using an external semaphore.
   pShmConnInfo->hClientSem = pConnParam->hClientSem;
   pszClientSemPath         = pConnParam->pszClientSemPath;
   pszClientSemName         = pConnParam->pszClientSemName;
  #else
   pszClientSemPath = SEM_USR;
   pszClientSemName = pszClientObjName;
   ulCommState      = sql33c_create_client_sem ( pszClientSemPath,
                                                 pszClientObjName,
                                                 &pShmConnInfo->hClientSem,
                                                 pConnParam->pWorldSA,
                                                 pErrText );
   if ( ulCommState != commErrOk_esp01 )
     {
     sql33c_connect_cleanup  ( pShmConnInfo );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }
  #endif

  ulCommState = sql33c_create_comm_seg ( pShmConnInfo->ulCSSize,
                                         pszClientObjName,
                                         pConnParam->pWorldSA,
                                         &pShmConnInfo->hCS,
                                         &pShmConnInfo->pCSHeader,
                                         pErrText );
  if ( ulCommState != commErrOk_esp01 )
    {
    sql33c_connect_cleanup  ( pShmConnInfo );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }


  sql33c_init_comm_seg( pShmConnInfo,  &KernelVersion, pszClientObjName,
                        pszClientSemPath, pszClientSemName, pErrText );


  ulCommState = sql33c_write_queue_mailslot( pShmConnInfo->hQueMailSlot,
                                             pszClientObjName,
                                             SQL_RTE_CONNECT,
                                             0,
                                             pShmConnInfo->ulCSSize,
                                             pErrText );

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_try_open_server_process ( pidServer, &pShmConnInfo->hServerProcess, pErrText );

  if ( ulCommState == commErrOk_esp01 )
    {
    ulCommState = sql33c_connect_wait ( pShmConnInfo->hClientSem,
                                        pShmConnInfo->ulServiceType,
                                        pConnParam->ulConnTimeout,
                                        pShmConnInfo->hServerProcess,
                                        pShmConnInfo->pCSHeader,
                                        pErrText );
    }

  // --- is everthing allright on the client side?
  if ( ulCommState == commErrOk_esp01 )
    {
    // --- check if the server has reported an error!
    if (( pShmConnInfo->pCSHeader->ulServerFlag      == 1 ) &&
        ( pShmConnInfo->pCSHeader->ulServerCommState != commErrOk_esp01 ))
      {
      // --- the server has reported an error!
      ulCommState = pShmConnInfo->pCSHeader->ulServerCommState;

      switch (  ulCommState )
        {
        case commErrTasklimit_esp01 :
          if ( pShmConnInfo->ulServiceType == srvUtility_esp01 )
            {
            MSGD (( ERR_UTIL_ALREADY_CONN ));
            eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_UTIL_ALREADY_CONN, 0 );
            }
          else
            {
            MSGD (( ERR_TO_MANY_SESSIONS ));
            eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_TO_MANY_DB_SESSIONS, 0 );
            }
          break;

        case commErrStartRequired_esp01:
          MSGD (( ERR_DATABASE_NOT_STARTED, pShmConnInfo->szServerDB ));
          eo46BuildErrorStringRC ( pErrText,
                                      ERRMSG_COM_DATABASE_NOT_STARTED, 0 );
          break;

        default:
          MSGD (( ERR_CONN_REFUSED ))
          eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_REFUSED, 0 );
          break;
        }
      }
    else if (( pShmConnInfo->pCSHeader->ulServerFlag != 1 ) ||
             ( pShmConnInfo->pCSHeader->ulClientFlag != 2 ))
      {
      MSGD (( ERR_PROTOCOL_ERROR, "sql33c_connect" ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

      ulCommState = commErrNotOk_esp01;
      }
    }

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_lock_comm_seg ( pShmConnInfo->hCS, pConnParam->pLockSA, pErrText );

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_eval_conn_reply( pShmConnInfo, pConnParam, pErrText );

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_open_server_sems ( pShmConnInfo, pErrText );

  // --- is everthing allright?
  if ( ulCommState != commErrOk_esp01 )
    {
    if ( pShmConnInfo->pCSHeader != NULL )
      {
      // --- signal to the server that we lost the connection!
      pShmConnInfo->pCSHeader->ulClientCommState  = commErrCrash_esp01;
      pShmConnInfo->pCSHeader->ulServerFlag       = 2;
      RTESys_WriteMemoryBarrier();
      pShmConnInfo->pCSHeader->ulClientFlag       = 1;
      }

    sql33c_connect_cleanup  ( pShmConnInfo );
    }
  else
    {
    // --- connected successfully!
    pShmConnInfo->pCSHeader->ulClientFlag = 0;
    pShmConnInfo->pCSHeader->ulServerFlag = 2;
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

#if defined(KERNEL)

 ULONG  sql33c_connect_xserver ( PCONNECT_PARAM_REC      pConnParam,
                                 PSHM_CONNECT_INFO_REC   pShmConnInfo,
                                 tsp00_ErrTextc               pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_connect_xserver"
   IPC_BASE_OBJ_NAME               szClientObjName;
   PSZ                             pszClientObjName = szClientObjName;
   PSZ                             pszClientSemPath;
   PSZ                             pszClientSemName;
   ULONG                           ulCommState;
   PID                             pidServer;

   DBGIN;

   // --- initialize the connect information record
   ulCommState = sql33c_init_cleanup_conn_info ( pShmConnInfo, pConnParam, 1, INVALID_HANDLE_VALUE, NULL, pErrText );

   if ( ulCommState != commErrOk_esp01 )
     {
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }

   sql41c_build_unique_obj_name ( pszClientObjName );

   //
   // --- calculate sizes
   //
   sql33c_calculate_sizes ( pShmConnInfo->ulPacketCnt,
                            pfcs->ulPacketSize,
                            &pShmConnInfo->ulPacketSize,
                            &pShmConnInfo->ulMaxDataLen,
                            &pShmConnInfo->ulCSSize );

   pShmConnInfo->ulMinReplySize = 0;

   if ( pShmConnInfo->ulPacketSize == 0 )
     {
     MSGD (( ERR_INVALID_PACKET_SIZE, pShmConnInfo->ulPacketSize ));
     eo46BuildErrorStringRC ( pErrText, ERRMSG_INVALID_PACKET_SIZE, 0 );
     sql33c_connect_cleanup  ( pShmConnInfo );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }


   ulCommState = sql33c_open_queue_mailslot ( XSERVER_OBJ_NAME,
                                              TRUE,
                                              &pShmConnInfo->hQueMailSlot,
                                              pErrText );

   if ( ulCommState != commErrOk_esp01 )
     {
     sql33c_connect_cleanup  ( pShmConnInfo );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }

   // --- we are using an external semaphore.
   pShmConnInfo->hClientSem = pConnParam->hClientSem;
   pszClientSemPath         = pConnParam->pszClientSemPath;
   pszClientSemName         = pConnParam->pszClientSemName;

   ulCommState = sql33c_create_comm_seg ( pShmConnInfo->ulCSSize,
                                         pszClientObjName,
                                         pConnParam->pWorldSA,
                                         &pShmConnInfo->hCS,
                                         &pShmConnInfo->pCSHeader,
                                         pErrText );
   if ( ulCommState != commErrOk_esp01 )
     {
     sql33c_connect_cleanup  ( pShmConnInfo );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }


   sql33c_init_comm_seg( pShmConnInfo, pszClientObjName,
                         pszClientSemPath, pszClientSemName, pErrText );


   ulCommState = sql33c_write_queue_mailslot( pShmConnInfo->hQueMailSlot,
                                             pszClientObjName,
                                             SQL_RTE_CONNECT,
                                             0,
                                             pShmConnInfo->ulCSSize,
                                             pErrText );

  if ( ulCommState == commErrOk_esp01 )
     {
     ulCommState = sql33c_connect_wait ( pShmConnInfo->hClientSem,
                                         pShmConnInfo->ulServiceType,
                                         pConnParam->ulConnTimeout,
                                         INVALID_HANDLE_VALUE,
                                         pShmConnInfo->pCSHeader,
                                         pErrText );
     }

   // --- is everthing allright on the client side?
   if ( ulCommState == commErrOk_esp01 )
     {
     // --- check if the server has reported an error!
     if (( pShmConnInfo->pCSHeader->ulServerFlag      == 1     ) &&
         ( pShmConnInfo->pCSHeader->ulServerCommState != commErrOk_esp01 ))
       {
       // --- the server has reported an error!
       ulCommState = pShmConnInfo->pCSHeader->ulServerCommState;

       switch (  ulCommState )
         {
         case commErrTasklimit_esp01 :
           if ( pShmConnInfo->ulServiceType == srvUtility_esp01 )
             {
             MSGD (( ERR_UTIL_ALREADY_CONN ));
             eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_UTIL_ALREADY_CONN, 0 );
             }
           else
             {
             MSGD (( ERR_TO_MANY_SESSIONS ));
             eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_TO_MANY_DB_SESSIONS, 0 );
             }
           break;

         case commErrStartRequired_esp01:
           MSGD (( ERR_COM_SERVER_OR_DB_NOT_ACC ));
           eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, 0 );
           break;

         default:
           MSGD (( ERR_CONN_REFUSED ))
           eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_REFUSED, 0 );
           break;
         }
       }
     else if (( pShmConnInfo->pCSHeader->ulServerFlag != 1 ) ||
              ( pShmConnInfo->pCSHeader->ulClientFlag != 2 ))
       {
       MSGD (( ERR_PROTOCOL_ERROR, "sql33c_connect_xserver" ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

       ulCommState = commErrNotOk_esp01;
       }
     }

   if ( ulCommState == commErrOk_esp01 )
     {
     ulCommState = sql33c_lock_comm_seg ( pShmConnInfo->hCS,
                                          pConnParam->pLockSA,
                                          pErrText );
     }


   if ( ulCommState == commErrOk_esp01 )
     {
     ULONG ulSemaNo = pShmConnInfo->pCSHeader->ulSemaNo;

     ulCommState = sql33c_open_server_sem ( pShmConnInfo->pCSHeader->szSemPath,
                                            pShmConnInfo->pCSHeader->szSemName,
                                            &pShmConnInfo->phServerSems[ulSemaNo],
                                            pErrText );
     }


   if ( ulCommState == commErrOk_esp01 )
     {
     ulCommState = sql33c_eval_xserv_conn_reply ( pShmConnInfo, pConnParam,
                                                  pErrText);
     }

   // --- is everthing allright?
   if ( ulCommState != commErrOk_esp01 )
     {
     if ( pShmConnInfo->pCSHeader != NULL )
       {
       // --- signal to the server that we lost the connection!
       pShmConnInfo->pCSHeader->ulClientCommState  = commErrCrash_esp01;
       pShmConnInfo->pCSHeader->ulServerFlag       = 2;
       RTESys_WriteMemoryBarrier();
       pShmConnInfo->pCSHeader->ulClientFlag       = 1;
       }

     sql33c_connect_cleanup  ( pShmConnInfo );
     }
   else
     {
     // --- connected successfully!
     pShmConnInfo->pCSHeader->ulClientFlag = 0;
     pShmConnInfo->pCSHeader->ulServerFlag = 2;
     }

   DBGOUT;
   return ( ulCommState );
   }

#endif

/*------------------------------*/

ULONG  sql33c_request ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                        ULONG                   ulCmdPacket,
                        ULONG                   ulCmdDataLen,
                        tsp00_Uint1             RTEFlags,
                        tsp00_ErrTextc          pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_request"
  ULONG                           rc          = NO_ERROR;
  PCOMM_SEG_HEADER_REC            pCSHeader;
  PCOMM_PACKET_REC                pCommPacket = NULL;
  ULONG                           ulCommState;
  ULONG                           ulMessClass;
  ULONG							  ulSemaNo; 

  DBGIN;

  pCSHeader                     = pShmConnInfo->pCSHeader;
  pCommPacket                   = pShmConnInfo->pCommPacketList[ulCmdPacket];
  pShmConnInfo->ulCurrentPacket = ulCmdPacket;
  pShmConnInfo->ulCmdDataLen    = ulCmdDataLen;

  DBG3 (( MF__, "ulServerCommState: %d", pCSHeader->ulServerCommState  ));
  DBG3 (( MF__, "ulClientCommState: %d", pCSHeader->ulClientCommState  ));
  DBG3 (( MF__, "ulServerFlag     : %d", pCSHeader->ulServerFlag ));
  DBG3 (( MF__, "ulClientFlag     : %d", pCSHeader->ulClientFlag ));

  // --- ClientFlag must be 0
  #if !defined (KERNEL)
   if ( pCSHeader->ulClientFlag != 0 )
     {
     MSGD (( ERR_UNBAL_REQUEST_REPLY ));
     eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_UNBALANCED_REQUEST_REPLY,
                                0 );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }
  #endif

  if (( pCSHeader->ulServerRef       != pShmConnInfo->ulServerRef ) ||
      ( pCSHeader->ulServerCommState != commErrOk_esp01 )                     ||
      ( pCSHeader->ulServerFlag      == 1 ))
    {
    if ( pCSHeader->ulServerCommState == commErrTimeout_esp01 )
      {
      MSGD (( INFO_COM_TIMEOUT ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_TIMEOUT, 0 );
      }
    else
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, 0 );

    if ( pCSHeader->ulServerCommState != commErrOk_esp01 )
      {
      DBGOUT;
      return ( pCSHeader->ulServerCommState );
      }
    else
      {
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  //
  // --- Setup the RTE header.
  //
  if ( pShmConnInfo->ulServiceType == srvDistribution_esp01 )
    ulMessClass = RSQL_KERN_DATA_REQUEST_EO003;
  else
    ulMessClass = RSQL_USER_DATA_REQUEST_EO003;

  pCommPacket->RTEHeader.ActSendLen      = (INT4)ulCmdDataLen +
                                           RTE_HEADER_SIZE;
  pCommPacket->RTEHeader.ProtocolID      = RSQL_RTE_PROT_LOCAL_EO003;
  pCommPacket->RTEHeader.MessClass       = (INT1)ulMessClass;
  pCommPacket->RTEHeader.RTEFlags        = RTEFlags;
  pCommPacket->RTEHeader.ResidualPackets = 0;
  pCommPacket->RTEHeader.SenderRef       = pShmConnInfo->ulClientRef;
  pCommPacket->RTEHeader.ReceiverRef     = pShmConnInfo->ulServerRef;
  pCommPacket->RTEHeader.RTEReturnCode   = commErrOk_esp01;
  pCommPacket->RTEHeader.NewSwapType     = eo420NewSwapType();
  pCommPacket->RTEHeader.Filler1         = 0;
  pCommPacket->RTEHeader.MaxSendLen      = (INT4)ulCmdDataLen +
                                           RTE_HEADER_SIZE;

  //
  // - set some communication segment values and post the
  //   communication flag to signal that data has been send to server
  //
  pCSHeader->ulClientCommState   = commErrOk_esp01;
  pCSHeader->ulCurrentPacket     = pShmConnInfo->ulCurrentPacket;

  if ( pShmConnInfo->pulCommFlag != NULL )
  {
    pCSHeader->ulClientFlag      = 1;
    RTESys_WriteMemoryBarrier();
    *pShmConnInfo->pulCommFlag   = 1;
  }
  else
  {
    RTESys_WriteMemoryBarrier();
    pCSHeader->ulClientFlag      = 1;
  }


  // ---  Wake up the UKT?
  RTESys_ReadMemoryBarrier();  // to get the correct semaphore
  ulSemaNo = pCSHeader->ulSemaNo; 

  /* Kernel may have corrupted the Communication Segment Header. Therefore we do a range check here. */
  if (ulSemaNo >= pShmConnInfo->ulNumOfServerSems)
  {
      MSGD ((ERR_COMMSEG_HEADER_CORRUPT))
      eo46BuildErrorStringRC(pErrText, ERRMSG_COM_COMMSEG_HEADER_CORRUPT, 0);
      return (commErrNotOk_esp01);
  }

  if ( pShmConnInfo->pulSemaFlag[ulSemaNo] == POST_SEM )
    {
    ulCommState = sql33c_post_server_sem (pShmConnInfo->phServerSems[ulSemaNo],
                                          pErrText);

    if ( ulCommState != commErrOk_esp01 )
      {
      DBGOUT;
      return ( ulCommState );
      }
    }

  DBG3 (( MF__, "ulServerCommState: %d", pCSHeader->ulServerCommState  ));
  DBG3 (( MF__, "ulClientCommState: %d", pCSHeader->ulClientCommState  ));
  DBG3 (( MF__, "ulServerFlag     : %d", pCSHeader->ulServerFlag ));
  DBG3 (( MF__, "ulClientFlag     : %d", pCSHeader->ulClientFlag ));

  DBGOUT;
  return ( commErrOk_esp01 );
  }


/*------------------------------*/

#if defined(KERNEL)

 ULONG  sql33c_reply ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                       PTASK_CTRL_REC          pTaskCtrl,
                       tsp00_ErrTextc               pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_reply"
   PCOMM_SEG_HEADER_REC           pCSHeader;

   pCSHeader               = pShmConnInfo->pCSHeader;
   pTaskCtrl->fTimeOut     = FALSE;
   pTaskCtrl->ulCOMTimeOut = MIN_COMMAND_TIMEOUT + kgs.ulCurrTime;

   do
     {
     // --- insert task into COM queue, where it waits for a reply
     sql73k_COM_enqu (pTaskCtrl->pUKT, pTaskCtrl);

     GOTO_DISP( &pTaskCtrl->pUKT );

     if ( ! kgs.fNetRestarted )
       {
       MSGD (( IERR_NETW_NOT_RESTARTED, "sql33c_reply" ))
       eo46BuildErrorStringRC ( pErrText, ERRMSG_NETW_NOT_RESTARTED, 0);
       DBGOUT;
       return ( commErrSendLineDown_esp01 );
       }

     if ( pTaskCtrl->fTimeOut == TRUE )
       {
       pTaskCtrl->fTimeOut = FALSE;
       MSGD (( ERR_REPLY_TIMEOUT ))
       eo46BuildErrorStringRC ( pErrText, ERRMSG_REPLY_TIMEOUT, 0);
       DBGOUT;
       return ( commErrTimeout_esp01 );
       }
     }
   while ( pCSHeader->ulServerFlag != 1 );

   RTESys_ReadMemoryBarrier();

   // --- signal that we received the data
   pCSHeader->ulClientFlag = 0;
   pCSHeader->ulServerFlag = 2;

   if ( pCSHeader->ulServerCommState != commErrOk_esp01 )
     {
     eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, 0 );
     DBGOUT;
     return ( pCSHeader->ulServerCommState );
     }

   DBGOUT;
   return ( commErrOk_esp01 );
   }

 /*------------------------------*/

 ULONG sql33c_crash_cleanup ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                              tsp00_ErrTextc               pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_crash_cleanup"
   ULONG                           rc          = NO_ERROR;
   PCOMM_SEG_HEADER_REC            pCSHeader;
   ULONG                           ulCommState;

   DBGIN;

   pCSHeader = pShmConnInfo->pCSHeader;

   if ( pCSHeader != NULL )
      {
      //
      // - set some communication segment values and post the
      //   communication flag to signal a event
      //
      pCSHeader->pidClientOrPeerPID = (PID)UNDEF;
      pCSHeader->ulClientCommState  = commErrSendLineDown_esp01;

      if ( pShmConnInfo->pulCommFlag != NULL )
      {
        pCSHeader->ulClientFlag    = 1;
        RTESys_WriteMemoryBarrier();
        *pShmConnInfo->pulCommFlag = 1;
      }
      else
      {
        RTESys_WriteMemoryBarrier();
        pCSHeader->ulClientFlag    = 1;
      }

      if ( pShmConnInfo->phServerSems[pCSHeader->ulSemaNo] != INVALID_HANDLE_VALUE )
        {
        ulCommState =  sql33c_post_server_sem( pShmConnInfo->phServerSems[pCSHeader->ulSemaNo],
                                               pErrText );
        }
      }

   DBGOUT;
   return ( commErrSendLineDown_esp01 );
   }

#else

 /*------------------------------*/

 ULONG  sql33c_replyavailable( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                               tsp00_ErrTextc               pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_replyavailable"
   PCOMM_SEG_HEADER_REC            pCSHeader;

   DBGIN;

   pCSHeader = pShmConnInfo->pCSHeader;


   if ( pCSHeader->ulClientFlag == 0 )
     {
     MSGD (( ERR_UNBAL_REQUEST_REPLY ));
     eo46BuildErrorStringRC(pErrText, ERRMSG_COM_UNBALANCED_REQUEST_REPLY, 0);

     return ( commErrNotOk_esp01 );
     }

   DBG3 (( MF__, "ulServerCommState: %d", pCSHeader->ulServerCommState  ));
   DBG3 (( MF__, "ulServerFlag     : %d", pCSHeader->ulServerFlag ));
   DBG3 (( MF__, "ulClientFlag     : %d", pCSHeader->ulClientFlag ));

   if ( pCSHeader->ulServerFlag == 1 )
     {
     DBGOUT;
     return ( commErrOk_esp01 );
     }

   /* check if Kernel is still alive */
  if( WaitForSingleObject(pShmConnInfo->hServerProcess, 0) != WAIT_TIMEOUT )
  {
      DBGOUT;
      //kernel has terminated without releasing the connection correctly
      return commErrCrash_esp01;
  }

   return ( commErrWouldBlock_esp01 );
   }

 /*------------------------------*/

 ULONG  sql33c_receive ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                         PCOMM_PACKET_REC        *ppResPacket,
                         PULONG                  pulResDataLen,
                         tsp00_ErrTextc               pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_receive"
   ULONG                           rc                = NO_ERROR;
   ULONG                           ulCommState;
   PCOMM_SEG_HEADER_REC            pCSHeader;
   PCOMM_PACKET_REC                pCmdCommPacket;
   PRTE_HEADER_REC                 pRTEHeader;
   ULONG                           ulAliveLookupTime;
   ULONG						   ulSemaNo;

   DBGIN;

   pCSHeader = pShmConnInfo->pCSHeader;

   DBG3 (( MF__, "ulServerCommState: %d", pCSHeader->ulServerCommState  ));
   DBG3 (( MF__, "ulClientCommState: %d", pCSHeader->ulClientCommState  ));
   DBG3 (( MF__, "ulServerFlag     : %d", pCSHeader->ulServerFlag ));
   DBG3 (( MF__, "ulClientFlag     : %d", pCSHeader->ulClientFlag ));

   if ( pCSHeader->ulClientFlag == 0 )
     {
     MSGD (( ERR_UNBAL_REQUEST_REPLY ));
     eo46BuildErrorStringRC(pErrText, ERRMSG_COM_UNBALANCED_REQUEST_REPLY, 0);

     return ( commErrNotOk_esp01 );
     }

   while ( pCSHeader->ulServerFlag != 1 )
     {
     ulAliveLookupTime = (pShmConnInfo->hServerProcess == INVALID_HANDLE_VALUE) ?  -1 : (ALIVE_LOOKUP_TIME * 1000);

     do 
       {
       ulCommState = sql33c_wait_client_sem ( pShmConnInfo->hClientSem,
                                              ulAliveLookupTime, pErrText );

       if ( ulCommState == commErrTimeout_esp01 )
         {
         if( WaitForSingleObject( pShmConnInfo->hServerProcess, 0) != WAIT_TIMEOUT )
           {
           MSGD (( ERR_CONN_BROKEN ));
           eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, GetLastError() );
           ulCommState = commErrNotOk_esp01;
           }
         }
       }
     while ( ulCommState == commErrTimeout_esp01 );

     if ( ulCommState != commErrOk_esp01 )
       {
       DBGOUT;
       return ( ulCommState );
       }
     }

   RTESys_ReadMemoryBarrier();

   // --- signal that we have received
   pCSHeader->ulServerFlag = 2;

   if ( pCSHeader->ulServerCommState != commErrOk_esp01 )
     {
     if ( pCSHeader->ulServerCommState == commErrTimeout_esp01 )
       {
       MSGD (( INFO_COM_TIMEOUT ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_TIMEOUT, 0 );
       DBGOUT;
       return ( commErrTimeout_esp01 );
       }
     else
       {
       eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, 0 );
       DBGOUT;
       return ( pCSHeader->ulServerCommState );
       }
     }

   // --- get the result packet position.
   //
   pCmdCommPacket = pShmConnInfo->pCommPacketList[pShmConnInfo->ulCurrentPacket];

   if ( pShmConnInfo->ulMinReplySize != 0 )
   {
     *ppResPacket   = (PCOMM_PACKET_REC) ((PCHAR)pCmdCommPacket->pDataPart +
                      ALIGN ( pShmConnInfo->ulCmdDataLen, SQL_PACKET_ALIGNMENT));
   }
   else
     *ppResPacket   = (PCOMM_PACKET_REC) pCmdCommPacket;

   pRTEHeader     = &(*ppResPacket)->RTEHeader;
   *pulResDataLen = pRTEHeader->ActSendLen - RTE_HEADER_SIZE;
   ulCommState    = (ULONG)pRTEHeader->RTEReturnCode;

   if (( pCSHeader->ulClientFlag != 2 )              ||
       ( pRTEHeader->MaxSendLen  < RTE_HEADER_SIZE ) ||
       ( pRTEHeader->SenderRef   == UNDEF ))
     {
     // --- the kernel has released the connection without a reply.
     eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, 0 );
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }

   if ( pShmConnInfo->ulServiceType == srvDistribution_esp01 )
     {
     // --- RTE-Header check
     if (( pRTEHeader->ResidualPackets    != 0 )                         ||
         ( (ULONG)pRTEHeader->SenderRef   != pShmConnInfo->ulServerRef ) ||
         ( (ULONG)pRTEHeader->ReceiverRef != pShmConnInfo->ulClientRef ) ||
         ( pRTEHeader->MessClass          != RSQL_KERN_DATA_REPLY_EO003 )      ||
         ( pRTEHeader->MaxSendLen         != pRTEHeader->ActSendLen ))
       {
       MSGD (( ERR_PROTOCOL_ERROR, "sql33c_receive" ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

       DBGOUT;
       return ( commErrNotOk_esp01 );
       }
     }
   else
     {
     // --- RTE-Header check
     if (( pRTEHeader->ResidualPackets    != 0 )                         ||
         ( (ULONG)pRTEHeader->SenderRef   != pShmConnInfo->ulServerRef ) ||
         ( (ULONG)pRTEHeader->ReceiverRef != pShmConnInfo->ulClientRef ) ||
         ( pRTEHeader->MessClass          != RSQL_USER_DATA_REPLY_EO003 )      ||
         ( pRTEHeader->ProtocolID         != RSQL_RTE_PROT_LOCAL_EO003 )       ||
         ( pRTEHeader->MaxSendLen         != pRTEHeader->ActSendLen ))
       {
       MSGD (( ERR_PROTOCOL_ERROR, "sql33c_receive" ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );

       DBGOUT;
       return ( commErrNotOk_esp01 );
       }
     }

   /* Kernel may have corrupted the Communication Segment Header. Therefore we do a range check here. */
   ulSemaNo = pCSHeader->ulSemaNo;
   if (ulSemaNo >= pShmConnInfo->ulNumOfServerSems)
   {
       MSGD ((ERR_COMMSEG_HEADER_CORRUPT))
	   eo46BuildErrorStringRC(pErrText, ERRMSG_COM_COMMSEG_HEADER_CORRUPT, 0);
       return (commErrNotOk_esp01);
   }

   if (( pShmConnInfo->phServerSems[ulSemaNo] == INVALID_HANDLE_VALUE ) &&
       ( pShmConnInfo->pSemaNames             != NULL ))
     {
     ulCommState = sql33c_open_server_sem ( pShmConnInfo->pszSemaPath,
                                            pShmConnInfo->pSemaNames[ulSemaNo],
                                            &pShmConnInfo->phServerSems[ulSemaNo],
                                            pErrText );
     if ( ulCommState != commErrOk_esp01 )
       {
       DBGOUT;
       return ( ulCommState );
       }
     }

   // --- signal that we are ready for the next request
   pCSHeader->ulClientFlag = 0;

   DBG3 (( MF__, "ulServerCommState: %d", pCSHeader->ulServerCommState  ));
   DBG3 (( MF__, "ulClientCommState: %d", pCSHeader->ulClientCommState  ));
   DBG3 (( MF__, "ulServerFlag     : %d", pCSHeader->ulServerFlag ));
   DBG3 (( MF__, "ulClientFlag     : %d", pCSHeader->ulClientFlag ));

   DBGOUT;
   return ( commErrOk_esp01 );
   }

#endif

/*------------------------------*/

ULONG sql33c_cancel_dump ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                           ULONG                   ulReqType,
                           tsp00_ErrTextc               pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_cancel_dump"
  ULONG                           ulCommState;

  DBGIN;

  //
  // --- send a cancel/dump request
  //
  ulCommState = sql33c_write_queue_mailslot( pShmConnInfo->hQueMailSlot,
                                            NULL,
                                            ulReqType,
                                            pShmConnInfo->ulServerRef,
                                            0,
                                            pErrText );

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

ULONG sql33c_release ( PSHM_CONNECT_INFO_REC   pShmConnInfo,
                       tsp00_ErrTextc               pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_release"
  ULONG                           ulCommState = commErrOk_esp01;
  ULONG                           rc          = NO_ERROR;
  PCOMM_SEG_HEADER_REC            pCSHeader;
  ULONG							  ulSemaNo;

  DBGIN;

  pCSHeader = pShmConnInfo->pCSHeader;

  if ( pCSHeader != NULL )
     {
     //
     // - set some communication segment values and post the
     //   communication flag to signal a event
     //
     pCSHeader->pidClientOrPeerPID = (PID)UNDEF;

     if ( pShmConnInfo->pulCommFlag != NULL )
     {
       RTESys_WriteMemoryBarrier();
       *pShmConnInfo->pulCommFlag  = 1;
     }

	 /* Kernel may have corrupted the Communication Segment Header. Therefore we do a range check here. */
	 ulSemaNo = pCSHeader->ulSemaNo;
     if (ulSemaNo >= pShmConnInfo->ulNumOfServerSems)
	 {
        MSGD ((ERR_COMMSEG_HEADER_CORRUPT))
  	    eo46BuildErrorStringRC(pErrText, ERRMSG_COM_COMMSEG_HEADER_CORRUPT, 0);
        return (commErrNotOk_esp01);
	 }

     if ( pShmConnInfo->phServerSems[ulSemaNo] != INVALID_HANDLE_VALUE )
       {
       ulCommState =  sql33c_post_server_sem( pShmConnInfo->phServerSems[ulSemaNo],
                                              pErrText );
       }

     if ( ulCommState == commErrOk_esp01 )
       ulCommState = sql33c_free_comm_seg( pShmConnInfo->hCS,
                                           pShmConnInfo->pCSHeader,
                                           pErrText );
     }

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_close_all_server_sems  ( pShmConnInfo, pErrText );

  if ( ulCommState == commErrOk_esp01 )
    ulCommState = sql33c_close_queue_mailslot ( pShmConnInfo->hQueMailSlot,
                                                pErrText );
  #if !defined (KERNEL)
   if ( ulCommState == commErrOk_esp01 )
     ulCommState = sql33c_close_server_process ( pShmConnInfo->hServerProcess, pErrText );

   if ( ulCommState == commErrOk_esp01 )
     ulCommState = sql33c_close_client_sem ( pShmConnInfo->hClientSem, pErrText );
  #endif

  if (( ulCommState == commErrOk_esp01 ) && ( pShmConnInfo->pFCS != NULL ))
    ulCommState = sql33c_free_flag_comm_seg ( pShmConnInfo->hFCS,
                                              pShmConnInfo->pFCS,
                                              pErrText );
  if ( ulCommState != commErrOk_esp01 )
    {
    // --- close without error handling !
    sql33c_close_queue_mailslot  ( pShmConnInfo->hQueMailSlot, NULL );
    sql33c_close_all_server_sems ( pShmConnInfo, NULL );
    sql33c_close_server_process ( pShmConnInfo->hServerProcess, NULL );

    #if !defined (KERNEL)
     sql33c_close_client_sem     ( pShmConnInfo->hClientSem, NULL );
    #endif
    }

  // --- initialize the handles and pointers of connect information record
  ulCommState = sql33c_init_cleanup_conn_info ( pShmConnInfo, NULL, 0, INVALID_HANDLE_VALUE, NULL, NULL );

  DBGOUT;
  return ( ulCommState );
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#if defined (OS2) && defined (KERNEL)   // - ignore unused parameter
# pragma info (nopar)
#endif

static ULONG sql33c_connect_wait ( HANDLE                hClientSem,
                                   ULONG                 ulServiceType,
                                   ULONG                 ulTimeOut,
                                   HANDLE                hServerProcess,
                                   PCOMM_SEG_HEADER_REC  pCSHeader,
                                   tsp00_ErrTextc        pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_connect_wait"
  ULONG                           ulCommState = commErrOk_esp01;
  ULONG                           ulAliveLookupTime;
  #if defined (KERNEL)
   PUKT_CTRL_REC                  pUKT        = THIS_UKT_CTRL;
   PTASK_CTRL_REC                 pTaskCtrl   = pUKT->pCTask;
  #endif

  DBGIN;


  #if defined (KERNEL)
    /*
     *  insert task into COM queue, where it waits for the connect reply.
     */
    pTaskCtrl->ulCOMTimeOut = ulTimeOut + kgs.ulCurrTime;
    pTaskCtrl->fTimeOut     = FALSE;

    {
        int gotCommuniction = 0;
    do
      {
      sql73k_COM_enqu ( pUKT, pTaskCtrl );

      GOTO_DISP ( &pUKT );

      if ( ! kgs.fNetRestarted )
        {
        MSGD (( IERR_NETW_NOT_RESTARTED, "connect wait" ))
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NETW_NOT_RESTARTED, 0);
        DBGOUT;
        return ( commErrSendLineDown_esp01 );
        }

      if ( pTaskCtrl->fTimeOut == TRUE )
        {
        pTaskCtrl->fTimeOut = FALSE;
        MSGD (( ERR_CONN_TIMEOUT ))
        eo46BuildErrorStringRC ( pErrText, ERRMSG_CONNECT_TIMED_OUT, 0 );
        DBGOUT;
        return ( commErrTimeout_esp01 );
        }
        if ( pCSHeader->ulServerFlag == 1 )
        {
            gotCommuniction = 1;
            RTESys_ReadMemoryBarrier();
        }
      }
    while (( !gotConnection ) &&
           ( pCSHeader->pidClientOrPeerPID != (PID)UNDEF ) &&
           ( pCSHeader->ulServerCommState  == commErrOk_esp01 ));
    }
  #else
   if ( ulTimeOut != (ULONG)UNDEF )
     ulTimeOut *= 1000;

   ulAliveLookupTime = (hServerProcess == INVALID_HANDLE_VALUE) ?  ulTimeOut : (10 * 1000);

   do 
   {
     ulCommState = sql33c_wait_client_sem ( hClientSem, ulAliveLookupTime, pErrText );
 
     if ( ulCommState == commErrTimeout_esp01 )
       {
       if( WaitForSingleObject( hServerProcess, 0) != WAIT_TIMEOUT )
         {
         MSGD (( ERR_CONN_BROKEN ));
         eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, GetLastError() );
         ulCommState = commErrNotOk_esp01;
         }
       }
       if ( ulTimeOut != (ULONG)UNDEF )
           ulTimeOut = ulTimeOut > ulAliveLookupTime ? ulTimeOut - ulAliveLookupTime : 0;
     }
   while (( ulCommState == commErrTimeout_esp01 ) && ulTimeOut );

   if ( ulCommState == commErrTimeout_esp01 )
   {
     MSGD (( ERR_CONN_TIMEOUT ));
     eo46BuildErrorStringRC ( pErrText, ERRMSG_CONN_TIMEOUT, 0 );
   }
  #endif

  DBGOUT;
  return ( ulCommState );
  }

#if defined (OS2) && defined (KERNEL)   // - ignore unused parameter
# pragma info (restore)
#endif

/*------------------------------*/

static ULONG  sql33c_open_queue_mailslot ( PSZ                    pszQueName,
                                           BOOL                   fConnectToXServer,
                                           PHANDLE                phQueMailSlot,
                                           tsp00_ErrTextc         pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_open_queue_mailslot"
  PSZ        pszPath;
  ULONG      rc = NO_ERROR;
  PID        pidServerDummy;

  DBGIN;

  //
  // --- open mailslot / fifo queue
  //
  pszPath = MAIL_REQ;

  rc = sql41c_open_queue_mailslot ( &pidServerDummy,   // - OS/2
                                    phQueMailSlot,
                                    pszPath,
                                    pszQueName );

  if ( rc == ERROR_FILE_NOT_FOUND )
    {
    *phQueMailSlot = INVALID_HANDLE_VALUE;

    if ( fConnectToXServer )
      {
      MSGD (( ERR_XSERVER_NOT_ACTIVE ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_XSERVER_NOT_ACTIVE, 0 );
      }
    else
      {
      MSGD (( ERR_DATABASE_NOT_STARTED, pszQueName ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_DATABASE_NOT_STARTED, 0 );
      }

    DBGOUT;
    return ( commErrStartRequired_esp01 );
    }

  if ( rc != NO_ERROR )
    {
    *phQueMailSlot = INVALID_HANDLE_VALUE;
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG  sql33c_write_queue_mailslot ( HANDLE             hQueMailSlot,
                                            PSZ                pszClientObjName,
                                            ULONG              ulReqType,
                                            ULONG              ulServerRef,
                                            ULONG              ulCSSize,
                                            tsp00_ErrTextc          pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_write_queue_mailslot"
  ULONG               rc = NO_ERROR;
  COMM_REQUEST_REC    Request;

  DBGIN;

  //
  //
  // --- send a connect request
  //
  //
  
  memset(&Request, 0, sizeof(Request));

  Request.ulReqType               = ulReqType;
  Request.ulServerRef             = ulServerRef;
  Request.ulCSSize                = ulCSSize;

  if ( pszClientObjName != NULL )
    strcpy ( Request.szClientObjName, pszClientObjName );
  else
    Request.szClientObjName[0] = '\0';

  rc = sql41c_write_queue_mailslot ( hQueMailSlot,
                                     &Request,
                                     sizeof (COMM_REQUEST_REC),
                                     ulCSSize,      // - OS/2 only
                                     pErrText );

  if ( rc != NO_ERROR )
    {
    DBG1 (( MF__, "ERROR: 'sql41c_write_queue_mailslot', rc =  %d", rc ));

    if (( rc == ERROR_HANDLE_EOF ) || ( rc == ERROR_FILE_NOT_FOUND ))
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, 0 );
    else
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_WRITE_COM_QUE, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }


/*------------------------------*/

static ULONG sql33c_close_queue_mailslot ( HANDLE      hQueMailSlot,
                                           tsp00_ErrTextc   pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_close_queue_mailslot"
  ULONG    rc          = NO_ERROR;
  ULONG    ulCommState = commErrOk_esp01;

  DBGIN;

  if ( hQueMailSlot != INVALID_HANDLE_VALUE )
    {
    rc = sql41c_close_queue_mailslot ( hQueMailSlot );

    if ( rc != NO_ERROR )
      {
      ulCommState = commErrNotOk_esp01;

      if ( pErrText != NULL )
        eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CLOSE_QUE_MAIL, rc );
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

#if !defined (KERNEL)

 static ULONG  sql33c_create_client_sem( PSZ                   pszSemPath,
                                         PSZ                   pszClientObjName,
                                         PHANDLE               phClientSem,
                                         PSECURITY_ATTRIBUTES  pWorldSA,
                                         tsp00_ErrTextc             pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_create_client_sem"
   ULONG      rc      = NO_ERROR;
   ULONG      ulRetry = 10;

   DBGIN;

   //
   // --- create event semaphore
   //
   do
     {
     rc = sql41c_create_event_sem ( phClientSem,
                                    pszSemPath, pszClientObjName,
                                    CREATE_EVENT_SEM_SHARED, 0,
                                    pWorldSA );

     // --- unique name already in use?
     if ( rc == ERROR_DUPLICATE_NAME )
       {
       SLEEP ( 1 );

       // -- build a new unique name
       sql41c_build_unique_obj_name ( pszClientObjName );
       }
     }
   while (( rc == ERROR_DUPLICATE_NAME ) && (--ulRetry) );

   if ( rc != NO_ERROR )
     {
     *phClientSem = INVALID_HANDLE_VALUE;
     DBG1 (( MF__, "ERROR: create event sem, rc =  %d", rc ));
     MSGD (( ERR_CANT_CREATE_COM_SEM, rc ));
     eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_CREATE_COM_SEM, rc );

     DBGOUT;
     return ( commErrNotOk_esp01 );
     }


   DBGOUT;
   return ( commErrOk_esp01 );
   }

 /*------------------------------*/

 static ULONG  sql33c_wait_client_sem ( HANDLE     hClientSem,
                                        ULONG      ulTimeOut,
                                        tsp00_ErrTextc  pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_wait_client_sem"
   ULONG   rc = NO_ERROR;

   DBGIN;

   rc =  sql41c_wait_event_sem ( hClientSem, ulTimeOut, "Client" );

   if ( rc != NO_ERROR )
     {
     if ( rc == ERROR_TIMEOUT )
       {
       DBGOUT;
       return ( commErrTimeout_esp01 );
       }
     else
       {
       MSGD (( ERR_WAIT_COM_SEM, rc ));
       eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_WAIT_COM_SEM, rc );

       DBGOUT;
       return ( commErrNotOk_esp01 );
       }
     }

   DBGOUT;
   return ( commErrOk_esp01 );
   }

 /*------------------------------*/

 static ULONG sql33c_close_client_sem ( HANDLE     hClientSem,
                                        tsp00_ErrTextc  pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_close_client_sem"
   ULONG    rc          = NO_ERROR;
   ULONG    ulCommState = commErrOk_esp01;

   DBGIN;

   if ( hClientSem != INVALID_HANDLE_VALUE )
     {
     rc = sql41c_close_event_sem ( hClientSem, "Client" );

     if ( rc != NO_ERROR )
       {
       ulCommState = commErrNotOk_esp01;

       if ( pErrText != NULL )
         eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CLOSE_CLIENT_SEM, rc );
       }
     }

   DBGOUT;
   return ( ulCommState );
   }
#endif

/*------------------------------*/

static ULONG  sql33c_open_server_sems ( PSHM_CONNECT_INFO_REC  pShmConnInfo,
                                        tsp00_ErrTextc              pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_open_server_sem"

  ULONG  ulCommState;
  ULONG  ulSemaNo;


  if ( pShmConnInfo->pSemaNames == NULL )
    { // --- Old kernel (less than  7.4.3)
      //     Kernel replies the UKT semaphore name via communication segment
     /* Kernel may have corrupted the Communication Segment Header. Therefore we do a range check here. */
	 ulSemaNo = pShmConnInfo->pCSHeader->ulSemaNo;
     if (ulSemaNo >= pShmConnInfo->ulNumOfServerSems)
     {
         MSGD ((ERR_COMMSEG_HEADER_CORRUPT))
         eo46BuildErrorStringRC(pErrText, ERRMSG_COM_COMMSEG_HEADER_CORRUPT, 0);
         return (commErrNotOk_esp01);
     }

     ulCommState = sql33c_open_server_sem ( pShmConnInfo->pCSHeader->szSemPath,
                                           pShmConnInfo->pCSHeader->szSemName,
                                           &pShmConnInfo->phServerSems[ulSemaNo],
                                           pErrText );
    }
    else if ( pShmConnInfo->pFCS->fSemDemandOpen )
    {
     /* Kernel may have corrupted the Communication Segment Header. Therefore we do a range check here. */
	 ulSemaNo = pShmConnInfo->pCSHeader->ulSemaNo;
     if (ulSemaNo >= pShmConnInfo->ulNumOfServerSems)
     {
         MSGD ((ERR_COMMSEG_HEADER_CORRUPT))
         eo46BuildErrorStringRC(pErrText, ERRMSG_COM_COMMSEG_HEADER_CORRUPT, 0);
         return (commErrNotOk_esp01);
     }

     ulCommState = sql33c_open_server_sem ( pShmConnInfo->pszSemaPath,
                                           pShmConnInfo->pSemaNames[ulSemaNo],
                                           &pShmConnInfo->phServerSems[ulSemaNo],
                                           pErrText );
    }
  else
    { 
    for ( ulSemaNo = 0, ulCommState = commErrOk_esp01; 
          ulSemaNo < pShmConnInfo->ulNumOfServerSems && ulCommState == commErrOk_esp01;
          ulSemaNo++ )
      {
      ulCommState = sql33c_open_server_sem ( pShmConnInfo->pszSemaPath,
                                             pShmConnInfo->pSemaNames[ulSemaNo],
                                             &pShmConnInfo->phServerSems[ulSemaNo],
                                             pErrText );
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG  sql33c_open_server_sem ( PSZ            pszSemPath,
                                       PSZ            pszSemName,
                                       PHANDLE        phServerSem,
                                       tsp00_ErrTextc      pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_open_server_sem"
  PSZ         pszPath;
  PSZ         pszName;
  ULONG       rc      = NO_ERROR;

  DBGIN;

  //
  // --- create event semaphore
  //
  pszPath = pszSemPath;
  pszName = pszSemName;

  rc = sql41c_open_event_sem ( phServerSem, pszPath, pszName, NO_ERROR );


  if ( rc != NO_ERROR )
    {
    *phServerSem = INVALID_HANDLE_VALUE;
    DBG1 (( MF__, "ERROR: open event sem, rc =  %d", rc ));
    MSGD (( ERR_CANT_OPEN_UKT_SEM, rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_OPEN_UKT_SEM, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG  sql33c_post_server_sem ( HANDLE         hServerSem,
                                       tsp00_ErrTextc      pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_post_server_sem"
  ULONG      rc = NO_ERROR;

  DBGIN;

  // ---  Wake up the UKT!
  rc = sql41c_post_event_sem ( hServerSem, "Server" );

  if (( rc != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED ))
    {
    DBG1 (( MF__, "ERROR: Posting event sem, rc =  %d", rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_POST_UKT_SEM, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql33c_close_all_server_sems ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                            tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_close_all_server_sems"
  ULONG     ulCommState       = commErrOk_esp01;
  ULONG     ulNumOfSemaphores = pShmConnInfo->ulNumOfServerSems;
  HANDLE    *phServerSems     = pShmConnInfo->phServerSems;

  ULONG     ulSemaNo;

  DBGIN;

  for ( ulSemaNo = 0; 
        ulSemaNo < ulNumOfSemaphores && ulCommState == commErrOk_esp01;
        ulSemaNo++ )
  {
    if ( phServerSems[ulSemaNo] != INVALID_HANDLE_VALUE )
      ulCommState = sql33c_close_server_sem ( phServerSems[ulSemaNo], pErrText );

    phServerSems[ulSemaNo] = INVALID_HANDLE_VALUE;
  }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG sql33c_close_server_sem ( HANDLE     hServerSem,
                                       tsp00_ErrTextc  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_close_server_sem"
  ULONG    rc;
  ULONG     ulCommState = commErrOk_esp01;

  DBGIN;

  if ( hServerSem != INVALID_HANDLE_VALUE )
    {
    rc = sql41c_close_event_sem ( hServerSem, "Server" );

    if ( rc != NO_ERROR )
      {
      ulCommState = commErrNotOk_esp01;

      if ( pErrText != NULL )
        eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CLOSE_SERV_SEM, rc );
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG  sql33c_try_open_server_process ( PID                 pidServerPID,
                                               PHANDLE             phServerProcess,
                                               tsp00_ErrTextc      pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_try_open_server_process"
  ULONG      rc      = NO_ERROR;

  DBGIN;

  *phServerProcess = OpenProcess ( SYNCHRONIZE, FALSE,
                                   pidServerPID );
  if ( *phServerProcess == NULL )
  {
    rc = GetLastError();

    *phServerProcess = INVALID_HANDLE_VALUE;

    DBG1((MF__, "Unable to open proccess, rc=%d", rc));

    // --- Probably, the user died in between because of too slow response
    if ( rc == ERROR_INVALID_PARAMETER )
    {
      MSGD (( ERR_CONN_BROKEN ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_BROKEN, NO_ERROR );

      DBGOUT;
      return ( commErrNotOk_esp01 );
    }
  }

  DBGOUT;
  return ( commErrOk_esp01 );
  }


/*------------------------------*/

static ULONG sql33c_close_server_process ( HANDLE     hServerProcess,
                                           tsp00_ErrTextc  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_close_server_process"
  ULONG    rc          = NO_ERROR;
  ULONG     ulCommState = commErrOk_esp01;

  DBGIN;

  if ( hServerProcess != INVALID_HANDLE_VALUE )
    {
      if ( !CloseHandle ( hServerProcess ) )
        {
        rc = GetLastError();
        
        if ( rc != NO_ERROR )
          {
          ulCommState = commErrNotOk_esp01;

          if ( pErrText != NULL )
            eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CLOSE_SERV_PROCESS, rc );

          MSGD (( ERR_CLOSING_PROCESS, rc));
          }
        }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG  sql33c_create_comm_seg( ULONG                 ulCSSize,
                                      PSZ                   pszClientObjName,
                                      PSECURITY_ATTRIBUTES  pWorldSA,
                                      PHANDLE               phCS,
                                      PCOMM_SEG_HEADER_REC  *ppCSHeader,
                                      tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_create_comm_seg"
  PSZ                       pszPath;
  PSZ                       pszName;
  ULONG                    rc      = NO_ERROR;

  DBGIN;

  //
  //
  // --- alloc shared communication memory segment with "world access"
  //
  //
  pszPath = SHM_COMMSEG;
  pszName = pszClientObjName;

  rc = sql41c_create_shrd_mem ( (PVOID*) ppCSHeader, ulCSSize,
                                pszPath, pszName,
                                pWorldSA,
                                phCS );

  if ( rc != NO_ERROR )
    {
    DBG1 (( MF__, "ERROR: 'CreateSharedMem', rc =  %d", rc ));
    *ppCSHeader = NULL;
    *phCS       = INVALID_HANDLE_VALUE;

    MSGD (( ERR_CANT_ALLOC_COM_SEG, rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_ALLOC_COM_SEG, rc );

    DBGOUT;
    return ( commErrPacketLimit_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG  sql33c_lock_comm_seg( HANDLE                   hCS,
                                    PSECURITY_ATTRIBUTES     pLockSA,
                                    tsp00_ErrTextc                pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_lock_comm_seg"
  ULONG    rc      = NO_ERROR;

  DBGIN;

  // --- set security descriptor with "no access"
  rc = sql49c_set_kernel_obj_sec ( pLockSA->lpSecurityDescriptor, hCS );

  if ( rc != NO_ERROR )
    {
    DBG1 (( MF__, "ERROR: 'sql49c_set_kernel_obj_sec', rc =  %d", rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_SET_OBJ_SEC, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG  sql33c_free_comm_seg( HANDLE                hCS,
                                    PCOMM_SEG_HEADER_REC  pCSHeader,
                                    tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_free_comm_seg"
  ULONG    rc          = NO_ERROR;
  ULONG     ulCommState = commErrOk_esp01;


  DBGIN;


  if ( pCSHeader != NULL )
    {
    rc = sql41c_free_shrd_mem ( pCSHeader, hCS );

    if ( rc != NO_ERROR )
      {
      ulCommState = commErrNotOk_esp01;

      if ( pErrText != NULL )
        eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_FREE_MEM, rc );
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static VOID sql33c_init_comm_seg ( PSHM_CONNECT_INFO_REC    pShmConnInfo,
                                   tsp100_VersionID2*       pKernelVersion,
                                   PSZ                      pszClientObjName,
                                   PSZ                      pszClientSemPath,
                                   PSZ                      pszClientSemName,
                                   tsp00_ErrTextc                pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_init_comm_seg"
  ULONG                 ulCnt;
  PCOMM_SEG_HEADER_REC  pCSHeader = pShmConnInfo->pCSHeader;
  PCOMM_SEG_OFFSET_REC  pCSOffsetList;

  DBGIN;

  //
  // - Structure of the communication segment
  //
  //      +------------------------------------+
  //   +--|     comm_seg_header_record         |
  //   |  +------------------------------------+
  //   +->|     comm_seg_offset_record         |--+
  //      +------------------------------------+  |
  //      | 1.  comm_packet_record             |<-+
  //      |     (request)                      |  |
  //      |  --------------------------------  |  |
  //      |     comm_packet_record             |  |
  //      |     (receive)                      |  |
  //      +------------------------------------+  |
  //      .                                    .  .
  //      .                                    .  .
  //      +------------------------------------+  |
  //      | n.  comm_packet_record             |<-+
  //      |     (request)                      |
  //      |  --------------------------------  |
  //      |     comm_packet_record             |
  //      |    /(receive)                      |
  //      +---/--------------------------------+
  //         /
  //        /
  // +-----+------------------------------+
  // |     rte_header_record +            |
  // |     sql_packet (data)              |
  // +------------------------------------+
  //
  //  n = MAX_SQL_PACKETS
  //

  //
  // --- init the communication segment header
  //

  // --- Does the Kernel reply a single UKT semaphore name via communication segment (kernel<7.4.3)?
  if ( pShmConnInfo->pSemaNames == NULL )
  {
     pCSHeader->iCSVersion      = COMM_SEG_VERSION_WITH_SEMNAME_REPLY;
  }
  else
  {

    if ( sql33c_VersionIsOlder(pKernelVersion, 7, 6, 0, 14) == IsOlder_esp100 )
    {
        pCSHeader->iCSVersion      = COMM_SEG_VERSION_LOAD_BALANCING;
    }
    else
    {
        pCSHeader->iCSVersion      = COMM_SEG_VERSION;
    }
  }


  pCSHeader->ulCSSize           = pShmConnInfo->ulCSSize;
  pCSHeader->ulPacketCnt        = pShmConnInfo->ulPacketCnt;
  pCSHeader->ulPacketSize       = pShmConnInfo->ulPacketSize;
  pCSHeader->ulMaxDataLen       = pShmConnInfo->ulMaxDataLen;
  pCSHeader->ulMinReplySize     = pShmConnInfo->ulMinReplySize;
  pCSHeader->ulOffsetRecord     = COMM_SEG_HEADER_SIZE;
  pCSHeader->ulCurrentPacket    = 0;
  pCSHeader->pidClientOrPeerPID = pShmConnInfo->pidClientPID;
  pCSHeader->pidServerPID       = (ULONG)UNDEF;
  pCSHeader->ulClientOrPeerRef  = pShmConnInfo->ulClientRef;
  pCSHeader->ulServerRef        = (ULONG)UNDEF;
  pCSHeader->ulClientCommState  = commErrOk_esp01;
  pCSHeader->ulServerCommState  = commErrOk_esp01;
  pCSHeader->ulClientFlag       = 0;
  pCSHeader->ulServerFlag       = 0;
  pCSHeader->ulServiceType      = pShmConnInfo->ulServiceType;
  pCSHeader->ulCommFlagNo       = pShmConnInfo->ulCommFlagNo;
  pCSHeader->ulSemaNo           = 0;

  // -- operating system specific part
  strcpy ( pCSHeader->szClientOrPeerNode, pShmConnInfo->szClientNode );

  GETPROCESSID (&pCSHeader->pidLocalPID);
  strcpy ( pCSHeader->szSemPath, pszClientSemPath );
  strcpy ( pCSHeader->szSemName, pszClientSemName );

  // -- distribution only
  if ( pShmConnInfo->ulServiceType == srvDistribution_esp01 )
    {
    strcpy ( pCSHeader->szServerNode, pShmConnInfo->szServerNode );
    strcpy ( pCSHeader->szServerDB, pShmConnInfo->szServerDB );

    #if defined(KERNEL)
     strcpy ( pCSHeader->szClientOrPeerServerDB, kgs.szServerDB );
    #else
     pCSHeader->szClientOrPeerServerDB[0] = '\0';
    #endif
    }

  //In version 7.6 the applPID is only transfered to support the transmission of the encryption information   
  pCSHeader->applPID    = pShmConnInfo->pidClientPID;
  pCSHeader->encryption = pShmConnInfo->encryption;

  //
  // --- initialize the offset record and save the pointers of
  //     the communication segment parts
  //

  // --- calculate the start address of the 'offset list'
  pCSOffsetList              = (PCOMM_SEG_OFFSET_REC)
                               ((PCHAR)pCSHeader + COMM_SEG_HEADER_SIZE);
  pCSOffsetList->ulOffsetCnt = pShmConnInfo->ulPacketCnt;
  pCSOffsetList->ulOffset[0] = ALIGN ( COMM_SEG_HEADER_SIZE  +
                                       COMM_OFFSET_LIST_SIZE +
                                       ALIGNMENT_VALUE,
                                       ALIGNMENT_VALUE );

  for ( ulCnt = 1; ulCnt < pShmConnInfo->ulPacketCnt; ulCnt++ )
    {
    pCSOffsetList->ulOffset[ulCnt]      = pCSOffsetList->ulOffset[ulCnt-1] +
                                          (RTE_HEADER_SIZE * 2)            +
                                          pShmConnInfo->ulMaxDataLen;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID sql33c_calculate_sizes ( ULONG                  ulPacketCnt,
                                     ULONG                  ulMaxPacketSize,
                                     PULONG                 pulPacketSize,
                                     PULONG                 pulMaxDataLen,
                                     PULONG                 pulCSSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_calculate_sizes"
  ULONG                           ulHeaderSize;
  ULONG                           ulMaxDataLen;
  ULONG                           ulCSSize;

  DBGIN

  // --- calculate the required header size
  ulHeaderSize = ALIGN ( COMM_SEG_HEADER_SIZE  + COMM_OFFSET_LIST_SIZE +
                         ALIGNMENT_VALUE, ALIGNMENT_VALUE ) +
                 (ulPacketCnt * RTE_HEADER_SIZE * 2);


  if ( ulMaxPacketSize == (ULONG)UNDEF )
    {
    if ( *pulPacketSize != 0 )
      {
      ulCSSize       = *pulPacketSize * ulPacketCnt;
      ulMaxDataLen   = (ulCSSize - ulHeaderSize) / ulPacketCnt;

      if ( *pulMaxDataLen > ulMaxDataLen )
        *pulMaxDataLen = ulMaxDataLen;

      *pulMaxDataLen = ALIGN(*pulMaxDataLen + 1,ALIGNMENT_VALUE) -
                       ALIGNMENT_VALUE;
      }
    else
      *pulMaxDataLen = 0;
    }
  else if ( *pulPacketSize == 0 )
    {
    *pulPacketSize = ulMaxPacketSize;
    ulCSSize       = *pulPacketSize * ulPacketCnt;
    *pulMaxDataLen = (ulCSSize - ulHeaderSize) / ulPacketCnt;
    *pulMaxDataLen = ALIGN(*pulMaxDataLen + 1, ALIGNMENT_VALUE) -
                     ALIGNMENT_VALUE;
    }
  else
    {
    if ( *pulPacketSize > ulMaxPacketSize )
      {
      *pulMaxDataLen -= *pulPacketSize - ulMaxPacketSize;
      *pulPacketSize  = ulMaxPacketSize;
      }

    ulMaxDataLen = ((*pulPacketSize * ulPacketCnt)- ulHeaderSize)/ulPacketCnt;

    if ( *pulMaxDataLen > ulMaxDataLen )
      *pulMaxDataLen = ulMaxDataLen;

    *pulMaxDataLen   = ALIGN((*pulMaxDataLen) + 1,ALIGNMENT_VALUE) -
                       ALIGNMENT_VALUE;
    }

  *pulCSSize = *pulPacketSize * ulPacketCnt;

  DBGOUT;
  return;
  }


/*------------------------------*/

static BOOL sql33c_VersionIsOlder( tsp100_VersionID2*      pVersion,
                                   unsigned char           majorVersion,
                                   unsigned char           minorVersion,
                                   unsigned char           corrLevel,
                                   unsigned char           buildNumberPrefix )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_compare_versions"
  tsp100_VersionID1       version;
  tsp100_VersionResult    Result;

  DBGIN;

  version.MajorVersion_sp100      = majorVersion;
  version.MinorVersion_sp100      = minorVersion;
  version.CorrLevel_sp100         = corrLevel;
  version.BuildNumberPrefix_sp100 = buildNumberPrefix;

  // --- set the RTE version string
  Result = sp100_Compare2VersionIDs (VersionIDType1_esp100, pVersion, &version );

  DBGOUT;
  return ( Result == IsOlder_esp100 );
  }

/*------------------------------*/

static ULONG sql33c_get_flag_comm_seg ( PSZ              pszFCSName,
                                        PHANDLE          phFCS,
                                        PFLAG_COMM_SEG   *ppFCS,
                                        tsp00_ErrTextc        pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_get_flag_comm_seg"
  ULONG                  rc = NO_ERROR;

  DBGIN;

  rc = sql41c_get_named_shrd_mem ( (PPVOID)ppFCS, SHM_FCS,
                                   pszFCSName, phFCS );

  if (( rc == ERROR_FILE_NOT_FOUND ) || ( rc == ERROR_INVALID_NAME ))
    {
    MSGD (( ERR_DATABASE_NOT_STARTED, pszFCSName ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_DATABASE_NOT_STARTED, 0 );

    DBGOUT;
    return ( commErrStartRequired_esp01 );
    }

  if ( rc != NO_ERROR )
    {
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql33c_free_flag_comm_seg ( HANDLE            hFCS,
                                         PFLAG_COMM_SEG    pFCS,
                                         tsp00_ErrTextc         pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_free_flag_comm_seg"
  ULONG                  rc = NO_ERROR;

  DBGIN;

  rc = sql41c_free_shrd_mem ( pFCS, hFCS );

  if ( rc != NO_ERROR )
    {
    if ( pErrText != NULL )
      eo46BuildErrorStringRC ( pErrText,  ERRMSG_COM_CANT_FREE_MEM, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql33c_eval_conn_reply ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                      PCONNECT_PARAM_REC    pConnParam,
                                      tsp00_ErrTextc        pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_eval_conn_reply"
  ULONG                           ulCnt;
  PCOMM_SEG_HEADER_REC            pCSHeader;
  PCOMM_SEG_OFFSET_REC            pCSOffsetList;

  DBGIN;

  pCSHeader     = pShmConnInfo->pCSHeader;
  pCSOffsetList = (PCOMM_SEG_OFFSET_REC) ((PCHAR)pCSHeader +
                                                COMM_SEG_HEADER_SIZE);

  pShmConnInfo->ulServerRef     = pCSHeader->ulServerRef;
  pShmConnInfo->ulCommFlagNo    = pCSHeader->ulCommFlagNo;
  pShmConnInfo->pulCommFlag     = (PULONG)((PCHAR) pShmConnInfo->pFCS  +
                                  pShmConnInfo->pFCS->ulCommFlagOffset);
  pShmConnInfo->pulCommFlag    += pShmConnInfo->ulCommFlagNo;

  for ( ulCnt = 0; ulCnt < pShmConnInfo->ulPacketCnt; ulCnt++ )
    {
    pShmConnInfo->pCommPacketList[ulCnt]= (PCOMM_PACKET_REC)((PCHAR)pCSHeader +
                                          pCSOffsetList->ulOffset[ulCnt]);
    }

  // --- set output parameter
  //
  pConnParam->ulServerRef             = pShmConnInfo->ulServerRef;
  pConnParam->ulPacketSize            = pShmConnInfo->ulPacketSize;
  pConnParam->ulMaxDataLen            = pShmConnInfo->ulMaxDataLen;
  pConnParam->ulMinReplySize          = pShmConnInfo->ulMinReplySize;

  for ( ulCnt = 0; ulCnt < pShmConnInfo->ulPacketCnt; ulCnt++ )
    pConnParam->pCommPacketList[ulCnt] = pShmConnInfo->pCommPacketList[ulCnt];

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

#if defined(KERNEL)
  static ULONG sql33c_eval_xserv_conn_reply( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                             PCONNECT_PARAM_REC    pConnParam,
                                             tsp00_ErrTextc             pErrText )
   {
   #undef  MF__
   #define MF__ MOD__"sql33c_eval_xserv_conn_reply"
   ULONG                           ulCnt;
   PCOMM_SEG_HEADER_REC            pCSHeader;
   PCOMM_SEG_OFFSET_REC            pCSOffsetList;
   ULONG                           ulMyHeaderSize;
   ULONG                           ulPeerHeaderSize;

   DBGIN;

   pCSHeader     = pShmConnInfo->pCSHeader;
   pCSOffsetList = (PCOMM_SEG_OFFSET_REC) ((PCHAR)pCSHeader +
                                                 COMM_SEG_HEADER_SIZE);

   ulMyHeaderSize   = pShmConnInfo->ulPacketSize -
                      pShmConnInfo->ulMaxDataLen;
   ulPeerHeaderSize = pCSHeader->ulPacketSize -
                      pCSHeader->ulMaxDataLen;

   if (( ulMyHeaderSize             > ulPeerHeaderSize )         ||
       ( pShmConnInfo->ulPacketSize < pCSHeader->ulPacketSize )  ||
       ( pCSHeader->ulMaxDataLen    < (pCSHeader->ulMinReplySize * 2) ))
     {
     eo46BuildErrorStringRC( pErrText, ERRMSG_INVALID_REPL_PACKET_SIZE, 0 );

     MSGD (( ERR_INVALID_REPL_PACKET_SIZE, pShmConnInfo->ulPacketSize,
             pShmConnInfo->pCSHeader->ulPacketSize, ulMyHeaderSize,
             ulPeerHeaderSize, pShmConnInfo->pCSHeader->ulMinReplySize ));
     DBGOUT;
     return ( commErrNotOk_esp01 );
     }

   pShmConnInfo->ulMinReplySize  = pCSHeader->ulMinReplySize;
   pShmConnInfo->ulPacketSize    = pCSHeader->ulPacketSize;
   pShmConnInfo->ulMaxDataLen    = pCSHeader->ulMaxDataLen;
   pShmConnInfo->ulServerRef     = pCSHeader->ulServerRef;


   for ( ulCnt = 0; ulCnt < pShmConnInfo->ulPacketCnt; ulCnt++ )
     {
     pShmConnInfo->pCommPacketList[ulCnt]= (PCOMM_PACKET_REC)((PCHAR)pCSHeader +
                                           pCSOffsetList->ulOffset[ulCnt]);
     }

   // --- set output parameter
   //
   pConnParam->ulServerRef       = pShmConnInfo->ulServerRef;
   pConnParam->ulPacketSize      = pShmConnInfo->ulPacketSize;
   pConnParam->ulMaxDataLen      = pShmConnInfo->ulMaxDataLen;
   pConnParam->ulMinReplySize    = pShmConnInfo->ulMinReplySize;

   for ( ulCnt = 0; ulCnt < pShmConnInfo->ulPacketCnt; ulCnt++ )
     pConnParam->pCommPacketList[ulCnt] = pShmConnInfo->pCommPacketList[ulCnt];

   DBGOUT;
   return ( commErrOk_esp01 );
   }
#endif

/*------------------------------*/

static VOID sql33c_connect_cleanup ( PSHM_CONNECT_INFO_REC    pShmConnInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_connect_cleanup"

  DBGPAS;

  sql33c_close_all_server_sems( pShmConnInfo, NULL );

  #if !defined (KERNEL)
   if ( pShmConnInfo->hClientSem  != INVALID_HANDLE_VALUE )
     sql33c_close_client_sem( pShmConnInfo->hClientSem, NULL );

   if ( pShmConnInfo->hServerProcess != INVALID_HANDLE_VALUE )
     sql33c_close_server_process ( pShmConnInfo->hServerProcess, NULL );
  #endif

  if ( pShmConnInfo->hQueMailSlot != INVALID_HANDLE_VALUE )
    sql33c_close_queue_mailslot( pShmConnInfo->hQueMailSlot, NULL );

  if ( pShmConnInfo->pFCS != NULL )
    sql33c_free_flag_comm_seg( pShmConnInfo->hFCS, pShmConnInfo->pFCS, NULL );

  if ( pShmConnInfo->pCSHeader != NULL )
    sql33c_free_comm_seg( pShmConnInfo->hCS, pShmConnInfo->pCSHeader, NULL );

  // --- initialize the handles and pointers of connect information record
  sql33c_init_cleanup_conn_info ( pShmConnInfo, NULL, 0, INVALID_HANDLE_VALUE, NULL, NULL );

  return;
  }

/*------------------------------*/

static  ULONG sql33c_init_cleanup_conn_info ( PSHM_CONNECT_INFO_REC pShmConnInfo,
                                              PCONNECT_PARAM_REC    pConnParam,
                                              ULONG                 ulNumOfServerSems,
                                              HANDLE                hFCS,
                                              FLAG_COMM_SEG         *pFCS,
                                              tsp00_ErrTextc             pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_init_cleanup_conn_info"

  ULONG  rc          = NO_ERROR;
  ULONG   ulCommState = commErrOk_esp01;

  DBGPAS;

  if ( ulNumOfServerSems > 0 )
    {
      rc = ALLOC_MEM ( (void**)&(pShmConnInfo->phServerSems), sizeof(HANDLE) * ulNumOfServerSems );

      if ( rc == NO_ERROR )
        {
        ULONG i;

        for ( i = 0; i < ulNumOfServerSems; i++ )
          pShmConnInfo->phServerSems[i] = INVALID_HANDLE_VALUE;
        }
      else
        {
        if ( pErrText != NULL )
          eo46BuildErrorStringRC ( pErrText, ERRMSG_ALLOC_MEMORY, rc );

        DBGOUT;
        return ( commErrNotOk_esp01 );
        }
    } 
  else if ( pShmConnInfo->phServerSems != NULL )
  {
    rc = FREE_MEM ( pShmConnInfo->phServerSems );

    if ( rc != NO_ERROR )
      {
      if ( pErrText != NULL )
        eo46BuildErrorStringRC ( pErrText, ERRMSG_CANT_FREE_MEM, rc );

      ulCommState = commErrNotOk_esp01;
      }
  }

  pShmConnInfo->hFCS        = hFCS;
  pShmConnInfo->pFCS        = pFCS;
  pShmConnInfo->pSemaNames  = NULL;
  pShmConnInfo->pszSemaPath = NULL;

  if ( pFCS != NULL )
    {
    pShmConnInfo->pulSemaFlag = (PULONG)((PCHAR) pFCS + pFCS->ulSemaFlagOffset);

    if ( pFCS->ulCommFlagOffset >= sizeof(FLAG_COMM_SEG) )
      {
      pShmConnInfo->pSemaNames      = (PIPC_BASE_OBJ_NAME) ((PCHAR) pFCS + pFCS->ulSemaNameOffset);
      pShmConnInfo->pszSemaPath     = pFCS->szSemPath;
      }
    }

  pShmConnInfo->hQueMailSlot      = INVALID_HANDLE_VALUE;
  pShmConnInfo->hClientSem        = INVALID_HANDLE_VALUE;
  pShmConnInfo->hCS               = INVALID_HANDLE_VALUE;
  pShmConnInfo->pCSHeader         = NULL;
  pShmConnInfo->hServerProcess    = INVALID_HANDLE_VALUE;
  pShmConnInfo->ulNumOfServerSems = ulNumOfServerSems;
  pShmConnInfo->encryption        = RTE_NO_ENCRYPTION;

  if ( pConnParam == NULL )
    {
    pShmConnInfo->pidClientPID    = (PID)UNDEF;
    pShmConnInfo->szClientNode[0] = '\0';
    pShmConnInfo->szServerDB[0]   = '\0';
    pShmConnInfo->ulServerRef     = (ULONG)UNDEF;
    pShmConnInfo->ulClientRef     = (ULONG)UNDEF;
    pShmConnInfo->ulServiceType   = (ULONG)UNDEF;

    pShmConnInfo->ulCurrentPacket = (ULONG)UNDEF;
    pShmConnInfo->ulCmdDataLen    = (ULONG)UNDEF;
    pShmConnInfo->ulPacketCnt     = 0;

    pShmConnInfo->ulPacketSize    = 0;
    pShmConnInfo->ulMaxDataLen    = 0;
    pShmConnInfo->ulMinReplySize  = (ULONG)UNDEF;
    pShmConnInfo->ulCommFlagNo    = (ULONG)UNDEF;
    }
  else
    {
    pShmConnInfo->pidClientPID    = pConnParam->pidClientPID;
    pShmConnInfo->ulServerRef     = (ULONG)UNDEF;
    pShmConnInfo->ulClientRef     = pConnParam->ulClientRef;
    pShmConnInfo->ulServiceType   = pConnParam->ulServiceType;

    pShmConnInfo->ulCurrentPacket = (ULONG)UNDEF;
    pShmConnInfo->ulCmdDataLen    = (ULONG)UNDEF;
    pShmConnInfo->ulPacketCnt     = pConnParam->ulPacketCnt;

    pShmConnInfo->ulPacketSize    = pConnParam->ulPacketSize;
    pShmConnInfo->ulMaxDataLen    = pConnParam->ulMaxDataLen;
    pShmConnInfo->ulMinReplySize  = (ULONG)UNDEF;

    pShmConnInfo->ulCommFlagNo    = pConnParam->ulCommFlagNo;

    if ( pConnParam->pszClientNode )
      strcpy ( pShmConnInfo->szClientNode, pConnParam->pszClientNode );
    else
      pShmConnInfo->szClientNode[0] = '\0';

    if ( pConnParam->pszServerNode )
      strcpy ( pShmConnInfo->szServerNode, pConnParam->pszServerNode );
    else
      pShmConnInfo->szServerNode[0] = '\0';

    strcpy ( pShmConnInfo->szServerDB,   pConnParam->pszServerDB );
    strupr ( pShmConnInfo->szServerDB );

    pShmConnInfo->encryption    = pConnParam->encryption;
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static ULONG sql33c_get_kernel_pid_and_version( PSZ                     pszServerDB,
                                                PID*                    ppidServer,
                                                tsp100_VersionID2*      pVersion,
                                                tsp00_ErrTextc          pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql33c_get_kernel_pid_and_version"
  ULONG                   rc = NO_ERROR;
  HANDLE                  hKSS     = 0;
  KERNEL_SHARED_SEG       *pKSS    = NULL;
  
  DBGIN;


  //
  // --- get database version
  //
  rc = sql41c_get_named_shrd_mem ( (PPVOID)&pKSS, SHM_KSS, pszServerDB,
                                   &hKSS );

  if (( rc == ERROR_FILE_NOT_FOUND ) || ( rc == ERROR_INVALID_NAME ))
    {
    MSGD (( ERR_DATABASE_NOT_STARTED, pszServerDB ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_DATABASE_NOT_STARTED, 0 );

    DBGOUT;
    return ( commErrStartRequired_esp01 );
    }

  if ( rc != NO_ERROR )
    {
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *pVersion   = pKSS->RTEVersionID;
  *ppidServer = pKSS->KernelPid;

  sql41c_free_shrd_mem ( pKSS, hKSS );

  DBGOUT;
  return ( commErrOk_esp01 );
  }


/*
 * =============================== END ========================================
 */
