/*****************************************************************************/
/*!

  @file         FileDir_IShortColumn.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_IShortColumn.

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

#ifndef FILEDIR_ISHORTCOLUMN_HPP
#define FILEDIR_ISHORTCOLUMN_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class Converter_Version;
class Kernel_IndexDescription;
struct tbd_fileinfo;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_IShortColumn
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief Additional interface methods for indices.
 */
class FileDir_IShortColumn
{
public:
    /*!
     * @brief Destructor. Does nothing.
     */
    virtual ~FileDir_IShortColumn() {};

    /*!
     * @brief Returns the file id of the table this index was defined on.
     */
    virtual FileDir_FileNo GetParentFileNo() const = 0;

    /*!
     * @brief Fills fileInfo for this short column file.
     *
     * @copydoc FileDir_ShortColumnEntry::FillFileInfo
     */
    virtual void FillFileInfo(tbd_fileinfo& fileInfo) const = 0;

    /*!
     * @brief Sets the file info for this short column file.
     *
     * @copydoc FileDir_ShortColumnEntry::SetFileInfo
     */
    virtual void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo) = 0;
};

#endif // FILEDIR_ISHORTCOLUMN_HPP
