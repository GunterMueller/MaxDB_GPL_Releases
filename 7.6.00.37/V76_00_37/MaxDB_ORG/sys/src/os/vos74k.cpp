/*!
  @file           vos74k.cpp
  @author         RaymondR
  @brief          dispatcher
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
#include "geo007_1.h"
#include "geo50_0.h"
#include "geo002.h"
#include "gos003.h"
#include "heo52.h"
#include "heo69.h"
#include "heo92.h"
#include "heo670.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "gos74.h"
#include "Trace/Trace_Manager.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS74KC : "
#define MF__   MOD__"UNDEFINED"


#define INFINIT_TIME_VAL         (SAPDB_UInt8)-1

#define DO_NOT_POST_FLAG 1
/*
 *  MACROS
 */

#define LOOP_CNT()                                                            \
        ( pUKT->ulNumRegWait &&                                               \
        ( XPARAM(ulRegionCollisionLoop) > XPARAM(ulDispLoopsBefSemop) ))\
        ? XPARAM(ulRegionCollisionLoop) : XPARAM(ulDispLoopsBefSemop)

// --  At system termination, ignore all requests
//     except for tracewriter
//
#define IGNORE_SOME_REQUESTS_AT_SYSTEM_END( _pTaskCtrl, _leave )             \
        if (( *kgs.pDBState      >= SERVER_SHUTDOWN) &&                      \
            ( (_pTaskCtrl)->TaskType != TT_TW_EO00 ))                        \
          {                                                                  \
          (_pTaskCtrl)->TaskState = TSK_LOCKED_EO00;                         \
          _pTaskCtrl              = NULL;                                    \
          _leave;                                                            \
          }

#define CHECK_RESCHEDULE_INACT_TASK( _pTaskCtrl )                            \
        if ((( (_pTaskCtrl)->TaskState == TSK_INITIALIZED_EO00 ) ||          \
             ( (_pTaskCtrl)->TaskState == TSK_MARKED_FOR_STARTUP_EO00 )) &&  \
            (( (_pTaskCtrl)->TaskType  == TT_US_EO00 )           ||          \
             ( (_pTaskCtrl)->TaskType  == TT_EV_EO00 )           ||          \
             ( (_pTaskCtrl)->TaskType  == TT_SV_EO00 )))                     \
          {                                                                  \
          MSGCD (( ERR_RESCHEDULE_INACT_TASK, (_pTaskCtrl)->ulTaskIndex ));  \
          DBG1 (( MF__, "Tried to reschedule inactive task" ));              \
          ABORT ();                                                          \
          }

#ifdef DEB_RUNQ
#define PRINT_ENQU(_Task, _FromTask, _Req)                                   \
{         if ( _FromTask )                                                   \
            MSG7 (( 11111,"disp: T%d added from T%d(%s) to r_que, len %d\n", \
                 _Task->index,                                               \
                  this_kp->RunQueue[this_kp->RunQueueLen].pFromTask->index , \
                  e38_req_name ( _Req->req_type ),                           \
                  this_kp->RunQueueLen+1 ))                                  \
          else                                                               \
            MSG7 (( 11111,"disp: T%d added from T? to r_que, len %d\n",      \
                 _Task->index, this_kp->RunQueueLen+1 ))                     \
}
#else
#define PRINT_ENQU
#endif

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
static  VOID os74kTerminationCleanup    ( PUKT_CTRL_REC          pUKT );
_INLINE VOID __os74SearchUKTtoUKT       ( PUKT_CTRL_REC          pUKT );
_INLINE VOID __os74SearchRUN            ( PUKT_CTRL_REC          pUKT,
                                          PTASK_CTRL_REC         pLastRunningTaskCtrl,
                                          PPTASK_CTRL_REC        ppCTask );
_INLINE VOID __os74EnquRUNQueue         ( PUKT_CTRL_REC          pUKT,
                                          PTASK_CTRL_REC         pTask,
                                          PTASK_CTRL_REC         pFromTask,
                                          PDLQ_REC               pReq,
                                          ULONG                  ulBasePrio,
                                          SAPDB_Bool             taskYield ) ;
_INLINE SAPDB_Bool __os74TaskMoveCheck  ( PUKT_CTRL_REC          *ppSrcUKT, 
                                          PUKT_CTRL_REC          *ppDstUKT );

VOID __os74SendMoveMeassIntRequest      ();

VOID __os74SendMoveRequest              ( PUKT_CTRL_REC          pSrcUKT,
                                          PUKT_CTRL_REC          pDstUKT,
                                          SAPDB_UInt8            StopWaitForBestFitTaskAt );

_INLINE void __os74UKTStatisticReset        ( PUKT_CTRL_REC      pUKT,
                                              PTASK_CTRL_REC     pCurrTaskCtrl );

_INLINE void __os74TaskStatisticReset       ( PTASK_CTRL_REC     pCurrTaskCtrl );

_INLINE void __os74DoEnterDispatcherTiming  ( PUKT_CTRL_REC      pUKT,
                                              PTASK_CTRL_REC     pCurrTaskCtrl );
_INLINE void __os74DoLeaveDispatcherTiming  ( PTASK_CTRL_REC     pCurrTaskCtrl );
_INLINE void __os74WaitForWakeup            ( PUKT_CTRL_REC      pUKT, 
                                              SAPDB_UInt4        ulTimeOut);
_INLINE VOID __os74MoveTask                 ( PTASK_CTRL_REC     pCTask,
                                              PDLQ_REC           pRequest );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */
void  sql74k_dispatcher ( PUKT_CTRL_REC   pUKT )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_dispatcher"

  pUKT->pTaskSchedulerObject->NextScheduledTask();
  }

/*---------------------------------------------------------------------------*/

RTETask_Task&  RTETask_TaskScheduler::NextScheduledTask()
{
  tsp00_Longuint                ulTimeOut           = 0;
  tsp00_Longuint                ulCurrTime;
  PUKT_CTRL_REC                 pUKT                 = m_LegacyUKTCtrl;
  PPTASK_CTRL_REC               ppCTask              = &pUKT->pCTask;
  PTASK_CTRL_REC                pLastRunningTaskCtrl = pUKT->pCTask;
  RTETask_TaskScheduler         &taskScheduler       = *pUKT->pTaskSchedulerObject;
  RTE_KGS*                      pKGS                 = &RTE_KGS::Instance();     // Shortcut to kgs


  // --- don't insert new variables behind 'ulLoopCnt'.
  //     'ulLoopCnt' is used for stack checks
  ULONG                         ulLoopCnt = LOOP_CNT();

  m_IsWaitingForNextScheduledTask = true;  // signal that we are waiting for the next task

  // - Signal that it's necessary to post the UKT-semaphore!
  //   This flag is used by the client applications only.
  kgs.pulSemaFlags[pUKT->ulSemaNo] = POST_SEM;


  // --- dispatcher called with information about current task?
  if (*ppCTask)
    {
    PTASK_CTRL_REC pCurrTaskCtrl = *ppCTask;

    DBGIN_T ((*ppCTask)->ulTaskIndex);

    (*ppCTask)->ulDispatcherCalls++;

    __os74UKTStatisticReset(pUKT, (*ppCTask));

    if ( 0 == (*ppCTask)->delayTimeMeasurement ) // no time measurement if task is yielding
        __os74DoEnterDispatcherTiming(pUKT, (*ppCTask));

    // --- get stack pointer to set the console storage info
    pCurrTaskCtrl->pCurrStackPointer = (PCHAR) &ulLoopCnt;

    // PTS 1110953
    if ( pCurrTaskCtrl->maxStackPointer > (SAPDB_UInt4 *)&ulLoopCnt )
    {
        pCurrTaskCtrl->maxStackPointer = (SAPDB_UInt4 *)&ulLoopCnt;
    }

    *ppCTask = NULL;
    }
  else
    DBGIN;

  //
  // --- set dispatcher counter
  //

#ifdef SRCH_HAENGER
  for (;;)
#endif
  {
  pUKT->DispatcherCalls++;

#ifndef SRCH_HAENGER
  for (;;)
#endif
  {
    if ( *kgs.pDBState >= SERVER_SHUTDOWN )
      os74kTerminationCleanup ( pUKT );

    //
    // +------------------------------------------------------------+
    // ³  Calculate queue search strategy                           ³
    // +------------------------------------------------------------+
    //

    //
    // +------------------------------------------------------------+
    // ³  Search queues                                             ³
    // +------------------------------------------------------------+
    //
    if ( kgs.ulNumOfUKTs > 0 )
      __os74SearchUKTtoUKT ( pUKT );

    SearchExternalRequestsQueue();

    ulTimeOut = MAXTIMEVALUE;

    if (( BASE_PRIO_COM > 0 || pUKT->ulRUNQueueLen <= 1 ) &&  
        ( false == kgs.fServicePaused ))
    {
        ulCurrTime = kgs.ulCurrTime;
        ulTimeOut  = (SAPDB_UInt4)SearchForCommunicationRequests( ulCurrTime );
    }

    __os74SearchRUN ( pUKT, pLastRunningTaskCtrl, ppCTask );

    if ( *ppCTask )
      {
      CHECK_RESCHEDULE_INACT_TASK( *ppCTask );

      #if !defined ( DO_NOT_POST_FLAG )
        // - Posting the UKT-semaphore is not necessary!
        //   This flag is used by the client applications only.
        kgs.pulSemaFlags[pUKT->ulSemaNo] = DO_NOT_POST_SEM;
      #endif


      if ( pLastRunningTaskCtrl != *ppCTask && pLastRunningTaskCtrl && pLastRunningTaskCtrl->delayTimeMeasurement )
        {
        /* we will spend some unexpected time in the run queue as runnable... */
        /* to allow dispatcher to move us later we reenable timing collection temporary */
        /* We also have to repeat the dispatcherEnterTime measurement which we skipped before... */
        __os74DoEnterDispatcherTiming(pUKT, pLastRunningTaskCtrl );
        pLastRunningTaskCtrl->delayTimeMeasurement = false;
        }

      __os74TaskStatisticReset(*ppCTask);

      /* if we rescheduled ourself(task yield), time collection will be suppressed */
      if  ( 0 == (*ppCTask)->delayTimeMeasurement ) 
        __os74DoLeaveDispatcherTiming(*ppCTask);

      (*ppCTask)->delayTimeMeasurement = false;

      m_IsWaitingForNextScheduledTask = false;  

      m_pCurrentTask = (*ppCTask)->pTaskObject;

      DBGOUT_T ((*ppCTask)->ulTaskIndex);
      return *m_pCurrentTask;
      }

    if ( *kgs.pDBState >= SERVER_SHUTDOWN )
      os74kTerminationCleanup ( pUKT );

    //
    // +------------------------------------------------------------+
    // ³  SLEEP on UKT's Semaphore                                  ³
    // +------------------------------------------------------------+
    //
    if ( ulTimeOut != MAXTIMEVALUE)
      ulTimeOut = (ulTimeOut - ulCurrTime) * 1000;

    if (--ulLoopCnt == 0 )
      {
      ulLoopCnt = XPARAM(ulDispLoopsBefSemop);

      // --- signal that there are no activities
      pUKT->ThrdCtrlHeader.ThrdState = KT_SLEEPING;

      __os74WaitForWakeup(pUKT, (SAPDB_UInt4)ulTimeOut);

      pUKT->ThrdCtrlHeader.ThrdState = KT_RUNNING;
      }
    } 
  } /* End of for loop (doubled because SRCH_HAENGER macros) */

  #if !defined ( DO_NOT_POST_FLAG )
    // - Posting the UKT-semaphore is not necessary!
    //   This flag is used by the client applications only.
    kgs.pulSemaFlags[pUKT->ulSemaNo] = DO_NOT_POST_SEM;
  #endif

    __os74DoLeaveDispatcherTiming(*ppCTask);

    m_IsWaitingForNextScheduledTask = false;  

    DBGOUT_T ((*ppCTask)->ulTaskIndex);
    return *m_pCurrentTask;
  }

/*------------------------------*/

VOID  sql74k_RUN_enqu ( PUKT_CTRL_REC        pUKT,
                        PTASK_CTRL_REC       pReqTaskCtrl,
                        PTASK_CTRL_REC       pFromTaskCtrl,
                        PDLQ_REC             pRequest,
                        ULONG                ulBasePrio)
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_RUN_enqu"
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

  DBG4 (( MF__, "[T:0x%03u] Send pRequest 0x%x, (type %u) to [T:0x%03u]",
          pTaskCtrl->ulTaskIndex,
          pRequest,
          pRequest->ulReqType,
          pReqTaskCtrl->ulTaskIndex ));

  __os74EnquRUNQueue( pUKT, pReqTaskCtrl, pFromTaskCtrl, pRequest, ulBasePrio, false ) ;

  return;
  }

/*------------------------------*/

VOID  sql74k_YieldTaskRunEnqu ( PUKT_CTRL_REC           pUKT,
                                PTASK_CTRL_REC          pTask,
                                PDLQ_REC                pRequest, 
                                tsp00_Bool              ulBasePrio )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_YieldTaskRunEnqu"

  __os74EnquRUNQueue( pUKT, pTask, pTask, pRequest, ulBasePrio, true );
  }

/*------------------------------*/

tsp00_Longuint sql74k_load_balancing()
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_load_balancing"
  static SAPDB_Bool    MeasInterval = false;
  tsp00_Longuint       SleepTime = XPARAM(uLoadBalancingCheck);

  if ( *kgs.pDBState == SERVER_WARM )
    {
    if ( kgs.TaskMoveMeasIntervals < 2 )
      { // we don't have enough statistic data!
      __os74SendMoveMeassIntRequest ();  
      SleepTime = XPARAM(uLoadBalancingCheck);
      }
    else if ( MeasInterval == true ) 
      {
      __os74SendMoveMeassIntRequest();

      if ( kgs.TaskMoved ) 
        { // A task has been moved during last measurement interval. Interval
          // statistics will be ignored. Don't make a move check next time.
        kgs.TaskMoved = false;
        SleepTime     = XPARAM(uLoadBalancingCheck);
        }
      else
        {
        MeasInterval  = false; // Next time we do a move check.
        SleepTime     = XPARAM(uLoadBalancingCheck) / 2;
        }
      }
    else
      {
      PUKT_CTRL_REC pSrcUKT, pDstUKT;

      if (__os74TaskMoveCheck( &pSrcUKT, &pDstUKT ))
      {
        __os74SendMoveRequest ( pSrcUKT, pDstUKT, XPARAM(uLoadBalancingCheck) / 2 + kgs.ulCurrTime );

        SleepTime = XPARAM(uLoadBalancingCheck);
      }
      else
        SleepTime = XPARAM(uLoadBalancingCheck) - (XPARAM(uLoadBalancingCheck) / 2);

      MeasInterval = true;   // Next time we start a new measurement interval.
      }
    }

  return SleepTime;
  }

/*------------------------------*/

VOID sql74k_remote_func_error ( PPTASK_CTRL_REC ppCTask,
                                PTASK_CTRL_REC  pFromTask,
                                PPDLQ_REC       ppCurrReq,
                                void*           pFuncParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_remote_func_error"

  MSGD(( WRN_REMOTEFUNC_ERROR_CALLED, (*ppCTask)->ulTaskIndex, (ULONG)pFuncParam)) ;    

  *ppCurrReq              = GetTaskDLQElement1( *ppCTask );
  (*ppCurrReq)->ulReqType = 99;

  }

/*------------------------------*/

VOID  sql74k_alter_task_cnt( PTASK_CTRL_REC   pCTask,
                             SAPDB_UInt2      AlterType )
{
  #undef  MF__
  #define MF__ MOD__"sql74k_alter_task_cnt"

  PUKT_CTRL_REC    pUKT = pCTask->pUKT;
  SAPDB_Int2       AlterVal = COUNTER_SUB & AlterType ? -1 : 1;

  if ( AlterType & TOTAL_COUNTER )
  {
    // alter total counter 
    switch ( pCTask->TaskType )
    {
    case TT_EV_EO00:
      pUKT->ulNumEventTasks       += AlterVal;
      break;
    case TT_UT_EO00:
      pUKT->ulNumUtilityTasks     += AlterVal;
      break;
    case TT_US_EO00:
      pUKT->ulNumUserTasks        += AlterVal;
      break;
    case TT_SV_EO00:
      pUKT->ulNumServTasks    += AlterVal;
      break;
    }
  }

  if ( AlterType & ACTIVE_COUNTER )
  {
    if ( pCTask->MoveLock.Val == 0 )
      pUKT->NumOfMovableTasks += AlterVal;

    pUKT->ulActiveTasks += AlterVal;

    switch ( pCTask->TaskType )
    {
    case TT_EV_EO00:
      pUKT->ulActiveEventTasks    += AlterVal;
      break;
    case TT_UT_EO00:
      pUKT->ulActiveUtilityTasks  += AlterVal;
      break;
    case TT_US_EO00:
      pUKT->ulActiveUserTasks     += AlterVal;
      kgs.ulActiveUserTasks       += AlterVal;
      break;
    case TT_SV_EO00:
      pUKT->ulActiveServTasks     += AlterVal;
      break;
    }
  }
}

/*------------------------------*/

VOID sql74k_temp_move_lock( PTASK_CTRL_REC pTaskCtrl,
                            SAPDB_UInt2    Cnt,
                            SAPDB_Bool     SetLock )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_temp_move_lock"

  if (SetLock)
    {
    if ( pTaskCtrl->MoveLock.Val == 0 )
      pTaskCtrl->pUKT->NumOfMovableTasks--;

    pTaskCtrl->MoveLock.Type.Temporary += Cnt;
    }
  else if ( pTaskCtrl->MoveLock.Type.Temporary != 0 )
    {
#   if defined (CHECK_MOVELOCK_UNDERRUN)
     if ( pTaskCtrl->MoveLock.Type.Temporary < Cnt )
         ABORT ();
#   endif

    pTaskCtrl->MoveLock.Type.Temporary -= Cnt;

    if ( pTaskCtrl->MoveLock.Val == 0 )
      pTaskCtrl->pUKT->NumOfMovableTasks++;
    }
  }

  /*------------------------------*/

VOID sql74k_long_term_move_lock( PTASK_CTRL_REC pTaskCtrl,
                                 SAPDB_UInt2    LockType,
                                 SAPDB_Bool     SetLock )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_long_term_move_lock"

  if (SetLock)
    {
    if ( pTaskCtrl->MoveLock.Val == 0 )
      pTaskCtrl->pUKT->NumOfMovableTasks--;

    pTaskCtrl->MoveLock.Type.LongTerm |= LockType;
    }
  else if (( pTaskCtrl->MoveLock.Type.LongTerm & LockType ) != 0 )
    {
    pTaskCtrl->MoveLock.Type.LongTerm &= ~LockType;

    if ( pTaskCtrl->MoveLock.Val == 0 )
      pTaskCtrl->pUKT->NumOfMovableTasks++;
    }
  }

/*------------------------------*/

// Since load balancing is implemented only this function should be used to 
// enqueue requests to the 'UToU-queue'.
VOID sql74k_UToU_enqu_and_wake ( PUKT_CTRL_REC   pFromUKT,
                                 PTASK_CTRL_REC  pFromTaskCtrl,
                                 PUKT_CTRL_REC   pRemoteUKTCtrl,
                                 PTASK_CTRL_REC  pRemoteTaskCtrl,
                                 PFNREMOTEFUNC   pFuncToExecute,
                                 void*           pFuncParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_UToU_enqu_and_wake"

  /* If the task is move to another UKT during enqueue operation, 
     the 'SearchExternalRequestsQueue' methode is forwarding this request to
     the new UKT.
   */
  sql73k_UToU_enqu ( pFromUKT, pFromTaskCtrl, pRemoteUKTCtrl, pRemoteTaskCtrl, 
                     pFuncToExecute, pFuncParam );
  sql72k_wake_ukt ( pRemoteUKTCtrl );
  }

/*------------------------------*/

// Since load balancing is implemented only this function should be used to 
// enqueue requests to the 'UKT-queue'.
VOID sql74k_UKT_enqu_and_wake ( PUKT_CTRL_REC   pRemoteUKTCtrl,
                                PTASK_CTRL_REC  pRemoteTaskCtrl,
                                PDLQ_REC        pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql74k_UKT_enqu_and_wake"

  /* If the task is move to another UKT during enqueue operation, 
     the 'SearchExternalRequestsQueue' function is forwarding this request to
     the new UKT.
   */
  sql73k_UKT_enqu ( pRemoteUKTCtrl, pRemoteTaskCtrl, pRequest );
  sql72k_wake_ukt ( pRemoteUKTCtrl );
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

_INLINE VOID __os74EnquRUNQueue ( PUKT_CTRL_REC    pUKT,
                                  PTASK_CTRL_REC   pTask,
                                  PTASK_CTRL_REC   pFromTask,
                                  PDLQ_REC         pReq,
                                  ULONG            ulBasePrio,
                                  SAPDB_Bool       taskYield ) 
  {
  #undef  MF__
  #define MF__ MOD__"__os74EnquRUNQueue"
  DBGIN;

  if ( pUKT != pTask->pUKT )
    { DBG3 (( MF__, "T%d not in this UKT ", pTask->ulTaskIndex)); 
      ABORT(); 
    } 

  if ( false == taskYield )
     pTask->TaskStateStat.runqueueEnqueueTime = RTESys_MicroSecTimer();
  else
      pTask->TaskStateStat.runqueueEnqueueTime = 0;

  pTask->delayTimeMeasurement                     = taskYield;  
  pTask->QueLocation                              = TASK_IS_IN_RUN_QUEUE_EO00 ; 
  pTask->ulDynPrio                                = ulBasePrio + pTask->PrioFlag * PRIO_FACTOR ; 
  pUKT->RUNQue[pUKT->ulRUNQueueLen].pTaskCtrl     = pTask; 
  pUKT->RUNQue[pUKT->ulRUNQueueLen].pFromTaskCtrl = pFromTask; 
  pUKT->RUNQue[pUKT->ulRUNQueueLen].pRequest      = pReq;
  PRINT_ENQU(pTask, pFromTask, pReq) ;
  pUKT->ulRUNQueueLen++ ;
  if (pUKT->ulRUNQueueLen > pUKT->maxRunQueueLen)
  {
    pUKT->maxRunQueueLen = pUKT->ulRUNQueueLen;
  }

  if (pUKT->ulRUNQueueLen > pUKT->maxRunQueueLenSinceLastReset)
  {
    pUKT->maxRunQueueLenSinceLastReset = pUKT->ulRUNQueueLen;
  }


  DBG3 (( MF__, "T%d (REQ: %d) added from T%d, len %d",pTask->ulTaskIndex,
                 pFromTask ? pFromTask->ulTaskIndex : 00, 
                 pReq      ?  pReq->ulReqType       : 00,
                 pUKT->ulRUNQueueLen)); 

  DBGOUT;
  return;
  }

/*------------------------------*/

_INLINE VOID __os74SearchRUN ( PUKT_CTRL_REC    pUKT,
                               PTASK_CTRL_REC   pLastRunningTaskCtrl,
                               PPTASK_CTRL_REC  ppCTask )
  {
  #undef  MF__
  #define MF__ MOD__"__os74SearchRUN"
  ULONG                         uldx, ulRunQueueLen;
  PUKT_RUN_QUEUE_REC            pRUNQue;
  APIRET                        rc = NO_ERROR;

  DBGIN;

  pRUNQue = pUKT->RUNQue;

  if ( (ulRunQueueLen = pUKT->ulRUNQueueLen) > 0 )
  {
      int             RunIndex        = 0;
      PTASK_CTRL_REC  HighestPrioTask;
      PTASK_CTRL_REC  pTempTaskCtrl, pMoveTask = NULL;

      if (( pUKT->pMoveTaskToUKT != NULL ) && ( pUKT->NumOfMovableTasks != 0 ))
      {
          SAPDB_UInt8  MaxRunnableMicroSeconds = 0;
          SAPDB_UInt8  TaskMaxRunnableMicroSeconds;
          SAPDB_UInt2  PrevTimeVar             = PREV_TIME_VAR(pUKT->CurrTimeVar);

          for ( uldx = 0; uldx < ulRunQueueLen; uldx++ )
          {
              pTempTaskCtrl               = pRUNQue[uldx].pTaskCtrl;
              TaskMaxRunnableMicroSeconds = pTempTaskCtrl->MaxRunnableMicroSeconds[PrevTimeVar];  

              if (( pTempTaskCtrl->MoveLock.Val == 0 ) && ( pTempTaskCtrl != pLastRunningTaskCtrl ))
              {
                  if ( pUKT->StopWaitForBestFitTaskAt > kgs.ulCurrTime )
                  { // we are searching for the UKTs task with maximum runnable time 
                      if ( TaskMaxRunnableMicroSeconds == pUKT->MaxRunnableMicroSeconds[PrevTimeVar] )
                      {
                          pMoveTask = pTempTaskCtrl;
                          break;
                      }
                  }
                  else if (( MaxRunnableMicroSeconds                               < TaskMaxRunnableMicroSeconds ) &&
                      ( pUKT->pMoveTaskToUKT->MaxRunnableMicroSeconds[PrevTimeVar] < TaskMaxRunnableMicroSeconds ))
                  {
                      pMoveTask                = pTempTaskCtrl;
                      MaxRunnableMicroSeconds  = TaskMaxRunnableMicroSeconds;
                  }
              }
          }
      }

      for ( uldx = 0, pUKT->ulImmediateDispatch = 0, HighestPrioTask = pRUNQue[0].pTaskCtrl; 
            uldx < ulRunQueueLen; 
            uldx++ )
      { 
          PDLQ_REC        pTempRequest;

          pTempTaskCtrl = pRUNQue[uldx].pTaskCtrl;
          pTempRequest  = pRUNQue[uldx].pRequest;

          if ( pTempTaskCtrl == pMoveTask )
          {
              pTempTaskCtrl->QueLocation    = TASK_IS_IN_NON_QUEUE;
              pTempTaskCtrl->ulDynPrio      = 0;

              __os74MoveTask( pTempTaskCtrl, pTempRequest );

              if ( (pUKT->ulRUNQueueLen = --ulRunQueueLen) )
              {
                  if ( ulRunQueueLen > uldx )
                  {
                      if ( uldx == 0 )
                          HighestPrioTask = pRUNQue[ulRunQueueLen].pTaskCtrl;

                      pRUNQue[uldx--] = pRUNQue [ulRunQueueLen];
                  }
                  else
                  {
                      pTempTaskCtrl = pRUNQue[--uldx].pTaskCtrl;
                      pTempRequest  = pRUNQue[uldx].pRequest;
                  }
                  continue;
              }
              else
              {
                  *ppCTask = NULL;
                  DBGOUT;
                  return;
              }
          }

          pTempTaskCtrl->ulDynPrio++;

          if ( pTempTaskCtrl->PrioFlag ) 
              pUKT->ulImmediateDispatch++;

          if ( REQ_DEBUG_TASK_EO00 == pRUNQue[uldx].pRequest->ulReqType ) 
          {
              HighestPrioTask = pTempTaskCtrl;
              RunIndex        = uldx;
              break;
          }
          if ( REQ_YIELDING_EO00 == pRUNQue[uldx].pRequest->ulReqType ) // Yield request?
              pTempTaskCtrl->yieldRequestAtPos = uldx;

          if ( HighestPrioTask->ulDynPrio < pTempTaskCtrl->ulDynPrio ) 
          { 
              HighestPrioTask = pTempTaskCtrl;
              RunIndex        = uldx;
          }
      }

      if ( (SAPDB_UInt4)-1 != HighestPrioTask->yieldRequestAtPos )
      {   
          // Swap run queue index because yield has to be served first
          RunIndex = HighestPrioTask->yieldRequestAtPos;
          HighestPrioTask->yieldRequestAtPos = (SAPDB_UInt4)-1;
      }

      HighestPrioTask->QueLocation = TASK_IS_IN_NON_QUEUE;
      HighestPrioTask->ulDynPrio   = 0;

      if ( HighestPrioTask->PrioFlag ) pUKT->ulImmediateDispatch--;

      *ppCTask       = HighestPrioTask ;
      pUKT->pCurrReq = pRUNQue [ RunIndex ].pRequest;

      if ( (pUKT->ulRUNQueueLen = --ulRunQueueLen) )
          pRUNQue [ RunIndex ] = pRUNQue [ ulRunQueueLen ];
  }
  else
      *ppCTask = NULL;

  DBGOUT;
  return;
  }

/*------------------------------*/

_INLINE VOID __os74SearchUKTtoUKT ( PUKT_CTRL_REC    pUKT )
  {
  #undef  MF__
  #define MF__ MOD__"__os74SearchUKTtoUKT"
  PUKT_TO_UKT_QUEUE_REC         pUToUQueElem;
  PUKT_TO_UKT_QUEUE_HEAD_REC    pUToUQueHead;
  LONG                          i;
  bool                          fTaskFound;
  PFNREMOTEFUNC                 pSvFuncToExecute;
  PTASK_CTRL_REC                pCTask;

  DBGIN;

  // -- they are only of interest when more than one UKT is active
  for ( i = kgs.ulNumOfUKTs - 1; i >= 0; i-- )
    {
    do
      {
      fTaskFound = FALSE ;

      if ( i == (LONG)pUKT->ulUKTIndex - 1 )
        continue;

      // --- 'pUToUQueHead' is the pointer to the indexed queue head.
      pUToUQueHead = &pUKT->pUToUQueHeads[ i ];

      // --- 'pUToUQueElem' is a pointer to the first queue element.
      while ( (pCTask = pUToUQueHead->pWork->pTaskCtrl) != NULL )
        {
        bool   fLoopEnd ;
        ULONG  ulLoopCnt = 0 ;

        RTESys_ReadMemoryBarrier();

        pUKT->pCTask   = pCTask;
        pUToUQueElem   = pUToUQueHead->pWork;

        do
          {
          if (( ((pSvFuncToExecute = pUToUQueElem->pFuncToExecute) == sql74k_remote_func_error ) ||
                 pUToUQueElem->pFromTaskCtrl == NULL ))
            { 
            MSGD(( ERR_REMOTEFUNC_NOT_SET, pCTask->ulTaskIndex, pUToUQueElem->pFuncParam )) ;    
            fLoopEnd = ++ulLoopCnt == 10 ;
            SLEEP(3000) ;
            }
          else
            fLoopEnd = TRUE ;
          }
        while ( !fLoopEnd ) ;

        /*
         *  Execute the request
         */
        IGNORE_SOME_REQUESTS_AT_SYSTEM_END( pCTask, return );

        if ( pUKT != pCTask->pUKT )
        {
          // The load balancing has moved the task to another UKT during
          // 'UKTtoUKT' enqueue operation. Now we forward this request..
          sql74k_UToU_enqu_and_wake ( pUKT, pUToUQueElem->pFromTaskCtrl,
                                      pCTask->pUKT, pCTask,
                                      pSvFuncToExecute, pUToUQueElem->pFuncParam );
          pCTask = NULL;
        } 
        else
        {
          pSvFuncToExecute( &pCTask, pUToUQueElem->pFromTaskCtrl, 
                            &pUKT->pCurrReq, pUToUQueElem->pFuncParam ); 
        }

        // ---  Dequeue the request
        pUToUQueElem->pFuncToExecute = sql74k_remote_func_error ;
        pUToUQueElem->pTaskCtrl      = NULL;
        pUToUQueHead->pWork          = pUToUQueElem->pNextElem;

        // ---  On behalf of the request a task could become runnable
        if ( pCTask != NULL )
          {
          fTaskFound   = TRUE;

		  if ( pUKT->pCurrReq )
		    pUKT->pCurrReq->pFromTaskCtrl = pUToUQueElem->pFromTaskCtrl ;

          __os74EnquRUNQueue( pUKT, pCTask, pUToUQueElem->pFromTaskCtrl,
                              pUKT->pCurrReq, BASE_PRIO_U2U, false ) ;
          }
        }
      }
    while ( fTaskFound );
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

void RTETask_TaskScheduler::SearchExternalRequestsQueue()
  {
  PUKT_CTRL_REC                                    pUKT;
  UKT_QUEUE_HEAD_REC                               *pUKTQue;
  PDLQ_REC                                         pCurrReq;
  RTETask_ReadyToRunQueueTS::InternalBasePriority  basePrio;
  RTETask_CommonQueueElement*                      pQueueElement;
  PTASK_CTRL_REC                                   pCTask;
  RTETask_Task*                                    pTask;

  pUKT    = m_LegacyUKTCtrl;
  pUKTQue = &pUKT->UKTQue;

  if ( pUKTQue->pLastUsedElem != NULL )
    {
    while ( (pUKT->pCurrReq = pUKTQue->pLastUsedElem->pRequest) != NULL )
      {
      RTESys_ReadMemoryBarrier();

      // --- dequeue the request
      pUKTQue->pLastUsedElem->pRequest = NULL;
      pUKTQue->pLastUsedElem           = pUKTQue->pLastUsedElem->pNextElem;
      pCurrReq                         = pUKT->pCurrReq;
      pQueueElement                    = pCurrReq->pQueueElementObject;
      pCTask                           = pCurrReq->pTaskCtrl;


      if ( pCTask ) 
        { 
        // since REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00 and REQ_MOVE_TASK_EO00 no task might be specified
        IGNORE_SOME_REQUESTS_AT_SYSTEM_END( pCurrReq->pTaskCtrl, return );

        pCTask->QueLocation = TASK_IS_IN_NON_QUEUE;
        pUKT->pCTask        = pCTask;
        pTask               = pCTask->pTaskObject;

        if ( pUKT != pCTask->pUKT )
          {
          // The load balancing has moved the task to another UKT during
          // 'UKT' enqueue operation. Now we forward this request..
          sql74k_UKT_enqu_and_wake ( pCTask->pUKT, pCTask, pCurrReq );

          pQueueElement = 0; // Don't enque this request into the ready to run queue
          continue;
          } 
        }

      basePrio = RTETask_ReadyToRunQueueTS::ExternalRequestsPrio;

      switch ( pCurrReq->ulReqType )
        {
        case REQ_CANCEL_TASK_EO00:
          if ( false == pTask->PrepareEnqueueOfCancelRequest() )
          {
            RELEASE_FREELIST_ELEM ( pCurrReq );  // drop cancel request
            pQueueElement = 0;                   // Don't enque this request into the ready to run queue
          }
          break;

        case REQ_REQUEST_TIMED_OUT_EO00:
          if ( false == pTask->PrepareEnqueueOfTimeoutRequest() )
          {
            RELEASE_FREELIST_ELEM ( pCurrReq );  // drop cancel request
            pQueueElement = 0;                   // Don't enque this request into the ready to run queue
          }
          break;

        case REQ_INITOK:
          // --- task is not a ukts task list
          if (( pCTask->pNextInTaskList      == NULL ) && 
              ( pCTask->pPrevInTaskList      == NULL ) &&
              ( pUKT->TaskList.pFirstInTaskList != pCTask ))
          { 
            // integrate task into UKTs task chain!
            sql73k_tlist_enque_task_nolock ( &pUKT->TaskList, pCTask ); 
          }
          // - don't release the freelist element here!
          break;

        case REQ_REMOVE_TASK_FROM_UKT:
          sql73k_tlist_dequ_task_nolock ( &pUKT->TaskList, pCTask );
          break;

        case REQ_CONNECT:             
        case REQ_CONNECT_REQUEST_EO00:
            
          if ( pTask->State() != RTETask_Task::TaskWaitingForConnect )
          {
              // - the request will be evaluated by 'AcceptClientConnect'.
              pTask->ClientConnections().EnqueueIntoConnectionRequestQueue(*pQueueElement);

              pQueueElement = 0; // Don't enque this request into the ready to run queue
          }
          break;

        // --- (shutdown) resume request for tracewriter?
        case REQ_SHUTDOWN_RESUME_TW:
        case REQ_RESUME_TW_EO00:
          RELEASE_FREELIST_ELEM ( pCurrReq );

          // --- tracewriter suspended and not already triggered? */
          if ( kgs.pTW->TaskState == TSK_VSUSPEND_EO00 && Trace_Flush (kgs.pTW->ulTaskIndex))
            {
            // --- resume tracewriter by 'REQ_VRESUME' and 'SpecDLQElem1'
            pUKT->pCurrReq              = GetTaskDLQElement1( kgs.pTW );
            pUKT->pCurrReq->ulReqType   = REQ_VRESUME;
            pUKT->pCurrReq->pTaskCtrl   = kgs.pTW;
            kgs.pTW->lResumeCount       = 0;
            ++kgs.pTW->totalResumerCount;

            pQueueElement = pUKT->pCurrReq->pQueueElementObject;
            }
          else
            pQueueElement = 0; // Don't enque this request into the ready to run queue

          break;

        case REQ_FINISH_SV:
          RELEASE_FREELIST_ELEM ( pCurrReq );

          sql51k_finish ();

          MSGD (( ERR_TASK_RETURNED ));   // --- we should never return here
          ABORT();
          break;

//        case REQ_VSTOP:    // --- called by vcontinue to wake an EventTask
//          break;

        case REQ_DEBUG_TASK_EO00:
#         if defined ( ALLOW_TASK_DEBUG_REQUESTS )
           if ( false == pCTask->pTaskObject->TaskIsInactive() && false == pCTask->fDebugTask )
           {
             pCTask->fDebugTask = TRUE;
             basePrio = RTETask_ReadyToRunQueueTS::DebugTaskPrio;  // - maximum priority
           }
           else
           {
            RELEASE_FREELIST_ELEM ( pCurrReq );
            pQueueElement = 0; // Don't enque this request into the ready to run queue
           }
#         else
            RELEASE_FREELIST_ELEM ( pCurrReq );
            pQueueElement = 0; // Don't enque this request into the ready to run queue
#         endif

          break;

        case REQ_INSERT_MOVING_TASK_E000:
          MSGD(( INFO_TASK_MOVE_REQUEST_REC, pCTask->ulTaskIndex ));

          // integrate task into UKTs task chain!
          sql73k_tlist_enque_task_nolock ( &pUKT->TaskList, pCTask );
          sql74k_alter_task_cnt( pCTask, ADD_ALL_COUNTER );

          if ( pCTask->PrioFlag ) pUKT->ulImmediateDispatch++;

          pCurrReq->ulReqType    = pCurrReq->ulOrgReqType;
          pCurrReq->ulOrgReqType = 0;
          break;

        case REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00:
          // --- skip old requests
          if ( pCurrReq->ReqArgs.NewTaskMoveMeasRequest.TaskMoveIntervalStartTime ==
               kgs.TaskMoveIntervalStartTimes[kgs.CurrTimeVar] )
          {
            tos00k_DlqNewTaskMoveMeasReqArgs *pReqArg;
            ULONG                             uldx, ulRunQueueLen = pUKT->ulRUNQueueLen;
            SAPDB_UInt8                       timeNow = RTESys_MicroSecTimer();
            SAPDB_UInt8                       RunnableMicroSeconds;

            for ( uldx = 0; uldx < ulRunQueueLen; uldx++ )
            {
                PTASK_CTRL_REC  pTempTaskCtrl = pUKT->RUNQue[uldx].pTaskCtrl;

                // long term move lock set, or task was not in run queue?
                if (( pTempTaskCtrl->MoveLock.Type.LongTerm != 0 ) || 
                    ( pTempTaskCtrl->TaskStateStat.runqueueEnqueueTime == 0 ))
                    continue;

                RunnableMicroSeconds = timeNow - pTempTaskCtrl->TaskStateStat.runqueueEnqueueTime;

                // accumulate the tasks time waititng in the run queue to be scheduled
                pTempTaskCtrl->RunnableMicroSeconds[pUKT->CurrTimeVar] += RunnableMicroSeconds;

                // Has the tasks a new runnable time maximum?
                if ( pTempTaskCtrl->MaxRunnableMicroSeconds[pUKT->CurrTimeVar] < RunnableMicroSeconds )
                    pTempTaskCtrl->MaxRunnableMicroSeconds[pUKT->CurrTimeVar] = RunnableMicroSeconds;

                // set the UKTs runnable time values
                pUKT->RunnableMicroSeconds[pUKT->CurrTimeVar] += RunnableMicroSeconds;

                if ( pUKT->MaxRunnableMicroSeconds[pUKT->CurrTimeVar] < RunnableMicroSeconds )
                    pUKT->MaxRunnableMicroSeconds[pUKT->CurrTimeVar] = RunnableMicroSeconds;
            }

            pReqArg = &pCurrReq->ReqArgs.NewTaskMoveMeasRequest;

            pUKT->CurrTimeVar                                 = pReqArg->TimeVar;
            pUKT->TaskMoveIntervalStartTime                   = pReqArg->TaskMoveIntervalStartTime;
            pUKT->UKTSleepTimeMicroSeconds[pUKT->CurrTimeVar] = 0;
            pUKT->RunnableMicroSeconds[pUKT->CurrTimeVar]     = 0;
            pUKT->MaxRunnableMicroSeconds[pUKT->CurrTimeVar]  = 0;
            pUKT->pMoveTaskToUKT                              = NULL; // drop old move task requests
          }

          RELEASE_FREELIST_ELEM ( pCurrReq );
          pQueueElement = 0; // Don't enque this request into the ready to run queue
          break;

        case REQ_MOVE_TASK_EO00:
          // --- skip old requests
          if ( pCurrReq->ReqArgs.MoveTaskRequest.TaskMoveIntervalStartTime ==
               kgs.TaskMoveIntervalStartTimes[kgs.CurrTimeVar] )
          {
            tos00k_DlqMoveTaskReqArgs *pReqArg = &pCurrReq->ReqArgs.MoveTaskRequest;

            pUKT->StopWaitForBestFitTaskAt   = pReqArg->StopWaitForBestFitTaskAt;
            pUKT->pMoveTaskToUKT             = pReqArg->pMoveTaskToUKT;
          }

          RELEASE_FREELIST_ELEM ( pCurrReq );
          pQueueElement = 0; // Don't enque this request into the ready to run queue
          break;

        default:
          break;
        }

      if ( pQueueElement )
         m_ReadyToRunQueue.Enqueue( *pQueueElement, basePrio );
      }
    }

  return;
  }

/*------------------------------*/

static VOID os74kTerminationCleanup ( PUKT_CTRL_REC  pUKT )
  {
  #undef  MF__
  #define MF__ MOD__"os74kTerminationCleanup"

  DBGIN;
  
  /*
  *  Release all user tasks.
  */
  for ( pUKT->pCTask = pUKT->TaskList.pFirstInTaskList; 
        pUKT->pCTask;
        pUKT->pCTask = pUKT->pCTask->pNextInTaskList )
    {
    switch ( pUKT->pCTask->TaskType )
      {
      case TT_UT_EO00:
      case TT_EV_EO00:
      case TT_US_EO00:
        // --- cleanup communication
        sql53k_comm_termination_cleanup ( pUKT->pCTask );
        break;
      }
    }

  if ( XPARAM(showMaxStackUse) )
    {
    for ( pUKT->pCTask = pUKT->TaskList.pFirstInTaskList; 
          pUKT->pCTask;
          pUKT->pCTask = pUKT->pCTask->pNextInTaskList )
      {
      /* Only active tasks are dumped */
      if ( TSK_INACTIVE_EO00 != pUKT->pCTask->TaskState )
        vsShowMaxStackUse( pUKT->pCTask->ulTaskIndex, eo92GetTaskTypeName(pUKT->pCTask->TaskType) );
      }
    }


  /*
   *  All threads not containing the tracewriter
   *  should exit as immediate as possible.
   */
  if ( kgs.pTW->pUKT != pUKT )
    {
    // --- set UKT state to 'UKT_TERMINATED' - this should be done before
    //     entering 'shutdown_free_regions'
    pUKT->ThrdCtrlHeader.ThrdState = UKT_TERMINATED;

    sql55k_free_regions ( pUKT );

    for ( pUKT->pCTask = pUKT->TaskList.pFirstInTaskList; pUKT->pCTask;
          pUKT->pCTask = pUKT->pCTask->pNextInTaskList )
      {
      pUKT->pCTask->TaskState = TSK_TERMINATED_EO00;
      }

    #if !defined(_WIN32)
     UNGUARD_EXCEPTION(pUKT->pExceptionRecord);
    #endif

    EXITTHREAD( 0, pUKT->ThrdCtrlHeader.hThrd );
    }
  else
    sql55k_free_regions ( pUKT );

  DBGOUT;
  return;
  }

/*------------------------------*/

_INLINE VOID __os74MoveTask ( PTASK_CTRL_REC   pCTask,
                              PDLQ_REC         pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"__os74MoveTask"
  PUKT_CTRL_REC    pUKT       = pCTask->pUKT;
  RTETask_Task*    pTask     = pCTask->pTaskObject;
  SAPDB_UInt2      Entry;
  SAPDB_UInt2      TimeVar;

  // alter move task static entry
  //
  WAIT_UNTIL_ALONE(&kgs.LastTaskMoves.lExclusive);
  Entry                       = kgs.LastTaskMoves.CurrEntry;
  kgs.LastTaskMoves.CurrEntry = NEXT_ENTRY( Entry, NUM_OF_TASK_MOVE_STAT_RECS);
  CLEARLOCK(&kgs.LastTaskMoves.lExclusive);

  TimeVar = PREV_TIME_VAR(pUKT->CurrTimeVar);

  kgs.LastTaskMoves.Entries[Entry].MoveTime                    = kgs.ulCurrTime;
  kgs.LastTaskMoves.Entries[Entry].pTaskCtrl                   = pCTask;
  kgs.LastTaskMoves.Entries[Entry].TaskMaxRunnableMicroSeconds = pCTask->MaxRunnableMicroSeconds[TimeVar];
  kgs.LastTaskMoves.Entries[Entry].pSrcUKT                     = pUKT;
  kgs.LastTaskMoves.Entries[Entry].SrcMaxRunnableMicroSeconds  = pUKT->MaxRunnableMicroSeconds[TimeVar];
  kgs.LastTaskMoves.Entries[Entry].pDstUKT                     = pUKT->pMoveTaskToUKT;
  kgs.LastTaskMoves.Entries[Entry].DstMaxRunnableMicroSeconds  = pUKT->pMoveTaskToUKT->MaxRunnableMicroSeconds[TimeVar];

  // --- remove this task from task list
  sql73k_tlist_dequ_task_nolock ( &pUKT->TaskList, pCTask );

  kgs.TaskMoved = true;

  MSGD(( INFO_TASK_MOVE_REQUEST_TO, pCTask->ulTaskIndex, pCTask->pUKT->ulUKTIndex,
         pUKT->pMoveTaskToUKT->ulUKTIndex ));

  sql74k_alter_task_cnt( pCTask, SUB_ALL_COUNTER );

  pCTask->pUKT                    = pUKT->pMoveTaskToUKT;
  pUKT->pMoveTaskToUKT            = NULL;
  pUKT->StopWaitForBestFitTaskAt = 0;

  /* 
   * The TaskScheduler has changed update task object and signal
   * this change to the clients
   */
  pTask->LoadTaskScheduler(); // load new task scheduler first!
  pTask->ClientConnections().UpdateClientsTaskSchedulerSemaphoreIndexes();

  pRequest->ulOrgReqType = pRequest->ulReqType;
  pRequest->ulReqType    = REQ_INSERT_MOVING_TASK_E000;

  sql74k_UKT_enqu_and_wake ( pCTask->pUKT, pCTask, pRequest );
  }

/*------------------------------*/

void __os74UKTStatisticReset( PUKT_CTRL_REC   pUKT, 
                              PTASK_CTRL_REC  pCurrTaskCtrl )
{

  SAPDB_Bool resetPending = pUKT->SleepStatRecResetPending;
  if ( resetPending )
  {
      SAPDB_UInt8 timeNow = RTESys_MicroSecTimer();

      union
      {
          FILETIME        fileTime;
          SAPDB_UInt8     timeCounter;
      }                               systemTime, userTime; // time the thread has spent in kernel/user mode         
      FILETIME                        creationTime;
      FILETIME                        exitTime;

      memset(&pUKT->UKTRunningStat, 0, sizeof(teo52_TaskSchedulerTime));
      memset(&pUKT->UKTIOWaitStat, 0, sizeof(teo52_TaskSchedulerTime));
      pUKT->RunningStateEnterMicroSeconds    = 0;
      pUKT->IdleTimeMeasureBeginMicroseconds = timeNow;
      GetThreadTimes(pUKT->ThrdCtrlHeader.hThrd, &creationTime, &exitTime, &systemTime.fileTime, &userTime.fileTime);
      pUKT->systemTimeAtLastReset = systemTime.timeCounter/10;
      pUKT->userTimeAtLastReset = userTime.timeCounter/10;
      pUKT->maxRunQueueLenSinceLastReset = 0;
      pUKT->SleepStatRecResetPending = false;
  }
}


/*------------------------------*/

void __os74TaskStatisticReset( PTASK_CTRL_REC  pCurrTaskCtrl )
{
  pCurrTaskCtrl->TimeCollectionEnabled = XPARAM(fTimeMeasure) != 0;

  if ( pCurrTaskCtrl->TimeCollectionEnabled )
  {
    if ( EO52_NO_RESET_PENDING != pCurrTaskCtrl->TaskStatRecResetPending )
    {
      memset( &(pCurrTaskCtrl->TaskStateStat), 0, sizeof(teo52_TaskStateStatisticRec) );

      if ( EO52_RESET_TASK_FULL_PENDING == pCurrTaskCtrl->TaskStatRecResetPending )
      {
        memset ( &pCurrTaskCtrl->QueStat, 0, sizeof ( teo52_TaskQueueStatistic ) );
        pCurrTaskCtrl->QueueStatRecResetPending = false;

        memset ( &pCurrTaskCtrl->PrioStat, 0, sizeof ( teo52_TaskPrioStatistic ) );

        pCurrTaskCtrl->ulDispatcherCalls = 1;
        pCurrTaskCtrl->ulBegExclCalls    = 0;
        pCurrTaskCtrl->ulSelfSuspCount   = 0;
      }
      pCurrTaskCtrl->TaskStatRecResetPending = EO52_NO_RESET_PENDING;
    }

    if ( pCurrTaskCtrl->QueueStatRecResetPending )
    {
      memset ( &pCurrTaskCtrl->QueStat, 0, sizeof ( teo52_TaskQueueStatistic ) );
      pCurrTaskCtrl->QueueStatRecResetPending = false;
    }
  }
}


/*------------------------------*/

void __os74DoEnterDispatcherTiming( PUKT_CTRL_REC   pUKT, 
                                    PTASK_CTRL_REC  pCurrTaskCtrl )
{
  SAPDB_UInt8 timeNow = RTESys_MicroSecTimer();

  if ( TASK_IS_IN_RUN_QUEUE_EO00 == pCurrTaskCtrl->QueLocation )
  {
    /* if self dispatching, reset runqueueEnqueueTime */
    /* to prevent negaitiv differenz to dispatcherEnterTime */
    pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime = timeNow;
  }

  pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime = timeNow;

  if ( pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime )
    pUKT->UKTRunTimeMicroSeconds += timeNow - pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime;
}

/*------------------------------*/

void __os74DoLeaveDispatcherTiming( PTASK_CTRL_REC pCurrTaskCtrl )
{
  SAPDB_UInt8     timeNow = RTESys_MicroSecTimer();

  pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime = timeNow;

  if ( pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime 
       > pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime )
  {
    MSGD (( ERR_INTERNAL_MICRO_TIMER_FAILED_I64,
            "Enter", pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime, 
            "Leave", pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime ));
    // System clock might be changed, Correct this leap!!
    pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime = 
        pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime;
  }

  if ( pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime )
  {
    SAPDB_UInt8     RunnableMicroSeconds;
    PUKT_CTRL_REC   pUKT           = pCurrTaskCtrl->pUKT;
    SAPDB_UInt2     CurrUKTTimeVar = pUKT->CurrTimeVar;

    if ( pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime
         > pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime )
    {
      MSGD (( ERR_INTERNAL_MICRO_TIMER_FAILED_I64,
              "Enqueue", pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime, 
              "Leave", pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime ));
      // System clock might be changed, Correct this leap!!
      pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime = 
          pCurrTaskCtrl->TaskStateStat.dispatcherLeaveTime;
    }

    /* PTS 1118094 problem with several requests pending... */
    if ( pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime 
         > pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime )
    {
        pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime = pCurrTaskCtrl->TaskStateStat.dispatcherEnterTime;
    }

    RunnableMicroSeconds = timeNow - pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime;

    if ( pCurrTaskCtrl->LastTimeVar != CurrUKTTimeVar )
    { // since the last runninig state the counter variable has changed!

      // set the Tasks runnable time values
      pCurrTaskCtrl->RunnableMicroSeconds[CurrUKTTimeVar]    = RunnableMicroSeconds;
      pCurrTaskCtrl->MaxRunnableMicroSeconds[CurrUKTTimeVar] = RunnableMicroSeconds;
      pCurrTaskCtrl->LastTimeVar                             = CurrUKTTimeVar;
    }
    else
    {
      // accumulate the tasks time waititng in the run queue to be scheduled
      pCurrTaskCtrl->RunnableMicroSeconds[CurrUKTTimeVar] += RunnableMicroSeconds;

      // Has the tasks a new runnable time maximum?
      if ( pCurrTaskCtrl->MaxRunnableMicroSeconds[CurrUKTTimeVar] < RunnableMicroSeconds )
        pCurrTaskCtrl->MaxRunnableMicroSeconds[CurrUKTTimeVar] = RunnableMicroSeconds;
    }

    // set the UKTs runnable time values
    pUKT->RunnableMicroSeconds[CurrUKTTimeVar] += RunnableMicroSeconds;

    // long term move lock set?
    if ( pCurrTaskCtrl->MoveLock.Type.LongTerm == 0 ) 
    { //  - Is there a new UKTs maximum of the tasks time staying in the run queue?
      if ( pUKT->MaxRunnableMicroSeconds[CurrUKTTimeVar] < RunnableMicroSeconds )
        pUKT->MaxRunnableMicroSeconds[CurrUKTTimeVar] = RunnableMicroSeconds;
    }
  }
  else
  {   /* runqueueEnqueueTime should have a value! */
      pCurrTaskCtrl->TaskStateStat.runqueueEnqueueTime = timeNow;
  }
}

/*------------------------------*/

void __os74WaitForWakeup( PUKT_CTRL_REC pUKT, SAPDB_UInt4 ulTimeOut )
{
    APIRET                          rc;
    SAPDB_UInt8                     UKTSleepingStateEnterMicroSeconds;
    SAPDB_UInt8                     UKTSleepTimeMicroSeconds;
    SAPDB_UInt8                     UKTRunningStateEnterMicroSeconds;


    UKTSleepingStateEnterMicroSeconds = RTESys_MicroSecTimer();

    if ( pUKT->RunningStateEnterMicroSeconds )      /* if it is set, it marks the time after wakeup below... */
    {
        SAPDB_UInt8 delta_time = UKTSleepingStateEnterMicroSeconds 
                               - pUKT->RunningStateEnterMicroSeconds;
        pUKT->UKTRunningStat.TotalTimeStat.ulCount++;
        pUKT->UKTRunningStat.TimeStat.ulCount++;
        pUKT->UKTRunningStat.TotalTimeStat.ulAbsMicroSeconds += delta_time;
        pUKT->UKTRunningStat.TimeStat.ulAbsMicroSeconds += delta_time;
    }

    rc = sql41c_wait_event_sem (pUKT->ThrdCtrlHeader.hevSem, ulTimeOut, "UKT");
    
    UKTRunningStateEnterMicroSeconds = RTESys_MicroSecTimer();
    UKTSleepTimeMicroSeconds         = UKTRunningStateEnterMicroSeconds 
                                     - UKTSleepingStateEnterMicroSeconds;


    if ( pUKT->lWaitForDeviIO )
    {
        /* UKTIOWaitStat.ulAbsMicroSeconds only counts the micro seconds spended waiting for DeviIO */
        pUKT->UKTIOWaitStat.TotalTimeStat.ulCount++;
        pUKT->UKTIOWaitStat.TimeStat.ulCount++;
        pUKT->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds += UKTSleepTimeMicroSeconds;
        pUKT->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds += UKTSleepTimeMicroSeconds;
    }

    if ((rc != NO_ERROR) && (rc != ERROR_TIMEOUT))
      ABORT();

    // --- set the UKT sleep time and the running state enter time in micro seconds.
    pUKT->UKTSleepTimeMicroSeconds[pUKT->CurrTimeVar] += UKTSleepTimeMicroSeconds;
    pUKT->RunningStateEnterMicroSeconds                = UKTRunningStateEnterMicroSeconds;
}

/*------------------------------*/

VOID __os74SendMoveMeassIntRequest()
  {
  #undef  MF__
  #define MF__ MOD__"__os74SendMoveMeassIntRequest"
  PUKT_CTRL_REC                    pTmpUKT;
  PDLQ_REC                         pRequest;
  tos00k_DlqNewTaskMoveMeasReqArgs *pReqArg;

  // - This check is needed, because of an non responding UKT leeds to an empty 
  //   'FREELIST' 
  for ( pTmpUKT =  kgs.pFirstUKTCtrl;
        pTmpUKT <= kgs.pLastUKTCtrl;
        pTmpUKT++ )
  {
      if ((pTmpUKT->DispatcherCallsDuringMesIntReq == pTmpUKT->DispatcherCalls) &&
          (pTmpUKT->ThrdCtrlHeader.ThrdState       != KT_SLEEPING ))
          return;
  }

  kgs.CurrTimeVar                                 = NEXT_TIME_VAR(kgs.CurrTimeVar);
  kgs.TaskMoveIntervalStartTimes[kgs.CurrTimeVar] = kgs.ulCurrTime;
  kgs.TaskMoveMeasIntervals++;

  for ( pTmpUKT =  kgs.pFirstUKTCtrl;
        pTmpUKT <= kgs.pLastUKTCtrl;
        pTmpUKT++ )
    {
        // --- send new measurement interval request to UKT
        pRequest                           = RESERVE_FREELIST_ELEM();
        pRequest->ulReqType                = REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00;
        pReqArg                            = &pRequest->ReqArgs.NewTaskMoveMeasRequest;
        pReqArg->TimeVar                   = kgs.CurrTimeVar;
        pReqArg->TaskMoveIntervalStartTime = kgs.TaskMoveIntervalStartTimes[kgs.CurrTimeVar];

        // --- store to use during next interval request
        pTmpUKT->DispatcherCallsDuringMesIntReq = pTmpUKT->DispatcherCalls;

        sql73k_UKT_enqu_no_task ( pTmpUKT, pRequest );
        sql72k_wake_ukt ( pTmpUKT );
    }
  }

/*------------------------------*/

VOID __os74SendMoveRequest ( PUKT_CTRL_REC   pSrcUKT,
                             PUKT_CTRL_REC   pDstUKT,
                             SAPDB_UInt8     StopWaitForBestFitTaskAt )
  {
  #undef  MF__
  #define MF__ MOD__"__os74SendMoveRequest"
  PDLQ_REC                        pRequest;
  tos00k_DlqMoveTaskReqArgs      *pReqArg;

  // - This check is needed, because of an non responding UKT leeds to an empty 
  //   'FREELIST' 
  if ((pSrcUKT->DispatcherCallsDuringSendMoveReq == pSrcUKT->DispatcherCalls) &&
      (pSrcUKT->ThrdCtrlHeader.ThrdState         != KT_SLEEPING ))
      return;

  // --- send move task
  pRequest                           = RESERVE_FREELIST_ELEM();
  pRequest->ulReqType                = REQ_MOVE_TASK_EO00;
  pReqArg                            = &pRequest->ReqArgs.MoveTaskRequest;
  pReqArg->pMoveTaskToUKT            = pDstUKT;
  pReqArg->StopWaitForBestFitTaskAt  = StopWaitForBestFitTaskAt;
  pReqArg->TaskMoveIntervalStartTime = kgs.TaskMoveIntervalStartTimes[kgs.CurrTimeVar];

  // --- store to use during next interval request
  pSrcUKT->DispatcherCallsDuringSendMoveReq = pSrcUKT->DispatcherCalls;

  sql73k_UKT_enqu_no_task ( pSrcUKT, pRequest );
  sql72k_wake_ukt ( pSrcUKT );
  }

/*------------------------------*/

SAPDB_Bool __os74TaskMoveCheck( PUKT_CTRL_REC *ppSrcUKT, PUKT_CTRL_REC *ppDstUKT )
  {
  #undef  MF__
  #define MF__ MOD__"__os74TaskMoveCheck"
  PUKT_CTRL_REC                   pTmpUKT;
  SAPDB_UInt8                     MicroSecondsCompare;
  SAPDB_UInt2                     TimeVar, PrevTimeVar;

  TimeVar     = kgs.CurrTimeVar;
  PrevTimeVar = PREV_TIME_VAR(TimeVar);

  for ( *ppSrcUKT =  NULL, MicroSecondsCompare = 0,
        pTmpUKT =  kgs.pFirstUKTCtrl;
        pTmpUKT <= kgs.pLastUKTCtrl;
        pTmpUKT++ )
    {
    if ( pTmpUKT->MaxRunnableMicroSeconds[PrevTimeVar] == 0 ) // This task has no problem!
      continue;

    // --- the UKT has not changed its time counter variable for a long time,
    //     might be running in a tight loop
    if ((pTmpUKT->TaskMoveIntervalStartTime != kgs.TaskMoveIntervalStartTimes[PrevTimeVar]) &&
        (pTmpUKT->TaskMoveIntervalStartTime != kgs.TaskMoveIntervalStartTimes[TimeVar]))
	    {
      continue;
      }

    // - Is there at least one movable task and are threre two or more active tasks inside the UKT
    if (( pTmpUKT->ulActiveTasks > 1 ) && ( pTmpUKT->NumOfMovableTasks != 0 )) 
      {
      // --- The time between the following values is treated as equal if the
      //     difference is within a 'XPARAM(uLoadBalancingEquality)' percent range.
      switch ( TASK_MOVE_TIME_COMP( MicroSecondsCompare, 
                                    pTmpUKT->MaxRunnableMicroSeconds[PrevTimeVar], 
                                    XPARAM(uLoadBalancingEquality )))
        {
        case TASK_MOVE_TIME_COMP_GREATER:
          *ppSrcUKT           = pTmpUKT;
          MicroSecondsCompare = (*ppSrcUKT)->MaxRunnableMicroSeconds[PrevTimeVar];
          break;
        case TASK_MOVE_TIME_COMP_EQUAL:
          if (( *ppSrcUKT == NULL ) ||( pTmpUKT->RunnableMicroSeconds[PrevTimeVar] > (*ppSrcUKT)->RunnableMicroSeconds[PrevTimeVar] ))
            {
            *ppSrcUKT           = pTmpUKT;
            MicroSecondsCompare = (*ppSrcUKT)->MaxRunnableMicroSeconds[PrevTimeVar];
            }
        break;
        }
      }
    }

  if ( (*ppSrcUKT) == NULL )
    return false;

  for ( *ppDstUKT = NULL, MicroSecondsCompare = INFINIT_TIME_VAL,
        pTmpUKT =  kgs.pFirstUKTCtrl;
        pTmpUKT <= kgs.pLastUKTCtrl;
        pTmpUKT++ )
    {
    // --- the UKT has not changed its time counter variable for a long time,
    //     might be running in a tight loop
    if ((pTmpUKT->TaskMoveIntervalStartTime != kgs.TaskMoveIntervalStartTimes[PrevTimeVar]) &&
        (pTmpUKT->TaskMoveIntervalStartTime != kgs.TaskMoveIntervalStartTimes[TimeVar]))
      {
      continue;
		  } 
	  
	  if (( pTmpUKT != *ppSrcUKT ) && ( pTmpUKT->TasksAllowedToMoveTo == true ))
      {
      // --- The time between the following UKTs is treated as equal if the
      //     difference is within a 'XPARAM(uLoadBalancingDiff)' percent range.
      if ( TASK_MOVE_TIME_COMP( (*ppSrcUKT)->MaxRunnableMicroSeconds[PrevTimeVar], 
                                pTmpUKT->MaxRunnableMicroSeconds[PrevTimeVar], 
                                XPARAM(uLoadBalancingDiff)) == TASK_MOVE_TIME_COMP_LESS )
        {
        // --- The time between the following values is treated as equal if the
        //     difference is within a 'XPARAM(uLoadBalancingEquality)' percent range.
        switch ( TASK_MOVE_TIME_COMP( MicroSecondsCompare, 
                                      pTmpUKT->MaxRunnableMicroSeconds[PrevTimeVar], 
                                      XPARAM(uLoadBalancingEquality )))
          {
          case TASK_MOVE_TIME_COMP_LESS:
            *ppDstUKT           = pTmpUKT;
            MicroSecondsCompare = (*ppDstUKT)->MaxRunnableMicroSeconds[PrevTimeVar];
            break;
          case TASK_MOVE_TIME_COMP_EQUAL:
            if (( *ppDstUKT == NULL ) || ( pTmpUKT->UKTSleepTimeMicroSeconds[PrevTimeVar] > (*ppDstUKT)->UKTSleepTimeMicroSeconds[PrevTimeVar] ))
              {
              *ppDstUKT           = pTmpUKT;
              MicroSecondsCompare = (*ppDstUKT)->MaxRunnableMicroSeconds[PrevTimeVar];
              }
          break;
          }
        }
      }
    }

  if ( *ppDstUKT != NULL )
    return true; // we have found a UKT which should move a task to another UKT

  return false;
  }


/*
 * =============================== END ========================================
 */
