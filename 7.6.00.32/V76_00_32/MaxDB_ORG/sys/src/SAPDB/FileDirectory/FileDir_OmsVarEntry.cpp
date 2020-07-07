/*****************************************************************************/
/*!
 @file    FileDir_OmsVarEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsVarEntry.

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
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "Converter/Converter_Version.hpp"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_Crash.hpp"

#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"

#include "heo51.h"    // vgetpid():
#include "hbd01_1.h"  // b01niltree_id

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

FileDir_OmsVarEntry::FileDir_OmsVarEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount)
    : FileDir_OmsEntry(taskId, allocator, sizeof(PersistentOmsVarData),
                       fileNo, converterVersion, rootPageNo,
                       objBodySize, objPerPageCount, chainCount,
                       c_contPartitionCount)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsVarEntry::FileDir_OmsVarEntry",
                             FileDir_Trace, 5 );

    if ( FileDir_OmsEntry::Initialize() != FileDir_Okay ) {
        return;
    }

    PersistentOmsVarPointer()->fileType = this->GetFileType();
    PersistentOmsVarPointer()->filler1  = 0;

    Data_PageNo invalidPage;
    for (SAPDB_UInt2 i=0; i < c_contPartitionCount; ++i) {
        PersistentOmsVarPointer()->partObjBodySizes[ i ] = 0;
        PersistentOmsVarPointer()->contPartitionRootPages[ i ] = invalidPage;
    }

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_OmsVarEntry::FileDir_OmsVarEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_OmsEntry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsVarEntry::FileDir_OmsVarEntry-fromPersistent",
        FileDir_Trace, 5 );

    if ( persistentEntry.GetLength() != sizeof( PersistentOmsVarData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_OmsVarFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentOmsVarData ) )
            );
        RTE_Crash( errMsg );
    }

    SetValid();
}

/**************************************************************************/

Data_FileType FileDir_OmsVarEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsVarEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_OmsVarFile;
}

/**************************************************************************/

Data_PageNo FileDir_OmsVarEntry::GetContPartition(
    SAPDB_UInt2 partitionNo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsVarEntry::GetContPartition",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( partitionNo >= 1 );
    SAPDBERR_ASSERT_ARGUMENT( partitionNo <= c_contPartitionCount );

    if ( (partitionNo < 1) || (partitionNo > c_contPartitionCount) ) {
        return Data_PageNo(); // return invalid id
    }
    return Data_PageNo(
        PersistentOmsVarPointer()->contPartitionRootPages[ partitionNo-1 ] );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsVarEntry::AddContPartition(
    const Data_PageNo& rootPage,
    SAPDB_UInt2        partitionNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsVarEntry::AddContPartition",
                             FileDir_Trace, 5 );

    if ( (partitionNo < 1) || (partitionNo > c_contPartitionCount) ) {
        return FileDir_PartitionNotFound;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );
    m_dataPage->LockPageForUpdate( taskId );

    PersistentOmsVarPointer()->contPartitionRootPages[ partitionNo-1 ] =
        rootPage;

    m_dataPage->UnlockPageForUpdate( taskId );

    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_OmsVarEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsVarEntry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );
    if ( !m_needsUpdateAfterSavepoint )
        return;

    FileDir_OmsEntry::UpdateAfterSavepoint( taskId, converterVersion );
    // FIXME: put all persistent entries that need to be updated here
}

/**************************************************************************/

FileDir_IOmsVar* FileDir_OmsVarEntry::AsOmsVar()
{
    return static_cast<FileDir_IOmsVar*>( this );
}

/**************************************************************************/

// SAPDB_Bool FileDir_OmsVarEntry::Verify()
// {
// }

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsVarEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay)     ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_OmsVarEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    // HP compiler does not allow to access c_contPartitionCount from
    // within struct:
    const SAPDB_UInt2 c_partitionCount = c_contPartitionCount;
    struct Dump {
        SAPDB_Int4 contPartitionRootPages[ c_partitionCount ];
    };

    Dump dump;

    int size = sizeof(Dump);
    if ( !FileDir_OmsEntry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - size)
          <= 0) ) {
        return false;
    }

    for (SAPDB_UInt2 i=1; i <= c_contPartitionCount; ++i) {
        dump.contPartitionRootPages[ i-1 ] = this->GetContPartition( i );
    }

    void* pointer = dumpBuffer.GetPointer( startPos, sizeof(dump) );
    SAPDB_MemCopyNoCheck( pointer, &dump, sizeof(dump) );
    startPos += sizeof(dump);

    return true;
}

/**************************************************************************/

void FileDir_OmsVarEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsVarEntry::BuildFileId", FileDir_Trace, 5 );

    FileDir_OmsEntry::BuildFileId( fileId );

    fileId.fileObjFileType_gg00().becomes( oftVarLenObjFile_egg00 );
}

/**************************************************************************/

void FileDir_OmsVarEntry::BuildContFileId(SAPDB_Int partitionNo,
                                          tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsVarEntry::BuildContFileId", FileDir_Trace, 5 );

    fileId = b01niltree_id;
    fileId.fileType_gg00() = tgg00_FiletypeSet::build(
        3, ftsObject_egg00, ftsConcurrent_egg00, ftsPerm_egg00);
    fileId.fileTfn_gg00().becomes( tfnContObj_egg00 );
    fileId.fileObjFileType_gg00().becomes( oftVarLenObjFile_egg00 );
    fileId.fileContObjFileNo_gg00() =
        static_cast<tsp00_Uint1>( partitionNo );
    fileId.fileDirFileId_gg00().rawAssign(
        PersistentOmsVarPointer()->fileNo.GetCharArray() );
    if ( (partitionNo < 1) || (partitionNo > c_contPartitionCount) )
        fileId.fileRoot_gg00()        = Data_PageNo(); // return invalid id
    else
        fileId.fileRoot_gg00() = Data_PageNo(
        PersistentOmsVarPointer()->contPartitionRootPages[ partitionNo-1 ] );
};

/**************************************************************************/

FileDir_OmsVarEntry::FileDir_OmsVarEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_OmsEntry(taskId, allocator, persistentEntry,
                       sizeof(PersistentOmsVarData), currentPersistentLayout,
                       curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsVarEntry::FileDir_OmsVarEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    SAPDB_Bool bNoMigration = true;

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentOmsVarPointer()->partObjBodySizes[ 0 ],
            curPersistentPos,
            sizeof( PersistentOmsVarData ) - sizeof( PersistentOmsData ) );

        curPersistentPos +=
            sizeof( PersistentOmsVarData ) - sizeof( PersistentOmsData );

    } else {
        // put update of new persistent structure here if necessary
    }

    SetValid();

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
