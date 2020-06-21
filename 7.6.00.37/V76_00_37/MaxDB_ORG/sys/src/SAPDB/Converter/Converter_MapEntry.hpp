/*!
  @file           Converter_MapEntry.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Entry of converter map
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


#ifndef CONVERTER_MAPENTRY_HPP
#define CONVERTER_MAPENTRY_HPP


#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_Entry.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"


/*!
   @class          Converter_MapEntry
   @brief          Entry of converter map
 */
class Converter_MapEntry :
public Converter_ChainNode <Converter_MapEntryNo>,
public Converter_Entry     <Converter_LeafPage>
{
public:

    /// a converter leaf page
    typedef Converter_LeafPage                  LeafPage;

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_MapEntry()
        :
        m_PageIOPending (false)
    {}

    /*!
       @brief          Assigns page handler
       @param          page [in] page handler
       @param          freeentrycount [in] number of free entries on the page
       @return         none
     */
    void AssignPage
        (LeafPage&                      page,
         PageEntryCount                 freeentrycount)
    {
        Converter_Entry<LeafPage>::AssignPage(page, freeentrycount);
        m_PageIOPending = false;
    }

    /*!
       @brief          Releases page frame from page handler
       @param          framealloc [in] page frame allocator for dallocation
       @return         none
     */
    void ReleasePageFrame
        (Converter_PageFrameAllocator  &framealloc)
    {
        Converter_Entry<LeafPage>::ReleasePageFrame(framealloc);
        m_PageIOPending = false;
    }

    /*!
       @brief          Sets page IO pending state
       @return         none
     */
    void SetPageIOPending()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        m_PageIOPending = true;
    }

    /*!
       @brief          Clears page IO pending state
       @return         none
     */
    void ClearPageIOPending()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        m_PageIOPending = false;
    }

    /*!
       @brief          Checks whether page IO is pending
       @return         (SAPDB_Bool) true if pending
     */
    SAPDB_Bool PageIOPending() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_PageIOPending;
    }

    /*!
       @brief          Writes recovery mode from associated leaf page
       @param          rec [in] page recovery mode
       @return         none
     */
    void SetPageRecoveryMode
        (const Data_PageRecoveryMode&   rec)
    {
        m_Page.WritePageRecoveryMode(rec);
    }

    /*!
       @brief          Reads recovery mode from associated leaf page
       @return         (Data_PageRecoveryMode) page recovery mode
     */
    Data_PageRecoveryMode GetPageRecoveryMode() const
    {
        return m_Page.ReadPageRecoveryMode();
    }

    /*!
       @brief          Checks whether associated page is recoverable
       @return         (SAPDB_Bool) true if recoverable
     */
    SAPDB_Bool PageIsRecoverable() const
    {
        return m_Page.ReadPageRecoveryMode().PageIsRecoverable();
    }

private:

    /// indicates whether associated page is currently in IO state
    SAPDB_Bool                      m_PageIOPending;
};


#endif // CONVERTER_PAGEIDMAPENTRY_HPP
