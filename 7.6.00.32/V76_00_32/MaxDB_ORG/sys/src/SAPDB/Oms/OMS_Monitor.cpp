/****************************************************************************

  module      : OMS_Monitor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: OMS 
  description : OMS monitor

  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "ggg00.h"
#include "ggg01.h"

#include "Oms/OMS_Monitor.hpp"

#include "livecachetypes.h" // nocheck


OMS_Monitor::OMS_Monitor(tsp00_TaskId taskId)
: m_streamRead(0)
, m_streamWrite(0)
, m_streamRowRead(0)
, m_streamRowWrite(0)
, m_NewObject(0)
, m_NewKeyedObject(0)
, m_NewVarObject(0)
, m_OmsRead(0)
, m_LCRead(0)
, m_LCKeyRead(0)
, m_OmsStore(0)
, m_LCStore(0)
, m_LCStoreVarObj(0)
, m_OmsDelete(0)
, m_LCDelete(0)
, m_LCDeleteVarObj(0)
, m_LCLock(0)
, m_LCIter(0)
, m_LogHop(0)
, m_LogHopIter(0)
, m_Exceptions(0)
, m_OutOfDate(0)
, m_Timeout(0)
, m_OutOfMemory(0)
, m_SubtransRollback(0)
, m_SubtransCommit(0)
, m_MaxSubtransLevel(0)
, m_LoadVarObj(0)
, m_LoadVarObjLC(0)
, m_StoreVarObj(0)
, m_SumVarObjSize(0)
, m_CntVarObj(0)
, m_CreateVersion(0)
, m_OpenVersion(0)
, m_CloseVersion(0)
, m_DropVersion(0)
, m_StoreInVersion(0)
, m_DeleteInVersion(0)
, m_LCReadInVersion(0)
, m_LCKeyReadInVersion(0)
, m_DeleteVarObject(0)
, m_MaxWaitNewConsView(0)
, m_MinWaitNewConsView(0)
, m_SumWaitNewConsView(0)
, m_CntWaitNewConsView(0)
, m_usrAllocCnt(0)
, m_usrDeallocCnt(0) 
, m_usrHeapDeltaMax(0)
, m_usrAlloc(0)
, m_usrAllocChunkMax(0)
, m_usrAllocChunkMin(0)
, m_usrDealloc(0)
, m_usrDeallocChunkMax(0)
, m_usrDeallocChunkMin(0)
, m_omsHeapDeltaMax(0)
, m_omsAllocCnt(0)
, m_omsDeallocCnt(0)
, m_omsAlloc(0)
, m_omsAllocChunkMax(0)
, m_omsAllocChunkMin(0)
, m_omsDealloc(0)
, m_omsDeallocChunkMax(0)
, m_omsDeallocChunkMin(0)
, m_OmsTerminate(0)
, m_OmsReadCacheHit(0)    // PTS 1117571
, m_OmsReadCacheMiss(0)   // PTS 1117571
, m_OmsReadVersion(0)     // PTS 1117571
, m_OmsMaxHashChainLen(0) // PTS 1118855
, m_OmsRehash(0)          // PTS 1118855
, m_OmsHashFindCount(0)   //PTS 1133314
, m_OmsHashFindChainSum(0) //PTS 1133314
, m_OmsReleaseCalled(0)    // PTS 1133314
, m_OmsReleaseExecuted(0)  // PTS 1133314
, m_taskId(taskId)
{
  this->Reset();
}

/*------------------------------------------------------------------*/

void OMS_Monitor::AggregateCallStatistics(tgg01_COMMonitorInfo* pMonInfo)
{
  pMonInfo->cmiCounters_gg00[cmiOmsRead           ] += m_OmsRead;
  pMonInfo->cmiCounters_gg00[cmiLCRead            ] += m_LCRead;
  pMonInfo->cmiCounters_gg00[cmiLCKeyRead         ] += m_LCKeyRead;
  pMonInfo->cmiCounters_gg00[cmiOmsStore          ] += m_OmsStore;
  pMonInfo->cmiCounters_gg00[cmiLCStore           ] += m_LCStore;
  pMonInfo->cmiCounters_gg00[cmiOmsDelete         ] += m_OmsDelete;
  pMonInfo->cmiCounters_gg00[cmiLCDelete          ] += m_LCDelete;
  pMonInfo->cmiCounters_gg00[cmiLCLock            ] += m_LCLock;
  pMonInfo->cmiCounters_gg00[cmiLCIter            ] += m_LCIter;    
  pMonInfo->cmiCounters_gg00[cmiLogHop            ] += m_LogHop;
  pMonInfo->cmiCounters_gg00[cmiLogHopIter        ] += m_LogHopIter;
  pMonInfo->cmiCounters_gg00[cmiExceptions        ] += m_Exceptions;
  pMonInfo->cmiCounters_gg00[cmiOutOfDate         ] += m_OutOfDate;
  pMonInfo->cmiCounters_gg00[cmiTimeout           ] += m_Timeout;
  pMonInfo->cmiCounters_gg00[cmiOutOfMemory       ] += m_OutOfMemory;
  pMonInfo->cmiCounters_gg00[cmiSubtransRollback  ] += m_SubtransRollback;
  pMonInfo->cmiCounters_gg00[cmiSubtransCommit    ] += m_SubtransCommit;
  if (m_MaxSubtransLevel > pMonInfo->cmiCounters_gg00[cmiMaxSubtransLevel])
  {
    pMonInfo->cmiCounters_gg00[cmiMaxSubtransLevel] = m_MaxSubtransLevel;
  }
  pMonInfo->cmiCounters_gg00[cmiLoadVarObj        ] += m_LoadVarObj;
  pMonInfo->cmiCounters_gg00[cmiStoreVarObj       ] += m_StoreVarObj;
  pMonInfo->cmiCounters_gg00[cmiSumVarObjSize     ] += m_SumVarObjSize;
  pMonInfo->cmiCounters_gg00[cmiCntVarObj         ] += m_CntVarObj;
  pMonInfo->cmiCounters_gg00[cmiCreateVersion     ] += m_CreateVersion;
  pMonInfo->cmiCounters_gg00[cmiOpenVersion       ] += m_OpenVersion;
  pMonInfo->cmiCounters_gg00[cmiCloseVersion      ] += m_CloseVersion;
  pMonInfo->cmiCounters_gg00[cmiDropVersion       ] += m_DropVersion;
  pMonInfo->cmiCounters_gg00[cmiStoreInVersion    ] += m_StoreInVersion;
  pMonInfo->cmiCounters_gg00[cmiDeleteInVersion   ] += m_DeleteInVersion;
  pMonInfo->cmiCounters_gg00[cmiLCReadInVersion   ] += m_LCReadInVersion;
  pMonInfo->cmiCounters_gg00[cmiLCKeyReadInVersion] += m_LCKeyReadInVersion;
  pMonInfo->cmiCounters_gg00[cmiDeleteVarObject   ] +=  m_DeleteVarObject;
  if (m_CntWaitNewConsView > 0)
  {
    if (m_MaxWaitNewConsView > pMonInfo->cmiCounters_gg00[cmiMaxWaitNewConsView])
    {
      pMonInfo->cmiCounters_gg00[cmiMaxWaitNewConsView] = m_MaxWaitNewConsView;
    }
    if (m_MinWaitNewConsView < pMonInfo->cmiCounters_gg00[cmiMinWaitNewConsView])
    {
      pMonInfo->cmiCounters_gg00[cmiMinWaitNewConsView] = m_MinWaitNewConsView;
    }
  }
  pMonInfo->cmiCounters_gg00[cmiSumWaitNewConsView] += m_SumWaitNewConsView;
  pMonInfo->cmiCounters_gg00[cmiCntWaitNewConsView] += m_CntWaitNewConsView;
  pMonInfo->cmiCounters_gg00[cmiOmsTerminate      ] += m_OmsTerminate; 
  if (m_streamRead < pMonInfo->cmiCounters_gg00[cmiMinReadApplCallbacks])
  {
    pMonInfo->cmiCounters_gg00[cmiMinReadApplCallbacks] = m_streamRead;
  }
  if (m_streamRead > pMonInfo->cmiCounters_gg00[cmiMaxReadApplCallbacks])
  {
    pMonInfo->cmiCounters_gg00[cmiMaxReadApplCallbacks] = m_streamRead;
  }
  pMonInfo->cmiCounters_gg00[cmiSumReadApplCallbacks] += m_streamRead;
  if (m_streamRowRead < pMonInfo->cmiCounters_gg00[cmiMinStreamRowsRead])
  {
	  pMonInfo->cmiCounters_gg00[cmiMinStreamRowsRead] = m_streamRowRead;
  }
  if (m_streamRowRead > pMonInfo->cmiCounters_gg00[cmiMaxStreamRowsRead])
  {
	  pMonInfo->cmiCounters_gg00[cmiMaxStreamRowsRead] = m_streamRowRead;
  }
	pMonInfo->cmiCounters_gg00[cmiSumStreamRowsRead] += m_streamRowRead; 
  if (m_streamWrite < pMonInfo->cmiCounters_gg00[cmiMinWriteApplCallbacks])
  {
	  pMonInfo->cmiCounters_gg00[cmiMinWriteApplCallbacks] = m_streamWrite;
  }
  if (m_streamWrite > pMonInfo->cmiCounters_gg00[cmiMaxWriteApplCallbacks])
  {
	  pMonInfo->cmiCounters_gg00[cmiMaxWriteApplCallbacks] = m_streamWrite;
  }
  pMonInfo->cmiCounters_gg00[cmiSumWriteApplCallbacks] += m_streamWrite;
  if (m_streamRowWrite < pMonInfo->cmiCounters_gg00[cmiMinStreamRowsWrite])
  {
	  pMonInfo->cmiCounters_gg00[cmiMinStreamRowsWrite] = m_streamRowWrite;
  }
  if (m_streamRowWrite > pMonInfo->cmiCounters_gg00[cmiMaxStreamRowsWrite])
  {
	  pMonInfo->cmiCounters_gg00[cmiMaxStreamRowsWrite] = m_streamRowWrite;
  }
  pMonInfo->cmiCounters_gg00[cmiSumStreamRowsWrite] += m_streamRowWrite;
  pMonInfo->cmiCounters_gg00[cmiOmsReadCacheHit   ] += m_OmsReadCacheHit;  // PTS 1117571 
  pMonInfo->cmiCounters_gg00[cmiOmsReadCacheMiss  ] += m_OmsReadCacheMiss; // PTS 1117571
  pMonInfo->cmiCounters_gg00[cmiOmsReadVersion    ] += m_OmsReadVersion;   // PTS 1117571
  if (m_OmsMaxHashChainLen > pMonInfo->cmiCounters_gg00[cmiOmsMaxHashChainLen]){
    pMonInfo->cmiCounters_gg00[cmiOmsMaxHashChainLen] = m_OmsMaxHashChainLen; // PTS 1118855
  }
  pMonInfo->cmiCounters_gg00[cmiOmsRehash         ] += m_OmsRehash;           // PTS 1118855
  pMonInfo->cmiCounters_gg00[cmiCntWaitOmsLockObj ] += m_CntWaitOmsLockObj;
  pMonInfo->cmiCounters_gg00[cmiWaitOmsLockObjSecs] += m_WaitOmsLockObjSecs;
  pMonInfo->cmiCounters_gg00[cmiHashDerefCount      ] += m_OmsHashFindCount;     // PTS 1133314
  pMonInfo->cmiCounters_gg00[cmiHashSumChainAccessed] += m_OmsHashFindChainSum;  // PTS 1133314
  pMonInfo->cmiCounters_gg00[cmiOmsReleaseCalled  ]   += m_OmsReleaseCalled;     // PTS 1133314
  pMonInfo->cmiCounters_gg00[cmiOmsReleaseDone    ]   += m_OmsReleaseExecuted;   // PTS 1133314

  pMonInfo->cmiCounters_gg00[cmiMallocCnt         ]   += m_usrAllocCnt;
  pMonInfo->cmiCounters_gg00[cmiMallocSum         ]   += m_usrAlloc;
  if( m_usrAllocChunkMin < pMonInfo->cmiCounters_gg00[cmiMallocMin])
    pMonInfo->cmiCounters_gg00[cmiMallocMin] = m_usrAllocChunkMin;
  if( m_usrAllocChunkMax > pMonInfo->cmiCounters_gg00[cmiMallocMax])
    pMonInfo->cmiCounters_gg00[cmiMallocMax] = m_usrAllocChunkMax;

  pMonInfo->cmiCounters_gg00[cmiFreeCnt         ]   += m_usrDeallocCnt;
  pMonInfo->cmiCounters_gg00[cmiFreeSum         ]   += m_usrDealloc;
  if( m_usrDeallocChunkMin < pMonInfo->cmiCounters_gg00[cmiFreeMin])
    pMonInfo->cmiCounters_gg00[cmiFreeMin] = m_usrDeallocChunkMin;
  if( m_usrDeallocChunkMax > pMonInfo->cmiCounters_gg00[cmiFreeMax])
    pMonInfo->cmiCounters_gg00[cmiFreeMax] = m_usrDeallocChunkMax;
  
  //if( m_usrHeapDeltaMax < pMonInfo->cmiCounters_gg00[cmiUsrHeapMin])
  //  pMonInfo->cmiCounters_gg00[cmiUsrHeapMin] = m_usrHeapDeltaMax;
  //if( m_usrHeapDeltaMax > pMonInfo->cmiCounters_gg00[cmiUsrHeapMax])
  //  pMonInfo->cmiCounters_gg00[cmiUsrHeapMax] = m_usrHeapDeltaMax;
  //int callCnt = pMonInfo->cmiCounters_gg00[cmiDeltaAvg];
  //pMonInfo->cmiCounters_gg00[cmiUsrHeapAvg] = (pMonInfo->cmiCounters_gg00[cmiUsrHeapAvg] * (callCnt - 1) + m_usrHeapDeltaMax) / callCnt;

  //if( m_omsHeapDeltaMax < pMonInfo->cmiCounters_gg00[cmiOmsHeapMin])
  //  pMonInfo->cmiCounters_gg00[cmiOmsHeapMin] = m_omsHeapDeltaMax;
  //if( m_omsHeapDeltaMax > pMonInfo->cmiCounters_gg00[cmiOmsHeapMax])
  //  pMonInfo->cmiCounters_gg00[cmiOmsHeapMax] = m_omsHeapDeltaMax;
  //pMonInfo->cmiCounters_gg00[cmiOmsHeapAvg] = (pMonInfo->cmiCounters_gg00[cmiOmsHeapAvg] * (callCnt - 1) + m_omsHeapDeltaMax) / callCnt;


}

/*------------------------------------------------------------------*/

void OMS_Monitor::GetCallStatistics(OmsMonitorData& info) const
{
   info.m_runTime                 = m_runTime;
   info.m_cntNewObject            = m_NewObject;
   info.m_cntNewKeyedObject       = m_NewKeyedObject;
   info.m_cntNewVarObject         = m_NewVarObject; 
   info.m_cntDeref                = m_OmsRead; 
   info.m_cntDerefLCBase          = m_LCRead; 
   info.m_cntKeyDeref             = m_LCKeyRead; 
   info.m_cntDerefVarObj          = m_LoadVarObj;
   info.m_cntDerefVarObjLCBase    = m_LoadVarObjLC;
   info.m_cntOmsStore             = m_OmsStore; 
   info.m_cntStoreVarObj          = m_StoreVarObj;
   info.m_cntOmsStoreLCBase       = m_LCStore;
   info.m_cntStoreVarObjLCBase    = m_LCStoreVarObj;
   info.m_cntOmsDelete            = m_OmsDelete; 
   info.m_cntOmsDeleteVarObject   = m_DeleteVarObject;
   info.m_cntOmsDeleteLC          = m_LCDelete;          
   info.m_cntOmsDeleteVarObjectLC = m_LCDeleteVarObj;
   info.m_cntOmsLock              = m_LCLock; 
   info.m_logHops                 = m_LogHop;
   info.m_logHopsIter             = m_LogHopIter;
   info.m_cntExceptions           = m_Exceptions;
   info.m_cntOutOfDate            = m_OutOfDate;
   info.m_cntOutOfMemory          = m_OutOfMemory;
   info.m_cntSubtransRollback     = m_SubtransRollback;
   info.m_cntSubtransCommit       = m_SubtransCommit;
   info.m_maxSubtransLevel        = m_MaxSubtransLevel;
   info.m_cntNewConsistentView    = m_CntWaitNewConsView;
   if (m_CntWaitNewConsView > 0)
   {
     info.m_avgWaitNewConsView = m_SumWaitNewConsView / m_CntWaitNewConsView; 
   }
   else
   {
     info.m_avgWaitNewConsView = 0;
   }
   info.m_OmsReadCacheHit         = m_OmsReadCacheHit;    // PTS 1117571
   info.m_OmsReadCacheMiss        = m_OmsReadCacheMiss;   // PTS 1117571
   info.m_OmsReadVersion          = m_OmsReadVersion;     // PTS 1117571
   info.m_OmsMaxHashChainLen      = m_OmsMaxHashChainLen; // PTS 1118855
   info.m_OmsRehash               = m_OmsRehash;          // PTS 1118855
   info.m_cntStreamRowsRead       = m_streamRowRead;      // PTS 1119462
   info.m_cntStreamRowsWritten    = m_streamRowWrite;     // PTS 1119462
   info.m_cntWaitForOmsLockObj    = m_CntWaitOmsLockObj;
   info.m_avgWaitForOmsLockObj    = (m_CntWaitOmsLockObj > 0) ? (m_WaitOmsLockObjSecs / m_CntWaitOmsLockObj) : 0;
   info.m_taskId                  = m_taskId;
} 
