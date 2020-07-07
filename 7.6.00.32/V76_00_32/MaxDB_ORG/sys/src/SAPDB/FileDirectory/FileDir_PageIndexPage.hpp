/*****************************************************************************/
/*!

  @file         FileDir_PageIndexPage.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_PageIndexPage.

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

#ifndef FILEDIR_PAGEINDEXPAGE_HPP
#define FILEDIR_PAGEINDEXPAGE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_BasePage.hpp"
// for Converter_Version:
#include "Converter/Converter_Types.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"
// PASCAL: SP_basic_constants_and_types
#include "gsp00.h"
// PASCAL: GG_basic_constants_and_type
#include "ggg00.h"
#include "Container/Container_Hash.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
class FileDir_Page;

/*!
 * @class FileDir_PageIndexPage
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief A page containing page numbers of file directory content
 *        pages (of class FileDir_Page) on disk.

 * The methods of this class are not synchronized and the caller has
 * to make sure access is safe if this class is used in a
 * multi-threaded environment.
 */
class FileDir_PageIndexPage : public Data_BasePage
{
private:
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

    typedef SAPDB_Int4 PageType;

    bool m_isDirty;        ///< true if page needs to be flushed

    bool m_initialized;    ///< true if page was initialized successfully

    char*       m_anchor;
    PageType*   m_currentEntry;

    /*!
     * @brief Returns a pointer to the first page number entry on this
     *        page.
     */
    PageType* FirstEntryPos()
    {
        return reinterpret_cast<PageType*>(&m_anchor[HeaderSize]);
    }

public:
    /*!
     * @brief Create a page handler for a page containing page numbers
     *        of file directory pages.
     *
     * @param pageNumber [in] data page number of this page
     */
    FileDir_PageIndexPage(const Data_PageNo& pageNumber);

    // Destructor. Frees frame.
    ~FileDir_PageIndexPage();

    /*!
     * @brief Verify integrity of page.
     */
    FileDir_ReturnCode CheckData(RTE_TaskId taskId);

    /*!
     * @brief Initializes page. Must be called after constructor and
     *        before working with page.
     *
     * @return true, if initialisation was successful, false otherwise
     */
    SAPDB_Bool Initialize();

    /*!
     * @brief Reinitializes page from its current contents. Should be
     *        called after page has been read from disk.
     *
     * @return true if initialization was successful
     */
    SAPDB_Bool ReinitializeFromContent();

    /*!
     * @brief Adds a file directory content page to the list of page
     *        numbers on this page.
     *
     * @param  page [in] the page whose data page number should be
     *         added to the list
     *
     * @return true if page number was added successfully, false
     *         otherwise (i.e. there was not enough space to add the page
     *         number)
     */
    SAPDB_Bool AddPage(const FileDir_Page& page);


    /*!
     * @brief Removes a file directory content page from the list of page
     *        numbers on this page.
     *
     * @param  page [in] the page whose data page number should be
     *         removed from the list
     *
     * @return true if page number was removed successfully, false
     *         otherwise (i.e. the page number was not found in this page)
     */
    SAPDB_Bool RemovePage(const FileDir_Page& page);

    /*!
     * @brief Sets the next page index page following this one.
     *
     * @param nextPage [in] the page index page following this one; if
     *                      nextPage is invalid then this page has no
     *                      successor
     */
    void SetNextPage(const Data_PageNo& nextPage);

    /*!
     * @brief Returns the next page index page following this one.
     *
     * @return the page index page following this one; if this page
     *         has no successor then the returned Data_PageNo will be invalid
     */
    Data_PageNo GetNextPage() const;

    /*!
     * @brief Returns true if this page is full and no more page
     *        numbers can be stored on it.
     */
    SAPDB_Bool IsFull() const;

    /*!
     * @brief Returns true if no page numbers are stored on this page,
     *        i.e. it can be safely removed.
     */
    SAPDB_Bool IsEmpty() const;


    /*!
     * @brief Returns true if page is filled by less than 50 percent.
     */
    SAPDB_Bool MergeRecommended() const;

    /*!
     * @brief Returns the number of page numbers that can be stored on
     *        this page.
     */
    SAPDB_UInt Capacity() const;

    /*!
     * @brief Writes this page to disk. Blocks until write operation
     *        has finished.
     */
    void Flush(const RTE_TaskId taskId);

    class Iterator;
    friend class Iterator;   ///< Iterator.
    /*!
     * @class Iterator
     * @ingroup FileDirectory_Classes
     * @author MartinKi
     *
     * @brief Iterator over all page numbers store on a FileDir_PageIndexPage.
     *
     * This iterator is not synchronized. If the content of the pages
     * changes the iterator behaviour is undefined.
     */
    class Iterator
    {
    private:
        FileDir_PageIndexPage* m_indexPage;
        PageType* m_current;
    public:
        /// Constructor.
        Iterator(FileDir_PageIndexPage& indexPage);

        /// Constructor for End() iterator.
        Iterator(FileDir_PageIndexPage& indexPage, bool);

        /// Advance by one.
        Iterator& operator++();
        /// Advance by one.
        Iterator operator++(int);

        /// Comparison operator.
        bool operator==(const Iterator& i) const;

        /// Assignement operator.
        Iterator& operator=(const Iterator& i);

        /// Deref operator.
        Data_PageNo operator*()
        {
            return Data_PageNo(*m_current);
        }
    };

    /*!
     * @brief Returns an iterator over all page numbers on this page.
     */
    Iterator Begin() { return Iterator(*this); }


    /*!
     * @brief Returns an iterator pointing behind the last page
     *        numbers on this page.
     */
    Iterator End() { return Iterator(*this,true); }
private:
    void SetEntryCount(SAPDB_Int4 entryCount);
    void ModifyEntryCount(SAPDB_Int4 delta);
    SAPDB_Int4 GetEntryCount() const;
};

#endif // FILEDIR_PAGEINDEXPAGE_HPP
