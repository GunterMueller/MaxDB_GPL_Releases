/*!
  @file           gen50.h
  @author         JoergM
  @special area   Kernel LZU UNIX
  @brief          Kernel interne Strukturen
  @see            example.html ...

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef GEN50_H
#define GEN50_H

#ifdef  USE_KGS

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gen00.h"
#include "geo00_3.h"
#include "geo52.h"
#include "heo00.h"
#include "heo07.h"
#include "vsp002.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/Tasking/RTETask_LegacyQueueElement.h"
#include "RunTime/Tasking/RTETask_LegacyQueueElementFreeList.h"
#include "KernelCommon/Kernel_SharedState.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define NO_DISTRIBUTION

/*
   --- Kernel threads stack sizes 
   These sizes are used as input for sqlbeginthread() calls
 */
#if defined (HP_IA64) || (defined(LINUX) && defined(IA64))
#define UKT_BASE_STACK_SIZE             ( 768 * 1024)
#define UKT_TEMP_STACK_SIZE             ( 768 * 1024)
#define KT_WORKER_STACK_SIZE            ( 512 * 1024)
#define KT_TIMER_STACK_SIZE             ( 512 * 1024)
#define KT_CLOCK_STACK_SIZE             ( 512 * 1024)
#define KT_DEV0_STACK_SIZE              ( 512 * 1024)
#define KT_DEVI_STACK_SIZE              ( 512 * 1024)
#define KT_CONSOLE_STACK_SIZE           ( 512 * 1024)
#define KT_ASYNC0_STACK_SIZE            ( 512 * 1024)
#define KT_ASYNCi_STACK_SIZE            ( 512 * 1024)
#define KT_REQUESTOR_STACK_SIZE         ( 512 * 1024)
#define KT_COORDINATOR_STACK_SIZE       ( 512 * 1024)
#define KT_DEV_WORKER_STACK_SIZE        ( 512 * 1024)
#else
#define UKT_BASE_STACK_SIZE             ( 128 * 1024)
#define UKT_TEMP_STACK_SIZE             ( 128 * 1024)
#define KT_WORKER_STACK_SIZE            ( 128 * 1024)
#define KT_TIMER_STACK_SIZE             ( 128 * 1024)
#define KT_CLOCK_STACK_SIZE             ( 128 * 1024)
#define KT_DEV0_STACK_SIZE              ( 128 * 1024)
#define KT_DEVI_STACK_SIZE              ( 128 * 1024)
#define KT_CONSOLE_STACK_SIZE           ( 128 * 1024)
#define KT_ASYNC0_STACK_SIZE            ( 128 * 1024)
#define KT_ASYNCi_STACK_SIZE            ( 128 * 1024)
#define KT_REQUESTOR_STACK_SIZE         ( 128 * 1024)
#define KT_COORDINATOR_STACK_SIZE       ( 128 * 1024)
#define KT_DEV_WORKER_STACK_SIZE        ( 128 * 1024)
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------------
 *  Macros for dispatching
 */
#if 0
#if defined(HPUX)
/* PTS 1114487 */
/* The HPUX compiler supports a much more effectiv way to use thread local storage... */
   extern __thread void *en81ThisUktControl;
#  define THIS_UKT_CTRL ((ten50_UKT_Control *)en81ThisUktControl)
#else
externC void *en81_Get_KT_TLS( void );
#  define THIS_UKT_CTRL ((ten50_UKT_Control *) en81_Get_KT_TLS())
#endif /* HPUX */
#else
#  define THIS_UKT_CTRL en88GetThisUkt()
#endif

#  define THREAD_INDEX(_t) ((SAPDB_Int4)((_t)->index))

#define __GOTO_DISPATCHER(_ppUKT) {                                                    \
   struct TASK_TYPE*   oldTask_ = (*_ppUKT)->curr_task ;                               \
  (*_ppUKT)->curr_task->begexcl_count = 0 ;                                            \
                                                                                       \
  en71Dispatcher (*_ppUKT);                                                            \
                                                                                       \
  if ( oldTask_ != (*_ppUKT)->curr_task )                                              \
  {                                                                                    \
      (*_ppUKT)->taskSwitches++;                                                       \
      en88TaskContextSwitch( oldTask_, (*_ppUKT)->curr_task );                         \
      *_ppUKT = oldTask_->ukt;                                                         \
  }                                                                                    \
}

#if !defined(__cplusplus)
typedef struct ten50_UKT_Control *UKT_CTRL;
#else
#define UKT_CTRL ten50_UKT_Control*
#endif

externC void RTETask_ProcessDebugTaskRequest ( UKT_CTRL *ppUKT ); 

#define GOTO_DISP(_ppUKT)                                                     \
      __GOTO_DISPATCHER(_ppUKT);                                              \
      if ( (*_ppUKT)->curr_task->fDebugTask )                                 \
         RTETask_ProcessDebugTaskRequest ( _ppUKT ); 

#if defined(__cplusplus)
#undef UKT_CTRL 
#endif

#define RELEASE_FREELIST_ELEM(_pReq)                                \
      PutElementToFreeList( _pReq )
#define RESERVE_FREELIST_ELEM()                                     \
      GetElementFromFreeList()

#define RESUME_TRACEWRITER(_pTW)                                    \
    {                                                               \
    struct DOUBLY_LINKED *  _pRequest = RESERVE_FREELIST_ELEM();    \
    _pRequest->req_type = REQ_RESUME_TW_EO00;                       \
    en71_iocEnquAndWake( _pTW->ukt, _pTW, _pRequest );              \
    }



/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined (__cplusplus)
extern "C" {
#endif

typedef void (* PFNREMOTEFUNC )( void*              funcParam,
                                 struct TASK_TYPE*  fromTask);

#if defined (__cplusplus)
}
#endif
/*
 * Some Queue elements  are initially  listed in the Free-list |
 */

/*
 * The UKT2UKT queue (alias XXX) element
 * contains pointer to nexdt in fixed ring as well as all parameters needed for the function call
 */

struct XXX_QUEUE
{
    struct XXX_QUEUE                   *next;
    PFNREMOTEFUNC                       to_execute;
    void                               *func_param ;
    struct         TASK_TYPE           *task;
    struct         TASK_TYPE           *FromTask;
};


/*
 * The UKT2UKT-queue (alias XXX) is a fixed list of maxtasks elements pointing in ring
 */
struct XXX_QUEUE_HEAD
{
    SAPDB_UInt4               num_overflows ;
    SAPDB_UInt4               filler;
    struct XXX_QUEUE *xx_work;
    struct XXX_QUEUE *element;
    struct XXX_QUEUE *xx_request;
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT   exclusive;
   ALONE_IN_CACHE(dum_behind)
    /*
     * xx_work        is only used by server
     * xx_request     is used to enter requests,
     * entries.task is nulled when completed
     * xx_work should always follow the xx_request
     */
};

/*
 * The ioc-queue is a fixed list pointing in ring
 */
struct IOC_QUEUE_HEAD
{
    struct IOC_QUEUE          *first;
    struct IOC_QUEUE          *last;
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT exclusive;
   ALONE_IN_CACHE(dum_behind) 
    /*
     * first is first free element
     * last is last used element
     */
};


/*
 * The ior-queue is a fixed list pointing in ring
 */
struct IOR_QUEUE_HEAD
{
    struct IOR_QUEUE           *first;
    struct IOR_QUEUE           *last;
    /*
     * first is first free element
     * last is last used element
     */
};

/*
 * allocate one table of MAX_TPP + 1 for each DEV_PCB, where io requests
 * and corresponding replies are stored
 * The table is circularly linked, and upon completion of a request
 * the completed result is moved one position backward
 */

struct IOC_QUEUE
{
    struct IOC_QUEUE           *next;
    struct IOC_QUEUE           *previous;
    struct DOUBLY_LINKED       *request;
};

struct IOR_QUEUE
{
    struct IOR_QUEUE           *next;
    struct IOR_QUEUE           *previous;
    struct DOUBLY_LINKED       *request;
};

struct DOUBLE_Q_HEAD
{
    struct DOUBLY_LINKED       *first;
    struct DOUBLY_LINKED       *last;
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT   dq_exclusive;
   ALONE_IN_CACHE(dum_behind)
    char                               name[ALIGN_EO00(sizeof(tsp00_C8),sizeof(void *))];
    /*
     * first is as forward, last is as backward used
     */
};

struct          RUN_QUEUE    
{
  struct TASK_TYPE            *Task ;
  struct DOUBLY_LINKED        *Req  ;
  struct TASK_TYPE            *FromTask ;
} ;


/*
 *  DEVICE - I/O
 */
struct dev_info
{
    SAPDB_Int4            ulDetachPending; /* Flag to indicate vdetach pending to block further I/O requests */
    SAPDB_Int4            block_size ;
    SAPDB_Int4            max_block_cnt ;
    long                  asyn_io_count ;
    char                  devspace[ALIGN_EO00(sizeof(tsp00_VFilenamec),sizeof(void *))];
    char                  devspace_label[ALIGN_EO00(sizeof(tsp_vf_label),sizeof(void *))];
    SAPDB_UInt4           asyn_write ;
	SAPDB_Bool            IsDevspace ;   /* PTS 1106176 */
    tsp2_dev_type         devtype;
    SAPDB_Bool            filler[2];
    RTE_VolumeAccessMode  accessMode;
    RTE_FileSystemAccessType fileSystemAccessType; /* for DAFS support PTS 1124541 */
};

struct TASK_TYPE
{
    /*
     *  pointer to itself and to the next task in the user kernel thread
     */
    struct TASK_TYPE                * this_tcb ;
    struct TASK_TYPE                * prev_task ;  /* Since load balancing, the task chain     */
    struct TASK_TYPE                * next_task ;  /* should only be used by the UKT its self  */
                                                     
    /*
     *  pointer to the thread control block
     *  of the user kernel thread which runs this task
     */
    struct ten50_UKT_Control        * ukt ;
    struct ten50_UKT_Control        * bigComSegBelongsToUKT; /* we need this because of task load balancing */
    /*
     *  stack management
     */
    tsp00_Byte                      * StackChunkStart ;
    tsp00_Byte                      * StackChunkEnd ;
    tsp00_Byte                      * GuardPageBegin ;
    tsp00_Byte                      * GuardPageEnd ;

    /*
     *  Communication Information.
     */
    struct connection_info          * connection ;                        
    SAPDB_Int4                      * reg_excl_cnt ;
    SAPDB_Int4                      * reg_coll_cnt ;
    char                            * DevNameWaitingFor;
    /*
     *  two special queue elements, reserved for this task
     */
    struct RTETask_CommonQueueElement    * asyn_requests ;
    void                                 * aio_op;

    /*
     * Special pointer for tracing stack
     */
    void                            * pTrackBuffer;
    /*
     *  kernel communication record (cancel byte,...)
     */
    tsp00_RteCommRec                  rte_comm ;
    void                             *AcvPointer ;        /* CR 1103950 */
    void                             (*AkDumpProc)() ;    /* CR 1103950 */

   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT         exclusive;
   ALONE_IN_CACHE(dum_behind)

/* -------------------- task statistic ------------------------ */
    teo52_TaskStateStatistic                TaskStateStat;
    teo52_TaskQueueStatistic                QueStat;
    teo52_TaskPrioStatistic                 PrioStat;
    teo52_TaskOMSStatistic                  TaskOMSStat; /* 1106969 */

    /* set to EO52_RESET_TASK_STAT_FULL_PENDING or EO52_RESET_TASK_STAT_PARTIAL_PENDING by console (RESET TASKS). */
    /* reset by dispatcher */
    SAPDB_Byte                      TaskStatRecResetPending; 
    SAPDB_Bool                      QueueStatRecResetPending; 

    SAPDB_Bool                      TimeCollectionEnabled;     /* actual time enabled flag (copy of XPARAM(fTimeMeasure)) */
    SAPDB_Bool                      MoveTimeCollectionEnabled; /* actual move time enabled flag (copy of   XPARAM(uLoadBalancingCheck))*/

    teo00_Longint                   cputim ;
    teo00_Longint                   dirio ;
    SAPDB_UInt8                     disp_counter;
    SAPDB_UInt8                     Totaldisp_counter ;
    SAPDB_UInt8                     ulBegExclCalls;
    SAPDB_UInt8                     begexcl_count ; /* Count for vbegexcl without dispatcher call */
    SAPDB_UInt8                     self_susp_cnt ;
    SAPDB_UInt8                     Totalself_susp_cnt ;
    teo00_Longint                   lWaitForRoot ;
    teo00_Longint                   lWaitForDevno;
    teo00_Longint                   lWaitForTask;

    /*
     *  Resume counter and flag for vsignal.
     *  Vsignal sets 'vsignal_ok' to 1 for 'TRUE' and to -1 for 'FALSE'.
     *  Zero means that no vsignal was issued.
     */
    SAPDB_Int4                              resume_count;
#define MAX_RESUMER_PIDS 3
    SAPDB_Int4                              totalResumerCount;
    tsp00_TaskId                            resumerPid [ MAX_RESUMER_PIDS ];

    /*
     *  the index of the task control block in the kernel shared segment
     *  is used as task 'pid' for the database kernel code
     */
    SAPDB_Int4                              index ;
    SAPDB_Int4                              type ;
    volatile SAPDB_Int4                     state ;
    volatile SAPDB_Int4                     TaskAlive;  /* 0 Task did not set alive flag recently */
                                                        /* 1 Task set alive flag */
    SAPDB_Bool                              connectable;
    SAPDB_Int4                              state_reason ;
    SAPDB_Int4                              stack_size ;
    /*
     *  time for task's timeout if still in VRECEIVE
     */
    time_t                                  CmdWaitTime  ;

    SAPDB_Int4                              signal_count ;
    SAPDB_Int4                              vsignal_ok ;
    SAPDB_Int4                              lastSignalTID;
    SAPDB_UInt4                             prio_flag ;
    SAPDB_UInt4                             save_prio ;
    SAPDB_UInt4                             ulDynPrio ;
    SAPDB_UInt4                             sid ;
    SAPDB_UInt4                             excl_schachtel ; /* UUHH! */
    SAPDB_UInt4                             is_in_queue ;
    SAPDB_UInt4                             aio_in_progress ;
    SAPDB_Int4                              aio_errno ;

    SAPDB_Int4                              lVStopCounter;
            /* PTS 1108470 */
    teo07_Thread                            taskThread;
			/* PTS 1111201 */
    teo07_ThreadId                          taskThreadId;

    struct RTE_CoroutineContext *           taskContext; /* used only if coroutines enabled */

    /* PTS 1110829 */
    SAPDB_UInt4 *                   pCurrStackPointer;  /* last stack pointer value on entry in dispatcher */

    /* PTS 1110953 */
    SAPDB_UInt4 *                   maxStackPointer;    /* deepest stack position of all dispatcher calls */
    SAPDB_UInt4                     commandMaxStack;    /* maximum stack usage in bytes per command */
    SAPDB_UInt4                     totalMaxStack;      /* maximum stack usage in bytes over all commands */
    tsp00_PageNo                    lockedPageNo;       /* page number set if suspended via vSuspendForPageLock() */

    union { 
      struct {
        SAPDB_UInt2                   temporary;      /* used for temporary locking */
        SAPDB_UInt2                   longTerm;       /* used for long term locking */
      }                              type;

      SAPDB_UInt4                    val;             /* used for lock checks */
    }                               moveLock;

    SAPDB_UInt2                     lastTimeVar;      /* Value 0 .. TIME_VAL_ARRAY_SIZE */
    SAPDB_UInt8                     runnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
    SAPDB_UInt8                     maxRunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];

    SAPDB_Bool                      fTaskIsWaitingForExclusiveAccess; /* ... to a RWRegion */
    struct TASK_TYPE               *nextTaskInWaitQueue ;

    SAPDB_Bool                      delayTimeMeasurement;
    SAPDB_Int4                      yieldRequestAtPos;

    SAPDB_Bool                      fDebugTask;

    struct RTETask_Task*            pTaskObject;         /* - pointer to the new scheduler object */
    struct RTETask_LegacyTaskCtrl*  pLegacyTaskCtrlObject;
    SAPDB_Char                      NameOfSpinlockYieldingFor[20]; /* - pointer to spinlock task is currently yielding for or 0 */

    /* no filler */
}; /* TASK_TYPE */

struct UKP_ASYNC_IO_STRUCT
{
    SAPDB_Int4                              aio_in_progress ;
    SAPDB_Int4                              aio_complete ;
    SAPDB_UInt4                             TotalAIOReq ;
    SAPDB_UInt4                             TotalAIOSig ;
};

typedef struct UKP_IO_STRUCT
{
  SAPDB_Int4                                ukp_attached_fd ;
  SAPDB_Int4                                selfIOStatisticResetPending;
  teo52_IOStatistic                         ReadSelfIo;
  teo52_IOStatistic                         WriteSelfIo;
} UKT_SINGLE_IO_REC; 

typedef UKT_SINGLE_IO_REC    *PUKT_SINGLE_IO_REC;

struct DEV_PCB
{
    teo07_Thread                              thread;
    teo07_ThreadId                            tid;
    SAPDB_Int4                                semid;
    volatile SAPDB_Int4                       state;
    SAPDB_Int4                                cpu_id;
    time_t                                    start_time;
    time_t                                    stopTime;
    volatile SAPDB_Int4                       fSemAlreadyPosted;
    /*
     * The entries so far MUST be kept identical to the entries for the
     * ten50_KT_Control
     */
    struct ten50_UKT_Control                  *ukt;
    struct DOUBLY_LINKED                      *request;
/*    INTFUNC_PTR                        io_f[2]; */
    int (*io_f[2])( struct DEV_PCB *, int, char *, long, long, SAPDB_UInt4 *);
    teo52_DevIOStatistic                      DevIOStat ;
    volatile struct IOC_QUEUE                 *io_work;
    volatile struct IOC_QUEUE                 *io_request;
    struct         IOC_QUEUE                  *ios;
    /*
     * points to functions to read and write for this threads' devspace
     */
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT                 alone ;
   ALONE_IN_CACHE(dum_behind) 
    off_t                                     old_io_endpos;
    SAPDB_Int4                                fd;  /* file decriptor */
    SAPDB_Bool                                isLocked; /* true if file is locked */
    SAPDB_Int4                                dev_type;
    SAPDB_Int4                                sfd; /* same file opened with O_SYNC */
    SAPDB_Int4                                set_io_error ;
    SAPDB_Int4                                devcap;
    SAPDB_Int4                                filler;
};

typedef int (*INTFUNC_PTR)( struct DEV_PCB *, int, char *, long, long, SAPDB_UInt4 *);

struct DEV_PCB0
{
    teo07_Thread                              thread;
    teo07_ThreadId                            tid;
    SAPDB_Int4                                semid;
    volatile SAPDB_Int4                       state;
    SAPDB_Int4                                cpu_id;
    time_t                                    start_time;
    time_t                                    stopTime;
    volatile SAPDB_Int4                       fSemAlreadyPosted;
    /*
     * The entries so far MUST be kept identical to the entries for the
     * ten50_KT_Control
     */
    volatile struct IOC_QUEUE                 *io_work;
    struct dev_info                           *dev ;
    struct         IOC_QUEUE                  *ios;
    volatile struct IOC_QUEUE                 *io_request;
    /*
     * points to functions to read and write for this threads' devspace
     */
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT                 alone ;
   ALONE_IN_CACHE(dum_behind) 
    /*
     * Dependencies from Taskcluster !!
     */
    SAPDB_Int4                                nice;
    SAPDB_UInt4                               fix_pri;
    SAPDB_UInt4                               dis_pre;
    SAPDB_Int4                                filler;
};

/*
 * There is one ten50_UKT_Control for each Kernel Thread ( KT ) and for each
 * User Kernel Thread  ( UKT )
 * 
 * state values only changed by the thread itself !!
 *
 */
typedef struct ten50_UKT_Control
{
    teo07_Thread                       thread;
    teo07_ThreadId                     tid;
    SAPDB_Int4                         semid;
    volatile SAPDB_Int4                state;
    SAPDB_Int4                         cpu_id;
    time_t                             start_time;
    time_t                             stopTime;
    volatile SAPDB_Int4                fSemAlreadyPosted;
    /*
     * The entries so far MUST be kept identical to the entries for the
     * ten50_KT_Control
     */
    struct connection_info              *connection ;

/*
 * Used to get a pointer to the current task
 */
    struct TASK_TYPE                    *curr_task;
/*
 * Used to transfer current_queue element between tasks
 */
    struct DOUBLY_LINKED                *curr_quu;
/*
 * This threads first task
 */
    struct         TASK_TYPE            *pFirstTaskCtrl;
    struct         IOC_QUEUE_HEAD       ioc_queue;
    struct         IOC_QUEUE            *iocs;
    struct         IOR_QUEUE_HEAD       aioc_queue;
    struct         IOR_QUEUE            *aiocs;
    struct         RUN_QUEUE            *RunQueue;
    /*
     * each KT has its own set of DEVi
     */
    PUKT_SINGLE_IO_REC                  pSingleIO;
    SAPDB_Int4                          ulSelfIOPending; /* Flag to indicate self I/O is pending to delay close of self I/O descriptors */
    struct XXX_QUEUE_HEAD              **xxx;
    struct TASK_TYPE                    *last_com_task ;

    struct    UKP_ASYNC_IO_STRUCT       aio ;

    ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT           exclusive ;
    ALONE_IN_CACHE(dum_behind)

    SAPDB_UInt8                         UKTStartTimeMicroSeconds;
    SAPDB_UInt8                         SleepingStateEnterMicroSeconds; /* - set when UKT is entering sleep state */
    SAPDB_UInt8                         RunningStateEnterMicroSeconds; /* - set when UKT is running again */
    SAPDB_UInt8                         IdleTimeMeasureBeginMicroseconds; /* - set when UKT resets idle statistic */
#ifndef HAS_CONSOLE_PIPE
    SAPDB_UInt8                         SysIdleTimeMeasureBeginMicroSeconds; /* needed for scaling internal microseconds */
#endif
    teo52_TaskSchedulerTimeStatistic    UKTRunningStat; /* - collect absolute running time */
    teo52_TaskSchedulerTimeStatistic    UKTIOWaitStat; /* - collect absolute time spend waiting for i/o */
    SAPDB_Bool                          SleepStatRecResetPending; /* Flag set by console thread, handled and reset by dispatcher entry code */

    /*
     *  the index of the thread in the kernel shared segment
     *  is used as threads 'pid' for the database kernel code
     */
    SAPDB_Int4                          index ;
    SAPDB_Int4                          stacksize ;
    SAPDB_Int4                          io_was_completed;
    /*
     * The entries so far MUST be kept identical to the entries for the
     * ten50_KT_Control
     */
    SAPDB_Bool                          tasksAllowedToMoveTo; /* specifies that a task is allowed to move to or from this UKT. */
    SAPDB_UInt4                         numOfMovableTasks;
    struct ten50_UKT_Control            *moveTaskToUKT;      /*  pointer to the UKT to which a Task  */

    SAPDB_UInt2                         currTimeVar;         /* Value 0 .. TIME_VAL_ARRAY_SIZE */
    SAPDB_UInt8                         taskMoveIntervalStartTime;
    SAPDB_UInt8                         UKTSleepTimeMicroSeconds[TIME_VAL_ARRAY_SIZE];
    SAPDB_UInt8                         runnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
    SAPDB_UInt8                         maxRunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
    SAPDB_UInt8                         stopWaitForBestFitTaskAt; 
    teo00_Longint                       dispCounterDuringMesIntReq;
    teo00_Longint                       dispCounterDuringSendMoveReq;

    SAPDB_UInt4                         activeTasks;

    SAPDB_UInt4                         numServTasks;
    SAPDB_UInt4                         activeServTasks;

    /* --- connectable tasks */
    SAPDB_UInt4                         numUserTasks;
    SAPDB_UInt4                         activeUserTasks;
    SAPDB_UInt4                         numUtilityTasks;
    SAPDB_UInt4                         activeUtilityTasks;
    SAPDB_UInt4                         numEventTasks;
    SAPDB_UInt4                         activeEventTasks;

    SAPDB_Int4                          RunQueueLen  ;
    SAPDB_UInt4                         maxRunQueueLen;
    SAPDB_UInt4                         maxRunQueueLenSinceLastReset;
    tsp00_Uint8                         disp_counter;
    tsp00_Uint8                         Totaldisp_counter;
    tsp00_Uint8                         cmd_counter;
    tsp00_Uint8                         Totalcmd_counter;
    time_t                              ukp_timeout;  /* RND_TIMEOUT rounded value of all tasks timeout */
    SAPDB_UInt4                         RegLockSlice;
    SAPDB_UInt4                         SelfDispatch;
    SAPDB_UInt4                         NumRegWait;
    SAPDB_Int4                          io_is_completed;
    SAPDB_UInt4                         ukp_is_awaiting;
    SAPDB_UInt4                         ActTasksPrioCmdWait ;
    SAPDB_UInt4                         ActTasksPrio ;
    SAPDB_UInt4                         ActTasksVwait;
    SAPDB_UInt4                         ulTotVsuspWhenSleep ;
    SAPDB_UInt4                         ulTotVwaitWhenSleep ;
    SAPDB_UInt4                         ulTotPrioWhenSleep ;
    SAPDB_UInt4                         allow_single_io ;
    SAPDB_Int4                          wait_for_ioproc_io ;

    /*
     * Dependencies from Taskcluster !!
     */
    SAPDB_Int4                          nice;
    SAPDB_UInt4                         fix_pri;
    SAPDB_UInt4                         dis_pre;
    SAPDB_Int4                          delay_active ; /* vdelay parameter */
    SAPDB_Int4                          delay_prio ;
    SAPDB_Int4                          act_rex_elem ;
    SAPDB_Int4                          end_switch_cnt ;

    tsp00_TaskId                        DispatcherLockingTask ;

    /* PTS 1108768 Space for Tracebuffer used for InMemory tracing */
    tsp00_PageAddr                      traceBuffer;
    SAPDB_Int4                          traceBufferSizeInPages;

    SAPDB_UInt8                         taskSwitches;  /* PTS 1108715 */

    SAPDB_Bool                          started;    /* PTS */

    struct RTE_CoroutineContext *       removeTaskContext; /* used only if coroutines enabled */

    struct RTETask_TaskScheduler*       pTaskSchedulerObject;    /* - pointer to the new scheduler object */
    struct RTETask_LegacyUKTCtrl*       pLegacyUKTCtrlObject;
} ten50_UKT_Control; /* ten50_UKT_Control */

externC ten50_UKT_Control *en88GetThisUkt();

/*
 * used for the requestor, coordinator, timer
 */
typedef struct ten50_KT_Control
{
    teo07_Thread                       thread;
    teo07_ThreadId                     tid;
    SAPDB_Int4                         semid;
    volatile SAPDB_Int4                state;
    SAPDB_Int4                         cpu_id;
    time_t                             start_time;
    time_t                             stopTime;
    volatile SAPDB_Int4                fSemAlreadyPosted;
    /*
     * The entries so far MUST be kept identical to the entries for the
     * ten50_UKT_Control
     */
    struct         DOUBLE_Q_HEAD       work;
    /* no filler */
} ten50_KT_Control;

struct N_QUEUE
{
    struct         N_QUEUE          *next;
    struct         TASK_TYPE        *taskid;
    /* no filler */
};

struct SEM_LISTE
{
   ALONE_IN_CACHE(dum_before) 
    volatile ALONE_UKP_STRUCT        sem_exclusive;
   ALONE_IN_CACHE(dum_behind) 
    struct         N_QUEUE          *first;
    struct         N_QUEUE          *last;
    struct         N_QUEUE          *sem ;
};

struct DDB4_SEMA
{
    volatile struct TASK_TYPE     * owner ;
    GUARDWORD( guard_before ) /* 1995-Sep-08 J.B. */
    struct SEM_LISTE                semaliste ;
    GUARDWORD( guard_after  ) /* 1995-Sep-08 J.B. */
    SAPDB_UInt4                     get_count ;
    SAPDB_UInt4                     begexcl_tas_count ;
    SAPDB_UInt4                     endexcl_tas_count ;
    SAPDB_UInt4                     collision_count ;
    SAPDB_UInt4                     wait_count ;
    SAPDB_UInt4                     getCountAtReset;
    SAPDB_UInt4                     begExclTasCountAtReset;
    SAPDB_UInt4                     endExclTasCountAtReset;
    SAPDB_UInt4                     waitCountAtReset;
    SAPDB_UInt4                     collisionCountAtReset;
    char                            semaname[ALIGN_EO00(sizeof(tsp00_C8),sizeof(void *))];
};

/*
   +---------------------------------------------------------------+
   |   TASK MOVE STATISTIC                                         |
   +---------------------------------------------------------------+
*/

struct last_task_move_entry
{
  SAPDB_UInt8                 moveTime;
  struct TASK_TYPE*           task;
  SAPDB_UInt8                 taskMaxRunnableMicroSeconds;
  struct ten50_UKT_Control*   srcUKT;
  SAPDB_UInt8                 srcMaxRunnableMicroSeconds;
  struct ten50_UKT_Control*   dstUKT;
  SAPDB_UInt8                 dstMaxRunnableMicroSeconds;
};

struct last_task_move_record
{
  SAPDB_UInt2                     currEntry;
  struct last_task_move_entry     entries[NUM_OF_TASK_MOVE_STAT_RECS];

  ALONE_IN_CACHE(dum_before) 
   volatile ALONE_UKP_STRUCT      exclusive;
  ALONE_IN_CACHE(dum_behind)
};



struct KERNEL_GLOBAL_SECTION
{
    struct KERNEL_GLOBAL_SECTION    * this_kgs ;
    struct SHARED_SEGMENT           * seg ;
    SAPDB_Int4                      * kgsshmid ;
    struct ten50_UKT_Control        * first_kp ;
    struct ten50_UKT_Control        * last_kp ;
    struct DEV_PCB                  * first_dev ;
    struct DEV_PCB                  * first_asdev ;
    struct DEV_PCB                  * last_dev ;
    struct TASK_TYPE                * pFirstTaskCtrl ;
    struct TASK_TYPE                * pLastTaskCtrl ;
    struct TASK_TYPE                * ti ;
    struct TASK_TYPE                * al ;
    struct TASK_TYPE                * tw ;
    struct TASK_TYPE                * cs ;
    struct TASK_TYPE                * ut ;
    struct TASK_TYPE                * first_datawriter ;
    struct TASK_TYPE                * first_event_task ;
    struct TASK_TYPE                * first_backup_task ;
    struct TASK_TYPE                * first_garbage_collector ;
    struct TASK_TYPE                * first_objectlog_writer ;
    struct TASK_TYPE                * first_server ;
    struct TASK_TYPE                * first_user ;

    struct XXX_QUEUE_HEAD           * first_xxx ;
    struct XXX_QUEUE_HEAD           * last_xxx ;

    struct DDB4_SEMA                * semaphore ;
    SAPDB_Bool                      * dumpflag ;
    SAPDB_Bool                      * traceflag ;

    UCHAR                           * dynpool_bot ;
    UCHAR                           * dynpool_ptr ;
    UCHAR                           * dynpool_top ;

    tsp00_PageAddr                  datacache_bot ;
    tsp00_PageAddr                  datacache_ptr ;
    tsp00_PageAddr                  datacache_top ;

    tsp00_PageAddr                  convcache_bot ;
    tsp00_PageAddr                  convcache_ptr ;
    tsp00_PageAddr                  convcache_top ;

    tsp00_PageAddr                  dyndata_bot ;
    tsp00_PageAddr                  dyndata_ptr ;
    tsp00_PageAddr                  dyndata_top ;

    struct connection_info          * connection ;
    SAPDB_UInt4                     connectedUserTasks;

    void                            * alone_thread;
    struct rte_xparam_record        * XParam ;
    void                            * Eventing;
    /* PTS 1103824 */
    void                            * MessageBufferSpace;

    struct ten50_KT_Control         coord ;
    struct ten50_KT_Control         console ;
    struct ten50_KT_Control         timer ;
    SAPDB_ULong                     timerSleepIntervals;
    time_t                          timerCurrentWakeupTime;
    SAPDB_Long                      timerCurrentSleepInterval;
    struct ten50_KT_Control         requestor ;
    struct DEV_PCB0                 dev0 ;

    long                            seg_size ;

    unsigned long                   ulDBPageShift;

    tsp00_Versionc                  RTEVersion;
    tsp00_Versionc                  KernelVersion;
    tsp00_NodeIdc                   servernode ;
    tsp00_DbNamec                   serverdb ;

    tsp100_VersionID2               RTEVersionID;
    uid_t                           uid ;
    pid_t                           pid ;
    pid_t                           parentPid ;

/*
  --- SERVERDB start/stop options
 */
#define AUTOCLOSE                        0x0001
#define IGNORE_AUTORESTART               0x0002
#define FORCE_AUTORESTART                0x0004
#define LOW_PROCESS_PRIORITY             0x0008
#define MS_CLUSTER_SERVER_STARTUP        0x8000
    SAPDB_UInt4                     ulStartupOptions; /* flags used as startup options */

    SAPDB_UInt4                     consoleShmSize;
    SAPDB_UInt4                     consoleSemTimeout;

    /*
     *  Database state
     */
    volatile SAPDB_UInt4            state ;
    /*
     *  Database stop/kill mode
     */
    SAPDB_UInt4                     mode ;

    SAPDB_Int4                      devpsets ;
    SAPDB_Int4                      max_asyn_request;
    SAPDB_Int4                      maxtapes ;
    SAPDB_Int4                      maxdevspaces ;
    SAPDB_Int4                      max_ukpio_devs;
    SAPDB_Int4                      dev_queue ;

    SAPDB_Int4                      ulNumOfTasks;
    SAPDB_Int4                      NumOfUserUKTs;

    SAPDB_Int4                      codesize ;

    SAPDB_UInt4                     restarted ;
    SAPDB_Int4                      restart_timeout ;

    time_t                          start_time ;
    volatile time_t                 current_time; /* set by watchdog every second */
    teo07_ThreadId                  clockTid;

    /*
     * MWa Spinlock
     */
#ifdef SPINLOCK_NO_MORE
    spinlock_t                      m_locks ;
#endif
    SAPDB_Bool                      alone_flag ;
    long                            alone_pcnt ;
    SAPDB_Int4                      alone_count ;
    SAPDB_Int4                      alone_collision ;

    SAPDB_Int4                      ulNumOfUKTs ;
    SAPDB_Int4                      dev_blocksize ;

    /*
     *  Communication information for some threads and tasks.
     */
    SAPDB_Int4                        connection_cnt ;

    /*
     * debuglevel can be modified by an external tool
     */
    SAPDB_Int4                        ConsoleModifiedKgs ;
    SAPDB_Int4                        debuglevel ;
    /*
     * coretaskindex can be set by an external tool to request a core
     * dump of a specific task. The kernel thread should then fork,
     * load the task's stack, and core dump.
     */
    SAPDB_Int4                        wantDump ;
    SAPDB_Int4                        gracefully ;
    SAPDB_Int4                        coretaskindex ;
    SAPDB_Int4                        corethreadindex ;
    SAPDB_UInt4                       TotalDataIOCnt ;
    SAPDB_UInt4                       SuspendReason[MX_SUSPEND_REASONS_EO00] ;

    volatile SAPDB_Int4               traceWriterGone ;
    SAPDB_UInt4                       ulSingleDebugBreakPos;

  ALONE_IN_CACHE(dum2_before) 
    volatile ALONE_UKP_STRUCT         ExclusiveVallocat;
  ALONE_IN_CACHE(dum2_behind) 

  ALONE_IN_CACHE(dum3_before) 
    volatile ALONE_UKP_STRUCT         ExclusiveVnewbuf;
  ALONE_IN_CACHE(dum3_behind) 

  /* PTS 1108471 Space for Tracebuffer used for InMemory tracing */
  tsp00_PageAddr                      traceBufferSpace;
  SAPDB_Int4                          totalTraceBufferSizeInPages;
  SAPDB_Int4                          traceNeedsSaving;
  SAPDB_Bool                          dumpDiagFiles;   /* PTS 1115180 */
  SAPDB_Bool                          pipeCheckWriteDone; /* PTS 1117651 */
  SAPDB_Bool                          isHotStandbyConfiguration;  
  key_t                               ipcBaseKey;   /* Basis key for all ipc resources */

  SAPDB_Bool                          taskMoved;
  SAPDB_UInt2                         currTimeVar;   /* Value 0 .. TIME_VAL_ARRAY_SIZE */
  SAPDB_UInt8                         taskMoveIntervalStartTimes[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt2                         taskMoveMeasIntervals;
  struct last_task_move_record        lastTaskMoves;
  SAPDB_Bool                          fBusyWaitingForbidden;

  /* PTS 1129710 */
  SAPDB_UInt4                         kernelStateSize;        /* - Size of struct Kernel_SharedState */
  SAPDB_ULong                         pendingUpdateCounter;   /* - Changed for every pending update on kernelState content */
  SAPDB_ULong                         completedUpdateCounter; /* - Changed for every completed update on kernelState content */
  struct Kernel_SharedState           kernelState;            /* - Extended kernel state */
  SAPDB_Int4                          taskResponsibleForOffline; /* id of task responsible for offline state */
  SAPDB_Int4                          gotCrashSignal;         /* Indicate how often crash signal handler was called */
  SAPDB_UInt8                         symbolResolutionLockCount; /* counts the locks of symbol resolution code */
};
/* KERNEL_GLOBAL_SECTION */


struct SHARED_SEGMENT
{
    struct SHARED_SEGMENT             * this_seg ;
    struct KERNEL_GLOBAL_SECTION      * kgs ;
    long                              seg_size ;
};
/* SHARED_SEGMENT */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#ifdef MAIN
#define PUBLIC
#define SEP ,
#define INITLIST( _v ) = _v
#else
#define PUBLIC extern
#define INITLIST( _v )
#endif

PUBLIC struct KERNEL_GLOBAL_SECTION * kgs INITLIST(0);

#ifdef strings
PUBLIC struct sembuf semsnd INITLIST( { 0 SEP  1 SEP 0        } );
PUBLIC struct sembuf semrcv INITLIST( { 0 SEP -1 SEP 0        } );
PUBLIC struct sembuf semrea INITLIST( { 0 SEP  0 SEP 0        } );
PUBLIC struct sembuf semadd INITLIST( { 0 SEP  1 SEP SEM_UNDO } );
#endif

#undef PUBLIC
#undef INITVAL
#ifdef MAIN
#undef SEP
#endif
    
externC void en88TaskContextSwitch( struct TASK_TYPE *pOld, struct TASK_TYPE *pNew );

#endif  /*USE_KGS*/

#endif  /* GEN50_H */
