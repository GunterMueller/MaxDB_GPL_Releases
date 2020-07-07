/*****************************************************************************/
/*!
 @file    FileDir_OmsKeyEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsKeyEntry.

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
#include "FileDirectory/FileDir_OmsKeyEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"

#include "Converter/Converter_Version.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"

#include "heo51.h" // vgetpid():

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

FileDir_OmsKeyEntry::FileDir_OmsKeyEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    const SAPDB_UInt2        keyPartitionCount)
    : FileDir_OmsEntry(taskId, allocator,
                       sizeof(PersistentOmsKeyData)
                       + (keyPartitionCount-1) * sizeof(FileDir_FileNo),
                       fileNo, converterVersion, rootPageNo,
                       objBodySize, objPerPageCount, chainCount,
                       keyPartitionCount)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::FileDir_OmsKeyEntry",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( (keyPartitionCount == 1)
                           || (keyPartitionCount == 16) );

    PersistentOmsKeyPointer()->fileType = this->GetFileType();

    for (SAPDB_UInt2 i = 0; i < keyPartitionCount; ++i) {
        PersistentOmsKeyPointer()->keyPartitionFileNos[ i ].Invalidate();
    }

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_OmsKeyEntry::FileDir_OmsKeyEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry)
    : FileDir_OmsEntry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyEntry::FileDir_OmsKeyEntry-fromPersistent",
        FileDir_Trace, 5 );

    SetValid();
}

/**************************************************************************/

Data_FileType FileDir_OmsKeyEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_OmsKeyFile;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_OmsKeyEntry::GetKeyPartitionCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::GetKeyPartitionCount",
                             FileDir_Trace, 5 );

    return this->GetPartitionCounter();
}

/**************************************************************************/

FileDir_FileNo FileDir_OmsKeyEntry::GetKeyPartition(
    const SAPDB_UInt2 partitionNo) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::GetKeyPartition",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( partitionNo >= 1 );
    SAPDBERR_ASSERT_ARGUMENT( partitionNo <= this->GetPartitionCounter() );

    if ( (partitionNo < 1) || (partitionNo > this->GetPartitionCounter()) ) {
        return FileDir_FileNo(); // return invalid id
    }

    return PersistentOmsKeyPointer()->keyPartitionFileNos[ partitionNo - 1 ];
}

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsKeyEntry::AddKeyPartition(
    const FileDir_FileNo keyPartitionFileNo,
    const SAPDB_UInt2    partitionNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::AddKeyPartition",
                             FileDir_Trace, 5 );

    if ( (partitionNo < 1) || (partitionNo > this->GetPartitionCounter()) ) {
        return FileDir_PartitionNotFound;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );
    m_dataPage->LockPageForUpdate( taskId );

    PersistentOmsKeyPointer()->keyPartitionFileNos[ partitionNo-1 ] =
        keyPartitionFileNo;

    m_dataPage->UnlockPageForUpdate( taskId );

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsKeyEntry::RemoveKeyPartition(
    const FileDir_FileNo keyPartitionNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::RemoveKeyPartition",
                             FileDir_Trace, 5 );

    // no locking done

    if ( keyPartitionNo.IsInvalid() ) {
        return FileDir_FileNoNotFound;
    }

    const SAPDB_UInt2 partitionCounter = this->GetPartitionCounter();
    for ( SAPDB_UInt2 i = 0; i <= partitionCounter; ++i ) {
        if ( PersistentOmsKeyPointer()
             ->keyPartitionFileNos[ i ] == keyPartitionNo ) {
            RTE_TaskId taskId;
            vgetpid( taskId );
            m_dataPage->LockPageForUpdate( taskId );
            PersistentOmsKeyPointer()->keyPartitionFileNos[ i ].Invalidate();
            m_dataPage->UnlockPageForUpdate( taskId );

            return FileDir_Okay;
        }
    }

    return FileDir_FileNoNotFound;
}

/**************************************************************************/

void FileDir_OmsKeyEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsKeyEntry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );
    if ( !m_needsUpdateAfterSavepoint )
        return;

    FileDir_OmsEntry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/

FileDir_IOmsKey* FileDir_OmsKeyEntry::AsOmsKey()
{
    return static_cast<FileDir_IOmsKey*>( this );
}

/**************************************************************************/

// SAPDB_Bool FileDir_OmsKeyEntry::Verify()
// {
// }

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsKeyEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
        (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_OmsKeyEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Entry {
        FileDir_FileNo key;
    };

    Entry entry;

    int size = this->GetPartitionCounter() * sizeof(Entry);
    if ( !FileDir_OmsEntry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - size)
          <= 0) ) {
        return false;
    }

    void* pointer = 0;
    for (SAPDB_UInt2 i=1; i <= this->GetPartitionCounter(); ++i) {
        entry.key = this->GetKeyPartition( i );
        pointer   = dumpBuffer.GetPointer( startPos, sizeof(entry) );
        SAPDB_MemCopyNoCheck( pointer, &entry, sizeof(entry) );
        startPos += sizeof(entry);
    }

    return true;
}

/**************************************************************************/

void FileDir_OmsKeyEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyEntry::BuildFileId", FileDir_Trace, 5 );

    FileDir_OmsEntry::BuildFileId( fileId );

    fileId.fileNumObjKeyFiles_gg00() =
        static_cast<tsp00_Uint1>( this->GetPartitionCounter() );

    fileId.fileObjFileType_gg00().becomes( oftFixLenObjFile_egg00 );
}

/**************************************************************************/

FileDir_OmsKeyEntry::FileDir_OmsKeyEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_OmsEntry(taskId, allocator, persistentEntry,
                       persistentEntry.PersistentPointer()->header
                       .recordHeader.length, // FIXME
                       currentPersistentLayout,
                       curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsKeyEntry::FileDir_OmsKeyEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    SAPDB_Bool bNoMigration = true;

    if ( bNoMigration ) {

        SAPDB_MemCopyNoCheck(
            &PersistentOmsKeyPointer()->keyPartitionFileNos[ 0 ],
            curPersistentPos,
            sizeof( PersistentOmsKeyData ) - sizeof( PersistentOmsData ) );

        curPersistentPos +=
            sizeof( PersistentOmsKeyData ) - sizeof( PersistentOmsData );

        SAPDB_UInt keyPartitionCount = PersistentOmsKeyPointer()->partitionCounter - 1;

        if ( keyPartitionCount > 0 ) {
            SAPDBERR_ASSERT_STATE( keyPartitionCount == 15 );
            SAPDB_MemCopyNoCheck(
                &PersistentOmsKeyPointer()->keyPartitionFileNos[ 1 ],
                curPersistentPos,
                sizeof( FileDir_FileNo ) * keyPartitionCount );

            curPersistentPos +=
                sizeof( FileDir_FileNo ) * keyPartitionCount;
        }

    } else {
        // put update of new persistent structure here if necessary
    }

    SetValid();

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
