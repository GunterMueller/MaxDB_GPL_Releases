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

#ifndef __LVCSim_RWLockManager_HPP
#define __LVCSim_RWLockManager_HPP

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

class RTESync_IRWRegion;
class SAPDBMem_IRawAllocator;

class LVCSim_RWLockManager {
public:
  LVCSim_RWLockManager(SAPDBMem_IRawAllocator &alloc, RTESync_SpinlockPool &spinlockPool);
  ~LVCSim_RWLockManager();

  /*!
   * \brief Get lock for given ID.
   *
   * \param id lock ID
   * \param createNew if true, create new lock if doesn't exist.
   * \return lock handle or null.
   */
  RTESync_IRWRegion *getLock(SAPDB_Int4 areaId, SAPDB_Int4 lockId, bool createNew);
private:

  class Entry {
  public:
    /// Area ID
    SAPDB_Int4              m_area;
    /// Lock ID
    SAPDB_Int4              m_id;
    /// Underlying lock
    RTESync_IRWRegion*      m_lock;
    /// Next entry in hash bucket
    Entry*                  m_next;
  };

  /// Allocator for R/W locks.
  SAPDBMem_IRawAllocator  &m_alloc;
  /// Spinlock pool for R/W regions.
  RTESync_SpinlockPool    &m_spinlock_pool;
  /// Reader/writer locks for R/W regions.
  Entry** volatile        m_rw_locks;
  /// Current size of the hash table
  volatile  SAPDB_UInt8   m_hash_size;
  /// Limit for size.
  SAPDB_UInt8                     m_limit;
  /// Count of items.
  SAPDB_UInt8                     m_count;
  /// Lock on entries.
  RTESync_Spinlock        m_lock;
};

#endif  // __LVCSim_RWLockManager_HPP
