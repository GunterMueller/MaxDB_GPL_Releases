/*****************************************************************************/
/*!

  @file         FileDir_Page.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_Page.

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

#ifndef FILEDIR_PAGE_HPP
#define FILEDIR_PAGE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Vector.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "DataAccess/Data_BasePage.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
class Converter_Version;
class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!
 * @class FileDir_Page
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Persistent data page of the FileDirectory.
 */
class FileDir_Page : public Data_BasePage
{
    friend class FileDir_PageManager;
    friend class FreeBlock;

public:
    /*!
     * @brief Everything the page knows about the structure/content
     *        of a record.
     */
    typedef struct RecordHeader {
        SAPDB_UInt2  length;   ///< Length of entry on page.
        SAPDB_Bool   isValid;  ///< True if entry is valid.
        SAPDB_Bool   lazyDelete;   ///< Entry can be deleted once all
                                   ///references are cleared.
    } RecordHeader;

private:
    /*!
     * @class FreeBlock
     * @brief A chunk of memory on the page
     */
    class FreeBlock
    {
        RecordHeader* pos;
    public:
        FreeBlock(char* p, SAPDB_UInt2 length);
        FreeBlock(char* p);
        char* GetPos();
        SAPDB_UInt2 GetLength() const;
        void SetPos(const char* newPos);
        void SetLength(const SAPDB_UInt2 newLength);
        bool CanBeMergedWith(const FreeBlock& candidate) const;
        void MergeWith(const FreeBlock& candidate);
        void ShrinkBy(const int shrinkBy);
        void MoveBy(const int moveBy);
        FreeBlock* Split(char* splitAddress);
    };

    /*!
     * @brief Constants
     */
    enum
    {
        PageSize     = PAGE_MXSP00,
        HeaderSize   = sizeof(PersistentHeader),
        TrailerSize  = sizeof(PersistentChecksumTrailer),
        PageDataSize = PageSize - HeaderSize - TrailerSize
    };


    typedef FileDir_Vector<FreeBlock*> FreeBlockList;

    /// Allocator this page uses for memory requests.
    SAPDBMem_IRawAllocator& m_allocator;

    /// List of free memory blocks on page.
    FreeBlockList m_freeBlockList;

    /// Overall free space on page.
    SAPDB_UInt2 m_freeSpace;

    SAPDB_Bool m_initialized;

    /// call InitFreeBlockList instead of UpdateFreeBlockList() after
    /// flush if true because the current free block list is outdated
    bool m_reinitFreeBlockList;

    /// True, if page is in page manager's free page list
    mutable SAPDB_Bool m_isInFreePageList;

    /// Available space on page (including space for header/trailer)
    SAPDB_UInt2 m_pageSize;
public:
    /*!
     * @brief Creates page pageNumber with converter version
     *        converterVersion. Memory for the page must be provided
     *        via frame.
     *
     * @param alloc            [in] Allocator to use
     * @param taskId           [in] Id of calling task
     * @param pageNumber       [in] Page number of this page
     * @param needsWrite       [in] page needs to be written to disk on
     *                              next savepoint (should be false if frame
     *                              content is to be loaded from disk)
     */
    FileDir_Page(
        SAPDBMem_IRawAllocator& alloc,
        const RTE_TaskId        taskId,
        const Data_PageNo&      pageNumber,
        const SAPDB_Bool        needsWrite = true);

    /// Destructor.
    ~FileDir_Page();

    /*!
     * @brief Verify integrity of page.
     */
    FileDir_ReturnCode CheckData(RTE_TaskId taskId);

    /*!
     * @brief Intializes the page.
     *
     * @return
     *         - true if successful
     *         - false if not
     */
    bool Initialize();

    /*!
     * @name Memory management
     */
    /*@{*/

    /*!
     * @brief Returns the size of the largest free block available on
     *        the page.
     *
     * The next call of ReserveSpace() is guaranteed to be able to reserve
     * as much space as returned from this function.
     *
     * @return Size of largest free data block.
     */
    SAPDB_UInt2 GetLargestFreeBlock() const;

    /*!
     * @brief Reserves recordLength bytes on page and registers
     *        pageEntry. Page must be locked for update already. The
     *        caller has to make sure that enough space is available
     *        on the page.
     *
     * @param taskId       [in] task id of calling task
     * @param recordLength [in] the length in bytes to be allocated on page.
     *
     * @return The memory address the reserved memory block.
     */
    char* ReserveSpace(
        const RTE_TaskId taskId,
        const int recordLength);
    /*@}*/


    /*!
     * @brief Immediately enter the block p is pointing to into free
     *        block list of page, i.e. it can be reused without having
     *        to wait for the next savepoint to finish. It is the
     *        caller's responsibility to lock and unlock the page for
     *        update if needed.
     *
     * @param taskId [in] task id of calling task
     * @param p      [in] pointer to memory address of block to be freed
     */
    void FreeEntry(
        const RTE_TaskId taskId,
        char* p);


    /*!
     * @brief Sets the entry at memory address p to valid. It is the
     *        caller's responsibility to lock and unlock the page for
     *        update if needed.
     */
    void SetValid(char* p);

    /*!
     * @brief Invalidates the entry at memory address p. It is the
     *        caller's responsibility to lock and unlock the page for
     *        update if needed.
     */
    void SetInvalid(char* p);

    /*!
     * @brief Returns true if the entry at memory address p is valid.
     */
    SAPDB_Bool IsValid(char* p) const;

    /*!
     * @brief Marks entry at p for deletion (during next restart at
     *        the latest).
     */
    void SetLazyDelete(char* p);

    /*!
     * @brief Returns true if the entry at memory address p should be deleted
     *        on restart.
     */
    SAPDB_Bool IsLazyDeleteSet(char* p);

    /*!
     * @name Savepoint relevant
     */
    /*@{*/

    /*!
     * @brief Flush page to disk.
     */
    void Flush(const RTE_TaskId taskId);

    /*!
     * @return Returns true, if page has changed since last savepoint
     *         (and needs to be flushed)
     */
    bool IsDirty();

    /*@}*/

    /*!
     * @brief Explicitly marks the page dirty and that it needs to be
     *        flushed.
     */
    void SetDirty();

    /*!
     * @name Used during restart
     */
    /*@{*/

    /*!
     * @brief Reinitializes list of free blocks and used entries based
     *        on the current contents of the page. Used during
     *        restart.
     */
    void Restart_ReinitializeFromContent();

    /*!
     * @brief Shrink the size of an entry by shrinkBy bytes.
     *
     * Although the entry chain on the page is being adjusted, no free
     * block is created for the space freed.
     * Restart_ReinitializeFromContent() has be called to properly
     * initialize the page!
     *
     * @param p        [in] pointer to memory address of block to be shrunk
     * @param shrinkBy [in] size in bytes to shrink entry by
     */
    void Restart_ShrinkEntry(char* p, SAPDB_UInt2 shrinkBy);

    /*!
     * @brief Defragment free memory on page. On return there will be
     *        only one free block on the page containing all the free space
     *        available.
     */
    void Restart_Defragment();

    /*!
     * @return The length in bytes of the record p is pointing to.
     */
    SAPDB_UInt2 GetRecordLength(char* p);


    /*!
     * @brief Returns the memory address of the entry with the
     *        location entryLocation on page.
     * @return The memory address the entry record is stored at.
     */
    char* GetEntryAddress(SAPDB_UInt2 entryLocation);

    /*!
     * @brief Sets transient marker that this page is part of the page
     *        manager's free page list. Is not relevant for the page
     *        itself but only to avoid having the page manager go
     *        through its list of free pages every time it wants to
     *        check for a page.
     */
    void SetIsInFreePageList(SAPDB_Bool isInList) const;

    /*!
     * @brief Returns transient marker indicating whether page is in
     *        page manager's free page list.
     */
    SAPDB_Bool GetIsInFreePageList() const;

    /*@}*/

    /*!
     * @brief Returns the amount of unused space on this page.
     */
    SAPDB_UInt2 GetAllFreeSpace(const RTE_TaskId taskId);

    /*!
     * @brief Returns some statistical data for this page.
     */
    void GetStatistics(
        SAPDB_Int& freeSpace,
        SAPDB_Int& freeBlockCount,
        SAPDB_Int& largestFreeBlock);

    /*!
     * @brief Returns a rough measure for the fragmentation of this page.
     */
    SAPDB_Int GetFragmentationMeasure();

    /*!
     * @brief Returns true if page contains no data.
     */
    bool IsEmpty()
    {
        return m_freeSpace == PageDataSize;
    }

    /*!
     * @name Debugging
     */
    /*@{*/

    /// FIXME: for debugging only
    SAPDB_Int4 GetEntryCount() const;

    /*@}*/

    class Iterator;
    friend class Iterator;  ///< Iterator.
    /*!
     * @class Iterator
     * @ingroup FileDirectory_Classes
     * @author MartinKi
     *
     * @brief This class provides an iterator over all valid entries
     *        on this page.
     */
    class Iterator
    {
        FileDir_Page* m_page;
        char* m_currentEntry;
    public:
        /// Constructor.
        Iterator()
            : m_page(0)
        {}
        /// Constructor.
        Iterator(FileDir_Page& page);
        /// Constructor for End() iterator.
        Iterator(FileDir_Page& page, bool);
        /// Destructor.
        ~Iterator();

        /// Deref operator.
        char* operator*();
        /// Advance by one.
        Iterator& operator++();

        /// Comparison operator.
        bool operator==(const Iterator& it) const;
        /// Comparison operator.
        bool operator!=(const Iterator& it) const;
        /// Assignment operator.
        Iterator& operator=(const Iterator& it);
    };

    /*!
     * @brief Returns an iterator over all entries on this page.
     */
    Iterator Begin() {
        return Iterator(*this);
    }

    /*!
     * @brief Returns an iterator pointing behind the last entry on this page.
     */
    Iterator End() {
        return Iterator(*this, true);
    }


    /*!
     * @name Locking
     */
    /*@{*/

    /*!
     * @brief Acquires a lock on this page for update. No I/O is
     *        possible while this lock is held. Multiple locks for
     *        update can be held at the same time.
     */
    void LockPageForUpdate(const RTE_TaskId taskId);

    /*!
     * @brief Tries to acquire a lock on this page for update. No I/O
     *        is possible while this lock is held. Multiple locks for
     *        update can be held at the same time. This call will not
     *        block and return false if the lock could not be
     *        acquired.
     */
    SAPDB_Bool TryLockPageForUpdate(const RTE_TaskId taskId);

    /*!
     * @brief Releases a lock for update.
     */
    void UnlockPageForUpdate(const RTE_TaskId taskId);

    /*!
     * @brief Acquires a lock for I/O on this page. No updates are
     *        possible while this lock is held. This is an exclusive
     *        lock.
     */
    void LockPageForIO(const RTE_TaskId taskId);

    /*!
     * @brief Releases a held I/O lock.
     */
    void UnlockPageForIO(const RTE_TaskId taskId);
    /*@}*/

    /*!
     * @brief  Returns the location on the page (i.e. offset relative
     *         to page start) of the entry p is pointing to.
     * @param  p Address of entry for which location on page is requested
     * @return entry number of p
     */
    SAPDB_UInt2 GetEntryLocation(void* p);

private:
    int m_largestFreeBlock;    ///< size of largest free block on page

    /// True if page needs to be flushed during savepoint.
    bool m_isDirty;
    char* m_anchor;

    /*!
     * @name Locking variables
     *
     * The order in which locks should be acquired is as follows:
     * - m_pageIOLock
     * - m_freeListLock
     */
    /*@{*/

    /// Updaters should request share lock, Flush needs exclusive lock.
    FileDir_RWRegionWrapper m_pageIOLock;
    /// Protects the free block list.
    FileDir_RWRegionWrapper m_freeListLock;

    /*@}*/

    /// Number of record pointers that can be reused.
    int m_recyclableEntryCount;

    void ShrinkFreeBlock(FreeBlock* b, int shrinkBy);
    void UpdateLargestFreeBlock();
    void InsertIntoFreeList(FreeBlock* freeBlock);
    void InitFreeBlockList();
    void UpdateFreeBlockList();
    void VerifyFreeBlockList(bool compareWithPage);
    void VerifyEntryCount();
    void SetEntryCount(SAPDB_Int4 entryCount);
    void ModifyEntryCount(SAPDB_Int4 delta);
};

/**************************************************************************/

inline SAPDB_Bool FileDir_Page::IsValid(char* p) const
{
    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );

    return reinterpret_cast<RecordHeader*>(p)->isValid;
}

/**************************************************************************/

inline void FileDir_Page::SetDirty()
{
    m_isDirty = true;
}

/**************************************************************************/

inline void FileDir_Page::SetIsInFreePageList(SAPDB_Bool isInList) const
{
    m_isInFreePageList = isInList;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_Page::GetIsInFreePageList() const
{
    return m_isInFreePageList;
}

/**************************************************************************/

#endif // FILEDIR_PAGE_HPP
