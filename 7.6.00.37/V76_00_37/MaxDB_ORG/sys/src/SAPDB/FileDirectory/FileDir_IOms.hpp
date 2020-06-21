/*****************************************************************************/
/*!
 @file    FileDir_IOms.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_IOms.

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

#ifndef FILEDIR_IOMS_HPP
#define FILEDIR_IOMS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_FileTypes.hpp"

#include "DataAccess/Data_Types.hpp"
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

class Converter_Version;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @class FileDir_IOms
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author MartinKi
 *
 * @brief Additional interface methods required for OMS entries.
 */
class FileDir_IOms
{
public:
    /*!
     * @brief Destructor. Does nothing.
     */
    virtual ~FileDir_IOms() {};

    /*!
     * @brief FIXME: Returns.
     */
    virtual SAPDB_UInt2 GetObjBodySize() const = 0;

    /*!
     * @brief FIXME: Returns.
     */
    virtual SAPDB_UInt4 GetObjPerPageCount() const = 0;

    /*!
     * @brief Modifies the free page counter by delta.
     */
    virtual void ModifyFreePageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta) = 0;

    /*!
     * @brief Returns the free page count.
     */
    virtual SAPDB_UInt4 GetFreePageCount(const RTE_TaskId taskId) const = 0;

    /*!
     * @brief Returns the current value of the page sequence counter.
     */
    virtual SAPDB_UInt8 GetCurrentPageSequenceNo() const = 0;

    /*!
     * @brief Increments the page sequence counter.
     */
    virtual void IncrementPageSequenceNo(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion) = 0;
};

#endif // FILEDIR_IOMS_HPP
