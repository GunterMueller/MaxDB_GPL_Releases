/*****************************************************************************/
/*!
 @file    FileDir_OmsKey.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsKey.

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

#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_OmsKeyEntry.hpp"
#include "FileDirectory/FileDir_IOmsKey.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

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

FileDir_OmsKey::FileDir_OmsKey()
{}

/*************************************************************************/

FileDir_OmsKey::FileDir_OmsKey(FileDir_OmsKeyEntry* fe)
    : FileDir_Oms(fe)
{}

/*************************************************************************/

FileDir_OmsKey::FileDir_OmsKey(
    const FileDir_OmsKey& fe)
    : FileDir_Oms(fe)
{}

/*************************************************************************/

SAPDB_UInt2 FileDir_OmsKey::GetKeyPartitionCount() const
{
    return fileEntry->AsOmsKey()->GetKeyPartitionCount();
}

/*************************************************************************/

FileDir_FileNo FileDir_OmsKey::GetKeyPartition(
    const SAPDB_UInt2 partitionNo) const
{
    return fileEntry->AsOmsKey()->GetKeyPartition( partitionNo );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_OmsKey::AddKeyPartition(
        const FileDir_FileNo keyPartitionFileNo,
        const SAPDB_UInt2    partitionNo)
{
    return fileEntry->AsOmsKey()->AddKeyPartition(
        keyPartitionFileNo, partitionNo );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_OmsKey::RemoveKeyPartition(
    const FileDir_FileNo keyPartitionFileNo)
{
    return fileEntry->AsOmsKey()->RemoveKeyPartition( keyPartitionFileNo );
}

/*************************************************************************/
