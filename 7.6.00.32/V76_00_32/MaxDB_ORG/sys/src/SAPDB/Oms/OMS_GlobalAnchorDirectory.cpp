/*!
 * \file    OMS_GlobalAnchorDirectory.cpp
 * \author  ChristophR
 * \brief   LiveCache global user allocator register.
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

#include "Oms/OMS_GlobalAnchorDirectory.hpp"
#include "Oms/OMS_GlobalAnchorItem.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_GlobalUserAllocator.hpp"
#include "Oms/OMS_Handle.hpp"
#include "hsp77.h"
//#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

//#include "Oms/OMS_GlobalAnchorObject.hpp"
//#include "Oms/OMS_AllocatorInterface.hpp"

/*---------------------------------------------------------------------------*/

///// Global user allocator hash items.
//static OMS_AnchorDirectoryItemPtr  lvcGlobalAnchorItems[BUCKET_CNT] = {NULL};
//
///// Global lockId counter;
//static SAPDB_Int4 lvcLockIdCounter = 1;
//
///// Lock for global anchor directory.
//static RTESync_Spinlock                     lvcGlobalAnchorLock;
//
///// Default instance.
//OMS_GlobalAnchorDirectory          OMS_GlobalAnchorDirectory::m_instance;

/*----------------------------------------------------------------------*/

void *OMS_AnchorDirectoryItem::operator new(size_t size, SAPDBMem_IRawAllocator &alloc){
  return alloc.Allocate(size);
}
  
/*----------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_AnchorDirectoryItem::operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc)
{
  alloc.Deallocate(ptr);
}
#endif

/*----------------------------------------------------------------------*/

void OMS_AnchorDirectoryItem::deleteSelf(SAPDBMem_IRawAllocator &alloc){
  this->~OMS_AnchorDirectoryItem();
  alloc.Deallocate(this);
}

/*----------------------------------------------------------------------*/

OMS_GlobalAnchorDirectory::OMS_GlobalAnchorDirectory() :
  m_lockIdCounter(1)
{
  for(int i=0; i<BUCKET_CNT; ++i){
    m_globalAnchorItems[i] = 0;
  }
}

/*---------------------------------------------------------------------------*/

OMS_GlobalAnchorItem *OMS_GlobalAnchorDirectory::get(
  OmsHandle *pHandle, int areaId, int anchorId)
{
  size_t bucket = (SAPDB_Int4)((((SAPDB_Int8)areaId<<32) | (SAPDB_Int8)anchorId) % BUCKET_CNT);
  int lockOffset = bucket % ANCHOR_DIR_SIZE;
  
  {
    OMS_InternalLockScope scope(pHandle->m_pSession, OMS_RWLOCK_AREA, ANCHOR_DIR_START_CNT + lockOffset, false/*share*/);

    OMS_AnchorDirectoryItem *i = m_globalAnchorItems[bucket];
    while (i) {
      if ( i->m_areaId == areaId && i->m_anchorId == anchorId) {
        // return existing anchorItem with the same areaId and anchorId
        return i->m_anchorItem;
      }
      i = i->m_next;
    }
  }

  OMS_InternalLockScope scope(pHandle->m_pSession, OMS_RWLOCK_AREA, ANCHOR_DIR_START_CNT + lockOffset, true/*exclusive*/);

  OMS_AnchorDirectoryItem *i = m_globalAnchorItems[bucket];
  while (i) {
    if ( i->m_areaId == areaId && i->m_anchorId == anchorId) {
      // return existing anchorItem with the same areaId and identifier
      return i->m_anchorItem;
    }
    i = i->m_next;
  }

  // create new anchor object
  if(m_lockIdCounter == MAX_INT4_SP00){
    return NULL; //TODO (id reuse)
  }
  char name[41];
  sp77sprintf(name,40,"OMS GSM(%d,%d)",areaId,anchorId);

  // create new global shared memory allocator
  OMS_GlobalUserAllocator *a = new(OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper)
    OMS_GlobalUserAllocator(name, OMS_Globals::m_globalsInstance->m_userAllocatorWrapper, 32 * 1024, 32 * 1024);

  a->SetBadAllocHandler(SAPDBMem_RawAllocator::THROW_STD_BAD_ALLOC);

  OMS_GlobalAnchorItem *anchorItem = new(OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper) 
    OMS_GlobalAnchorItem(a,m_lockIdCounter++);

  //anchorItem->m_lock = RTESync_CreateRWRegion(lockId++, m_spinlock_pool, alloc);
  //anchorItem->m_lock->enter(false);

  m_globalAnchorItems[bucket] = new(OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper) 
    OMS_AnchorDirectoryItem(anchorItem, areaId, anchorId, m_globalAnchorItems[bucket]);

  return m_globalAnchorItems[bucket]->m_anchorItem;
}

/*---------------------------------------------------------------------------*/

