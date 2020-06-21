/*!
  @file           ven56.c
  @author         JoergM
  @brief          Kernel Runtime: Scheduling functions
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

#define MOD__  "ven56.c:"

#include "heo56.h"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "geo00_2.h"
#include "heo52.h"
#include "geo002.h"

#include "gen71.h"
#include "gen83.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "hen50.h"


#if defined (SAPDB_SLOW) || defined (SAPDB_QUICK)
# define MULTIPLE_SIGNAL_CHECK     1
#endif

extern  int                             e60_dbgdbglvl ;

#if ! ( defined(_IBMR2) || defined(OSF1) || defined(NMP)|| defined(FREEBSD) )	/* already declared in include file */
#   if defined(HP9) || defined(MI_ULT)
    extern  clock_t                         times ();
#   else
    extern  long                            times ();
#   endif
#endif  /* _IBMR2 */

/* local functions */

static  void    en56_remvresume (void *pDummy,
                                 struct TASK_TYPE *FromTask );
static  void    en56_remvsignal ( void *paraOK,
								 struct TASK_TYPE *FromTask );

static void en56_StoreResumer( struct TASK_TYPE                * tcb ,
							   tsp00_TaskId                      pid,
							   tsp00_Int4						 allowedResCount );

/*
 * ===========================================================================
 */
  #undef  MF__
  #define MF__ MOD__"vstop"
externC void vstop(tsp00_TaskId pid)
{
#undef  MF__
#define MF__ MOD__"vstop"

    struct TASK_TYPE * pTaskCtrl;
    ten50_UKT_Control * pUkt;

	DBGIN_T(pid);

    PID_TCB(pid, pTaskCtrl);

    pUkt = pTaskCtrl->ukt;

    pTaskCtrl->state = TSK_STOPPED_EO00;

    WAIT_UNTIL_ALONE( pTaskCtrl->exclusive );

    --(pTaskCtrl->lVStopCounter);

    if ( pTaskCtrl->lVStopCounter == -1 )
    {
        CLEARLOCK( pTaskCtrl->exclusive );
        GOTO_DISP(&pUkt);
        if ( pUkt->curr_quu != GetTaskDLQElement1( pTaskCtrl ))
        {
            MSGALL (( ERR_VSTOP_WRONG_EVENT ));
            vabort(WRITE_CORE);
        }

        if ( pUkt->curr_quu->req_type != REQ_VSTOP_EO00 )
        {
            MSGALL (( ERR_VSTOP_WRONG_REQ, pUkt->curr_quu->req_type ));
            vabort(WRITE_CORE);
        }
    }
    else
    {
        CLEARLOCK( pTaskCtrl->exclusive );
    }

    pTaskCtrl->state = TSK_RUNNING_EO00;

    DBGOUT_T(pTaskCtrl->index);
}

/* ----------------------------------- */

  #undef  MF__
  #define MF__ MOD__"vcontinue"
externC void vcontinue(tsp00_TaskId pid)
{
#undef  MF__
#define MF__ MOD__"vcontinue"
    struct TASK_TYPE * pReqTaskCtrl = KGS->pFirstTaskCtrl + pid - 1;

    DBGIN;

    WAIT_UNTIL_ALONE( pReqTaskCtrl->exclusive );

    ++(pReqTaskCtrl->lVStopCounter);

    if ( pReqTaskCtrl->lVStopCounter == 0 )
    {
        struct DOUBLY_LINKED *request;

        CLEARLOCK( pReqTaskCtrl->exclusive );

        request =  GetTaskDLQElement1( pReqTaskCtrl );
        request->req_type = REQ_VSTOP_EO00;

        /* Insert request in IOC Queue */
        en71_iocEnquAndWake( pReqTaskCtrl->ukt, pReqTaskCtrl, request );
    }
    else
    {
        CLEARLOCK( pReqTaskCtrl->exclusive );
    }
    DBGOUT;
}

/*
 * ===========================================================================
 */

void    vsuspend ( tsp00_TaskId pid ,
				   tsp00_Int2  suspend_reason )
{
#undef  MF__
#define MF__ MOD__"vsuspend"

    struct TASK_TYPE * pTaskCtrl;

	DBGIN_T(pid);

    PID_TCB(pid, pTaskCtrl);

    DBG1((MF__,"T%d suspending T%d cnt %d",
		pTaskCtrl->index , (int) pid ,
		pTaskCtrl->resume_count))

        pTaskCtrl->resume_count--;

    if ( pTaskCtrl->resume_count < -1 )
    {
        int i;
        int j0;
        MSGD (( ERR_WRONG_RESUME_COUNT, pTaskCtrl->index, pTaskCtrl->resume_count ));

        i = pTaskCtrl->totalResumerCount;
        j0 = MAX_RESUMER_PIDS;

        while( (i-- > 0) && (j0-- > 0) )
        {
            MSGD (( WRN_RESUMER_HISTORY, pTaskCtrl->resumerPid[i%MAX_RESUMER_PIDS]));
        }
        vabort( WRITE_CORE );
    }

    if ( pTaskCtrl->resume_count < 0 )
	{
        ten50_UKT_Control * this_ukt;

        this_ukt = pTaskCtrl->ukt;

		pTaskCtrl->state = TSK_VSUSPEND_EO00;

        if ( suspend_reason < MX_SUSPEND_REASONS_EO00 )
		    KGS->SuspendReason [ suspend_reason ] ++ ;

		pTaskCtrl->state_reason = suspend_reason ;
		pTaskCtrl->TaskStateStat.VsuspStat.TotalTimeStat.ulCount++;
		pTaskCtrl->TaskStateStat.VsuspStat.TimeStat.ulCount++;
		DBG1 (( MF__,"before goto_disp T%d \n", pTaskCtrl->index ));

        GOTO_DISP (&this_ukt);

        if ( this_ukt->curr_quu != GetTaskDLQElement1( pTaskCtrl ) )
        {
            MSGALL (( ERR_AWOKE_FROM_WRONG_EVENT, "vsuspend", this_ukt->curr_task->index, this_ukt->curr_quu, GetTaskDLQElement1( pTaskCtrl ) ));
            MSGALL (( ERR_VSUSPEND_WRONG_REQ_ARGS,
	  		          this_ukt->curr_task->index,
                      this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
			          this_ukt->curr_quu->req_type, REQ_VRESUME_EO00 ));

            vabort ( WRITE_CORE );
        }
        if ( this_ukt->curr_quu->req_type != REQ_VRESUME_EO00 )
        {
            MSGALL (( ERR_VSUSPEND_WRONG_REQ ));
            MSGALL (( ERR_VSUSPEND_WRONG_REQ_ARGS,
	  		          this_ukt->curr_task->index,
                      this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
			          this_ukt->curr_quu->req_type, REQ_VRESUME_EO00 ));
            vabort ( WRITE_CORE );
        }

        if ( pTaskCtrl->TimeCollectionEnabled )
        {
            eo52UpdateTaskStateStatisticRec( &pTaskCtrl->TaskStateStat.VsuspStat, &pTaskCtrl->TaskStateStat );
        }
		pTaskCtrl->state_reason = 0 ;
		DBG1 (( MF__,"after  goto_disp T%d \n", pTaskCtrl->index ));
		pTaskCtrl->state = TSK_RUNNING_EO00 ;
	}

	pTaskCtrl->lWaitForRoot = pTaskCtrl->lWaitForTask = undef_ceo00  ;

	DBG1(( MF__,"T%d suspendend T%d cnt %d",
			pTaskCtrl->index , (int) pid ,
			pTaskCtrl->resume_count))
	DBGOUT_T(pTaskCtrl->index)
}

/*
 * ===========================================================================
 */

void   vSuspendForPageLock ( tsp00_TaskId      pid,
							 tsp00_Int2        suspend_reason,
                             tsp00_Int4        page_no )
{
#undef  MF__
#define MF__ MOD__"vSuspendForPageLock"
    struct TASK_TYPE * pTaskCtrl;

	DBGIN_T(pid);

    PID_TCB(pid, pTaskCtrl);

    pTaskCtrl->lockedPageNo = page_no;

    vsuspend(pid, suspend_reason);

    pTaskCtrl->lockedPageNo = MAX_INT4_SP00;

    DBGOUT_T(pTaskCtrl->index);
}

/*
 * ===========================================================================
 */

tsp00_Bool visresume ( tsp00_TaskId pid )
{
#undef  MF__
#define MF__ MOD__"visresume"

    struct TASK_TYPE * pTaskCtrl;
    tsp00_Bool IsResume ;

	DBGIN_T(pid);

    PID_TCB(pid, pTaskCtrl);

    IsResume = ( pTaskCtrl->resume_count > 0 );

	DBGOUT_T(pTaskCtrl->index);

    return(IsResume);
}

/*
 * ===========================================================================
 */
void    vresume ( tsp00_TaskId pid )
{
#undef  MF__
#define MF__ MOD__"vresume"
  ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
  struct TASK_TYPE                * tcb ;
  struct DOUBLY_LINKED            * lquu ;


  DBGIN_T(this_ukt->curr_task->index)

  if ( pid < 1 || pid > kgs->ulNumOfTasks )
  {
    MSGCD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, pid ));
    vabort ( WRITE_CORE );
  }

  /* Cannot resume myself... */
  if ( this_ukt->curr_task->index == pid )
  {
    MSGALL (( ERR_VXXXX_WRONG_TASK, "vresume", pid ));
    vabort( WRITE_CORE ) ;
  }

  tcb = kgs->pFirstTaskCtrl + pid - 1 ;

  DBG1((MF__,"T%d resuming   T%d cnt %d",
	this_ukt->curr_task->index , (int) pid ,
	tcb->resume_count ))

    if ( tcb->ukt == this_ukt )
    {
		tsp00_Bool  IsSuspended = tcb->state == TSK_VSUSPEND_EO00;

        DBG1 (( MF__,"resume local task \n"));
        DBG1 (( MF__,"act resume_count = %d\n",
            tcb->resume_count ));

        tcb->resume_count++ ;

		en56_StoreResumer(tcb, this_ukt->curr_task->index, IsSuspended ? 0 : 1 );

        DBG1 (( MF__,"new resume_count = %d\n", tcb->resume_count));

        if ( tcb->resume_count == 0 )
        {
            if ( IsSuspended )
            {
                lquu = GetTaskDLQElement1( tcb );
                lquu->req_type = REQ_VRESUME ;

                e71_EnquRun ( this_ukt, tcb, this_ukt->curr_task, lquu, BASE_PRIO_RAV );

                if ( tcb->prio_flag )
                  tcb->ulDynPrio +=  PRIO_FACTOR;

                if ( this_ukt->curr_task->excl_schachtel == 0 &&
                    !this_ukt->curr_task->prio_flag && tcb->prio_flag )
                {
                    struct DOUBLY_LINKED *request = GetTaskDLQElement1( this_ukt->curr_task );
                    request->req_type = REQ_RESCHEDULE_EO00;
                    e71_YieldTaskRunEnqu ( this_ukt, this_ukt->curr_task, request, BASE_PRIO_REX );
                    this_ukt->curr_task->Totalself_susp_cnt++ ;
                    this_ukt->curr_task->self_susp_cnt++ ;
                    GOTO_DISP (&this_ukt);
                }
            }
            else
			{
                MSGALL (( ERR_VRESUME_WRONG_STATE ,
                    this_ukt->curr_task->index , tcb->state ))
            }

        }
    }
    else
    {
        DBG1 (( MF__,"resume remote task T%d \n", tcb->index ));
        en71EnqueueUkt2UktAndWake ( this_ukt, tcb, this_ukt->curr_task,  en56_remvresume, (void *)NULL);
    }

	DBG1((MF__,"T%d resumed    T%d cnt %d",
				this_ukt->curr_task->index , (int) pid ,
				tcb->resume_count))
	DBGOUT_T(this_ukt->curr_task->index)
}

/*
 * ===========================================================================
 */

void    vwait ( tsp00_TaskId pid,
		        tsp00_Bool  *ok )
{
#undef  MF__
#define MF__ MOD__"vwait"
  ten50_UKT_Control * this_ukt;
  struct TASK_TYPE  * tcb;
  SAPDB_Bool          isSignaled;

  DBGIN_T(pid);

#if defined (MULTIPLE_SIGNAL_CHECK)
  this_ukt = THIS_UKT_CTRL;
  tcb      = this_ukt->curr_task;

  if ( pid != tcb->index )
  {
      MSGD (( ERR_VXXXX_WRONG_TASK, "vwait", pid ));
      vabort ( WRITE_CORE ) ;
  }
#else
  PID_TCB(pid, tcb);

  this_ukt = tcb->ukt;
#endif

  this_ukt->ActTasksVwait++ ;

  tcb->state = TSK_VWAIT_EO00 ;
  tcb->TaskStateStat.VwaitStat.TotalTimeStat.ulCount++;
  tcb->TaskStateStat.VwaitStat.TimeStat.ulCount++;
  /*IST tcb->signal_count -- ; */
  /*IST if ( tcb->signal_count < 0 ) */

#if defined (MULTIPLE_SIGNAL_CHECK)
  WAIT_UNTIL_ALONE( tcb->exclusive );
   isSignaled = tcb->vsignal_ok != 0;
  CLEARLOCK( tcb->exclusive );
#else
   isSignaled = tcb->vsignal_ok != 0;
#endif

  if ( !isSignaled )
  {
      DBG1 (( MF__,"before goto_disp T%d \n", tcb->index ));
      GOTO_DISP (&this_ukt);
      if ( tcb->TimeCollectionEnabled )
      {
          eo52UpdateTaskStateStatisticRec( &tcb->TaskStateStat.VwaitStat, &tcb->TaskStateStat );
      }

      if ( this_ukt->curr_quu != GetTaskDLQElement1( tcb ) )
      {
          MSGALL (( ERR_AWOKE_FROM_WRONG_EVENT, "vsignal", this_ukt->curr_task->index, this_ukt->curr_quu, GetTaskDLQElement1( tcb ) ));
          MSGALL (( ERR_VWAIT_WRONG_REQ_ARGS,
                    this_ukt->curr_task->index,
                    this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
                    this_ukt->curr_quu->req_type, REQ_VSIGNAL_EO00 ));

          vabort ( WRITE_CORE );
      }
      if ( this_ukt->curr_quu->req_type != REQ_VSIGNAL_EO00 )
      {
          MSGALL (( ERR_VWAIT_WRONG_REQ ));
          MSGALL (( ERR_VWAIT_WRONG_REQ_ARGS,
                    this_ukt->curr_task->index,
                    this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
                    this_ukt->curr_quu->req_type, REQ_VSIGNAL_EO00 ));
          vabort ( WRITE_CORE );
      }

      DBG1 (( MF__,"after  goto_disp T%d \n", pid ));
  }

#if defined (MULTIPLE_SIGNAL_CHECK)
  WAIT_UNTIL_ALONE( tcb->exclusive );
#endif

  switch ( tcb->vsignal_ok )
  {
    case  1 : * ok = 1 ; break ;
    case -1 : * ok = 0 ; break ;
    default : 
#             if defined (MULTIPLE_SIGNAL_CHECK)
               CLEARLOCK( tcb->exclusive );
               MSGALL (( ERR_VWAIT_ILL_BOOL_MPS, tcb->vsignal_ok , pid, tcb->lastSignalTID )) ;
#             else
               MSGALL (( ERR_VWAIT_ILL_BOOL, tcb->vsignal_ok , pid )) ;
#             endif
    	      vabort ( WRITE_CORE ) ;
              break;
  }
  
  tcb->vsignal_ok = 0 ;

#if defined (MULTIPLE_SIGNAL_CHECK)
  CLEARLOCK( tcb->exclusive );
#endif
  tcb->lWaitForTask = undef_ceo00  ;
  tcb->state = TSK_RUNNING_EO00 ;
  this_ukt->ActTasksVwait-- ;

  DBGOUT_T(pid);
}

/*
 * ===========================================================================
 */

void vsignal ( tsp00_TaskId pidToSignal ,
	             tsp00_Bool   ok )
{
#undef  MF__
#define MF__ MOD__"vsignal"
  ten50_UKT_Control    * this_ukt  = THIS_UKT_CTRL;
  struct TASK_TYPE     * tcbCaller = this_ukt->curr_task;
  struct TASK_TYPE     * tcbToSignal ;
  struct DOUBLY_LINKED * lquu ;

  DBGIN_T (tcbCaller->index);

  if ( pidToSignal < 1 || pidToSignal > kgs->ulNumOfTasks )
  {
    MSGCD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, pidToSignal ));
    vabort ( WRITE_CORE );
  }
  tcbToSignal = kgs->pFirstTaskCtrl + pidToSignal - 1 ;

#if defined (MULTIPLE_SIGNAL_CHECK)
  WAIT_UNTIL_ALONE( tcbToSignal->exclusive );

  if ( tcbToSignal->vsignal_ok != 0 )
  {
      CLEARLOCK( tcbToSignal->exclusive );
      MSGALL(( ERR_MULTIPLE_VSIGNAL_CALL_MPS, tcbToSignal->index,
               tcbCaller->index, tcbToSignal->lastSignalTID  ));
      vabort ( WRITE_CORE );
  }

  tcbToSignal->lastSignalTID = tcbCaller->index;
  tcbToSignal->vsignal_ok    = ( ok ? 1 : -1 );

  CLEARLOCK( tcbToSignal->exclusive );
#endif

  if ( tcbToSignal->ukt == this_ukt )
  {
#   if !defined (MULTIPLE_SIGNAL_CHECK)

    if ( tcbToSignal->vsignal_ok != 0 )
    {
      MSGALL(( ERR_MULTIPLE_VSIGNAL_CALL, tcbToSignal->index ));
      vabort(WRITE_CORE); 
    }

    tcbToSignal->vsignal_ok = ( ok ? 1 : -1 );

#   endif

    DBG1 (( MF__,"signal local task \n" ));
    /*IST     tcbToSignal->signal_count ++ ; */
    if ( tcbToSignal->state == TSK_VWAIT_EO00 )
    {
      lquu = GetTaskDLQElement1( tcbToSignal );
      lquu->req_type = REQ_VSIGNAL_EO00 ;

      e71_EnquRun ( this_ukt, tcbToSignal, this_ukt->curr_task, lquu, BASE_PRIO_RAV );

      if ( tcbToSignal->prio_flag )
          tcbToSignal->ulDynPrio +=  PRIO_FACTOR;

      /*
         Dispatch yourself, if other task has a higher priority
         and calling task holds no region
       */
      if ( tcbCaller->excl_schachtel == 0
        && !tcbCaller->prio_flag
        && tcbToSignal->prio_flag )
      {
        struct DOUBLY_LINKED *request = GetTaskDLQElement1( tcbCaller );
        request->req_type = REQ_RESCHEDULE_EO00;
        e71_YieldTaskRunEnqu ( this_ukt, tcbCaller, request, BASE_PRIO_REX );
        tcbCaller->Totalself_susp_cnt++ ;
        tcbCaller->self_susp_cnt++ ;
        GOTO_DISP (&this_ukt);
      }
    }
  }
  else
  {
    en71EnqueueUkt2UktAndWake ( this_ukt, tcbToSignal , this_ukt->curr_task,
                                en56_remvsignal, (void *)(((char *)0)+ ok) );
  }

  DBGOUT_T (tcbCaller->index);
}

/*------------------------------*/

void    vsleep ( tsp00_TaskId  pid,
				 tsp00_Int2    limit )
{
#undef  MF__
#define MF__ MOD__"vsleep"
  ten50_UKT_Control    * this_ukt;
  struct TASK_TYPE     * tcb;
  struct DOUBLY_LINKED * lquu ;
  time_t                 time_now ;
  teo00_Int4             BegSec, BegMicroSec, EndSec, EndMicroSec; /* PTS 1110250 */

  DBGIN_T(pid);

  PID_TCB(pid, tcb);
  this_ukt = tcb->ukt;

  tcb->state = TSK_SLEEP_EO00 ;
  tcb->TaskStateStat.VsleepStat.TotalTimeStat.ulCount++;
  tcb->TaskStateStat.VsleepStat.TimeStat.ulCount++;

  if ( limit == 0 )
  {
  /*
   *  vsleep ( 0 ) reschedules only
   *  append myself to the rex queue (the least priority queue)
   */
    lquu = GetTaskDLQElement1( tcb );;
    lquu->req_type = REQ_RESCHEDULE_EO00 ;
    lquu->taskid   = tcb ;

    e71_YieldTaskRunEnqu ( this_ukt, tcb, lquu, BASE_PRIO_REX );
    if ( tcb->prio_flag )
        tcb->ulDynPrio +=  PRIO_FACTOR;

    DBG1 (( MF__,"before goto_disp T%d \n", pid ));
    GOTO_DISP (&this_ukt);
    if ( tcb->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &tcb->TaskStateStat.VsleepStat, &tcb->TaskStateStat );
    }
    DBG1 (( MF__,"after  goto_disp T%d \n", pid ));
  }
  else
  {
    DBG1 (( MF__,"sleep %d seconds \n", limit ));
    lquu = GetTaskDLQElement1( tcb );
    lquu->req_type = REQ_VSLEEP_EO00;
    lquu->taskid   = tcb;
    time_now = KGS->current_time;
    lquu->args.timer_auftrag.time_to_wake = time_now + limit ;
    en83TimerJob ( lquu );

    DBG1 (( MF__,"before goto_disp T%d \n", pid ));
    GOTO_DISP (&this_ukt);
    if ( tcb->TimeCollectionEnabled )
    {
        eo52UpdateTaskStateStatisticRec( &tcb->TaskStateStat.VsleepStat, &tcb->TaskStateStat );
    }
    DBG1 (( MF__,"after  goto_disp T%d \n", pid ));

    if ( this_ukt->curr_quu != lquu )
    {
        MSGALL (( ERR_AWOKE_FROM_WRONG_EVENT, "vsleep", this_ukt->curr_task->index, this_ukt->curr_quu, lquu ));
        MSGALL (( ERR_VSLEEP_WRONG_REQ_ARGS,
            this_ukt->curr_task->index,
            this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
            this_ukt->curr_quu->req_type, REQ_VSLEEP_EO00 ));

        vabort ( WRITE_CORE );
    }
    if ( this_ukt->curr_quu->req_type != REQ_VSLEEP_EO00 )
    {
        MSGALL (( ERR_VSLEEP_WRONG_REQ ));
        MSGALL (( ERR_VSLEEP_WRONG_REQ_ARGS,
            this_ukt->curr_task->index,
            this_ukt->curr_quu->taskid  ? this_ukt->curr_quu->taskid->index : 0,
            this_ukt->curr_quu->req_type, REQ_VSLEEP_EO00 ));
        vabort ( WRITE_CORE );
    }
  }
		
  tcb->state = TSK_RUNNING_EO00 ;
  DBGOUT_T (pid)
}

/*------------------------------*/

void  vreschedule_msec     ( tsp00_TaskId      pid,
                             tsp00_Int4        *StartMsec,
                             tsp00_Int4        *RemainMsec )

{
#undef  MF__
#define MF__ MOD__"vreschedule_msec"
  ten50_UKT_Control *   this_ukt;
  struct TASK_TYPE  *   tcb;
  struct DOUBLY_LINKED* request;
  SAPDB_Int4            time_now_in_msec;

  DBGIN_T(pid);

  PID_TCB(pid, tcb);

  this_ukt = tcb->ukt;

  tcb->state = TSK_RESCHEDULE_MSEC_EO00;

  if ( *StartMsec == 0 )
    *StartMsec = (SAPDB_Int4)(RTESys_MicroSecTimer()/1000);

  request = GetTaskDLQElement1( tcb );
  request->req_type = REQ_RESCHEDULE_EO00;

  e71_YieldTaskRunEnqu ( this_ukt, tcb, request, BASE_PRIO_REX );
  if ( tcb->prio_flag )
      tcb->ulDynPrio +=  PRIO_FACTOR;

  GOTO_DISP (&this_ukt);

  time_now_in_msec = (SAPDB_Int4)(RTESys_MicroSecTimer()/1000);

  if ( time_now_in_msec - *StartMsec >= *RemainMsec ||
       time_now_in_msec < *StartMsec )
    *RemainMsec = 0 ;
  else
  {
    *RemainMsec -= time_now_in_msec - *StartMsec ;
    *StartMsec   = time_now_in_msec ;
  }

  DBG4 (( MF__, "[T:0x%03u] awakend, RemainMsec: %d",
          tcb->index, *RemainMsec ));

  tcb->state  = TSK_RUNNING_EO00;

  DBGOUT_T (tcb->index);
  return;
}

/*------------------------------*/

static  void    en56_remvresume (void               *pDummy,
                                 struct TASK_TYPE   *FromTask )
{
#undef  MF__
#define MF__ MOD__"en56_remvresume"
    ten50_UKT_Control               * this_ukt	= THIS_UKT_CTRL;
	tsp00_Bool						IsSuspended = this_ukt->curr_task->state == TSK_VSUSPEND_EO00;

	DBGIN_T(this_ukt->curr_task->index)

    this_ukt->curr_task->resume_count++ ;

    en56_StoreResumer( this_ukt->curr_task, FromTask->index, IsSuspended ? 0 : 1 );

    if ( this_ukt->curr_task->resume_count == 0 && IsSuspended )
	{
        GetTaskDLQElement1( this_ukt->curr_task )->req_type = REQ_VRESUME;
		DBGOUT_T(this_ukt->curr_task->index)
	}
    else
	{
	    this_ukt->curr_task = 0;
		DBG1 (( MF__,"Returning, with no task \n" ));
	}
}

/*------------------------------*/

static  void    en56_remvsignal ( void *paraOK,
								                  struct TASK_TYPE *FromTask )
{
#undef  MF__
#define MF__ MOD__"en56_remvsignal"
ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
tsp00_Bool                      ok;

  DBGIN_T( this_ukt->curr_task->index );

# if !defined (MULTIPLE_SIGNAL_CHECK)

  ok = (tsp00_Bool)(((char*)paraOK)-(char *)0);

    if ( this_ukt->curr_task->vsignal_ok != 0 )
    {
        MSGALL(( ERR_MULTIPLE_VSIGNAL_CALL, this_ukt->curr_task->index ));
        vabort(WRITE_CORE); 
    }

  this_ukt->curr_task->vsignal_ok = ( ok ? 1 : -1 );

# endif

	/*IST this_ukt->curr_task->signal_count ++ ; */
	/*IST if ( this_ukt->curr_task->signal_count == 0 ) */
	if ( this_ukt->curr_task->state == TSK_VWAIT_EO00 )
	{
        GetTaskDLQElement1( this_ukt->curr_task )->req_type = REQ_VSIGNAL_EO00;
		DBGOUT_T(this_ukt->curr_task->index);
	}
	else
	{
		this_ukt->curr_task = 0;
		DBG1 (( MF__,"returning, with no task \n"));
	}
}

/*------------------------------*/

static void en56_StoreResumer( struct TASK_TYPE                * tcb ,
							   tsp00_TaskId                      pid,
							   tsp00_Int4                        allowedResCount
)
{
    tcb->resumerPid [ tcb->totalResumerCount%MAX_RESUMER_PIDS ] = pid;

    ++tcb->totalResumerCount;

    if ( tcb->resume_count > allowedResCount )
    {
        int i;
        int j0;
        MSGD (( ERR_WRONG_RESUME_COUNT, tcb->index, tcb->resume_count ));

        i = tcb->totalResumerCount;
        j0 = MAX_RESUMER_PIDS;

        while( (i-- > 0) && (j0-- > 0) )
        {
            MSGD (( WRN_RESUMER_HISTORY, tcb->resumerPid[i%MAX_RESUMER_PIDS]));
        }
        vabort(WRITE_CORE);
    }
}
