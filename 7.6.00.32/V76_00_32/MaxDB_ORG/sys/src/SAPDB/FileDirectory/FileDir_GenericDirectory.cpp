/*****************************************************************************/
/*!
 @file    FileDir_GenericDirectory.cpp
 @author  MartinKi
 @ingroup FileDir_GenericDirectory_Files

 @brief Implementation of class FileDir_GenericDirectory.

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

#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_TempTableEntry.hpp"
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_ShortColumnEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyPartitionEntry.hpp"
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_OmsFixedEntry.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "FileDirectory/FileDir_OmsVar.hpp"
#include "FileDirectory/FileDir_OmsFixed.hpp"

#include "FileDirectory/FileDir_BucketLocks.hpp"
#include "FileDirectory/FileDir_HashBucket.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_GenericDirectory.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

#include "Converter/Converter_Version.hpp"

#include "DataAccess/Data_Types.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Types.hpp"

#include "KernelCommon/Kernel_Dump.hpp"

#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "heo51.h"   // vgetpid()
#include "ggg00.h"
#include "hbd01_1.h" // b01niltree_id
#include "hgg01_3.h" // g01tabid

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

/// Maximum allowed size for a single file directory dump entry.
const int c_dumpBufferSize = 3000;

const SAPDB_Int4 c_initialCapacity = 151; ///< standard initial
                                          /// capacity of hash table

/**************************************************************************/

FileDir_GenericDirectory::FileDir_GenericDirectory(
    SAPDBMem_IRawAllocator& alloc,
    GeneralLockingBehaviour lockingType)
    : m_allocator(alloc),
      m_initialized(false),
      m_entryCount(0),
      m_hash(0),
      m_largestBucketSize(0),
      m_useBucketLocks(lockingType == FileDir_RegularLocking),
      m_bucketLocks(
          alloc,
          lockingType == FileDir_RegularLocking ?
          FileDir_BucketLocks::RealLock :
          FileDir_BucketLocks::DummyLock),
      m_lockingBehaviour(
          lockingType == FileDir_RegularLocking ?
          FileDir_UseLocking : FileDir_NoLocking)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::FileDir", FileDir_Trace, 5 );

    if ( !FileDir_RWRegionWrapper::InitSpinlockPool() ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_SPINLOCKPOOL_INIT_FAILED );
        RTE_Crash( errMsg );
    }
}

/**************************************************************************/

SAPDB_Bool FileDir_GenericDirectory::Initialize(SAPDB_Int4 initialCapacity)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::Initialize",
                             FileDir_Trace, 5 );

    if ( m_initialized ) {
        return true;
    }

    if ( initialCapacity == 0 ) {
        initialCapacity = c_initialCapacity;
    }

    m_hashLock = new(m_allocator) FileDir_RWRegionWrapper();
    m_hash     = new(m_allocator) HashInfo();

    if ( (m_hashLock == 0) || (m_hash == 0) ) {
        destroy( m_hashLock, m_allocator );
        destroy( m_hash, m_allocator );
        return false;
    }

    m_hash->bucketCount     = initialCapacity;
    m_hash->resizeCount     = 0;
    m_hash->usedBucketCount = 0;
    m_entryCount            = 0;

    FileDir_HashBucket emptyBucket;
    m_initialized = ( newarray(
                          m_hash->bucketList, m_hash->bucketCount,
                          m_allocator,
                          emptyBucket )
                      != 0);

    // make sure all buckets are properly initialized before proceeding
    for (BucketNo i=0; (i < m_hash->bucketCount) && m_initialized; i++) {
        if ( !m_hash->bucketList[i].Initialize() ) {
            destroyarray( m_hash->bucketList,
                          m_hash->bucketCount, m_allocator );
            m_hash->bucketList = 0;
            m_initialized      = false;
        }
    }

    m_initialized &= m_bucketLocks.Initialize( initialCapacity );

    if ( !m_initialized ) {
        FileDir_Common::OutOfMemoryCrash(
            sizeof(FileDir_HashBucket) * m_hash->bucketCount );
    }

    return m_initialized;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::Shutdown(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::Shutdown",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "FileDir_GenericDirectory::Shutdown";
    }

    // use locking just to be on the safe side
    m_hashLock->AcquireWriteLock( taskId );

    for (BucketNo i=0; i < m_hash->bucketCount; i++) {
        while ( !m_hash->bucketList[i].IsEmpty() ) {
            FileDir_Node* n = m_hash->bucketList[i].RemoveFirstNode();

            // we cannot use n->entry directly in destroy as n is part
            // of the object we want to destroy and destroy will set
            // n->entry = 0 _after_ having freed the memory, resulting
            // in potentially nasty side effects...
            FileDir_BaseEntry* entryAddress = n->entry;
            n->entry->decRefCounter();
            destroy( entryAddress, m_allocator );
        }
    }

    destroyarray( m_hash->bucketList, m_hash->bucketCount, m_allocator );

    m_hashLock->ReleaseWriteLock( taskId );
    destroy( m_hashLock, m_allocator );
    destroy( m_hash, m_allocator );

#ifdef SAPDB_SLOW
//    m_allocator.CheckConsistency();
#endif

    m_initialized = false;

    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_GenericDirectory::ResizeHash(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::ResizeHash",
                             FileDir_Trace, 5 );
    Kernel_VTrace trace;
    m_hashLock->AcquireWriteLock( taskId );
    if ( m_entryCount < m_hash->bucketCount ) {
        m_hashLock->ReleaseWriteLock( taskId );
        return true;
    }

    if ( FileDir_Trace.TracesLevel( 5 ) ) {
        trace << "FileDir_GenericDirectory::ResizeHash" << NewLine;
        trace.Flush();
    }

    SAPDBTRACE_WRITELN( FileDir_Trace, 5, " allocating new buckets" );
    // allocate objects for new hash
    BucketNo newBucketCount;
    FileDir_HashBucket* newBucketList;
    double multiplier = 2.0;
    do {
        newBucketCount = static_cast<BucketNo>(
            m_hash->bucketCount * multiplier);

        if ( (newBucketCount % 2) == 0 ) {
            // don't use even numbers for hash size
            ++newBucketCount;
        }
        multiplier = multiplier - 0.45; // in case there'll be another round

        FileDir_HashBucket emptyBucket;
        newarray( newBucketList, newBucketCount, m_allocator, emptyBucket );

        if ( (newBucketList == 0) ||
             !m_bucketLocks.Resize(newBucketCount) ) {
            trace
                << "FileDir_GenericDirectory::ResizeHash:" << NewLine
                << "  not enough memory to allocate " << newBucketCount
                << "buckets"  << NewLine;
            trace.Flush();
            destroyarray( newBucketList, newBucketCount, m_allocator );
            newBucketList  = m_hash->bucketList;
            newBucketCount = m_hash->bucketCount;
            continue;
        }

        // make sure all buckets are properly initialized before proceeding
        BucketNo i;
        for ( i=0; i < newBucketCount; ++i ) {
            if ( !newBucketList[i].Initialize() ) {
                trace
                    << "FileDir_GenericDirectory::ResizeHash:" << NewLine
                    << "  initialize of new buckets failed for bucket no "
                    << i << NewLine;
                trace.Flush();
                destroyarray( newBucketList, newBucketCount, m_allocator );
                newBucketList  = m_hash->bucketList;
                newBucketCount = m_hash->bucketCount;
                break;
            }
        }
    } while ( (m_hash->bucketList == newBucketList) && (multiplier > 1) );

    if ( newBucketList == m_hash->bucketList ) {
        trace
            << "FileDir_GenericDirectory::ResizeHash:" << NewLine
            << "  not enough memory for buckets, going on with old hash list"
            << NewLine;
        trace.Flush();
        m_hashLock->ReleaseWriteLock( taskId );
        return false;
    }

    HashInfo* newHash = new (m_allocator) HashInfo;
    if ( 0 == newHash ) {
        trace
            << "FileDir_GenericDirectory::ResizeHash:" << NewLine
            << "  not enough memory for hash list, going on with old hash list"
            << NewLine;
        trace.Flush();
        destroyarray( newBucketList, newBucketCount, m_allocator );
        m_hashLock->ReleaseWriteLock( taskId );
        return false;
    }

    // lock all buckets
    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "read lock all buckets" );
    BucketNo i;
    for ( i=0; i < m_hash->bucketCount; ++i ) {
        m_bucketLocks.LockShared( taskId, i );
    }

    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "copy bucket contents" );
    // copy contents from old buckets to new buckets
    SAPDB_UInt largestBucketSize = 0;
    BucketNo usedBucketCount     = 0;
    for ( i=0; i < m_hash->bucketCount; ++i ) {
        FileDir_Node* n        = 0;
        FileDir_Node* nextNode = 0;
        m_hash->bucketList[ i ].GetNextNode( nextNode );
        while ( nextNode != 0 ) {
            n = nextNode;
            m_hash->bucketList[ i ].GetNextNode( nextNode );
            SAPDB_UInt bucketSize;
            BucketNo bucketNo = HashFunction(
                n->entry->GetFileNo(), newBucketCount );
            if ( newBucketList[bucketNo].GetSize() == 0 ) {
                ++usedBucketCount;
            }
            FileDir_ReturnCode ret =
                newBucketList[bucketNo].AddNode(
                    taskId, n, bucketSize, FileDir_HashBucket::NoConcurrency );

            if ( ret != FileDir_Okay ) {
                trace
                    << "newBucketCount: " << newBucketCount << NewLine
                    << "bucketCount   : " << m_hash->bucketCount << NewLine
                    << "i             : " << i << NewLine
                    << "fileNo        : " << n->entry->GetFileNo()
                    << NewLine;
                trace.Flush();
                FileDir_Exception errMsg(
                    __CONTEXT__, FILEDIR_HASH_RESIZE_ADD_ERROR,
                    SAPDB_ToString(ret),
                    FileDir_FileNoToString(n->entry->GetFileNo()) );
                RTE_Crash(errMsg);
            }
            if ( bucketSize > largestBucketSize ) {
                largestBucketSize = bucketSize;
            }
        }
    }

    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "activating new hash list" );
    // activate new hash
    newHash->bucketList      = newBucketList;
    newHash->bucketCount     = newBucketCount;
    newHash->resizeCount     = m_hash->resizeCount + 1;
    newHash->usedBucketCount = usedBucketCount;
    HashInfo* oldHash        = m_hash;

    // make sure all content memory is in order before switching pointer
    RTESys_WriteMemoryBarrier();
    m_hash = newHash;

    // it is safe to work on the new hash now while we're cleaning up
    m_hashLock->ReleaseWriteLock( taskId );

    // make sure no one is working on the old hash any more by
    // acquiring a write lock for each bucket once (this will also
    // lock the corresponding new bucket which is annoying...)
    //
    // Attention: these are no-ops if buckets are protected by dummy
    // locks, so we can't guarantee that no one is working on the old
    // hash (e.g. some iterators) once we are through here !!!
    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "clearing old hash list users" );
    for ( i=0; i < oldHash->bucketCount; ++i ) {
        m_bucketLocks.UnlockShared( taskId, i );
        m_bucketLocks.LockExclusive( taskId, i );
        oldHash->bucketList[ i ].Clear();
        m_bucketLocks.UnlockExclusive( taskId, i );
    }
    // now it is safe to destroy the old hash
    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "destroying old hash" );
    destroyarray( oldHash->bucketList, oldHash->bucketCount, m_allocator );
    destroy( oldHash, m_allocator );

    // FIXME: call m_bucketLocks.Cleanup() at a later time to free space for old bucket pointer array once we can be reasonably certain that no one is working on it any more
    return true;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddFileEntry(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    FileDir_BaseEntry*       entry,
    Msg_List&                msgList,
    const LockingBehaviour   lockType)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddFileEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    SAPDBERR_ASSERT_ARGUMENT( fileNo == entry->GetFileNo() );

    // FIXME: helper to catch old style temp entries that still make
    // it into the file directory: to be removed
    SAPDBERR_ASSERT_ARGUMENT(
        reinterpret_cast<char*>(
            const_cast<FileDir_FileNo*>(&fileNo))[ 2 ] != 0xFF );

    if ( fileNo.IsInvalid() ) {
        return FileDir_FileNoInvalid;
    }

    // dirty check whether resize might be necessary
    if ( m_entryCount > m_hash->bucketCount ) {
        if ( !ResizeHash( taskId ) ) {
            return FileDir_OutOfMemory;
        }
    }

    // if we use locking, there won't be any concurrency on the bucket
    FileDir_HashBucket::Concurrency concurrency =
        (lockType == FileDir_UseLocking) ?
        FileDir_HashBucket::NoConcurrency :
        FileDir_HashBucket::ConcurrentReads;

    SAPDB_UInt bucketSize;

    FileDir_HashBucket* bucket;
    BucketNo bucketNo = LockBucketExclusive( taskId, fileNo, bucket );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "fileNo       : " << fileNo << NewLine <<
        "bucket no    : " << bucketNo << NewLine <<
        "bucketCount  : " << m_hash->bucketCount << NewLine <<
        "useLocking   : " << (lockType == FileDir_UseLocking) );

    FileDir_ReturnCode retCode =
        bucket->AddNode( taskId, entry->GetNodePtr(),
                         bucketSize, concurrency );

    if ( retCode == FileDir_FileNoExists ) {
        Kernel_VTrace() << "duplicate fileNo: " << fileNo;
        msgList = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_DUPLICATE_FILENO,
            Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )));
    }

    if ( bucketSize > m_largestBucketSize ) {
        m_largestBucketSize = bucketSize;
    }

    if ( retCode == FileDir_Okay ) {
        // add one reference count for being in hash, necessary to
        // avoid synchronisation problems during remove (PTS 1139062 M.Ki.)
        entry->incRefCounter();

        // needs to be protected by bucket lock to make sure we're
        // using the correct m_hash and bucketNo
        RTESys_AtomicModify( m_entryCount, 1 );
        if ( bucket->GetSize() == 1 ) {
            ++m_hash->usedBucketCount;
        }
    }

    m_bucketLocks.UnlockExclusive( taskId, bucketNo );

    return retCode;
}

/**************************************************************************/

SAPDB_Bool FileDir_GenericDirectory::GetFileEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      fileEntry) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetFileEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_BaseEntry* e = 0;

    SAPDBTRACE_WRITELN( FileDir_Trace, 9, "file no: " << fileNo );

    // FIXME: helper to catch old style temp entries that still make
    // it into the file directory: to be removed
    SAPDBERR_ASSERT_ARGUMENT(
        reinterpret_cast<char*>(
            const_cast<FileDir_FileNo*>(&fileNo))[ 2 ] != 0xFF );

    FileDir_HashBucket* bucket;
    BucketNo bucketNo = LockBucketShared( taskId, fileNo, bucket );
    e = bucket->Find( fileNo );
    if ( e != 0 ) {
        fileEntry.AssignEntry( e );
        SAPDBERR_ASSERT_STATE( ! e->IsLazyDeleteSet() );
    } else {
        fileEntry.Detach();
    }
    m_bucketLocks.UnlockShared( taskId, bucketNo );

    if ( g01vtrace.vtrAny_gg00 ) {
        if ( (e != 0) && (e->GetComment() != 0) ) {
            SAPDBTRACE_WRITELN(
                FileDir_Trace, 5,
                "file no: " << fileNo << "\t" << e->GetComment() );
        }
    }
    return fileEntry.IsAssigned();
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      fileEntry,
    Msg_List&             msgList) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode ret;
    if ( this->GetFileEntry( taskId, fileNo, fileEntry )
        && (fileEntry.GetFileState() != Data_FileDeleted) ) {
        ret = FileDir_Okay;
    } else {
        if ( g01vtrace.vtrAny_gg00 ) {
            Kernel_VTrace trace;
            if ( fileEntry.IsAssigned()
                 && (fileEntry.GetFileState() == Data_FileDeleted) ) {
                trace << "FileDir::GetEntry fileNo marked deleted: " << fileNo;
            } else {
                trace << "FileDir::GetEntry fileNo not found: " << fileNo;
            }
        }
        fileEntry.Detach();
        ret = FileDir_FileNoNotFound;
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GcGetFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GcGetFile", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = FileDir_Okay;
    if ( ! this->GetFileEntry( taskId, fileNo, p ) ) {
        retCode = FileDir_FileNoNotFound;
#ifdef SAPDB_SLOW
        Kernel_VTrace() << "FileDir::GcGetFile fileNo not found: " << fileNo;
#endif
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddTableFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_Table*           pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddTableFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_BaseEntry* fi;
    if ( fileNo.IsPersistentFileNo()   ||
         fileTypeSet.includes(ftsShared_egg00) ) {
        fi = new(m_allocator) FileDir_TableEntry(
            taskId,
            m_allocator,
            fileNo,
            converterVersion,
            rootPage,
            blobColCount,
            fileTypeSet,
            fileVersion);
    } else {
        fi = new(m_allocator) FileDir_TempTableEntry(
            m_allocator,
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            blobColCount,
            fileTypeSet);
    }

    if ( fi == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fi->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fi, msgList, m_lockingBehaviour );
    }

    if ( ret != FileDir_Okay ) {
        fi->Remove( taskId );
        destroy( fi, m_allocator );
    }

    if ( (pointer != 0) && (ret == FileDir_Okay) ) {
        pointer->AssignEntry( fi );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddSharedTempTableFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet,
    Msg_List&                msgList,
    FileDir_Table*           pointer)
{
    tgg91_FileVersion fversion;
    fversion.gg91SetDummyVers();

    return this->AddTableFile(
        taskId,
        fileNo,
        converterVersion,
        rootPage,
        blobColCount,
        fileTypeSet,
        fversion,
        msgList,
        pointer);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddIndexFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_Index*           indexPointer)
{

    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddIndexFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( (indexId < 1) || (indexId > 255) ) {
        return FileDir_InvalidIndexNo;
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( indexPointer != 0 ) {
        indexPointer->Detach();
    }

    FileDir_Generic parentEntry;
    this->GetFileEntry( taskId, parentFileNo, parentEntry );

    if ( (! parentEntry.IsAssigned())
         || (parentEntry.GetFileState() == Data_FileDeleted) ) {
        return FileDir_FileNoNotFound;
    }

    if ( parentEntry.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

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
        // index needs to be accessible in FileDir before it can be
        // added to table
        ret = AddFileEntry( taskId, fileNo, fe, msgList, m_lockingBehaviour );

        FileDir_Table tableEntry;
        tableEntry.Assign( parentEntry );
        if ( (ret == FileDir_Okay) &&
             ((ret = tableEntry.AddIndex(fileNo, fe->GetIndexId()))
              != FileDir_Okay) ) {
            // remove file from directory if add failed
            if ( RemoveFileEntry( taskId, fileNo, tableEntry ) == 0 ) {
                // if we reach here something has really gone wrong...
                msgList.AppendNewMessage(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_FILEDIR_ROLLBACK_ADD_FILE_FAILED,
                        Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                        Msg_Arg(
                            "FILETYPE",
                            SAPDB_ToString(Data_IndexFile))
                        ) );
                RTE_Crash( msgList );
            }
        }
    }

    if ( ret != FileDir_Okay ) {
        fe->Remove( taskId );
        destroy( fe, m_allocator );
    }

    if ( (indexPointer != 0) && (ret == FileDir_Okay) ) {
        indexPointer->AssignEntry( fe );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddShortColumnFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_ShortColumn*     shortColPointer)
{

    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddshortColumnFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( shortColPointer != 0 ) {
        shortColPointer->Detach();
    }

    FileDir_Generic parentEntry;
    this->GetFileEntry( taskId, parentFileNo, parentEntry );

    if ( (! parentEntry.IsAssigned())
         || (parentEntry.GetFileState() == Data_FileDeleted) ) {
        return FileDir_FileNoNotFound;
    }

    if ( parentEntry.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

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
        // short column entry needs to be accessible in FileDir before
        // it can be added to table
        ret = AddFileEntry( taskId, fileNo, fe, msgList, m_lockingBehaviour );

        if ( ret == FileDir_Okay ) {
            FileDir_Table tableEntry;
            tableEntry.Assign( parentEntry );
            tableEntry.SetShortColumnFileNo( fileNo );
        }
    }

    if ( ret != FileDir_Okay ) {
        fe->Remove( taskId );
        destroy( fe, m_allocator );
    }

    if ( (shortColPointer != 0) && (ret == FileDir_Okay) ) {
        shortColPointer->AssignEntry( fe );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddOmsFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_Oms*             pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddOmsFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsEntry* fi = new(m_allocator) FileDir_OmsEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        rootPage,
        objBodySize,
        objPerPageCount,
        chainCount);

    if ( fi == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fi->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fi, msgList, m_lockingBehaviour );
    }

    if ( ret != FileDir_Okay ) {
        fi->Remove( taskId );
        destroy( fi, m_allocator );
    }

    if ( (pointer != 0) && (ret == FileDir_Okay) ) {
        pointer->AssignEntry( fi );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddOmsKeyFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    const SAPDB_UInt2        keyPartitionCount,
    Msg_List&                msgList,
    FileDir_OmsKey*          pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddOmsKeyFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsKeyEntry* fi = new(m_allocator) FileDir_OmsKeyEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        rootPage,
        objBodySize,
        objPerPageCount,
        chainCount,
        keyPartitionCount);

    if ( fi == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fi->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fi, msgList, m_lockingBehaviour );
    }

    if ( ret != FileDir_Okay ) {
        fi->Remove( taskId );
        destroy( fi, m_allocator );
    }

    if ( (pointer != 0) && (ret == FileDir_Okay) ) {
        pointer->AssignEntry( fi );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddOmsKeyPartitionFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const SAPDB_UInt2        partitionNo,
    const Data_PageNo&       rootPage,
    Msg_List&                msgList,
    FileDir_OmsKeyPartition* omsKeyPartitionPointer)
{

    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::AddOmsKeyPartitionNo",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( (partitionNo < 1) || (partitionNo > 255) ) {
        return FileDir_InvalidIndexNo;
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( omsKeyPartitionPointer != 0 ) {
        omsKeyPartitionPointer->Detach();
    }

    FileDir_Generic parentEntry;
    this->GetFileEntry( taskId, parentFileNo, parentEntry );

    if ( (! parentEntry.IsAssigned()) ||
         (parentEntry.GetFileState() == Data_FileDeleted) ) {
        return FileDir_FileNoNotFound;
    }

    if ( parentEntry.GetFileType() != Data_OmsKeyFile ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_OmsKeyPartitionEntry* fe =
        new(m_allocator) FileDir_OmsKeyPartitionEntry(
            taskId,
            m_allocator,
            fileNo,
            converterVersion,
            parentFileNo,
            partitionNo,
            rootPage);

    if ( fe == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fe->Initialize();

    if ( ret == FileDir_Okay ) {
        // key partition needs to be accessible in FileDir before it
        // can be added to table
        ret = AddFileEntry( taskId, fileNo, fe, msgList, m_lockingBehaviour );

        FileDir_OmsKey omsKeyEntry;
        omsKeyEntry.Assign( parentEntry );
        if ( (ret == FileDir_Okay) &&
             ((ret = omsKeyEntry.AddKeyPartition(fileNo, fe->GetPartitionNo()))
              != FileDir_Okay) ) {
            // remove file from directory if add failed
            if ( RemoveFileEntry( taskId, fileNo, omsKeyEntry ) == 0 ) {
                // if we reach here something has really gone wrong...
                msgList.AppendNewMessage(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_FILEDIR_ROLLBACK_ADD_FILE_FAILED,
                        Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                        Msg_Arg(
                            "FILETYPE",
                            SAPDB_ToString(Data_OmsKeyPartitionFile))
                        ) );
                RTE_Crash( msgList );
            }
        }
    }

    if ( ret != FileDir_Okay ) {
        fe->Remove( taskId );
        destroy( fe, m_allocator );
    }

    if ( (omsKeyPartitionPointer != 0) && (ret == FileDir_Okay) ) {
        omsKeyPartitionPointer->AssignEntry( fe );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddOmsVarFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_OmsVar*          pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddOmsVarFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsVarEntry* fi = new(m_allocator) FileDir_OmsVarEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        rootPage,
        objBodySize,
        objPerPageCount,
        chainCount);

    if ( fi == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fi->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fi, msgList, m_lockingBehaviour );
    }

    if ( ret != FileDir_Okay ) {
        fi->Remove( taskId );
        destroy( fi, m_allocator );
    }

    if ( (pointer != 0) && (ret == FileDir_Okay) ) {
        pointer->AssignEntry( fi );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::AddOmsFixedFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_OmsFixed*        pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::AddOmsFixedFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsFixedEntry* fi = new(m_allocator) FileDir_OmsFixedEntry(
        taskId,
        m_allocator,
        fileNo,
        converterVersion,
        rootPage,
        objBodySize,
        objPerPageCount,
        chainCount);

    if ( fi == 0 ) {
        return FileDir_OutOfMemory;
    }

    FileDir_ReturnCode ret = fi->Initialize();

    if ( ret == FileDir_Okay ) {
        ret = AddFileEntry( taskId, fileNo, fi, msgList, m_lockingBehaviour );
    }

    if ( ret != FileDir_Okay ) {
        fi->Remove( taskId );
        destroy( fi, m_allocator );
    }

    if ( (pointer != 0) && (ret == FileDir_Okay) ) {
        pointer->AssignEntry( fi );
    }

    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetTableFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Table&        p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetTableFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_TableFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetFile(
    const RTE_TaskId&     taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetFile", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized == true );

    return GetEntry( taskId, fileNo, p, msgList );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetShortColumnFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_ShortColumn&  p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetShortColumnFile", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_ShortColumnFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetIndexFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Index&        p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetIndexFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_OmsKeyPartitionFile)
         && (fileEntry.GetFileType() != Data_IndexFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetOmsFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Oms&          p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetOmsFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_OmsFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetOmsKeyFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsKey&       p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetOmsKeyFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_OmsKeyFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetOmsVarFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsVar&       p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetOmsVarFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_OmsVarFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetOmsFixedFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsFixed&     p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::GetOmsFixedFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic fileEntry;
    FileDir_ReturnCode retCode =
        GetEntry( taskId, fileNo, fileEntry, msgList );

    if ( (retCode == FileDir_Okay)
         && (fileEntry.GetFileType() != Data_OmsFixedFile) ) {
        retCode = FileDir_FileTypeMismatch;
    }

    if ( retCode == FileDir_Okay ) {
        FileDir_Generic::TransferReference( p, fileEntry );
    } else {
        p.Detach();
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::ReassignIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& indexFileNo,
        const FileDir_FileNo& newTableFileNo,
        Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::ReassignIndexFile",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic indexE;
    FileDir_Generic newTableE;
    this->GetFileEntry( taskId, indexFileNo, indexE );
    this->GetFileEntry( taskId, newTableFileNo, newTableE );

    if ( (! indexE.IsAssigned()) || (! newTableE.IsAssigned()) ) {
        return FileDir_FileNoNotFound;
    }
    if ( (indexE.GetFileType()    != Data_IndexFile) ||
         (newTableE.GetFileType() != Data_TableFile) ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_IndexEntry* index    = static_cast<FileDir_IndexEntry*>(indexE.GetPointer());
    FileDir_TableEntry* newTable = static_cast<FileDir_TableEntry*>(newTableE.GetPointer());

    FileDir_Generic fileEntry;
    if ( ! this->GetFileEntry( taskId, index->GetParentFileNo(), fileEntry ) ) {
        return FileDir_FileNoNotFound;
    }

    if ( fileEntry.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_TableEntry* oldTable =
        static_cast<FileDir_TableEntry*>( fileEntry.GetPointer() );

    int indexNo = index->GetIndexId();
    FileDir_ReturnCode retCode = newTable->AddIndex( indexFileNo, indexNo );
    if ( retCode == FileDir_Okay ) {
        index->Reassign( taskId, newTableFileNo );
        retCode = oldTable->RemoveIndexNo( indexNo );
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::ReassignShortColumnFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& shortColumnFileNo,
    const FileDir_FileNo& newTableFileNo,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::ReassignShortColumnFile",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_Generic shortColE;
    FileDir_Generic newTableE;
    this->GetFileEntry( taskId, shortColumnFileNo, shortColE );
    this->GetFileEntry( taskId, newTableFileNo, newTableE );

    if ( (! shortColE.IsAssigned()) || (! newTableE.IsAssigned()) ) {
        return FileDir_FileNoNotFound;
    }
    if ( (shortColE.GetFileType() != Data_ShortColumnFile) ||
         (newTableE.GetFileType() != Data_TableFile) ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_ShortColumnEntry* shortColEntry =
        static_cast<FileDir_ShortColumnEntry*>(shortColE.GetPointer());
    FileDir_TableEntry* newTable =
        static_cast<FileDir_TableEntry*>(newTableE.GetPointer());

    FileDir_Generic tableE;
    if ( ! this->GetFileEntry(
             taskId, shortColEntry->GetParentFileNo(), tableE ) ) {
        return FileDir_FileNoNotFound;
    }

    if ( tableE.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_TableEntry* oldTable =
        static_cast<FileDir_TableEntry*>( tableE.GetPointer() );


    newTable->SetShortColumnFileNo( shortColumnFileNo );
    shortColEntry->Reassign( taskId, newTableFileNo );
    FileDir_FileNo dummy;
    oldTable->SetShortColumnFileNo( dummy );

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::DetachIndexFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& indexFileNo)
{
    FileDir_Generic indexE;

    if ( ! this->GetFileEntry( taskId, indexFileNo, indexE ) ) {
        return FileDir_FileNoNotFound;
    }

    if ( indexE.GetFileType() != Data_IndexFile) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_IndexEntry* index    =
        static_cast<FileDir_IndexEntry*>( indexE.GetPointer() );

    FileDir_Generic oldTableE;
    if ( ! this->GetFileEntry(
             taskId, index->GetParentFileNo(), oldTableE ) )  {
        return FileDir_FileNoNotFound;
    }

    if ( oldTableE.GetFileType() != Data_TableFile ) {
        return FileDir_FileTypeMismatch;
    }

    FileDir_TableEntry* oldTable =
        static_cast<FileDir_TableEntry*>( oldTableE.GetPointer() );

    int indexNo = index->GetIndexId();
    return  oldTable->RemoveIndexNo( indexNo );
}

/**************************************************************************/

FileDir_GenericDirectory::IteratorGetRc
FileDir_GenericDirectory::IteratorGetFile(
    tgg00_TransContext*   transContext,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::IteratorGetFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    RTE_TaskId taskId;
    if ( transContext != 0 ) {
        taskId = transContext->trTaskId_gg00;
    } else {
        vgetpid( taskId );
    }

    /* always use locking in case we're a session-local fdir and we have */
    /* concurrent access here                                            */
    m_hashLock->AcquireReadLock( taskId );

    FileDir_HashBucket* bucket;
    FileDir_BaseEntry* e = 0;
    BucketNo bucketNo    = LockBucketShared( taskId, fileNo, bucket );
    e = bucket->Find( fileNo );
    if ( e != 0 ) {
        p.AssignEntry( e );
    } else {
        p.Detach();
    }
    m_bucketLocks.UnlockShared( taskId, bucketNo );

    m_hashLock->ReleaseReadLock( taskId );

    return p.IsAssigned() ? igOkay : igFileNoNotFound;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::GetFileByOtherSession(
    const FileDir_FileNo&     fileNo,
    FileDir_Generic&          fileEntry,
    Msg_List&                 msgList) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetFileByOtherSesson",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    IteratorGetRc retCode =
        this->IteratorGetFile( 0, fileNo, fileEntry );

    if ( ! fileEntry.IsAssigned()
         || (fileEntry.GetFileState() == Data_FileDeleted) ) {
        return FileDir_FileNoNotFound;
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::DetachFromParentEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_BaseEntry*    fir)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::DetachFromParentEntry", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( fir == 0 ) {
        FileDir_Generic fileEntry;
        if ( ! this->GetFileEntry( taskId, fileNo, fileEntry ) ) {
            return FileDir_FileNoNotFound;
        }
        fir = fileEntry.GetPointer();
    }

    SAPDBERR_ASSERT_STATE( fir->GetFileState() == Data_FileDeleted );

    switch ( fir->GetFileType() ) {
    case Data_IndexFile:
    {
        FileDir_IndexEntry* fi = static_cast<FileDir_IndexEntry*>(fir);
        FileDir_Generic fileEntry;
        if ( this->GetFileEntry( taskId, fi->GetParentFileNo(), fileEntry ) ) {
            if ( fileEntry.GetFileType() != Data_TableFile ) {
                RTE_Crash(
                    SAPDBErr_Exception(
                        __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                        "index must be defined on table") );
            }
            // only set to invalid if table points to this entry
            // (could be otherwise if entry is removed as part of
            // UpdatePersistentEntryParts)
            FileDir_TableEntry* tableEntry =
                static_cast<FileDir_TableEntry*>( fileEntry.GetPointer() );
            if ( tableEntry->GetIndexFileNo( fi->GetIndexId() )
                 == fi->GetFileNo() ) {
                // ignore possible error as index might have already been
                // removed:
                tableEntry->RemoveIndex( fi->GetFileNo() );
            }
        } else {
            /*
             * garbage collector works from end of transaction towards
             * beginning; this might be changed in the future, so the
             * following code may be useful at some point in time...
             */
//             RTE_Crash(
//                 SAPDBErr_Exception(
//                     __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
//                     "index must be deleted before table") );
        }
    }
    break;
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumnEntry* fs =
            static_cast<FileDir_ShortColumnEntry*>(fir);
        FileDir_Generic fileEntry;
        if ( this->GetFileEntry( taskId, fs->GetParentFileNo(), fileEntry ) ) {
            if ( fileEntry.GetFileType() != Data_TableFile ) {
                RTE_Crash(
                    SAPDBErr_Exception(
                        __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                        "short column file must be defined on table") );
            }
            // only set to invalid if table points to this entry
            // (could be otherwise if entry is removed as part of
            // UpdatePersistentEntryParts)
            FileDir_TableEntry* tableEntry =
                static_cast<FileDir_TableEntry*>( fileEntry.GetPointer() );
            if ( tableEntry->GetShortColumnFileNo() == fs->GetFileNo() ) {
                FileDir_FileNo invalidFileNo;
                tableEntry->SetShortColumnFileNo( invalidFileNo );
            }
        } else {
            /*
             * garbage collector works from end of transaction towards
             * beginning; this might be changed in the future, so the
             * following code may be useful at some point in time...
             */
//             RTE_Crash(
//                 SAPDBErr_Exception(
//                     __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
//                     "short column file must be deleted before table") );
        }
    }
    break;
    case  Data_OmsKeyPartitionFile:
    {
        FileDir_OmsKeyPartitionEntry* fi =
            static_cast<FileDir_OmsKeyPartitionEntry*>(fir);
        FileDir_Generic fileEntry;
        if ( this->GetFileEntry( taskId, fi->GetParentFileNo(), fileEntry ) ) {
            if ( fileEntry.GetFileType() != Data_OmsKeyFile ) {
                RTE_Crash(
                    SAPDBErr_Exception(
                        __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                        "key partition must be defined on OMS key entry") );
            }
            // ignore possible error as key partition might have
            // already been removed
            static_cast<FileDir_OmsKeyEntry*>( fileEntry.GetPointer() )
                ->RemoveKeyPartition( fi->GetFileNo() );
        } else {
            /*
             * garbage collector works from end of transaction towards
             * beginning; this might be changed in the future, so the
             * following code may be useful at some point in time...
             */
//             RTE_Crash(
//                 SAPDBErr_Exception(
//                     __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
//                     "key partition must be deleted before key entry") );
        }
    }
    } // switch
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_GenericDirectory::DelFile(
    RTE_TaskId            taskId,
    const FileDir_FileNo& fileNo,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::DelFile", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( (fileNo.IsPersistentFileNo() && FileDir_Trace.TracesLevel( 5 ))
         || FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace() << "FDir_DelFile:: fileNo " << fileNo;
    }

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    FileDir_Generic fileEntry;
    if ( ! RemoveFileEntry( taskId, fileNo, fileEntry ) ) {
        return FileDir_FileNoNotFound;
    }

    if ( (fileNo.IsPersistentFileNo() && FileDir_Trace.TracesLevel( 5 ))
         || FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace() << "  root " << fileEntry.GetRootPage();
    }

    SAPDBERR_ASSERT_STATE( ! fileEntry.GetPointer()->IsLazyDeleteSet() );

    if ( fileEntry.GetFileState() != Data_FileDeleted ) {
        Msg_List subMsg =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILE_MUST_BE_DELETED,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo ) )
                );
        this->AddFileEntry(
            taskId, fileNo, fileEntry.GetPointer(),
            msgList, m_lockingBehaviour );
        msgList.Overrule( subMsg );
        subMsg.ClearMessageList();
        // crash with meaningful assertion in slowknl
        SAPDBERR_ASSERT_STATE( fileEntry.GetFileState() == Data_FileDeleted );
        return FileDir_SystemError;
    }

    // set root to nil page
    fileEntry.SetRootPage( taskId, Data_PageNo() );

    if ( fileEntry.GetFileType() == Data_TableFile ) {
        FileDir_ITable* tableEntry = fileEntry.GetPointer()->AsTable();
        if ( tableEntry->HasShortColumnFile()
             || ( tableEntry->GetMaxIndexId() > 0 ) ) {
            Msg_List subMsg =
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_FILEDIR_FILE_MUST_BE_DELETED,
                    Msg_Arg("FILENO", FileDir_FileNoToString( fileNo ) )
                    );
            this->AddFileEntry(
                taskId, fileNo, fileEntry.GetPointer(), msgList, m_lockingBehaviour );
            msgList.Overrule( subMsg );
            subMsg.ClearMessageList();
            // crash with meaningful assertion in slowknl
            SAPDBERR_ASSERT_STATE(
                (! tableEntry->HasShortColumnFile() )
                && ( tableEntry->GetMaxIndexId() == 0 ) );
            return FileDir_SystemError;
        }
    }

    this->DetachFromParentEntry( taskId, fileNo, fileEntry.GetPointer() );

    // mark entry for lazy delete
    fileEntry.GetPointer()->SetLazyDelete( taskId );

    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_GenericDirectory::RemoveFileEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      fileEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::RemoveFileEntry", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "file id : " << fileNo );

    SAPDBERR_ASSERT_STATE( m_entryCount > 0 );

    if ( !m_useBucketLocks ) {
        // synchronise with iterators via m_hashLock if we don't have
        // bucket locks
        m_hashLock->AcquireWriteLock( taskId );
    }

    FileDir_HashBucket* bucket;
    BucketNo bucketNo     = LockBucketExclusive( taskId, fileNo, bucket );
    FileDir_BaseEntry* fb = bucket->Remove( taskId, fileNo );

    if ( fb == 0 ) {
        m_bucketLocks.UnlockExclusive( taskId, bucketNo );
        if ( !m_useBucketLocks ) {
            m_hashLock->ReleaseWriteLock( taskId );
        }
        fileEntry.Detach();
        return false;
    }

    fileEntry.AssignEntry( fb );

    // remove reference for being in hash
    SAPDBERR_ASSERT_STATE( fb->GetRefCount() >= 2 );
    fb->decRefCounter(); // PTS 1139062 M.Ki.

    // this has to be part of the region protected by the lock to make
    // sure we are using the correct m_hash value and bucketNo
    RTESys_AtomicModify( m_entryCount, -1 );
    if ( bucket->GetSize() == 0 ) {
        RTESys_AtomicModify( m_hash->usedBucketCount, -1 );
    }

    m_bucketLocks.UnlockExclusive( taskId, bucketNo );

    if ( !m_useBucketLocks ) {
        m_hashLock->ReleaseWriteLock( taskId );
    }

    return true;
}

/**************************************************************************/

void FileDir_GenericDirectory::Dump(
    const RTE_TaskId taskId,
    Kernel_Dump& dump)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::Dump", FileDir_Trace, 5 );

    char dumpBuffer[ c_dumpBufferSize ];
    SAPDBFields_Field field( dumpBuffer, c_dumpBufferSize );

    if ( !m_initialized ) {
        // nothing to dump
        return;
    }

    for ( BucketNo i = 0; i < m_hash->bucketCount; i++ ) {
        FileDir_Generic f;
        FileDir_Node* node = 0;
        while ( true ) {
            m_hash->bucketList[i].GetNextEntry( node, f );
            if ( node == 0 ) {
                break;
            }
            SAPDBERR_ASSERT_STATE( f.IsAssigned() );

            FileDir_BaseEntry* fe  = f.GetPointer();
            SAPDB_Int          pos = 0;
            if ( fe->FillDumpEntry( taskId, field, pos ) ) {
                dump.InsertEntry(
                    Kernel_Dump::DmpFileDirectory,
                    Kernel_DumpPage::Entry(
                        field.GetPointer( 0, pos ),
                        pos ) );
            } else {
                // FIXME: insert message in knldiag
            }
        }
    }
}

/**************************************************************************/

FileDir_GenericDirectory::BucketNo
FileDir_GenericDirectory::LockBucketExclusive(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_HashBucket*&  bucket) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::LockBucketExclusive",
                             FileDir_Trace, 5 );

    // FIXME: if !m_useBucketLocks, we could return HashFunction( fileNo )
    // right away here

    BucketNo bucketNo;
    SAPDB_UInt resizeSave;

    while ( true ) {
        resizeSave = m_hash->resizeCount;
        bucketNo   = HashFunction( fileNo );
        bucket     = &m_hash->bucketList[bucketNo];
        m_bucketLocks.LockExclusive( taskId, bucketNo );
        if ( resizeSave == m_hash->resizeCount ) {
            break;
        }
        m_bucketLocks.UnlockExclusive( taskId, bucketNo );
    }

    return bucketNo;
}

/**************************************************************************/

FileDir_GenericDirectory::BucketNo
FileDir_GenericDirectory::LockBucketShared(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_HashBucket*&  bucket) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_GenericDirectory::LockBucketShared",
                             FileDir_Trace, 5 );

    // FIXME: if !m_useBucketLocks, we could return HashFunction( fileNo )
    // right away here

    BucketNo bucketNo;
    SAPDB_UInt resizeSave;

    while ( true ) {
        resizeSave = m_hash->resizeCount;
        bucketNo   = HashFunction( fileNo );
        bucket     = &m_hash->bucketList[bucketNo];
        m_bucketLocks.LockShared( taskId, bucketNo );
        if ( resizeSave == m_hash->resizeCount ) {
            break;
        }
        m_bucketLocks.UnlockShared( taskId, bucketNo );
    }

    return bucketNo;
}

/**************************************************************************/

FileDir_FileNo FileDir_GenericDirectory::GetNewFileNo()
{
    return FileDir_SharedDirectory::Instance().GetNewFileNo();
}

/**************************************************************************/

FileDir_FileNo FileDir_GenericDirectory::GetNewTempFileNo()
{
    return FileDir_SharedDirectory::Instance().GetNewTempFileNo();
}

/**************************************************************************/


FileDir_FileNo FileDir_GenericDirectory::GetMaxFileNo() const
{
    return FileDir_SharedDirectory::Instance().GetMaxFileNo();
}

/**********************************************************************/

FileDir_FileNo FileDir_GenericDirectory::GetMaxTempFileNo() const
{
    return FileDir_SharedDirectory::Instance().GetMaxTempFileNo();
}

/**************************************************************************/

SAPDBMem_IRawAllocator& FileDir_GenericDirectory::GetAllocator() const
{
    return m_allocator;
}

/**************************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetEntryCount", FileDir_Trace, 5 );

    return m_entryCount;
}

/**********************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetHashTableSlotCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetHashTableSlotCount",
        FileDir_Trace, 5 );

    return m_hash->bucketCount;
}

/**********************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetUsedHashTableSlotCount() const
{
    return m_hash->usedBucketCount;
}

/**********************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetMaxHashTableChainEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetMaxHashTableChainEntrycount",
        FileDir_Trace, 5 );

    return m_largestBucketSize;
}

/**********************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetResizeHashTableCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetResizeHashTableCount",
        FileDir_Trace, 5 );

    return m_hash->resizeCount;
}

/**********************************************************************/

SAPDB_UInt FileDir_GenericDirectory::GetCacheEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_GenericDirectory::GetCacheEntryCount", FileDir_Trace, 5 );

    return 0;
}

/**************************************************************************/

void FileDir_IDirectory::BuildNewTableFileId(
    const FileDir_FileNo&    tableFileNo,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    tgg00_FileId&            tableFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_GenericDirectory::BuildNewTableFileId", FileDir_Trace, 5 );

    tableFileId = b01niltree_id;

    tableFileId.fileTabId_gg00().rawAssign( tableFileNo.GetCharArray() );
    tableFileId.fileTfn_gg00().becomes( tfnTable_egg00 );
    tableFileId.fileTfnNo_gg00()[0] = ttfnNone_egg00;
    tableFileId.fileVersion_gg00()  = fileVersion;
    tableFileId.fileType_gg00()     = fileTypeSet;
}

/**************************************************************************/

void FileDir_IDirectory::BuildNewOmsFileId(
    const FileDir_FileNo&    fileNo,
    const tgg00_ObjFileType& fileType,
    const SAPDB_UInt1&       partitionNo,
    tgg00_FileId&            fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_GenericDirectory::BuildNewOmsFileId", FileDir_Trace, 5 );

    switch ( fileType ) {
    case oftUnknownType_egg00: /* KeyPartition */
        SAPDB_MemCopyNoCheck ( &fileId, &(g01tabid.obj_hash), sizeof (fileId) );
        fileId.fileTfn_gg00().becomes( tfnOmsInv_egg00 );
        fileId.fileType_gg00() = tgg00_FiletypeSet::build(
            2, ftsConcurrent_egg00, ftsPerm_egg00 );
        fileId.fileTabId_gg00().rawAssign( fileNo.GetCharArray() );
        break;
    case oftFixLenObjFile_egg00: /* Fixed, Keyed or continous container */
    case oftVarLenObjFile_egg00: /* primary container for variable long objects */
        fileId = b01niltree_id;
        if ( 0 == partitionNo ) {
            fileId.fileTfn_gg00().becomes( tfnObj_egg00 );
        } else {
            fileId.fileTfn_gg00().becomes( tfnContObj_egg00 );
            fileId.fileContObjFileNo_gg00() = partitionNo;
        }
        fileId.fileType_gg00() = tgg00_FiletypeSet::build(
            3, ftsObject_egg00, ftsConcurrent_egg00, ftsPerm_egg00 );
        fileId.fileObjFileType_gg00() = fileType;
        fileId.fileDirFileId_gg00().rawAssign( fileNo.GetCharArray() );
        break;
    }
}

/**************************************************************************/

const SAPDB_ToStringClass SAPDB_ToString(const FileDir_ReturnCode rc)
{
    switch ( rc ) {
        case FileDir_Okay:
            return SAPDB_ToStringClass( "Okay" );
        case FileDir_InitFailed:
            return SAPDB_ToStringClass( "InitFailed" );
        case FileDir_AlreadyRestarted:
            return SAPDB_ToStringClass( "AlreadyRestarted" );
        case FileDir_ReadError:
            return SAPDB_ToStringClass( "ReadError" );
        case FileDir_WriteError:
            return SAPDB_ToStringClass( "WriteError" );
        case FileDir_FileNoNotFound:
            return SAPDB_ToStringClass( "FileNoNotFound" );
        case FileDir_AlreadyMarkedForDeletion:
            return SAPDB_ToStringClass( "AlreadyMarkedForDeletion" );
        case FileDir_FileNoExists:
            return SAPDB_ToStringClass( "FileNoExists" );
        case FileDir_FileTypeMismatch:
            return SAPDB_ToStringClass( "FileTypeMismatch" );
        case FileDir_DuplicateIndex:
            return SAPDB_ToStringClass( "DuplicateIndex" );
        case FileDir_IndexExists:
            return SAPDB_ToStringClass( "IndexExists" );
        case FileDir_IndexNotFound:
            return SAPDB_ToStringClass( "IndexNotFound" );
        case FileDir_InvalidIndexNo:
            return SAPDB_ToStringClass( "InvalidIndexNo" );
        case FileDir_DuplicateBlobColumn:
            return SAPDB_ToStringClass( "DuplicateBlobColumn" );
        case FileDir_ConsistentViewCancelled:
            return SAPDB_ToStringClass( "ConcistentViewCancelled" );
        case FileDir_PartitionExists:
            return SAPDB_ToStringClass( "PartitionExists" );
        case FileDir_PartitionNotFound:
            return SAPDB_ToStringClass( "PartitionNotFound" );
        case FileDir_FileNoInvalid:
            return SAPDB_ToStringClass( "FileNoInvalid" );
        case FileDir_SystemError:
            return SAPDB_ToStringClass( "SystemError" );
        default:
            return SAPDB_ToStringClass( rc );
    }
}

/**************************************************************************/
