/*****************************************************************************/
/*!

  @file         FileDir_RWRegionWrapper.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_RWRegionWrapper.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

\endif
*/
/*****************************************************************************/

#ifndef FILEDIR_RWREGIONWRAPPER_HPP
#define FILEDIR_RWREGIONWRAPPER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/RunTime/RTE_Types.hpp"

class RTESync_SpinlockPool;
class RTESync_IRWRegion;

/*!
 * @class FileDir_RWRegionWrapper
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief Wrapper for RTESync_RWRegion to simplify use within FileDirectory.
 */
class FileDir_RWRegionWrapper
{
    static RTESync_SpinlockPool* m_pool;
    static SAPDB_Int8 m_lockCounter;

    RTESync_IRWRegion *m_plock;
    SAPDB_Int8 m_lockId;
    bool m_dummy;

    // no copy-construction or assignment
    FileDir_RWRegionWrapper(const FileDir_RWRegionWrapper&);
    FileDir_RWRegionWrapper& operator=(const FileDir_RWRegionWrapper&);
public:
    /// Initializes spinlock pool for all RW locks
    static bool InitSpinlockPool();

    /// Constructor.
    FileDir_RWRegionWrapper();

    /// Constructs a dummy lock that does not do anything.
    FileDir_RWRegionWrapper(SAPDB_Bool dummyOnly);
    /// Destructor.
    ~FileDir_RWRegionWrapper();

    /// Returns true if lock was initialized successfully.
    SAPDB_Bool Initialize();

    /// Returns true if lock is a dummy and does not do any locking.
    SAPDB_Bool IsDummy() const;

    /// Acquires read lock.
    void AcquireReadLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Tries to acquire a read lock. Returns true on success, or
    /// false if no read lock could be acquired. Does not block.
    SAPDB_Bool TryAcquireReadLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Releases read lock.
    void ReleaseReadLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Acquires exclusive lock.
    void AcquireWriteLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Tries to acquire a write lock. Returns true on success, or
    /// false if no write lock could be acquired. Does not block.
    SAPDB_Bool TryAcquireWriteLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Releases exclusive lock.
    void ReleaseWriteLock(const RTE_TaskId pid = RTE_UnknownTaskId);

    /// Acquire lock.
    void AcquireLock(const RTE_TaskId pid, SAPDB_Bool exclusive);

    /// Release lock.
    void ReleaseLock(const RTE_TaskId pid, SAPDB_Bool exclusive);
};

/*************************************************************************/

/*!
 * @class FileDir_RWAutoRegion
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Class for lock objects that requests a lock on construction
 *        and releases it on destruction.
 */
class FileDir_RWAutoRegion
{
    FileDir_RWRegionWrapper& m_region;
    SAPDB_Bool m_writeLock;
    RTE_TaskId m_taskId;

public:
    /*!
     * @brief Constructor. Requests a lock.
     *
     * @param rwRegion  lock that is to be requested
     * @param writeLock request a write lock if true, request a read lock
     *                   if false
     * @param taskId    task id of running task
     */
    FileDir_RWAutoRegion(
        FileDir_RWRegionWrapper& rwRegion,
        SAPDB_Bool               writeLock,
        RTE_TaskId               taskId = RTE_UnknownTaskId);

    /*!
     * @brief Destructor. Releases lock.
     */
    ~FileDir_RWAutoRegion();
};

/*************************************************************************/

inline FileDir_RWAutoRegion::FileDir_RWAutoRegion(
    FileDir_RWRegionWrapper& rwRegion,
    SAPDB_Bool writeLock,
    RTE_TaskId taskId)
    : m_region(rwRegion),
      m_writeLock(writeLock),
      m_taskId(taskId)
{
    if ( m_writeLock ) {
        m_region.AcquireWriteLock( m_taskId );
    } else {
        m_region.AcquireReadLock( m_taskId );
    }
}

/*************************************************************************/

inline FileDir_RWAutoRegion::~FileDir_RWAutoRegion()
{
    if ( m_writeLock ) {
        m_region.ReleaseWriteLock( m_taskId );
    } else {
        m_region.ReleaseReadLock( m_taskId );
    }
}

/*************************************************************************/

#endif  // FILEDIR_RWREGIONWRAPPER_HPP
