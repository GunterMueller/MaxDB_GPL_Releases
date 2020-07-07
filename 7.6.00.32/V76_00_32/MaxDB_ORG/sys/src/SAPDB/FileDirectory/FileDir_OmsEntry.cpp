/*****************************************************************************/
/*!
 @file    FileDir_OmsEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsEntry.

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

#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "Converter/Converter_Version.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "gbd900.h" // PRIM_CONT_OBJ_FILE_NO_BD900

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

FileDir_OmsEntry::FileDir_OmsEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount)
    : FileDir_Entry(taskId, allocator, sizeof(PersistentOmsData),
                    fileNo, converterVersion, rootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::FileDir_OmsEntry",
                             FileDir_Trace, 5 );

    if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentOmsPointer()->fileType         = GetFileType();

    PersistentOmsPointer()->objBodySize      = objBodySize;
    PersistentOmsPointer()->objPerPageCount  = objPerPageCount;
    PersistentOmsPointer()->chainCount       = chainCount;
    PersistentOmsPointer()->partitionCounter = 0;
    PersistentOmsPointer()->maxPageSeqNo     = 0;

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_OmsEntry::FileDir_OmsEntry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const SAPDB_UInt2        requiredSize,
        const FileDir_FileNo     fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPageNo,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        const SAPDB_UInt2        partitionCount)
    : FileDir_Entry(taskId, allocator, requiredSize,
                    fileNo, converterVersion, rootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::FileDir_OmsEntry-fromSubclass",
                             FileDir_Trace, 5 );

    PersistentOmsPointer()->fileType         = GetFileType();

    PersistentOmsPointer()->objBodySize      = objBodySize;
    PersistentOmsPointer()->objPerPageCount  = objPerPageCount;
    PersistentOmsPointer()->chainCount       = chainCount;
    PersistentOmsPointer()->partitionCounter = partitionCount;

    //
    // entry is not yet set valid and page is still locked !!!
    //
}

/**************************************************************************/

FileDir_OmsEntry::FileDir_OmsEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_Entry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsEntry::FileDir_OmsEntry-fromPersistent",
        FileDir_Trace, 5 );

    SetValid();
}

/**************************************************************************/

Data_FileType FileDir_OmsEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_OmsFile;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_OmsEntry::GetObjBodySize() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetObjBodySize",
                             FileDir_Trace, 5 );
    return PersistentOmsPointer()->objBodySize;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_OmsEntry::GetObjPerPageCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetObjPerPageCount",
                             FileDir_Trace, 5 );

    return PersistentOmsPointer()->objPerPageCount;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_OmsEntry::GetChainCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetChainCount",
                             FileDir_Trace, 5 );

    return PersistentOmsPointer()->chainCount;
}

/**************************************************************************/

void FileDir_OmsEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );
    if ( !m_needsUpdateAfterSavepoint )
        return;

    if ( m_indexPageCountDelta != 0 ) {
        this->ModifyFreePageCount(
            taskId, converterVersion, m_indexPageCountDelta );
        m_indexPageCountDelta = 0;
    }

    FileDir_Entry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/

FileDir_IOms* FileDir_OmsEntry::AsOms()
{
    return static_cast<FileDir_IOms*>(this);
}

/**************************************************************************/

// SAPDB_Bool FileDir_OmsEntry::Verify()
// {
// }

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_OmsEntry::FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos)
{
    struct Dump {
        SAPDB_UInt2 chainCount;
        SAPDB_UInt2 objBodySize;
        SAPDB_UInt2 objPerPageCount;
        SAPDB_UInt2 partitionCounter;
        SAPDB_UInt8 maxPageSeqNo;
    };
    Dump dump;

    if ( !FileDir_Entry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - sizeof(dump)) <= 0) ) {
        return false;
    }

    dump.chainCount      = PersistentOmsPointer()->chainCount;
    dump.objBodySize     = PersistentOmsPointer()->objBodySize;
    dump.objPerPageCount = PersistentOmsPointer()->objPerPageCount;
    dump.partitionCounter= PersistentOmsPointer()->partitionCounter;
    dump.maxPageSeqNo    = PersistentOmsPointer()->maxPageSeqNo;

    void* pointer = dumpBuffer.GetPointer( startPos, sizeof(dump) );
    SAPDB_MemCopyNoCheck( pointer, &dump, sizeof(dump) );
    startPos += sizeof(dump);

    return true;
}

/**************************************************************************/

void FileDir_OmsEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsEntry::BuildFileId", FileDir_Trace, 5 );

    fileId = b01niltree_id;
    fileId.fileType_gg00()          = tgg00_FiletypeSet::build(
        3, ftsObject_egg00, ftsConcurrent_egg00, ftsPerm_egg00);
    fileId.fileTfn_gg00().becomes( tfnObj_egg00 );
    fileId.fileObjFileType_gg00().becomes( oftFixLenObjFile_egg00 );
    fileId.fileDirFileId_gg00().rawAssign(
        PersistentOmsPointer()->fileNo.GetCharArray() );
    fileId.fileRoot_gg00()          = PersistentOmsPointer()->rootPage;
}

/**************************************************************************/

void FileDir_OmsEntry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsEntry::FillPreparedFileId", FileDir_Trace, 5 );

    FileDir_Entry::FillPreparedFileId( fileId );

    fileId.fileType_gg00() =
        tgg00_FiletypeSet::build(
            3, ftsObject_egg00, ftsConcurrent_egg00, ftsPerm_egg00);
}

/**************************************************************************/

void FileDir_OmsEntry::ModifyPageCounterUnprotected(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    PageCount&              pageCounter,
    SAPDB_Int4&             deltaCounter,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::ModifyPageCounterUnprotected",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( m_persistentData == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
            SAPDB_ToString(m_refCounter),
            FileDir_FileNoToString( m_persistentData->fileNo ) );
        RTE_Crash(errMsg);
    }

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
            deltaCounter += delta;
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentOmsPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    // in this method we do not need to use AtomicModify to update the
    // counter because the caller assures us that there can be no
    // concurrent modifications
    m_dataPage->SetDirty();
    if ( ( delta < 0 ) &&
         ( pageCounter <
           static_cast<PageCount>( -delta ) ) ) {
        pageCounter = 0;
    } else {
        pageCounter += delta;
    }
}

/**************************************************************************/

void FileDir_OmsEntry::IncrementSequenceCounter(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        SAPDB_UInt8&            sequenceCounter)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::IncrementSequenceCounter",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( m_persistentData == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
            SAPDB_ToString(m_refCounter),
            FileDir_FileNoToString( m_persistentData->fileNo ) );
        RTE_Crash(errMsg);
    }

    if ( ! FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        RTESys_AtomicModify(
            sequenceCounter,
            static_cast<SAPDB_Int8>( 1 ) );
    } else {
        // make sure we are not writing to data page while it is being
        // flushed to disk; if we want to avoid the locking we need to
        // use a delta counter here
        m_dataPage->LockPageForUpdate( taskId );
        m_dataPage->SetDirty();
        ++sequenceCounter;
        m_dataPage->UnlockPageForUpdate( taskId );
    }
}

/**************************************************************************/

void FileDir_OmsEntry::ModifyFreePageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::ModifyFreePageCount",
                             FileDir_Trace, 5 );

    this->ModifyPageCounterUnprotected(
        taskId,
        converterVersion,
        PersistentOmsPointer()->indexPageCount,
        m_indexPageCountDelta,
        delta);
}

/**************************************************************************/

SAPDB_UInt4 FileDir_OmsEntry::GetFreePageCount(
    const RTE_TaskId taskId) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetFreePageCount",
                             FileDir_Trace, 5 );

    return FileDir_Entry::GetIndexPageCount( taskId );
}

/**************************************************************************/

SAPDB_UInt8 FileDir_OmsEntry::GetCurrentPageSequenceNo() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetCurrentPageSequenceNo",
                             FileDir_Trace, 5 );

    return RTESys_AtomicRead( PersistentOmsPointer()->maxPageSeqNo );
}

/**************************************************************************/

void FileDir_OmsEntry::IncrementPageSequenceNo(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::IncrementPageSequenceNo",
                             FileDir_Trace, 5 );

    this->IncrementSequenceCounter(
        taskId,
        converterVersion,
        PersistentOmsPointer()->maxPageSeqNo );
}

/**************************************************************************/

void FileDir_OmsEntry::ModifyIndexPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::ModifyIndexPageCount",
                             FileDir_Trace, 5 );

    // index page count is used to implement free page counter and
    // cannot be used
    SAPDBERR_ASSERT_STATE( false );
}

/**************************************************************************/

SAPDB_UInt4 FileDir_OmsEntry::GetIndexPageCount(
    const RTE_TaskId taskId) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsEntry::GetIndexPageCount",
                             FileDir_Trace, 5 );

    // index page count is used to implement free page counter and
    // cannot be used
    SAPDBERR_ASSERT_STATE( false );

    return 0;
}

/**************************************************************************/

FileDir_OmsEntry::FileDir_OmsEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    const SAPDB_UInt2       requiredSize,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_Entry(taskId, allocator, persistentEntry,
                    requiredSize, currentPersistentLayout,
                    curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsEntry::FileDir_OmsEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    SAPDB_Bool bNoMigration = true;

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentOmsPointer()->chainCount,
            curPersistentPos,
            sizeof( PersistentOmsData ) - sizeof( PersistentData ) );

        curPersistentPos +=
            sizeof( PersistentOmsData ) - sizeof( PersistentData );

    } else {
        // put update of new persistent structure here if necessary
    }

    //
    // entry is not yet set valid and page is still locked !!!
    //
}

/**************************************************************************/
