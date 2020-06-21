/*****************************************************************************/
/*!
 @file    FileDir_OmsKeyPartitionEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsKeyPartitionEntry.

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
#include "FileDirectory/FileDir_OmsKeyPartitionEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Types.hpp"
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

FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry(
    const RTE_TaskId taskId,
    SAPDBMem_IRawAllocator& allocator,
    const FileDir_FileNo fileNo,
    const Converter_Version& converterVersion,
    const FileDir_FileNo parentFileNo,
    const SAPDB_UInt2 partitionNo,
    const Data_PageNo rootPage)
    : FileDir_Entry(taskId, allocator, sizeof(PersistentOmsKeyPartitionData),
                    fileNo, converterVersion, rootPage)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( partitionNo >  0 );

    if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentOmsKeyPartitionPointer()->fileType     = GetFileType();

    PersistentOmsKeyPartitionPointer()->parentFileNo = parentFileNo;
    PersistentOmsKeyPartitionPointer()->partitionNo  = partitionNo;
    PersistentOmsKeyPartitionPointer()->filler       = 0;
    PersistentOmsKeyPartitionPointer()->filler1      = 0;

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_Entry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry-fromPersistent",
        FileDir_Trace, 5 );

    if ( persistentEntry.GetLength() != sizeof( PersistentOmsKeyPartitionData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_OmsKeyPartitionFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentOmsKeyPartitionData ) )
            );
        RTE_Crash( errMsg );
    }

    SetValid();
}

/**************************************************************************/

FileDir_OmsKeyPartitionEntry::~FileDir_OmsKeyPartitionEntry()
{}

/**************************************************************************/

Data_FileType FileDir_OmsKeyPartitionEntry::GetFileType() const
{
    return Data_OmsKeyPartitionFile;
}

/**************************************************************************/

void FileDir_OmsKeyPartitionEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    if ( !m_needsUpdateAfterSavepoint )
        return;

    FileDir_Entry::UpdateAfterSavepoint( taskId, converterVersion );
    // FIXME: put all persistent entries that need to be updated here
}

/**************************************************************************/

FileDir_FileNo FileDir_OmsKeyPartitionEntry::GetParentFileNo() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyPartitionEntry::FileDir_GetParentFileNo",
        FileDir_Trace, 5 );

    FileDir_FileNo pfd = PersistentOmsKeyPartitionPointer()->parentFileNo;
    return pfd;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_OmsKeyPartitionEntry::GetPartitionNo() const
{
    SAPDB_UInt2 no = PersistentOmsKeyPartitionPointer()->partitionNo;

    return no;
}

/**************************************************************************/

void FileDir_OmsKeyPartitionEntry::Reassign(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& tableFileNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyPartitionEntry::FileDir_Reassign",
                             FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentOmsKeyPartitionPointer()->parentFileNo = tableFileNo;

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_IOmsKeyPartition* FileDir_OmsKeyPartitionEntry::AsOmsKeyPartition()
{
    return static_cast<FileDir_IOmsKeyPartition*>(this);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsKeyPartitionEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_OmsKeyPartitionEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Dump {
        FileDir_FileNo parentFileNo;
        SAPDB_UInt2    partitionNo;
        SAPDB_Int2     filler;
        SAPDB_Int4     filler1;
    };
    Dump dump;

    if ( !FileDir_Entry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - sizeof(dump))
          <= 0) ) {
        return false;
    }

    int pos = startPos;

    dump.parentFileNo = PersistentOmsKeyPartitionPointer()->parentFileNo;
    dump.partitionNo  = PersistentOmsKeyPartitionPointer()->partitionNo;
    dump.filler       = 0;
    dump.filler1      = 0;

    void* pointer = dumpBuffer.GetPointer( pos, sizeof(dump) );
    SAPDB_MemCopyNoCheck ( pointer, &dump, sizeof(dump) );
    pos += sizeof(dump);

    startPos = pos;
    return true;
}

/**************************************************************************/

void FileDir_OmsKeyPartitionEntry::Detach(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyPartitionEntry::Detach",
                             FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentOmsKeyPartitionPointer()->parentFileNo.Invalidate();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

void FileDir_OmsKeyPartitionEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyPartitionEntry::BuildFileId", FileDir_Trace, 5 );

    SAPDB_MemCopyNoCheck (&fileId, &(g01tabid.obj_hash), sizeof (fileId));
    fileId.fileTfn_gg00().becomes(tfnOmsInv_egg00);
    fileId.fileType_gg00() =
        tgg00_FiletypeSet::build(2, ftsConcurrent_egg00, ftsPerm_egg00);
    fileId.fileRoot_gg00()        = PersistentOmsKeyPartitionPointer()->rootPage;
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );
}

/**************************************************************************/

FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_Entry(taskId, allocator, persistentEntry,
                    sizeof(PersistentOmsKeyPartitionData),
                    currentPersistentLayout, curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyPartitionEntry::FileDir_OmsKeyPartitionEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    SAPDB_Bool bNoMigration = true;

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentOmsKeyPartitionPointer()->parentFileNo,
            curPersistentPos,
            sizeof( PersistentOmsKeyPartitionData ) - sizeof( PersistentData ) );

        curPersistentPos +=
            sizeof( PersistentOmsKeyPartitionData ) - sizeof( PersistentData );

    } else {
        // put update of new persistent structure here if necessary
    }

    SetValid();

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
