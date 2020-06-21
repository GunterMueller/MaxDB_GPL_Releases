/*****************************************************************************/
/*!
  @file         FileDir_Types.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class .

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

#ifndef FILEDIR_TYPES_HPP
#define FILEDIR_TYPES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief FileDirectory return codes.
 */
typedef enum
{
    FileDir_Okay = 0,
    FileDir_InitFailed,
    FileDir_AlreadyRestarted,
    FileDir_OutOfMemory,
    FileDir_ReadError,
    FileDir_WriteError,
    FileDir_FileNoNotFound,
    FileDir_AlreadyMarkedForDeletion,
    FileDir_FileNoExists,
    FileDir_FileTypeMismatch,

    FileDir_DuplicateIndex,
    FileDir_IndexExists,
    FileDir_IndexNotFound,
    FileDir_InvalidIndexNo,

    FileDir_DuplicateBlobColumn,

    FileDir_ConsistentViewCancelled,
    FileDir_PartitionExists,
    FileDir_PartitionNotFound,
    FileDir_FileNoInvalid,
    FileDir_SystemError

    // !!! update error text converter when changing this enum !!!
} FileDir_ReturnCode;

/**************************************************************************/

const SAPDB_ToStringClass SAPDB_ToString(const FileDir_ReturnCode rc);
// implemented in FileDir_GenericDirectory.cpp

/**************************************************************************/

#endif // FILEDIR_TYPES_HPP
