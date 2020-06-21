/*****************************************************************************/
/*!

  @file         FileDir_BaseEntry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_BaseEntry.

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

#ifndef FILEDIR_BASEENTRY_HPP
#define FILEDIR_BASEENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"

// for Converter_Version:
#include "Converter/Converter_Version.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/**************************************************************************/

class FileDir_ITable;
class FileDir_IIndex;
class FileDir_IShortColumn;
class FileDir_IOms;
class FileDir_IOmsKey;
class FileDir_IOmsKeyPartition;
class FileDir_IOmsFixed;
class FileDir_IOmsVar;
class FileDir_Test;

class Kernel_Dump;
class SAPDBMem_IRawAllocator;

class SAPDBFields_Field;
struct tgg00_FileId;

/**************************************************************************/

/*!
 * @class FileDir_BaseEntry
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * This class defines the basic interface for all file
 * directory entries. It cannot be instantiated.
 *
 * Some methods that can be shared between persistent and temporary
 * entries are implemented.
 */
class FileDir_BaseEntry {
     /*!
      * @brief Frees all resources for this entry.
      *
      * All allocated memory for this entry (except for the object
      * itself) will be freed and the persistent data marked for
      * deletion.
      */
    virtual void Remove(const RTE_TaskId taskId) = 0;


public:
    /// Typedef for entry count.
    typedef SAPDB_UInt8 EntryCount;

    /// Typedef for page count.
    typedef SAPDB_UInt4 PageCount;

    /*!
     * @brief Returns the file id of this entry.
     */
    virtual FileDir_FileNo GetFileNo() const = 0;

    /*!
     * @brief Returns the file type of this entry.
     */
    virtual Data_FileType GetFileType() const = 0;

    /*!
     * @brief Returns the file state of this entry.
     *
     * @return file state of entry.
     */
    virtual Data_FileState GetFileState() const = 0;

    /*!
     * @brief Set the file state of this entry.
     *
     * @param newFileState [in] the new file state of this entry
     */
    virtual void SetFileState(const Data_FileState newFileState) = 0;

    /*!
     * @brief Marks the file as deleted.
     */
    virtual FileDir_ReturnCode MarkFileDeleted(
        const tgg91_TransNo& transNo) = 0;

    /*!
     * @brief Removes the deleted mark from the file.
     */
    virtual FileDir_ReturnCode RemoveFileDeletedMark() = 0;

    /*!
     * @brief Returns the root page of this entry.
     */
    virtual Data_PageNo GetRootPage() const = 0;

    /*!
     * @brief Sets the root page of this entry.
     */
    virtual void SetRootPage(
        const RTE_TaskId taskId,
        const Data_PageNo& rootPage) = 0;

     /*!
      * @brief Sets the leaf page count used on disk for this entry's data.
      */
    virtual void SetLeafPageCount(
        const SAPDB_UInt4 pageCount,
        const SAPDB_Bool  setInitialized = true) = 0;

     /*!
      * @brief Sets the index (non-leaf) page count used on disk for
      *        this entry's data.
      */
    virtual void SetIndexPageCount(const SAPDB_UInt4 pageCount) = 0;

     /*!
      * @brief Sets the entry count of this entry.
      */
    virtual void SetEntryCount(const SAPDB_UInt8 entryCount) = 0;

    /*!
     * @brief Returns true if the page and entry counter values are
     * initialised and can be used, false otherwise.
     *
     * @return true if counter values have been initialised and can be
     * used, false otherwise
     */
    virtual SAPDB_Bool AreCountersInitialized() const = 0;

    /*!
     * @brief Initializes the statistics counters with a given set of
     *        values.
     *
     * @param taskId         [in] id of current task
     * @param leafPageCount  [in] new leaf page count
     * @param indexPageCount [in] new leaf page count
     * @param entryCount     [in] new entry count
     */
    virtual void InitializeBasicCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount) = 0;

    /*!
     * @brief Returns the leaf page count used on disk for this entry's data.
     *
     * @param taskId [in] id of current task
     */
    virtual SAPDB_UInt4 GetLeafPageCount(const RTE_TaskId taskId) const = 0;

     /*!
      * @brief Returns the index (non-leaf ) page count used on disk
      *        for this entry's data.
      */
    virtual SAPDB_UInt4 GetIndexPageCount(const RTE_TaskId taskId) const = 0;

    /*!
     * @brief Returns the entry count of this entry.
     */
    virtual SAPDB_UInt8 GetEntryCount(const RTE_TaskId taskId) const = 0;

    /*!
     * @brief Returns true if the entry is already marked for
     *        deletion and no write operations are allowed on its
     *        content.
     */
    virtual SAPDB_Bool IsReadOnly() const = 0;

    /*!
     * @brief Adds delta pages to count of tree-leaves of
     *        this entry.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] the converterVersion the pages
     *                              were added in
     * @param delta            [in] number of pages added/removed
     */
    virtual void ModifyLeafPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta) = 0;

    /*!
     * @brief Adds delta pages to count of index (non-tree) leaves of
     *        this entry.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] the converterVersion the pages
     *                              were added in
     * @param delta            [in] number of pages added/removed
     */
    virtual void ModifyIndexPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta) = 0;

    /*!
     * @brief Adds delta entries to entry count of this entry.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] the converterVersion the pages
     *                              were added in
     * @param delta            [in] number of entries added/removed
     */
    virtual void ModifyEntryCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int8        delta) = 0;

    /*!
     * @brief Increase the cache-hit counter by one.
     */
    virtual void IncrementCacheHits();

    /*!
     * @brief Increase the cache-miss counter by one.
     */
    virtual void IncrementCacheMisses();

    /*!
     * @brief Returns the cache hitrate based on the cache-hit/-miss
     *        counters in percent.
     *
     * If both hit and miss counters are 0, a hitrate of 100 is returned.
     */
    virtual SAPDB_Real8 GetCacheHitrate() const;

    /*!
     * @brief Returns true if this entry is persistent.
     */
    virtual SAPDB_Bool IsPersistent() const = 0;


    /*!
     * @brief Sets the comment for this entry.
     *
     * The comment will be output along with trace information. It is
     * only temparary and will not be written to disc.
     */
    virtual void SetComment(const char* commentPtr);

    /*!
     * @brief Returns a pointer to the comment for this entry.
     *
     * @return a pointer to the comment for this entry or 0 if no
     *         comment text is set.
     */
    virtual const char* GetComment() const;

    /// Destructor.
    virtual ~FileDir_BaseEntry();

    /// Returns allocator this entry was created with.
    SAPDBMem_IRawAllocator& GetAllocator();

    /// Returns the number of references to this object held.
    SAPDB_Int GetRefCount() const;

    /// Type definition for cache hit/miss counters.
    typedef SAPDB_Int8 CacheAccessCounter;

    /*!
     * @brief Must be called after construction, returns true if entry
     *        is properly initialized.
     */
    virtual FileDir_ReturnCode Initialize();

    /*!
     * @brief Returns the delete stamp of the entry.
     */
    virtual void GetDeleteStamp(tgg91_TransNo& deleteStamp) const = 0;

    /*!
     * @brief Prepares fileId for this file entry for use in bd
     *        procedures. fileId will be completely overwritten.
     */
    virtual void BuildFileId(tgg00_FileId& fileId) const = 0;

    /*!
     * @brief Adds the file root page and file type to an already
     * prepared file id. fileVersion_gg00 will <em>not</em> be changed.
     */
    virtual void FillPreparedFileId(tgg00_FileId& fileId) const = 0;

    /*!
     * @brief Returns the file version of the entry. If the entry has
     * no file version associated with it, the dummy file version is
     * returned.
     */
    virtual void GetFileVersion(tgg91_FileVersion& fileVersion) const;

    /*!
     * @name File attributes
     */
    /*@{*/

    /*!
     * Sets whether pages should be stored in clusters.
     *
     * @param taskId [in] task id of running task
     * @param pagesClustered [in] pages should be clustered (if true),
     * pages need not be clustered (if false)
     */
    virtual void SetPagesClustered(
        const RTE_TaskId taskId,
        const SAPDB_Bool pagesClustered) = 0;

    /*!
     * @return true, if pages should be clustered on disk.
     */
    virtual SAPDB_Bool ArePagesClustered() const = 0;

    /*@}*/
protected:
    /// The node pointer for the file directory. Has been moved in
    /// here mainly to avoid having to have two separate allocations
    /// for entry and node.
    mutable FileDir_Node m_node;

    /*!
     * Points to a C-String containing a comment describing the type
     * of entry. This comment is not persistent and will not be
     * written to disc, but will be output during tracing.
     */
    const char* m_commentPtr;

    /// Allocator this entry should use and which it was created with.
    SAPDBMem_IRawAllocator& m_allocator;

    // the cache hit and miss counters are not yet in use, so comment
    // them out to save memory
    //     CacheAccessCounter m_cacheHitCounter;  ///< Counter for cache hits.
    //     CacheAccessCounter m_cacheMissCounter; ///< Counter for cache misses.

    SAPDB_Int  m_refCounter; ///< Reference counter.

    /// Constructor.
    FileDir_BaseEntry(SAPDBMem_IRawAllocator& allocator);

    /*!
     * @name Reference counting
     */
    /*@{*/
    void incRefCounter();
    SAPDB_Int decRefCounter();
    /*@}*/

    /*!
     * @brief Returns a pointer to the hash node that is part of this entry.
     */
    FileDir_Node* GetNodePtr() const;

    /*!
     * @brief Update persistent entry parts after a savepoint has finished.
     */
    virtual void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion) = 0;

    /*!
     * @brief Fill dumpBuffer with dump information for entry.
     *
     * @returns
     * - True, if dump information was entered successfully
     * - False, if dump information did not fit into dumpBuffer
     */
    virtual SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos) = 0;

    /*!
     * @return the converter version of the last DDL statement on the entry,
     *         used by FileDir_Test
     */
    virtual Converter_Version GetEntryConverterVersion() = 0;

    /*!
     * @brief Marks this entry as invalid on the persistent data
     *        page. Does not care about locking.
     *
     * @warning It is the caller's responsibility to lock and unlock
     *          the data page for update.
     */
    virtual void SetInvalid() = 0;

    /*!
     * @brief Marks this entry for deletion during restart or as soon
     *        as no references to it exist anymore.
     */
    virtual void SetLazyDelete(const RTE_TaskId taskId) = 0;

    /*!
     * @brief Returns true if this entry is marked for deletion during
     *        restart or as soon as no references to it exist anymore.
     */
    virtual SAPDB_Bool IsLazyDeleteSet() = 0;

    /// Returns file directory table entry interface for this entry.
    virtual FileDir_ITable*           AsTable();
    /// Returns file directory index entry interface for this entry.
    virtual FileDir_IIndex*           AsIndex();
    /// Returns file directory short column entry interface for this entry.
    virtual FileDir_IShortColumn*     AsShortColumn();
    /// Returns file directory oms entry interface for this entry.
    virtual FileDir_IOms*             AsOms();
    /// Returns file directory oms key partition entry interface for
    /// this entry.
    virtual FileDir_IOmsKeyPartition* AsOmsKeyPartition();
    /// Returns file directory oms key entry interface for this entry.
    virtual FileDir_IOmsKey*          AsOmsKey();
    /// Returns file directory oms fixed entry interface for this entry.
    virtual FileDir_IOmsFixed*        AsOmsFixed();
    /// Returns file directory oms var entry interface for this entry.
    virtual FileDir_IOmsVar*          AsOmsVar();

    friend class FileDir_GenericDirectory;
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;
    friend class FileDir_Generic;
    friend class FileDir_Table;
    friend class FileDir_Index;
    friend class FileDir_ShortColumn;
    friend class FileDir_Oms;
    friend class FileDir_OmsKey;
    friend class FileDir_OmsKeyPartition;
    friend class FileDir_OmsFixed;
    friend class FileDir_OmsVar;
    friend class FileDir_Test;

private:
    // no copy-construction or assignment
    FileDir_BaseEntry(const FileDir_BaseEntry&);
    FileDir_BaseEntry& operator=(const FileDir_BaseEntry&);
};

/**************************************************************************/

#endif // FILEDIR_BASEENTRY_HPP
