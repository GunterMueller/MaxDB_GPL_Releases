/*!
  @file     Converter_Map.cpp
  @author   TorstenS
  @author   TillL
  @ingroup  Converter
  @brief    Map for pageNos
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

#include "Converter/Converter_Dump.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_FlagInfo.hpp"
#include "Converter/Converter_Map.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_MapSection.hpp"
#include "Converter/Converter_PermMapSection.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "RunTime/RTE_Crash.hpp"


/*******************************************************************************

   class: Converter_Map

*******************************************************************************/

void
Converter_Map::Initialize(
    const Converter_MapEntryCount   entryClusterSize,
    const SAPDB_UInt                sectionCount,
    const SAPDB_UInt                leafPageSize )
{
    m_EntryClusterSize = entryClusterSize;
    m_SectionCount     = sectionCount;
    m_PeriodCount      = 0;
    m_LeafPageSize     = leafPageSize;
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_Map::Expand(
    const Data_PageNo maxPageNo,
    SAPDB_Int         &addPeriodCount ) // PTS 1126701 TS 2003-12-19
{
    // calculate the number of periods to be added
    // to the existing map

    addPeriodCount = CalcPeriodCount( maxPageNo ) - m_PeriodCount;

    if( 0 >= addPeriodCount ){
        return( SAPDB_TRUE ); // map has the right size
    }

    const Converter_MapEntryCount newCapacity  =
        m_SectionCount * m_EntryClusterSize * (m_PeriodCount + addPeriodCount);

    if( newCapacity > m_EntryVector.GetSize())
    {
        if( !m_EntryVector.Resize( newCapacity )){
            return SAPDB_FALSE;
        }

        if( !m_ChangedInfoVector.Resize( newCapacity )){
            return SAPDB_FALSE;
        }
    }

    m_PeriodCount += addPeriodCount;
    m_HighestPossiblePageNo = GetMapEntryCount() * GetLeafPageCapacity();

    return SAPDB_TRUE;
}

// -----------------------------------------------------------------------------

void
Converter_Map::ExpandOneSection(
    Converter_MapSection    &section,
    const SAPDB_Int         addPeriodCount ) // PTS 1126701 TS 2003-12-19
{
    const SAPDB_UInt startPeriodCount = m_PeriodCount - addPeriodCount;
    const SAPDB_UInt stopPeriodCount  = m_PeriodCount;
    const SAPDB_UInt sectionNo        = section.GetSectionNo();

    // loop over all new defined periods

    for( SAPDB_UInt periodNo = startPeriodCount; periodNo < stopPeriodCount; ++periodNo )
    {
        // these are map entry iterators of a single entry cluster

        Converter_MapEntryNo       insertIter = EntryClusterBeginNo( sectionNo, periodNo);
        const Converter_MapEntryNo endIter    = EntryClusterEndNo( sectionNo, periodNo);

        for( ; insertIter != endIter; ++insertIter ){
            section.AppendToUnusedEntryChain( insertIter );
        }
    }
}

// -----------------------------------------------------------------------------

void
Converter_Map::Shutdown()
{
    Converter_PageFrameAllocator        frameAlloc;
    Converter_MapVector::Iterator       iter    = m_EntryVector.Begin();
    const Converter_MapVector::Iterator endIter = m_EntryVector.End();

    for( ; iter != endIter; ++iter)
    {
        Converter_MapEntry  &entry = *iter;

        if (entry.PageIsAssigned())
        {
            entry.ReleasePageFrame( frameAlloc );
        }
    }

    m_EntryVector.Delete();
    m_ChangedInfoVector.Delete();
    m_IsPageUsedAfterRestorePages.Delete();
}

// -----------------------------------------------------------------------------

void
Converter_Map::IntegratePage(
    Converter_PermMapSection    &section,
    Converter_LeafPage          &page,
    const Data_PageNo           basePageNo,
    const SAPDB_UInt            usedEntryCount,
    const SAPDB_UInt            freeEntryCount )
{
    SAPDBERR_ASSERT_ARGUMENT( page.IsAssigned());
    SAPDBERR_ASSERT_ARGUMENT( page.DetermineFreeEntryCount() == freeEntryCount );
    SAPDBERR_ASSERT_ARGUMENT( usedEntryCount == page.GetEntryCount() - freeEntryCount );

    const Converter_MapEntryNo  entryNo = CalcEntryNo( basePageNo );
    Converter_MapEntry          &entry  = GetEntry( entryNo );

    SAPDBERR_ASSERT_STATE( ! entry.PageIsAssigned() );

    entry.AssignPage( page, freeEntryCount );

    section.IntegratePage( entryNo, usedEntryCount, freeEntryCount );
}

// -----------------------------------------------------------------------------

void
Converter_Map::DeletePage(
    const Converter_MapEntryNo  entryNo )
{
    Converter_MapEntry  &entry = GetEntry( entryNo );

    DeletePage(entry);
}

//-----------------------------------------------------------------------------

void
Converter_Map::DeletePage(
    Converter_MapEntry  &entry )
{
    Converter_PageFrameAllocator    frameAlloc;

    SAPDBERR_ASSERT_ARGUMENT( entry.PageIsAssigned());
    SAPDBERR_ASSERT_ARGUMENT( entry.GetPage().DetermineFreeEntryCount() == entry.GetPage().GetEntryCount());

    entry.ReleasePageFrame( frameAlloc );
}

// -----------------------------------------------------------------------------

void
Converter_Map::PreparePageForIO(
    Converter_PermMapSection    &section,
    Converter_MapEntry          &entry )
{
    SAPDBERR_ASSERT_STATE( entry.GetPage().ReadPageRecoveryMode().PageIsRecoverable());

    section.IncrementPendingIO();
    entry.SetPageIOPending();
}

// -----------------------------------------------------------------------------

void
Converter_Map::ResetPageStateAfterIO(
    Converter_PermMapSection    &section,
    Converter_MapEntry          &entry )
{
    entry.ClearPageIOPending();
    section.DecrementPendingIO();
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::RequestNewPageNo(
    Converter_MapSection    &section,
    Data_PageNo             &pageNo )
{
    Converter_MapSection::PartiallyUsedEntryIterator       iter    = section.PartiallyUsedEntriesBegin();
    const Converter_MapSection::PartiallyUsedEntryIterator enditer = section.PartiallyUsedEntriesEnd();

    while ((iter != enditer) && (*iter).PageIOPending()){
        ++iter;
    }

    if (iter != enditer)
    {
        // -------------------------------------------------------------------------
        // get reference to map entry
        // -------------------------------------------------------------------------
        Converter_MapEntryNo entryNo = iter.GetPosition();
        Converter_MapEntry&  entry   = *iter;

        // -------------------------------------------------------------------------
        // get reference to converter leaf page
        // -------------------------------------------------------------------------
        Converter_LeafPage& page = entry.GetPage();

        // -------------------------------------------------------------------------
        // get unused entry from page and calculate pno
        // -------------------------------------------------------------------------
        pageNo = page.ReserveEntry() + CalcBasePageNo(entryNo);

        // -------------------------------------------------------------------------
        // adjust entry state
        // (page is n o t marked as changed upon reservation)
        // -------------------------------------------------------------------------
        entry.DecFreePageEntryCount();

        // -------------------------------------------------------------------------
        // rechain entry if necessary
        // -------------------------------------------------------------------------
        if (entry.PageIsFull())
        {
            section.RechainEntryFromPartiallyUsedToFull( entryNo );
        }

        // -------------------------------------------------------------------------
        // check consistency of page and entry
        // -------------------------------------------------------------------------
        SAPDBERR_ASSERT_ARGUMENT (page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
        #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
        SAPDBErr_MessageList dummyerrlist;
        // The messages are printed directly until this code (the caller)
        // uses message lists. UH 2004-11-15
        #endif
        SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));
    }
    else
    {
        if( section.IsUnusedEntryChainEmpty()) // no free map entry is available
            return( Converter_NoPageNotAvailable );

        // -------------------------------------------------------------------------
        // get reference to a unused map entry
        // -------------------------------------------------------------------------
        const Converter_MapSection::UnusedEntryIterator   unUsedIter = section.UnusedEntriesBegin();

        const Converter_MapEntryNo  entryNo    = unUsedIter.GetPosition();
        Converter_MapEntry          &entry     = *unUsedIter;

        // -------------------------------------------------------------------------
        // get reference to empty converter leaf page
        // -------------------------------------------------------------------------
        Converter_LeafPage page;

        if (!UseUnusedEntryAndGetPage(page, section.GetRecoveryMode(), entryNo))
        {
            return Converter_FrameAllocationFailed;
        }

        // -------------------------------------------------------------------------
        // get unused entry from page and calculate pno
        // -------------------------------------------------------------------------
        pageNo = page.ReserveEntry() + CalcBasePageNo( entryNo );

        // -------------------------------------------------------------------------
        // rechain entry from unused to partially used
        // -------------------------------------------------------------------------
        section.RechainEntryFromUnusedToPartiallyUsed( entryNo );

        // -------------------------------------------------------------------------
        // check for max. data pageno
        // -------------------------------------------------------------------------
        section.CheckIncMaxUsedEntryNo( entryNo );

        // -------------------------------------------------------------------------
        // adjust entry state
        // (page is n o t marked as changed upon reservation)
        // -------------------------------------------------------------------------
        entry.DecFreePageEntryCount();

        // -------------------------------------------------------------------------
        // check consistency of page and entry
        // -------------------------------------------------------------------------
        SAPDBERR_ASSERT_ARGUMENT (page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
        #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
        SAPDBErr_MessageList dummyerrlist;
        // The messages are printed directly until this code (the caller)
        // uses message lists. UH 2004-11-15
        #endif
        SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));
        SAPDBERR_ASSERT_STATE( ! entry.PageIsFull() );
        SAPDBERR_ASSERT_STATE( ! entry.PageIsEmpty() );
    }

    section.IncrementNumUsedDataPages();

    return Converter_Ok;
}

// -----------------------------------------------------------------------------


Converter_Map::RequestSpecifiedNewPageNoRc
Converter_Map::RequestSpecifiedNewPageNo(
    Converter_PermMapSection &section,
    const Data_PageNo        pageNo )
{
    const Converter_MapEntryNo  entryNo = CalcEntryNo( pageNo );
    Converter_MapEntry          &entry  = GetEntry( entryNo );

    if( entry.PageIsAssigned() && entry.PageIOPending()){
        return rsnpRcPendingIo;
    }

    const Converter_PageEntryNo pageEntryNo = CalcPageEntryNo( pageNo );

    // -------------------------------------------------------------------------
    // get reference to converter leaf page
    // -------------------------------------------------------------------------
    if (entry.PageIsAssigned() && !entry.PageIsEmpty())
    {
        Converter_LeafPage  &page = entry.GetPage();

        if (page.EntryInUse( pageEntryNo ))
            return rsnpRcPageNoInUse;

        page.ReserveSpecifiedEntry( pageEntryNo );

        entry.DecFreePageEntryCount();

        if( entry.PageIsFull() ){
            section.RechainEntryFromPartiallyUsedToFull( entryNo );
        }
    }
    else
    {
        // -------------------------------------------------------------------------
        // must use a yet unused leaf page or empty leaf page
        // -------------------------------------------------------------------------

        if( entry.PageIsAssigned() )
        {
            // the page is available, empty and the map entry is within partially used chain

            Converter_LeafPage  &page = entry.GetPage();

            page.WritePageRecoveryMode( section.GetRecoveryMode() );
            page.ReserveSpecifiedEntry( pageEntryNo );
        }
        else
        {
            // page is not available and the map entry is within unused chain
            Converter_LeafPage page;

            if (!UseUnusedEntryAndGetPage( page, section.GetRecoveryMode(), entryNo))
            {
                return rsnpRcFrameAllocationFailed;
            }
            page.ReserveSpecifiedEntry( pageEntryNo );
            section.RechainEntryFromUnusedToPartiallyUsed( entryNo ); // PTS 1114856 TS 2002-04-16
        }

        entry.DecFreePageEntryCount();
        section.CheckIncMaxUsedEntryNo( entryNo );

        SAPDBERR_ASSERT_STATE( ! entry.PageIsFull() );
        SAPDBERR_ASSERT_STATE( ! entry.PageIsEmpty() );
    }

    SAPDBERR_ASSERT_ARGUMENT( entry.GetPage().DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
    #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
    SAPDBErr_MessageList dummyerrlist;
    // The messages are printed directly until this code (the caller)
    // uses message lists. UH 2004-11-15
    #endif
    SAPDBERR_ASSERT_ARGUMENT( entry.GetPage().Verify(false,dummyerrlist));

    section.IncrementNumUsedDataPages();

    return rsnpRcOkay;
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::FreeTempPageNo(
    Converter_MapSection    &section,
    const Data_PageNo       pageNo,
    IOMan_BlockAddress      &bladdr )
{
    SAPDB_Bool deletedPage = false;

    const Converter_MapEntryNo  entryNo     = CalcEntryNo( pageNo );
    const Converter_PageEntryNo pageEntryNo = CalcPageEntryNo( pageNo );

    Converter_MapEntry  &entry  = GetEntry( entryNo );
    Converter_LeafPage  &page   = entry.GetPage();
    const SAPDB_Bool    wasfull = entry.PageIsFull();

    SAPDBERR_ASSERT_ARGUMENT( ! section.GetRecoveryMode().PageIsRecoverable());

    if( !page.EntryInUse( pageEntryNo )){
        return Converter_PageNoAlreadyFree;
    }

    page.ReadBlockAddress( pageEntryNo , bladdr);

    page.ReleaseEntry( pageEntryNo );

    entry.IncFreePageEntryCount();

    // -------------------------------------------------------------------------
    // check for necessary rechaining:
    // -------------------------------------------------------------------------

    if( wasfull ){
        section.RechainEntryFromFullToPartiallyUsed( entryNo );
    }
    else
    {
        if( entry.PageIsEmpty() )
        {
            section.RechainEntryFromPartiallyUsedToUnused( entryNo );

            DeletePage( entryNo );

            deletedPage = true; // converter leaf page is empty and deleted

            section.CheckDecMaxDataPageNo( entryNo );
        }
    }

    if( !deletedPage )
    {
        SAPDBERR_ASSERT_ARGUMENT (page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
        #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
        SAPDBErr_MessageList dummyerrlist;
        // The messages are printed directly until this code (the caller)
        // uses message lists. UH 2004-11-15
        #endif
        SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));
    }

    section.DecrementNumUsedDataPages();

    return Converter_Ok;
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::FreePermPageNo(
    Converter_PermMapSection    &section,
    const Converter_Version     &snapShotVersion,
    const Data_PageNo           pageNo,
    IOMan_BlockAddress          &bladdr,
    bool                        &bPageUpdated,
    bool                        &bPendingPageUpdate )
{
    SAPDB_Bool deletedPage = false;

    const Converter_MapEntryNo  entryNo     = CalcEntryNo( pageNo );
    const Converter_PageEntryNo pageEntryNo = CalcPageEntryNo( pageNo );

    Converter_MapEntry  &entry  = GetEntry( entryNo );
    Converter_LeafPage  &page   = entry.GetPage();
    const SAPDB_Bool    wasfull = entry.PageIsFull();

    SAPDBERR_ASSERT_ARGUMENT( section.GetRecoveryMode().PageIsRecoverable());

    if (! page.EntryInUse( pageEntryNo )){
        return Converter_PageNoAlreadyFree;
    }

    bPageUpdated       = page.SavePagesRequested( pageEntryNo );
    bPendingPageUpdate = page.SavePagesPending( pageEntryNo );

    page.ReadBlockAddress( pageEntryNo , bladdr);

    page.ReleaseEntry( pageEntryNo );

    entry.IncFreePageEntryCount();

    section.MarkPageAsChanged( entryNo );

    // -------------------------------------------------------------------------
    // check for necessary rechaining:
    // -------------------------------------------------------------------------
    if( wasfull ){
        section.RechainEntryFromFullToPartiallyUsed( entryNo );
    }
    else
    {
        if( entry.PageIsEmpty() )
        {
            section.RechainEntryFromPartiallyUsedToUnused( entryNo );

            // -------------------------------------------------------------------------
            // add parentReference for removal of index entry during flush/savepoint
            // -------------------------------------------------------------------------

            const Converter_ParentReference             parentReference = page.ReadParentReference();
            Container_Vector<Converter_ParentReference> &parentRefs     = section.GetParentReferences();

            if(parentReference.GetIndexEntryNo().IsValid() && parentReference.GetPageEntryNo().IsValid())
            {
                if( parentRefs.IsFull() ){
                    parentRefs.Reserve( parentRefs.GetCapacity() + 5 );
                }
                parentRefs.InsertEnd( parentReference);

                // is old block address needed for frozen converter version
                if(( ! snapShotVersion.IsValid()) || ( page.ReadVersion() > snapShotVersion ))
                {
                    FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( page.ReadBlockAddress() );
                }
            }

            DeletePage( entryNo );

            // we don't want to flush this empty LeafPage
            section.MarkPageAsUnChanged( entryNo );

            deletedPage = true;

            section.CheckDecMaxDataPageNo( entryNo );
        }
    }

    if( !deletedPage )
    {
        SAPDBERR_ASSERT_ARGUMENT (page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
        #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
        SAPDBErr_MessageList dummyerrlist;
        // The messages are printed directly until this code (the caller)
        // uses message lists. UH 2004-11-15
        #endif
        SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));
    }

    section.DecrementNumUsedDataPages();

    return Converter_Ok;
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::GetBlockAddress(
    const Data_PageNo       pageNo,
    IOMan_BlockAddress      &bladdr ) const
{
    // -------------------------------------------------------------------------
    // check page no against upper limit
    // -------------------------------------------------------------------------

    if(( pageNo > GetHighestPossiblePageNo()) || ( pageNo < 0 ))
    {
        bladdr.Invalidate();
        return( Converter_PageNoOutOfRange );
    }

    const Converter_MapEntryNo  entryNo = CalcEntryNo( pageNo );
    const Converter_MapEntry    &entry  = GetEntry( entryNo );

    // -------------------------------------------------------------------------
    // get reference to converter leaf page
    // -------------------------------------------------------------------------
    if( ! entry.PageIsAssigned() )
    {
        bladdr.Invalidate();
        return( Converter_NoConverterEntry );
    }

    const Converter_LeafPage  &page = entry.GetPage();

    // -------------------------------------------------------------------------
    // get slot on converter leaf page
    // -------------------------------------------------------------------------
    const Converter_PageEntryNo pageEntryNo  = CalcPageEntryNo( pageNo );

    // -------------------------------------------------------------------------
    // check if slot is in use, as requested
    // -------------------------------------------------------------------------
    if ( ! page.EntryInUse( pageEntryNo ))
    {
        bladdr.Invalidate();
        return( Converter_NoConverterEntry );
    }

    // -------------------------------------------------------------------------
    // retrieve block address from converter page
    // -------------------------------------------------------------------------
    page.ReadBlockAddress( pageEntryNo, bladdr );

    // -------------------------------------------------------------------------
    // check if slot contains valid block address
    // -------------------------------------------------------------------------
    if ( ! bladdr.IsValid())
    {
        bladdr.Invalidate();
        return( Converter_InvalidBlockAddressFound );
    }

    return Converter_Ok;
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::SetTempBlockAddress(
    const Data_PageNo           pageNo,
    const IOMan_BlockAddress    &addr,
    IOMan_BlockAddress          &oldaddr )
{
    const Converter_PageEntryNo pageEntryNo = CalcPageEntryNo( pageNo );
    const Converter_MapEntryNo  entryNo     = CalcEntryNo( pageNo );
    Converter_MapEntry          &entry      = GetEntry( entryNo );
    Converter_LeafPage          &page       = entry.GetPage();

    const Converter_ReturnCode retCode = (Converter_ReturnCode)SetBlockAddress( page, addr, oldaddr, pageEntryNo );

    if( Converter_Ok == retCode ){
        SAPDBERR_ASSERT_ARGUMENT( page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
    }
    return( retCode );
}

// -----------------------------------------------------------------------------

Converter_Map::SetPermBlockRc
Converter_Map::SetPermBlockAddress(
    Converter_PermMapSection    &section,
    const Data_PageNo           pageNo,
    const IOMan_BlockAddress    &addr,
    IOMan_BlockAddress          &oldaddr,
    bool                        &bPageUpdated,
    bool                        &bPendingPageUpdate,
    bool                        bRestoreDataActive )
{
    const Converter_PageEntryNo pageEntryNo = CalcPageEntryNo( pageNo );
    const Converter_MapEntryNo  entryNo     = CalcEntryNo( pageNo );
    Converter_MapEntry          &entry      = GetEntry( entryNo );
    Converter_LeafPage          &page       = entry.GetPage();

    const SetBlockRc sbRc = SetBlockAddress( page, addr, oldaddr, pageEntryNo );

    if( sbaRcOkay != sbRc ){
        return spbaRcPageNoNotReserved;
    }
    if( bRestoreDataActive )
        bPageUpdated = page.SavePagesRequested( pageEntryNo );
    else
        bPageUpdated = page.SetSavePagesRequested( pageEntryNo );

    bPendingPageUpdate = page.SavePagesPending( pageEntryNo );

    section.MarkPageAsChanged( entryNo );

    SAPDBERR_ASSERT_ARGUMENT( page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());

    return spbaRcOkay;
}

// -----------------------------------------------------------------------------

Converter_ReturnCode
Converter_Map::RestoreUsedPageNos(
    Converter_PermMapSection                    &section,
    const Converter_Version                     &snapShotVersion,
    const Converter_BitMapPage::ConstIterator   &slotIter )
{
#   define  CONVERTER_MAP_BITBLOCKSIZE (SAPDB_BITS_PER_BYTE * sizeof(Converter_BitMapPage::BitBlock))

    typedef Converter_BitMapPage::BitBlock         BitBlock;
    typedef Converter_BitMapPage::ConstIterator    SlotIterator;
    typedef SlotIterator::ConstBlockIterator       BlockIterator;


    const Data_PageNo           pageNo  = slotIter.ReadDataPageNo();
    const Converter_MapEntryNo  entryNo = CalcEntryNo( pageNo );
    Converter_MapEntry          &entry  = GetEntry( entryNo );
    Converter_LeafPage          page;

    // -------------------------------------------------------------------------
    // obtain leaf page handle (alloc page if not yet available):
    // -------------------------------------------------------------------------
    if( entry.PageIsAssigned() ){
        page = entry.GetPage();
    }
    else
    {
        if (!UseUnusedEntryAndGetPage( page, section.GetRecoveryMode(), entryNo ))
        {
            return Converter_FrameAllocationFailed;
        }
        section.RechainEntryFromUnusedToPartiallyUsed( entryNo ); // PTS 1115279 TS 2002-04-17
    }
    // -------------------------------------------------------------------------
    // mark leaf page as 'checked'. Do not look at this converter leaf page at
    // the end of the restore pages to find converter leaf pages which could
    // be set to free, because this leaf page is still in use.
    // -------------------------------------------------------------------------

    m_IsPageUsedAfterRestorePages[ entryNo ] = true;

    // -------------------------------------------------------------------------
    // get iterators to 'in use' bitmaps on bitmappage slot and leaf page
    // -------------------------------------------------------------------------
    // This iterator operates on the given converter bit map page and provides
    // the 'inUse' information for one converter leaf page!

    BlockIterator slotblockiter      = slotIter.DataBegin();
    const BlockIterator slotblockend = slotIter.DataEnd();

    // This iterator operates on the converter leaf page which belongs to the
    // bit slot given by the converter bit map page. It provides the current
    // 'inUse' information of the leaf page.

    BlockIterator pageblockiter = reinterpret_cast<SlotIterator::ConstPointer>(page.GetInUseFlagsBlock().Begin());
    const BlockIterator pageblockend  = reinterpret_cast<SlotIterator::ConstPointer>(page.GetInUseFlagsBlock().End());

    SAPDBERR_ASSERT_STATE(slotblockend-slotblockiter == pageblockend-pageblockiter);

    // -------------------------------------------------------------------------
    // memorize page state:
    // -------------------------------------------------------------------------
    const SAPDB_Bool wasfull          = entry.PageIsFull();
    const SAPDB_Bool wasempty         = entry.PageIsEmpty();
    const SAPDB_Bool waspartiallyused = /*!wasempty && */ !wasfull; // PTS 1115279 TS 2002-04-17

    // -------------------------------------------------------------------------
    // iterate bitmaps bitblock-wise:
    // -------------------------------------------------------------------------
    Converter_PageEntryNo    basepageentryno = 0; // first entry on a converter page
    Converter_PageEntryCount reservedcount   = 0;
    Converter_PageEntryCount releasedcount   = 0;

    // The following algorithmn takes advantage of the equality of the 'inUse'
    // bit maps within converter bit map page and converter leaf page.

    while (slotblockiter != slotblockend)
    {
        // mustreserve-bits: '1': pageno is not reserved on leaf page but in
        //                        used pageno info - we must reserve it!
        // mustrelease-bits: '1': pageno is used on leaf page but not in
        //                        used pageno info - we must release it!
        BitBlock mustreserve = (*slotblockiter)  & (~*pageblockiter);
        BitBlock mustrelease = (~*slotblockiter) & (*pageblockiter);

        // ---------------------------------------------------------------------
        // perform necessary reservations: Set unused leaf page entry to reserved.
        // ---------------------------------------------------------------------
        if (mustreserve != 0)
        {
            for (SAPDB_UInt i=0; i<CONVERTER_MAP_BITBLOCKSIZE; ++i)
            {
                if ((mustreserve & Converter_FlagInfo::FlagInfoBitMask[i]) != 0)
                {
                    // must reserve corresponding leaf page entry
                    page.ReserveSpecifiedEntry(basepageentryno + i);
                    ++reservedcount;
                }
            }
        }

        // ---------------------------------------------------------------------
        // perform necessary releases: Set used leaf page entry to unused
        // ---------------------------------------------------------------------
        if (mustrelease != 0)
        {
            const SAPDB_Bool bIsPerm = section.GetRecoveryMode().UseDynamicPageAddressing();

            for (SAPDB_UInt i=0; i<CONVERTER_MAP_BITBLOCKSIZE; ++i)
            {
                if ((mustrelease & Converter_FlagInfo::FlagInfoBitMask[i]) != 0)
                {
                    // must release corresponding leaf page entry and block addr:
                    IOMan_BlockAddress block;

                    page.ReadBlockAddress( basepageentryno + i, block );
                    page.ReleaseEntry( basepageentryno + i );

                    if( snapShotVersion.IsValid())
                    {
                        // without reading the converter version of the page it's not possible
                        // to decide whether the old block is needed for the frozen version,
                        // therefore the old block is not set to free. If the block is not
                        // needed and therefore not part of the frozen converter, the block
                        // is set to free in the FBM at the next restart!

                        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block: " <<
                                            block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                            (( bIsPerm ) ? "permanent pageNo " : "static pageNo " ) <<
                                            basepageentryno + i << " is not marked as free");

                    }
                    else
                    {
                        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Block: " <<
                                            block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                            (( bIsPerm ) ? "permanent pageNo " : "static pageNo " ) <<
                                            basepageentryno + i << " is marked as free");

                        FBM_IManager::Instance().SetBlockStateToFree( block );
                    }
                    ++releasedcount;
                }
            }
        }

        ++slotblockiter;
        ++pageblockiter;
        basepageentryno += CONVERTER_MAP_BITBLOCKSIZE;
    }
    // -------------------------------------------------------------------------
    // update entry statistics:
    // -------------------------------------------------------------------------
    entry.IncFreePageEntryCount(releasedcount);
    entry.DecFreePageEntryCount(reservedcount);

    if(( 0 != reservedcount ) || ( 0 != releasedcount )) // PTS 1116251 TS 2002-07-09
        section.MarkPageAsChanged( entryNo ); // mark page as changed if changes are made

    // -------------------------------------------------------------------------
    // check consistency of page and entry
    // -------------------------------------------------------------------------
    SAPDBERR_ASSERT_ARGUMENT (page.DetermineFreeEntryCount() == entry.GetFreePageEntryCount());
    #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
    SAPDBErr_MessageList dummyerrlist;
    // The messages are printed directly until this code (the caller)
    // uses message lists. UH 2004-11-15
    #endif
    SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));

    // -------------------------------------------------------------------------
    // get new page state:
    // -------------------------------------------------------------------------
    const SAPDB_Bool isfull  = entry.PageIsFull();
    const SAPDB_Bool isempty = entry.PageIsEmpty();

    // -------------------------------------------------------------------------
    // rechain page if necessary:
    // -------------------------------------------------------------------------

    if(( wasempty ) || ( waspartiallyused ))// PTS 1115279 TS 2002-04-17
    {
        // PTS 1116251 TS 2002-07-09
        // empty pages are still within parially used chain

        if( isempty ){
            section.RechainEntryFromPartiallyUsedToUnused( entryNo );
        }
        else
        {
            if( isfull ){
                section.RechainEntryFromPartiallyUsedToFull( entryNo );
            }
        }
    }
    if( wasfull )
    {
        if( !isfull )
        {
            section.RechainEntryFromFullToPartiallyUsed( entryNo );

            // PTS 1116251 TS 2002-07-09
            // empty pages are still within parially used chain

            if( isempty )
                section.RechainEntryFromPartiallyUsedToUnused( entryNo );
        }
    }

    if( isempty )
    {
        // return frame to frame control (also updates entry statistics):

        // -------------------------------------------------------------------------
        // add parentReference for removal of index entry during flush/savepoint
        // -------------------------------------------------------------------------
        const Converter_ParentReference             parentReference = page.ReadParentReference();
        Container_Vector<Converter_ParentReference> &parentRefs     = section.GetParentReferences();

        if(parentReference.GetIndexEntryNo().IsValid() && parentReference.GetPageEntryNo().IsValid())
        {
            if (parentRefs.IsFull())
            {
                parentRefs.Reserve( parentRefs.GetCapacity() + 5 );
            }
            parentRefs.InsertEnd( parentReference );

            // is old block address needed for frozen converter version
            if(( ! snapShotVersion.IsValid()) || ( page.ReadVersion() > snapShotVersion )){
                FBM_IManager::Instance().SetBlockStateToFree( page.ReadBlockAddress() );
            }
        }

        DeletePage( entryNo );
        // we don't want to flush this empty LeafPage
        section.MarkPageAsUnChanged( entryNo );
    }

    // -------------------------------------------------------------------------
    // update statistics info:
    // -------------------------------------------------------------------------
    const SAPDB_Int reserveddiff = (SAPDB_Int)reservedcount - (SAPDB_Int)releasedcount;

    if( 0 < reserveddiff )
        section.IncrementNumUsedDataPages( ( SAPDB_UInt) reserveddiff );
    else
        section.DecrementNumUsedDataPages( ( SAPDB_UInt) reserveddiff );

    return Converter_Ok;

}

// -----------------------------------------------------------------------------

void
Converter_Map::FinishSectionRestoreUsedPageNos(
    Converter_PermMapSection    &section,
    const Converter_Version     &snapShotVersion )
{
    // check completely filled pages:

    Converter_MapSection::FullEntryUsedIterator       fullIter    = section.FullUsedEntriesBegin();
    const Converter_MapSection::FullEntryUsedIterator fullEndIter = section.FullUsedEntriesEnd();

    SAPDBTRACE_WRITELN( Converter_Trace, 7,
                        "Delete converter leaves from section: " << section.GetSectionNo());

    while( fullIter != fullEndIter )
    {
        const Converter_MapEntryNo entryNo = fullIter.GetPosition();

        if ( m_IsPageUsedAfterRestorePages[ entryNo ] )
        {
            ++fullIter;
            continue;
        }

        RemoveUnusedPageAfterRestore( entryNo, section, snapShotVersion );
        ++fullIter;

        // +++ rechain modifies partiallyUsedEnty chain!!! +++

        section.RechainEntryFromFullToPartiallyUsed( entryNo );
        section.RechainEntryFromPartiallyUsedToUnused( entryNo );
    }

    // check partially used pages:

    Converter_MapSection::PartiallyUsedEntryIterator       partiallyIter    = section.PartiallyUsedEntriesBegin();
    const Converter_MapSection::PartiallyUsedEntryIterator partiallyEndIter = section.PartiallyUsedEntriesEnd();

    while( partiallyIter != partiallyEndIter )
    {
        const Converter_MapEntryNo entryNo = partiallyIter.GetPosition();

        if( m_IsPageUsedAfterRestorePages[ entryNo ] )
        {
            ++partiallyIter;
            continue;
        }

        RemoveUnusedPageAfterRestore( entryNo, section, snapShotVersion );
        ++partiallyIter;

        // +++ rechain modifies partiallyUsedEnty chain!!! +++
        section.RechainEntryFromPartiallyUsedToUnused( entryNo );
    }
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_Map::IsMember(
    const Data_PageNo           basePageNo,
    const IOMan_BlockAddress    &block )
{
    const Converter_MapEntryNo  entryNo = CalcEntryNo( basePageNo );
    const Converter_MapEntry    &entry  = GetEntry( entryNo );

    if( ! entry.PageIsAssigned())
        return( SAPDB_FALSE );

    return( block == entry.GetPage().ReadBlockAddress());
}

// -----------------------------------------------------------------------------

void
Converter_Map::DumpEntries( Kernel_Dump &dump )
{
    Converter_MapVector::Iterator       iter    = m_EntryVector.Begin();
    const Converter_MapVector::Iterator endIter = m_EntryVector.End();

    struct ConverterMapEntry    dumpEntry;

    for( ; iter != endIter; ++iter )
    {
        if( (*iter).PageIsAssigned() )
        {
            Converter_LeafPage  &page = (*iter).GetPage();

            dumpEntry.dmpEntryNo     = m_EntryVector.GetIndex( iter );
            dumpEntry.dmpSectionNo   = CalcSectionNo( page.ReadBasePageNo() );
            dumpEntry.dmpBaseNo      = page.ReadBasePageNo();
            dumpEntry.dmpReservedCnt = page.ReadReservedEntryCount();
            dumpEntry.dmpFreeCnt     = (*iter).GetFreePageEntryCount();
            dumpEntry.dmpUsedCnt     = page.GetEntryCount() - dumpEntry.dmpFreeCnt - dumpEntry.dmpReservedCnt;
            dumpEntry.dmpIOPendig    = (*iter).PageIOPending();
            dumpEntry.dmpChanged     = PageIsChanged( m_EntryVector.GetIndex( iter ) );
            if( (*iter).GetPredecessor() == m_EntryVector.GetIndex( iter ) )
                dumpEntry.dmpChainPrev  = SAPDB_MAX_UINT4; // better for vxt08
            else
                dumpEntry.dmpChainPrev  = (*iter).GetPredecessor();
            dumpEntry.dmpChainNext   = (*iter).GetSuccessor();

            dump.InsertEntry( Kernel_Dump::DmpConverterMapEntry,
                              Kernel_DumpPage::Entry( &dumpEntry, sizeof( dumpEntry )));
        }
    }

    for( iter = m_EntryVector.Begin(); iter != endIter; ++iter )
    {
        if( (*iter).PageIsAssigned() )
            dump.InsertPage( (*iter).GetPage() );
    }
}

// -----------------------------------------------------------------------------

void
Converter_Map::Verify( Converter_MapSection &section ) const
{
    const SAPDB_UInt                sectionNo   = section.GetSectionNo();
    const Converter_MapEntryCount   maxEntryCnt = m_PeriodCount * m_EntryClusterSize;
    Converter_MapEntryCount         loopCnt     = 0;

    {
        Converter_MapSection::UnusedEntryIterator       iter    = section.UnusedEntriesBegin();
        const Converter_MapSection::UnusedEntryIterator endIter = section.UnusedEntriesEnd();

        for( ; iter != endIter; ++iter, ++loopCnt )
        {
            if( GetEntry( iter.GetPosition() ).PageIsAssigned() )
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_UNUSED_CHAIN_CORRUPTED,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( iter.GetPosition(), _T_d ));
                RTE_Crash( errMsg );
            }
            if( loopCnt > maxEntryCnt )
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_UNUSED_CHAIN_LOOP,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( maxEntryCnt, _T_d ));
                RTE_Crash( errMsg );
            }
        }
    }
    {
        Converter_MapSection::PartiallyUsedEntryIterator       iter    = section.PartiallyUsedEntriesBegin();
        const Converter_MapSection::PartiallyUsedEntryIterator endIter = section.PartiallyUsedEntriesEnd();

        for( loopCnt = 0; iter != endIter; ++iter, ++loopCnt )
        {
            if( GetEntry( iter.GetPosition() ).PageIsFull() )
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_PARTIALLY_CHAIN_CORRUPTED,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( iter.GetPosition(), _T_d ));
                RTE_Crash( errMsg );
            }
            if( loopCnt > maxEntryCnt )
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_PARTIALLY_CHAIN_LOOP,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( maxEntryCnt, _T_d ));
                RTE_Crash( errMsg );
            }
        }
    }
    {
        Converter_MapSection::FullEntryUsedIterator       iter    = section.FullUsedEntriesBegin();
        const Converter_MapSection::FullEntryUsedIterator endIter = section.FullUsedEntriesEnd();

        for( loopCnt = 0; iter != endIter; ++iter, ++loopCnt )
        {
            if( ! GetEntry( iter.GetPosition() ).PageIsFull() )
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_FULL_CHAIN_CORRUPTED,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( iter.GetPosition(), _T_d ));
                RTE_Crash( errMsg );
            }
            if( loopCnt > maxEntryCnt )
            {
                Converter_Exception errMsg (__CONTEXT__, CONVERTER_FULL_CHAIN_LOOP ,
                                            SAPDB_ToString( sectionNo, _T_d ),
                                            SAPDB_ToString( maxEntryCnt, _T_d ));

                RTE_Crash( errMsg );
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/* private methods                                                           */
/*---------------------------------------------------------------------------*/

Converter_Map::SetBlockRc
Converter_Map::SetBlockAddress(
    Converter_LeafPage          &page,
    const IOMan_BlockAddress    &addr,
    IOMan_BlockAddress          &oldaddr,
    const Converter_PageEntryNo pageEntryNo )
{
    // -------------------------------------------------------------------------
    // entry must have been reserved by call of RequestNewPageNo()
    // -------------------------------------------------------------------------

    if( !page.EntryInUse( pageEntryNo ))
    {
        Kernel_FileIO   dumpPage( KERNEL_CONV_FILE, KERNEL_COR_FILE_EXTENSION,
                                  SAPDB_ToString( page.ReadBasePageNo(), _T_d ));
        dumpPage.Write( page );
        dumpPage.Close();

        return sbaRcOkay;
    }
    // -------------------------------------------------------------------------
    // write i/o address to converter leaf page + read old addr
    // -------------------------------------------------------------------------
    page.WriteBlockAddress( pageEntryNo, addr, oldaddr );

    #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
    SAPDBErr_MessageList dummyerrlist;
    // The messages are printed directly until this code (the caller)
    // uses message lists. UH 2004-11-15
    #endif
    SAPDBERR_ASSERT_ARGUMENT (page.Verify(false,dummyerrlist));

    return sbaRcOkay;
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_Map::UseUnusedEntryAndGetPage(
    Converter_LeafPage            &page,
    const Data_PageRecoveryMode   &recoveryMode,
    const Converter_MapEntryNo    entryNo )
{
    Converter_MapEntry              &entry = m_EntryVector[ entryNo ];
    Converter_PageFrameAllocator    frameAlloc;
    Converter_LeafPage::PageFrame   frame = frameAlloc.New( recoveryMode.PageIsRecoverable() );

    SAPDBERR_ASSERT_ARGUMENT(!entry.PageIsAssigned());

    if(!frame.IsAssigned())
        return( SAPDB_FALSE );

    page.SetAndInitializeFrame( frame, Converter_Version(), Converter_ParentReference(),
                                CalcBasePageNo( entryNo ), recoveryMode);

    entry.AssignPage(page, page.GetEntryCount());

    return( SAPDB_TRUE );
}

// -----------------------------------------------------------------------------

void
Converter_Map::RemoveUnusedPageAfterRestore(  // PTS 1132116 TS 2004-10-25
    const Converter_MapEntryNo  entryNo,
    Converter_PermMapSection&   section,
    const Converter_Version&    snapShotVersion )
{

    // During the foregoing restore pages nobody looks at this leaf
    // page therefore it could be removed and the corresponding data
    // pages could be set to free.

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "Delete leaf of entryNo: " << entryNo );

    Converter_MapEntry&         entry         = GetEntry( entryNo );
    Converter_LeafPage          page          = entry.GetPage();
    Converter_PageEntryCount    releasedCount = 0;

    const SAPDB_Bool bIsPerm = section.GetRecoveryMode().UseDynamicPageAddressing();
    const Converter_PageEntryNo maxPageEntries = page.GetEntryCount();

    for( Converter_PageEntryNo pageEntry = 0; pageEntry < maxPageEntries; ++pageEntry)
    {
        if( ! page.EntryInUse( pageEntry )){
            continue;
        }

        ++releasedCount;

        IOMan_BlockAddress block;

        page.ReadBlockAddress( pageEntry, block);
        page.ReleaseEntry( pageEntry );

        // release unused data page

        if( snapShotVersion.IsValid())
        {
            // without reading the converter version of the page it's not possible
            // to decide whether the old block is needed for the frozen version,
            // therefore the old block is not set to free. If the block is not
            // needed and therefore not part of the frozen converter, the block
            // is set to free in the FBM at the next restart!

            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block: " <<
                                block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                (( bIsPerm ) ? "permanent pageNo " : "static pageNo " ) <<
                                page.ReadBasePageNo() + pageEntry << " is not marked as free");
        }
        else
        {
            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Block: " <<
                                block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                (( bIsPerm ) ? "permanent pageNo " : "static pageNo " ) <<
                                page.ReadBasePageNo() + pageEntry << " is marked as free");

            FBM_IManager::Instance().SetBlockStateToFree( block );
        }
    }

    // add parentReference for removal of index entry during flush/savepoint

    const Converter_ParentReference             parentReference = page.ReadParentReference();
    Container_Vector<Converter_ParentReference> &parentRefs     = section.GetParentReferences();

    if( parentReference.GetIndexEntryNo().IsValid() && parentReference.GetPageEntryNo().IsValid())
    {
        if( parentRefs.IsFull()){
            parentRefs.Reserve( parentRefs.GetCapacity() + 5 );
        }
        parentRefs.InsertEnd( parentReference );

        // release unused converter leaf page
        // is old block address needed for frozen converter version
        if(( ! snapShotVersion.IsValid()) || ( page.ReadVersion() > snapShotVersion )){
            FBM_IManager::Instance().SetBlockStateToFree( page.ReadBlockAddress() );
        }
    }

    entry.SetFreePageEntryCount( 0 );

    DeletePage( entryNo );
    // we don't want to flush this empty LeafPage
    section.MarkPageAsUnChanged( entryNo );

    section.DecrementNumUsedDataPages( ( SAPDB_UInt) releasedCount );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
