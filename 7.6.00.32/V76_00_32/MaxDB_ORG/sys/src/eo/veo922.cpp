/*!
  @file           veo922.cpp
  @author         JoergM
  @special area   internal sysmon / console
  @brief          description ...
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Tasking/RTETask_Task.hpp"
#include "gsp100.h"
#include "heo922.h"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo57.h"
#include "geo002.h"
#include "heo92.h"
#include "heo47.h"
#include "heo00x.h"

#ifdef WIN32
#include "gos003.h" /* nocheck */
#include <time.h>
#else
#include "gen45.h" /* nocheck */
#include "gen003.h" /* nocheck */
#endif
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/RTE_ConsoleStandardOutput.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define ZERO_IF_RESET_PENDING(pTaskCtrl_, value_) \
 ( ( EO52_RESET_TASK_FULL_PENDING == (pTaskCtrl_)->TaskStatRecResetPending ) ? 0 : (value_) )

//===========
#ifdef WIN32
//===========
#define MAX_STORAGE_INFOS_EO922       8
#define MAX_SPEC_THREAD_INFOS_EO922   7

#define DEVSP_NAME(_DevNo)        (KGS->Dev0.pIOCtrl[_DevNo].szDevspace)

#define SEMA_NAME(_SemId )       (KGS->pSemaphores[ _SemId ].szSemaName )
#define FIRST_UKT                (KGS->pFirstUKTCtrl)
#define LAST_UKT                 (KGS->pLastUKTCtrl)
#define UKT_INDEX(_Ukt)          (_Ukt->ulUKTIndex)
#define FIRST_DEV                (KGS->pFirstDeviCtrl)
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED )

#define TASK_QUQUE_LOCATION(_pTaskCtrl) (_pTaskCtrl)->QueLocation
#define TASK_STATE_REASON(_pTaskCtrl)   (_pTaskCtrl)->ulStateReason
#define TASK_INDEX(_pTaskCtrl)          (_pTaskCtrl)->ulTaskIndex
#define TASK_SELF_SUSP_CNT(_pTaskCtrl)  ZERO_IF_RESET_PENDING((_pTaskCtrl),(_pTaskCtrl)->ulSelfSuspCount)
#define TASK_DISP_CNT(_pTaskCtrl)       ZERO_IF_RESET_PENDING((_pTaskCtrl),(_pTaskCtrl)->ulDispatcherCalls)

#define FIRST_REGION                    KGS->pSemaphores


#define  PTR_TO_PEER_NODE(_ActTask) ( _ActTask->pUserCommCtrl->szClientNode )
#define  PTR_TO_DB_NODE                 (KGS->szServerNode)
//===========
#else
//===========
#define MAX_STORAGE_INFOS_EO922       8
#define MAX_SPEC_THREAD_INFOS_EO922   5

#define DEVSP_NAME(_DevNo) (KGS->dev0.dev[_DevNo].devspace)

#define SEMA_NAME(_SemId ) (KGS->semaphore [ _SemId].semaname)
#define IS_SEMA_IN_USE(_SemId )  (kgs->semaphore [ _SemId].semaname[ 0 ] != '\0')

typedef struct DEV_PCB      *PDEVI_CTRL_REC ;
typedef struct DEV_PCB      *PASYNCI_CTRL_REC;
typedef ten50_UKT_Control   *PUKT_CTRL_REC ;
typedef struct DDB4_SEMA    *PSEM_CTRL_REC ;
#define FIRST_UKT                (KGS->first_kp)
#define LAST_UKT                 (KGS->last_kp)
#define UKT_INDEX(_Ukt)          (_Ukt->index)
#define FIRST_DEV                (KGS->first_dev)
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->state != KT_INITIALIZED )

#define TASK_QUQUE_LOCATION(_pTaskCtrl) (_pTaskCtrl)->is_in_queue
#define TASK_STATE_REASON(_pTaskCtrl)   (_pTaskCtrl)->state_reason
#define TASK_INDEX(_pTaskCtrl)          (_pTaskCtrl)->index
#define TASK_SELF_SUSP_CNT(_pTaskCtrl)  ZERO_IF_RESET_PENDING((_pTaskCtrl),(_pTaskCtrl)->self_susp_cnt)
#define TASK_DISP_CNT(_pTaskCtrl)       ZERO_IF_RESET_PENDING((_pTaskCtrl),(_pTaskCtrl)->disp_counter)

#define FIRST_REGION                    (KGS->semaphore)

#define  PTR_TO_PEER_NODE(_ActTask) ( sql45_peer_node(_ActTask) )
#define  PTR_TO_DB_NODE                 (KGS->servernode.asCharp() )
//===========
#endif
//===========

#define TASK_REGION_ACCESS(_pTaskCtrl)  ZERO_IF_RESET_PENDING((_pTaskCtrl),(_pTaskCtrl)->ulBegExclCalls)
#define TASK_AT_INDEX(_IDX)      (KGS->pFirstTaskCtrl + _IDX - 1)
#define IS_DEVSP_IN_USE(_DevNo)  (DEVSP_NAME(_DevNo)[0] != '\0')
#if defined(_WIN32)
#define DEVI_PER_VOLUME          (XPARAM(lDeviPerDevspace))
#else
#define DEVI_PER_VOLUME          (KGS->devpsets)
#endif
#define LAST_DEV                 (FIRST_DEV + (XPARAM(lMaxDevspaces) * DEVI_PER_VOLUME - 1) )
/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static teo52_TaskStateStatisticRec eo922_EmptyTaskStateStatisticRecord = { 0 };

static teo52_TaskStateStatisticRec *eo922_GetTaskStateStatistic(TASK_CTRL ActTask)
{
    if ( EO52_NO_RESET_PENDING != ActTask->TaskStatRecResetPending )
    {
        return &eo922_EmptyTaskStateStatisticRecord;
    }
    else
    {
        return &ActTask->TaskStateStat;
    }
}

/*----------------*/

static teo52_TaskQueueStatistic eo922_EmptyTaskQueueStatisticRecord = { 0 };

static teo52_TaskQueueStatistic *eo922_GetTaskQueueStatistic(TASK_CTRL ActTask)
{
    if ( ActTask->QueueStatRecResetPending 
     || (EO52_RESET_TASK_FULL_PENDING == ActTask->TaskStatRecResetPending) )
    {
        return &eo922_EmptyTaskQueueStatisticRecord;
    }
    else
    {
        return &ActTask->QueStat;
    }
}

/*----------------*/

static bool eo922RteInfoRegionSearchNextSem ( const tsp00_Int4 StartSem,
                                              tsp00_Int4      *NextSem )
{
  ROUTINE_DBG_MEO60 (_T("eo922RteInfoRegionSearchNextSem"));

  bool Found = false ;

  for ( *NextSem = StartSem ;
        *NextSem < (tsp00_Longint )XPARAM(ulNoOfRegions) &&
        !(Found = *SEMA_NAME(*NextSem) != '\0' );
       (*NextSem)++ ) ;

  return ( Found ) ;
}

/*------------------------------*/

//=======================================================================
#ifdef WIN32
//=======================================================================

static void eo922GetRteOsSpecificInfoRegion ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("eo922GetRteOsSpecificInfoRegion"));

  PSEM_CTRL_REC                  pSemCtrl = FIRST_REGION + RteRegionInfo->RegionNo ;
  TASK_CTRL                      pOwnerTaskCtrl ;

  if ( (pOwnerTaskCtrl = pSemCtrl->pOwnerTaskCtrl) )
  {
    // -- Region is busy
    RteRegionInfo->OwnerTaskIndex = pOwnerTaskCtrl->ulTaskIndex;
    if ( RteRegionInfo->WaiterArray )
    {
       // -- check if somebody is waiting
       PSEM_QUEUE_REC                 pSemQueElem;
       TASK_CTRL                      pWaitTaskCtrl ;
       tsp00_Int4 *pWaitTasks = (tsp00_Int4 *) RteRegionInfo->WaiterArray ;

       for ( pSemQueElem   =  pSemCtrl->SemaList.pRemovePos, RteRegionInfo->NumWaiter = 0;
             pSemQueElem && (pWaitTaskCtrl = pSemQueElem->pTaskCtrl) != NULL &&
             RteRegionInfo->NumWaiter < (tsp00_Int4 )KGS->ulNumOfTasks ;
             pSemQueElem =  pSemQueElem->pNextElem, (RteRegionInfo->NumWaiter)++ )
      {
        pWaitTasks[RteRegionInfo->NumWaiter] = pWaitTaskCtrl->ulTaskIndex;
      }
    }
  }
  else
    RteRegionInfo->OwnerTaskIndex = undef_ceo00 ;

  RteRegionInfo->RegionName.c2p( pSemCtrl->szSemaName ) ;
  RteRegionInfo->Exclusive     = pSemCtrl->SemaList.lExclusive ;

  RteRegionInfo->GetCount      = REGION_ACCESS(pSemCtrl);
  RteRegionInfo->WaitCount     = REGION_WAITS(pSemCtrl) ;
  RteRegionInfo->CollisionCount= REGION_COLLISION(pSemCtrl);
  RteRegionInfo->TasCount      = REGION_BEGEXCL_TAS_COLL(pSemCtrl) +
                                 REGION_ENDEXCL_TAS_COLL(pSemCtrl);
}

//=======================================================================
#else
//=======================================================================

static void eo922GetRteOsSpecificInfoRegion ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("eo922GetRteOsSpecificInfoRegion"));

  struct DDB4_SEMA    *pSemCtrl = & kgs->semaphore [ RteRegionInfo->RegionNo ];
  TASK_CTRL            pOwnerTaskCtrl ;
  int                  Exclusive ;

  if ( (pOwnerTaskCtrl = (struct TASK_TYPE  *) pSemCtrl->owner) )
  {
    // -- Region is busy
    RteRegionInfo->OwnerTaskIndex = pOwnerTaskCtrl->index;

    if ( RteRegionInfo->WaiterArray )
    {
       // -- check if somebody is waiting
       TASK_CTRL              pWaitTaskCtrl ;
       struct N_QUEUE        *pSemQueElem;
       tsp00_Int4 *pWaitTasks = (tsp00_Int4 *) RteRegionInfo->WaiterArray ;

       for ( pSemQueElem = pSemCtrl->semaliste.last, RteRegionInfo->NumWaiter = 0 ;
             pSemQueElem && (pWaitTaskCtrl = pSemQueElem->taskid) != NULL &&
             RteRegionInfo->NumWaiter < (tsp00_Int4 )KGS->ulNumOfTasks ;
             pSemQueElem = pSemQueElem->next, (RteRegionInfo->NumWaiter)++ )
      {
        pWaitTasks[RteRegionInfo->NumWaiter] = pWaitTaskCtrl->index;
      }
    }
  }
  else
    RteRegionInfo->OwnerTaskIndex = undef_ceo00;

  RteRegionInfo->RegionName.c2p( pSemCtrl->semaname ) ;

#if   defined(NMP)
      Exclusive = pSemCtrl->semaliste.sem_exclusive[0].sl_lock == 1 ;
#elif defined(HPUX) && !defined (HP_IA64)
      Exclusive = *ADDR64(pSemCtrl->semaliste.sem_exclusive) == 0 ;
#else
      Exclusive = pSemCtrl->semaliste.sem_exclusive [0] == 1 ;
#endif
  RteRegionInfo->Exclusive     = Exclusive ;

  RteRegionInfo->GetCount      = REGION_ACCESS(pSemCtrl);
  RteRegionInfo->WaitCount     = REGION_WAITS(pSemCtrl) ;
  RteRegionInfo->CollisionCount= REGION_COLLISION(pSemCtrl);
  RteRegionInfo->TasCount      = REGION_BEGEXCL_TAS_COLL(pSemCtrl) +
                                 REGION_ENDEXCL_TAS_COLL(pSemCtrl);
}

//=======================================================================
#endif
//=======================================================================

static bool eo922GetRteInfoRegion ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("eo922GetRteInfoRegion"));

  bool Found = eo922RteInfoRegionSearchNextSem (RteRegionInfo->LastRegionGiven, 
                                               &RteRegionInfo->RegionNo ) ;

  if ( Found )
  {
    float                      CollisionPercent;

    RteRegionInfo->NumWaiter       = 0 ; /* PTS 1119583 */
    RteRegionInfo->LastWaiterGiven = 0 ;
    RteRegionInfo->WaitTaskIndex   = undef_ceo00 ;
    
	eo922GetRteOsSpecificInfoRegion (RteRegionInfo);

    if ( RteRegionInfo->GetCount )
      CollisionPercent = (float) RteRegionInfo->CollisionCount * 100.0f /
                         (float) RteRegionInfo->GetCount;
    else
      CollisionPercent = 0.0 ;

    RteRegionInfo->CollisionRate = (tsp00_Longreal ) CollisionPercent ;

    RteRegionInfo->RegionNo++ ;
    RteRegionInfo->LastRegionGiven = RteRegionInfo->RegionNo ;
  }

  return ( Found ) ;
}


/*------------------------------*/

static void eo922GetRteInfoRegionWaiter ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("eo922GetRteInfoRegionWaiter"));

  tsp00_Int4 *pWaitTasks = (tsp00_Int4 *) RteRegionInfo->WaiterArray ;

  RteRegionInfo->WaitTaskIndex   = pWaitTasks[RteRegionInfo->LastWaiterGiven] ;
  RteRegionInfo->LastWaiterGiven++;
  RteRegionInfo->GetCount      = 0;
  RteRegionInfo->TasCount      = 0;
  RteRegionInfo->WaitCount     = 0;
  RteRegionInfo->CollisionCount= 0;
  RteRegionInfo->Exclusive     = 0;
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

// ============================= REGION ======================================
externC tsp00_Bool vGetRteInfoRegionNext ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoRegionNext"));

  bool Found = RteRegionInfo->LastWaiterGiven < RteRegionInfo->NumWaiter ;

  if ( Found )
    eo922GetRteInfoRegionWaiter ( RteRegionInfo) ;
  else
    Found = eo922GetRteInfoRegion ( RteRegionInfo ) ;

  if ( !Found && RteRegionInfo->WaiterArray ) 
  {
    FREE_MEM_EO57 ( (void *)RteRegionInfo->WaiterArray ) ;
  }

  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoRegionFirst ( tsp92_RteRegionInfo *RteRegionInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoRegionFirst"));

  SAPDB_MemFillNoCheck ( RteRegionInfo, 0, sizeof( tsp92_RteRegionInfo ) ) ;

  ALLOC_MEM_EO57 ((void **)&(RteRegionInfo->WaiterArray),
                   KGS->ulNumOfTasks * sizeof(tsp00_Int4) );

  return ( vGetRteInfoRegionNext ( RteRegionInfo ) ) ;
}


// ================================ IO =======================================

static bool eo922RteInfoIOSearchNextDev ( const tsp00_Int4 StartDev,
                                          tsp00_Int4      *NextDev )
{
  ROUTINE_DBG_MEO60 (_T("eo922RteInfoIOSearchNextDev"));

  bool Found = false ;

  for ( *NextDev = StartDev ;
        *NextDev < (tsp00_Longint )XPARAM(lMaxDevspaces) &&
        !(Found = IS_DEVSP_IN_USE(*NextDev));
       (*NextDev)++ ) ;

  return ( Found ) ;
}

/*------------------------------*/

static void eo922GetRteOsSpecificUKTIO( PUKT_CTRL_REC   pTmpUKTCtrl,
                                        tsp00_Int4      DevNo,
                                        SAPDB_UInt8    *UKTReadCnt ,
                                        SAPDB_UInt8    *UKTReadPages ,
                                        SAPDB_UInt8    *UKTWriteCnt,
                                        SAPDB_UInt8    *UKTWritePages)
{
  if ( !pTmpUKTCtrl->pSingleIO[DevNo].selfIOStatisticResetPending )
  {
      *UKTReadCnt   = pTmpUKTCtrl->pSingleIO[DevNo].ReadSelfIo.IOStat.TimeStat.ulCount;
      *UKTReadPages = pTmpUKTCtrl->pSingleIO[DevNo].ReadSelfIo.ulPageCount;
      *UKTWriteCnt  = pTmpUKTCtrl->pSingleIO[DevNo].WriteSelfIo.IOStat.TimeStat.ulCount;
      *UKTWritePages= pTmpUKTCtrl->pSingleIO[DevNo].WriteSelfIo.ulPageCount;
  }
  else
  {
      *UKTReadCnt   = 0;
      *UKTReadPages = 0;
      *UKTWriteCnt  = 0;
      *UKTWritePages= 0;
  }
}

/*------------------------------*/

static bool eo922GetRteInfoIO( tsp00_Int4          DevNo,
                               tsp00_8ByteCounter *ReadCnt ,
                               tsp00_8ByteCounter *ReadPageCnt ,
                               tsp00_8ByteCounter *WriteCnt ,
                               tsp00_8ByteCounter *WritePageCnt )
{
  ROUTINE_DBG_MEO60 (_T("eo922GetRteInfoIO"));

  SAPDB_UInt8 ulReadCnt  = 0, ulReadPageCnt = 0, 
              UKTReadCnt = 0, UKTReadPageCnt= 0; 
  SAPDB_UInt8 ulWriteCnt = 0, ulWritePageCnt= 0, 
              UKTWriteCnt= 0, UKTWritePageCnt=0 ;
  PDEVI_CTRL_REC     pDeviCtrl;
  tsp00_Longuint     ulDevi;
  PUKT_CTRL_REC      pTmpUKTCtrl;
#if defined(_WIN32)
  tsp00_Longuint lastDevi = (tsp00_Longuint)DEVI_PER_VOLUME;
#else
  tsp00_Longuint lastDevi = (tsp00_Longuint)(DevNo ? DEVI_PER_VOLUME : 1);
#endif
  // --- get counts of DEVi's
  for ( ulDevi = 0; ulDevi < lastDevi; ulDevi++ )
  {
    pDeviCtrl = FIRST_DEV + (DevNo * DEVI_PER_VOLUME) + ulDevi;
    if ( DEVI_IN_USE(pDeviCtrl) && !pDeviCtrl->DevIOStat.ResetPending )
    {
      ulReadCnt      += pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulCount;
      ulReadPageCnt  += pDeviCtrl->DevIOStat.DevRead.ulPageCount;
      ulWriteCnt     += pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulCount;
      ulWritePageCnt += pDeviCtrl->DevIOStat.DevWrite.ulPageCount ;
    }
  }

  // --- get counts of UKT's ( single block I/O )
  for ( pTmpUKTCtrl = FIRST_UKT ; pTmpUKTCtrl <= LAST_UKT ; pTmpUKTCtrl++ )
  {
    eo922GetRteOsSpecificUKTIO( pTmpUKTCtrl, DevNo, &UKTReadCnt, &UKTReadPageCnt, 
                                                    &UKTWriteCnt,&UKTWritePageCnt ) ;
    ulReadCnt      += UKTReadCnt ;
    ulReadPageCnt  += UKTReadPageCnt ;
    ulWriteCnt     += UKTWriteCnt;
    ulWritePageCnt += UKTWritePageCnt ;
  }

  *ReadCnt      = ( tsp00_8ByteCounter) ulReadCnt ;
  *ReadPageCnt  = ( tsp00_8ByteCounter) ulReadPageCnt ;
  *WriteCnt     = ( tsp00_8ByteCounter) ulWriteCnt ;
  *WritePageCnt = ( tsp00_8ByteCounter) ulWritePageCnt ;
 
  return ( ulReadCnt || ulWriteCnt ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoIONext ( tsp92_RteIOInfo *RteIOInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoIONext"));

  bool Found, IODone ;
      
  do
  {
    Found = eo922RteInfoIOSearchNextDev ( RteIOInfo->NextDevNo, &RteIOInfo->DevNo);

    if ( Found ) 
    {
      RteIOInfo->DevspaceName.c2p( DEVSP_NAME(RteIOInfo->DevNo) ) ;
      IODone = eo922GetRteInfoIO ( RteIOInfo->DevNo, 
                                  &RteIOInfo->ReadCnt , &RteIOInfo->ReadPageCnt,
                                  &RteIOInfo->WriteCnt, &RteIOInfo->WritePageCnt) ;
      RteIOInfo->NextDevNo = RteIOInfo->DevNo + 1 ;
    }
  }
  while ( Found && !IODone );
 
  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoIOFirst ( tsp92_RteIOInfo *RteIOInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoIOFirst"));

  SAPDB_MemFillNoCheck ( RteIOInfo, 0, sizeof( tsp92_RteIOInfo ) ) ;

  return ( vGetRteInfoIONext ( RteIOInfo ) ) ;
}

// ================================ BACKUP ================================

externC tsp00_Bool vGetRteInfoBackupNext ( tsp92_RteBackupInfo *RteBackupInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoBackupNext"));

  PASYNCI_CTRL_REC    pAsynciCtrl;
  bool                Found = false ;
  teo52_DevIOStatistic *DevIOStat ;

  RteBackupInfo->DevNo = RteBackupInfo->NextDevNo;

//=======================================================================
#ifdef WIN32
//=======================================================================

  for ( pAsynciCtrl =  KGS->pFirstAsynciCtrl + RteBackupInfo->DevNo; 
        RteBackupInfo->DevNo < (tsp00_Int4 ) KGS->ulNumOfAsyncis &&
       !(Found = !pAsynciCtrl->fAsyncClosePending && 
                  pAsynciCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED ) ;
        pAsynciCtrl ++, RteBackupInfo->DevNo++ ) ;

  if ( Found ) 
  {
    RteBackupInfo->DevspaceName.c2p(pAsynciCtrl->szLogDeviceName);
    DevIOStat = &pAsynciCtrl->DevIOStat;
  }

//=======================================================================
#else
//=======================================================================

  for ( pAsynciCtrl =  kgs->first_asdev + RteBackupInfo->DevNo; 
        pAsynciCtrl <= kgs->last_dev && 
       !(Found = pAsynciCtrl->state != KT_INITIALIZED) ; 
        pAsynciCtrl ++, RteBackupInfo->DevNo++ ) ;

   if ( Found ) 
   {
     RteBackupInfo->DevspaceName.c2p(KGS->dev0.dev[kgs->maxdevspaces + RteBackupInfo->DevNo].devspace);
     DevIOStat = &pAsynciCtrl->DevIOStat ;
   }

//=======================================================================
#endif
//=======================================================================

  if ( Found )
  {
   RteBackupInfo->ReadCnt    = (tsp00_8ByteCounter )DevIOStat->DevRead.IOStat.TimeStat.ulCount;
   RteBackupInfo->WriteCnt   = (tsp00_8ByteCounter )DevIOStat->DevWrite.IOStat.TimeStat.ulCount; 
   RteBackupInfo->PageCnt    = (tsp00_8ByteCounter )(DevIOStat->DevRead.ulPageCount +
                                                     DevIOStat->DevWrite.ulPageCount) ;

   RteBackupInfo->QueLen     = (tsp00_Int4 ) DevIOStat->ulIOQueLen ;
   RteBackupInfo->MaxQueLen  = (tsp00_Int4 ) DevIOStat->ulMaxIOQueLen ;
   RteBackupInfo->NextDevNo = RteBackupInfo->DevNo + 1 ;
  }

  return ( Found ) ;
}

/*------------------------------*/


externC tsp00_Bool vGetRteInfoBackupFirst ( tsp92_RteBackupInfo *RteBackupInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoBackupFirst"));

  SAPDB_MemFillNoCheck ( RteBackupInfo, 0, sizeof( tsp92_RteBackupInfo ) ) ;

  return ( vGetRteInfoBackupNext ( RteBackupInfo ) ) ;
}

// ================================ STORAGE ================================

   
typedef struct 
{
   const char   *StorageName;
   tsp00_Int4    StorageVal;
   const char   *StorageUnit;
} StorageInfo;


static StorageInfo StorageInfoArray[MAX_STORAGE_INFOS_EO922] ;

/*------------------------------*/

static void eo922FillStorageArray ( tsp00_Int4  ArrayIndex ,
                                    const char *StorageName,
                                    tsp00_Int4  StorageVal,
                                    const char *StorageUnit )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoStorageFirst"));

  if ( ArrayIndex < MAX_STORAGE_INFOS_EO922 )
  {
      StorageInfo *ActStorageInfo = &StorageInfoArray[ArrayIndex] ;
      ActStorageInfo->StorageName = StorageName ;
      ActStorageInfo->StorageUnit = StorageUnit ;
      ActStorageInfo->StorageVal  = StorageVal  ;
  }
}


/*------------------------------*/

externC tsp00_Bool vGetRteInfoStorageNext ( tsp92_RteStorageInfo *RteStorageInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoStorageNext"));

  bool                Found = RteStorageInfo->StorageIdx < MAX_STORAGE_INFOS_EO922 ;

  if ( Found )
  {
    StorageInfo *ActStorageInfo = &StorageInfoArray[RteStorageInfo->StorageIdx] ;

    RteStorageInfo->StorageName.c2p(ActStorageInfo->StorageName );
    RteStorageInfo->StorageUnit.c2p(ActStorageInfo->StorageUnit );
    RteStorageInfo->StorageValue = ActStorageInfo->StorageVal ;
    RteStorageInfo->StorageIdx++ ;
  }

  return ( Found ) ;

}

/*------------------------------*/

//=======================================================================
#ifdef WIN32
//=======================================================================
#define KGS_SIZE       (tsp00_Int4 )(sizeof( kgs ) )
#define DYN_POOL_FREE  (tsp00_Int4 )((tsp00_Longuint)kgs.pSharedDynPoolTop - (tsp00_Longuint)kgs.pSharedDynPoolPtr) 
#define DYN_POOL_USED  (tsp00_Int4 )((tsp00_Longuint)kgs.pSharedDynPoolPtr - (tsp00_Longuint)kgs.pSharedDynPoolBot)
#define DYN_DATA_FREE  (tsp00_Int4 )((tsp00_Longuint)kgs.pSharedDynDataTop - (tsp00_Longuint)kgs.pSharedDynDataPtr) / XPARAM(ulDBPageSize)
#define DYN_DATA_USED  (tsp00_Int4 )((tsp00_Longuint)kgs.pSharedDynDataPtr - (tsp00_Longuint)kgs.pSharedDynDataBot) / XPARAM(ulDBPageSize)
//=======================================================================
#else
//=======================================================================
#define KGS_SIZE       (tsp00_Int4 )(KGS-> seg_size )
#define DYN_POOL_FREE  (tsp00_Int4 )(KGS-> dynpool_top - KGS-> dynpool_ptr)
#define DYN_POOL_USED  (tsp00_Int4 )(KGS-> dynpool_ptr - KGS-> dynpool_bot) 
#define DYN_DATA_FREE  (tsp00_Int4 )(KGS-> dyndata_top - KGS-> dyndata_ptr)
#define DYN_DATA_USED  (tsp00_Int4) (KGS-> dyndata_ptr - KGS-> dyndata_bot)
//=======================================================================
#endif
//=======================================================================

externC tsp00_Bool vGetRteInfoStorageFirst ( tsp92_RteStorageInfo *RteStorageInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoStorageFirst"));

  SAPDB_MemFillNoCheck ( RteStorageInfo, 0, sizeof( tsp92_RteStorageInfo ) ) ;

  eo922FillStorageArray( 0, XCONS_STOR_DES_EXISTING_TASKS ,(tsp00_Int4 )KGS-> ulNumOfTasks, XCONS_STOR_VDES_EXISTING_TASKS    );
  eo922FillStorageArray( 1, XCONS_STOR_DES_KGS               , KGS_SIZE      / 1024, XCONS_STOR_VDES_KGS  );
  eo922FillStorageArray( 2, XCONS_STOR_DES_SHRD_DYN_POOLFREE , DYN_POOL_FREE / 1024, XCONS_STOR_VDES_SHRD_DYN_POOLFREE );
  eo922FillStorageArray( 3, XCONS_STOR_DES_SHRD_DYN_POOL_USED, DYN_POOL_USED / 1024, XCONS_STOR_VDES_SHRD_DYN_POOL_USED);
  eo922FillStorageArray( 4, XCONS_STOR_DES_SHRD_DYN_DATFREE  , DYN_DATA_FREE, XCONS_STOR_VDES_SHRD_DYN_DATFREE  );
  eo922FillStorageArray( 5, XCONS_STOR_DES_SHRD_DYN_DAT_USED , DYN_DATA_USED, XCONS_STOR_VDES_SHRD_DYN_DAT_USED );
  eo922FillStorageArray( 6, XCONS_STOR_DES_TSK_STACK_SIZE    , 0, XCONS_STOR_VDES_TSK_STACK_SIZE    ); 

//=======================================================================
#ifdef WIN32
//=======================================================================
  eo922FillStorageArray( 7, XCONS_STOR_DES_KSS               , 
                            ALIGN(kgs.ulKSSSize, kgs.ulSystemPageSize) / 1024,
                            XCONS_STOR_VDES_KSS               );
//=======================================================================
#else
//=======================================================================
  eo922FillStorageArray( 7, XCONS_STOR_DES_CODESIZE          , 
                            (tsp00_Int4)(KGS-> codesize / 1024), 
                            XCONS_STOR_VDES_CODESIZE          );
//=======================================================================
#endif
//=======================================================================

  return ( vGetRteInfoStorageNext ( RteStorageInfo ) ) ;
}

// ================================ Special Threads ================================

//=======================================================================
#ifdef WIN32
//=======================================================================
#define COORD_TID    kgs.Coord.ThrdCtrlHeader.Tid
#define REQU_TID     kgs.Requestor.ThrdCtrlHeader.Tid
#define TIMER_TID    kgs.Timer.ThrdCtrlHeader.Tid
#define DEV0_TID     kgs.Dev0.ThrdCtrlHeader.Tid
#define CONSOLE_TID  kgs.Console.ThrdCtrlHeader.Tid
#define THREAD_TID(_Thread)   ( _Thread->ThrdCtrlHeader.Tid )
//=======================================================================
#else
//=======================================================================
#define COORD_TID    kgs->coord.tid
#define REQU_TID     kgs->requestor.tid
#define TIMER_TID    kgs->timer.tid
#define DEV0_TID     kgs->dev0.tid
#define CONSOLE_TID  kgs->console.tid
#define THREAD_TID(_Thread)   ( _Thread->tid )
//=======================================================================
#endif
//=======================================================================

externC tsp00_Bool vGetRteInfoSpecThreadsNext ( tsp92_RteSpecThreadsInfo *RteSpecThreadsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoStorageNext"));

  bool                Found = RteSpecThreadsInfo->SpecThreadIdx < MAX_SPEC_THREAD_INFOS_EO922 ;

  if ( Found )
  {
    switch ( RteSpecThreadsInfo->SpecThreadIdx )
    {
    case 0 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_COORD) ;
              RteSpecThreadsInfo->SpecThreadId = COORD_TID ;
        break ;
    case 1 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_REQUESTOR) ;
              RteSpecThreadsInfo->SpecThreadId = REQU_TID ;
        break ;
    case 2 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_TIMER) ;
              RteSpecThreadsInfo->SpecThreadId = TIMER_TID ;
        break ;
    case 3 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_CONSOLE) ;
              RteSpecThreadsInfo->SpecThreadId = CONSOLE_TID ;
        break ;
    case 4 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_DEV0) ;
              RteSpecThreadsInfo->SpecThreadId = DEV0_TID ;
        break ;
//=======================================================================
#ifdef WIN32
//=======================================================================
    case 5 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_ASYNC0);
              RteSpecThreadsInfo->SpecThreadId = kgs.Async0.ThrdCtrlHeader.Tid ;
        break ;
    case 6 :  RteSpecThreadsInfo->SpecThreadName.c2p(XCONS_SHOW_RTE_KT_CLOCK);
              RteSpecThreadsInfo->SpecThreadId = kgs.Clock.ThrdCtrlHeader.Tid ;
        break ;
//=======================================================================
#endif
//=======================================================================
    default : Found = false ; break ;
    }
    RteSpecThreadsInfo->SpecThreadIdx++ ;
  }
  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoSpecThreadsFirst ( tsp92_RteSpecThreadsInfo *RteSpecThreadsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoSpecThreadsFirst"));

  SAPDB_MemFillNoCheck ( RteSpecThreadsInfo, 0, sizeof( tsp92_RteSpecThreadsInfo ) ) ;
  return ( vGetRteInfoSpecThreadsNext ( RteSpecThreadsInfo ) ) ;
}

// ================================ UKT ================================


#define RTE_DESC_UKT_EO922  "UKT"
#define RTE_DESC_DEVI_EO922 "I/O"

/*------------------------------*/

static void eo922BuildThreadName( char       *ThreadName,
                                  const char *ThreadDesc,
                                  tsp00_Int4  ThreadIndex,
                                  tsp00_Int4  MaxThreadNameLen )
{
  ROUTINE_DBG_MEO60 (_T("eo922BuildThreadName"));

  strcpy ( ThreadName, ThreadDesc ) ;

  sql47_ltoa( (long )ThreadIndex, 
              (char *)(&ThreadName[strlen(ThreadDesc)]), 
              MaxThreadNameLen - (tsp00_Int4)strlen(ThreadDesc) );
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoUKTNext ( tsp92_RteUKTInfo *RteUKTInfo  )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoUKTNext"));

  bool                Found = RteUKTInfo->ActUKT <= RteUKTInfo->LastUKT;

  if ( Found )
  {
    PUKT_CTRL_REC       pActUKTCtrl = ( PUKT_CTRL_REC)RteUKTInfo->ActUKT ;

    RteUKTInfo->ActUKT = (tsp00_Longint )( pActUKTCtrl + 1 ) ;
    RteUKTInfo->UKTTotalTasks = eo92CreateTaskcluster ( pActUKTCtrl,
                                                        RteUKTInfo->UKTTaskCluster, 
                                                        sizeof ( RteUKTInfo->UKTTaskCluster ) ) ;

    RteUKTInfo->UKTTaskCluster.c2p(RteUKTInfo->UKTTaskCluster) ;

    eo922BuildThreadName( RteUKTInfo->UKTName, RTE_DESC_UKT_EO922, 
                          UKT_INDEX(pActUKTCtrl), sizeof(RteUKTInfo->UKTName)) ;

    RteUKTInfo->UKTName.c2p(RteUKTInfo->UKTName) ;

    RteUKTInfo->UKTId = THREAD_TID(pActUKTCtrl);
    RteUKTInfo->UKTIdleCount	 = (tsp00_8ByteCounter) pActUKTCtrl->UKTRunningStat.TimeStat.ulCount;
    RteUKTInfo->UKTState.c2p(SAPDB_ToString(pActUKTCtrl->pTaskSchedulerObject->State ()));

//=======================================================================
#ifdef WIN32
//=======================================================================
    RteUKTInfo->UKTDispatcherCnt = (tsp00_8ByteCounter) (SAPDB_Int8)pActUKTCtrl->DispatcherCalls;

    TASK_CTRL ActTask;

    for ( RteUKTInfo->UKTCommandCnt = 0, 
          ActTask = kgs.pFirstTaskCtrl;
          ActTask <= kgs.pLastTaskCtrl; 
          ActTask++ )
    {
      if (( ActTask->pUKT == pActUKTCtrl ) && ( !ActTask->QueueStatRecResetPending ))
        RteUKTInfo->UKTCommandCnt += ActTask->QueStat.Resetable.ulCOMQueCount;
    }

    RteUKTInfo->UKTActiveTasks   = pActUKTCtrl->ulActiveTasks;
    RteUKTInfo->UKTRunQueueLen   = pActUKTCtrl->ulRUNQueueLen;

//=======================================================================
#else
//=======================================================================
    RteUKTInfo->UKTDispatcherCnt = pActUKTCtrl->disp_counter ;
    RteUKTInfo->UKTCommandCnt    = pActUKTCtrl->cmd_counter  ;
    RteUKTInfo->UKTActiveTasks   = pActUKTCtrl->activeTasks ;
    RteUKTInfo->UKTRunQueueLen   = pActUKTCtrl->RunQueueLen;
//=======================================================================
#endif
//=======================================================================
    RteUKTInfo->UKTTaskSwitchCnt = (tsp00_8ByteCounter)pActUKTCtrl->taskSwitches;
  }
  return ( Found ) ;
}


/*------------------------------*/
externC tsp00_Bool vGetRteInfoUKTFirst ( tsp92_RteUKTInfo *RteUKTInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoUKTFirst"));

  SAPDB_MemFillNoCheck ( RteUKTInfo, 0, sizeof( tsp92_RteUKTInfo ) ) ;
 
  RteUKTInfo->ActUKT   = ( tsp00_Longint) FIRST_UKT ;
  RteUKTInfo->LastUKT  = ( tsp00_Longint) LAST_UKT ;
  return ( vGetRteInfoUKTNext ( RteUKTInfo ) ) ;
}

// ================================ I/O Details ==============================

externC tsp00_Bool vGetRteInfoIODetailsNext ( tsp92_RteIODetailsInfo *RteIODetailsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoIODetailsNext"));

  bool             Found    = false;
  SAPDB_UInt8      ulReadCnt  = 0, ulWriteCnt   = 0 ;
  SAPDB_UInt8      ulReadPages= 0, ulWritePages = 0 ;

  if ( RteIODetailsInfo->ActUKT <= RteIODetailsInfo->LastUKT )
  {
     PUKT_CTRL_REC      pTmpUKTCtrl = ( PUKT_CTRL_REC) RteIODetailsInfo->ActUKT;

     for (  tsp00_Int4 ActDevno = RteIODetailsInfo->DevNo+1 ;
           ( tsp00_Longint) pTmpUKTCtrl <= RteIODetailsInfo->LastUKT && !Found ; ) 
     {
       for ( ; ActDevno < (tsp00_Int4) XPARAM(lMaxDevspaces) && !Found; ActDevno++ )
       {
         eo922GetRteOsSpecificUKTIO( pTmpUKTCtrl, ActDevno, &ulReadCnt , &ulReadPages, 
                                                            &ulWriteCnt, &ulWritePages) ;
         Found = ulReadCnt > 0 || ulWriteCnt > 0 ;
       }
       if ( !Found ) 
       {
         pTmpUKTCtrl++; 
         RteIODetailsInfo->ActUKT = ( tsp00_Longint) pTmpUKTCtrl ;
         ActDevno = 0 ;
       }
       else
       {
         RteIODetailsInfo->DevNo    = ActDevno - 1;
         eo922BuildThreadName( RteIODetailsInfo->ThreadName, RTE_DESC_UKT_EO922, 
                               UKT_INDEX(pTmpUKTCtrl), sizeof(RteIODetailsInfo->ThreadName)) ;
         RteIODetailsInfo->ThreadId =THREAD_TID(pTmpUKTCtrl) ;
         RteIODetailsInfo->QueLen   = RteIODetailsInfo->MaxQueLen = 0 ;
       }
     }
  }

  if ( !Found ) 
  {
    PDEVI_CTRL_REC     pDeviCtrl ;
    for ( pDeviCtrl =  ( PDEVI_CTRL_REC ) RteIODetailsInfo->ActDEV ;
          pDeviCtrl <= ( PDEVI_CTRL_REC ) RteIODetailsInfo->LastDEV && !Found; 
          pDeviCtrl++, RteIODetailsInfo->InternalDevno++ ) 
    {
      if ( (Found = DEVI_IN_USE(pDeviCtrl)) )
      {
        eo922BuildThreadName( RteIODetailsInfo->ThreadName, RTE_DESC_DEVI_EO922, 
                              RteIODetailsInfo->InternalDevno, sizeof(RteIODetailsInfo->ThreadName)) ;
        if ( pDeviCtrl->DevIOStat.ResetPending )
        {
            ulReadCnt  = 0;
            ulWriteCnt = 0;
        }
        else
        {
            ulReadCnt  = pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulCount;
            ulWriteCnt = pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulCount;
        }
        RteIODetailsInfo->ThreadId =THREAD_TID(pDeviCtrl) ;
        RteIODetailsInfo->QueLen     = (tsp00_Int4 ) pDeviCtrl->DevIOStat.ulIOQueLen ;
        RteIODetailsInfo->MaxQueLen  = (tsp00_Int4 ) pDeviCtrl->DevIOStat.ulMaxIOQueLen ;
        RteIODetailsInfo->DevNo  = RteIODetailsInfo->InternalDevno / DEVI_PER_VOLUME;
        RteIODetailsInfo->ActDEV = ( tsp00_Longint) (pDeviCtrl+1) ;
      }
    }
  }

  if ( Found ) 
  {
    RteIODetailsInfo->DevspaceName.c2p( DEVSP_NAME(RteIODetailsInfo->DevNo) ) ;
    RteIODetailsInfo->ReadCnt  = ( tsp00_8ByteCounter) ulReadCnt  ;
    RteIODetailsInfo->WriteCnt = ( tsp00_8ByteCounter) ulWriteCnt ;
    RteIODetailsInfo->ThreadName.c2p(RteIODetailsInfo->ThreadName) ;
  }

  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoIODetailsFirst ( tsp92_RteIODetailsInfo *RteIODetailsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoIODetailsFirst"));
  SAPDB_MemFillNoCheck ( RteIODetailsInfo, 0, sizeof( tsp92_RteIODetailsInfo ) ) ;

  RteIODetailsInfo->DevNo    = -1 ;
  RteIODetailsInfo->InternalDevno = 0;
  RteIODetailsInfo->ActUKT   = ( tsp00_Longint) FIRST_UKT ;
  RteIODetailsInfo->LastUKT  = ( tsp00_Longint) LAST_UKT ;
  RteIODetailsInfo->ActDEV   = ( tsp00_Longint) FIRST_DEV ;
  RteIODetailsInfo->LastDEV  = ( tsp00_Longint) LAST_DEV ;

  return ( vGetRteInfoIODetailsNext ( RteIODetailsInfo ) ) ;
}

// ================================ TASK INFO ================================

static void eo922FillTaskInfo( TASK_CTRL      pTaskCtrl,
                               tsp92_RteTaskInfo *RteTaskInfo)
{
  ROUTINE_DBG_MEO60 (_T("eo922FillTaskInfo"));

  bool          CancelFlagSet ;
  char         *WaitingForDevsp = pTaskCtrl->DevNameWaitingFor ;
  tsp00_Int     TaskState       = TASK_STATE(pTaskCtrl) ;
  int           StateReason     = TASK_STATE_REASON(pTaskCtrl) ;

//=======================================================================
#ifdef WIN32
//=======================================================================
  RteTaskInfo->UKTId         = pTaskCtrl->pUKT->ThrdCtrlHeader.Tid;
  RteTaskInfo->ApplPid       = pTaskCtrl->pUserCommCtrl ? pTaskCtrl->pUserCommCtrl->pidClientPID : undef_ceo00 ;
  RteTaskInfo->TimeOut       = pTaskCtrl->pTaskObject->ClientConnections().GetSessionTimeOut();
  RteTaskInfo->HoldingRegion = pTaskCtrl->Sid > 0 ? pTaskCtrl->Sid: undef_ceo00 ;
  CancelFlagSet              = pTaskCtrl->KernelCtrlRecord.to_cancel != (char )0;
//=======================================================================
#else
//=======================================================================
  char PeerPidBuffer[10];
  RteTaskInfo->ApplPid = atoi ( sql45_peer_pid(pTaskCtrl, PeerPidBuffer) );

  RteTaskInfo->TimeOut       = pTaskCtrl->pTaskObject->ClientConnections().GetSessionTimeOut();
  RteTaskInfo->UKTId         = pTaskCtrl->ukt->tid;
  RteTaskInfo->HoldingRegion = pTaskCtrl->sid > 0 ? (tsp00_Int4 )pTaskCtrl->sid: undef_ceo00 ;
  CancelFlagSet              = (bool )pTaskCtrl->rte_comm.to_cancel ;
//=======================================================================
#endif
//=======================================================================

  if ( TASK_STATE(pTaskCtrl) != TSK_VRECEIVE_EO00 || RteTaskInfo->TimeOut == undef_ceo00 )
    RteTaskInfo->TimeOut = undef_ceo00 ;
  else
  { 
#if defined(WIN32)
    SAPDB_UInt4 ActTime = (SAPDB_UInt4)kgs.ulCurrTime;
#else
    SAPDB_UInt4 ActTime = (SAPDB_UInt4)KGS->current_time;
#endif
    if ( ((SAPDB_UInt4)RteTaskInfo->TimeOut) < ActTime ) 
      RteTaskInfo->TimeOut = 0 ;
    else 
      RteTaskInfo->TimeOut = ((SAPDB_UInt4)RteTaskInfo->TimeOut) - ActTime ;
  }

  RteTaskInfo->WaitingForRoot= pTaskCtrl->lWaitForRoot ;
  RteTaskInfo->WaitingForTask= pTaskCtrl->lWaitForTask ;
  { tsp00_PageNo pageNo = pTaskCtrl->lockedPageNo; /* PTS 1115172 */
    RteTaskInfo->LockedPageNo = ( pageNo > 0 ? pageNo : undef_ceo00 );
  }
  RteTaskInfo->TaskIndex = TASK_INDEX(pTaskCtrl);
  RteTaskInfo->TaskName.c2p (eo92GetTaskTypeName ( TASK_TYPE(pTaskCtrl) ) );
  RteTaskInfo->TaskState.c2p(eo92GetTaskStateName( TaskState, StateReason ) );

  /* PTS 1105996 */
  if ( WaitingForDevsp )
     RteTaskInfo->ExtendedTaskState.c2p(WaitingForDevsp);
  else
  if ( TaskState == TSK_VSUSPEND_EO00 && StateReason > 0 && StateReason < MX_SUSPEND_REASONS_EO00 )
    RteTaskInfo->ExtendedTaskState.c2p(SuspendReasonText[StateReason].szLongReason) ;
  else
  if ( (TaskState == TSK_VBEGEXCL_EO00 || TaskState == TSK_VENDEXCL_EO00) &&
       (RteTaskInfo->HoldingRegion > 0 && RteTaskInfo->HoldingRegion < (tsp00_Longint )XPARAM(ulNoOfRegions) ))
    RteTaskInfo->ExtendedTaskState.c2p(SEMA_NAME(RteTaskInfo->HoldingRegion-1));
  else
  if ( TaskState == TSK_YIELDING_ON_SPINLOCK_EO00 )
    RteTaskInfo->ExtendedTaskState.c2p(&pTaskCtrl->NameOfSpinlockYieldingFor[0]);
  else
    RteTaskInfo->ExtendedTaskState.c2p("");

  RteTaskInfo->CancelFlag.c2p( CancelFlagSet ? "!" : "") ;
  RteTaskInfo->OMSCallActive.c2p( pTaskCtrl->TaskOMSStat.OMSCallActive ? "&" : "") ; /* CR 1106969 */
  RteTaskInfo->SpecialFlag.c2p("") ; 
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoTaskNext ( tsp92_RteTaskInfo *RteTaskInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoTaskNext"));

  bool                Found    = RteTaskInfo->ActTask <= RteTaskInfo->LastTask ;

  if ( Found )
  {
    TASK_CTRL  pTaskCtrl = (TASK_CTRL ) RteTaskInfo->ActTask;
    for ( Found = false; !Found && pTaskCtrl <= (TASK_CTRL ) RteTaskInfo->LastTask; pTaskCtrl++ ) 
    {
      Found = IS_TASK_TO_SHOW_EO92(TASK_TYPE(pTaskCtrl),
                                   TASK_STATE(pTaskCtrl),
                                   TASK_STATE_REASON(pTaskCtrl),
                                   pTaskCtrl->TaskOMSStat.OMSCallActive,    /* PTS 1116933 */
                                   TASK_QUQUE_LOCATION(pTaskCtrl),
                                   RteTaskInfo->TaskTypeToShow,
                                   RteTaskInfo->TaskStateToShow ) ;
      if ( Found )
        eo922FillTaskInfo( pTaskCtrl, RteTaskInfo ) ;
    }

    RteTaskInfo->ActTask = ( tsp00_Longint) pTaskCtrl ;
  }

  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoTaskFirst ( tsp92_RteTaskInfo                 *RteTaskInfo,
                                          tsp92_RteTaskInfoStateToShow_Enum  StateToShow,
                                          tsp92_RteTaskInfoTypeToShow_Enum   TypeToShow )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoTaskFirst"));

  SAPDB_MemFillNoCheck ( RteTaskInfo, 0, sizeof( tsp92_RteTaskInfo ) ) ;
  RteTaskInfo->ActTask  = ( tsp00_Longint) KGS->pFirstTaskCtrl ;
  RteTaskInfo->LastTask = ( tsp00_Longint) KGS->pLastTaskCtrl ;

  switch ( StateToShow ) 
  {
    case sp92State_Active   : RteTaskInfo->TaskStateToShow = SHOW_ACTIVE_EO92   ; break ;
    case sp92State_Runnable : RteTaskInfo->TaskStateToShow = SHOW_RUNNABLE_EO92 ; break ;
    default                 : RteTaskInfo->TaskStateToShow = SHOW_ALL_EO92      ; break ;
  }

  switch ( TypeToShow ) 
  {
    case sp92Type_User       : RteTaskInfo->TaskTypeToShow = TT_US_EO00         ; break ;
    case sp92Type_Server     : RteTaskInfo->TaskTypeToShow = TT_SV_EO00         ; break ;
    case sp92Type_DataWriter : RteTaskInfo->TaskTypeToShow = TT_DW_EO00         ; break ;
    default                  : RteTaskInfo->TaskTypeToShow = SHOW_TT_ALL_EO92   ; break ;
  }

  return ( vGetRteInfoTaskNext ( RteTaskInfo ) ) ;
}

// ============================= TASK DETAIL INFO ============================
// PTS 1111217 : every call to eo922SetTime has been changed to implement the
//               parameter "TotTimeToSetInMicroSec"

inline void eo922SetTime( tsp00_Longreal        *AvgTimeToSetInSec,
                          tsp00_Longreal        *TotTimeToSetInMicroSec,  
                          SAPDB_UInt8            Cnt,  
                          tsp00_Uint8            MicroSeconds ) 
{
  if (XPARAM(fTimeMeasure) && Cnt > 0 )
  {
    *AvgTimeToSetInSec = ( (tsp00_Longreal)(tsp00_Int8)MicroSeconds / 1000000.0 / (tsp00_Longreal)(tsp00_Int8)Cnt ) ;
	*TotTimeToSetInMicroSec = (tsp00_Longreal)(tsp00_Int8)MicroSeconds;
  }
  else
    *AvgTimeToSetInSec = *TotTimeToSetInMicroSec = (tsp00_Longreal ) undef_ceo00 ; 
}

/*------------------------------*/

inline void eo922SetStateStatistic( tsp92_RteTaskStateInfo                     * StateStatistic, 
                                    teo52_TaskRelativeAndAbsoluteTimeStatistic * StateStat )
{
  StateStatistic->Cnt   =  (tsp00_8ByteCounter )  StateStat->TimeStat.ulCount  ;

  eo922SetTime( &StateStatistic->AvgAbsTime, &StateStatistic->TotAbsTime, StateStat->TimeStat.ulMeasurements,
                 StateStat->TimeStat.ulAbsMicroSeconds );

  eo922SetTime( &StateStatistic->AvgRelTime, &StateStatistic->TotRelTime, StateStat->TimeStat.ulMeasurements,
                 StateStat->TimeStat.ulRelMicroSeconds );
}

/*------------------------------*/

inline void eo922SetSelfIOStatistic( tsp92_RteTaskSelfIOInfo *SelfIOStatistic, 
                                     teo52_IOStatisticRec    *StateStat ) 
{
  SelfIOStatistic->Cnt   = (tsp00_8ByteCounter ) StateStat->IOStat.TimeStat.ulCount ;
  SelfIOStatistic->Pages = (tsp00_8ByteCounter ) StateStat->ulPageCount  ;

  eo922SetTime( &SelfIOStatistic->AvgAbsTime, &SelfIOStatistic->TotAbsTime, StateStat->IOStat.TimeStat.ulCount,
                 StateStat->IOStat.TimeStat.ulAbsMicroSeconds );

}

/*------------------------------*/

inline void eo922SetDevIOStatistic( tsp92_RteTaskDEVIOInfo   *DevIOStatistic, 
                                    teo52_TaskDevIOStatistic *StateStat ) 
{
  DevIOStatistic->Cnt   = (tsp00_8ByteCounter ) StateStat->IOStat.TimeStat.ulCount ;
  DevIOStatistic->Pages = (tsp00_8ByteCounter ) StateStat->ulPageCount ;

  eo922SetTime( &DevIOStatistic->AvgAbsTime,
                &DevIOStatistic->TotAbsTime,
                StateStat->IOStat.TimeStat.ulCount,
                StateStat->IOStat.TimeStat.ulAbsMicroSeconds );

  eo922SetTime( &DevIOStatistic->AvgRelTime,
                &DevIOStatistic->TotRelTime,
                StateStat->IOStat.TimeStat.ulCount,
                StateStat->IOStat.TimeStat.ulRelMicroSeconds );
}

/*------------------------------*/

static void eo922GetTaskDetails(TASK_CTRL ActTask, tsp92_RteTaskDetailsInfo  *RteTaskDetailsInfo)
{
	teo52_TaskStateStatisticRec *TaskStateStat = eo922_GetTaskStateStatistic(ActTask);
	teo52_TaskQueueStatistic    *QueStat       = eo922_GetTaskQueueStatistic(ActTask);
	int StateReason                            = TASK_STATE_REASON(ActTask) ;
	
	RteTaskDetailsInfo->TaskName.c2p (eo92GetTaskTypeName ( TASK_TYPE(ActTask) ) );
	RteTaskDetailsInfo->TaskState.c2p(eo92GetTaskStateName( TASK_STATE(ActTask), StateReason ) );
	
	RteTaskDetailsInfo->TaskIndex       = TASK_INDEX(ActTask);
	RteTaskDetailsInfo->DispatcherCnt   = (tsp00_8ByteCounter) TASK_DISP_CNT(ActTask) ;
	RteTaskDetailsInfo->CommandCnt      = QueStat->Resetable.ulCOMQueCount ;
	RteTaskDetailsInfo->TaskSelfSuspCnt = (tsp00_8ByteCounter) TASK_SELF_SUSP_CNT(ActTask);      
	RteTaskDetailsInfo->RegionAccessCnt = (tsp00_8ByteCounter) TASK_REGION_ACCESS(ActTask);
	RteTaskDetailsInfo->PrioCnt         = ( 
            EO52_RESET_TASK_FULL_PENDING == ActTask->TaskStatRecResetPending
            ? 0 : ActTask->PrioStat.Resetable.ulTotalCount );

	eo922SetDevIOStatistic ( &RteTaskDetailsInfo->BackupRead  , &TaskStateStat->AsynIOReadStat);
	eo922SetDevIOStatistic ( &RteTaskDetailsInfo->BackupWrite , &TaskStateStat->AsynIOWriteStat);
	eo922SetDevIOStatistic ( &RteTaskDetailsInfo->DEVIORead   , &TaskStateStat->DevIOReadStat);
	eo922SetDevIOStatistic ( &RteTaskDetailsInfo->DEVIOWrite  , &TaskStateStat->DevIOWriteStat);
	eo922SetSelfIOStatistic( &RteTaskDetailsInfo->SelfIORead  , &TaskStateStat->SelfIOReadStat);
	eo922SetSelfIOStatistic( &RteTaskDetailsInfo->SelfIOWrite , &TaskStateStat->SelfIOWriteStat);
	eo922SetStateStatistic ( &RteTaskDetailsInfo->Vsuspend    , &TaskStateStat->VsuspStat);
	eo922SetStateStatistic ( &RteTaskDetailsInfo->Vwait       , &TaskStateStat->VwaitStat);
	eo922SetStateStatistic ( &RteTaskDetailsInfo->Vsleep      , &TaskStateStat->VsleepStat);
	eo922SetStateStatistic ( &RteTaskDetailsInfo->ReplyReceive, &TaskStateStat->RplRcvStat);

    RteTaskDetailsInfo->ReceiveReply.Cnt   =  (tsp00_8ByteCounter )  TaskStateStat->RcvRplStat.TimeStat.ulCount;
    eo922SetTime( &RteTaskDetailsInfo->ReceiveReply.AvgAbsTime, 
                  &RteTaskDetailsInfo->ReceiveReply.TotAbsTime, 
                  TaskStateStat->RcvRplStat.TimeStat.ulMeasurements,
                  TaskStateStat->RcvRplStat.TimeStat.ulAbsMicroSeconds );
	// RelRcvRplTime is always equal to AbsRcvRplTime
	RteTaskDetailsInfo->ReceiveReply.AvgRelTime = RteTaskDetailsInfo->ReceiveReply.AvgAbsTime ;
	RteTaskDetailsInfo->ReceiveReply.TotRelTime = RteTaskDetailsInfo->ReceiveReply.TotAbsTime ;
	
	/* PTS 1106969 */
	teo52_TaskOMSStatisticRec *pTaskOMSStat = &ActTask->TaskOMSStat ;
	RteTaskDetailsInfo->COMProcedureName.c2p( pTaskOMSStat->ProcedurName.asCharp() ) ;
	RteTaskDetailsInfo->OMSCallbackCount    = pTaskOMSStat->CallbackCount ;
    {
		eo92UnknownMethodName spaceForUnknownMethodName;
        SAPDB_Char const * methodName = "";

        if ( pTaskOMSStat->OMSCallActive )
        {
            SAPDB_Int4 methodIdx = pTaskOMSStat->MethodIdx;
            if ( methodIdx > 0 )
            {
                methodName = eo92GetOMSCallbackMethodName(methodIdx, spaceForUnknownMethodName );
            }
        }
		RteTaskDetailsInfo->OMSMCallbackMethodName.c2p(methodName);
    }

	//=======================================================================
#ifdef WIN32
	//=======================================================================
	RteTaskDetailsInfo->ApplPid  = ActTask->pUserCommCtrl ? ActTask->pUserCommCtrl->pidClientPID : undef_ceo00 ;
	RteTaskDetailsInfo->UKTId	 = ActTask->pUKT->ThrdCtrlHeader.Tid;
	//=======================================================================
#else
	//=======================================================================
	char PeerPidBuffer[10];
	RteTaskDetailsInfo->ApplPid  = atoi ( sql45_peer_pid(ActTask, PeerPidBuffer) );
	RteTaskDetailsInfo->UKTId	 = ActTask->ukt->tid;
	//=======================================================================
#endif
	//=======================================================================
	if ( RteTaskDetailsInfo->ApplPid != undef_ceo00 )
	{
        if ( *PTR_TO_PEER_NODE(ActTask) != '\0' ) 
            RteTaskDetailsInfo->RemoteApplNode.c2p( PTR_TO_PEER_NODE(ActTask) ) ;
		else
            RteTaskDetailsInfo->RemoteApplNode.c2p( PTR_TO_DB_NODE ) ;
	}
	else
		RteTaskDetailsInfo->RemoteApplNode.c2p("");
}

/*------------------------------*/

externC void vGetRteInfoTaskDetails ( tsp00_TaskId taskid, tsp92_RteTaskDetailsInfo *RteTaskDetailsInfo )
{
	TASK_CTRL pTaskCtrl = TASK_AT_INDEX(taskid);
    eo922GetTaskDetails (pTaskCtrl, RteTaskDetailsInfo);    
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoTaskDetailsNext ( tsp92_RteTaskDetailsInfo  *RteTaskDetailsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoTaskDetailsNext"));

  bool                Found    = RteTaskDetailsInfo->ActTask <= RteTaskDetailsInfo->LastTask ;

  if ( Found )
  {
    TASK_CTRL  ActTask = (TASK_CTRL ) RteTaskDetailsInfo->ActTask;
    for ( Found = false; !Found && ActTask <= (TASK_CTRL ) RteTaskDetailsInfo->LastTask; ActTask++ ) 
    {
      if ( Found = (TASK_DISP_CNT(ActTask) > 0) )
      {
        eo922GetTaskDetails (ActTask, RteTaskDetailsInfo);    
      }
    }
    RteTaskDetailsInfo->ActTask = ( tsp00_Longint) ActTask ;
  }
  return ( Found ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoTaskDetailsFirst ( tsp92_RteTaskDetailsInfo  *RteTaskDetailsInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoTaskDetailsFirst"));

  SAPDB_MemFillNoCheck ( RteTaskDetailsInfo, 0, sizeof( tsp92_RteTaskDetailsInfo ) ) ;
  RteTaskDetailsInfo->ActTask  = ( tsp00_Longint) KGS->pFirstTaskCtrl ;
  RteTaskDetailsInfo->LastTask = ( tsp00_Longint) KGS->pLastTaskCtrl ;

  return ( vGetRteInfoTaskDetailsNext ( RteTaskDetailsInfo ) ) ;
}

// ================================ TOTAL INFO ===============================

externC void vGetRteInfoTotalCnt ( tsp92_RteTotalCntInfo *RteTotalCntInfo ) 
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoTotalCnt"));

  SAPDB_MemFillNoCheck ( RteTotalCntInfo, 0, sizeof( tsp92_RteTotalCntInfo ) ) ;

  TASK_CTRL ActTask ;
     
  for ( ActTask = KGS->pFirstTaskCtrl; ActTask <= KGS->pLastTaskCtrl ; ActTask++ )
  {
      if ( TASK_DISP_CNT(ActTask) > 0 )
      {
	    teo52_TaskStateStatisticRec *TaskStateStat = eo922_GetTaskStateStatistic(ActTask);
	    teo52_TaskQueueStatistic    *QueStat       = eo922_GetTaskQueueStatistic(ActTask);

        RteTotalCntInfo->CommandCnt += QueStat->Resetable.ulCOMQueCount;
        RteTotalCntInfo->VwaitCnt   += TaskStateStat->VwaitStat.TimeStat.ulCount;
        RteTotalCntInfo->VsuspendCnt+= TaskStateStat->VsuspStat.TimeStat.ulCount;
        RteTotalCntInfo->VsleepCnt  += TaskStateStat->VsleepStat.TimeStat.ulCount;
        RteTotalCntInfo->DevIOCnt   += TaskStateStat->DevIOReadStat.IOStat.TimeStat.ulCount +
                                       TaskStateStat->DevIOWriteStat.IOStat.TimeStat.ulCount;
        RteTotalCntInfo->TaskIOCnt  += TaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulCount +
                                       TaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulCount;
	    RteTotalCntInfo->PrioCnt         = ( 
            EO52_RESET_TASK_FULL_PENDING == ActTask->TaskStatRecResetPending
            ? 0 : ActTask->PrioStat.Resetable.ulTotalCount );
        RteTotalCntInfo->TaskSelfSuspCnt += TASK_SELF_SUSP_CNT(ActTask);      
        RteTotalCntInfo->DispatcherCnt   += TASK_DISP_CNT(ActTask);

        if ( TASK_TYPE(ActTask) == TT_US_EO00 )
        {
          RteTotalCntInfo->UserTaskReadCnt  += TaskStateStat->SelfIOReadStat.IOStat.TimeStat.ulCount + 
                                               TaskStateStat->DevIOReadStat.IOStat.TimeStat.ulCount ;
          RteTotalCntInfo->UserTaskWriteCnt += TaskStateStat->SelfIOWriteStat.IOStat.TimeStat.ulCount + 
                                               TaskStateStat->DevIOWriteStat.IOStat.TimeStat.ulCount ;
        }
      }
  }

  tsp00_Int4 BegSearchRegion = 0, ActRegion ;

  while ( eo922RteInfoRegionSearchNextSem (BegSearchRegion, &ActRegion ) )
  {
    PSEM_CTRL_REC  pSemCtrl = FIRST_REGION + ActRegion ;
    RteTotalCntInfo->RegionAccessCnt    += REGION_ACCESS(pSemCtrl);
    RteTotalCntInfo->RegionCollCnt      += REGION_COLLISION(pSemCtrl);
    RteTotalCntInfo->RegionWaitCnt      += REGION_WAITS(pSemCtrl);
    RteTotalCntInfo->VbegexclTasCollCnt += REGION_BEGEXCL_TAS_COLL(pSemCtrl);
    RteTotalCntInfo->VendexclTasCollCnt += REGION_ENDEXCL_TAS_COLL(pSemCtrl);
    BegSearchRegion = ActRegion+1 ;
  }

  RteTotalCntInfo->SymbolResolutionCnt = (tsp00_8ByteCounter)RTEDiag_SymbolResolutionLockCalls();

  return ;
}

// ================================ ALLOCATOR INFO ===============================

#define RTE_REGISTER_FIRST_INDEX (-1)

static void eo922_FillSystemInfo( tsp92_RteAllocatorInfo & RteAllocatorInfo )
{
    SAPDB_ULong MemoryLimitInBytes;
    SAPDB_ULong MemoryCommitedInBytes;
    SAPDB_ULong MemoryUsedInBytes;
    SAPDB_ULong MemoryMaxUsedInBytes;   // PTS 1120445 D.T.
    SAPDB_UInt8 MemorySuccessfullAllocs;
    SAPDB_UInt8 MemoryFailedAllocs;
    SAPDB_UInt8 MemoryCommits;
    SAPDB_UInt8 MemoryFrees;

    RTE_ISystem::Instance().GetMemoryStatistics( MemoryLimitInBytes,
                                                 MemoryCommitedInBytes,
                                                 MemoryUsedInBytes,
                                                 MemoryMaxUsedInBytes,    // PTS 1120445 D.T.
                                                 MemorySuccessfullAllocs,
                                                 MemoryFailedAllocs,
                                                 MemoryCommits,
                                                 MemoryFrees );

    SAPDB_MemFillNoCheck(RteAllocatorInfo.AllocatorName.asCharp(), ' ', sizeof(tsp00_C40));
    SAPDB_MemCopyNoCheck(RteAllocatorInfo.AllocatorName.asCharp(), "SystemHeap", strlen("SystemHeap") );

    RteAllocatorInfo.UsedBytes = MemoryUsedInBytes;

    RteAllocatorInfo.MaxUsedBytes = MemoryMaxUsedInBytes;

    RteAllocatorInfo.AllocatedBytes = MemoryLimitInBytes;

	RteAllocatorInfo.CountAlloc = (tsp00_8ByteCounter) 
		((SAPDB_Int8) (MemorySuccessfullAllocs + MemoryFailedAllocs));
	
    RteAllocatorInfo.CountDealloc = (tsp00_8ByteCounter) ((SAPDB_Int8) MemoryFrees);

	RteAllocatorInfo.BaseCountAlloc   = (tsp00_8ByteCounter) 0;
	RteAllocatorInfo.BaseCountDealloc = (tsp00_8ByteCounter) 0;

    RteAllocatorInfo.ErrorCount = 0;

    SAPDB_MemFillNoCheck(RteAllocatorInfo.AllocatorBaseName.asCharp(), ' ', sizeof(tsp00_C40));

    RteAllocatorInfo.InfoArray = 0;
    RteAllocatorInfo.LastIndex = 0;
    RteAllocatorInfo.InfoIndex = RTE_REGISTER_FIRST_INDEX;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoAllocatorNext ( tsp92_RteAllocatorInfo * RteAllocatorInfo )
{
    ROUTINE_DBG_MEO60 (_T("vGetRteInfoAllocatorNext"));

    RTEMem_AllocatorRegister::StatisticInfo *pInfoArray = 0;

    if ( RteAllocatorInfo->InfoIndex == RTE_REGISTER_FIRST_INDEX )
    {
        if ( RTEMem_AllocatorRegister::Instance().GetStatisticInfo(pInfoArray, RteAllocatorInfo->LastIndex) )
        {
            RteAllocatorInfo->InfoArray = reinterpret_cast<tsp00_Addr>(pInfoArray);
            /* LastIndex filled already since call succeeded */
            RteAllocatorInfo->InfoIndex = 0;
        }
        else
        {
            RteAllocatorInfo->InfoArray = 0;
            RteAllocatorInfo->LastIndex = 0; /* this trigger return false... */
            RteAllocatorInfo->InfoIndex = 0;
        }
    }

    if ( RteAllocatorInfo->InfoIndex >= RteAllocatorInfo->LastIndex )
    {
        return false; // at end of list
    }

    pInfoArray = reinterpret_cast<RTEMem_AllocatorRegister::StatisticInfo *>(RteAllocatorInfo->InfoArray);

    RTEMem_AllocatorRegister::StatisticInfo &currentInfo = pInfoArray[RteAllocatorInfo->InfoIndex];

    RteAllocatorInfo->UsedBytes = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.UsedBytes;
    RteAllocatorInfo->AllocatedBytes = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.AllocatedBytes;

    RteAllocatorInfo->CountAlloc = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.CountAlloc;
    RteAllocatorInfo->CountDealloc = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.CountDealloc;

    RteAllocatorInfo->BaseCountAlloc = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.BaseCountAlloc;
    RteAllocatorInfo->BaseCountDealloc = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.BaseCountDealloc;
    RteAllocatorInfo->ErrorCount = (tsp00_Int4)currentInfo.CountFoundError;

    RteAllocatorInfo->AllocatorName.c2p((const char *)currentInfo.AllocatorName);
    RteAllocatorInfo->AllocatorBaseName.c2p((const char *)currentInfo.AllocatorBaseName);

    ++(RteAllocatorInfo->InfoIndex);

    if ( RteAllocatorInfo->InfoIndex >= RteAllocatorInfo->LastIndex )
    {
        RTEMem_AllocatorRegister::Instance().FreeStatisticInfo(pInfoArray);
        RteAllocatorInfo->InfoArray = 0;
    }

    // true since entry was filled
    return true;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoAllocatorFirst ( tsp92_RteAllocatorInfo * RteAllocatorInfo )
{
    ROUTINE_DBG_MEO60 (_T("vGetRteInfoAllocatorFirst"));

    // First entry in list is always System heap memory usage...
    eo922_FillSystemInfo( *RteAllocatorInfo );

    return true;
}

// ================================ SPINLOCK INFO ===============================

externC tsp00_Bool vGetRteInfoSpinlockNext ( tsp92_RteSpinlockInfo *RteSpinlockInfo )
{
    ROUTINE_DBG_MEO60 (_T("vGetRteInfoSpinlockNext"));

    RTESync_SpinlockRegister::StatisticInfo *pInfoArray = 0;

    if ( RteSpinlockInfo->InfoIndex == RTE_REGISTER_FIRST_INDEX )
    {
        if ( RTESync_SpinlockRegister::Instance().GetStatisticInfo(pInfoArray, RteSpinlockInfo->LastIndex) )
        {
            RteSpinlockInfo->InfoArray = reinterpret_cast<tsp00_Addr>(pInfoArray);
            /* LastIndex filled already since call succeeded */
            RteSpinlockInfo->InfoIndex = 0;
        }
        else
        {
            RteSpinlockInfo->InfoArray = 0;
            RteSpinlockInfo->LastIndex = 0; /* this trigger return false... */
            RteSpinlockInfo->InfoIndex = 0;
        }
    }

    if ( RteSpinlockInfo->InfoIndex >= RteSpinlockInfo->LastIndex )
    {
        return false; // at end of list
    }

    pInfoArray = reinterpret_cast<RTESync_SpinlockRegister::StatisticInfo *>(RteSpinlockInfo->InfoArray);

    RTESync_SpinlockRegister::StatisticInfo &currentInfo = pInfoArray[RteSpinlockInfo->InfoIndex];

    RteSpinlockInfo->SpinlockName.c2p((const char *)currentInfo.SpinlockName);

    RteSpinlockInfo->Locks           = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.StatisticRecord.locks;           /* total number of locks */
    RteSpinlockInfo->Collisions      = (tsp00_8ByteCounter)(SAPDB_Int8)currentInfo.StatisticRecord.collisions;      /* total number of collisions */
    RteSpinlockInfo->TotalSpinLoops  = currentInfo.StatisticRecord.totalSpinLoops;  /* total number of spin loops needed */
    RteSpinlockInfo->TotalYieldLoops = currentInfo.StatisticRecord.totalYieldLoops; /* total number of yield loops needed */
    RteSpinlockInfo->MaxSpinLoops    = currentInfo.StatisticRecord.maxSpinLoops;    /* maximum number of spin loops needed */
    RteSpinlockInfo->MaxYieldLoops   = currentInfo.StatisticRecord.maxYieldLoops;   /* maximum number of yield loops needed */
    RteSpinlockInfo->CurrentLoops    = currentInfo.StatisticRecord.currentLoops;    /* current number of loops needed (spin loops or yield loops) */
    ++(RteSpinlockInfo->InfoIndex);

    if ( RteSpinlockInfo->InfoIndex >= RteSpinlockInfo->LastIndex )
    {
        RTESync_SpinlockRegister::Instance().FreeStatisticInfo(pInfoArray);
        RteSpinlockInfo->InfoArray = 0;
    }

    // true since entry was filled
    return true;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoSpinlockFirst ( tsp92_RteSpinlockInfo * RteSpinlockInfo )
{
    ROUTINE_DBG_MEO60 (_T("vGetRteInfoSpinlockFirst"));

    RteSpinlockInfo->InfoArray = 0;
    RteSpinlockInfo->LastIndex = 0;
    RteSpinlockInfo->InfoIndex = RTE_REGISTER_FIRST_INDEX;

    return vGetRteInfoSpinlockNext ( RteSpinlockInfo );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#ifdef FHTEST

externC void vTestGetRteInfo ()
{
  tsp00_Bool Found ;
  int Len ;
  char Line [ 500 ] ;
  void sql60_write_to_util_diag(char *, int);

#ifdef TEST_REGION

  tsp92_RteRegionInfo RteRegionInfo ;
  Found = vGetRteInfoRegionFirst ( &RteRegionInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%3d %s %5d %5d %5d %5d %5d %5d %7.2f %d",
       RteRegionInfo.RegionNo ,
       RteRegionInfo.RegionName.asCharp(),
       RteRegionInfo.OwnerTaskIndex ,
       RteRegionInfo.WaitTaskIndex ,
       RteRegionInfo.GetCount,
       RteRegionInfo.TasCount,
       RteRegionInfo.CollisionCount,
       RteRegionInfo.WaitCount,
       RteRegionInfo.CollisionRate,
       RteRegionInfo.Exclusive );
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoRegionNext (&RteRegionInfo) ;
  }
#endif

#ifdef TEST_IO

  tsp92_RteIOInfo RteIOInfo ;
  Found = vGetRteInfoIOFirst ( &RteIOInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%3d %64.64s %5.0f %5.0f\n",
       RteIOInfo.DevNo ,
       (char *)RteIOInfo.DevspaceName,
       RteIOInfo.ReadCnt,
       RteIOInfo.WriteCnt );
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoIONext (&RteIOInfo) ;
  }
#endif

#ifdef TEST_BACKUP

  tsp92_RteBackupInfo RteBackupInfo ;
  Found = vGetRteInfoBackupFirst ( &RteBackupInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%3d %40.40s %5.0f %5.0f %5.0f %2d %2d\n",
       RteBackupInfo.DevNo ,
       RteBackupInfo.DevspaceName.asCharp(),
       RteBackupInfo.ReadCnt,
       RteBackupInfo.WriteCnt,
       RteBackupInfo.PageCnt,
       RteBackupInfo.QueLen,
       RteBackupInfo.MaxQueLen);
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoBackupNext (&RteBackupInfo) ;
  }
#endif

#ifdef TEST_STORAGE

  tsp92_RteStorageInfo RteStorageInfo ;
  Found = vGetRteInfoStorageFirst ( &RteStorageInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%40.40s %5d %10.10s\n",
       RteStorageInfo.StorageName.asCharp(),
       RteStorageInfo.StorageValue,
       RteStorageInfo.StorageUnit.asCharp() );
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoStorageNext (&RteStorageInfo) ;
  }
#endif

#ifdef TEST_SPEC_THREADS
  tsp92_RteSpecThreadsInfo RteSpecThreadsInfo ;
  Found = vGetRteInfoSpecThreadsFirst ( &RteSpecThreadsInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%40.40s %5d\n",
       RteSpecThreadsInfo.SpecThreadName.asCharp(),
       RteSpecThreadsInfo.SpecThreadId );
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoSpecThreadsNext (&RteSpecThreadsInfo) ;
  }
#endif

#ifdef TEST_UKT
  tsp92_RteUKTInfo RteUKTInfo ;
  Found = vGetRteInfoUKTFirst ( &RteUKTInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%5d %16.16s %5.0f %5d %5d %40.40s\n",
       RteUKTInfo.UKTId,
       RteUKTInfo.UKTName.asCharp(),
       RteUKTInfo.UKTDispatcherCnt,
       RteUKTInfo.UKTActiveTasks,
       RteUKTInfo.UKTTotalTasks,
       RteUKTInfo.UKTTaskCluster.asCharp());

    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoUKTNext (&RteUKTInfo) ;
  }
#endif


#ifdef TEST_IO_DETAILS
  tsp92_RteIODetailsInfo RteIODetailsInfo ;
  Found = vGetRteInfoIODetailsFirst ( &RteIODetailsInfo ) ;

  while  ( Found )
  {
    Len = sprintf(Line,"%5d %16.16s %3d %40.40s %5.0f %5.0f %2d %2d\n",
       RteIODetailsInfo.ThreadId,
       RteIODetailsInfo.ThreadName.asCharp(),
       RteIODetailsInfo.DevNo ,
       RteIODetailsInfo.DevspaceName.asCharp(),
       RteIODetailsInfo.ReadCnt,
       RteIODetailsInfo.WriteCnt,
       RteIODetailsInfo.QueLen,
       RteIODetailsInfo.MaxQueLen);

    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoIODetailsNext (&RteIODetailsInfo) ;
  }
#endif

#ifdef TEST_TASK
  tsp92_RteTaskInfo      RteTaskInfo ;
  Found = vGetRteInfoTaskFirst ( &RteTaskInfo, sp92State_All, sp92Type_All ) ;
  while  ( Found )
  {
    Len = sprintf(Line,"%2d %5d %12.12s %5d %12.12s %c %3d %3d %3d %3d %16.16s\n",
          RteTaskInfo.TaskIndex, RteTaskInfo.UKTId,
          RteTaskInfo.TaskName.asCharp(), RteTaskInfo.ApplPid, 
          RteTaskInfo.TaskState.asCharp(), RteTaskInfo.SpecialFlag[0],
          RteTaskInfo.TimeOut, RteTaskInfo.HoldingRegion,
          RteTaskInfo.WaitingForTask,RteTaskInfo.WaitingForRoot,
          RteTaskInfo.WaitingForDevname.asCharp() ) ;

    sql60_write_to_util_diag(Line, Len);
    Found = vGetRteInfoTaskNext (&RteTaskInfo) ;
  }
#endif

#ifdef TEST_TOTAL
  tsp92_RteTotalCntInfo RteTotalCntInfo ;
  vGetRteInfoTotalCnt ( &RteTotalCntInfo ) ;

    Len = sprintf(Line,"%5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f %5.0f 5d %5.0f %5.0f %5.0f %5.0f %5.0f\n",
      RteTotalCntInfo.CommandCnt,
      RteTotalCntInfo.VwaitCnt,
      RteTotalCntInfo.VsuspendCnt,
      RteTotalCntInfo.VsleepCnt,
      RteTotalCntInfo.DevIOCnt,
      RteTotalCntInfo.UserTaskReadCnt,
      RteTotalCntInfo.UserTaskWriteCnt,
      RteTotalCntInfo.TaskIOCnt,
      RteTotalCntInfo.PrioCnt,
      RteTotalCntInfo.TaskSelfSuspCnt,   
      RteTotalCntInfo.DispatcherCnt, 
      RteTotalCntInfo.RegionAccessCnt,
      RteTotalCntInfo.RegionCollCnt ,
      RteTotalCntInfo.RegionWaitCnt,
      RteTotalCntInfo.VbegexclTasCollCnt,
      RteTotalCntInfo.VendexclTasCollCnt );
      sql60_write_to_util_diag(Line, Len);
#endif

#ifdef TEST_TASK_DETAILS
#endif

  tsp92_RteTaskDetailsInfo  RteTaskDetailsInfo ;
  Found = vGetRteInfoTaskDetailsFirst ( &RteTaskDetailsInfo ) ;
  while  ( Found )
  {
    Len = sprintf(Line,"%2d %12.12s %5d %12.12s\n",
          RteTaskDetailsInfo.TaskIndex,
          RteTaskDetailsInfo.TaskName.asCharp(), 
          RteTaskDetailsInfo.ApplPid, 
          RteTaskDetailsInfo.RemoteApplNode.asCharp() ) ;

    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"%5.0f %5.0f %5.0f %5.0f\n",
          RteTaskDetailsInfo.DispatcherCnt,
          RteTaskDetailsInfo.CommandCnt, 
          RteTaskDetailsInfo.TaskSelfSuspCnt, 
          RteTaskDetailsInfo.RegionAccessCnt ) ;

    Len = sprintf(Line,"%5.0f %5.0f %5.0f %5.2f\n",
          RteTaskDetailsInfo.DispatcherCnt,
          RteTaskDetailsInfo.CommandCnt, 
          RteTaskDetailsInfo.TaskSelfSuspCnt, 
          RteTaskDetailsInfo.RegionAccessCnt ) ;


    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"BackupR: %5.0f %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.BackupRead.Cnt,
          RteTaskDetailsInfo.BackupRead.Pages, 
          RteTaskDetailsInfo.BackupRead.AvgAbsTime, 
          RteTaskDetailsInfo.BackupRead.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"BackupW: %5.0f %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.BackupWrite.Cnt,
          RteTaskDetailsInfo.BackupWrite.Pages, 
          RteTaskDetailsInfo.BackupWrite.AvgAbsTime, 
          RteTaskDetailsInfo.BackupWrite.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"DEVIOR: %5.0f %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.DEVIORead.Cnt,
          RteTaskDetailsInfo.DEVIORead.Pages, 
          RteTaskDetailsInfo.DEVIORead.AvgAbsTime, 
          RteTaskDetailsInfo.DEVIORead.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);
    
    Len = sprintf(Line,"DEVIOW: %5.0f %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.DEVIOWrite.Cnt,
          RteTaskDetailsInfo.DEVIOWrite.Pages, 
          RteTaskDetailsInfo.DEVIOWrite.AvgAbsTime, 
          RteTaskDetailsInfo.DEVIOWrite.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"SelfIOR: %5.0f %5.0f %5.2f\n",
          RteTaskDetailsInfo.SelfIORead.Cnt,
          RteTaskDetailsInfo.SelfIORead.Pages, 
          RteTaskDetailsInfo.SelfIORead.AvgAbsTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"SelfIOW: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.SelfIOWrite.Cnt,
          RteTaskDetailsInfo.SelfIOWrite.Pages, 
          RteTaskDetailsInfo.SelfIOWrite.AvgAbsTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"Vsuspend: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.Vsuspend.Cnt,
          RteTaskDetailsInfo.Vsuspend.AvgAbsTime, 
          RteTaskDetailsInfo.Vsuspend.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"Vwait: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.Vwait.Cnt,
          RteTaskDetailsInfo.Vwait.AvgAbsTime, 
          RteTaskDetailsInfo.Vwait.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"Vsleep: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.Vsleep.Cnt,
          RteTaskDetailsInfo.Vsleep.AvgAbsTime, 
          RteTaskDetailsInfo.Vsleep.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"ReplyReceive: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.ReplyReceive.Cnt,
          RteTaskDetailsInfo.ReplyReceive.AvgAbsTime, 
          RteTaskDetailsInfo.ReplyReceive.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"ReceiveReply: %5.0f %5.2f %5.2f\n",
          RteTaskDetailsInfo.ReceiveReply.Cnt,
          RteTaskDetailsInfo.ReceiveReply.AvgAbsTime, 
          RteTaskDetailsInfo.ReceiveReply.AvgRelTime ) ;
    sql60_write_to_util_diag(Line, Len);

    Len = sprintf(Line,"---------------------------------------------------------------------------\n");
    sql60_write_to_util_diag(Line, Len);

    Found = vGetRteInfoTaskDetailsNext ( &RteTaskDetailsInfo ) ;
  }
}
#endif
// ============================= CONNECTION ======================================
static void eo922_TrimTimeInfo(char *ptr, int length)
{
    while ( length-- > 0 )
    {
        if ( ptr[length] == '\n' || ptr[length] == '\r' )
        {
            ptr[length] = ' ';
        }
        else
        {
            break;
        }
    }
}

#if defined(WIN32)
#  define TASK_CONNECTION_RECORD(tsk_) ((tsk_)->pUserCommCtrl)
#  define TASK_UKT_INDEX(tsk_) UKT_INDEX((tsk_)->pUKT)
#else
#  define TASK_CONNECTION_RECORD(tsk_) ((tsk_)->connection)
#  define TASK_UKT_INDEX(tsk_) UKT_INDEX((tsk_)->ukt)
#endif
externC tsp00_Bool vGetRteInfoConnectionNext ( tsp92_RteConnectionInfo *RteConnectionInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoConnectionNext"));

  tsp00_Int4 currentTask = RteConnectionInfo->TaskIndex + 1;
  char tmpStringBuffer[128];
#if !defined(WIN32)
  struct tm resultBuffer;
#endif
  struct tm *pTime;
  TASK_CTRL pTaskCtrl = TASK_AT_INDEX(currentTask);

  for ( pTaskCtrl = TASK_AT_INDEX(currentTask);
        pTaskCtrl <= KGS->pLastTaskCtrl;
        ++currentTask, ++pTaskCtrl )
  {
      if ( (TASK_CONNECTION_RECORD(pTaskCtrl) != 0)
        && (TASK_STATE(pTaskCtrl) != TSK_INACTIVE_EO00)
        && (TASK_STATE(pTaskCtrl) != TSK_CONNECTWAIT_EO00) )
      {
          break;
      }
  }

  if ( pTaskCtrl > KGS->pLastTaskCtrl )
  {
      return false;
  }

  SAPDB_MemFillNoCheck ( RteConnectionInfo, 0, sizeof( tsp92_RteConnectionInfo ) ) ;

  RteConnectionInfo->TaskIndex = currentTask;
  RteConnectionInfo->UKTId = TASK_UKT_INDEX(pTaskCtrl);

#ifdef WIN32
  RteConnectionInfo->ApplPid  = pTaskCtrl->pUserCommCtrl->pidClientPID;
  RteConnectionInfo->PeerPid = TASK_CONNECTION_RECORD(pTaskCtrl)->pidLocalClientPID;
  pTime = localtime(  (time_t *)&(TASK_CONNECTION_RECORD(pTaskCtrl)->ulConnected) );
  RteConnectionInfo->ConnectState.c2p ( eo92GetConnectStateName( sql53k_get_connection_state(pTaskCtrl),
                                    tmpStringBuffer, sizeof(tmpStringBuffer) ) );
#else
  RteConnectionInfo->ApplPid  = atoi ( sql45_peer_pid(pTaskCtrl, tmpStringBuffer) );
  RteConnectionInfo->PeerPid = TASK_CONNECTION_RECORD(pTaskCtrl)->ci_peer_pid;
  pTime = localtime_r( &(TASK_CONNECTION_RECORD(pTaskCtrl)->ci_connect_time), &resultBuffer );
  RteConnectionInfo->ConnectState.c2p ( eo92GetConnectStateName( TASK_CONNECTION_RECORD(pTaskCtrl)->ci_state,
                                    tmpStringBuffer, sizeof(tmpStringBuffer) ) );
#endif

  RteConnectionInfo->TaskName.c2p (eo92GetTaskTypeName ( TASK_TYPE(pTaskCtrl) ) );

  if ( RteConnectionInfo->ApplPid != undef_ceo00 )
  {
	if ( *PTR_TO_PEER_NODE(pTaskCtrl) != '\0' ) 
    {
            RteConnectionInfo->RemoteApplNode.c2p( PTR_TO_PEER_NODE(pTaskCtrl) ) ;
    }
	else
    {
            RteConnectionInfo->RemoteApplNode.c2p( PTR_TO_DB_NODE ) ;
    }
  }
  else
  {
	RteConnectionInfo->RemoteApplNode.c2p("");
  }

#if !defined(WIN32)
  /* manual page for asctime_r says at least 26 characters... */
  {
      SAPDB_Char timeBuffer[32];
#if (defined(SUN) || defined(SOLARIS)) && !defined(_POSIX_PTHREAD_SEMANTICS)
      RteConnectionInfo->ConnectTime.c2p (asctime_r( pTime, &timeBuffer[0], sizeof(timeBuffer) ) );
#else
      RteConnectionInfo->ConnectTime.c2p (asctime_r( pTime, &timeBuffer[0] ) );
#endif
  }
#else
  RteConnectionInfo->ConnectTime.c2p (asctime( pTime ) ); /* no not MT save variant on WIN32... */
#endif
  eo922_TrimTimeInfo( RteConnectionInfo->ConnectTime.asCharp(), RteConnectionInfo->ConnectTime.length() );

  return ( true ) ;
}

/*------------------------------*/

externC tsp00_Bool vGetRteInfoConnectionFirst ( tsp92_RteConnectionInfo *RteConnectionInfo )
{
  ROUTINE_DBG_MEO60 (_T("vGetRteInfoConnectionFirst"));

  SAPDB_MemFillNoCheck ( RteConnectionInfo, 0, sizeof( tsp92_RteConnectionInfo ) ) ;

  return ( vGetRteInfoConnectionNext ( RteConnectionInfo ) ) ;
}

