/*!
  @file           rte_consolestandarddatarequests.h
  @author         StefanP
  @special area   Console
  @brief          Common Defines and Typedefs for the Standard Data Requests
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTE_CONSOLESTANDARDDATAREQUESTS_H
#define RTE_CONSOLESTANDARDDATAREQUESTS_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include "gos00k.h"     /* nocheck */
#endif                  /*&endif*/

#include "heo46.h"
#include "heo00.h"
#include "geo52.h"
#include "geo002.h"
#include "RunTime/Tasking/RTETask_Task.h"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined (_WIN32) 
#define SECURITY_CHECK_ACCESS_RIGHTS    0x8000
#else
#define SECURITY_CHECK_ACCESS_RIGHTS    0
#endif

#define CON_REQ_TASK                    1
#define CON_REQ_IO                      2
#define CON_REQ_REGION                  3
#define CON_REQ_STORAGE                 4
#define CON_REQ_STATE                   5
#define CON_REQ_RTE                     6
#define CON_REQ_CANCEL                  7 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_KILL                    8 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_TASK_STAT               9
#define CON_REQ_TASK_COUNTS            10
#define CON_REQ_QUEUE_STAT             11
#define CON_REQ_TOTAL_COUNTS           12
#define CON_REQ_ENABLE_TIME_MEASURE    13 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_DISABLE_TIME_MEASURE   14 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_UKT_SLEEPTIME_STAT     15
#define CON_REQ_SHOW_SUSPENDS          16
#define CON_REQ_THREAD_TIMES           17
#define CON_REQ_AWE                    18
#define CON_REQ_CONNECTIONS            19
#define CON_REQ_MOVE_INFO              20
#define CON_REQ_TASK_MOVE              21
#define CON_REQ_SET_CHECK_INTERVAL     22 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_TRACE_FLUSH            26 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_CRASH_DUMP             27 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_RES_IO                 28 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_REGION             29 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_TASK_COUNTS        30 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_UKT_SLEEPTIME_STAT 31 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_QUEUE_STAT         32 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_SUSPENDS           33 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_RES_ALL                34 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_GETPARAM               35 | SECURITY_CHECK_ACCESS_RIGHTS
//#define CON_REQ_PUTPARAM               36 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_SET_DEBUGLEVEL         37 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_UKT_QUEUES             38
#define CON_REQ_SET_UKT_AFFINITY       40 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_AIO                    41 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_FORCE_IO_ERROR         42 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_DEBUG_BREAK            43 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_SET_PROCESSOR_MASK     44 | SECURITY_CHECK_ACCESS_RIGHTS
#define CON_REQ_DEBUG_TASK             45 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQ_TRACE_CONSOLE          50
#define CON_REQ_PAGECACHE_RELEASE      51
#define CON_REQ_DIAGNOSE_PAGECACHE     52

#define CON_REQ_RWREGIONS              53
#define CON_REQ_MINI_DUMP              55 | SECURITY_CHECK_ACCESS_RIGHTS

#define CON_REQUEST(_request)              ((_request) & ~SECURITY_CHECK_ACCESS_RIGHTS)  
#define CON_REQUEST_SECURITY(_request)     ((_request) != CON_REQUEST (_request))

/* --- Request types (SM)--------------------------------------------------- */

/* Special Requests: 0xFF000000 - 0xFFFF0000 */
#define CON_REQ_OPEN            0xFF000000
#define CON_REQ_CLOSE           0xFF010000
#define CON_REQ_UNDEFINED       0xFFFF0000


/* Tests */
#define CON_REQ_TEST01          0xFFFF0001
#define CON_REQ_TEST02          0xFFFF0002


//
// --- RTE return codes ( cancel, kill, debug break )
//
#define CON_NO_ERROR                    0
#define CON_ERROR                       1
#define CON_WRONG_TASK                  2
#define CON_WRONG_TASK_TYPE             3
#define CON_WRONG_UKT                   4
#define CON_NO_PROCESSOR                5
#define CON_NO_INTERACT_WITH_DECTOP     6
#define CON_TASK_INACTIVE               7
#define CON_WRONG_VALUE                 8

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/* SM Struktur */

#if defined (_WIN32)
//
// --- Common DEV info structs
//
typedef struct cons_dev_info_record
{
    SAPDB_ULong                     ulDevspaceNo;
    tsp2_devnamec                   DevspaceName;
    SAPDB_UInt8                     ulReadCnt;
    SAPDB_UInt8                     ulReadPageCnt;
    SAPDB_UInt8                     ulWriteCnt;
    SAPDB_UInt8                     ulWritePageCnt;
} CONS_DEV_INFO_REC;

typedef CONS_DEV_INFO_REC         *PCONS_DEV_INFO_REC;

typedef struct cons_devi_info_record
{
    TID                             Tid;
    tsp2_devnamec                   DevspaceName;
    SAPDB_Long                      ulDEViIndex;
    SAPDB_ULong                     ulDevspaceNo;
    teo52_DevIOStatistic            DevIOStat ;
} CONS_DEVI_INFO_REC;

typedef CONS_DEVI_INFO_REC        *PCONS_DEVI_INFO_REC;

typedef struct cons_asynci_info_record
{
    TID                             Tid;
    VF_FILENAMEC                    szLogDeviceName;
    BOOL                            fForWriting;
    BOOL                            ulBlockSize;
    BOOL                            ulMaxBlockCnt;
    teo52_DevIOStatistic            DevIOStat ;
} CONS_ASYNCI_INFO_REC;

typedef CONS_ASYNCI_INFO_REC      *PCONS_ASYNCI_INFO_REC;

//
// --- IO info
//
typedef struct cons_io_info_record
{
    SAPDB_Long                      lNumOfDevspaces;
    SAPDB_ULong                     ulDevspaceInfoOffset;    // - CONS_DEV_INFO_REC  * 'MAX_DEVSPACES'
} CONS_IO_INFO_REC;

typedef CONS_IO_INFO_REC          *PCONS_IO_INFO_REC;

//
// --- AIO info
//
typedef struct cons_aio_info_record
{
    SAPDB_Long                      lNumOfASYNCis;
    SAPDB_ULong                     ulASYNCiInfoOffset;      // - CONS_ASYNCI_INFO_REC * 'MAX_DEVSPACES' + 'MAX_BACKUP_DEVS'
} CONS_AIO_INFO_REC;

typedef CONS_AIO_INFO_REC         *PCONS_AIO_INFO_REC;

//
// --- Region info
//
typedef struct cons_single_sem_info_record
{
    SAPDB_ULong                     ulSid;
    SAPDB_ULong                     ulOwnerTaskIndex;
    C8C                             szSemaName;
    SAPDB_ULong                     lGetCount;
    SAPDB_ULong                     lBegExclTasCount;
    SAPDB_ULong                     lEndExclTasCount;
    SAPDB_ULong                     lCollisionCount;
    SAPDB_ULong                     lWaitCount;
    SAPDB_Long                      lExclusive;
    SAPDB_ULong                     lNumWaitTasks;
    SAPDB_ULong                     ulWaitTaskOffset;        // - SAPDB_ULong * 'MAX_TASKS'
} CONS_SINGLE_SEM_INFO_REC;

typedef CONS_SINGLE_SEM_INFO_REC  *PCONS_SINGLE_SEM_INFO_REC;


typedef struct cons_sem_info_record
{
    SAPDB_Long                      lNumOfSems;
    SAPDB_ULong                     ulSingleSemInfoOffset;   // - CONS_SINGLE_SEM_INFO_REC * 'MAX_SEMAPHORES'
} CONS_SEM_INFO_REC;

typedef CONS_SEM_INFO_REC         *PCONS_SEM_INFO_REC;



//
// --- Storage info
//

typedef struct cons_task_storage_info_record
{
    SAPDB_ULong                     ulTaskIndex;
    UINT1                           TaskType;           // - task type (TT_US,...)
    RTE_TaskDisplayNameShort        displayName;
    tsp00_Longint                   ulStackUsed;
    tsp00_Longint                   ulStackFree;
} CONS_TASK_STORAGE_INFO_REC;

typedef CONS_TASK_STORAGE_INFO_REC *PCONS_TASK_STORAGE_INFO_REC;


typedef struct cons_storage_info_record
{
    tsp00_Longint                   lDynDatAvail;
    tsp00_Longint                   lDynDatUsed;
    tsp00_Longint                   lDynPoolAvail;
    tsp00_Longint                   lDynPoolUsed;
    SAPDB_Long                      lTaskStacks;
    SAPDB_ULong                     ulKGSSize;
    SAPDB_ULong                     ulKSSSize;
    SAPDB_ULong                     ulTasksTotal;
    SAPDB_Long                      lNumOfTasks;
    SAPDB_ULong                     ulTaskStorInfoOffset;    // - CONS_TASK_STORAGE_INFO_REC * 'MAX_TASKS'
} CONS_STORAGE_INFO_REC;

typedef CONS_STORAGE_INFO_REC     *PCONS_STORAGE_INFO_REC;



//
// --- State info
//
typedef struct cons_state_info_record
{
    UINT1                           DatabaseState;         // - Database state
    SAPDB_ULong                     ulTerminationTimeOut;
} CONS_STATE_INFO_REC;

typedef CONS_STATE_INFO_REC       *PCONS_STATE_INFO_REC;

typedef struct cons_set_ukt_affinity_record
{
    USHORT                          usReturnCode;
    SAPDB_ULong                     ulUKTIndex;
} CONS_SET_UKT_AFFINITY_REC;

typedef CONS_SET_UKT_AFFINITY_REC *PCONS_SET_UKT_AFFINITY_REC;

// PTS 1107070
typedef struct cons_set_processor_mask_record
{
    USHORT                          usReturnCode;
    SAPDB_ULong                     ProcessAffinityMask;
    UINT1                           TaskType;             // - task type (TT_US,...)
} CONS_SET_PROCESSOR_MASK_REC;

typedef CONS_SET_PROCESSOR_MASK_REC *PCONS_SET_PROCESSOR_MASK_REC;

//
// --- RTE info
//
typedef struct cons_ukt_info_record
{
    SAPDB_ULong                     ulUKTIndex;
    TID                             Tid;
    INT4                            ThrdState;
    double                          dDispatcherCalls;
    double                          dTaskSwitchCalls;
    SAPDB_ULong                     ulActiveTasks;
    SAPDB_ULong                     ulTotalTasks;
    CHAR                            szCluster[80];
    SAPDB_ULong                     ulProcessor;
    SAPDB_UInt4                     CPURelationship;
    SAPDB_Long                      lNumOfDevspaces;
    SAPDB_ULong                     ulDevspaceInfoOffset;    // - CONS_DEV_INFO_REC  * 'MAX_DEVSPACES'
} CONS_UKT_INFO_REC;

typedef CONS_UKT_INFO_REC         *PCONS_UKT_INFO_REC;

typedef struct cons_dev_worker_info_record
{
    TID                             Tid;
    INT4                            ThrdState;
    SAPDB_ULong                     ulIOCount;
} CONS_DEV_WORKER_INFO_REC;

typedef CONS_DEV_WORKER_INFO_REC *PCONS_DEV_WORKER_INFO_REC;


typedef struct cons_rte_info_record
{
    SAPDB_Long                      lNumOfDevWorkers;
    SAPDB_ULong                     ulDevWorkerInfoOffset;

    SAPDB_Long                      lNumOfUKTs;
    SAPDB_ULong                     ulUKTInfoOffset;         // - CONS_UKT_INFO_REC * 'MAXUKT'

    SAPDB_Long                      lNumOfDEVis;
    SAPDB_ULong                     ulDEViInfoOffset;        // - CONS_DEVI_INFO_REC * 'MAX_DEVSPACES' * 'MAX_DEVI_P_DEVSPACE'

    SAPDB_ULong                     ulNumOfConcurrentDevWorkers;
    SAPDB_Long                      lTraceDevspaceNo;
    TID                             CoordTid;
    UINT1                           CoordState;
    TID                             ConsoleTid;
    UINT1                           ConsoleState;
    TID                             TimerTid;
    UINT1                           TimerState;
    TID                             ClockTid;
    UINT1                           ClockState;
    TID                             Dev0Tid;
    UINT1                           Dev0State;
    TID                             Async0Tid;
    UINT1                           Async0State;
    TID                             RequestorTid;
    UINT1                           RequestorState;
    SAPDB_ULong                     numLogProcessors;
    SAPDB_ULong                     numPhysProcessors;
} CONS_RTE_INFO_REC;

typedef CONS_RTE_INFO_REC         *PCONS_RTE_INFO_REC;


typedef struct cons_cancel_task_record
{
    USHORT                          usReturnCode;
    SAPDB_ULong                     ulTask;
} CONS_CANCEL_TASK_REC;

typedef CONS_CANCEL_TASK_REC      *PCONS_CANCEL_TASK_REC;


typedef struct cons_kill_task_record
{
    USHORT                          usReturnCode;
    SAPDB_ULong                     ulTask;
} CONS_KILL_TASK_REC;

typedef CONS_KILL_TASK_REC        *PCONS_KILL_TASK_REC;

//
// --- Task Statistic Counts
//
typedef struct cons_single_task_counts_record
{
    SAPDB_ULong                     ulTaskIndex;
    UINT1                           TaskType;           // - task type (TT_US,...)
    RTE_TaskDisplayName             displayName;
    SAPDB_Int8                      ApplPid;            // - process id of connected user process
    RTE_Nodename                    szClientNode;
    SAPDB_UInt8                     ulSelfSuspCount;
    SAPDB_UInt8                     ulBegExclCalls;
    teo52_TaskPrioStatistic         PrioStat;
    teo52_TaskStateStatisticRec     StateStat;
    teo52_TaskOMSStatisticRec       TaskOMSStat;        // 1106969

    SAPDB_ULong                     ulCommands;
    SAPDB_UInt8                     ulDispatcherCalls;
    SAPDB_Long                      lResumeCount;
    tsp00_Int4                      totalResumerCount;
    tsp00_TaskId                    resumerPid[MAX_RESUMER_PIDS];
    tsp2_devnamec                   DevNameWaitingFor; // PTS 1106969
} CONS_SINGLE_TASK_COUNTS_INFO_REC;

typedef CONS_SINGLE_TASK_COUNTS_INFO_REC  *PCONS_SINGLE_TASK_COUNTS_INFO_REC;

typedef struct cons_task_counts_info_record
{
    BOOL                            fTimeMeasureActive ;
    USHORT                          usReturnCode;           // -  PTS 1104727
    SAPDB_Long                      lNumOfActiveTasks;
    SAPDB_ULong                     ulTaskCountsInfoOffset; // -  CONS_SINGLE_TASK_COUNTS_INFO_REC * 'MAX_TASKS'
} CONS_TASK_COUNTS_INFO_REC;

typedef CONS_TASK_COUNTS_INFO_REC *PCONS_TASK_COUNTS_INFO_REC;


//
// --- Queue Statistic info
//
typedef struct cons_single_queue_info_record
{
    SAPDB_ULong                     ulTaskIndex;
    teo52_TaskQueueStatistic        QueStat;
} CONS_SINGLE_QUEUE_STAT_INFO_REC;

typedef CONS_SINGLE_QUEUE_STAT_INFO_REC *PCONS_SINGLE_QUEUE_STAT_INFO_REC;


typedef struct cons_queue_stat_info_record
{
    SAPDB_Long                      lNumOfActiveTasks;
    SAPDB_ULong                     ulQueStatInfoOffset;     // -  CONS_SINGLE_QUEUE_STAT_INFO_REC * 'MAX_TASKS'
} CONS_QUEUE_STAT_INFO_REC;

typedef CONS_QUEUE_STAT_INFO_REC  *PCONS_QUEUE_STAT_INFO_REC;

typedef struct cons_debug_break_record
{
    USHORT                          usReturnCode;
    BOOL                            fClearPosition;
    SAPDB_ULong                     ulBreakPosition;
} CONS_DEBUG_BREAK_REC;

typedef CONS_DEBUG_BREAK_REC *PCONS_DEBUG_BREAK_REC;

//
// --- Total Counter Statistic Info
//
typedef struct cons_total_counts_info_record
{
    SAPDB_UInt8                     ulTotalDispCalls;
    SAPDB_UInt8                     ulTotalCommands;
    SAPDB_UInt8                     ulTotalVwait;
    SAPDB_UInt8                     ulTotalVsleep;
    SAPDB_UInt8                     ulTotalVsusp;
    SAPDB_UInt8                     ulTotalSelfIO;
    SAPDB_UInt8                     ulTotalDevIO;
    SAPDB_UInt8                     ulTotalRegAccess;
    SAPDB_UInt8                     ulTotalRegColl;
    SAPDB_UInt8                     ulTotalRegWait;
    SAPDB_UInt8                     ulTotalBegExclTasCount;
    SAPDB_UInt8                     ulTotalEndExclTasCount;
    teo52_TaskPrioStatistic         TotalPrioStat;
    SAPDB_UInt8                     ulTotalSymbolResolutionLocks;
} CONS_TOTAL_COUNTS_INFO_REC;

typedef CONS_TOTAL_COUNTS_INFO_REC  *PCONS_TOTAL_COUNTS_INFO_REC;


//------------------

typedef struct cons_set_debug_level_record
{
    SAPDB_ULong                     ulDebugLevel;
} CONS_SET_DEBUGLEVEL_REC;

typedef CONS_SET_DEBUGLEVEL_REC     *PCONS_SET_DEBUGLEVEL_REC;

typedef struct cons_single_ukt_sl_stat_info_rec
{
    SAPDB_ULong                     ulUKTIndex;
    TID                             Tid;
    SAPDB_ULong                     ulRUNQueueLen;
    SAPDB_UInt8                     measuredMicroSeconds;
    teo52_TaskSchedulerTime         UKTIdleStat;        
    teo52_TaskSchedulerTime         UKTIOWaitStat;
} CONS_SINGLE_UKT_SL_STAT_INFO_REC;

typedef CONS_SINGLE_UKT_SL_STAT_INFO_REC *PCONS_SINGLE_UKT_SL_STAT_INFO_REC;


typedef struct cons_ukt_sleep_stat_info_record
{
    SAPDB_Long                      lNumOfActiveUKTs;
    SAPDB_ULong                     ulSleepStatInfoOffset;  // - CONS_SINGLE_UKT_SL_STAT_INFO_REC * 'MAXUKT'
} CONS_UKT_SLEEPTIME_REC;

typedef CONS_UKT_SLEEPTIME_REC    *PCONS_UKT_SLEEPTIME_REC;


typedef struct cons_show_suspend_rec
{
    SAPDB_ULong                     ulSuspends [ MAX_SUSPEND_REASONS ] ;
} CONS_SHOW_SUSPEND_REC;

typedef CONS_SHOW_SUSPEND_REC     *PCONS_SHOW_SUSPEND_REC;


typedef struct cons_getparam_rec
{
    RTE_XPARAM_REC                  XParam;
} CONS_GETPARAM_REC;

typedef CONS_GETPARAM_REC         *PCONS_GETPARAM_REC;

typedef struct cons_single_queue_info_rec
{
    SAPDB_ULong                     ulTaskIndex;
    SAPDB_ULong                     ulDynPrio;
    SAPDB_ULong                     ulReqType;
    SAPDB_ULong                     ulFromTask;
    UINT1                           TaskState;
    UINT1                           PrioFlag;
} CONS_SINGLE_QUEUE_INFO_REC;

typedef CONS_SINGLE_QUEUE_INFO_REC *PCONS_SINGLE_QUEUE_INFO_REC;

typedef struct cons_single_ukt_queue_info_rec
{
    SAPDB_ULong                     ulUKTIndex;
    TID                             Tid;
    SAPDB_ULong                     ulRUNQueueLen;
    SAPDB_ULong                     ulRUNQueueOffset;
    SAPDB_ULong                     ulCOMQueueLen;
    SAPDB_ULong                     ulCOMQueueOffset;
    SAPDB_ULong                     ulUKTQueueLen;
    SAPDB_ULong                     ulUKTQueueOffset;
    SAPDB_ULong                     ulUToUQueueLen;
    SAPDB_ULong                     ulUToUQueueOffset;
} CONS_SINGLE_UKT_QUEUES_INFO_REC;

typedef CONS_SINGLE_UKT_QUEUES_INFO_REC *PCONS_SINGLE_UKT_QUEUES_INFO_REC;


typedef struct cons_ukt_queue_info_record
{
    SAPDB_Long                      lNumOfActiveUKTs;
    SAPDB_ULong                     ulUKTQueuesOffset;  // - CONS_SINGLE_UKT_QUEUE_INFO_REC * 'MAXUKT'
} CONS_UKT_QUEUES_REC;

typedef CONS_UKT_QUEUES_REC    *PCONS_UKT_QUEUES_REC;

typedef struct cons_force_io_error_record
{
    SAPDB_ULong                     ulDevno;
    BOOL                            fErrOnRead ;
} CONS_FORCE_IO_ERROR_REC;

typedef struct cons_single_thread_times_record
{
    TID                             Tid;
    CHAR                            szThreadName[20];
    CHAR                            szCluster[80];
    FILETIME                        SystemTime; // time the thread has spent in kernel mode 
    FILETIME                        UserTime;   // time the thread has spent in user mode 
} CONS_SINGLE_THREAD_TIMES_INFO_REC ;

typedef CONS_SINGLE_THREAD_TIMES_INFO_REC *PCONS_SINGLE_THREAD_TIMES_INFO_REC;

typedef struct cons_thread_times_record
{
    SAPDB_Long                      lNumOfUKTs;
    SAPDB_ULong                     ulThreadTimesOffset; // -  CONS_SINGLE_THREAD_TIMES_INFO_REC * 'lNumOfUKTs'
} CONS_THREAD_TIMES_INFO_REC;

typedef CONS_THREAD_TIMES_INFO_REC *PCONS_THREAD_TIMES_INFO_REC;


//
// --- AWE info
//
typedef struct cons_awe_info_record
{
    bool                            AWEAvailable;

    SAPDB_UInt4                     NumOfPhysPages;
    SAPDB_UInt4                     NumOfFreePhysPages;
    SAPDB_UInt4                     NumOfMapAreaPages;
    SAPDB_UInt4                     NumOfFreeMapAreaPages;
    SAPDB_UInt4                     NumOfMaps;
    SAPDB_UInt4                     NumOfUnMaps;
} CONS_AWE_INFO_REC;

typedef CONS_AWE_INFO_REC          *PCONS_AWE_INFO_REC;

//
// --- AWE map page info
//
typedef struct cons_awe_map_pageinfo_record
{
    void *                          pMappingPageAddr;
    SAPDB_UInt4                     PhysPageNo;
    SAPDB_UInt8                     Reserved:1,
        MapCount;
} CONS_AWE_MAP_PAGE_INFO_REC;

typedef CONS_AWE_MAP_PAGE_INFO_REC          *PCONS_AWE_MAP_PAGE_INFO_REC;

//
// --- AWE phys page info
//
typedef struct cons_awe_phys_pageinfo_record
{
    SAPDB_UInt8                     Reserved:1,
        MapCount;
} CONS_AWE_PHYS_PAGE_INFO_REC;

typedef CONS_AWE_PHYS_PAGE_INFO_REC          *PCONS_AWE_PHYS_PAGE_INFO_REC;

//
// --- Connection info
//
typedef struct cons_single_connection_info_record
{
    SAPDB_UInt4                     taskIndex;         /* Task index */
    SAPDB_UInt4                     uktIndex;          /* User kernel thread index */
    SAPDB_UInt4                     taskType;          /* Task type (TT_US,...) */
    RTE_TaskDisplayName             displayName;
    SAPDB_UInt4                     connState;         /* Connection state */
    time_t                          connectTime;       /* Connect time in seconds */
    PID                             peerPid;           /* Local connected pid */
    SAPDB_Int8                      remotePid;         /* Remote client pid */
    RTE_Nodename                    clientNode;        /* Client node */
} CONS_SINGLE_CONNECTION_INFO_REC;

typedef CONS_SINGLE_CONNECTION_INFO_REC  *PCONS_SINGLE_CONNECTION_INFO_REC;


typedef struct cons_connection_info_record
{
    SAPDB_Long                      lNumOfConnections;
    SAPDB_ULong                     ulConnectionInfoOffset;   // - CONS_SINGLE_CONNECTION_INFO_REC * 'lNumOfConnections'
} CONS_CONNECTION_INFO_REC;

typedef CONS_CONNECTION_INFO_REC         *PCONS_CONNECTION_INFO_REC;

//
// --- Task info
//
typedef struct cons_single_task_info_record
{
    SAPDB_ULong                     ulTaskIndex;
    TID                             Tid;
    SAPDB_Int8                      ApplPid;            // - process id of connected user procces
    RTE_Nodename                    szClientNode;
    SAPDB_ULong                     ulStateReason;      // - more state infos
    tsp00_Longuint                  ulTaskTimeOut;      // - time for task's timeout if still in VRECEIVE
    REGION_ID                       Sid;
    SAPDB_UInt8                     ulBegExclCnt;
    SAPDB_Long                      lWaitForTask;
    SAPDB_Long                      lWaitForRoot ;
    SAPDB_Long                      lWaitForDevno ;
    tsp2_devnamec                   DevNameWaitingFor;
    UINT1                           TaskState;          // - task state (TSK_RUNNING,...)
    UINT1                           TaskType;           // - task type (TT_US,...)
    RTE_TaskDisplayNameShort        displayName;
    UINT1                           PrioFlag;
    UINT1                           SavePrioFlag;
    UINT1                           QueLocation;
    UINT1                           CancelFlag;         /* CR 1103476 */
    UINT1                           OmsCallbackActive;  /* CR 1106969 */
    UINT1                           UktSleepingFlag;    /* CR 1115172 */
    tsp00_PageNo                    LockedPageNo;       /* CR 1115172 */
    SAPDB_ULong                     ulUktIndex;         /* CR 1115172 */
    SAPDB_UInt8                     UktSleepCount;      /* CR 1115172 */
    SAPDB_Long                      WaitTime;           /* CR 1115172 */
    SAPDB_ULong                     ulDynPrio;          /* CR 1115172 */
    SAPDB_Char                      NameOfSpinlockYieldingFor[20];
    UINT1                           WaitingForExclusiveAccess;
} CONS_SINGLE_TASK_INFO_REC;

typedef CONS_SINGLE_TASK_INFO_REC *PCONS_SINGLE_TASK_INFO_REC;


typedef struct cons_task_info_record
{
    UINT1                           StateToShow ;
    UINT1                           TypeToShow ;
    SAPDB_Long                      lNumOfActiveTasks;
    SAPDB_ULong                     ulTaskInfoOffset;        // - CONS_SINGLE_TASK_INFO_REC * 'MAX_TASKS'
} CONS_TASK_INFO_REC;

typedef CONS_TASK_INFO_REC        *PCONS_TASK_INFO_REC;

#endif

/*!
  --- TIME ENABLE / DISABLE
 */
typedef struct cons_time_measure_record
{
    SAPDB_Bool                      dummy;
} CONS_TIME_MEASURE_REC;

typedef CONS_TIME_MEASURE_REC     *PCONS_TIME_MEASURE_REC;

/*!
 --- Debug Task
 */
typedef struct cons_debug_task_record
{
    SAPDB_UInt2                     usReturnCode;
    SAPDB_ULong                     ulTask;
} CONS_DEBUG_TASK_REC;

typedef CONS_DEBUG_TASK_REC *PCONS_DEBUG_TASK_REC;

/*!
 --- Task info
 */
typedef struct cons_single_task_move_info_record
{
    UINT1                           TaskState;          // - task state (TSK_RUNNING,...)
    SAPDB_ULong                     ulStateReason;      // - more state infos
    SAPDB_ULong                     ulTaskIndex;
    SAPDB_ULong                     ulUktIndex;         
    UINT1                           TaskType;           // - task type (TT_US,...)
    RTE_TaskDisplayNameShort        displayName;
    SAPDB_UInt8                     RunnableMicroSeconds;
    SAPDB_UInt8                     MaxRunnableMicroSeconds;
    SAPDB_UInt2                     TemporaryMoveLock;    
    SAPDB_UInt2                     LongTermMoveLock;  
} CONS_SINGLE_TASK_MOVE_INFO_REC;

typedef CONS_SINGLE_TASK_MOVE_INFO_REC *PCONS_SINGLE_TASK_MOVE_INFO_REC;


/*!
 --- Task move info
 */
typedef struct cons_task_move_info_record
{
    SAPDB_Long                      lNumOfActiveTasks;
    SAPDB_ULong                     ulTaskMoveInfoOffset;        // - CONS_SINGLE_TASK_MOVE_INFO_REC * 'MAX_TASKS'
} CONS_TASK_MOVE_INFO_REC;

typedef CONS_TASK_MOVE_INFO_REC        *PCONS_TASK_MOVE_INFO_REC;


/*!
 --- Load balancing
 */

typedef struct cons_last_moves_entry
{
    SAPDB_UInt8                     MoveTime;    
    SAPDB_ULong                     ulTaskIndex;
    SAPDB_UInt8                     TaskMaxRunnableMicroSeconds;
    SAPDB_ULong                     ulSrcUKTIndex;
    SAPDB_UInt8                     SrcMaxRunnableMicroSeconds;
    SAPDB_ULong                     ulDstUKTIndex;
    SAPDB_UInt8                     DstMaxRunnableMicroSeconds;
} CONS_LAST_MOVES_ENTRY;

typedef CONS_LAST_MOVES_ENTRY         *PCONS_LAST_MOVES_ENTRY;


/*!
 --- Single UKT move info
 */
typedef struct cons_single_ukt_move_info_record
{
    SAPDB_ULong                     ulUKTIndex;
    SAPDB_ULong                     ulMoveTaskToUKTUKTIndex;
    SAPDB_UInt4                     NumOfMovableTasks;
    SAPDB_UInt8                     TaskMoveIntervalStartTime;
    SAPDB_UInt8                     UKTSleepTimeMicroSeconds;
    SAPDB_UInt8                     RunnableMicroSeconds;
    SAPDB_UInt8                     MaxRunnableMicroSeconds;

} CONS_SINGLE_UKT_MOVE_INFO_REC;

typedef CONS_SINGLE_UKT_MOVE_INFO_REC         *PCONS_SINGLE_UKT_MOVE_INFO_REC;

/*!
 --- complete move info
 */
typedef struct cons_move_info_record
{
    SAPDB_Int4                      CheckIntervalTime;
    SAPDB_UInt8                     TaskMoveIntervalStartTime;
    SAPDB_UInt2                     TaskMoveMeasIntervals;
    SAPDB_UInt2                     MinimumDiff;
    SAPDB_UInt2                     ValueEquality;
    SAPDB_Long                      lNumOfLastRecs;
    SAPDB_ULong                     ulLastMovesInfoOffset;   // - CONS_LAST_MOVES_ENTRY * 'lNumOfLastRecs'
    SAPDB_Long                      lNumOfUKTs;
    SAPDB_ULong                     ulUKTInfoOffset;         // - CONS_SINGLE_UKT_MOVE_INFO_REC * 'lNumOfUKTs'
} CONS_MOVE_INFO_REC;

typedef CONS_MOVE_INFO_REC         *PCONS_MOVE_INFO_REC;

/*!
 --- check interval
 */
typedef struct cons_check_interval_record
{
    SAPDB_UInt4                     usReturnCode;
    SAPDB_Int4                      CheckIntervalTime;
} CONS_CHECK_INTERVAL_RECORD;

/*!
  show RWRegions
 */

typedef struct cons_rwregion_info_record
{
    union
    {
        struct
        {
            SAPDB_ULong ulInfoNum;
            SAPDB_ULong numberOfSpinlockPools;
        }
        outParams;
        SAPDB_Bool  showAll;
    } info;
} CONS_RWREGION_INFO_REC;

typedef CONS_RWREGION_INFO_REC        *PCONS_RWREGION_INFO_REC;

/*!
  show single SpinlockPool
 */
typedef struct cons_single_spinlockpool_info_record
{
    SAPDB_UTF8  m_Name[40+1];
} CONS_SINGLE_SPINLOCKPOOL_INFO_REC;

typedef CONS_SINGLE_SPINLOCKPOOL_INFO_REC        *PCONS_SINGLE_SPINLOCKPOOL_INFO_REC;

/*!
  show single RWRegions
 */
typedef struct cons_single_rwregion_info_record
{
    void        *pAddress;
    /*! RWRegion access types */
    enum RWRegionAccess
    {
        none=0,
        holdingExclusive=1,
        holdingNonExclusive=2,
        waitingForExclusive=3,
        waitingForNonExclusive=4
    }
    access;
    RTE_TaskId  taskID;
    SAPDB_UInt4 collisionCounter;
    SAPDB_UInt4 exclusiveAccessCounter;
    SAPDB_UInt4 nonExclusiveAccessCounter;
    SAPDB_UInt4 waitCounter;
    SAPDB_UInt4 tasCounter;
    SAPDB_UInt4 spinlockPoolIndex;
} CONS_SINGLE_RWREGION_INFO_REC;

typedef CONS_SINGLE_RWREGION_INFO_REC        *PCONS_SINGLE_RWREGION_INFO_REC;

/*!
 --- Setting trace level for console thread
 */

typedef struct cons_set_tracelevel
{
    SAPDB_UInt4                     level;
} CONS_SET_TRACELEVEL;


/*!
 --- Union of all request records
 */
typedef union console_data_record
{
#if defined (_WIN32)
    CONS_CANCEL_TASK_REC            CancelTaskCtrl;
    CONS_KILL_TASK_REC              KillTaskCtrl;
    CONS_SET_DEBUGLEVEL_REC         SetDebugLevelCtrl;
    CONS_GETPARAM_REC               GetParamInfo;
    CONS_SET_UKT_AFFINITY_REC       UKTAffinity;
    CONS_FORCE_IO_ERROR_REC         ForceIOError;
    CONS_DEBUG_BREAK_REC            DebugBreak;
    CONS_SET_PROCESSOR_MASK_REC     SetProcessorMask;
    CONS_STATE_INFO_REC             StateInfo;
    CONS_SHOW_SUSPEND_REC           SuspendReasons;
    CONS_TASK_COUNTS_INFO_REC       TaskCountsInfo;  
    CONS_TOTAL_COUNTS_INFO_REC      TotalCountsInfo;
    CONS_TASK_INFO_REC              TaskInfo;
#endif
    CONS_DEBUG_TASK_REC             DebugTask;
    CONS_TASK_MOVE_INFO_REC         TaskMoveInfo;
    CONS_CHECK_INTERVAL_RECORD      SetCheckInterval;
    CONS_SET_TRACELEVEL             SetTraceLevel;
    CONS_RWREGION_INFO_REC          RWRegionInfo;
    //only for TEST!!!!!
    SAPDB_Byte                      test01String[5];
} CONSOLE_DATA_REC;

typedef CONSOLE_DATA_REC          *PCONSOLE_DATA_REC;


/*!
  standard data header
 */
typedef struct
{
    SAPDB_UInt4                     type;       /* CON_REQ_... */
    SAPDB_Bool                      lastSend;
    SAPDB_UInt4                     dataLen;
    SAPDB_UInt4                     rc;
} RTE_ConsoleStandardDataHeader;

/*!
  standard error packet
 */
typedef struct
{
    RTE_ConsoleStandardDataHeader  header;
    SAPDB_Byte                     *messageText; 
} RTE_ConsoleStandardErrorPacket;

/*!
  standard data packet
 */
typedef struct
{
    RTE_ConsoleStandardDataHeader  header;
    CONSOLE_DATA_REC               data; 
} RTE_ConsoleStandardDataPacket;

//************************************************
/*! Test Packet string  */
typedef SAPDB_Byte          TestLongString[4*4096];        
//************************************************

/*!
  test data packet
 */
typedef struct
{
    RTE_ConsoleStandardDataHeader  header;
    TestLongString                 data; 
} RTE_ConsoleTestDataPacket;

/*!
  connect data packet
 */
typedef struct
{
    RTE_ConsoleStandardDataHeader  header;
    RTE_ConsoleHandle              hConnect; 
} RTE_ConsoleStandardConnectPacket;

#endif  /* RTE_CONSOLESTANDARDDATAREQUESTS_H */
