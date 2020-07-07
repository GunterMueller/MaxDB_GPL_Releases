/*!
  @file           RTEMem_SystemPageCache.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Singleton for System Page allocation/buffering


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
extern SAPDBTrace_Topic Memory_Trace; ///< global memory trace object
#include "RunTime/RTE_Message.hpp"
#define RTE_CONTEXT "RTE",__FILE__,__LINE__
#include "RunTime/RTE_SystemPageCache_Messages.hpp"
#include <stdlib.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

RTEMem_SystemPageCache * RTEMem_SystemPageCache::m_Instance = NULL;

//---------------------------------------------------------------------------

RTEMem_SystemPageCache & RTEMem_SystemPageCache::Instance( )
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_SystemPageCache));

    if ( !m_Instance )
    {
        m_Instance = new (Space) RTEMem_SystemPageCache();
    }

    return *m_Instance;
}

//----------------------------------------------------------------------

RTEMem_SystemPageCache::RTEMem_SystemPageCache()
    : m_pageSize(RTESys_SystemPageSize())
    , m_descriptorPool(0)
    , m_descriptorPoolLock((const SAPDB_UTF8 *)"SystemPageCacheDescriptorPool")
    , m_chainHeadPool(0)
    , m_chainHeadPoolLock((const SAPDB_UTF8 *)"SystemPageCacheChainHeadPool")
    , m_freeBlockChainHead(0)
    , m_freeBlockChainLock((const SAPDB_UTF8 *)"SystemPageCacheFreeBlockChain")
    , m_usedBlockChainHead(0)
    , m_usedBlockChainLock((const SAPDB_UTF8 *)"SystemPageCacheUsedBlockChain")
    , m_emergencyFreeBlockChainHead(0)
    , m_BytesUsed(0)
    , m_MaxBytesUsed(0)
    , m_BytesControlled(0)
    , m_CountBaseAlloc(0)
    , m_CountBaseDealloc(0)
    , m_CountAlloc(0)
    , m_CountDealloc(0)
    , m_ErrorCount(0)
    , m_isDisabled(0)
    , m_FreeBlocks(0)
    , m_SplittedBlocks(0)
    , m_SplinterBlocks(0)
    , m_FreeSplinterBlocks(0)
    , m_UsedBlocks(0)
{
      static RTEMem_AllocatorInfo AllocatorInfo(this->GetIdentifier(), 
		this, (const SAPDB_UTF8 *)"SystemHeap");
      RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
      // during start there will be memory...
      m_emergencyFreeBlockChainHead = GetChainHeadFromPool();
      SAPDBERR_ASSERT_STATE(m_emergencyFreeBlockChainHead);
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::GetBaseAllocatorCallStatistics( SAPDB_ULong &CountAlloc,
                                                             SAPDB_ULong &CountDealloc ) const
{
    CountAlloc   = m_CountBaseAlloc;
    CountDealloc = m_CountBaseDealloc;
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::GetCallStatistics( SAPDB_ULong &CountAlloc,
                                                SAPDB_ULong &CountDealloc ) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::CalcStatistics( SAPDB_ULong &BytesUsed,
                                             SAPDB_ULong &MaxBytesUsed,
                                             SAPDB_ULong &BytesControlled)
{
    BytesUsed       = m_BytesUsed;
    MaxBytesUsed    = m_MaxBytesUsed;
    BytesControlled = m_BytesControlled;
}
//---------------------------------------------------------------------------
#ifdef DUMP_ALL_ALLOCATIONS
static SAPDB_Int4 hasEverCalledDeallocate = 0;
#endif

void *RTEMem_SystemPageCache::Allocate(void *FixedAddress, SAPDB_ULong blockCount, SAPDB_Bool DoCommit)
{
    SAPDB_ULong blockCountInBytes = blockCount * m_pageSize;

    if ( m_isDisabled )
    {
        return RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit);
    }

    SAPDB_ULong newAllocCallCount = m_CountAlloc.Increment();
    RTEMem_BlockChainHead * pFreeChainHead;
    RTEMem_BlockChainHead * pUsedChainHead;

    if ( FindBlockChainHead( blockCount,
                             pUsedChainHead,
                             LockUsedChainHead )
      && FindBlockChainHead( blockCount,
                             pFreeChainHead,
                             LockFreeChainHead ) )
    {
        RTEMem_BlockDescriptor * pBlockDescriptor = 0;

        // Special case follows in else part: Fixed address memory and uncommited memory never taken from free list 
        if ( !FixedAddress && DoCommit )
        {
            pBlockDescriptor = LockedDequeueFreeBlock( blockCount,
                                                       *pFreeChainHead );
        }

        if ( !pBlockDescriptor )
        {
            pBlockDescriptor = GetDescriptorFromPool();
            if ( !pBlockDescriptor )
            {
                m_ErrorCount.Increment();
                return (void *)0;
            }

            pBlockDescriptor->SetBlockAddress(RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit));
            if ( !(pBlockDescriptor->GetBlockAddress()) )
            {
                // retry after emergency releasing hold memory, hoping system may recombine to larger chunks....
                ReleaseFreeBlocks();
                pBlockDescriptor->SetBlockAddress(RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit));
                if ( !(pBlockDescriptor->GetBlockAddress()) ) // if still no memory, we must fail
                {
                    LockedReturnDescriptorToPool(pBlockDescriptor);
                    m_ErrorCount.Increment();
                    return (void *)0;
                }
            }
            pBlockDescriptor->MarkAsUsedBlock();
            m_CountBaseAlloc.Increment();
            m_BytesControlled.Increment(blockCountInBytes);
        }

        SAPDB_ULong newBytesUsed = m_BytesUsed.Increment(blockCountInBytes);
        if ( newBytesUsed > m_MaxBytesUsed )
        {
            m_MaxBytesUsed = newBytesUsed;
        }

        LockedEnqueueBlockDescriptor( pUsedChainHead->firstBlockDescriptor,
                                      pBlockDescriptor,
                                      m_UsedBlocks,
                                      m_usedBlockChainLock );

        void *blockAddress = pBlockDescriptor->GetBlockAddress();
#ifdef DUMP_ALL_ALLOCATIONS
        if ( hasEverCalledDeallocate )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                               RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK,
                                               SAPDB_ToString(blockAddress),
                                               SAPDB_ToString(blockCount, 6) ) );
        }
#endif
        return blockAddress;
    }
 
    m_ErrorCount.Increment();
    return (void *)0;
}

//---------------------------------------------------------------------------

void *RTEMem_SystemPageCache::Allocate(void *FixedAddress, SAPDB_ULong blockCount, SAPDB_Bool DoCommit, Msg_List &messageList)
{
    SAPDB_ULong blockCountInBytes = blockCount * m_pageSize;

    if ( m_isDisabled )
    {
        return RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit, messageList);
    }

    SAPDB_ULong newAllocCallCount = m_CountAlloc.Increment();
    RTEMem_BlockChainHead * pFreeChainHead;
    RTEMem_BlockChainHead * pUsedChainHead;

    if ( FindBlockChainHead( blockCount,
                             pUsedChainHead,
                             LockUsedChainHead )
      && FindBlockChainHead( blockCount,
                             pFreeChainHead,
                             LockFreeChainHead ) )
    {
        RTEMem_BlockDescriptor * pBlockDescriptor = 0;

        // Special case follows in else part: Fixed address memory and uncommited memory never taken from free list 
        if ( !FixedAddress && DoCommit )
        {
            pBlockDescriptor = LockedDequeueFreeBlock( blockCount,
                                                       *pFreeChainHead );
        }

        if ( !pBlockDescriptor )
        {
            pBlockDescriptor = GetDescriptorFromPool(messageList);
            if ( !pBlockDescriptor )
            {
                m_ErrorCount.Increment();
                return (void *)0;
            }

            pBlockDescriptor->SetBlockAddress(RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit, messageList));
            if ( !(pBlockDescriptor->GetBlockAddress()) )
            {
                // retry after emergency releasing hold memory, hoping system may recombine to larger chunks....
                ReleaseFreeBlocks();
                pBlockDescriptor->SetBlockAddress(RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(FixedAddress, blockCountInBytes, DoCommit, messageList));
                if ( !(pBlockDescriptor->GetBlockAddress()) ) // if still no memory, we must fail
                {
                    LockedReturnDescriptorToPool(pBlockDescriptor);
                    m_ErrorCount.Increment();
                    return (void *)0;
                }
            }
            pBlockDescriptor->MarkAsUsedBlock();
            m_CountBaseAlloc.Increment();
            m_BytesControlled.Increment(blockCountInBytes);
        }

        SAPDB_ULong newBytesUsed = m_BytesUsed.Increment(blockCountInBytes);
        if ( newBytesUsed > m_MaxBytesUsed )
        {
            m_MaxBytesUsed = newBytesUsed;
        }

        LockedEnqueueBlockDescriptor( pUsedChainHead->firstBlockDescriptor,
                                      pBlockDescriptor,
                                      m_UsedBlocks,
                                      m_usedBlockChainLock );

        void *blockAddress = pBlockDescriptor->GetBlockAddress();
#ifdef DUMP_ALL_ALLOCATIONS
        if ( hasEverCalledDeallocate )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                               RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK,
                                               SAPDB_ToString(blockAddress),
                                               SAPDB_ToString(blockCount, 6) ) );
        }
#endif
        return blockAddress;
    }
 
    m_ErrorCount.Increment();
    return (void *)0;
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::Deallocate(void *blockAddress, SAPDB_ULong blockCount)
{
#ifdef DUMP_ALL_ALLOCATIONS
    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                       RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK,
                                       SAPDB_ToString(blockCount, 6),
                                       SAPDB_ToString(blockAddress) ) );

    hasEverCalledDeallocate = 1;
#endif
    SAPDB_ULong newDeallocCallCount = m_CountDealloc.Increment();

    RTEMem_BlockChainHead * pUsedChainHead;

    SAPDBERR_ASSERT_STATE(blockCount > 0);

    if ( !FindBlockChainHead( blockCount,
                              pUsedChainHead,
                              LockUsedChainHead ) )
    {
        SAPDBERR_ASSERT_STATE(blockCount == 0);

        if ( !m_isDisabled )
        {
            m_ErrorCount.Increment();
        }

        SAPDB_ULong blockCountInBytes = blockCount * m_pageSize;
        RTE_ISystem::Instance().UncachedFreeSystemPages(blockAddress, blockCountInBytes);
        return; /* potential second free silently passed through */
    }

    RTEMem_BlockDescriptor * pBlockDescriptor = LockedDequeueUsedBlockDescriptor( pUsedChainHead->firstBlockDescriptor,
                                                                                  blockAddress );

    if ( !pBlockDescriptor )
    {
        if ( !m_isDisabled )
        {
            m_ErrorCount.Increment();
        }

        SAPDB_ULong blockCountInBytes = blockCount * m_pageSize;
        RTE_ISystem::Instance().UncachedFreeSystemPages(blockAddress, blockCountInBytes);
        return; /* potential second free silently passed through */
    }

    m_BytesUsed.Decrement(blockCount*m_pageSize);

    SAPDB_ULong mergedBlockCount = blockCount;

    if ( pBlockDescriptor->GetFirstSplinter() )
    {
        RTEMem_BlockDescriptor * pDequeuedBlockDescriptorChain = 0;
        mergedBlockCount = MergeWithFreeBlocks(pBlockDescriptor, blockCount, pDequeuedBlockDescriptorChain);
        if ( pDequeuedBlockDescriptorChain )
        {
            RTEMem_BlockDescriptor * pReturnDescriptor = pDequeuedBlockDescriptorChain;
            while ( pReturnDescriptor )
            {
                pDequeuedBlockDescriptorChain = pReturnDescriptor->GetNextDescriptor();
                LockedReturnDescriptorToPool(pReturnDescriptor);
                pReturnDescriptor = pDequeuedBlockDescriptorChain;
            }
        }
    }

    
    if ( !pBlockDescriptor->GetFirstSplinter() )
    {
        if ( m_isDisabled )
        {
            LockedReturnDescriptorToPool(pBlockDescriptor);

            SAPDB_ULong blockCountInBytes = mergedBlockCount * m_pageSize;
            RTE_ISystem::Instance().UncachedFreeSystemPages(pBlockDescriptor->GetBlockAddress(), blockCountInBytes);
            return;
        }
    }

    RTEMem_BlockChainHead * pFreeChainHead;
    if ( !FindBlockChainHead( mergedBlockCount,
                              pFreeChainHead,
                              LockFreeChainHead ) )
    {
       if ( pBlockDescriptor->GetFirstSplinter() )
       {
           pFreeChainHead = m_emergencyFreeBlockChainHead;
       }
       else
       {
           RTE_ISystem::Instance().UncachedFreeSystemPages( pBlockDescriptor->GetBlockAddress(),
                                                            m_pageSize*mergedBlockCount );
           m_BytesControlled.Decrement(mergedBlockCount * m_pageSize);
           LockedReturnDescriptorToPool(pBlockDescriptor);
           return;
       }
    }

    pBlockDescriptor->MarkAsFreeBlock(mergedBlockCount);
    LockedEnqueueBlockDescriptor( pFreeChainHead->firstBlockDescriptor,
                                  pBlockDescriptor,
                                  m_FreeBlocks,
                                  m_freeBlockChainLock );
}

//------------------------------------------------------------------------------------

SAPDB_ULong RTEMem_SystemPageCache::ReleaseFreeBlocks()
{
    SAPDB_ULong releasedPages;
    SAPDB_ULong releasedBlocks;
    SAPDB_ULong holdingPages;
    SAPDB_ULong holdingBlocks;
    RTEMem_BlockDescriptor *freeBlockDescriptorChain = GetDequeuedFreeBlocks( releasedPages
                                                                             ,releasedBlocks
                                                                             ,holdingPages
                                                                             ,holdingBlocks
                                                                             );
    if ( releasedPages )
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                           RTEINFO_SYSTEMPAGECACHE_RELEASE_BLOCKS, 
                                           SAPDB_ToString(releasedPages),
                                           SAPDB_ToString(releasedBlocks) ) );
    }
    else
    {
        if ( holdingPages )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                               RTEINFO_SYSTEMPAGECACHE_RELEASE_NO_BLOCKS ) );
        }
    }

    if ( holdingPages )
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                           RTEINFO_SYSTEMPAGECACHE_RELEASE_HOLDING,
                                           SAPDB_ToString(holdingPages),
                                           SAPDB_ToString(holdingBlocks) ) );
    }

    if ( freeBlockDescriptorChain )
    {
        RTEMem_BlockDescriptor *lastBlockDescriptor = 0;
        SAPDB_ULong totalDequeuedBlockCount = 0;

        for ( RTEMem_BlockDescriptor *pBlockDescriptor = freeBlockDescriptorChain; 
              0 != pBlockDescriptor;
            )
        {
            lastBlockDescriptor = pBlockDescriptor;

            void *StartPageAddress;
            SAPDB_ULong blockCount;
            SAPDB_Bool blockFreed = pBlockDescriptor->GetReleasableBlockInformation(StartPageAddress, blockCount);
            SAPDBERR_ASSERT_STATE(blockFreed);
            totalDequeuedBlockCount += blockCount;
            RTE_ISystem::Instance().UncachedFreeSystemPages(StartPageAddress, m_pageSize*blockCount);

            pBlockDescriptor = lastBlockDescriptor->GetNextDescriptor();
            lastBlockDescriptor->MarkAsUnusedBlock(pBlockDescriptor);
        }

        m_BytesControlled.Decrement(totalDequeuedBlockCount * m_pageSize);

        LockedAddDescriptorChainToPool( freeBlockDescriptorChain,
                                        lastBlockDescriptor );
    }
    return releasedPages;
}

//------------------------------------------------------------------------------------

void RTEMem_SystemPageCache::Dump(SAPDB_Bool toConsole)
{
    SAPDB_Bool outOfMemory = false;
    RTEMem_BlockDescriptor *dumpBlockChain = 0;
    RTEMem_BlockDescriptor *pBlockDescriptor = 0;
    SAPDB_ULong chainHeads = 0;
    SAPDB_ULong blockDescriptors = 0;

    if ( GetDumpBlockChain( m_freeBlockChainHead,
                            m_freeBlockChainLock,
                            dumpBlockChain, 
                            chainHeads, 
                            blockDescriptors) )
    {
        SAPDB_ULong lastBlockCount = 0;
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                           RTEINFO_SYSTEMPAGECACHE_FREE_BLOCKS, 
                                           SAPDB_ToString(chainHeads),
                                           SAPDB_ToString(blockDescriptors) ),
                                           toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
        for ( pBlockDescriptor = dumpBlockChain; pBlockDescriptor; )
        {
            void *blockAddress;
            SAPDB_ULong blockCount;
            RTEMem_BlockDescriptor *firstSplinter;
            RTEMem_BlockDescriptor *nextSplinter;

            pBlockDescriptor->GetDumpBlockInformation(blockAddress, blockCount, firstSplinter, nextSplinter);
            if ( lastBlockCount != blockCount )
            {
                if ( !blockAddress )
                {
                    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                       RTEINFO_SYSTEMPAGECACHE_EMPTY_CHAIN,
                                                       SAPDB_ToString(blockCount, 6) ),
                                                   toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                }
                else
                {
                    if ( firstSplinter )
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK_SPLIT,
                                                           SAPDB_ToString(blockCount, 6),
                                                           SAPDB_ToString(blockAddress),
                                                           SAPDB_ToString(firstSplinter),
                                                           SAPDB_ToString(nextSplinter) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                    else
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK,
                                                           SAPDB_ToString(blockCount, 6),
                                                           SAPDB_ToString(blockAddress) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                }
                lastBlockCount = blockCount;
            }
            else
            {
                if ( blockAddress )
                {
                    if ( firstSplinter )
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK_SPLIT,
                                                           SAPDB_ToString(blockAddress),
                                                           SAPDB_ToString(firstSplinter),
                                                           SAPDB_ToString(nextSplinter) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                    else
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK,
                                                           SAPDB_ToString(blockAddress) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                }
            }
            pBlockDescriptor = dumpBlockChain->GetNextDescriptor();
            LockedReturnDescriptorToPool(dumpBlockChain);
            dumpBlockChain = pBlockDescriptor;
        }
    }

    dumpBlockChain = 0;
    chainHeads = 0;
    blockDescriptors = 0;
    if ( GetDumpBlockChain( m_usedBlockChainHead,
                            m_usedBlockChainLock,
                            dumpBlockChain, 
                            chainHeads, 
                            blockDescriptors) )
    {
        SAPDB_ULong lastBlockCount = 0;
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                           RTEINFO_SYSTEMPAGECACHE_USED_BLOCKS,
                                           SAPDB_ToString(chainHeads),
                                           SAPDB_ToString(blockDescriptors) ),
                                           toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
        for ( pBlockDescriptor = dumpBlockChain; pBlockDescriptor; )
        {
            void *blockAddress;
            SAPDB_ULong blockCount;
            RTEMem_BlockDescriptor *firstSplinter;
            RTEMem_BlockDescriptor *nextSplinter;

            pBlockDescriptor->GetDumpBlockInformation(blockAddress, blockCount, firstSplinter, nextSplinter);
            if ( lastBlockCount != blockCount )
            {
                if ( !blockAddress )
                {
                    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                       RTEINFO_SYSTEMPAGECACHE_EMPTY_CHAIN,
                                                       SAPDB_ToString(blockCount, 6) ),
                                                   toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                }
                else
                {
                    if ( firstSplinter )
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK_SPLIT,
                                                           SAPDB_ToString(blockCount, 6),
                                                           SAPDB_ToString(blockAddress),
                                                           SAPDB_ToString(firstSplinter),
                                                           SAPDB_ToString(nextSplinter) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                    else
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK,
                                                           SAPDB_ToString(blockCount, 6),
                                                           SAPDB_ToString(blockAddress) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                }
                lastBlockCount = blockCount;
            }
            else
            {
                if ( blockAddress )
                {
                    if ( firstSplinter )
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK_SPLIT,
                                                           SAPDB_ToString(blockAddress),
                                                           SAPDB_ToString(firstSplinter),
                                                           SAPDB_ToString(nextSplinter) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                    else
                    {
                        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                                           RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK,
                                                           SAPDB_ToString(blockAddress) ),
                                                       toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
                    }
                }
            }
            pBlockDescriptor = dumpBlockChain->GetNextDescriptor();
            LockedReturnDescriptorToPool(dumpBlockChain);
            dumpBlockChain = pBlockDescriptor;
        }
    }

    DumpStatistic(toConsole);
}

/*===========================================================================*/

extern "C" void RTEMem_DumpSystemPageCache()
{
    RTEMem_SystemPageCache::Instance().Dump(false);
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

void RTEMem_SystemPageCache::LockedReturnDescriptorToPool(RTEMem_BlockDescriptor *pBlockDescriptor)
{
    RTESync_LockedScope poolLock(m_descriptorPoolLock);
    {
        pBlockDescriptor->MarkAsUnusedBlock(m_descriptorPool);
        m_descriptorPool = pBlockDescriptor;
    }
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::LockedAddDescriptorChainToPool( RTEMem_BlockDescriptor *firstBlockDescriptor,
                                                             RTEMem_BlockDescriptor *lastDescriptor )
{
    RTESync_LockedScope poolLock(m_descriptorPoolLock);
    {
        lastDescriptor->SetNextDescriptor(m_descriptorPool);
        m_descriptorPool = firstBlockDescriptor;
    }
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::LockedGetDescriptorFromPool(RTEMem_BlockDescriptor * &pBlockDescriptor)
{
    RTESync_LockedScope poolLock(m_descriptorPoolLock);
    {
        pBlockDescriptor = m_descriptorPool;
        if ( 0 != pBlockDescriptor )
        {
            m_descriptorPool = pBlockDescriptor->GetNextDescriptor();
            return true;
        }
        else
        {
            return false;
        }
    }
}

//---------------------------------------------------------------------------

RTEMem_BlockDescriptor * RTEMem_SystemPageCache::GetDescriptorFromPool()
{
    RTEMem_BlockDescriptor *pBlockDescriptor;

    for(;;)
    {
        if ( LockedGetDescriptorFromPool(pBlockDescriptor) )
        {
            break;
        }

        SAPDB_Byte *blockDescriptorArray = (SAPDB_Byte *)RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress((void *)0, m_pageSize, true);
        if ( 0 == blockDescriptorArray )
        {
            pBlockDescriptor = 0;
            break;
        }
        m_BytesControlled.Increment(m_pageSize);

        pBlockDescriptor = 0;
        for ( SAPDB_Int4 iDescriptor = (m_pageSize/sizeof(RTEMem_BlockDescriptor)); (--iDescriptor) >= 0; )
        {
            pBlockDescriptor = new(blockDescriptorArray + iDescriptor * sizeof(RTEMem_BlockDescriptor))
                                RTEMem_BlockDescriptor(pBlockDescriptor);
        }

        LockedAddDescriptorChainToPool( pBlockDescriptor,
                                        pBlockDescriptor + (m_pageSize/sizeof(RTEMem_BlockDescriptor)) - 1 );
    }
    /* completely out of memory.... */
    return pBlockDescriptor;
}

//---------------------------------------------------------------------------

RTEMem_BlockDescriptor * RTEMem_SystemPageCache::GetDescriptorFromPool(Msg_List &messageList)
{
    RTEMem_BlockDescriptor *pBlockDescriptor;

    for(;;)
    {
        if ( LockedGetDescriptorFromPool(pBlockDescriptor) )
        {
            break;
        }

        SAPDB_Byte *blockDescriptorArray = (SAPDB_Byte *)RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress((void *)0, m_pageSize, true, messageList);
        if ( 0 == blockDescriptorArray )
        {
            pBlockDescriptor = 0;
            break;
        }
        m_BytesControlled.Increment(m_pageSize);

        pBlockDescriptor = 0;
        for ( SAPDB_Int4 iDescriptor = (m_pageSize/sizeof(RTEMem_BlockDescriptor)); (--iDescriptor) >= 0; )
        {
            pBlockDescriptor = new(blockDescriptorArray + iDescriptor * sizeof(RTEMem_BlockDescriptor))
                                RTEMem_BlockDescriptor(pBlockDescriptor);
        }

        LockedAddDescriptorChainToPool( pBlockDescriptor,
                                        pBlockDescriptor + (m_pageSize/sizeof(RTEMem_BlockDescriptor)) - 1 );
    }
    /* completely out of memory.... */
    return pBlockDescriptor;
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::LockedReturnChainHeadToPool(RTEMem_BlockChainHead *pChainHead)
{
    RTESync_LockedScope poolLock(m_chainHeadPoolLock);
    {
        pChainHead->firstBlockDescriptor = 0;
        pChainHead->blockCount = 0;
        pChainHead->pNextChain = m_chainHeadPool;
        m_chainHeadPool = pChainHead;
    }
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::LockedAddChainHeadChainToPool( RTEMem_BlockChainHead *pFirstChainHead,
                                                            RTEMem_BlockChainHead *pLastChainHead )
{
    RTESync_LockedScope poolLock(m_chainHeadPoolLock);
    {
        pLastChainHead->pNextChain = m_chainHeadPool;
        m_chainHeadPool = pFirstChainHead;
    }
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::LockedGetChainHeadFromPool(RTEMem_BlockChainHead * &pChainHead)
{
    RTESync_LockedScope poolLock(m_chainHeadPoolLock);
    {
        pChainHead = m_chainHeadPool;
        if ( 0 != pChainHead )
        {
            m_chainHeadPool = pChainHead->pNextChain;
            return true;
        }
        else
        {
            return false;
        }
    }
}

//---------------------------------------------------------------------------

RTEMem_BlockChainHead * RTEMem_SystemPageCache::GetChainHeadFromPool()
{
    RTEMem_BlockChainHead *pChainHead;

    for(;;)
    {
        if ( LockedGetChainHeadFromPool(pChainHead) )
        {
            break;
        }

        RTEMem_BlockChainHead *chainHeadArray = (RTEMem_BlockChainHead *)RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress((void *)0, m_pageSize, true);
        if ( 0 == chainHeadArray )
        {
            pChainHead = 0;
            break;
        }
        memset(chainHeadArray, 0, m_pageSize);

        m_BytesControlled.Increment(m_pageSize);

        pChainHead = 0;
        for ( SAPDB_Int4 iChainHead = (m_pageSize/sizeof(RTEMem_BlockChainHead)); (--iChainHead) >= 0; )
        {
            chainHeadArray[iChainHead].pNextChain = pChainHead;
            pChainHead = &chainHeadArray[iChainHead];
        }

        LockedAddChainHeadChainToPool( chainHeadArray,
                                       chainHeadArray + (m_pageSize/sizeof(RTEMem_BlockChainHead)) - 1 );
    }
    /* completely out of memory.... */
    return pChainHead;
}

//---------------------------------------------------------------------------

RTEMem_BlockDescriptor * RTEMem_SystemPageCache::LockedDequeueFreeBlock( SAPDB_ULong                blockCount,
                                                                         RTEMem_BlockChainHead    & blockChain )
{
    RTEMem_BlockDescriptor * pSearchDescriptor = 0;
    RTESync_LockedScope chainLock(m_freeBlockChainLock);
    {
        if ( blockChain.firstBlockDescriptor )
        {
            pSearchDescriptor = blockChain.firstBlockDescriptor;
            blockChain.firstBlockDescriptor = pSearchDescriptor->GetNextDescriptor();
            return pSearchDescriptor;
        }

        RTEMem_BlockChainHead *pNextChain = &blockChain;
        RTEMem_BlockDescriptor *pSplittedBlockDescriptor;
        while ( 0 != (pNextChain = pNextChain->pNextChain) )
        {
            if ( pNextChain->firstBlockDescriptor )
            {
                SAPDB_ULong splinterBlockCount = pNextChain->blockCount - blockCount;

                RTEMem_BlockChainHead * pFreeChainHead;
                RTEMem_BlockChainHead * pUsedChainHead;

                if ( FindBlockChainHead( splinterBlockCount,
                                         pUsedChainHead,
                                         LockUsedChainHead )
                  && FindBlockChainHead( splinterBlockCount,
                                         pFreeChainHead,
                                         NoLockFreeChainHead ) // <--- free block chain already locked!!!
                  && (pSplittedBlockDescriptor = GetDescriptorFromPool()) )
                {
                    pSearchDescriptor = pNextChain->firstBlockDescriptor;
                    pNextChain->firstBlockDescriptor = pSearchDescriptor->GetNextDescriptor();
                    pSearchDescriptor->MarkAsUsedBlock();

                    m_SplinterBlocks++; // we produce a new splinter
                    m_FreeSplinterBlocks++; // we produce a new free splinter
                    if ( !pSearchDescriptor->GetFirstSplinter() )
                    {
                        m_SplittedBlocks++; // if we splitted a block, we produce a splitted block 
                        m_SplinterBlocks++; // and another splinter...
                    }
                    ReinsertRemainingSplinter( *pFreeChainHead,
                                               pSearchDescriptor,
                                               (SAPDB_Byte *)(pSearchDescriptor->GetBlockAddress())
                                                              + (blockCount * m_pageSize),
                                               splinterBlockCount,
                                               pSplittedBlockDescriptor );
                    break;
                }
            }
        }
    }
    return pSearchDescriptor;
}
//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::EnqueueBlockDescriptor( RTEMem_BlockDescriptor * & firstBlockDescriptor,
                                                     RTEMem_BlockDescriptor *   pBlockDescriptor )
{
    pBlockDescriptor->SetNextDescriptor(firstBlockDescriptor);
    firstBlockDescriptor = pBlockDescriptor;
}

//---------------------------------------------------------------------------

void RTEMem_SystemPageCache::LockedEnqueueBlockDescriptor( RTEMem_BlockDescriptor * & firstBlockDescriptor,
                                                           RTEMem_BlockDescriptor *   pBlockDescriptor,
                                                           SAPDB_ULong              & blocks,
                                                           RTESync_Spinlock  & lock )
{
    RTESync_LockedScope chainLock(lock);
    {
        ++blocks;
        EnqueueBlockDescriptor( firstBlockDescriptor
                               ,pBlockDescriptor );
    }
}

//---------------------------------------------------------------------------

RTEMem_BlockDescriptor * RTEMem_SystemPageCache::LockedDequeueUsedBlockDescriptor( RTEMem_BlockDescriptor * & firstBlockDescriptor,
                                                                                   void *                     blockAddress )
{
    RTESync_LockedScope chainLock(m_usedBlockChainLock);
    {
        RTEMem_BlockDescriptor * pSearchDescriptor;
        for ( RTEMem_BlockDescriptor **pCurrentDescriptor = &firstBlockDescriptor; 
              0 != (pSearchDescriptor = (*pCurrentDescriptor)); 
              pCurrentDescriptor = &(pSearchDescriptor->GetNextDescriptor()) )
        {
            if ( blockAddress == pSearchDescriptor->GetBlockAddress() )
            {
                *pCurrentDescriptor = pSearchDescriptor->GetNextDescriptor();
                return pSearchDescriptor;
            }
        }
        return (RTEMem_BlockDescriptor *)0;
    }
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::LockedFindBlockChainHead( SAPDB_ULong                blockCount,
                                                             RTEMem_BlockChainHead *  & pChainHead,
                                                             RTESync_Spinlock         & lock,
                                                             RTEMem_BlockChainHead *    pNewHead,
                                                             RTEMem_BlockChainHead *  & pResultHead )
{
    RTESync_LockedScope chainLock(lock);
    {
        RTEMem_BlockChainHead ** ppChainHead = &pChainHead;
        /* second loop locked */
        while ( (*ppChainHead) 
             && blockCount != (*ppChainHead)->blockCount
             && blockCount >  (*ppChainHead)->blockCount )
        {
            ppChainHead = &((*ppChainHead)->pNextChain);
        }

        if ( !(*ppChainHead) || blockCount != (*ppChainHead)->blockCount )
        {
            pNewHead->blockCount           = blockCount;
            pNewHead->firstBlockDescriptor = 0;
            pNewHead->pNextChain           = (*ppChainHead);
            (*ppChainHead) = pResultHead = pNewHead;
            return true; // pNewHead is used
        }
        else
        {
            pResultHead = (*ppChainHead);
        }
    }
    return false; // release unused pNewHead
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::SearchFreeBlockChainHead( SAPDB_ULong                blockCount,
                                                                   RTEMem_BlockChainHead *  & pChainHead )
{
    pChainHead = m_freeBlockChainHead;
    while ( 0          != pChainHead 
         && blockCount != pChainHead->blockCount
         && blockCount >  pChainHead->blockCount )
    {
        pChainHead = pChainHead->pNextChain;
    }

    return ( 0 != pChainHead
          && blockCount == pChainHead->blockCount );
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::FindBlockChainHead( SAPDB_ULong               blockCount,
                                                       RTEMem_BlockChainHead * & pResultHead,
                                                       LockSelection             whichLock )
{
    RTEMem_BlockChainHead ** ppChainHead =
        ( LockUsedChainHead == whichLock ? &m_usedBlockChainHead : &m_freeBlockChainHead );

    // first loop dirty read. This is working, since once the block descriptor exists it will never
    // be released
    while ( (*ppChainHead) 
         && blockCount != (*ppChainHead)->blockCount
         && blockCount >  (*ppChainHead)->blockCount )
    {
        ppChainHead = &((*ppChainHead)->pNextChain);
    }

    if ( !(*ppChainHead) || blockCount != (*ppChainHead)->blockCount )
    {
        RTEMem_BlockChainHead * pNewHead = GetChainHeadFromPool();
        if ( !pNewHead )
        {
            return false;
        }

        if ( NoLockFreeChainHead == whichLock )
        {
            pNewHead->blockCount            = blockCount;
            pNewHead->firstBlockDescriptor  = 0;
            pNewHead->pNextChain            = (*ppChainHead);
            (*ppChainHead) = pResultHead = pNewHead;
        }
        else
        {
            if ( !LockedFindBlockChainHead( blockCount,
                      ( LockUsedChainHead == whichLock ? m_usedBlockChainHead : m_freeBlockChainHead ),
                      ( LockUsedChainHead == whichLock ? m_usedBlockChainLock : m_freeBlockChainLock ),
                                            pNewHead, 
                                            pResultHead) )
            {
                // meanwhile found it, thats ok...
                LockedReturnChainHeadToPool(pNewHead);
            }
        }
    }
    else
    {
        pResultHead = (*ppChainHead);
    }
    return true;
}

//---------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::DequeueSpecifiedFreeBlockDescriptor( RTEMem_BlockChainHead  & chainHead,
                                                                              RTEMem_BlockDescriptor * pBlockDescriptor )
{ 
    RTEMem_BlockDescriptor **pFreeBlockDescriptor = &(chainHead.firstBlockDescriptor);
    while ( *pFreeBlockDescriptor )
    {
        if ( *pFreeBlockDescriptor == pBlockDescriptor )
        {
            /* we found the search block descriptor */
            /* dequeue from free list */
            *pFreeBlockDescriptor = pBlockDescriptor->GetNextDescriptor();
            return true;
        }
        else
        {
            pFreeBlockDescriptor = &((*pFreeBlockDescriptor)->GetNextDescriptor());
        }
    }
    return false; // not found 
}

//---------------------------------------------------------------------------

RTEMem_BlockDescriptor * RTEMem_SystemPageCache::GetDequeuedFreeBlocks( SAPDB_ULong & releasedPages, 
                                                                        SAPDB_ULong & releasedBlocks,
                                                                        SAPDB_ULong & holdingPages,
                                                                        SAPDB_ULong & holdingBlocks )
{
    releasedPages = 0;
    releasedBlocks = 0;
    holdingPages = 0;
    holdingBlocks = 0;
    RTEMem_BlockDescriptor *freeBlockDescriptorChain = 0;

    RTESync_LockedScope chainLock(m_freeBlockChainLock);
    {
        RTEMem_BlockChainHead * pNextFreeChain = m_freeBlockChainHead;

        while ( pNextFreeChain )
        {
            RTEMem_BlockDescriptor **pFreeBlockDescriptor = &(pNextFreeChain->firstBlockDescriptor);
            while ( *pFreeBlockDescriptor )
            {
                RTEMem_BlockDescriptor *pCandidate = *pFreeBlockDescriptor;

                if ( !pCandidate->GetFirstSplinter() )
                {
                    ++releasedBlocks;
                    releasedPages += pNextFreeChain->blockCount;
                    /* we found a block descriptor for an unsplitted block, which can be freed later */
                    /* dequeue of pCandidate from free list */
                    *pFreeBlockDescriptor = pCandidate->GetNextDescriptor();
                    /* enqueue of pCandidate in freeBlockDescriptorChain */
                    EnqueueBlockDescriptor(freeBlockDescriptorChain, pCandidate);
                }
                else
                {
                    holdingPages += pNextFreeChain->blockCount;
                    holdingBlocks++;
                    pFreeBlockDescriptor = &pCandidate->GetNextDescriptor();
                }
            }
            pNextFreeChain = pNextFreeChain->pNextChain;
        }
    }
    return freeBlockDescriptorChain;
}

//------------------------------------------------------------------------------------

SAPDB_Bool RTEMem_SystemPageCache::GetDumpBlockChain( RTEMem_BlockChainHead  * pChainHead,
                                                      RTESync_Spinlock         &lock,
                                                      RTEMem_BlockDescriptor * &dumpBlockChain,
                                                      SAPDB_ULong              &chainHeads,
                                                      SAPDB_ULong              &blockDescriptors )
{
    RTESync_LockedScope chainLock(lock);
    {
        RTEMem_BlockChainHead * pNextChain = pChainHead;
        chainHeads = 0;
        blockDescriptors = 0;
        while ( pNextChain )
        {
            ++chainHeads;
            RTEMem_BlockDescriptor *pBlockDescriptor;
            RTEMem_BlockDescriptor *nextDescriptor = pNextChain->firstBlockDescriptor;

            pBlockDescriptor = GetDescriptorFromPool();
            if ( !pBlockDescriptor ) // out of memory
            {
                for ( pBlockDescriptor = dumpBlockChain; pBlockDescriptor; )
                {
                    pBlockDescriptor = dumpBlockChain->GetNextDescriptor();
                    LockedReturnDescriptorToPool(dumpBlockChain);
                    dumpBlockChain = pBlockDescriptor;
                }
                return false;
            }

            pBlockDescriptor->MarkAsUnusedBlock( dumpBlockChain );
            pBlockDescriptor->MarkAsFreeBlock(pNextChain->blockCount);
            dumpBlockChain = pBlockDescriptor;

            while ( nextDescriptor )
            {
                ++blockDescriptors;

                pBlockDescriptor = GetDescriptorFromPool();
                if ( !pBlockDescriptor ) // out of memory
                {
                    for ( pBlockDescriptor = dumpBlockChain; pBlockDescriptor; )
                    {
                        pBlockDescriptor = dumpBlockChain->GetNextDescriptor();
                        LockedReturnDescriptorToPool(dumpBlockChain);
                        dumpBlockChain = pBlockDescriptor;
                    }
                    return false;
                }

                pBlockDescriptor->MarkAsDumpBlock( dumpBlockChain, 
                                                   nextDescriptor,
                                                   pNextChain->blockCount );
                dumpBlockChain = pBlockDescriptor;

                nextDescriptor = nextDescriptor->GetNextDescriptor();
            }
            pNextChain = pNextChain->pNextChain;
        }
    }
    return true;
}

//------------------------------------------------------------------------------------
// Merging with free blocks uses the 'm_FirstSplinter' and 'm_NextSplinter' information
// of the block descriptor. It first follows the m_NextSplinter chain and merges all 
// blocks in that chain, that are found in free list.
// After that it starts merging with 'm_FirstSplinter' and follows the 'm_NextSplinter'
// merging all found blocks in free list that can be merged.

SAPDB_ULong RTEMem_SystemPageCache::MergeWithFreeBlocks( RTEMem_BlockDescriptor * &pBlockDescriptor,
                                                         SAPDB_ULong               blockCount,
                                                         RTEMem_BlockDescriptor * &pDequeuedBlockDescriptorChain)
{
    m_FreeSplinterBlocks++;

    SAPDB_ULong  mergedBlockCount   = blockCount;
    RTESync_LockedScope poolLock(m_freeBlockChainLock);
    {
        // begin with merging all 'next' splinters
        // follow up the chain of pNextSplinterAfterMergedBlock until chain ends or a splinter cannot be dequeued
        // each sucessfully dequeued block is added to the pDequeuedBlockDescriptorChain
        // and its size is added to the mergedBlockCount


        RTEMem_BlockDescriptor * pCandidate;                        // current candidate for merging
        RTEMem_BlockDescriptor * pNextSplinterAfterMergedBlock;     // next splinter of pBlockDescriptor
        SAPDB_ULong              candidateBlockCount;               // current candidate block count
        SAPDB_ULong              dummyBlockCount;                   // dummy if returned block count is unimportant
        RTEMem_BlockChainHead  * pFreeChainHead;      // head of chain used to search free splinter block
        // if we have no splinter behind merged block, the complete block so far is completly merged ...
        SAPDB_Bool isCompletlyMerged = true;

        // just to retrieve pNextSplinterAfterMergedBlock, candidateBlockCount will be blockCount
        pBlockDescriptor->GetSplitBlockInformation(pNextSplinterAfterMergedBlock, dummyBlockCount);
        candidateBlockCount = blockCount;

        // now work an all splinter that are behind the given block.
        // if block count is 0 the block is not free

        while ( pNextSplinterAfterMergedBlock && candidateBlockCount != 0 )
        {
            pCandidate = pNextSplinterAfterMergedBlock;
            pCandidate->GetSplitBlockInformation(pNextSplinterAfterMergedBlock, candidateBlockCount);

            if ( 
                 ( SearchFreeBlockChainHead( candidateBlockCount,
                                             pFreeChainHead )
                && DequeueSpecifiedFreeBlockDescriptor( *pFreeChainHead,
                                                        pCandidate )
                 )
              ||
                 (
                   DequeueSpecifiedFreeBlockDescriptor( *m_emergencyFreeBlockChainHead,
                                                        pCandidate )
                 )
               )
            {
                mergedBlockCount += candidateBlockCount;
                // do a splinter merge: update next splinter pointer and block size of block descriptor
                m_SplinterBlocks--;
                m_FreeSplinterBlocks--;
                pBlockDescriptor->MarkAsBiggerSplinter( mergedBlockCount,
                                                        pCandidate );
                EnqueueBlockDescriptor( pDequeuedBlockDescriptorChain, 
                                        pCandidate);
            }
            else
            {
                isCompletlyMerged = false;
                break;
            }

        }

        // Ok, we have now work an all splinter that are behind the given block

        // begin with searching last merging all 'first' splinters
        // if consecutive first splinters are found they are merged too.
        // since merging may create a new block size, the chance of a out of memory situation
        // exists while allocating a chain head. In that case the emergency free queue is used
        RTEMem_BlockDescriptor * pMergeNextSplinter = 0;
        RTEMem_BlockDescriptor * pMergeCandidateBlockDescriptor = 0;
        SAPDB_ULong mergeCandidateBlockCount = 0;

        pMergeNextSplinter = pBlockDescriptor->GetFirstSplinter();
        while ( pMergeNextSplinter != pBlockDescriptor )
        {
            pCandidate = pMergeNextSplinter;

            SAPDBERR_ASSERT_STATE(pCandidate);
            pCandidate->GetSplitBlockInformation(pMergeNextSplinter, candidateBlockCount);

            if (   candidateBlockCount != 0
                &&
                   (
                     ( SearchFreeBlockChainHead( candidateBlockCount,
                                                       pFreeChainHead )
                    && DequeueSpecifiedFreeBlockDescriptor( *pFreeChainHead,
                                                            pCandidate)
                     )
                  ||
                     (
                       DequeueSpecifiedFreeBlockDescriptor( *m_emergencyFreeBlockChainHead,
                                                            pCandidate )
                     )
                   )
              )
            {
                mergeCandidateBlockCount += candidateBlockCount;

                if ( !pMergeCandidateBlockDescriptor )
                {
                    pMergeCandidateBlockDescriptor = pCandidate;
                }
                else
                {
                    EnqueueBlockDescriptor( pDequeuedBlockDescriptorChain, 
                                            pCandidate);
                    m_SplinterBlocks--;
                    m_FreeSplinterBlocks--;
                    pMergeCandidateBlockDescriptor->MarkAsBiggerSplinter( mergeCandidateBlockCount,
                                                                          pCandidate );
                }
            }
            else
            {
                // we found a reason to stop merging....
                isCompletlyMerged = false;

                if ( pMergeCandidateBlockDescriptor )
                {
                    // reinsert into free queue failed, but since it is a splinter, this should not
                    // create a memory leak...
                    if ( !FindBlockChainHead( mergeCandidateBlockCount,
                                              pFreeChainHead,
                                              NoLockFreeChainHead ) )
                    {
                        // oh we have not enaugh memory to create a new chain head...
                        // To prevent memory leak, we use the emergency chain head...
                        // This means memory is temporarily lost
                        pFreeChainHead = m_emergencyFreeBlockChainHead;
                    }

                    ++m_FreeBlocks;
                    EnqueueBlockDescriptor( pFreeChainHead->firstBlockDescriptor,
                                            pMergeCandidateBlockDescriptor );
                    pMergeCandidateBlockDescriptor = 0;
                    mergeCandidateBlockCount = 0;
                }
            }
        }

        // if after the loop, the merge candidate block is not 0, it can be merged with the input block
        if ( pMergeCandidateBlockDescriptor )
        {
            mergedBlockCount += mergeCandidateBlockCount;

            // given block may be free now
            EnqueueBlockDescriptor( pDequeuedBlockDescriptorChain,
                                    pBlockDescriptor);

            if ( !isCompletlyMerged )
            {
                m_SplinterBlocks--;
                m_FreeSplinterBlocks--;

                pMergeCandidateBlockDescriptor->MarkAsBiggerSplinter( mergedBlockCount,
                                                                      pBlockDescriptor );
            }

            // return pointer to the descriptor of the first spliter of those merged
            pBlockDescriptor = pMergeCandidateBlockDescriptor;
        }

        if ( isCompletlyMerged )
        {
            m_SplittedBlocks--;
            m_SplinterBlocks--;
            m_FreeSplinterBlocks--;

            // if all block are merged, the block is completely merged -> no longer splitted...
            pBlockDescriptor->MarkAsMergedBlock(mergedBlockCount);
        }
        else
        {
            // otherwise it is marked as free block only and is still splitted
            pBlockDescriptor->MarkAsFreeBlock(mergedBlockCount);
        }

    }
    return mergedBlockCount;
}

//------------------------------------------------------------------------------------

void RTEMem_SystemPageCache::ReinsertRemainingSplinter( RTEMem_BlockChainHead  & freeChainHead,
                                                        RTEMem_BlockDescriptor * pBlockDescriptor,
                                                        void                   * splittedBlockAddress,
                                                        SAPDB_ULong              splittedBlockCount,
                                                        RTEMem_BlockDescriptor * splittedBlockDescriptor )
{
    splittedBlockDescriptor->MarkAsSplittedBlock(pBlockDescriptor,
                                                 splittedBlockAddress,
                                                 splittedBlockCount);
    EnqueueBlockDescriptor( freeChainHead.firstBlockDescriptor, 
                            splittedBlockDescriptor);
}

//------------------------------------------------------------------------------------

void RTEMem_SystemPageCache::DumpStatistic(SAPDB_Bool toConsole)
{
    SAPDB_ULong bytesUsed ;
    SAPDB_ULong maxBytesUsed ;
    SAPDB_ULong bytesControlled ;
    SAPDB_ULong countAlloc ;
    SAPDB_ULong countBaseAlloc ;
    SAPDB_ULong countDealloc ;
    SAPDB_ULong countBaseDealloc ;
    SAPDB_ULong errorCount ;
    SAPDB_ULong freeBlocks ;
    SAPDB_ULong splittedBlocks ;
    SAPDB_ULong splinterBlocks ;
    SAPDB_ULong freeSplinterBlocks ;
    SAPDB_ULong usedBlocks ;

    {
        m_freeBlockChainLock.Lock();
        m_usedBlockChainLock.Lock();
        {
            bytesUsed = m_BytesUsed;
            maxBytesUsed = m_MaxBytesUsed;
            bytesControlled = m_BytesControlled;
            countAlloc = m_CountAlloc;
            countBaseAlloc = m_CountBaseAlloc;
            countDealloc = m_CountDealloc;
            countBaseDealloc = m_CountBaseDealloc;
            errorCount = m_ErrorCount;
            freeBlocks = m_FreeBlocks;
            splittedBlocks = m_SplittedBlocks;
            splinterBlocks = m_SplinterBlocks;
            freeSplinterBlocks = m_FreeSplinterBlocks;
            usedBlocks = m_UsedBlocks;
        }
        m_usedBlockChainLock.Unlock();
        m_freeBlockChainLock.Unlock();
    }

    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                   RTEINFO_SYSTEMPAGECACHE_STATISTIC_BYTES,
                                   SAPDB_ToString(bytesUsed),
                                   SAPDB_ToString(maxBytesUsed),
                                   SAPDB_ToString(bytesControlled) ),
                               toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );

    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                   RTEINFO_SYSTEMPAGECACHE_STATISTIC_CALLS,
                                   SAPDB_ToString(countAlloc),
                                   SAPDB_ToString(countBaseAlloc),
                                   SAPDB_ToString(countDealloc),
                                   SAPDB_ToString(countBaseDealloc),
                                   SAPDB_ToString(errorCount) ),
                               toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );

    RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                   RTEINFO_SYSTEMPAGECACHE_STATISTIC_BLOCKS,
                                   SAPDB_ToString(usedBlocks),
                                   SAPDB_ToString(freeBlocks),
                                   SAPDB_ToString(freeSplinterBlocks),
                                   SAPDB_ToString(splittedBlocks),
                                   SAPDB_ToString(splinterBlocks) ),
                               toConsole ? MSG_DIAG_CONSOLE : MSG_DIAG );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
