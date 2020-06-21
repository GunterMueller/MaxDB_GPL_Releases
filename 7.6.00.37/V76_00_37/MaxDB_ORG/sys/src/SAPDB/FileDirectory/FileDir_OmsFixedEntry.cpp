/*****************************************************************************/
/*!
 @file    FileDir_OmsFixedEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsFixedEntry.

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
#include "FileDirectory/FileDir_OmsFixedEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

// for Converter_Version:
#include "Converter/Converter_Types.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// Number of partitions that are connected to this entry.
const SAPDB_UInt2 c_zeroPartitionCount = 0;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

FileDir_OmsFixedEntry::FileDir_OmsFixedEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount)
    : FileDir_OmsEntry(taskId, allocator, sizeof(PersistentOmsFixedData),
                       fileNo, converterVersion, rootPageNo,
                       objBodySize, objPerPageCount, chainCount,
                       c_zeroPartitionCount)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsFixedEntry::FileDir_OmsFixedEntry",
                             FileDir_Trace, 5 );

    PersistentOmsFixedPointer()->fileType = this->GetFileType();

    SetValid();
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_OmsFixedEntry::FileDir_OmsFixedEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_OmsEntry(allocator, persistentEntry)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsFixedEntry::FileDir_OmsFixedEntry-fromPersistent",
        FileDir_Trace, 5 );

    if ( persistentEntry.GetLength() != sizeof( PersistentOmsFixedData ) ) {
        Msg_List errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_FILEDIR_INVALID_ENTRY_SIZE,
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__SIZE,
                     persistentEntry.GetLength() ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__FILETYPE,
                     Data_OmsFixedFile ),
            Msg_Arg( SDBMSGTAG_FILEDIR_INVALID_ENTRY_SIZE__LENGTH,
                     sizeof( PersistentOmsFixedData ) )
            );
        RTE_Crash( errMsg );
    }

    SetValid();
}

/**************************************************************************/

// TO BE REMOVED:
// FileDir_OmsFixedEntry::FileDir_OmsFixedEntry(
//     const RTE_TaskId         taskId,
//     SAPDBMem_IRawAllocator&  allocator,
//     const FileId             fileId,
//     const Converter_Version& converterVersion,
//     const Data_PageNo&       rootPageNo)
//     : FileDir_OmsEntry(allocator, 0, 0, 0)
// {
//     SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsFixedEntry::FileDir_OmsFixedEntry",
//                              FileDir_Trace, 5 );

//     if ( FileDir_Entry::Initialize() != FileDir_Okay ) {
//         return;
//     }

//     // no page locking as this should be part of restart
//     m_persistentData = reinterpret_cast<PersistentOmsFixedData*>(
//         FileDir_PageManager::Instance()->ReserveSpace(
//             taskId,
//             sizeof(PersistentOmsFixedData),
//             false,
//             m_dataPage) );

//     if ( m_persistentData == 0 ) {
//         return;
//     }

//     PersistentOmsFixedPointer()->header.converterVersion = converterVersion;

//     PersistentOmsFixedPointer()->fileType        = GetFileType();
//     PersistentOmsFixedPointer()->fileState       = Data_FileOk;
//     PersistentOmsFixedPointer()->fileId          = fileId;
//     PersistentOmsFixedPointer()->rootPage        = rootPageNo;
//     PersistentOmsFixedPointer()->entryCount      = 0;
//     PersistentOmsFixedPointer()->pageCount       = 1;

//     PersistentOmsFixedPointer()->header.hasBackref = false;

//     bd98OmsFixedFileGetRootValues(
//         PersistentOmsFixedPointer()->fileId,
//         PersistentOmsFixedPointer()->rootPage,
//         m_chainCount,
//         m_objBodySize,
//         m_objPerPageCount );

//     SetValid();
// }

/**************************************************************************/

Data_FileType FileDir_OmsFixedEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsFixedEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_OmsFixedFile;
}

/**************************************************************************/

FileDir_IOmsFixed* FileDir_OmsFixedEntry::AsOmsFixed()
{
    return static_cast<FileDir_IOmsFixed*>( this );
}

/**************************************************************************/

void FileDir_OmsFixedEntry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_OmsFixedEntry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );
    if ( !m_needsUpdateAfterSavepoint )
        return;

    FileDir_OmsEntry::UpdateAfterSavepoint( taskId, converterVersion );
}

/**************************************************************************/
// SAPDB_Bool FileDir_OmsFixedEntry::Verify()
// {
// }

/**************************************************************************/

FileDir_ReturnCode FileDir_OmsFixedEntry::Initialize()
{
    if ( (FileDir_Entry::Initialize() != FileDir_Okay) ||
         (m_persistentData == 0) ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_OmsFixedEntry::FileDir_OmsFixedEntry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
    : FileDir_OmsEntry(taskId, allocator, persistentEntry,
                       sizeof(PersistentOmsFixedData), currentPersistentLayout,
                       curPersistentPos)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_OmsFixedEntry::FileDir_OmsFixedEntry-migration",
        FileDir_Trace, 5 );

    if ( m_persistentData == 0 ) {
        return;
    }

    // nothing to do, FileDir_OmsEntry took care of everything

    SetValid();

    // free lock before removing old entry
    m_dataPage->UnlockPageForUpdate( taskId );

    persistentEntry.Remove( taskId );
}

/**************************************************************************/
