/*!
  @file           ven83.cpp
  @author         JoergM
  @brief          Kernel Runtime: Timer Thread
  @see            


  Working on jobs from timer queue.

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
#undef MOD__
#define MOD__ "ven83.c:"

#include "gen00.h"
#include "heo00.h"
#include "geo002.h"
#include "gen50.h"
#include "gen500.h"
#include "gen81.h"
#include "gen83.h"
#include "gen45.h"
#include "geo50_0.h"
#include "gen72.h"
#include "gen73.h"
#include "gen84.h"
#include "hen40.h"
#include "RunTime/Threading/RTEThread_ConsoleConnections.h"
#include "RunTime/RTE_KernelMain.h"
#include "gen71.h"
#include "gen72.h"
#include "gen73.h"
#include "gen84.h"
#include "hen50.h"
#include "Messages/Msg_OutputKnlDiag.h"
#include "RunTime/Tasking/RTETask_Task.hpp"

#define		UKT_WAKE_INTERVAL	3

/* imported functions */

/* local functions */

static  void                  en83_trigger ( ten50_UKT_Control *ukt );
#ifdef NEED_SUN_HEARTBEAT
static  void                  en83_maytrigger ( ten50_UKT_Control *ukt );
#endif /* NEED_SUN_HEARTBEAT */
static  void                  en83_activate ( struct DOUBLY_LINKED *quu );
static  int                   en83_tasks_started( struct TASK_TYPE *task);
static  struct DOUBLY_LINKED *en83_dequeue( time_t time_now, struct DOUBLY_LINKED **pNext_auftr);
static  time_t                en83HandleRequestTimeouts ();

/* imported data */

volatile SAPDB_Bool           en84_neverStop = true;

/* local data */

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"en83TimerThread"
/*ARGSUSED*/
extern "C" void * en83TimerThread ( void *arg )
{
    time_t                          time_now ;
    time_t                          sleep_time ;
    time_t                          nextApplicationTestTime ;
    time_t                          nextMoveTaskInterval    = 0;
    time_t                          loadBalancingCheck      = 0;
    time_t                          nextRequestTimeoutCheck = 0;
    struct TASK_TYPE               *tcb ;
    ten50_UKT_Control              *ukt ;
    struct DEV_PCB		    	   *devpcb ;
    struct DOUBLY_LINKED           *curr_auftr ;
    struct DOUBLY_LINKED           *next_auftr ;
    struct DOUBLY_LINKED           *head_auftr ;
#ifdef NEED_SUN_HEARTBEAT
    int	                            FSt_timer_count = UKT_WAKE_INTERVAL ;
#endif /* NEED_SUN_HEARTBEAT */
    tsp00_ErrText                   errtext;
    SAPDB_ULong                     lastConsoleScanTime;



    MSGD (( INFO_PROCESS_STARTED, "TIMER" ))

    KGS->timerSleepIntervals = 0;

    KGS->timer.start_time = KGS->current_time;
    nextApplicationTestTime = KGS->timer.start_time + MAXSLEEPTIME;

    DBG1 (( MF__,"start_time %ld \n", (long)KGS->timer.start_time ));

    lastConsoleScanTime = KGS->timer.start_time;

    /*
     *  wait on own semaphore forever
     */
    while (en84_neverStop)
	{
		KGS->timer.state = KT_RUNNING ;
		time_now = KGS->current_time;
        
    /* PTS 1107963 do no longer react on SERVER_STOP/KILL ... */
		/*==================================
		 * TIMER thread work queue handling
		 *==================================*/

	    next_auftr = NULL;
		head_auftr = en83_dequeue( time_now, &next_auftr );

		/*
		 *  head_auftr points to list of tasks to awaken or is NULL
		 */
		while ( head_auftr )
		{
			curr_auftr = head_auftr ;
			head_auftr = curr_auftr->pForward;
			en83_activate ( curr_auftr );
		}

        /* PTS 1113932 */
        /* set maximum sleep time */
	    sleep_time = time_now + MAXSLEEPTIME;

        /*==============================================
		 * Shorten sleep_time using next entry in TIMER work queue
		 *==============================================*/
		if ( next_auftr )
		{
			if ( next_auftr->args.timer_auftrag.time_to_wake < sleep_time )
            {   /* PTS 1113932 */
				sleep_time = next_auftr->args.timer_auftrag.time_to_wake ;
			}
		}

        /*==============================================
		 * Timeout scanner for console connections
		 *==============================================*/
        if (time_now - lastConsoleScanTime > CONNECTION_SCAN_INTERVALL)
        {
            /*SAPDBTRACE_WRITELN (Console_Trace, 1, "Starting timeout scanning");*/
            RTEThread_ConsoleScanConnectionsForTimeout ();
            lastConsoleScanTime = time_now;
        }


		/*==============================================
		 * Before going to sleep wake all UKPs with timeout
		 * Reduce sleep_time if possible
		 *==============================================*/

		for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
		{
			if ( ukt->ukp_timeout != 0
			  && ukt->ukp_timeout <= time_now )
			{
				en83_trigger( ukt );
			}
			else
			{
				if ( ukt->ukp_timeout < sleep_time )
				{
					sleep_time = ukt->ukp_timeout ;
				}
			}
		}

#ifdef NEED_SUN_HEARTBEAT
		/*==============================================
		 *  FSt regularily wake all UKPs and DEVs
		 *==============================================*/
		if ( ! -- FSt_timer_count )
		{
			FSt_timer_count = UKT_WAKE_INTERVAL ;

			for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
			{
				en83_trigger( ukt );
			}

			for ( devpcb = KGS->first_dev ; devpcb <= KGS-> last_dev ; devpcb ++ )
			{
				if ( devpcb->state != KT_INITIALIZED )
				{
					en83_maytrigger( (ten50_UKT_Control *)devpcb );
				}
			}
		}

#endif /* NEED_SUN_HEARTBEAT */

		/*==============================================
		  Check for DATABASE ADMIN state reached and inform
		  starter process if so, otherwise reduce sleep_time 
		  to 1 second...

		  FH. if maxserverdb < 2 we don't need SENDER and RECEIVER task
		  ( see ven70c ) 
		 *==============================================*/

		if ( KGS-> state == SERVER_STARTING )
		{
#ifdef DEBUG_RTE
            if (KGS->ti->state != TSK_SLEEP_EO00)
            DBG1 (( MF__,"ti state is %d wait for %d\n", KGS->ti->state, TSK_SLEEP_EO00 ));
			if (KGS->al->state != TSK_VSUSPEND_EO00)
            DBG1 (( MF__,"al state is %d wait for %d\n", KGS->al->state, TSK_VSUSPEND_EO00 ));
			if (KGS->tw->state != TSK_VSUSPEND_EO00)
            DBG1 (( MF__,"tw state is %d wait for %d\n", KGS->tw->state, TSK_VSUSPEND_EO00 ));
			if (KGS->ut->state != TSK_CONNECTWAIT_EO00)
            DBG1 (( MF__,"ut state is %d wait for %d\n", KGS->ut->state, TSK_CONNECTWAIT_EO00 ));
#endif
            if (KGS-> ti->state == TSK_SLEEP_EO00
			 && KGS-> al->state == TSK_VSUSPEND_EO00
			 && KGS-> tw->state == TSK_VSUSPEND_EO00
			 && en83_tasks_started ( KGS-> first_datawriter ) 
			 && en83_tasks_started ( KGS-> first_server )
			   )
			{
                RTE_SetDatabaseState(SERVER_ADMIN_EO00);
				/*
				 *  Enable the CONSOLE process to start the wrap around of the
				 *  diag file.
				 */
#ifdef NEW_DIAG_FILES  
                Msg_StartupPhaseCompleted();
#endif
				MSGCD (( INFO_TIMER_START_COMPLETE )) 

				/*
				 *  Notify starter process that startup seems OK
				 */
                en81NotifyStarter();
			}
			else
			{
				/* each second look if all special tasks are ready */
				sleep_time = time_now + 1 ;
				DBG1 (( MF__, "sleep only 1 second during startup" ));
			}
		}
        else
		{
            if ( nextApplicationTestTime <= time_now )
            {   /* PTS 1113932 */
                nextApplicationTestTime += MAXSLEEPTIME;

                /*============================================================
			     *  If not SERVER_STARTING:
			     *  test if all connections are still alive.
			     *  Utility task + all user tasks + all receivers are tested
		 	     *===========================================================*/
			    DBG1 (( MF__,"Test applications\n" ));

                e84_test_application ( KGS-> ut ) ;
			    for ( tcb = KGS-> first_user ; tcb <= KGS-> pLastTaskCtrl; tcb ++ )
			    {
				    e84_test_application ( tcb );
			    }
                for ( tcb = KGS-> first_event_task ; 
                        (tcb != NULL) && (tcb <= KGS-> first_backup_task) ; tcb ++ )
                {
		            if ( tcb->connection ) 
			              e84_test_application ( tcb );
                }
            }

            if (  XPARAM(uLoadBalancingCheck) != 0 )
            {
                if ( loadBalancingCheck != XPARAM(uLoadBalancingCheck))
                { /* XPARAM(uLoadBalancingCheck) value has changed. */
                    /* From now we will use the new one */
                    loadBalancingCheck   = XPARAM(uLoadBalancingCheck);
                    nextMoveTaskInterval = time_now + loadBalancingCheck;
                }
                if ( nextMoveTaskInterval <= time_now )
                {                         
                    time_t  waitTime = en71LoadBalancing();

                    nextMoveTaskInterval = time_now + waitTime;
                }

                if ( sleep_time >  nextMoveTaskInterval )
                    sleep_time = nextMoveTaskInterval;
            }

            if ( nextRequestTimeoutCheck <= time_now )
            {                         
                nextRequestTimeoutCheck = time_now + en83HandleRequestTimeouts ();

                if ( sleep_time > nextRequestTimeoutCheck )
                    sleep_time = nextRequestTimeoutCheck;
            }
		}

        if ( sleep_time > time_now )
        {
		    DBG1 (( MF__,"SLEEP (max %ld seconds) \n", (long)(sleep_time - time_now) ));

            KGS->timerCurrentWakeupTime = sleep_time;
            KGS->timerCurrentSleepInterval = (SAPDB_Long)(sleep_time - time_now);
		    KGS->timer.state = KT_SLEEPING ;

            en81TimerThreadSleep( (long)(sleep_time - time_now) );
            if ( time_now != KGS->current_time )
            {
                ++(KGS->timerSleepIntervals);
            }
        }
	}
    /* PTS 1107963 */
    /*NOTREACHED*/
    return NULL;
}

/*
 * ======================================================================
 */

#undef MF__
#define MF__ MOD__"en83_tasks_started"
static int en83_tasks_started ( 
struct TASK_TYPE                *task)
{
    int task_type = task->type ;
    int tasks_started ;
    DBG1 (( MF__,"check if tasktype %d started", task_type ));
    do
    {
        tasks_started = task->state == TSK_VSUSPEND_EO00 ;
		DBG1 (( MF__,"check T%d : %s started",
                   task->index, tasks_started ? "" : "not ")) ;
        task++ ;
    } while ( task->type == task_type && tasks_started ) ;

    return ( tasks_started ) ;
}

/*
 * ======================================================================
 */

#undef MF__
#define MF__ MOD__"en83HandleRequestTimeouts"
static  time_t en83HandleRequestTimeouts ()
{
  struct TASK_TYPE               *tcb ;

  for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
    tcb->pTaskObject->HandleRequestTimeout();

  return HANDLE_REQUEST_TIMEOUT_INTERVAL;
}


/*
 * ===========================================================================
 */

extern "C" void SetTrigger(void **trigger_pointer);

#undef MF__
#define MF__ MOD__"en83TimerJob"
extern "C" void en83TimerJob ( 
struct  DOUBLY_LINKED         * quu )
{
struct  DOUBLY_LINKED         * this_quu ;
struct  DOUBLY_LINKED         * next_quu ;
tsp00_Bool                         wake ;

	DBGIN;

	DBG1 (( MF__,"quu 0x%08x \n", quu ));
    DBG1 (( MF__,"Task T%ld time_to_wake %ld\n", 
			(long)quu->taskid->index, quu->args.timer_auftrag.time_to_wake ));

	wake  = FALSE ;

	WAIT_UNTIL_ALONE ( KGS-> timer.work.dq_exclusive );

	if ( KGS-> timer.work.first == NULL )
    {
		KGS-> timer.work.first  = quu ;
		quu->pForward           = NULL ;
		wake                    = TRUE ;
    }
	else
    {
		this_quu = KGS-> timer.work.first ;
		if ( this_quu->args.timer_auftrag.time_to_wake >
			  quu->args.timer_auftrag.time_to_wake   )
		{
			/*
			 * First entry new - wake
			 */
			wake                     = TRUE ;
			quu->pForward            = this_quu ;
			KGS-> timer.work.first   = quu ;
		}
		else
		{
			int found = FALSE;
			this_quu = KGS-> timer.work.first ;
			next_quu = this_quu->pForward;
			while ( ! found )
			{
				if ( ! next_quu )
				{
					found              = TRUE ;
					quu->pForward      = next_quu ;
					this_quu->pForward = quu ;
					break ;
				}

				if ( next_quu->args.timer_auftrag.time_to_wake >
					  quu->args.timer_auftrag.time_to_wake   )
				{
					found              = TRUE ;
					quu->pForward      = next_quu ;
					this_quu->pForward = quu ;
					break ;
				}

				this_quu = next_quu ;
				next_quu = this_quu->pForward;
			}
		}
    }

	CLEARLOCK ( KGS-> timer.work.dq_exclusive );

	if ( wake )
    {
		en81TimerThreadWakeup();
    }

	DBGOUT;
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"en83_activate"
static  void en83_activate ( 
	struct  DOUBLY_LINKED * quu )
{
    ten50_UKT_Control * ukt ;

    DBGIN;

	DBG1 (( MF__,"quu 0x%p \n", quu ));
	DBG1 (( MF__," time    %ld \n", (long)quu->args.timer_auftrag.time_to_wake ));
	DBG1 (( MF__," task    T%d \n", (int)quu->taskid->index ));
	DBG1 (( MF__," thread  %ld \n", (long)quu->taskid->ukt->tid ));

	ukt = quu->taskid->ukt ;
	e73_ioc_enqu( & ukt->ioc_queue , quu );
	e72_wake ( ukt );

	DBGOUT;
}

/*
  Function: en83_dequeue
  Description: Dequeue all entries with time less or equal time_now from timer.work queue
  'forward' Links remain intact, but last 'forward' link is reset to NULL.
  Returns NULL if no job waiting...
  Arguments: time_now [in] timestamp to use
             pNextAuftr [out] returns pointer to first entry that has sleep a little bit longer
  Return value: Pointer to list of entries ready for wakeup or NULL if nothing to wakeup
 */
#undef MF__
#define MF__ MOD__"en83_dequeue"
static  struct DOUBLY_LINKED *en83_dequeue( time_t time_now,
    										struct DOUBLY_LINKED **pNextAuftr )
{
    struct DOUBLY_LINKED            *head_auftr ;
    struct DOUBLY_LINKED            *curr_auftr ;
    struct DOUBLY_LINKED            *next_auftr ;

    DBGIN;

    curr_auftr = NULL;
    next_auftr = NULL;

    WAIT_UNTIL_ALONE ( KGS-> timer.work.dq_exclusive );

    head_auftr = KGS-> timer.work.first ;
    if ( head_auftr )
    {
        if ( head_auftr->args.timer_auftrag.time_to_wake > time_now )
		{
			next_auftr = head_auftr; /* PTS 1107612 */
			head_auftr = NULL ;
	    }
	    else
	    {
			curr_auftr = head_auftr;
			next_auftr = curr_auftr->pForward;
			while  ( next_auftr )
			{
				if ( next_auftr->args.timer_auftrag.time_to_wake > time_now )
				{
					break;
				}
				curr_auftr = next_auftr ;
				next_auftr = curr_auftr->pForward;
			}
			KGS-> timer.work.first = next_auftr ;
		}
    }

    CLEARLOCK ( KGS-> timer.work.dq_exclusive );

    *pNextAuftr = next_auftr;
	if ( curr_auftr ) curr_auftr->pForward = NULL;

    DBGOUT;

    return head_auftr;
}

#undef MF__
#define MF__ MOD__"en83_trigger"
static void en83_trigger( 
ten50_UKT_Control *ukt)
{
	int rc;
	union semun   arg ;

	DBGIN;
	/*
	 *  Use semctl (setval) here periodically to prevent
	 *  the semaphore from overflow
	 */
	DBG1 (( MF__,"setting thread %ld semid %d state %d\n", 
			(long)ukt->tid, ukt->semid, ukt->state ));
	arg.val = 1 ;
	rc = semctl ( ukt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
	if ( rc != 0 )
	{
#define ERR_TIMER_BAD_TRIGGER 11036,IERR_TYPE,"timer   ","ABEND: bad trigger forthread %ld state %d semid %d failed: %s"
		MSGCD (( ERR_TIMER_BAD_TRIGGER, ukt->tid, ukt->state, ukt->semid, sqlerrs() ));
		vabort ( WRITE_CORE );
	}

	DBGOUT;
	return;
}

#undef MF__
#define MF__ MOD__"en83_maytrigger"
static void en83_maytrigger( 
ten50_UKT_Control *ukt )
{
	union semun   arg ;

	DBGIN;
	/*
	 *  Use semctl (setval) here periodically to prevent
	 *  the semaphore from overflow
	 */
	DBG1 (( MF__,"setting thread %ld semid %d state %d\n", 
			(long)ukt->tid, ukt->semid, ukt->state ));
	arg.val = 1 ;
	(void)semctl ( ukt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );

	DBGOUT;
	return;
}
