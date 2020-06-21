/*****************************************************************************/
/*!
  @file         SQLMan_TempFileCollection.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_TempFileCollection.

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

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Wrapper.hpp"

#include "SQLManager/SQLMan_LogicalFileNameGenerator.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_Types.hpp"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "DataAccess/Data_TempFileTypes.hpp"

#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "SAPDB/Messages/Msg_List.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd998.h"
#include "hgg01_3.h" // g01vtrace
#include "hgg04.h"
#include "ggg00.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// initial number concurrent temp files in list
const int c_initialListSize = 10;

/*****************************************************************************/

SQLMan_TempFileCollection::SQLMan_TempFileCollection(
    SAPDBMem_IRawAllocator& alloc)
    : m_allocator( alloc ),
      m_fileInfoList( alloc ),
      m_fileNoMapper( alloc ),
      m_procLevel( 0 )
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::SQLMan_TempFileCollection",
        SQLMan_Trace, 1 );
}

/*****************************************************************************/

SQLMan_TempFileCollection::~SQLMan_TempFileCollection()
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::~SQLMan_TempFileCollection",
        SQLMan_Trace, 1 );
}

/*****************************************************************************/

SAPDB_Bool SQLMan_TempFileCollection::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::Initialize", SQLMan_Trace, 1 );

    SAPDBERR_ASSERT_STATE( m_fileInfoList.GetSize() == 0 );
    bool ok = m_fileInfoList.Reserve( c_initialListSize );
    if ( !ok ) {
        return SAPDB_FALSE;
    }
    return SAPDB_TRUE;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::CreateFileFromFileName(
        SQLMan_TransContext&  trans,
        SQLMan_Surrogate&     logicalFileName,
        SQLMan_FileId&        logicalFileId,
        Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::CreateFileFromFileName", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "logicalFileId       : " <<
        SAPDBTrace_Hex(
            &logicalFileId.fileName_gg00(),
            sizeof(logicalFileId.fileName_gg00())) );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "logical surrogate   : " <<
        SAPDBTrace_Hex(
            logicalFileName,
            sizeof(logicalFileName)) );

    FileLevel level;
    FileSubLevel subLevel;
    FileIndex index;
    Data_TempFileTypes::FileType fileType;
    FileDir_FileNo               newFileNo;
    SQLMan_TempFileReturnCode    retCode;
    bool                         newEntry = true;

    if ( logicalFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        // we still have the logical file name in the file id
        if ( ! SQLMan_LogicalFileNameGenerator::ParseFileName(
                 logicalFileName, fileType, level, subLevel, index) ) {
            // FIXME: write error message
            trans.trError_gg00 = e_invalid_tablename;
            return SQLMan_TempInvalidLogicalName;
        }
        retCode =
            this->NewFile(
                trans, fileType, level, subLevel, index,
                newFileNo, message);
        if ( retCode != SQLMan_TempFileOkay ) {
            if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
                Kernel_VTrace()
                    << "SQL_TempFileCollection::CreateFileFromFileName duplicate filename"
                    << NewLine
                    << "  "
                    << SAPDBTrace_Hex(
                        &logicalFileId.fileName_gg00(),
                        sizeof(logicalFileId.fileName_gg00()));
            }
            trans.trError_gg00 = e_duplicate_filename;
            return retCode;
        }
    } else {
        // if we don't have the logical file name, the file has to be
        // already in our list:
        SAPDB_MemCopyNoCheck( &newFileNo, &logicalFileName, sizeof(newFileNo) );
        retCode =
            this->GetFileInfo( newFileNo, fileType, level, subLevel, index,
                               message );
        newEntry = false;
        SAPDBERR_ASSERT_STATE( retCode == SQLMan_TempFileOkay );
    }

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "procLevel: " << m_procLevel << NewLine <<
        "level    : " << level << NewLine <<
        "subLevel : " << subLevel << NewLine <<
        "index    : " << index );

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
        Kernel_VTrace()
            << "logical fn "
            << SAPDBTrace_Hex(
                &logicalFileId.fileName_gg00(),
                sizeof(logicalFileId.fileName_gg00()))
            << " -> physical fno " << newFileNo
            << ", procLevel " << this->GetFileTypeProcLevel( fileType ) ;
    }

    // make sure that everything behind fileTabId_gg00 is zero
    memset(
        reinterpret_cast<char*>(&logicalFileId.fileTabId_gg00())
        + sizeof(tgg00_Surrogate),
        0,
        sizeof(SQLMan_FileId)
        - FILE_ID_HEAD_MXGG00
        - sizeof( tgg00_Tfn )  // fileTfn
        - sizeof( tsp00_C1 )   // fileTfnNo
        - sizeof(tgg00_Surrogate) // fileTabId
        );
    // store file type in last byte of filename:
    logicalFileId.fileName_gg00()[ 15 ] =
        Data_TempFileTypes::MapFileTypeToTfnTemp( fileType );

    bd998CreateGroupedTempFile( trans, newFileNo, logicalFileId, fileType );

    if ( trans.trError_gg00 == e_ok ) {
        logicalFileId.fileRootCheck_gg00() =
            logicalFileId.fileRoot_gg00() ^ ROOT_CHECK_GG00;
    } else {
        if ( newEntry ) {
            RemoveFileEntry( fileType, level, subLevel, index, message );
            return SQLMan_TempFileCreationFailed;
        } else if ( trans.trError_gg00 == e_duplicate_filename ) {
            return SQLMan_DuplicateTempFile;
        } else {
            return SQLMan_TempFileCreationFailed;
        }
    }

    return SQLMan_TempFileOkay;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::FindFile(
    const Data_TempFileTypes::FileType fileType,
    const FileLevel       level,
    const FileSubLevel    subLevel,
    const FileIndex       indexNo,
    FileTypeInfo&         fileInfo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::FindFile", SQLMan_Trace, 1 );

    const SAPDB_Int procLevel    = this->GetFileTypeProcLevel( fileType );
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        if ( (it->m_fileType == fileType)
             && (it->m_procLevel == procLevel)
             && (it->m_level == level)
             && (it->m_subLevel == subLevel)
             && (it->m_index == indexNo)
            ) {
            fileInfo = *it;
            return SQLMan_TempFileOkay;
        }
    }

    return SQLMan_TempFileNotFound;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::GetFile(
    const Data_TempFileTypes::FileType fileType,
    const FileLevel       level,
    const FileSubLevel    subLevel,
    const FileIndex       indexNo,
    FileDir_FileNo&       fileNo,
    Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::GetFile", SQLMan_Trace, 1 );

    const SAPDB_Int procLevel    = this->GetFileTypeProcLevel( fileType );
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        if ( (it->m_fileType == fileType)
             && (it->m_procLevel == procLevel)
             && (it->m_level == level)
             && (it->m_subLevel == subLevel)
             && (it->m_index == indexNo)
            ) {
            fileNo = it->m_fileNo;
            break;
        }
    }

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "SQLMan_TempFileCollection: no entry for ("
        << Data_TempFileTypes::GetTypeName( fileType ) << ", "
        << procLevel << ", " << level << ", " << subLevel << ", "
        << indexNo)

    return fileNo.IsValid() ? SQLMan_TempFileOkay : SQLMan_TempFileNotFound;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::DestroyFileByFileNo(
    SQLMan_TransContext&  trans,
    FileDir_FileNo&       fileNo,
    Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::DestroyFileByFileNo", SQLMan_Trace, 1 );

    // find entry
    FileInfoList::Iterator it;
    if ( ! this->FindFileByFileNo( fileNo, it ) ) {
        // FIXME: write error message
        trans.trError_gg00 = e_file_not_found;
        return SQLMan_TempFileNotFound;
    }

    Data_TempFileTypes::FileType fileType = it->m_fileType;
    // delete file
    SQLMan_Surrogate fileSurrogate;
    SAPDB_MemCopyNoCheck( &fileSurrogate, &fileNo, sizeof(fileSurrogate) );
    bd998DestroyTempFile( trans, fileSurrogate );
    if ( trans.trError_gg00 != e_ok ) {
        return SQLMan_TempFileDestructionFailed;
    }

    // reset fileId to logical file name:
    SQLMan_LogicalFileNameGenerator::GeneratePreparedFileName(
        fileType,
        it->m_level,
        it->m_subLevel,
        it->m_index,
        reinterpret_cast<SQLMan_Surrogate&>(fileNo) );

    this->UnmapFile( fileNo, message ); // remove logical fileNo from mapper

    // remove entry from list
    return this->RemoveFileEntry(
        fileType, it->m_level, it->m_subLevel, it->m_index, message );
}

/*****************************************************************************/

bool SQLMan_TempFileCollection::FindFileByFileNo(
    const FileDir_FileNo&         fileNo,
    FileInfoList::Iterator&       it)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::FindFileByFileNo", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1, "looking up: " << fileNo );

    it                           = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        if ( it->m_fileNo == fileNo ) {
            return true;
        }
    }
    return false;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::DestroyAtLevel(
    SQLMan_TransContext&  trans,
    const Data_TempFileTypes::FileType fileType,
    const FileLevel       level,
    const FileSubLevel    subLevel,
    Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::DestroyAtLevel", SQLMan_Trace, 1 );

    SAPDBERR_ASSERT_STATE( level >= -1 );
    SAPDBERR_ASSERT_STATE( subLevel >= -1 );

    SAPDBERR_ASSERT_STATE( level == -1 ? (subLevel == -1) : true );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "destroy files of type '"
        << Data_TempFileTypes::GetTypeName( fileType )
        << "', procLevel = " << m_procLevel << ", starting at ("
        << level << ", " << subLevel << ")"
        );

    SQLMan_TempFileReturnCode rc = SQLMan_TempFileOkay;
    const SAPDB_Int procLevel    = this->GetFileTypeProcLevel( fileType );
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    while ( (it != m_fileInfoList.End()) && (rc == SQLMan_TempFileOkay)  ) {
        if ( ( it->m_fileType == fileType )
             && (it->m_procLevel == procLevel)
             && ( (level == -1) || (it->m_level == level) )
             && ( (subLevel == -1) || (it->m_subLevel == subLevel) ) ) {
            SAPDBTRACE_WRITELN(
                SQLMan_Trace, 1,
                "procLevel : " << it->m_procLevel
                << ", level : " << it->m_level
                << ", subLevel : " << it->m_subLevel
                << ", index : " << it->m_index );
            SQLMan_Surrogate fileSurrogate;
            SAPDB_MemCopyNoCheck( &fileSurrogate,
                                  &it->m_fileNo,
                                  sizeof(fileSurrogate) );
            bd998DestroyTempFile( trans, fileSurrogate );
            if ( trans.trError_gg00 != e_ok ) {
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
                    Kernel_VTrace()
                        << "SQL_TempFileCollection::DestroyAtLevel"
                        << " destruction of file "
                        << it->m_fileNo << " failed, trError = "
                        << trans.trError_gg00;
                    Kernel_VTrace()
                        << "t: " << it->m_fileType
                        << ", pl: " << it->m_procLevel
                        << ", l: " << it->m_level
                        << ", sl: " << it->m_subLevel
                        << ", i: " << it->m_index
                        << "; ignoring and going on";
                }
                trans.trError_gg00 = e_ok;
            }
            // remove logical fileNo from mapper
            this->UnmapFile(
                fileType, it->m_level, it->m_subLevel, it->m_index, message );

            m_fileInfoList.Delete( it );
            continue;
        }
        ++it;
    }

    return rc;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::DestroyAll(
    SQLMan_TransContext&  trans,
    Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::DestroyAll", SQLMan_Trace, 1 );

    SQLMan_TempFileReturnCode rc = SQLMan_TempFileOkay;
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    while ( (it != m_fileInfoList.End()) && (rc == SQLMan_TempFileOkay)  ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "type     : " << Data_TempFileTypes::GetTypeName(it->m_fileType)
            << NewLine <<
            "procLevel: " << it->m_procLevel << NewLine <<
            "level    : " << it->m_level << NewLine <<
            "subLevel : " << it->m_subLevel << NewLine <<
            "index    : " << it->m_index );
        SQLMan_Surrogate fileSurrogate;
        SAPDB_MemCopyNoCheck( &fileSurrogate,
                              &it->m_fileNo,
                              sizeof(fileSurrogate) );
        bd998DestroyTempFile( trans, fileSurrogate );
        if ( trans.trError_gg00 != e_ok ) {
            rc = SQLMan_TempFileDestructionFailed;
            SAPDBTRACE_WRITELN(
                SQLMan_Trace, 1,
                "ERROR: destruction of temp file failed, trError = "
                << trans.trError_gg00 << ". ignoring and going on");
            trans.trError_gg00 = e_ok;
            rc = SQLMan_TempFileOkay;
        } else {
            this->UnmapFile(
                it->m_fileType, it->m_level, it->m_subLevel, it->m_index,
                message );

            m_fileInfoList.Delete( it );
            continue;
        }
        ++it;
    }

    return rc;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::NewFile(
    const tgg00_TransContext& trans,
    const Data_TempFileTypes::FileType fileType,
    const FileLevel         level,
    const FileSubLevel      subLevel,
    const FileIndex         index,
    FileDir_FileNo&         fileNo,
    Msg_List&               message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::NewFile", SQLMan_Trace, 1 );

    fileNo = FileDir_IDirectory::SessionInstance( trans ).GetNewTempFileNo();

    FileTypeInfo fileInfo;
    if ( this->FindFile( fileType, level, subLevel, index, fileInfo )
         == SQLMan_TempFileOkay ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "duplicate: type     : "
            << Data_TempFileTypes::GetTypeName( fileType ) << NewLine <<
            "           procLevel: " << m_procLevel << NewLine <<
            "           level    : " << level << NewLine <<
            "           subLevel : " << subLevel << NewLine <<
            "           index    : " << index );
        return SQLMan_DuplicateTempFile;
    }

    if ( m_fileInfoList.IsFull() ) {
        if ( ! m_fileInfoList.Reserve( (m_fileInfoList.GetSize() + 1) * 2 ) ) {
            // FIXME: return error message
            return SQLMan_TempFileOutOfMemory;
        }
    }

    const SAPDB_Int procLevel = this->GetFileTypeProcLevel( fileType );

    m_fileInfoList.InsertEnd(
        FileTypeInfo( fileType, procLevel, level, subLevel, index, fileNo ) );

    return SQLMan_TempFileOkay;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::RemoveFileEntry(
    const Data_TempFileTypes::FileType fileType,
    const FileLevel       level,
    const FileSubLevel    subLevel,
    const FileIndex       index,
    Msg_List&             message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::RemoveFileEntry", SQLMan_Trace, 1 );

    const SAPDB_Int procLevel    = this->GetFileTypeProcLevel( fileType );
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        if ( (it->m_fileType == fileType)
             && (it->m_procLevel == procLevel)
             && (it->m_level == level)
             && (it->m_subLevel == subLevel)
             && (it->m_index == index)
            ) {
            m_fileInfoList.Delete( it );
            return SQLMan_TempFileOkay;
        }
    }

    // FIXME: add error message
    return SQLMan_TempFileNotFound;
}

/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::GetFileInfo(
    const FileDir_FileNo&         fileNo,
    Data_TempFileTypes::FileType& fileType,
    FileLevel&                    level,
    FileSubLevel&                 subLevel,
    FileIndex&                    indexNo,
    Msg_List&                     message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::GetFileInfo", SQLMan_Trace, 1 );

    FileInfoList::Iterator pos;
    if ( ! FindFileByFileNo( fileNo, pos ) ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "no file info found for fileno : " << fileNo );
            message.ClearMessageList();
        // FIXME: add error message
        return SQLMan_TempFileNotFound;
    }

    fileType = pos->m_fileType;
    level    = pos->m_level;
    subLevel = pos->m_subLevel;
    indexNo  = pos->m_index;

    return SQLMan_TempFileOkay;
}


/*****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::AddFileEntry(
    SQLMan_TransContext&    trans,
    const SQLMan_Surrogate& actualFileNo,
    const SQLMan_Surrogate& logicalFileNo,
    Msg_List&               message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::AddFileEntry", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "logicalFileNo       : "
        << SAPDBTrace_Hex(
            &logicalFileNo,
            sizeof(logicalFileNo))
        << " as file no         : "
        << SAPDBTrace_Hex(
            &actualFileNo,
            sizeof(actualFileNo)) );

    FileLevel level;
    FileSubLevel subLevel;
    FileIndex index;
    Data_TempFileTypes::FileType fileType;

    if ( ! SQLMan_LogicalFileNameGenerator::ParseFileName(
             logicalFileNo, fileType, level, subLevel, index) ) {
        // FIXME: write error message
        trans.trError_gg00 = e_invalid_tablename;
        return SQLMan_TempInvalidLogicalName;
    }

    FileTypeInfo fileInfo;
    if ( this->FindFile( fileType, level, subLevel, index, fileInfo )
         == SQLMan_TempFileOkay ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "duplicate: type     : "
            << Data_TempFileTypes::GetTypeName( fileType ) << NewLine <<
            "           procLevel: " << m_procLevel << NewLine <<
            "           level    : " << level << NewLine <<
            "           subLevel : " << subLevel << NewLine <<
            "           index    : " << index );
        return SQLMan_DuplicateTempFile;
    }

    if ( m_fileInfoList.IsFull() ) {
        if ( ! m_fileInfoList.Reserve( (m_fileInfoList.GetSize() + 1) * 2 ) ) {
            // FIXME: return error message
            return SQLMan_TempFileOutOfMemory;
        }
    }

    const SAPDB_Int procLevel = this->GetFileTypeProcLevel( fileType );

    m_fileInfoList.InsertEnd(
        FileTypeInfo(
            fileType, procLevel, level, subLevel, index, actualFileNo ) );

    return SQLMan_TempFileOkay;
}

/****************************************************************************/

void SQLMan_TempFileCollection::Dump(
    Kernel_Dump& dump)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::Dump", SQLMan_Trace, 1 );

    struct DumpEntry {
        SAPDB_Int1     fileType;
        SAPDB_Int1     procLevel;
        SAPDB_Int2     level;
        SAPDB_Int4     sublevel;
        SAPDB_Int4     index;
        SAPDB_Int4     filler;
        FileDir_FileNo fileNo;

        DumpEntry(
            const Data_TempFileTypes::FileType logicalFileType,
            const SAPDB_Int1     logicalProcLevel,
            const SAPDB_Int2     logicalLevel,
            const SAPDB_Int2     logicalSublevel,
            const SAPDB_Int4     logicalIndex,
            const FileDir_FileNo instanceFileNo)
            : fileType( static_cast<SAPDB_Int1>( logicalFileType ) ),
              procLevel( logicalProcLevel ),
              level( logicalLevel ),
              sublevel( logicalSublevel ),
              index( logicalIndex ),
              fileNo( instanceFileNo ),
              filler( 0 )
        {}
    };

    Kernel_DumpPage::Entry dumpPageEntry;
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    while ( it != itEnd ) {
        DumpEntry dumpEntry(
            it->m_fileType,
            it->m_procLevel,
            it->m_level,
            it->m_subLevel,
            it->m_index,
            it->m_fileNo );
        dumpPageEntry.Assign ( &dumpEntry, sizeof(dumpEntry) );

        dump.InsertEntry(
            Kernel_Dump::DmpSQLTempFileCollection,
            dumpPageEntry );

        ++it;
    }
}

/****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::MapFile(
    const Data_TempFileTypes::FileType tempFileType,
    const FileDir_FileNo&    fileNoToMap,
    FileDir_FileNo&          logicalFileNo,
    Msg_List&                message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::MapFile", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN( SQLMan_Trace, 1, "mapping fileNo: " << fileNoToMap );

    // check if file is already in mapper
    FileNoMap::Iterator it    = m_fileNoMapper.Begin();
    FileNoMap::Iterator itEnd = m_fileNoMapper.End();
    while ( it != itEnd ) {
        if ( (*it).keyFileNo == fileNoToMap ) {
            logicalFileNo = (*it).logicalFileNo;
            SAPDBTRACE_WRITELN(
                SQLMan_Trace, 1, "found in map  : " << logicalFileNo );
            return SQLMan_TempFileOkay;
        }

        ++it;
    }

    // fileNo is not yet in mapper, find an unused logical file name
    // for tempFileType
    SAPDB_Int2 index = 0;
    it    = m_fileNoMapper.Begin();
    itEnd = m_fileNoMapper.End();
    while ( it != itEnd ) {
        Data_TempFileTypes::FileType itFileType;
        SAPDB_Int usedIndex;
        SAPDB_Int dummy;

        SQLMan_LogicalFileNameGenerator::ParseFileName(
                (*it).logicalFileNo, itFileType, dummy, dummy, usedIndex );
        if ( itFileType == tempFileType ) {
            if ( index <= usedIndex ) {
                index = usedIndex + 1;
            }
        }
        ++it;
    }

    SQLMan_LogicalFileNameGenerator::GeneratePreparedFileName(
        tempFileType, 0, 0, index, logicalFileNo );

    SAPDBTRACE_WRITELN( SQLMan_Trace, 1, "adding to map: " << logicalFileNo );

    if ( m_fileNoMapper.IsFull() ) {
        if ( ! m_fileNoMapper.Reserve( m_fileNoMapper.GetSize() * 2 + 1 ) ) {
            return SQLMan_TempFileOutOfMemory;
        }
    }

    m_fileNoMapper.InsertEnd(
        FileNoMapEntry( fileNoToMap, logicalFileNo ) );

    return SQLMan_TempFileOkay;
}

/****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::MapFile(
    const Data_TempFileTypes::FileType tempFileType,
    const FileDir_FileNo&              fileNoToMap,
    SQLMan_FileId&                     logicalFileId,
    Msg_List&                          message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::MapFile-FileId", SQLMan_Trace, 1 );

    FileDir_FileNo logicalFileNo;
    SQLMan_TempFileReturnCode retCode =
        this->MapFile( tempFileType, fileNoToMap, logicalFileNo, message );

    SAPDB_MemCopyNoCheck( &(logicalFileId.fileTfnTemp_gg00()),
                          &logicalFileNo,
                          sizeof(logicalFileNo) );
    return retCode;
}

/****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::UnmapFile(
    const FileDir_FileNo&              logicalFileNo,
    Msg_List&                          message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::UnmapFile", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1, "remove from map: " << logicalFileNo );

    FileNoMap::Iterator it    = m_fileNoMapper.Begin();
    FileNoMap::Iterator itEnd = m_fileNoMapper.End();
    while ( it != itEnd ) {
        if ( (*it).logicalFileNo == logicalFileNo ) {
            SAPDBTRACE_WRITELN( SQLMan_Trace, 1, "  removed." );
            m_fileNoMapper.Delete( it );
            return SQLMan_TempFileOkay;
        }
        ++it;
    }
    return SQLMan_TempFileNotFound;
}

/****************************************************************************/

SQLMan_TempFileReturnCode SQLMan_TempFileCollection::UnmapFile(
    const Data_TempFileTypes::FileType tempFileType,
    const FileLevel                    level,
    const FileSubLevel                 sublevel,
    const FileIndex                    index,
    Msg_List&                          message)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_TempFileCollection::UnmapFile-1", SQLMan_Trace, 1 );

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "remove from map: " << NewLine
        << " tempFileType: " << Data_TempFileTypes::GetTypeName(tempFileType)
        << NewLine
        << " level       : " << level << NewLine
        << " sublevel    : " << sublevel << NewLine
        << " index       : " << index );

    FileNoMap::Iterator it    = m_fileNoMapper.Begin();
    FileNoMap::Iterator itEnd = m_fileNoMapper.End();
    while ( it != itEnd ) {
        Data_TempFileTypes::FileType itFileType;
        FileLevel    itLevel;
        FileSubLevel itSublevel;
        FileIndex    itIndex;

        SQLMan_LogicalFileNameGenerator::ParseFileName(
            (*it).logicalFileNo, itFileType, itLevel, itSublevel, itIndex );
        if ( ( itFileType== tempFileType)
             && ( itLevel == level )
             && ( itSublevel == sublevel )
             && ( itIndex == index ) ) {
            SAPDBTRACE_WRITELN( SQLMan_Trace, 1, "found in map" );
            m_fileNoMapper.Delete( it );
            return SQLMan_TempFileOkay;
        }
        ++it;
    }
    return SQLMan_TempFileNotFound;
}

/****************************************************************************/

void
SQLMan_TempFileCollection::IncrementProcLevel()
{
    m_procLevel++;

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
        Kernel_VTrace()
            << "SQL_TempFileCollection::IncrementProcLevel to "
            << m_procLevel;
    }
}

/****************************************************************************/

void
SQLMan_TempFileCollection::DecrementProcLevel()
{
#ifdef SAPDB_SLOW
    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        SAPDBERR_ASSERT_STATE(
            (it->m_procLevel < m_procLevel )
            ||
            (this->GetFileTypeProcLevel(it->m_fileType) < m_procLevel )
            );
    }
#endif

    m_procLevel--;

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
        Kernel_VTrace()
            << "SQL_TempFileCollection::DecrementProcLevel to "
            << m_procLevel;
    }
}

/****************************************************************************/

SAPDB_Bool
SQLMan_TempFileCollection::SetProcLevel(
    const SAPDB_Int newProcLevel)
{
    if ( m_procLevel == newProcLevel ) {
        return true;
    }

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrAk_gg00 ) {
        Kernel_VTrace()
            << "SQL_TempFileCollection::SetProcLevel from "
            << m_procLevel
            << " to " << m_procLevel;
    }

    m_procLevel = newProcLevel;

    FileInfoList::Iterator it    = m_fileInfoList.Begin();
    FileInfoList::Iterator itEnd = m_fileInfoList.End();
    for ( ; it != itEnd; ++it ) {
        SAPDBERR_ASSERT_STATE( it->m_procLevel <= m_procLevel );
        if ( it->m_procLevel > m_procLevel ) {
            return false;
        }
    }

    return true;
}

/****************************************************************************/

/*!
 * @brief Returns the current proc level depending on fileType.
 *
 * For some files the proc level must be ignored because they must be
 * visible in all proc levels. For these files the proc level is always 0.
 */
SAPDB_Int
SQLMan_TempFileCollection::GetFileTypeProcLevel(
    const Data_TempFileTypes::FileType fileType) const
{
    switch ( fileType ) {
        case Data_TempFileTypes::UserResult:
        case Data_TempFileTypes::TempTable:
        case Data_TempFileTypes::Pars:
        case Data_TempFileTypes::Into:
        case Data_TempFileTypes::Column:
        case Data_TempFileTypes::CacheRollback:
        case Data_TempFileTypes::TempLog:
            return 0;
        default:
            return m_procLevel;
    }
}

/****************************************************************************/
