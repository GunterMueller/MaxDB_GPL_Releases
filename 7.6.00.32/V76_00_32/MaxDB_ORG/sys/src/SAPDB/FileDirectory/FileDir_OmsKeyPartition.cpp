/*****************************************************************************/
/*!
 @file    FileDir_OmsKeyPartition.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_OmsKeyPartition.

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

#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_OmsKeyPartitionEntry.hpp"
#include "FileDirectory/FileDir_IOmsKeyPartition.hpp"
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

FileDir_OmsKeyPartition::FileDir_OmsKeyPartition()
{}

/*************************************************************************/

FileDir_OmsKeyPartition::FileDir_OmsKeyPartition(
    FileDir_OmsKeyPartitionEntry* fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_OmsKeyPartition::FileDir_OmsKeyPartition(
    const FileDir_OmsKeyPartition& fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_FileNo FileDir_OmsKeyPartition::GetParentFileNo() const
{
    return fileEntry->AsOmsKeyPartition()->GetParentFileNo();
}

/*************************************************************************/

SAPDB_UInt2 FileDir_OmsKeyPartition::GetPartitionNo() const
{
    return fileEntry->AsOmsKeyPartition()->GetPartitionNo();
}

/*************************************************************************/
