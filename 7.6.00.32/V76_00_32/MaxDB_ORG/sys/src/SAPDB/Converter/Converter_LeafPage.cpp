/*!*****************************************************************************

  module:       Converter_LeafPage.cpp

  ------------------------------------------------------------------------------

  responsible:  TorstenS

  author:       TillL

  special area: Converter

  description:  see .hpp

  see also:

  Copyright (c) 2000-2004 SAP AG

  ------------------------------------------------------------------------------

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

*******************************************************************************/


#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_BlockAddressIterator.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp" /* nocheck */
#include "RunTime/RTE_Message.hpp"

/*******************************************************************************

  class: Converter_LeafPage

*******************************************************************************/

SAPDB_Bool Converter_LeafPage::Verify(bool                  isAdmin,
                                      SAPDBErr_MessageList &errlist) const
{
    const bool           dummyIsAdminMode   = true;
    SAPDB_Bool           PageIsOk           = Converter_Page::Verify(dummyIsAdminMode,errlist);
    PageEntryCount       NumEntriesInUse    = 0;
    PageEntryCount       NumEntriesReserved = 0;
    PageEntryCount       NumEntriesFree     = DetermineFreeEntryCount();


    PageEntryCount i = GetEntryCount();

    while (i-- > 0)
    {
        if (IOMan_PackedBlockAddress (PersistentEntryPtr(i)->m_BlockAddress).IsReserved())
        {
            ++NumEntriesReserved;
            if (!EntryInUse(i))
            {
                Converter_Exception PageEntryInUseConflict5 (__CONTEXT__,
                        CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT5,
                        SAPDB_ToString(i, _T_d));
                errlist.AppendNewMessage(PageEntryInUseConflict5);
                RTE_Message (PageEntryInUseConflict5);
                PageIsOk = false;
            }
        }

        if (EntryInUse(i))
            ++ NumEntriesInUse;
        else
        {
            if (SavePagesRequested(i))
            {
                Converter_Exception PageEntryInUseConflict3
                (__CONTEXT__, CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT3,
                 SAPDB_ToString(i, _T_d));
                errlist.AppendNewMessage(PageEntryInUseConflict3);
                RTE_Message (PageEntryInUseConflict3);
                PageIsOk = false;

            }
            if (SavePagesPending(i))
            {
                Converter_Exception PageEntryInUseConflict4
                (__CONTEXT__, CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT4,
                 SAPDB_ToString(i, _T_d));

                errlist.AppendNewMessage(PageEntryInUseConflict4);
                RTE_Message (PageEntryInUseConflict4);
                PageIsOk = false;
            }
        }
    }

    if ((NumEntriesFree + NumEntriesInUse) != GetEntryCount())
    {
        Converter_Exception WrongCounter1
        (__CONTEXT__, CONVERTER_IPAGE_WRONG_COUNTER1,
         SAPDB_ToString(NumEntriesFree,  _T_d),
         SAPDB_ToString(NumEntriesInUse, _T_d),
         SAPDB_ToString(GetEntryCount(), _T_d));

        errlist.AppendNewMessage(WrongCounter1);
        RTE_Message (WrongCounter1);
        PageIsOk = false;
    }

    if (NumEntriesReserved != ReadReservedEntryCount())
    {
        Converter_Exception WrongCounter2
        (__CONTEXT__, CONVERTER_IPAGE_WRONG_COUNTER2,
         SAPDB_ToString(NumEntriesReserved, _T_d),
         SAPDB_ToString(ReadReservedEntryCount(), _T_d));

        errlist.AppendNewMessage(WrongCounter2);
        RTE_Message (WrongCounter2);
        PageIsOk = false;
    }

    return PageIsOk;
}

//------------------------------------------------------------------------------

SAPDB_Bool
Converter_LeafPage::CheckAfterRead() const
{
    if( ! Converter_Page::CheckAfterRead())
        return( SAPDB_FALSE );

    if( Converter_LeafPageType != ReadConverterPageType() )
    {
        Converter_Exception errMsg(__CONTEXT__, CONVERTER_ERR_BAD_LEAF_PAGE_TYPE );
        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

//------------------------------------------------------------------------------

Converter_LeafPage::PageEntryCount
Converter_LeafPage::CalcEntryCount() const
{
    return CalcEntryCount (m_Frame.GetLength());
}

//------------------------------------------------------------------------------

Converter_LeafPage::PageEntryCount
Converter_LeafPage::CalcEntryCount (SAPDB_UInt Capacity)
{
    SAPDB_UInt hs    = sizeof(PersistentHeader);
    SAPDB_UInt ts    = sizeof(Kernel_Page74::PersistentTrailer);
    SAPDB_UInt as    = GetEntrySize();
    SAPDB_UInt us    = Capacity - hs - ts;

    SAPDB_UInt count = SAPDB_BITS_PER_BYTE * us / (SAPDB_BITS_PER_BYTE * as + 3);

    if (count % (SAPDB_BITS_PER_BYTE * sizeof(Converter_FlagInfo::FlagInfoData)) != 0)
        count -= 3;

    return count;
}

// -----------------------------------------------------------------------------

Converter_BlockAddressIterator
Converter_LeafPage::GetUsedBlockIterator()
{
    return Converter_BlockAddressIterator( *this );
}

// -----------------------------------------------------------------------------

Converter_LeafPageEntryIteratorDrop
Converter_LeafPage::BeginDrop(const IOMan_DeviceNo volNo)
{
    return Converter_LeafPageEntryIteratorDrop(*this, volNo);
}

// -----------------------------------------------------------------------------

Converter_LeafPageEntryIteratorDrop
Converter_LeafPage::EndDrop(const IOMan_DeviceNo volNo)
{
    return Converter_LeafPageEntryIteratorDrop(*this, volNo, GetEntryCount());
}

// -----------------------------------------------------------------------------

