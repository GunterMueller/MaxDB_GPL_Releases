/*****************************************************************************/
/*!
 @file    FileDir_TableEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_TableEntry.

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
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "Converter/Converter_Version.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
// vgetpid():
#include "heo51.h"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd300.h" // bd300SetRootCheck
#include "ggg00.h"
#include "gbd00.h"

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

FileDir_TableEntry::FileDir_TableEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion)
    : FileDir_Entry(taskId, allocator, sizeof(PersistentTableData),
                    fileNo, converterVersion, rootPageNo),
      m_indexCount(0),
      m_indexList(allocator)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::FileDir_TableEntry",
                             FileDir_Trace, 5 );

    if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentTablePointer()->fileType        = GetFileType();

    PersistentTablePointer()->blobColCount    = blobColCount;
    PersistentTablePointer()->lobPageCount    = 0;
    m_lobPageCountDelta                       = 0;
    m_newPersistentBlobColCount               = blobColCount;
    PersistentTablePointer()->tableAttributes = tAttrNone;
    PersistentTablePointer()->filler          = 0;

    SAPDB_MemCopyNoCheck( &PersistentTablePointer()->fileVersion,
            &fileVersion, sizeof(SAPDB_Int2) );
    SAPDB_MemCopyNoCheck( &m_newPersistentFileVersion,
                          &PersistentTablePointer()->fileVersion,
                          sizeof(SAPDB_Int2) );

    SAPDBERR_ASSERT_ARGUMENT(
        sizeof(fileTypeSet) <= sizeof(PersistentTablePointer()->fileTypeSet) );

    PersistentTablePointer()->fileTypeSet      = 0;
    SAPDB_MemCopyNoCheck( &PersistentTablePointer()->fileTypeSet,
            &fileTypeSet,
            sizeof(fileTypeSet) );

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_TableEntry::FileDir_TableEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_Entry(allocator, persistentEntry),
      m_indexList(allocator),
      m_indexCount(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::FileDir_TableEntry-fromPersistent",
        FileDir_Trace, 5 );

    if ( persistentEntry.GetLength() != sizeof( PersistentTableData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_TableFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentTableData ) )
            );
        RTE_Crash( errMsg );
    }

    SAPDB_MemCopyNoCheck( &m_newPersistentFileVersion, &PersistentTablePointer()->fileVersion, sizeof(SAPDB_Int2) );
    m_newPersistentBlobColCount = PersistentTablePointer()->blobColCount;
    m_lobPageCountDelta         = 0;
    SetValid();
}

/**************************************************************************/

Data_FileType FileDir_TableEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_TableFile;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_TableEntry::GetBlobColCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetBlobColCount",
                             FileDir_Trace, 5 );

    return m_newPersistentBlobColCount;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_TableEntry::GetBlobColNo(const SAPDB_UInt2 colNo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetBlobColNo",
                             FileDir_Trace, 5 );

    SAPDB_UInt2* blobColNo = reinterpret_cast<SAPDB_UInt2*>(
        reinterpret_cast<char*>(&PersistentTablePointer()->blobColCount)
        + sizeof(SAPDB_UInt2)
        + sizeof(SAPDB_UInt2) * (colNo-1));
    return *blobColNo;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::AddBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::AddBlobColumn",
                             FileDir_Trace, 5 );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "FileNo            : " << PersistentTablePointer()->fileNo << NewLine
        << "Blob column number: " << columnNumber << NewLine
        << "converter version : " << converterVersion );

    RTE_TaskId taskId;
    vgetpid( taskId );

    // make sure column number is not already marked as a blob column:
    int i;
    for (i=1; i <= GetBlobColCount(); i++) {
        if ( GetBlobColNo( i ) == columnNumber ) {
            return FileDir_DuplicateBlobColumn;
        }
    }

    FileDir_Page* newDataPage;

    // DDL - no internal locking should be necessary
    PersistentTableData* newRecord =
        reinterpret_cast<PersistentTableData*>(
            FileDir_PageManager::Instance()->ReserveSpace(
                taskId,
                this->GetPersistentLength()
                + sizeof(Backref)
                + sizeof(SAPDB_UInt2),
                true,
                newDataPage,
                m_dataPage->PageNo()));

    if ( newRecord == 0 ) {
        return FileDir_OutOfMemory;
    }

    CopyBody(newRecord, PersistentTablePointer());
    ++newRecord->blobColCount;

#ifdef SAPDB_SLOW
    // make sure this is not supposed to have been added in a previous
    // converter version
    Converter_Version pageVersion = newDataPage->ConverterVersion();
    if ( pageVersion.IsValid() ) {
        SAPDBERR_ASSERT_ARGUMENT(
            converterVersion > pageVersion );
    }
#endif
    newRecord->header.converterVersion = converterVersion;
    AddBackrefToNewRecord(newRecord);

    SAPDB_UInt2* newCol = reinterpret_cast<SAPDB_UInt2*>(
        reinterpret_cast<char*>(newRecord)
        + this->GetFirstBlobDescOffset()
        + sizeof(SAPDB_UInt2) * GetBlobColCount());
    *newCol = columnNumber;

    RechainPersistentPointer( taskId, newDataPage, newRecord );

    m_dataPage->UnlockPageForUpdate( taskId ); // this unlocks the
                                               // 'new' data page
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::RemoveBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::RemoveBlobColumn",
                             FileDir_Trace, 5 );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "FileNo            : " << PersistentTablePointer()->fileNo << NewLine
        << "Blob column number: " << columnNumber << NewLine
        << "converter version : " << converterVersion );

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_Page* newDataPage;

    // DDL - no internal locking should be necessary
    PersistentTableData* newRecord =
        reinterpret_cast<PersistentTableData*>(
            FileDir_PageManager::Instance()->ReserveSpace(
                taskId,
                this->GetPersistentLength()
                + sizeof(Backref)
                - sizeof(SAPDB_UInt2),
                true,
                newDataPage,
                m_dataPage->PageNo()));
    CopyBody(newRecord, PersistentTablePointer());
    --newRecord->blobColCount;

#ifdef SAPDB_SLOW
    Converter_Version pageVersion = newDataPage->ConverterVersion();
    if ( pageVersion.IsValid() ) {
        SAPDBERR_ASSERT_ARGUMENT(
            converterVersion > pageVersion );
    }
#endif
    newRecord->header.converterVersion = converterVersion;

    // remove column
    SAPDB_UInt2* colPos = reinterpret_cast<SAPDB_UInt2*>(
        reinterpret_cast<char*>(newRecord)
        + this->GetFirstBlobDescOffset());
    bool found = false;
    for ( int i=1;
          i<GetBlobColCount();
          ++i, ++colPos) {
        if ( *colPos == columnNumber ) {
            found = true;
        }
        if ( found ) {
            *colPos = *(colPos + 1);
        }
    }
    *colPos = 0;  // zero freed byte

    AddBackrefToNewRecord(newRecord);

    RechainPersistentPointer( taskId, newDataPage, newRecord );

    m_dataPage->UnlockPageForUpdate( taskId ); // this unlocks the
                                               // 'new' data page
    return FileDir_Okay;
}

/**************************************************************************/

/**
 * @brief Returns length of current entry that is actually filled with
 *        data, i.e. padding bytes at the end of the entry are not included.
 */
SAPDB_UInt2 FileDir_TableEntry::GetPersistentLength()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetPersistentLength",
                             FileDir_Trace, 5 );
    char* persistentStart = reinterpret_cast<char*>(PersistentTablePointer());
    char* persistentEnd   =
        reinterpret_cast<char*>(&PersistentTablePointer()->blobColCount)
        + sizeof(SAPDB_UInt2)
        + PersistentTablePointer()->blobColCount * sizeof(SAPDB_UInt2);

    return
        static_cast<SAPDB_UInt2>(persistentEnd - persistentStart);
}

/**************************************************************************/

/*!
 * @brief Returns offset of first blob column description from start
 *        of persistent data.
 */
SAPDB_UInt2 FileDir_TableEntry::GetFirstBlobDescOffset()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetFirstBlobDescOffset",
                             FileDir_Trace, 5 );
    char* blobStartPos   =
        reinterpret_cast<char*>(&PersistentTablePointer()->blobColCount)
        + sizeof(SAPDB_UInt2);

    return static_cast<SAPDB_UInt2>(
        blobStartPos - reinterpret_cast<char*>(PersistentTablePointer()));
}

/**************************************************************************/

void FileDir_TableEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );
    if ( !m_needsUpdateAfterSavepoint )
        return;

    if ( memcmp( &m_newPersistentFileVersion, &PersistentTablePointer()->fileVersion, sizeof(SAPDB_Int2) ) ) {
        SAPDB_MemCopyNoCheck( &PersistentTablePointer()->fileVersion, &m_newPersistentFileVersion, sizeof(SAPDB_Int2) );
    }

    if ( m_newPersistentBlobColCount != PersistentTablePointer()->blobColCount ) {
        this->SetBlobColCount( m_newPersistentBlobColCount );
    }

    if ( m_lobPageCountDelta != 0 ) {
        RTESys_AtomicModify(
            PersistentTablePointer()->lobPageCount,
            static_cast<SAPDB_Int8>(m_lobPageCountDelta)  );
        m_lobPageCountDelta  = 0;
    }

    FileDir_Entry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::AddIndex(
    const FileDir_FileNo& indexFileNo,
    const SAPDB_UInt2     indexNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::AddIndex",
                             FileDir_Trace, 5 );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        " fileNo " << indexFileNo << ", indexNo: " << indexNo);

    SAPDBERR_ASSERT_ARGUMENT( indexNo >  0 );

    // during restart index entry might not have been entered in the
    // file directory yet, so no check for its existence is done here

    if ( m_indexCount < indexNo ) {
        if ( !m_indexList.Reserve( indexNo ) ) {
            return FileDir_OutOfMemory;
        }
        int i=0;
        FileDir_FileNo invalidFileNo;
        for (i=m_indexList.GetSize(); i < indexNo; i++) {
            m_indexList.InsertEnd( invalidFileNo );
        }
        m_indexCount = indexNo;
    }

    if ( m_indexList[ indexNo-1].IsValid() ) {
        return FileDir_IndexExists;
    }

    m_indexList[ indexNo-1 ] = indexFileNo;

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::RemoveIndex(
    const FileDir_FileNo& indexFileNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::RemoveIndex",
                             FileDir_Trace, 5 );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        " fileNo " << indexFileNo);

    // DDL, no locking should be necessary

    if ( indexFileNo.IsInvalid() ) {
        return FileDir_IndexNotFound;
    }

    IndexList::Iterator it = m_indexList.Begin();
    bool found = false;
    while ( it != m_indexList.End() ) {
        if ( *it == indexFileNo ) {
            (*it).Invalidate();
            // determine number of used index slots
            int i;
            for (i = m_indexCount-1; i >= 0; --i ) {
                if ( m_indexList[ i ].IsValid() ) {
                    break;
                }
                m_indexCount = i;
            }
            found = true;
            break;
        }
        ++it;
    }

    if ( !found ) {
        return FileDir_IndexNotFound;
    }
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::RemoveIndexNo(
    const SAPDB_UInt2 indexNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::RemoveIndex",
                             FileDir_Trace, 5 );

    // DDL, no locking should be necessary

    if ( indexNo > m_indexCount ) {
        return FileDir_IndexNotFound;
    }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        " fileNo " << m_indexList[ indexNo - 1 ]
        << ", indexNo " << indexNo);

    m_indexList[ indexNo - 1 ].Invalidate();

    // determine number of used index slots
    int i;
    for (i = m_indexCount-1; i >= 0; --i ) {
        if ( m_indexList[ i ].IsValid() ) {
            break;
        }
        m_indexCount = i;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_TableEntry::GetMaxIndexId() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetIndexCount",
                             FileDir_Trace, 5 );
    return m_indexCount;
}

/**************************************************************************/

FileDir_FileNo FileDir_TableEntry::GetIndexFileNo(SAPDB_UInt2 indexNo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetIndexFileNo",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( indexNo >  0 );
//    SAPDBERR_ASSERT_ARGUMENT( indexNo <= m_indexCount );

    if ( indexNo > m_indexCount ) {
        return FileDir_FileNo(); // return invalid fileno
    }

    return m_indexList[ indexNo - 1 ];
}

/**************************************************************************/

FileDir_ITable* FileDir_TableEntry::AsTable()
{
    return static_cast<FileDir_ITable*>(this);
}

/**************************************************************************/

// SAPDB_Bool FileDir_TableEntry::Verify()
// {
//     // check if length fits to blob col count

//     // check if indices exist
// }

/*************************************************************************/

FileDir_ReturnCode FileDir_TableEntry::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::Initialize",
                             FileDir_Trace, 5 );

    if ( (FileDir_Entry::Initialize() != FileDir_Okay) || (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_TableEntry::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::FillFileInfo",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( sizeof(fileInfo.fi_type)
                               <= sizeof(SAPDB_Int4) );
    fileInfo.fi_type.clear();
    SAPDB_MemCopyNoCheck( &fileInfo.fi_type,
            &PersistentTablePointer()->fileTypeSet,
            sizeof(fileInfo.fi_type) );

    fileInfo.fi_state.clear();

    Data_FileState fileState = this->GetFileState();
    if ( Data_FileBad == fileState ) {
        fileInfo.fi_state.addElement( f_bad );
    }
    if ( Data_FileReadOnly == fileState ) {
        fileInfo.fi_state.addElement( f_read_only );
    }
    if ( Data_FileNotAccessible == fileState ) {
        fileInfo.fi_state.addElement( f_not_accessible );
    }

    fileInfo.fi_fill1           = false;
    fileInfo.fi_descr.clear();
    fileInfo.fi_root            = this->GetRootPage();
    SAPDB_MemCopyNoCheck( &fileInfo.fi_vers,
            &this->GetFileVersion(),
            sizeof(SAPDB_Int2) );
    fileInfo.fi_col_cnt         = this->GetBlobColCount();
    fileInfo.fi_user_ref.gg91SetNilRef ();
    fileInfo.fi_var_col_cnt_pos = cgg_nil_varcol_cnt_off;
}

/**************************************************************************/

const tgg91_FileVersion& FileDir_TableEntry::GetFileVersion() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetFileVersion",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    return m_newPersistentFileVersion;
}

/**************************************************************************/

void FileDir_TableEntry::SetFileVersion(
    const tgg91_FileVersion& newFileVersion)
{
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( newFileVersion == this->GetFileVersion() ) {
        return;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newPersistentFileVersion = newFileVersion;
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId, PersistentPointer()->fileNo,
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

    //  delta value must be set before persistent part to avoid race
    // condition when querying
    m_newPersistentFileVersion = newFileVersion;
    RTESys_WriteMemoryBarrier();
    m_dataPage->SetDirty();
    SAPDB_MemCopyNoCheck( &PersistentTablePointer()->fileVersion,
            &newFileVersion, sizeof(SAPDB_Int2) );
}

/**************************************************************************/

void FileDir_TableEntry::SetBlobColCount(
    const SAPDB_UInt2 newBlobColCount)
{
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( newBlobColCount == this->GetBlobColCount() ) {
        return;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newPersistentBlobColCount = newBlobColCount;
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId, PersistentPointer()->fileNo,
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

    //  delta value must be set before persistent part to avoid race
    // condition when querying
    m_newPersistentBlobColCount = newBlobColCount;
    RTESys_WriteMemoryBarrier();
    m_dataPage->SetDirty();
    PersistentTablePointer()->blobColCount = newBlobColCount;
}

/**************************************************************************/

void FileDir_TableEntry::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::SetFileInfo",
                             FileDir_Trace, 5 );

    // FIXME: only write to and lock page if there actually are
    // changes to its contents
    m_dataPage->LockPageForUpdate( taskId );

    SAPDB_MemCopyNoCheck( &PersistentTablePointer()->fileTypeSet,
            &fileInfo.fi_type,
            sizeof(fileInfo.fi_type) );

    // release lock here because other set routines will lock the page
    // again, possible resulting in deadlocks during savepoints
    m_dataPage->UnlockPageForUpdate( taskId );

    if ( fileInfo.fi_state.includes( f_not_accessible ) ) {
        this->SetFileState( Data_FileNotAccessible );
    }  else if ( fileInfo.fi_state.includes( f_bad ) ) {
        this->SetFileState( Data_FileBad );
    }  else if ( fileInfo.fi_state.includes( f_read_only ) ) {
        this->SetFileState( Data_FileReadOnly );
    } else {
        this->SetFileState( Data_FileOk );
    }

    // PTS 1133273 M.Ki.
    this->SetBlobColCount( fileInfo.fi_col_cnt );

    if ( fileInfo.fi_root != this->GetRootPage() ) {
        this->SetRootPage( taskId, fileInfo.fi_root );
    }

    this->SetFileVersion( fileInfo.fi_vers );

}

/**************************************************************************/

void FileDir_TableEntry::SetArchive(SAPDB_Bool isArchive)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::SetArchive",
                             FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );

    tgg00_FiletypeSet ft;
    SAPDB_MemCopyNoCheck( &ft,
            &PersistentTablePointer()->fileTypeSet,
            sizeof(ft) );
    ft.addElement( ftsArchive_egg00 );

    m_dataPage->LockPageForUpdate( taskId );
    SAPDB_MemCopyNoCheck(
        &PersistentTablePointer()->fileTypeSet,
        &ft,
        sizeof(ft));
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool FileDir_TableEntry::IsArchive() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::IsArchive",
                             FileDir_Trace, 5 );

    tgg00_FiletypeSet ft;
    SAPDB_MemCopyNoCheck( &ft,
            &PersistentTablePointer()->fileTypeSet,
            sizeof(ft) );
    return ft.includes( ftsArchive_egg00 );
}

/**************************************************************************/

void
FileDir_TableEntry::SetRecordsPacked(
    const RTE_TaskId taskId,
    const SAPDB_Bool recordsPacked)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::SetRecordsPacked", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( this->GetEntryCount( taskId ) == 0 );

    m_dataPage->LockPageForUpdate( taskId );

    if ( recordsPacked ) {
        PersistentTablePointer()->tableAttributes |= tAttrRecordsPacked;
    } else {
        PersistentTablePointer()->tableAttributes &= ! tAttrRecordsPacked;
    }

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool
FileDir_TableEntry::AreRecordsPacked() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::AreRecordsPacked", FileDir_Trace, 5 );

    return
        (PersistentTablePointer()->tableAttributes & tAttrRecordsPacked)
        != 0;
}

/**************************************************************************/

SAPDB_Bool
FileDir_TableEntry::IsDynamic() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::IsDynamic", FileDir_Trace, 5 );

    tgg00_FiletypeSet ft;
    SAPDB_MemCopyNoCheck( &ft,
            &PersistentTablePointer()->fileTypeSet,
            sizeof(ft) );
    return ft.includes( ftsDynamic_egg00 );
}

/**************************************************************************/

void FileDir_TableEntry::SetShortColumnFileNo(
    const FileDir_FileNo shortColumnFileNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::SetShortColumnFileNo",
                             FileDir_Trace, 5 );

    if ( FileDir_Trace.TracesLevel( 1 ) ) {
        Kernel_VTrace()
            << "FileDir_SetShortColumnFileNo:: tableFileNo : "
            << this->GetFileNo()
            << " short column fileNo: " << shortColumnFileNo;
    }

    m_shortColumnFileNo = shortColumnFileNo;
}

/**************************************************************************/

void FileDir_TableEntry::GetShortColumnFileNo(
    FileDir_FileNo& shortColumnFileNo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::GetShortColumnFileNo",
                             FileDir_Trace, 5 );

    shortColumnFileNo = m_shortColumnFileNo;
}

/**************************************************************************/

FileDir_FileNo FileDir_TableEntry::GetShortColumnFileNo() const
{
    return m_shortColumnFileNo;
}

/**************************************************************************/

SAPDB_Bool FileDir_TableEntry::HasShortColumnFile() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TableEntry::HasShortColumnFile",
                             FileDir_Trace, 5 );

    return m_shortColumnFileNo.IsValid();
}

/**************************************************************************/

SAPDB_Bool FileDir_TableEntry::IsLobPageCounterInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::IsLobPageCounterInitialized",
        FileDir_Trace, 6 );

    return PersistentTablePointer()->lobPageCount != -1;
}

/**************************************************************************/

void FileDir_TableEntry::ModifyLobPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::ModifyLobPageCount",
        FileDir_Trace, 5 );

    if ( ! this->IsLobPageCounterInitialized() ) {
        return;
    }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        SAPDB_ToString( converterVersion )
        << ", " << SAPDB_ToString( this->GetLobPageCount() )
        << ", " <<  SAPDB_ToString( delta ) );

    if ( static_cast<SAPDB_Int8>( this->GetLobPageCount() ) + delta < 0 ) {
        SAPDBERR_ASSERT_STATE( false );
        return;
    }

    this->ModifyCounter(
        taskId,
        converterVersion,
        PersistentTablePointer()->lobPageCount,
        m_lobPageCountDelta,
        delta);
}

/**************************************************************************/

SAPDB_UInt8 FileDir_TableEntry::GetLobPageCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::GetLobPageCount",
        FileDir_Trace, 6 );

    // if lob page count is not initialized, -1 will be returned
    return
        static_cast<SAPDB_UInt8>(
            PersistentTablePointer()->lobPageCount + m_lobPageCountDelta );
}

/**************************************************************************/

void FileDir_TableEntry::SetLobPageCount(
    const SAPDB_UInt8 lobPageCount,
    const SAPDB_Bool  setInitialized)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::SetLobPageCount",
        FileDir_Trace, 5 );

    if ( setInitialized ) {
        RTE_TaskId taskId;
        vgetpid( taskId );

        m_dataPage->LockPageForUpdate( taskId );
        PersistentTablePointer()->lobPageCount = lobPageCount;
        m_dataPage->UnlockPageForUpdate( taskId );

        m_lobPageCountDelta = 0;
    } else {
        PersistentTablePointer()->lobPageCount = -1;
    }
}

/**************************************************************************/

void
FileDir_TableEntry::InitializeTableCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount,
    const SAPDB_UInt8 lobPageCount)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::InitializeTableCounters", FileDir_Trace, 5 );

    const SAPDB_Bool bKeepPageLocked = true;
    this->InternalInitializeBasicCounters(
        taskId,
        leafPageCount,
        indexPageCount,
        entryCount,
        bKeepPageLocked );

    this->PersistentTablePointer()->lobPageCount = lobPageCount;
    m_lobPageCountDelta                          = 0;
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool FileDir_TableEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Dump {
        SAPDB_UInt2 blobColCount;
        SAPDB_Int2  fileVersion;
        SAPDB_UInt1 fileTypeSet;
        SAPDB_UInt1 tableAttributes;
        SAPDB_Int2  indexCount;
        SAPDB_Int8  lobPageCount;
        SAPDB_Int4  lobPageCountDelta;
        SAPDB_Int4  filler1;
        FileDir_FileNo shortColFileNo;
    };
    Dump dump;

    SAPDB_UInt size = sizeof(dump) + m_indexCount * sizeof(FileDir_FileNo);
    if ( !FileDir_Entry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         ( (static_cast<SAPDB_Int>( dumpBuffer.GetLength()
            - startPos - size) ) <= 0 ) ) {
        return false;
    }

    int pos = startPos;

    dump.blobColCount = this->GetBlobColCount();
    dump.fileVersion  = *reinterpret_cast<const SAPDB_UInt2*>( &this->GetFileVersion());
    dump.fileTypeSet  =
        static_cast<SAPDB_UInt1>( PersistentTablePointer()->fileTypeSet );
    dump.tableAttributes =
        static_cast<SAPDB_UInt1>( PersistentTablePointer()->tableAttributes );
    dump.indexCount        = static_cast<SAPDB_Int2>( m_indexCount );
    dump.lobPageCount      = PersistentTablePointer()->lobPageCount;
    dump.lobPageCountDelta = m_lobPageCountDelta;
    dump.filler1           = 0;
    dump.shortColFileNo    = m_shortColumnFileNo;

    void* pointer = dumpBuffer.GetPointer( pos, sizeof(dump) );
    SAPDB_MemCopyNoCheck( pointer, &dump, sizeof(dump) );
    pos += sizeof(dump);


    for ( int i=0; i < m_indexCount; ++i, pos += sizeof(FileDir_FileNo) ) {
        pointer = dumpBuffer.GetPointer( pos, sizeof(dump) );
        SAPDB_MemCopyNoCheck(
            pointer, &m_indexList[ i ], sizeof(FileDir_FileNo) );
    }

    startPos = pos;
    return true;
}

/**************************************************************************/

void FileDir_TableEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::BuildFileId", FileDir_Trace, 5 );

    fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    fileId.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    fileId.fileUserRef_gg00().gg91SetNilRef ();
    SAPDB_MemCopyNoCheck(
        &fileId.fileVersion_gg00(),
        &this->GetFileVersion(),
        sizeof(SAPDB_Int2) );
    fileId.fileRoot_gg00()      = this->GetRootPage();
    fileId.fileHandling_gg00().clear();
    fileId.fileBdUse_gg00().clear();

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentTablePointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );

    fileId.fileName_gg00().rawAssign( cgg_zero_fn );
    if ( this->IsPersistent() ) {
        fileId.fileTfn_gg00().becomes( tfnTable_egg00 );
    } else {
        fileId.fileTfn_gg00().becomes( tfnTemp_egg00 );
    }
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

void FileDir_TableEntry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::FillPreparedFileId", FileDir_Trace, 5 );

    FileDir_Entry::FillPreparedFileId( fileId );

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentTablePointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );
}

/**************************************************************************/

void FileDir_TableEntry::GetFileVersion(tgg91_FileVersion& fileVersion) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::GetFileVersion", FileDir_Trace, 5 );

    SAPDB_MemCopyNoCheck(
            &fileVersion,
            &this->GetFileVersion(),
            sizeof(SAPDB_Int2) );
}

/**************************************************************************/

FileDir_TableEntry::FileDir_TableEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_Entry(taskId, allocator, persistentEntry,
                    sizeof(PersistentTableData), currentPersistentLayout,
                    curPersistentPos),
      m_indexList(allocator),
      m_indexCount(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::FileDir_TableEntry-migration", FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    // contains all fields that have ever been in use by this entry
    struct MigrationHelpStruct {
        SAPDB_UInt2 blobColCount;
        SAPDB_Int2  fileVersion;
        SAPDB_Byte  fileTypeSet;
        SAPDB_Byte  tableAttributes;
        SAPDB_Int8  lobPageCount;
    };

    MigrationHelpStruct helpStruct;

    /*
     * add initial values for all fields that have been added at a
     * later date here
     */

    // lob counter
    helpStruct.lobPageCount = -1;

    // table attributes
    helpStruct.tableAttributes = tAttrNone;

    SAPDB_Bool bPersistentDataParsed = false;
    SAPDB_Bool bNoMigration          = true;

    if ( currentPersistentLayout < fdelWithLobSize ) {

        // migrate to layout that contains the lob-size
        bNoMigration = false;

        if ( ! bPersistentDataParsed ) {
            struct LayoutWithoutLobs {
                SAPDB_UInt2 blobColCount;
                SAPDB_Int2  fileVersion;
                SAPDB_Byte  fileTypeSet[4];
            };

            LayoutWithoutLobs* oldLayout =
                reinterpret_cast<LayoutWithoutLobs*>( curPersistentPos );

            helpStruct.blobColCount    = oldLayout->blobColCount;
            helpStruct.fileVersion     = oldLayout->fileVersion;
            // fileTypeSet needs explicit addressing because of byte swap
            helpStruct.fileTypeSet     =
                static_cast<SAPDB_UInt1>( oldLayout->fileTypeSet[0] );

            bPersistentDataParsed = true;
            curPersistentPos     += sizeof( LayoutWithoutLobs );
        }

        helpStruct.lobPageCount    = -1;
    }

    if ( currentPersistentLayout < fdelWithFileAttributes ) {

        // migrate to layout that contains table attributes
        bNoMigration = false;

        if ( ! bPersistentDataParsed ) {
            struct LayoutWithoutTableAttributes {
                SAPDB_UInt2 blobColCount;
                SAPDB_Int2  fileVersion;
                SAPDB_Byte  fileTypeSet[4];
                SAPDB_Int8  lobPageCount;
            };

            LayoutWithoutTableAttributes* oldLayout =
                reinterpret_cast<LayoutWithoutTableAttributes*>(
                    curPersistentPos );

            helpStruct.blobColCount    = oldLayout->blobColCount;
            helpStruct.fileVersion     = oldLayout->fileVersion;
            // fileTypeSet needs explicit addressing because of byte swap
            helpStruct.fileTypeSet     =
                static_cast<SAPDB_UInt1>( oldLayout->fileTypeSet[0] );
            helpStruct.lobPageCount    = oldLayout->lobPageCount;

            bPersistentDataParsed = true;
            curPersistentPos     += sizeof( LayoutWithoutTableAttributes );
        }

        helpStruct.tableAttributes = tAttrNone;
    }

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentTablePointer()->blobColCount,
            curPersistentPos,
            sizeof( PersistentTableData ) - sizeof( PersistentData ) );

        curPersistentPos +=
            sizeof( PersistentTableData )
            - sizeof( FileDir_Entry::PersistentData );

    } else {
        PersistentTablePointer()->blobColCount    = helpStruct.blobColCount;
        PersistentTablePointer()->fileVersion     = helpStruct.fileVersion;
        PersistentTablePointer()->fileTypeSet     = helpStruct.fileTypeSet;
        PersistentTablePointer()->tableAttributes = helpStruct.tableAttributes;
        PersistentTablePointer()->filler          = 0;
        PersistentTablePointer()->lobPageCount    = helpStruct.lobPageCount;
    }

    SetValid();

    // init member variables
    SAPDB_MemCopyNoCheck(
        &m_newPersistentFileVersion,
        &PersistentTablePointer()->fileVersion,
        sizeof(SAPDB_Int2) );
    m_newPersistentBlobColCount = PersistentTablePointer()->blobColCount;
    m_lobPageCountDelta         = 0;

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}
