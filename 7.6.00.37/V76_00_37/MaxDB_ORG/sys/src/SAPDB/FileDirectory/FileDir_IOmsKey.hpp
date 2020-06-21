/*****************************************************************************/
/*!
 @file    FileDir_IOmsKey.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_IOmsKey.

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

#ifndef FILEDIR_IOMSKEY_HPP
#define FILEDIR_IOMSKEY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

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
 * @class   FileDir_IOmsKey
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author  MartinKi
 *
 * @brief Additional interface methods required for OMS entries.
 */
class FileDir_IOmsKey
{
public:
    /*!
     * @brief Destructor. Does nothing.
     */
    virtual ~FileDir_IOmsKey() {};

    /*!
     * @copydoc FileDir_OmsKeyEntry::GetKeyPartitionCount
     */
    virtual SAPDB_UInt2 GetKeyPartitionCount() const = 0;

    /*!
     * @copydoc FileDir_OmsKeyEntry::GetKeyPartition
     */
    virtual FileDir_FileNo GetKeyPartition(
        const SAPDB_UInt2 partitionNo) const = 0;


    /*!
     * @copydoc FileDir_OmsKeyEntry::AddKeyPartition
     */
    virtual FileDir_ReturnCode AddKeyPartition(
        const FileDir_FileNo keyPartitionFileNo,
        const SAPDB_UInt2    partitionNo) = 0;

    /*!
     * @copydoc FileDir_OmsKeyEntry::RemoveKeyPartition
     */
    virtual FileDir_ReturnCode RemoveKeyPartition(
        const FileDir_FileNo keyPartitionFileNo) = 0;
};

#endif // FILEDIR_IOMSKEY_HPP
