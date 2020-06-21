/*!
  @file           vos70k.cpp
  @author         RaymondR
  @brief          initialize kernel global storage
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




/*
 * INCLUDE FILES
 */
#include "RunTime/MemoryManagement/RTEMem_PageAllocator.h"
#include "RunTime/MemoryManagement/RTEMem_AWEAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "gos003.h"
#include "hos63k.h"
#include "geo70k.h"
#include "geo52.h"
#include "heo15.h"
#include "geo00_1.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h"

#if !defined ( WIN32 )
 #include <utils.h>
#endif


/*
 *  DEFINES
 */
#define MOD__  "VOS70KC : "
#define MF__   MOD__"UNDEFINED"

#define ASYN_WAITS 1
#define  MAX_REQ_PER_TASK                2


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
LONG  sql70k_get_num_of_dev_worker        ( ULONG                   ulNumOfUKTs );
LONG  sql70k_init_dev_ctrls               ( teo70k_ClusterInfo      *pCluInfo );
LONG  sql70k_init_async_ctrls             ( teo70k_ClusterInfo      *pCluInfo );
LONG  sql70k_init_UKT_ctrls               ( teo70k_ClusterInfo      *pCluInfo );
LONG  sql70k_init_KT_ctrls                ( VOID );
LONG  sql70k_init_kgs                     ( teo70k_ClusterInfo      *pCluInfo );
LONG  sql70k_init_task_ctrls              ( teo70k_ClusterInfo      *pCluInfo );
VOID  sql70k_init_thrd_ctrl_header        ( PTHREAD_CTRL_HEADER_REC pThrdCtrlHd,
                                            LONG                    lThrdPrio,
                                            LONG                    lThrdState,
                                            tsp00_Longuint          ulStartTime,
                                            LONG                    Tid,
                                            HANDLE                  hThrd );

LONG  sql70k_check_pos                    ( PLONG             plDest,
                                            LONG              lValue,
                                            PSZ               pszToken );

LONG  sql70k_check_task_types             ( teo70k_ClusterInfo *pCluInfo,
                                            LONG                lLastDWCnt,
                                            LONG                lLastUserCnt,
                                            LONG                lLastServerCnt );

LONG  sql70k_check_pos_comp               ( PLONG             plDest,
                                            LONG              lValue,
                                            LONG              lCompValue,
                                            PSZ               pszToken );

LONG sql70k_insert_tasks                  ( PTASK_CTRL_REC    pTaskCtrl,
                                            PUKT_CTRL_REC     pUKTCtrl,
                                            ULONG             ulNumOfTasks,
                                            PULONG            pulTaskIndex,
                                            UINT1             TaskType,
                                            tsp00_Longuint    ulTaskStackSize,
                                            PPVOID            ppCommCtrl,
                                            PULONG            pulCommFlagNo );

LONG sql70k_init_task                     ( PTASK_CTRL_REC    pTaskCtrl,
                                            ULONG             ulTaskIndex,
                                            UINT1             TaskType,
                                            tsp00_Longuint    ulTaskStackSize,
                                            PUKT_CTRL_REC     pUKTCtrl,
                                            PPVOID            ppCommCtrl,
                                            PULONG            pulCommFlagNo );

LONG  sql70k_init_allocators              ( teo70k_ClusterInfo      *pCluInfo );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

LONG sql70k_init_control_records (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_control_records"
  LONG             rc          = NO_ERROR;
  teo70k_ClusterInfo *pCluInfo    = NULL;
  #if defined(_WIN32)
   ULONG            ulMajorVersion;
   ULONG            ulMinorVersion;
  #endif

  DBGIN;

  rc = eo70k_AnalyzeTaskcluster ( kgs.XParam,
                                  &pCluInfo,
                                  kgs.ulSystemPageSize ) ;

  if (rc == NO_ERROR)
    {
      if ( pCluInfo->fOneToOne && XPARAM(uLoadBalancingCheck) == 0 )
      {
          // disable load balancing permanently
          XPARAM(fForbidLoadBalancing) = true;
      }

    kgs.ulUKTBaseStackSize = pCluInfo->ulUKTBaseStackSize; // PTS 1106437
    kgs.ulNumOfUKTs        = pCluInfo->ulNumOfUKTs;
    kgs.NumOfUserUKTs      = pCluInfo->ulNumOfUserUKTs;
    kgs.ulNumOfTasks       = pCluInfo->ulNumOfTasks;
    kgs.ulNumOfAsyncis     = XPARAM(lNoOfBackupDevs) + 1 +
                             XPARAM(lMaxDevspaces);
   
    #if defined(_WIN32)
     kgs.ulNumOfDevWorker = 0;

     if ( XPARAM(fUseAsyncIO) == TRUE  )
       {
       sql02_get_os_version ( &ulMajorVersion,
                              &ulMinorVersion );

       if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
         {
         if ( ulMajorVersion >= 4 )
           kgs.ulNumOfDevWorker = sql70k_get_num_of_dev_worker( kgs.ulNumOfUKTs );
         else
           MSGD(( INFO_OVERLAPPED_IO_NOT_SUP ))
         }
       }
    #endif

    kgs.ulNumOfDevis = XPARAM(lDeviPerDevspace) *
                       XPARAM(lMaxDevspaces);
    }

  if (rc == NO_ERROR)
    rc = sql70k_init_kgs ( pCluInfo );

  if (rc == NO_ERROR)
    rc = sql70k_init_dev_ctrls ( pCluInfo );

  if (rc == NO_ERROR)
    rc = sql70k_init_async_ctrls ( pCluInfo );

  if (rc == NO_ERROR)
    rc = sql70k_init_UKT_ctrls ( pCluInfo );

  if (rc == NO_ERROR)
    rc = sql70k_init_task_ctrls ( pCluInfo );

  if (rc == NO_ERROR)
    rc = sql70k_init_KT_ctrls ();

  if (rc == NO_ERROR)
    rc = sql70k_init_allocators ( pCluInfo );

  if ( !RTEDiag_CreatedRegisteredSymbolResolutionLock() )
  {
    MSGD (( ERR_CANT_REGISTER_SYMBOL_RESOLUTION_LOCK ))
  }

  if ( pCluInfo != NULL )
    {
    if (rc == NO_ERROR) rc = FREE_MEM ( pCluInfo );
    else                     FREE_MEM ( pCluInfo );
    }

  DBGOUT
  return ( rc );
  }

/*------------------------------*/

VOID sql70k_init_task_ctrl ( PTASK_CTRL_REC  pCurrTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_task_ctrl"
  
  DBGIN;

  //
  // --- only this part of the control record should be initialized
  //     which might be modfied during task life time !!!!
  //
  
  pCurrTaskCtrl->lResumeCount = 0;

  pCurrTaskCtrl->lExclusive                 = 0;
  pCurrTaskCtrl->Sid                        = UNDEF;
  pCurrTaskCtrl->ulBegExclCnt               = 0;
  pCurrTaskCtrl->lNestingExcl               = 0;
  pCurrTaskCtrl->lVSignalOk                 = 0;

  pCurrTaskCtrl->KernelCtrlRecord.to_cancel = FALSE;
  pCurrTaskCtrl->DevNameWaitingFor          = NULL;
  pCurrTaskCtrl->fDebugTask                 = FALSE;
  pCurrTaskCtrl->yieldRequestAtPos          = (SAPDB_UInt4)-1;

  switch ( pCurrTaskCtrl->TaskType )
    {
    case TT_US_EO00:
    case TT_UT_EO00:
    case TT_EV_EO00:
      pCurrTaskCtrl->Connectable = true;
      break;
    default:
      pCurrTaskCtrl->Connectable = false;
      break;
    }

  memset ( &pCurrTaskCtrl->RunnableMicroSeconds,    0, sizeof ( pCurrTaskCtrl->RunnableMicroSeconds ) );
  memset ( &pCurrTaskCtrl->MaxRunnableMicroSeconds, 0, sizeof ( pCurrTaskCtrl->MaxRunnableMicroSeconds ) );

  if ( pCurrTaskCtrl->TaskType != TT_US_EO00 )
    pCurrTaskCtrl->ulDispatcherCalls = 0;

  // --- Task Control initialized
  pCurrTaskCtrl->TaskState = TSK_INITIALIZED_EO00;

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql70k_init_task_ctrl_cntr ( PTASK_CTRL_REC  pCurrTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_task_ctrl_cntr"

  DBGIN;

  pCurrTaskCtrl->ulDispatcherCalls          = 0;
  pCurrTaskCtrl->ulSelfSuspCount            = 0;
  pCurrTaskCtrl->ulBegExclCalls             = 0;
  pCurrTaskCtrl->lWaitForTask               = UNDEF ;
  pCurrTaskCtrl->lWaitForRoot               = UNDEF ;
  pCurrTaskCtrl->lWaitForDevno              = UNDEF ;
  pCurrTaskCtrl->DevNameWaitingFor          = NULL ; 
  pCurrTaskCtrl->QueLocation                = TASK_IS_IN_NON_QUEUE;
  pCurrTaskCtrl->PrioFlag                   = 0;
  pCurrTaskCtrl->SavePrioFlag               = 0;
  pCurrTaskCtrl->lockedPageNo               = MAX_INT4_SP00; /* PTS 1115172 */

  memset ( &pCurrTaskCtrl->TaskStateStat, 0, sizeof ( teo52_TaskStateStatisticRec ) );
  memset ( &pCurrTaskCtrl->QueStat      , 0, sizeof ( teo52_TaskQueueStatistic ) );
  memset ( &pCurrTaskCtrl->PrioStat     , 0, sizeof ( teo52_TaskPrioStatistic ) );
  memset ( &pCurrTaskCtrl->TaskOMSStat  , 0, sizeof ( teo52_TaskOMSStatisticRec ) );

  pCurrTaskCtrl->maxStackPointer            = 0; // PTS 1110953
  pCurrTaskCtrl->commandMaxStack            = 0;
  pCurrTaskCtrl->totalMaxStack              = 0;
  
  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql70k_init_user_comm_ctrl ( PUSER_TASK_COMM_CTRL  pUserCommCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_user_comm_ctrl"
  INT i;

  DBGIN;

  pUserCommCtrl->ulCommState          = commErrOk_esp01;
  pUserCommCtrl->ulGlobCommState      = commErrOk_esp01;

  pUserCommCtrl->ulCmdWaitTime        = 0;

  pUserCommCtrl->hLocalClientProcess  = (HANDLE)INVALID_HANDLE_VALUE;
  pUserCommCtrl->pidLocalClientPID    = (PID)UNDEF;

  pUserCommCtrl->hClientSem           = INVALID_HANDLE_VALUE;
  pUserCommCtrl->pCSHeader            = NULL;
  pUserCommCtrl->hCS                  = (HANDLE)INVALID_HANDLE_VALUE;

  pUserCommCtrl->pidClientPID         = (PID)UNDEF;
  pUserCommCtrl->szClientNode[0]      = '\0';
  pUserCommCtrl->ulClientRef          = (ULONG)UNDEF;
  pUserCommCtrl->ulMaxDataLen         = 0;
  pUserCommCtrl->ulMaxCmdDataLen      = 0;
  pUserCommCtrl->pReplyCommPacket     = NULL;
  pUserCommCtrl->ulPacketCnt          = 0;

  for ( i = 0; i < MAX_SQL_PACKETS; i++ )
    pUserCommCtrl->pCommPacketList[i] = NULL;

  pUserCommCtrl->encryption           = RTETask_ITaskClientConnection::UnknownEncryption;

  //
  // --- this value must be last !!!!!!!!!
  //
  pUserCommCtrl->ulConnected          = 0;

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql70k_init_dev_io ( PDEV_IO_CTRL_REC pIOCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_dev_io"

  DBGIN;

  // --- init memory
  memset ( pIOCtrl, 0, sizeof (DEV_IO_CTRL_REC) );

  pIOCtrl->DevHandle  = INVALID_HANDLE_VALUE;
  pIOCtrl->ulDevType  = DT_UNKNOWN;

  DBGOUT;

  return;
  }

/*------------------------------*/

VOID  sql70k_init_devi_ctrl ( PDEVI_CTRL_REC pDeviCtrl,
                              PIO_QUEUE_REC  pIOs )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_devi_ctrl"

  DBGIN;

  // --- init memory
  memset ( pDeviCtrl, 0, sizeof (DEVI_CTRL_REC) );

  sql70k_init_thrd_ctrl_header ( &pDeviCtrl->ThrdCtrlHeader,
                                 DEVI_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  sql73k_IO_queue_init ( pIOs, kgs.ulNumOfTasks * MAX_DEVI_REQUESTS );

  pDeviCtrl->DevHandle         = INVALID_HANDLE_VALUE;
  pDeviCtrl->ulOldIOPos        = (ULONG)UNDEF;
  pDeviCtrl->pIOs              = pIOs;
  pDeviCtrl->pIOWorkQue        = pIOs;
  pDeviCtrl->pIOReqQue         = pIOs;

  DBGOUT;

  return;
  }

/*------------------------------*/

VOID  sql70k_init_asynci_ctrl ( PASYNCI_CTRL_REC pAsynciCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_asynci_ctrl"
  PIO_QUEUE_REC                   pIOs;
  ULONG                           ulIOs;
  RTETask_CommonQueueElement*     pAsynRequests;
  ULONG                           ulAsynRequests;

  DBGIN;

  // --- save some values
  pIOs             = pAsynciCtrl->pIOs;
  ulIOs            = pAsynciCtrl->ulIOs;
  pAsynRequests    = pAsynciCtrl->pAsynRequests;
  ulAsynRequests   = pAsynciCtrl->ulAsynRequests;

  // --- init memory
  memset ( pAsynciCtrl, 0, sizeof(*pAsynciCtrl) );

  sql73k_IO_queue_init ( pIOs, ulIOs );

  sql70k_init_thrd_ctrl_header ( &pAsynciCtrl->ThrdCtrlHeader,
                                 ASYNCI_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  pAsynciCtrl->DevHandle          = INVALID_HANDLE_VALUE;
  memset ( &(pAsynciCtrl->DevIOStat), 0, sizeof( teo52_DevIOStatistic )) ;
  pAsynciCtrl->ulOldIOPos         = (ULONG)UNDEF;
  pAsynciCtrl->ulIOs              = ulIOs;
  pAsynciCtrl->pIOs               = pIOs;
  pAsynciCtrl->pIOWorkQue         = pIOs;
  pAsynciCtrl->pIOReqQue          = pIOs;
  pAsynciCtrl->lExclusive         = 0;
  pAsynciCtrl->pUKT               = (PUKT_CTRL_REC) UNDEF;
  pAsynciCtrl->fIsDevspace        = FALSE;
  pAsynciCtrl->fForWriting        = FALSE;
  pAsynciCtrl->ulBlockSize        = (ULONG)UNDEF;
  pAsynciCtrl->ulMaxBlockCnt      = (ULONG)UNDEF;
  pAsynciCtrl->ulDevType          = (ULONG)DT_UNKNOWN;
  pAsynciCtrl->fAsyncClosePending = FALSE;
  pAsynciCtrl->pAsynRequests      = pAsynRequests;
  pAsynciCtrl->ulAsynRequests     = ulAsynRequests;


  DBGOUT;
  return;
  }

/*
 *========================== LOCAL FUNCTIONS =================================
 */

static LONG sql70k_get_num_of_dev_worker ( ULONG  ulNumOfUKTs )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_get_num_of_dev_worker"
  LONG              rc = NO_ERROR;
  #if defined(_WIN32)
   PATHNAME         szSubKey;
   REG_ENTRY_REC    RegistryEntries[1];
   SYSTEM_INFO      SystemInfo;
   ULONG            ulNumOfDevWorkers;
   ULONG            ulDefaultNumOfDevWorkers;
  #endif

  DBGIN;

  #if defined(_WIN32)
   GetSystemInfo ( &SystemInfo );

   if ( RTESys_NumberOfPhysicalCPUs() == 1)
     ulDefaultNumOfDevWorkers = 2;
   else
   {
     ulDefaultNumOfDevWorkers  = min(RTESys_NumberOfPhysicalCPUs(), ulNumOfUKTs ); 
     ulDefaultNumOfDevWorkers += ulDefaultNumOfDevWorkers / 2;
   }

   if ( kgs.fRunningAsNTService == TRUE )
     {
     strcpy ( szSubKey, kgs.szServiceName);
     strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

     RegistryEntries[0].pszValueName = REG_VN_DEV_WORKERS;
     RegistryEntries[0].pValue       = &ulNumOfDevWorkers;
     RegistryEntries[0].ulValueSize  = sizeof(DWORD);
     RegistryEntries[0].ulValueType  = REG_DWORD;

     rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

     if (rc != NO_ERROR)
       ulNumOfDevWorkers = ulDefaultNumOfDevWorkers;
     else
       ulNumOfDevWorkers = max( min(ulNumOfDevWorkers,
                                    RTESys_NumberOfPhysicalCPUs() * 2), 1);
     }
   else
     ulNumOfDevWorkers = ulDefaultNumOfDevWorkers;
  #endif

  DBGOUT;
  return(ulNumOfDevWorkers);
  }

/*------------------------------*/

static LONG  sql70k_init_UKT_ctrls (teo70k_ClusterInfo  *pCluInfo)
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_UKT_ctrls"
  LONG                            rc            = NO_ERROR;
  PUKT_SINGLE_IO_REC              pSingleIO     = NULL;
  PUCHAR                          puchBuff      = NULL;
  ULONG                           ulIdx         = 0;
  LONG                            ulUKTCtrlSize;
  PUKT_CTRL_REC                   pCurrUKTCtrl;
  PUKT_TO_UKT_QUEUE_HEAD_REC      pUToUQueHead;
  PUKT_TO_UKT_QUEUE_REC           pUToUQueElem;
  ULONG                           ulSingleIOSize;
  ULONG                           ulTotalSize;
  ULONG                           ulCount;
  ULONG                           ulNumRUNQueueElem;
  ULONG                           ulNumUKTQueueElem;
  ULONG                           ulNumUToUQueueHeads;
  ULONG                           ulNumUToUQueueElem;
  PUKT_RUN_QUEUE_REC              pRUNs;
  PUKT_QUEUE_REC                  pUKTs;
  ULONG                           ulNumUToUQueueElemForActUKT;
  char                           *nextUktTraceBuffer;

  DBGIN;

  // --- compute the alloc space for ukt single_io records
  ulSingleIOSize =  sizeof ( UKT_SINGLE_IO_REC ) *
                    XPARAM(lMaxDevspaces)             *
                    kgs.ulNumOfUKTs ;

  // --- compute the alloc space of the control records for the user kernel threads
  ulUKTCtrlSize = sizeof(UKT_CTRL_REC) * kgs.ulNumOfUKTs;
  ulTotalSize   = ulUKTCtrlSize + ulSingleIOSize;

  rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

  if( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (puchBuff, 0, ulTotalSize);

  // --- Set a pointer to the first and last control record
  kgs.pFirstUKTCtrl = (PUKT_CTRL_REC) puchBuff;
  kgs.pLastUKTCtrl  = kgs.pFirstUKTCtrl + kgs.ulNumOfUKTs - 1;

  // --- set the pointers read and write counter pool
  pSingleIO  = (PUKT_SINGLE_IO_REC) (puchBuff + ulUKTCtrlSize);

  //
  // --- init UKT Controls
  //
  /* PTS 1108471 Space for Tracebuffer used for InMemory tracing */
  kgs.totalTraceBufferSizeInPages = XPARAM(lTraceDevspaceSize);
  kgs.traceBufferSpace = (tsp00_PageAddr ) VirtualAlloc(NULL, kgs.totalTraceBufferSizeInPages * XPARAM(ulDBPageSize),
                                           MEM_COMMIT, PAGE_READWRITE); /* Can be used for direct I/O */
  kgs.traceNeedsSaving = 0;
  if( kgs.traceBufferSpace == NULL )
    {
// PTS 1112502
    MSGD (( ERR_MALLOC_FAILED_FOR, "trace buffer (check TRACE_PAGES_-config parameters)" ))
    DBGOUT;
    return ( GetLastError() );
    }
  /* Initialize trace buffer space to ZERO */
  memset(kgs.traceBufferSpace, 0, kgs.totalTraceBufferSizeInPages * XPARAM(ulDBPageSize) );

  nextUktTraceBuffer = (char *)kgs.traceBufferSpace;
  nextUktTraceBuffer += XPARAM(ulDBPageSize);

  for ( pCurrUKTCtrl = kgs.pFirstUKTCtrl, ulIdx = 0;
        pCurrUKTCtrl <= kgs.pLastUKTCtrl;
        pCurrUKTCtrl++, ulIdx++ )
    {
    sql70k_init_thrd_ctrl_header ( &pCurrUKTCtrl->ThrdCtrlHeader,
                                   pCluInfo->pThrdInfo[ulIdx].lThrdPrio,
                                   UKT_INITIALIZED,
                                   0,
                                   UNDEF,
                                   INVALID_HANDLE_VALUE);

    sql73k_tlist_init( &pCurrUKTCtrl->TaskList );

    pCurrUKTCtrl->ulActiveTasks         = 0;
    pCurrUKTCtrl->ulActiveUserTasks     = 0;
    pCurrUKTCtrl->ulActiveServTasks     = 0;
    pCurrUKTCtrl->ulUKTIndex            = ulIdx + 1;
    pCurrUKTCtrl->ulUKTTimeOut          = MAXTIMEVALUE;
    pCurrUKTCtrl->ulUKTStackSize        = pCluInfo->pThrdInfo[ulIdx].ulStackSize;
    pCurrUKTCtrl->pCTask                = NULL;

    pCurrUKTCtrl->DispatcherCalls       = 0;
    pCurrUKTCtrl->ulSemaNo              = ulIdx;
    pCurrUKTCtrl->taskSwitches          = 0; // PTS 1108715

    ulNumRUNQueueElem           = kgs.ulNumOfTasks * MAX_REQ_PER_TASK;
    ulNumUKTQueueElem           = kgs.ulNumOfTasks * MAX_ASYN_REQUESTS *
                                  (XPARAM(lNoOfBackupDevs) + XPARAM(lMaxDevspaces));
    ulNumUToUQueueHeads         = kgs.ulNumOfUKTs;

    if ( XPARAM(fForbidLoadBalancing) )    
    {

    	ulNumUToUQueueElemForActUKT = pCluInfo->pThrdInfo[ulIdx].ulNumOfTasks + 1;
    }
    else
    {
    	ulNumUToUQueueElemForActUKT = kgs.ulNumOfTasks + 1;
    }
    ulNumUToUQueueElem          = ulNumUToUQueueElemForActUKT * (kgs.ulNumOfUKTs - 1);

    //
    // --- compute the total size need for all UKT queues
    //    (RUN, UKT )
    //
    ulTotalSize = ( ulNumRUNQueueElem   * sizeof(UKT_RUN_QUEUE_REC))         +
                  ( ulNumUKTQueueElem   * sizeof(UKT_QUEUE_REC))             +
                  ( ulNumUToUQueueHeads * sizeof(UKT_TO_UKT_QUEUE_HEAD_REC)) +
                  ( ulNumUToUQueueElem  * sizeof(UKT_TO_UKT_QUEUE_REC));

    rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

    if( rc != NO_ERROR )
      {
      DBGOUT;
      return ( rc );
      }

    // --- init memory
    memset (puchBuff, 0, ulTotalSize);

    pRUNs        = (PUKT_RUN_QUEUE_REC)puchBuff;
    puchBuff     = (PUCHAR)(pRUNs + ulNumRUNQueueElem);

    pUKTs        = (PUKT_QUEUE_REC)puchBuff;
    puchBuff     = (PUCHAR)(pUKTs + ulNumUKTQueueElem);

    pUToUQueHead = (PUKT_TO_UKT_QUEUE_HEAD_REC)puchBuff;
    puchBuff     = (PUCHAR)(pUToUQueHead + ulNumUToUQueueHeads);

    pUToUQueElem = (PUKT_TO_UKT_QUEUE_REC)puchBuff;
    puchBuff     = (PUCHAR)(pUToUQueElem + ulNumUToUQueueElem);


    pCurrUKTCtrl->RUNQue    = pRUNs ;
    pCurrUKTCtrl->ulRUNQueueLen = 0 ;

    memset(&pCurrUKTCtrl->UKTRunningStat, 0, sizeof(teo52_TaskSchedulerTimeStatistic));
    memset(&pCurrUKTCtrl->UKTIOWaitStat, 0, sizeof(teo52_TaskSchedulerTimeStatistic));
    pCurrUKTCtrl->RunningStateEnterMicroSeconds    = 0;
    pCurrUKTCtrl->systemTimeAtLastReset = 0;
    pCurrUKTCtrl->userTimeAtLastReset = 0;
    pCurrUKTCtrl->maxRunQueueLen = 0;
    pCurrUKTCtrl->maxRunQueueLenSinceLastReset = 0;
    pCurrUKTCtrl->SleepStatRecResetPending = false;

    sql73k_UKT_queue_init ( &pCurrUKTCtrl->UKTQue, pUKTs, ulNumUKTQueueElem );

    // --- used for self I/O
    pCurrUKTCtrl->pSingleIO          = pSingleIO;
    pCurrUKTCtrl->lWaitForDeviIO     = 0;
    pCurrUKTCtrl->lWaitForAsynciIO   = 0;

    pSingleIO  += XPARAM(lMaxDevspaces);

    for ( ulCount = 0; ulCount < (ULONG) XPARAM(lMaxDevspaces); ulCount ++ )
      {
      pCurrUKTCtrl->pSingleIO [ulCount] . ulOldSingleIOPos = 0;
      pCurrUKTCtrl->pSingleIO [ulCount] . SingleIOHandle   = INVALID_HANDLE_VALUE;
      }

    // --- init the UKT to UKT queue heads and elements
    pCurrUKTCtrl->pUToUQueHeads = pUToUQueHead;

    for ( ulCount = 0; ulCount < kgs.ulNumOfUKTs; ulCount++ )
      {
      if ( ulCount == pCurrUKTCtrl->ulUKTIndex - 1 )
        {
        // --- it's me!
        sql73k_UToU_queue_init ( &pUToUQueHead[ulCount], NULL, 0 );
        }
      else
        {
        sql73k_UToU_queue_init ( &pUToUQueHead[ulCount],
                                 pUToUQueElem,
                                 ulNumUToUQueueElemForActUKT ); // PTS 1106562


        pUToUQueElem += ulNumUToUQueueElemForActUKT;
        }
      }
      /* PTS 1108471 Space for Tracebuffer used for InMemory tracing */
      pCurrUKTCtrl->traceBufferSizeInPages = pCluInfo->pThrdInfo[ulIdx].tracePages;
      pCurrUKTCtrl->traceBuffer            = (tsp00_PageAddr)nextUktTraceBuffer;
      nextUktTraceBuffer                  += pCurrUKTCtrl->traceBufferSizeInPages * XPARAM(ulDBPageSize);
    }

  RTETask_TaskScheduler* pLastTaskScheduler = 0;

  // we step backward through the UKT list to get a correct TaskScheduler next-chain
  for ( pCurrUKTCtrl = kgs.pLastUKTCtrl;
        pCurrUKTCtrl >= kgs.pFirstUKTCtrl;
        pCurrUKTCtrl-- )
    {
    pCurrUKTCtrl->pTaskSchedulerObject = new (RTEMem_RteAllocator::Instance()) RTETask_TaskScheduler(pCurrUKTCtrl, pLastTaskScheduler);

    if ( 0 == pCurrUKTCtrl->pTaskSchedulerObject )
      {
      MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
      return ( ERROR_NOT_ENOUGH_MEMORY );
      }

    pLastTaskScheduler = pCurrUKTCtrl->pTaskSchedulerObject;
    }

  // the last will be the first...
  RTE_KGS::Instance().SetTaskSchedulerList( *pLastTaskScheduler );

  DBGOUT;
  return ( rc );
  }
/*------------------------------*/

static LONG  sql70k_init_task_ctrls (teo70k_ClusterInfo  *pCluInfo)
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_task_ctrls"
  ULONG                           ulIdx, ulVarTask ;
  LONG                            rc            = NO_ERROR;
  ULONG                           ulNumOfTasks  = 0;
  ULONG                           ulCommFlagNo;
  ULONG                           ulTaskIndex;
  PTASK_CTRL_REC                  pCurrTaskCtrl;
  PPTASK_CTRL_REC                 ppFristXXTaskCtrl, ppLastXXTaskCtrl ;
  PUKT_CTRL_REC                   pCurrUKTCtrl;
  PVOID                           pCurrCommCtrl;
  PUCHAR                          puchBuff          = NULL;
  ULONG                           ulTotalSize;
  ULONG                           ulTaskCtrlSize;
  ULONG                           ulUserCommCtrlSize;
  teo70k_VarTaskCntInfo          *pVarTask ;
  UINT1                           ActTaskType, NeedComSeg ;

  DBGIN;

  //
  // --- Alloc space for the task controls and task connection controls
  //
  ulTaskCtrlSize       = sizeof(TASK_CTRL_REC) *
                         kgs.ulNumOfTasks;
  ulUserCommCtrlSize   = sizeof(USER_TASK_COMM_CTRL) *
                         (XPARAM(ulMaxUser) + 1 + XPARAM(ulMaxEventTasks) );

  ulTotalSize = ulTaskCtrlSize       +
                ulUserCommCtrlSize;

  //
  // --- Alloc space for the control records
  //
  rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (puchBuff, 0, ulTotalSize);

  pCurrTaskCtrl     = (PTASK_CTRL_REC) puchBuff;
  pCurrCommCtrl     = (PUSER_TASK_COMM_CTRL)(puchBuff + ulTaskCtrlSize);

  // --- Set a pointer to the last control record
  kgs.pFirstTaskCtrl = pCurrTaskCtrl;
  kgs.pLastTaskCtrl  = kgs.pFirstTaskCtrl + kgs.ulNumOfTasks - 1;
  ulTaskIndex        = 1;
  ulCommFlagNo       = 0;

  //
  // --- TIMEOUT
  //
  kgs.pTI      = pCurrTaskCtrl++;
  pCurrUKTCtrl = kgs.pFirstUKTCtrl + pCluInfo->TI_Pos - 1;
  DBG3 ((MF__, " --- TI ------------------------------------"));
  /* No own entry for task stack . Reuse entry for data writer task... */
  rc = sql70k_insert_tasks          (kgs.pTI, pCurrUKTCtrl, 1, &ulTaskIndex, TT_TI_EO00,
                                     pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, NULL, NULL );
  //
  // --- ARCHIVELOGWRITER
  //
  if (rc == NO_ERROR)
    {
    kgs.pAL      = pCurrTaskCtrl++;
    pCurrUKTCtrl = kgs.pFirstUKTCtrl + pCluInfo->AL_Pos - 1;
    DBG3 ((MF__, " --- AL ------------------------------------"));
  /* No own entry for task stack ... Reuse entry for data writer task... */
    rc = sql70k_insert_tasks          (kgs.pAL, pCurrUKTCtrl, 1, &ulTaskIndex, TT_AL_EO00,
                                       pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, NULL, NULL );
    }

  //
  // --- TRACEWRITER
  //
  if (rc == NO_ERROR)
    {
    kgs.pTW      = pCurrTaskCtrl++;
    pCurrUKTCtrl = kgs.pFirstUKTCtrl + pCluInfo->TW_Pos - 1;
    DBG3 ((MF__, " --- TW ------------------------------------"));
  /* No own entry for task stack ... Reuse entry for data writer task... */
    rc = sql70k_insert_tasks          (kgs.pTW, pCurrUKTCtrl, 1, &ulTaskIndex, TT_TW_EO00,
                                       pCluInfo->VarTaskCntRec[VAR_TASK_DW_EO70K].lStackSize, NULL, NULL );
    }

  //
  // --- UTILITY
  //
  if (rc == NO_ERROR)
    {
    kgs.pUT      = pCurrTaskCtrl++;
    pCurrUKTCtrl = kgs.pFirstUKTCtrl + pCluInfo->UT_Pos - 1;
    DBG3 ((MF__, " --- UT ------------------------------------"));
    rc = sql70k_insert_tasks          (kgs.pUT, pCurrUKTCtrl, 1, &ulTaskIndex, TT_UT_EO00,
                                       XPARAM(ulMaxUsUtStack),
                                       &pCurrCommCtrl, &ulCommFlagNo );
    }


  for ( ulVarTask = 0; ulVarTask < MX_VAR_TASK_TYPES_EO70K && rc == NO_ERROR ; ulVarTask ++  ) 
  { 
    pVarTask = &pCluInfo->VarTaskCntRec [ ulVarTask ] ;

    DBG3 ((MF__, " --- %s ------------", pVarTask->TaskTypeToken));
    switch ( pVarTask->lVarTaskType )
    {
      case VAR_TASK_DW_EO70K :
                         ppFristXXTaskCtrl = &kgs.pFirstDWTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastDWTaskCtrl ;
                         ActTaskType       = TT_DW_EO00 ;
                         NeedComSeg        = FALSE ;
                         break ;
      case VAR_TASK_EV_EO70K :
                         ppFristXXTaskCtrl = &kgs.pFirstEVTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastEVTaskCtrl ;
                         ActTaskType       = TT_EV_EO00 ;
                         NeedComSeg        = TRUE ;
                         break ;
      case VAR_TASK_US_EO70K :
                         ppFristXXTaskCtrl = &kgs.pFirstUserTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastUserTaskCtrl ;
                         ActTaskType       = TT_US_EO00 ;
                         NeedComSeg        = TRUE ;
                         break ;
      case VAR_TASK_SV_EO70K :
                         ppFristXXTaskCtrl = &kgs.pFirstServerTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastServerTaskCtrl ;
                         ActTaskType       = TT_SV_EO00 ;
                         NeedComSeg        = FALSE ;
                         break ;
      case VAR_TASK_GC_EO70K :
                         ppFristXXTaskCtrl = &kgs.pFirstGCTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastGCTaskCtrl ;
                         ActTaskType       = TT_GC_EO00 ;
                         NeedComSeg        = FALSE ;
                         break ;
      case VAR_TASK_BUP_EO70K:
                         ppFristXXTaskCtrl = &kgs.pFirstBackUpTaskCtrl ;
                         ppLastXXTaskCtrl  = &kgs.pLastBackUpTaskCtrl ;
                         ActTaskType       = TT_BUP_EO00 ;
                         NeedComSeg        = FALSE ;
                         break ;
      default:
                         break ;
    }

    *ppFristXXTaskCtrl = pCurrTaskCtrl ;

    for ( pCurrUKTCtrl   = kgs.pFirstUKTCtrl, ulIdx = 0;
         (pCurrUKTCtrl <= kgs.pLastUKTCtrl) && (rc == NO_ERROR);
          pCurrUKTCtrl++, ulIdx++ )
    {
      ulNumOfTasks = *eo70k_GetVarCntFromThrdInfo (&pCluInfo->pThrdInfo[ulIdx], pVarTask->lVarTaskType) ;

      if ( ulNumOfTasks > 0 )
        {
        DBG3 ((MF__, " -------------------------------------------"));
        rc = sql70k_insert_tasks         ( pCurrTaskCtrl, pCurrUKTCtrl, ulNumOfTasks,
                                           &ulTaskIndex, ActTaskType, pVarTask->lStackSize,
                                           NeedComSeg ? &pCurrCommCtrl : NULL , 
                                           NeedComSeg ? &ulCommFlagNo  : NULL );
        pCurrTaskCtrl += ulNumOfTasks;
        }
    }

    if ( *ppFristXXTaskCtrl == pCurrTaskCtrl )
      {
        *ppFristXXTaskCtrl = *ppLastXXTaskCtrl = NULL ;
        DBG3 ((MF__, " There is no '%s' task", pVarTask->TaskTypeToken));
      }
    else
      {
        *ppLastXXTaskCtrl = pCurrTaskCtrl - 1 ;
        DBG3 ((MF__, " There are %d '%s' task", 
               pCurrTaskCtrl - *ppFristXXTaskCtrl, pVarTask->TaskTypeToken));
      }
  }

  kgs.fMoreUserUKTsThanMaxCPU = (ULONG)kgs.NumOfUserUKTs > XPARAM(ulMaxCPU);
  kgs.fBusyWaitingForbidden   = kgs.ulNumOfPhysicalProcessors <= 1 || kgs.fMoreUserUKTsThanMaxCPU;

  kgs.ulNumOfCommFlags = ulCommFlagNo;

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static  LONG sql70k_insert_tasks ( PTASK_CTRL_REC  pTaskCtrl,
                                   PUKT_CTRL_REC   pUKTCtrl,
                                   ULONG           ulNumOfTasks,
                                   PULONG          pulTaskIndex,
                                   UINT1           TaskType,
                                   tsp00_Longuint  ulTaskStackSize,
                                   PPVOID          ppCommCtrl,
                                   PULONG          pulCommFlagNo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_insert_tasks"
  ULONG                      ulTaskCnt;
  LONG                       rc  = NO_ERROR;

  DBGIN;

  // --- insert tasks
  for ( ulTaskCnt = 0; ulTaskCnt < ulNumOfTasks;
        pTaskCtrl++, ulTaskCnt++, (*pulTaskIndex)++ )
    {
    rc = sql70k_init_task ( pTaskCtrl, *pulTaskIndex, TaskType, ulTaskStackSize,
                            pUKTCtrl, ppCommCtrl, pulCommFlagNo );

    if ( rc != NO_ERROR )
        break;

    if ( 0 == ( pTaskCtrl->MoveLock.Type.LongTerm & NEVER_MOVE_TASK ))
        pUKTCtrl->TasksAllowedToMoveTo = true;

#if defined(UNCONNECT_TASK_LIST)
    if (( pTaskCtrl->MoveLock.Type.LongTerm == NEVER_MOVE_TASK ) || 
        ( pTaskCtrl->Connectable            == false ))
      {
      // - This task will never move between UKTs or is not a connectable task, so 
      //   we have to put it directly into the UKTs task chain
      sql73k_tlist_enque_task_nolock ( &pUKTCtrl->TaskList, pTaskCtrl );
      }
    else
      {
      pTaskCtrl->pUKT = NULL;
      sql73k_tlist_enque_task_nolock ( &kgs.UnconnectedMoveTaskList, pTaskCtrl );
      sql74k_alter_task_cnt ( pTaskCtrl, ADD_TOTAL_COUNTER );
      }
#else
    sql73k_tlist_enque_task_nolock ( &pUKTCtrl->TaskList, pTaskCtrl );
    sql74k_alter_task_cnt ( pTaskCtrl, ADD_TOTAL_COUNTER );
#endif
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static LONG  sql70k_init_KT_ctrls ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_KT_ctrls"
  LONG  rc = NO_ERROR;

  DBGIN;

  // --- Initialize kernel thread control headers and doubly link queues
  sql70k_init_thrd_ctrl_header ( &kgs.Coord.ThrdCtrlHeader,
                                 COORDINATOR_THRD_PRIO,
                                 KT_RUNNING,
                                 kgs.ulDatabaseStartTime,
                                 THREADID,
                                 INVALID_HANDLE_VALUE);

  sql73k_DLQ_init ( &kgs.Coord.DLQ_Work );


  sql70k_init_thrd_ctrl_header ( &kgs.Timer.ThrdCtrlHeader,
                                 TIMER_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  sql73k_DLQ_init ( &kgs.Timer.DLQ_Work );


  sql70k_init_thrd_ctrl_header ( &kgs.Clock.ThrdCtrlHeader,
                                 CLOCK_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  sql73k_DLQ_init ( &kgs.Clock.DLQ_Work );


  sql70k_init_thrd_ctrl_header ( &kgs.Console.ThrdCtrlHeader,
                                 CONSOLE_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  sql73k_DLQ_init ( &kgs.Console.DLQ_Work );


  sql70k_init_thrd_ctrl_header ( &kgs.Requestor.ThrdCtrlHeader,
                                 REQUESTOR_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  sql73k_DLQ_init ( &kgs.Requestor.DLQ_Work );


  DBGOUT
  return (rc);
  }

/*------------------------------*/

static LONG sql70k_init_allocators ( teo70k_ClusterInfo  *pCluInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_allocators"
  LONG                  rc        = NO_ERROR;
  ULONG                 ulMajorVersion;
  ULONG                 ulMinorVersion;
  SAPDB_UInt4           NumberOfPhysPages;
  SAPDB_UInt8           ReservedPhysMemory;
  PATHNAME              szSubKey;
  REG_ENTRY_REC         RegistryEntries[1];

  DBGIN;

  sql02_get_os_version ( &ulMajorVersion, &ulMinorVersion );

  if (( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT ) &&
      ( ulMajorVersion           >= 5 )                     &&   // >= Windows 2000
  	  ( XPARAM(fUseMemEnhancement) ))
  {
     strcpy ( szSubKey, kgs.szServiceName);
     strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );
     ReservedPhysMemory              = 0;
     RegistryEntries[0].pszValueName = REG_VN_AWE_RESERVED_MEMORY;
     RegistryEntries[0].pValue       = (DWORD*)&ReservedPhysMemory;
     RegistryEntries[0].ulValueSize  = sizeof(DWORD);
     RegistryEntries[0].ulValueType  = REG_DWORD;

     rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

     if (rc != NO_ERROR)
       ReservedPhysMemory = 0;
     else
       ReservedPhysMemory = ReservedPhysMemory * 1024*1024;

    if ( XPARAM(lMemEnhancementLimit) == 0 ) // no limit requested
      NumberOfPhysPages = 0;
    else
      NumberOfPhysPages = (ULONG)(((SAPDB_UInt8)XPARAM(lMemEnhancementLimit)*1024*1024) / XPARAM(ulDBPageSize)) + XPARAM(cacheSize);

//  RTEMem_AWEInitialize( XPARAM(ulDBPageSize), XPARAM(cacheSize), &NumberOfPhysPages, ReservedPhysMemory );
  }


  /*
   * If no AWE is available initialize I/O Page allocator.
   */
  if ( ( XPARAM(cacheSize) > 0 ) && ( RTEMem_AWEAvailable() == false ))
  {
    SAPDB_UInt4 wantedPages;
    wantedPages = XPARAM(cacheSize);

    if ( ! RTEMem_InitializePageAllocator(XPARAM(ulDBPageSize), wantedPages) )
    {
      MSGD (( ERR_NOT_ENOUGH_MEMORY, "I/O buffer cache", (wantedPages * XPARAM(ulDBPageSize)) / 1024 ))
      return ( ENOMEM );
    }
    MSGD (( INFO_RTE_PAGE_ALLOCATOR_MEM_INFO, wantedPages ))
  }

  DBGOUT
  return (rc);
  }

/*------------------------------*/

static void SearchUnreleasedConnectionsCallback(bool atLeastOneFound)
{
    if ( !atLeastOneFound )
    {
        // --- sleep a moment and retry find free task
        SLEEP ( 3000 );
    }

    sql53k_remove_unrel_connnections();
}

/*------------------------------*/

static LONG sql70k_init_kgs ( teo70k_ClusterInfo  *pCluInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_kgs"
  PUCHAR                puchBuff  = NULL;
  LONG                  rc        = NO_ERROR;
  ULONG                 ulIdx;
  ULONG                 ulTotalSize;

  DBGIN;

  kgs.fSingleIOOpenLocked = FALSE;

  PAGESIZE_TO_SHIFTVALUE( XPARAM(ulDBPageSize), &kgs.ulDBPageShift );

  //
  // --- inititialize the semalist
  //
  rc = ALLOC_MEM ( (PPVOID)&kgs.pSemaphores,
                   sizeof(SEM_CTRL_REC) * XPARAM(ulNoOfRegions) );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (kgs.pSemaphores, 0,
          sizeof(SEM_CTRL_REC) * XPARAM(ulNoOfRegions) );

  ulTotalSize = sizeof(SEM_QUEUE_REC) *
                kgs.ulNumOfTasks      *
                XPARAM(ulNoOfRegions);

  rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (puchBuff, 0, ulTotalSize);

  for ( ulIdx = 0; ulIdx < XPARAM(ulNoOfRegions); ulIdx++ )
    {
    kgs.pSemaphores[ulIdx].SemaList.pSEMs = (PSEM_QUEUE_REC) puchBuff;

    puchBuff += (sizeof(SEM_QUEUE_REC) * kgs.ulNumOfTasks);
    }

  //
  // --- inititialize the list of free common queue elements
  //
  SAPDB_UInt4 commonQueueElements = MAX_FREE_QUE_ELEM_PER_TASK * kgs.ulNumOfTasks + kgs.ulNumOfUKTs;

  RTETask_CommonQueueElement*  pElements = newarray ( pElements, commonQueueElements, 
                                                      RTEMem_RteAllocator::Instance() );
  if ( 0 == pElements )
    {
    MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

  RTETask_SynchronizedCommonQueue* 
      pFreeList = new (RTEMem_RteAllocator::Instance()) RTETask_SynchronizedCommonQueue (pElements, commonQueueElements, "GlobalCommonQueueFreeList");

  if ( 0 == pFreeList )
    {
    MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

  RTE_KGS::Instance().SetGlobalFreeList( *pFreeList );

  /*
   * create global kernel communication object 
   */ 
  RTETask_UnconnectedTaskPool::Strategy strategy =
                                            pCluInfo->lConnectStrategy == STRAT_EQUALIZE ? 
                                            RTETask_UnconnectedTaskPool::Equalize : 
                                            RTETask_UnconnectedTaskPool::Compress;

  RTECKC_KernelCommunication* 
      pKernelCommunication = new (RTEMem_RteAllocator::Instance()) RTECKC_KernelCommunication( strategy, SearchUnreleasedConnectionsCallback );

  if ( 0 == pKernelCommunication )
    {
    MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

  RTE_KGS::Instance().SetKernelCommunication( *pKernelCommunication );

  #if defined ( _WIN32 )
   kgs.hCompletionPort = INVALID_HANDLE_VALUE;
  #else
   rc = ALLOC_MEM ( (PPVOID)&kgs.pThrdBasedUKTCtrlLst,
                    sizeof(PUKT_CTRL_REC) *
                    ( kgs.ulNumOfUKTs    +
                      kgs.ulNumOfDevis   +
                      kgs.ulNumOfAsyncis +
                      NUM_SPECIAL_THREADS ));

   if( rc != NO_ERROR )
     {
     DBGOUT;
     return ( rc );
     }
  #endif

  /* PTS 1103824 */
  if ( XPARAM(ulMaxMessageFiles) > 0 )
  {
      rc = ALLOC_MEM ( &kgs.MessageBufferSpace,
                   sizeof(teo15MessageBufferSpace) * XPARAM(ulMaxMessageFiles) );
      if( rc != NO_ERROR )
      {
         DBGOUT;
         return ( rc );
      }
  }
  else
  {
      kgs.MessageBufferSpace = NULL;
  }


  DBGOUT
  return (rc);
  }

/*------------------------------*/

static LONG  sql70k_init_dev_ctrls ( teo70k_ClusterInfo  *pCluInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_dev_ctrls"
  LONG                  rc            = NO_ERROR;
  PUCHAR                puchBuff      = NULL;
  LONG                  ulDeviCtrlSize;
  LONG                  ulDevIOCtrlSize;
  LONG                  ulDevWorkerCtrlSize;
  ULONG                 ulDev0IOQueSize;
  ULONG                 ulDeviIOQueSize;
  ULONG                 ulTotalSize;
  PDEVI_CTRL_REC        pDeviCtrl;
  PDEV_WORKER_CTRL_REC  pDevWorkerCtrl;
  PDEV_IO_CTRL_REC      pIOCtrl;
  PDEV_IO_CTRL_REC      pFirstIOCtrl;
  PDEV_IO_CTRL_REC      pLastIOCtrl;
  PIO_QUEUE_REC         pIOs;

  DBGIN;

  // --- Alloc space for the DEVi control records and IO-Queue elements
  ulDeviCtrlSize      = sizeof(DEVI_CTRL_REC)       * kgs.ulNumOfDevis;
  ulDevWorkerCtrlSize = sizeof(DEV_WORKER_CTRL_REC) * kgs.ulNumOfDevWorker;
  ulDevIOCtrlSize     = sizeof(DEV_IO_CTRL_REC)     * XPARAM(lMaxDevspaces);
  ulDev0IOQueSize     = sizeof(IO_QUEUE_REC)        * kgs.ulNumOfTasks;
  ulDeviIOQueSize     = sizeof(IO_QUEUE_REC)        * kgs.ulNumOfTasks *
                        kgs.ulNumOfDevis            * MAX_DEVI_REQUESTS;

  ulTotalSize = ulDeviCtrlSize      +
                ulDevWorkerCtrlSize +
                ulDevIOCtrlSize +
                ulDev0IOQueSize +
                ulDeviIOQueSize;

  rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (puchBuff, 0, ulTotalSize);

  // --- use overlapped io?
  if ( kgs.ulNumOfDevWorker != 0 )
    {
    kgs.pFirstDevWorkerCtrl = (PDEV_WORKER_CTRL_REC) puchBuff;
    kgs.pLastDevWorkerCtrl  = kgs.pFirstDevWorkerCtrl+kgs.ulNumOfDevWorker -1;
    puchBuff                = (PUCHAR)(kgs.pFirstDevWorkerCtrl +
                                        kgs.ulNumOfDevWorker);

    //
    // --- Initialize DEV_WORKER's
    //
    for ( pDevWorkerCtrl  = kgs.pFirstDevWorkerCtrl;
          pDevWorkerCtrl <= kgs.pLastDevWorkerCtrl;
          pDevWorkerCtrl++)
      {
      sql70k_init_thrd_ctrl_header ( &pDevWorkerCtrl->ThrdCtrlHeader,
                                      DEV_WORKER_THRD_PRIO,
                                      KT_INITIALIZED,
                                      0,
                                      UNDEF,
                                      INVALID_HANDLE_VALUE);
      }
    }

  kgs.pFirstDeviCtrl = (PDEVI_CTRL_REC) puchBuff;
  kgs.pLastDeviCtrl  = kgs.pFirstDeviCtrl + kgs.ulNumOfDevis - 1;
  puchBuff           = (PUCHAR)(kgs.pFirstDeviCtrl + kgs.ulNumOfDevis);

  //
  // --- Initialize DEVi's
  //
  for ( pDeviCtrl  = kgs.pFirstDeviCtrl;
        pDeviCtrl <= kgs.pLastDeviCtrl;
        pDeviCtrl++)
    {
    pIOs     = (PIO_QUEUE_REC)puchBuff;
    puchBuff = (PUCHAR)(pIOs + (kgs.ulNumOfTasks * MAX_DEVI_REQUESTS));

    sql70k_init_devi_ctrl ( pDeviCtrl, pIOs );
    }

  pFirstIOCtrl           = (PDEV_IO_CTRL_REC) puchBuff;
  pLastIOCtrl            = pFirstIOCtrl + XPARAM(lMaxDevspaces) - 1;
  puchBuff               = (PUCHAR)(pFirstIOCtrl + XPARAM(lMaxDevspaces));

  //
  // --- Initialize DEVIO's
  //
  for ( pIOCtrl  = pFirstIOCtrl;
        pIOCtrl <= pLastIOCtrl;
        pIOCtrl++)
    {
    sql70k_init_dev_io ( pIOCtrl );
    }


  //
  // --- Initialize DEV0
  //
  sql70k_init_thrd_ctrl_header ( &kgs.Dev0.ThrdCtrlHeader,
                                DEV0_THRD_PRIO,
                                KT_INITIALIZED,
                                0,
                                UNDEF,
                                INVALID_HANDLE_VALUE);

  kgs.Dev0.pIOs = (PIO_QUEUE_REC)puchBuff;
  puchBuff      = (PUCHAR)(kgs.Dev0.pIOs + kgs.ulNumOfTasks);

  sql73k_IO_queue_init ( kgs.Dev0.pIOs, kgs.ulNumOfTasks );

  kgs.Dev0.pIOCtrl              = pFirstIOCtrl;
  kgs.Dev0.pIOWorkQue           = kgs.Dev0.pIOs;
  kgs.Dev0.pIOReqQue            = kgs.Dev0.pIOs;
  kgs.Dev0.lExclusive           = 0;
  kgs.Dev0.lTraceDevspaceNo     = UNDEF;

  DBGOUT;

  return ( rc );
  }

/*------------------------------*/

static LONG  sql70k_init_async_ctrls ( teo70k_ClusterInfo  *pCluInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_async_ctrls"
  LONG              rc             = NO_ERROR;
  PUCHAR            puchBuff       = NULL;
  LONG              ulAsyncCtrlSize;
  ULONG             ulAsync0IOQueSize;
  ULONG             ulAsynciIOQueSize;
  ULONG             ulTotalSize;
  PASYNCI_CTRL_REC  pAsynciCtrl;
  PIO_QUEUE_REC     pIOs;
  ULONG             ulIOs;


  DBGIN;

  // --- Alloc space for the ASYNCi control records and IO-Queue elements
  ulAsyncCtrlSize     = sizeof(ASYNCI_CTRL_REC)* kgs.ulNumOfAsyncis;
  ulAsync0IOQueSize   = sizeof(IO_QUEUE_REC)   * kgs.ulNumOfTasks;
  ulAsynciIOQueSize   = sizeof(IO_QUEUE_REC)   * kgs.ulNumOfTasks *
                        kgs.ulNumOfAsyncis     * (MAX_ASYN_REQUESTS + ASYN_WAITS);
  ulTotalSize        = ulAsyncCtrlSize   + ulAsync0IOQueSize + ulAsynciIOQueSize;

  rc = ALLOC_MEM ((PPVOID)&puchBuff, ulTotalSize );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( rc );
    }

  // --- init memory
  memset (puchBuff, 0, ulTotalSize);

  kgs.pFirstAsynciCtrl = (PASYNCI_CTRL_REC) puchBuff;
  kgs.pLastAsynciCtrl  = kgs.pFirstAsynciCtrl + kgs.ulNumOfAsyncis - 1;
  puchBuff             = (PUCHAR)(kgs.pFirstAsynciCtrl + kgs.ulNumOfAsyncis);

  //
  // --- Initialize ASYNC0
  //
  sql70k_init_thrd_ctrl_header ( &kgs.Async0.ThrdCtrlHeader,
                                 ASYNC0_THRD_PRIO,
                                 KT_INITIALIZED,
                                 0,
                                 UNDEF,
                                 INVALID_HANDLE_VALUE);

  kgs.Async0.pIOs = (PIO_QUEUE_REC)puchBuff;
  puchBuff        = (PUCHAR)(kgs.Async0.pIOs + kgs.ulNumOfTasks);

  sql73k_IO_queue_init ( kgs.Async0.pIOs, kgs.ulNumOfTasks );

  kgs.Async0.pIOWorkQue           = kgs.Async0.pIOs;
  kgs.Async0.pIOReqQue            = kgs.Async0.pIOs;
  kgs.Async0.lExclusive           = 0;

  //
  // --- Initialize ASYNCi's
  //
  ULONG asyniRequests       = XPARAM(lMaxDevspaces) * MAX_ASYN_REQUESTS;
  ULONG totalAsynciRequests = asyniRequests * kgs.ulNumOfAsyncis;

  RTETask_CommonQueueElement*  pAsynciRequests = newarray ( pAsynciRequests, totalAsynciRequests,
                                                            RTEMem_RteAllocator::Instance() );
  if ( 0 == pAsynciRequests )
    {
    MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

  pIOs   = (PIO_QUEUE_REC)puchBuff;
  ulIOs  = kgs.ulNumOfTasks  * (MAX_ASYN_REQUESTS + ASYN_WAITS);

  for (pAsynciCtrl =  kgs.pFirstAsynciCtrl;
       pAsynciCtrl <= kgs.pLastAsynciCtrl;
       pAsynciCtrl++, pIOs += ulIOs, pAsynciRequests += asyniRequests)
    {
    pAsynciCtrl->pIOs            = pIOs;
    pAsynciCtrl->ulIOs           = ulIOs;
    pAsynciCtrl->pAsynRequests   = pAsynciRequests;
    pAsynciCtrl->ulAsynRequests  = asyniRequests;

    sql70k_init_asynci_ctrl( pAsynciCtrl );
    }

  DBGOUT;

  return ( rc );
  }

/*------------------------------*/

static VOID sql70k_init_thrd_ctrl_header ( PTHREAD_CTRL_HEADER_REC pThrdCtrlHd,
                                           LONG                    lThrdPrio,
                                           LONG                    lThrdState,
                                           tsp00_Longuint          ulStartTime,
                                           LONG                    Tid,
                                           HANDLE                  hThrd )
  {
  pThrdCtrlHd->Tid                = Tid;
  pThrdCtrlHd->hThrd              = hThrd;
  pThrdCtrlHd->lThrdPrio          = lThrdPrio;
  pThrdCtrlHd->ThrdState          = lThrdState;
  pThrdCtrlHd->ulStartTime        = ulStartTime;
  pThrdCtrlHd->hComm              = INVALID_HANDLE_VALUE;
  pThrdCtrlHd->hevSem             = INVALID_HANDLE_VALUE;
  }

/*------------------------------*/

static  LONG sql70k_init_task ( PTASK_CTRL_REC  pTaskCtrl,
                                ULONG           ulTaskIndex,
                                UINT1           TaskType,
                                tsp00_Longuint  ulTaskStackSize,
                                PUKT_CTRL_REC   pUKTCtrl,
                                PPVOID          ppCommCtrl,
                                PULONG          pulCommFlagNo )
  {
  #undef  MF__
  #define MF__ MOD__"sql70k_init_task"

  DBGIN;

  memset(pTaskCtrl, 0, sizeof(TASK_CTRL_REC) );

  pTaskCtrl->ulTaskIndex     = ulTaskIndex;
  pTaskCtrl->TaskType        = TaskType;
  pTaskCtrl->ulTaskStackSize = ALIGN( ulTaskStackSize, kgs.ulSystemPageSize);
  pTaskCtrl->pUKT            = pUKTCtrl;
  pTaskCtrl->MoveLock.Val    = 0;  // reset lock

  pTaskCtrl->MoveLock.Type.LongTerm = TASK_IS_INACTIVE;        // Set move lock. Will be removed
                                                               // by the first 'vinit'
  if ( pTaskCtrl->TaskType != TT_US_EO00 )
      pTaskCtrl->MoveLock.Type.LongTerm |= NEVER_MOVE_TASK;

  // --- initialize the "dynamic" part
  sql70k_init_task_ctrl      ( pTaskCtrl );
  sql70k_init_task_ctrl_cntr ( pTaskCtrl );

  // GetTaskDLQElementX needs this object first!
  pTaskCtrl->pTaskObject = new (RTEMem_RteAllocator::Instance()) RTETask_Task ( pTaskCtrl->ulTaskIndex, static_cast<RTETask_Task::TaskType>(pTaskCtrl->TaskType), pTaskCtrl );

  if ( 0 == pTaskCtrl->pTaskObject )
    {
    MSGD (( ERR_ALLOC_MEMORY, ERROR_NOT_ENOUGH_MEMORY ));
    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

  // this new structure allows two outstanding overlapped I/O per task
  pTaskCtrl->OvIO[0].pTaskCtrl     = pTaskCtrl; // back pointer for overlapped IO
  pTaskCtrl->OvIO[0].pRequest      = GetTaskDLQElement1( pTaskCtrl );; // queuing element used for overlapped I/O

  pTaskCtrl->OvIO[1].pTaskCtrl     = pTaskCtrl; // back pointer for overlapped IO
  pTaskCtrl->OvIO[1].pRequest      = GetTaskDLQElement2( pTaskCtrl ); // queuing element used for second overlapped I/O


  switch ( TaskType )
    {
    case TT_SV_EO00:
      // --- mark as inactive
      pTaskCtrl->TaskState = TSK_INACTIVE_EO00;
      break;
    case TT_US_EO00:
    case TT_UT_EO00:
    case TT_EV_EO00:
      // --- mark as inactive
      pTaskCtrl->TaskState = TSK_INACTIVE_EO00;

      // --- set the communication control
      pTaskCtrl->pUserCommCtrl     = (PUSER_TASK_COMM_CTRL)*ppCommCtrl;
      *ppCommCtrl                  = (PUSER_TASK_COMM_CTRL)*ppCommCtrl + 1;

      pTaskCtrl->ulCommFlagNo = *pulCommFlagNo;
      (*pulCommFlagNo)++;

      sql70k_init_user_comm_ctrl ( pTaskCtrl->pUserCommCtrl );

      break;
    }

  DBGOUT;

  return (NO_ERROR);
  }


/*
 * =============================== END ========================================
 */
