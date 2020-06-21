/*!
 * \file    LVC_RWLockManager.hpp
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

#ifndef __LVC_RWLOCKMANAGER_HPP
#define __LVC_RWLOCKMANAGER_HPP

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

class RTESync_IRWRegion;
class SAPDBMem_IRawAllocator;

class LVC_RWLockManager {
public:
  LVC_RWLockManager(SAPDBMem_IRawAllocator &alloc);
  ~LVC_RWLockManager();

  /*!
   * \brief Get lock for given ID.
   *
   * \param areaId area ID
   * \param lockId lock ID
   * \param createNew if true, create new lock if doesn't exist.
   * \return lock handle or null.
   */
  RTESync_IRWRegion *getLock(SAPDB_Int4 areaId, SAPDB_Int4 lockId, bool createNew);

  /*!
   * \brief Get lock for given ID.
   *
   * \param areaId lock ID
   * \param name Identifier of the area.
   */
  void registerArea(SAPDB_Int4 areaId, const char *name);
private:

  class Entry {
  public:
    /// Area ID
    SAPDB_Int4              m_areaId;
    /// Lock ID
    SAPDB_Int4              m_lockId;
    /// Underlying lock
    RTESync_IRWRegion*      m_lock;
    /// Next entry in hash bucket
    Entry*                  m_next;
  };

  class SpinlockPoolEntry {
  public:
    SpinlockPoolEntry(SAPDB_Int4 areaId, const SAPDB_UTF8 *name);
    void* operator new(size_t ByteCount, SAPDBMem_IRawAllocator &RawAllocator);
    /// Area ID
    SAPDB_Int4              m_areaId;
    /// Spinlock pool for R/W regions.
    RTESync_SpinlockPool    m_spinlock_pool;
    /// Next spinlockpool in list
    SpinlockPoolEntry*      m_next;
  };

  /// Allocator for R/W locks.
  SAPDBMem_IRawAllocator  &m_alloc;
  /// Spinlock pool list for R/W regions.
  SpinlockPoolEntry       *m_spinlock_pool_list;
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

#endif  // __LVC_RWLOCKMANAGER_HPP
