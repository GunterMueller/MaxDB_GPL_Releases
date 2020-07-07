/*****************************************************************************/
/*!
 @file    FileDir_Table.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Table.

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

#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_ITable.hpp"
#include "FileDirectory/FileDir_SysKeyCache.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "RunTime/RTE_Types.hpp"

#include "ggg00.h" // tgg00_Surrogate

class Converter_Version;

/*************************************************************************/

FileDir_Table::FileDir_Table()
{}

/*************************************************************************/

FileDir_Table::FileDir_Table( FileDir_TableEntry* fe )
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_Table::FileDir_Table(
    const FileDir_Table& fe )
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_ReturnCode FileDir_Table::AddBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    return fileEntry->AsTable()->AddBlobColumn(
        converterVersion,
        columnNumber);
}

/*************************************************************************/

FileDir_ReturnCode FileDir_Table::RemoveBlobColumn(
    const Converter_Version& converterVersion,
    const SAPDB_UInt2 columnNumber)
{
    return fileEntry->AsTable()->RemoveBlobColumn(
        converterVersion,
        columnNumber);
}

/*************************************************************************/

SAPDB_UInt2 FileDir_Table::GetBlobColCount() const
{
    return fileEntry->AsTable()->GetBlobColCount();
}

/*************************************************************************/

SAPDB_UInt2 FileDir_Table::GetMaxIndexId() const
{
    return fileEntry->AsTable()->GetMaxIndexId();
}

/*************************************************************************/

FileDir_FileNo FileDir_Table::GetIndexFileNo(SAPDB_UInt2 indexNo) const
{
    return fileEntry->AsTable()->GetIndexFileNo( indexNo );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_Table::AddIndex(
    const FileDir_FileNo& indexFileNo,
    const SAPDB_UInt2     indexId)
{
    return fileEntry->AsTable()->AddIndex( indexFileNo, indexId );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_Table::RemoveIndex(
    const FileDir_FileNo& indexFileNo)
{
    return fileEntry->AsTable()->RemoveIndex( indexFileNo );
}

/*************************************************************************/

void FileDir_Table::SetArchive(SAPDB_Bool isArchive)
{
    fileEntry->AsTable()->SetArchive( isArchive );
}

/**************************************************************************/

SAPDB_Bool FileDir_Table::IsArchive() const
{
    return fileEntry->AsTable()->IsArchive();
}

/**************************************************************************/

void
FileDir_Table::SetRecordsPacked(
    const RTE_TaskId taskId,
    const SAPDB_Bool recordsPacked)
{
    fileEntry->AsTable()->SetRecordsPacked(
        taskId,
        recordsPacked );
}

/**************************************************************************/

SAPDB_Bool
FileDir_Table::AreRecordsPacked() const
{
    return fileEntry->AsTable()->AreRecordsPacked();
}

/**************************************************************************/

SAPDB_Bool
FileDir_Table::IsDynamic() const
{
    return fileEntry->AsTable()->IsDynamic();
}

/**************************************************************************/

void FileDir_Table::SetShortColumnFileNo(
    const FileDir_FileNo shortColumnFileNo)
{
    fileEntry->AsTable()->SetShortColumnFileNo( shortColumnFileNo );
}

/**************************************************************************/

void FileDir_Table::GetShortColumnFileNo(
        FileDir_FileNo& shortColumnFileNo) const
{
    fileEntry->AsTable()->GetShortColumnFileNo( shortColumnFileNo );
}

/**************************************************************************/

FileDir_FileNo FileDir_Table::GetShortColumnFileNo() const
{
    return fileEntry->AsTable()->GetShortColumnFileNo();
}

/**************************************************************************/

SAPDB_Bool FileDir_Table::HasShortColumnFile() const
{
    return fileEntry->AsTable()->HasShortColumnFile();
}

/**************************************************************************/

SAPDB_Bool FileDir_Table::IsLobPageCounterInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Table::IsLobPageCounterInitialized",
        FileDir_Trace, 6 );

    return fileEntry->AsTable()->IsLobPageCounterInitialized();
}

/**************************************************************************/

SAPDB_UInt8 FileDir_Table::GetLobPageCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Table::GetLobPageCount",
        FileDir_Trace, 6 );

    return fileEntry->AsTable()->GetLobPageCount();
}

/**************************************************************************/

void FileDir_Table::ModifyLobPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Table::ModifyLobPageCount", FileDir_Trace, 5 );

    fileEntry->AsTable()->ModifyLobPageCount(
        taskId,
        converterVersion,
        delta );
}

/**************************************************************************/

void FileDir_Table::SetLobPageCount(
    const SAPDB_UInt8 lobPageCount,
    const SAPDB_Bool  setInitialized)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Table::SetLobPageCount", FileDir_Trace, 5 );

    fileEntry->AsTable()->SetLobPageCount( lobPageCount, setInitialized );
}

/*************************************************************************/

void
FileDir_Table::InitializeTableCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount,
    const SAPDB_UInt8 lobPageCount )
{
    fileEntry->AsTable()->InitializeTableCounters(
        taskId, leafPageCount, indexPageCount, entryCount, lobPageCount );
}

/**************************************************************************/

void FileDir_Table::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    fileEntry->AsTable()->FillFileInfo( fileInfo );
}

/**************************************************************************/

void FileDir_Table::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    fileEntry->AsTable()->SetFileInfo( taskId, fileInfo );
}

/**************************************************************************/

FileDir_Table::SysKeyRc
FileDir_Table::GetNewSysKey(tgg00_Surrogate& sysKey)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Table::GetNewSysKey",
        FileDir_Trace, 6 );

    FileDir_SysKeyCache& keyCache = FileDir_SharedDirectory::Instance()
        .GetSysKeyCache();

    if ( keyCache.GetNewSysKeyForFile( this->GetFileNo(), sysKey ) ) {
        return skOkay;
    } else {
        return skNotInitialized;
    }
}

/**************************************************************************/

FileDir_Table::SysKeyRc
FileDir_Table::InitAndGetNewSysKey(tgg00_Surrogate& sysKey)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Table::InitAndGetNewSysKey",
        FileDir_Trace, 6 );

    FileDir_SysKeyCache& keyCache = FileDir_SharedDirectory::Instance()
        .GetSysKeyCache();

    if ( keyCache.AddAndGetNewSysKeyForFile( this->GetFileNo(), sysKey ) ) {
        return skOkay;
    } else {
        return skOutOfMemory;
    }
}

/**************************************************************************/
