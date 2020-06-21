/*****************************************************************************/
/*!
 @file    FileDir_OmsVar.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsVar.

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

#include "FileDirectory/FileDir_OmsVar.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_IOmsVar.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "RunTime/RTE_Types.hpp"

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


/*************************************************************************/

FileDir_OmsVar::FileDir_OmsVar()
{}

/*************************************************************************/

FileDir_OmsVar::FileDir_OmsVar(FileDir_OmsVarEntry* fe)
    : FileDir_Oms(fe)
{}

/*************************************************************************/

FileDir_OmsVar::FileDir_OmsVar(
    const FileDir_OmsVar& fe)
    : FileDir_Oms(fe)
{}

/*************************************************************************/

Data_PageNo FileDir_OmsVar::GetContPartition(SAPDB_UInt2 partitionNo) const
{
    return fileEntry->AsOmsVar()->GetContPartition( partitionNo );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_OmsVar::AddContPartition(
    const Data_PageNo& rootPage,
    SAPDB_UInt2        partitionNo)
{
    return fileEntry->AsOmsVar()->AddContPartition( rootPage, partitionNo );
}

/*************************************************************************/

void FileDir_OmsVar::BuildContFileId(
        SAPDB_Int partitionNo, 
        tgg00_FileId& fileId) const
{
    return fileEntry->AsOmsVar()->BuildContFileId( partitionNo, fileId );
}
