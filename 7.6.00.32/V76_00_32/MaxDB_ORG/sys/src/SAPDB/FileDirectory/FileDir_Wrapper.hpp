/*****************************************************************************/
/*!
  @file         FileDir_Wrapper.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Declarations for some of the file directory wrapper functions
                that need to be accessed from C++.

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

#ifndef FILEDIR_WRAPPER_HPP
#define FILEDIR_WRAPPER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_TempFileTypes.hpp"
#include "SAPDBCommon/SAPDB_Types.h"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class FileDir_FileNo;
class FileDir_Generic;

/*!
 * @brief Creates a temporary file with file type tempFileType.
 *
 * @param trans trans context
 * @param fileNo [in/out] if fileNo is invalid, get new fileNo,
 *        otherwise use given fileno
 * @param fileId [out] file id of created file
 * @param tempFileType [in] file type
 */
void  bd998CreateGroupedTempFile(
    tgg00_TransContext&   trans,
    FileDir_FileNo&       fileNo,
    tgg00_FileId&         fileId,
    const Data_TempFileTypes::FileType tempFileType);

#endif // FILEDIR_WRAPPER_HPP
