/*****************************************************************************/
/*!
 @file    FileDir_IOmsVar.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_IOmsVar.

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

#ifndef FILEDIR_IOMSVAR_HPP
#define FILEDIR_IOMSVAR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"

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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @class FileDir_IOmsVar
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author MartinKi
 *
 * @brief Additional interface methods required for OMS entries.
 */
class FileDir_IOmsVar
{
public:
    /*!
     * @brief Destructor. Does nothing.
     *
     * @copydoc FileDir_OmsVarEntry::~FileDir_OmsVarEntry
     */
    virtual ~FileDir_IOmsVar() {};

    /*!
     * @brief FIXME: Returns.
     *
     * @copydoc FileDir_OmsVar::GetContPartition
     */
    virtual Data_PageNo GetContPartition(SAPDB_UInt2 partitionNo) const = 0;

    /*!
     * @brief FIXME: Returns.
     *
     * @copydoc FileDir_OmsVar::AddContPartition
     */
    virtual FileDir_ReturnCode AddContPartition(
        const Data_PageNo& rootPage,
        SAPDB_UInt2        partitionNo) = 0;

    /*!
     * @brief Prepares fileId for partition partitionNo for use in bd
     *        procedures.
     *
     */
    virtual void BuildContFileId(
        SAPDB_Int partitionNo, 
        tgg00_FileId& fileId) const =0;
};

#endif // FILEDIR_IOMSVAR_HPP
