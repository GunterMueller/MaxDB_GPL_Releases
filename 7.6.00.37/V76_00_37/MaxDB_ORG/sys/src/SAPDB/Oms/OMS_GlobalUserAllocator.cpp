/*!
 * \file    OMS_UserAllocator.cpp
 * \author  ChristophR
 * \brief   LiveCache user allocator (for session-specific memory).
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

#include "Oms/OMS_GlobalUserAllocator.hpp"
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "hsp77.h"
#include "hak34.h"
#include "hgg01_3.h"
#include "hgg01_1.h"
#include "gsp03_1.h"

#define REGISTER_ALLOCATOR true

static char *decodeType(OmsCallbackInterface::BlockType tp)
{
  switch (tp) {
  case OmsCallbackInterface::BlockCOMRoutine:
    return "COM";
  case OmsCallbackInterface::BlockSession:
    return "session";
  case OmsCallbackInterface::BlockGlobal:
    return "global";
  case OmsCallbackInterface::BlockTransaction:
    return "TX";
  case OmsCallbackInterface::BlockInvalid:
    return "invalid";
  case OmsCallbackInterface::BlockFreed:
    return "free";
  default:
    return "???";
  }
}

/*---------------------------------------------------------------------------*/

static void dumpMessage(bool        isOpError, 
                        SAPDB_UTF8* pMsg,
                        int         msgLen)
{
  tsp00_C40 c40;
  unsigned char* pC40    = &c40[0];
  int            restLen = sizeof(c40) ;
  memset (c40, ' ', sizeof(c40));
  while (true)
  {
    int characterSize = (int) SAPDB_UTF8Basis::CharacterSize(pMsg);

    if ((0 == characterSize) || (restLen < characterSize) || (msgLen <= 0))
    {
      // !"§$ TODO g01optextmsg (sp3p_console, (isOpError) ? sp3m_error : sp3m_info, csp3_ak_msg, csp3_n_obj, c40);
      if (msgLen <= 0)
      {
        return;
      }
      memset (c40, ' ', sizeof(c40));
      pC40    = &c40[0];
      restLen = sizeof(c40);
      if (0 == characterSize)
      {
        // we are out of step
        sp77sprintf ( c40, sizeof(c40), "invalid UTF8 string, truncated");
        // !"§$ TODO g01optextmsg (sp3p_console, sp3m_error, csp3_ak_msg, csp3_n_obj, c40);
        return;
      }
    }
    for (int ix = 0; ix < characterSize; ++ix)
    {
      *pC40 = *pMsg;
      ++pC40;
      ++pMsg;
    }
    restLen -= characterSize;
    msgLen  -= characterSize;
  }
}

//extern char *decodeType(OmsCallbackInterface::BlockType tp);
//
//extern void dumpMessage(bool isOpError, SAPDB_UTF8* pMsg, int msgLen);

/*---------------------------------------------------------------------------*/

OMS_GlobalUserAllocator::OMS_GlobalUserAllocator(
  const char                *Identifier,
  SAPDBMem_IRawAllocator    &BaseAllocator,
  SAPDB_ULong                FirstBlockSize,
  SAPDB_ULong                SupplementBlockSize)
  : SAPDBMem_RawAllocator((const SAPDB_UTF8*) Identifier, BaseAllocator, &m_lock, FirstBlockSize, 
  SupplementBlockSize, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS, SAPDBMEM_ALLOCATOR_UNLIMITED, !REGISTER_ALLOCATOR), 
  m_callback(NULL),
  m_heapTracer(OMS_AllocatorTracer::KERNEL_CRASH),
  m_lock()
{ 
  SetTracer     (&m_heapTracer);
  SetBadAllocHandler(SAPDBMem_RawAllocator::NO_THROW_HANDLER);
  SetCheckLevel (OMS_Globals::GetKernelInterface()->GetHeapCheckLevel());
  OMS_Globals::GetKernelInterface()->RegisterAllocator(GetAllocatorInfo());
}

/*---------------------------------------------------------------------------*/

OMS_GlobalUserAllocator::~OMS_GlobalUserAllocator()
{
  OMS_Globals::GetKernelInterface()->DeregisterAllocator(GetAllocatorInfo());
}

/*---------------------------------------------------------------------------*/

void *OMS_GlobalUserAllocator::operator new(size_t sz, SAPDBMem_IRawAllocator &alloc){
  return alloc.Allocate(sz);
}

/*---------------------------------------------------------------------------*/  
#if defined(OMS_PLACEMENT_DELETE)
void OMS_GlobalUserAllocator::operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc){
  alloc.Deallocate(ptr);
}
#endif
  
/*---------------------------------------------------------------------------*/

void OMS_GlobalUserAllocator::deleteSelf(SAPDBMem_IRawAllocator &alloc){
  this->~OMS_GlobalUserAllocator();
  alloc.Deallocate(this);
}

/*---------------------------------------------------------------------------*/

void *OMS_GlobalUserAllocator::omsAllocate(size_t size) // allocation
{
  return Allocate(size);
}

/*---------------------------------------------------------------------------*/

void OMS_GlobalUserAllocator::omsDeallocate(void *p)    // deallocation
{
  if ( p == NULL){
    // free(0) has no effect (PTS 1133478)
    return;
  }
  if (GetAllocator(p) != this) {
    SAPDB_ULong size, callstack;
    bool inUse;
    GetChunkInfo(p, inUse, size, callstack);
    if (inUse) {
      SetChunkProperties(p, 0);
    }
    // TODO: use memory debugger's allocator m_callback instead to report the
    // block to the user (when memory debugger integrated)
    memInvalidFree(p, callstack, GetChunkSize(p), OmsCallbackInterface::BlockGlobal, 
      inUse ? OmsCallbackInterface::BlockInvalid : OmsCallbackInterface::BlockFreed);
    return;
  }
  int code = GetChunkProperties(p);
  if (code == 1) {
    Deallocate(p);
  } else {
    SAPDB_ULong size, callstack;
    bool inUse;
    GetChunkInfo(p, inUse, size, callstack);
    if (inUse) {
      SetChunkProperties(p, 0);
    }
    // TODO: use memory debugger's allocator m_callback instead to report the
    // block to the user (when memory debugger integrated)
    memInvalidFree(p, callstack, size,
      OmsCallbackInterface::BlockGlobal, (OmsCallbackInterface::BlockType) code);
  }
}

/*---------------------------------------------------------------------------*/

bool OMS_GlobalUserAllocator::omsCheckAllocatorEmpty()  // check for emptyness
{
  // make emptyness check? doesn't make sense...
  return true;
}

/*---------------------------------------------------------------------------*/

int OMS_GlobalUserAllocator::omsGetAllocatorType() const
{
  return 1;
}

/*---------------------------------------------------------------------------*/

void OMS_GlobalUserAllocator::memLeftOver(const void *blk, SAPDB_ULong stack, size_t size,
                                              OmsCallbackInterface::BlockType blockType)
{
  char buf[256];
  int bufsize = sp77sprintf(buf, sizeof(buf), "LEAK: %p, size %d, type %s",
    blk, size, decodeType(blockType));
  dumpMessage(false, reinterpret_cast<SAPDB_UTF8*>(buf), bufsize);
  // TODO: dump stack traces
  if (m_callback) {
    m_callback->omsMemLeftOver(blk, size, blockType);
  }
}

/*---------------------------------------------------------------------------*/

void OMS_GlobalUserAllocator::memInvalidFree(const void *blk, SAPDB_ULong stack, size_t size,
                                                 OmsCallbackInterface::BlockType blockType, OmsCallbackInterface::BlockType freeType)
{
  char buf[256];
  int bufsize = sp77sprintf(buf, sizeof(buf), "INVALID FREE: %p, size %d, type %s, alloc %s",
    blk, size, decodeType(freeType), decodeType(blockType));
  dumpMessage(false, reinterpret_cast<SAPDB_UTF8*>(buf), bufsize);
  // TODO: dump stack traces
  if (m_callback) {
    m_callback->omsMemInvalidFree(blk, size, blockType, freeType);
  }
}

/*---------------------------------------------------------------------------*/

void OMS_GlobalUserAllocator::omsRegisterCallback(OmsCallbackInterface *cb)
{
  m_callback = cb;
}

/*---------------------------------------------------------------------------*/

OmsCallbackInterface *OMS_GlobalUserAllocator::omsGetCallback() const
{
  return m_callback;
}