/*!
 * \file    OMS_CheckedUserAllocator.cpp
 * \author  ChristophR
 * \brief   LiveCache user allocator checked wrapper (for session-specific memory).
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

#include "Oms/OMS_CheckedUserAllocator.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_UserAllocator.hpp"

/*---------------------------------------------------------------------------*/

OMS_CheckedUserAllocator::OMS_CheckedUserAllocator(
  const char* pName, 
  tsp00_TaskId taskId,
  OMS_UserAllocator *parent, 
  int code,
  bool DoRegister) :
  m_alloc(parent), 
  m_code(code), 
  m_allocSize(0),
  m_doRegister(DoRegister),
  m_bytesUsed(0), 
  m_maxBytesUsed(0), 
  m_allocCnt(0), 
  m_deallocCnt(0),
  m_allocatorInfo (0)
{
    SAPDB_UTF8 heapName[41];
    sp77sprintf (REINTERPRET_CAST(char*, &heapName[0]), sizeof(heapName), "%s T%03d", pName, taskId);
    strncpy((char *)&m_name[0], (const char *)heapName, sizeof(heapName)-1);
    m_name[sizeof(m_name)-1] = 0;
    if(m_doRegister){
      m_allocatorInfo = RTEMem_AllocatorInfo(&m_name[0], this, parent->GetIdentifier());
      OMS_Globals::m_globalsInstance->GetKernelInterface()->RegisterAllocator(m_allocatorInfo);
    }
}

/*---------------------------------------------------------------------------*/

OMS_CheckedUserAllocator::~OMS_CheckedUserAllocator()
{
    omsCheckAllocatorEmpty();
    if(m_doRegister){
      OMS_Globals::m_globalsInstance->GetKernelInterface()->DeregisterAllocator(m_allocatorInfo);
    }
}

/*---------------------------------------------------------------------------*/

void *OMS_CheckedUserAllocator::omsAllocate(size_t size) // allocation
{
  bool isUserRequest = true;
  ++m_allocCnt;
  void *ptr = m_alloc->Allocate(size);
  unsigned long chunkSize = m_alloc->GetChunkSize(ptr);
  m_pSession->MonitorAlloc(chunkSize, isUserRequest);
  m_bytesUsed += chunkSize;
  if (m_bytesUsed > m_maxBytesUsed)
  {
      m_maxBytesUsed = m_bytesUsed;
  }
  m_alloc->SetChunkProperties(ptr, m_code);
  return ptr;
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::omsDeallocate(void *p)    // deallocation
{
  if ( p == NULL){
    // free(0) has no effect (PTS 1133478)
    return;
  }
  // decrement alloc size and check code
  int code = m_alloc->GetChunkProperties(p);
  if (m_alloc->GetAllocator(p) != m_alloc) {
    SAPDB_ULong size, callstack;
    bool inUse;
    m_alloc->GetChunkInfo(p, inUse, size, callstack);
    if (inUse) {
      m_alloc->SetChunkProperties(p, 0);
    }
    // TODO: use memory debugger's allocator callback instead to report the
    // block to the user (when memory debugger integrated)
    m_alloc->memInvalidFree(p, callstack, m_alloc->GetChunkSize(p), 
      (OmsCallbackInterface::BlockType) m_code, inUse ?
      (OmsCallbackInterface::BlockType) code : OmsCallbackInterface::BlockFreed);
    return;
  }
  if (code == m_code) {
    bool isUserRequest = true;
    unsigned long chunkSize = m_alloc->GetChunkSize(p);
    m_pSession->MonitorDealloc(chunkSize, isUserRequest);
    m_bytesUsed -= chunkSize;
    ++m_deallocCnt;
    m_alloc->SetChunkProperties(p, 0);
    m_alloc->Deallocate(p);
  } else {
    SAPDB_ULong size, callstack;
    bool inUse;
    m_alloc->GetChunkInfo(p, inUse, size, callstack);
    if (inUse) {
      m_alloc->SetChunkProperties(p, 0);
    }
    // TODO: use memory debugger's allocator callback instead to report the
    // block to the user (when memory debugger integrated)
    m_alloc->memInvalidFree(p, callstack, size,
      (OmsCallbackInterface::BlockType) m_code,
      inUse ? ((OmsCallbackInterface::BlockType) code) :
      OmsCallbackInterface::BlockFreed);
  }
}

/*---------------------------------------------------------------------------*/

void* OMS_CheckedUserAllocator::Allocate(SAPDB_ULong ByteCount)
{
  return omsAllocate(ByteCount);
}

/*---------------------------------------------------------------------------*/

void* OMS_CheckedUserAllocator::Allocate(SAPDB_ULong ByteCount, const void*)
{
  return omsAllocate(ByteCount);
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::Deallocate(void* p)
{
  omsDeallocate(p);
}

/*---------------------------------------------------------------------------*/

bool OMS_CheckedUserAllocator::omsCheckAllocatorEmpty()  // check for emptyness
{
  if (m_bytesUsed != 0) {
    bool empty = true;
    SAPDBMem_RawAllocator::HeapIterator iter = m_alloc->Begin();
    while (iter) {
      const void *p = iter();
      bool inUse;
      SAPDB_ULong chunkSize, stackId;
      m_alloc->GetChunkInfo(p, inUse, chunkSize, stackId);
      if (inUse) {
        int typ = m_alloc->GetChunkProperties(p);
        if (typ >= m_code) {
          m_alloc->SetChunkProperties(p, 0);
          // TODO: use memory debugger's allocator callback instead to report the
          // block to the user (when memory debugger integrated)
          m_alloc->memLeftOver(p, stackId, chunkSize,
            (OmsCallbackInterface::BlockType) typ);
          empty = false;
        }
      }
      ++iter;
    }
    m_bytesUsed = 0;
    return empty;
  }
  return true;
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::omsRegisterCallback(OmsCallbackInterface *cb)
{
  reinterpret_cast<OMS_UserAllocator*>(m_alloc)->omsRegisterCallback(cb);
}

/*---------------------------------------------------------------------------*/

OmsCallbackInterface *OMS_CheckedUserAllocator::omsGetCallback() const
{
  return reinterpret_cast<OMS_UserAllocator*>(m_alloc)->omsGetCallback();
}

/*---------------------------------------------------------------------------*/

int OMS_CheckedUserAllocator::omsGetAllocatorType() const
{
  return m_code;
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::CalcStatistics(SAPDB_ULong &BytesUsed,
        SAPDB_ULong &MaxBytesUsed,
        SAPDB_ULong &BytesControlled) 
{
    BytesUsed       = m_bytesUsed;
    MaxBytesUsed    = m_maxBytesUsed;
    BytesControlled = m_bytesUsed;
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_allocCnt;
    CountDealloc = m_deallocCnt;
}

/*---------------------------------------------------------------------------*/

void OMS_CheckedUserAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_allocCnt;
    CountDealloc = m_deallocCnt;
}

/*---------------------------------------------------------------------------*/

const SAPDB_UTF8 *OMS_CheckedUserAllocator::GetIdentifier() const
{
    return &m_name[0];
}