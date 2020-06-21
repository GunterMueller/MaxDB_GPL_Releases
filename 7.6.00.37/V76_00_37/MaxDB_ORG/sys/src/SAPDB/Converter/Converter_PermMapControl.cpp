/*!
  @file     Converter_PermMapControl.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            permanent dynamic pageNo's to their physical block address
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "Converter/Converter_PermMapControl.hpp"
#include "Converter/Converter_BitMapPage.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_Index.hpp"
#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_MapError.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_Pager.hpp"
#include "Converter/Converter_PermMapSection.hpp"
#include "Converter/Converter_BlockAddressIterator.hpp"

#include "DataAccess/Data_Types.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"

#include "IOManager/IOMan_BlockAddress.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "SAPDBCommon/Messages/SDBMsg_Converter.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


class Converter_ContainerLock
{
public:

    Converter_ContainerLock( RTESync_Spinlock &lock )
    :
    m_Lock( lock )
    {
        m_Lock.Lock();
    };

    ~Converter_ContainerLock()
    {
        m_Lock.Unlock();
    };

private:

    RTESync_Spinlock    &m_Lock;
};


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDB_Bool
Converter_PermMapControl::Initialize(
    const RTETask_ITask  &task,
    const SAPDB_UInt     maxDataPnoInMap,
    const RTE_RegionId   &baseRegionId,
    const SAPDB_UInt     numConverterRegions,
    const SAPDB_UInt     clusterSize,
    const SAPDB_UInt     converterPageSize,
    const SAPDB_Bool     bLastSaveDataSuccessful )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::Initialize", Converter_Trace, 5 );

    SAPDB_UInt  addedPeriods = 0;

    if( ! Converter_MapControl::Init( task, maxDataPnoInMap, baseRegionId,
                                      numConverterRegions, clusterSize, converterPageSize )){
        return( SAPDB_FALSE );
    }

    if( ! m_SectionPermData.Resize( numConverterRegions, Converter_PermMapSectionData(
                                        m_SectionPermData.GetAllocator()))){
        return( SAPDB_FALSE );
    }

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        m_SectionPermData[ sectionNo ].Initialize( m_Map,
                m_NumChangedConverterPages, bLastSaveDataSuccessful );
    }

    Converter_PageFrameAllocator frameAlloc;

    m_UsedPageNoList.Delete( frameAlloc ); // make sure that the list is empty
    m_PageNoList.Delete();           // make sure that the list is empty

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::Shutdown( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::Shutdown", Converter_Trace, 5 );

    {
        Converter_PermMapSection sections[ MAX_CONVERTER_SECTIONS ];

        for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
        {
            sections[ sectionNo ].Lock( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);
            sections[ sectionNo ].Shutdown();
        }

    }
    // Do not destroy section data, because concurrent running
    // task should not fail/crash.
    // m_SectionData.Delete();
    // m_SectionPermData.Delete();

    m_Map.Shutdown();
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::IntegratePage(
    const RTETask_ITask &task,
    Converter_LeafPage  &leafPage,
    SAPDB_UInt          &numPermPagesUsed )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::IntegratePage", Converter_Trace, 5 );


    const Data_PageNo   basePageNo = leafPage.ReadBasePageNo();
    const SAPDB_UInt    sectionNo  = CalculateSectionNo( basePageNo );

    SAPDB_UInt freeEntryCount = 0;
    numPermPagesUsed          = 0;

    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ] );

    Converter_PageEntryIterator         pageEntryIter    = leafPage.Begin();
    const Converter_PageEntryIterator   pageEntryEndIter = leafPage.End();

    for( ; pageEntryIter != pageEntryEndIter; ++pageEntryIter )
    {
        if( ! pageEntryIter.EntryInUse())
        {
            ++freeEntryCount;
            continue;
        }

        const Converter_PageEntryNo entryPos = pageEntryIter.GetPosition();

        if( pageEntryIter.GetPackedBlockAddress().IsReserved())
        {
            leafPage.ReleaseEntry( entryPos );
            ++freeEntryCount;
            continue;
        }

        ++numPermPagesUsed;

        // Maintain the number of changed data pages since the last save data

        const bool bPageUpdated       = leafPage.SavePagesRequested( entryPos );
        const bool bPendingPageUpdate = leafPage.SavePagesPending( entryPos );

        section.ReInitialzeUpdatePages( bPageUpdated, bPendingPageUpdate );
    }
    m_Map.IntegratePage( section, leafPage, basePageNo, numPermPagesUsed, freeEntryCount );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::RemoveEmptyLeaves(
    const RTETask_ITask     &task,
    const Converter_Version &snapShotVersion,
    Converter_Index         &index)
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::RemoveEmptyLeafs", Converter_Trace, 5 );

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ] );

        index.RemoveEmptyLeaves( task, snapShotVersion, section.GetParentReferences());
    }
}

/*---------------------------------------------------------------------------*/

Data_PageNo
Converter_PermMapControl::Flush(
    const RTETask_ITask &task,
    Converter_Index     &index,
    Converter_Pager     &pager )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::Flush", Converter_Trace, 5 );

    SAPDB_UInt totalPages = 0;
    SAPDB_UInt totalIO    = 0;

    Data_PageNo maxUsedPageNo = 0;
    Data_PageNo maxUsedPageNoInSection = 0;

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        this->FlushSection( task, index, pager, sectionNo, totalPages,
                            totalIO, maxUsedPageNoInSection );

        maxUsedPageNo = SAPDB_MAX( maxUsedPageNo, maxUsedPageNoInSection );
    };

    return( maxUsedPageNo );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::Expand(
    const RTETask_ITask                             &task,
    const SAPDB_UInt                                newNumAllPages,
    Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
    Kernel_SynchronizedTaskWaitList                 &expandWaitQueue )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::Expand", Converter_Trace, 5 );

    Converter_PermMapSection sections[ MAX_CONVERTER_SECTIONS ];
    SAPDB_UInt               sectionNo = 0;

    /* try to get exclusive access to all sections                                           */
    /* during a savepoint iterators may exist which store a particular state of the map but  */
    /* are not protected by a region. To avoid that the data stored within the iterators     */
    /* get wrong expanding (=reallocation) of the maps is prohibited                         */

    for( sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        sections[ sectionNo ].Lock( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ] );

        if( sections[ sectionNo ].IsSavepointActive() )
        {
            expandWaitQueue.InsertTask( task.ID(), waitContext );
            return Converter_SavePointActive;
        }
    }

    SAPDB_Int  addPeriodCount; // PTS 1126701 TS 2003-12-19

    if( ! m_Map.Expand( newNumAllPages, addPeriodCount )){
        return Converter_MemoryAllocationFailed;
    }

    if( 0 >= addPeriodCount ) // PTS 1126701 TS 2003-12-19
        return Converter_Ok;

    for( sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        m_Map.ExpandOneSection( sections[ sectionNo ], addPeriodCount );
    }

    return Converter_Ok;
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::MarkPermPagesForSaveData(
    const RTETask_ITask &task,
    Msg_List            &errMsg,
    SAPDB_UInt          &numPermConvPages,
    SAPDB_UInt          &numPermPagesForBackUp )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::MarkPermPagesForSaveData", Converter_Trace, 5 );

    numPermConvPages      = 0;
    numPermPagesForBackUp = 0;

    const Converter_ReturnCode retCode = InitializePageNoList( task, errMsg );

    if( Converter_Ok != retCode )
    {
        errMsg.Overrule( Msg_List( Msg_List::Error,
                                   SDBMSG_CONVERTER_ALLOCATION_OF_BACKUP_PNOLIST_FAILED ));
        return retCode;
    }

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        /* lock the current section to keep the chains of PartiallyUsedEntries  */
        /* and FullEntries constant while the iterators walk along this chains  */

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        numPermConvPages += section.GetPageCount();

        section.FreezeNumUpdatedPages();

        // iterator over partially used entry chain

        Converter_PermMapSection::PartiallyUsedEntryIterator
        partUsedIter = section.PartiallyUsedEntriesBegin();

        const Converter_PermMapSection::PartiallyUsedEntryIterator
        partUsedEndIter = section.PartiallyUsedEntriesEnd();


        for( ; partUsedIter != partUsedEndIter; ++partUsedIter )
        {
            SAPDBERR_ASSERT_STATE( ! (*partUsedIter).PageIsFull() );

            this->MarkPermPagesForSaveData( task, section,(*partUsedIter).GetPage(),
                                            numPermPagesForBackUp );
        }

        // iterator over full used entry chain

        Converter_PermMapSection::FullEntryUsedIterator
        fullUsedIter = section.FullUsedEntriesBegin();

        const Converter_PermMapSection::FullEntryUsedIterator
        fullUsedEndIter = section.FullUsedEntriesEnd();


        for( ; fullUsedIter != fullUsedEndIter; ++fullUsedIter )
        {
            SAPDBERR_ASSERT_STATE( (*fullUsedIter).PageIsFull() );

            this->MarkPermPagesForSaveData( task, section,(*fullUsedIter).GetPage(),
                                            numPermPagesForBackUp );
        }
    }
    return( retCode );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::MarkPermPagesForSavePages(
    const RTETask_ITask     &task,
    Msg_List                &errMsg,
    SAPDB_UInt              &numPermConvPages,
    SAPDB_UInt              &numPermPagesForBackUp,
    const Converter_Version &converterVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::MarkPermPagesForSavePages", Converter_Trace, 5 );

    numPermConvPages      = 0;
    numPermPagesForBackUp = 0;

    const Converter_ReturnCode retCode = InitializePageNoList( task, errMsg );

    if( Converter_Ok != retCode )
    {
        errMsg.Overrule( Msg_List( Msg_List::Error,
                                   SDBMSG_CONVERTER_ALLOCATION_OF_BACKUP_PNOLIST_FAILED ));
        return retCode;
    }

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        /* lock the current section to keep the chains of PartiallyUsedEntries  */
        /* and FullEntries constant while the iterators walk along this chains  */

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        numPermConvPages += section.GetPageCount();

        // iterator over partially used entry chain

        Converter_PermMapSection::PartiallyUsedEntryIterator
        partUsedIter = section.PartiallyUsedEntriesBegin();

        const Converter_PermMapSection::PartiallyUsedEntryIterator
        partUsedEndIter = section.PartiallyUsedEntriesEnd();


        for( ; partUsedIter != partUsedEndIter; ++partUsedIter )
        {
            SAPDBERR_ASSERT_STATE( ! (*partUsedIter).PageIsFull() );

            this->MarkPermPagesForSavePages( task, section, (*partUsedIter).GetPage(),
                                             numPermPagesForBackUp, converterVersion );
        }

        // iterator over full used entry chain

        Converter_PermMapSection::FullEntryUsedIterator
        fullUsedIter = section.FullUsedEntriesBegin();

        const Converter_PermMapSection::FullEntryUsedIterator
        fullUsedEndIter = section.FullUsedEntriesEnd();


        for( ; fullUsedIter != fullUsedEndIter; ++fullUsedIter )
        {
            SAPDBERR_ASSERT_STATE( (*fullUsedIter).PageIsFull() );

            this->MarkPermPagesForSavePages( task, section, (*fullUsedIter).GetPage(),
                                             numPermPagesForBackUp, converterVersion );
        }
    }
    return( retCode );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::MarkPermPagesChanged( const RTETask_ITask  &task )
{

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        /* lock the current section to keep the chains of PartiallyUsedEntries  */
        /* and FullEntries constant while the iterators walk along this chains  */

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        const Converter_PermMapSection::PartiallyUsedEntryIterator PartiallyUsedEntryEndIter =
            section.PartiallyUsedEntriesEnd();

        Converter_PermMapSection::PartiallyUsedEntryIterator PartiallyUsedEntryIter =
            section.PartiallyUsedEntriesBegin ();

        for (;PartiallyUsedEntryIter != PartiallyUsedEntryEndIter; ++PartiallyUsedEntryIter)
        {
            section.MarkPageAsChanged(PartiallyUsedEntryIter.GetPosition());
        }

        const Converter_PermMapSection::FullEntryUsedIterator FullEntriesEndIter =
            section.FullUsedEntriesEnd();

        Converter_PermMapSection::FullEntryUsedIterator FullEntryIter =
            section.FullUsedEntriesBegin();

        for(;FullEntryIter != FullEntriesEndIter; ++FullEntryIter)
        {
            section.MarkPageAsChanged(FullEntryIter.GetPosition());
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_PermMapControl::GetBitMapPage( Converter_BitMapPage   &bitMapPage )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::GetBitMapPage", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE( bitMapPage.IsAssigned());

    Data_PageFrame   frame;

    {
        Converter_ContainerLock   lock( m_ContainerLock );

        if( m_UsedPageNoList.IsEmpty())
            return false;

        const SAPDB_UInt newSize = m_UsedPageNoList.GetSize() -1;

        Converter_UsedPageNoContainer::Iterator iter = m_UsedPageNoList.Position( newSize );
        frame = ( *iter ).GetFrame();
        m_UsedPageNoList.Resize( newSize );
    }

    SAPDBERR_ASSERT_STATE( frame.IsAssigned());

    const SAPDB_UInt frameSize = frame.GetLength();
    SAPDB_MemCopyNoCheck( bitMapPage.GetFramePtr(), frame.GetPointer( 0, frameSize ), frameSize );

    Converter_PageFrameAllocator frameAllocator;
    frameAllocator.Free( frame, false );

    return true;
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::RemoveBitMapPages()
{
    Converter_PageFrameAllocator frameAllocator;

    Converter_ContainerLock lock( m_ContainerLock );

    m_UsedPageNoList.Delete( frameAllocator );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::ExtractArchiveConverterForSaveData( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::ExtractArchiveConverterForSaveData", Converter_Trace, 5 );

    Converter_ReturnCode returnCode = Converter_Ok;

    Converter_PageFrameAllocator frameAlloc( task );

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        /* lock the current section to keep the chains of PartiallyUsedEntries  */
        /* and FullEntries constant while the iterators walk along this chains  */

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        const Converter_PermMapSection::PartiallyUsedEntryIterator PartiallyUsedEntryEndIter =
            section.PartiallyUsedEntriesEnd();

        Converter_PermMapSection::PartiallyUsedEntryIterator PartiallyUsedEntryIter =
            section.PartiallyUsedEntriesBegin ();

        for (;PartiallyUsedEntryIter != PartiallyUsedEntryEndIter; ++PartiallyUsedEntryIter)
        {
            SAPDBERR_ASSERT_STATE( ! (*PartiallyUsedEntryIter).PageIsFull() );

            if (!m_ArchiveConverter.Append((*PartiallyUsedEntryIter).GetPage(), frameAlloc))
            {
                returnCode = Converter_FrameAllocationFailed;
            }
        }

        const Converter_PermMapSection::FullEntryUsedIterator FullEntriesEndIter =
            section.FullUsedEntriesEnd();

        Converter_PermMapSection::FullEntryUsedIterator FullEntryIter =
            section.FullUsedEntriesBegin();

        for(;FullEntryIter != FullEntriesEndIter; ++FullEntryIter)
        {
            SAPDBERR_ASSERT_STATE( (*FullEntryIter).PageIsFull() );

            if (!m_ArchiveConverter.Append((*FullEntryIter).GetPage(), frameAlloc))
            {
                returnCode = Converter_FrameAllocationFailed;
            }
        }
    }
    return returnCode;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_PermMapControl::GetArchiveLeafPage( Converter_LeafPage &leafPage )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::GetArchiveLeafPage", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE( leafPage.IsAssigned());

    Data_PageFrame   frame;

    {
        Converter_ContainerLock   lock( m_ContainerLock );

        if(  m_ArchiveConverter.IsEmpty())
            return false;

        const SAPDB_UInt newSize = m_ArchiveConverter.GetSize() -1;

        Converter_ArchiveContainer::Iterator iter = m_ArchiveConverter.Position( newSize );
        frame = ( *iter ).GetFrame();
        m_ArchiveConverter.Resize( newSize );
    }

    SAPDBERR_ASSERT_STATE( frame.IsAssigned());

    const SAPDB_UInt frameSize = frame.GetLength();
    SAPDB_MemCopyNoCheck( leafPage.GetFramePtr(), frame.GetPointer( 0, frameSize ), frameSize );

    Converter_PageFrameAllocator frameAllocator;
    frameAllocator.Free( frame, false );

    return true;
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::RemoveArchiveLeafPages()
{
    Converter_PageFrameAllocator frameAllocator;

    Converter_ContainerLock   lock( m_ContainerLock );

    m_ArchiveConverter.Delete( frameAllocator );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::InitializePageNoList(
    const RTETask_ITask  &task,
    Msg_List             &errMsg )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::InitializePageNoList", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_PageNoList.IsEmpty() );

    /* find maximum used page no */
    Data_PageNo  maxUsedPageNo             = 0;
    SAPDB_UInt   numUsedConverterLeafPages = 0;


    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        maxUsedPageNo = SAPDB_MAX( section.GetMaxUsedPageNo(), maxUsedPageNo );
        numUsedConverterLeafPages += section.GetPageCount();
    }

    return m_PageNoList.Initialize( errMsg, maxUsedPageNo, numUsedConverterLeafPages,
                                    m_Map.GetLeafPageCapacity());
}

/*---------------------------------------------------------------------------*/

bool
Converter_PermMapControl::SetBlockAddressOfRestoredPage(
    const RTETask_ITask&        task,
    Msg_List&                   msgList,
    const Data_PageNo           pageNo,
    const Converter_Version&    snapShotVersion,
    const IOMan_BlockAddress&   block,
    SAPDB_Bool                  bRestoreDataActive )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::SetBlockAddressOfRestoredPage", Converter_Trace, 5 );

    bool                bPageUpdated       = false;
    bool                bPendingPageUpdate = false;
    IOMan_BlockAddress  oldBlock;

    SAPDBTRACE_WRITELN( Converter_Trace, 5, "Restore  " <<
                        ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                         "perm pageNo " : "static pageNo " ) << pageNo );
    if( pageNo > this->MaxPageNo())
    {
        msgList = Msg_List(
                      Msg_List::Error, SDBMSG_CONVERTER_INVALID_RECOVERY_PAGENO,
                      Msg_Arg(SDBMSGTAG_CONVERTER_INVALID_RECOVERY_PAGENO__PAGENO, pageNo),
                      Msg_Arg(SDBMSGTAG_CONVERTER_INVALID_RECOVERY_PAGENO__PERSISTENT_TYPE,
                              SAPDB_ToString( GetRecoveryMode())),
                      Msg_Arg(SDBMSGTAG_CONVERTER_INVALID_RECOVERY_PAGENO__PAGENO_1, MaxPageNo()));
        return false;
    }
    {
        const SAPDB_UInt sectionNo = CalculateSectionNo( pageNo );

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        const Converter_Map::RequestSpecifiedNewPageNoRc newRc =
            m_Map.RequestSpecifiedNewPageNo( section, pageNo );

        if(( Converter_Map::rsnpRcOkay != newRc )&& ( Converter_Map::rsnpRcPageNoInUse != newRc ))
        {
            msgList = Msg_List(
                Msg_List::Error, SDBMSG_CONVERTER_RECOVERY,
                Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__PAGENO, pageNo ),
                Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__PERSISTENT_TYPE, SAPDB_ToString( GetRecoveryMode())),
                Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__ERRORTEXT, SAPDB_ToString((Converter_ReturnCode)newRc )),
                Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__VOLUME_ID, block.GetVolumeNo()),
                Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__BLOCK_NO, block.GetBlockNo()),
                Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__BOOL, bRestoreDataActive ));
            return false;
        }
        const Converter_Map::SetPermBlockRc setRc =
            m_Map.SetPermBlockAddress( section, pageNo, block, oldBlock,
                                       bPageUpdated, bPendingPageUpdate, bRestoreDataActive );
        if( Converter_Map::spbaRcOkay != setRc )
        {
            msgList = Msg_List(
                     Msg_List::Error, SDBMSG_CONVERTER_RECOVERY,
                     Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__PAGENO, pageNo ),
                     Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__PERSISTENT_TYPE, SAPDB_ToString( GetRecoveryMode())),
                     Msg_Arg(SDBMSGTAG_CONVERTER_RECOVERY__ERRORTEXT, SAPDB_ToString((Converter_ReturnCode)setRc )),
                     Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__VOLUME_ID, block.GetVolumeNo()),
                     Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__BLOCK_NO, block.GetBlockNo()),
                     Msg_OptArg(SDBMSGTAG_CONVERTER_RECOVERY__BOOL, bRestoreDataActive ));
            return false;
        }
        if( ! bRestoreDataActive ){
            section.IncrementUpdatedPages( bPageUpdated, bPendingPageUpdate );
        }
    }
    if( ! oldBlock.IsValid()){
        return true;
    }
    if( snapShotVersion.IsValid() && ( ! bRestoreDataActive )) // == restore pages
    {
        // without reading the converter version of the page it's not possible
        // to decide whether the old block is needed for the frozen version,
        // therefore the old block is not set to free. If the block is not
        // needed and therefore not part of the frozen converter, the block
        // is set to free in the FBM at the next restart!

        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block: " <<
                            oldBlock.GetDeviceNo() << "/" << oldBlock.GetBlockNo() <<
                            " of" << ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                                      " permanent pageNo " : " static pageNo " ) << pageNo <<
                            " is not marked as free");
        return true;
    }

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "Old block: " <<
                        oldBlock.GetDeviceNo() << "/" << oldBlock.GetBlockNo() <<
                        " of" << ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                                  " permanent pageNo " : " static pageNo " ) << pageNo <<
                        " is marked as free");

    FBM_IManager::Instance().SetBlockStateToFree( task.ID(), oldBlock );
    return true;
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::RestoreUsedPageNos(
    const RTETask_ITask         &task,
    const Converter_BitMapPage  &usedPageNoBitMapPage,
    const Converter_Version     &snapShotVersion,
    SAPDB_Int                   &diffNumUsedPages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::RestoreUsedPageNos", Converter_Trace, 5 );

    SAPDB_UInt  numUsedPageBeforeRestore;

    diffNumUsedPages = 0;

    for (Converter_BitMapPage::ConstIterator slotIter = usedPageNoBitMapPage.Begin();
            slotIter != usedPageNoBitMapPage.End(); ++slotIter)
    {
        const SAPDB_UInt sectionNo = CalculateSectionNo( slotIter.ReadDataPageNo());

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        numUsedPageBeforeRestore = section.GetNumUsedDataPages();

        const Converter_ReturnCode retCode =
            m_Map.RestoreUsedPageNos( section, snapShotVersion, slotIter );

        /* calculated number of pages which is created and deleted respectively by restoring the used pages */
        diffNumUsedPages += ( section.GetNumUsedDataPages() - numUsedPageBeforeRestore );

        switch( retCode )
        {
        case Converter_Ok:
            break;
        case Converter_FrameAllocationFailed:
            {
                Converter_Exception noMoreMemory (__CONTEXT__,CONVERTER_NO_MORE_MEMORY,
                                                  SAPDB_ToString( retCode, _T_d ));
                RTE_Crash( noMoreMemory );
            }
            break;
        default:
            {
                Converter_Exception unknownError(__CONTEXT__,CONVERTER_UNEXPECTED_ERROR,
                                                 SAPDB_ToString( retCode, _T_d ));
                RTE_Crash( unknownError );
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::FinishRestoreUsedPageNos(
    const RTETask_ITask     &task,
    const Converter_Version &snapShotVersion,
    SAPDB_Int               &diffNumUsedPages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::FinishRestoreUsedPageNos", Converter_Trace, 5 );

    SAPDB_Int   numUsedPageBeforeRestore;

    diffNumUsedPages = 0;

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        numUsedPageBeforeRestore = section.GetNumUsedDataPages();

        m_Map.FinishSectionRestoreUsedPageNos( section, snapShotVersion );

        /* calculated number of pages which is created and deleted respectively by restoring the used pages */
        diffNumUsedPages += ( section.GetNumUsedDataPages() - numUsedPageBeforeRestore );
    }
    m_Map.FinishRestoreUsedPageNos();
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::SaveDataFinishedSuccessfuly( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::SaveDataFinishedSuccessfuly", Converter_Trace, 5 );

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        section.ClearFrozenNumUpdatedPages();
        section.SetStateOfLastSaveDataToSuccessful();
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::SaveDataFinishedUnSuccessfuly( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::SaveDataFinishedUnSuccessfuly", Converter_Trace, 5 );

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        section.SetStateOfLastSaveDataToUnSuccessful();
    }
}

/*---------------------------------------------------------------------------*/

Data_PageNo
Converter_PermMapControl::SetSavepointActiveFlag(
    const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::SetSavepointActiveFlag", Converter_Trace, 5 );

    Data_PageNo  maxUsedPageNo = 0;

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        section.SetSavepointActive();
        maxUsedPageNo = SAPDB_MAX( maxUsedPageNo, section.GetMaxUsedPageNo() );
    }
    return( maxUsedPageNo );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::ReSetSavepointActiveFlag (
    const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::ReSetSavepointActiveFlag", Converter_Trace, 5 );

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        section.ResetSavepointActive();

        SAPDBERR_ASSERT_STATE( section.GetParentReferences().IsEmpty() );
    }
}

/*---------------------------------------------------------------------------*/


void
Converter_PermMapControl::FreePageNosAfterSavepoint(
    const RTETask_ITask     &task,
    const Converter_Version &snapShotVersion )

{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::FreePageNosAfterSavepoint", Converter_Trace, 5 );

    IOMan_BlockAddress  blockAddress;
    bool                bPageUpdated       = false;
    bool                bPendingPageUpdate = false;


    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        Converter_PermMapSection::FreePageListIterator       iter;
        const Converter_PermMapSection::FreePageListIterator endIter = section.FreePageNoListEnd();

        for( iter = section.FreePageNoListBegin(); iter != endIter; ++iter )
        {
            const Data_PageNo pageNo = ( *iter ).GetPageNo();

            SAPDBTRACE_WRITELN( Converter_Trace, 5, "Free " <<
                                ((GetRecoveryMode().UseDynamicPageAddressing()) ? "permanent " : "static " ) <<
                                "pageNo: " << pageNo );

            const Converter_ReturnCode retCode = m_Map.FreePermPageNo( section, snapShotVersion,
                                                 pageNo, blockAddress, bPageUpdated, bPendingPageUpdate );
            if( Converter_Ok != retCode )
            {
                Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode );
                RTE_Crash( mapError );
            }

            section.DecrementUpdatedPages( bPageUpdated, bPendingPageUpdate );

            /* if the block address is not valid the page number was  */
            /* reserved but the page had not been written to a volume */

            if( blockAddress.IsValid() )
            {
                if( snapShotVersion.IsValid() && (( *iter ).GetPageVersion() <= snapShotVersion )){

                    SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block of " <<
                                        ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                                         "permanent pageNo " : "static pageNo " ) << pageNo <<
                                        " is not marked as free");

                    continue; // oldBlock is needed for frozen version
                }
                else
                    FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( task.ID(), blockAddress);
            }
        }
        section.GetFreeList().Clear();
    }
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::GetBlockAddress(
    const RTETask_ITask &task,
    const Data_PageNo   pageNo,
    IOMan_BlockAddress  &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::GetBlockAddress", Converter_Trace, 5 );

    const SAPDB_UInt sectionNo = CalculateSectionNo( pageNo );

    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

    // Check if the wanted page no is already released, but the converter is
    // not updated because of a running savepoint.

    if( section.GetFreeList().IsContained( pageNo ))
    {
        block.Invalidate();
        return( Converter_NoConverterEntry );
    }
    return( m_Map.GetBlockAddress( pageNo, block ));
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PermMapControl::GetNewPositionForWrite(
    const RTETask_ITask     &task,
    const Data_PageNo       pageNo,
    const Converter_Version &pageConverterVersion,
    const Converter_Version &snapShotVersion,
    const SAPDB_Bool        bReqSequential,
    IOMan_BlockAddress      &newBlock )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::GetNewPositionForWrite", Converter_Trace, 5 );

    // precondition: pageConverterVersion less than current converter version or invalid

    bool                bPageUpdated;
    bool                bPendingPageUpdate;
    IOMan_BlockAddress  oldBlock;
    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    newBlock = FBM_IManager::Instance().GetFreeBlock( task.ID() , bReqSequential );

    {

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        const Converter_ReturnCode retCode =
            (Converter_ReturnCode) m_Map.SetPermBlockAddress( section, pageNo,
                    newBlock, oldBlock, bPageUpdated, bPendingPageUpdate );
        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode);
            RTE_Crash( mapError );
        }

        if( pageConverterVersion.IsInvalid())
        {
            SAPDBERR_ASSERT_STATE (!oldBlock.IsValid() && !bPageUpdated);

            section.IncrementUpdatedPages( false, false );

            return( Converter_Ok );
        }

        // pageConverterVersion is less than current converter version

        SAPDBERR_ASSERT_STATE ( oldBlock.IsValid() );

        section.IncrementUpdatedPages( bPageUpdated, bPendingPageUpdate );

        if( snapShotVersion.IsValid() && ( pageConverterVersion <= snapShotVersion )){

            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block of " <<
                                ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                                 "permanent pageNo " : "static pageNo " ) << pageNo <<
                                " is not marked as free");

            return( Converter_Ok ); // oldBlock is needed for frozen version
        }
    }

    FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( task.ID(), oldBlock );

    return( Converter_Ok );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::SetPositionForWriting(
    const RTETask_ITask         &task,
    const Converter_Version     &converterVersion,
    const Converter_Version     &pageConverterVersion,
    const Converter_Version     &snapShotVersion,
    const Data_PageNo           pageNo,
    const IOMan_BlockAddress    &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::SetPositionForWriting", Converter_Trace, 5 );

    bool                bPageUpdated;
    bool                bPendingPageUpdate;
    IOMan_BlockAddress  oldBlock;
    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    {

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        const Converter_ReturnCode retCode =
            (Converter_ReturnCode) m_Map.SetPermBlockAddress( section, pageNo,
                    block, oldBlock, bPageUpdated, bPendingPageUpdate );

        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode );
            RTE_Crash( mapError );
        }

        if( pageConverterVersion.IsInvalid() )
        {
            SAPDBERR_ASSERT_STATE (! oldBlock.IsValid() && ! bPageUpdated);

            section.IncrementUpdatedPages( false, false );
            return;
        }
        section.IncrementUpdatedPages( bPageUpdated, bPendingPageUpdate );
    }

    SAPDBERR_ASSERT_STATE ( oldBlock.IsValid() );

    if( snapShotVersion.IsValid() && ( pageConverterVersion <= snapShotVersion )){

        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block of " <<
                            ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                             "permanent pageNo " : "static pageNo " ) << pageNo <<
                            " is not marked as free");

        return; // oldBlock is needed for frozen version
    }

    if( pageConverterVersion < converterVersion )
    {
        FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( task.ID(), oldBlock );
        return;
    }

    SAPDBERR_ASSERT_STATE( pageConverterVersion == converterVersion );

    FBM_IManager::Instance().SetBlockStateToFree( task.ID(), oldBlock );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::FreePageNo(
    const RTETask_ITask                             &task,
    const Converter_Version                         &pageConverterVersion,
    const Converter_Version                         &snapShotVersion,
    const Data_PageNo                               pageNo,
    Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
    Kernel_SynchronizedTaskWaitList                 &freePageNoWaitQueue,
    SAPDB_Bool                                      &bWaitUntilSavepointFinished,
    SAPDB_Bool                                      &bOldBlockIsUsableImmediately,
    SAPDB_Bool                                      &bDoDecrementPermUseCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::FreePageNo", Converter_Trace, 5 );

    IOMan_BlockAddress  oldBlock;
    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    bWaitUntilSavepointFinished  = false;
    bOldBlockIsUsableImmediately = true;
    bDoDecrementPermUseCount     = true;

    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        // Note: For a page number which is reserved it is not realy necessary to wait
        //       until a savepoint is finished. It is sufficient to guarantee that the
        //       converter page is not involved into an i/o-operation, while the converter
        //       page is changed. To make the code easier reserved and 'used' entries
        //       are treated in the same way.

        if ( section.IsSavepointActive() )
        {
            // if there is no more space to append the page number into the FreePageNoList
            // one has to wait until the currently active savepoint has finished

            if( ! section.GetFreeList().Append( pageNo, pageConverterVersion ))
            {
                SAPDBTRACE_WRITELN( Converter_Trace, 5, "Suspend Task " << task.ID() <<
                                    " because FreePageNo list is full!" );

                freePageNoWaitQueue.InsertTask( task.ID(), waitContext );
                bWaitUntilSavepointFinished = true;
            }
            return;
        }

        // No Savepoint is active, therefore the given pageNo could be set to free

        bool  bPageUpdated       = false;
        bool  bPendingPageUpdate = false;

        const Converter_ReturnCode retCode = m_Map.FreePermPageNo( section, snapShotVersion,
                                             pageNo, oldBlock, bPageUpdated, bPendingPageUpdate );
        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode );
            RTE_Crash( mapError );
        }

        if( ! oldBlock.IsValid())
            return;

        section.DecrementUpdatedPages( bPageUpdated, bPendingPageUpdate );

        // Block will be set to FreeAfterSavepoint and therefore he
        // is not available in the current converter version
        bOldBlockIsUsableImmediately = false;
    }

    if( snapShotVersion.IsValid() && ( pageConverterVersion <= snapShotVersion )){

        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block of " <<
                            ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                             "permanent pageNo " : "static pageNo " ) << pageNo <<
                            " is not marked as free");

        bDoDecrementPermUseCount = false;

        return; // oldBlock is needed for frozen version
    }

    FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( task.ID(), oldBlock );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::MarkPageNoAsUsed(
    const RTETask_ITask  &task,
    const Data_PageNo    pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::MarkPageNoAsUsed", Converter_Trace, 5 );


    SAPDBTRACE_WRITELN( Converter_Trace, 7, "Mark " << ((GetRecoveryMode().UseDynamicPageAddressing()) ?
                        "permanent pageno" : "static pageno" ) << pageNo << " as used" );

    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

#   ifdef  SAPDB_QUICK
    {
        // check if this page is already written to a data volume
        IOMan_BlockAddress block;
        const Converter_ReturnCode retCode =  m_Map.GetBlockAddress( pageNo, block );

        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__, GetRecoveryMode(), pageNo, retCode );
            RTE_Crash( mapError );
        }
    }
#   endif

    const Converter_ReturnCode retCode = m_PageNoList.Insert( pageNo );

    if (Converter_Ok != retCode)
    {
        Converter_MapError mapError( __CONTEXT__, GetRecoveryMode(), pageNo, retCode );
        RTE_Crash( mapError );
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::ReleaseUnusedPageNos (
    const RTETask_ITask     &task,
    const Converter_Version &snapShotVersion,
    SAPDB_UInt              &numPermDataPages,
    SAPDB_UInt              &numPermConPages,
    SAPDB_UInt              &numReleasesBlocks )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::ReleaseUnusedPageNos", Converter_Trace, 5 );

    numPermDataPages  = 0;
    numPermConPages   = 0;
    numReleasesBlocks = 0;

    SAPDB_UInt  numUsedDataPages = 0;
    SAPDB_UInt  numUnusedBlocks  = 0;


    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        /* lock the current section to keep the chains of PartiallyUsedEntries  */
        /* and FullEntries constant while the iterators walk along this chains  */

        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        {
            // iterator over partially used entry chain

            Converter_PermMapSection::PartiallyUsedEntryIterator
            partUsedIter = section.PartiallyUsedEntriesBegin();

            const Converter_PermMapSection::PartiallyUsedEntryIterator
            partUsedEndIter = section.PartiallyUsedEntriesEnd();

            Converter_PermMapSection::PartiallyUsedEntryIterator
            currentPartIter = partUsedIter;


            for( ; partUsedIter != partUsedEndIter; )
            {
                // we use the current iterator position but increment the iterator
                // in advance in case the current entry is moved to a different
                // chain in ReleaseUnusedPageNosOfOneConverterPage()
                currentPartIter = partUsedIter;
                ++partUsedIter;

                SAPDBERR_ASSERT_STATE( ! (*currentPartIter).PageIsFull() );

                ReleaseUnusedPageNosOfOneConverterPage( section, task, snapShotVersion,
                                                        (*currentPartIter).GetPage(), numUsedDataPages,
                                                        numUnusedBlocks );
                numPermDataPages  += numUsedDataPages;
                numPermConPages   += section.GetPageCount();
                numReleasesBlocks += numUnusedBlocks;
            }
        }
        {
            // iterator over full used entry chain

            Converter_PermMapSection::FullEntryUsedIterator
            fullUsedIter = section.FullUsedEntriesBegin();

            const Converter_PermMapSection::FullEntryUsedIterator
            fullUsedEndIter = section.FullUsedEntriesEnd();

            Converter_PermMapSection::PartiallyUsedEntryIterator
            currentFullIter = fullUsedIter;

            for( ; fullUsedIter != fullUsedEndIter; )
            {
                // we use the current iterator position but increment the iterator
                // in advance in case the current entry is moved to a different
                // chain in ReleaseUnusedPageNosOfOneConverterPage()
                currentFullIter = fullUsedIter;
                ++fullUsedIter;

                SAPDBERR_ASSERT_STATE( (*currentFullIter).PageIsFull() );

                ReleaseUnusedPageNosOfOneConverterPage( section, task, snapShotVersion,
                                                        (*currentFullIter).GetPage(), numUsedDataPages,
                                                        numUnusedBlocks );
                numPermDataPages  += numUsedDataPages;
                numPermConPages   += section.GetPageCount();
                numReleasesBlocks += numUnusedBlocks;
            }
        }
    }
    SAPDBERR_ASSERT_STATE( m_PageNoList.IsEmpty() );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::EndFailedCheckData( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::EndFailedColdVerify", Converter_Trace, 5 );

#   ifdef  SAPDB_QUICK
    FBM_IManager::Instance().RestoreAllBlockStatesMarkedForBackup( task.ID());
#   endif

    ClearPageNoList();
}

/*---------------------------------------------------------------------------*/

Data_PageCount
Converter_PermMapControl::NumUpdatedPermPages( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::NumUpdatedPermPages", Converter_Trace, 5 );

    Data_PageCount numUpdatedPermPages = 0;


    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

        numUpdatedPermPages += section.GetNumUpdatedPages();
    }

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "total updated pages: " <<
                        numUpdatedPermPages );

    return( numUpdatedPermPages );
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::Dump(
    const RTETask_ITask &task,
    Kernel_Dump         &dump,
    const SAPDB_Bool    bEnterRegion )
{
    Converter_PermMapSection sections[ MAX_CONVERTER_SECTIONS ];

    // Attention: Don't check the correctness of the converter in case
    // of a dump, because it's possible that the dump was initiated by
    // a corrupted converter map!

    for( SAPDB_UInt sectionNo = 0; sectionNo < GetSectionCount(); ++sectionNo )
    {

        if( SAPDB_TRUE == bEnterRegion ){
            sections[ sectionNo ].Lock( task, m_SectionData[ sectionNo ],
                                        m_SectionPermData[ sectionNo ], SAPDB_FALSE );
        }

        struct ConverterMapSection  dumpEntry;

        dumpEntry.dmpIsPerm    = GetRecoveryMode().PageIsRecoverable();
        dumpEntry.dmpIsDynamic = GetRecoveryMode().UseDynamicPageAddressing();

        m_SectionData[ sectionNo ].Dump( dumpEntry );
        m_SectionPermData[ sectionNo ].Dump( dumpEntry );

        dump.InsertEntry( Kernel_Dump::DmpConverterMapSection,
                          Kernel_DumpPage::Entry( &dumpEntry, sizeof( dumpEntry )));
    }

    // Use the all sections spanned(!) iterator of the proper converter map

    if( 0 < GetSectionCount() )
        m_Map.DumpEntries( dump );
}

/*---------------------------------------------------------------------------*/
/* private methods                                                           */
/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::FlushSection(
    const RTETask_ITask &task,
    Converter_Index     &index,
    Converter_Pager     &pager,
    const SAPDB_UInt    sectionNo,
    SAPDB_UInt          &totalPages,
    SAPDB_UInt          &totalIO,
    Data_PageNo         &maxUsedPageNoInSection )
{
    maxUsedPageNoInSection = 0;

    // It's possible that the given sectionNo index is out of range, because
    // it's the logical id of the corresponding pager task.The number of
    // pager tasks is at least as high as the number of converter sections

    if( sectionNo >= GetSectionCount() )
        return;

    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

    Converter_PermMapSection::ChangedEntryIterator       iter    = section.ChangedEntriesBegin();
    const Converter_PermMapSection::ChangedEntryIterator endIter = section.ChangedEntriesEnd();

    while( iter != endIter )
    {
        Converter_LeafPage &leafPage = (*iter).GetPage();

        m_Map.PreparePageForIO( section, *iter );

        WriteChangedConverterPage( task, section, leafPage, pager, totalPages, totalIO ); // Write page outside region

        m_Map.ResetPageStateAfterIO( section, *iter );

        // write the new block address into the index
        index.IntegratePage( task, leafPage );

        iter = section.MarkPageAsUnchangedAndGoToNextChangedEntry( iter.GetPosition());
    }
    maxUsedPageNoInSection = section.GetMaxUsedPageNo();
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::WriteChangedConverterPage(
    const RTETask_ITask      &task,
    Converter_PermMapSection &section,
    Converter_LeafPage       &leafPage,
    Converter_Pager          &pager,
    SAPDB_UInt               &totalPages,
    SAPDB_UInt               &totalIO )
{
    // Attention: This is a virtual method! The corresponding method from
    // StaticMapControl::WriteChangedConverterPage has additional funcionality
    // concerning the wakeup of suspended tasks. This suspend situations could
    // happen for static data pages only and therefore is not implemented in
    // this virtual method.

    SAPDBTRACE_ROUTINE_DEBUG( "PermMapControl::WriteChangedConverterPage", Converter_Trace, 5 );

    section.Unlock(); // release region during i/o

    pager.Write( leafPage );

    totalPages++;
    totalIO++;

    section.Lock(); // lock region again after i/o
}

/*---------------------------------------------------------------------------*/


void
Converter_PermMapControl::ReleaseUnusedPageNosOfOneConverterPage(
    Converter_PermMapSection &section,
    const RTETask_ITask      &task,
    const Converter_Version  &snapShotVersion,
    Converter_LeafPage       &leafPage,
    SAPDB_UInt               &numUsedDataPages,
    SAPDB_UInt               &numReleasedBlocks )
{
    SAPDBERR_ASSERT_STATE( leafPage.ReadPageRecoveryMode().PageIsRecoverable() );

    const SAPDB_Bool bIsDynamic = GetRecoveryMode().UseDynamicPageAddressing();
    numUsedDataPages  = 0;
    numReleasedBlocks = 0;

    Converter_PageEntryIterator         pageEntryIter    = leafPage.Begin();
    const Converter_PageEntryIterator   pageEntryEndIter = leafPage.End();

    for( ; pageEntryIter != pageEntryEndIter; ++pageEntryIter )
    {
        // page will be deleted if the last entry is removed in FreePermPageNo()
        if(! leafPage.IsAssigned())
            break;

        if(! pageEntryIter.EntryInUse())
            continue;

        /* in the cold state reserved states must not exist */
        SAPDBERR_ASSERT_STATE (!leafPage.IsReserved (pageEntryIter.GetPosition()));

        const Data_PageNo pageNo = leafPage.GetPageNo( pageEntryIter.GetPosition());

        if( m_PageNoList.Remove( pageNo )){
            ++numUsedDataPages;
        }
        else
        {
            // pageNo is not referenced by a db file and
            // therefore the entry is not needed anymore

            IOMan_BlockAddress  block;
            bool                bPageUpdated       = false;
            bool                bPendingPageUpdate = false;

            ++numReleasedBlocks;

            const Converter_ReturnCode retCode =
                m_Map.FreePermPageNo( section, snapShotVersion, pageNo, block,
                                      bPageUpdated, bPendingPageUpdate );

            if( Converter_Ok != retCode )
            {
                Converter_MapError mapError ( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode);
                RTE_Crash( mapError );
            }

            section.DecrementUpdatedPages( bPageUpdated, bPendingPageUpdate );

            SAPDBERR_ASSERT_STATE( block.IsValid());

            Converter_Exception errMsg( __CONTEXT__, CONVERTER_PAGENO_NOT_REFERENCED,
                                        (( bIsDynamic ) ? "perm" : "static"),
                                        SAPDB_ToString( pageNo, _T_d ),
                                        SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                                        SAPDB_ToString( block.GetBlockNo(), _T_d ));
            RTE_Message( errMsg );

            if( snapShotVersion.IsValid())
            {
                // without reading the converter version of the page it's not possible
                // to decide whether the old block is needed for the frozen version,
                // therefore the old block is not set to free. If the block is not
                // needed and therefore not part of the frozen converter, the block
                // is set to free in the FBM at the next restart!

                SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Block: " <<
                                    block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                    (( bIsDynamic ) ? "permanent pageNo " : "static pageNo " ) <<
                                    pageNo << " is not marked as free");
            }
            else
            {
                SAPDBTRACE_WRITELN( Converter_Trace, 7, "Block: " <<
                                    block.GetDeviceNo() << "/" << block.GetBlockNo() << " of " <<
                                    (( bIsDynamic ) ? "permanent pageNo " : "static pageNo " ) <<
                                    pageNo << " is marked as free");

                FBM_IManager::Instance().SetBlockStateToFree( task.ID(), block );
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::MarkPermPagesForSaveData(
    const RTETask_ITask 		&task,
    Converter_PermMapSection    &section,
    Converter_LeafPage  		&leafPage,
    SAPDB_UInt          		&numPermPagesForBackUp )
{
    SAPDBERR_ASSERT_STATE( leafPage.ReadPageRecoveryMode().PageIsRecoverable());
    SAPDBERR_ASSERT_STATE( leafPage.GetEntryCount() > 0);

    const bool bLastSaveDataSuccessful = section.IsLastSaveDataSuccessful();
    bool bConvPageUpdated              = false;

    Converter_BlockAddressIterator leafIter = leafPage.GetUsedBlockIterator();

    if( SAPDB_TRUE != FBM_IManager::Instance().SetBlockStateToBackup( task.ID(), leafIter ))
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_BAD_ENTRY_STATE,
                                    SAPDB_ToString( leafPage.GetPageNo( leafIter.GetPosition()), _T_d),
                                    ((GetRecoveryMode().UseDynamicPageAddressing()) ? "perm" : "static"));
        RTE_Crash( errMsg );
    }


    Converter_PageEntryIterator       pageEntryIter    = leafPage.Begin();
    const Converter_PageEntryIterator pageEntryEndIter = leafPage.End();

    for ( ; pageEntryIter != pageEntryEndIter; ++pageEntryIter )
    {
        /* check if the page entry is in use and not only reserved */
        if ((!pageEntryIter.EntryInUse()) || leafPage.IsReserved (pageEntryIter.GetPosition()))
            continue;

        const IOMan_BlockAddress block = pageEntryIter.GetBlockAddress();

        /* we don't backup archive pages */
        if( RTE_VolumeAccessModeSequential == FBM_IManager::Instance().GetVolMode( block.GetDeviceNo()))
            continue;

        const Converter_PageEntryNo entryPos = pageEntryIter.GetPosition();
        const Data_PageNo           pageNo   = leafPage.GetPageNo( entryPos );

        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Mark " <<
                            ((GetRecoveryMode().UseDynamicPageAddressing()) ? "permanent " : "static " ) <<
                            "pageNo: " <<  pageNo << " for SaveData" );

        m_PageNoList.Insert( leafPage.GetPageNo( entryPos ));
        ++numPermPagesForBackUp;

        if ( leafPage.SavePagesRequested( entryPos ))
        {
            leafPage.ClearSavePagesRequested( entryPos );
            leafPage.SetSavePagesPending( entryPos );
            bConvPageUpdated = true;
        }
        else if( bLastSaveDataSuccessful && leafPage.SavePagesPending( entryPos ))
        {
            // pending flag is obsolete, because the last complete backup was successful
            // and therefore this data page is not part of the next incremental backup
            leafPage.ClearSavePagesPending( entryPos );
            bConvPageUpdated = true;
        }
    }
    if( bConvPageUpdated ){
        m_Map.MarkPageAsChanged( section, leafPage );
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapControl::MarkPermPagesForSavePages(
    const RTETask_ITask         &task,
    Converter_PermMapSection    &section,
    Converter_LeafPage          &leafPage,
    SAPDB_UInt                  &numPermPagesForBackUp,
    const Converter_Version     &converterVersion )
{
    SAPDBERR_ASSERT_STATE( leafPage.ReadPageRecoveryMode().PageIsRecoverable());
    SAPDBERR_ASSERT_STATE( leafPage.GetEntryCount() > 0);

    const bool bLastSaveDataSuccessful = section.IsLastSaveDataSuccessful();

    Converter_PageEntryIterator       pageEntryIter    = leafPage.Begin();
    const Converter_PageEntryIterator pageEntryEndIter = leafPage.End();

    for( ; pageEntryIter != pageEntryEndIter; ++pageEntryIter )
    {
        /* check if the page entry is in use and not only reserved */
        if (( ! pageEntryIter.EntryInUse()) || leafPage.IsReserved ( pageEntryIter.GetPosition()))
            continue;

        const Converter_PageEntryNo entryPos = pageEntryIter.GetPosition();

        if( leafPage.SavePagesRequested( entryPos ) ||
                ( ! bLastSaveDataSuccessful && leafPage.SavePagesPending( entryPos )))
        {
            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Mark " <<
                                ((GetRecoveryMode().UseDynamicPageAddressing()) ? "permanent " : "static " ) <<
                                "pageNo: " <<  leafPage.GetPageNo( entryPos) << " for SavePages" );

            if( SAPDB_TRUE != FBM_IManager::Instance().SetBlockStateToBackup( task.ID(), pageEntryIter.GetBlockAddress()))
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_BAD_ENTRY_STATE,
                                            SAPDB_ToString( leafPage.GetPageNo( entryPos ), _T_d),
                                            ((GetRecoveryMode().UseDynamicPageAddressing()) ? "perm" : "static"));
                RTE_Crash( errMsg );
            }

            m_PageNoList.Insert( leafPage.GetPageNo( entryPos ));
            ++numPermPagesForBackUp;
        }
    }

    Converter_PageFrameAllocator frameAlloc( task );
    m_UsedPageNoList.Append( leafPage, frameAlloc, converterVersion );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_PermMapControl::IsMember(
    const RTETask_ITask         &task,
    const Data_PageNo           basePageNo,
    const IOMan_BlockAddress    &block )
{
    const SAPDB_UInt sectionNo = CalculateSectionNo( basePageNo );

    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

    return( m_Map.IsMember( basePageNo, block ));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
