/*****************************************************************************/
/*!

  @file         FileDir_IteratorObj.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_IteratorObj.

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

#ifndef FILEDIR_ITERATOROBJ_HPP
#define FILEDIR_ITERATOROBJ_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Container/Container_Vector.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

class SAPDBMem_IRawAllocator;
class FileDir_IDirectory;
class FileDir_Directory;
class FileDir_GenericDirectory;
class FileDir_BaseEntry;
struct tgg00_TransContext;

/*!
 * @class FileDir_IteratorObj
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
class FileDir_IteratorObj
{
public:
    /*!
     * @brief Constructor.
     *
     * @param alloc [in] allocator
     * @param transContext [in] pointer to trans context this iterator
     * should operate in; if not 0 this context is used to determine
     * whether a file directory entry is visible for the iterator and
     * trans context may be modifier during a call to operator++(); if
     * transContext is 0, all file directory entries will be returned
     * regardless of their state
     */
    FileDir_IteratorObj(
        SAPDBMem_IRawAllocator& alloc,
        tgg00_TransContext*     transContext = 0 );

    /*!
     * @brief Constructor.
     *
     * @param alloc [in] allocator
     * @param transContext [in] pointer to trans context this iterator
     * should operate in; if not 0 this context is used to determine
     * whether a file directory entry is visible for the iterator and
     * trans context may be modifier during a call to operator++(); if
     * transContext is 0, all file directory entries will be returned
     * regardless of their state
     */
    FileDir_IteratorObj(
        SAPDBMem_IRawAllocator& alloc,
        tgg00_TransContext&     transContext );

    /// Destructor.
    ~FileDir_IteratorObj();

    /*!
     * @brief Sets the transContext for this iterator.
     */
    void SetTransContext(tgg00_TransContext* transContext);

    /*!
     * @brief Empty snapshot iterator is working on. Must be called
     *        before a second call to Initialize().
     */
    void Clear();

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
     * @warning This filter is not failsafe, as a file can still be
     *          deleted after it has been returned by the iterator.
     *
     * @return
     *  - true, if add of filter was successful
     *  - false, on error
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
     *        is currently defined on.
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
    // no copy-construction or assignment
    FileDir_IteratorObj(const FileDir_IteratorObj&);
    FileDir_IteratorObj& operator=(const FileDir_IteratorObj&);

    enum Status {
        stOk,
        stFileDirShuttingDown
    };

    typedef Container_Vector<FileDir_FileNo> FileNoList;
    typedef Container_Vector<Data_FileType> FileTypeFilterList;

    FileNoList m_fileNoList;
    FileNoList::Iterator m_IteratorObj;

    FileTypeFilterList m_fileTypeFilterList;
    bool               m_hideDeletedFilesFilter;

    FileDir_Generic           m_currentEntry;
    FileDir_GenericDirectory* m_FDir;
    tgg00_TransContext*       m_transContext;
    mutable SAPDB_UInt        m_refCounter;
    FileDir_Directory*        m_parentFDir;
    bool                      m_firstRun;

    /// Returns a pointer to the file dir entry the iterator is at.
    FileDir_BaseEntry* CurrentEntry() const;

    bool ReadFileDir(FileDir_GenericDirectory& fd);

    bool CheckPassesFilter(FileDir_BaseEntry* fe);

    SAPDBMem_IRawAllocator& GetAllocator() const;

    /*!
     * @brief Initializes iterator.
     *
     * @return true on success, false otherwise
     */
    bool Initialize(
        FileDir_GenericDirectory& fdir,
        FileDir_GenericDirectory* fdir1,
        FileDir_Directory*        parentFDir);

    void IncRefCounter() const;
    void DecRefCounter() const;
    SAPDB_UInt GetRefCounter() const;

    friend class FileDir_Directory;
    friend class FileDir_SharedDirectory;
    friend class FileDir_Iterator;
};

/*************************************************************************/

#endif // FILEDIR_ITERATOROBJ_HPP
