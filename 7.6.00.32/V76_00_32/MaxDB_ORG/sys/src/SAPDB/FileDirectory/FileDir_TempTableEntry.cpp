/*****************************************************************************/
/*!
 @file    FileDir_TempTableEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_TempTableEntry.

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

#include "FileDirectory/FileDir_TempEntry.hpp"
#include "FileDirectory/FileDir_TempTableEntry.hpp"
#include "FileDirectory/FileDir_Types.hpp"

#include "Converter/Converter_Version.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd300.h" // bd300SetRootCheck
#include "gbd00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// Initial page count of a temp file.
const FileDir_TempEntry::PageCount  c_initialPageCount = 0;

/// Initial entry count of a temp file.
const FileDir_TempEntry::EntryCount c_initialEntryCount = 0;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

FileDir_TempTableEntry::FileDir_TempTableEntry(
    SAPDBMem_IRawAllocator&  allocator,
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet)

    : FileDir_TempEntry( allocator, fileNo, converterVersion, rootPage,
                         Data_TableFile, Data_FileOk,
                         c_initialEntryCount, c_initialPageCount ),
      m_indexCount( 0 ),
      m_indexList( allocator ),
      m_fileTypeSet( fileTypeSet )
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::FileDir_TempTableEntry",
                             FileDir_Trace, 5 );

    if ( FileDir_TempEntry::Initialize() != FileDir_Okay ) {
        return;
    }

    m_blobColCount = blobColCount;

    m_initialized = true;
    SetValid();
}

/**************************************************************************/

Data_FileType FileDir_TempTableEntry::GetFileType() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::GetFileType",
                             FileDir_Trace, 5 );
    return Data_TableFile;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_TempTableEntry::GetBlobColCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::GetBlobColCount",
                             FileDir_Trace, 5 );
    return m_blobColCount;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempTableEntry::AddBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::AddBlobColumn",
                             FileDir_Trace, 5 );

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempTableEntry::RemoveBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::RemoveBlobColumn",
                             FileDir_Trace, 5 );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "FileNo            : " << GetFileNo() << NewLine
        << "Blob column number: " << columnNumber << NewLine
        << "converter version : " << converterVersion );

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempTableEntry::AddIndex(
    const FileDir_FileNo& indexFileNo,
    const SAPDB_UInt2     indexId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::AddIndex",
                             FileDir_Trace, 5 );

    // FIXME: check if index is valid

    IndexList::Iterator it = m_indexList.Begin();
    unsigned i = 0;
    while ( it != m_indexList.End() ) {
        if (i > indexId) {
            break;
        }
        ++i;
        ++it;
    }
    if ( it != m_indexList.End() ) {
        m_indexList.Insert( it, indexFileNo );
    } else {
        m_indexList.InsertEnd( indexFileNo );
    }

    // DDL - no locking necessary
	++m_indexCount;
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempTableEntry::RemoveIndex(
    const FileDir_FileNo& indexFileNo)
{
    IndexList::Iterator it = m_indexList.Begin();
    unsigned i = 0;
    while ( it != m_indexList.End() ) {
        if ( *it == indexFileNo ) {
            m_indexList.Delete( it );
            break;
        }
        ++i;
        ++it;
    }
    if ( it == m_indexList.End() ) {
        return FileDir_IndexNotFound;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_TempTableEntry::GetMaxIndexId() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempTableEntry::GetIndexCount",
                             FileDir_Trace, 5 );

    return m_indexCount;
}

/**************************************************************************/

FileDir_FileNo FileDir_TempTableEntry::GetIndexFileNo(
    SAPDB_UInt2 indexNo) const
{
    return FileDir_FileNo();
}

/**************************************************************************/

FileDir_ITable* FileDir_TempTableEntry::AsTable()
{
    return static_cast<FileDir_ITable*>(this);
}

/**************************************************************************/

// SAPDB_Bool FileDir_TempTableEntry::Verify()
// {
//     // check if length fits to blob col count

//     // check if indices exist
// }

/**************************************************************************/

FileDir_ReturnCode FileDir_TempTableEntry::Initialize()
{
    if ( !m_initialized ) {
        return FileDir_OutOfMemory;
    }
    return FileDir_TempEntry::Initialize();
}

/**************************************************************************/
/**************************************************************************/

void FileDir_TempTableEntry::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    fileInfo.fi_type = m_fileTypeSet;

    fileInfo.fi_state.clear();

    if (  Data_FileBad == m_fileState ) {
        fileInfo.fi_state.addElement( f_bad );
    }
    if (  Data_FileReadOnly == m_fileState ) {
        fileInfo.fi_state.addElement( f_read_only );
    }
    if (  Data_FileNotAccessible == m_fileState ) {
        fileInfo.fi_state.addElement( f_not_accessible );
    }

    fileInfo.fi_fill1           = false;
    fileInfo.fi_descr.clear();
    fileInfo.fi_root            = m_rootPage;
    fileInfo.fi_vers            = m_fileVersion;
    fileInfo.fi_col_cnt         = 0;
    fileInfo.fi_user_ref.gg91SetNilRef ();
    fileInfo.fi_var_col_cnt_pos = cgg_nil_varcol_cnt_off;
}

/**************************************************************************/

void FileDir_TempTableEntry::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    SAPDBERR_ASSERT_STATE( fileInfo.fi_type.includes( ftsTemp_egg00 ) );
    SAPDBERR_ASSERT_STATE( fileInfo.fi_type.includes( ftsConcurrent_egg00 ) );
    SAPDBERR_ASSERT_STATE( !fileInfo.fi_type.includes( ftsArchive_egg00 ) );

    if ( fileInfo.fi_state.includes( f_not_accessible ) ) {
        m_fileState = Data_FileNotAccessible;
    } else {
        SAPDBERR_ASSERT_STATE( false );
    }

    m_rootPage    = fileInfo.fi_root;
    m_fileVersion = fileInfo.fi_vers;
}

/**************************************************************************/

void FileDir_TempTableEntry::SetArchive(SAPDB_Bool isArchive)
{
    SAPDBERR_ASSERT_STATE( false );
}

/**************************************************************************/

SAPDB_Bool FileDir_TempTableEntry::IsArchive() const
{
    return false;
}

/**************************************************************************/

void FileDir_TempTableEntry::SetShortColumnFileNo(
        const FileDir_FileNo shortColumnFileNo)
{}

/**************************************************************************/

void FileDir_TempTableEntry::GetShortColumnFileNo(
    FileDir_FileNo& shortColumnFileNo) const
{
    shortColumnFileNo.Invalidate();
}

/**************************************************************************/

FileDir_FileNo FileDir_TempTableEntry::GetShortColumnFileNo() const
{
    return FileDir_FileNo();
}

/**************************************************************************/

SAPDB_Bool FileDir_TempTableEntry::HasShortColumnFile() const
{
    return false;
}

/**************************************************************************/

SAPDB_Bool FileDir_TempTableEntry::IsLobPageCounterInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TableEntry::IsLobPageCounterInitialized",
        FileDir_Trace, 6 );

    return false;
}

/**************************************************************************/

SAPDB_UInt8 FileDir_TempTableEntry::GetLobPageCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::GetLobPageCount",
        FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( false );
    return 0;
}

/**************************************************************************/

void FileDir_TempTableEntry::ModifyLobPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::ModifyLobPageCount",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( false );
}

/**************************************************************************/

void FileDir_TempTableEntry::SetLobPageCount(
    const SAPDB_UInt8 lobPageCount,
    const SAPDB_Bool  setInitialized)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::SetLobPageCount",
        FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( false );
}

/**************************************************************************/

void
FileDir_TempTableEntry::InitializeTableCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount,
    const SAPDB_UInt8 lobPageCount)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::InitializeTableCounters", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( lobPageCount == 0 );

    this->InitializeBasicCounters(
        taskId, leafPageCount, indexPageCount, entryCount );

}

/**************************************************************************/

void FileDir_TempTableEntry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::BuildFileId", FileDir_Trace, 5 );

    fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    fileId.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    fileId.fileUserRef_gg00().gg91SetNilRef ();
    fileId.fileVersion_gg00().gg91SetDummyVers();
    fileId.fileRoot_gg00()      = m_rootPage;
    fileId.fileHandling_gg00().clear();
    fileId.fileBdUse_gg00().clear();

    fileId.fileType_gg00() = m_fileTypeSet;

    fileId.fileName_gg00().rawAssign( cgg_zero_fn );
    fileId.fileTfn_gg00().becomes( tfnTemp_egg00 );
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

void FileDir_TempTableEntry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempTableEntry::FillPreparedFileId", FileDir_Trace, 5 );

    fileId.fileRoot_gg00()      = m_rootPage;
    fileId.fileType_gg00()      = m_fileTypeSet;

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

SAPDB_Bool FileDir_TempTableEntry::FillDumpEntry(
    const RTE_TaskId   taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int&         startPos)
{
    struct Dump {
        SAPDB_UInt2 blobColCount;
        SAPDB_Int2  fileVersion;
        tgg00_FiletypeSet fileTypeSet;
        SAPDB_Bool  filler;
        SAPDB_Int2  indexCount;
        SAPDB_Int8  lobPageCount;
        SAPDB_Int4  lobPageCountDelta;
        SAPDB_Int4  filler1;
        FileDir_FileNo shortColFileNo;
    };
    Dump dump;

    SAPDB_UInt size = sizeof(dump) + m_indexCount * sizeof(FileDir_FileNo);
    if ( !FileDir_TempEntry::FillDumpEntry( taskId, dumpBuffer, startPos ) ||
         (static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - size)
          <= 0) ) {
        return false;
    }

    int pos = startPos;

    dump.blobColCount      = m_blobColCount;
    SAPDB_MemCopyNoCheck( &dump.fileVersion, &m_fileVersion, sizeof(SAPDB_Int2) );
    dump.fileTypeSet       = m_fileTypeSet;
    dump.filler            = 0;
    dump.indexCount        = static_cast<SAPDB_Int2>( m_indexCount );
    dump.lobPageCount      = 0;
    dump.lobPageCountDelta = 0;
    dump.filler1           = 0;
    dump.shortColFileNo.Invalidate();

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

void
FileDir_TempTableEntry::SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked)
{
}

/**************************************************************************/

SAPDB_Bool
FileDir_TempTableEntry::AreRecordsPacked() const
{
    return false;
}

/**************************************************************************/

SAPDB_Bool
FileDir_TempTableEntry::IsDynamic() const
{
    return false;
}

/**************************************************************************/
