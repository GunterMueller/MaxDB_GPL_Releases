/*!
  @file           geo002.h
  @author         JoergM
  @special area   XParam
  @brief          Parameter names and structure used inside LZU for parameter access
  @see            example.html ...
  @first created  000-11-22  15:19

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



#ifndef GEO002_H
#define GEO002_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef XPARAM_NAMES_ONLY
#  include "gsp00.h"
#  include "vsp002.h"
#  include "geo00_0.h"
#  include "RunTime/RTE_Types.h"
#endif /* XPARAM_NAMES_ONLY */

#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define PAN_AK_DUMP_ALLOWED           (SAPDB_Char *)KERNELPARAM__AK_DUMP_ALLOWED
#define PAN_ALLOW_DEMANGLE            (SAPDB_Char *)KERNELPARAM_ALLOW_DEMANGLE
#define PAN_ARCHIVE_LOG_N             (SAPDB_Char *)KERNELPARAM_LOG_VOLUME_NAME_
#define         PAN_ARCHIVE_LOG_N_LN          16
#define PAN_ARCHIVE_LOG_N_OLD         (SAPDB_Char *)KERNELPARAM_ARCHIVE_LOG_
#define PAN_ARCHIVE_LOG_N_TYPE        (SAPDB_Char *)KERNELPARAM_LOG_VOLUME_TYPE_
#define PAN_ARCHIVE_LOG_N_TYPE_OLD    (SAPDB_Char *)KERNELPARAM_ARCHIVE_TYPE_
#define PAN_ARCHIVE_LOG_N_SIZE        (SAPDB_Char *)KERNELPARAM_LOG_VOLUME_SIZE_
#define         PAN_ARCHIVE_LOG_N_SIZE_LN     16
#define PAN_ARCHIVE_LOG_N_SIZE_OLD    (SAPDB_Char *)KERNELPARAM_ARCHIVE_SIZE_
#define PAN_BACKUP_BLOCK_CNT          (SAPDB_Char *)KERNELPARAM_BACKUP_BLOCK_CNT
#define PAN_BACKUP_HISTFILE           (SAPDB_Char *)KERNELPARAM__BACKUP_HISTFILE
#define PAN_BACKUP_MED_DEF            (SAPDB_Char *)KERNELPARAM__BACKUP_MED_DEF
#define PAN_CACHE_SIZE                (SAPDB_Char *)KERNELPARAM_CACHE_SIZE
#define PAN_CATALOG_CACHE             (SAPDB_Char *)KERNELPARAM_CAT_CACHE_SUPPLY
#define PAN_COMMENT                   (SAPDB_Char *)KERNELPARAM__COMMENT
#define PAN_CONTROLID                 (SAPDB_Char *)KERNELPARAM_CONTROLUSERID
#define PAN_CONTROLPW                 (SAPDB_Char *)KERNELPARAM_CONTROLPASSWORD
#define PAN_CONV_CACHE_PAGES          (SAPDB_Char *)KERNELPARAM_CONVERTER_CACHE
#define PAN_CONVERTER_REGIONS         (SAPDB_Char *)KERNELPARAM_CONVERTER_REGIONS
#define PAN_CRASH_HISTORY_PATH        (SAPDB_Char *)KERNELPARAM_DIAG_HISTORY_PATH
#define PAN_CRASH_HISTORY_NUM         (SAPDB_Char *)KERNELPARAM_DIAG_HISTORY_NUM
#define PAN_CRASH_SEM                 (SAPDB_Char *)KERNELPARAM__DIAG_SEM
#define PAN_DATA_CACHE_PAGS           (SAPDB_Char *)KERNELPARAM_DATA_CACHE
#define PAN_DATA_VOLUME_MODE_N        (SAPDB_Char *)KERNELPARAM_DATA_VOLUME_MODE_
#define         PAN_DATA_VOLUME_MODE_N_LN     17
#define PAN_DATACACHE_REGIONS         (SAPDB_Char *)KERNELPARAM__DATA_CACHE_RGNS
#define PAN_DATADEV_N                 (SAPDB_Char *)KERNELPARAM_DATA_VOLUME_NAME_
#define         PAN_DATADEV_N_LN              17
#define PAN_DATADEV_N_OLD             (SAPDB_Char *)KERNELPARAM_DATADEV_
#define PAN_DATADEV_N_TYPE            (SAPDB_Char *)KERNELPARAM_DATA_VOLUME_TYPE_
#define PAN_DATADEV_N_TYPE_OLD        (SAPDB_Char *)KERNELPARAM_DATA_TYPE_
#define PAN_DATADEV_N_SIZE            (SAPDB_Char *)KERNELPARAM_DATA_VOLUME_SIZE_
#define         PAN_DATADEV_N_SIZE_LN         17
#define PAN_DATADEV_N_SIZE_OLD        (SAPDB_Char *)KERNELPARAM_DATA_SIZE_
#define PAN_DELAY_COMMIT              (SAPDB_Char *)KERNELPARAM__DELAY_COMMIT
#define PAN_DELAY_LW                  (SAPDB_Char *)KERNELPARAM__DELAY_LOGWRITER
#define PAN_DEVNO_BIT_COUNT           (SAPDB_Char *)KERNELPARAM_VOLUMENO_BIT_COUNT
#define PAN_DEVNO_BIT_COUNT_OLD       (SAPDB_Char *)KERNELPARAM_DEVNO_BIT_COUNT
#define PAN_DEV_QUEUE                 (SAPDB_Char *)KERNELPARAM__IOPROCS_SWITCH
#define PAN_DIAG_SIZE                 (SAPDB_Char *)KERNELPARAM_KERNELDIAGSIZE
#define PAN_EVENTFILE                 (SAPDB_Char *)KERNELPARAM__EVENTFILE
#define PAN_EVENT_SIZE                (SAPDB_Char *)KERNELPARAM__EVENTSIZE
#define PAN_EXPAND_COM_TRACE          (SAPDB_Char *)KERNELPARAM_EXPAND_COM_TRACE
#define PAN_FBM_DEVSP_BALANCE         (SAPDB_Char *)KERNELPARAM_FBM_VOLUME_BALANCE
#define PAN_FBM_DEVSP_BALANCE_OLD     (SAPDB_Char *)KERNELPARAM__FBM_DEVSP_BALANCE
#define PAN_FBM_DEVSP_COMPR           (SAPDB_Char *)KERNELPARAM_FBM_VOLUME_COMPRESSION
#define PAN_FBM_DEVSP_COMPR_OLD       (SAPDB_Char *)KERNELPARAM__FBM_DEVSP_COMPR
#define PAN_FORMAT_DATAVOLUME         (SAPDB_Char *)KERNELPARAM_FORMAT_DATAVOLUME
#define PAN_FORMATTING_MODE           (SAPDB_Char *)KERNELPARAM_FORMATTING_MODE
#define PAN_GATEWAYTYPE               (SAPDB_Char *)KERNELPARAM_GATEWAYTYPE
#define PAN_HEAP_MEM_POOL             (SAPDB_Char *)KERNELPARAM_HEAP_MEM_POOL
#define PAN_HS_DELAY_TIME_N            (SAPDB_Char *)KERNELPARAM_HS_DELAY_TIME_
#define         PAN_HS_DELAY_TIME_N_LN         14
#define         PAN_HS_DELAY_TIME_DIGITS       3
#define PAN_HS_NODE_N                 (SAPDB_Char *)KERNELPARAM_HS_NODE_
#define         PAN_HS_NODE_N_LN              8
#define PAN_HS_STORAGE_DLL            (SAPDB_Char *)KERNELPARAM_HS_STORAGE_DLL
#define PAN_INSTANCE_TYPE             (SAPDB_Char *)KERNELPARAM_INSTANCE_TYPE
#define PAN_IOTHREADS_FOR_PRIO        (SAPDB_Char *)KERNELPARAM__IOPROCS_FOR_PRIO
#define PAN_KERNELDUMPFILE            (SAPDB_Char *)KERNELPARAM__KERNELDUMPFILE
#define PAN_KERNELTRACEFILE           (SAPDB_Char *)KERNELPARAM__KERNELTRACEFILE
#define PAN_KERNELTRACESIZE           (SAPDB_Char *)KERNELPARAM_KERNELTRACESIZE
#define PAN_LOAD_BALANCING_CHK        (SAPDB_Char *)KERNELPARAM_LOAD_BALANCING_CHK
#define PAN_LOAD_BALANCING_DIFF       (SAPDB_Char *)KERNELPARAM_LOAD_BALANCING_DIF
#define PAN_LOAD_BALANCING_EQUALITY   (SAPDB_Char *)KERNELPARAM_LOAD_BALANCING_EQ
#define PAN_LOG_MIRRORED              (SAPDB_Char *)KERNELPARAM_LOG_MIRRORED
#define PAN_LOG_MODE                  (SAPDB_Char *)KERNELPARAM_LOG_MODE
#define PAN_M_ARCHIVE_LOG_N           (SAPDB_Char *)KERNELPARAM_M_LOG_VOLUME_NAME_
#define         PAN_M_ARCHIVE_LOG_N_LN        18
#define PAN_M_ARCHIVE_LOG_N_TYPE      (SAPDB_Char *)KERNELPARAM_M_LOG_VOLUME_TYPE_
#define PAN_M_DATADEV_N               (SAPDB_Char *)KERNELPARAM_M_DATA_VOLUME_NAME_
#define         PAN_M_DATADEV_N_LN            19
#define PAN_M_SYS_N                   (SAPDB_Char *)KERNELPARAM_M_SYSDEV_
#define         PAN_M_SYS_N_LN                9
#define PAN_MAX_BACKUP_TASKS          (SAPDB_Char *)KERNELPARAM__MAX_BACKUP_TASKS
#define PAN_MAX_MESSAGE_FILES         (SAPDB_Char *)KERNELPARAM__MAX_MESSAGE_FILES
#define PAN_MAXACTIVE                 (SAPDB_Char *)KERNELPARAM_MAXUSERTASKS
#define PAN_MAXARCHIVELOGS            (SAPDB_Char *)KERNELPARAM_MAXLOGVOLUMES
#define PAN_MAXARCHIVELOGS_OLD        (SAPDB_Char *)KERNELPARAM_MAXARCHIVELOGS
#define PAN_MAXBACKUPDEVS             (SAPDB_Char *)KERNELPARAM_MAXBACKUPDEVS
#define PAN_MAXCPU                    (SAPDB_Char *)KERNELPARAM_MAXCPU
#define PAN_MAXDATADEVSPACES          (SAPDB_Char *)KERNELPARAM_MAXDATAVOLUMES
#define PAN_MAXDATADEVSPACES_OLD      (SAPDB_Char *)KERNELPARAM_MAXDATADEVSPACES
#define PAN_MAXDATAWRITER             (SAPDB_Char *)KERNELPARAM_MAXPAGER
#define PAN_MAXDATAWRITER_OLD         (SAPDB_Char *)KERNELPARAM__MAXDATAWRITER
#define PAN_MAXDEVSPACES              (SAPDB_Char *)KERNELPARAM_MAXVOLUMES
#define PAN_MAXDEVSPACES_OLD          (SAPDB_Char *)KERNELPARAM__MAXDEVSPACES
#define PAN_MAXEVENTS                 (SAPDB_Char *)KERNELPARAM__MAXEVENTS
#define PAN_MAXEVENTTASKS             (SAPDB_Char *)KERNELPARAM__MAXEVENTTASKS
#define PAN_MAXGARBAGE_COLLECTOR      (SAPDB_Char *)KERNELPARAM__MAXGARBAGE_COLL
#define PAN_MAXIOTHREADS              (SAPDB_Char *)KERNELPARAM__IOPROCS_PER_DEV
#define PAN_MAXRGN_REQUEST            (SAPDB_Char *)KERNELPARAM_MAXRGN_REQUEST
#define PAN_MAXSERVER                 (SAPDB_Char *)KERNELPARAM_MAXSERVERTASKS
#define PAN_MAXSYSDEVSPACES           (SAPDB_Char *)KERNELPARAM__MAXSYSDEVSPACES
#define PAN_MAXUSERSTACK              (SAPDB_Char *)KERNELPARAM__MAXTASK_STACK
#define PAN_MEMORY_SLOT_SIZE          (SAPDB_Char *)KERNELPARAM_MEMORY_SLOT_SIZE
#define PAN_MIN_REPLY_SIZE            (SAPDB_Char *)KERNELPARAM__MINREPLY_SIZE
#define PAN_MIRR_DATA                 (SAPDB_Char *)KERNELPARAM__MIRRORED_DATA
#define PAN_MMAP_MEM_POOL             (SAPDB_Char *)KERNELPARAM_MMAP_MEM_POOL
#define PAN_MP_DISP_LOOPS             (SAPDB_Char *)KERNELPARAM__MP_DISP_LOOPS
#define PAN_MP_DISP_PRIO              (SAPDB_Char *)KERNELPARAM__MP_DISP_PRIO
#define PAN_MP_RGN_BUSY_WAIT          (SAPDB_Char *)KERNELPARAM__MP_RGN_BUSY_WAIT
#define PAN_MP_RGN_DIRTY_READ         (SAPDB_Char *)KERNELPARAM__MP_RGN_DIRTY_READ
#define PAN_MP_RGN_LOOP               (SAPDB_Char *)KERNELPARAM_MP_RGN_LOOP
#define PAN_MP_RGN_PRIO               (SAPDB_Char *)KERNELPARAM__MP_RGN_PRIO
#define PAN_MP_RGN_QUEUE              (SAPDB_Char *)KERNELPARAM__MP_RGN_QUEUE
#define PAN_OFFICIAL_NODE             (SAPDB_Char *)KERNELPARAM_OFFICIAL_NODE
#define PAN_OMS_HEAP_LIMIT            (SAPDB_Char *)KERNELPARAM_OMS_HEAP_LIMIT
#define PAN_OMS_REGIONS               (SAPDB_Char *)KERNELPARAM__OMS_RGNS
#define PAN_PACKET_SIZE               (SAPDB_Char *)KERNELPARAM__PACKET_SIZE
#define PAN_PAGE_SIZE                 (SAPDB_Char *)KERNELPARAM__PAGE_SIZE
#define PAN_PARAM_KERNELVERSION       (SAPDB_Char *)KERNELPARAM_KERNELVERSION
#define PAN_PARAM_VERSION_TIME        (SAPDB_Char *)KERNELPARAM___PARAM_VERIFIED__
#define PAN_PRIO_BASE_COM             (SAPDB_Char *)KERNELPARAM__PRIO_BASE_COM
#define PAN_PRIO_BASE_IOC             (SAPDB_Char *)KERNELPARAM__PRIO_BASE_IOC
#define PAN_PRIO_BASE_RAV             (SAPDB_Char *)KERNELPARAM__PRIO_BASE_RAV
#define PAN_PRIO_BASE_REX             (SAPDB_Char *)KERNELPARAM__PRIO_BASE_REX
#define PAN_PRIO_BASE_U2U             (SAPDB_Char *)KERNELPARAM__PRIO_BASE_U2U
#define PAN_PRIO_FACTOR               (SAPDB_Char *)KERNELPARAM__PRIO_FACTOR
#define PAN_ROW_REGIONS               (SAPDB_Char *)KERNELPARAM__ROW_RGNS
#define PAN_RTEDUMPFILE               (SAPDB_Char *)KERNELPARAM__RTEDUMPFILE
#define PAN_RUNDIR                    (SAPDB_Char *)KERNELPARAM_RUNDIRECTORY
#define PAN_SERVERDB                  (SAPDB_Char *)KERNELPARAM_SERVERDB
#define PAN_SET_VOLUME_LOCK           (SAPDB_Char *)KERNELPARAM_SET_VOLUME_LOCK
#define PAN_SHM_MEM_POOL              (SAPDB_Char *)KERNELPARAM_SHM_MEM_POOL
#define PAN_SHOW_MAX_STACK_USE        (SAPDB_Char *)KERNELPARAM_SHOW_MAX_STACK_USE
#define PAN_SUPPRESS_CORE             (SAPDB_Char *)KERNELPARAM_SUPPRESS_CORE
#define PAN_SYS_N                     (SAPDB_Char *)KERNELPARAM_SYSDEV_
#define         PAN_SYS_N_LN                  7
#define PAN_SYS_N_SIZE                (SAPDB_Char *)KERNELPARAM_SYS_SIZE_
#define         PAN_SYS_N_SIZE_LN             8
#define PAN_TAB_REGIONS               (SAPDB_Char *)KERNELPARAM__TAB_RGNS
#define PAN_TASK_STACK_AREA           (SAPDB_Char *)KERNELPARAM__TASK_STACK_AREA
#define PAN_TASKCLUSTER_N             (SAPDB_Char *)KERNELPARAM__TASKCLUSTER_
#define         PAN_TASKCLUSTER_N_LN          13
#define PAN_TRACE_PAGES_AL            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_LW
#define PAN_TRACE_PAGES_AL_OLD        (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_AL
#define PAN_TRACE_PAGES_BUP           (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_BUP
#define PAN_TRACE_PAGES_DW            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_PG
#define PAN_TRACE_PAGES_DW_OLD        (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_DW
#define PAN_TRACE_PAGES_EV            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_EV
#define PAN_TRACE_PAGES_GC            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_GC
#define PAN_TRACE_PAGES_SV            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_SV
#define PAN_TRACE_PAGES_TI            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_TI
#define PAN_TRACE_PAGES_TW            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_TW
#define PAN_TRACE_PAGES_US            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_US
#define PAN_TRACE_PAGES_UT            (SAPDB_Char *)KERNELPARAM_TRACE_PAGES_UT
#define PAN_TRANS_REGIONS             (SAPDB_Char *)KERNELPARAM__TRANS_RGNS
#define PAN_TREE_REGIONS              (SAPDB_Char *)KERNELPARAM__TREE_RGNS
#define PAN_UNICODE_CATALOG           (SAPDB_Char *)KERNELPARAM__UNICODE
#define PAN_USE_ASYNC_IO              (SAPDB_Char *)KERNELPARAM__USE_ASYNC_IO
#define PAN_USE_IOTHREAD_ONLY         (SAPDB_Char *)KERNELPARAM__USE_IOPROCS_ONLY
#define PAN_USE_OPEN_DIRECT           (SAPDB_Char *)KERNELPARAM_USE_OPEN_DIRECT
#define PAN_USE_PROC_SCOPE            (SAPDB_Char *)KERNELPARAM__USE_PROC_SCOPE
#define PAN_UTILITY_PROTFILE          (SAPDB_Char *)KERNELPARAM__UTILITY_PROTFILE
#define PAN_UTILITY_PROTSIZE          (SAPDB_Char *)KERNELPARAM_UTILITY_PROTSIZE
#define PAN_VALLOC_MEM_POOL           (SAPDB_Char *)KERNELPARAM_VALLOC_MEM_POOL
#define PAN_XPARAM_CHANGED            (SAPDB_Char *)KERNELPARAM___PARAM_CHANGED___
#define PAN_PREALLOCATE_IOWORKER      (SAPDB_Char *)KERNELPARAM_PREALLOCATE_IOWORKER
/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
#define PAN_ALLOW_COMPR_DEVSP         (SAPDB_Char *)KERNELPARAM_ALLOW_COMPR_DEVSP
#define PAN_DYNAMIC_SERVER            (SAPDB_Char *)KERNELPARAM_DYNAMIC_SERVER
#define PAN_HIRES_TIMER_TYPE          (SAPDB_Char *)KERNELPARAM_HIRES_TIMER_TYPE
#define PAN_MEM_ENHANCEMENT_LIM       (SAPDB_Char *)KERNELPARAM_MEM_ENHANCE_LIMIT
#define PAN_RESTART_SHUTDOWN          (SAPDB_Char *)KERNELPARAM_RESTART_SHUTDOWN
#define PAN_USE_MEM_ENHANCEMENT       (SAPDB_Char *)KERNELPARAM_USE_MEM_ENHANCE
#define PAN_USE_FIBERS                (SAPDB_Char *)KERNELPARAM_USE_FIBERS
/***************************************************************/
#else
/***************************************************************/
#define PAN_OPMSG1                    (SAPDB_Char *)KERNELPARAM_OPMSG1
#define PAN_OPMSG2                    (SAPDB_Char *)KERNELPARAM_OPMSG2
#define PAN_SHMCHUNK                  (SAPDB_Char *)KERNELPARAM__SHMCHUNK
#define PAN_SHMKERNEL                 (SAPDB_Char *)KERNELPARAM__SHMKERNEL
#define PAN_USE_COROUTINES            (SAPDB_Char *)KERNELPARAM_USE_COROUTINES
#define PAN_USE_STACK_ON_STACK        (SAPDB_Char *)KERNELPARAM_USE_STACK_ON_STACK
#define PAN_USE_UCONTEXT              (SAPDB_Char *)KERNELPARAM_USE_UCONTEXT
/***************************************************************/
#endif
/***************************************************************/

#ifndef XPARAM_NAMES_ONLY

#define DEVSPACE_UNUSED  -1

#define NUM_DEVSP_TYPES 8

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(_WIN32)
# define XPARAM(_param_elem) (kgs.XParam->_param_elem)
#else /* UNIX */
# define XPARAM(_param_elem) (kgs->XParam->_param_elem)
#endif

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if !defined(_WIN32)
typedef enum _RTE_SimulateVectorIo
{
    RTE_SimVioUnknown = 0,
    RTE_SimVioNever   = 1,
    RTE_SimVioIfOpenDirect = 2,
    RTE_SimVioIfRawDevice = 3,
    RTE_SimVioIfOpenDirectOrRawDevice = 4,
    RTE_SimVioAlways = 5
} RTE_SimulateVectorIo;
#endif

/* --- RTE_XPARAM_REC ------------------------------------------------------ */

typedef struct  dev_descr
  {
    tsp2_devnamec               szDevspaceName;
    tsp00_Int                   lDevspaceSize;
    tsp00_Int                   lDevspaceType;
    RTE_DevID                   physicalDevID; /* 1109850 */
    RTE_VolumeAccessMode        accessMode;
    RTE_FileSystemAccessType    fileSystemAccessType; /* for DAFS support PTS 1124541 */
    SAPDB_Bool                  sizeLimitedByUlimit;
  } DEV_DESCR;

typedef struct  devspaces_descr
  {
    tsp00_Uint                ulNumDevs;
    DEV_DESCR                 *Devspaces;
  } DEVSPACES_DESCR;

typedef struct  rte_xparam_record
{
  tsp00_VFilenamec            szRunDirectory;       /*!< working directory of kernel */
  tsp00_Uint                  ulDBPageSize;         /*!< size of database page in bytes */
  tsp00_Uint                  ulMaxUsUtStack;       /*!< size of user/utility task stack in bytes */
  tsp00_C40c                  szKernelVersion;      /*!< kernel version string */

  tsp00_Uint                  ulDiagSize;           /*!< size of knldiag file */

  tsp00_VFilenamec            szEventFile;          /*!< event file */
  tsp00_Uint                  ulEventSize;          /*!< size of event file */
  tsp00_Uint                  ulMaxEvents;          /*!< maximum number of events queued */

  tsp2_devnamec               szTraceDevspace;      /*!< name of trace volume */
  tsp00_Int                   lTraceDevspaceSize;   /*!< size of trace volume */
  tsp00_VFilenamec            szDumpFilename;       /*!< knldump file */

  tsp00_VFilenamec            szUtilityProtFile;    /*!< utility protocol file */
  tsp00_Uint                  ulUtilityProtSize;    /*!< size of utility protocol file */

  tsp00_VFilenamec            szBackupHistFile;     /*!< backup history file */
  tsp00_VFilenamec            szBackupMedDefFile;   /*!< backup medium definition file */

  tsp00_VFilenamec            szRTEDumpFilename;    /*!< rtedump file */

  tsp00_Uint                  ulMaxMessageFiles;    /*!< maximum number of message files */

  tsp00_Int                   lNoOfBackupDevs;      /*!< maximum number of parallel backup devices */
  tsp00_Int                   lMaxDevspaces;        /*!< maximum number of volumes configured */
  tsp00_Int                   lDeviPerDevspace;     /*!< volume worker threads per volume */
  tsp00_Int                   lPrioDeviPerDevspace; /*!< additional volume worker threads reserved for priorized tasks */
  tsp00_Int                   lDevQueue;            /*!< number of requests to collect before switching to next device queue */
  tsp00_Int                   lReaderDeviPerDevspace; /*!< additional volume worker threads reserved for reading tasks */
  tsp00_Uint                  ulMaxCPU;             /*!< configured number of cpus to use */
  tsp00_Bool                  fAllowSingleIO;       /*!< allow direct I/O by task thread */
  tsp00_Bool                  fUseAsyncIO;          /*!< use asyncronous I/O requests */

  /* task clustering */
  char                        **TaskClusterArray;   /*!< array ?? of strings filled with task cluster */
  tsp00_Uint                  ulMaxDataWriter;
  tsp00_Uint                  ulMaxEventTasks;
  tsp00_Uint                  ulMaxBackupTasks;
  tsp00_Uint                  ulMaxGarbageCollector;
  tsp00_Uint                  ulMaxUser;
  tsp00_Uint                  ulMaxServer;

  /* region parameters */
  tsp00_Uint                  ulNoOfRegions;        /*!< number of regions */
  tsp00_Uint                  ulRegionCollisionLoop;/*!< vbegexcl */
  tsp00_Uint                  ulRegLockSlice;       /*!< vbegexcl */
  tsp00_Bool                  fBegExclQueWhenPrio;  /*!< vbegexcl */
  tsp00_Bool                  fBegExclDirtyRead;    /*!< vbegexcl */
  tsp00_Bool                  fEndExclBusyWait;     /*!< vbegexcl */
  tsp00_Bool                  fExclQueue;           /*!< vbegexcl */

  /* dispatching parameter */
  tsp00_Uint                  ulDispLoopsBefSemop;
  tsp00_Int                   lPrioBaseU2U;
  tsp00_Int                   lPrioBaseRAV;
  tsp00_Int                   lPrioBaseIOC;
  tsp00_Int                   lPrioBaseCOM;
  tsp00_Int                   lPrioBaseREX;
  tsp00_Int                   lPrioFactor;
  tsp00_Int                   lDelayLW;
  tsp00_Uint                  uLoadBalancingCheck;       /* PTS 1117672 */
  tsp00_Uint                  uLoadBalancingDiff;        /* PTS 1117672 */
  tsp00_Uint                  uLoadBalancingEquality;    /* PTS 1117672 */
  tsp00_Bool                  fDelayCommit;
  tsp00_Bool                  fDynDispQueSrch;

  DEVSPACES_DESCR             DevspDescr   [NUM_DEVSP_TYPES];
  tsp00_Bool                  fMirrData;
  tsp00_Bool                  fFormatDataVolume;
  tsp00_Uint                  ulDataIOBlockCount;

  tsp00_Bool                  fAKDumpAllowed;
  tsp00_Uint                  uCrashHistoryNum;
  tsp00_VFilenamec            szCrashHistoryPath;

  tsp00_Uint                  ulPacketSize;
  tsp00_Uint                  ulMinReplySize;

  tsp00_Uint                  ulXparamChanged;

  tsp00_Uint                  ulOMSHeapLimit;
  tsp00_Bool                  fTimeMeasure;
  tsp00_Bool                  preallocateIOWorker;
/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
  tsp00_Bool                  fUseMemEnhancement;
  tsp00_Uint                  lMemEnhancementLimit;
  tsp00_Bool                  fDynamicServerTasks;
  tsp00_Bool                  fAllowCompressedDevsp;
  tsp00_Bool                  fDummy;                   /* PTS 1125102 was fAutoRestartShutdown; */
  tsp00_Bool                  fProfileDCOMObject;       /* CR 1000241 */
  tsp00_Bool                  fUseFibers;
/***************************************************************/
#else
/***************************************************************/
  tsp00_VFilenamec            szOpMsg1;
  tsp00_VFilenamec            szOpMsg2;
  tsp00_Int                   lShmKernel;
  tsp00_Bool                  fUseProcessScope;          /* PTS 1109046 */
  tsp00_Bool                  suppressCore;
  tsp00_Bool                  fUseCoroutines;
  tsp00_Bool                  fUseStackOnStack;
  tsp00_Bool                  fUseUcontext;
  tsp00_C32c                  szVectorIoMode;            /* PTS 1132876 */
  RTE_SimulateVectorIo        simulateVectorIo;          /* see RTE_SimulateVectorIo above PTS 1132876 */
/***************************************************************/
#endif
/***************************************************************/
#if defined(_WIN32) || defined(LINUX)
  tsp00_C16c                  szHiResTimerType;         /* PTS 1117672 */
#endif
  tsp00_Int                   cacheSize;

  tsp00_Int                   tracePagesAL;
  tsp00_Int                   tracePagesBUP;
  tsp00_Int                   tracePagesDW;
  tsp00_Int                   tracePagesEV;
  tsp00_Int                   tracePagesGC;
  tsp00_Int                   tracePagesSV;
  tsp00_Int                   tracePagesTI;
  tsp00_Int                   tracePagesTW;
  tsp00_Int                   tracePagesUS;
  tsp00_Int                   tracePagesUT;

  tsp00_Bool                  showMaxStackUse;

  tsp00_Int                   mmapMemPoolSize;
  tsp00_Int                   shmMemPoolSize;
  tsp00_Int                   vallocMemPoolSize;
  tsp00_Int                   heapMemPoolSize;
  tsp00_Int                   memorySlotSize;

  tsp00_Uint                  ulFormattingMode;
  tsp00_NodeIdc               szOfficialNode;
  tsp00_Bool                  allowDemangle;
  tsp00_Bool                  fLogMirrored;
  tsp00_Bool                  fSetVolumeLock;
  tsp00_Bool                  fForbidLoadBalancing;
  tsp00_Bool                  allowSymbolResolution;
} RTE_XPARAM_REC;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*================================================*/

/* --- VEO79.C -------------------------------------------------------------- */

void   sql79k_create_taskcluster(   RTE_XPARAM_REC  *XParam,
                                    char            **TaskCluster );

void   sql79k_free_taskcluster  (   RTE_XPARAM_REC  *XParam,
                                    char            *TaskCluster );


#endif /* XPARAM_NAMES_ONLY */

#endif  /* GEO002_H */
