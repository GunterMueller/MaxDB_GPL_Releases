/****************************************************************************

  module      : LVCMem_BlockAllocator.cpp

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

#include "RunTime/RTE_CompilerFeatures.h"
#include "RunTime/RTE_Message.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "liveCache/MemoryManagement/LVCMem_BlockAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "SAPDBCommon/Messages/SDBMsg_LiveCache.h"


LVCMem_BlockAllocator* LVCMem_BlockAllocator::m_Instance = 0;

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

LVCMem_BlockAllocator::LVCMem_BlockAllocator()
: m_allocator(RTEMem_BlockAllocator::Instance())
, m_defaultBlockCount(0)
, m_limit(0xFFFFFFFF)
, m_allocated(0)
, m_maxAllocated(0)
, m_controlled(0)
, m_CountAlloc(0)
, m_CountDealloc(0)
, m_CountBaseAlloc(0)
, m_CountBaseDealloc(0) 
, m_firstFree(0)
, m_spinlock((const SAPDB_UTF8 *) "LVCMem_BlockAllocator")
, m_writeHeapLimitOpMsg(true)
{
	static RTEMem_AllocatorInfo AllocatorInfo(this->GetIdentifier(), 
		this, m_allocator.GetIdentifier());
    RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
}

/*-----------------------------------------------------*/

void* LVCMem_BlockAllocator::Allocate(SAPDB_ULong BlockCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(BlockCount * GetBlockSize());
  return p;
#else
  RTESync_LockedScope lockedScope(m_spinlock);
  ++m_CountAlloc;
  do
  {
      if ((BlockCount == m_defaultBlockCount) && (0 != m_firstFree))
      {
          void* p = m_firstFree;
          m_firstFree = m_firstFree->m_next;
          m_allocated += BlockCount;
          if (m_allocated > m_maxAllocated)
          {
              m_maxAllocated = m_allocated;
          }
          return p;
      }
      if (m_controlled + BlockCount <= m_limit)
      {
          void* p = m_allocator.Allocate(BlockCount);
          if (0 != p)
          {
              ++m_CountBaseAlloc;
              m_writeHeapLimitOpMsg = true;
              m_allocated += BlockCount;
              if (m_allocated > m_maxAllocated)
              {
                  m_maxAllocated = m_allocated;
              }
              m_controlled += BlockCount;
          }
          else
          {
              if (0 != m_firstFree)
              {
                  this->ReleaseFreeList();
                  continue;
              }
          }
          return p;
      }
      else
      {
          if (0 != m_firstFree)
          {
              this->ReleaseFreeList();
              continue;
          }
          if (m_writeHeapLimitOpMsg)
          {
              Msg_List msgList (Msg_List::Info,
                  SDBMSG_LIVECACHE_NO_MORE_MEMROY,
                  Msg_Arg("BYTES_USED", m_allocated * GetBlockSize()),
                  Msg_Arg("BYTES_NEEDED", BlockCount  * GetBlockSize()),
                  Msg_Arg("BYTE_SIZE", m_limit * GetBlockSize()));
              RTE_Message (msgList);
              m_writeHeapLimitOpMsg = false;
          }
          return 0;
      }
  }
  while (true);
#endif
}

/*-----------------------------------------------------*/

SAPDB_ULong LVCMem_BlockAllocator::CalcBlockCount(SAPDB_ULong NumberOfBytes) const
{
  return m_allocator.CalcBlockCount(NumberOfBytes);
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::CalcStatistics( SAPDB_ULong &BytesUsed,
                                            SAPDB_ULong &MaxBytesUsed,
                                            SAPDB_ULong &BytesControlled)
{
  BytesUsed       = m_allocated * m_allocator.GetBlockSize();
  MaxBytesUsed    = m_maxAllocated * m_allocator.GetBlockSize();
  BytesControlled = m_controlled * m_allocator.GetBlockSize(); 
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::Deallocate(void* p, SAPDB_ULong BlockCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  if (p) free(p);
#else
  RTESync_LockedScope lockedScope(m_spinlock);
  ++m_CountDealloc;
  m_allocated -= BlockCount;
  if ((BlockCount == m_defaultBlockCount) && (m_controlled <= m_limit))
  {
    NextPointer* pBlock = REINTERPRET_CAST(NextPointer*, p);
    pBlock->m_next = m_firstFree;
    m_firstFree = pBlock;
  }
  else
  {
    m_allocator.Deallocate(p, BlockCount);
	++m_CountBaseDealloc;
    m_controlled -= BlockCount;
  }
#endif
}

/*-----------------------------------------------------*/

SAPDB_ULong LVCMem_BlockAllocator::GetAlignmentSize() const
{
  return m_allocator.GetAlignmentSize();
}

/*-----------------------------------------------------*/

SAPDB_ULong LVCMem_BlockAllocator::GetBlockSize() const
{
  return m_allocator.GetBlockSize();
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
	CountAlloc   = m_CountBaseAlloc;
	CountDealloc = m_CountBaseDealloc;
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                              SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc; 	
}

/*-----------------------------------------------------*/

const SAPDB_UTF8 * LVCMem_BlockAllocator::GetIdentifier() const 
{
	return (const SAPDB_UTF8 *)"LVCMem_BlockAllocator"; 
}

/*-----------------------------------------------------*/

LVCMem_BlockAllocator & LVCMem_BlockAllocator::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(LVCMem_BlockAllocator));
  if (0 == m_Instance)
  {
    m_Instance = new(Space) LVCMem_BlockAllocator();
  }
  return *m_Instance;
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::ReleaseFreeList()
{
  while (0 != m_firstFree)
  {
    void* pCurr = m_firstFree;
    m_firstFree = m_firstFree->m_next;
    m_allocator.Deallocate(pCurr, m_defaultBlockCount);
    m_controlled -= m_defaultBlockCount;
  }
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::SetLimit(SAPDB_ULong limit)
{
	if (limit > 0)
	{
		RTESync_LockedScope lockedScope(m_spinlock);
		m_limit = this->CalcBlockCount(limit);
		if (m_controlled > m_limit)
		{
			this->ReleaseFreeList();
		}
	}
}

/*-----------------------------------------------------*/

void LVCMem_BlockAllocator::SetDefaultBlockCount(SAPDB_ULong BlockCount)
{
  RTESync_LockedScope lockedScope(m_spinlock);
  if (m_defaultBlockCount == BlockCount)
  {
    return;
  }
  this->ReleaseFreeList();
  m_defaultBlockCount = BlockCount;
}
