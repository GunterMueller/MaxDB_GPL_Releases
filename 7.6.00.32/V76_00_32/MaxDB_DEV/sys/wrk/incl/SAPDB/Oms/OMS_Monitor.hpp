/*!================================================================
 module:        OMS_Types.hpp

 responsible:   ThomasA
 
 special area:  OMS
 
 description:   OMS monitor
 
 see:
 
 Copyright (c) 1998-2005 SAP AG 



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 ===================================================================*/



#ifndef OMS_MONITOR_HPP
#define OMS_MONITOR_HPP

#include "gsp92.h"
#include "ggg01.h"


struct OmsMonitorData;
struct tgg01_COMMonitorInfo;

/*! 
  Class: OMS_Monitor

  Description: counts OMS events
 */

class OMS_Monitor
{
public : 
  OMS_Monitor(tsp00_TaskId taskId);

/*!----------------------------------------------------------------------
   function: AggregateCallStatistics
   description:  aggregates the statistics of the last db-procedure call into
                 the COM-Objects Monintor.
   arguments:    
     pMonInfo[in] pointer to the COM-Object monitor
   return value: none
   -----------------------------------------------------------------------*/
  
  void AggregateCallStatistics(tgg01_COMMonitorInfo* pMonInfo);

/*!----------------------------------------------------------------------
   function: GetCallStatistics
   description:  transfers the statistics of the last db-procedure call into
                 a OmsMonitorData structure
   arguments:    
     info[in] reference to the OmsMonitorData structure
   return value: none
   -----------------------------------------------------------------------*/

  void GetCallStatistics(OmsMonitorData& info) const;

/*!----------------------------------------------------------------------
   function: Reset
   description:  sets all counters to their initial state
   arguments: none    
   return value: none
   -----------------------------------------------------------------------*/
  inline void Reset();

  inline void IncReadSteam();
  inline void IncWriteSteam();
  inline void IncReadStreamRow();
  inline void IncWriteStreamRow();
  inline void IncDeref();
  inline void IncDerefLCBase();
  inline void IncKeyDeref();
  inline void IncStore();
  inline void IncLCStore();
  inline void IncLCStoreVarObj();
  inline void IncDelete();
  inline void IncLCDelete(int deleted);
  inline void IncLCDeleteVarObj();
  inline void IncLock();
  inline void IncDerefIter();
  inline void IncLogHops(int cnt);
  inline void IncLogHopsIter(int cnt);
  inline void IncExceptions();
  inline void IncOutOfDate();
  inline void IncTimeout();
  inline void IncBadAlloc();
  inline void IncSubtransRollback();
  inline void IncSubtransCommit();
  inline void MaxSubtransLevel(int level);
  inline void IncLoadVarObject();
  inline void IncLoadVarObjectLC();
  inline void IncNewObject();
  inline void IncNewKeyedObject();
  inline void IncNewVarObject();
  inline void IncStoreVarObject();
  inline void IncCreateVersion();
  inline void IncOpenVersion();
  inline void IncCloseVersion();
  inline void IncDropVersion();
  inline void IncStoreInVersion();
  inline void IncDeleteInVersion(int deleted);
  inline void IncLCDerefInVersion();
  inline void IncLCKeyDerefInVersion();
  inline void IncDeleteVarObject();
  inline void IncOmsTerminate();
  inline void MonitorAlloc(SAPDB_ULong chunkSize, bool isUsrRequest);
  inline void MonitorDealloc(SAPDB_ULong chunkSize, bool isUsrRequest);
  inline void MonitorNewConsistentView(int waitTime);
  inline void MonitorVarObjSize(int size);
  inline void SetRunTime(SAPDB_Int4 runTime);
  inline void IncOmsReadCacheMiss();   // PTS 1117571
  inline void IncOmsReadCacheHit();    // PTS 1117571
  inline void IncOmsReadVersion();     // PTS 1117571   
  inline void MaxHashChainLen(int len); // PTS 1118855
  inline void IncRehash();              // PTS 1118855
  inline void IncCntWaitOmsLockObj();
  inline void IncWaitOmsLockObjSecs(tsp00_Int4);
  inline void OmsIncOmsRelease();          // PTS 1133314
  inline void OmsIncOmsReleaseExecuted();  // PTS 1133314
private :
  SAPDB_Int4 m_runTime;
  SAPDB_Int4 m_streamRead;
  SAPDB_Int4 m_streamWrite;
  SAPDB_Int4 m_streamRowRead;
  SAPDB_Int4 m_streamRowWrite;
  SAPDB_Int4 m_NewObject;
  SAPDB_Int4 m_NewKeyedObject;
  SAPDB_Int4 m_NewVarObject;
  SAPDB_Int4 m_OmsRead;
  SAPDB_Int4 m_LCRead;
  SAPDB_Int4 m_LCKeyRead;
  SAPDB_Int4 m_OmsStore;
  SAPDB_Int4 m_LCStore;
  SAPDB_Int4 m_LCStoreVarObj;
  SAPDB_Int4 m_OmsDelete;
  SAPDB_Int4 m_LCDelete;
  SAPDB_Int4 m_LCDeleteVarObj;
  SAPDB_Int4 m_LCLock;
  SAPDB_Int4 m_LCIter;
  SAPDB_Int4 m_LogHop;
  SAPDB_Int4 m_LogHopIter;
  SAPDB_Int4 m_Exceptions;
  SAPDB_Int4 m_OutOfDate;
  SAPDB_Int4 m_Timeout;
  SAPDB_Int4 m_OutOfMemory;
  SAPDB_Int4 m_SubtransRollback;
  SAPDB_Int4 m_SubtransCommit;
  SAPDB_Int4 m_MaxSubtransLevel;
  SAPDB_Int4 m_LoadVarObj;
  SAPDB_Int4 m_LoadVarObjLC;
  SAPDB_Int4 m_StoreVarObj;
  SAPDB_Int4 m_SumVarObjSize;
  SAPDB_Int4 m_CntVarObj;
  SAPDB_Int4 m_CreateVersion;
  SAPDB_Int4 m_OpenVersion;
  SAPDB_Int4 m_CloseVersion;
  SAPDB_Int4 m_DropVersion;
  SAPDB_Int4 m_StoreInVersion;
  SAPDB_Int4 m_DeleteInVersion;
  SAPDB_Int4 m_LCReadInVersion;
  SAPDB_Int4 m_LCKeyReadInVersion;
  SAPDB_Int4 m_DeleteVarObject;
  SAPDB_Int4 m_MaxWaitNewConsView;
  SAPDB_Int4 m_MinWaitNewConsView;
  SAPDB_Int4 m_SumWaitNewConsView;
  SAPDB_Int4 m_CntWaitNewConsView;
  SAPDB_Int4 m_OmsTerminate; 
  SAPDB_Int4 m_OmsReadCacheHit;    // PTS 1117571
  SAPDB_Int4 m_OmsReadCacheMiss;   // PTS 1117571
  SAPDB_Int4 m_OmsReadVersion;     // PTS 1117571
  SAPDB_Int4 m_OmsMaxHashChainLen; // PTS 1118855
  SAPDB_Int4 m_OmsRehash;          // PTS 1118855
  SAPDB_UInt8 m_OmsHashFindCount;    //PTS 1133314
  SAPDB_UInt8 m_OmsHashFindChainSum; //PTS 1133314
  SAPDB_UInt4 m_usrAllocCnt;
  SAPDB_UInt4 m_usrDeallocCnt; 
  SAPDB_Int8 m_usrHeapDeltaMax;
  SAPDB_Int8 m_usrAlloc;
  SAPDB_UInt4 m_usrAllocChunkMax;
  SAPDB_UInt4 m_usrAllocChunkMin;
  SAPDB_Int8 m_usrDealloc;
  SAPDB_UInt4 m_usrDeallocChunkMax;
  SAPDB_UInt4 m_usrDeallocChunkMin;
  SAPDB_Int8 m_omsHeapDeltaMax;
  SAPDB_UInt4 m_omsAllocCnt;
  SAPDB_UInt4 m_omsDeallocCnt;
  SAPDB_Int8 m_omsAlloc;
  SAPDB_Int8 m_omsAllocChunkMax;
  SAPDB_Int8 m_omsAllocChunkMin;
  SAPDB_Int8 m_omsDealloc;
  SAPDB_Int8 m_omsDeallocChunkMax;
  SAPDB_Int8 m_omsDeallocChunkMin;
  SAPDB_Int4 m_CntWaitOmsLockObj;
  SAPDB_Int4 m_WaitOmsLockObjSecs;
  SAPDB_Int4 m_OmsReleaseCalled;    // PTS 1133314
  SAPDB_Int4 m_OmsReleaseExecuted;  // PTS 1133314
  tsp92_RteTaskDetailsInfo m_RteStatistics;
  tsp00_TaskId             m_taskId;
};

/*! endclass: OMS_Monitor */

inline void OMS_Monitor::IncReadSteam()
{
  ++m_streamRead;
}

inline void OMS_Monitor::IncWriteSteam()
{
  ++m_streamWrite;
}

inline void OMS_Monitor::IncReadStreamRow() 
{
  ++m_streamRowRead;
}

inline void OMS_Monitor::IncWriteStreamRow() 
{
  ++m_streamRowWrite;
}

inline void OMS_Monitor::IncDeref() 
{
  ++m_OmsRead;
}

inline void OMS_Monitor::IncDerefLCBase() 
{
  ++m_LCRead;
}

inline void OMS_Monitor::IncKeyDeref() 
{
  ++m_LCKeyRead;
}

inline void OMS_Monitor::IncStore() 
{
  ++m_OmsStore;
}

inline void OMS_Monitor::IncLCStore() 
{
  ++m_LCStore;
}

inline void OMS_Monitor::IncLCStoreVarObj()
{
  ++m_LCStoreVarObj;
}

inline void OMS_Monitor::IncDelete() 
{
  ++m_OmsDelete;
}

inline void OMS_Monitor::IncLCDelete(int deleted) 
{
  m_LCDelete += deleted;
}

inline void OMS_Monitor::IncLCDeleteVarObj() 
{
  ++m_LCDeleteVarObj;
}

inline void OMS_Monitor::IncLock() 
{
  ++m_LCLock;
}

inline void OMS_Monitor::IncDerefIter() 
{
  ++m_LCIter;
}

inline void OMS_Monitor::IncLogHops(int cnt) 
{
  m_LogHop += cnt;
}

inline void OMS_Monitor::IncLogHopsIter(int cnt) 
{
  m_LogHopIter += cnt;
}

inline void OMS_Monitor::IncExceptions() 
{
  ++m_Exceptions;
}

inline void OMS_Monitor::IncOutOfDate() 
{
  ++m_OutOfDate;
}

inline void OMS_Monitor::IncTimeout() 
{
  ++m_Timeout;
}

inline void OMS_Monitor::IncBadAlloc() 
{
  ++m_OutOfMemory;
}

inline void OMS_Monitor::IncSubtransRollback() 
{
  ++m_SubtransRollback;
}

inline void OMS_Monitor::IncSubtransCommit() 
{
  ++m_SubtransCommit;
}

inline void OMS_Monitor::MaxSubtransLevel(int level) 
{
  if (level > m_MaxSubtransLevel)
  {
    m_MaxSubtransLevel = level;
  }
}

inline void OMS_Monitor::IncLoadVarObject() 
{
  ++m_LoadVarObj;
}

inline void OMS_Monitor::IncLoadVarObjectLC() 
{
  ++m_LoadVarObjLC;
}

inline void OMS_Monitor::IncNewObject()
{
  ++m_NewObject;
}

inline void OMS_Monitor::IncNewKeyedObject()
{
  ++m_NewKeyedObject;
}

inline void OMS_Monitor::IncNewVarObject()
{
  ++m_NewVarObject;
}

inline void OMS_Monitor::IncStoreVarObject() 
{
  ++m_StoreVarObj;
}

inline void OMS_Monitor::IncCreateVersion() 
{
  ++m_CreateVersion;
}

inline void OMS_Monitor::IncOpenVersion() 
{
  ++m_OpenVersion;
}

inline void OMS_Monitor::IncCloseVersion() 
{
  ++m_CloseVersion;
}

inline void OMS_Monitor::IncDropVersion() 
{
  ++m_DropVersion;
}

inline void OMS_Monitor::IncStoreInVersion() 
{
  ++m_StoreInVersion;
}

inline void OMS_Monitor::IncDeleteInVersion(int deleted) 
{
  m_DeleteInVersion += deleted;
}

inline void OMS_Monitor::IncLCDerefInVersion() 
{
  ++m_LCReadInVersion;
}

inline void OMS_Monitor::IncLCKeyDerefInVersion() 
{
  ++m_LCKeyReadInVersion;
}

inline void OMS_Monitor::IncDeleteVarObject() 
{
  ++m_DeleteVarObject;
}

inline void OMS_Monitor::IncOmsTerminate() 
{
  ++m_OmsTerminate;
}

inline void OMS_Monitor::MonitorAlloc(SAPDB_ULong chunkSize, bool isUsrRequest)
{
  if(isUsrRequest){
    ++m_usrAllocCnt;
    m_usrAlloc += chunkSize;
    if((m_usrAlloc - m_usrDealloc) > m_usrHeapDeltaMax)
      m_usrHeapDeltaMax = m_usrAlloc - m_usrDealloc;
    if(chunkSize < m_usrAllocChunkMin) 
      m_usrAllocChunkMin = chunkSize;
    if(chunkSize > m_usrAllocChunkMax) 
      m_usrAllocChunkMax = chunkSize;
  }
  else {
    ++m_omsAllocCnt;
    m_omsAlloc += chunkSize;
    if((m_omsAlloc - m_omsDealloc) > m_omsHeapDeltaMax)
      m_omsHeapDeltaMax = m_omsAlloc - m_omsDealloc;
    if(chunkSize < m_omsAllocChunkMin) 
      m_omsAllocChunkMin = chunkSize;
    if(chunkSize > m_omsAllocChunkMax) 
      m_omsAllocChunkMax = chunkSize;
  }
}

inline void OMS_Monitor::MonitorDealloc(SAPDB_ULong chunkSize, bool isUsrRequest)
{
  if(isUsrRequest){
    ++m_usrDeallocCnt;
    m_usrDealloc += chunkSize;
    if(chunkSize < m_usrDeallocChunkMin) 
      m_usrDeallocChunkMin = chunkSize;
    if(chunkSize > m_usrDeallocChunkMax) 
      m_usrDeallocChunkMax = chunkSize;
  }
  else {
    ++m_omsDeallocCnt;
    m_omsDealloc += chunkSize;
    if(chunkSize < m_omsDeallocChunkMin) 
      m_omsDeallocChunkMin = chunkSize;
    if(chunkSize > m_omsDeallocChunkMax) 
      m_omsDeallocChunkMax = chunkSize;
  }
}

inline void OMS_Monitor::MonitorVarObjSize(int size)
{
  m_SumVarObjSize += size;
  ++m_CntVarObj;
}

inline void OMS_Monitor::MonitorNewConsistentView(int waitTime)
{

  if (waitTime > m_MaxWaitNewConsView)
  {
    m_MaxWaitNewConsView = waitTime;
  }
  if (waitTime < m_MinWaitNewConsView)
  {
    m_MinWaitNewConsView = waitTime;
  }
  m_SumWaitNewConsView += waitTime;
  ++m_CntWaitNewConsView;
}

inline void OMS_Monitor::Reset()
{
  memset (this, 0, sizeof(*this));
  m_MinWaitNewConsView = SAPDB_MAX_INT4;
}

inline void OMS_Monitor::SetRunTime(SAPDB_Int4 runTime)
{
  m_runTime = runTime;
}

inline void OMS_Monitor::IncOmsReadCacheMiss(){   // PTS 1117571
  ++m_OmsReadCacheMiss;
}

inline void OMS_Monitor::IncOmsReadCacheHit(){    // PTS 1117571
  ++m_OmsReadCacheHit;
}

inline void OMS_Monitor::IncOmsReadVersion(){     // PTS 1117571
  ++m_OmsReadVersion;
}

inline void OMS_Monitor::MaxHashChainLen(int len){  // PTS 1118855
  if (len > m_OmsMaxHashChainLen){
    m_OmsMaxHashChainLen = len;
  }
  ++m_OmsHashFindCount;         // PTS 1133314
  m_OmsHashFindChainSum += len; // PTS 1133314
}

inline void OMS_Monitor::IncRehash(){               // PTS 1118855
  ++m_OmsRehash; 
}

inline void OMS_Monitor::IncCntWaitOmsLockObj()
{
  ++m_CntWaitOmsLockObj;
}

inline void OMS_Monitor::IncWaitOmsLockObjSecs(tsp00_Int4 secs)
{
    ++m_WaitOmsLockObjSecs += secs;
}

inline void OMS_Monitor::OmsIncOmsRelease()  // PTS 1133314
{
    ++m_OmsReleaseCalled;
}

inline void OMS_Monitor::OmsIncOmsReleaseExecuted() // PTS 1133314
{
    ++m_OmsReleaseExecuted;
}
#endif
