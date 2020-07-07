/*!
  @file           vos56k.c
  @author         RaymondR
  @brief          scheduling
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
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo50_0.h"
#include "geo002.h"
#include "heo56.h"
#include "heo52.h"
#include "geo00_2.h"
#include "gos74.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"


/*
 *  DEFINES
 */
#define MOD__  "vos56kc: "
#define MF__   MOD__"UNDEFINED"

#if defined (SAPDB_SLOW) || defined (SAPDB_QUICK)
# define MULTIPLE_SIGNAL_CHECK     1
#endif

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
static void sql56k_remote_vsignal ( PPTASK_CTRL_REC ppCTask,
                                    PTASK_CTRL_REC  pFromTask,
                                    PPDLQ_REC       ppCurrReq,
                                    void*           pParam );

static void sql56k_remote_vresume ( PPTASK_CTRL_REC ppCTask,
                                    PTASK_CTRL_REC  pFromTask,
                                    PPDLQ_REC       ppCurrReq,
                                    void*           pDummyParam );

static void os56_StoreResumer(      PTASK_CTRL_REC	tcb ,
                                    tsp00_TaskId	pid,
									tsp00_Int4      allowedResCount );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */


void vstop(tsp00_TaskId pid)
{
  #undef  MF__
  #define MF__ MOD__"vstop"
    PTASK_CTRL_REC pTaskCtrl = KGS->pFirstTaskCtrl + pid - 1;
    PUKT_CTRL_REC  pUKT      = pTaskCtrl->pUKT;
    pTaskCtrl->TaskState = TSK_STOPPED_EO00;

    WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

    --(pTaskCtrl->lVStopCounter);

    if ( pTaskCtrl->lVStopCounter == -1 )
    {
        CLEARLOCK( &pTaskCtrl->lExclusive );
        GOTO_DISP(&pUKT);
        if ( pUKT->pCurrReq != GetTaskDLQElement1( pTaskCtrl ) )
        {
            MSGD (( ERR_VSTOP_WRONG_EVENT ));
            ABORT();
        }

        if ( pUKT->pCurrReq->ulReqType != REQ_VSTOP_EO00 )
        {
            MSGD (( ERR_VSTOP_WRONG_REQ, pUKT->pCurrReq->ulReqType ));
            ABORT();
        }
    }
    else
    {
        CLEARLOCK( &pTaskCtrl->lExclusive );
    }
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
}

/* ----------------------------------- */

void vcontinue(tsp00_TaskId pid)
{
  #undef  MF__
  #define MF__ MOD__"vcontinue"
    PTASK_CTRL_REC pTaskCtrl = KGS->pFirstTaskCtrl + pid - 1;

    WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

    ++(pTaskCtrl->lVStopCounter);

    if ( pTaskCtrl->lVStopCounter == 0 )
    {
        DLQ_ELEMENT* pSpecDLQElem1;

        CLEARLOCK( &pTaskCtrl->lExclusive );
        
        pSpecDLQElem1 = GetTaskDLQElement1( pTaskCtrl );

        pSpecDLQElem1->ulReqType = REQ_VSTOP_EO00;
        sql74k_UKT_enqu_and_wake( pTaskCtrl->pUKT, pTaskCtrl, pSpecDLQElem1 );
    }
    else
    {
        CLEARLOCK( &pTaskCtrl->lExclusive );
    }
}

/*------------------------------*/

VOID vsuspend          ( tsp00_TaskId      pid,
                         tsp00_Int2        suspend_reason )
  {
  #undef  MF__
  #define MF__ MOD__"vsuspend"
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif


  DBGIN_T (pTaskCtrl->ulTaskIndex);

#ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vsuspend", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
#endif

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
  #endif


  pTaskCtrl->TaskState = TSK_VSUSPEND_EO00;
  pTaskCtrl->ulStateReason = suspend_reason ;
  if ( suspend_reason < MAX_SUSPEND_REASONS )
    kgs.ulSuspendReasons [ suspend_reason ] ++ ;

  pTaskCtrl->TaskStateStat.VsuspStat.TotalTimeStat.ulCount++ ;
  pTaskCtrl->TaskStateStat.VsuspStat.TimeStat.ulCount++ ;

  pTaskCtrl->lResumeCount--;

  if ( pTaskCtrl->lResumeCount < -1 )
  { 
      int i;
      int j;
      MSGD (( ERR_WRONG_RESUME_COUNT, pTaskCtrl->ulTaskIndex, pTaskCtrl->lResumeCount ));

      i = pTaskCtrl->totalResumerCount;
      j = MAX_RESUMER_PIDS;
      while( (i-- > 0) && (j-- > 0) )
      {
          MSGD (( WRN_RESUMER_HISTORY, pTaskCtrl->resumerPid[i%MAX_RESUMER_PIDS]));
      }
      ABORT();
  }

  if ( pTaskCtrl->lResumeCount < 0 )
    {
    DBG4 (( MF__, "[T:0x%03u] suspending myself",
            pTaskCtrl->ulTaskIndex));

    GOTO_DISP(&pUKT);

    if ( pTaskCtrl->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &pTaskCtrl->TaskStateStat.VsuspStat, &pTaskCtrl->TaskStateStat );
    }

    if ( pUKT->pCurrReq != GetTaskDLQElement1( pTaskCtrl ) )
      {
	  MSGD ((ERR_AWOKE_FROM_WRONG_EVENT, "vsuspend", pTaskCtrl->ulTaskIndex, pUKT->pCurrReq, GetTaskDLQElement1(pTaskCtrl)));	  	    
      MSGD (( ERR_VSUSPEND_WRONG_REQ_ARGS,
	  		  pTaskCtrl->ulTaskIndex,
			  pUKT->pCurrReq->pFromTaskCtrl ? pUKT->pCurrReq->pFromTaskCtrl->ulTaskIndex : 0,
			  pUKT->pCurrReq->ulReqType, REQ_VRESUME_EO00 ));

      DBG1 (( MF__, "[T:0x%03u] pUKT->pCurrReq: 0x%x expacted pRequest: 0x%x",
              pTaskCtrl->ulTaskIndex, pUKT->pCurrReq,
              GetTaskDLQElement1( pTaskCtrl ) ));

      ABORT();
      }

    if ( pUKT->pCurrReq->ulReqType != REQ_VRESUME_EO00 )
      {
      MSGD (( ERR_VSUSPEND_WRONG_REQ ));

      MSGD (( ERR_VSUSPEND_WRONG_REQ_ARGS,
              pTaskCtrl->ulTaskIndex,
			  pUKT->pCurrReq->pFromTaskCtrl ? pUKT->pCurrReq->pFromTaskCtrl->ulTaskIndex : 0,
              pUKT->pCurrReq->ulReqType, REQ_VRESUME_EO00 ));

      ABORT();
      }
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  pTaskCtrl->ulStateReason = 0 ;
  pTaskCtrl->lWaitForTask = pTaskCtrl->lWaitForRoot = UNDEF ;
  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

void   vSuspendForPageLock ( tsp00_TaskId      pid,
							 tsp00_Int2        suspend_reason,
                             tsp00_Int4        page_no )
{
#undef  MF__
#define MF__ MOD__"vSuspendForPageLock"
#ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
#else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
#endif

   DBGIN_T (pTaskCtrl->ulTaskIndex);

   pTaskCtrl->lockedPageNo = page_no;

   vsuspend(pid, suspend_reason);

   pTaskCtrl->lockedPageNo = MAX_INT4_SP00;

   DBGOUT_T(pTaskCtrl->ulTaskIndex);
}

/*------------------------------*/

tsp00_Bool visresume ( tsp00_TaskId pid )
{
#ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
#else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
#endif
   tsp00_Bool IsResume ;


   DBGIN_T (pTaskCtrl->ulTaskIndex);

#ifdef DEVELOP_CHECKS
   if ( pid != (tsp00_TaskId)pTaskCtrl->ulTaskIndex )
   {
     MSGD (( ERR_VXXXX_WRONG_TASK, "visresume", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
   }

  IsResume =  (pUKT->pCTask->lResumeCount > 0 );

#else
  IsResume = ( pUKT->pCTask->lResumeCount > 0 );
#endif

  DBGOUT_T (pTaskCtrl->ulTaskIndex);

  return(IsResume);
}

/*------------------------------*/

VOID vresume   ( tsp00_TaskId      pid)

  {
  #undef  MF__
  #define MF__ MOD__"vresume"
  PUKT_CTRL_REC                 pUKT         = THIS_UKT_CTRL;
  PTASK_CTRL_REC                pTaskCtrl    = pUKT->pCTask;
  PTASK_CTRL_REC                pReqTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  PDLQ_REC                      pRequest;


  DBGIN_T (pTaskCtrl->ulTaskIndex);

  if ( pid == (tsp00_TaskId)pTaskCtrl->ulTaskIndex )
    {
    MSGD (( ERR_VXXXX_WRONG_TASK, "vresume", pid ));
    DBG1 (( MF__, "[T:0x%03u] pid out range %u", pTaskCtrl->ulTaskIndex, pid ));
    ABORT();
    }


  #ifdef DEVELOP_CHECKS
   // --- 'pid' out of range
   if ( pid < 1 || pid > (tsp00_TaskId)kgs.ulNumOfTasks )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vresume", pid ));
     DBG1 (( MF__, "[T:0x%03u] pid out range %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }

  #endif

  // ---  is the requested UKT eq. to the local UKT
  if ( pReqTaskCtrl->pUKT == pUKT )
  {
	tsp00_Bool  IsSuspended = pReqTaskCtrl->TaskState == TSK_VSUSPEND_EO00;

    DBG4 (( MF__,
            "[T:0x%03u] vresume request for local task [T:0x%03u]",
            pTaskCtrl->ulTaskIndex, pReqTaskCtrl->ulTaskIndex));

    pReqTaskCtrl->lResumeCount++ ;
    os56_StoreResumer(pReqTaskCtrl, pUKT->pCTask->ulTaskIndex, IsSuspended ? 0 : 1 );

    if ( pReqTaskCtrl->lResumeCount == 0 )
    {
        if ( IsSuspended )
        {
          DBG4 (( MF__, "[T:0x%03u] resuming local task [T:0x%03u]",
              pTaskCtrl->ulTaskIndex, pReqTaskCtrl->ulTaskIndex ));

          pRequest                 = GetTaskDLQElement1( pReqTaskCtrl );
          pRequest->pFromTaskCtrl  = pUKT->pCTask ;
          pRequest->ulReqType      = REQ_VRESUME_EO00;

          sql74k_RUN_enqu ( pUKT, pReqTaskCtrl, pTaskCtrl, pRequest, BASE_PRIO_RAV );
        }
        else
        {
		    MSGD (( ERR_VRESUME_WRONG_STATE ,
                pTaskCtrl->ulTaskIndex, pid, pReqTaskCtrl->ulTaskIndex ));
        }
    }
  }
  else
  {
    DBG4 (( MF__,
            "[T:0x%03u] vresume request for remote task [T:0x%03u]",
            pTaskCtrl->ulTaskIndex, pReqTaskCtrl->ulTaskIndex ));

    sql74k_UToU_enqu_and_wake  ( pUKT, pTaskCtrl, pReqTaskCtrl->pUKT,
                                 pReqTaskCtrl, sql56k_remote_vresume, 0);

  }

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
}

/*------------------------------*/

VOID  vwait   ( tsp00_TaskId      pid,
                bool             *ok )
  {
  #undef  MF__
  #define MF__ MOD__"vwait"
  #if defined (DEVELOP_CHECKS) || defined(MULTIPLE_SIGNAL_CHECK)
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif
   SAPDB_Bool                    isSignaled;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #if defined (DEVELOP_CHECKS) || defined(MULTIPLE_SIGNAL_CHECK)
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vwait", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
  #endif

  //
  // ---  Vsignal sets 'lVSignalOk' to 1 for 'TRUE' and to -1 for 'FALSE'.
  //      Zero means that no vsignal was issued.

  // --- vsignal not issued?
#if defined (MULTIPLE_SIGNAL_CHECK)
  WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
   isSignaled = pTaskCtrl->lVSignalOk != 0;
  CLEARLOCK( &pTaskCtrl->lExclusive );
#else
   isSignaled = pTaskCtrl->lVSignalOk != 0;
#endif

  if ( !isSignaled )
    {
    pTaskCtrl->TaskState = TSK_VWAIT_EO00;
    pTaskCtrl->TaskStateStat.VwaitStat.TotalTimeStat.ulCount++ ;
    pTaskCtrl->TaskStateStat.VwaitStat.TimeStat.ulCount++ ;

    DBG4 (( MF__, "[T:0x%03u] waiting for 'vsignal'",
            pTaskCtrl->ulTaskIndex ));

    GOTO_DISP(&pUKT);
    if ( pTaskCtrl->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &pTaskCtrl->TaskStateStat.VwaitStat, &pTaskCtrl->TaskStateStat );
    }

    if ( pUKT->pCurrReq != GetTaskDLQElement1( pTaskCtrl ) )
      {
      MSGD ((ERR_AWOKE_FROM_WRONG_EVENT, "vwait", pTaskCtrl->ulTaskIndex, pUKT->pCurrReq, GetTaskDLQElement1( pTaskCtrl )));	  	    
      MSGD (( ERR_VWAIT_WRONG_REQ_ARGS,
              pTaskCtrl->ulTaskIndex,
		      pUKT->pCurrReq->pFromTaskCtrl ? pUKT->pCurrReq->pFromTaskCtrl->ulTaskIndex : 0,
              pUKT->pCurrReq->ulReqType, REQ_VSIGNAL_EO00 ));
      DBG1 (( MF__, "[T:0x%03u] pUKT->pCurrReq: 0x%x expacted pRequest: 0x%x",
              pTaskCtrl->ulTaskIndex, pUKT->pCurrReq,
              GetTaskDLQElement1( pTaskCtrl ) ));

      ABORT();
      }

    if ( pUKT->pCurrReq->ulReqType != REQ_VSIGNAL_EO00 )
      {
      MSGD (( ERR_VWAIT_WRONG_REQ ));

      MSGD (( ERR_VWAIT_WRONG_REQ_ARGS,
              pTaskCtrl->ulTaskIndex,
			  pUKT->pCurrReq->pFromTaskCtrl ? pUKT->pCurrReq->pFromTaskCtrl->ulTaskIndex : 0,
              pUKT->pCurrReq->ulReqType, REQ_VSIGNAL_EO00 ));

      ABORT();
      }
    pTaskCtrl->TaskState  = TSK_RUNNING_EO00;
    }

#if defined (MULTIPLE_SIGNAL_CHECK)
  WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
#endif

  switch ( pTaskCtrl->lVSignalOk )
    {
    case  1 : *ok = TRUE; break;
    case -1 : *ok = FALSE; break;
    default : CLEARLOCK( &pTaskCtrl->lExclusive );
#             if defined (MULTIPLE_SIGNAL_CHECK)
               CLEARLOCK( &pTaskCtrl->lExclusive );
               MSGD (( ERR_VWAIT_ILL_BOOL_MPS, pTaskCtrl->lVSignalOk,
                       pTaskCtrl->ulTaskIndex, pTaskCtrl->lastSignalTID )); 
#             else
               MSGD (( ERR_VWAIT_ILL_BOOL, pTaskCtrl->lVSignalOk,
                       pTaskCtrl->ulTaskIndex ));
#             endif
              ABORT();
              break;
    }

  pTaskCtrl->lVSignalOk = 0;
#if defined (MULTIPLE_SIGNAL_CHECK)
  CLEARLOCK( &pTaskCtrl->lExclusive );
#endif
  pTaskCtrl->lWaitForTask = 0 ;

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID  vsignal    ( tsp00_TaskId      pid,
                   bool              ok )

  {
  #undef  MF__
  #define MF__ MOD__"vsignal"
  PUKT_CTRL_REC                 pUKT         = THIS_UKT_CTRL;
  PTASK_CTRL_REC                pReqTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  PDLQ_REC                      pRequest;
  PTASK_CTRL_REC                 pTaskCtrl    = pUKT->pCTask;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  // --- 'pid' out of range
  if ( pid < 1 || pid > (PROCESS_ID)kgs.ulNumOfTasks )
    {
    MSGD (( ERR_VXXXX_WRONG_TASK, "vsignal", pid ));
    DBG1 (( MF__, "[T:0x%03u] pid out range %u", pTaskCtrl->ulTaskIndex, pid ));
    ABORT();
    }

#if defined (MULTIPLE_SIGNAL_CHECK)

  WAIT_UNTIL_ALONE( &pReqTaskCtrl->lExclusive );

  if ( pReqTaskCtrl->lVSignalOk )
  {
      CLEARLOCK( &pReqTaskCtrl->lExclusive );
      MSGALL(( ERR_MULTIPLE_VSIGNAL_CALL_MPS, pTaskCtrl->ulTaskIndex,
               pReqTaskCtrl->ulTaskIndex, pReqTaskCtrl->lastSignalTID  ));
      ABORT(); 
  }

  pReqTaskCtrl->lastSignalTID = pTaskCtrl->ulTaskIndex;

  if ( ok )
      pReqTaskCtrl->lVSignalOk = 1;      // - means TRUE
  else
      pReqTaskCtrl->lVSignalOk = -1;     // - means FALSE

  CLEARLOCK( &pReqTaskCtrl->lExclusive );

#endif

  // ---  is the requested UKT eq. to the local UKT
  if ( pReqTaskCtrl->pUKT == pUKT )
    {
#   if !defined (MULTIPLE_SIGNAL_CHECK)

    if ( pReqTaskCtrl->lVSignalOk )
        {
        MSGD (( ERR_MULTIPLE_VSIGNAL_CALL, pReqTaskCtrl->ulTaskIndex ));
        ABORT(); 
        }

    if ( ok )
      pReqTaskCtrl->lVSignalOk = 1;      // - means TRUE
    else
      pReqTaskCtrl->lVSignalOk = -1;     // - means FALSE

#   endif


    DBG4 (( MF__, "[T:0x%03u] vsignal request for local task [T:0x%03u]",
            pTaskCtrl->ulTaskIndex, pReqTaskCtrl->ulTaskIndex ));

    if ( pReqTaskCtrl->TaskState  == TSK_VWAIT_EO00 )
      {
      pRequest                 = GetTaskDLQElement1( pReqTaskCtrl );
      pRequest->pFromTaskCtrl  = pTaskCtrl;
      pRequest->ulReqType      = REQ_VSIGNAL_EO00;

      sql74k_RUN_enqu ( pUKT, pReqTaskCtrl, pTaskCtrl, pRequest, BASE_PRIO_RAV );
      }
    }
  else
    {
    DBG4 (( MF__, "[T:0x%03u] vsignal request for remote task [T:0x%03u]",
            pTaskCtrl->ulTaskIndex, pReqTaskCtrl->ulTaskIndex ));

    sql74k_UToU_enqu_and_wake  ( pUKT, pTaskCtrl, pReqTaskCtrl->pUKT,
                                 pReqTaskCtrl, sql56k_remote_vsignal, (void*)ok );
    }

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID  vsleep    ( tsp00_TaskId      pid,
                  tsp00_Int2        limit )

  {
  #undef  MF__
  #define MF__ MOD__"vsleep"
  PDLQ_REC                       pRequest;
  tsp00_Longuint                 ulTimeNow;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);


  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vsleep", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  pTaskCtrl->TaskState = TSK_SLEEP_EO00;
  pTaskCtrl->TaskStateStat.VsleepStat.TotalTimeStat.ulCount++ ;
  pTaskCtrl->TaskStateStat.VsleepStat.TimeStat.ulCount++ ;

  if ( limit == 0 )
  {
    // --- reschedule myself via rex queue
    pRequest            = GetTaskDLQElement1( pTaskCtrl );
    pRequest->ulReqType = REQ_RESCHEDULE_EO00;
    pRequest->pTaskCtrl = pTaskCtrl;

    sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pRequest, BASE_PRIO_REX );

    GOTO_DISP(&pUKT);

    if ( pTaskCtrl->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &pTaskCtrl->TaskStateStat.VsleepStat, &pTaskCtrl->TaskStateStat );
    }
  }
  else
  {
    DBG4 (( MF__, "[T:0x%03u] sleep %d seconds", pTaskCtrl->ulTaskIndex, limit ));

    ulTimeNow                                   = kgs.ulCurrTime;
    pRequest                                    = GetTaskDLQElement1( pTaskCtrl );
    pRequest->ReqArgs.TimerRequest.ulTimeToWake = ulTimeNow + limit;
    pRequest->ulReqType                         = REQ_VSLEEP_EO00;
    pRequest->pTaskCtrl                         = pTaskCtrl;

    sql83k_timer_job ( pRequest );

    GOTO_DISP(&pUKT);

    if ( pTaskCtrl->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &pTaskCtrl->TaskStateStat.VsleepStat, &pTaskCtrl->TaskStateStat );
    }

    if ( pUKT->pCurrReq != pRequest )
      {
      MSGD ((ERR_AWOKE_FROM_WRONG_EVENT, "vsleep", pTaskCtrl->ulTaskIndex, pUKT->pCurrReq, pRequest));
      MSGD (( ERR_VSLEEP_WRONG_REQ_ARGS,
              pTaskCtrl->ulTaskIndex,
		      pUKT->pCurrReq->pFromTaskCtrl ? pUKT->pCurrReq->pFromTaskCtrl->ulTaskIndex : 0,
              pUKT->pCurrReq->ulReqType, REQ_VRESUME_EO00 ));

      DBG1 (( MF__, "[T:0x%03u] pUKT->pCurrReq: 0x%x expacted pRequest: 0x%x",
              pTaskCtrl->ulTaskIndex, pUKT->pCurrReq, pRequest ));

      ABORT();
      }

    if ( pUKT->pCurrReq->ulReqType != REQ_VSLEEP_EO00 )
      {
      MSGD (( ERR_VSLEEP_WRONG_REQ ));
      DBG1 (( MF__, "[T:0x%03u] awoke from wrong request type %d",
              pTaskCtrl->ulTaskIndex,
              pUKT->pCurrReq->ulReqType ));

      ABORT();
      }
    }

  DBG4 (( MF__, "[T:0x%03u] awakend",
          pTaskCtrl->ulTaskIndex, limit ));

  pTaskCtrl->TaskState  = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID  vreschedule_msec     ( tsp00_TaskId      pid,
                             tsp00_Int4        *StartMsec,
                             tsp00_Int4        *RemainMsec )

  {
  #undef  MF__
  #define MF__ MOD__"vreschedule_msec"
  PDLQ_REC                       pRequest;
  DWORD                          dwTimeNow;
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);


  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vwait_time", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  pTaskCtrl->TaskState = TSK_RESCHEDULE_MSEC_EO00;

  if ( *StartMsec == 0 ) 
    (DWORD) *StartMsec = GetTickCount();
   
  if ( pUKT->ulRUNQueueLen == 0 && kgs.fMoreUserUKTsThanMaxCPU )
    Sleep(0) ; /* no busy wait, ==> try to reschedule Thread */

  // --- reschedule myself via rex queue
  pRequest            = GetTaskDLQElement1( pTaskCtrl );
  pRequest->ulReqType = REQ_RESCHEDULE_EO00;
  pRequest->pTaskCtrl = pTaskCtrl;

  sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pRequest, BASE_PRIO_REX );

  GOTO_DISP(&pUKT);

  dwTimeNow = GetTickCount();

  if ( dwTimeNow - (DWORD )*StartMsec >= (DWORD )*RemainMsec || 
       dwTimeNow < (DWORD )*StartMsec )
    (DWORD )*RemainMsec = 0 ;
  else
  {
    (DWORD )*RemainMsec -= dwTimeNow - (DWORD )*StartMsec ;
    (DWORD )*StartMsec   = dwTimeNow ;
  } 

  DBG4 (( MF__, "[T:0x%03u] awakend, RemainMsec: %d",
          pTaskCtrl->ulTaskIndex, *RemainMsec ));

  pTaskCtrl->TaskState  = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static void sql56k_remote_vresume ( PPTASK_CTRL_REC ppCTask,
                                    PTASK_CTRL_REC  pFromTask,
                                    PPDLQ_REC       ppCurrReq,
                                    void*           pDummyParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql56k_remote_vresume"

  tsp00_Bool  IsSuspended = (*ppCTask)->TaskState == TSK_VSUSPEND_EO00;

  DBGPAS_T ((*ppCTask)->ulTaskIndex);

  (*ppCTask)->lResumeCount++ ;

  os56_StoreResumer( *ppCTask, pFromTask->ulTaskIndex, IsSuspended ? 0 : 1 );

  if (( (*ppCTask)->lResumeCount == 0 ) && IsSuspended )
    {
    DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( *ppCTask );

    DBG4 (( MF__, "[T:0x%03u] task resumed", (*ppCTask)->ulTaskIndex));
    *ppCurrReq               = pSpecDLQElem1;
    pSpecDLQElem1->ulReqType = REQ_VRESUME_EO00;
    return;
    }
  else
    {
    DBG4 (( MF__, "[T:0x%03u] task not resumed", (*ppCTask)->ulTaskIndex));
    (*ppCTask)   = 0;
    }

  return;
  }

/*------------------------------*/

static void sql56k_remote_vsignal ( PPTASK_CTRL_REC ppCTask,
                                    PTASK_CTRL_REC  pFromTask,
                                    PPDLQ_REC       ppCurrReq,
                                    void*           pParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql56k_remote_vsignal"

  BOOL            Ok = pParam != 0;

  DBGPAS_T ((*ppCTask)->ulTaskIndex);

# if !defined (MULTIPLE_SIGNAL_CHECK)

  if ( (*ppCTask)->lVSignalOk )
    {
    MSGD (( ERR_MULTIPLE_VSIGNAL_CALL, (*ppCTask)->ulTaskIndex ));
    ABORT(); 
    }

  if ( Ok )
    (*ppCTask)->lVSignalOk = 1;      // - means TRUE
  else
    (*ppCTask)->lVSignalOk = -1;     // - means FALSE
# endif

  if ( (*ppCTask)->TaskState == TSK_VWAIT_EO00 )
    {
    DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( *ppCTask );

    DBG4 (( MF__, "[T:0x%03u] signal ok", (*ppCTask)->ulTaskIndex));
    *ppCurrReq               = pSpecDLQElem1;
    pSpecDLQElem1->ulReqType = REQ_VSIGNAL_EO00;
    return;
    }
  else
    (*ppCTask) = 0;

  return;
  }

/*------------------------------*/

static void os56_StoreResumer(  PTASK_CTRL_REC pTaskCtrl ,
                                tsp00_TaskId   pid,
								tsp00_Int4     allowedResCount )
{
    pTaskCtrl->resumerPid [ pTaskCtrl->totalResumerCount%MAX_RESUMER_PIDS ] = pid;

    ++(pTaskCtrl->totalResumerCount);

    if ( pTaskCtrl->lResumeCount > allowedResCount )
    {
        int i;
        int j;
            MSGD (( ERR_WRONG_RESUME_COUNT, pTaskCtrl->ulTaskIndex, pTaskCtrl->lResumeCount ));

            i = pTaskCtrl->totalResumerCount;
            j = MAX_RESUMER_PIDS;
            while( (i-- > 0) && (j-- > 0) )
            {
        		MSGD (( WRN_RESUMER_HISTORY, pTaskCtrl->resumerPid[i%MAX_RESUMER_PIDS]));
            }
            ABORT();
    }
}

/*
 * =============================== END ========================================
 */
