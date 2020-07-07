/*!
  @file           Data_ChainPage.hpp
  @author         UweH
  @brief          defines class Data_ChainPage

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#ifndef Data_ChainPage_HPP
#define Data_ChainPage_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Page.hpp"
/*!
   @class Data_ChainPage
   @brief The page which is used by Data_Chain.
 */
class Data_ChainPage : public Data_Page
{
public:
    /// initialize the page frame with invalid values
    virtual void InitializeFrame ()
    {
        Data_Page::InitializeFrame();
        SetNextPageNo (SAPDB_MAX_INT4);
        SetLastPageNo (SAPDB_MAX_INT4);
    }
    /*!
        @brief  The page id of the next page is returned.

        If no next page is available, PageId.IsValid() returns false.
     */
    Data_PageNo NextPageNo () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return STATIC_CAST(Data_PageNo,NodePtr()->nd_right());
    }
    /// The right neighbor page id is written to the page
    void SetNextPageNo (const Data_PageNo& PageNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->nd_right() = (tsp00_PageNo)PageNo;
    }
    /// The page id of the last page is returned.
    Data_PageNo LastPageNo () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return STATIC_CAST(Data_PageNo,NodePtr()->nd_last());
    }
    /// The last page id is written to the page
    void SetLastPageNo (const Data_PageNo& PageNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->nd_last() = (tsp00_PageNo)PageNo;
    }
    /*!
        @brief A new page is appended to the current page.
        @param NewPage    [out] This handle is filled with the new page.
        @param LastPageNo [out] if the new page has no next page its pageno is assigned here.
        @param errlist    [out] on error this is filled with information
        
        The current page points to the new page as the next page.
     */
    bool Append (Data_ChainPage&       NewPage,
                 SAPDBErr_MessageList &errlist);
	/// the current page is removed from the end of the chain. NextPageNo must be nil.
    bool Remove (Data_ChainPage       &prevPage,
                 SAPDBErr_MessageList &errlist);
    /// The PageChain context is written as a message into the message list.
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
};
#endif // Data_ChainPage_HPP