/*!
  @file           vos84k.cpp
  @author         RaymondR
  @brief          REQUESTOR thread
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
//#include "gos00.h"
#include "geo00_1.h"
#include "geo00_3.h"
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "gsp01.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "gos003.h"
#include "hos63k.h"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS84KC : "
#define MF__   MOD__"UNDEFINED"


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
ULONG         sql84k_connect              ( PCOMM_SEG_HEADER_REC         pCSHeader,
                                            PSZ                          pszClientObjName,
                                            ULONG                        ulCSSize );

VOID          sql84k_connect_reject       ( ULONG                        ulCommState,
                                            PCOMM_SEG_HEADER_REC         pCSHeader,
                                            PFLAG_COMM_SEG               pPeerFCS,
                                            HANDLE                       hClientSem );

ULONG         sql84k_find_task            ( ULONG                        ulServiceType,
                                            PTASK_CTRL_REC              &pTaskCtrl,
                                            RTETask_CommonQueueElement* &pQueueElement );
ULONG         sql84k_is_ut_task_free      ( VOID );
ULONG         sql84k_find_free_us_ev_task ( ULONG                        ulServiceType,
                                            PTASK_CTRL_REC              &pFreeTaskCtrl,
                                            RTETask_CommonQueueElement* &pQueueElement );
VOID          sql84k_leave_task           ( ULONG                        ulServiceType,
                                            PTASK_CTRL_REC               pTaskCtrl,
                                            RTETask_CommonQueueElement*  pQueueElement );

ULONG         sql84k_send_conn_to_task    ( PCOMM_SEG_HEADER_REC         pCSHeader,
                                            HANDLE                       hCS,
                                            HANDLE                       hPeerFCS,
                                            PFLAG_COMM_SEG               pPeerFCS,
                                            PID                          pidLocalClientPID,
                                            HANDLE                       hLocalClientProcess,
                                            HANDLE                       hClientSem,
                                            PTASK_CTRL_REC               pFreeTaskCtrl,
                                            RTETask_CommonQueueElement*  pQueueElement );

ULONG         sql84k_check_dbstate        ( VOID );

ULONG         sql84k_get_commseg          ( PCOMM_SEG_HEADER_REC         *ppCSHeader,
                                            PHANDLE                      phCS,
                                            PSZ                          pszClientObjName,
                                            ULONG                        ulCSSize );

ULONG         sql84k_get_peer_FCS         ( PSZ                          pszServerDB,
                                            PHANDLE                      phPeerFCS,
                                            PFLAG_COMM_SEG               *ppPeerFCS );
ULONG         sql84k_free_peer_FCS        ( HANDLE                       hPeerFCS,
                                            PFLAG_COMM_SEG               pPeerFCS );
ULONG         sql84k_open_client_sem      ( PCOMM_SEG_HEADER_REC         pCSHeader,
                                            PHANDLE                      phClientSem );

ULONG        sql84k_open_client_process   ( PID                          pidLocalClientPID,
                                            PHANDLE                      phLocalClientProcess );

SAPDB_Bool   sql84k_dump_allowed          ();


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID _System sql84k_requestor ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_requestor"
  APIRET                         rc = NO_ERROR;
  PCOMM_SEG_HEADER_REC           pCSHeader;
  ULONG                          ulCSSize;
  ULONG                          ulCommState;
  COMM_REQUEST_REC               Request;
  ULONG                          ulReqType;
  ULONG                          ulServerRef;
  PSZ                            pszClientObjName;
  PSZ                            pszPath;
  PSZ                            pszName;

  DBGIN;

  __try 
   {
#   if defined(_FASTCAP)
     CAPNameThread ("Requestor");
#   endif

    // --- set thread control info
    kgs.Requestor.ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    //
    //
    // --- set the thread priority
    //
    //

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( kgs.Requestor.ThrdCtrlHeader.lThrdPrio,
                           kgs.Requestor.ThrdCtrlHeader.Tid,
                           kgs.Requestor.ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, kgs.Requestor.ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Requestor.ThrdCtrlHeader.Tid));
      ABORT();
      }

    //
    //
    // --- Create named queue / mailslot for Requestor
    //
    //
    pszPath = MAIL_REQ;
    pszName = kgs.szServerDB;

    rc = sql41c_create_queue_mailslot ( &kgs.Requestor.ThrdCtrlHeader.hComm,
                                        pszPath, pszName,
                                        sizeof (COMM_REQUEST_REC),     
                                        &kgs.WorldSA );                
    if (rc != NO_ERROR)
      ABORT();

    MSGD (( INFO_KERNEL_THREAD_STARTED, "REQUESTOR" ));

    //
    //
    // --- start requestor loop
    //
    //
    for (;;)
      {
      pCSHeader = NULL;
      //
      //
      // --- get the next request
      //
      //
      kgs.Requestor.ThrdCtrlHeader.ThrdState = KT_SLEEPING;

      rc = sql41c_read_queue_mailslot ( kgs.Requestor.ThrdCtrlHeader.hComm,
                                        &Request,
                                        sizeof (COMM_REQUEST_REC),
                                        &ulCSSize,                 // - OS/2
                                        (PPVOID)&pCSHeader );      // - OS/2

      if ( *kgs.pDBState >= SERVER_SHUTDOWN )
      {
        sql41c_close_queue_mailslot ( kgs.Requestor.ThrdCtrlHeader.hComm );
        EXITTHREAD( 0, kgs.Requestor.ThrdCtrlHeader.hThrd );
      }

      if ( rc != NO_ERROR )
        continue;

      kgs.Requestor.ThrdCtrlHeader.ThrdState = KT_RUNNING;


      ulCSSize         = Request.ulCSSize;
      ulReqType        = Request.ulReqType;
      ulServerRef      = Request.ulServerRef;
      pszClientObjName = Request.szClientObjName;


      //
      // --- examine request
      //
      switch ( ulReqType )
        {
        case SQL_RTE_WAKEUP:
          break;  // --- nothing to do, might be a request from a second kernel

        case SQL_RTE_CONNECT:           // --- Connect request?
          sql84k_connect ( pCSHeader, pszClientObjName, ulCSSize );
          break;

        case SQL_RTE_CANCEL:            // --- Cancel request?
          ulCommState = sql84k_check_dbstate ();

          if ( ulCommState == commErrOk_esp01 )
            {
            if (( ulServerRef >= 1) && ( ulServerRef <= kgs.ulNumOfTasks ))
              sql53k_comm_cancel ( &kgs.pFirstTaskCtrl[ulServerRef - 1], commErrOk_esp01 );
            else
              MSGALL(( ERR_CANCEL_REQ_UNKNOWN_TSK, ulServerRef ));
            }
          break;

        case SQL_RTE_DUMP:              // --- Dump request?

          if ( sql84k_dump_allowed() )
            {
            MSGALL(( ERR_DUMP_REQUEST ));
            ABORT();
            }
          break;

        default:                        // --- Wrong request!

          MSGCD (( ERR_WRONG_REQ_REQUEST ));
          DBG1 ((MF__, "Wrong requestor thread request" ));
          break;
        }
      }
    }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  MSGD (( INFO_KERNEL_THREAD_STOPPED, "REQUESTOR" ));

  DBGOUT;
  EXITPROCESS(0);
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static SAPDB_Bool sql84k_dump_allowed()
{
  SAPDB_Bool                     dumpEnabled = false;

   SAPDBErr_MessageList          errList;
   RTEConf_Parameter::String     valueString;

   if((RTEConf_ParameterAccess::Instance())->GetString( KERNELPARAM_EXTERNAL_DUMP_REQUEST, valueString, errList))
   {
       if ( 0 == strcmp("YES", (char*)valueString ))
           dumpEnabled = true;
       else
           dumpEnabled = false;
   }
   else
       dumpEnabled = false;

   return dumpEnabled;
}

/*------------------------------*/

static ULONG sql84k_connect ( PCOMM_SEG_HEADER_REC   pCSHeader,
                              PSZ                    pszClientObjName,
                              ULONG                  ulCSSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_connect"

  PTASK_CTRL_REC                 pTaskCtrl           = NULL;
  HANDLE                         hPeerFCS            = (HANDLE)INVALID_HANDLE_VALUE;
  PFLAG_COMM_SEG                 pPeerFCS            = NULL;
  HANDLE                         hCS                 = (HANDLE)INVALID_HANDLE_VALUE;
  ULONG                          ulCommState         = commErrOk_esp01;
  APIRET                         rc                  = NO_ERROR;
  HANDLE                         hLocalClientProcess = (HANDLE)NULL;
  HANDLE                         hClientSem          = INVALID_HANDLE_VALUE;
  RTETask_CommonQueueElement*    pQueueElement       = 0;

  DBGIN;

  //
  //
  // --- get shared communication segment
  //
  //
  ulCommState = sql84k_get_commseg ( &pCSHeader, &hCS,
                                     pszClientObjName, ulCSSize );

  //
  // --- Connect peer kernel comm. segment. This must be done
  //     after openning the shared communication segment because
  //     it's also needed for connect rejects.
  //
  if (( ulCommState                          == commErrOk_esp01 )            &&
      ( pCSHeader->ulServiceType             == srvDistribution_esp01 ) &&
      ( pCSHeader->szClientOrPeerServerDB[0] != '\0' ))
    {
    ulCommState = sql84k_get_peer_FCS( pCSHeader->szClientOrPeerServerDB,
                                       &hPeerFCS, &pPeerFCS );
    }

  //
  //
  // --- if no error, open client semaphore
  //
  //
  if ( ulCommState == commErrOk_esp01 )
    {
    ulCommState = sql84k_open_client_sem ( pCSHeader, &hClientSem );
    }

  //
  //
  // --- if no error, check datbase state
  //
  //
  if ( ulCommState == commErrOk_esp01 )
  {
    ulCommState = sql84k_check_dbstate ();
  }
  //
  //
  // --- if no error, find a free task 
  //
  //
  if ( ulCommState == commErrOk_esp01 )
  {
    ulCommState = sql84k_find_task ( pCSHeader->ulServiceType, pTaskCtrl, pQueueElement );
  }

  //
  //
  // --- if no error, open client proccess handle
  //
  //
  if ( ulCommState == commErrOk_esp01 )
    {
    ulCommState = sql84k_open_client_process  ( pCSHeader->pidLocalPID,
                                                &hLocalClientProcess );
    }

  // --- if no error, send connect request to the requested task
  if ( ulCommState == commErrOk_esp01 )
    {
    ulCommState = sql84k_send_conn_to_task ( pCSHeader, hCS, hPeerFCS, pPeerFCS, pCSHeader->pidLocalPID,
                                             hLocalClientProcess, hClientSem, pTaskCtrl, pQueueElement );
    }

  if ( ulCommState != commErrOk_esp01 )
    {
    //
    //
    // --- on error, reject the connect request
    //
    //
    sql84k_connect_reject ( ulCommState, pCSHeader, pPeerFCS, hClientSem );

    if ( pTaskCtrl != NULL )
      {
      sql84k_leave_task ( pCSHeader->ulServiceType, pTaskCtrl, pQueueElement  );
      }
    //
    // --- close all obejects
    //
    if ( hClientSem != INVALID_HANDLE_VALUE )
      sql41c_close_event_sem ( hClientSem, "Client" );

    if ( hLocalClientProcess != NULL )
      CloseHandle ( hLocalClientProcess );

    // --- free peer kernel communication segment
    if ( pPeerFCS != NULL )
      {
      sql84k_free_peer_FCS ( hPeerFCS, pPeerFCS );

      if ( rc != NO_ERROR )
        {
        DBG1 ((MF__, "Cannot free memory, rc = %u", rc ));
        ABORT();
        }
      }

    if ( pCSHeader != NULL )
      {
      rc = sql41c_free_shrd_mem( pCSHeader, hCS );

      if ( rc != NO_ERROR )
        {
        DBG1 ((MF__, "Cannot free memory, rc = %u", rc ));
        ABORT();
        }
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/


static ULONG sql84k_check_dbstate ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_check_dbstate"

  switch ( *kgs.pDBState )
    {
    case SERVER_STARTING      :
      return ( commErrStartRequired_esp01 );

    case SERVER_SHUTDOWN      :
    case SERVER_SHUTDOWNREINIT:
    case SERVER_SHUTDOWNKILL  :
      return ( commErrShutdown_esp01 );

    case SERVER_ABORT         :
    case SERVER_STOP          :
    case SERVER_KILL          :
    default                   :
      return ( commErrCrash_esp01 );

    case SERVER_COLD          :
    case SERVER_WARM          :
    case SERVER_STANDBY_EO00  :
      break;
    }

  return ( commErrOk_esp01 );
  }

/*------------------------------*/


static ULONG sql84k_find_task ( ULONG                       ulServiceType,
                                PTASK_CTRL_REC              &pTaskCtrl,
                                RTETask_CommonQueueElement* &pQueueElement )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_find_task"
  ULONG     ulCommState = commErrOk_esp01;
  APIRET    rc          = NO_ERROR;

  DBGIN;

  //
  // --- find free task
  //
  switch ( ulServiceType )
    {
    case srvDistribution_esp01:        // -- Is it for the receiver task?
      ulCommState = commErrNotOk_esp01; // not supported */
      break;

    case srvUtility_esp01:         
      ulCommState = sql84k_is_ut_task_free ();

      if ( ulCommState == commErrOk_esp01 )
      {
        pTaskCtrl     = kgs.pUT;
        pQueueElement = RTE_KGS::Instance().GlobalFreeList()->Dequeue();
      }
      else
        pTaskCtrl  = 0;
      break;

    default:   // ---  It's a user or event task!
      ulCommState = sql84k_find_free_us_ev_task ( ulServiceType, pTaskCtrl, pQueueElement );
      break;
    }

  if ( ulCommState == commErrOk_esp01 )
    pTaskCtrl->pUserCommCtrl->ulConnected = kgs.ulCurrTime;

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static VOID sql84k_leave_task  ( ULONG                        ulServiceType,
                                 PTASK_CTRL_REC               pTaskCtrl,
                                 RTETask_CommonQueueElement*  pQueueElement )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_leave_task"


  if ( 0 != pQueueElement )
  {
      // we don't need the queue element release it!
      pQueueElement->Release();

      pTaskCtrl->pTaskObject->EnqueueIntoUnconnectedTasksQueue();
  }

  if ( pTaskCtrl != NULL )
    pTaskCtrl->pUserCommCtrl->ulConnected = 0;


  return;
  }

/*------------------------------*/

static ULONG sql84k_find_free_us_ev_task ( ULONG                       ulServiceType,
                                           PTASK_CTRL_REC              &pFreeTaskCtrl,
                                           RTETask_CommonQueueElement* &pQueueElement )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_find_free_us_ev_task"

  static RTECKC_KernelCommunication *  pKernelCommunication = 0;

  if ( !pKernelCommunication )
      pKernelCommunication = RTE_KGS::Instance().KernelCommunication();


  if ( kgs.fAlwaysCheckConnections == true )
    { 
    // --- try to remove unreleased connections
    sql53k_remove_unrel_connnections ( );
    }

  pQueueElement = pKernelCommunication->UnconnectedTaskPool().GetTask( 
      ulServiceType == srvEvent_esp01 ? RTETask_ITask::Event : RTETask_ITask::User );

  if ( 0 == pQueueElement )
    {
    MSGD (( INFO_CON_REQ_BUT_US_BUSY ));

    pFreeTaskCtrl = 0;
    return ( commErrTasklimit_esp01 );
    }

  pFreeTaskCtrl = pQueueElement->Task()->LegacyTaskCtrl();
  pFreeTaskCtrl->pUserCommCtrl->ulServiceType = ulServiceType;

  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_is_ut_task_free ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_is_ut_task_free"
  PTASK_CTRL_REC                  pTaskCtrl = kgs.pUT;

  DBGIN;

  if ( kgs.pUT->pUserCommCtrl->ulConnected )
    {
    // --- sleep a moment
    SLEEP ( 3000 );

    // --- try to remove unreleased connections
    sql53k_remove_unrel_connnections ( );

    }

  DBG3 (( MF__, "Trying to connect utility" ));

  if ((( pTaskCtrl->TaskState                  == TSK_INACTIVE_EO00    ) ||
       ( pTaskCtrl->TaskState                  == TSK_CONNECTWAIT_EO00 )) &&
      (  pTaskCtrl->pUserCommCtrl->ulConnected == 0 ))
    {
    DBGOUT;
    return ( commErrOk_esp01 );
    }

  MSGD (( INFO_CON_REQ_BUT_UT_BUSY ));

  DBGOUT;
  return ( commErrTasklimit_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_get_commseg ( PCOMM_SEG_HEADER_REC   *ppCSHeader,
                                  PHANDLE                phCS,
                                  PSZ                    pszClientObjName,
                                  ULONG                  ulCSSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_get_commseg"
  APIRET                  rc = NO_ERROR;

  DBGIN;

  if ( ulCSSize < MIN_COMSEG_SIZE )
    {
    MSGD (( ERR_WRONG_COMM_REC ));
    DBG1 ((MF__, "Wrong communication segment" ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }
  else
    {
    rc = sql41c_get_named_shrd_mem ( (PPVOID)ppCSHeader,
                                     SHM_COMMSEG, pszClientObjName,
                                     phCS );
    if ( rc != NO_ERROR )
      {
      if ( rc == ERROR_FILE_NOT_FOUND )
        MSGD (( INFO_USER_CONN_LOST ));

      *ppCSHeader = NULL;

      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_get_peer_FCS ( PSZ              pszServerDB,
                                   PHANDLE          phPeerFCS,
                                   PFLAG_COMM_SEG  *ppPeerFCS )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_get_peer_FCS"
  APIRET                  rc = NO_ERROR;

  DBGIN;

  rc = sql41c_get_named_shrd_mem ( (PPVOID)ppPeerFCS, SHM_FCS,
                                   pszServerDB, phPeerFCS );

  if (rc != NO_ERROR)
    {
    if ( rc == ERROR_INVALID_NAME )
      MSGD (( ERR_DATABASE_NOT_STARTED, pszServerDB ));

    *ppPeerFCS = NULL;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_free_peer_FCS ( HANDLE            hPeerFCS,
                                    PFLAG_COMM_SEG    pPeerFCS )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_free_peer_FCS"
  APIRET                  rc = NO_ERROR;

  DBGIN;

  rc = sql41c_free_shrd_mem ( pPeerFCS, hPeerFCS );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_open_client_sem ( PCOMM_SEG_HEADER_REC   pCSHeader,
                                      PHANDLE                phClientSem )
  {
  #undef  MF__
  #define MF__ MOD__""
  APIRET                         rc = NO_ERROR;
  PSZ                            pszPath        = NULL;
  PSZ                            pszName        = NULL;

  DBGIN;

  pszPath        = pCSHeader->szSemPath;
  pszName        = pCSHeader->szSemName;
  *phClientSem = 0;

  rc = sql41c_open_event_sem( phClientSem, pszPath, pszName, NO_ERROR );

  if ( rc != NO_ERROR )
    {
    // --- Probably, the user died in between because of too slow response
    if ( rc == ERROR_FILE_NOT_FOUND )
      MSGD (( INFO_USER_CONN_LOST ));

    *phClientSem = INVALID_HANDLE_VALUE;

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG sql84k_open_client_process ( PID     pidLocalClientPID,
                                          PHANDLE phLocalClientProcess )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_open_client_process"
  APIRET                         rc = NO_ERROR;

  *phLocalClientProcess = OpenProcess ( SYNCHRONIZE, FALSE,
                                        pidLocalClientPID );

  if ( *phLocalClientProcess == NULL )
    {
    rc = GetLastError();

    DBG1((MF__, "Unable to open proccess, rc=%d", rc));

    // --- Probably, the user died in between because of too slow response
    if ( rc == ERROR_INVALID_PARAMETER )
      MSGD (( INFO_USER_CONN_LOST ))
    else if (rc == ERROR_ACCESS_DENIED)
      MSGCD (( ERR_SYNC_ACCESS_DENIED ))
    else
      MSGCD (( ERR_OPEN_PROCCESS, rc ));

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static ULONG  sql84k_send_conn_to_task ( PCOMM_SEG_HEADER_REC         pCSHeader,
                                         HANDLE                       hCS,
                                         HANDLE                       hPeerFCS,
                                         PFLAG_COMM_SEG               pPeerFCS,
                                         PID                          pidLocalClientPID,
                                         HANDLE                       hLocalClientProcess,
                                         HANDLE                       hClientSem,
                                         PTASK_CTRL_REC               pFreeTaskCtrl,
                                         RTETask_CommonQueueElement*  pQueueElement )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_send_conn_to_task"
  PDLQ_REC   pRequest;

  DBGIN;

  // --- start user or utility task if inactive!
  if (( pFreeTaskCtrl->TaskType == TT_US_EO00 ||
        pFreeTaskCtrl->TaskType == TT_EV_EO00 )||
      ( pFreeTaskCtrl->TaskType == TT_UT_EO00 ))
    {
    SAPDBErr_MessageList messageList;

    if ( false == pFreeTaskCtrl->pTaskObject->Startup( messageList ))
      {
      RTE_Message (messageList, MSG_DIAG_CONSOLE);
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  //
  // --- build up connect request DLQ element
  //
  pRequest = *pQueueElement; 

  pRequest->ulReqType = REQ_CONNECT;

  pRequest->ReqArgs.ConnectRequest.hCS                 = hCS;            
  pRequest->ReqArgs.ConnectRequest.pCSHeader           = pCSHeader;
  pRequest->ReqArgs.ConnectRequest.hPeerFCS            = hPeerFCS;       
  pRequest->ReqArgs.ConnectRequest.pPeerFCS            = pPeerFCS;
  pRequest->ReqArgs.ConnectRequest.hLocalClientProcess = hLocalClientProcess; 
  pRequest->ReqArgs.ConnectRequest.pidLocalClientPID   = pidLocalClientPID;
  pRequest->ReqArgs.ConnectRequest.hClientSem          = hClientSem;

  sql74k_UKT_enqu_and_wake ( pFreeTaskCtrl->pUKT, pFreeTaskCtrl, pRequest );

  DBGOUT
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static VOID sql84k_connect_reject ( ULONG                  ulCommState,
                                    PCOMM_SEG_HEADER_REC   pCSHeader,
                                    PFLAG_COMM_SEG         pPeerFCS,
                                    HANDLE                 hClientSem )
  {
  #undef  MF__
  #define MF__ MOD__"sql84k_connect_reject"
  PULONG        pulPeerCommFlag;

  DBGPAS;

  MSGD (( INFO_REQUEST_REJECT ));


  if ( pCSHeader != NULL )
    {
    if ( pPeerFCS != NULL )
      {
      pulPeerCommFlag  = (PULONG)((PCHAR) pPeerFCS          +
                                 pPeerFCS->ulCommFlagOffset +
                                 pCSHeader->ulCommFlagNo);
      *pulPeerCommFlag = 1;
      }

    pCSHeader->ulServerCommState = ulCommState;  // --- set comm. state
    pCSHeader->ulServerFlag      = 1;

    if ( hClientSem != INVALID_HANDLE_VALUE )
      {
      // --- try to wake up the application
      sql41c_post_event_sem ( hClientSem, "Application" );
      }
    }

  return;
  }

/*
 * =============================== END ========================================
 */
