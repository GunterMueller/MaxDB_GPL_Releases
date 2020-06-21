/*!*****************************************************************************

  module:       Converter_Index.cpp

  ------------------------------------------------------------------------------

  responsible:  TorstenS

  author:       TillL

  special area: Converter

  description:  see .hpp
                
  see also:     

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



#include "Converter/Converter_Dump.hpp"
#include "Converter/Converter_Index.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_Lock.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"
#include "Converter/Converter_Pager.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"

/*******************************************************************************

   class: Converter_Index

*******************************************************************************/

void Converter_Index::Create()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::Create", Converter_Trace, 6 );

    Converter_PageFrameAllocator    frameAlloc;
    IndexPage rootpage( frameAlloc.New(), Converter_Version(),
                        Converter_ParentReference());

    PageEntryCount indexpagecapacity = rootpage.GetEntryCount();

    if (!Reserve(indexpagecapacity + 1)) // PTS 1113242 TS 2001-12-20
    {
        Converter_Exception errMsg(__CONTEXT__,CONVERTER_NO_MORE_MEMORY,
                                   SAPDB_ToString( Converter_MemoryAllocationFailed, _T_d ));
        RTE_Crash( errMsg );
    }

    // insert all index pages into EmptyChain
    SAPDBERR_ASSERT_STATE( m_EmptyEntryChain.IsEmpty() );

    EntryIterator iter      = Level1Begin();
    const EntryIterator end = Level1End();
    while(iter != end) {
        m_EmptyEntryChain.InsertEnd(m_EntryVector.GetIndex(iter));
        ++iter;
    }

    SetRootPage(rootpage);

    SAPDBERR_ASSERT_STATE( Verify() );
}

// -----------------------------------------------------------------------------

void Converter_Index::Restart(
    const IOMan_BlockAddress   &rootaddr,
    Converter_Pager            &pager )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::Restart", Converter_Trace, 6 );

    // -------------------------------------------------------------------------
    // load and integrate root page
    // -------------------------------------------------------------------------

    SAPDBTRACE_WRITELN( Converter_Trace, 6, "Root block : "
                        << rootaddr.GetDeviceNo() << "/" << rootaddr.GetBlockNo());

    IndexPage   rootpage;

    pager.Read( rootaddr, rootpage );

    const PageEntryCount indexpagecapacity = rootpage.GetEntryCount();

    if (!Reserve(indexpagecapacity + 1)) // PTS 1113242 TS 2001-12-20
    {
        Converter_Exception errMsg(__CONTEXT__,CONVERTER_NO_MORE_MEMORY,
                                   SAPDB_ToString( Converter_MemoryAllocationFailed, _T_d ));
        RTE_Crash( errMsg );
    }

    // insert all index pages into EmptyChain
    SAPDBERR_ASSERT_STATE( m_EmptyEntryChain.IsEmpty() );

    EntryIterator entryIter      = Level1Begin();
    const EntryIterator entryEnd = Level1End();

    while(entryIter != entryEnd) {
        m_EmptyEntryChain.InsertEnd(m_EntryVector.GetIndex(entryIter));
        ++entryIter;
    }

    SetRootPage(rootpage);

    // -------------------------------------------------------------------------
    // scan root page and integrate other index pages
    // -------------------------------------------------------------------------
    PageEntryIterator   iter     = rootpage.Begin();
    PageEntryIterator   end      = rootpage.End();

    while (iter != end)
    {
        if (iter.EntryInUse())
        {
            const IOMan_BlockAddress    addr = iter.GetBlockAddress();
            IndexPage                   page;

            pager.Read( addr, page );
            page.WriteParentReference( Converter_ParentReference(RootEntryNo(), iter.GetPosition()) );
            InsertPage( page );

        }
        ++iter;
    }
    SAPDBERR_ASSERT_STATE( Verify() );
}

// -----------------------------------------------------------------------------

void
Converter_Index::SetRootPage(IndexPage &page)
{
    m_EntryVector[RootEntryNo()].AssignPage(page, page.DetermineFreeEntryCount());
}

// -----------------------------------------------------------------------------

Converter_Index::IndexEntryNo
Converter_Index::InsertPage( IndexPage  &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::InsertPage", Converter_Trace, 6 );

    // locking of entry chains is in scope of calling method!!

    if( m_EmptyEntryChain.IsEmpty() )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_MAXIMUN_SIZE_REACHED );
        RTE_Crash( errMsg );
    };

    IndexEntryNo entryno = m_EmptyEntryChain.Begin().GetPosition();
    m_EmptyEntryChain.Remove(entryno);

    m_EntryVector[entryno].AssignPage(page, page.DetermineFreeEntryCount());

    if (page.HasFreeEntryAvailable())
    {
        m_PartiallyUsedEntryChain.InsertEnd(entryno);
    }
    else
    {
        m_FullEntryChain.InsertEnd(entryno);
    }

    return entryno;
}

// -----------------------------------------------------------------------------

void
Converter_Index::RemoveEmptyLeaves(
    const RTETask_ITask                         &task,
    const Converter_Version                     &snapShotVersion,
    Container_Vector<Converter_ParentReference> &parentRefs)
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::RemoveEmptyLeaves", Converter_Trace, 6 );

    Container_Vector<Converter_ParentReference>::Iterator       iter    = parentRefs.Begin();
    const Container_Vector<Converter_ParentReference>::Iterator iterEnd = parentRefs.End();

    // lock entry chains
    Converter_Lock lock( task );

    for (;iter < iterEnd; iter++)
    {
        Converter_ParentReference parentReference = *iter;

        IndexEntryNo      entryno = parentReference.GetIndexEntryNo();
        IndexEntry        &entry  = GetEntry(entryno);

        // -------------------------------------------------------------------------
        // get reference to converter index page
        // -------------------------------------------------------------------------
        IndexPage         &indexpage   = entry.GetPage();
        SAPDB_Bool         pageWasFull = entry.PageIsFull();

        indexpage.ReleaseEntry(parentReference.GetPageEntryNo());
        entry.IncFreePageEntryCount();

        if (pageWasFull)
        {
            m_FullEntryChain.Remove(entryno);
            m_PartiallyUsedEntryChain.InsertFront(entryno);
        }

        if (entry.PageIsEmpty())
        {
            m_PartiallyUsedEntryChain.Remove(entryno);
            m_EmptyEntryChain.InsertEnd(entryno);

            // -------------------------------------------------------------------------
            // get reference to converter root page
            // -------------------------------------------------------------------------
            IndexEntry  &rootEntry = GetRootEntry();
            IndexPage   &rootPage  = rootEntry.GetPage();

            Converter_ParentReference rootReference = indexpage.ReadParentReference();

            rootPage.ReleaseEntry(rootReference.GetPageEntryNo());

            // is old block address needed for frozen converter version
            if(( ! snapShotVersion.IsValid()) || ( indexpage.ReadVersion() > snapShotVersion ))
            {
                FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( indexpage.ReadBlockAddress() );
            }

            Converter_PageFrameAllocator    frameAlloc( task );
            frameAlloc.Free(indexpage.GetFrame());

            rootEntry.IncFreePageEntryCount();
        }
    }
    parentRefs.Clear();

    SAPDBERR_ASSERT_STATE( Verify() );
}

// -----------------------------------------------------------------------------

Converter_ParentReference
Converter_Index::RequestFreeParentReference( const RTETask_ITask    &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::RequestFreeParentReference", Converter_Trace, 6 );

    IndexEntryNo    entryno;
    PageEntryNo     pageentryno;

    // lock entry chains
    Converter_Lock lock( task );

    if (! m_PartiallyUsedEntryChain.IsEmpty())
    {
        // -------------------------------------------------------------------------
        // get reference to index entry
        // -------------------------------------------------------------------------
        PartiallyUsedEntryIterator iter = m_PartiallyUsedEntryChain.Begin();
        entryno                         = iter.GetPosition();
        IndexEntry&  entry              = *iter;

        // -------------------------------------------------------------------------
        // get reference to converter index page
        // -------------------------------------------------------------------------
        IndexPage& page      = entry.GetPage();

        // -------------------------------------------------------------------------
        // get unused entry from page
        // -------------------------------------------------------------------------
        pageentryno          = page.ReserveEntry();

        // -------------------------------------------------------------------------
        // adjust entry state
        // -------------------------------------------------------------------------

        entry.DecFreePageEntryCount(); // PTS 1112982 2001-12-05

        // -------------------------------------------------------------------------
        // rechain entry if necessary
        // -------------------------------------------------------------------------

        if (entry.PageIsFull())
        {
            // it seems that we have popped the last free entry on the page.
            // rechain entry from partially used to full
            m_PartiallyUsedEntryChain.Remove(entryno);
            m_FullEntryChain.InsertFront(entryno);
        }
    }
    else
    {
        // -------------------------------------------------------------------------
        // get reference to converter root page
        // -------------------------------------------------------------------------
        IndexEntry  &rootentry = GetRootEntry();

        if( rootentry.PageIsFull() ) // PTS 1113242 TS 2001-12-19
        {
            Converter_Exception errMsg( __CONTEXT__, CONVERTER_MAXIMUN_SIZE_REACHED );
            RTE_Crash( errMsg );
        };

        IndexPage   &rootpage  = rootentry.GetPage();

        // -------------------------------------------------------------------------
        // get unused entry from root page
        // -------------------------------------------------------------------------
        PageEntryNo rootpageentryno = rootpage.ReserveEntry();

        rootentry.DecFreePageEntryCount(); // PTS 1113242 TS 2001-12-19

        // -------------------------------------------------------------------------
        // construct new index page
        // -------------------------------------------------------------------------
        Converter_PageFrameAllocator frameAlloc;

        IndexPage page( frameAlloc.New(), Converter_Version(),
                        Converter_ParentReference(RootEntryNo(), rootpageentryno));

        // -------------------------------------------------------------------------
        // get reference to index entry
        // -------------------------------------------------------------------------
        entryno              = InsertPage(page);

        IndexEntry&  entry   = GetEntry(entryno);

        // -------------------------------------------------------------------------
        // get unused entry from page
        // -------------------------------------------------------------------------
        pageentryno          = page.ReserveEntry();

        // -------------------------------------------------------------------------
        // adjust entry state
        // -------------------------------------------------------------------------
        entry.DecFreePageEntryCount();
    }

    SAPDBERR_ASSERT_STATE( Verify() );

    return Converter_ParentReference(entryno, pageentryno);
}

// -----------------------------------------------------------------------------

void Converter_Index::IntegratePage(
    const RTETask_ITask &task,
    LeafPage            &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::IntegratePage", Converter_Trace, 6 );

    IOMan_BlockAddress          addr = page.ReadBlockAddress();
    Converter_ParentReference   ref  = page.ReadParentReference();

    if (ref.GetIndexEntryNo().IsInvalid())
    {
        ref = RequestFreeParentReference( task );
        page.WriteParentReference( ref );
    }

    IndexEntry&        entry     = GetEntry(ref.GetIndexEntryNo());
    IndexPage&         indexpage = entry.GetPage();

    SAPDBERR_ASSERT_STATE(indexpage.EntryInUse(ref.GetPageEntryNo()));

    // -------------------------------------------------------------------------
    // write the page's address to the index page
    // -------------------------------------------------------------------------
    IOMan_BlockAddress dummy;
    indexpage.WriteBlockAddress(ref.GetPageEntryNo(), addr, dummy);

    SAPDBERR_ASSERT_STATE( Verify() );
}

// -----------------------------------------------------------------------------


IOMan_BlockAddress Converter_Index::Flush(
    Converter_Pager          &pager,
    SAPDB_UInt               &NumPages,
    SAPDB_UInt               &NumIO)
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::Flush", Converter_Trace, 6 );

    IndexEntry&         rootentry = GetRootEntry();
    IndexPage&          rootpage  = rootentry.GetPage();
    EntryIterator       iter      = Level1Begin();
    const EntryIterator endIter   = Level1End();
    IOMan_BlockAddress  addr;
    IOMan_BlockAddress  oldaddr;

    NumPages = 0;
    NumIO    = 0;

    // write out all level1 index pages:
    while (iter != endIter)
    {
        if( (*iter).PageIsAssigned() )
        {
            IndexEntry&        entry   = *iter;
            IndexPage&         page    = entry.GetPage();
            Converter_ParentReference    parent  = page.ReadParentReference();

            SAPDBERR_ASSERT_STATE(parent.GetIndexEntryNo() == RootEntryNo());

            PageEntryNo        entryno = parent.GetPageEntryNo();

            pager.Write( page );
            NumPages++;
            addr = page.ReadBlockAddress();

            // write new block address to root page:
            rootpage.WriteBlockAddress(entryno, addr, oldaddr);
        }
        ++iter;
    }

    pager.Write( rootpage );
    NumPages++;
    NumIO = NumPages;
    addr  = rootpage.ReadBlockAddress();

    SAPDBERR_ASSERT_STATE( Verify() );

    return addr;
}

// -----------------------------------------------------------------------------

void Converter_Index::CreateInputQueues(
    Container_Vector<BlockVector> &volumeList,
    const SAPDB_UInt               ConverterPageSize)
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::CreateInputQueues", Converter_Trace, 6 );

    // get number of used dataVolumes
    SAPDB_Int const numVolumes = IOMan_IDataInfo::GetInstance().MaxConfigurableDataVolumes();

    // create container of empty queues
    BlockVector initBlockVector(m_Allocator);
    volumeList.Initialize(numVolumes,initBlockVector);

    // init
    for (Container_Vector<BlockVector>::Iterator myIter = volumeList.Begin();
            myIter != volumeList.End();
            myIter++) {
        // calculate approximate number of converterPages per device (iteratorIndexPos+1)
        IOMan_BlockCount converterBlocks = IOMan_IDataInfo::GetInstance().GetDataVolumeSize(volumeList.GetIndex(myIter)+1) /
                                           ConverterPageSize;
        if(!myIter->Reserve(converterBlocks)){
            Converter_Exception errMsg(__CONTEXT__,CONVERTER_NO_MORE_MEMORY,
                                       SAPDB_ToString( Converter_MemoryAllocationFailed, _T_d ));
            RTE_Crash( errMsg );
        }
    }

    // iterate over all ConverterIndexes
    for (Converter_IndexIterator LeafPageBlockAddressIter = GetIterator();
            LeafPageBlockAddressIter.CanAdvance();
            ++LeafPageBlockAddressIter)
    {   // fill queues

        if(! LeafPageBlockAddressIter.EntryInUse() )
            continue;

        IOMan_BlockAddress block = LeafPageBlockAddressIter.GetBlockAddress();
        SAPDB_UInt listIndex = block.GetDeviceNo() - 1;  // container index starts with '0', listIndex with '1'

        // if IsFull() -> increase capacity by factor 1.1 -> +10%
        if(volumeList[listIndex].IsFull()){
            if (!volumeList[listIndex].Reserve((SAPDB_UInt) (volumeList[listIndex].GetCapacity() * 1.1) + 1)){
                Converter_Exception errMsg(__CONTEXT__,CONVERTER_NO_MORE_MEMORY,
                                           SAPDB_ToString( Converter_MemoryAllocationFailed, _T_d ));
                RTE_Crash( errMsg );
            }
        }

        Converter_ParentReference parRef(GetParentReference(LeafPageBlockAddressIter));
        volumeList[listIndex].InsertEnd( Converter_LeafReference(block, parRef));
    }

    // Sort queues!
    for (Container_Vector<BlockVector>::Iterator volumeIter = volumeList.Begin();
            volumeIter != volumeList.End();
            volumeIter++) {
        if (volumeIter->GetSize() > 0) {
            volumeIter->QuickSort();
        }
    }
}

// -----------------------------------------------------------------------------

void Converter_Index::Shutdown()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::Shutdown", Converter_Trace, 6 );

    if (m_EntryVector.GetSize() > 0)
    {
        Converter_PageFrameAllocator frameAlloc;

        // free root page:
        IndexEntry&   rootentry = GetRootEntry();
        IndexPage&    rootpage  = rootentry.GetPage();
        frameAlloc.Free(rootpage.GetFrame());

        // free level1 index pages:
        EntryIterator iter      = Level1Begin();
        const EntryIterator end = Level1End();

        while (iter != end)
        {
            if( (*iter).PageIsAssigned() )
            {
                IndexEntry&        entry   = *iter;
                IndexPage&         page    = entry.GetPage();
                frameAlloc.Free(page.GetFrame());
            }
            ++iter;
        }
    }

    // free/reset other structures:
    m_PartiallyUsedEntryChain.Reset();
    m_FullEntryChain.Reset();
    m_EmptyEntryChain.Reset();
    m_EntryVector.Delete();
}

// -----------------------------------------------------------------------------

void
Converter_Index::Dump( Kernel_Dump     &dump )
{
    Converter_IndexVector::Iterator            iter    = m_EntryVector.Begin();
    const Converter_IndexVector::ConstIterator enditer = m_EntryVector.End();
    ConverterIndexEntry                   cie;
    IOMan_BlockAddress                    addr;

    while( iter != enditer )
    {
        if( ( *iter) .PageIsAssigned() )
        {
            cie.dmpEntryNo  = m_EntryVector.GetIndex( iter );
            addr            = (*iter).GetPage().ReadBlockAddress();
            if( addr.IsValid() )
            {
                cie.dmpDevNo   = addr.GetDeviceNo();
                cie.dmpBlockNo = addr.GetBlockNo();
            }
            else
            {
                cie.dmpDevNo   = 0;
                cie.dmpBlockNo = 0;
            }
            cie.dmpChainPrev = (*iter).GetPredecessor();
            cie.dmpChainNext = (*iter).GetSuccessor();
            cie.dmpFreeEntryCnt = (*iter).GetFreePageEntryCount();
            cie.dmpPageFreeEntryCnt = (*iter).GetPage().DetermineFreeEntryCount();
            cie.dmpEntryCnt = (*iter).GetPage().GetEntryCount() - (*iter).GetFreePageEntryCount();


            SAPDB_Char entryType = 'i';  // invalid

            if(m_EntryVector.GetIndex(iter) == RootEntryNo())
            {
                entryType = 'r';    // root
            }
            if(m_PartiallyUsedEntryChain.IsMember(m_EntryVector.GetIndex(iter)))
            {
                entryType = 'p';    // partially
            }
            if(m_FullEntryChain.IsMember(m_EntryVector.GetIndex(iter)))
            {
                entryType = 'f';    // full
            }
            cie.dmpChainType = entryType;

            dump.InsertEntry( Kernel_Dump::DmpConverterIndex,
                              Kernel_DumpPage::Entry( &cie, sizeof( cie )));
        }
        ++iter;
    }
    iter = m_EntryVector.Begin();
    while( iter != enditer )
    {
        if( ( *iter) .PageIsAssigned() )
        {
            dump.InsertPage( ( *iter ).GetPage() );
        }
        ++iter;
    }
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_Index::Verify() const
{
    Converter_IndexVector::ConstIterator       iter    = m_EntryVector.Begin();
    const Converter_IndexVector::ConstIterator endIter = m_EntryVector.End();

    SAPDB_UInt4 numPartiallyUsedEntries = 0;
    SAPDB_UInt4 numFullEntries = 0;

    while( iter != endIter )
    {
        if( ( *iter ).PageIsAssigned() )
        {
            if( ( *iter ).GetFreePageEntryCount() !=
                    ( *iter ).GetPage().DetermineFreeEntryCount() )
            {
                Converter_Exception errMsg(__CONTEXT__,CONVERTER_FREE_CNT_MISMATCH,
                                           SAPDB_ToString( ( *iter ).GetFreePageEntryCount(), _T_d ),
                                           SAPDB_ToString( ( *iter ).GetPage().DetermineFreeEntryCount(), _T_d ));
                RTE_Message( errMsg );
                return( false );
            }
        }
        ++iter;
    }

    if(! m_PartiallyUsedEntryChain.IsEmpty() )
    {
        EntryChain::ConstIterator       iter    = m_PartiallyUsedEntryChain.Begin();
        const EntryChain::ConstIterator endIter = m_PartiallyUsedEntryChain.End();

        while( iter != endIter )
        {
            if( 0 == ( *iter ).GetFreePageEntryCount() )
            {
                Converter_Exception errMsg(__CONTEXT__,CONVERTER_FULL_PAGE_IN_WRONG_CHAIN,
                                           SAPDB_ToString( ( *iter ).GetFreePageEntryCount(), _T_d ));
                RTE_Message( errMsg );
                return( false );
            }
            if ( ( *iter ).PageIsEmpty() )
            {
                Converter_Exception errMsg(__CONTEXT__,CONVERTER_EMPTY_PAGE_IN_WRONG_CHAIN,
                                           SAPDB_ToString( ( *iter ).GetFreePageEntryCount(), _T_d ));
                RTE_Message( errMsg );
                return( false );
            }
            ++iter;
        }
    }

    if( ! m_FullEntryChain.IsEmpty() )
    {
        EntryChain::ConstIterator       iter    = m_FullEntryChain.Begin();
        const EntryChain::ConstIterator endIter = m_FullEntryChain.End();

        while( iter != endIter )
        {
            if( 0 != ( *iter ).GetFreePageEntryCount() )
            {
                Converter_Exception errMsg(__CONTEXT__,CONVERTER_NOT_FULL_PAGE_IN_WRONG_CHAIN,
                                           SAPDB_ToString( ( *iter ).GetFreePageEntryCount(), _T_d ));
                RTE_Message( errMsg );

                return( false );
            }
            ++iter;
        }
    }
    return( true );
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_Index::IsMember( const IOMan_BlockAddress   &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Index::IsMember", Converter_Trace, 6 );

    EntryIterator          iter = m_EntryVector.Begin();
    const EntryIterator endIter = m_EntryVector.End();

    for( ; iter != endIter; ++iter )
    {
        if( ( *iter).PageIsAssigned() )
        {
            if( block == ( *iter ).GetPage().ReadBlockAddress()){
                return( SAPDB_TRUE );
            }
        }
    }
    return( SAPDB_FALSE );
}
