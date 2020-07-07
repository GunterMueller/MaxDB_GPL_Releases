/*!
 * \file    LVCPS_KernelInterface.cpp
 * \author  MarkusSi
 * \brief   Interface for special functions in ProcServer, like memory allocation.
 */
/*

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



*/

#include "DBProc/liveCache/LVCPS_KernelInterface.hpp"
#include "liveCache/LVC_ProcServerInterface.hpp"
#include "liveCache/MemoryManagement/LVCMem_DefaultAllocator.hpp"


static SAPDBMem_IRawAllocator  *procServerDefaultAlloc = new LVCMem_DefaultAllocator();


void* LVCPS_KernelInterface::Allocate(SAPDB_ULong count, bool isUserRequest) 
{ 
  return procServerDefaultAlloc->Allocate(count);
}

void* LVCPS_KernelInterface::AllocateThrowsNothing(SAPDB_ULong count) 
{ 
  return procServerDefaultAlloc->AllocateThrowsNothing(count);
}

void LVCPS_KernelInterface::CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled)
{
}

tsp00_Int4 LVCPS_KernelInterface::Clock() 
{
  return 0;
}

void LVCPS_KernelInterface::Deallocate(void* p, bool isUserRequest)
{
  procServerDefaultAlloc->Deallocate(p);
}

void LVCPS_KernelInterface::DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}


void LVCPS_KernelInterface::GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
}

const SAPDB_UTF8* LVCPS_KernelInterface::GetAllocatorIdentifier() const
{
  return procServerDefaultAlloc->GetIdentifier();
}

void LVCPS_KernelInterface::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
}

void* LVCPS_KernelInterface::GetKnlVersion(void* pBuffer,int bufferLength) 
{
  return NULL;
}

tsp00_Int4 LVCPS_KernelInterface::GetLockRequestTimeout() 
{
  return 0;
}

int   LVCPS_KernelInterface::GetOmsRegionCnt() 
{
  return 8;
}

tsp00_Int4 LVCPS_KernelInterface::GetPacketMaxUsableArea()
{
  return 32000;
}

IliveCacheSink* LVCPS_KernelInterface::GetSinkPtr()
{
  return m_pSink;
}

tsp00_Uint4 LVCPS_KernelInterface::GetOmsVersionThreshold()
{
  return 0;
}
  
bool  LVCPS_KernelInterface::HeapThresholdReached() 
{
  return false;
}

void LVCPS_KernelInterface::IncOmsVersionUnloadCounter()
{
}

bool LVCPS_KernelInterface::IsUnicodeInstance() 
{
  return true;
}

void LVCPS_KernelInterface::RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}

void LVCPS_KernelInterface::Signal(tsp00_TaskId taskId, bool timeout)
{
}

void LVCPS_KernelInterface::SignalUnloadLiboms()
{
  // should not come, since we don't know about others
}

void LVCPS_KernelInterface::TestBadAlloc()
{
  // TODO: exception safety test
}

void LVCPS_KernelInterface::SimCtlAttachSink(class OmsHandle &handle)
{
  // simulator functionality - NOP in procserver
}

void LVCPS_KernelInterface::ConsoleMessage(const char *msg)
{
  // NOP in procserver?
}

void* LVCPS_KernelInterface::AllocateArray(SAPDB_ULong count, bool isUserRequest)
{
  return malloc(count);
}

void LVCPS_KernelInterface::DeallocateArray(void* p, bool isUserRequest)
{
  free(p);
}

void LVCPS_KernelInterface::DumpStackTrace()
{
  // TODO: where to print stacktrace?
}

unsigned int LVCPS_KernelInterface::SaveStackTrace(void **buffer, unsigned int levels)
{
  return 0;
}

void LVCPS_KernelInterface::EvalStackTrace(void **buffer, unsigned int levels,
    OmsStackTraceCallbackInterface *cb)
{
}

SAPDB_UInt8 LVCPS_KernelInterface::GetMicroTime()
{
  return 0;
}

  /// Get file information.
bool LVCPS_KernelInterface::StatFile(
      const char  *fn,
      SAPDB_UInt8 &ctime,
      SAPDB_UInt8 &fsize)
{
  return false;
}

/// Format file time into ASCII.
void LVCPS_KernelInterface::FormatTime(
      char        *buffer,
      size_t      bufsize,
      SAPDB_UInt8 time)
{
}

/// Lock request, return error code.
short LVCPS_KernelInterface::LockRequest(LVC_LockRequest &lockReq)
{
  return 0;
}

bool LVCPS_KernelInterface::UseReaderWriterLocks()
{
  return false;
}

/*-----------------------------------------------------*/

void LVCPS_KernelInterface::SetErrorCallback(DbpErrorCallbackInterface *cb){
}


