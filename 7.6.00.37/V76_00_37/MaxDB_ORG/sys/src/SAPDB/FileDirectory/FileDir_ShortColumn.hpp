/*****************************************************************************/
/*!

  @file         FileDir_ShortColumn.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for smart point and interface class
                FileDir_ShortColumn.

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

#ifndef FILEDIR_SHORTCOLUMN_HPP
#define FILEDIR_SHORTCOLUMN_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IShortColumn.hpp"
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

class FileDir_ShortColumnEntry;
class Kernel_IndexDescription;
struct tbd_fileinfo;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_ShortColumn
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory index entries.
 */
class FileDir_ShortColumn : public FileDir_Generic,
                            public FileDir_IShortColumn
{
public:
    /*!
     * @brief Returns the file id of the table this index was defined on.
     */
    FileDir_FileNo GetParentFileNo() const;

    /*!
     * @copydoc FileDir_ShortColumnEntry::SetFileInfo
     */
    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    /*!
     * @copydoc FileDir_ShortColumnEntry::SetFileInfo
     */
    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_ShortColumn();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_ShortColumn or subclass.
     */
    FileDir_ShortColumn(FileDir_ShortColumnEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_ShortColumn(const FileDir_ShortColumn& fe);
};

/*************************************************************************/

template <class T>
void FileDir_ShortColumn::Assign(T& Ptr)
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

#endif // FILEDIR_SHORTCOLUMN_HPP
