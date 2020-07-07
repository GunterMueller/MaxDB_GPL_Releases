/*!
  @file           vos55k.c
  @author         RaymondR
  @brief          Mutual Exclusion
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
#include "gos00k.h"
#include "geo002.h"
#include "heo55k.h"
#include "gos74.h"
#include "geo50_0.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS55KC : "
#define MF__   MOD__"UNDEFINED"

#define MAX_TAS_CNT   -1        // TAS is an abbrevation for test-and-set, not for task!!!!
// #define SLEEP_CNT     10
#define SLEEP_CNT     MAX_TAS_CNT
#define BUSY_WAIT    1
#define NO_BUSY_WAIT 0


#define RESCHEDULE { SLEEP ( 0 ); }

/*
 *  MACROS
 */
#if defined (USE_HUGE_MACROS)   // not defined 
 #define WAIT_LOCK( _ppUKT, _pTaskCtrl, _pSemCtrl, _plExclusive, _sid,       \
                                       _plExclTasCnt, _fBusyWait )          \
   {                                                                        \
   LONG                           lTasCount;                                \
                                                                            \
   if ( INTERLOCK( _plExclusive ) )                                         \
     {                                                                      \
     lTasCount = MAX_TAS_CNT;                                               \
                                                                            \
     DBG4 (( MF__, "[T:0x%03u] Waiting for region interlock %u",            \
             _pTaskCtrl->ulTaskIndex, _sid ));                              \
                                                                            \
     while ( INTERLOCK( _plExclusive ) )                                    \
       {                                                                    \
       lTasCount--;                                                         \
                                                                            \
       if ( !lTasCount )                                                    \
         {                                                                  \
         MSGD (( ERR_INDEF_LOOP, _sid ));                                   \
         ABORT();                                                           \
         }                                                                  \
                                                                            \
       if ( kgs.fBusyWaitingForbidden || !( lTasCount % SLEEP_CNT )) \
         {                                                                  \
         RESCHEDULE ;                                                       \
         }                                                                  \
       else                                                                 \
         if ( !_fBusyWait )                                                 \
         {                                                                  \
         /* --- reschedule myself via rex queue  */                         \
         DLQ_ELEMENT* pSpecDLQElem2 = GetTaskDLQElement2( _pTaskCtrl );     \
         pSpecDLQElem2->ulReqType = REQ_RESCHEDULE;                         \
         sql74k_YieldTaskRunEnqu(*_ppUKT, _pTaskCtrl,                       \
                         pSpecDLQElem2, BASE_PRIO_REX );                    \
                                                                            \
         GOTO_DISP(_ppUKT);                                                 \
         }                                                                  \
       }                                                                    \
       *_plExclTasCnt += MAX_TAS_CNT - lTasCount + 1;                       \
     }                                                                      \
   }
#else
 #define WAIT_LOCK( _ppUKT, _pTaskCtrl, _pSemCtrl, _plExclusive, _sid,      \
                                       _plExclTasCnt, _fBusyWait )          \
   if ( INTERLOCK( _plExclusive ) )                                         \
     {                                                                      \
     __sql55k_wait_lock( _ppUKT, _pTaskCtrl, _pSemCtrl, _plExclusive,       \
                         _sid, _plExclTasCnt, _fBusyWait );                 \
     }
#endif

#define DO_WHEN_DUMMY_REGION(_RegId,_Action) \
        { if ( _RegId == -1 ) _Action ; }

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
VOID         sql55k_init_semaliste  ( PSEM_CTRL_REC   pSemCtrl );
VOID         sql55k_remote_vendexcl ( PPTASK_CTRL_REC ppCTask,
                                      PTASK_CTRL_REC  pFromTask,
                                      PPDLQ_REC       ppCurrReq,
                                      void*           pDummyParam );
// --- inline functions
_INLINE VOID __sql55k_wait_lock     ( PUKT_CTRL_REC   *ppUKT,
                                      PTASK_CTRL_REC  pTaskCtrl,
                                      PSEM_CTRL_REC   pSemCtrl,
                                      PLONG  plExclusive,
                                      tsp00_RegionId  sid ,
                                      ULONG           *_plExclTasCnt ,
                                      BOOLEAN         fBusyWait );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID  vsemaphid ( tsp00_Region    semaname,
                  tsp00_RegionId  *sid )
  {
  #undef  MF__
  #define MF__ MOD__"vsemaphid"
  ULONG                          ulSem;
  PSEM_CTRL_REC                  pSemCtrl;
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  DBG3 (( MF__, "[T:0x%03u] Region '%.8s'",
          pTaskCtrl->ulTaskIndex, semaname ));

  // --- Initialisation already done
  for ( ulSem = 0; ulSem < XPARAM(ulNoOfRegions); ulSem ++ )
    {
    if ( kgs.pSemaphores[ ulSem ].szSemaName[ 0 ] == '\0' )
      break;
    }

  if ( ulSem >= XPARAM(ulNoOfRegions) )
    {
    MSGD (( ERR_TOO_MANY_REGIONS_REQ ));
    DBG1 (( MF__, "[T:0x%03u] Wrong SID:%u", pTaskCtrl->ulTaskIndex, sid ));
    ABORT();
    }

  *sid                     = ulSem + 1;
  pSemCtrl                 = &kgs.pSemaphores[ ulSem ];
  pSemCtrl->pOwnerTaskCtrl = NULL;

  eo46PtoC ( pSemCtrl->szSemaName, semaname, sizeof(pSemCtrl->szSemaName) -1 );

  sql55k_init_semaliste ( pSemCtrl );

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*
 *=======================================================================
 */

VOID  vsemainfo ( tsp00_RegionId sid ,
                  tsp00_Region   semaname)

{
  if ( (ULONG ) sid <= XPARAM(ulNoOfRegions) )
    { eo46CtoP ( semaname,
                 kgs.pSemaphores[sid-1].szSemaName, sizeof(tsp00_Region) );
    }
}

/*------------------------------*/

BOOLEAN visexcl ( tsp00_TaskId    pid, 
                  tsp00_RegionId  sid )
  {
  #undef  MF__
  #define MF__ MOD__"visexcl"
  PSEM_CTRL_REC                  pSemCtrl;
  PTASK_CTRL_REC                 pTaskCtrl;
  PUKT_CTRL_REC                  pUKT;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  PID_TCB(pid,pTaskCtrl);
  pUKT = UKT(pTaskCtrl);

  DO_WHEN_DUMMY_REGION( sid , return(TRUE) );

  if ( sid == -1 )  // I guess that this point is never reached, as the case "sid == -1" is also handled by DO_WHEN_DUMMY_REGION (?)
    return ( pTaskCtrl->lNestingExcl != 0 ) ? TRUE : FALSE;

  if ( sid < 1 || sid > (tsp00_RegionId)XPARAM(ulNoOfRegions) )
     {
     MSGD (( ERR_WRONG_SEMAPHORE_IDX ));
     ABORT();
     }

  pSemCtrl = &kgs.pSemaphores[ sid - 1 ];

  if ( pSemCtrl->pOwnerTaskCtrl == pTaskCtrl )
    {
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return (TRUE);
    }

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return (FALSE);
  }
/*------------------------------*/
VOID vbegexcl ( tsp00_TaskId    pid, 
                tsp00_RegionId  sid  )
  {
  #undef  MF__
  #define MF__ MOD__"vbegexcl"
  PTASK_CTRL_REC                 pTaskCtrl;
  PUKT_CTRL_REC                  pUKT;

  PSEM_CTRL_REC                  pSemCtrl;
  PSEM_QUEUE_HEAD_REC            pSemQueHead;
  ULONG                          lReservedLoopCnt;
  BOOLEAN                        fRegionFree,fLoopEnd;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  PID_TCB(pid,pTaskCtrl);
  pUKT = UKT(pTaskCtrl);
#ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vbegexcl", pid ));
     ABORT();
     }

   if ( sid < 1 || sid > (tsp00_RegionId)XPARAM(ulNoOfRegions) )
     {
     MSGD (( ERR_WRONG_SEMAPHORE_IDX ));
     ABORT();
     }
  #endif

  DO_WHEN_DUMMY_REGION( sid , return );

  pTaskCtrl->TaskState       = TSK_VBEGEXCL_EO00;
  pTaskCtrl->Sid             = sid;
  pTaskCtrl->ulBegExclCalls++;
  pTaskCtrl->ulBegExclCnt++;
  pTaskCtrl->lNestingExcl++;

  pSemCtrl    = kgs.pSemaphores + sid - 1;
  pSemQueHead = &pSemCtrl->SemaList;
  pSemCtrl->lGetCount++;

  if ( pSemCtrl->pOwnerTaskCtrl == pTaskCtrl )
    {
    MSGD (( ERR_SEC_REGION_REQUEST, sid ));
    ABORT();
    }

  DBG3 (( MF__, "[T:0x%03u] Region '%s', sid: %d",
          pTaskCtrl->ulTaskIndex, pSemCtrl->szSemaName, sid ));



  if ( !XPARAM(fExclQueue) )    /* the no_semalist case */
    {
      do
        { fRegionFree = pSemCtrl->pOwnerTaskCtrl == NULL; /* dirty read */
          if ( fRegionFree )
            { WAIT_LOCK ( &pUKT, pTaskCtrl, pSemCtrl, &pSemQueHead->lExclusive,
                          sid , & pSemCtrl->lBegExclTasCount, NO_BUSY_WAIT);
              if ( !(fRegionFree = pSemCtrl->pOwnerTaskCtrl == NULL) )
                CLEARLOCK( &pSemQueHead->lExclusive );
            }
          if ( !fRegionFree )
            { DLQ_ELEMENT* pSpecDLQElem2 = GetTaskDLQElement2( pTaskCtrl );
              pSemCtrl->lCollisionCount++;
              pSpecDLQElem2->ulReqType = REQ_RESCHEDULE;
              sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pSpecDLQElem2, BASE_PRIO_REX  );
              GOTO_DISP(&pUKT);
            }
        }
      while ( !fRegionFree ) ;
    } 
  else /* the loop case (with and without dirty read)*/
  if ( XPARAM(ulRegionCollisionLoop) &&
       ( !pTaskCtrl->PrioFlag || !XPARAM(fBegExclQueWhenPrio) )) 
    {
    lReservedLoopCnt = XPARAM(ulRegionCollisionLoop);
    do
      {
      fLoopEnd    = lReservedLoopCnt <= 0 ||
                    ( pTaskCtrl->PrioFlag && XPARAM(fBegExclQueWhenPrio) ) ;
      fRegionFree = !XPARAM(fBegExclDirtyRead) ||
                    pSemCtrl->pOwnerTaskCtrl == NULL;   /* dirty read? */
      if ( fRegionFree || fLoopEnd )
        {
        WAIT_LOCK ( &pUKT, pTaskCtrl, pSemCtrl, &pSemQueHead->lExclusive,
                    sid , & pSemCtrl->lBegExclTasCount, NO_BUSY_WAIT);

        fRegionFree = pSemCtrl->pOwnerTaskCtrl == NULL;

        if ( fRegionFree )
          fLoopEnd = TRUE;
        else
          {
          if ( !fLoopEnd )  /* if the loop is to be left, the lock is still needed */
            CLEARLOCK( &pSemQueHead->lExclusive );
          }
        }
      if ( !fLoopEnd )
        {
        DLQ_ELEMENT* pSpecDLQElem2 = GetTaskDLQElement2( pTaskCtrl );
        lReservedLoopCnt--;

        pSpecDLQElem2->ulReqType = REQ_RESCHEDULE;
        sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pSpecDLQElem2, BASE_PRIO_REX  );

        GOTO_DISP(&pUKT);
        }
      }
    while ( !fLoopEnd );
    if (( (ULONG)lReservedLoopCnt != XPARAM(ulRegionCollisionLoop) ) &&
        ( !pSemCtrl->pOwnerTaskCtrl ))
      {
      // --- normaly 'vendexcl' detects collisions. In
      //     this situation it can't but we do
      pSemCtrl->lCollisionCount += XPARAM(ulRegionCollisionLoop) - lReservedLoopCnt;
      }
    }
  else  /* the no-loop case: */
    { WAIT_LOCK ( &pUKT, pTaskCtrl, pSemCtrl, &pSemQueHead->lExclusive, sid ,
                  &pSemCtrl->lBegExclTasCount, pTaskCtrl->PrioFlag );
    }
  /*
   * we are alone now
   */
  if ( !pSemCtrl->pOwnerTaskCtrl )
    {
    pSemCtrl->pOwnerTaskCtrl = pTaskCtrl;

    CLEARLOCK( &pSemQueHead->lExclusive );
    }
  else
    {
    pSemQueHead->pInsertPos->pTaskCtrl  = pTaskCtrl;
    pSemQueHead->pInsertPos             = pSemQueHead->pInsertPos->pNextElem;

    CLEARLOCK( &pSemQueHead->lExclusive );
    pSemCtrl->lWaitCount++;
    pUKT->ulNumRegWait++;

    #ifdef DEVELOPMENT
     pTaskCtrl->ulWaitTime = kgs.ulCurrTime;
    #endif

    GOTO_DISP(&pUKT);
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/
VOID vendexcl ( tsp00_TaskId    pid, 
                tsp00_RegionId  sid )
  {
  #undef  MF__
  #define MF__ MOD__"vendexcl"
  PTASK_CTRL_REC                pTaskCtrl;
  PUKT_CTRL_REC                 pUKT;

  PSEM_CTRL_REC                  pSemCtrl;
  PTASK_CTRL_REC                 pNextTaskCtrl;
  PSEM_QUEUE_HEAD_REC            pSemQueHead;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  PID_TCB(pid,pTaskCtrl);
  pUKT = UKT(pTaskCtrl);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vendexcl", pid ));
     ABORT();
     }
  #endif

  DO_WHEN_DUMMY_REGION( sid , return );
  pTaskCtrl->TaskState = TSK_VENDEXCL_EO00;

  pSemCtrl    = kgs.pSemaphores + sid - 1;
  pSemQueHead = &pSemCtrl->SemaList;

  DBG3 (( MF__, "[T:0x%03u] Region '%s'",
          pTaskCtrl->ulTaskIndex, pSemCtrl->szSemaName ));

  if ( XPARAM(fExclQueue ) )    // no sema-list -> no need to acquire lock
    WAIT_LOCK ( &pUKT, pTaskCtrl, pSemCtrl, &pSemQueHead->lExclusive, sid ,
                &pSemCtrl->lEndExclTasCount, XPARAM(fEndExclBusyWait));

  /*
   * we are alone now
   */

   // --- check if i am the owner
  if (pSemCtrl->pOwnerTaskCtrl != pTaskCtrl)
    {
    if ( XPARAM(fExclQueue ) )  // as the lock is not acquired in the no sema-list case, it must not be released
      CLEARLOCK( &pSemQueHead->lExclusive );
    MSGD (( ERR_ENDEXCL_WITHOUT_BEG, pTaskCtrl->ulTaskIndex ));
    ABORT();
    }

  #ifdef DEVELOPMENT
   pTaskCtrl->ulWaitTime = 0;
  #endif

  pTaskCtrl->lNestingExcl--;

  pNextTaskCtrl = pSemQueHead->pRemovePos->pTaskCtrl;

  if ( pNextTaskCtrl )
    {
    //
    // --- somebody else is waiting for 'bendexcl'
    //
    pSemCtrl->pOwnerTaskCtrl            = pNextTaskCtrl;
    /* remove from the wait queue */
    pSemQueHead->pRemovePos->pTaskCtrl  = NULL;
    pSemQueHead->pRemovePos             = pSemQueHead->pRemovePos->pNextElem;

    CLEARLOCK( &pSemQueHead->lExclusive );
    pSemCtrl->lCollisionCount++;

    // ---  is the next tasks UKT eq. to the local UKT
    if ( pNextTaskCtrl->pUKT == pUKT )
      {
      DLQ_ELEMENT* pSpecDLQElem1;

      pUKT->ulNumRegWait--;
      DBG4 (( MF__, "[T:0x%03u] vendexcl request for local task [T:0x%03u]",
              pTaskCtrl->ulTaskIndex, pNextTaskCtrl->ulTaskIndex ));

      pSpecDLQElem1            = GetTaskDLQElement1( pNextTaskCtrl );
      pSpecDLQElem1->ulReqType = REQ_VENDEXCL;
      sql74k_RUN_enqu ( pUKT, pNextTaskCtrl, pTaskCtrl, pSpecDLQElem1, BASE_PRIO_RAV );

      // --- reschedule myself via rex queue
      pSpecDLQElem1            = GetTaskDLQElement1( pTaskCtrl );
      pSpecDLQElem1->ulReqType = REQ_RESCHEDULE;
      sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pSpecDLQElem1, BASE_PRIO_REX );

      GOTO_DISP(&pUKT);
      }
    else
      {
      DBG4 (( MF__, "[T:0x%03u] vendexcl request for remote task [T:0x%03u]",
              pTaskCtrl->ulTaskIndex, pNextTaskCtrl->ulTaskIndex ));

      sql74k_UToU_enqu_and_wake ( pUKT, pTaskCtrl, pNextTaskCtrl->pUKT,
                                  pNextTaskCtrl, sql55k_remote_vendexcl, 0 );

      if ( kgs.fBusyWaitingForbidden )
         {
         RESCHEDULE ;
         }
      }
    }
  else
    {
    /*
     * Queue is empty
     */
    pSemCtrl->pOwnerTaskCtrl = NULL;

    if ( XPARAM(fExclQueue) )   // as the lock is not acquired in the no sema-list case, it must be released
      CLEARLOCK( &pSemQueHead->lExclusive );

    if (( pTaskCtrl->ulBegExclCnt > XPARAM(ulRegLockSlice) ||
          pTaskCtrl->pUKT->ulImmediateDispatch ) &&
          !pTaskCtrl->lNestingExcl && !pTaskCtrl->PrioFlag )
      {
      DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( pTaskCtrl );

      // --- reschedule myself via rex queue
      if ( pTaskCtrl->pUKT->ulImmediateDispatch ) pTaskCtrl->pUKT->ulImmediateDispatch--;
      pSpecDLQElem1->ulReqType = REQ_RESCHEDULE;
      sql74k_YieldTaskRunEnqu( pUKT, pTaskCtrl, pSpecDLQElem1, BASE_PRIO_REX  );
      pTaskCtrl->ulSelfSuspCount++;
      pTaskCtrl->TaskState = TSK_RUNNABLE_EO00;

      GOTO_DISP(&pUKT);
      }
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;
  pTaskCtrl->Sid = 0;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }
/*------------------------------*/

VOID sql55k_free_regions ( PUKT_CTRL_REC   pUKT )
  {
  #undef  MF__
  #define MF__ MOD__"sql55k_free_regions"
  ULONG                          ulSem;
  PSEM_CTRL_REC                  pSemCtrl;
  PTASK_CTRL_REC                 pOwnerTaskCtrl;
  PSEM_QUEUE_REC                 pSemQueElem;

  DBGPAS;

  for ( ulSem = 0; ulSem < XPARAM(ulNoOfRegions); ulSem ++ )
    {
    pSemCtrl = kgs.pSemaphores + ulSem;


    WAIT_UNTIL_ALONE( &pSemCtrl->SemaList.lExclusive );

    /*
     * we are alone now
     */
    // --- save current owner
    pOwnerTaskCtrl = pSemCtrl->pOwnerTaskCtrl;

    // --- continue
    //      - if there is no owner
    //      - if i am not the owner and the owner isn't dead
    //
    if (( pOwnerTaskCtrl == NULL ) || (( pOwnerTaskCtrl->pUKT != pUKT ) &&
        ( pOwnerTaskCtrl->pUKT->ThrdCtrlHeader.ThrdState != UKT_TERMINATED )))
      {
      CLEARLOCK( &pSemCtrl->SemaList.lExclusive );
      continue;
      }

    // --- is the current owner not the tracewriter?
    if ( pOwnerTaskCtrl != kgs.pTW )
      {
      pSemCtrl->pOwnerTaskCtrl->TaskState = TSK_LOCKED_EO00;
      pSemCtrl->pOwnerTaskCtrl            = NULL;

      // --- try to make tracewriter to new owner
      for ( pSemQueElem =  pSemCtrl->SemaList.pRemovePos;
            pSemQueElem->pTaskCtrl;
            pSemQueElem =  pSemQueElem->pNextElem )
        {
        if ( pSemQueElem->pTaskCtrl == kgs.pTW )
          {
          // -- tracewriter is the new owner
          pSemCtrl->pOwnerTaskCtrl      = pSemQueElem->pTaskCtrl;
          pSemQueElem->pTaskCtrl        = NULL;
          pSemQueElem                   = pSemQueElem->pNextElem;
          }
        else
          {
          pSemQueElem->pTaskCtrl->TaskState = TSK_LOCKED_EO00;
          pSemQueElem->pTaskCtrl            = NULL;
          }
        }
      pSemCtrl->SemaList.pRemovePos = pSemQueElem;
      pSemCtrl->SemaList.pInsertPos = pSemQueElem;
      }

    // --- owner changed?
    if ( pOwnerTaskCtrl != pSemCtrl->pOwnerTaskCtrl )
      pOwnerTaskCtrl = pSemCtrl->pOwnerTaskCtrl;
    else
      pOwnerTaskCtrl = NULL;

    CLEARLOCK( &pSemCtrl->SemaList.lExclusive );


    // --- is there a new owner
    if ( pOwnerTaskCtrl )
      {
      // --- the enqueue functions and the following output
      //     needs the current-task-value for DEBUG purpose!
      if (pUKT->pCTask) pUKT->pCTask = pOwnerTaskCtrl;

      DBG4 (( MF__, "[T:0x%03u] Regions freed, wake up TW",
              pOwnerTaskCtrl->ulTaskIndex ));

      if ( pOwnerTaskCtrl->pUKT == pUKT )
        {
        // --- 'vendexcl' via rav queue
        DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( pOwnerTaskCtrl );
        pSpecDLQElem1->ulReqType = REQ_VENDEXCL;
        sql74k_RUN_enqu ( pUKT, pOwnerTaskCtrl, pUKT->pCTask, pSpecDLQElem1, BASE_PRIO_RAV );
        }
      else
        {
        sql74k_UToU_enqu_and_wake ( pUKT, pUKT->pCTask, pOwnerTaskCtrl->pUKT,
                                    pOwnerTaskCtrl, sql55k_remote_vendexcl, 0 );
        }
      }
    }
  return;
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static VOID  sql55k_init_semaliste ( PSEM_CTRL_REC  pSemCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql55k_init_semaliste"
  INT                            i;
  PSEM_QUEUE_REC                 pSemQueElem1;
  PSEM_QUEUE_REC                 pSemQueElem2;
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pSemQueElem1 = &pSemCtrl->SemaList.pSEMs[0];

  for ( i = 0; (ULONG)i < kgs.ulNumOfTasks; i ++ )
    {
    pSemQueElem1->pTaskCtrl = NULL;
    pSemQueElem2            = pSemQueElem1 + 1;
    pSemQueElem1->pNextElem = pSemQueElem2;
    pSemQueElem1            = pSemQueElem2;
    }

  pSemQueElem1--;
  pSemQueElem2            = &pSemCtrl->SemaList.pSEMs[0];
  pSemQueElem1->pNextElem = pSemQueElem2;

  pSemCtrl->SemaList.pInsertPos = pSemQueElem2;
  pSemCtrl->SemaList.pRemovePos = pSemQueElem2;
  pSemCtrl->SemaList.lExclusive = 0;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

#if defined (OS2)   // -- ignore unused parameter  'ulDummyParam'
# pragma info (nopar)
#endif

VOID sql55k_remote_vendexcl ( PPTASK_CTRL_REC ppCTask,
                              PTASK_CTRL_REC  pFromTask,
                              PPDLQ_REC       ppCurrReq,
                              void*           pDummyParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql55k_remote_vendexcl"

  DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( *ppCTask );

  DBGPAS_T ((*ppCTask)->ulTaskIndex);

  (*ppCTask)->pUKT->ulNumRegWait--;
  *ppCurrReq               = pSpecDLQElem1;
  pSpecDLQElem1->ulReqType = REQ_VENDEXCL;

  return;
  }

#if defined (OS2)
# pragma info (restore)
#endif

/*------------------------------*/

#if !defined (USE_HUGE_MACROS)
_INLINE VOID __sql55k_wait_lock ( PUKT_CTRL_REC   *ppUKT,
                                  PTASK_CTRL_REC  pTaskCtrl,
                                  PSEM_CTRL_REC   pSemCtrl,
                                  PLONG           plExclusive,
                                  tsp00_RegionId  sid ,
                                  ULONG           *plExclTasCnt,
                                  BOOLEAN         fBusyWait )
  {
  #undef  MF__
  #define MF__ MOD__"__sql55k_wait_lock"
  LONG                     lTasCount;

  DBGPAS_T (pTaskCtrl->ulTaskIndex);


  lTasCount = MAX_TAS_CNT;

  DBG4 (( MF__, "[T:0x%03u] Waiting for region interlock %u",
          pTaskCtrl->ulTaskIndex, sid ));

  while ( INTERLOCK( plExclusive ) )
    {
    lTasCount--;

    if ( !lTasCount )
      {
      MSGD (( ERR_INDEF_LOOP, sid ));
      ABORT();
      }

    if ( kgs.fBusyWaitingForbidden || !( lTasCount % SLEEP_CNT )) 
      {
      RESCHEDULE ;
      }
    else
      if ( !fBusyWait )
      {
      /* --- reschedule myself via rex queue  */
      DLQ_ELEMENT* pSpecDLQElem2 = GetTaskDLQElement2( pTaskCtrl );
      pSpecDLQElem2->ulReqType = REQ_RESCHEDULE;
      sql74k_YieldTaskRunEnqu(*ppUKT, pTaskCtrl, pSpecDLQElem2, BASE_PRIO_REX);
      GOTO_DISP(ppUKT);
      }
    }
  *plExclTasCnt += MAX_TAS_CNT - lTasCount + 1;

  return;
  }
#endif

/*
 * =============================== END ========================================
 */
