/*!
 * \file    LVCSim_RWLockManager.hpp
 * \author  IvanS
 * \brief   Reader-writer lock manager.
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

#include "LVCSimulator/LVCSim_RWLockManager.hpp"
#include "liveCache/LVC_LockRequest.hpp"

/*-----------------------------------------------------*/

LVCSim_RWLockManager::LVCSim_RWLockManager(SAPDBMem_IRawAllocator &alloc, RTESync_SpinlockPool &spinlockPool)
  : m_alloc(alloc), m_spinlock_pool(spinlockPool),
    m_rw_locks(NULL), m_hash_size(0), m_limit(0), m_count(0)
{
}

/*-----------------------------------------------------*/

LVCSim_RWLockManager::~LVCSim_RWLockManager()
{
}

/*-----------------------------------------------------*/

RTESync_IRWRegion *LVCSim_RWLockManager::getLock(SAPDB_Int4 area ,SAPDB_Int4 id, bool createNew)
{
  RTESync_LockedScope lck(m_lock);

  SAPDB_UInt8 bucket;
  Entry *start;
  // search for entry
  if (m_hash_size > 0) {
    bucket = ((SAPDB_UInt8)area*MAX_INT4_SP00 + id) % m_hash_size;
    start = m_rw_locks[bucket];
    while (start != NULL) {
      if (start->m_area == area && start->m_id == id) {
        // found it
        return start->m_lock;
      }
      start = start->m_next;
    }
  }
  if (createNew) {
    // create new entry
    if (m_hash_size == 0) {
      // create new hash
      void *space = m_alloc.Allocate(sizeof(Entry*) * 11);
      if (space == NULL) {
        // sorry, no memory
        return NULL;
      }
      memset(space, 0, sizeof(Entry*) * 11);
      m_rw_locks = (Entry**) space;
      m_hash_size = 11;
      m_count = 0;
      m_limit = 7;
      bucket = ((SAPDB_UInt8)area*MAX_INT4_SP00 + id) % m_hash_size;
    }
    void *space = m_alloc.Allocate(sizeof(Entry));
    if (space == NULL) {
      // sorry, no memory
      return NULL;
    }
    RTESync_IRWRegion *lock = RTESync_CreateRWRegion((SAPDB_Int8)id, m_spinlock_pool, m_alloc);
    if (lock == NULL) {
      // sorry, no memory
      m_alloc.Deallocate(space);
      return NULL;
    }

    // chain in the new entry
    start = new(space) Entry;
    start->m_area = area;
    start->m_id = id;
    start->m_lock = lock;
    start->m_next = m_rw_locks[bucket];
    m_rw_locks[bucket] = start;

    if (++m_count >= m_limit) {
      // try to increase hash size
      SAPDB_ULong newsize = (SAPDB_ULong)m_hash_size * 2 + 1;
      void *space = m_alloc.Allocate(sizeof(Entry*) * newsize);
      if (space != NULL) {
        // have new block
        Entry **newentry = (Entry**) space;
        memset(newentry, 0, sizeof(Entry*) * newsize);
        for (int i = 0; i < m_hash_size; ++i) {
          Entry *e = m_rw_locks[i];
          while (e != NULL) {
            Entry *n = e->m_next;
            SAPDB_UInt8 newbucket = ((SAPDB_UInt8)e->m_area*MAX_INT4_SP00 + e->m_id) % newsize;
            e->m_next = newentry[newbucket];
            newentry[newbucket] = e;
            e = n;
          }
        }
        space = m_rw_locks;
        m_rw_locks = newentry;
        m_hash_size = newsize;
        m_limit = (m_hash_size * 7) / 10;
        m_alloc.Deallocate(space);
      }
    }
    return start->m_lock;
  }
  return NULL;
}


