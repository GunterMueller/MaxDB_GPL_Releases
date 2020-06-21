/*!
  @file           Converter_Entry.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Base class for entry of converter index or map
*/


/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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
*/

#ifndef CONVERTER_ENTRY_HPP
#define CONVERTER_ENTRY_HPP

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"


/*!
   @class          Converter_Entry
   @brief          Entry of converter parent index
 */
template <class Page>
class Converter_Entry
{
public:

    /// converter page entry count
    typedef Converter_PageEntryCount        PageEntryCount;

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_Entry()
    {}

    /*!
       @brief          Assigns page handler
       @param          page [in] page handler
       @param          freeentrycount [in] number of free entries on the page
       @return         none
     */
    void AssignPage
        (Page&                          page,
         PageEntryCount                 freeentrycount)
    {
        SAPDBERR_ASSERT_ARGUMENT(freeentrycount == 0 || page.IsAssigned());
        m_Page               = page;
        m_FreePageEntryCount = freeentrycount;
    }

    /*!
       @brief          Releases page frame from page handler
       @param          framealloc [in] page frame allocator for dallocation
       @return         none
     */

    void ReleasePageFrame
        (Converter_PageFrameAllocator  &framealloc)
    {
        framealloc.Free(m_Page.GetFrame(), m_Page.ReadPageRecoveryMode().PageIsRecoverable());
        m_FreePageEntryCount = 0;
    }

    /*!
       @brief          Returns reference to associated page
       @return         (Page&amp;) reference to page
     */
    Page& GetPage()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_Page;
    }

    /*!
       @brief          Returns const reference to associated page
       @return         (const Page&amp;) reference to page
     */
    const Page& GetPage() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_Page;
    }

    /*!
       @brief          Checks whether a page has been assigned to this entry
       @return         (SAPDB_Bool) true if assigned
     */
    SAPDB_Bool PageIsAssigned() const
    {
        return m_Page.IsAssigned();
    }

    /*!
       @brief          Checks whether actual page is empty
       @return         (SAPDB_Bool) true if empty
     */
    SAPDB_Bool PageIsEmpty() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        SAPDBERR_ASSERT_STATE(m_Page.HasFreeEntryAvailable() ||
                              (m_FreePageEntryCount == 0));

        return m_FreePageEntryCount == m_Page.GetEntryCount();
    }

    /*!
       @brief          Checks whether actual page is full
       @return         (SAPDB_Bool) true if full
     */
    SAPDB_Bool PageIsFull() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        SAPDBERR_ASSERT_STATE(m_Page.HasFreeEntryAvailable() ||
                              (m_FreePageEntryCount == 0));

        return m_FreePageEntryCount == 0;
    }

    /*!
       @brief          Sets actual free page entry count
       @param          count [in] free page entry count
       @return         none
     */
    void SetFreePageEntryCount
        (PageEntryCount                 count)
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        m_FreePageEntryCount = count;
    }

    /*!
       @brief          Returns actual free page entry count
       @return         (PageEntryCount) free page entry count
     */
    PageEntryCount GetFreePageEntryCount() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_FreePageEntryCount;
    }

    /*!
       @brief          Increments free page entry count
       @param          inc [in] increment
       @return         none
     */
    void IncFreePageEntryCount
        (PageEntryCount                 inc = 1)
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        m_FreePageEntryCount += inc;
    }

    /*!
       @brief          Decrements free page entry count
       @param          dec [in] decrement
       @return         none
     */
    void DecFreePageEntryCount
        (PageEntryCount                 dec = 1)
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        SAPDBERR_ASSERT_ARGUMENT(m_FreePageEntryCount >= dec);
        m_FreePageEntryCount -= dec;
    }

protected:

    /// pointer to converter page
    Page                       m_Page;

private:
    /// number of free page entries on associated page
    PageEntryCount             m_FreePageEntryCount;

};


#endif // CONVERTER_ENTRY_HPP
