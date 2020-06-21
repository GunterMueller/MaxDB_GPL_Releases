/*****************************************************************************/
/*!
 @file    FileDir_SingleIndexEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_SingleIndexEntry.

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

#include "FileDirectory/FileDir_SingleIndexEntry.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "Converter/Converter_Version.hpp"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
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

FileDir_SingleIndexEntry::FileDir_SingleIndexEntry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const FileDir_FileNo     parentFileNo,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPageNo,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion)
    : FileDir_IndexEntry(taskId, allocator, fileNo, converterVersion,
                         parentFileNo, indexId, rootPageNo, fileTypeSet,
                         fileVersion)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SingleIndexEntry::FileDir_SingleIndexEntry",
        FileDir_Trace, 5 );

    if ( FileDir_IndexEntry::Initialize() != FileDir_Okay ) {
        return;
    }

    m_dataPage->LockPageForUpdate( taskId );

    PersistentIndexPointer()->fileType = Data_SingleIndexFile;

    SetValid();

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

FileDir_SingleIndexEntry::FileDir_SingleIndexEntry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& persistentEntry)
    : FileDir_IndexEntry(allocator, persistentEntry)
{
    SetValid();
}

/**************************************************************************/

FileDir_SingleIndexEntry::~FileDir_SingleIndexEntry()
{}

/**************************************************************************/

Data_FileType FileDir_SingleIndexEntry::GetFileType() const
{
    // this will return either Data_SingleIndexFile or Data_IndexFile
    // depending on whether the file type switch has already happened
    // or not
    return
        static_cast<Data_FileType>( PersistentIndexPointer()->fileType );
}

/**************************************************************************/

SAPDB_Bool FileDir_SingleIndexEntry::SwitchTypeToIndex(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SingleIndexEntry::SwitchTypeToIndex",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( this->GetFileType() == Data_SingleIndexFile );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentIndexPointer()->fileType = Data_IndexFile;

    m_dataPage->UnlockPageForUpdate( taskId );

    return true;
}

/**************************************************************************/

void FileDir_SingleIndexEntry::SetIndexId(
    RTE_TaskId  taskId,
    SAPDB_UInt2 indexId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_SingleIndexEntry::SetIndexId",
        FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    PersistentIndexPointer()->indexId = indexId;

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/
