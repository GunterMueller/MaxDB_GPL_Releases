/*****************************************************************************/
/*!
  @file         SQLMan_DiagnoseDropFile.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_DiagnoseDropFile.

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

#ifndef SQLMAN_DIAGNOSEDROPFILE_HPP
#define SQLMAN_DIAGNOSEDROPFILE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SQLMan_Context;
class FileDir_FileNo;
class Msg_List;

/*!
 * @class  SQLMan_DiagnoseDropFile
 * @author MartinKi
 *
 * @brief Handles the 'Diagnose Drop File surrogate' command.
 *
 * Before dropping the file, it will be verified that no catalog
 * objects exist that reference it.
 */
class SQLMan_DiagnoseDropFile
{
public:
    enum ReturnCode {
        DropOk,
        FileNotFound,
        CatalogObjectsExist
    };

    static ReturnCode Execute(
        SQLMan_Context&       sqlContext,
        const FileDir_FileNo& fileNo,
        Msg_List&             errMsg);

private:
    /// Object cannot be instantiated.
    SQLMan_DiagnoseDropFile() {};
};

#endif // SQLMAN_DIAGNOSEDROPFILE_HPP
