/*!
  @file           vos83k.cpp
  @author         RaymondR
  @brief          TIMER Thread
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
#define KERNEL

#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "gos003.h"
#include "RunTime/Threading/RTEThread_ConsoleConnections.h"
#include "RunTime/RTE_KSS.h"
#include "RunTime/RTE_DiagPipe.h"
#include "RunTime/RTE_KernelMain.h"
#include "RunTime/RTE_SpawnRestartShutdown.h"
#include "RunTime/Tasking/RTETask_Task.hpp"

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS83KC : "
#define MF__   MOD__"UNDEFINED"

#define INFINIT_TIME_VAL         (tsp00_Longuint)-1

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
static VOID  sql83k_activate_task             ( PDLQ_REC         pDLQElem );
static BOOL  sql83k_special_tasks_ready       ( VOID );
static BOOL  sql83k_tasks_ready               ( PTASK_CTRL_REC   pTaskCtrl ) ;
static VOID  sql83k_remove_unrel_connections  ( tsp00_Longuint   ulTimeNow,
                                               tsp00_Longuint    *pulSleepTime );
static VOID  sql83k_load_balancing            ( tsp00_Longuint   ulTimeNow,
                                                tsp00_Longuint   *pulSleepTime );

static VOID  sql83k_handle_request_timeouts   ( tsp00_Longuint   ulTimeNow,
                                                tsp00_Longuint   *pulSleepTime );




/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

extern "C" VOID _System sql83k_timer ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_timer"
  APIRET                       rc = NO_ERROR;
  PDLQ_REC                     pRequest;
  PDLQ_REC                     pFirstRequest;
  PDLQ_REC                     pNextRequest;
  tsp00_Longuint               ulTimeNow;
  tsp00_Longuint               ulSleepTime;
  SAPDB_ULong                  lastConsoleScanTime;



  DBGIN;

  __try 
   {
#   if defined(_FASTCAP)
     CAPNameThread ("Timer");
#   endif

    // --- Create event semaphore (SET) for TIMER
    rc = sql41c_create_event_sem (&kgs.Timer.ThrdCtrlHeader.hevSem,
                                  NULL,
                                  NULL,
                                  0,
                                  FALSE,
                                  NULL);

    if (rc != NO_ERROR)
      ABORT();

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( kgs.Timer.ThrdCtrlHeader.lThrdPrio,
                           kgs.Timer.ThrdCtrlHeader.Tid,
                           kgs.Timer.ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, kgs.Timer.ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Timer.ThrdCtrlHeader.Tid));
      ABORT();
      }

    MSGD (( INFO_KERNEL_THREAD_STARTED, "TIMER" ));

    // --- set thread control info
    kgs.Timer.ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;
    kgs.Timer.ThrdCtrlHeader.ThrdState   = KT_RUNNING;

    lastConsoleScanTime = kgs.ulCurrTime;
    /*
     *  loop forever
     */
    for (;;)
      {
      pRequest    = NULL;
      ulTimeNow   = kgs.ulCurrTime;
      ulSleepTime = (ULONG)-1;

      //
      // --- get all tasks which need to be awakened
      //
      WAIT_UNTIL_ALONE(&kgs.Timer.DLQ_Work.lExclusive);

      pFirstRequest = kgs.Timer.DLQ_Work.pFirstFreeElem;
      pNextRequest  = pFirstRequest;

      if (pFirstRequest)
        {
        if (pFirstRequest->ReqArgs.TimerRequest.ulTimeToWake <= ulTimeNow)
          {
          pNextRequest = pFirstRequest->pForward;
          pRequest     = pFirstRequest;

          while (pNextRequest)
            {
            if (pNextRequest->ReqArgs.TimerRequest.ulTimeToWake > ulTimeNow)
              break;

            pRequest     = pNextRequest;
            pNextRequest = pRequest->pForward;
            }
          }
        else
          {
          pFirstRequest = NULL;
          }
        }

      kgs.Timer.DLQ_Work.pFirstFreeElem = pNextRequest;

      // --- compute the sleep time
      if ( pNextRequest )
        ulSleepTime = (ULONG)(pNextRequest->ReqArgs.TimerRequest.ulTimeToWake - ulTimeNow);

      if ( pRequest )
        pRequest->pForward = NULL;

      CLEARLOCK(&kgs.Timer.DLQ_Work.lExclusive);


      // ---  'pFirstRequest' points to list of tasks to awaken
      while ( pFirstRequest )
        {
        pRequest      = pFirstRequest;
        pFirstRequest = pRequest->pForward;
        sql83k_activate_task ( pRequest );
        }

      // ---- are all special tasks ready
      if (( *kgs.pDBState             == SERVER_STARTING ) &&
          ( sql83k_special_tasks_ready () == FALSE ))
        {
        // ---  each 'STARTUPSLEEPTIME' seconds look
        //      if all special tasks are ready
        ulSleepTime = STARTUPSLEEPTIME;
        }
      else
        {
        sql83k_remove_unrel_connections ( ulTimeNow, &ulSleepTime );
        sql83k_load_balancing ( ulTimeNow, &ulSleepTime );
        sql83k_handle_request_timeouts( ulTimeNow, &ulSleepTime );
        }

      //
      // ---  Timeout scanner for console connections
      //
 
      if (ulTimeNow - lastConsoleScanTime > CONNECTION_SCAN_INTERVALL)
      {
          //SAPDBTRACE_WRITELN (Console_Trace, 1, "Starting timeout scanning");
          RTEThread_ConsoleScanConnectionsForTimeout ();
          lastConsoleScanTime = ulTimeNow;
      }
      

      //
      // ---  limit the sleep time
      //
      if ( ulSleepTime < 1 )
        ulSleepTime = 1;
      else if ( ulSleepTime > MIN_COMMAND_TIMEOUT )
        ulSleepTime = MIN_COMMAND_TIMEOUT;

      DBG3 (( MF__, "ulSleepTime : %u", ulSleepTime ));

      kgs.Timer.ThrdCtrlHeader.ThrdState = KT_SLEEPING;

      rc =  sql41c_wait_event_sem (kgs.Timer.ThrdCtrlHeader.hevSem, (ULONG)(ulSleepTime * 1000), "Timer");

      kgs.Timer.ThrdCtrlHeader.ThrdState = KT_RUNNING;

      if ((rc != NO_ERROR) && (rc != ERROR_TIMEOUT))
        ABORT();

      }
  }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  MSGD (( INFO_KERNEL_THREAD_STOPPED, "TIMER" ));

  DBGOUT;
  return;
  }

/*------------------------------*/

extern "C" VOID  sql83k_timer_job ( PDLQ_REC pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_timer_job"
  PDLQ_REC                     pCurrReq;
  PDLQ_REC                     pNextReq;
  BOOLEAN                      fWake,
                               fFound;

  DBGIN;

  fWake  = FALSE ;
  fFound = FALSE ;

  WAIT_UNTIL_ALONE(&kgs.Timer.DLQ_Work.lExclusive);

  if ( kgs.Timer.DLQ_Work.pFirstFreeElem == NULL )
    {
    kgs.Timer.DLQ_Work.pFirstFreeElem  = pRequest;
    pRequest->pForward                 = NULL;
    fWake                              = TRUE;
    }
  else
    {
    pCurrReq = kgs.Timer.DLQ_Work.pFirstFreeElem;
    if ( pCurrReq->ReqArgs.TimerRequest.ulTimeToWake >
         pRequest->ReqArgs.TimerRequest.ulTimeToWake )
      {
      // --- First entry new - wake
      fFound                            = TRUE;
      fWake                             = TRUE;
      pRequest->pForward                = pCurrReq;
      kgs.Timer.DLQ_Work.pFirstFreeElem = pRequest;
      }
    else
      {
      pCurrReq = kgs.Timer.DLQ_Work.pFirstFreeElem;
      pNextReq = pCurrReq->pForward;
      while ( ! fFound )
        {
        if ( ! pNextReq )
          {
          fFound             = TRUE;
          pRequest->pForward = pNextReq;
          pCurrReq->pForward = pRequest;
          break;
          }

        if ( pNextReq->ReqArgs.TimerRequest.ulTimeToWake >
             pRequest->ReqArgs.TimerRequest.ulTimeToWake )
          {
          fFound             = TRUE;
          pRequest->pForward = pNextReq;
          pCurrReq->pForward = pRequest;
          break;
          }
        pCurrReq = pNextReq;
        pNextReq = pCurrReq->pForward;
        }
      }
    }

  CLEARLOCK(&kgs.Timer.DLQ_Work.lExclusive);

  if ( fWake )
    sql72k_wake_kt ( &kgs.Timer.ThrdCtrlHeader );

  DBGOUT;
  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


static BOOL  sql83k_special_tasks_ready (VOID)
{
  #undef  MF__
  #define MF__ MOD__"sql83k_special_tasks_ready"
  BOOL      fReady       = FALSE;

  DBGPAS;

  if (    kgs.pTI->TaskState == TSK_SLEEP_EO00
       && kgs.pAL->TaskState == TSK_VSUSPEND_EO00
       && kgs.pTW->TaskState == TSK_VSUSPEND_EO00
       && sql83k_tasks_ready ( kgs.pFirstDWTaskCtrl )
       && ( XPARAM(fDynamicServerTasks) ||
            sql83k_tasks_ready ( kgs.pFirstServerTaskCtrl ) )
       )

  {
    fReady = TRUE;

    RTE_SetDatabaseState(SERVER_ADMIN_EO00);

    // --- Write a message to the event log (Version included)
    //     and start the diagnostic file write cycle
    if (( pkss->KernelVersion != NULL ) && ( pkss->KernelVersion[0] != '\0' ))
    {
      MSGCEL (( INFO_DB_STARTED_VERS_DBROOT,
                kgs.szServerDB,
                pkss->KernelVersion,
                kgs.szDBRoot ));
    }
    else
    {
      MSGCEL (( INFO_DB_STARTED_VERS_DBROOT,
                kgs.szServerDB,
                pkss->RTEVersion,
                kgs.szDBRoot ));
    }
    MSGD(( INFO_DATABASE_READY ));
    sql60_strt_msg_wrt_cycle (  );


    if ( FORCE_AUTORESTART != (kgs.ulStartupOptions & FORCE_AUTORESTART)
      || !RTE_SpawnRestartShutdown( true ) )
    {
        RTE_CloseDiagPipe();
        sql61k_post_xstart_sem ( kgs.szServerDB, FALSE );
    }
  }

  return ( fReady );
}

/*------------------------------*/

static BOOL  sql83k_tasks_ready ( PTASK_CTRL_REC pTaskCtrl )

{
  #undef  MF__
  #define MF__ MOD__"sql83k_tasks_ready"

  BOOL            fTasksStarted ;
  UINT1           TaskType = pTaskCtrl->TaskType ;

  DBGIN;

  do
    {
      fTasksStarted = pTaskCtrl->TaskState == TSK_VSUSPEND_EO00 ;
      pTaskCtrl++ ;
    }
  while ( TaskType == pTaskCtrl->TaskType && fTasksStarted ) ;

  DBGOUT;

  return ( fTasksStarted ) ;
}

/*------------------------------*/

static VOID  sql83k_activate_task ( PDLQ_REC pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_activate_task"

  DBGIN;

  sql74k_UKT_enqu_and_wake( pRequest->pTaskCtrl->pUKT, pRequest->pTaskCtrl, pRequest );

  DBG3 (( MF__, "Timer wakes TID:%u, [T:0x%03u], wake time is %u",
                pRequest->pTaskCtrl->pUKT->ThrdCtrlHeader.Tid,
                pRequest->pTaskCtrl->ulTaskIndex,
                pRequest->ReqArgs.TimerRequest.ulTimeToWake ));

  DBGOUT;
  return;
  }
  
/*------------------------------*/

static VOID  sql83k_remove_unrel_connections ( tsp00_Longuint   ulTimeNow,
                                               tsp00_Longuint   *pulSleepTime )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_remove_unrel_connections"
  static tsp00_Longuint        ulNextUnrelConnTest = 0;

  DBGIN;

  if ( ulNextUnrelConnTest <= ulTimeNow )
    {
    ulNextUnrelConnTest = ulTimeNow + MIN_UNREL_CONN_TEST;

    // - try to remove unreleased user connections
    sql53k_remove_unrel_connnections ( );
    }

  if (( *pulSleepTime == INFINIT_TIME_VAL ) || ( ulTimeNow + *pulSleepTime > ulNextUnrelConnTest ))
    *pulSleepTime = ulNextUnrelConnTest - ulTimeNow;

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID  sql83k_load_balancing ( tsp00_Longuint   ulTimeNow,
                                     tsp00_Longuint   *pulSleepTime )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_load_balancing"
  static tsp00_Longuint     uNextMoveTaskInterval = 0;
  static tsp00_Longuint     uLoadBalancingCheck   = 0;

  DBGIN;

  if (  XPARAM(uLoadBalancingCheck) == 0 )
    return;

  if ( uLoadBalancingCheck != XPARAM(uLoadBalancingCheck))
  { // XPARAM(uLoadBalancingCheck) value has changed. 
    // From now we will use the new one
    uLoadBalancingCheck   = XPARAM(uLoadBalancingCheck);
    uNextMoveTaskInterval = ulTimeNow + uLoadBalancingCheck;
  }

  if ( uNextMoveTaskInterval <= ulTimeNow )
  {                         
    tsp00_Longuint     WaitTime;

    WaitTime = sql74k_load_balancing();

    if ( ulTimeNow != INFINIT_TIME_VAL )
      uNextMoveTaskInterval = ulTimeNow + WaitTime;
  }

  if (( *pulSleepTime == INFINIT_TIME_VAL ) || 
      ( ulTimeNow + *pulSleepTime >  uNextMoveTaskInterval ))
  {
    *pulSleepTime = uNextMoveTaskInterval - ulTimeNow;
  }

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID  sql83k_handle_request_timeouts ( tsp00_Longuint   ulTimeNow,
                                              tsp00_Longuint   *pulSleepTime )
  {
  #undef  MF__
  #define MF__ MOD__"sql83k_handle_request_timeouts"
  static tsp00_Longuint     uNextRequestTimeoutInterval = 0;

  DBGIN;

  if ( uNextRequestTimeoutInterval <= ulTimeNow )
  {                         
      for ( PTASK_CTRL_REC pCurrTask = kgs.pFirstTaskCtrl;
          ( pCurrTask != NULL ) && (pCurrTask <= kgs.pLastTaskCtrl);
          pCurrTask++)
      {
        pCurrTask->pTaskObject->HandleRequestTimeout();
      }

    if ( ulTimeNow != INFINIT_TIME_VAL )
      uNextRequestTimeoutInterval = ulTimeNow + HANDLE_REQUEST_TIMEOUT_INTERVAL;
  }

  if (( *pulSleepTime == INFINIT_TIME_VAL ) || 
      ( ulTimeNow + *pulSleepTime >  uNextRequestTimeoutInterval ))
  {
    *pulSleepTime = uNextRequestTimeoutInterval - ulTimeNow;
  }

  DBGOUT;
  return;
  }

/*
 * =============================== END ========================================
 */
