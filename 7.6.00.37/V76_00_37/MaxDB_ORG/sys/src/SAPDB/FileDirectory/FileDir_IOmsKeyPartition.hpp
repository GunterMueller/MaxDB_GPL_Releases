/*****************************************************************************/
/*!

  @file         FileDir_IOmsKeyPartition.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_IOmsKeyPartition.

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

#ifndef FILEDIR_IOMSKEYPARTITION_HPP
#define FILEDIR_IOMSKEYPARTITION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_FileTypes.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

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

struct tbd_fileinfo;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_IOmsKeyPartition
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief Additional interface methods for Oms key partitions.
 */
class FileDir_IOmsKeyPartition
{
public:
    /*!
     * @brief Destructor. Does nothing.
     */
    virtual ~FileDir_IOmsKeyPartition() {};

    /*!
     * @brief Returns the file id of the table this index was defined on.
     */
    virtual FileDir_FileNo GetParentFileNo() const = 0;

    /*!
     * @brief Returns the id of this index as it is used by its parent.
     */
    virtual SAPDB_UInt2 GetPartitionNo() const = 0;
};

#endif // FILEDIR_IOMSKEYPARTITION_HPP
