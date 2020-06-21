/*****************************************************************************/
/*!

  @file         FileDir_PageManager.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_PageManager.

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

#ifndef FILEDIR_PAGEMANAGER_HPP
#define FILEDIR_PAGEMANAGER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_PageIndexPage.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Types.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "Container/Container_Hash.hpp"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;
class Kernel_Dump;
class Kernel_FileIO;
class Msg_List;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*!
 * @class FileDir_PageManager
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Processes memory requests for new records and requests additional
 *        pages if necessary.
 */
class FileDir_PageManager
{
    friend class FileDir_Test;

    typedef FileDir_Vector<FileDir_Page*> PageList;
    typedef FileDir_Vector<FileDir_PageIndexPage*> PageIndexPageList;
    typedef Container_Hash<Data_PageNo, FileDir_Page*, false> PageHash;

    enum {
        c_initialPageListSize = 100,
        c_minimalBlockSize = 48
    };

    enum SavepointState {
        SavepointNotActive,
        SavepointActive
    };

    /// List of all pages used.
    PageHash m_pageHash;

    /// List of pages that contain references to the actual file
    /// directory pages.
    PageIndexPageList m_pageIndexPageList;

    /// List of pages that are queried for free blocks by Reserve().
    PageList m_freePageList;

    /// List of pages that need to be defragmented.
    PageList m_fragmentedPageList;

    /// List of pages that were added during restart
    PageList m_restartPageList;

    static FileDir_PageManager* m_instance;
    SAPDBMem_IRawAllocator& m_alloc;
    SAPDB_Int m_savepointActive;

    FileDir_RWRegionWrapper m_lock;

    /// Used by FileDir_Test to synchronize with savepoint.
    FileDir_RWRegionWrapper m_savepointLock;

    bool AddPage(
        const RTE_TaskId taskId,
        FileDir_Page* dataPage,
        const bool inRestart = false);
    void RemovePage(const RTE_TaskId taskId,
                    PageHash::Iterator& iterator);

    bool AddIndexPage(const RTE_TaskId taskId);
    void RemoveIndexPage(const RTE_TaskId taskId,
                         FileDir_PageIndexPage* indexPage);

    /// To be used by FileDir_Test only.
    void AcquireSharedSavepointLock();
    void ReleaseSharedSavepointLock();
public:
    /// Iterator over all pages that need to be defragmented.
    typedef PageList::Iterator FragPageIterator;

    /*!
     * @brief Defines the address of an entry on a FileDir_Page.
     */
    struct EntryAddress {
        FileDir_Page* dataPage;   ///< data page entry is located on
        char* entryAddress;       ///< in memory address of entry (on dataPage)
        EntryAddress() : dataPage(0), entryAddress(0) {};  ///< Constructor.
        EntryAddress(FileDir_Page* page, char* address) {  ///< Constructor.
            dataPage     = page;
            entryAddress = address;
        }
    };

    /*!
     * @brief Constructor. Initialize() needs to be called before the
     *        object can be actually used.
     *
     * @param alloc [in] allocator page manager will use
     */
    FileDir_PageManager(SAPDBMem_IRawAllocator& alloc);

    /*!
     * @brief Destructor. Shutdown() needs to have been called before
     *        calling the destructor.
     */
    ~FileDir_PageManager();

    /*!
     * @brief Initializes the page manager structures.
     *
     * @return true if initialization was successful.
     */
    SAPDB_Bool Initialize();


    /*!
     * @brief Shuts the page manager down and destroys all pages used.
     *
     * @return FileDir_Okay.
     */
    FileDir_ReturnCode Shutdown(const RTE_TaskId taskId);

    /*!
     * @brief Creates and initializes a root page for the page manager
     * on a volume.
     *
     * @param taskId     [in] task id of calling task
     * @param rootPageNo [in] data base page no to be used for root page
     */
    SAPDB_Bool CreateRootPage(
        const RTE_TaskId taskId,
        const Data_PageNo &rootPageNo);

    /*!
     * @brief Returns the first page index page, i.e. the file
     *        directory root page.
     */
    Data_PageNo GetRootPageNo();

    /*!
     * @brief Checks all pages.
     */
    FileDir_ReturnCode CheckData(RTE_TaskId taskId, SAPDB_Bool withUpdate);

    /*!
     * @brief Marks the given page to be flushed to disk during the
     *        next savepoint.
     *
     * @param taskId   [in] task id of calling task
     * @param pageNo   [in] page number of file directory page to be
     *                      marked dirty
     *
     * @return
     *   - true, on success
     *   - false, if pageNo is not known to the page manager
     */
    SAPDB_Bool MarkPageForWrite(
        const RTE_TaskId   taskId,
        const Data_PageNo& pageNo);

    /*!
     * @brief Returns a pointer to the FileDir_PageManager instance.
     */
    static FileDir_PageManager* Instance();

    /*!
     * @brief Write contents into kernel dump.
     */
    void Dump(const RTE_TaskId taskId, Kernel_Dump& dump);

    /*!
     * @brief Write contents into file.
     */
    void Extract(const RTE_TaskId taskId, Kernel_FileIO& ioFile);

    /*!
     * @brief Reserves memory on a data page and allocates a new data
     *        page if necessary. The size of the requested block is
     *        increased if necessary to provide proper alignment of
     *        blocks on the page.
     *
     * @param taskId [in] task id of calling task
     * @param recordLength [in] the size of the requested memory block
     *                          in bytes
     * @param keepPageLock [in] if true, keep the page locked for
     *                          update when returning; in this case it is
     *                          the caller's responsibility to clear the
     *                          lock once finished
     * @param notOnPageNo  [in] do not reserve memory on the page with
     *                          page number notOnPageNo; if -1 all pages
     *                          can be used
     * @param entryDataPage [out] on return, this will point to the
     *              page handler of the page the memory was reserved on
     * @return      the start (memory) address of the memory block reserved
     */
    char* ReserveSpace(
        const RTE_TaskId taskId,
        const SAPDB_Int recordLength,
        const SAPDB_Bool keepPageLock,
        FileDir_Page*& entryDataPage,
        const Data_PageNo notOnPageNo = -1);

    /*!
     * @brief Verifies page manager structures on disc.
     */
    void Verify();

    /*!
     * @name Savepoint related
     */
    /*@{*/

    /*!
     * @brief Informs the page manager that a savepoint has been started.
     *
     * @param taskId [in] task id of calling task
     */
    void BeginSavepoint(const RTE_TaskId taskId);

    /*!
     * @brief Tells the page manager to flush all pages that are dirty
     *        to disk.
     *
     * @param taskId [in] task id of calling task
     */
    void FlushDirtyPages(const RTE_TaskId taskId);

    /*!
     * @brief Informs the page manager that the savepoint has finished.
     *
     * @param taskId [in] task id of calling task
     */
    void EndSavepoint(const RTE_TaskId taskId);

    /*!
     * @brief Returns true if a savepoint is active.
     */
    SAPDB_Bool IsSavepointActive();

    /*!
     * @brief Returns the percentage of fragmented pages.
     */
    SAPDB_Int GetFragmentedPagesPercentage() const;

    /*@}*/

    /*!
     * @brief Returns the number of pages in use.
     */
    SAPDB_UInt GetPageCount() const;

    /*!
     * @brief Returns the allocator this page manager uses.
     */
    SAPDBMem_IRawAllocator& GetAllocator();

    /*!
     * @name Restart
     */
    /*@{*/

    /*!
     * @brief Loads all file directory index and content pages
     *        starting with the root page startPageNo.
     *
     * @param taskId      [in] task id of calling task
     * @param startPageNo [in] first page of file directory pages (root
     *                         page of file directory)
     */
    FileDir_ReturnCode Restart_LoadPages(
        const RTE_TaskId taskId,
        const Data_PageNo& startPageNo);

    /*!
     * @brief Iterates over all pages and initializes them. If
     * requested the pages also will be defragmented.
     *
     * @param taskId [in] task id
     * @param cleanupPages [in] if true, free memory on each page will
     *                          lumped together in one big block by
     *                          moving all used entries right next to each
     *                          other; otherwise all entries keep their
     *                          original position
     */
    void Restart_InitializePages(
        const RTE_TaskId taskId,
        const SAPDB_Bool cleanupPages);

    /*!
     * @brief Reserves memory on a data page and allocates a new data
     *        page if necessary during restart. The size of the
     *        requested block is increased if necessary to provide
     *        proper alignment of blocks on the page.
     *
     * This method does not use the pool of pages that has been loaded
     * during restart but uses different pages. This is necessary to
     * make sure that iterators over the regular pages ignore entries
     * that have been allocated via this method.
     *
     * @param taskId [in] task id of calling task
     * @param recordLength [in] the size of the requested memory block
     *                          in bytes
     * @param keepPageLock [in] if true, keep the page locked for
     *                          update when returning; in this case it is
     *                          the caller's responsibility to clear the
     *                          lock once finished
     * @param entryDataPage [out] on return, this will point to the
     *              page handler of the page the memory was reserved on
     * @return      the start (memory) address of the memory block reserved
     */
    char* RestartReserveSpace(
        const RTE_TaskId taskId,
        const SAPDB_Int  recordLength,
        const SAPDB_Bool keepPageLock,
        FileDir_Page*&   entryDataPage);

    /*!
     * @brief Ends the restart phase in the page manager.
     *
     * This method enters all pages that have been allocated via
     * RestartReserveSpace into the regular page list.
     */
    SAPDB_Bool RestartFinished(
        const RTE_TaskId taskId,
        Msg_List&        msgList);

    /*@}*/

    /*!
     * @brief Returns the entry address of the specified entry.
     *
     * @param pageNumber    [in] data base page number of page entry is on
     * @param entryLocation [in] location of entry relative to page start
     *
     * @return EntryAddress object pointing to entry.
     */
    EntryAddress GetEntry(
        const SAPDB_Int4 pageNumber,
        const SAPDB_UInt2 entryLocation);

    class EntryIterator;
    friend class EntryIterator;   ///< Entry iterator.
    /*!
     * @class EntryIterator
     * @ingroup FileDirectory_Classes
     * @author MartinKi
     *
     * @brief This class provides an iterator over all entries
     *        contained on all pages managed by a FileDir_PageManager
     *        object.
     */
    class EntryIterator
    {
        FileDir_PageManager& m_pageManager;
        PageHash::Iterator m_pageIterator;
        PageHash::Iterator m_lastPagePos;
        FileDir_Page::Iterator m_entryIterator;
        FileDir_Page::Iterator m_lastEntryPos;
        char* m_currentEntry;
        EntryAddress m_currentEntryAddress;
        FileDir_Page* m_currentPage;
    public:
        /// Constructor.
        EntryIterator(FileDir_PageManager& m_pageManager);
        /// Constructor for End() iterator.
        EntryIterator(FileDir_PageManager& m_pageManager, bool);
        /// Destructor.
        ~EntryIterator();

        /// Deref operator.
        EntryAddress* operator*();
        /// Advance by one.
        EntryIterator& operator++();
        /// Advance by one.
        EntryIterator operator++(int);

        /// Comparison operator.
        bool operator==(const EntryIterator& mm) const;
        /// Comparison operator.
        bool operator!=(const EntryIterator& mm) const;
    };
    /*!
     * @brief Returns an iterator over the page entries of all pages.
     */
    EntryIterator Begin() { return EntryIterator(*this); }

    /*!
     * @brief Returns an iterator pointing behind the last page entry.
     */
    EntryIterator End()   { return EntryIterator(*this, true); }

    class PageNoIterator;
    friend class PageNoIterator;   ///< Page number iterator.
    /*!
     * @class PageNoIterator
     * @brief An iterator over all page numbers of the pages that
     *        constitute the file directory contents. Needed
     *        during restart. This iterator is multi-thread safe.
     */
    class PageNoIterator
    {
        FileDir_PageManager& m_pageManager;
        PageIndexPageList::Iterator m_pageIterator;
        FileDir_PageIndexPage::Iterator m_pNoIterator;
        FileDir_RWRegionWrapper m_lock;
    public:
        /// Constructor.
        PageNoIterator(FileDir_PageManager& pageManager);
        /// Constructor for End() iterator.
        PageNoIterator(FileDir_PageManager& pageManager, bool);
        /// Copy-constructor.
        PageNoIterator(const PageNoIterator& mm);

        /// Return page number and advance by one.
        Data_PageNo Next(const RTE_TaskId taskId);

        /// Comparison operator.
        SAPDB_Bool operator==(const PageNoIterator& mm) const;
        /// Comparison operator.
        SAPDB_Bool operator!=(const PageNoIterator& mm) const;
    };

    /*!
     * @brief Returns an iterator over all page numbers of the pages that
     *        constitute the file directory contents.
     */
    PageNoIterator PageNoListBegin() { return PageNoIterator(*this); }

    /*!
     * @brief Returns an iterator pointing behind the last page number.
     */
    PageNoIterator PageNoListEnd()   { return PageNoIterator(*this, true); }

    /*!
     * @brief Returns an iterator over all fragmented pages.
     */
    FragPageIterator FragPageListBegin()
    {
        return m_fragmentedPageList.Begin();
    }

    /*!
     * @brief Returns an iterator pointing behind the last fragmented page.
     */
    FragPageIterator FragPageListEnd()
    {
        return m_fragmentedPageList.End();
    }
};

/**************************************************************************/

inline FileDir_PageManager* FileDir_PageManager::Instance()
{
    return m_instance;
}

/**************************************************************************/

inline SAPDB_UInt FileDir_PageManager::GetPageCount() const
{
    return m_pageHash.GetSize() + m_pageIndexPageList.GetSize();
}

/**************************************************************************/

inline SAPDBMem_IRawAllocator& FileDir_PageManager::GetAllocator()
{
    return m_alloc;
}

/**************************************************************************/

#endif // FILEDIR_PAGEMANAGER_HPP
