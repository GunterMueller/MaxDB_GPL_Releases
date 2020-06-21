/*!
  @file           gos00k.h
  @author         RaymondR
  @special area   RTE kernel include module
  @brief          
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



#ifndef GOS00K_H
#define GOS00K_H


#include "gos00.h"
#include "vsp002c.h"
#include "heo61.h"
#include "geo52.h"
#include "geo00_2.h"
#include "geo00_3.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/Tasking/RTETask_LegacyQueueElement.h"
#include "RunTime/Tasking/RTETask_LegacyQueueElementFreeList.h"

//
// +===============================================================+
// |   NETWORK INCLUDES   ('fd_set', 'struct sockaddr' ... )       |
// +===============================================================+
//
#include         <winsock.h>

//
// +===============================================================+
// |   PRAGMAS                                                     |
// +===============================================================+
//

//
// +===============================================================+
// |   DEFINES                                                     |
// +===============================================================+
//
#define ALLOW_TASK_DEBUG_REQUESTS

#if !defined (__cplusplus) && !defined (true)
  #define true                          1
  #define false                         0
#endif

#define DEF_KERNEL_TASKS                8

#ifdef PE
 #define MAX_ACTIVE                     5          // -- max user
 #define MAXLOCATIONS                   1          // - max locations in network
#else
 #define MAXLOCATIONS                   2048        // - max locations in network
#endif

#define MAX_PROG_ARGS                   100

#define MAX_FREE_QUE_ELEM_PER_TASK      2
#define CRASH_SLEEP_TIME                5           // - seconds

#define MAX_ASYN_REQUESTS               2
#define MAX_DEVI_REQUESTS               2           // - v2blockio needs 2 requests
#define MINDIAGFILESIZE                 1           // - num. of 'PAGES'
#define MAX_DEVI_P_DEVSPACE             4
#define NUM_SPECIAL_THREADS             40          // - DEV0, ASYNC0, TIMER, CLOCK, REQUESTOR...

#define PRODUCT_ID_POS                  8
#define PRODUCT_ID_STRING               "SAPDB "
#define DEVSPACE_ID_POS                 (PRODUCT_ID_POS + sizeof (PRODUCT_ID_STRING))
#define DEVSPACE_ID_STRING              "DEVSPACE"
#define DEVSPACE_MAXSIZE_POS            256
#define DEVSPACE_SIZE_POS               (DEVSPACE_MAXSIZE_POS   + sizeof (ULONG))
#define DEVSPACE_BAD_MARK_POS           (DEVSPACE_SIZE_POS      + sizeof (ULONG))
#define DEVSPACE_REINT_INFO_POS         (DEVSPACE_BAD_MARK_POS  + sizeof (ULONG))  

#define STRAT_EQUALIZE                  0           // -- Requester - connect strategy
#define STRAT_COMPRESS                  1           // -- Requester - connect strategy
#define MAXTIMEVALUE                    ((ULONG)-1)
#define STARTUPSLEEPTIME                1           // -- Seconds (used by Timer Thread)
#define MAX_SUSPEND_REASONS             256
#define MAX_RESUMER_PIDS                3           // Number of Pids stored in PID History
#define MAX_REG_COLL_LOOPS              100000

// UKT Processor Relationship
#define UKT_CPU_RELATIONSHIP_NONE              0
#define UKT_CPU_RELATIONSHIP_AFFINITY          1
#define UKT_CPU_RELATIONSHIP_IDEAL_PROCESSOR   2

// --- Kernel threads stack sizes
#if defined (_WIN64)
# define UKT_BASE_STACK_SIZE             (  24 * 1024)
# define UKT_TEMP_STACK_SIZE             (  48 * 1024)
# define KT_TIMER_STACK_SIZE             (  36 * 1024)
# define KT_CLOCK_STACK_SIZE             (  12 * 1024)
# define KT_DEV0_STACK_SIZE              (  48 * 1024)
# define KT_DEVI_STACK_SIZE              (  48 * 1024)
# define KT_CONSOLE_STACK_SIZE           (  48 * 1024)
# define KT_ASYNC0_STACK_SIZE            (  48 * 1024)
# define KT_ASYNCi_STACK_SIZE            (  48 * 1024)
# define KT_REQUESTOR_STACK_SIZE         (  48 * 1024)
# define KT_COORDINATOR_STACK_SIZE       (  64 * 1024)
# define KT_DEV_WORKER_STACK_SIZE        (  24 * 1024)
#else
# define UKT_BASE_STACK_SIZE             (   8 * 1024)
# define UKT_TEMP_STACK_SIZE             (  16 * 1024)
# define KT_TIMER_STACK_SIZE             (  12 * 1024)
# define KT_CLOCK_STACK_SIZE             (   4 * 1024)
# define KT_DEV0_STACK_SIZE              (  16 * 1024)
# define KT_DEVI_STACK_SIZE              (  16 * 1024)
# define KT_CONSOLE_STACK_SIZE           (  16 * 1024)
# define KT_ASYNC0_STACK_SIZE            (  16 * 1024)
# define KT_ASYNCi_STACK_SIZE            (  16 * 1024)
# define KT_REQUESTOR_STACK_SIZE         (  16 * 1024)
# define KT_COORDINATOR_STACK_SIZE       (  32 * 1024)
# define KT_DEV_WORKER_STACK_SIZE        (   8 * 1024)
#endif


/*>>>>>>>>>>>  N O   L O N G E R  U S E D .  >>>><<<<<<<<>>>>>>>>>>*/
// --- Values for database state
#define SERVER_UNKOWN                   SERVER_UNKOWN_EO00
#define SERVER_OFFLINE                  SERVER_OFFLINE_EO00
#define SERVER_STARTING                 SERVER_STARTING_EO00
#define SERVER_COLD                     SERVER_ADMIN_EO00
#define SERVER_WARM                     SERVER_ONLINE_EO00
#define SERVER_SHUTDOWN                 SERVER_SHUTDOWN_EO00
#define SERVER_SHUTDOWNREINIT           SERVER_SHUTDOWNREINIT_EO00
#define SERVER_SHUTDOWNKILL             SERVER_SHUTDOWNKILL_EO00
#define SERVER_STOP                     SERVER_STOP_EO00
#define SERVER_KILL                     SERVER_KILL_EO00
#define SERVER_ABORT                    SERVER_ABORT_EO00
#define SERVER_STOPPED                  SERVER_STOPPED_EO00
#define SERVER_UNDEFINED                SERVER_UNDEFINED_EO00
/*>>>>>>>>>>>  N O   L O N G E R  U S E D .  >>>><<<<<<<<>>>>>>>>>>*/

#define D_SERVER_STARTING               "START  "
#define D_SERVER_COLD                   "ADMIN  "
#define D_SERVER_WARM                   "ONLINE "
#define D_SERVER_SHUTDOWN               "SHTDWN "
#define D_SERVER_SHUTDOWNREINIT         "SDRINIT"
#define D_SERVER_SHUTDOWNKILL           "SDKILL "
#define D_SERVER_STOP                   "STOP   "
#define D_SERVER_KILL                   "KILL   "
#define D_SERVER_ABORT                  "ABORT  "

#define KERNEL_PROFILE_AUTOSCROLL       "AutoScroll"
#define KERNEL_PROFILE_FONT             "Font"
#define KERNEL_PROFILE_SIZE             "Size"
#define KERNEL_PROFILE_EXITPROCESS      "ExitProccess"
#define KERNEL_PROFILE_MAX_DIAG_LINES   "MaxDiagLines"

// --- Values for kernel thread states
#define KT_INITIALIZED                  1
#define KT_RUNNING                      2
#define KT_TERMINATING                  3
#define KT_TERMINATED                   4
#define KT_SLEEPING                     99

#define KT_DETACHED                     98  // --- devspace thread states
#define KT_ATTACHED                     97

// --- Values for user kernel thread states
#define UKT_INITIALIZED                 KT_INITIALIZED
#define UKT_RUNNING                     KT_RUNNING
#define UKT_TERMINATED                  KT_TERMINATED
#define UKT_SLEEPING                    KT_SLEEPING


// - used by 'sql74k_alter_task_cnt'
#define ALL_COUNTER                0x00FF
#define ACTIVE_COUNTER             0x0001
#define TOTAL_COUNTER              0x0002

#define COUNTER_ADD                0x0100
#define COUNTER_SUB                0x0200

#define ADD_ALL_COUNTER            ALL_COUNTER | COUNTER_ADD
#define SUB_ALL_COUNTER            ALL_COUNTER | COUNTER_SUB
#define ADD_ACTIVE_COUNTER         ACTIVE_COUNTER | COUNTER_ADD
#define SUB_ACTIVE_COUNTER         ACTIVE_COUNTER | COUNTER_SUB
#define ADD_TOTAL_COUNTER          TOTAL_COUNTER | COUNTER_ADD
#define SUB_TOTAL_COUNTER          TOTAL_COUNTER | COUNTER_SUB

// --- ASYNCNTL request types
#define ASYNCNTL_FILETYPE               0
#define ASYNCNTL_REWIND                 1

// --- task priority
#define TASK_IS_IN_NON_QUEUE            0
#define TASK_IS_IN_UToU_QUEUE           1
#define TASK_IS_IN_UKT_QUEUE            3
#define TASK_IS_IN_COM_QUEUE            4

#define XPARAM_PRIO_SET                 (XPARAM(lPrioTask))
#define BASE_PRIO_U2U (XPARAM(lPrioBaseU2U))
#define BASE_PRIO_RAV (XPARAM(lPrioBaseRAV))
#define BASE_PRIO_UKT (XPARAM(lPrioBaseIOC))
#define BASE_PRIO_COM (XPARAM(lPrioBaseCOM))
#define BASE_PRIO_REX (XPARAM(lPrioBaseREX))
#define PRIO_FACTOR   (XPARAM(lPrioFactor))
#define PRIO_LEVEL_2_ALLOWED            (TRUE)


// ---- long term move task locks
#define NEVER_MOVE_TASK                  0x0001
#define CLIENT_NOT_TASK_MOVE_ENABLED     0x0002
#define WAITING_FOR_NEW_COMMAND          0x0004
#define TASK_IS_INACTIVE                 0x0008

//
// +===============================================================+
// |   Makros                                                      |
// +===============================================================+
//
#define  THIS_UKT_CTRL                                                       \
          ((PUKT_CTRL_REC) TlsGetValue (kgs.TlsUKTCtrlIdx))

#define  UPDATE_SERVICE_OR_CONSOLE_STATE(_dbState)                           \
        if ( kgs.fRunningAsNTService ) sql80k_NewSrvState(_dbState);          \
        else                           sql80kn_set_db_state(_dbState);

#define RELEASE_FREELIST_ELEM(_pReq)                                          \
      PutElementToFreeList( _pReq )
#define RESERVE_FREELIST_ELEM()                                               \
      GetElementFromFreeList()

#define GOTO_FIRST_TASK(_pCTask)                                             \
      sql71k_first_task_stack ( &_pCTask->Context )

#define GOTO_TASK()                                                     \
      return


#define SHUTDOWN_RESUME_TRACEWRITER(_pTW)                                        \
    {                                                                   \
    PDLQ_REC  _pRequest = RESERVE_FREELIST_ELEM();                      \
    _pRequest->ulReqType = REQ_SHUTDOWN_RESUME_TW;                      \
    sql74k_UKT_enqu_and_wake( _pTW->pUKT, _pTW, _pRequest );                                 \
    }

#define RESUME_TRACEWRITER(_pTW)                                        \
    {                                                                   \
    PDLQ_REC  _pRequest = RESERVE_FREELIST_ELEM();                      \
    _pRequest->ulReqType = REQ_SHUTDOWN_RESUME_TW;                      \
    sql74k_UKT_enqu_and_wake( _pTW->pUKT, _pTW, _pRequest );            \
    }

//
// +===============================================================+
// |   TYPEDEFS AND STRUCTS                                        |
// +===============================================================+
//
typedef VOID (* PFNENTRYFUNC )( VOID );         // --- kernel entry function type

// --- program argument structure
typedef struct
  {
      PSZ                              ppszVal[MAX_PROG_ARGS];
      ULONG                            ulCount;
  } PROG_ARGS;

typedef PROG_ARGS    *PPROG_ARGS;

// --- IO queue controls
//     The io-queue is a fixed list pointing in ring

typedef struct io_queue_record
  {
  struct io_queue_record              *pNextElem;
  struct io_queue_record              *pPreviousElem;
  PDLQ_REC                            pRequest;
  } IO_QUEUE_REC, tos00k_IOQueue;

typedef IO_QUEUE_REC                  *PIO_QUEUE_REC;
typedef PIO_QUEUE_REC                 *PPIO_QUEUE_REC;


// --- UKT queue controls
//     The ukt-queue is a fixed list pointing in ring

typedef struct ukt_queue_head_record
  {
  struct ukt_queue_record             *pFirstFreeElem;
  struct ukt_queue_record             *pLastUsedElem;

  LONG                                lExclusive; // - used to grant exclusive exccess
  } UKT_QUEUE_HEAD_REC, tos00k_UKTQueueHead;

typedef UKT_QUEUE_HEAD_REC            *PUKT_QUEUE_HEAD_REC;


typedef struct ukt_queue_record
  {
  struct ukt_queue_record             *pNextElem;
  struct ukt_queue_record             *pPreviousElem;
  PDLQ_REC                            pRequest;
  } UKT_QUEUE_REC, tos00k_UKTQueue;

typedef UKT_QUEUE_REC                 *PUKT_QUEUE_REC;


// --- ukt to ukt queue controls

typedef VOID (* PFNREMOTEFUNC )( struct task_ctrl_record **ppTaskCtrl,
                                 struct task_ctrl_record *pFromTaskCtrl,
                                 PPDLQ_REC               ppCurrReq,
                                 void*                   pParam);
typedef PFNREMOTEFUNC tos00k_RemoteFuncPtr;

typedef struct ukt_to_ukt_queue_record
  {
  struct  ukt_to_ukt_queue_record     *pNextElem;
  struct  task_ctrl_record            *pTaskCtrl;
  void *                              pFuncParam;
  tos00k_RemoteFuncPtr                pFuncToExecute;
  struct  task_ctrl_record            *pFromTaskCtrl;
  } UKT_TO_UKT_QUEUE_REC, tos00k_UKTToUKTQueue;

typedef UKT_TO_UKT_QUEUE_REC          *PUKT_TO_UKT_QUEUE_REC;


typedef struct ukt_to_ukt_queue_head_record
  {
  PUKT_TO_UKT_QUEUE_REC               pWork;     // - is used by server
  PUKT_TO_UKT_QUEUE_REC               pUToUs;
  PUKT_TO_UKT_QUEUE_REC               pRequest;  // - is used to enter requests
  } UKT_TO_UKT_QUEUE_HEAD_REC, tos00k_UKTToUKTQueueHead;                   //   pWork should always follow the pRequest

typedef UKT_TO_UKT_QUEUE_HEAD_REC     *PUKT_TO_UKT_QUEUE_HEAD_REC;


typedef struct          ukt_run_queue
{
  struct task_ctrl_record       *pTaskCtrl ;
  PDLQ_REC                       pRequest;
  struct task_ctrl_record       *pFromTaskCtrl;
} UKT_RUN_QUEUE_REC , tos00k_UKTRunQueue;

typedef UKT_RUN_QUEUE_REC             *PUKT_RUN_QUEUE_REC ;

// --- semaphore queue controls

typedef struct sem_queue_record
  {
  struct  sem_queue_record            *pNextElem;
  struct  task_ctrl_record            *pTaskCtrl;
  } SEM_QUEUE_REC, tos00k_SemQueue;

typedef SEM_QUEUE_REC                 *PSEM_QUEUE_REC;


typedef struct sem_queue_head_record
  {
  LONG                                 lExclusive; // - used to grant exclusive exccess
  PSEM_QUEUE_REC                       pInsertPos;
  PSEM_QUEUE_REC                       pRemovePos;
  PSEM_QUEUE_REC                       pSEMs;
  } SEM_QUEUE_HEAD_REC, tos00k_SemQueueHead;

typedef SEM_QUEUE_HEAD_REC            *PSEM_QUEUE_HEAD_REC;

//
// +---------------------------------------------------------------+
// |   SEMAPHORE CONTROL                                           |
// +---------------------------------------------------------------+
//

typedef struct sem_ctrl_record
  {
  C8C                                szSemaName;
  ULONG                              lGetCount;
  ULONG                              lBegExclTasCount;
  ULONG                              lEndExclTasCount;
  ULONG                              lCollisionCount;
  ULONG                              lWaitCount;
  SAPDB_UInt4                        getCountAtReset;
  SAPDB_UInt4                        begExclTasCountAtReset;
  SAPDB_UInt4                        endExclTasCountAtReset;
  SAPDB_UInt4                        waitCountAtReset;
  SAPDB_UInt4                        collisionCountAtReset;
  struct  task_ctrl_record           *pOwnerTaskCtrl;
  SEM_QUEUE_HEAD_REC                 SemaList;
  } SEM_CTRL_REC, tos00k_SemCtrl;

typedef SEM_CTRL_REC                *PSEM_CTRL_REC;

//
// +---------------------------------------------------------------+
// |   TASK STACK SWITCHING                                        |
// +---------------------------------------------------------------+
//
#if defined(_WIN64)
 typedef struct context_record
   {
   PUCHAR                          pStackPointer;
   PUCHAR                          pStackBase;
   PUCHAR                          pStackLimit;
   VOID                            (* pFunc)(VOID);
   UINT64                          UKT_gp;
   UINT64                          UKT_b0; // return address from task
   UINT64                          UKT_b1;
   UINT64                          UKT_b2;
   UINT64                          UKT_b3;
   UINT64                          UKT_b4;
   UINT64                          UKT_b5;
   UINT64                          UKT_r4;
   UINT64                          UKT_r5;
   UINT64                          UKT_r6;
   UINT64                          UKT_r7;
   UINT64                          UKT_pr;
   UINT64                          UKT_pfs;
   UINT64                          UKT_lc;
   UINT64                          UKT_unat;
   } CONTEXT_REC, tos00k_Context;
#else
 typedef struct context_record
   {
   PUCHAR                          pStackPointer;
   PUCHAR                          pStackBase;
   PUCHAR                          pStackLimit;

   VOID                            (* pFunc) (VOID);

   DWORD                           UKT_ebp;
   DWORD                           UKT_esi;
   DWORD                           UKT_edi;
   DWORD                           UKT_edx;
   DWORD                           UKT_ecx;
   DWORD                           UKT_ebx;
   DWORD                           UKT_flags;
   DWORD                           UKT_return_addr;
   DWORD                           UKT_exception_unwind;
   } CONTEXT_REC, tos00k_Context;
#endif


//
// +---------------------------------------------------------------+
// |   TASK CONTROL                                                |
// +---------------------------------------------------------------+
//

// Since overlapped I/O on NT does not allow to pass a user defined pointer,
// this structure is used. The received 'completedOverlapped' structure is
// known to be part of this structure, so a simple cast allows to access
// the other elements
typedef struct task_overlapped_io_
{
    OVERLAPPED               Overlapped;
    struct task_ctrl_record  *pTaskCtrl;
    DLQ_REC                  *pRequest;
} TaskOverlappedIO;

typedef struct task_ctrl_record
  {
  ULONG                           ulTaskIndex;
  UINT1                           TaskType;           // - task type (TT_US,...)
  volatile UINT1                  TaskAlive;          // 0 = Task did not set alive for some time

  struct task_ctrl_record         *pPrevInTaskList;   // - Previous in task list (do not use this pointer outside the ukt)
  struct task_ctrl_record         *pNextInTaskList;   // - Next in task list (do not use this pointer outside the ukt)
  struct ukt_ctrl_record          *pUKT;              // - pointer to the UKT to which this Task belongs

  double                          ContextAlignment;   // - don't insert any element between 'ContextAlignment' and 'Context'!
  CONTEXT_REC                     Context;            // - used for task stack switching if fibers are disabled

  LPVOID                          pFiberAddress;      // - used if fibers are enabled                           

  PCHAR                           pCurrStackPointer;  // - used for console storage information
  tsp00_Longuint                  ulTaskStackSize;

  volatile SAPDB_Int4             TaskState;          // - task state (TSK_RUNNING,...)

  ULONG                           lastSignalTID;

  LONG                            lVSignalOk;         // -   1  TRUE
                  //    -1  FALSE
                  //     0  no vsignal was issued

  LONG                            lResumeCount;

  REGION_ID                       Sid;
  SAPDB_UInt8                     ulBegExclCnt;       // - Count for 'vbegexcl' without dispatcher call
  SAPDB_ULong                     lNestingExcl;

  //
  // --- IO
  //
  TaskOverlappedIO                OvIO[2];       // For two simultanous overlapped I/O per task

  //
  // --- communication
  //
  RTE_COMM_REC                    KernelCtrlRecord;   // - kernel control
  void                           *AcvPointer ;        // CR 1103950
  void                           (*AkDumpProc)() ;    // CR 1103950

  SAPDB_ULong                     ulCommFlagNo;

  SAPDB_Bool                      Connectable;        // - if task is connectable
  struct user_task_comm_ctrl      *pUserCommCtrl;

  LONG                            lExclusive;         // - used to grant exclusive exccess

  //
  // --- statistic
  //
  SAPDB_Int4                      ulStateReason;

  UINT1                           fEnqPrioInUToU;
  UINT1                           PrioFlag;
  UINT1                           SavePrioFlag;
  UINT1                           QueLocation;
  SAPDB_ULong                     ulDynPrio;
  SAPDB_Long                      lWaitForTask;
  SAPDB_Long                      lWaitForRoot;
  SAPDB_Long                      lWaitForDevno;
  char                           *DevNameWaitingFor;

  SAPDB_UInt8                     ulBegExclCalls;
  SAPDB_UInt8                     ulSelfSuspCount;
  SAPDB_UInt8                     ulDispatcherCalls;
  teo52_TaskStateStatistic        TaskStateStat;
  teo52_TaskQueueStatistic        QueStat;
  teo52_TaskPrioStatistic         PrioStat;
  teo52_TaskOMSStatistic          TaskOMSStat; /* 1106969 */

  /* set to EO52_RESET_TASK_STAT_FULL_PENDING or EO52_RESET_TASK_STAT_PARTIAL_PENDING by console (RESET TASKS). */
  /* reset by dispatcher */
  SAPDB_Byte                      TaskStatRecResetPending; 
  SAPDB_Bool                      QueueStatRecResetPending; 

  SAPDB_ULong                     ulWaitTime;

  SAPDB_Bool                      TimeCollectionEnabled; /* actual time enabled flag (copy of KGS->fTimeMeasurte)*/

                                                        // 1 = Task used vpid_alive recently
  union { 
    struct {
      SAPDB_UInt2                     Temporary;      // used for temporary locking
      SAPDB_UInt2                     LongTerm;       // used for long term locking
      }                             Type;

    SAPDB_UInt4                     Val;              // used for lock checks
    }                             MoveLock;

  SAPDB_UInt2                     LastTimeVar;        // Value 0 .. TIME_VAL_ARRAY_SIZE
  SAPDB_UInt8                     RunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt8                     MaxRunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];

   /*
    * Special per Task TrackBuffer
    */
   void                          *pTrackBuffer;

   tsp00_Int4                     totalResumerCount;
   tsp00_TaskId                   resumerPid[MAX_RESUMER_PIDS];

   LONG                           lVStopCounter;  // -1  In dispatcher
                                                  //  0  Running
                                                  //  >0 vcontinue already called
   // PTS 1110953
   SAPDB_UInt4 *                  maxStackPointer;    // deepest stack position of all dispatcher calls
   SAPDB_UInt4                    commandMaxStack;    // maximum stack usage in bytes per command
   SAPDB_UInt4                    totalMaxStack;      // maximum stack usage in bytes over all commands
   tsp00_PageNo                   lockedPageNo;       // page number set if suspended via vSuspendForPageLock()

   SAPDB_Bool                     delayTimeMeasurement;
   SAPDB_UInt4                    yieldRequestAtPos;

   BOOL                           fDebugTask;
   struct task_ctrl_record       *nextTaskInWaitQueue;

   struct RTETask_Task*           pTaskObject;         // - pointer to the new scheduler object
   struct RTETask_LegacyTaskCtrl* pLegacyTaskCtrlObject;
   SAPDB_Char                     NameOfSpinlockYieldingFor[20]; /* - pointer to spinlock task is currently yielding for or 0 */
  } TASK_CTRL_REC, tos00k_TaskCtrl;


typedef TASK_CTRL_REC             *PTASK_CTRL_REC, tos00k_TaskCtrl;
typedef PTASK_CTRL_REC            *PPTASK_CTRL_REC;

typedef struct task_list_head_record
  {
  PTASK_CTRL_REC                      pFirstInTaskList;
  LONG                                lExclusive;  // - used to grant exclusive exccess
  } TASK_LIST_HEAD_REC;

typedef TASK_LIST_HEAD_REC        *PTASK_LIST_HEAD_REC;

//
// +---------------------------------------------------------------+
// |   THREAD CONTROL HEADER                                       |
// +---------------------------------------------------------------+
//

// --- Header of the the thread control records like
//     UKT_CTRL_REC, KT_THREAD_ENTRY_REC ...
typedef struct thread_ctrl_header_record
  {
  TID                             Tid;
  HANDLE                          hThrd;
  LONG                            lThrdPrio;
  tsp00_Int4                      ThrdState;
  tsp00_Longuint                  ulStartTime;
  HANDLE                          hComm;

  HANDLE                          hevSem;             
                                                      
  tsp00_Longuint                  Processor;
  SAPDB_UInt4                     CPURelationship;
  } THREAD_CTRL_HEADER_REC, tos00k_ThreadCtrlHeader;

typedef THREAD_CTRL_HEADER_REC    *PTHREAD_CTRL_HEADER_REC;
typedef THREAD_CTRL_HEADER_REC    tos00k_ThreadCtrlHeader;  

typedef struct ukt_single_io_record
  {
  HANDLE                          SingleIOHandle;
  SAPDB_Bool                      isLocked; /* Flag to indicate lock is set */
  ULONG                           ulOldSingleIOPos;
  teo52_IOStatistic               ReadSelfIo;
  teo52_IOStatistic               WriteSelfIo;
  tsp00_Int4                      selfIOStatisticResetPending;
  } UKT_SINGLE_IO_REC; 

typedef UKT_SINGLE_IO_REC    *PUKT_SINGLE_IO_REC;


//
// +---------------------------------------------------------------+
// |   USER KERNEL THREAD CONTROL                                  |
// +---------------------------------------------------------------+
//


typedef struct ukt_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!
  ULONG                           ulUKTIndex;
  tsp00_Longuint                  ulUKTStackSize;
  ULONG                           ulUKTTimeOut;
 
  double                          ContextAlignment;   // - don't insert any element between 'ContextAlignment' and 'Context'!
  CONTEXT_REC                     Context;            // - used to free task stack if fibers are disabled
  bool                            fRunningOnTempStack;// - set if we are running on the UKTs stack to relase
                                                      //   a task

  LPVOID                          pRelaseFiber;       // - only used if fibers are enabled

  ULONG                           ulActiveTasks;

  ULONG                           ulNumServTasks;
  ULONG                           ulActiveServTasks;

  struct ukt_ctrl_record          *pMoveTaskToUKT; // - pointer to the UKT to which the Task has to be moved
  SAPDB_UInt8                     StopWaitForBestFitTaskAt;
  SAPDB_UInt8                     DispatcherCallsDuringMesIntReq;
  SAPDB_UInt8                     DispatcherCallsDuringSendMoveReq;

                                  // --- connectable tasks
  ULONG                           ulNumUserTasks;
  ULONG                           ulActiveUserTasks;
  ULONG                           ulNumUtilityTasks;
  ULONG                           ulActiveUtilityTasks;
  ULONG                           ulNumEventTasks;
  ULONG                           ulActiveEventTasks;

  TASK_LIST_HEAD_REC              TaskList;

  PTASK_CTRL_REC                  pCTask;             // - pointer to current task control record
  PDLQ_REC                        pCurrReq;           // - pointer to current request queue element

  ULONG                           ulImmediateDispatch;
  ULONG                           ulNumRegWait;

  ULONG                           ulSemaNo;

  PUKT_RUN_QUEUE_REC              RUNQue;
  ULONG                           ulRUNQueueLen;
  SAPDB_UInt4                     maxRunQueueLen;
  SAPDB_UInt4                     maxRunQueueLenSinceLastReset;
  UKT_QUEUE_HEAD_REC              UKTQue;

  PUKT_TO_UKT_QUEUE_HEAD_REC      pUToUQueHeads;

  // --- each UKT has its own set of DEVi for single io
  PUKT_SINGLE_IO_REC              pSingleIO;
  tsp00_Int4                      ulSelfIOPending; /* Flag to indicate self I/O is pending to delay close of self I/O descriptors */
  LONG                            lWaitForDeviIO;
  LONG                            lWaitForAsynciIO;

  SAPDB_UInt8                     DispatcherCalls;

  LONG                            lExclusive;         // - used to grant exclusive exccess
  ULONG                           ulActInRexQueue;    // - used for LW delay
  teo52_TaskSchedulerTimeStatistic UKTRunningStat;     // - used for console
  teo52_TaskSchedulerTimeStatistic UKTIOWaitStat;      // - used for console
  SAPDB_Bool                      SleepStatRecResetPending; // Flag set by console thread, handled and reset by dispatcher entry code
  tsp00_TaskId                    DispatcherLockingTask;

  tsp00_PageAddr                  traceBuffer;        // PTS 1108768 Space for Tracebuffer used for InMemory tracing 
  tsp00_Int4                      traceBufferSizeInPages;

  SAPDB_UInt8                     ulUKTStartTimeMicroSeconds;

  SAPDB_UInt8                     SleepingStateEnterMicroSeconds;   // - set when UKT is entering sleep state
  SAPDB_UInt8                     RunningStateEnterMicroSeconds;    // - set when UKT is running again
  SAPDB_UInt8                     IdleTimeMeasureBeginMicroseconds; // - set when idle time measurement starts
  SAPDB_UInt8                     UKTRunTimeMicroSeconds;
  SAPDB_UInt8                     systemTimeAtLastReset; //CPU time in micro seconds used for system since last reset
  SAPDB_UInt8                     userTimeAtLastReset;   //CPU time in micro seconds used for user since last reset

  SAPDB_Bool                      TasksAllowedToMoveTo;// specifies that a task is allowed to move to or from this UKT.
  SAPDB_UInt4                     NumOfMovableTasks;

  SAPDB_UInt2                     CurrTimeVar;         // Value 0 .. TIME_VAL_ARRAY_SIZE
  SAPDB_UInt8                     TaskMoveIntervalStartTime;
  SAPDB_UInt8                     UKTSleepTimeMicroSeconds[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt8                     RunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt8                     MaxRunnableMicroSeconds[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt8                     taskSwitches;            // PTS 1108715

  struct RTETask_TaskScheduler*   pTaskSchedulerObject;    // - pointer to the new scheduler object
  struct RTETask_LegacyUKTCtrl*   pLegacyUKTCtrlObject;
  } UKT_CTRL_REC, tos00k_UKTCtrl;


typedef UKT_CTRL_REC              *PUKT_CTRL_REC, tos00k_UKTCtrl;

//
// +---------------------------------------------------------------+
// |   KERNEL THREAD CONTROL                                       |
// +---------------------------------------------------------------+
//

typedef struct kt_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!
  DLQ_HEAD_REC                    DLQ_Work;
  } KT_CTRL_REC, tos00k_KTCtrl;

//
// +---------------------------------------------------------------+
// |   DEV WORKER THREAD CONTROL                                   |
// +---------------------------------------------------------------+
//

typedef struct dev_worker_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!
  ULONG                           ulIOCount;
  } DEV_WORKER_CTRL_REC, tos00k_DevWorkerCtrl;

typedef DEV_WORKER_CTRL_REC     *PDEV_WORKER_CTRL_REC;

//
// +---------------------------------------------------------------+
// |   DEV CONTROL                                                 |
// +---------------------------------------------------------------+
//
typedef struct dev_io_ctrl_record
  {
  LONG                            lExclusive;     // - used to grant exclusive exccess

  tsp00_Longuint                  ulDetachPending; // - detach pending since
  tsp00_Longuint                  ulAttached;      // - attached since
  ULONG                           ulDevPages;
  ULONG                           ulDBPageSize;
  ULONG                           ulDevType;
  LONG                            lDevspaceNo;

  //
  // --- overlapped io only
  //
  HANDLE                          hCompletionPort;
  HANDLE                          DevHandle;
  SAPDB_Bool                      isLocked;           // - flag to indicate locked state
  tsp2_devnamec                   szDevspace;      // - devspace name
  tsp2_dev_type_Param             DevspaceType;
  RTE_VolumeAccessMode            accessMode;
  } DEV_IO_CTRL_REC, tos00k_DevIOCtrl;

typedef DEV_IO_CTRL_REC        *PDEV_IO_CTRL_REC;


//
// +---------------------------------------------------------------+
// |   DEV0 THREAD CONTROL                                         |
// +---------------------------------------------------------------+
//

typedef struct dev0_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!

  LONG                            lTraceDevspaceNo;
  PDEV_IO_CTRL_REC                pIOCtrl;

  // ---  The io-queue is a fixed list pointing in ring
  PIO_QUEUE_REC                   pIOWorkQue;
  PIO_QUEUE_REC                   pIOReqQue;
  PIO_QUEUE_REC                   pIOs;               // - io-queue elements

  LONG                            lExclusive;         // - used to grant exclusive exccess
  } DEV0_CTRL_REC, tos00k_Dev0Ctrl;

typedef DEV0_CTRL_REC             *PDEV0_CTRL_REC;

//
// +---------------------------------------------------------------+
// |   DEVi THREAD CONTROL                                         |
// +---------------------------------------------------------------+
//
typedef struct devi_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!

  LONG                            lNDevi;             // - devspace related DEVi number
  PDEV_IO_CTRL_REC                pIOCtrl;

  HANDLE                          DevHandle;
  SAPDB_Bool                      isLocked;           // - flag to indicate locked state
  ULONG                           ulOldIOPos;

  // ---  The io-queue is a fixed list pointing in ring
  PIO_QUEUE_REC                   pIOWorkQue;
  PIO_QUEUE_REC                   pIOReqQue;
  PIO_QUEUE_REC                   pIOs;               // - io-queue elements

  LONG                            lExclusive;         // - used to grant exclusive exccess
  LONG                            lForceIOError;      // - used to grant exclusive exccess
  teo52_DevIOStatistic            DevIOStat ;
  } DEVI_CTRL_REC, tos00k_DeviCtrl;

typedef DEVI_CTRL_REC             *PDEVI_CTRL_REC;


//
// +---------------------------------------------------------------+
// |   ASYNC0 THREAD CONTROL                                       |
// +---------------------------------------------------------------+
//
typedef struct async0_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!


  // ---  The io-queue is a fixed list pointing in ring
  PIO_QUEUE_REC                   pIOWorkQue;
  PIO_QUEUE_REC                   pIOReqQue;
  PIO_QUEUE_REC                   pIOs;               // - io-queue elements
  LONG                            lExclusive;         // - used to grant exclusive exccess
  } ASYNC0_CTRL_REC, tos00k_Async0Ctrl;

typedef ASYNC0_CTRL_REC          *PASYNC0_CTRL_REC;


//
// +---------------------------------------------------------------+
// |   WATCHDOG FOR BLOCKING PIPE CONNECTS                         |
// +---------------------------------------------------------------+
//
typedef struct pipe_watchdog_ctrl_record
  {
  TID                             Tid;
  HANDLE                          hThrd;

  PTASK_CTRL_REC                  pReqTaskCtrl;
  bool                            fStopWatchDog;
  } PIPE_WATCHDOG_CTRL_REC, tos00k_PipeWatchdogCtrl;

typedef PIPE_WATCHDOG_CTRL_REC  *PPIPE_WATCHDOG_CTRL_REC;


//
// +---------------------------------------------------------------+
// |   ASYNCi THREAD CONTROL                                       |
// +---------------------------------------------------------------+
//
typedef struct asynci_ctrl_record
  {
  THREAD_CTRL_HEADER_REC               ThrdCtrlHeader;     // - !!! must be the first entry - see above !!!

  struct ukt_ctrl_record               *pUKT;              // - pointer to the UKT to
                  //   which this ASYNCi-thread belongs
  HANDLE                               DevHandle;
  ULONG                                ulDevType;
  PIPE_WATCHDOG_CTRL_REC               PipeWatchdogCtrl;
  ULONG                                ulOldIOPos;

  ULONG                                ulTapeFeatures;
  ULONG                                ulMaxTapeBlkSize;
  ULONG                                ulTapeBlkSize;

  ULONG                                ulBlockSize;
  ULONG                                ulPageShftOp;
  ULONG                                ulMaxBlockCnt;

  ULONG                                ulAsynIOCnt;

  ULONG                                ulAsynRequests;
  struct RTETask_CommonQueueElement*   pAsynRequests;

  // ---  The io-queue is a fixed list pointing in ring
  PIO_QUEUE_REC                        pIOWorkQue;
  PIO_QUEUE_REC                        pIOReqQue;
  PIO_QUEUE_REC                        pIOs;               // - io-queue elements
  ULONG                                ulIOs;

  LONG                                 lExclusive;         // - used to grant exclusive exccess
  teo52_DevIOStatistic                 DevIOStat ;
  VF_FILENAMEC                         szLogDeviceName;
  PATHNAME                             szPhysDeviceName;

  tsp00_Bool                          fIsDevspace;
  tsp00_Bool                          fForWriting;
  tsp00_Bool                          fAutoRewind;
  tsp00_Bool                          fAsyncClosePending;
  tsp00_Bool *                        cancelPointer;
  tsp00_Bool                          fMediaChanged;
  tsp00_Bool                          fECC;
  tsp00_Bool                          fCompression;
  tsp00_Bool                          fDataPadding;
  } ASYNCI_CTRL_REC, tos00k_AsynciCtrl;

typedef ASYNCI_CTRL_REC           *PASYNCI_CTRL_REC;

//
// +---------------------------------------------------------------+
// |   TASK MOVE STATISTIC                                         |
// +---------------------------------------------------------------+
//

typedef struct last_task_move_entry
{
  SAPDB_UInt8                 MoveTime;    
  PTASK_CTRL_REC              pTaskCtrl;
  SAPDB_UInt8                 TaskMaxRunnableMicroSeconds;
  PUKT_CTRL_REC               pSrcUKT;
  SAPDB_UInt8                 SrcMaxRunnableMicroSeconds;
  PUKT_CTRL_REC               pDstUKT;
  SAPDB_UInt8                 DstMaxRunnableMicroSeconds;
} LAST_TASK_MOVE_ENTRY;

typedef struct last_task_move_record
{
  SAPDB_UInt2                     CurrEntry;
  LAST_TASK_MOVE_ENTRY            Entries[NUM_OF_TASK_MOVE_STAT_RECS];
  LONG                            lExclusive;  // - used to grant exclusive exccess
} LAST_TASK_MOVE_REC;


//
// +---------------------------------------------------------------+
// |   KERNEL GLOBAL SECTION                                       |
// +---------------------------------------------------------------+
//

typedef struct kernel_global_sec_record
  {
  LONG                            lExclusive;     // - used to grant exclusive exccess

  PID                             KernelPid;
  PATHNAME                        szDBRoot;
  SQL_NODEIDC                     szServerNode;
  SQL_DBNAMEC                     szServerDB;
  PSZ                             pszUserName;
  bool                            fIsGateway;
  BOOL                            fHibernationEnabled;

  PFNENTRYFUNC                    pKernelEntryFunc;      // - callback function

  DWORD                           dwEventCategory;
  tsp00_Longuint                  ProcessorUsageMask;
  SAPDB_UInt4                     UKTProcessorRelationship;  // Hyperthreading support
  bool                            fServicePaused;
  CHAR                            szServiceName[256];

  tsp00_Longuint                  ulDatabaseStartTime;    // sec
  SAPDB_UInt4 *                   pDBState;               // pointer to tmpDBState in the startup phase, or to KSS otherwise
  SAPDB_UInt4                     tmpDBState;             // only valid in the startup phase
  ULONG                           ulDBPageShift;

  // PTS 1123712 Autoshutdown
  bool                            fWinShutdownPending;   // set to true if stopping occurs due to system shutdown
  bool                            fWinShutdownCompleted; // set to true if voffline was called and terminates the waiting loop triggered by fWinShutdownPending

  volatile tsp00_Longuint         ulCurrTime;            // - sec

  void                            *Eventing;
  struct  rte_xparam_record       *XParam;
  bool                            *pfTraceFlag;
  bool                            *pfDump;
  ULONG                           ulStartupOptions;
  bool                            fRestarted;
  PSEM_CTRL_REC                   pSemaphores;
  bool                            fSingleIOOpenLocked;
  tsp00_Uint8                     TimeMeasureBeginMicroseconds;
  ULONG                           ulDebugLevel;

  KT_CTRL_REC                     Coord;
  KT_CTRL_REC                     Console;
  KT_CTRL_REC                     Timer;
  KT_CTRL_REC                     Clock;
  KT_CTRL_REC                     Requestor;

  ASYNC0_CTRL_REC                 Async0;                // - async0 is for vasynopen
                                               //   and vasynclose only
  PASYNCI_CTRL_REC                pFirstAsynciCtrl;
  PASYNCI_CTRL_REC                pLastAsynciCtrl;

  DEV0_CTRL_REC                   Dev0;                  // - dev0 is for attach
                                               //   and detach only
  PDEVI_CTRL_REC                  pFirstDeviCtrl;
  PDEVI_CTRL_REC                  pLastDeviCtrl;

  PDEV_WORKER_CTRL_REC            pFirstDevWorkerCtrl;
  PDEV_WORKER_CTRL_REC            pLastDevWorkerCtrl;
  
  DWORD                           TlsUKTCtrlIdx;
                                               // - if a thread is no UKT the
                                               //   corresponding entry is zero.
                                               //   see macro 'THIS_UKT_CTRL'
  PUKT_CTRL_REC                   pFirstUKTCtrl;
  PUKT_CTRL_REC                   pLastUKTCtrl;

  PTASK_CTRL_REC                  pFirstFreeUserTask;

  bool                            fAlwaysCheckConnections;
  PULONG                          pulCommFlags;
  PULONG                          pulSemaFlags;
  PIPC_BASE_OBJ_NAME              pSemaNames;
  ULONG                           ulActiveUserTasks;

  tsp00_Longuint                  ulUKTBaseStackSize;    // PTS 1106437
  ULONG                           ulNumOfCommFlags;
  ULONG                           ulNumOfDevis;
  ULONG                           ulNumOfAsyncis;
  ULONG                           ulNumOfUKTs;           // - number of User Kernel Thread
  tsp00_Int4                      NumOfUserUKTs;
  ULONG                           ulNumOfTasks;          // - number of Tasks
  PTASK_CTRL_REC                  pFirstTaskCtrl;        // - Index 1
  PTASK_CTRL_REC                  pTI;                   // - Index 1
  PTASK_CTRL_REC                  pAL;                   // - Index 2
  PTASK_CTRL_REC                  pTW;                   // - Index 3
  PTASK_CTRL_REC                  pUT;                   // - Index 5

  PTASK_CTRL_REC                  pFirstDWTaskCtrl;      // - Index 6
  PTASK_CTRL_REC                  pLastDWTaskCtrl;       // - Index 6
  PTASK_CTRL_REC                  pFirstEVTaskCtrl;      // - Index 6  + num. of dw
  PTASK_CTRL_REC                  pLastEVTaskCtrl;       // - Index 6  + num. of dw
  PTASK_CTRL_REC                  pFirstBackUpTaskCtrl;  // - 
  PTASK_CTRL_REC                  pLastBackUpTaskCtrl;   // - 
  PTASK_CTRL_REC                  pFirstGCTaskCtrl;      // - 
  PTASK_CTRL_REC                  pLastGCTaskCtrl;       // - 
  PTASK_CTRL_REC                  pFirstOLTaskCtrl;      // - 
  PTASK_CTRL_REC                  pLastOLTaskCtrl;       // - 

  PTASK_CTRL_REC                  pFirstUserTaskCtrl;    // - Index FirstEV  + num. of ev
  PTASK_CTRL_REC                  pLastUserTaskCtrl;     // - first user + num. of user
  PTASK_CTRL_REC                  pFirstServerTaskCtrl;  // - last  user + 1
  PTASK_CTRL_REC                  pLastServerTaskCtrl;   // - first sv + num. of server
  PTASK_CTRL_REC                  pLastTaskCtrl;         // - = pLastServerTaskCtrl

  ULONG                           ulSystemPageSize;
  ULONG                           ulKSSSize;
  ULONG                           ulFCSSize;

  LONG                            lSharedDynDataExcl;    // - used to grant exclusive exccess
  PUCHAR                          pSharedDynDataBot;
  PUCHAR                          pSharedDynDataPtr;
  PUCHAR                          pSharedDynDataTop;

  PUCHAR                          pSharedConvCacheBot;
  PUCHAR                          pSharedConvCachePtr;
  PUCHAR                          pSharedConvCacheTop;

  LONG                            lSharedDynPoolExcl;    // - used to grant exclusive exccess
  PUCHAR                          pSharedDynPoolBot;
  PUCHAR                          pSharedDynPoolPtr;
  PUCHAR                          pSharedDynPoolTop;

  SECURITY_ATTRIBUTES             WorldSA;               // - NT only
  SECURITY_ATTRIBUTES             LockSA;                // - NT only
  SECURITY_ATTRIBUTES             AdminSA;               // - NT only
  SECURITY_ATTRIBUTES             FileSA;                // - NT only

  HANDLE                          hCompletionPort;
  ULONG                           ulNumOfDevWorker;
  ULONG                           ulNumOfConcurrentDevWorkers;

  bool                            fRunningAsNTService;
  bool                            fContinueSearchExcpt;
  bool                            fBusyWaitingForbidden;
  bool                            fMoreUserUKTsThanMaxCPU ;
  ULONG                           ulNumOfProcessors;
  ULONG                           ulNumOfPhysicalProcessors;
  ULONG                           ulStaticDebugBreakPos;
  ULONG                           ulSingleDebugBreakPos;
  ULONG                           ulServiceType;

  SAPDB_Bool                      TaskMoved;
  SAPDB_UInt2                     CurrTimeVar;         // Value 0 .. TIME_VAL_ARRAY_SIZE
  SAPDB_UInt8                     TaskMoveIntervalStartTimes[TIME_VAL_ARRAY_SIZE];
  SAPDB_UInt2                     TaskMoveMeasIntervals;
#if defined(UNCONNECT_TASK_LIST)
  TASK_LIST_HEAD_REC              UnconnectedMoveTaskList; // - unconnected moveable tasks
#endif
  LAST_TASK_MOVE_REC              LastTaskMoves;
  SAPDB_UInt4                     ConnectedUserTasks;

  ULONG                           ulTerminationTimeOut;
  DWORD                           CrashingTid;
  HANDLE                          CrashingThrdHandle;

  ULONG                           ulSuspendReasons[MAX_SUSPEND_REASONS];
  ULONG                           TotalDataIOCnt;
  void                           *MessageBufferSpace; // PTS 1103824
  tsp00_PageAddr                  traceBufferSpace; // PTS 1108768 Space for Tracebuffer used for InMemory tracing 
  tsp00_Int4                      totalTraceBufferSizeInPages;
  tsp00_Int4                      traceNeedsSaving;
  bool                            dumpDiagFiles;   //PTS 1115180
  SAPDB_Bool                      isHotStandbyConfiguration;

  SAPDB_Int4                      taskResponsibleForOffline; /* id of task responsible for offline */
  SAPDB_UInt8                     symbolResolutionLockCount; /* counts the locks of symbol resolution code */
  } KERNEL_GLOBAL_SEC, tos00k_KernelGlobalSec;


//
// +===============================================================+
// |   GLOBAL VARIABLE DEFINITIONS                                 |
// +===============================================================+
//

externC  KERNEL_GLOBAL_SEC          kgs;
externC  struct kernel_shared_seg   *pkss;
externC  FLAG_COMM_SEG              *pfcs;

//
// +---------------------------------------------------------------+
// |   PROTOTYPES (Debug/Development)                              |
// +---------------------------------------------------------------+
//
PUKT_CTRL_REC sql01k_get_and_check_current_PUKT_CTRL_REC ( VOID );
VOID          sqlk01_view_stack_page_flags               ( VOID );
VOID          sql01k_halt                                ( PSZ pszServerDB );

//
// +---------------------------------------------------------------+
// |   PROTOTYPES (Internal functions)                             |
// +---------------------------------------------------------------+
//
externC VOID           sql50_dump_SEH                   ( PSZ  pszFormatStr, ... );
externC void           sql50k_resume_tracewriter        ();

externC DWORD          sql50k_excepthandler             ( LPEXCEPTION_POINTERS lpExcPtrs );
externC LONG           sql50k_disable_error_handling    ( VOID );
externC VOID           sql50k_rte_abort                 ( BOOLEAN        write_crash_dump,
                                                          ULONG          ulLine,
                                                          PSZ            pszModule );
externC LONG           sql50k_set_exit_list             ( VOID );
externC VOID _System   sql50k_exithandler               ( VOID );
externC ULONG          sql50k_get_termination_timeout   ( VOID );

externC VOID           sql51k_finish                    ( VOID );
externC BOOL           sql51k_auto_shutdown             ( VOID );
externC VOID           sql51k_debug_break               ( ULONG ulDebugBreakPosition );


externC VOID           sql52k_vclock_init               ( VOID );
externC VOID           sql53k_cancel_task               ( PTASK_CTRL_REC  pTaskCtrl );
externC BOOL           sql53k_comm_cancel               ( PTASK_CTRL_REC  pTaskCtrl,
                                                          ULONG           ulCommState );
externC BOOL           sql53k_kill_connection           ( PTASK_CTRL_REC  pTaskCtrl,
                                                          ULONG           ulCommState );
externC VOID           sql53k_remove_unrel_connnections ( VOID );
externC BOOL           sql53k_comm_termination_cleanup  ( PTASK_CTRL_REC  pTaskCtrl );
externC ULONG          sql53k_get_connection_state      ( PTASK_CTRL_REC  pTaskCtrl );

externC VOID _System   sql54k_dev0                      ( VOID );
externC VOID _System   sql54k_dev_worker                ( PDEV_WORKER_CTRL_REC pDevWorkerCtrl );
externC VOID           sql54k_dev0_devi_crash_cleanup   ( TID                  CrashedTid );

externC VOID           sql55k_free_regions              ( PUKT_CTRL_REC        pUKT );


externC LONG           sql57k_pmalloc                   ( ULONG   ulLine,
                                                          PSZ     pszModuleName,
                                                          PPVOID  ppMemory,
                                                          ULONG   ulSize );
externC LONG           sql57k_pfree                     ( ULONG   ulLine,
                                                          PSZ     pszModuleName,
                                                          PVOID   pMemory );
externC LONG           sql57k_alloc_vnewbuf_pool        ( VOID );
externC LONG           sql57k_alloc_vallocat_pool       ( VOID );

externC VOID           sql61k_post_xstart_sem           ( PSZ  pszServerDB,
                                                          BOOL fCrash );
externC VOID           sql61k_post_xstop_sem            ( PSZ  pszServerDB );

externC BOOL           sql63k_distrib_crash_cleanup     ( PTASK_CTRL_REC  pTaskCtrl );
externC VOID           sql63k_remove_unrel_connnections ( VOID );

externC VOID           sql65k_async0                    ( VOID );

externC LONG           sql70k_init_control_records      ( VOID );
externC VOID           sql70k_init_user_comm_ctrl       ( struct user_task_comm_ctrl    *pUserCommCtrl );
externC VOID           sql70k_init_task_ctrl            ( PTASK_CTRL_REC           pCurrTaskCtrl );
externC VOID           sql70k_init_task_ctrl_cntr       ( PTASK_CTRL_REC           pCurrTaskCtrl );
externC VOID           sql70k_init_devi_ctrl            ( PDEVI_CTRL_REC           pDeviCtrl,
                                                          PIO_QUEUE_REC            pIOs );
externC VOID           sql70k_init_dev_io               ( PDEV_IO_CTRL_REC         pIOCtrl );

externC VOID           sql70k_init_asynci_ctrl          ( PASYNCI_CTRL_REC         pAsynciCtrl );

externC VOID           sql71k_init_ukt_and_task_stacks  ( PUKT_CTRL_REC            pUKTCtrl,
                                                          PUCHAR                   pStackLimit );

externC VOID           sql71k_free_task_stack           ( CONTEXT_REC              *pContext );
externC VOID           sql71k_init_task_stack           ( CONTEXT_REC              *pContext,
                                                          VOID                     (* pFunc) (VOID) );
externC LONG           sql71k_alloc_task_stack          ( CONTEXT_REC              *pContext );

externC BOOL _Optlink  sql71k_first_task_stack          ( CONTEXT_REC              *pNewContext );

externC BOOL _Optlink  sql71k_chg_task_stack            ( CONTEXT_REC              *pNewContext,
                                                          CONTEXT_REC              *pOldContext );
externC VOID _Optlink  sql71k_init_task_stack_frame     ( CONTEXT_REC              *pContext,
                                                          VOID                     (* pFunc) (VOID) );

externC VOID           sql72k_wake_kt                   ( PTHREAD_CTRL_HEADER_REC  pThrdCtrlHd );
externC VOID           sql72k_wake_ukt                  ( PUKT_CTRL_REC            pUKTCtrl );

externC VOID           sql73k_IO_queue_init             ( PIO_QUEUE_REC  pIO_QueElem,
                                                          LONG           lNum );

externC VOID           sql73k_UKT_queue_init            ( PUKT_QUEUE_HEAD_REC  pUKT_QueHead,
                                                          PUKT_QUEUE_REC       pUKT_QueElem,
                                                          LONG                 lNum );

externC VOID           sql73k_UKT_enqu                  ( PUKT_CTRL_REC        pUKT,
                                                          PTASK_CTRL_REC       pTaskCtrl,
                                                          PDLQ_REC             pDLQElem );
externC VOID           sql73k_UKT_enqu_no_task          ( PUKT_CTRL_REC        pUKT, 
                                                          PDLQ_REC             pRequest );
externC VOID           sql73k_DLQ_init                  ( PDLQ_HEAD_REC        pDLQHead );

externC VOID           sql73k_UToU_queue_init           ( PUKT_TO_UKT_QUEUE_HEAD_REC  pUToUQueHead,
                                                          PUKT_TO_UKT_QUEUE_REC       pUToUs,
                                                          LONG                        lNum );
externC VOID           sql73k_UToU_enqu                 ( PUKT_CTRL_REC        pUKT,
                                                          PTASK_CTRL_REC       pFromTaskCtrl,
                                                          PUKT_CTRL_REC        pRemoteUKTCtrl,
                                                          PTASK_CTRL_REC       pRemoteTaskCtrl,
                                                          PFNREMOTEFUNC        pFuncToExecute,
                                                          void*                pFuncParam );
externC VOID           sql73k_tlist_init                ( PTASK_LIST_HEAD_REC  pTaskList );
externC VOID           sql73k_tlist_dequ_task_nolock    ( PTASK_LIST_HEAD_REC  pTaskList,
                                                          PTASK_CTRL_REC       pTaskCtrl );
externC VOID           sql73k_tlist_enque_task_nolock   ( PTASK_LIST_HEAD_REC  pTaskList,
                                                          PTASK_CTRL_REC       pTaskCtrl );
externC VOID           sql73k_tlist_dequ_task           ( PTASK_LIST_HEAD_REC  pTaskList,
                                                          PTASK_CTRL_REC       pTaskCtrl );
externC VOID           sql73k_tlist_enque_task          ( PTASK_LIST_HEAD_REC  pTaskList,
                                                          PTASK_CTRL_REC       pTaskCtrl );
externC PTASK_CTRL_REC sql73k_tlist_first_task          ( PTASK_LIST_HEAD_REC  pTaskList );
externC PTASK_CTRL_REC sql73k_tlist_next_task           ( PTASK_LIST_HEAD_REC  pTaskList,
                                                          PTASK_CTRL_REC       pTaskCtrl );

externC VOID           sql74k_alter_task_cnt            ( PTASK_CTRL_REC       pCTask,
                                                          SAPDB_UInt2          AlterType );
externC tsp00_Longuint sql74k_load_balancing            ();
externC VOID           sql74k_temp_move_lock            ( PTASK_CTRL_REC       pTaskCtrl,
                                                          SAPDB_UInt2          Cnt,
                                                          SAPDB_Bool           SetLock );
externC VOID           sql74k_long_term_move_lock       ( PTASK_CTRL_REC       pTaskCtrl,
                                                          SAPDB_UInt2          LockType,
                                                          SAPDB_Bool           SetLock );

externC VOID           sql74k_remote_func_error         ( PPTASK_CTRL_REC      ppCTask,
                                                          PTASK_CTRL_REC       pFromTask,
                                                          PPDLQ_REC            ppCurrReq,
                                                          void *               pFuncParam);
externC VOID           sql74k_UKT_enqu_and_wake         ( PUKT_CTRL_REC        pRemoteUKT,
                                                          PTASK_CTRL_REC       pRemoteTaskCtrl,
                                                          PDLQ_REC             pRequest );
externC VOID           sql74k_UToU_enqu_and_wake        ( PUKT_CTRL_REC        pFromUKT,
                                                          PTASK_CTRL_REC       pFromTaskCtrl,
                                                          PUKT_CTRL_REC        pRemoteUKTCtrl,
                                                          PTASK_CTRL_REC       pRemoteTaskCtrl,
                                                          PFNREMOTEFUNC        pFuncToExecute,
                                                          void*                pFuncParam );
externC VOID           sql74k_RUN_enqu                  ( PUKT_CTRL_REC        pUKT,
                                                          PTASK_CTRL_REC       pReqTaskCtrl,
                                                          PTASK_CTRL_REC       pFromTaskCtrl,
                                                          PDLQ_REC             pRequest,
                                                          ULONG                ulBasePrio);
externC VOID           sql74k_YieldTaskRunEnqu          ( PUKT_CTRL_REC        pUKT,
                                                          PTASK_CTRL_REC       pTask,
                                                          PDLQ_REC             pRequest, 
                                                          tsp00_Bool           ulBasePrio );
externC VOID           sql74k_on_task_debug_request     ( PUKT_CTRL_REC        *ppUKT );

externC VOID           sql74k_dispatcher                ( PUKT_CTRL_REC        pUKT );
externC BOOL           sql74k_checkcom                  ( PUKT_CTRL_REC        pUKT );
externC BOOL           sql74k_kill_dead_conn            ( PTASK_CTRL_REC       pTaskCtrl );
externC VOID           sql74k_kill_dead_receiver_conn   ( INT                  iIndex );

externC LONG           sql79k_read_params               ( VOID );

externC INT            sql80kn_CreateWindow             ( HINSTANCE            hInstance,
                                                          HINSTANCE            hPrevInstance,
                                                          PPROG_ARGS           pArgs,
                                                          INT                  nCmdShow );
externC VOID           sql80k_NewSrvState               ( SAPDB_UInt4          ulDBState );
externC VOID           sql80kn_set_db_state             ( SAPDB_UInt4          ulDBState );

externC BOOL           sql81k_PortableInstallationAcitve();

externC LONG           sql81k_Coordinator               ( PSZ                  pszServerDB,
                                                          INT                  argc,
                                                          PCHAR                argv[] );

externC APIRET         sql81k_create_kernel_thread      ( SAPDB_Char const      *threadName
                                                        , TID                   *pTid
                                                        , HANDLE                *phThrd
                                                        , LPVOID                 threadMain
                                                        , LPVOID                 threadArg
                                                        , SIZE_T                 threadStackSize
                                                        , SAPDB_Bool             unconditional );
externC LPVOID         sql81k_create_kernel_fiber       ( SIZE_T                fiberStackSize
                                                        , LPVOID                fiberMain
                                                        , LPVOID                fiberArg );
externC VOID           sql81k_delete_kernel_fiber       ( SIZE_T                fiberStackSize
                                                        , LPVOID                fiberHandle );
externC SAPDB_Bool     sql88k_reset_UKT_CPU_relationship( PUKT_CTRL_REC         pUKT );
externC SAPDB_Bool     sql88k_set_UKT_CPU_relationship  ( PUKT_CTRL_REC         pUKT
                                                        , SAPDB_Bool            forceAffinity );

externC LPVOID         RTE_AllocStack                   (LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize, SAPDB_Bool doCommit);
externC SAPDB_Bool     RTE_FreeStack                    (LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize);
externC SAPDB_Bool     RTE_PseudoAllocStack             (SAPDB_ULong stackSize, SAPDB_ULong guardSize);
externC SAPDB_Bool     RTE_PseudoFreeStack              (SAPDB_ULong stackSize, SAPDB_ULong guardSize);

externC BOOL           sql81k_wakeup_corrdinator        ( VOID );
externC BOOL           sql81k_hibernation_enabled       ( VOID );
 
externC VOID _System   sql82k_console                   ( VOID );

externC VOID _System   sql83k_timer                     ( VOID );
externC VOID           sql83k_timer_job                 ( PDLQ_REC             pDLQElem );

externC VOID _System   sql84k_requestor                 ( VOID );
externC VOID           sql84k_test_receiver_conns       ( VOID );
externC VOID           sql84k_test_appl_conns           ( VOID );
externC VOID           sql84k_test_util_conn            ( VOID );

externC VOID _System   sql85k_clock                     ( VOID );

externC VOID _System   sql88k_ukt                       ( PUKT_CTRL_REC        pUKT );
externC APIRET         sql88k_create_task_or_fiber      ( PTASK_CTRL_REC       pTaskCtrl );
externC VOID           sql88k_release_curr_task_or_fiber( PUKT_CTRL_REC*       ppUKT );
#endif  /* GOS00K_H */
