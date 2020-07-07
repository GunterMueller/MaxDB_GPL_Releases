/*****************************************************************************/
/*!
 @file    FileDir_Oms.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Oms.

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

#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_IOms.hpp"
#include "FileDirectory/FileDir_IOmsVar.hpp"
#include "FileDirectory/FileDir_IOmsKey.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
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

FileDir_Oms::FileDir_Oms()
{}

/*************************************************************************/

FileDir_Oms::FileDir_Oms(FileDir_OmsEntry* fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

FileDir_Oms::FileDir_Oms(
    const FileDir_Oms& fe)
    : FileDir_Generic(fe)
{}

/*************************************************************************/

SAPDB_UInt2 FileDir_Oms::GetObjBodySize() const
{
    return fileEntry->AsOms()->GetObjBodySize();
}

/*************************************************************************/

SAPDB_UInt4 FileDir_Oms::GetObjPerPageCount() const
{
    return fileEntry->AsOms()->GetObjPerPageCount();
}


/*************************************************************************/

void FileDir_Oms::ModifyFreePageCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4 delta)
{
    fileEntry->AsOms()->ModifyFreePageCount(
        taskId,
        converterVersion,
        delta);
}

/*************************************************************************/

SAPDB_UInt4 FileDir_Oms::GetFreePageCount(
    const RTE_TaskId taskId) const
{
    return fileEntry->AsOms()->GetFreePageCount( taskId );
}

/*************************************************************************/

void FileDir_Oms::IncrementPageSequenceNo(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion)
{
    fileEntry->AsOms()->IncrementPageSequenceNo(
        taskId, converterVersion );
}

/*************************************************************************/

SAPDB_UInt8 FileDir_Oms::GetCurrentPageSequenceNo() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Oms::GetCurrentPageSequenceNo",
                             FileDir_Trace, 5 );

    return fileEntry->AsOms()->GetCurrentPageSequenceNo();
}

/*************************************************************************/
/*************************************************************************/

SAPDB_UInt2 FileDir_Oms::GetKeyPartitionCount() const
{
    return fileEntry->AsOmsKey()->GetKeyPartitionCount();
}

/*************************************************************************/

FileDir_FileNo FileDir_Oms::GetKeyPartition(
    const SAPDB_UInt2 partitionNo) const
{
    return fileEntry->AsOmsKey()->GetKeyPartition( partitionNo );
}

/*************************************************************************/

Data_PageNo FileDir_Oms::GetContPartition(SAPDB_UInt2 partitionNo) const
{
    return fileEntry->AsOmsVar()->GetContPartition( partitionNo );
};

/*************************************************************************/
