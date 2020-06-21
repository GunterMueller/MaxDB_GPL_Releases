/*!
 * \file    OMS_GlobalUserAllocatorDirectory.cpp
 * \author  Roterring
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

#include "Oms/OMS_GlobalUserAllocatorDirectory.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_GlobalUserAllocator.hpp"
#include "Oms/OMS_AllocatorInterface.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_LockRequest.hpp"
//#include "Messages/Msg_List.hpp"

/*---------------------------------------------------------------------------*/

/// Global user allocator hash items.
///static OMS_UserAllocatorDirectoryItemPtr  lvcGlobalUserAllocItems[BUCKET_CNT] = {NULL};

/// Lock for global allocator directory.
///static RTESync_Spinlock                     lvcGlobalUserAllocLock;

/// Default instance.
///OMS_GlobalUserAllocatorDirectory          OMS_GlobalUserAllocatorDirectory::m_instance;

/*---------------------------------------------------------------------------*/

void *OMS_UserAllocatorDirectoryItem::operator new(size_t size){
  return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(size);
}
  
/*---------------------------------------------------------------------------*/

void OMS_UserAllocatorDirectoryItem::deleteSelf(){
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(this);
}

/*---------------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_UserAllocatorDirectoryItem::operator delete(void *p)
{
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p);
}
#endif

/*------------------------------------------------------------------*/

OMS_GlobalUserAllocatorDirectory::OMS_GlobalUserAllocatorDirectory()
: m_isInitialized(false)
{
  for(int i=0; i<BUCKET_CNT; ++i){
    m_globalUserAllocItems[i] = 0;
  }
}



void OMS_GlobalUserAllocatorDirectory::init()
{
  for(int offset=0; offset<ALLOCATOR_DIR_SIZE; ++offset){
    LVC_LockRequest lock( LVC_LockRequest::RWLOCK_CREATE, OMS_RWLOCK_AREA, ALLOCATOR_DIR_START_CNT + offset, 0, 0);
    short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
    if(err)
      OMS_Globals::Throw(err, "OMS_GlobalUserAllocatorDirectory::OMS_GlobalUserAllocatorDirectory(), could not create rwlock", __MY_FILE__, __LINE__);
  }
  m_isInitialized = true;
}

/*---------------------------------------------------------------------------*/

size_t OMS_GlobalUserAllocatorDirectory::hash(const char *name)
{
  size_t val = 0;
  while (*name) {
    val = (val << 1) ^ (val >> 31) ^ *(name++);
  }
  return val;
}

/*---------------------------------------------------------------------------*/

OmsAllocatorInterface *OMS_GlobalUserAllocatorDirectory::get(
  //OmsHandle *pHandle, 
  const char *name)
{
  if(!m_isInitialized){
    init();
  }
  if (*name == 0) {
    name = "OMS GLOBAL USER ALLOCATOR";
  }
  size_t bucket = hash(name) % BUCKET_CNT;
  int lockOffset = bucket % ALLOCATOR_DIR_SIZE; 
  
  {
    //OMS_InternalLockScope(pHandle->m_pSession, OMS_RWLOCK_AREA, ALLOCATOR_DIR_START_CNT + lockOffset, false/*share*/);
    OMS_InternalKernelLockScope scope(OMS_RWLOCK_AREA, ALLOCATOR_DIR_START_CNT + lockOffset, false/*share*/);

    OMS_UserAllocatorDirectoryItem *i = m_globalUserAllocItems[bucket];
    while (i) {
      if(strncmp(name, &i->m_name[0], 40) == 0) {
        return i->m_alloc;
      }
      i = i->m_next;
    }
  }

  //OMS_InternalLockScope(pHandle->m_pSession, OMS_RWLOCK_AREA, ALLOCATOR_DIR_START_CNT + lockOffset, true/*exclusive*/);
  OMS_InternalKernelLockScope scope(OMS_RWLOCK_AREA, ALLOCATOR_DIR_START_CNT + lockOffset, true/*exclusive*/);

  OMS_UserAllocatorDirectoryItem *i = m_globalUserAllocItems[bucket];
  while (i) {
    if (strncmp(name, &i->m_name[0], 40) == 0) {
         return i->m_alloc;
      }
      i = i->m_next;
  }

  // create new allocator
  OMS_GlobalUserAllocator *a = new( OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper)
    OMS_GlobalUserAllocator(name, OMS_Globals::m_globalsInstance->m_userAllocatorWrapper, 
    OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize());

  a->SetBadAllocHandler(SAPDBMem_RawAllocator::NO_THROW_HANDLER);

  i = new OMS_UserAllocatorDirectoryItem(a, name, m_globalUserAllocItems[bucket]);  
  m_globalUserAllocItems[bucket] = i;
  return i->m_alloc;
}

/*---------------------------------------------------------------------------*/
