/*****************************************************************************/
/*!
 @file    FileDir_Oms.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for smart point and interface class
          FileDir_Oms.

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

#ifndef FILEDIR_OMS_HPP
#define FILEDIR_OMS_HPP

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IOms.hpp"
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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class FileDir_OmsEntry;

/*!
 * @class FileDir_Oms
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory OMS entries.
 */
class FileDir_Oms : public FileDir_Generic,
                    public FileDir_IOms
{
public:
    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt2 GetObjBodySize() const;

    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt4 GetObjPerPageCount() const;


    /*!
     * @brief Actually part of FileDir_OmsKey, in here for performance reasons.
     */
    SAPDB_UInt2 GetKeyPartitionCount() const;

    /*!
     * @brief Actually part of FileDir_OmsKey, in here for performance reasons.
     */
    FileDir_FileNo GetKeyPartition(const SAPDB_UInt2 partitionNo) const;

    /*!
     * @brief Actually part of FileDir_OmsVar, in here for performance reasons.
     *
     * @copydoc FileDir_OmsVarEntry::GetContPartition
     */
    Data_PageNo GetContPartition(SAPDB_UInt2 partitionNo) const;

    /*!
     * @brief Modifies the free page counter by delta.
     */
    void ModifyFreePageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Returns the free page count.
     */
    SAPDB_UInt4 GetFreePageCount(const RTE_TaskId taskId) const;

    /*!
     * @brief Returns the current value of the page sequence counter.
     */
    SAPDB_UInt8 GetCurrentPageSequenceNo() const;

    /*!
     * @brief Increments the page sequence counter.
     */
    void IncrementPageSequenceNo(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion);

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_Oms();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_OmsEntry or subclass.
     */
    FileDir_Oms(FileDir_OmsEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_Oms(const FileDir_Oms& fe);
};

/*************************************************************************/

template <class T>
void FileDir_Oms::Assign(T& Ptr)
{
    /*
     * the following statement makes sure that down- and upcasts
     * via Assign() are typesafe; if you encounter a compile error
     * this probably means that you tried to Assign() a smart
     * pointer to unrelated entry types
     */
    T* test = static_cast<T*>(this); // check previous comment if you get a compile error here
    if ( Ptr.IsSecondaryPointer() ) {
        AssignSecondary( Ptr );
    } else {
        AssignEntry( Ptr.fileEntry );
    }
}

/*************************************************************************/

#endif // FILEDIR_OMS_HPP
