/*****************************************************************************/
/*!
 @file    FileDir_OmsKey.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for smart point and interface class
          FileDir_OmsKey.

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

#ifndef FILEDIR_OMSKEY_HPP
#define FILEDIR_OMSKEY_HPP

#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_IOmsKey.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class FileDir_OmsKeyEntry;

/*!
 * @class FileDir_OmsKey
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory OMS entries.
 */
class FileDir_OmsKey : public FileDir_Oms,
                       public FileDir_IOmsKey
{
public:
    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt2 GetKeyPartitionCount() const;

    /*!
     * @brief FIXME: Returns.
     */
    FileDir_FileNo GetKeyPartition(const SAPDB_UInt2 partitionNo) const;

    /*!
     * @copydoc FileDir_OmsKeyEntry::AddKeyPartition
     */
    FileDir_ReturnCode AddKeyPartition(
        const FileDir_FileNo keyPartitionFileNo,
        const SAPDB_UInt2    partitionNo);

    /*!
     * @copydoc FileDir_OmsKeyEntry::RemoveKeyPartition
     */
    FileDir_ReturnCode RemoveKeyPartition(
        const FileDir_FileNo keyPartitionFileNo);

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_OmsKey();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_OmsEntry or subclass.
     */
    FileDir_OmsKey(FileDir_OmsKeyEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_OmsKey(const FileDir_OmsKey& fe);
};

/*************************************************************************/

template <class T>
void FileDir_OmsKey::Assign(T& Ptr)
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

#endif // FILEDIR_OMSKEY_HPP
