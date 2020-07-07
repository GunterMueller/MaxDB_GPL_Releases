/*!*****************************************************************************

  module:       Converter_Page.cpp

  ------------------------------------------------------------------------------

  responsible:  TorstenS

  author:       TillL

  special area: Converter

  description:  Handler classes for converter pages

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


#include "Converter/Converter_Page.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"


/*******************************************************************************

   class: Converter_Page

*******************************************************************************/

Converter_PageEntryIterator Converter_Page::Begin()
{
    return Converter_PageEntryIterator(*this, 0);
}
//------------------------------------------------------------------------------
Converter_PageEntryIterator Converter_Page::End()
{
    return Converter_PageEntryIterator(*this, GetEntryCount());
}
//------------------------------------------------------------------------------
void Converter_Page::BuildFreeEntryChain()
{
    PersistentEntry* pentry = PersistentEntryPtr(0);

    PageEntryCount ec = GetEntryCount();

    // --------------------------------
    // initialize first entry:
    // --------------------------------
    pentry->m_Free.m_Predecessor = ec-1;
    pentry->m_Free.m_Successor   = 1;
    ++pentry;

    // --------------------------------
    // initialize intermediate entries:
    // --------------------------------
    PageEntryNo i = 0;
    while (++i < ec-1)
    {
        pentry->m_Free.m_Predecessor = i-1;
        pentry->m_Free.m_Successor   = i+1;
        ++pentry;
    }

    // --------------------------------
    // initialize last entry:
    // --------------------------------
    pentry->m_Free.m_Predecessor = i-1;
    pentry->m_Free.m_Successor   = 0;

    // --------------------------------
    // initialize all flags with 0
    // --------------------------------
    InitializeFlagInfo();

    // --------------------------------
    // initialize free entry anchor
    // --------------------------------
    WriteFreeEntryAnchor(0);
}
//------------------------------------------------------------------------------
Converter_Page::PageEntryCount
Converter_Page::ReintegrateReservedEntries()
{
    PersistentEntry* pentry = PersistentEntryPtr(0);
    PageEntryCount   count  = 0;

    // --------------------------------
    // scan all entries on the page
    // --------------------------------
    for (PageEntryNo entryno = 0; entryno < GetEntryCount(); ++entryno)
    {
        IOMan_PackedBlockAddress addr(pentry->m_BlockAddress);
        // addr may be reserved or invalid
        if (addr.IsReserved() || addr.IsInvalid())
        {
            SetEntryInUse(entryno); // otherwise ReleaseEntry() crashes
            ReleaseEntry(entryno);
            ++count;
        }
        ++pentry;
    }

    return count;
}
//------------------------------------------------------------------------------
Converter_Page::PageEntryCount
Converter_Page::DetermineFreeEntryCount() const
{
    if (!HasFreeEntryAvailable())
        return 0;

    PageEntryNo             firstfreeNo = ReadFreeEntryAnchor();
    const PersistentEntry*  entry       = PersistentEntryPtr(firstfreeNo);
    PageEntryCount          count       = 1;

    // --------------------------------
    // navigate free entry chain
    // --------------------------------
    while (entry->m_Free.m_Successor != firstfreeNo)
    {
        entry = PersistentEntryPtr(entry->m_Free.m_Successor);
        ++count;
    }
    return count;
}
//------------------------------------------------------------------------------
Converter_Page::PageEntryNo
Converter_Page::ReserveEntry()
{

    if( !HasFreeEntryAvailable() ) // PTS 1113243 TS 2001-12-19
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_PAGE_IS_FULL );
        RTE_Crash( errMsg );
    }

    PageEntryNo         anchor = ReadFreeEntryAnchor();
    PersistentEntry&    entry  = *PersistentEntryPtr(anchor);

    // ------------------------------------------------
    // chain out entry at chain head:
    // ------------------------------------------------
    // check if chain consists of only 1 entry
    if (entry.m_Free.m_Successor == anchor)
    {
        // invalidate the anchor (page will be full after last entry is retrieved)
        WriteFreeEntryAnchor(PageEntryNo());
    }
    else
    {
        WriteFreeEntryAnchor(entry.m_Free.m_Successor);

        PersistentEntry& forwardentry =
            *PersistentEntryPtr(entry.m_Free.m_Successor);

        PersistentEntry& backwardentry =
            *PersistentEntryPtr(entry.m_Free.m_Predecessor);

        forwardentry.m_Free.m_Predecessor = entry.m_Free.m_Predecessor;
        backwardentry.m_Free.m_Successor  = entry.m_Free.m_Successor;
    }

    // ------------------------------------------------
    // reserve entry
    // ------------------------------------------------
    Reserve(anchor);

    return anchor;
}
//------------------------------------------------------------------------------
void Converter_Page::ReleaseEntry
(PageEntryNo                    entryno)
{
    SAPDBERR_ASSERT_STATE(EntryInUse(entryno));

    Release(entryno);

    PageEntryNo         anchor = ReadFreeEntryAnchor();
    PersistentEntry&    entry  = *PersistentEntryPtr(entryno);
    // ------------------------------------------------
    // chain in free entry:
    // ------------------------------------------------
    if (anchor.IsInvalid())
    {
        // ------------------------------------------------
        // chain is empty:
        // ------------------------------------------------
        WriteFreeEntryAnchor(entryno);
        entry.m_Free.m_Successor        =
            PersistentPageEntryNo::ConvertInternalToPersistent(entryno);
        entry.m_Free.m_Predecessor      =
            PersistentPageEntryNo::ConvertInternalToPersistent(entryno);
    }
    else
    {
        // ------------------------------------------------
        // chain in free entry between first and last entry:
        // ------------------------------------------------
        PersistentEntry& firstentry     = *PersistentEntryPtr(anchor);

        PersistentEntry& lastentry      = *PersistentEntryPtr
                                          (firstentry.m_Free.m_Predecessor);

        entry.m_Free.m_Successor        =
            PersistentPageEntryNo::ConvertInternalToPersistent(anchor);
        entry.m_Free.m_Predecessor      = firstentry.m_Free.m_Predecessor;
        firstentry.m_Free.m_Predecessor =
            PersistentPageEntryNo::ConvertInternalToPersistent(entryno);
        lastentry.m_Free.m_Successor    =
            PersistentPageEntryNo::ConvertInternalToPersistent(entryno);
    }
}
//------------------------------------------------------------------------------
Converter_Page::PageEntryNo
Converter_Page::ReserveSpecifiedEntry
(PageEntryNo                        entryno)
{
    SAPDBERR_ASSERT_ARGUMENT(!EntryInUse(entryno));

    PersistentEntry& entry       = *PersistentEntryPtr(entryno);
    PersistentEntry& successor   = *PersistentEntryPtr(entry.m_Free.m_Successor);
    PersistentEntry& predecessor = *PersistentEntryPtr(entry.m_Free.m_Predecessor);

    successor.m_Free.m_Predecessor  = entry.m_Free.m_Predecessor;
    predecessor.m_Free.m_Successor  = entry.m_Free.m_Successor;

    // check whether free entry anchor is involved
    PageEntryNo anchor = ReadFreeEntryAnchor();
    if (anchor == entryno)
    {
        // check if chain consists of only 1 entry
        if (entry.m_Free.m_Successor == anchor)
        {
            // invalidate the anchor (page will be full after last entry is retrieved)
            WriteFreeEntryAnchor(PageEntryNo());
        }
        else
        {
            WriteFreeEntryAnchor(entry.m_Free.m_Successor);
        }
    }

    // reserve entry
    Reserve(entryno);

    return entryno;
}
//------------------------------------------------------------------------------
SAPDB_Bool Converter_Page::Verify(bool                  isAdmin,
                                  SAPDBErr_MessageList &errlist) const
{
    SAPDB_Bool     PageIsOk = true;
    PageEntryCount i        = GetEntryCount();

    while (i-- > 0)
    {
        if (EntryInUse(i))
        {
            // -------------------------------------------------------------------------
            // entry must contain valid block address:
            // -------------------------------------------------------------------------
            if (IOMan_PackedBlockAddress (PersistentEntryPtr(i)->m_BlockAddress).IsInvalid())
            {
                Converter_Exception PageEntryInUseConflict1 (__CONTEXT__,
                        CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT1,
                        SAPDB_ToString(i, _T_d),
                        SAPDB_ToString(PersistentEntryPtr(i)->m_BlockAddress, _T_d));

                RTE_Message (PageEntryInUseConflict1);
                PageIsOk = false;

            }
        }
        else
        {
            // -------------------------------------------------------------------------
            // entry must contain valid forward and backward references to free entries:
            // -------------------------------------------------------------------------
            PageEntryNo backwardno =
                PersistentPageEntryNo::ConvertPersistentToInternal
                (PersistentEntryPtr(i)->m_Free.m_Predecessor);

            PageEntryNo forwardno =
                PersistentPageEntryNo::ConvertPersistentToInternal
                (PersistentEntryPtr(i)->m_Free.m_Successor);

            if ((backwardno >= GetEntryCount()) ||
                    (forwardno >= GetEntryCount())  ||
                    EntryInUse(backwardno)          ||
                    EntryInUse(forwardno))
            {
                Converter_Exception PageEntryInUseConflict2(__CONTEXT__,
                        CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT2,
                        SAPDB_ToString(i, _T_d),
                        SAPDB_ToString(backwardno, _T_d),
                        SAPDB_ToString(forwardno, _T_d));

                RTE_Message (PageEntryInUseConflict2);
                PageIsOk = false;
            }
        }
    }
    return PageIsOk;
}
//------------------------------------------------------------------------------

SAPDB_Bool
Converter_Page::CheckAfterRead() const
{
    if( ! Kernel_Page74::CheckAfterRead())
        return( SAPDB_FALSE );

    if( Kernel_ConverterPageType != ReadPageType() )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_BAD_PAGE_TYPE_2 );
        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

//------------------------------------------------------------------------------
Converter_Page::BitCount Converter_Page::DevNoBitCount = 0;
//------------------------------------------------------------------------------




