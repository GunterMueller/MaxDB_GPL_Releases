/*!
  @file           ven71.cpp
  @author         JoergM
  @brief          Kernel RunTime: Dispatcher and Queue Management
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

#define MOD__  "ven71.c:"

#include "gen00.h"
#include "heo00.h"
#include "geo002.h"
#include "heo52.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "gen71.h"
#include "hen71.h"
#include "hen40.h"
#include "geo00_2.h"
#include "gen72.h"
#include "gen73.h"
#include "gen75.h"
#include "gen81.h"
#include "gen55.h"
#include "hen56.h"
#include "hen50.h"
#include "hsp77.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "Trace/Trace_Manager.h"
#include "RunTime/System/RTESys_MemoryBarrier.h"


/*
 *  DEFINES
 */
#define ALLOW_TASK_DEBUG_REQUESTS

#define INFINIT_TIME_VAL         (SAPDB_UInt8)-1

#define  ERR_INTERNAL_MICRO_TIMER_FAILED              N(903), ERR_TYPE,_T("TASKING "),_T("Dispatcher Timing wrong %s %qu > %s %qu")

#define DISPATCHER_LOOPS    \
        (this_ukt->NumRegWait && (XPARAM(ulRegionCollisionLoop) > \
                                 XPARAM(ulDispLoopsBefSemop))) \
        ? XPARAM(ulRegionCollisionLoop) : XPARAM(ulDispLoopsBefSemop) ;

/*
 *  At system termination, ignore all requests except for bufwriter.
 */
#define IGNORE_REQUEST_AT_SYSTEM_END() \
    if ( (KGS->state >= SERVER_SHUTDOWNREINIT) &&\
         (this_ukt->curr_task->type != TT_TW_EO00) ) continue ;


#if defined(ALPHA) || defined(I386)
extern  void                 *e76_mem_sync( struct TASK_TYPE *pTask );
#define MEM_SYNC( task_ )      e76_mem_sync( task_ )
#else
#define MEM_SYNC( task_ )      task_
#endif

#define FIND_ALL_TASKS        1
#define FIND_PRIO_TASKS       2

/* local functions */

static  void       en71_freeUktOwnedRegions (ten50_UKT_Control * this_ukt);
static  void       en71_shutdownKill (ten50_UKT_Control * this_ukt);
static SAPDB_Bool  en71TaskMoveCheck( ten50_UKT_Control**   srcUKT,
                                      ten50_UKT_Control**   dstUKT );
static void        en71MoveTask ( struct TASK_TYPE*       tcb,
                                  struct DOUBLY_LINKED*   request );
static void        en71SendMoveRequest ( ten50_UKT_Control*   srcUKT,
                                         ten50_UKT_Control*   dstUKT,
                                         SAPDB_UInt8          stopWaitForBestFitTaskAt );
static void        en71SendMoveMeassIntRequest();
static void        en71_StopUkt(SAPDB_Int4 uktIndex);

static void        en71_EnquRun ( struct ten50_UKT_Control * pUKT,
                                  struct TASK_TYPE         * pTask,
                                  struct TASK_TYPE         * pFromTask,
                                  struct DOUBLY_LINKED     * pReq, 
                                  SAPDB_UInt4                basePrio,
                                  SAPDB_Bool                 taskYield );


#ifdef DEBUG_SLEEP_STATE
static  void       en71_debugSleepState (ten50_UKT_Control *this_ukt);
#endif

#ifndef SAPDB_NO_SPINLOCK_FOR_UKT2UKT
static  int        en71_xxxDequeue( struct XXX_QUEUE_HEAD * queueHead, struct XXX_QUEUE * copyOfItemToDequeue );
#endif

#ifdef DEB_RUNQ
#include "gen38.h" /* nocheck */
#define PRINT_ENQU(_Task, _FromTask, _Req)                                        \
{         if ( _FromTask )                                                        \
            {                                                                     \
                DBG1 (( MF__,"T%d added from T%d(%s) to r_que, len %d\n",         \
                  _Task->index,                                                   \
                  this_ukt->RunQueue[this_ukt->RunQueueLen].FromTask->index ,     \
                  e38_req_name ( _Req->req_type ),                                \
                  this_ukt->RunQueueLen+1 ));                                     \
            } else {                                                              \
                DBG1 (( MF__,"T%d added from T? to r_que, len %d\n",              \
                    _Task->index, this_ukt->RunQueueLen+1 ));                     \
            }                                                                     \
}

#define PRINT_RUN_TASK                                                            \
        if ( this_ukt->curr_task->ukt != this_ukt )                               \
          {                                                                       \
            DBG1 (( MF__,"T%d NOT in this UKP!\n", this_ukt->curr_task->index));  \
            vabort ( WRITE_CORE ) ;                                               \
          }                                                                       \
        DBG1 (( MF__,"RUN T%3d (%d) QueueLen: %d\n", this_ukt->curr_task->index,  \
                  this_ukt->curr_task->ulDynPrio, this_ukt->RunQueueLen )) ;
#else
#define PRINT_ENQU 
#define PRINT_RUN_TASK                                                            \
        if ( this_ukt->curr_task->ukt != this_ukt )                               \
          {                                                                       \
            DBG1 (( MF__,"T%d NOT in this UKP!\n", this_ukt->curr_task->index));  \
            vabort ( WRITE_CORE ) ;                                               \
          } 
#endif

/* imported data */

extern  int                             e60_dbgdbglvl ;

static void en71_DoEnterDispatcherTiming( ten50_UKT_Control *this_ukt,
                                          struct TASK_TYPE  *pTaskCtrl );

static void en71_DoLeaveDispatcherTiming( ten50_UKT_Control *this_ukt,
                                          struct TASK_TYPE  *pTaskCtrl );

static void en71_HandleUKTStatisticReset( ten50_UKT_Control *this_ukt );


/* local data */

/*
 * ===========================================================================
 */

void    en71Dispatcher ( ten50_UKT_Control *this_ukt)
{
#  undef  MF__
#  define MF__ MOD__"en71_dispatcher"

  this_ukt->pTaskSchedulerObject->NextScheduledTask();
}

/*---------------------------------------------------------------------------*/

void en71_ResetTaskStatisticRecords( ten50_UKT_Control *this_ukt
                                   , struct TASK_TYPE   *pTaskCtrl )
{
    teo52_TaskStateStatisticRec &taskStat = pTaskCtrl->TaskStateStat;

    WAIT_UNTIL_ALONE( this_ukt->exclusive );

        eo52ResetStatisticRecord(taskStat);

        if ( EO52_RESET_TASK_FULL_PENDING == pTaskCtrl->TaskStatRecResetPending )
        {
            eo52ResetStatisticRecord(pTaskCtrl->QueStat);
            pTaskCtrl->QueueStatRecResetPending = false;
            eo52ResetStatisticRecord(pTaskCtrl->PrioStat);
            pTaskCtrl->disp_counter = 1;
            if ( 0 == pTaskCtrl->Totaldisp_counter )
            {
                pTaskCtrl->Totaldisp_counter = 1;
            }
            pTaskCtrl->self_susp_cnt = 0;
        }

        pTaskCtrl->TaskStatRecResetPending = EO52_NO_RESET_PENDING;

    CLEARLOCK( this_ukt->exclusive );

}

/*---------------------------------------------------------------------------*/

RTETask_Task&  RTETask_TaskScheduler::NextScheduledTask()
{
    int                 rc;
    ULONG              loop_count;
    time_t             new_timeout;
    ten50_UKT_Control  *this_ukt            = m_LegacyUKTCtrl;
    struct TASK_TYPE   *pTaskCtrl           = this_ukt->curr_task;
    struct TASK_TYPE   *lastRunningTaskCtrl = pTaskCtrl;
    SAPDB_UInt8        timeNow;

#ifdef DEBUG_RTE
    char eBuffer[256];

    sp77sprintf(eBuffer, sizeof(eBuffer), "ENTER en71Dispatcher UKT%d curr_task T%d", 
                this_ukt->index, 
                (this_ukt->curr_task ? this_ukt->curr_task->index : 0) );
    MSGD(( INFO_TEST_WILDCARD, &eBuffer[0] ));
#endif

    m_IsWaitingForNextScheduledTask = true;  /* signal that we are waiting for the next task */

    en71_HandleUKTStatisticReset(this_ukt);


    /* PTS 1110829 */
    if ( 0 != pTaskCtrl )
    {
        DBGIN_T(pTaskCtrl->index);

        pTaskCtrl->Totaldisp_counter++;
        pTaskCtrl->disp_counter++;

        if ( 0 == pTaskCtrl->delayTimeMeasurement ) // no time measurement if task is yielding
            en71_DoEnterDispatcherTiming(this_ukt, pTaskCtrl);

        pTaskCtrl->pCurrStackPointer = (SAPDB_UInt4 *)&rc;

        /* PTS 1110953 */
        if ( pTaskCtrl->maxStackPointer < (SAPDB_UInt4 *)&rc )
        {
            pTaskCtrl->maxStackPointer = (SAPDB_UInt4 *)&rc;
        }
    }
    else
    {
        DBGIN_T(0);
    }

    this_ukt->Totaldisp_counter ++ ;
    this_ukt->disp_counter ++ ;
    loop_count = DISPATCHER_LOOPS ;

    for ( ; ; )
    {
        if ( loop_count > 0 )
        {
            --loop_count;
        }

        if ( KGS->state >= SERVER_SHUTDOWNREINIT ) 
        {
            MSGD (( INFO_DISP_SHUTKILL_STATE1, KGS->state ));
            en71_shutdownKill (this_ukt);
        }

    /*
     *  Copy the debuglevel from the shared segment to the data segment.
     *  The debuglevel could be changed by the console driver utility.
     */
        e60_dbgdbglvl = KGS->debuglevel ;

        /******************************************************************/
        /*  the xxx queues are used for requests from other UKP's         */
        /*  and are as such only of interest when more than one UKP       */
        /*  is active                                                     */
        /******************************************************************/
        {
            if ( KGS->ulNumOfUKTs > 1 )
            {
                struct XXX_QUEUE_HEAD           * lxx ;
                struct XXX_QUEUE                * lxxx ;
                int                               ix ; 
                int                               EntryFound ;
#ifndef SAPDB_NO_SPINLOCK_FOR_UKT2UKT
                struct XXX_QUEUE                  activeRequest ;
#endif
                DBG1 (( MF__,"searching in XXX-Queue \n" ));
                /*
                 *  Step through the columns of the matrix.
                 */
                for ( ix = KGS->ulNumOfUKTs - 1 ; ix >= 0 ; ix -- )
                {
                    do 
                    {
                      lxx = this_ukt->xxx[ix];
                      if   ( ! lxx ) break ;

#ifndef SAPDB_NO_SPINLOCK_FOR_UKT2UKT

                      EntryFound = en71_xxxDequeue(lxx, &activeRequest);
                      if ( !EntryFound )
                      {
                          break; /* next XXX queue */
                      }

                      lxxx = &activeRequest;
#ifdef SAPDB_SLOW
/* SAPDB_SLOW */          if ( ! lxxx->FromTask )
/* SAPDB_SLOW */          {
/* SAPDB_SLOW */              MSGD(( INFO_REMOTEFUNC_NOT_SET, 0, lxxx->func_param )) ;
/* SAPDB_SLOW */              break;
/* SAPDB_SLOW */          }
/* SAPDB_SLOW */
/* SAPDB_SLOW */          if ( lxxx->task->ukt != this_ukt )
/* SAPDB_SLOW */          {
/* SAPDB_SLOW */              MSGD(( INFO_REMOTEFUNC_NOT_SET, -1, lxxx->func_param )) ;
/* SAPDB_SLOW */              break;
/* SAPDB_SLOW */          }
/* SAPDB_SLOW */
/* SAPDB_SLOW */          if ( ! lxxx->to_execute )
/* SAPDB_SLOW */          {
/* SAPDB_SLOW */              MSGD(( INFO_REMOTEFUNC_NOT_SET, lxxx->FromTask->index, lxxx->func_param )) ;
/* SAPDB_SLOW */              break;
/* SAPDB_SLOW */          }
#endif /* SAPDB_SLOW */

                      IGNORE_REQUEST_AT_SYSTEM_END() ;

                      if( this_ukt != lxxx->task->ukt )
                      {
                          /* The load balancing has moved the task to another UKT during
                             'Ukt2Ukt' enqueue operation. Now we forward this request.. */
                          en71EnqueueUkt2UktAndWake ( this_ukt , lxxx->task, lxxx->FromTask, 
                                                      lxxx->to_execute, lxxx->func_param );
                          this_ukt->curr_task = NULL;
                      }
                      else
                      {
                        /* this_ukt->curr_task may be reset to 0 by the call... */
                        this_ukt->curr_task = lxxx->task;
                        lxxx->to_execute ( lxxx->func_param, lxxx->FromTask );
                      }

#else

                      /* Code without using spinlocks for XXX queue */
                      EntryFound = FALSE ;
                      /*
                       *  lxxx is a pointer to the first queue element.
                       */
                      lxxx = lxx->xx_work ;

                      if ( ( this_ukt->curr_task = MEM_SYNC ( lxxx->task ) ) != 0 )
                      {
                        int fLoopEnd ;
                        int LoopCnt = 0 ;
                        PFNREMOTEFUNC pSvFuncToExecute ;
                        /*
                         *  Dequeue the request
                         */
                        IGNORE_REQUEST_AT_SYSTEM_END() ;

                        if( this_ukt != this_ukt->curr_task->ukt )
                        {
                            en71EnqueueUkt2UktAndWake ( this_ukt , tcb , lxxx->FromTask, 
                                                        lxxx->to_execute, lxxx->func_param );
                            this_ukt->curr_task = NULL;
                        }
                        else
                        {
                            /*
                            *  Execute the request
                            */
                            /* PTS 1106134 */
                            do
                            {
                                if ( (pSvFuncToExecute = lxxx->to_execute) == NULL )
                                {
                                    MSGD(( INFO_REMOTEFUNC_NOT_SET, lxxx->FromTask->index, lxxx->func_param )) ;
                                    fLoopEnd = ++LoopCnt == 10 ;
                                    sqlyieldthread();
                                }
                                else
                                {
                                    pSvFuncToExecute = lxxx->to_execute;
                                    fLoopEnd = TRUE ;
                                }
                            }
                            while ( !fLoopEnd ) ;

                            pSvFuncToExecute ( lxxx->func_param, lxxx->FromTask );
                            lxxx->task = 0 ;
                            lxxx->to_execute = NULL; /* PTS 1000746 */
                            lxx->xx_work = lxxx->next ;
                            EntryFound = TRUE  ;
                        }

#endif /* SAPDB_NO_SPINLOCK_FOR_UKT2UKT */

                        /*
                         *  On behalf of the request a task could become runnable
                         *  otherwise this_ukt->curr_task is reset to 0
                         */
                        if ( this_ukt->curr_task )
                        {
                            DBG1 (( MF__,"XXX task    T%d  request %d \n", 
                                    this_ukt->curr_task->index , 
                                    GetTaskDLQElement1(this_ukt->curr_task)->req_type ));
                            en71_EnquRun(this_ukt, this_ukt->curr_task, lxxx->FromTask, 
                                    GetTaskDLQElement1(this_ukt->curr_task), BASE_PRIO_U2U, false ) ;
                        }

#ifdef SAPDB_NO_SPINLOCK_FOR_UKT2UKT
                      } /* if ( lxxx->task != 0 ) */
#endif /* SAPDB_NO_SPINLOCK_FOR_UKT2UKT */

                    } while ( EntryFound ) ;
                } /* for */             
            } /* if ulNumOfUKTs > 1 */
        } /* XXX queue */


        /******************************************************************/
        /* search AIOC queue, add elemets to RUN_QUEU                     */
        /******************************************************************/
        while ( (this_ukt->curr_quu = this_ukt->aioc_queue.last->request) )
        {
            DBG1 (( MF__,"queue: %s, last->request = 0x%x\n", "AIOC", 
                                 this_ukt->aioc_queue.last->request )) ;
            this_ukt->aioc_queue.last->request = 0 ;                   
            this_ukt->aioc_queue.last    = this_ukt->aioc_queue.last->next; 
            IGNORE_REQUEST_AT_SYSTEM_END() ;

            DBG1 (( MF__,"queue: %s, task    T%d  request %d\n",
                    "AIOC", this_ukt->curr_quu->taskid ->index , 
                    this_ukt->curr_quu->req_type ));

            if ( this_ukt->curr_quu->req_type == REQ_ASYNIO )
            {
                this_ukt->curr_task = this_ukt->curr_quu->taskid ;

                DBG1 (( MF__,"REQ_ASYNIO, this_ukt 0x%x, quu->ukt 0x%x \n",
                                this_ukt , this_ukt->curr_task->ukt ));

                this_ukt->curr_quu->args.vai_parms.finished = TRUE ;
                /* task searches for finished io req. on call of 'vasynwait' */
                if ( this_ukt->curr_task->state != TSK_ASYNWAIT_READ_EO00 &&
                     this_ukt->curr_task->state != TSK_ASYNWAIT_WRITE_EO00 ) 
                {
                    DBG1 (( MF__,"REQ_ASYNIO continue, state is %s\n", 
                              e38_tskstate_name(this_ukt->curr_task->state) ));
                    continue ;
                }
            }
            en71_EnquRun(this_ukt, this_ukt->curr_quu->taskid, NULL, 
                         this_ukt->curr_quu, BASE_PRIO_IOC, false); 
        }

        /******************************************************************/
        /* search IOC queue, add elemets to RUN_QUEU                      */
        /******************************************************************/
#ifdef  DEBUG_RTE
        DBG1 (( MF__,"searching in IOC-Queue \n" ));

        WAIT_UNTIL_ALONE ( this_ukt->ioc_queue.exclusive );
        {
            struct IOC_QUEUE *ioptr ;
            for ( ioptr = this_ukt->ioc_queue.last ; 
                    ioptr->request != NULL; 
                        ioptr = ioptr->next )
            {
                DBG1 (( MF__,"IOC-QUEUE 0x%lx 0x%lx\n",
                    (long) ioptr , (long) ioptr->request ));
                if ( ioptr->next == this_ukt->ioc_queue.last
                  || ioptr->next == ioptr )
                {
                    break;
                }
            }
        }
        CLEARLOCK ( this_ukt->ioc_queue.exclusive );
#endif /*DEBUG_RTE*/

        
        while ( 0 != ( this_ukt->curr_quu = this_ukt->ioc_queue.last->request ) )
        {
            RTESys_ReadMemoryBarrier();

            /* dequeue the request */
            this_ukt->ioc_queue.last->request = 0 ;
            this_ukt->ioc_queue.last = this_ukt->ioc_queue.last->next ;
            this_ukt->curr_task      = this_ukt->curr_quu->taskid ;


            if ( this_ukt->curr_task) 
            { /* since REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00 and REQ_MOVE_TASK_EO00 no task might be specified */
                this_ukt->curr_task->is_in_queue = TASK_IS_IN_NON_QUEUE_EO00 ;
                IGNORE_REQUEST_AT_SYSTEM_END() ;

                /* The load balancing has moved the task to another UKT during
                   'ioc' enqueue operation. Now we forward this request.. */
                if( this_ukt != this_ukt->curr_task->ukt )
                {
                    en71_iocEnquAndWake ( this_ukt->curr_task->ukt, this_ukt->curr_task, 
                                          this_ukt->curr_quu );
                    this_ukt->curr_task = NULL;
                    continue;
                }
            }

            /*
             *  INSERT_MOVING_TASK requests
             */
            if (this_ukt->curr_quu->req_type == REQ_INSERT_MOVING_TASK_E000)
            {
                struct  TASK_TYPE * tmp_tcb;

                MSGD(( INFO_TASK_MOVE_REQUEST_REC, this_ukt->curr_task->index ));

                /* integrate task into UKTs task chain! */
                tmp_tcb = this_ukt->pFirstTaskCtrl;

                if ( tmp_tcb ) tmp_tcb->prev_task   = this_ukt->curr_task;

                this_ukt->pFirstTaskCtrl       = this_ukt->curr_task;
                this_ukt->curr_task->next_task = tmp_tcb;
                this_ukt->curr_task->prev_task = NULL;

                if (this_ukt->curr_task->prio_flag)  this_ukt->SelfDispatch++;

                en71AlterTaskCnt( this_ukt->curr_task, ADD_ALL_COUNTER );

                this_ukt->curr_quu->req_type     = this_ukt->curr_quu->req_type_org;
                this_ukt->curr_quu->req_type_org = 0;
            }

            /*
             *  NEW_MOVE_TASK_MEAS_INTERVAL requests
             */
            if (this_ukt->curr_quu->req_type == REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00)
            {   
                tsp00_Int4                           idx;
                struct REQ_PARMS_MOVE_TASK_MESS_INT* reqArg;
                SAPDB_UInt8                          runnableMicroSeconds;
                struct TASK_TYPE*                    pTaskCtrl;
                SAPDB_UInt8                          timeNow = RTESys_MicroSecTimer();
                tsp00_Int4                           runQueueLen = this_ukt->RunQueueLen;

                for ( idx = 0; idx < runQueueLen; idx++ )
                {
                    pTaskCtrl = this_ukt->RunQueue[idx].Task;

                    if (( pTaskCtrl->moveLock.type.longTerm != 0 ) ||
                        ( pTaskCtrl->TaskStateStat.runqueueEnqueueTime == 0 ))
                        continue;

                    runnableMicroSeconds = timeNow - pTaskCtrl->TaskStateStat.runqueueEnqueueTime;

                    /* accumulate the tasks time waititng in the run queue to be scheduled */
                    pTaskCtrl->runnableMicroSeconds[this_ukt->currTimeVar] += runnableMicroSeconds;

                    /* Has the tasks a new runnable time maximum? */
                    if ( pTaskCtrl->maxRunnableMicroSeconds[this_ukt->currTimeVar] < runnableMicroSeconds )
                        pTaskCtrl->maxRunnableMicroSeconds[this_ukt->currTimeVar] = runnableMicroSeconds;

                    /* set the UKTs runnable time values */
                    this_ukt->runnableMicroSeconds[this_ukt->currTimeVar] += runnableMicroSeconds;

                    if ( this_ukt->maxRunnableMicroSeconds[this_ukt->currTimeVar] < runnableMicroSeconds )
                        this_ukt->maxRunnableMicroSeconds[this_ukt->currTimeVar] = runnableMicroSeconds;
                }

                reqArg = &this_ukt->curr_quu->args.mtmi_parms;
                /* skip old requests */
                if ( reqArg->taskMoveIntervalStartTime == KGS->taskMoveIntervalStartTimes[KGS->currTimeVar])
                {
                    this_ukt->currTimeVar                                     = reqArg->timeVar;
                    this_ukt->taskMoveIntervalStartTime                       = reqArg->taskMoveIntervalStartTime;
                    this_ukt->UKTSleepTimeMicroSeconds[this_ukt->currTimeVar] = 0;
                    this_ukt->runnableMicroSeconds[this_ukt->currTimeVar]     = 0;
                    this_ukt->maxRunnableMicroSeconds[this_ukt->currTimeVar]  = 0;
                    this_ukt->moveTaskToUKT                                   = NULL; /* drop old move task requests */
                }
                /* free the queue element which woke us */
                RELEASE_FREELIST_ELEM( this_ukt->curr_quu );
                this_ukt->curr_task = 0 ;
                continue ;
            }

            if ( this_ukt->curr_quu->req_type == REQ_CANCEL_TASK_EO00 )
            {
                RTETask_Task*  pTask = this_ukt->curr_task->pTaskObject;

                if ( false == pTask->PrepareEnqueueOfCancelRequest() )
                {
                    /* free the queue element which woke us */
                    RELEASE_FREELIST_ELEM( this_ukt->curr_quu );
                    this_ukt->curr_task = 0 ;
                    continue ;
                }
            }

            if ( this_ukt->curr_quu->req_type == REQ_REQUEST_TIMED_OUT_EO00 )
            {
                RTETask_Task*  pTask = this_ukt->curr_task->pTaskObject;

                if ( false == pTask->PrepareEnqueueOfTimeoutRequest() )
                {
                    /* free the queue element which woke us */
                    RELEASE_FREELIST_ELEM( this_ukt->curr_quu );
                    this_ukt->curr_task = 0 ;
                    continue ;
                }
            }


            /*
             *  MOVE_TASK requests
             */
            if (this_ukt->curr_quu->req_type == REQ_MOVE_TASK_EO00)
            {   
                struct REQ_PARMS_MOVE_TASK *reqArg;

                reqArg = &this_ukt->curr_quu->args.mt_parms;
                /*  skip old requests */
                if ( reqArg->taskMoveIntervalStartTime == KGS->taskMoveIntervalStartTimes[KGS->currTimeVar])
                {
                    this_ukt->stopWaitForBestFitTaskAt = reqArg->stopWaitForBestFitTaskAt;
                    this_ukt->moveTaskToUKT            = reqArg->moveTaskToUKT;
                }
                /* free the queue element which woke us */
                RELEASE_FREELIST_ELEM( this_ukt->curr_quu );
                this_ukt->curr_task = 0 ;
                continue ;
            }

            /*
             *  CONNECT requests
             */
            if ( (this_ukt->curr_quu->req_type == REQ_CONNECT) ||
                 (this_ukt->curr_quu->req_type == REQ_CONNECT_REQUEST_EO00) ||
                 (this_ukt->curr_quu->req_type == REQ_BIG_CONNECT) )
            {
                RTETask_Task*                pTask         = this_ukt->curr_task->pTaskObject;
                RTETask_CommonQueueElement*  pQueueElement = this_ukt->curr_quu->pQueueElementObject;

                if ( pTask->State() != RTETask_Task::TaskWaitingForConnect )
                {
                    // - the request will be evaluated by 'AcceptClientConnect'.
                    pTask->ClientConnections().EnqueueIntoConnectionRequestQueue(*pQueueElement);
                }
                else
                {
                    en71_EnquRun(this_ukt, this_ukt->curr_task, NULL, this_ukt->curr_quu, BASE_PRIO_COM, false ) ;
                    this_ukt->curr_task = 0 ;
                }
                continue ;
            }

#ifdef DEBUG_RTE
            if ( this_ukt->curr_quu->req_type == REQ_VBLOCKIO )
            {
                DBG1 (( MF__,"REQ_VBLOCKIO, this_ukt 0x%x, quu->ukt 0x%x \n",
                            this_ukt , this_ukt->curr_task->ukt ));
            }
#endif /* DEBUG_RTE */

            if ( this_ukt->curr_quu->req_type == REQ_ASYNIO )
            {
                DBG1 (( MF__,"REQ_ASYNIO, this_ukt 0x%x, quu->ukt 0x%x \n",
                            this_ukt , this_ukt->curr_task->ukt ));
                this_ukt->curr_quu->args.vai_parms.finished = TRUE ;
                /* task searches for finished io req. on call of 'vasynwait' */
                if ( this_ukt->curr_task->state != TSK_ASYNWAIT_READ_EO00 &&
                     this_ukt->curr_task->state != TSK_ASYNWAIT_WRITE_EO00 ) continue ;
            }

            /*
			 *  RESUME Tracewriter
			 */
			if ( this_ukt->curr_quu->req_type == REQ_RESUME_TW_EO00 )
			{
   				DBG1 (( MF__,"REQ_RESUME_TW, this_ukt 0x%x, quu->ukt 0x%x \n",
					    this_ukt , this_ukt->curr_task->ukt ));

                RELEASE_FREELIST_ELEM (this_ukt->curr_quu);

                /* --- tracewriter suspended and not already triggered? */
                if ( KGS->tw->state == TSK_VSUSPEND_EO00 && Trace_Flush (KGS->tw->index))
                {
       				DBG1 (( MF__,"REQ_RESUME_TW, this_ukt 0x%x, quu->ukt 0x%x \n",
						    this_ukt , this_ukt->curr_task->ukt ));

                    /* --- resume tracewriter by 'REQ_VRESUME' */
                    this_ukt->curr_quu              = GetTaskDLQElement1(KGS->tw);
                    this_ukt->curr_quu->req_type    = REQ_VRESUME;
                    KGS->tw->resume_count = 0; /* This is done instead of increment the counter by one as */
                                               /* vresume would do */
                    ++KGS->tw->totalResumerCount;
                }
                else
                {
                    this_ukt->curr_task = 0;
                    continue;
                }
            }

            /*
			 *  DEBUGTASK
			 */
            if ( this_ukt->curr_quu->req_type == REQ_DEBUG_TASK_EO00 )
			{
#           if defined ( ALLOW_TASK_DEBUG_REQUESTS )
                if ( false == this_ukt->curr_task->pTaskObject->TaskIsInactive() && 
                     false == this_ukt->curr_task->fDebugTask )
                {
                    this_ukt->curr_task->fDebugTask = TRUE;
                    en71_EnquRun(this_ukt, this_ukt->curr_task, NULL, 
                                 this_ukt->curr_quu,  BASE_PRIO_DEBUGTASK, false) ;// - maximum priority
                }
                else
                    RELEASE_FREELIST_ELEM (this_ukt->curr_quu);
#           else
                RELEASE_FREELIST_ELEM (this_ukt->curr_quu);
#           endif
                continue;
            }

			DBG1 (( MF__,"IOC task    T%d  request %d \n", 
					this_ukt->curr_task->index ,  
					this_ukt->curr_quu->req_type ));
            en71_EnquRun(this_ukt, this_ukt->curr_task, NULL, 
                    this_ukt->curr_quu,  BASE_PRIO_IOC, false);
        }
        /* IOC queue */

        /******************************************************************/
        /* search COM queue, add elemets to RUN_QUEU                      */
        /******************************************************************/
        new_timeout = MAXTIMEVALUE ;
        /* PTS 1110829 adopted to NT behaviour */
        if ( BASE_PRIO_COM > 0 || this_ukt->RunQueueLen <= 1 )
        {
            new_timeout = SearchForCommunicationRequests( KGS->current_time );
        }

        if ( KGS->state >= SERVER_SHUTDOWNREINIT ) 
        {
            MSGD (( INFO_DISP_SHUTKILL_STATE1, KGS->state ));
            en71_shutdownKill (this_ukt);
        }

        /******************************************************************/
        /* search RUN queue, search for higest prio Task                  */
        /******************************************************************/
        if ( this_ukt->RunQueueLen > 0 )
        {
            tsp00_Int4        idx, runIndex   = 0;
            struct TASK_TYPE* highestPrioTask;
            struct TASK_TYPE* tempTaskCtrl;
            struct TASK_TYPE* moveTask = NULL;
            struct RUN_QUEUE* runQueue               = this_ukt->RunQueue;
            tsp00_Int4        runQueueLen            = this_ukt->RunQueueLen;

            if (( this_ukt->moveTaskToUKT != NULL ) && ( this_ukt->numOfMovableTasks != 0 ))
            {
                SAPDB_UInt8  maxRunnableMicroSeconds = 0;
                SAPDB_UInt2  prevTimeVar             = PREV_TIME_VAR(this_ukt->currTimeVar);
                SAPDB_UInt8  taskMaxRunnableMicroSeconds;

                for ( idx = 0; idx < runQueueLen; idx++ )
                {
                    tempTaskCtrl                = runQueue[idx].Task;
                    taskMaxRunnableMicroSeconds = tempTaskCtrl->maxRunnableMicroSeconds[prevTimeVar];  

                    if (( tempTaskCtrl->moveLock.val == 0 ) && ( tempTaskCtrl != lastRunningTaskCtrl ))
                    {
                        if ( this_ukt->stopWaitForBestFitTaskAt > KGS->current_time )
                        { /* we are searching for the UKTs task with maximum runnable time */
                            if ( taskMaxRunnableMicroSeconds == this_ukt->maxRunnableMicroSeconds[prevTimeVar] )
                            {
                                moveTask = tempTaskCtrl;
                                break;
                            }
                        }
                        else if (( maxRunnableMicroSeconds  
                                 < taskMaxRunnableMicroSeconds ) &&
                                 ( this_ukt->moveTaskToUKT->maxRunnableMicroSeconds[prevTimeVar] 
                                 < taskMaxRunnableMicroSeconds ))
                        {
                            moveTask                = tempTaskCtrl;
                            maxRunnableMicroSeconds = taskMaxRunnableMicroSeconds;
                        }
                    }
                }
            }


            for ( idx = 0, this_ukt->SelfDispatch = 0, highestPrioTask = runQueue[0].Task; 
                idx < runQueueLen; 
                idx++ ) 
            {
                struct DOUBLY_LINKED*    tmpRequest;

                tempTaskCtrl = runQueue[idx].Task;
                tmpRequest   = runQueue[idx].Req;

                if ( tempTaskCtrl == moveTask )
                {
                    tempTaskCtrl->is_in_queue = TASK_IS_IN_NON_QUEUE_EO00;
                    tempTaskCtrl->ulDynPrio   = 0; 

                    en71MoveTask ( tempTaskCtrl, tmpRequest );

                    if ( (this_ukt->RunQueueLen = --runQueueLen) != 0 )
                    {
                        if ( runQueueLen > idx )
                        {
                            if ( idx == 0 )
                                highestPrioTask = runQueue[runQueueLen].Task;

                            runQueue[idx--] = runQueue [runQueueLen];
                        }
                        else
                        {
                            tempTaskCtrl = runQueue[--idx].Task;
                            tmpRequest   = runQueue[idx].Req;
                        }
                        continue;
                    }
                    else
                    {   
                        highestPrioTask = NULL;
                        break; /* no more entries in run queue */
                    }
                }

                tempTaskCtrl->ulDynPrio++;

                if ( tempTaskCtrl->prio_flag ) 
                    this_ukt->SelfDispatch++;

                if ( REQ_DEBUG_TASK_EO00 == runQueue[idx].Req->req_type )
                {   // We have use the yield element first because the task 
                    // is waiting for the yield request first.
                    highestPrioTask = tempTaskCtrl;
                    runIndex        = idx;
                    break;
                }

                if ( REQ_YIELDING_EO00 == runQueue[idx].Req->req_type ) /* Yield request? */
                    tempTaskCtrl->yieldRequestAtPos = idx;

                if ( highestPrioTask->ulDynPrio < tempTaskCtrl->ulDynPrio )
                { 
                    highestPrioTask = tempTaskCtrl;
                    runIndex        = idx;
                }
            }

            if ( highestPrioTask )
            {
                if ( -1 != highestPrioTask->yieldRequestAtPos )
                {   
                    // Swap run queue index because yield has to be served first
                    runIndex = highestPrioTask->yieldRequestAtPos;
                    highestPrioTask->yieldRequestAtPos = -1;
                }
                highestPrioTask->is_in_queue = TASK_IS_IN_NON_QUEUE_EO00;
                highestPrioTask->ulDynPrio   = 0; 

                if ( highestPrioTask->prio_flag ) this_ukt->SelfDispatch--;

                if ( lastRunningTaskCtrl != highestPrioTask && lastRunningTaskCtrl && lastRunningTaskCtrl->delayTimeMeasurement )
                {
                    /* we will spend some unexpected time in the run queue as runnable... */
                    /* to allow dispatcher to move us later we reenable timing collection temporary */
                    /* We also have to repeat the dispatcherEnterTime measurement which we skipped before... */
                    en71_DoEnterDispatcherTiming(this_ukt, lastRunningTaskCtrl);
                    lastRunningTaskCtrl->delayTimeMeasurement = false;
                }

                this_ukt->curr_task = highestPrioTask ;
                this_ukt->curr_quu  = runQueue[runIndex].Req ; 

                if ( --this_ukt->RunQueueLen )
                    runQueue[runIndex] = runQueue[this_ukt->RunQueueLen] ; 


                PRINT_RUN_TASK ;
                this_ukt->state = KT_RUNNING ;

                if ( 0 != XPARAM(fTimeMeasure) )
                {
                    if ( EO52_NO_RESET_PENDING != highestPrioTask->TaskStatRecResetPending )
                    {
                        en71_ResetTaskStatisticRecords(this_ukt, highestPrioTask);
                    }

                    if ( highestPrioTask->QueueStatRecResetPending )
                    {
                        eo52ResetStatisticRecord(highestPrioTask->QueStat);
                        highestPrioTask->QueueStatRecResetPending = false;
                    }
                }

                /* if we rescheduled ourself(task yield), time collection will be suppressed */
                if  ( 0 == this_ukt->curr_task->delayTimeMeasurement ) 
                    en71_DoLeaveDispatcherTiming(this_ukt, this_ukt->curr_task);

                this_ukt->curr_task->delayTimeMeasurement = false;

#ifdef DEBUG_RTE
                sp77sprintf(eBuffer, sizeof(eBuffer),
                    "LEAVE en71Dispatcher UKT%d curr_task T%d", 
                    this_ukt->index, 
                    highestPrioTask->index );
                MSGD(( INFO_TEST_WILDCARD, &eBuffer[0] ));
#endif
                /* Wild running kernel... Do not leave dispatcher if not trace writer... */
                if ( KGS->state >= SERVER_SHUTDOWNREINIT ) 
                {
                    MSGD (( INFO_DISP_SHUTKILL_STATE1, KGS->state ));
                    en71_shutdownKill (this_ukt);
                }

                m_IsWaitingForNextScheduledTask = false;

                m_pCurrentTask = highestPrioTask->pTaskObject;

                return *m_pCurrentTask;
            }
        }

        if ( !loop_count 
          && this_ukt->fSemAlreadyPosted == FALSE )
        {
            loop_count = DISPATCHER_LOOPS ;

            this_ukt->ukp_timeout = new_timeout ;
            DBG1 (( MF__,"SLEEP, timeout %d seconds \n", (int) (new_timeout - KGS->current_time) ));

WAIT_AGAIN :

#ifdef DEBUG_SLEEP_STATE
            en71_debugSleepState ( this_ukt );
#endif
            this_ukt->state = KT_SLEEPING;

            this_ukt->SleepingStateEnterMicroSeconds = RTESys_MicroSecTimer();

            if ( this_ukt->RunningStateEnterMicroSeconds )      /* if it is set, it marks the time after wakeup below... */
            {
                SAPDB_UInt8 delta_time = this_ukt->SleepingStateEnterMicroSeconds 
                                        - this_ukt->RunningStateEnterMicroSeconds;
                this_ukt->UKTRunningStat.TotalTimeStat.ulCount++;
                this_ukt->UKTRunningStat.TimeStat.ulCount++;
                this_ukt->UKTRunningStat.TotalTimeStat.ulAbsMicroSeconds += delta_time;
                this_ukt->UKTRunningStat.TimeStat.ulAbsMicroSeconds += delta_time;
            }

            rc = semop ( this_ukt->semid , &semrcv , 1 );

            timeNow = RTESys_MicroSecTimer();

            this_ukt->RunningStateEnterMicroSeconds                   = timeNow;
            this_ukt->UKTSleepTimeMicroSeconds[this_ukt->currTimeVar] = timeNow - this_ukt->SleepingStateEnterMicroSeconds;

            /* UKTIOWaitStat.ulAbsMicroSeconds only counts the micro seconds spended waiting for DeviIO */
            if ( this_ukt->wait_for_ioproc_io )
            {
                SAPDB_UInt8 delta_time = this_ukt->RunningStateEnterMicroSeconds 
                                        - this_ukt->SleepingStateEnterMicroSeconds;
                this_ukt->UKTIOWaitStat.TotalTimeStat.ulCount++;
                this_ukt->UKTIOWaitStat.TimeStat.ulCount++;
                this_ukt->UKTIOWaitStat.TotalTimeStat.ulAbsMicroSeconds += delta_time;
                this_ukt->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds += delta_time;
            }

            if ( rc )
            {
                if ( errno == EINTR )
                { 
                    if ( this_ukt->aio.aio_in_progress <= 0 )
                    {
#ifndef LINUX
                        MSGD (( INFO_DISP_IGN_SIGNAL )) ;
#endif
                        goto WAIT_AGAIN ;
                    }
                    else
                    {
                        DBG1 (( MF__,"AWOKE, perhaps aio_complete\n")) ;
                    }
                }
                else
                {
                    /* The UKT semaphore is unusable... We cannot wake up tracewriter
                       so we let him die... */
                    en81_NotifyCoordinator( KGS->tw->ukt == this_ukt );
                    if ( KGS->state < SERVER_SHUTDOWNREINIT )
                    {
                        MSGCD (( ERR_DISP_SEMOP_ERR, sqlerrs() ))
                    }

                    en71_StopUkt( THREAD_INDEX(this_ukt) );
                }
            }
            DBG1 (( MF__,"AWOKE \n" ));

            this_ukt->ukp_timeout = MAXTIMEVALUE ;
            this_ukt->state = KT_RUNNING;
        }
        this_ukt->fSemAlreadyPosted = FALSE;
    } /* for (;;) */
    /*NOTREACHED*/
}

/*
 * ===========================================================================
 */

#ifdef DEBUG_SLEEP_STATE
#undef  MF__
#define MF__ MOD__"en71_debugSleepState"
static  void    en71_debugSleepState ( ten50_UKT_Control * this_ukt )
{
    DBGIN;
    if ( this_ukt->wait_for_ioproc_io )
        DBG1(( MF__, "disp sleeps, %d tasks are waiting for io\n", 
                this_ukt->wait_for_ioproc_io ));

    if ( this_ukt->ActTasksVwait || this_ukt->ActTasksPrio )
    {
      struct  TASK_TYPE * tcb ;

      DBG1(( MF__, "disp sleeps, %d tasks are waiting, %d prios\n",
             this_ukt->ActTasksVwait , this_ukt->ActTasksPrio ));

        for ( tcb = this_ukt->pFirstTaskCtrl ; tcb ; tcb = tcb->next_task )
        {
            if ( tcb->state == TSK_VWAIT )
            { 
                if ( tcb->SetPrioToTask )
                    DBG1(( MF__, "T%d is waiting for T%d (%s) prio %d\n",
                      tcb->index, tcb->SetPrioToTask , 
                      e38_tskstate_name((KGS->pFirstTaskCtrl + tcb->SetPrioToTask -1)->state),
                      (KGS->pFirstTaskCtrl + tcb->SetPrioToTask -1)->prio_flag ));
            }
            else
            {
                if ( tcb->prio_flag )
                  DBG1(( MF__, "T%d (%s) prio %d\n", tcb->index,
                        e38_tskstate_name(tcb->state), tcb->prio_flag )) ;
            }
        }
    }
    DBGOUT;
}
#endif

/*
 * ===========================================================================
 */
/*
 * ===========================================================================
 */

static  void    en71_shutdownKill ( ten50_UKT_Control * this_ukt )
{
#undef  MF__
#define MF__ MOD__"en71_shutdownKill"
    struct TASK_TYPE            *tcb ;
    DBGIN;

    /*
     *  Release all tasks of this UKP.
     */
    e75_release_all_ukp_tasks ();

    /*
     *  Free all regions held by this process.
     */
    en71_freeUktOwnedRegions ( this_ukt);

    /*
     *  All processes not containing bufwriter
     *  should exit as immediate as possible.
     */
    if ( KGS->tw->ukt != this_ukt )
    {
        en81_NotifyCoordinator(0);
        en71_StopUkt( THREAD_INDEX(this_ukt) );
    }
    DBGOUT;
}

/*
 * ===========================================================================
 */

static  void    en71_freeUktOwnedRegions ( ten50_UKT_Control * this_ukt )

{
#undef  MF__
#define MF__ MOD__"en71_freeOwnRegions"
    int                             idx ;
    struct DDB4_SEMA                * sem ;
    struct TASK_TYPE                * tcb ;
    struct N_QUEUE                  * qp ;

    DBGIN;

    for ( idx = 0 ; idx < XPARAM(ulNoOfRegions) ; idx ++ )
    {
        sem = KGS->semaphore + idx ;

        tcb = (struct TASK_TYPE *)sem->owner ;

        if ( tcb == NULL || tcb->ukt != this_ukt ) continue ;

        if ( tcb == KGS->tw ) continue ;

        /* clear list and check for waiting tracewriter */
        WAIT_UNTIL_ALONE( sem->semaliste.sem_exclusive );
        sem->owner = 0;
        for ( qp = sem->semaliste.last ; qp && qp->taskid ; qp = qp->next )
        {
            /* the tracewriter is already waiting... */
            if ( qp->taskid == KGS->tw )
            {
                sem->owner = KGS->tw; /* new owner tracewriter */
            }
            qp->taskid = 0;
            sem->semaliste.last = qp;
        }
        CLEARLOCK ( sem->semaliste.sem_exclusive );

        /* if tracewriter is new owner wake him up */
        if ( sem->owner )
        {
            /* fake an vendexcl by current semaphore owning task... */
            this_ukt->curr_task = tcb;

            if ( KGS->tw->ukt == this_ukt )
            {
                struct DOUBLY_LINKED *request;
                request = GetTaskDLQElement1(tcb);
                request->req_type = REQ_EXCLUSIVE;
                
                e71_EnquRun ( this_ukt, tcb, 0, request, BASE_PRIO_RAV );
                tcb->ulDynPrio += PRIO_FACTOR;
            }
            else
            {
                en71EnqueueUkt2UktAndWake ( this_ukt , KGS->tw , tcb , e55_remactive, (void *)(idx+1) );
            }
        }
    }
    DBGOUT;
}

/*
 * ===========================================================================
 */

void    en71_iocEnquAndWake ( 
ten50_UKT_Control               * ukt ,
struct TASK_TYPE                * tcb ,
struct DOUBLY_LINKED            * quu )
{
#undef  MF__
#define MF__ MOD__"en71_iocEnquAndWake"

    en71_ioc ( ukt, tcb, quu );
    e72_wake ( ukt );
}
/*
 * ===========================================================================
 */

void    en71_ioc ( 
ten50_UKT_Control               * ukt ,
struct TASK_TYPE                * tcb ,
struct DOUBLY_LINKED            * quu )
{
#undef  MF__
#define MF__ MOD__"en71_ioc"

    DBGIN;
    /*
     *  Enter queue element in IOC queue
     *  this is the only enque-procedure that may be used by other
     *  processes.......................
     */
    DBG1 (( MF__,"called, task T%d   kt tid %ld \n",
            tcb->index , (long) ukt->tid ));
    DBG1 (( MF__,"request 0x%x  req_type %d \n",
                quu , quu->req_type ));

    quu->taskid = tcb ;

    e73_ioc_enqu( & ukt->ioc_queue , quu );
    tcb->QueStat.Total.ulUKTQueCount++ ;
    tcb->QueStat.Resetable.ulUKTQueCount++ ;
    tcb->is_in_queue = TASK_IS_IN_UKT_QUEUE_EO00 ;

    DBGOUT;
}

/*
 * ===========================================================================
 */

void    en71_iocNoTask ( 
ten50_UKT_Control               * ukt ,
struct DOUBLY_LINKED            * quu )

{
#undef  MF__
#define MF__ MOD__"en71_iocNoTask"

    DBGIN;
    /*
     *  Enter queue element in IOC queue
     *  this is the only enque-procedure that may be used by other
     *  processes.......................
     */
    quu->taskid = 0;
    e73_ioc_enqu( &ukt->ioc_queue , quu );

    DBGOUT;
}

/*
 * ===========================================================================
 */
void v2prio ( tsp00_Int4 task_id ,
              UINT1 prio ,
              tsp00_Bool set_prio ,
              int root , 
              int leaf, 
              int locktype )
{
#undef  MF__
#define MF__ MOD__"v2prio"
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    DBGIN;
    DBG1 (( MF__,"T%d tries to prio Task T%d (%d/%d/%d)\n",
          this_ukt->curr_task->index , task_id, root, leaf, locktype ));
    if ( prio == 2 && set_prio )
    { 
      this_ukt->curr_task->lWaitForRoot = root ;
    }
    vprio ( task_id, prio, set_prio ) ;
    DBGOUT;
}

/*
 * ===========================================================================
 */
void vprio ( tsp00_Int4 task_id ,
             UINT1 prio ,
             tsp00_Bool set_prio )
{
#undef  MF__
#define MF__ MOD__"vprio"
    ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;
    struct TASK_TYPE *tcb;
    
    DBGIN;
    if ( ! XPARAM(lPrioFactor) )
        return;
    
    if ( set_prio ) this_ukt->curr_task->lWaitForTask = task_id ;
    
    tcb    = KGS->pFirstTaskCtrl + ( task_id - 1 ) ;
    if ( ( !tcb->prio_flag         && !set_prio ) ||
        ( tcb->prio_flag == prio  &&  set_prio ) )
        return ;
    
    if ( set_prio )
    { 
        if ( prio == 2 )
        { 
            tcb->save_prio = tcb->prio_flag ;
            DBG5 (( MF__, "Saving PRIO %d, NEW_PRIO is %d\n",
                tcb->prio_flag, prio ));
        }
        else
            if ( tcb->prio_flag == 2 )
            { 
                tcb->save_prio = prio ;
                DBG4 (( MF__, "Saving PRIO %d, ACT_PRIO is 2\n", prio ));
                return ;
            }
    }
    else               /* "set_prio" is FALSE, looking for "SavePrioFlag" */
    {
        if ( tcb->save_prio )
        { 
            tcb->prio_flag = tcb->save_prio;
            tcb->save_prio = 0 ;
            return ;
        }
    }       

    tcb->prio_flag = set_prio ? prio : 0 ;
    if ( set_prio )
    { 
        tcb->PrioStat.Total.ulTotalCount++ ;
        tcb->PrioStat.Resetable.ulTotalCount++ ;
        tcb->ukt->ActTasksPrio++ ;
        if ( tcb != this_ukt->curr_task )
        {
            switch ( tcb->is_in_queue )
            { 
            case TASK_IS_IN_NON_QUEUE_EO00 : 
                break ;
            case TASK_IS_IN_UKT_QUEUE_EO00 :
                tcb->ukt->SelfDispatch++;
                break ;
            case TASK_IS_IN_RUN_QUEUE_EO00 :
                tcb->ulDynPrio += prio * XPARAM(lPrioFactor) ;
                tcb->ukt->SelfDispatch++;
                break ;
            }
            if ( tcb->ukt != this_ukt ) 
            {
                tcb->PrioStat.Total.ulOtherUkt ++;
                tcb->PrioStat.Resetable.ulOtherUkt ++;
            }
        }
    }
    else tcb->ukt->ActTasksPrio-- ;

    DBGOUT;
}

/*
 * ===========================================================================
 */

global void e71_EnquRun ( struct ten50_UKT_Control * pUKT,
                          struct TASK_TYPE         * pTask,
                          struct TASK_TYPE         * pFromTask,
                          struct DOUBLY_LINKED     * pReq, 
                          SAPDB_UInt4                basePrio )
{
#undef  MF__
#define MF__ MOD__"e71_EnquRun"

    en71_EnquRun( pUKT, pTask, pFromTask, pReq, basePrio, false );
}

/*
 * ===========================================================================
 */

global void e71_YieldTaskRunEnqu ( ten50_UKT_Control     * pUKT,
                                   struct TASK_TYPE      * pTask,
                                   struct DOUBLY_LINKED  * pReq, 
                                   SAPDB_UInt4             basePrio )
{
#undef  MF__
#define MF__ MOD__"e71_YieldTaskRunEnqu"

  en71_EnquRun( pUKT, pTask, pTask, pReq, basePrio, true );
}


/*
 * ===========================================================================
 */

void    en71EnqueueUkt2UktAndWake ( 
ten50_UKT_Control               * this_ukt ,
struct TASK_TYPE                * tcb ,
struct TASK_TYPE                * fromtcb ,
PFNREMOTEFUNC                   func ,
void                            *func_param )
{
#undef  MF__
#define MF__ MOD__"en71EnqueueUkt2UktAndWake"
    struct XXX_QUEUE_HEAD * xxxqh ;
    struct XXX_QUEUE      * xxxq ;
    ten50_UKT_Control     * ukt  = tcb->ukt;


    DBGIN_T(this_ukt->curr_task->index);
    DBG1 (( MF__,"wakes T%d \n", tcb->index ));

    xxxqh             = ukt->xxx [ this_ukt->index - 1 ];

#ifndef SAPDB_NO_SPINLOCK_FOR_UKT2UKT
    WAIT_UNTIL_ALONE ( xxxqh->exclusive );
    /*
     * we are alone now
     */
    xxxq              = xxxqh->xx_request ;
    xxxq->to_execute  = func ;
    xxxq->func_param  = func_param ;
    xxxq->FromTask    = fromtcb;
    xxxq->task        = tcb;
    xxxqh->xx_request = xxxq->next ;

    CLEARLOCK ( xxxqh->exclusive );
#else
    while ( xxxqh->xx_request->to_execute != NULL )
    {
         /* PTS 1000746 */
        xxxqh->num_overflows++ ;
    }
    xxxq              = xxxqh->xx_request ;
    xxxq->to_execute  = func ;
    xxxq->func_param  = func_param ;
    xxxq->FromTask    = fromtcb;
    xxxq->task        = MEM_SYNC ( tcb );
    xxxqh->xx_request = xxxq->next ;
#endif

    if ( tcb->prio_flag && XPARAM(fDynDispQueSrch) )
    {
      ukt->SelfDispatch ++ ; 
    }

    tcb->QueStat.Total.ulUToUQueCount++ ;
    tcb->QueStat.Resetable.ulUToUQueCount++ ;

    e72_wake ( ukt );
    DBGOUT_T(this_ukt->curr_task->index );
}


#ifndef SAPDB_NO_SPINLOCK_FOR_UKT2UKT
/*
  Function: en71_xxxDequeue
  Description: Dequeue from given XXX queue using spinlock protected code
  Argument: queueHead [in] Pointer to queue head
            copyOfItemToDequeue [out] filled with content of item to dequeue
  Return value: false if nothing to dequeue, true else
 */
static int
en71_xxxDequeue( struct XXX_QUEUE_HEAD * queueHead,
                 struct XXX_QUEUE      * copyOfItemToDequeue )
{
    int foundItem     = 0;
    struct XXX_QUEUE * itemToDequeue;

    WAIT_UNTIL_ALONE ( queueHead->exclusive );

    /*
     * we are alone now
     */
    itemToDequeue = queueHead->xx_work;
    if ( 0 != itemToDequeue && 0 != itemToDequeue->task )
    {
        foundItem = 1;
        SAPDB_memcpy(copyOfItemToDequeue, itemToDequeue, sizeof(struct XXX_QUEUE));
        itemToDequeue->task = 0;
        itemToDequeue->to_execute = 0;
        queueHead->xx_work = itemToDequeue->next;
    }

    CLEARLOCK ( queueHead->exclusive );

    return ( foundItem );
}
#endif

/*
 * =====================================================================
 */

void    vdelay_commit ( PROCESS_ID pid )
{
#undef  MF__
#define MF__ MOD__"vdelay_commit"

  DBGIN;
  if ( XPARAM(fDelayCommit) )
    vsleep ( pid, 0 );
  DBGOUT;
}

void    vdelay_lw ( PROCESS_ID pid )
{
#undef  MF__
#define MF__ MOD__"vdelay_lw"
  ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
  SAPDB_UInt4                     actInComQueue = NumberOfCommunicationQueueElements(this_ukt);


  DBGIN;
  if ( this_ukt->delay_active || (XPARAM(lDelayLW) && ( actInComQueue + this_ukt->act_rex_elem > 4 )) )
  { 
    struct DOUBLY_LINKED            * lquu ;
    
    this_ukt->curr_task->state = TSK_SLEEP_EO00;
    eo52UpdateCountInStatisticRec(this_ukt->curr_task->TaskStateStat.VsleepStat);
    lquu = GetTaskDLQElement1(this_ukt->curr_task);
    lquu->req_type = REQ_VSLEEP ;
    lquu->taskid = this_ukt->curr_task ;

    DBG1 (( MF__,"before : act_rex_elem = %d, act_com_elem = %d\n",
            this_ukt->act_rex_elem, actInComQueue )) ;

    if ( !this_ukt->delay_active )
    {
        this_ukt->end_switch_cnt = 
            XPARAM(lDelayLW) > 1 ? XPARAM(lDelayLW) :
                         this_ukt->activeTasks > DEF_KERNEL_TASKS ?
                         this_ukt->activeTasks - DEF_KERNEL_TASKS :
                         this_ukt->activeTasks ;

        if ( actInComQueue > 10 ) 
        {
            this_ukt->delay_prio = this_ukt->curr_task->prio_flag = 
                    1 ;/* first in rex */
        }
        else 
        {
            this_ukt->delay_prio = this_ukt->curr_task->prio_flag = 
                    this_ukt->act_rex_elem > 10 ? 10 : 0 ;
        }
    }
    else
    {
       this_ukt->curr_task->prio_flag = 
          ( this_ukt->delay_prio > 1 
         && this_ukt->act_rex_elem < this_ukt->delay_prio ) 
                ? 0 : this_ukt->delay_prio ;
    }

    e71_EnquRun ( this_ukt, this_ukt->curr_task, this_ukt->curr_task, GetTaskDLQElement1(this_ukt->curr_task), BASE_PRIO_REX );

    this_ukt->curr_task->ulDynPrio += this_ukt->curr_task->prio_flag * PRIO_FACTOR;

    this_ukt->curr_task->prio_flag = 0 ;
    this_ukt->delay_active++ ;
    GOTO_DISP(&this_ukt);
    this_ukt->delay_active-- ;
    DBG1 (( MF__,"after  : act_rex_elem = %d, act_com_elem = %d\n",
        this_ukt->act_rex_elem, actInComQueue )) ;
    if ( !this_ukt->delay_active )
    { 
        this_ukt->end_switch_cnt = 0 ;
    }
  }
  DBGOUT;
}

/*
 * =====================================================================
 */

time_t en71LoadBalancing()
  {
#undef  MF__
#define MF__ MOD__"en71LoadBalancing"

  static SAPDB_Bool    measInterval = false;
  tsp00_Longuint       sleepTime = XPARAM(uLoadBalancingCheck);

  if ( KGS->state == SERVER_WARM )
    {
    if ( KGS->taskMoveMeasIntervals < 2 )
      { /* we don't have enough statistic data! */
      en71SendMoveMeassIntRequest ();  
      sleepTime = XPARAM(uLoadBalancingCheck);
      }
    else if ( measInterval == true ) 
      {
      en71SendMoveMeassIntRequest();

      if ( KGS->taskMoved ) 
        { /* A task has been moved during last measurement interval. Interval */
          /* statistics will be ignored. Don't make a move check next time. */
        KGS->taskMoved = false;
        sleepTime      = XPARAM(uLoadBalancingCheck);
        }
      else
        {
        measInterval  = false; /* Next time we do a move check. */
        sleepTime     = XPARAM(uLoadBalancingCheck) / 2;
        }
      }
    else
      {
      ten50_UKT_Control *srcUKT, *dstUKT;

      if (en71TaskMoveCheck( &srcUKT, &dstUKT ))
      {
        en71SendMoveRequest ( srcUKT, dstUKT, XPARAM(uLoadBalancingCheck) / 2 + KGS->current_time );

        sleepTime = XPARAM(uLoadBalancingCheck);
      }
      else
        sleepTime = XPARAM(uLoadBalancingCheck) - (XPARAM(uLoadBalancingCheck) / 2);

      measInterval = true;   /* Next time we start a new measurement interval. */
      }
    }
  return sleepTime;
  }

/*
 * =====================================================================
 */

void en71TempMoveLock( struct TASK_TYPE   *tcb,
                       SAPDB_UInt2        cnt,
                       SAPDB_Bool         setLock )
{
#undef  MF__
#define MF__ MOD__"en71TempMoveLock"
    if (setLock)
    {
        if ( tcb->moveLock.val == 0 )
            tcb->ukt->numOfMovableTasks--;

        tcb->moveLock.type.temporary += cnt;
    }
    else
    {
#if defined (CHECK_MOVELOCK_UNDERRUN)
        if (tcb->moveLock.type.temporary < cnt )
             vabort ( WRITE_CORE ) ;
#endif
        tcb->moveLock.type.temporary -= cnt;

        if ( tcb->moveLock.val == 0 )
            tcb->ukt->numOfMovableTasks++;
    }
}

/*
 * =====================================================================
 */

void en71LongTermMoveLock( struct TASK_TYPE   *tcb,
                           SAPDB_UInt2        lockType,
                           SAPDB_Bool         setLock )
{
#undef  MF__
#define MF__ MOD__"en71LongTermMoveLock"

    if (setLock)
    {
        if ( tcb->moveLock.val == 0 )
            tcb->ukt->numOfMovableTasks--;

        tcb->moveLock.type.longTerm |= lockType;
    }
    else
    {
        tcb->moveLock.type.longTerm &= ~lockType;

        if ( tcb->moveLock.val == 0 )
            tcb->ukt->numOfMovableTasks++;
    }
}

/*
 * =====================================================================
 */

void  en71AlterTaskCnt( struct TASK_TYPE   *tcb,
                        SAPDB_UInt2        alterType )
{
#undef  MF__
#define MF__ MOD__"en71AlterTaskCnt"
    ten50_UKT_Control   *this_ukt = tcb->ukt;
    SAPDB_Int2          alterVal  = COUNTER_SUB & alterType ? -1 : 1;

    if ( alterType & TOTAL_COUNTER )
    {
        /* alter total counter  */
        switch ( tcb->type )
        {
        case TT_EV_EO00:
            this_ukt->numEventTasks       += alterVal;
            break;
        case TT_UT_EO00:
            this_ukt->numUtilityTasks     += alterVal;
            break;
        case TT_US_EO00:
            this_ukt->numUserTasks        += alterVal;
            break;
        case TT_SV_EO00:
            this_ukt->numServTasks        += alterVal;
            break;
        }
    }

    if ( alterType & ACTIVE_COUNTER )
    {
        if ( tcb->moveLock.val == 0 )
            this_ukt->numOfMovableTasks += alterVal;

        this_ukt->activeTasks += alterVal;

        switch ( tcb->type )
        {
        case TT_EV_EO00:
            this_ukt->activeEventTasks    += alterVal;
            break;
        case TT_UT_EO00:
            this_ukt->activeUtilityTasks  += alterVal;
            break;
        case TT_US_EO00:
            this_ukt->activeUserTasks     += alterVal;
            break;
        case TT_SV_EO00:
            this_ukt->activeServTasks     += alterVal;
            break;
        }
    }
}

/*
 * =====================================================================
 */

static void en71SendMoveMeassIntRequest()
{
#undef  MF__
#define MF__ MOD__"en71SendMoveMeassIntRequest"
  ten50_UKT_Control*                   tmpUKT;
  struct DOUBLY_LINKED*                request;
  struct REQ_PARMS_MOVE_TASK_MESS_INT  *reqArg;

  for ( tmpUKT = KGS->first_kp; tmpUKT <= KGS->last_kp; tmpUKT++ )
  {
    /* - This check is needed, because of an non responding UKT leeds to an empty 
         queue free element list */
    if (( tmpUKT->dispCounterDuringMesIntReq == tmpUKT->disp_counter ) &&
        ( tmpUKT->state                      != KT_SLEEPING ))
        return;
  }

  KGS->currTimeVar                                  = NEXT_TIME_VAR(KGS->currTimeVar);
  KGS->taskMoveIntervalStartTimes[KGS->currTimeVar] = KGS->current_time;
  KGS->taskMoveMeasIntervals++;

  for ( tmpUKT =  KGS->first_kp; tmpUKT <= KGS->last_kp; tmpUKT++ )
  {
      /* send new measurement interval to UKT */
      request                            = RESERVE_FREELIST_ELEM();
      request->req_type                  = REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00;
      reqArg                             = &request->args.mtmi_parms;
      reqArg->timeVar                    = KGS->currTimeVar;
      reqArg->taskMoveIntervalStartTime  = KGS->taskMoveIntervalStartTimes[KGS->currTimeVar];

      /* --- store to use during next interval request */
      tmpUKT->dispCounterDuringMesIntReq = tmpUKT->disp_counter;

      en71_iocNoTask ( tmpUKT, request );
      e72_wake ( tmpUKT );
  }
}

/*
 * =====================================================================
 */

static void en71SendMoveRequest ( ten50_UKT_Control*   srcUKT,
                                  ten50_UKT_Control*   dstUKT,
                                  SAPDB_UInt8          stopWaitForBestFitTaskAt )
{
#undef  MF__
#define MF__ MOD__"en71SendMoveRequest"
    
    struct DOUBLY_LINKED*                request;
    struct REQ_PARMS_MOVE_TASK           *reqArg;
    
  /* - This check is needed, because of an non responding UKT leeds to an empty 
       queue free element list ( e73_dl_dequ ) */
    if (( srcUKT->dispCounterDuringSendMoveReq == srcUKT->disp_counter ) &&
        ( srcUKT->state                        != KT_SLEEPING ))
      return;
    

    /* send move task */
    request                            = RESERVE_FREELIST_ELEM();
    request->req_type                  = REQ_MOVE_TASK_EO00;
    reqArg                             = &request->args.mt_parms;
    reqArg->moveTaskToUKT              = dstUKT;
    reqArg->stopWaitForBestFitTaskAt   = stopWaitForBestFitTaskAt;
    reqArg->taskMoveIntervalStartTime  = KGS->taskMoveIntervalStartTimes[KGS->currTimeVar];
    
    /* --- store to use during next request */
    srcUKT->dispCounterDuringSendMoveReq = srcUKT->disp_counter;

    en71_iocNoTask ( srcUKT, request );
    e72_wake ( srcUKT );
}


/*
 * =====================================================================
 */

static void en71MoveTask ( struct TASK_TYPE*       tcb,
                           struct DOUBLY_LINKED*   request )
{
#undef  MF__
#define MF__ MOD__"en71MoveTask"
  ten50_UKT_Control*   ukt = tcb->ukt;
  SAPDB_UInt2          entry;
  SAPDB_UInt2          timeVar;
  RTETask_Task*        pTask = tcb->pTaskObject;

  /* alter move task static entry */
  WAIT_UNTIL_ALONE ( KGS->lastTaskMoves.exclusive );
  entry                        = KGS->lastTaskMoves.currEntry;
  KGS->lastTaskMoves.currEntry = NEXT_ENTRY( entry, NUM_OF_TASK_MOVE_STAT_RECS);
  CLEARLOCK ( KGS->lastTaskMoves.exclusive );

  timeVar = PREV_TIME_VAR(ukt->currTimeVar);

  KGS->lastTaskMoves.entries[entry].moveTime                    = KGS->current_time;
  KGS->lastTaskMoves.entries[entry].task                        = tcb;
  KGS->lastTaskMoves.entries[entry].taskMaxRunnableMicroSeconds = tcb->maxRunnableMicroSeconds[timeVar];
  KGS->lastTaskMoves.entries[entry].srcUKT                      = ukt;
  KGS->lastTaskMoves.entries[entry].srcMaxRunnableMicroSeconds  = ukt->maxRunnableMicroSeconds[timeVar];
  KGS->lastTaskMoves.entries[entry].dstUKT                      = ukt->moveTaskToUKT;
  KGS->lastTaskMoves.entries[entry].dstMaxRunnableMicroSeconds  = ukt->moveTaskToUKT->maxRunnableMicroSeconds[timeVar];

  /* remove this task from task list */
  if ( tcb->next_task ) tcb->next_task->prev_task = tcb->prev_task;
  if ( tcb->prev_task ) tcb->prev_task->next_task = tcb->next_task;
  else                  ukt->pFirstTaskCtrl       = tcb->next_task;

  tcb->next_task = NULL;
  tcb->prev_task = NULL;

  KGS->taskMoved = true;

  MSGD(( INFO_TASK_MOVE_REQUEST_TO, tcb->index, tcb->ukt->index,
         ukt->moveTaskToUKT->index ));

  en71AlterTaskCnt( tcb, SUB_ALL_COUNTER );

  tcb->ukt                      = ukt->moveTaskToUKT;
  ukt->moveTaskToUKT            = NULL;
  ukt->stopWaitForBestFitTaskAt = 0;

  /* 
   * The TaskScheduler has changed update task object and signal
   * this change to the clients
   */
  pTask->LoadTaskScheduler(); // load new task scheduler first!
  pTask->ClientConnections().UpdateClientsTaskSchedulerSemaphoreIndexes();

  request->req_type_org = request->req_type;
  request->req_type     = REQ_INSERT_MOVING_TASK_E000;

  en71_iocEnquAndWake ( tcb->ukt, tcb, request );
}


/*
 * =====================================================================
 */

static SAPDB_Bool en71TaskMoveCheck( ten50_UKT_Control**   srcUKT,
                                     ten50_UKT_Control**   dstUKT )
{
#undef  MF__
#define MF__ MOD__"en71TaskMoveCheck"
  ten50_UKT_Control*              tmpUKT;
  SAPDB_UInt8                     microSecondsCompare;
  SAPDB_UInt2                     timeVar, prevTimeVar;

  timeVar     = KGS->currTimeVar;
  prevTimeVar = PREV_TIME_VAR(timeVar);

  for ( *srcUKT =  NULL, microSecondsCompare = 0,
        tmpUKT  =  KGS->first_kp;
        tmpUKT  <= KGS->last_kp;
        tmpUKT++ )
  {
    if ( tmpUKT->maxRunnableMicroSeconds[prevTimeVar] == 0 ) /* This task has no problem! */
        continue;

    /* the UKT has not changed its time counter variable for a long time, 
       might be running in a tight loop */
    if ((tmpUKT->taskMoveIntervalStartTime != KGS->taskMoveIntervalStartTimes[prevTimeVar]) &&
        (tmpUKT->taskMoveIntervalStartTime != KGS->taskMoveIntervalStartTimes[timeVar]))
      {
        continue;
      }

    /* Is there at least one movable task and are threre two or more active tasks inside the UKT */
    if (( tmpUKT->activeTasks > 1 ) && ( tmpUKT->numOfMovableTasks != 0 )) 
    {
      /* The time between the following values is treated as equal if the
         difference is within a 'XPARAM(uLoadBalancingEquality)' percent range. */
      switch ( TASK_MOVE_TIME_COMP( microSecondsCompare, 
                                    tmpUKT->maxRunnableMicroSeconds[prevTimeVar], 
                                    XPARAM(uLoadBalancingEquality )))
      {
        case TASK_MOVE_TIME_COMP_GREATER:
          *srcUKT           = tmpUKT;
          microSecondsCompare = (*srcUKT)->maxRunnableMicroSeconds[prevTimeVar];
          break;
        case TASK_MOVE_TIME_COMP_EQUAL:
          if (( *srcUKT == NULL ) ||( tmpUKT->runnableMicroSeconds[prevTimeVar] > (*srcUKT)->runnableMicroSeconds[prevTimeVar] ))
          {
            *srcUKT           = tmpUKT;
            microSecondsCompare = (*srcUKT)->maxRunnableMicroSeconds[prevTimeVar];
          }
        break;
      }
    }
  }

  if ( (*srcUKT) == NULL )
    return false;

  for ( *dstUKT = NULL, microSecondsCompare = INFINIT_TIME_VAL,
        tmpUKT =  KGS->first_kp;
        tmpUKT <= KGS->last_kp;
        tmpUKT++ )
  {
    /* the UKT has not changed its time counter variable for a long time,
       might be running in a tight loop */
    if ((tmpUKT->taskMoveIntervalStartTime != KGS->taskMoveIntervalStartTimes[prevTimeVar]) &&
        (tmpUKT->taskMoveIntervalStartTime != KGS->taskMoveIntervalStartTimes[timeVar]))
    {
      continue;
    } 
	  
	if (( tmpUKT != *srcUKT ) && ( tmpUKT->tasksAllowedToMoveTo == true ))
    {
      /* The time between the following UKTs is treated as equal if the
         difference is within a 'XPARAM(uLoadBalancingDiff)' percent range. */
      if ( TASK_MOVE_TIME_COMP( (*srcUKT)->maxRunnableMicroSeconds[prevTimeVar], 
                                tmpUKT->maxRunnableMicroSeconds[prevTimeVar], 
                                XPARAM(uLoadBalancingDiff)) == TASK_MOVE_TIME_COMP_LESS )
      {
        /* The time between the following values is treated as equal if the
           difference is within a 'XPARAM(uLoadBalancingEquality)' percent range. */
        switch ( TASK_MOVE_TIME_COMP( microSecondsCompare, 
                                      tmpUKT->maxRunnableMicroSeconds[prevTimeVar], 
                                      XPARAM(uLoadBalancingEquality )))
        {
          case TASK_MOVE_TIME_COMP_LESS:
            *dstUKT           = tmpUKT;
            microSecondsCompare = (*dstUKT)->maxRunnableMicroSeconds[prevTimeVar];
            break;
          case TASK_MOVE_TIME_COMP_EQUAL:
            if (( *dstUKT == NULL ) || ( tmpUKT->UKTSleepTimeMicroSeconds[prevTimeVar] > (*dstUKT)->UKTSleepTimeMicroSeconds[prevTimeVar] ))
            {
              *dstUKT           = tmpUKT;
              microSecondsCompare = (*dstUKT)->maxRunnableMicroSeconds[prevTimeVar];
            }
          break;
        }
      }
    }
  }

  if ( *dstUKT != NULL )
    return true; 

  return false;
}

/*
 * ===========================================================================
 */

static void en71_EnquRun ( struct ten50_UKT_Control * pUKT,
                           struct TASK_TYPE         * pTask,
                           struct TASK_TYPE         * pFromTask,
                           struct DOUBLY_LINKED     * pReq, 
                           SAPDB_UInt4                basePrio,
                           SAPDB_Bool                 taskYield )
{
#undef  MF__
#define MF__ MOD__"en71_EnquRun"
  DBGIN;

  pTask->is_in_queue = TASK_IS_IN_RUN_QUEUE_EO00 ;
  pTask->delayTimeMeasurement  = taskYield;
  pTask->ulDynPrio = basePrio + pTask->prio_flag * PRIO_FACTOR ; 
  pUKT->RunQueue[pUKT->RunQueueLen].Task = pTask;                       
  pUKT->RunQueue[pUKT->RunQueueLen].FromTask = pFromTask;               
  pUKT->RunQueue[pUKT->RunQueueLen].Req  = pReq;                        
  PRINT_ENQU(pTask, pFromTask, pReq) ;                                          
  pUKT->RunQueueLen++ ;                                                     

  if ((( pTask->MoveTimeCollectionEnabled ) || ( pTask->TimeCollectionEnabled )) && ( false == taskYield )) 
      pTask->TaskStateStat.runqueueEnqueueTime = RTESys_MicroSecTimer();        
  else
      pTask->TaskStateStat.runqueueEnqueueTime = 0;

  if (pUKT->RunQueueLen > pUKT->maxRunQueueLen)                       
  {                                                                             
      pUKT->maxRunQueueLen = pUKT->RunQueueLen;                         
  }                                                                             
  if (pUKT->RunQueueLen > pUKT->maxRunQueueLenSinceLastReset)         
  {                                                                             
      pUKT->maxRunQueueLenSinceLastReset = pUKT->RunQueueLen;           
  }                                                                             

  DBGOUT;
}

/*
 * ===========================================================================
 */

/*
  Instead of terminating the UKT thread, the UKT ends in an endless loop
 */
volatile int en71_neverStop = 1;

static void en71_StopUkt(SAPDB_Int4 uktIndex)
{
    char UKTNameBuf [20] ;
    sp77sprintf(UKTNameBuf, sizeof(UKTNameBuf), "UKT%d", uktIndex );
    MSGCD (( INFO_PROCESS_STOPPED, UKTNameBuf ));
    while ( en71_neverStop )
    {
        sqlyieldthread();
    }
}

/*
 * ===========================================================================
 */
static void en71_DoEnterDispatcherTiming( ten50_UKT_Control *this_ukt,
                                          struct TASK_TYPE  *pTaskCtrl )
{
    pTaskCtrl->TimeCollectionEnabled     = XPARAM(fTimeMeasure);
    pTaskCtrl->MoveTimeCollectionEnabled = XPARAM(uLoadBalancingCheck);

    if ( pTaskCtrl->MoveTimeCollectionEnabled || pTaskCtrl->TimeCollectionEnabled )
    {
        SAPDB_UInt8   timeNow = RTESys_MicroSecTimer();

        if ( TASK_IS_IN_RUN_QUEUE_EO00 == pTaskCtrl->is_in_queue )
        {
            /* if self dispatching, reset runqueueEnqueueTime */
            /* to prevent negaitiv differenz to dispatcherEnterTime */
            pTaskCtrl->TaskStateStat.runqueueEnqueueTime = timeNow;
        }
        pTaskCtrl->TaskStateStat.dispatcherEnterTime = timeNow;
    }
}

/*
 * ===========================================================================
 */
static void en71_DoLeaveDispatcherTiming(ten50_UKT_Control *this_ukt,
                                         struct TASK_TYPE  *pTaskCtrl )
{
    pTaskCtrl->TimeCollectionEnabled = XPARAM(fTimeMeasure);

    if  ( pTaskCtrl->MoveTimeCollectionEnabled || pTaskCtrl->TimeCollectionEnabled )
    {
        SAPDB_UInt8   timeNow = RTESys_MicroSecTimer();

        pTaskCtrl->TaskStateStat.dispatcherLeaveTime = timeNow;

        if ( pTaskCtrl->TaskStateStat.dispatcherEnterTime > pTaskCtrl->TaskStateStat.dispatcherLeaveTime )
        {
            MSGD (( ERR_INTERNAL_MICRO_TIMER_FAILED, 
                "Enter", pTaskCtrl->TaskStateStat.dispatcherEnterTime, 
                "Leave", pTaskCtrl->TaskStateStat.dispatcherLeaveTime )); /* --- we should never return here */
            /* System clock might be changed, Correct this leap!! */
            pTaskCtrl->TaskStateStat.dispatcherEnterTime = pTaskCtrl->TaskStateStat.dispatcherLeaveTime;
        }

        if ( pTaskCtrl->TaskStateStat.runqueueEnqueueTime )
        {
            SAPDB_UInt8         runnableMicroSeconds;
            SAPDB_UInt2         currUKTTimeVar = this_ukt->currTimeVar;

            if ( pTaskCtrl->TaskStateStat.runqueueEnqueueTime > pTaskCtrl->TaskStateStat.dispatcherLeaveTime )
            {
                MSGD (( ERR_INTERNAL_MICRO_TIMER_FAILED,
                    "Enqueue", pTaskCtrl->TaskStateStat.runqueueEnqueueTime, 
                    "Leave", pTaskCtrl->TaskStateStat.dispatcherLeaveTime )); /* --- we should never return here */
                /* System clock might be changed, Correct this leap!! */
                pTaskCtrl->TaskStateStat.runqueueEnqueueTime = pTaskCtrl->TaskStateStat.dispatcherLeaveTime;
            }

            /* PTS 1118094 problem with several requests pending... */
            if ( pTaskCtrl->TaskStateStat.dispatcherEnterTime > pTaskCtrl->TaskStateStat.runqueueEnqueueTime )
            {
                pTaskCtrl->TaskStateStat.runqueueEnqueueTime = pTaskCtrl->TaskStateStat.dispatcherEnterTime;
            }

            runnableMicroSeconds = timeNow - pTaskCtrl->TaskStateStat.runqueueEnqueueTime;

            if ( pTaskCtrl->lastTimeVar != currUKTTimeVar )
            {   /* since the last runninig state the counter variable has changed! */
                /* set the Tasks runnable time values */
                pTaskCtrl->runnableMicroSeconds[currUKTTimeVar]    = runnableMicroSeconds;
                pTaskCtrl->maxRunnableMicroSeconds[currUKTTimeVar] = runnableMicroSeconds;
                pTaskCtrl->lastTimeVar                             = currUKTTimeVar;
            }
            else
            {
                /* accumulate the tasks time waititng in the run queue to be scheduled */
                pTaskCtrl->runnableMicroSeconds[currUKTTimeVar] += runnableMicroSeconds;

                /* Has the tasks a new runnable time maximum? */
                if ( pTaskCtrl->maxRunnableMicroSeconds[currUKTTimeVar] < runnableMicroSeconds )
                    pTaskCtrl->maxRunnableMicroSeconds[currUKTTimeVar] = runnableMicroSeconds;
            }

            /* set the UKTs runnable time values */
            this_ukt->runnableMicroSeconds[currUKTTimeVar] += runnableMicroSeconds;

            /* long term move lock set? */
            if ( pTaskCtrl->moveLock.type.longTerm == 0 ) 
            { /* Is there a new UKTs maximum of the tasks time staying in the run queue? */
                if ( this_ukt->maxRunnableMicroSeconds[currUKTTimeVar] < runnableMicroSeconds )
                    this_ukt->maxRunnableMicroSeconds[currUKTTimeVar] = runnableMicroSeconds;
            }
        }
        else
        {
            pTaskCtrl->TaskStateStat.runqueueEnqueueTime = timeNow;
        }
    }
}
/*
 * ===========================================================================
 */

/* UKT statistic record is resetted independend from current setting of 'TimeEnabled' */
static void en71_HandleUKTStatisticReset( ten50_UKT_Control *this_ukt )
{
    if ( this_ukt->SleepStatRecResetPending )
    {
        SAPDB_UInt8   timeNow = RTESys_MicroSecTimer();

        memset(&this_ukt->UKTRunningStat, 0, sizeof(teo52_TaskSchedulerTime));
        memset(&this_ukt->UKTIOWaitStat, 0, sizeof(teo52_TaskSchedulerTime));
        this_ukt->RunningStateEnterMicroSeconds = 0;
#ifndef HAS_CONSOLE_PIPE
        /* If we would have a console pipe, the internal MicroSecTimer */
        /* could be used, but if we use shared memory to collect information */
        /* the MicroSecondTimer cannot be used, since it is process local. To */
        /* provide propper adjustment time() is used, which is identical for all processes */
        {
            struct timeval  tv ;
            (void) gettimeofday ( &tv , 0 );
            this_ukt->SysIdleTimeMeasureBeginMicroSeconds = tv.tv_sec * ((SAPDB_UInt8)1000000) + tv.tv_usec;
        }
#endif
        this_ukt->IdleTimeMeasureBeginMicroseconds = timeNow;
        this_ukt->maxRunQueueLenSinceLastReset     = 0;
        this_ukt->SleepStatRecResetPending         = false;
    }
}
