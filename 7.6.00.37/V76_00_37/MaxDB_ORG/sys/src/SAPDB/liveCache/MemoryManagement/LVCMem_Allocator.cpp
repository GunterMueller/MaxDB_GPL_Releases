/****************************************************************************

  module      : LVCMem_Allocator.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache memory management 
  description : Block allocator for LVCMem_Allocator

                This allocator is based on RteMem_BlockAllocator.

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "geo573.h"
#include "heo670.h"
#include "hsp77.h"

#include "Oms/OMS_DbpError.hpp"

#include "liveCache/MemoryManagement/LVCMem_Allocator.h"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_BlockAllocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_EmergencyAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/RTE_Message.hpp"
#include "liveCache/LVC_LibOmsInterface.hpp"
#include "Oms/OMS_Types.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_LiveCache.h"
#include "KernelCommon/MemoryManagement/KernelMem_RawAllocatorTracer.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"

/*-----------------------------------------------------*/

class LVCMem_SignalRawTracer : public  SAPDBMem_RawAllocator::ITracerAndExceptionHandler 
{
public :
    virtual void Dump (void* p, int size)
    {
    }
    virtual void Trace (const char* msg)
    {
        eo670WriteString(msg);
    }
    virtual void VTrace (const char* msg)
    {
    }

    virtual void Exception(const char* msg)
    {
    }
};

/*-----------------------------------------------------*/

class LVCMem_ReserveEmergencyAllocator
{
public :
  LVCMem_ReserveEmergencyAllocator()
    : m_release(false)
    , m_currTaskId(0)
  {
    m_currTaskId = LVCMem_Wrapper::GetPid();
    vgetpid(m_currTaskId);
    m_release = LVCMem_EmergencyAllocator::Instance().Reserve(m_currTaskId);
  }
  ~LVCMem_ReserveEmergencyAllocator()
  {
    if (m_release)
    {
      LVCMem_EmergencyAllocator::Instance().Release(m_currTaskId);
    }
  }
private :
  bool                              m_release;
  LVCMem_EmergencyAllocator::TaskId m_currTaskId;
};

/*-----------------------------------------------------*/

class LVCMem_OmsAllocatorWrapper : public SAPDBMem_IRawAllocator
{
public :
   LVCMem_OmsAllocatorWrapper(LVCMem_Allocator* pLvcAllocator)
       : m_lvcAllocator(pLvcAllocator)
   {
   }
   virtual void* Allocate(SAPDB_ULong ByteCount)
   {
       const bool cIsUsrRequest = true;
       return m_lvcAllocator->AllocateImplementation(ByteCount, !cIsUsrRequest);
   }
   virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
   {
       const bool cIsUsrRequest = true;
       return m_lvcAllocator->AllocateImplementation(ByteCount, !cIsUsrRequest);
   }
   virtual void* AllocateThrowsNothing(SAPDB_ULong ByteCount)
   {
       const bool cIsUsrRequest = true;
       const bool cThrowBadAlloc = true;
       return m_lvcAllocator->AllocateImplementation(ByteCount, !cIsUsrRequest, !cThrowBadAlloc);
   }
   virtual void Deallocate(void* p)
   {
       const bool cIsUsrRequest = true;
       m_lvcAllocator->DeallocateImplementation(p, !cIsUsrRequest);
   }
   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
   {
       m_lvcAllocator->GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
   }
   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
   {
       m_lvcAllocator->GetCallStatistics(CountAlloc, CountDealloc);
   }
   virtual const SAPDB_UTF8 *GetIdentifier() const
   {
       return m_lvcAllocator->GetIdentifier();
   }
private :
    LVCMem_Allocator* m_lvcAllocator;
};

/*-----------------------------------------------------*/

LVCMem_Allocator*       LVCMem_Allocator::m_Instance    = 0;
SAPDBMem_IRawAllocator* LVCMem_Allocator::m_OmsInstance = 0;

/*-----------------------------------------------------*/

LVCMem_Allocator::LVCMem_Allocator()
: m_Monitoring(false)
, m_askForClient(false)
, m_SubAllocatorCount(0)
, m_firstAllocator( (const SAPDB_UTF8 *) "LVCMem_Allocator_001",
      LVCMem_BlockAllocator::Instance(), 1024 * 1024, 0,
      SAPDBMem_SynchronizedRawAllocator::FREE_RAW_EXTENDS)
{
    m_SubAllocators[0] = &m_firstAllocator;
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator LVCMem_Allocator::Begin()
{
  ContentIterator iter(*this);
  ++iter.m_members->m_refCount;
  return iter;
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::CalcStatistics
(
 SAPDB_ULong &BytesUsed,
 SAPDB_ULong &MaxBytesUsed,
 SAPDB_ULong &BytesControlled
 )
{
    BytesUsed       = 0;
    MaxBytesUsed    = 0;
    BytesControlled = 0;
    
    for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
    {
        SAPDB_ULong Used;
        SAPDB_ULong MaxUsed;
        SAPDB_ULong Controlled;
        this->GetSubAllocator(ix).CalcStatisticsConst(Used, MaxUsed, Controlled);
        BytesUsed       += Used;
        MaxBytesUsed    += MaxUsed;
        BytesControlled += Controlled;
    }
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
													  SAPDB_ULong &CountDealloc) const
{
	CountAlloc   = 0;
    CountDealloc = 0;
    
    for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
    {
        SAPDB_ULong countAlloc;
        SAPDB_ULong countDealloc;
        this->GetSubAllocator(ix).GetBaseAllocatorCallStatistics(countAlloc, countDealloc);
        CountAlloc   += countAlloc;
        CountDealloc += countDealloc;
    }
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::GetCallStatistics
(
 SAPDB_ULong &CountAlloc,
 SAPDB_ULong &CountDealloc
 ) const
{
    CountAlloc   = 0;
    CountDealloc = 0;
    
    for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
    {
        SAPDB_ULong countAlloc;
        SAPDB_ULong countDealloc;
        this->GetSubAllocator(ix).GetCallStatistics (countAlloc, countDealloc); 
        CountAlloc   += countAlloc;
        CountDealloc += countDealloc;
    }
}

/*-----------------------------------------------------*/

int LVCMem_Allocator::GetCallStackLevel() const
{
    return this->GetSubAllocator(this->FirstAllocatorIndex()).GetCallStackLevel();
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::DisableClientInquiry()
{
  m_askForClient = false;
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::GetEmergencyAllocatorInfo
(
 SAPDB_ULong &BytesUsed,
 SAPDB_ULong &BytesControlled,
 SAPDB_ULong &MaxBytesUsed
 ) const
{
    LVCMem_EmergencyAllocator::Instance().GetEmergencyAllocatorInfo(BytesUsed, 
        BytesControlled, MaxBytesUsed);
}

/*-----------------------------------------------------*/

SAPDB_ULong LVCMem_Allocator::GetErrorCount() const
{
    int errorCnt = 0;
    for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
    {
        errorCnt += this->GetSubAllocator(ix).GetErrorCount();

    }
    return errorCnt;
}

/*-----------------------------------------------------*/


void LVCMem_Allocator::DeallocateInEmergencyAllocator(void* p)
{
  LVCMem_EmergencyAllocator::Instance().Deallocate(p);
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::Create
(
 int         subAllocatorCount,
 SAPDB_ULong RawChunkSize,
 SAPDB_ULong limitKB,
 bool        isLiveCache
 )
{
  this->GetSubAllocator(0).SetSupplementChunkParameter(RawChunkSize * 1024);
  this->GetSubAllocator(0).SetCheckLevel();
  this->GetSubAllocator(0).DisableOpMessages();
  LVCMem_BlockAllocator::Instance().SetLimit(limitKB * 1024);
  LVCMem_BlockAllocator::Instance().SetDefaultBlockCount(
    LVCMem_BlockAllocator::Instance().CalcBlockCount(RawChunkSize * 1024));
  char allocatorIdentifier[24];
  if (subAllocatorCount > MAX_SUB_HEAPS)
  {
    subAllocatorCount = MAX_SUB_HEAPS;
  }
  // allocator[0] already created via Instance call
  for (int ix = 1; ix < subAllocatorCount; ++ix)
  {
      sp77sprintf (allocatorIdentifier, sizeof(allocatorIdentifier),
          "LVCMem_Allocator_%03d", ix + 1);
      m_SubAllocators[ix] = new(this->GetSubAllocator(0)) SAPDBMem_SynchronizedRawAllocator(
          (const SAPDB_UTF8 *) &allocatorIdentifier[0], 
          LVCMem_BlockAllocator::Instance(), 
          RawChunkSize * 1024,
          RawChunkSize * 1024,
          SAPDBMem_SynchronizedRawAllocator::FREE_RAW_EXTENDS);
      if (!m_SubAllocators[ix])
      {
          char msg[100];
          sp77sprintf (msg, sizeof(msg), "Could not create %s", allocatorIdentifier);
          KernelMem_RawAllocatorTracer::GetTracer().Exception(msg); 
      }
      this->GetSubAllocator(ix).SetTracer(&KernelMem_RawAllocatorTracer::GetTracer());
      this->GetSubAllocator(ix).DisableOpMessages();
  }
  if (isLiveCache)
  {
      SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(LVCMem_OmsAllocatorWrapper));
      m_OmsInstance = new((void*) Space) LVCMem_OmsAllocatorWrapper(this);
      LVCMem_EmergencyAllocator::Instance();
  }
  else
  {
      m_OmsInstance = &this->GetSubAllocator(0);
  }
  m_SubAllocatorCount = subAllocatorCount;
  m_askForClient      = true;
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::CreateInstance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(LVCMem_Allocator));
  // synchronize instance creation to be sure that instance in not
  // created twice by mistake                                
  static RTESync_Spinlock spinlock;
  RTESync_LockedScope     lockedScope(spinlock);
  if (0 == m_Instance)
  {
    LVCMem_Allocator* pInstance = new(Space) LVCMem_Allocator();
    pInstance->GetSubAllocator(0).SetTracer(&KernelMem_RawAllocatorTracer::GetTracer());
    pInstance->GetSubAllocator(0).SetFullChecking();
    pInstance->m_SubAllocatorCount = 1;
    m_Instance = pInstance;
  }
}

/*-----------------------------------------------------*/

SAPDBMem_IRawAllocator & LVCMem_Allocator::Instance()
{
  return TheLiveCacheAllocator;
}

/*-----------------------------------------------------*/

SAPDBMem_IRawAllocator& LVCMem_Allocator::LVCOmsAllocatorInstance()
{
    if (0 == m_OmsInstance)
    {
        SAPDBMem_ConsistencyException message(__CONTEXT__, "oms allocator not initialized"); 
        RTE_Crash(message); 
    }
    return *m_OmsInstance;
}

/*-----------------------------------------------------*/


void* LVCMem_Allocator::Allocate(SAPDB_ULong count)
{
    const bool cIsUsrRequest = true;
    return AllocateImplementation(count, cIsUsrRequest);
}

/*-----------------------------------------------------*/

void* LVCMem_Allocator::AllocateThrowsNothing(SAPDB_ULong count)
{
    const bool cIsUsrRequest = true;
    const bool cThrowBadAlloc = true;
    return AllocateImplementation(count, cIsUsrRequest, !cThrowBadAlloc);
}

/*-----------------------------------------------------*/

void* LVCMem_Allocator::Allocate(SAPDB_ULong count, const void * hint)
{
    const bool cIsUsrRequest = true;
    return AllocateImplementation(count, cIsUsrRequest);
}

/*-----------------------------------------------------*/

void* LVCMem_Allocator::BadAllocHandling(int allocatorIdx, SAPDB_ULong count, bool badAllocRequired, bool throwBadAlloc)
{
    void* p = (void*) 0;
    if (LVCMem_Wrapper::IsCurrentTaskInCppDBProc())
    {
        if (!badAllocRequired)
        {

            while (LVC_LibOmsInterface::Instance()->VersionUnloaded())
            {
                for (int slot = 0; slot < m_SubAllocatorCount; ++slot)
                {
                    p = this->GetSubAllocator(slot).Allocate(count);
                    if ((void*) 0 != p)
                    {
                        return p;
                    }
                }
            }

        }
        p = LVCMem_EmergencyAllocator::Instance().Allocate(count);
    }
    if (((void*) 0 == p) && throwBadAlloc)
    {
        // attach emergeny allocator for construction of bad_alloc instance
        LVCMem_ReserveEmergencyAllocator reservedScope;
        void* pContext  = (m_askForClient) ? LVCMem_Wrapper::GetContext() : NULL; // PTS 1126797
        LVCMem_Wrapper::MonitorBadAlloc(pContext);
        RTE_Message (Msg_List(Msg_List::Warning, SDBMSG_LIVECACHE_BAD_ALLOC));
        THROW_BAD_ALLOC_GEO573;
    }
  return p;
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::Deallocate(void * p)
{
    const bool cIsUsrRequest = true;
    this->DeallocateImplementation(p, cIsUsrRequest);
}

/*-----------------------------------------------------*/

SAPDB_ULong LVCMem_Allocator::GetChunkSize(const void* p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  // TODO: we can't get chunk size
  return 1;
#else
  return this->GetSubAllocator(0).GetChunkSize(p);
#endif
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::SetCallStackMonitoring (int level)
{
  for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
  {
    this->GetSubAllocator(ix).SetCallStackMonitoring(level);
  }
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::SetLimit(SAPDB_ULong limit)
{
  LVCMem_BlockAllocator::Instance().SetLimit(limit);
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::SignalAnalysis()
{
    LVCMem_SignalRawTracer tracer;
    for (int ix = this->FirstAllocatorIndex(); ix < m_SubAllocatorCount; ++ix)
    {
        this->GetSubAllocator(ix).SetTracer(&tracer);
        this->GetSubAllocator(ix).CheckConsistency();
        this->GetSubAllocator(ix).SetTracer(NULL);
    }
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator::AddressInfo::AddressInfo()
{
  memset(&m_addressInfo, 0, sizeof(m_addressInfo));
}

/*-----------------------------------------------------*/


LVCMem_Allocator::ContentIterator::AddressInfo::~AddressInfo()
{
  eo670_FreeAddressInfo(&m_addressInfo);
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator::Members::Members(LVCMem_Allocator& allocator)
: m_refCount(1)
, m_callInfoContext(NULL)
, m_vector(allocator)
, m_callStackDirIter(SAPDBMem_CallDirectory::Instance().Begin())
, m_CallAddressInfo(allocator)
{
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator::ContentIterator(LVCMem_Allocator& allocator)
: m_allocator(allocator)
, m_members(new(allocator) LVCMem_Allocator::ContentIterator::Members(allocator))
{
  if (m_members->m_vector.Initialize(SAPDBMem_CallDirectory::Instance().GetMaxCallStackId()))
  {
    for (int ix = allocator.FirstAllocatorIndex(); ix < allocator.m_SubAllocatorCount; ++ix)
    {
      SAPDBMem_SynchronizedRawAllocator::HeapIterator heapIter = allocator.GetSubAllocator(ix).Begin();
      while (heapIter)
      {
        bool        InUse;
        SAPDB_ULong ChunkSize;
        SAPDB_ULong CallStackId;
        allocator.GetSubAllocator(ix).GetChunkInfo(heapIter(), InUse, ChunkSize, CallStackId);
        if (InUse && (CallStackId < m_members->m_vector.GetSize()))
        {
          ++m_members->m_vector[CallStackId].m_ChunkCount;
          m_members->m_vector[CallStackId].m_Size += ChunkSize;
        }
        ++heapIter;
      }
    }
    // travers session allocator
    SQLMan_Context*                     pContext   = SQLMan_Context::GetContext();
    SAPDBMem_RawAllocator*              pAllocator = reinterpret_cast<SAPDBMem_RawAllocator*>(&pContext->GetAllocator());
    SAPDBMem_RawAllocator::HeapIterator heapIter   = pAllocator->Begin();
    while (heapIter)
      {
        bool        InUse;
        SAPDB_ULong ChunkSize;
        SAPDB_ULong CallStackId;
        pAllocator->GetChunkInfo(heapIter(), InUse, ChunkSize, CallStackId);
        if (InUse && (CallStackId < m_members->m_vector.GetSize()))
        {
          ++m_members->m_vector[CallStackId].m_ChunkCount;
          m_members->m_vector[CallStackId].m_Size += ChunkSize;
        }
        ++heapIter;
      }
  }
  int     callStackId;
  int     stackLength;
  void**  pStack;
  if (m_members->m_callStackDirIter)
  {
    m_members->m_callStackDirIter.Current(callStackId, stackLength, &pStack);
    if ((callStackId >= (int) m_members->m_vector.GetSize()) ||
       ( 0 == m_members->m_vector[callStackId].m_ChunkCount))
    {
      this->operator++();
    }
  }
  eo670_GetAddressInfoProlog(&m_members->m_callInfoContext);
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator::~ContentIterator()
{
  --m_members->m_refCount;
  if (0 == m_members->m_refCount)
  {
    eo670_GetAddressInfoEpilog(m_members->m_callInfoContext);
    destroy(m_members, m_allocator);
  }
}

/*-----------------------------------------------------*/

LVCMem_Allocator::ContentIterator::operator bool() const
{
  return (m_members->m_callStackDirIter) ? true : false;
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::ContentIterator::operator++()
{
  int     callStackId;
  int     stackLength;
  void**  pStack;
  do
  {
    ++m_members->m_callStackDirIter;
    if (m_members->m_callStackDirIter)
    {
      m_members->m_callStackDirIter.Current(callStackId, stackLength, &pStack);
      if ((callStackId < (int) m_members->m_vector.GetSize()) &&
         ( m_members->m_vector[callStackId].m_ChunkCount > 0))
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  while(true);
}

/*-----------------------------------------------------*/

void LVCMem_Allocator::ContentIterator::Current
(
 SAPDB_UInt4&  ChunkCount, 
 SAPDB_UInt4&  Size, 
 int&          CallStackId,
 SymbolVector& Symbols) const
{
  int     stackLength;
  void**  pStack;
  m_members->m_callStackDirIter.Current(CallStackId, stackLength, &pStack);
  ChunkCount = m_members->m_vector[CallStackId].m_ChunkCount;
  Size       = m_members->m_vector[CallStackId].m_Size;
  m_members->m_CallAddressInfo.Initialize(stackLength);
  Symbols.Initialize(stackLength);
  for (int ix = 0; ix < stackLength; ++ix)
  {
    eo670_GetAddressInfo(*pStack, &m_members->m_CallAddressInfo[ix].m_addressInfo,
      m_members->m_callInfoContext);
    Symbols[ix].m_pSymb = m_members->m_CallAddressInfo[ix].m_addressInfo.demangledSymbol;
    if (0 == *Symbols[ix].m_pSymb)
    {
      Symbols[ix].m_pSymb = m_members->m_CallAddressInfo[ix].m_addressInfo.symbol;
    }
    Symbols[ix].m_offset = m_members->m_CallAddressInfo[ix].m_addressInfo.offsetToSymbol;
    Symbols[ix].m_pcAddr = *pStack;
    pStack++;
  }
}

/*-----------------------------------------------------*/

externC void LVCMem_SignalAnalysis()
{
    TheLiveCacheAllocator.SignalAnalysis();
}

/*-----------------------------------------------------*/

void *operator new (size_t Size) THROW_NEW_GEO573
{
    LVCMem_Allocator& lvcAllocator = TheLiveCacheAllocator;
    const bool cIsUsrRequest = true;
    return lvcAllocator.AllocateImplementation(Size, cIsUsrRequest);
}

/*-----------------------------------------------------*/

void operator delete (void * pBuffer) THROW_DELETE_GEO573
{
    LVCMem_Allocator& lvcAllocator = TheLiveCacheAllocator;
    lvcAllocator.IncrementDelete(pBuffer);
    const bool cIsUsrRequest = true;
    lvcAllocator.DeallocateImplementation(pBuffer, cIsUsrRequest);
}

/*-----------------------------------------------------*/

void *operator new[] (size_t Size) THROW_NEW_GEO573
{
    LVCMem_Allocator& lvcAllocator = TheLiveCacheAllocator;
    const bool cIsUsrRequest = true;
    return lvcAllocator.AllocateImplementation(Size, cIsUsrRequest);
}

/*-----------------------------------------------------*/

void operator delete[]  (void * pBuffer) THROW_DELETE_GEO573
{
    LVCMem_Allocator& lvcAllocator = TheLiveCacheAllocator;
    lvcAllocator.IncrementDelete(pBuffer);
    const bool cIsUsrRequest = true;
    lvcAllocator.DeallocateImplementation(pBuffer, cIsUsrRequest);
}

/*-----------------------------------------------------*/
/* PTS 1115474 */

#if defined(OSF1)
#ifdef __MODEL_ANSI 
extern "C" void *__7__nw__FUl(size_t Size) { 
#else  //__MODEL_ARM 
extern "C" void *__nw__XUl(size_t Size) { 
#endif
    LVCMem_Allocator& lvcAllocator = TheLiveCacheAllocator;
    const bool cIsUsrRequest = true;
    return lvcAllocator.AllocateImplementation(Size, cIsUsrRequest);
}
#endif