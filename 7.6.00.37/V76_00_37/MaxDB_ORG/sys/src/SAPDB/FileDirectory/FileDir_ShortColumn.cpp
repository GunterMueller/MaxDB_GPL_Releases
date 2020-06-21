/*****************************************************************************/
/*!
 @file    FileDir_ShortColumn.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_ShortColumn.

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

#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_ShortColumnEntry.hpp"
#include "FileDirectory/FileDir_IShortColumn.hpp"
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

FileDir_ShortColumn::FileDir_ShortColumn()
{}

/*************************************************************************/

FileDir_ShortColumn::FileDir_ShortColumn(FileDir_ShortColumnEntry* fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_ShortColumn::FileDir_ShortColumn(
    const FileDir_ShortColumn& fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_FileNo FileDir_ShortColumn::GetParentFileNo() const
{
    return fileEntry->AsShortColumn()->GetParentFileNo();
}

/*************************************************************************/

void FileDir_ShortColumn::FillFileInfo(tbd_fileinfo& fileInfo) const
{
    fileEntry->AsShortColumn()->FillFileInfo( fileInfo );
}

/**************************************************************************/

void FileDir_ShortColumn::SetFileInfo(
    const RTE_TaskId taskId,
    const tbd_fileinfo& fileInfo)
{
    fileEntry->AsShortColumn()->SetFileInfo( taskId, fileInfo );
}

/**************************************************************************/
