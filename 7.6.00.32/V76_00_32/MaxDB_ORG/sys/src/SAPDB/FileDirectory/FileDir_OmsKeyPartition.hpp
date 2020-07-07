/*****************************************************************************/
/*!

  @file         FileDir_OmsKeyPartition.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for smart point and interface class
                FileDir_OmsKeyPartition.

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

#ifndef FILEDIR_OMSKEYPARTITION_HPP
#define FILEDIR_OMSKEYPARTITION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IOmsKeyPartition.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class FileDir_OmsKeyPartitionEntry;
struct tbd_fileinfo;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_OmsKeyPartition
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory OMS key partition entries.
 */
class FileDir_OmsKeyPartition : public FileDir_Generic,
                                public FileDir_IOmsKeyPartition
{
public:
    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the file id of the table this OMS key partition was
     *        defined on.
     */
    FileDir_FileNo GetParentFileNo() const;

    /*!
     * @brief Returns the id of this OMS key partition as it is used by
     *        its parent.
     */
    SAPDB_UInt2 GetPartitionNo() const;

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_OmsKeyPartition();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_OmsKeyPartition or subclass.
     */
    FileDir_OmsKeyPartition(FileDir_OmsKeyPartitionEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_OmsKeyPartition(const FileDir_OmsKeyPartition& fe);
};

/*************************************************************************/

template <class T>
void FileDir_OmsKeyPartition::Assign(T& Ptr)
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

#endif // FILEDIR_OMSKEYPARTITION_HPP
