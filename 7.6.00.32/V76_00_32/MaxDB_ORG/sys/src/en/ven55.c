/*!
  @file           ven55.c
  @author         JoergM
  @brief          Kernel RunTime: Mutual Exclusion
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

#define MOD__ "ven55.c:"

#include "gen00.h"
#include "heo00.h"
#include "geo002.h"
#include "geo50_0.h"
#include "gen500.h"
#include "gen55.h"
#include "heo55k.h"

/* imported functions */
#include "gen71.h"
#include "hen50.h" /* vabort () */
#include "hen56.h" /* vsleep () */
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "heo46.h"

#define MAX_INTERLOCK_LOOPS -1

/* local functions */

static   void init_semaliste ( struct DDB4_SEMA                * scb);

static   void vbegexcl_one_ukp         ( RTE_TaskId pid, REGION_ID sid );
static   void vbegexcl_n_ukps_loop     ( RTE_TaskId pid, REGION_ID sid );
static   void vbegexcl_n_ukps_noloop   ( RTE_TaskId pid, REGION_ID sid );
static   void vbegexcl_dirty_read_loop ( RTE_TaskId pid, REGION_ID sid );
static   void vbegexcl_no_semalist     ( RTE_TaskId pid, REGION_ID sid );

VOIDFUNC_PTR  vbegexcl_func_ptr = vbegexcl_n_ukps_noloop ;

/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"e55_init_vbegexcl_func_ptr"
void    e55_init_vbegexcl_func_ptr ( 
int nr_of_ukps )
{
    DBGIN;
  
    if ( nr_of_ukps == 1 ) /* all-in-one */
        vbegexcl_func_ptr = vbegexcl_one_ukp ;
    else
        if ( XPARAM(ulRegionCollisionLoop) == 0 ) /* this means that MAXCPU is 1 - a single cpu system */
            vbegexcl_func_ptr = vbegexcl_n_ukps_noloop ;
        else                                /* now the boring cases are left behind... */
            if ( XPARAM(fBegExclDirtyRead) )
            { 
                if ( ! XPARAM(fExclQueue) )
                    vbegexcl_func_ptr = vbegexcl_no_semalist ;
                else
                    vbegexcl_func_ptr = vbegexcl_dirty_read_loop ;
            }
            else
                vbegexcl_func_ptr = vbegexcl_n_ukps_loop ;
            
    DBGOUT;
    return ;  
}
/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"init_semaliste"
static  void    init_semaliste ( 
struct DDB4_SEMA                * scb)
{
    int                             i;
    struct N_QUEUE * ior1;
    struct N_QUEUE * ior2;

	DBGIN;
    DBG1 (( MF__,"called, scb 0x%lx", scb ));

    ior1 = & scb->semaliste.sem [ 0 ];
    for ( i = 0; i < KGS-> ulNumOfTasks ; i ++ )
    {
        ior1->taskid  = 0;
        ior2          = ior1 + 1;
        ior1->next    = ior2;
        ior1          = ior2;
    }

    ior1 --;
    ior2 = & scb->semaliste.sem [ 0 ];
    ior1->next = ior2;

    scb->semaliste.first = ior2;
    scb->semaliste.last  = ior2;

    INIT_LOCK(scb->semaliste.sem_exclusive);

    DBGOUT;
}

/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"vbegexcl_one_ukp"
/*ARGSUSED*/ /* pid unused */
static  void    vbegexcl_one_ukp ( RTE_TaskId pid, REGION_ID  sid )
{
    ten50_UKT_Control              * this_ukt;
    struct DDB4_SEMA               * scb;
    struct SEM_LISTE               * scbs;
    struct TASK_TYPE               * l_curr_task;

	DBGIN;

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);

    if ( sid == -1 )
    {
        return;
    }
    if ( KGS->state >= SERVER_SHUTDOWNREINIT && l_curr_task->type != TT_TW_EO00 )
    {
        GOTO_DISP (&this_ukt);    /* never come back... */
    }
    DBG1 (( MF__,"called, T%d, sid =%d\n", l_curr_task->index,sid ));
    scb = KGS-> semaphore + sid - 1 ;
    if ( scb->owner == l_curr_task )
    {
        MSGALL(( IERR_SECOND_REGION_REQUEST, MF__, sid, scb->semaname ));
        vabort ( WRITE_CORE ) ;
    }
    if ( KGS->traceflag && *(KGS->traceflag) && l_curr_task->type == TT_TW_EO00 )
        return;
    l_curr_task->state = TSK_VBEGEXCL_EO00;
    l_curr_task->sid   = sid;
    l_curr_task->begexcl_count++;
    l_curr_task->ulBegExclCalls++;
    l_curr_task->reg_excl_cnt [ sid-1 ]++; /* -1 because sid is a pascal style index */
    l_curr_task->excl_schachtel++;
    DBG1 (( MF__, "l_curr_task->sid = %d \n", l_curr_task->sid ));
    scbs = & scb->semaliste;
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    scb->get_count++;

    if ( ! scb->owner )
    {
        /* during shutdown or crash force task other than tracewriter to dispatch... */
        if ( KGS->state >= SERVER_SHUTDOWNREINIT
          && l_curr_task != KGS->tw )
        {
            GOTO_DISP (&this_ukt);
        }
        scb->owner = l_curr_task;
    }
    else
    {
        scbs->first->taskid = l_curr_task;
        scbs->first         = scbs->first->next;

        DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index ));
        GOTO_DISP (&this_ukt);
        DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index ));
        scb->owner->reg_coll_cnt [ sid-1 ]++;
        scb->collision_count++;
    }

    l_curr_task->state = TSK_RUNNING_EO00;

    DBGOUT_T(( l_curr_task->index ));
}

/*
 *=======================================================================
 */

#undef MF__
#define MF__ MOD__"vbegexcl_n_ukps_loop"
/*ARGSUSED*/ /* pid unused */
static  void    vbegexcl_n_ukps_loop ( RTE_TaskId pid, REGION_ID sid )
{
    ten50_UKT_Control              * this_ukt;
    struct DDB4_SEMA               * scb;
    struct SEM_LISTE               * scbs;
    struct TASK_TYPE               * l_curr_task;
    long                           tas_count = MAX_INTERLOCK_LOOPS;
    long loop_region_coll = XPARAM(ulRegionCollisionLoop) ;
    long region_reserved;
    long reserved_cnt = loop_region_coll;
    long num_coll ; 

	DBGIN;

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);

    if ( sid == -1 )
    {
        return;
    }
    if ( KGS->state >= SERVER_SHUTDOWNREINIT && l_curr_task->type != TT_TW_EO00 )
    {
        GOTO_DISP (&this_ukt);    /* never come back... */
    }
    DBG1 (( MF__,"called, T%d, sid =%d\n", l_curr_task->index,sid ));
    scb = KGS-> semaphore + sid - 1 ;
    if ( scb->owner == l_curr_task )
    {
        MSGALL(( IERR_SECOND_REGION_REQUEST, MF__, sid, scb->semaname ));
        vabort ( WRITE_CORE ) ;
    }
    if ( KGS->traceflag && *(KGS->traceflag) && l_curr_task->type == TT_TW_EO00 )
        return;
    l_curr_task->state = TSK_VBEGEXCL_EO00;
    l_curr_task->sid   = sid;
    l_curr_task->begexcl_count++;
    l_curr_task->ulBegExclCalls++;
    l_curr_task->reg_excl_cnt [ sid-1 ]++; /* -1 because sid is a pascal style index */
    l_curr_task->excl_schachtel++;
    DBG1 (( MF__, "l_curr_task->sid = %d \n", l_curr_task->sid ));
    scbs = & scb->semaliste;
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    scb->get_count++;

    do  /* fh: if we have a multi-cpu machine, loop here */
    {
        while ( INTERLOCK( scbs->sem_exclusive ) )
        { 
            if ( ! -- tas_count )
                if ( l_curr_task->type == TT_TW_EO00 &&
                     l_curr_task->ukt->activeTasks == 1 )
                {
				    MSGD (( INFO_EXCL_TW_IS_SLEEPING ))
                    vsleep(l_curr_task->index,1);
                    tas_count = MAX_INTERLOCK_LOOPS;
                }
                else
                {
				    MSGALL (( IERR_EXCL_TW_LOOPS_INDEF, MF__, sid ));
                    vabort ( WRITE_CORE );
                }

            {
                struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
                request->req_type = REQ_TEST_AND_SET ;
                e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );
                l_curr_task->ulDynPrio +=  PRIO_FACTOR;
            }
            DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index));
            GOTO_DISP (&this_ukt);
            DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index));
        }  /* end-while interukp */
/*  loop_region_coll is > 0 !!!  	    if ( reserved_cnt ) */
        if ( ( region_reserved = scb->owner != (struct TASK_TYPE *) 0 )     /* someone else is in the region */
                 && (--reserved_cnt > 0) )
        {
            struct DOUBLY_LINKED *request;
 
            CLEARLOCK ( scbs->sem_exclusive ) ;/* free the lock */
            request =  GetTaskDLQElement1( l_curr_task );
            request->req_type = REQ_EXCLUSIVE ;
            e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );  /* to give tasks with urgent needs a chance */
            if (l_curr_task->prio_flag)
                l_curr_task->ulDynPrio +=  PRIO_FACTOR;

            GOTO_DISP (&this_ukt);
        }                 /* else leave the loop */  
    } while ( region_reserved && reserved_cnt > 0 ) ;  /* fh: do-while end */

    /*
     * we are alone now
     */
    if ( ! scb->owner )
    {   
        /* during shutdown or crash force task other than tracewriter to dispatch... */
        if ( KGS->state >= SERVER_SHUTDOWNREINIT
          && l_curr_task != KGS->tw )
        {
            CLEARLOCK ( scbs->sem_exclusive );
            GOTO_DISP (&this_ukt);
        }
        scb->owner = l_curr_task;
        CLEARLOCK ( scbs->sem_exclusive );
    }
    else
    {
        scbs->first->taskid = l_curr_task;
        scbs->first         = scbs->first->next;
        CLEARLOCK ( scbs->sem_exclusive );
        scb->wait_count++;
        this_ukt->NumRegWait++ ;
        DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index ));
        GOTO_DISP (&this_ukt);
        DBG1 (( MF__,"after goto_disp T%d \n", l_curr_task->index ));
    }

    scb->begexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
    if ( ( num_coll = loop_region_coll - reserved_cnt ) )
    {
        scb->collision_count += num_coll ;
        l_curr_task->reg_coll_cnt [ sid-1 ] += num_coll ;
      }
    l_curr_task->state = TSK_RUNNING_EO00;

    DBGOUT_T (( l_curr_task->index ));
}

/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"vbegexcl_n_ukps_noloop"
/*ARGSUSED*/ /* pid unused */
static  void    vbegexcl_n_ukps_noloop ( RTE_TaskId pid, REGION_ID sid )
{
    ten50_UKT_Control               * this_ukt;
    struct DDB4_SEMA               * scb;
    struct SEM_LISTE               * scbs;
    struct TASK_TYPE               * l_curr_task;
    long                           tas_count = MAX_INTERLOCK_LOOPS;

	DBGIN;

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);

    if ( sid == -1 )
    {
        return;
    }
    if ( KGS->state >= SERVER_SHUTDOWNREINIT && l_curr_task->type != TT_TW_EO00 )
    {
        GOTO_DISP (&this_ukt);    /* never come back... */
    }
    DBG1 (( MF__,"called, T%d, sid =%d\n", l_curr_task->index,sid ));
    scb = KGS-> semaphore + sid - 1 ;
    if ( scb->owner == l_curr_task )
    {
        MSGALL(( IERR_SECOND_REGION_REQUEST, MF__, sid, scb->semaname ));
        vabort ( WRITE_CORE ) ;
    }
    if ( KGS->traceflag && *(KGS->traceflag) && l_curr_task->type == TT_TW_EO00 )
        return;
    l_curr_task->state = TSK_VBEGEXCL_EO00;
    l_curr_task->sid   = sid;
    l_curr_task->begexcl_count++;
    l_curr_task->ulBegExclCalls++;
    l_curr_task->reg_excl_cnt [ sid-1 ]++; /* -1 because sid is a pascal style index */
    l_curr_task->excl_schachtel++;
    DBG1 (( MF__, "l_curr_task->sid = %d \n", l_curr_task->sid ));
    scbs = & scb->semaliste;
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    scb->get_count++;

    while ( INTERLOCK( scbs->sem_exclusive ))  /* try to lock the list - this is actually a call to e76_interlock(scbs->sem_exclusive) */
    { 
        if ( ! -- tas_count )
            if ( l_curr_task->type == TT_TW_EO00 &&
                 l_curr_task->ukt->activeTasks == 1 )
            {
				MSGD (( INFO_EXCL_TW_IS_SLEEPING ))
                vsleep(l_curr_task->index,1);
                tas_count = MAX_INTERLOCK_LOOPS;
            }
            else
            {
				MSGALL (( IERR_EXCL_TW_LOOPS_INDEF, MF__, sid ));
                vabort ( WRITE_CORE );
            }

        {
            struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
            request->req_type = REQ_TEST_AND_SET ;
            e71_YieldTaskRunEnqu( this_ukt, l_curr_task, request, BASE_PRIO_REX );
            l_curr_task->ulDynPrio +=  PRIO_FACTOR;
        }
        DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index));
        GOTO_DISP (&this_ukt);
        DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index));
    }  /* end-while interukp */
    /*
     * we are alone now
     */
    if ( ! scb->owner ) /* the list is empty */
    {
        /* during shutdown or crash force task other than tracewriter to dispatch... */
        if ( KGS->state >= SERVER_SHUTDOWNREINIT
          && l_curr_task != KGS->tw )
        {
            CLEARLOCK ( scbs->sem_exclusive ); /* and unlock it - actually a call to e76_clearlock(scbs->sem_exclusive) */
            GOTO_DISP (&this_ukt);
        }
        scb->owner = l_curr_task;   /* fine, insert myself to show the list is in use and go on */
        CLEARLOCK ( scbs->sem_exclusive ); /* and unlock it - actually a call to e76_clearlock(scbs->sem_exclusive) */
    }
    else
    { 
        scbs->first->taskid = l_curr_task;
        scbs->first         = scbs->first->next;
        CLEARLOCK ( scbs->sem_exclusive );
        scb->wait_count++ ;
        this_ukt->NumRegWait++ ;
        DBG1 (( MF__,"before goto_disp T%d", l_curr_task->index ));
        GOTO_DISP (&this_ukt);
        DBG1 (( MF__,"after goto_disp T%d", l_curr_task->index ));

        scb->collision_count++; 
        scb->owner->reg_coll_cnt [ sid-1 ]++;
    }
    scb->begexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
    l_curr_task->state = TSK_RUNNING_EO00;

    DBGOUT_T (( l_curr_task->index ));
}
/*
 *=======================================================================
 */

#undef MF__
#define MF__ MOD__"vbegexcl_no_semalist"
/*ARGSUSED*/ /* pid unused */
static  void    vbegexcl_no_semalist ( 
RTE_TaskId pid ,
REGION_ID sid )
{
    ten50_UKT_Control * this_ukt;
    struct DDB4_SEMA               * scb = KGS-> semaphore + sid - 1 ;
    struct SEM_LISTE               * scbs= & scb->semaliste;    /* this is only needed for sem_exclusive, the lock */
    struct TASK_TYPE               * l_curr_task;
    long                           tas_count = MAX_INTERLOCK_LOOPS;
    long loop_region_coll = XPARAM(ulRegionCollisionLoop) ;
    long reserved_cnt = loop_region_coll, num_coll ; /* only needed for calculation of the parameter for e71_EnquRex() */

	DBGIN;

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);

    if ( sid == -1 )
    {
        return;
    }
    if ( KGS->state >= SERVER_SHUTDOWNREINIT && l_curr_task->type != TT_TW_EO00 )
    {
        GOTO_DISP (&this_ukt);    /* never come back... */
    }
    DBG1 (( MF__,"called, T%d, sid =%d\n", l_curr_task->index,sid ));
    scb = KGS-> semaphore + sid - 1 ;
    if ( scb->owner == l_curr_task )
    {
        MSGALL(( IERR_SECOND_REGION_REQUEST, MF__, sid, scb->semaname ));
        vabort ( WRITE_CORE ) ;
    }
    if ( KGS->traceflag && *(KGS->traceflag) && l_curr_task->type == TT_TW_EO00 )
        return;
    l_curr_task->state = TSK_VBEGEXCL_EO00;
    l_curr_task->sid   = sid;
    l_curr_task->begexcl_count++;
    l_curr_task->ulBegExclCalls++;
    l_curr_task->reg_excl_cnt [ sid-1 ]++; /* -1 because sid is a pascal style index */
    l_curr_task->excl_schachtel++;
    DBG1 (( MF__, "l_curr_task->sid = %d \n", l_curr_task->sid ));
    scbs = & scb->semaliste;
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    scb->get_count++;

    do  /* fh: if we have a multi-cpu machine, loop here */
    {
        if ( scb->owner != (struct TASK_TYPE *) 0  )    /* the dirty read... */
        {
            struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
            --reserved_cnt ;
  	        request->req_type = REQ_EXCLUSIVE ;  /* call the dispatcher before trying again */

            e71_YieldTaskRunEnqu( this_ukt, l_curr_task, request, BASE_PRIO_REX ); 
            if (reserved_cnt > 0 || l_curr_task->excl_schachtel > 1) 
                l_curr_task->ulDynPrio +=  PRIO_FACTOR; 

            GOTO_DISP (&this_ukt);
        }                 /* else leave the loop */  
        else
        {
            while ( INTERLOCK( scbs->sem_exclusive  ))
            { 
                if ( ! -- tas_count )
                    if ( l_curr_task->type == TT_TW_EO00 &&
                         l_curr_task->ukt->activeTasks == 1 )
                    {
				        MSGD (( INFO_EXCL_TW_IS_SLEEPING ))
                        vsleep(l_curr_task->index,1);
                        tas_count = MAX_INTERLOCK_LOOPS;
                    }
                    else
                    {
				        MSGALL (( IERR_EXCL_TW_LOOPS_INDEF, MF__, sid ));
                        vabort ( WRITE_CORE );
                    }

                {
                    struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
                    request->req_type = REQ_TEST_AND_SET ;
                    e71_YieldTaskRunEnqu( this_ukt, l_curr_task, request, BASE_PRIO_REX );
                    l_curr_task->ulDynPrio +=  PRIO_FACTOR;
                }
                DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index));
                GOTO_DISP (&this_ukt);
                DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index));
            }  /* end-while interukp */
            if ( scb->owner == (struct TASK_TYPE *) 0 )
            {
                if ( KGS->state >= SERVER_SHUTDOWNREINIT
                  && l_curr_task != KGS->tw )
                {
                    CLEARLOCK ( scbs->sem_exclusive );
                    GOTO_DISP (&this_ukt);
                }
                scb->owner = l_curr_task;
                CLEARLOCK ( scbs->sem_exclusive );
                break ;
            }
            else
            {
                struct DOUBLY_LINKED *request;
                CLEARLOCK ( scbs->sem_exclusive );
                --reserved_cnt ;
                request =  GetTaskDLQElement1( l_curr_task );
      	        request->req_type = REQ_EXCLUSIVE ;

                e71_YieldTaskRunEnqu( this_ukt, l_curr_task, request, BASE_PRIO_REX );  
                if (reserved_cnt > 0 || l_curr_task->excl_schachtel > 1) 
                    l_curr_task->ulDynPrio +=  PRIO_FACTOR; 

                GOTO_DISP (&this_ukt);
            } /* else leave the loop */  
        }
    } while ( true ) ;

    /*
     * we are alone now
     */

    if ( ( num_coll = loop_region_coll - reserved_cnt ) )
    {
        scb->collision_count += num_coll ;
        l_curr_task->reg_coll_cnt [ sid-1 ] += num_coll ;
    }
    scb->begexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
    l_curr_task->state = TSK_RUNNING_EO00;
    DBGOUT_T (( l_curr_task->index ));
}

/*
 *=======================================================================
 */

#undef MF__
#define MF__ MOD__"vbegexcl_dirty_read_loop"
/*ARGSUSED*/ /* pid unused */
static  void    vbegexcl_dirty_read_loop ( RTE_TaskId pid, REGION_ID sid )
{
    ten50_UKT_Control               * this_ukt;
    struct DDB4_SEMA               * scb = KGS-> semaphore + sid - 1 ;
    struct SEM_LISTE               * scbs= & scb->semaliste;
    struct TASK_TYPE               * l_curr_task;
    long                           tas_count = MAX_INTERLOCK_LOOPS;
    long LoopEnd ;
    long loop_region_coll = XPARAM(ulRegionCollisionLoop) ;
    long reserved_cnt = loop_region_coll, num_coll ; 

	DBGIN;

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);
    if ( sid == -1 )
    {
        return;
    }
    if ( KGS->state >= SERVER_SHUTDOWNREINIT && l_curr_task->type != TT_TW_EO00 )
    {
        GOTO_DISP (&this_ukt);    /* never come back... */
    }
    DBG1 (( MF__,"called, T%d, sid =%d\n", l_curr_task->index,sid ));
    scb = KGS-> semaphore + sid - 1 ;
    if ( scb->owner == l_curr_task )
    {
        MSGALL(( IERR_SECOND_REGION_REQUEST, MF__, sid, scb->semaname ));
        vabort ( WRITE_CORE ) ;
    }
    if ( KGS->traceflag && *(KGS->traceflag) && l_curr_task->type == TT_TW_EO00 )
        return;
    l_curr_task->state = TSK_VBEGEXCL_EO00;
    l_curr_task->sid   = sid;
    l_curr_task->begexcl_count++;
    l_curr_task->ulBegExclCalls++;
    l_curr_task->reg_excl_cnt [ sid-1 ]++; /* -1 because sid is a pascal style index */
    l_curr_task->excl_schachtel++;
    DBG1 (( MF__, "l_curr_task->sid = %d \n", l_curr_task->sid ));
    scbs = & scb->semaliste;
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    scb->get_count++;

    do  /* fh: if we have a multi-cpu machine, loop here */
    {
        LoopEnd = reserved_cnt <= 0 /* || scbs->last->taskid */ ||
                     ( l_curr_task->prio_flag && 
                       XPARAM(fBegExclQueWhenPrio) ) ;    
        if ( scb->owner != (struct TASK_TYPE *) 0 && !LoopEnd )     /* someone else is in the region (this is the dirty read) */
        {
            struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
            --reserved_cnt ;
            request->req_type = REQ_EXCLUSIVE ;
            e71_YieldTaskRunEnqu( this_ukt, l_curr_task, request, BASE_PRIO_REX );  
            if (reserved_cnt > 0 || l_curr_task->excl_schachtel > 1) 
                l_curr_task->ulDynPrio +=  PRIO_FACTOR; 

            GOTO_DISP (&this_ukt);
        }                 /* else leave the loop */  
        else
        {
            while ( INTERLOCK( scbs->sem_exclusive ))
            { 
                if ( ! -- tas_count )
                    if ( l_curr_task->type == TT_TW_EO00 &&
                         l_curr_task->ukt->activeTasks == 1 )
                    {
				        MSGD (( INFO_EXCL_TW_IS_SLEEPING ))
                        vsleep(l_curr_task->index,1);
                        tas_count = MAX_INTERLOCK_LOOPS;
                    }
                    else
                    {
				        MSGALL (( IERR_EXCL_TW_LOOPS_INDEF, MF__, sid ));
                        vabort ( WRITE_CORE );
                    }
                {
                    struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
                    request->req_type = REQ_TEST_AND_SET ;
                    e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );
                    l_curr_task->ulDynPrio +=  PRIO_FACTOR;
                }
                DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index));
                GOTO_DISP (&this_ukt);
                DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index));
            }  /* end-while interukp */
            if ( scb->owner == (struct TASK_TYPE *) 0 )   /* if there is noone else in the region, go on directly... */
                LoopEnd = true ;
            else
                if ( ! LoopEnd )
                {
                    struct DOUBLY_LINKED *request;
                    CLEARLOCK ( scbs->sem_exclusive );
                    --reserved_cnt ;
                    request =  GetTaskDLQElement1( l_curr_task );
  	                request->req_type = REQ_EXCLUSIVE ;

                    e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );  
                    if (reserved_cnt > 0 || l_curr_task->excl_schachtel > 1) 
                        l_curr_task->ulDynPrio +=  PRIO_FACTOR; 

                    GOTO_DISP (&this_ukt);
                }                 /* else leave the loop */  
        }
    } while ( !LoopEnd ) ;

    /*
     * we are alone now
     */
    if ( ! scb->owner )
    {   
        if ( KGS->state >= SERVER_SHUTDOWNREINIT
          && l_curr_task != KGS->tw )
        {
            CLEARLOCK ( scbs->sem_exclusive );
            GOTO_DISP (&this_ukt);
        }
        scb->owner = l_curr_task;
        CLEARLOCK ( scbs->sem_exclusive );
    }
    else
    {
        scbs->first->taskid = l_curr_task;
        scbs->first         = scbs->first->next;
        CLEARLOCK ( scbs->sem_exclusive );
        scb->wait_count++ ;
        this_ukt->NumRegWait++ ;
        DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index ));
        GOTO_DISP (&this_ukt);
        DBG1 (( MF__,"after goto_disp T%d \n", l_curr_task->index ));
    }

    if ( ( num_coll = loop_region_coll - reserved_cnt ) )
    {
        scb->collision_count += num_coll ;
        l_curr_task->reg_coll_cnt [ sid-1 ] += num_coll ;
    }
    scb->begexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
    l_curr_task->state = TSK_RUNNING_EO00;
    DBGOUT_T (( l_curr_task->index ));
}

/*
 *=======================================================================
 */

#undef MF__
#define MF__ MOD__"vendexcl"
/*ARGSUSED*/ /* pid unused */
void    vendexcl ( RTE_TaskId pid, REGION_ID sid )
{
    ten50_UKT_Control               * this_ukt;
    struct DDB4_SEMA   * scb;
    struct SEM_LISTE   * scbs;
    struct TASK_TYPE   * l_curr_task;
    long                 tas_count = MAX_INTERLOCK_LOOPS ;

	DBGIN_T(l_curr_task->index);

    PID_TCB(pid,l_curr_task);
    this_ukt = UKT(l_curr_task);
    if ( -1 == sid )
    {
        return;
    }

    /*JH frickel*/
    if ( KGS-> traceflag && * KGS-> traceflag && l_curr_task->type == TT_TW_EO00 )
        return;

    l_curr_task->state = TSK_VENDEXCL_EO00;

    scb  = KGS-> semaphore + sid - 1;
    scbs = & scb->semaliste;
 
    DBG1 (( MF__,"region   '%.8s' \n", scb->semaname ));
    if ( scb->owner != l_curr_task )
    {
		MSGALL (( IERR_VENDEXCL_WITHOUT_VBEG ,
                   l_curr_task->index,  sid, scb->semaname ));
		vabort ( WRITE_CORE ) ;
    }

        /* lock the list */
    if ( XPARAM(fExclQueue ) )  
        while ( INTERLOCK( scbs->sem_exclusive ))
        {
            if ( ! -- tas_count )
                if ( l_curr_task->type == TT_TW_EO00 &&
                     l_curr_task->ukt->activeTasks == 1 )
                {
				    MSGD (( INFO_EXCL_TW_IS_SLEEPING ))
                    vsleep(l_curr_task->index,1);
                    tas_count = MAX_INTERLOCK_LOOPS;
                }
                else
                {
				    MSGALL (( IERR_EXCL_TW_LOOPS_INDEF, MF__, sid ));
                    vabort ( WRITE_CORE );
                }
            if ( ! XPARAM(fEndExclBusyWait)  )  /* the parameter MP_RGN_BUSY_WAIT decides whether blocking or non-blocking wait is to be used here (and nothing else!)  */
            {
                struct DOUBLY_LINKED *request =  GetTaskDLQElement1( l_curr_task );
                request->req_type = REQ_TEST_AND_SET ;
                e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );
                l_curr_task->ulDynPrio +=  PRIO_FACTOR;
                DBG1 (( MF__,"before goto_disp T%d \n", l_curr_task->index));
                GOTO_DISP (&this_ukt);
                DBG1 (( MF__,"after  goto_disp T%d \n", l_curr_task->index));
            }
        }
    /*
     * we are alone now
     */

    if ( scbs->last->taskid  )
    {
        /*
         * somebody else is waiting for begexcl
         */
        scb->owner             = scbs->last->taskid  ; /* the new owner is the one that has been waiting for the longest time */
        scbs->last->taskid     = 0;
        scbs->last             = scbs->last->next;
        CLEARLOCK ( scbs->sem_exclusive ); /* free the list - This is only possible if fExclQueue is set. But if this was set, we would not have come here...*/
        scb->endexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
        if ( --l_curr_task->excl_schachtel == 0 )
          l_curr_task->sid = 0;
        if ( scb->owner->ukt == this_ukt ) /* everything happens within this ukt. This could be done by en71EnqueueUkt2UktAndWake(), but this is much faster, because no synchronisation mechanisms must be used */
        {
            struct DOUBLY_LINKED *request = GetTaskDLQElement1( (struct TASK_TYPE*)scb->owner );
            this_ukt->NumRegWait-- ;
            request->req_type = REQ_EXCLUSIVE ;

            e71_EnquRun ( this_ukt, (struct TASK_TYPE*)scb->owner, this_ukt->curr_task, request, BASE_PRIO_RAV );
            scb->owner->ulDynPrio += PRIO_FACTOR;

            if ( !l_curr_task->prio_flag )  /* */
            {
                request = GetTaskDLQElement1( l_curr_task );
                request->req_type  = REQ_RESCHEDULE ;
                l_curr_task->state = TSK_RUNNABLE_EO00;
                e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );  /* enter myself in the run queue, with a lower priority than the process waiting for access to the task list */
                GOTO_DISP (&this_ukt);   /* and call the dispatcher */
            }
        }
        else
        {
            en71EnqueueUkt2UktAndWake ( this_ukt, 
                                        (struct TASK_TYPE *)scb->owner, 
                                        this_ukt->curr_task,
                                        e55_remactive, 
                                        (void *)sid );
        }
    }
    else
    {
        /*
         * Queue is empty
         */
        scb->owner               = (struct TASK_TYPE *) 0;  /* show that the list is now empty */
        if ( XPARAM(fExclQueue) )
        {
            CLEARLOCK ( scbs->sem_exclusive ); /* free the list */
            scb->endexcl_tas_count += MAX_INTERLOCK_LOOPS - tas_count ;
        }
        --l_curr_task->excl_schachtel;
        if ( ( l_curr_task->begexcl_count > XPARAM(ulRegLockSlice) || this_ukt->SelfDispatch > 0 ) 
           &&
             !l_curr_task->prio_flag 
           &&
              l_curr_task->excl_schachtel == 0 
           )
        {
            struct DOUBLY_LINKED *request;
            if ( this_ukt->SelfDispatch > 0 ) this_ukt->SelfDispatch-- ; 
            l_curr_task->state = TSK_RUNNABLE_EO00;
            l_curr_task->Totalself_susp_cnt++ ;  /* why only here??? */
            l_curr_task->self_susp_cnt++ ;  /* why only here??? */

            request = GetTaskDLQElement1( l_curr_task );
            request->req_type = REQ_RESCHEDULE ;
            e71_YieldTaskRunEnqu ( this_ukt, l_curr_task, request, BASE_PRIO_REX );  /* enter myself in the run queue, with a lower priority than the process waiting for access to the task list */
            GOTO_DISP (&this_ukt);   /* and call the dispacther */
        }
    }

/*
    scb->last_owner = l_curr_task ;  
*/
    l_curr_task->state = TSK_RUNNING_EO00;
    l_curr_task->sid   = 0 ;
	DBGOUT_T(l_curr_task->index);
}
/*
 * ===========================================================================
 */
#undef MF__
#define MF__ "e55_remactive"
void    e55_remactive ( 
void *param ,
struct TASK_TYPE *FromTask)
{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
	DBGIN_T(this_ukt->curr_task->index);
    /*
     * this procedure is a dummy, only to ensure that the task is activated
     */
    this_ukt->NumRegWait-- ;
    DBGOUT_T(this_ukt->curr_task->index);
}

/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"visexcl"
tsp00_Bool visexcl ( 
RTE_TaskId		pid ,
REGION_ID               sid)

{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    struct DDB4_SEMA                * scb;

	DBGIN;

    if ( -1 == sid )
    {
        return (true);
    }

    if ( sid == -1 )
        return ( this_ukt->curr_task->excl_schachtel != 0 ) ? true : false;

    scb  = KGS-> semaphore + sid - 1;

    if ( scb->owner == this_ukt->curr_task )
    {
        return (true);
    }
    else
    {
        return (false);
    }
	DBGOUT;
}
/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"vsemaphid"
void    vsemaphid ( 
tsp00_C8                          semaname,
REGION_ID                       * sid)
{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    int                             i;
    struct DDB4_SEMA                * scb;

	DBGIN_T(this_ukt->curr_task->index);
    DBG1 (( MF__,"region   '%.8s' \n", semaname ));

    /*
     * Initialisation already done
     */
    for ( i = 0; i < XPARAM(ulNoOfRegions) ; i ++ )
    {
        if ( KGS-> semaphore [ i ].semaname [ 0 ] == '\0' ) break;
    }
    if ( i >= XPARAM(ulNoOfRegions) )
    {
        MSGCD (( IERR_TOO_MANY_REGIONS_REQ ))  
#ifdef  DEBUG_RTE
        for ( i = 0; i < XPARAM(ulNoOfRegions) ; i ++ )
        {
            DBG1 (( MF__,"region '%.8s' \n", KGS-> semaphore [ i ].semaname ));
        }
#endif
        vabort ( !WRITE_CORE );
    }
    * sid = i + 1;
    scb = & KGS-> semaphore [ i ];
	eo46PtoC ( scb->semaname, semaname, sizeof(tsp00_C8) );
    scb->owner = (struct TASK_TYPE *) 0;
    init_semaliste ( scb );
    
    DBG1 (( MF__,"returning, sid %d \n", * sid ));
	DBGOUT;
}


/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"vsemainfo"
void vsemainfo ( 
REGION_ID                       sid,
tsp00_C8                          semaname)
{
    DBGIN;
    if ( sid <= XPARAM(ulNoOfRegions) )
    {
	    eo46CtoP ( semaname, KGS-> semaphore [sid-1].semaname, sizeof(tsp00_C8) );
        DBG1 (( MF__,"region %d '%.8s' \n", sid, KGS-> semaphore [sid-1].semaname ));
    }
    else
    {
        SAPDB_memset(semaname, ' ', sizeof(tsp00_C8));
    }
    DBGOUT;
}
