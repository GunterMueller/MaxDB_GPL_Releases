/*!*****************************************************************************

  module:       SAPDBMem_BuddyAllocator.cpp

  ------------------------------------------------------------------------------

  responsible:  Stefan Baier

  special area: Memory Management

  description:  Implementation of 'SAPDBMem_IRawAllocator' interface.

  ------------------------------------------------------------------------------

  copyright:                 Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#include "SAPDBCommon/MemoryManagement/SAPDBMem_BuddyAllocator.hpp"


SAPDBMem_BuddyAllocator::SAPDBMem_BuddyAllocator( 
    SAPDB_ULong ByteCount,
    SAPDBMem_IBlockAllocator& BlockAllocator
) : m_BlockAllocator(BlockAllocator),
     m_Log2MaxSize(0),
     m_Blocks(0), 
     m_Sentinels(0), 
     m_BlockCount(0),
     m_BytesUsed(0),
     m_MaxBytesUsed(0)
{
    // Check the alignment of block and raw allocator
    const SAPDB_ULong requested = SAPDBMem_IRawAllocator::GetAlignmentSize();
    const SAPDB_ULong available = m_BlockAllocator.GetAlignmentSize();

    SAPDBERR_ASSERT_STATE(available % requested == 0);

    // Calculate the memory size that is sufficient to be able 
    // to allocate a chunk of at least <ByteCount> bytes at once.
    m_Log2MaxSize = GetLog2RequiredSize(ByteCount);
    const SAPDB_ULong RequiredSize = static_cast<SAPDB_ULong>(1) << m_Log2MaxSize;

    // Calculate the size of the memory region that includes the required size 
    // and the sentinels for memory managment.
    const SAPDB_ULong RegionSize =       
        // heap memory
        RequiredSize +
        // space for sentinel blocks (one more for easy indexing)
        (m_Log2MaxSize + 1) * sizeof(BuddyBlock);
        
    // calculate the number of blocks necessary for <RegionSize> bytes (rounded up)
    const SAPDB_ULong BlockSize = m_BlockAllocator.GetBlockSize();
    m_BlockCount = (RegionSize + BlockSize - 1) / BlockSize;

    // get initial memory from the backend block allocator 
    unsigned char* p = 
        reinterpret_cast<unsigned char *>(m_BlockAllocator.Allocate(m_BlockCount));
        
    m_Blocks = reinterpret_cast<BuddyBlock*>(p);
        
    m_Sentinels = reinterpret_cast<BuddyBlock*>(p + RequiredSize);

    //ToDo: check if Log2MaxSize is really the largest block size that can be allocated
    for (unsigned int i=0; i<=m_Log2MaxSize; ++ i) {
        // initialize sentinels of free lists 
        m_Sentinels[i].Initialize();
    }
    
    // Initially there is one maximally sized chunk
    BuddyBlock& Head = m_Blocks[0];
    Head.SetLog2Size(m_Log2MaxSize);
    Head.SetFree(); 
    // Insert this chunk into the corresponding free list.
    Head.Insert(m_Sentinels[m_Log2MaxSize]);
}


SAPDBMem_BuddyAllocator::~SAPDBMem_BuddyAllocator()
{
    // free memory from backend allocator
    m_BlockAllocator.Deallocate(m_Blocks, m_BlockCount );
}


void * SAPDBMem_BuddyAllocator::Allocate(SAPDB_ULong ByteCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(ByteCount);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#endif
  return p;
#else
    SAPDBERR_ASSERT_ARGUMENT(ByteCount > 0);

    const unsigned int k = GetLog2RequiredSize(ByteCount);
    
    unsigned int i = k;
    while ( 
        // there is a suitable free list 
        ( i <= m_Log2MaxSize )
        && 
        // there is no chunk on this list
        (m_Sentinels[i].GetNext() == m_Sentinels + i ) )
    {
        // Try next list 
        ++ i;
    }

    if ( i > m_Log2MaxSize ) {
        // found no free chunk 
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
        #else
            return 0;
        #endif
    }
    // Take block out of free list 
    BuddyBlock* Block = m_Sentinels[i].GetNext();
    Block->Remove();

    // While chunk is too large ... 
    while (Block->GetLog2Size() > k) {
        // ... split it
        // Half the size.
        Block->DecrementLog2Size();
        // Determine the buddy and add it to the free list.
        BuddyBlock* Buddy = Block->GetBuddy(*m_Blocks);
        Buddy->SetLog2Size(Block->GetLog2Size());
        Buddy->SetFree();
        Buddy->Insert(m_Sentinels[Buddy->GetLog2Size()]);
    } 

    // Allocate the block.
    Block->SetReserved();
    
    // Update statistics information.
    m_BytesUsed += Block->GetSize();
    if (m_BytesUsed > m_MaxBytesUsed)
        m_MaxBytesUsed = m_BytesUsed;

    // Return the usable part of the block
    return Block->GetBody();
#endif
}


void SAPDBMem_BuddyAllocator::Deallocate(void* p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(p);
#else
    if (p == 0) return;

    // get pointer to corresponding block from pointer to body. 
    BuddyBlock* Block = BuddyBlock::GetBlock(p);

    // sanity checks
    SAPDBERR_ASSERT_STATE(!Block->IsFree());
    SAPDBERR_ASSERT_STATE(Block->GetLog2Size() <= m_Log2MaxSize);
    SAPDBERR_ASSERT_STATE((Block >= m_Blocks)&&(Block < m_Sentinels));

    // update statistics information.
    m_BytesUsed -= Block->GetSize();

    // Free the block;
    Block->SetFree();

    // Try to join buddies 
    while (Block->GetLog2Size() < m_Log2MaxSize) {
        BuddyBlock* Buddy = Block->GetBuddy(*m_Blocks);
        // If buddy is reserved or is not free as a whole ...
        if ( !(Buddy->IsFree()) || (Buddy->GetLog2Size() != Block->GetLog2Size())) {    
            break;  // ... cannot join
        }
        // Take block out of free list
        Buddy->Remove();
        // Take the smallest address as new block
        if (Buddy < Block) {
            Block = Buddy;
        }
        // Join Block and Buddy to one Block
        Block->IncrementLog2Size();
    }

    Block->Insert(m_Sentinels[Block->GetLog2Size()]);
#endif
}

bool SAPDBMem_BuddyAllocator::CheckConsistency()
{
    unsigned int Count, k;
    BuddyBlock* Ptr;

    // Loop over all sentinel blocks (list heads)
    for (unsigned int i=0; i<=m_Log2MaxSize; ++ i) {
        Count = 0;
        Ptr = m_Sentinels + i;
        // Follow the next pointer list until the begin is reached.
        do {
            Count++;
            Ptr = Ptr->GetNext();
        } while (Ptr != (m_Sentinels + i));
        // Now follow the previous pointer list <Count> times.
        // Then the begin also must have been reached.
        Ptr = m_Sentinels + i;
        for (k=0; k<Count; ++k) {
            Ptr = Ptr->GetPrevious();
        }
        if (Ptr != (m_Sentinels + i)) {
            #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
                throw SAPDBMem_ConsistencyException(__CONTEXT__, "Inconsistent doubly linked list!");
            #else
                return false;
            #endif
        }
    }
    return true;
}

void SAPDBMem_BuddyAllocator::CalcStatistics(SAPDB_ULong &BytesUsed,
                                             SAPDB_ULong &MaxBytesUsed,
                                             SAPDB_ULong &BytesControlled)
{
    BytesUsed       = m_BytesUsed;
    MaxBytesUsed    = m_MaxBytesUsed;
    BytesControlled = m_BlockCount * m_BlockAllocator.GetBlockSize();
}

unsigned int SAPDBMem_BuddyAllocator::GetLog2RequiredSize(SAPDB_ULong ByteCount) const 
{
    // overhead for block header
    SAPDB_ULong size = ByteCount + BuddyBlock::GetOffsetOfBody();     
    // Check, that the size is at least the minimum size of a BuddyBlock
    // The minimum size of a BuddyBlock is the size of the header plus the minimum size of
    // body of a BuddyBlock (see 'SAPDBMem_BuddyBlock.hpp' for more details).
    if (size < sizeof(BuddyBlock)) { 
        size = sizeof(BuddyBlock);
    }
    // memory chunk must be sized a power of 2. 
    size = SAPDB_Log2Ceil(size);
    
    return (static_cast<unsigned int>(size));
}

