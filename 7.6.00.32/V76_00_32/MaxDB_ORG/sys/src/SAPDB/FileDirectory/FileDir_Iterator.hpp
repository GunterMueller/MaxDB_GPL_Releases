/*****************************************************************************/
/*!

  @file         FileDir_Iterator.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_Iterator.

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

#ifndef FILEDIR_ITERATOR_HPP
#define FILEDIR_ITERATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_FileTypes.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

class FileDir_Generic;
class FileDir_BaseEntry;
class FileDir_IteratorObj;
class FileDir_IDirectory;
class SAPDBMem_IRawAllocator;

/*!
 * @class FileDir_Iterator
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief This class provides an iterator over entries of the
 *        file directory.
 *        <em>This iterator is not multi-thread safe!</em>
 *
 * The iterator has be initialised after construction. Use the
 * Get...FileDirIterator()-methods defined in class FileDir_IDirectory
 * for initialisation.
 */
class FileDir_Iterator
{
public:
    /*!
     * @brief Constructor.
     */
    FileDir_Iterator(SAPDBMem_IRawAllocator& allocator);

    /// Copy constructor.
    FileDir_Iterator(const FileDir_Iterator& it);

    /*!
     * @brief Destructor.
     */
    ~FileDir_Iterator();

    /// Assignment operator.
    FileDir_Iterator& operator=(const FileDir_Iterator& it);

    /*!
     * @brief Adds a file type to the file type filter of the iterator.
     *
     * If a file type has been added to the iterator, only files of
     * those types will be returned. If the filter does not contain
     * any file types, all files will be returned regardless of their
     * type.
     *
     * File types can be added to this filter before and after initialisation.
     *
     * @param fileType [in] files of this file type should be returned
     *                      by the iterator
     */
    SAPDB_Bool AddFileTypeToFilter(const Data_FileType fileType);

    /*!
     * @brief Adds a filter that will prevent deleted files from being
     * returned.
     *
     * @copydoc FileDir_IteratorObj::AddHideDeletedFilesFilter
     */
    SAPDB_Bool AddHideDeletedFilesFilter();

    /*!
     * @brief Resets all filters. However, this does not alter the
     *        contents of the snapshot the iterator is working on.
     */
    void ClearFilter();

    /*!
     * @brief Retrieve next available entry.
     *
     * @warning If a trans context has been given during
     *          initialisation, this might get modified within
     *          this method.
     *
     * @param p [out] smart pointer to entry; invalid if no entry was found
     *
     * @return true, if valid entry was found, false otherwise
     */
    SAPDB_Bool GetNext(FileDir_Generic& p);

    /*!
     * @brief Returns a pointer to the file directory this iterator
     *        is currently registered to.
     */
    FileDir_IDirectory* GetFileDirectory();

    /*!
     * @brief Detaches the iterator from the file directory where it
     * is registered
     *
     * GetNext() will always return false once the iterator has been detached.
     */
    void Detach();
private:
    SAPDBMem_IRawAllocator* m_allocator;
    FileDir_IteratorObj* m_iterator;

    FileDir_Iterator(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_IteratorObj* iterator);

    SAPDB_Bool CreateIteratorObj();

    FileDir_IteratorObj* GetIteratorObj() const;

    void DecRefCounter();

    friend class FileDir_Directory;
    friend class FileDir_SharedDirectory;
    friend class FileDir_Test;
};

/*************************************************************************/

#endif // FILEDIR_ITERATOR_HPP
