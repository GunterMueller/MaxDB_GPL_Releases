/*****************************************************************************/
/*!
 @file    FileDir_BucketLocks.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_HashBucket.

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_BucketLocks.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

FileDir_BucketLocks::FileDir_BucketLocks(
    SAPDBMem_IRawAllocator& alloc,
    const LockType lockType)
    : m_allocator(alloc),
      m_lockType(lockType),
      m_lockPtrArray(0),
      m_lockArrayList(0),
      m_size(0),
      m_oldLockPtrArray(0),
      m_oldSize(0)
{}

/*************************************************************************/

FileDir_BucketLocks::~FileDir_BucketLocks()
{
    this->Cleanup();

    if ( m_lockPtrArray == 0 ) {
        return;
    }

    LockArrayList* next = m_lockArrayList;
    while ( 0 != next ) {
        m_lockArrayList = next;
        next            = next->next;
        destroyarray(
            m_lockArrayList->lockArray,
            m_lockArrayList->arraySize,
            m_allocator );
        destroy ( m_lockArrayList, m_allocator );
    }

    if ( m_lockPtrArray != 0 ) {
        m_allocator.Deallocate( m_lockPtrArray );
    }
}

/*************************************************************************/

SAPDB_Bool FileDir_BucketLocks::Initialize(const BucketNo initialSize)
{
    if ( (initialSize == 0) || (m_lockType == DummyLock) ) {
        return true;
    }

    return AddLocks( initialSize );
}

/*************************************************************************/

SAPDB_Bool FileDir_BucketLocks::Resize(const BucketNo newSize)
{
    if ( (newSize <= m_size) || (newSize == 0) ) {
        return true;
    }

    return AddLocks( newSize - m_size );
}

/*************************************************************************/

SAPDB_Bool FileDir_BucketLocks::AddLocks(const BucketNo lockCount)
{
    if ( (lockCount == 0) || (m_lockType == DummyLock) ) {
        return true;
    }

    this->Cleanup();

    // allocate space for pointer array
    FileDir_RWRegionWrapper** newLockPtrArray;
    newLockPtrArray = reinterpret_cast<FileDir_RWRegionWrapper**>(
        m_allocator.Allocate(
            (m_size + lockCount) * sizeof(FileDir_RWRegionWrapper*)));
    if ( 0 == newLockPtrArray ) {
        return false;
    }

    // allocate additional locks
    FileDir_RWRegionWrapper* lock;

    newarray( lock, lockCount, m_allocator );
    if ( 0 == lock ) {
        m_allocator.Deallocate( newLockPtrArray );
        return false;
    }

    LockArrayList* lockArrayListEntry =
        new (m_allocator) LockArrayList(
            lock, lockCount, m_lockArrayList );

    SAPDB_Bool ok = (lockArrayListEntry != 0);
    if ( ok && (m_size > 0) ) {
        SAPDB_MemCopyNoCheck(
            newLockPtrArray,
            m_lockPtrArray,
            sizeof(FileDir_RWRegionWrapper*) * m_size );
    }

    BucketNo i;
    for ( i = m_size, lock = lockArrayListEntry->lockArray;
          (i < (m_size + lockCount)) && ok;
          ++i, ++lock ) {
        newLockPtrArray[ i ] = lock;
        ok &= newLockPtrArray[ i ]->Initialize();
    }

    if ( !ok ) {
        destroy( lockArrayListEntry, m_allocator );
        destroyarray( newLockPtrArray, lockCount, m_allocator );
        m_allocator.Deallocate( newLockPtrArray );
        return false;
    }

    m_oldSize         = m_size;
    m_oldLockPtrArray = m_lockPtrArray;

    m_lockArrayList   = lockArrayListEntry;
    m_lockPtrArray    = newLockPtrArray;
    m_size            += lockCount;
    return true;
}

/*************************************************************************/

void FileDir_BucketLocks::Cleanup()
{
    if ( 0 != m_oldLockPtrArray ) {
        destroyarray( m_oldLockPtrArray, m_oldSize, m_allocator );
        m_oldLockPtrArray = 0;
        m_oldSize         = 0;
    }
}

/*************************************************************************/

void FileDir_BucketLocks::LockExclusive(
    const RTE_TaskId taskId,
    const BucketNo bucketNo) const
{
    if ( m_lockType == DummyLock ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( bucketNo < m_size );

    m_lockPtrArray[ bucketNo ]->AcquireWriteLock( taskId );
}

/*************************************************************************/

void FileDir_BucketLocks::UnlockExclusive(
    const RTE_TaskId taskId,
    const BucketNo bucketNo) const
{
    if ( m_lockType == DummyLock ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( bucketNo < m_size );

    m_lockPtrArray[ bucketNo ]->ReleaseWriteLock( taskId );
}

/*************************************************************************/

void FileDir_BucketLocks::LockShared(
    const RTE_TaskId taskId,
    const BucketNo bucketNo) const
{
    if ( m_lockType == DummyLock ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( bucketNo < m_size );

    m_lockPtrArray[ bucketNo ]->AcquireReadLock( taskId );
}

/*************************************************************************/

void FileDir_BucketLocks::UnlockShared(
    const RTE_TaskId taskId,
    const BucketNo bucketNo) const
{
    if ( m_lockType == DummyLock ) {
        return;
    }

    SAPDBERR_ASSERT_STATE( bucketNo < m_size );

    m_lockPtrArray[ bucketNo ]->ReleaseReadLock( taskId );
}

/*************************************************************************/
