/*****************************************************************************/
/*!
  @file         FileDir_BucketLocks.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_BucketLocks.

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

#ifndef FILEDIR_BUCKETLOCKS_HPP
#define FILEDIR_BUCKETLOCKS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class SAPDBMem_IRawAllocator;
class FileDir_RWRegionWrapper;

/*!
 * @class FileDir_BucketLocks
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * Provides locking for file directory hash buckets from
 * FileDir_GenericDirectory. There is no real connection between locks
 * here and the actual hash buckets in FileDir_GenericDirectory but
 * they are associated by using the same bucket number for bucket and
 * lock in FileDir_GenericDirectory. By doing so, one lock here can be
 * used both for the old and the new bucket during a resize of the
 * hash structure.
 */
class FileDir_BucketLocks
{
public:
    /// Used to address a bucket.
    typedef SAPDB_UInt BucketNo;

    /// Determines whether or not actual locking is performed.
    enum LockType
    {
        RealLock,   ///< Do locking.
        DummyLock   ///< No locking is done.
    };

    /*!
     * @brief Constructor.
     *
     * @param alloc    [in] allocator to use
     * @param lockType [in] use real or just dummy locks
     */
    FileDir_BucketLocks(
        SAPDBMem_IRawAllocator& alloc,
        const LockType          lockType);

    /*!
     * @brief Destructor.
     */
    ~FileDir_BucketLocks();

    /*!
     * @brief Initializes the bucket locks.
     *
     * @param initialSize [in] the initial number of bucket locks provided
     *
     * @return true on success, false otherwise
     */
    SAPDB_Bool Initialize(const BucketNo initialSize);

    /*!
     * @brief Resizes the list of locks. The list will never shrink
     *        and can only grow.
     *
     * @param newSize [in] the desired size of the list; the list will
     *                     never shrink
     *
     * @return
     *  - true, if list could be resized
     *  - false, if resize failed
     */
    SAPDB_Bool Resize(const BucketNo newSize);

    /*!
     * @brief Acquires an exclusive lock for bucket bucketNo.
     */
    void LockExclusive(
        const RTE_TaskId taskId,
        const BucketNo bucketNo) const;
    /*!
     * @brief Releases an exclusive lock for bucket bucketNo.
     */
    void UnlockExclusive(
        const RTE_TaskId taskId,
        const BucketNo bucketNo) const;

    /*!
     * @brief Acquires a share lock for bucket bucketNo.
     */
    void LockShared(
        const RTE_TaskId taskId,
        const BucketNo bucketNo) const;
    /*!
     * @brief Releases a share lock for bucket bucketNo.
     */
    void UnlockShared(
        const RTE_TaskId taskId,
        const BucketNo bucketNo) const;

private:
    /// Linked list to track different arrays of
    /// FileDir_RWRegionWrapper objects created during resizes.
    struct LockArrayList {
        FileDir_RWRegionWrapper* lockArray;
        BucketNo                 arraySize;
        LockArrayList*           next;

        LockArrayList(
            FileDir_RWRegionWrapper* lA,
            BucketNo                 aS,
            LockArrayList*           n) {
            lockArray = lA;
            arraySize = aS;
            next      = n;
        }
    };

    SAPDBMem_IRawAllocator& m_allocator;

    BucketNo m_size;
    LockType m_lockType;
    FileDir_RWRegionWrapper** m_lockPtrArray;
    LockArrayList* m_lockArrayList;

    BucketNo m_oldSize;
    FileDir_RWRegionWrapper** m_oldLockPtrArray;

    SAPDB_Bool AddLocks(const BucketNo lockCount);

    void Cleanup();
};

#endif // FILEDIR_BUCKETLOCKS_HPP
