/*****************************************************************************/
/*!
 @file    FileDir_Index.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Index.

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

#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_IIndex.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class Kernel_IndexDescription;

/*************************************************************************/

FileDir_Index::FileDir_Index()
{}

/*************************************************************************/

FileDir_Index::FileDir_Index(FileDir_IndexEntry* fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_Index::FileDir_Index(
    const FileDir_Index& fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_FileNo FileDir_Index::GetParentFileNo() const
{
    return fileEntry->AsIndex()->GetParentFileNo();
}

/*************************************************************************/

SAPDB_UInt2 FileDir_Index::GetIndexId() const
{
    return fileEntry->AsIndex()->GetIndexId();
}

/*************************************************************************/

void FileDir_Index::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    fileEntry->AsIndex()->FillFileInfo( fileInfo );
}

/**************************************************************************/

void FileDir_Index::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    fileEntry->AsIndex()->SetFileInfo( taskId, fileInfo );
}

/**************************************************************************/

void
FileDir_Index::SetRecordsPacked(
    const RTE_TaskId taskId,
    const SAPDB_Bool recordsPacked)
{
    fileEntry->AsIndex()->SetRecordsPacked(
        taskId,
        recordsPacked );
}

/**************************************************************************/

SAPDB_Bool
FileDir_Index::AreRecordsPacked() const
{
    return fileEntry->AsIndex()->AreRecordsPacked();
}

/**************************************************************************/

void FileDir_Index::IncrementUsageCount(const RTE_TaskId taskId)
{
    fileEntry->AsIndex()->IncrementUsageCount( taskId );
}

/**************************************************************************/

SAPDB_Int FileDir_Index::GetUsageCount() const
{
    return fileEntry->AsIndex()->GetUsageCount();
}

/**************************************************************************/

void FileDir_Index::ResetUsageCount(const RTE_TaskId taskId)
{
    fileEntry->AsIndex()->ResetUsageCount( taskId );
}

/**************************************************************************/
