/*!
  @file           ven51.cpp
  @author         JoergM
  @brief          Kernel RunTime: Kernel Initialization Functions
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

#define MOD__  "ven51.c:"

#include "gen00.h"
#include "geo50_0.h"
#include "geo00_2.h"
/*
 *  DEFINES
 */

/* exported functions */

#include "heo51.h"
#include "hen51.h"

/* imported functions */

#include "heo46.h"
#include "heo92.h"
#include "gen41.h"
#include "gen71.h"
#include "gen72.h"
#include "gen73.h"
#include "gen75.h"
#include "gen81.h"
#include "gen88.h"
#include "hen50.h" /* vabort () */
#include "hen56.h" /* vsleep () */
#include "hen40.h" /* sqlabort () */
#include "geo007_1.h"
#include "heo15.h"
#include "heo69.h"
#include "RunTime/RTE_KernelMain.h"
#include "RunTime/RTE_DiagPipe.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/RTE_Version.h"

/* PTS 1108470 */
/* after dynamic thread allocation is implemented.... */
/* include "geo07.h" */ /* sqlendthread () */

/* exported data */

/* used to inform Coordinator that a core dump is wanted */
global volatile int     en51NoCoreDumpWanted = 0;
volatile int en51_NeverStop = 1;

/* local functions */

static  void    en51_start_single_task   ( struct TASK_TYPE *tcb );
static  void	en51_start_multiple_tasks( struct TASK_TYPE *first,
										   unsigned long     count );
/*
 * =====================================================================
 */
#undef  MF__
#define MF__ MOD__"vinitok"
void    vinitok ( )
{
    int                            i;
    struct TASK_TYPE             * tcb;
    ten50_UKT_Control            * lkp;
    ten50_UKT_Control            * this_ukt = THIS_UKT_CTRL;

    DBGIN_T (this_ukt->curr_task->index );

    if ( this_ukt->curr_task->type != TT_TI_EO00 )
    {
        MSGD (( ERR_VINITOK_NOT_TIMEOUT ));
        vabort ( WRITE_CORE );
    }

    MSGD (( INFO_VINITOK ));

    /*
     *  wake all non user kernel tasks
     */
	en51_start_single_task( KGS->al );
	en51_start_single_task( KGS->tw );

	en51_start_multiple_tasks( KGS->first_datawriter, 
								XPARAM(ulMaxDataWriter) );
	en51_start_multiple_tasks( KGS->first_event_task, 
								XPARAM(ulMaxEventTasks) );
	en51_start_multiple_tasks( KGS->first_backup_task, 
								XPARAM(ulMaxBackupTasks) );
	en51_start_multiple_tasks( KGS->first_garbage_collector, 
								XPARAM(ulMaxGarbageCollector) ) ;

    /*
     *  wake all server kernel tasks
     */
	if ( XPARAM(ulMaxServer) > 0 ) 
	{
		/*
		 *	 Only first must be tested, others are in same state
		 */
	    if ( KGS->first_server->state == TSK_INITIALIZED_EO00 )
		{
			en51_start_multiple_tasks( KGS->first_server, XPARAM(ulMaxServer) ) ;
		}
	}

    /*
     *  wake all user kernel processes
     */
    for ( lkp = KGS->first_kp; lkp <= KGS->last_kp ; lkp ++ )
    {
        if ( lkp->semid > 0 )
        {
            e72_wake ( lkp );
        }
    }

    DBGOUT_T ( this_ukt->curr_task->index );
}

/*
 *=======================================================================
 */
#undef  MF__
#define MF__ MOD__"vinit"
void    vinit ( tsp00_RteCommAddr VAR_VALUE_REF  rte_comm_ptr,
                tsp00_Int2                       len )
{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;

    DBGIN_T (this_ukt->curr_task->index);

    if ( len != sizeof(tsp00_RteCommRec) )
    {
        MSGCD (( ERR_WRONG_COMM_REC_SIZE , len ))
        vabort ( WRITE_CORE );
    }

    SAPDB_memset ( &this_ukt->curr_task->runnableMicroSeconds,    0, sizeof ( this_ukt->curr_task->runnableMicroSeconds ) );
    SAPDB_memset ( &this_ukt->curr_task->maxRunnableMicroSeconds, 0, sizeof ( this_ukt->curr_task->maxRunnableMicroSeconds ) );

    /* - alter task activ counters */
    en71AlterTaskCnt( this_ukt->curr_task, ADD_ACTIVE_COUNTER );

    /* Remove inactive move lock */
    en71LongTermMoveLock( this_ukt->curr_task, TASK_IS_INACTIVE, false );

    /* return address of rte_comm */
    rte_comm_ptr = &this_ukt->curr_task->rte_comm;

    this_ukt->curr_task->state = TSK_RUNNING_EO00;

    DBGOUT_T (this_ukt->curr_task->index);
}

/*
 *=======================================================================
 */

#undef  MF__
#define MF__ MOD__"vdebug_break"
void    vdebug_break ( tsp00_Int4 i4DebugBreakPosition )
{
  DBGIN;

  if ( i4DebugBreakPosition < 10000 )
  {
    MSGALL (( ERR_WRONG_DEBUG_BREAK_POS, i4DebugBreakPosition, 10000 ));
    DBGOUT;
    return;
  }

  if ( KGS->ulSingleDebugBreakPos == i4DebugBreakPosition )
  {
      KGS->ulSingleDebugBreakPos = 0;

      en81CallDebugger();
  }

  DBGOUT;
  return;
}

/*
 *=======================================================================
 */

#undef  MF__
#define MF__ MOD__"vgetuktid"
void    vgetuktid ( tsp00_Int4 VAR_VALUE_REF  uktId )
{
    ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;
    DBGIN_T (this_ukt->curr_task->index);

    uktId = this_ukt->index;

    DBGOUT_T(this_ukt->curr_task->index);
}

/*
 *=======================================================================
 */

/* void vgetpid(tsp00_TaskId *pid) now implemented in ven88.cpp */

/*
 *=======================================================================
 */

#undef  MF__
#define MF__ MOD__"vgetfirstuserpid"
void    vgetfirstuserpid ( tsp00_TaskId VAR_VALUE_REF  pid )
{
#ifdef DEBUG_RTE
ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;

	DBGIN_T (this_ukt->curr_task->index);
#endif
    
    pid = (RTE_TaskId) KGS->first_user->index ;
	DBG1((MF__,"returning pid %ld\n", *pid));
#ifdef DEBUG_RTE
    DBGOUT_T(this_ukt->curr_task->index);
#endif
}

/*
 *=======================================================================
 */

#undef  MF__
#define MF__ MOD__"vptype"
void    vptype ( RTE_TaskId                        pid,
                 tsp2_process_type VAR_VALUE_REF   proc_type )
{
    struct TASK_TYPE *tcb;

    PID_TCB(pid, tcb);

    DBGIN_T (tcb->index);

    switch ( tcb->type )
    {
    case TT_UT_EO00:
        proc_type.becomes(sp2pt_utility);
        break;
    case TT_TI_EO00:
        proc_type.becomes(sp2pt_timeout);
        break;
    case TT_US_EO00:
        proc_type.becomes(sp2pt_user);
        break;
    case TT_AL_EO00:
        proc_type.becomes(sp2pt_log_writer);
        break;
    case TT_TW_EO00:
        proc_type.becomes(sp2pt_trace_writer);
        break;
    case TT_SV_EO00:
        proc_type.becomes(sp2pt_server);
        break;
    case TT_DW_EO00:
        proc_type.becomes(sp2pt_pager);
        break;
    case TT_EV_EO00:
        proc_type.becomes(sp2pt_event);
        break;
    case TT_GC_EO00:
        proc_type.becomes(sp2pt_garbage_collector);
        break;
    case TT_BUP_EO00:
        proc_type.becomes(sp2pt_backup);
        break;
    default:
        MSGCD (( ERR_VPTYPE_WRONG_TASK_TYPE, tcb->type )) ;
        vabort ( WRITE_CORE );
        break;
    }

    DBG1 (( MF__,"returning %d \n", * proc_type ));

	DBGOUT_T (tcb->index);
}

/*
 *=======================================================================
 */

#undef  MF__
#define MF__ MOD__"vversion"
void    vversion ( tsp00_Version VAR_ARRAY_REF kernelversion, 
				   tsp00_Version VAR_ARRAY_REF rteversion )
{
    eo46CtoP ( (tsp00_Byte*)rteversion.asCharp(), RTE_GetRTEVersion(), sizeof(tsp00_Version) );
}

/*
 * =======================================================================
 */
#undef  MF__
#define MF__ MOD__"vutil_startup"

void  vutil_startup( tsp1_utility_startup VAR_VALUE_REF  startupReason )
{
#ifdef DEBUG_RTE
ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;
	DBGIN_T(this_ukt->curr_task->index);
	DBGOUT_T(this_ukt->curr_task->index);
#endif

    startupReason.becomes(sp1us_connect);
}

/*
 * =======================================================================
 */

#undef  MF__
#define MF__ MOD__"vrestart"
/*ARGSUSED*/ /* sysdevspace unused */
void    vrestart ( RTE_TaskId                    pid,
                   tsp00_DevName VAR_ARRAY_REF   sysdevspace )
{
#ifdef DEBUG_RTE
    struct TASK_TYPE *tcb;

    PID_TCB(pid, tcb);

    DBGIN_T(tcb->index);
#endif /* DEBUG_RTE */

    KGS->restarted = TRUE;

    RTE_SetDatabaseState(SERVER_ONLINE_EO00);

    if ( KGS->ulStartupOptions & FORCE_AUTORESTART )
    {
        if(RTE_DiagPipeIsOpen())
        {
            RTE_CloseDiagPipe();
            (void)en41RemovePipeFDFile(en81_dbname);
        }
    }

#ifdef DEBUG_RTE
	DBGOUT_T(tcb->index);
#endif /* DEBUG_RTE */
}

extern void en81_LastWords(int runKernelExitCode, int isKilled);

/* PTS 1115383
 *=======================================================================
 */
#undef  MF__
#define MF__ MOD__"voffline"
void    voffline ( RTE_TaskId                 pid,
                   tsp00_ShutdownMode_Param  mode)
{
#undef  MF__
#define MF__ MOD__"voffline"
    struct TASK_TYPE  * tcb;

    PID_TCB(pid, tcb);

    DBGIN_T(tcb->index);

    tcb->state = TSK_VSHUTDOWN_EO00;
    KGS->taskResponsibleForOffline = (SAPDB_Int4)pid;
    KGS->state = SERVER_SHUTDOWN;

    MSGD (( INFO_CHANGE_STATE, "SHUTDOWN", KGS->state ));

    /* If enabled show max stack usage */
    en81ShowMaxStackUse();

    if (mode == shtShutdKill_esp00 )
    {
        KGS->dumpDiagFiles = true;
        en81FinalAction();   /* Set dump and trace flag and resume trace writer */
    }

#ifdef SDB_WITH_PROFILER
    KGS->state = SERVER_STOP ;

    en81_LastWords(0, 0);

    exit(2);
#else /* SDB_WITH_PROFILER */
#if defined(LINUX)
	exit(0);	/* LINUX does not support real threading, so _exit(0) will only exit a single clone process... */
#else
	_exit(0);	/* this is the normal end of SAP DB Kernel... */
#endif
#endif /* SDB_WITH_PROFILER */
}

/*
 * ===========================================================================
 */

void en51Finish()
{
#   undef  MF__
#   define MF__ MOD__"en51Finish"
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;

    DBGIN_T (this_ukt->curr_task->index);

    /* Don't move this task during finish and next startup phase... */
    en71LongTermMoveLock( this_ukt->curr_task, TASK_IS_INACTIVE, true );

    /* PTS 1110953 */
    if ( XPARAM(showMaxStackUse) )
    {
        vsShowMaxStackUse( this_ukt->curr_task->index, eo92GetTaskTypeName(this_ukt->curr_task->type) );
    }

    switch ( this_ukt->curr_task->type )
    {
    case TT_TW_EO00: 
        /* - alter task activ counters */
        en71AlterTaskCnt( this_ukt->curr_task, SUB_ACTIVE_COUNTER );

        /*
        *  Crash server after bufwriter wrote trace (and dump).
        */
        switch ( KGS->state )
        {
        case SERVER_SHUTDOWNKILL :
        case SERVER_SHUTDOWNREINIT :
            KGS->state = SERVER_KILL;
            MSGD (( INFO_CHANGE_STATE, "KILL", KGS->state ));
            /*FALLTHROUGH*/
        case SERVER_KILL :
        case SERVER_STOP :
            en81_NotifyCoordinator(1);
            /* PTS 1108470 */
            while( en51_NeverStop )
            {
                sqlyieldthread();
            }
            /* PTS 1111155 */
            break;

        default:
            break;
        }
        MSGD (( ERR_VFINISH_TW_DUR_NORM_OP )) ;
        break;

    default:

        /* - alter task activ counters */
        en71AlterTaskCnt( this_ukt->curr_task, SUB_ACTIVE_COUNTER );

        if ( this_ukt->curr_task->connectable )
        {
            // release all open connections
            this_ukt->curr_task->pTaskObject->ClientConnections().ReleaseAllConnections();

            /* 
            * Wait for the REQ_INITOK request from requestor 
            */
            MSGD (( INFO_VCON_WAITING, this_ukt->curr_task->index ));
            this_ukt->curr_task->state = TSK_INACTIVE_EO00;

            //  Enqueue task in list of unconnected tasks of the TaskScheduler. If the
            //  task type is not connectable this call is ignored
            this_ukt->curr_task->pTaskObject->EnqueueIntoUnconnectedTasksQueue();

            GOTO_DISP (&this_ukt);

            if ( this_ukt->curr_quu->req_type != REQ_INITOK )
            {
                MSGALL (( ERR_ILLEGAL_REQUEST, "initial", this_ukt->curr_quu->req_type ));
                vabort ( WRITE_CORE );
            }
            RELEASE_FREELIST_ELEM( this_ukt->curr_quu );
            this_ukt->curr_quu = 0;
        }
        break;
    }

    this_ukt->curr_task->state = TSK_INITIALIZED_EO00;

    DBGOUT_T(this_ukt->curr_task->index)
}


/*
 * ===========================================================================
 */

void    vfinish ( )
{
#   undef  MF__
#   define MF__ MOD__"vfinish"

    void en51Finish();
}
/*
 * ===========================================================================
 */

void vmovelock ( RTE_TaskId  pid )
{
    struct TASK_TYPE *tcb;

    PID_TCB(pid, tcb);

    en71TempMoveLock ( tcb, 1, true );
}

/*
 * ===========================================================================
 */

void vmoveunlock ( RTE_TaskId  pid )
{
    struct TASK_TYPE *tcb;

    PID_TCB(pid, tcb);

    /* remove temporary move lock */
    en71TempMoveLock ( tcb, 1, false );
}

/*
 * ===========================================================================
 */

externPascal tsp00_Bool visutilitytaskconnected()
{
    return  ( KGS != 0 
           && KGS->ut != 0 
           && KGS->ut->connection != 0 
           && 0 != KGS->ut->connection->ci_connect_time );
}

/*
 * ===========================================================================
 */

#undef  MF__
#define MF__ MOD__"vos"
void    vos ( tsp00_Os VAR_VALUE_REF os )
{
#ifdef DEBUG_RTE
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;

    DBGIN_T (this_ukt->curr_task->index);
#endif

    os.becomes(os_unix);

#ifdef DEBUG_RTE
    DBGOUT_T (this_ukt->curr_task->index);
#endif
}

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#undef  MF__
#define MF__ MOD__"en51_start_single_task"
static  void en51_start_single_task( struct TASK_TYPE *tcb )
{
    SAPDBErr_MessageList           messageList;
	DBGIN

    if ( false == tcb->pTaskObject->Startup( messageList ))
        RTE_Crash (messageList);

	DBGOUT
}

/*---------------------------------------------------------------------------*/

#undef  MF__
#define MF__ MOD__"en51_start_multiple_tasks"
static  void en51_start_multiple_tasks( struct TASK_TYPE *first,
										unsigned long     count )
{
	DBGIN
	if ( count > 0 )
	{
		unsigned long i;
		struct TASK_TYPE				* tcb;

		for ( i = 0, tcb = first; i < count; i++, tcb++ )
		{
			en51_start_single_task( tcb );
		}
	}
	DBGOUT
}

