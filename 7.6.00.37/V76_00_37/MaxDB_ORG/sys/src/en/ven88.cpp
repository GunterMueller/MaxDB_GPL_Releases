/*!
  @file           ven88.cpp
  @author         JoergM
  @brief          Kernel RunTime: User Kernel Thread
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



#define MOD__ "ven88.cpp:"

/* exported functions */

#include "gen88.h"

/* imported functions */
#include "geo007_1.h"
#include "geo50_0.h"
#include "heo69.h"  /* vsReinit() */
#include "geo002.h" /* XPARAM(showMaxStackUse/fUseProcessScope) */
#include "heo92.h"  /* eo92GetTaskTypeName() */
#include "heo07.h"  /* Thread functions */
#include "hen40.h"  /* sqlerrs() */
#include "hen50.h"  /* vabort() */
#include "gen71.h"  /* en71Dispatcher() */
#include "gen73.h"  /* en73_dl_enqu() */
#include "gen74.h"  /* e74_init_big_comseg() */
#include "gen81.h"
#ifdef DEBUG_RTE
#include "SAPDBCommon/SAPDB_sprintf.h" /*nocheck*/
#endif

#ifdef DEBUG_RTE
#include "gen38.h" /* e38_type_name() */
#endif /* DEBUG_RTE */

#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/RTE_Coroutine.h"
#include "RunTime/RTE_ITaskStackControl.hpp"
#include "hen51.h"
#include "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"
#include "RunTime/Tasking/RTETask_Task.h"
#include "RunTime/System/RTESys_MicroTime.h"
#ifndef FREEBSD
#include <alloca.h>
#endif

/*
 * Interface function of AK layer pascal procedure (for which no header file will ever exist...)
 */
externC void en88_CallCoroutineKernelTaskMain ();

/* local functions */

//  Must be visible only because C++ does not allow static extern "C" but pthread main entry must be extern "C"
externC void *      en88_RunPthreadTask(void *arg);
static  void        en88_InitializePthreadTaskUKT( struct  TASK_TYPE * firstTask );
static  void        en88_CallKernelTaskMain ( struct  TASK_TYPE * tcb );
static  void        en88_InitPthreadTasks ( ten50_UKT_Control *this_ukt );

volatile int en88_NeverStop = 1; /* used to prevent compiler warnings on wanted endless loops */

/*
  Function: en88CreateAndPrepareTaskStacks
  Description: Create and prepare all kernel task stacks

  There are several possible implementation variations:

  a) The kernel task stacks occupy a single contigous section in memory which is not separated by guard pages or gaps.
     Each task stack size is system page aligned. This model is used for HAS_VERY_LIMITED_VIRTUAL_ADDRESS_SPACE
     (default model for 32Bit maschines)

  b) The kernel task stacks occupy a single contigous section in memory which but are separated by guard pages or gaps.
     Each task stack size is system page aligned. This model is used for HAS_LIMITED_VIRTUAL_ADDRESS_SPACE

  c) The kernel task stacks occupy a single contigous section in memory which is not separated by guard pages or gaps
     but each task stack separated by the maximum task stack size of all task stacks HAS_LIMITED_NUMBER_OF_MMAP_SECTIONS

  d) Each kernel task stack gets its own mmap section, protected by gaps and with a fixed size spacing allowing gaps
     of unmapped pages in between (default model for 64Bit maschines).

  The fixed separation allows quiet fast calculation of task id using current task stack addresses. To allow this in model
  a) and b) only two kind of task stacks are allowed: UserTaskStacks and NonUserTaskStacks. In model c) and d) the maximum
  task stack which is allowed for all tasks is used as spacing distance. In model c) and d) the task stacks are allowed
  to grow up beginning with a definied minimum up to a defined maximum, while in model a) and b) the task stack are allocated
  once and forever at start.
 */
extern "C" void en88CreateAndPrepareTaskStacks()
{
#undef  MF__
#define MF__ MOD__"en88CreateAndPrepareTaskStacks"

    RTE_ITaskStackControl::Instance().Initialize(KGS->ulNumOfTasks, KGS->pFirstTaskCtrl, KGS->pLastTaskCtrl);
}

/*---------------------------------------------------------------------------
  Function: vgetpid
  Description: Return task id

  Using the address of a local variable the stack area is searched and the task index returned.
  If called from non task thread, a zero is returned.

  Argument: taskId [out] task id set with current task id
 */
extern "C" void vgetpid (
tsp00_TaskId &taskId )
{
#undef  MF__
#define MF__ MOD__"vgetpid"
    if ( !KGS || !KGS->XParam )
    {
        taskId = 0;
    }
    else
    {
        struct TASK_TYPE *tcb = RTE_ITaskStackControl::Instance().GetTaskControl();

        taskId = ( 0 == tcb ? 0 : tcb->index );
    }
}

/*---------------------------------------------------------------------------
  @brief Return task

  Using the address of a local variable the stack area is searched and the
  task control pointer returned.
  If called from non task thread, a zero is returned.
 */
extern "C" struct TASK_TYPE * RTE_GetOwnTask()
{
    if ( !KGS || !KGS->XParam )
        return 0;

    return RTE_ITaskStackControl::Instance().GetTaskControl();
}

/*---------------------------------------------------------------------------
  @brief Return task id

  Using the address of a local variable the stack area is searched and the task index returned.
  If called from non task thread, a zero is returned.
 */
RTE_TaskId RTE_GetOwnTaskId()
{
    if ( !KGS || !KGS->XParam )
        return 0;

    struct TASK_TYPE *tcb = RTE_ITaskStackControl::Instance().GetTaskControl();
    return (RTE_TaskId)( 0 == tcb ? 0 : tcb->index );
}

/*---------------------------------------------------------------------------
  @brief Return task context pointer
  @param the task id
 */
RTE_CoroutineContext *RTE_GetTaskContext(RTE_TaskId id)
{
    if ( !KGS || !KGS->XParam )
        return 0;

    return (KGS->pFirstTaskCtrl + (id-1))->taskContext;
}

/*---------------------------------------------------
  Function: en88GetThisUkt
  Description: Returns current UKT control structure pointer or NULL if not called from within UKT

  This implements the THIS_UKT macro
  Return value: pointer to current UKT control structure or NULL if not called from within a UKT
 */
extern "C" ten50_UKT_Control *en88GetThisUkt()
{
#undef MF__
#define MF__ MOD__ "en88GetThisUkt"
    if ( !KGS || !KGS->XParam )
        return 0;

    struct TASK_TYPE *tcb = RTE_ITaskStackControl::Instance().GetTaskControl();

    return ( 0 == tcb ? (ten50_UKT_Control *)0 : tcb->ukt );
}

/*---------------------------------------------------------------------------
  Function: en88InitCoroutineTask
  Description: Initialize tasks using coroutines for task

  Arguments: tcb [in] Pointer to current task control block
 */
extern "C" tsp00_Int4 en88InitCoroutineTask ( struct  TASK_TYPE * tcb )
{
#undef MF__
#define MF__ MOD__ "en88InitCoroutineTask"
    DBGIN_T(tcb->index);

    tcb->taskContext = RTE_ContextAllocate(RTEMem_RteAllocator::Instance());
    if ( !tcb->taskContext )
    {
        MSGD(( 11900, ERR_TYPE, "TASKING ", "allocating context Task T%d failed", tcb->index ));
        vabort(WRITE_CORE);
        return false;
    }
    DBGOUT_T(tcb->index);

    return true;
}

/*---------------------------------------------------------------------------
  Function: en88_InitDynPthreadTasks
  Description: Initialize tasks using pthread for task

  The given tcb is recreated dynamically

  Arguments: tcb [in] Pointer to current task control block
 */
extern "C" tsp00_Int4 en88_InitDynPthreadTasks ( struct  TASK_TYPE * tcb )
{
#undef MF__
#define MF__ MOD__ "en88_InitDynPthreadTasks"
    DBGIN_T(tcb->index);

    tsp00_ErrTextc threadErrText;
    teo07_ThreadErr threadCallErr;

    sqlbeginthread_on_own_stack( (SAPDB_Int4)(tcb->StackChunkEnd - tcb->StackChunkStart),
                                 tcb->StackChunkStart,
                                 en88_RunPthreadTask,
                                 tcb,
                                 THR_CREATE_SUSPENDED_EO07
                               | ( XPARAM(fUseProcessScope) ? THR_CREATE_PROCESS_SCOPE_EO07 : 0 ),
                                &tcb->taskThread,
                                 threadErrText,
                                &threadCallErr );
    if ( THR_OK_EO07 != threadCallErr )
    {
        MSGD(( ERR_CREATING_THREAD, threadErrText.asCharp(), threadCallErr )); /* PTS 1115185 */
        return false;
    }

    DBGOUT_T(tcb->index);

    return true;
}

/*-------------------------------------------------------------*/
/*   U K T       Code                                          */
/*-------------------------------------------------------------*/

/*
   @brief Continue executing coroutine based UKT
   Separated to allow stack allocation via alloca() in calling subroutine

   @param this_ukt [in] the running ukt
   @return always 0
 */
extern "C" void *en88ExecuteCoroutineUKT(ten50_UKT_Control *this_ukt)
{
    // starting task of UKT has to complete initialization of UKT structure
    this_ukt->state      = KT_RUNNING;

    this_ukt->start_time = KGS->current_time;
    this_ukt->IdleTimeMeasureBeginMicroseconds = this_ukt->UKTStartTimeMicroSeconds = RTESys_MicroSecTimer();

    // Initialize communication segment of this UKT
    e74_init_big_comseg (this_ukt);

    this_ukt->started = true;
    /*
     * If timer task was the initializing task, dispatcher is not needed
     */
    if ( KGS->ti->ukt == this_ukt )
    {
        this_ukt->curr_task = KGS->ti;
        // we altered the current task in legacy code,
        // so we have to alter the current task object in
        // the TaskScheduler object
        UpdateCurrentTaskInTaskScheduler( this_ukt );
    }
    else
    {
        this_ukt->curr_task = (struct TASK_TYPE*) 0 ;
        en71Dispatcher (this_ukt);
    }

//    do {
        RTE_ContextSwitch( *(this_ukt->removeTaskContext),
                           *(this_ukt->curr_task->taskContext) );
        // finalize....
//        this_ukt->curr_task = (struct TASK_TYPE*) 0 ;
//        en71Dispatcher (this_ukt);
//    } while ( this_ukt->curr_task );
    return (void *)0;
}

//---------------------------------------------------------------

static struct TASK_TYPE *
en88_RelatedTaskStackInfo( struct TASK_TYPE *pTask,
                           SAPDB_ULong &singleStackSize,
                           RTE_TaskId  &firstTaskId,
                           SAPDB_ULong &numberOfTasksToAdd )
{
struct TASK_TYPE *pNextTask;
RTE_TaskId nextTaskId;

    singleStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(pTask->stack_size);
    firstTaskId = pTask->index;
    nextTaskId = firstTaskId + 1;

    for ( pNextTask = pTask->next_task; pNextTask; pNextTask = pNextTask->next_task )
    {
        if ( pNextTask->index != nextTaskId
          || RTE_ISystem::Instance().RoundUpToSystemPageSize(pNextTask->stack_size) != singleStackSize )
        {
            break;
        }
        ++nextTaskId;
    }
    numberOfTasksToAdd = nextTaskId - firstTaskId;
    return pNextTask;
}

/*-------------------------------------------------------------
  Function: en88RunCoroutineUKT
  Description: Common entry point for all user kernel threads using coroutines

  This routine is non static only because C++ does not allow extern "C" functions to be static...

  en88_CallKernelTaskMain is the C++ exception catching code.

  Arguments: arg [in] the user kernel thread control block for this ukt

  Return value: always 0, since no exit code transferred
 */
extern "C" void *en88RunCoroutineUKT(void *arg)
{
#undef MF__
#define MF__ MOD__ "en88RunCoroutineUKT"
    ten50_UKT_Control *this_ukt = (ten50_UKT_Control *)arg;

    MSGD(( INFO_UNIX_UKT_STARTED , this_ukt->index )) ;

    this_ukt->removeTaskContext = RTE_ContextAllocate(RTEMem_RteAllocator::Instance());
    if ( !this_ukt->removeTaskContext )
    {
        MSGD(( 11900, ERR_TYPE, "TASKING ", "allocating remove context of UKT%d failed", this_ukt->index ));
        vabort(WRITE_CORE);
    }

    RTE_ContextPrepare( *(this_ukt->removeTaskContext), 0, 0, 0);

    SAPDB_Int4 totalAlignedStackSize;
    struct TASK_TYPE *pTask;
    for ( totalAlignedStackSize = 0, pTask = this_ukt->pFirstTaskCtrl;
          pTask;
          pTask = pTask->next_task )
    {
#ifdef DEBUGCOROUTINE
        printf("UKT%d T%d\n", this_ukt->index, pTask->index);
        fflush(stdout);
#endif
        totalAlignedStackSize += RTE_ISystem::Instance().RoundUpToSystemPageSize(pTask->stack_size);
    }

    /* as long as it need tasks on stack of pthread this will be true... */
    if ( XPARAM(fUseStackOnStack) )
    {
        SAPDB_Byte *UKTtaskStack = (SAPDB_Byte *)alloca(totalAlignedStackSize+RTE_ISystem::Instance().GetSystemPageSize());

        SAPDB_Byte *stackStartAddress = (SAPDB_Byte *)0 +
            RTE_ISystem::Instance().RoundUpToSystemPageSize(UKTtaskStack - (SAPDB_Byte *)0);

        SAPDB_Int   alignedStackSize;
        for ( pTask = this_ukt->pFirstTaskCtrl; pTask; pTask = pTask->next_task )
        {
            alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(pTask->stack_size);
            pTask->StackChunkStart = stackStartAddress;
            pTask->StackChunkEnd   = stackStartAddress + alignedStackSize;
            pTask->stack_size      = pTask->stack_size;

            pTask->GuardPageBegin  = 0;
            pTask->GuardPageEnd    = 0;
            stackStartAddress     += alignedStackSize;
        }

        SAPDB_Byte *taskStack = (SAPDB_Byte *)0 +
            RTE_ISystem::Instance().RoundUpToSystemPageSize(UKTtaskStack - (SAPDB_Byte *)0);

        for ( pTask = this_ukt->pFirstTaskCtrl; pTask; )
        {
            SAPDB_ULong singleStackSize;
            RTE_TaskId  firstTaskId;
            SAPDB_ULong numberOfTasksToAdd;

            struct TASK_TYPE *pNextTask = en88_RelatedTaskStackInfo(pTask, singleStackSize, firstTaskId, numberOfTasksToAdd);

            if ( !RTE_ITaskStackControl::Instance().AddTaskStackMapping( taskStack
                                                                       ,singleStackSize
                                                                       ,firstTaskId
                                                                       ,numberOfTasksToAdd ) )
            {
                MSGD(( 11900, ERR_TYPE, "TASKING ", "add task stack mapping Task%d (%d tasks) failed", firstTaskId, numberOfTasksToAdd ));
                vabort(WRITE_CORE);
            }
            taskStack += (singleStackSize * numberOfTasksToAdd);
            pTask = pNextTask;
        }

        for ( pTask = this_ukt->pFirstTaskCtrl; pTask; pTask = pTask->next_task )
        {
#ifdef DEBUGCOROUTINE
            printf("Preparing 0x%p for Task T%d\n", pTask->taskContext, pTask->index);
            fflush(stdout);
#endif
            RTE_ContextPrepare( *(pTask->taskContext),
                                pTask->StackChunkStart,
                                pTask->stack_size,
                                (void *)en88_CallCoroutineKernelTaskMain);
        }
        return en88ExecuteCoroutineUKT(this_ukt);

    }
    else
    {
        for ( pTask = this_ukt->pFirstTaskCtrl; pTask; pTask = pTask->next_task )
        {
#ifdef DEBUGCOROUTINE
            printf("Preparing 0x%p for Task T%d\n", pTask->taskContext, pTask->index);
            fflush(stdout);
#endif
            RTE_ContextPrepare( *(pTask->taskContext),
                                pTask->StackChunkStart,
                                pTask->stack_size,
                                (void *)en88_CallCoroutineKernelTaskMain);
        }
        return en88ExecuteCoroutineUKT(this_ukt);
    }
}

/*-------------------------------------------------------------
  Function: en88_RunPthreadTask
  Description: Common entry point for all task threads

  This routine is non static only because C++ does not allow extern "C" functions to be static...

  en88_CallKernelTaskMain is the C++ exception catching code.

  Arguments: arg [in] the task control block for this task

  Return value: always 0, since no exit code transferred
 */
extern "C" void *en88_RunPthreadTask(void *arg)
{
#undef MF__
#define MF__ MOD__ "en88_RunPthreadTask"
    struct  TASK_TYPE * tcb = (struct TASK_TYPE *)arg;

    DBGIN_T(tcb->index);

    tcb->taskThreadId = sqlgetthreadid();

    if ( KT_INITIALIZED == tcb->ukt->state  )
    {
        // starting task of UKT has to complete initialization of UKT structure
        en88_InitializePthreadTaskUKT(tcb);
    }

    en88_CallKernelTaskMain( tcb );

    DBGOUT_T(tcb->index);

    return (void *)0;
}

/*----------------------------------------------------*/
extern "C" void en88TaskContextSwitch( struct TASK_TYPE * pOldTask,
                                       struct TASK_TYPE * pNewTask )
{
   if ( XPARAM(fUseCoroutines) )
   {
       if ( !RTE_ContextSwitch( *(pOldTask->taskContext),
                                *(pNewTask->taskContext) ) )
       {
           MSGD(( 11900, ERR_TYPE, "TASKING ", "switching to next task failed" ));
           vabort(true);
       }
   }
   else
   {
      tsp00_ErrTextc   threadErrtext;
      teo07_ThreadErr  threadErr;
      teo07_Thread     oldThread = pOldTask->taskThread;

      sqlresumethread( pNewTask->taskThread, threadErrtext, &threadErr );
      if ( threadErr != THR_OK_EO07 )
      {
          MSGD(( 11900, ERR_TYPE, "TASKING ", "resuming next task failed" ));
          MSGD(( 11900, ERR_TYPE, "TASKING ", &threadErrtext[0] ));
          vabort(true);
      }

      sqlsuspendthread( oldThread, threadErrtext, &threadErr );
      if ( threadErr != THR_OK_EO07 )
      {
          MSGD(( 11901, ERR_TYPE, "TASKING ", "suspend task itself failed" ));
          MSGD(( 11901, ERR_TYPE, "TASKING ", &threadErrtext[0] ));
          vabort(true);
      }
   }
}

/*-------------------------------------------------------------
   Function: en88_InitializePthreadTaskUKT
   Description: Initialize an UKT

   Called from first active task in UKT (state == KT_INITIALIZED)
   Arguments: firstTask [in]
 */
static void en88_InitializePthreadTaskUKT( struct  TASK_TYPE * firstTask )
{
#undef MF__
#define MF__ MOD__"en88_InitializePthreadTaskUKT"

    ten50_UKT_Control *this_ukt = firstTask->ukt;

    this_ukt->state = KT_RUNNING;
    this_ukt->start_time = KGS->current_time;
    this_ukt->IdleTimeMeasureBeginMicroseconds = this_ukt->UKTStartTimeMicroSeconds = RTESys_MicroSecTimer();

    MSGD(( INFO_UNIX_UKT_STARTED , this_ukt->index )) ;

    DBG1 (( MF__,"ukt     0x%lx", this_ukt ));
    DBG1 (( MF__,"semid   %d", this_ukt->semid ));

    /*
     * Calling thread is UKT thread
     */
    SAPDB_memcpy( &this_ukt->thread, &firstTask->taskThread, sizeof(teo07_Thread) );
    this_ukt->tid = firstTask->taskThreadId;

    // Initialize communication segment of this UKT
    e74_init_big_comseg (this_ukt);

    /*
     * If timer task was the initializing task, dispatcher is not needed
     */
    if ( KGS->ti == firstTask )
    {
        DBG1 (( MF__,"starting the timeout task T%d", this_ukt->curr_task->index ));
        this_ukt->curr_task = firstTask ;
    }
    else
    {
        DBG1 (( MF__,"entering dispatcher to find first task" ));
        this_ukt->curr_task = (struct TASK_TYPE*) firstTask;
        GOTO_DISP (&this_ukt);
    }
}

/*---------------------------------------------------------------------------
  Function: en88_CallCoroutineKernelTaskMain
  Description: All UKT C++ main function

  Check for initial request. If not found call pascal main in endless loop.
  Will never return. If exception is caught, it is printed via RTE_Crash()

  Arguments; tcb [in] task control block of this task
 */
extern "C" void en88_CallCoroutineKernelTaskMain ()
{
    en88_CallKernelTaskMain(RTE_ITaskStackControl::Instance().GetTaskControl());
}

/*---------------------------------------------------------------------------
  Function: en88_CallKernelTaskMain
  Description: All UKT C++ main function

  Check for initial request. If not found call pascal main in endless loop.
  Will never return. If exception is caught, it is printed via RTE_Crash()

  Arguments; tcb [in] task control block of this task
 */
static void en88_CallKernelTaskMain ( struct  TASK_TYPE * tcb )
{
#undef MF__
#define MF__ MOD__"en88_CallKernelTaskMain"
    try {
        ten50_UKT_Control *this_ukt = tcb->ukt;
#ifdef DEBUG_RTE
char eBuffer[256];
SAPDB_sprintf(eBuffer, sizeof(eBuffer),"---> Task%d(%s) UKT%d (curr T%d) curr_quu 0x%p",
    tcb->index,
    e38_type_name(this_ukt->curr_task->type),
    this_ukt->index,
    this_ukt->curr_task->index,
    this_ukt->curr_quu);
MSGD(( INFO_TEST_WILDCARD, eBuffer ));
#endif

        while ( en88_NeverStop )
        {
            /* PTS 1110953 */
            if ( XPARAM(showMaxStackUse) )
            {
                vsReinit(this_ukt->curr_task->index,0);
            }

/*-----------------------------------------------*/
/*            Entering kernel zone               */
/*-----------------------------------------------*/
            MSGD(( INFO_UNIX_TASK_STARTED , tcb->index )) ;
            RTETask_TaskMain();

/*-----------------------------------------------*/
/*            Leaving kernel zone                */
/*-----------------------------------------------*/
            en51Finish();

        }
    }
    catch(SAPDBErr_MessageList &lastMsg)
    {
        RTE_Crash(lastMsg);
    }

}


extern "C" void en88memcpy(void *a,void *b,SAPDB_UInt s)
{
    memcpy(a,b,s);
}
