/*****************************************************************************/
/*!
 @file    FileDir_IndexEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_IndexEntry.

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
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"

#include "Converter/Converter_Version.hpp"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "hbd300.h"

#include "KernelCommon/Kernel_VTrace.hpp"
#include "hgg01_3.h" // g01vtrace

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

FileDir_IndexEntry::FileDir_IndexEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const FileDir_FileNo     parentFileNo,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPageNo,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion)
    : FileDir_Entry(taskId, allocator, sizeof(PersistentIndexData),
                    fileNo, converterVersion, rootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IndexEntry::FileDir_IndexEntry",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( indexId >  0 );

    if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentIndexPointer()->fileType     = GetFileType();
    PersistentIndexPointer()->parentFileNo = parentFileNo;
    m_newParentFileNo                      = parentFileNo;
    PersistentIndexPointer()->indexId      = indexId;
    ;
    PersistentIndexPointer()->fileTypeSet  = 0;
    SAPDB_MemCopyNoCheck( &PersistentIndexPointer()->fileTypeSet,
            &fileTypeSet,
            sizeof(fileTypeSet) );

    PersistentIndexPointer()->usageCount      = 0;
    PersistentIndexPointer()->indexAttributes = iAttrNone;

    SetValid();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_IndexEntry::FileDir_IndexEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_Entry(allocator, persistentEntry)
{
    if ( persistentEntry.GetLength() != sizeof( PersistentIndexData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_IndexFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentIndexData ) )
            );
        RTE_Crash( errMsg );
    }

    m_newParentFileNo = PersistentIndexPointer()->parentFileNo;
    SetValid();
}

/**************************************************************************/

FileDir_IndexEntry::~FileDir_IndexEntry()
{}

/**************************************************************************/

Data_FileType FileDir_IndexEntry::GetFileType() const
{
    return Data_IndexFile;
}

/**************************************************************************/

void FileDir_IndexEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    this->Reassign( taskId, m_newParentFileNo );

    FileDir_Entry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/

FileDir_FileNo FileDir_IndexEntry::GetParentFileNo() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IndexEntry::FileDir_GetParentFileNo",
                             FileDir_Trace, 5 );

    FileDir_FileNo pfn = PersistentIndexPointer()->parentFileNo;

    if ( m_newParentFileNo != pfn ) {
        return m_newParentFileNo;
    } else {
        return pfn;
    }
}

/**************************************************************************/

SAPDB_UInt2 FileDir_IndexEntry::GetIndexId() const
{
    SAPDB_UInt2 id = PersistentIndexPointer()->indexId;

    return id;
}

/**************************************************************************/

void FileDir_IndexEntry::Reassign(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& tableFileNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IndexEntry::FileDir_Reassign",
                             FileDir_Trace, 5 );

    if ( g01vtrace.vtrAny_gg00 ) {
        Kernel_VTrace() << "FileDir_IndexEntry: " << this->GetFileNo()
                        << " reassign from " << this->GetParentFileNo()
                        << " to " << tableFileNo;
    }

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
            m_newParentFileNo = tableFileNo;
            deltaCounterUsed = true;
            if ( FileDir_Trace.TracesLevel( 1 ) ) {
                Kernel_VTrace()
                    << "FileDir_IndexEntry: " << this->GetFileNo()
                    << " delay reassignment";
            }
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
                m_needsUpdateAfterSavepoint = true;
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

    m_newParentFileNo = tableFileNo; // must be set before persistent
                                     // part to avoid race condition
                                     // when querying
    m_dataPage->LockPageForUpdate( taskId );
    PersistentIndexPointer()->parentFileNo = tableFileNo;
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

// SAPDB_UInt2 FileDir_IndexEntry::GetColumnCount() const
// {
//     return PersistentIndexPointer()->colCount;
// }

/**************************************************************************/

FileDir_IIndex* FileDir_IndexEntry::AsIndex()
{
    return static_cast<FileDir_IIndex*>(this);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_IndexEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_IndexEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Dump {
        FileDir_FileNo parentFileNo;
        SAPDB_Int2     indexId;
        SAPDB_UInt1    fileTypeSet;
        SAPDB_UInt1    indexAttr;
        SAPDB_UInt4    usageCount;
    };
    Dump dump;

    if ( !FileDir_Entry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - sizeof(dump))
          <= 0) ) {
        return false;
    }

    int pos = startPos;

    dump.parentFileNo = PersistentIndexPointer()->parentFileNo;
    dump.indexId      = PersistentIndexPointer()->indexId;
    dump.fileTypeSet  = PersistentIndexPointer()->fileTypeSet;
    dump.indexAttr    = PersistentIndexPointer()->indexAttributes;
    dump.usageCount   = PersistentIndexPointer()->usageCount;

    void* pointer = dumpBuffer.GetPointer( pos, sizeof(dump) );
    SAPDB_MemCopyNoCheck ( pointer, &dump, sizeof(dump) );
    pos += sizeof(dump);

    startPos = pos;
    return true;
}

/**************************************************************************/

void FileDir_IndexEntry::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    SAPDBERR_ASSERT_ARGUMENT( sizeof(fileInfo.fi_type)
                               <= sizeof(SAPDB_Int4) );
    fileInfo.fi_type.clear();
    SAPDB_MemCopyNoCheck( &fileInfo.fi_type,
            &PersistentIndexPointer()->fileTypeSet,
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
    fileInfo.fi_vers.gg91SetDummyVers();
    fileInfo.fi_col_cnt         = 0;
    fileInfo.fi_user_ref.gg91SetNilRef ();
    fileInfo.fi_var_col_cnt_pos = cgg_nil_varcol_cnt_off;
}

/**************************************************************************/

void FileDir_IndexEntry::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::SetFileInfo",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( fileInfo.fi_vers.gg91IsDummyVers() );

    // FIXME: only write to and lock page if there actually are
    // changes to its contents
    m_dataPage->LockPageForUpdate( taskId );

    SAPDB_MemCopyNoCheck( &PersistentIndexPointer()->fileTypeSet,
            &fileInfo.fi_type,
            sizeof(fileInfo.fi_type) );

    if ( fileInfo.fi_state.includes( f_not_accessible ) ) {
        this->SetFileState( Data_FileNotAccessible );
    }  else if ( fileInfo.fi_state.includes( f_bad ) ) {
        this->SetFileState( Data_FileBad );
    }  else if ( fileInfo.fi_state.includes( f_read_only ) ) {
        this->SetFileState( Data_FileReadOnly );
    } else {
        this->SetFileState( Data_FileOk );
    }

    if ( fileInfo.fi_root != this->GetRootPage() ) {
        this->SetRootPage( taskId, fileInfo.fi_root );
    }

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

void FileDir_IndexEntry::Detach(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_IndexEntry::Detach",
                             FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentIndexPointer()->parentFileNo.Invalidate();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

void FileDir_IndexEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::BuildFileId", FileDir_Trace, 5 );

    fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    fileId.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    fileId.fileUserRef_gg00().gg91SetNilRef ();
    fileId.fileVersion_gg00().gg91SetDummyVers();
    fileId.fileRoot_gg00()      = this->GetRootPage();
    fileId.fileHandling_gg00().clear();
    fileId.fileBdUse_gg00().clear();

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentIndexPointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );

    fileId.fileName_gg00().rawAssign( cgg_zero_fn );
    fileId.fileTfn_gg00().becomes( tfnMulti_egg00 );
    fileId.fileTfnNo_gg00()[ 0 ] = static_cast<unsigned char>(
        PersistentIndexPointer()->indexId );
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

void FileDir_IndexEntry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::FillPreparedFileId", FileDir_Trace, 5 );

    FileDir_Entry::FillPreparedFileId( fileId );

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentIndexPointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );
}

/**************************************************************************/

void
FileDir_IndexEntry::SetRecordsPacked(
    const RTE_TaskId taskId,
    const SAPDB_Bool recordsPacked)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::SetRecordsPacked", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( this->GetEntryCount( taskId ) == 0 );

    m_dataPage->LockPageForUpdate( taskId );

    if ( recordsPacked ) {
        PersistentIndexPointer()->indexAttributes |= iAttrRecordsPacked;
    } else {
        PersistentIndexPointer()->indexAttributes ^= iAttrRecordsPacked;
    }

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool
FileDir_IndexEntry::AreRecordsPacked() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::AreRecordsPacked", FileDir_Trace, 5 );

    return
        (PersistentIndexPointer()->indexAttributes & iAttrRecordsPacked)
        != 0;
}

/**************************************************************************/

void FileDir_IndexEntry::IncrementUsageCount(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::IncrementUsageCount", FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    if ( PersistentIndexPointer()->usageCount < (SAPDB_MAX_UINT4-1) ) {
        ++PersistentIndexPointer()->usageCount;
    }

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Int FileDir_IndexEntry::GetUsageCount() const
{
    if ( PersistentIndexPointer()->usageCount <= SAPDB_MAX_INT4 ) {
        return PersistentIndexPointer()->usageCount;
    } else {
        return SAPDB_MAX_INT4;
    }
}

/**************************************************************************/

void FileDir_IndexEntry::ResetUsageCount(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::ResetUsageCount", FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentIndexPointer()->usageCount = 0;

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_IndexEntry::FileDir_IndexEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_Entry(taskId, allocator, persistentEntry,
                    sizeof(PersistentIndexData), currentPersistentLayout,
                    curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_IndexEntry::FileDir_IndexEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    // contains all fields that have ever been in use by this entry
    struct MigrationHelpStruct {
        FileDir_FileNo parentFileNo;    ///< fileNo of table index is defined on
        SAPDB_UInt2    indexId;         ///< indexId
        SAPDB_Int2     fileVersion;     ///< fileVersion of index (usually dummy)
        SAPDB_UInt1    fileTypeSet;     ///< fileTypeSet of index
        SAPDB_UInt4    usageCount;      ///< number of times index was used
        SAPDB_Int1     indexAttributes; ///< attributes of index file
    };

    MigrationHelpStruct helpStruct;

    /*
     * add initial values for all fields that have been added at a
     * later date here
     */

    // index attributes
    helpStruct.indexAttributes = iAttrNone;

    SAPDB_Bool bPersistentDataParsed = false;
    SAPDB_Bool bNoMigration          = true;

    if ( currentPersistentLayout < fdelWithFileAttributes ) {

        // migrate to layout that contains index attributes
        bNoMigration = false;

        if ( ! bPersistentDataParsed ) {
            struct LayoutWithoutIndexAttributes
            {
                FileDir_FileNo parentFileNo;
                SAPDB_UInt2    indexId;
                SAPDB_Int2     fileVersion;
                SAPDB_Byte     fileTypeSet[4];
                SAPDB_UInt4    usageCount;
                SAPDB_Int4     filler;
            };

            LayoutWithoutIndexAttributes* oldLayout =
                reinterpret_cast<LayoutWithoutIndexAttributes*>(
                    curPersistentPos );

            helpStruct.parentFileNo = oldLayout->parentFileNo;
            helpStruct.indexId      = oldLayout->indexId;
            helpStruct.fileVersion  = oldLayout->fileVersion;
            // fileTypeSet needs explicit addressing because of byte swap
            helpStruct.fileTypeSet  =
                static_cast<SAPDB_UInt1>( oldLayout->fileTypeSet[0] );
            helpStruct.usageCount   = oldLayout->usageCount;

            bPersistentDataParsed = true;
            curPersistentPos     += sizeof( LayoutWithoutIndexAttributes );
        }

        helpStruct.indexAttributes = iAttrNone;
    }


    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentIndexPointer()->parentFileNo,
            curPersistentPos,
            sizeof( PersistentIndexData ) - sizeof( PersistentData ) );

        curPersistentPos +=
            sizeof( PersistentIndexData ) - sizeof( PersistentData );

    } else {

        PersistentIndexPointer()->parentFileNo    = helpStruct.parentFileNo;
        PersistentIndexPointer()->indexId         = helpStruct.indexId;
        PersistentIndexPointer()->fileTypeSet     = helpStruct.fileTypeSet;
        PersistentIndexPointer()->indexAttributes = helpStruct.indexAttributes;
        PersistentIndexPointer()->usageCount      = helpStruct.usageCount;
    }

    SetValid();

    m_newParentFileNo = PersistentIndexPointer()->parentFileNo;

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
