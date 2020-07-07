/*****************************************************************************/
/*!
 @file    FileDir_IteratorObj.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_IteratorObj.

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

#include "FileDirectory/FileDir_IteratorObj.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_BucketLocks.hpp"
#include "FileDirectory/FileDir_HashBucket.hpp"
#include "FileDirectory/FileDir_Directory.hpp"
#include "FileDirectory/FileDir_GenericDirectory.hpp"
#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Container/Container_Vector.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

// vgetpid():
#include "heo51.h"

struct tgg00_TransContext;

/**************************************************************************/

FileDir_IteratorObj::FileDir_IteratorObj(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext*     transContext)
    : m_fileNoList(alloc),
      m_refCounter(0),
      m_fileTypeFilterList(alloc),
      m_firstRun(true),
      m_transContext(transContext),
      m_IteratorObj(m_fileNoList.Begin()),
      m_hideDeletedFilesFilter(false),
      m_parentFDir(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IteratorObj::IteratorObj", FileDir_Trace, 5 );
}

/**************************************************************************/

FileDir_IteratorObj::FileDir_IteratorObj(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext&     transContext)
    : m_fileNoList(alloc),
      m_refCounter(0),
      m_fileTypeFilterList(alloc),
      m_firstRun(true),
      m_transContext(&transContext),
      m_IteratorObj(m_fileNoList.Begin()),
      m_hideDeletedFilesFilter(false),
      m_parentFDir(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IteratorObj::IteratorObj1", FileDir_Trace, 5 );
}

/**************************************************************************/

FileDir_IteratorObj::~FileDir_IteratorObj()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IteratorObj::~IteratorObj", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT( m_refCounter == 0 );

    if ( m_parentFDir != 0 ) {
        m_parentFDir->UnregisterIterator( *this );
    }
}

/**************************************************************************/

void FileDir_IteratorObj::SetTransContext(tgg00_TransContext* transContext)
{
    m_transContext = transContext;
}

/**************************************************************************/

bool FileDir_IteratorObj::Initialize(
    FileDir_GenericDirectory& fDir,
    FileDir_GenericDirectory* fDir1,
    FileDir_Directory*        parentFDir)
{
    SAPDBERR_ASSERT_ARGUMENT( m_fileNoList.GetSize() == 0 );

    if ( (m_parentFDir != 0) && (m_parentFDir != parentFDir) ) {
        m_parentFDir->UnregisterIterator( *this );
        m_parentFDir = 0;
    }

    if ( (parentFDir != 0) && (m_parentFDir == 0) ) {
        m_parentFDir = parentFDir;
        if ( m_parentFDir->RegisterIterator( *this ) == false ) {
            m_parentFDir = 0; // make sure we will not try to unregister
            return false;
        }
    }

    m_FDir = &fDir;

    SAPDB_UInt entryCount;
    entryCount = m_FDir->GetEntryCount();
    if ( fDir1 != 0 ) {
        entryCount += fDir1->GetEntryCount();
    }
    // reserve space for entries + some buffer for new entries
    bool ok = m_fileNoList.Reserve( entryCount + 50 );

    if ( entryCount == 0 ) {
        m_IteratorObj = m_fileNoList.End();
        return true;
    }

    ok &= ReadFileDir( *m_FDir );
    if ( fDir1 != 0 ) {
        ok &= ReadFileDir( *fDir1 );
    }

    if ( !ok ) {
        return false;
    }

    m_IteratorObj = m_fileNoList.Begin();
    m_firstRun    = true;

    // reset filter list, so we don't filter again later
    m_fileTypeFilterList.Clear();
    return true;
}

/**************************************************************************/

bool FileDir_IteratorObj::ReadFileDir(FileDir_GenericDirectory& fd)
{
    RTE_TaskId taskId;
    vgetpid( taskId );

    bool checkFilters = m_hideDeletedFilesFilter
        ||(m_fileTypeFilterList.GetSize() > 0);
    fd.m_hashLock->AcquireReadLock();
    for (SAPDB_UInt bucketNo=0; bucketNo<fd.m_hash->bucketCount;
         bucketNo++) {
        FileDir_Generic entry;
        if ( fd.m_hash->bucketList[bucketNo].GetSize() > 0 ) {
            // size was read dirty
            fd.m_bucketLocks.LockShared( taskId, bucketNo );
            FileDir_Node* node = 0;
            do {
                if ( fd.m_hash->bucketList[bucketNo].GetNextEntry(
                         node, entry ) ) {
                    if ( checkFilters &&
                         !CheckPassesFilter(entry.GetPointer()) ) {
                        continue;
                    }
                    if ( m_fileNoList.IsFull() ) {
                        if ( !m_fileNoList
                             .Reserve(m_fileNoList.GetCapacity()*2) ) {
                            fd.m_bucketLocks.UnlockShared( taskId, bucketNo );
                            fd.m_hashLock->ReleaseReadLock();
                            return false;
                        }
                    }
                    m_fileNoList.InsertEnd( entry.GetFileNo() );
                }
            } while ( entry.IsAssigned() );
            fd.m_bucketLocks.UnlockShared( taskId, bucketNo );
        }
    }
    fd.m_hashLock->ReleaseReadLock();
    return true;
}

/**************************************************************************/

void FileDir_IteratorObj::Clear()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IteratorObj::Clear",
                             FileDir_Trace, 5 );

    m_fileNoList.Clear();
    m_IteratorObj = m_fileNoList.End();
}

/**************************************************************************/

SAPDB_Bool FileDir_IteratorObj::GetNext(FileDir_Generic& p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IteratorObj::GetNext",
                             FileDir_Trace, 7 );

    RTE_TaskId taskId;
    if ( m_transContext != 0 ) {
        taskId = m_transContext->trTaskId_gg00;
    } else {
        vgetpid( taskId );
    }

    p.Detach();

    if ( !m_firstRun && (m_IteratorObj != m_fileNoList.End()) ) {
        ++m_IteratorObj;
    } else {
        m_firstRun = false;
    }
    while ( m_IteratorObj != m_fileNoList.End() ) {
        FileDir_Directory::IteratorGetRc retCode =
            m_FDir->IteratorGetFile(
                m_transContext,
                *m_IteratorObj,
                m_currentEntry );
        if ( m_currentEntry.IsAssigned() ) {
            if ( (m_fileTypeFilterList.GetSize() == 0) ||
               CheckPassesFilter(m_currentEntry.GetPointer()) ) {
                break;
            }
            m_currentEntry.Detach();
        } else {
            if ( retCode == FileDir_GenericDirectory::igFileDirShuttingDown ) {
                if ( m_parentFDir != 0 ) {
                    m_parentFDir->UnregisterIterator( *this );
                    m_parentFDir = 0;
                }
                m_IteratorObj = m_fileNoList.End();
                break;
            }
        }
        ++m_IteratorObj;
    }

    if ( ! m_currentEntry.IsAssigned() ) {
        return false;
    }

    // use TransferReference to save synchronisation, but most of all
    // to make sure we are not increasing the reference count on a
    // file entry that is marked for lazy delete
    FileDir_Generic::TransferReference( p, m_currentEntry );
    return true;
}

/**************************************************************************/

FileDir_BaseEntry* FileDir_IteratorObj::CurrentEntry() const
{
    return m_currentEntry.GetPointer();
}

/**************************************************************************/

SAPDB_Bool FileDir_IteratorObj::AddFileTypeToFilter(
    const Data_FileType fileType)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IteratorObj::AddFileTypeToFilter", FileDir_Trace, 5 );

    SAPDB_Bool ok = true;
    if ( m_fileTypeFilterList.IsFull() ) {
        ok = m_fileTypeFilterList.Reserve(
            m_fileTypeFilterList.GetSize() * 2 + 1);
    }
    if ( ok ) {
        m_fileTypeFilterList.InsertEnd( fileType );
    }
    return ok;
}

/*************************************************************************/

SAPDB_Bool FileDir_IteratorObj::AddHideDeletedFilesFilter()
{
    m_hideDeletedFilesFilter = true;
    return true;
}

/*************************************************************************/

bool FileDir_IteratorObj::CheckPassesFilter(FileDir_BaseEntry* fe)
{
    if ( m_hideDeletedFilesFilter
         && (fe->GetFileState() == Data_FileDeleted) ) {
        return false;
    }

    if ( m_fileTypeFilterList.GetSize() == 0 ) {
        return true;
    }

    Data_FileType fileType = fe->GetFileType();
    FileTypeFilterList::Iterator it  = m_fileTypeFilterList.Begin();
    FileTypeFilterList::Iterator end = m_fileTypeFilterList.End();
    while ( it != end ) {
        if ( fileType == (*it) ) {
            return true;
        }
        ++it;
    }
    return false;
}

/*************************************************************************/

void FileDir_IteratorObj::IncRefCounter() const
{
    RTESys_AtomicModify( m_refCounter, 1 );
}

/*************************************************************************/

void FileDir_IteratorObj::DecRefCounter() const
{
    SAPDBERR_ASSERT_ARGUMENT( m_refCounter > 0 );
    RTESys_AtomicModify( m_refCounter, -1 );
}

/*************************************************************************/

SAPDB_UInt FileDir_IteratorObj::GetRefCounter() const
{
    return m_refCounter;
}

/*************************************************************************/

void FileDir_IteratorObj::ClearFilter()
{
    m_hideDeletedFilesFilter = false;
    m_fileTypeFilterList.Clear();
}

/*************************************************************************/

FileDir_IDirectory* FileDir_IteratorObj::GetFileDirectory()
{
    if ( m_parentFDir ) {
        return m_parentFDir;
    }

    return m_FDir;
}

/*************************************************************************/

void FileDir_IteratorObj::Detach()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IteratorObj::Detach", FileDir_Trace, 5 );

    if ( m_parentFDir == 0 ) {
        return;
    }

    m_parentFDir->UnregisterIterator( *this );
    m_parentFDir = 0;

    m_IteratorObj = m_fileNoList.End();
    m_currentEntry.Detach();
}

/*************************************************************************/
