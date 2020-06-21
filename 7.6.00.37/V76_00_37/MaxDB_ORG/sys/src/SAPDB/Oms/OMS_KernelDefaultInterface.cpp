/****************************************************************************

  module      : OMS_KernelDefaultInterface.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache  
  description : OMS kernel default interface


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
#include "Oms/OMS_KernelDefaultInterface.hpp"
#include "liveCache/MemoryManagement/LVCMem_DefaultAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_IInterface.hpp"

OMS_KernelDefaultInterface* OMS_KernelDefaultInterface::m_instance = 0;

/*-----------------------------------------------------*/

LVC_KernelInterface& OMS_KernelDefaultInterface::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(OMS_KernelDefaultInterface));
  if (0 == m_instance)
  {
    m_instance = new(Space) OMS_KernelDefaultInterface;
  }
  return *m_instance;
}

/*-----------------------------------------------------*/

void* OMS_KernelDefaultInterface::Allocate(SAPDB_ULong count, bool isUserRequest)
{
  return  RTE_IInterface::Instance().Allocator().Allocate(count);
}

/*-----------------------------------------------------*/

void* OMS_KernelDefaultInterface::AllocateThrowsNothing(SAPDB_ULong count)
{
  return  RTE_IInterface::Instance().Allocator().AllocateThrowsNothing(count);
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled)
{
  RTE_IInterface::Instance().Allocator().CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
}


/*-----------------------------------------------------*/

tsp00_Int4 OMS_KernelDefaultInterface::Clock()
{
  return 0;
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::Crash(const char *msg)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::Deallocate(void* p, bool isUserRequest)
{
  RTE_IInterface::Instance().Allocator().Deallocate(p);
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::DumpChunk (void* p, int size)
{
}


/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  RTE_IInterface::Instance().Allocator().GetCallStatistics(CountAlloc, CountDealloc);
}

/*-----------------------------------------------------*/

const SAPDB_UTF8* OMS_KernelDefaultInterface::GetAllocatorIdentifier() const
{
  return RTE_IInterface::Instance().Allocator().GetIdentifier();
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  RTE_IInterface::Instance().Allocator().GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
}

/*-----------------------------------------------------*/

SAPDB_UInt4 OMS_KernelDefaultInterface::GetHeapCheckLevel() 
{
    return 0;
}

/*-----------------------------------------------------*/

bool OMS_KernelDefaultInterface::IsOneDBSystem(short &rc) 
{
    rc = e_ok;
    return false;
}

/*-----------------------------------------------------*/

bool OMS_KernelDefaultInterface::HeapThresholdReached()
{
  return false;
}

/*-----------------------------------------------------*/

void* OMS_KernelDefaultInterface::GetKnlVersion(void* pBuf, int bufSize)
{
  reinterpret_cast<char*>(pBuf)[0] = 0;
  return pBuf;
}

/*-----------------------------------------------------*/

tsp00_Int4 OMS_KernelDefaultInterface::GetLockRequestTimeout()
{
  return 0;
}

/*-----------------------------------------------------*/

int OMS_KernelDefaultInterface::GetOmsRegionCnt()
{
  return 0;
}

/*-----------------------------------------------------*/

IliveCacheSink* OMS_KernelDefaultInterface::GetSinkPtr()
{
   return NULL;
}

/*-----------------------------------------------------*/

tsp00_Uint4 OMS_KernelDefaultInterface::GetOmsVersionThreshold()
{
  return 0;
}

/*-----------------------------------------------------*/

tsp00_Int4 OMS_KernelDefaultInterface::GetPacketMaxUsableArea()
{
  return 0;
}

/*-----------------------------------------------------*/ 
  
bool OMS_KernelDefaultInterface::IsUnicodeInstance()
{
  return false;
}

/*-----------------------------------------------------*/
 
void OMS_KernelDefaultInterface::IncOmsVersionUnloadCounter()
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::Signal(tsp00_TaskId taskId, bool timeout)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::SignalUnloadLiboms()
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::TestBadAlloc()
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::SimCtlAttachSink(class OmsHandle &handle)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::ConsoleError(const char *msg)
{
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::ConsoleMessage(const char *msg)
{
	// ignore
}

/*-----------------------------------------------------*/

void* OMS_KernelDefaultInterface::AllocateArray(SAPDB_ULong count, bool isUserRequest)
{
  return RTE_IInterface::Instance().Allocator().Allocate (count);
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::DeallocateArray(void* p, bool isUserRequest)
{
  RTE_IInterface::Instance().Allocator().Deallocate(p);
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::DumpStackTrace()
{
}

/*-----------------------------------------------------*/

unsigned int OMS_KernelDefaultInterface::SaveStackTrace(void **buffer, unsigned int levels)
{
  return 0;
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::EvalStackTrace(void **buffer, unsigned int levels,
  OmsStackTraceCallbackInterface *cb)
{
}

/*-----------------------------------------------------*/

SAPDB_UInt8 OMS_KernelDefaultInterface::GetMicroTime()
{
  return 0;
}

/*-----------------------------------------------------*/

bool OMS_KernelDefaultInterface::StatFile(
    const char  *fn,
    SAPDB_UInt8 &ctime,
    SAPDB_UInt8 &fsize)
{
  return false;
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::FormatTime(
    char        *buffer,
    size_t      bufsize,
    SAPDB_UInt8 time)
{
  buffer[0] = 0;
}

/*-----------------------------------------------------*/

short OMS_KernelDefaultInterface::LockRequest(LVC_LockRequest &lockReq)
{
  return 0;
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::RegisterArea(SAPDB_Int4 areaId, const char *name)
{
}

/*-----------------------------------------------------*/

bool OMS_KernelDefaultInterface::UseReaderWriterLocks(){
  return true;
}

/*-----------------------------------------------------*/

RTE_IInterface &OMS_KernelDefaultInterface::GetKernelRunTimeInterface() {
    return RTE_IInterface::Instance();
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::SetErrorCallback(DbpErrorCallbackInterface *cb){
}

/*-----------------------------------------------------*/

void OMS_KernelDefaultInterface::VTrace(const char* msg){
}