/*****************************************************************************/
/*!

  @file         FileDir_IIndex.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_IIndex.

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

#ifndef FILEDIR_IINDEX_HPP
#define FILEDIR_IINDEX_HPP

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
 * @class FileDir_IIndex
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief Additional interface methods for indices.
 */
class FileDir_IIndex
{
public:
    /*!
     * @brief Destructor. Does nothing.
     *
     * @copydoc FileDir_IndexEntry::~FileDir_IndexEntry
     */
    virtual ~FileDir_IIndex() {};

    /*!
     * @brief Returns the file id of the table this index was defined on.
     *
     * @copydoc FileDir_IndexEntry::GetParentFileNo
     */
    virtual FileDir_FileNo GetParentFileNo() const = 0;

    /*!
     * @brief Returns the id of this index as it is used by its parent.
     *
     * @copydoc FileDir_IndexEntry::GetIndexId
     */
    virtual SAPDB_UInt2 GetIndexId() const = 0;

    /*!
     * @brief Fills fileInfo for this index.
     *
     * @copydoc FileDir_IndexEntry::FillFileInfo
     */
    virtual void FillFileInfo(tbd_fileinfo& fileInfo) const = 0;

    /*!
     * @brief Sets the file info for this index.
     *
     * @copydoc FileDir_IndexEntry::SetFileInfo
     */
    virtual void SetFileInfo(
        const RTE_TaskId taskId,
        const tbd_fileinfo& fileInfo) = 0;


    /*!
     * @name Index attributes
     */
    /*@{*/

    /*!
     * @copydoc FileDir_IndexEntry::SetRecordsPacked
     */
    virtual void SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked) = 0;

    /*!
     * @copydoc FileDir_IndexEntry::AreRecordsPacked
     */
    virtual SAPDB_Bool AreRecordsPacked() const = 0;

    /*@}*/

    /*!
     * @name Index statistics
     */
    /*@{*/

    /*!
     * @brief Increases the usage counter of this index.
     *
     * @copydoc FileDir_IndexEntry::IncrementUsageCount
     */
    virtual void IncrementUsageCount(const RTE_TaskId taskId) = 0;

    /*!
     * @brief Returns the current usage count.
     *
     * @copydoc FileDir_IndexEntry::GetUsageCount
     */
    virtual SAPDB_Int GetUsageCount() const = 0;

    /*!
     * @brief Resets the usage counter of this index.
     *
     * @copydoc FileDir_IndexEntry::ResetUsageCount
     */
    virtual void ResetUsageCount(const RTE_TaskId taskId) = 0;
    /*@}*/
};

#endif // FILEDIR_IINDEX_HPP
