/*****************************************************************************/
/*!
 @file    FileDir_ShortColumnEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_ShortColumnEntry.

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
#include "FileDirectory/FileDir_ShortColumnEntry.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"

#include "DataAccess/Data_FileTypes.hpp"

#include "Converter/Converter_Version.hpp"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "hbd300.h" // bd300SetRootCheck

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

FileDir_ShortColumnEntry::FileDir_ShortColumnEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const FileDir_FileNo&    parentFileNo,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion)
    : FileDir_Entry(taskId, allocator, sizeof(PersistentShortColumnData),
                    fileNo, converterVersion, rootPage)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_ShortColumnEntry::FileDir_ShortColumnEntry",
                             FileDir_Trace, 5 );

    if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentShortColumnPointer()->fileType          = GetFileType();
    PersistentShortColumnPointer()->parentFileNo      = parentFileNo;
    m_newParentFileNo                                 = parentFileNo;
    ;
    SAPDB_MemCopyNoCheck( &PersistentShortColumnPointer()->fileTypeSet,
            &fileTypeSet,
            sizeof(fileTypeSet) );
    SAPDB_MemCopyNoCheck( &PersistentShortColumnPointer()->fileVersion,
            &fileVersion, sizeof(SAPDB_Int2) );

    SetValid();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_ShortColumnEntry::FileDir_ShortColumnEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_Entry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_ShortColumnEntry::FileDir_ShortColumnEntry-fromPersistent",
                             FileDir_Trace, 5 );

    if ( persistentEntry.GetLength() != sizeof( PersistentShortColumnData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_ShortColumnFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentShortColumnData ) )
            );
        RTE_Crash( errMsg );
    }

    m_newParentFileNo = PersistentShortColumnPointer()->parentFileNo;
    SetValid();
}

/**************************************************************************/

FileDir_ShortColumnEntry::~FileDir_ShortColumnEntry()
{}

/**************************************************************************/

Data_FileType FileDir_ShortColumnEntry::GetFileType() const
{
    return Data_ShortColumnFile;
}

/**************************************************************************/

void FileDir_ShortColumnEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    this->Reassign( taskId, m_newParentFileNo );

    FileDir_Entry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/

FileDir_FileNo FileDir_ShortColumnEntry::GetParentFileNo() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_ShortColumnEntry::FileDir_GetParentFileNo",
                             FileDir_Trace, 5 );

    return m_newParentFileNo;
}

/**************************************************************************/

void FileDir_ShortColumnEntry::Reassign(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& tableFileNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_ShortColumnEntry::FileDir_Reassign",
                             FileDir_Trace, 5 );

    if ( g01vtrace.vtrAny_gg00 ) {
        Kernel_VTrace() << "FDir_ShortColumn: " << this->GetFileNo()
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
                Kernel_VTrace() << "FDir_ShortColumn: " << this->GetFileNo()
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
    PersistentShortColumnPointer()->parentFileNo = tableFileNo;
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_IShortColumn* FileDir_ShortColumnEntry::AsShortColumn()
{
    return static_cast<FileDir_IShortColumn*>(this);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_ShortColumnEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_ShortColumnEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Dump {
        FileDir_FileNo parentFileNo;
    };
    Dump dump;

    if ( !FileDir_Entry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - sizeof(dump))
          <= 0) ) {
        return false;
    }

    int pos = startPos;

    dump.parentFileNo = PersistentShortColumnPointer()->parentFileNo;

    void* pointer = dumpBuffer.GetPointer( pos, sizeof(dump) );
    SAPDB_MemCopyNoCheck ( pointer, &dump, sizeof(dump) );
    pos += sizeof(dump);

    startPos = pos;
    return true;
}

/**************************************************************************/

void FileDir_ShortColumnEntry::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    SAPDBERR_ASSERT_ARGUMENT( sizeof(fileInfo.fi_type)
                               <= sizeof(SAPDB_Int4) );
    fileInfo.fi_type.clear();
    SAPDB_MemCopyNoCheck( &fileInfo.fi_type,
            &PersistentShortColumnPointer()->fileTypeSet,
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
            &PersistentShortColumnPointer()->fileVersion,
            sizeof(SAPDB_Int2) );
    fileInfo.fi_col_cnt         = 0;
    fileInfo.fi_user_ref.gg91SetNilRef ();
    fileInfo.fi_var_col_cnt_pos = cgg_nil_varcol_cnt_off;
}

/**************************************************************************/

void FileDir_ShortColumnEntry::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    // FIXME: only write to and lock page if there actually are
    // changes to its contents
    m_dataPage->LockPageForUpdate( taskId );

    SAPDB_MemCopyNoCheck( &PersistentShortColumnPointer()->fileTypeSet,
            &fileInfo.fi_type,
            sizeof(fileInfo.fi_type) );

    if ( fileInfo.fi_state.includes( f_not_accessible ) ) {
        PersistentShortColumnPointer()->fileState = Data_FileNotAccessible;
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

    SAPDB_MemCopyNoCheck( &PersistentShortColumnPointer()->fileVersion,
            &fileInfo.fi_vers, sizeof(SAPDB_Int2) );
}

/**************************************************************************/

void FileDir_ShortColumnEntry::Detach(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_ShortColumnEntry::Detach",
                             FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentShortColumnPointer()->parentFileNo.Invalidate();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

void FileDir_ShortColumnEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_ShortColumnEntry::BuildFileId", FileDir_Trace, 5 );

    fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    fileId.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    fileId.fileUserRef_gg00().gg91SetNilRef ();
    fileId.fileVersion_gg00().gg91SetDummyVers();
    fileId.fileRoot_gg00()      = this->GetRootPage();
    fileId.fileHandling_gg00().clear();
    fileId.fileBdUse_gg00().clear();

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentShortColumnPointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );

    fileId.fileName_gg00().rawAssign( cgg_zero_fn );
    fileId.fileTfn_gg00().becomes( tfnShortScol_egg00 );
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

void FileDir_ShortColumnEntry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_ShortColumnEntry::FillPreparedFileId", FileDir_Trace, 5 );

    FileDir_Entry::FillPreparedFileId( fileId );

    SAPDB_MemCopyNoCheck(
        &fileId.fileType_gg00(),
        &PersistentShortColumnPointer()->fileTypeSet,
        sizeof(tgg00_FiletypeSet) );
}

/**************************************************************************/

FileDir_ShortColumnEntry::FileDir_ShortColumnEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_Entry(taskId, allocator, persistentEntry,
                    sizeof(PersistentShortColumnData),
                    currentPersistentLayout, curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_ShortColumnEntry::FileDir_ShortColumnEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    SAPDB_Bool bNoMigration = true;

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentShortColumnPointer()->parentFileNo,
            curPersistentPos,
            sizeof( PersistentShortColumnData ) - sizeof( PersistentData ) );

        curPersistentPos +=
            sizeof( PersistentShortColumnData ) - sizeof( PersistentData );

    } else {
        // put update of new persistent structure here if necessary
    }

    SetValid();

    // init online delta state variables
    m_newParentFileNo = PersistentShortColumnPointer()->parentFileNo;

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
