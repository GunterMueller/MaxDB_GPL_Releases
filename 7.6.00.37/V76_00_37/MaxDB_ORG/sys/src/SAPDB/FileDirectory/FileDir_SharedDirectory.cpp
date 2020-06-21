/*****************************************************************************/
/*!
 @file    FileDir_SharedDirectory.cpp
 @author  MartinKi
 @ingroup FileDir_SharedDirectory_Files

 @brief Implementation of class FileDir_SharedDirectory.

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

#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_ShortColumnEntry.hpp"
#include "FileDirectory/FileDir_SingleIndexEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyPartitionEntry.hpp"
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_OmsFixedEntry.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_IteratorObj.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_HashBucket.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_IPageNoManager.hpp"

#include "Transaction/Trans_Context.hpp"

#include "KernelCommon/Kernel_IndexDescription.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

// vgetpid():
#include "heo51.h"

#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hkb57.h"
#include "hbd17.h"
#include "hgg11.h" // g11kernel_version
#include "ggg00.h"
#include "gbd00.h" // tbd_fileinfo
#include "hbd17.h" // b17add_fdir

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

/// Number of fileNos that are reserved for special shared temp files
/// (e.g. monitoring tables)
const int c_sharedFileNoRange = 100;

/// Initial/Minimum capacity of the shared file directory.
const SAPDB_Int4 c_sharedInitialCapacity = 557;

FileDir_SharedDirectory* FileDir_SharedDirectory::m_fdir = 0;

/**************************************************************************/

void FileDir_SharedDirectory::CreateInstance()
{
    if ( NULL == m_fdir ) {

        m_fdir = new( RTEMem_Allocator::Instance() )
                     FileDir_SharedDirectory( RTEMem_Allocator::Instance() );

//         // test memory constraints for file dir
//         SAPDBMem_IRawAllocator* alloc = new (RTEMem_Allocator::Instance())
//             SAPDBMem_RawAllocator(
//                 UTF8("FileDir"),
//                 RTEMem_Allocator::Instance(),
//                 32768,
//                 8192,
//                 SAPDBMem_RawAllocator::NOT_FREE_RAW_EXTENDS,
//                 65536*15);
//         m_fdir = new( RTEMem_Allocator::Instance() )
//                      FileDir_SharedDirectory( *alloc );


        if( NULL == m_fdir ) {
             FileDir_Common::OutOfMemoryCrash(
                 sizeof( FileDir_SharedDirectory ));
        }

// FIXME        m_fdir->Initialize();
    }
}

/**************************************************************************/

FileDir_SharedDirectory::FileDir_SharedDirectory(SAPDBMem_IRawAllocator& alloc)
    : m_allocator(alloc),
      FileDir_GenericDirectory(alloc, FileDir_RegularLocking),
      m_initialized(false),
      m_crashAfterFlush(false),
      m_pageManager(FileDir_PageManager::Instance()),
      m_needUpdateList(alloc),
      m_statisticsDeltaCount(0),
      m_badIndexCount(0),
      m_migrationRunning(false),
      m_savepointActive(false),
      m_singleIndexList(alloc),
      m_sysKeyCache(alloc)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::FileDir", FileDir_Trace, 5 );
    m_converterVersion.Invalidate();
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::InitializeSharedFDir(
    SAPDB_Bool isCreateFDir)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::InitializeSharedFDir",
                             FileDir_Trace, 5 );

    if ( m_initialized ) {
        return true;
    }

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::InitializeSharedFDir";
    }

    // reset highest given file id:
    m_maxFileNo                  = FileDir_FileNo::GetFirstPersistentFileNo();
    m_maxTempSharedFileNo        = FileDir_FileNo::GetFirstTempFileNo();
    m_maxTempSharedFileNoAllowed = FileDir_FileNo::AddOp( m_maxTempSharedFileNo, c_sharedFileNoRange );
    m_maxTempFileNo              = FileDir_FileNo::AddOp( m_maxTempSharedFileNoAllowed, 1 );
    m_statisticsDeltaCount = 0;
    m_savepointActive            = false;
    m_badIndexCount              = 0;
    m_delOpCounter               = 0;

    m_sysKeyCache.Initialize();

    SAPDB_Int4 initialCapacity = c_sharedInitialCapacity;
    if ( ! isCreateFDir ) {
        // start with hash list size that is 30% bigger than our last
        // known entry count
        if ( initialCapacity < kb57GetFdirEntryCount() ) {
            initialCapacity = static_cast<SAPDB_Int4>(
                kb57GetFdirEntryCount() * 1.3 );
        }
        if ( (initialCapacity % 2) == 0 ) {
            // don't use even numbers for hash size
            ++initialCapacity;
        }
    }

    m_initialized = FileDir_GenericDirectory::Initialize( initialCapacity );

    if ( !m_initialized ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_FILEDIR_INIT_FAILED );
        RTE_Crash(errMsg);
    }

    if ( m_pageManager == 0 ) {
        m_pageManager = new(m_allocator) FileDir_PageManager(m_allocator);
        if ( m_pageManager == 0 ) {
            FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_PageManager) );
        }
    }

    m_lock           = new(m_allocator) FileDir_RWRegionWrapper();
    m_updateListLock = new(m_allocator) FileDir_RWRegionWrapper();
    if ( (m_lock == 0) || (m_updateListLock == 0) ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_RWRegionWrapper) );
    }

    return m_initialized;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::ShutdownSharedFDir(
    RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::ShutdownSharedFDir",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::ShutdownSharedFDir";
    }

    FileDir_GenericDirectory::Shutdown( taskId );

    destroy( m_lock, m_allocator );
    destroy( m_updateListLock, m_allocator );

    m_initialized   = false;

    return m_pageManager->Shutdown( taskId );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::Create(RTE_TaskId  taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::Create", FileDir_Trace, 5 );

    const SAPDB_Bool c_isCreateFDir = true;

    if ( !InitializeSharedFDir( c_isCreateFDir ) ) {
        return FileDir_InitFailed;
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    m_converterVersion = Converter_ICommon::Instance().Version();

    Data_PageNo rootPageNo;
    const RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );

    Converter_IPageNoManager::Instance().RequestNewPermPageNo(
        task, false, rootPageNo );

    return m_pageManager->CreateRootPage(taskId, rootPageNo) == true ?
        FileDir_Okay : FileDir_InitFailed;
}

/**************************************************************************/

Data_PageNo FileDir_SharedDirectory::GetRootPageNo(RTE_TaskId taskId)
{
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    return m_pageManager->GetRootPageNo();
}

/**************************************************************************/

FileDir_FileNo FileDir_SharedDirectory::GetNewFileNo()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::GetNewFileNo",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "increment of file no during restart not allowed") );
    }

    FileDir_FileNo curFileNo;

    RTE_TaskId taskId;
    vgetpid( taskId );
    tgg00_BasisError b_err;
    k57stamp( taskId, mm_table,
              &curFileNo,
              sizeof(m_maxFileNo),
              b_err );

    // assignment of m_maxFileNo is not protected, so it might be of
    m_maxFileNo = curFileNo;

    if ( b_err != e_ok ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "Error during increment of file no") );
    }

    if ( curFileNo.IsTempFileNo() ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_FILEID_SPACE_EXHAUSTED,
            SAPDB_ToString(0) );
        RTE_Crash(errMsg);
    }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "fileNo : " << curFileNo);

    return curFileNo;
}

/**************************************************************************/

FileDir_FileNo FileDir_SharedDirectory::MigrationGetNewFileNo()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::MigrationGetNewFileNo",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    // we know we are in restart and we don't have a problem with
    // colliding file nos, so hand out a regular file id

    if ( FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::MigrationGetNewFileNo";
    }

    FileDir_FileNo curFileNo;
    RTE_TaskId taskId;
    vgetpid( taskId );
    m_updateListLock->AcquireWriteLock( taskId );
    // do not use k57stamp as we normally do, as the surrogate counter
    // will be changed during log recovery and therefore cannot be
    // trusted:
    m_maxFileNo.Increment();
    curFileNo = m_maxFileNo;
    m_updateListLock->ReleaseWriteLock();

    if ( curFileNo.IsTempFileNo() ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_FILEID_SPACE_EXHAUSTED,
            SAPDB_ToString(0) );
        RTE_Crash(errMsg);
    }

    return curFileNo;
}

/**************************************************************************/

FileDir_FileNo FileDir_SharedDirectory::GetNewTempFileNo()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::GetNewTempFileNo",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_FileNo curFileNo;
    m_updateListLock->AcquireWriteLock();
    curFileNo = m_maxTempFileNo.Increment();
    m_updateListLock->ReleaseWriteLock();

    return curFileNo;
}

/**************************************************************************/

FileDir_FileNo FileDir_SharedDirectory::GetNewTempSharedFileNo()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::GetNewTempSharedFileNo",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_FileNo curFileNo;
    RTE_TaskId taskId;
    vgetpid( taskId );

    // try to make sure to return an unused file no; this is not
    // really safe, e.g. someone could have requested a temp shared
    // file no but not yet used it to add an entry
    m_updateListLock->AcquireWriteLock();

    FileDir_Generic dummyEntry;
    FileDir_FileNo startFileNo( m_maxTempSharedFileNo );
    do {
        curFileNo = m_maxTempSharedFileNo.Increment();

        if ( m_maxTempSharedFileNoAllowed.IsLessOrEqual( curFileNo ) ) {
            // wrap counter (in case of many successive load_systabs
            m_maxTempSharedFileNo = FileDir_FileNo::GetFirstTempFileNo();
        }

    } while ( this->GetFileEntry(taskId, curFileNo, dummyEntry)
              && ( curFileNo != startFileNo ) );

    m_updateListLock->ReleaseWriteLock();

    if ( this->GetFileEntry(taskId, curFileNo, dummyEntry) ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "Temporary shared file id must not be in use") );
    }

    return curFileNo;
}

/**************************************************************************/

void FileDir_SharedDirectory::Restart_SetFileNoHighWaterMark(
    FileDir_FileNo maxFileNo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::Restart_SetFileNoHighWaterMark",
        FileDir_Trace, 5 );

    if ( (! Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing()) &&
         (! Kernel_IAdminInfo::Instance().KernelStateIsAdmin()) ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_SET_FILEID_ONLY_DURING_RESTART );
        RTE_Crash(errMsg);
    }

    if ( m_maxFileNo.IsLessThan( maxFileNo ) ) {
        m_lock->AcquireWriteLock();
        if ( m_maxFileNo.IsLessThan( maxFileNo ) ) {
            m_maxFileNo = maxFileNo;
        }
        m_lock->ReleaseWriteLock();
    }
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::CheckDataWithUpdate(
    RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::CheckDataWithUpdate",
                             FileDir_Trace, 5 );

    const bool c_checkWithUpdate = true;

    return m_pageManager->CheckData( taskId, c_checkWithUpdate );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::CheckData(
    RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::CheckData",
                             FileDir_Trace, 5 );

    const bool c_checkWithUpdate = true;

    return m_pageManager->CheckData( taskId, !c_checkWithUpdate );
}

/**************************************************************************/

void FileDir_SharedDirectory::BeginSavepoint(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::BeginSavepoint",
                             FileDir_Trace, 5 );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::BeginSavepoint";
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    m_updateListLock->AcquireWriteLock( taskId );
    m_savepointActive = true;
    m_updateListLock->ReleaseWriteLock( taskId );

    // store entry count for restart
    kb57SetFdirEntryCount( this->GetEntryCount() );

    m_pageManager->BeginSavepoint(taskId);
}

/**************************************************************************/

void FileDir_SharedDirectory::Flush(
    RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::Flush", FileDir_Trace, 5 );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::Flush";
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    m_pageManager->FlushDirtyPages(taskId);

    k57SetLastFileId( *(reinterpret_cast<tgg00_Surrogate*>(&m_maxFileNo)) );
}

/**************************************************************************/

void FileDir_SharedDirectory::EndSavepoint(
    RTE_TaskId taskId,
    const Converter_Version& newConverterVersion )
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::EndSavepoint",
                             FileDir_Trace, 5 );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::EndSavepoint";
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    m_pageManager->EndSavepoint(taskId);

    // set new converter version before updating entries
    RTESys_AtomicWrite(
        *reinterpret_cast<SAPDB_UInt4*>(&m_converterVersion),
        newConverterVersion );

    // write all deltas that accumulated during last savepoint to
    // pages and set m_savepointActive to false while holding the lock
    this->UpdatePersistentEntryParts( taskId );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::PersistentEntryPartsUpdated";
    }

    if ( m_crashAfterFlush ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_TEST_FAILED,
            SAPDB_ToString(0) );
        RTE_Crash(errMsg);
    }

// FIXME: defragmentation can only be handled safely in garbage
// collector; everyone else might collide with a delete operation

// BTW: what about changes to statistics counters during move? will
// this ever be safe? (if we really use the lock list, then there
// should be no changes to statistics counters...)

//    this->DefragPages();
}

/**************************************************************************/

void FileDir_SharedDirectory::UpdatePersistentEntryParts(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::UpdatePersistentEntryParts",
        FileDir_Trace, 5 );

    m_updateListLock->AcquireWriteLock( taskId );

    /*
     * holding m_updateListLock also guarantees that no new savepoint
     * can be started while we are at work here; this allows the
     * UpdateAfterSavepoint methods to write directly to file
     * directory pages
     */
    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::UpdatePersistentEntryParts"
            << " entry count " << m_needUpdateList.GetSize();
    }

    m_savepointActive = false;

    UpdateInfoList::Iterator it     = m_needUpdateList.Begin();
    UpdateInfoList::Iterator it_end = m_needUpdateList.End();
    for ( ; it != it_end; ++it ) {

        if ( ! (*it).fileEntry.IsAssigned() ) {
            // entry has been removed
            continue;
        }

        switch ( (*it).action ) {
            case euaUpdateEntry:
                if ( FileDir_Trace.TracesLevel( 6 ) ) {
                    Kernel_VTrace()
                        << "  updateEntry: "
                        << (*it).fileEntry.GetFileNo();
                }
                if ( ! (*it).fileEntry.GetPointer()->IsLazyDeleteSet() ) {
                    (*it).fileEntry.UpdateAfterSavepoint(
                        taskId, Converter_ICommon::Instance().Version() );
                }

                ++m_statisticsDeltaCount;
                break;
            case euaRemoveEntry:
                if ( FileDir_Trace.TracesLevel( 6 ) ) {
                Kernel_VTrace()
                    << "  removeEntry: "
                    << (*it).fileEntry.GetFileNo();
                }

                (*it).fileEntry.GetPointer()->SetLazyDelete( taskId );
            break;
        }
    }
    m_needUpdateList.Clear();

    m_updateListLock->ReleaseWriteLock( taskId );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::AddFileEntry(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    FileDir_BaseEntry*       entry,
    Msg_List&                msgList,
    const LockingBehaviour   lockType)
{
    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
         fileNo.IsPersistentFileNo() ) {
        // refresh surrogate counter if necessary
        FileDir_FileNo nextFileNo( fileNo );
        nextFileNo.Increment();
        tgg00_Surrogate surrogate;
        SAPDB_MemCopyNoCheck( &surrogate, &nextFileNo, sizeof(nextFileNo) );
        kb57RefreshSurrogateIfNecessary( taskId, surrogate );
        if ( m_maxFileNo.IsLessThan( fileNo ) ) {
            if ( FileDir_Trace.TracesLevel( 5 ) ) {
                Kernel_VTrace()
                    << "FileDir_SharedDirectory::AddFileEntry - refreshing counter";
            }

            m_maxFileNo = fileNo;
        }
    }
    return FileDir_GenericDirectory::AddFileEntry(
        taskId, fileNo, entry, msgList, lockType);
}

/**************************************************************************/

/*!
 * @warning Does not use any locking, assumes there are no conflicting
 *          threads.
 */
void FileDir_SharedDirectory::RecreateFileEntry(
    const RTE_TaskId     taskId,
    FileDir_Entry&       pageEntry,
    SAPDB_Bool           bMigrate,
    FileDir_EntryLayouts migrationStartLayout)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::RecreateFileEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    SAPDBERR_ASSERT_STATE( pageEntry.GetFileNo().IsPersistentFileNo() )

    if ( m_maxFileNo.IsLessThan( pageEntry.GetFileNo() ) ) {
        m_maxFileNo = pageEntry.GetFileNo();
    }

    // update surrogate counter
    tgg00_Surrogate surrogate;
    FileDir_FileNo  entryFileNo = pageEntry.GetFileNo();
    SAPDB_MemCopyNoCheck( &surrogate, &entryFileNo, sizeof(tgg00_Surrogate) );
    kb57RefreshSurrogateIfNecessary( taskId, surrogate );

    if ( FileDir_Trace.TracesLevel( 7 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::RecreateFileEntry" << NewLine
            << "  fileNo:   " << FileDir_FileNoToString( entryFileNo ) << NewLine
            << "  fileType: " << pageEntry.GetFileType( migrationStartLayout );
    }

    Msg_List msgList;
    FileDir_Entry* newEntry = 0;
    SAPDB_Byte*    migPersistentEntryPos;
    switch(pageEntry.GetFileType( migrationStartLayout )) {
    case Data_TableFile:
    {
        FileDir_TableEntry* tableEntry;
        if ( bMigrate ) {
            tableEntry =
                new(m_allocator) FileDir_TableEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos);
        } else {
            tableEntry =
                new(m_allocator) FileDir_TableEntry(m_allocator, pageEntry);
                }
        if ( (tableEntry == 0) ||
             (tableEntry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_TableEntry) );
        }
        newEntry = tableEntry;
    }
    break;
    case Data_IndexFile:
    {
        FileDir_IndexEntry* indexEntry;
        if ( bMigrate ) {
            indexEntry =
                new(m_allocator) FileDir_IndexEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            indexEntry =
                new(m_allocator) FileDir_IndexEntry( m_allocator, pageEntry );
        }
        if ( (indexEntry == 0) ||
             (indexEntry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_IndexEntry) );
        }
        if ( ( indexEntry->GetFileState() == Data_FileOk )
             || ( indexEntry->GetFileState() == Data_FileNotAccessible ) ) {
            FileDir_Generic parentEntry;
            if ( (! GetFileEntry(
                     taskId, indexEntry->GetParentFileNo(),
                     parentEntry) )
                || (parentEntry.GetFileType() != Data_TableFile) ) {
#ifdef SAPDB_SLOW
                FileDir_Exception errMsg(
                    __CONTEXT__, FILEDIR_RECREATE_INDEX_FAILED,
                    FileDir_FileNoToString( indexEntry->GetParentFileNo() ),
                    FileDir_FileNoToString( indexEntry->GetFileNo() ) );
                RTE_Crash(errMsg);
#else
                Kernel_VTrace()
                    << "FileDir_SharedDirectory::RecreateFileEntry: missing table "
                    << indexEntry->GetParentFileNo() << " for index "
                    << indexEntry->GetFileNo();
                return;
#endif
            }
            FileDir_Table tableEntry;
            tableEntry.Assign( parentEntry );
            if ( tableEntry.AddIndex(
                     indexEntry->GetFileNo(),
                     indexEntry->GetIndexId()) != FileDir_Okay ) {
                FileDir_Exception errMsg(
                    __CONTEXT__, FILEDIR_ADD_INDEX_FAILED,
                    FileDir_FileNoToString( indexEntry->GetFileNo() ),
                    FileDir_FileNoToString( indexEntry->GetParentFileNo() ) );
                RTE_Crash(errMsg);
            }
        }
        if ( indexEntry->GetFileState() == Data_FileNotAccessible ) {
            ++m_badIndexCount;
        }
        newEntry = indexEntry;
    }
    break;
    case Data_SingleIndexFile:
    {
        FileDir_SingleIndexEntry* indexEntry =
            new(m_allocator) FileDir_SingleIndexEntry(m_allocator, pageEntry);
        if ( (indexEntry == 0) ||
             (indexEntry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_IndexEntry) );
        }
        if ( ! m_singleIndexList.InsertEnd( indexEntry ) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_IndexEntry) );
        }
        // do not add entry to file hash
        return;
    }
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumnEntry* shortColEntry;
        if ( bMigrate ) {
            shortColEntry =
                new(m_allocator) FileDir_ShortColumnEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            shortColEntry =
                new(m_allocator) FileDir_ShortColumnEntry(
                    m_allocator, pageEntry );
        }

        if ( (shortColEntry == 0) ||
             (shortColEntry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_ShortColumnEntry) );
        }
        if (shortColEntry->GetFileState() == Data_FileOk) {
            FileDir_Generic parentEntry;
            if ( ( ! GetFileEntry(
                       taskId, shortColEntry->GetParentFileNo(), parentEntry))
                 || (parentEntry.GetFileType() != Data_TableFile) ) {
#ifdef SAPDB_SLOW
                FileDir_Exception errMsg(
                    __CONTEXT__, FILEDIR_RECREATE_SHORTCOLUMN_FAILED,
                    FileDir_FileNoToString( shortColEntry->GetParentFileNo() ),
                    FileDir_FileNoToString( shortColEntry->GetFileNo() ) );
                RTE_Crash(errMsg);
#else
                Kernel_VTrace()
                    << "FileDir_SharedDirectory::RecreateFileEntry: missing table "
                    << shortColEntry->GetParentFileNo() << " for short column file "
                    << shortColEntry->GetFileNo();
                return;
#endif
            }
            FileDir_Table tableEntry;
            tableEntry.Assign( parentEntry );
            tableEntry.SetShortColumnFileNo( shortColEntry->GetFileNo() );
        }
        newEntry = shortColEntry;
    }
    break;
    case Data_OmsFile:
    {
        FileDir_OmsEntry* entry =
            new(m_allocator) FileDir_OmsEntry(m_allocator, pageEntry);
        if ( (entry == 0) ||
             (entry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_OmsEntry) );
        }
        newEntry = entry;
    }
    break;
    case Data_OmsVarFile:
    {
        FileDir_OmsVarEntry* entry;
        if ( bMigrate ) {
            entry =
                new(m_allocator) FileDir_OmsVarEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            entry =
                new(m_allocator) FileDir_OmsVarEntry(
                    m_allocator, pageEntry );
        }
        if ( (entry == 0) ||
             (entry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_OmsVarEntry) );
        }
        newEntry = entry;
    }
    break;
    case Data_OmsKeyFile:
    {
        FileDir_OmsKeyEntry* entry;
        if ( bMigrate ) {
            entry =
                new(m_allocator) FileDir_OmsKeyEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            entry =
                new(m_allocator) FileDir_OmsKeyEntry(
                    m_allocator, pageEntry );
        }
        if ( (entry == 0) ||
             (entry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_OmsKeyEntry) );
        }
        newEntry = entry;
    }
    break;
    case Data_OmsKeyPartitionFile:
    {
        FileDir_OmsKeyPartitionEntry* keyPartitionEntry;
        if ( bMigrate ) {
            keyPartitionEntry =
                new(m_allocator) FileDir_OmsKeyPartitionEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            keyPartitionEntry =
                new(m_allocator) FileDir_OmsKeyPartitionEntry(
                    m_allocator, pageEntry );
        }
        if ( (keyPartitionEntry == 0) ||
             (keyPartitionEntry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_OmsKeyPartitionEntry) );
        }
        FileDir_Generic parentEntry;
        if ( (! GetFileEntry(
                  taskId, keyPartitionEntry->GetParentFileNo(), parentEntry ) )
             || (parentEntry.GetFileType() != Data_OmsKeyFile) ) {
#ifdef SAPDB_SLOW
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_RECREATE_KEYPARTITION_FAILED,
                FileDir_FileNoToString( keyPartitionEntry->GetParentFileNo() ),
                FileDir_FileNoToString( keyPartitionEntry->GetFileNo() ) );
            RTE_Crash(errMsg);
#else
            return;
#endif
        }

        FileDir_OmsKey omsKeyEntry;
        omsKeyEntry.Assign( parentEntry );
        if ( omsKeyEntry.AddKeyPartition(
                 keyPartitionEntry->GetFileNo(),
                 keyPartitionEntry->GetPartitionNo()) != FileDir_Okay ) {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_ADD_KEYPARTITION_FAILED,
                FileDir_FileNoToString( keyPartitionEntry->GetFileNo() ),
                FileDir_FileNoToString( keyPartitionEntry->GetParentFileNo() ) );
            RTE_Crash(errMsg);
        }
        newEntry = keyPartitionEntry;
    }
    break;
    case Data_OmsFixedFile:
    {
        FileDir_OmsFixedEntry* entry;
        if ( bMigrate ) {
            entry =
                new(m_allocator) FileDir_OmsFixedEntry(
                    taskId, m_allocator,
                    pageEntry,
                    migrationStartLayout,
                    migPersistentEntryPos );
        } else {
            entry =
                new(m_allocator) FileDir_OmsFixedEntry(
                    m_allocator, pageEntry );
        }
        if ( (entry == 0) ||
             (entry->Initialize() != FileDir_Okay) ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_OmsFixedEntry) );
        }
        newEntry = entry;
    }
    break;
    default:
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_RECREATE_INVALID_FILETYPE,
            Msg_Arg( SDBMSGTAG_FILEDIR_RECREATE_INVALID_FILETYPE__FILETYPE,
                     pageEntry.GetFileType() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_RECREATE_INVALID_FILETYPE__FILENO,
                     FileDir_FileNoToString( entryFileNo ) ),
            Msg_Arg( SDBMSGTAG_FILEDIR_RECREATE_INVALID_FILETYPE__PAGENO,
                     pageEntry.GetFileDirPageNo() ) );
        RTE_Crash( errMsg );
    }

    if ( newEntry == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_RECREATE_ENTRY_FAILED,
            FileDir_FileNoToString( entryFileNo ) );
        RTE_Crash(errMsg);
    }

    FileDir_ReturnCode ret =
        AddFileEntry( taskId, newEntry->GetFileNo(), newEntry, msgList,
                      FileDir_NoLocking );

    if ( ret != FileDir_Okay ) {
        if ( ret == FileDir_OutOfMemory ) {
            FileDir_Common::OutOfMemoryCrash( 0 );
        }
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_RECREATE_ADD_ENTRY_FAILED,
            FileDir_FileNoToString( newEntry->GetFileNo() ),
            SAPDB_ToString(ret) );
        RTE_Crash(errMsg);
    }
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::RestartFDir(
    tgg00_TransContext&      trans,
    const Data_PageNo&       rootPageNo,
    const Converter_Version& converterVersion,
    const Converter_Version& restartConverterVersion,
    const SAPDB_Bool         cleanupFileDirPages,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::RestartFDir",
                             FileDir_Trace, 5 );
    Msg_List localMsgList(
        Msg_List::Info,
        SDBMSG_FILEDIR_RESTART );
    RTE_Message( localMsgList );
    localMsgList.ClearMessageList();

    const RTE_TaskId taskId = trans.trTaskId_gg00;

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::RestartFDir";
    }

    if ( m_initialized && m_maxFileNo.IsValid() ) {
        // file directory has already been restarted
        return FileDir_AlreadyRestarted;
    }

    if ( !InitializeSharedFDir() ) {
        return FileDir_InitFailed;
    }

    // this must always be the current layout version:
    FileDir_EntryLayouts migrateStartLayout = fdelWithFileAttributes;
    SAPDB_Bool           bMigrate = false;

    tsp00_Version kernelVersion;
    kb57GetRestartRecordKernelVersion( kernelVersion );
    if ( ! Kernel_IAdminConfig::Instance().MigrationHandler()
         .CheckCompatibility(
            Kernel_Migration::filedir,
            Kernel_Version(kernelVersion), msgList) ) {
        return FileDir_InitFailed;
    }

    const SAPDB_Char* changeTitle = "NEW FILE DIRECTORY LAYOUT";
    Kernel_Migration::ChangeInfo& change =
        Kernel_IAdminConfig::Instance().MigrationHandler()
        .SearchForChangeInfo (changeTitle);

    if ( change.IsEnabled() ) {
        localMsgList = Msg_List(
            Msg_List::Info,
            SDBMSG_FILEDIR_MIGRATION_STARTED );
        RTE_Message( localMsgList );
        localMsgList.ClearMessageList();
        // migration from old style file directory necessary
        // TODO: create should be passed msg_list
        if ( this->Create( taskId ) != FileDir_Okay ) {
            msgList.AppendNewMessage(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_FILEDIR_CREATE_FAILED) );
            return FileDir_InitFailed;
        }

        bd17MigrateFdir( trans, rootPageNo );
        Kernel_IAdminConfig::Instance().MigrationHandler().CollectMigrationInfo (Kernel_Migration::filedir, changeTitle);
        if ( trans.trError_gg00 != e_ok ) {
            msgList.AppendNewMessage(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_FILEDIR_MIGRATION_FAILED,
                    Msg_Arg("NO", SAPDB_ToString( trans.trError_gg00 )) ));
            return FileDir_InitFailed;
        }
        // if we reach here the file directory has already been
        // initialized during the migration
        kb57SetFdirRoot( m_pageManager->GetRootPageNo() );
        localMsgList.AppendNewMessage(
            Msg_List(
                Msg_List::Info,
                SDBMSG_FILEDIR_MIGRATION_OK) );
        RTE_Message( localMsgList );
        localMsgList.ClearMessageList();
        return FileDir_Okay;
    } else {
        // add any migrations that are necessary for the new file
        // directory layout here; start with the latest migration
        // migrations

        const SAPDB_Char* fileAttributesChange =
            "FILE ATTRIBUTES IN FILE DIRECTORY";
        const SAPDB_Char* lobInfoChange =
            "LOB SIZE INFORMATION IN FILE DIRECTORY";

        Kernel_Migration::ChangeInfo& lobInfo =
            Kernel_IAdminConfig::Instance().MigrationHandler()
            .SearchForChangeInfo (lobInfoChange);

        Kernel_Migration::ChangeInfo& fileAttributes =
            Kernel_IAdminConfig::Instance().MigrationHandler()
            .SearchForChangeInfo (fileAttributesChange);

        // the first migration that determines the layout we are coming from
        if ( lobInfo.IsEnabled() ) {
            bMigrate = true;
            migrateStartLayout = fdelInitial;
        } else if ( fileAttributes.IsEnabled() ) {
            bMigrate = true;
            migrateStartLayout = fdelWithLobSize;
        }
    }

    FileDir_FileNo lastFileNo;
    k57GetLastFileId( *(reinterpret_cast<tgg00_Surrogate*>(&lastFileNo)) );
    this->Restart_SetFileNoHighWaterMark( lastFileNo );

    m_converterVersion = converterVersion;

    // load file directory pages:
    m_pageManager->Restart_LoadPages(taskId, rootPageNo);

    // rebuild file directory
    // ======================
    FileDir_PageManager::EntryIterator it    = m_pageManager->Begin();
    FileDir_PageManager::EntryIterator itEnd = m_pageManager->End();
    FileDir_Entry currentEntry( m_allocator );
    // first run: determine valid entries for this converter version
    while ( it != itEnd ) {
        FileDir_PageManager::EntryAddress* ea = *it;
        ++it; // this is faster than using *it++ in the line above
              // because of the way operator++ is implemented in
              // FileDir_PageManager
        currentEntry.SetPersistentDataAddress(ea->dataPage,
                                              ea->entryAddress);
        if ( currentEntry.IsValid() ) {
            if ( currentEntry.GetFileNo().IsTempFileNo() ||
                 currentEntry.IsLazyDeleteSet() ) {
                // temporary entries in shared directory will not get recreated
                currentEntry.SetInvalid();
            } else {
                DetermineValidEntryVersion(
                    currentEntry, restartConverterVersion );
                if ( currentEntry.IsValid() ) {
                    // remove any back references entry might have
                    currentEntry.Restart_SetHasNoBackref();
                }
            }
        }
    }

    // now we can safely remove all invalid entries
    m_pageManager->Restart_InitializePages( taskId, cleanupFileDirPages );

    // second run: recreate all tables, OmsVar entries, and old style
    // file directory entries
    Data_FileType entryFileType;
    FileDir_PageManager::EntryIterator it1    = m_pageManager->Begin();
    FileDir_PageManager::EntryIterator it1End = m_pageManager->End();
    while ( it1 != it1End ) {
        FileDir_PageManager::EntryAddress* ea = *it1;
        ++it1; // this is faster than using *it++ in the line above
               // because of the way operator++ is implemented in
               // FileDir_PageManager
        currentEntry.SetPersistentDataAddress(ea->dataPage,
                                              ea->entryAddress);
        SAPDBERR_ASSERT_STATE( currentEntry.IsValid() );
        entryFileType = currentEntry.GetFileType( migrateStartLayout );
        if ( (entryFileType == Data_TableFile)  ||
             (entryFileType == Data_OmsVarFile) ||
             (entryFileType == Data_OmsKeyFile) ) {
            RecreateFileEntry( taskId, currentEntry, bMigrate, migrateStartLayout );
        }
    }

    // third run: recreate the rest
    FileDir_PageManager::EntryIterator it2    = m_pageManager->Begin();
    FileDir_PageManager::EntryIterator it2End = m_pageManager->End();
    while ( it2 != it2End ) {
        FileDir_PageManager::EntryAddress* ea = *it2;
        ++it2; // this is faster than using *it++ in the line above
               // because of the way operator++ is implemented in
               // FileDir_PageManager
        currentEntry.SetPersistentDataAddress(ea->dataPage,
                                              ea->entryAddress);
        entryFileType = currentEntry.GetFileType( migrateStartLayout );
        if ( (entryFileType != Data_TableFile)  &&
             (entryFileType != Data_OmsVarFile) &&
             (entryFileType != Data_OmsKeyFile) ) {
            RecreateFileEntry( taskId, currentEntry, bMigrate, migrateStartLayout );
        }
    }

    if ( ! m_pageManager->RestartFinished( taskId, localMsgList ) ) {
        RTE_Crash( localMsgList );
    }

    localMsgList = Msg_List(
        Msg_List::Info,
        SDBMSG_FILEDIR_RESTART_OK);
    RTE_Message( localMsgList );
    localMsgList.ClearMessageList();

    return FileDir_Okay;
}

/**************************************************************************/

/*!
 * @brief Determine the latest valid version of pageEntry for the
 *        current converter version by tracing back if necessary. If
 *        no valid entry version is found, pageEntry is set to invalid.
 *
 * @param pageEntry
 *                   - [in]  the entry whose valid version is to be determined
 *                   - [out] the valid version of this entry or invalid
 * @param converterVersion [in] highest converterVersion a valid entry can have
 */
void FileDir_SharedDirectory::DetermineValidEntryVersion(
    FileDir_Entry&           pageEntry,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::DetermineValidEntryVersion",
        FileDir_Trace, 5 );

    if ( pageEntry.GetEntryConverterVersion()
         >
         converterVersion) {
        // entry was added in higher converter version
        if ( pageEntry.GetHasBackref() ) {
            FileDir_PageManager::EntryAddress ea = m_pageManager->GetEntry(
                pageEntry.GetBackrefPageNumber(),
                pageEntry.GetBackrefEntryLocation());
#ifdef SAPDB_SLOW
            FileDir_Entry referencedEntry( m_allocator );
            referencedEntry.SetPersistentDataAddress(ea.dataPage,
                                                     ea.entryAddress);
            SAPDBERR_ASSERT_STATE(
                pageEntry.GetFileNo()
                ==
                referencedEntry.GetFileNo() );
#endif
            // The current version of the entry is not valid:
            pageEntry.SetInvalid();

            pageEntry.SetPersistentDataAddress(ea.dataPage,
                                               ea.entryAddress);

            // Assume this version is now the correct one:
            pageEntry.SetValid();
            SAPDBERR_ASSERT_STATE( !pageEntry.IsLazyDeleteSet() );

            DetermineValidEntryVersion(pageEntry, converterVersion);
        } else {
            // no back ref -> ignore
            pageEntry.SetInvalid();
        }
    }
    return;
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::IsSavepointInProgress()
{
    return m_savepointActive;
}

/**********************************************************************/

void FileDir_SharedDirectory::DefragPages() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::DefragPages",
                             FileDir_Trace, 5 );

// FIXME: defragmentation can only be handled safely in garbage
// collector; everyone else might collide with a delete operation

// BTW: what about changes to statistics counters during move? will
// this ever be safe? (if we really use the lock list, then there
// should be no changes to statistics counters...)

//     RTE_TaskId taskId;
//     vgetpid( taskId );

//     FileDir_PageManager::FragPageIterator pit =
//         m_pageManager->FragPageListBegin();
//     for ( ; pit != m_pageManager->FragPageListEnd(); ++pit ) {
//         FileDir_Page::Iterator eit = (*pit)->Begin();
//         if ( eit != (*pit)->End() ) { ++eit; } // skip first entry
//         for ( ; eit != (*pit)->End(); ++eit ) {
//             FileDir_Entry currentEntry( m_allocator );
//             currentEntry.SetPersistentDataAddress( *pit, *eit );
//             if ( currentEntry.IsValid() ) {
//                 FileDir_FileNo fid = currentEntry.GetFileNo();
//                 FileDir_Entry* entry = static_cast<FileDir_Entry*>(
//                     GetFileEntry( taskId, fid ) );
//                 if ( (entry != 0) &&
//                      (entry->GetFileState() != Data_FileDeleted) ) {
//                     // FIXME: schedule move operation with garbage
//                     // collector instead of doing this...
//                     FileDir_Generic p;
//                     p.AssignEntry( entry );
//                     p.LockExclusive( taskId );
//                     // file might have been marked deleted in between
//                     // our check and before we acquired the lock, but
//                     // as we assume no new savepoint could have
//                     // started we know the entry is still there...
//                     if ( entry->GetFileState() != Data_FileDeleted) {
//                         entry->MoveEntry();
//                     }
//                     p.UnlockExclusive( taskId );
//                     /// ...
//                 }
//             }
//         }
//     }
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::IsTempFile(FileDir_FileNo fileNo)
{
    return fileNo.IsTempFileNo();
}

/**************************************************************************/

void FileDir_SharedDirectory::CrashAfterFlush()
{
    m_crashAfterFlush = true;
}

/**************************************************************************/

FileDir_ISharedDirectory& FileDir_ISharedDirectory::Instance()
{
    return FileDir_SharedDirectory::Instance();
}

/**********************************************************************/

FileDir_IDirectory& FileDir_ISharedDirectory::IFileDirInstance()
{
    return FileDir_SharedDirectory::Instance();
}

/**********************************************************************/

SAPDB_Bool FileDir_SharedDirectory::GetFileDirIterator(
    FileDir_Iterator& iterator,
    tgg00_TransContext* transContext)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::GetFileDirIterator",
        FileDir_Trace, 5 );

    if ( ! m_initialized ) {
        return false;
    }

    if ( !iterator.CreateIteratorObj() ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_IteratorObj) );
    }

    iterator.GetIteratorObj()->Clear();
    iterator.GetIteratorObj()->SetTransContext( transContext );

    return iterator.GetIteratorObj()->Initialize( *this, 0, 0 );
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::GetPrivateFileDirIterator(
    FileDir_Iterator& iterator,
    tgg00_TransContext* transContext)
{
    return this->GetFileDirIterator( iterator, transContext );
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::GetSharedFileDirIterator(
        FileDir_Iterator& iterator,
        tgg00_TransContext* transContext)
{
    return this->GetFileDirIterator( iterator, transContext );
}

/**************************************************************************/

void FileDir_SharedDirectory::WriteLockNeedUpdateList(const RTE_TaskId taskId)
{
   m_updateListLock->AcquireWriteLock( taskId );
}

/**************************************************************************/

void FileDir_SharedDirectory::ReadLockNeedUpdateList(const RTE_TaskId taskId)
{
   m_updateListLock->AcquireReadLock( taskId );
}

/**************************************************************************/
void FileDir_SharedDirectory::WriteUnlockNeedUpdateList(const RTE_TaskId taskId)
{
   m_updateListLock->ReleaseWriteLock( taskId );
}

/**************************************************************************/
void FileDir_SharedDirectory::ReadUnlockNeedUpdateList(const RTE_TaskId taskId)
{
   m_updateListLock->ReleaseReadLock( taskId );
}

/**************************************************************************/

void FileDir_SharedDirectory::AddToNeedUpdateList(
    const RTE_TaskId        taskId,
    const FileDir_FileNo    fileNo,
    const EntryUpdateAction action)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::AddToNeedUpdateList",
        FileDir_Trace, 5 );

#ifdef SAPDB_SLOW
    // verify that lock is set
    if ( m_updateListLock->TryAcquireReadLock( taskId ) ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "m_updateListLock must be set") );
    }
#endif

    // bd20GetDestinationConverterVersion can return too high a
    // converter version which results in entries being added to
    // m_needUpdateList although no savepoint is running any more;
    // until this has been fixed, we cannot have the following
    // assertion:
    // SAPDBERR_ASSERT_STATE( ! m_savepointActive );

    FileDir_Generic fileEntry;
    if ( this->GetFileEntry( taskId, fileNo, fileEntry ) ) {
        m_needUpdateList.InsertEnd( EntryUpdateInfo( fileEntry, action ) );
    }
 }

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::DelFile(
    RTE_TaskId            taskId,
    const FileDir_FileNo& fileNo,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::DelFile",
        FileDir_Trace, 5 );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::DelFile "
            << FileDir_FileNoToString( fileNo );
    }

    m_sysKeyCache.DeleteSysKeyFile( fileNo );

    if ( this->IsSavepointInProgress() ) {
        // there is a danger that a file entry might be removed after
        // the svp has started but before the file directory has been
        // flushed so the entry might get removed persistently
        // although the overlying action is relevant for the next svp
        // only; to avoid this, we do not remove entries during a svp
        FileDir_RWAutoRegion updateListLock( *m_updateListLock, true, taskId );
        if ( this->IsSavepointInProgress() ) {
            SAPDBTRACE_WRITELN(
                FileDir_Trace, 5,
                "fileNo : " << fileNo);

            if ( FileDir_Trace.TracesLevel( 4 ) ) {
                Kernel_VTrace()
                    << "FileDir_SharedDirectory::DelFile "
                    << "remove later: " << FileDir_FileNoToString( fileNo );
            }

            // entry needs to detach while lock is held, otherwise
            // savepoint might end and we might have two detach
            // operations for the same entry running concurrently
            FileDir_Generic fileEntry;
            if ( ! RemoveFileEntry( taskId, fileNo, fileEntry ) ) {
                return FileDir_FileNoNotFound;
            }

            this->DetachFromParentEntry( taskId, fileNo, fileEntry.GetPointer() );

// will deadlock with m_updateListLock:  fileEntry.SetRootPage( taskId, Data_PageNo() );

            m_needUpdateList.InsertEnd(
                EntryUpdateInfo( fileEntry, euaRemoveEntry ) );

            return FileDir_Okay;
        }
    }

    // check if file is in updateList

    // actually, there shouldn't be any entries in m_needUpdateList
    // while no savepoint is running and there would be no need for
    // this check; but bd20GetDestinationConverterVersion can return
    // too high a converter version which results in entries being
    // added to m_needUpdateList although no savepoint is running any
    // more; until this has been fixed, we need to do this check
    m_updateListLock->AcquireReadLock();

    UpdateInfoList::Iterator it     = m_needUpdateList.Begin();
    UpdateInfoList::Iterator it_end = m_needUpdateList.End();
    for (; it != it_end; ++it ) {
        if ( (*it).fileEntry.IsAssigned() &&
             ( fileNo == (*it).fileEntry.GetFileNo() ) ) {
            (*it).fileEntry.Detach();
        }
    }
    m_updateListLock->ReleaseReadLock();

    ++m_delOpCounter;
    return FileDir_GenericDirectory::DelFile( taskId, fileNo, msgList );
}

/**************************************************************************/

void FileDir_SharedDirectory::Dump(
    const RTE_TaskId taskId,
    Kernel_Dump&     dump)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::Dump",
        FileDir_Trace, 5 );

    FileDir_GenericDirectory::Dump( taskId, dump );
    // FIXME: dump all session local file directories as well

    if ( m_pageManager != 0 ) {
        m_pageManager->Dump( taskId, dump );
    }
}

/**************************************************************************/

void FileDir_SharedDirectory::Extract(
    const RTE_TaskId       taskId,
    const tsp00_VFilename& fileName)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::Extract",
        FileDir_Trace, 5 );

    Kernel_FileIO ioFile( fileName );

    if ( m_pageManager != 0 ) {
        m_pageManager->Extract( taskId, ioFile );
    }
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetPageCount() const
{
    return m_pageManager->GetPageCount();
}

/**************************************************************************/

SAPDB_Bool FileDir_SharedDirectory::MarkPageForWrite(
    const RTE_TaskId   taskId,
    const Data_PageNo& pageNo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::MarkPageForWrite",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_pageManager != 0 );

    if ( m_pageManager != 0 ) {
        return m_pageManager->MarkPageForWrite( taskId, pageNo );
    }
    return false;
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetEntryCount() const
{
    return FileDir_GenericDirectory::GetEntryCount();
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetHashTableSlotCount() const
{
    return FileDir_GenericDirectory::GetHashTableSlotCount();
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetUsedHashTableSlotCount() const
{
    return FileDir_GenericDirectory::GetUsedHashTableSlotCount();
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetMaxHashTableChainEntryCount() const
{
    return FileDir_GenericDirectory::GetMaxHashTableChainEntryCount();
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetResizeHashTableCount() const
{
    return FileDir_GenericDirectory::GetResizeHashTableCount();
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetStatisticsDeltaCount() const
{
    return m_statisticsDeltaCount;
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetCurrentStatisticsDeltaCount() const
{
    return m_needUpdateList.GetSize();
}

/**********************************************************************/

FileDir_FileNo FileDir_SharedDirectory::GetMaxFileNo() const
{
    return m_maxFileNo;
}

/**********************************************************************/

FileDir_FileNo FileDir_SharedDirectory::GetMaxTempFileNo() const
{
    return m_maxTempFileNo;
}

/**************************************************************************/

void FileDir_SharedDirectory::SetMigrationRunning(SAPDB_Bool running)
{
    m_migrationRunning = running;
}

/**************************************************************************/

SAPDB_UInt FileDir_SharedDirectory::GetDeleteOpCounter() const
{
    return m_delOpCounter;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_SharedDirectory::GetBadIndexCount() const
{
   SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::GetBadIndexCount",
                             FileDir_Trace, 5 );

    return static_cast<SAPDB_UInt4>( m_badIndexCount );
}

/**************************************************************************/

void FileDir_SharedDirectory::IncrementBadIndexCount()
{
   SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::IncrementBadIndexCount",
                             FileDir_Trace, 5 );

   RTESys_AtomicModify( m_badIndexCount, static_cast<SAPDB_UInt>( 1 ) );
}

/**************************************************************************/

void FileDir_SharedDirectory::DecrementBadIndexCount()
{
   SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::DecrementBadIndexCount",
                             FileDir_Trace, 5 );

   SAPDBERR_ASSERT_STATE( m_badIndexCount > 0 );

   if ( m_badIndexCount > 0 ) {
       RTESys_AtomicModify( m_badIndexCount, static_cast<SAPDB_Int>( -1 ) );
   }
}

/**************************************************************************/

void FileDir_SharedDirectory::AddFileNoSafetyMargin()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_SharedDirectory::AddFileNoSafetyMargin",
                             FileDir_Trace, 5 );

    ;
    this->Restart_SetFileNoHighWaterMark(
        FileDir_FileNo::AddOp( m_maxFileNo, 100000 ) );

}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::AddLooseShortColumnFile(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::AddLooseShortColumnFile",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::AddLooseShortColumnFile " << NewLine
            << "  fileNo:  " << FileDir_FileNoToString( fileNo ) << NewLine
            << "  parent:  " << FileDir_FileNoToString( parentFileNo ) << NewLine;
    }

    Converter_Version converterVersion =
        Converter_ICommon::Instance().Version();  // FIXME: or just m_converterVersion?


    FileDir_ShortColumnEntry* fe = new(m_allocator) FileDir_ShortColumnEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        parentFileNo,
        rootPage,
        fileTypeSet,
        fileVersion);

    if ( fe == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fe->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fe, msgList, FileDir_UseLocking );
    }

    if ( ret != FileDir_Okay ) {
        destroy( fe, m_allocator );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::AddLooseIndexFile(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::AddLooseIndexFile",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::AddLooseShortIndexFile " << NewLine
            << "  fileNo:  " << FileDir_FileNoToString( fileNo ) << NewLine
            << "  parent:  " << FileDir_FileNoToString( parentFileNo ) << NewLine
            << "  indexId: " << indexId;
    }

    if ( (indexId < 1) || (indexId > 255) ) {
        return FileDir_InvalidIndexNo;
    }

    Converter_Version converterVersion =
        Converter_ICommon::Instance().Version();  // FIXME: or just m_converterVersion?

    FileDir_IndexEntry* fe = new(m_allocator) FileDir_IndexEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        parentFileNo,
        indexId,
        rootPage,
        fileTypeSet,
        fileVersion);

    if ( fe == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fe->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fe, msgList, FileDir_UseLocking );
    }

    if ( ret != FileDir_Okay ) {
            destroy( fe, m_allocator );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::MigrationAddSingleIndexFile(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::MigrationAddSingleIndexFile",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::MigrationAddSingleIndexFile " << NewLine
            << "  fileNo:  " << FileDir_FileNoToString( fileNo ) << NewLine
            << "  parent:  " << FileDir_FileNoToString( parentFileNo ) << NewLine
            << "  indexId: " << indexId;
    }

    if ( (indexId < 1) || (indexId > 255) ) {
        return FileDir_InvalidIndexNo;
    }

    Converter_Version converterVersion =
        Converter_ICommon::Instance().Version();

    FileDir_SingleIndexEntry* fe = new(m_allocator) FileDir_SingleIndexEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        parentFileNo,
        indexId,
        rootPage,
        fileTypeSet,
        fileVersion);

    if ( fe == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fe->Initialize();

    if ( ret == FileDir_Okay ) {
        SAPDB_Bool ok = m_singleIndexList.InsertEnd( fe );
        if ( ok ) {
            ret = FileDir_Okay;
        } else {
            ret = FileDir_OutOfMemory;
        }
    }

    if ( ret != FileDir_Okay ) {
            destroy( fe, m_allocator );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::MigrationSwitchSingleIndexToMulti(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    tableFileNo,
    const SAPDB_Int          oldSingleIndexId,
    const SAPDB_Int          newMultiIndexId,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::MigrationSwitchSingleIndexToMulti",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_SharedDirectory::MigrationSwitchSingleIndexToMulti " << NewLine
            << "  tableFileNo  : " << tableFileNo << NewLine
            << "  singleIndexId: " << oldSingleIndexId << NewLine
            << "  multiIndexId : " << newMultiIndexId << NewLine;
    }

    FileDir_SingleIndexEntry* indexEntry = 0;
    SingleIndexList::Iterator it = m_singleIndexList.Begin();
    for ( SingleIndexList::Iterator itEnd = m_singleIndexList.End();
          it != itEnd;
          ++it ) {
        if ( ((*it)->GetParentFileNo() == tableFileNo)
             && ((*it)->GetIndexId() == oldSingleIndexId) ) {
            indexEntry = *it;
            break;
        }
    }

    if ( indexEntry == 0 ) {
        return FileDir_FileNoNotFound;
    }

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "  indexFileNo  : " << indexEntry->GetFileNo();
    }

    indexEntry->SetIndexId( taskId, newMultiIndexId );

    // attach index to table
    FileDir_Generic parentEntry;
    this->GetFileEntry( taskId, tableFileNo, parentEntry );

    if ( (! parentEntry.IsAssigned())
         || (parentEntry.GetFileState() == Data_FileDeleted) ) {
        return FileDir_FileNoNotFound;
    }

    if ( parentEntry.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_ReturnCode ret = this->AddFileEntry(
        taskId, indexEntry->GetFileNo(), indexEntry, msgList,
        FileDir_UseLocking );

    FileDir_Table tableEntry;
    tableEntry.Assign( parentEntry );
    if ( (ret == FileDir_Okay) &&
         ((ret = tableEntry.AddIndex(
               indexEntry->GetFileNo(), indexEntry->GetIndexId()))
          != FileDir_Okay) ) {
        // FIXME: error message

        FileDir_Generic dummyEntry;
        if ( ! RemoveFileEntry( taskId, indexEntry->GetFileNo(), dummyEntry ) ) {
            // if we reach here something has really gone wrong...
            msgList.AppendNewMessage(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_FILEDIR_ROLLBACK_ADD_FILE_FAILED,
                    Msg_Arg("FILENO",
                            FileDir_FileNoToString(indexEntry->GetFileNo() )),
                    Msg_Arg("FILETYPE",
                            SAPDB_ToString(Data_IndexFile))
                    ) );
            RTE_Crash( msgList );
        }
    }

    indexEntry->SwitchTypeToIndex( taskId );

    m_singleIndexList.Delete( it );

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_SharedDirectory::MigrationCleanupSingleIndexes(
    Trans_Context& transContext,
    Msg_List&      msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SharedDirectory::MigrationCleanupSingleIndexes",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    tbd_fileinfo fileInfo;
    tgg00_FileId dropFileId;
    Msg_List     knlMsg;
    SingleIndexList::Iterator it = m_singleIndexList.Begin();
    for ( SingleIndexList::Iterator itEnd = m_singleIndexList.End();
          it != itEnd;
          ++it ) {
        FileDir_SingleIndexEntry* indexEntry = *it;

        // build file id for server task and prefix destroy
        indexEntry->FillFileInfo( fileInfo );

        indexEntry->BuildFileId( dropFileId );
        dropFileId.fileTfn_gg00().becomes( tfnAux_egg00 );
        dropFileId.fileOldTfn_gg00().becomes( tfnMulti_egg00 );
        dropFileId.fileName_gg00()[15] =
            static_cast<teo00_Byte>( indexEntry->GetIndexId() );
        dropFileId.fileTfnNo_gg00()[0] = '\0';

        b17add_fdir( dropFileId.fileName_gg00(), fileInfo, transContext.OldTrans() );

        knlMsg = Msg_List(
            Msg_List::Info,
            SDBMSG_FILEDIR_MIGRATE_LEFTOVER_SINGLE_INDEX_DELETED,
            Msg_Arg(SDBMSGTAG_FILEDIR_MIGRATE_LEFTOVER_SINGLE_INDEX_DELETED__FILENO,
                    FileDir_FileNoToString( indexEntry->GetFileNo() ) ),
            Msg_Arg(SDBMSGTAG_FILEDIR_MIGRATE_LEFTOVER_SINGLE_INDEX_DELETED__FILENO1,
                    FileDir_FileNoToString( indexEntry->GetParentFileNo() ) )
            );
        RTE_Message( knlMsg );
        knlMsg.ClearMessageList();
    }

    m_singleIndexList.Clear();
    return FileDir_Okay;
}

/**************************************************************************/
