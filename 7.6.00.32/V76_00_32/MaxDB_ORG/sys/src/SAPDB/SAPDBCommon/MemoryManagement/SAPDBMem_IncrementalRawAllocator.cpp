/*!*****************************************************************************

  module:        SAPDBMem_IncrementalRawAllocator.cpp

  ------------------------------------------------------------------------------

  responsible:  Stefan Baier

  special area: Memory Management

  description:  Implementation of 'SAPDBMem_IncrementalRawAllocator' class.

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




*******************************************************************************/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#include <stdlib.h>
#endif


/*!***************************************************************************
    Implementation of class SAPDBMem_IncrementalRawAllocator
*****************************************************************************/
  
SAPDBMem_IncrementalRawAllocator::SAPDBMem_IncrementalRawAllocator(
    SAPDBMem_IRawAllocator& RawAlloc,
    SAPDB_ULong MinSize
)
 : m_RawAllocator(RawAlloc),
    m_MinSize(MinSize),
    m_HistoryUsedSize(0),
    m_HistoryMaxUsedSize(0),
    m_FirstBlock(0),
    m_CountAlloc(0),
    m_CountDealloc(0), 
    m_BufferAllocator((SAPDB_Byte*)&MinSize+CalcAlignmentOffset(&MinSize), 1)   // dummy initialization
{
    // Let the actual block point to the address of the member m_FirstBlock.
    // Thus, when calling GetNewBlock(), m_FirstBlock will be filled with the address
    // of the first block.
    m_ActualBlock = &m_FirstBlock;
    // Initialize the incremental buffer allocator with a zero length block.
    // Thus, the first call to Allocate() will get a block from backend raw allocator.
    m_BufferAllocator.Initialize(0, 0);
}

//------------------------------------------------------------------------------

SAPDBMem_IncrementalRawAllocator::~SAPDBMem_IncrementalRawAllocator()
{
    // Free the memory of all blocks in the chain.
    while (m_FirstBlock != 0) {
        m_ActualBlock = m_FirstBlock;
        m_FirstBlock = *(reinterpret_cast<void**>(m_ActualBlock));
        m_RawAllocator.Deallocate(m_ActualBlock);
    };
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalRawAllocator::FreeAll()
{
    // Deallocate only, if blocks where allocated.
    if (m_FirstBlock == 0) { return; }
    // Deallocate all but the last block in the chain.
    void *NextBlock;
    while (m_FirstBlock != m_ActualBlock) {
        NextBlock = *(reinterpret_cast<void**>(m_FirstBlock));
        m_RawAllocator.Deallocate(m_FirstBlock);
        m_FirstBlock = NextBlock;
    };
    InitializeBufferAllocator(m_ActualBlock, m_ActualBlockSize);
    m_HistoryUsedSize = 0;
}

//------------------------------------------------------------------------------

void* SAPDBMem_IncrementalRawAllocator::Allocate(SAPDB_ULong ByteCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(ByteCount);
#else
    SAPDB_ULong BlockSize;
    void *p;

    if (m_BufferAllocator.GetCapacity() < ByteCount) {
        BlockSize = (ByteCount/m_MinSize + 1) * m_MinSize;
        InitializeBufferAllocator(GetNewBlock(BlockSize), BlockSize);
    }
    p = m_BufferAllocator.Allocate(ByteCount);
    return p;
#endif
}

//------------------------------------------------------------------------------

void* SAPDBMem_IncrementalRawAllocator::Allocate(SAPDB_ULong ByteCount, const void* Hint)
{
    return Allocate(ByteCount);
}

//------------------------------------------------------------------------------

bool SAPDBMem_IncrementalRawAllocator::CheckConsistency()
{
    void *NextBlock;
    SAPDB_ULong Used, MaxUsed, Controlled;
    CalcStatistics(Used, MaxUsed, Controlled);
    unsigned int MaxBlocks = Controlled/m_MinSize;

    NextBlock = m_FirstBlock;
    for (unsigned int i=0; i<MaxBlocks; ++i) {
        NextBlock = *(reinterpret_cast<void**>(NextBlock));
        if (NextBlock == 0) break;
    }
    if (NextBlock != 0) {
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_ConsistencyException(__CONTEXT__, "Too many blocks within chain!");
        #else
            return false;
        #endif
    }
    return true;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalRawAllocator::CalcStatistics(SAPDB_ULong &BytesUsed,
                                                      SAPDB_ULong &MaxBytesUsed,
                                                      SAPDB_ULong &BytesControlled)
{
    // Get statistics of the actual block.
    SAPDB_ULong Used, MaxUsed, Controlled;
    m_BufferAllocator.CalcStatistics(Used, MaxUsed, Controlled);

    BytesUsed       = m_HistoryUsedSize + Used;
    MaxBytesUsed    = m_HistoryMaxUsedSize + MaxUsed;
    BytesControlled = m_HistoryUsedSize + m_ActualBlockSize;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalRawAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = 0;
  CountDealloc = 0;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalRawAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                                         SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
}

//------------------------------------------------------------------------------

void* SAPDBMem_IncrementalRawAllocator::GetNewBlock(SAPDB_ULong Size)
{
    // Update the monitor statistics for the former block.
    m_HistoryUsedSize += m_ActualBlockSize;
    if (m_HistoryUsedSize > m_HistoryMaxUsedSize)
        m_HistoryMaxUsedSize = m_HistoryUsedSize;
    // Get new block from raw allocator
    void* Block = m_RawAllocator.Allocate(Size);
    // Store the block address at the beginning of the actual block
    // as a singly linked list.
    *(reinterpret_cast<void**>(m_ActualBlock)) = Block;
    // Remember the actual block and its size
    m_ActualBlock = Block;
    m_ActualBlockSize = Size;
    // Store zero at the beginning of the actual block
    *(reinterpret_cast<void**>(m_ActualBlock)) = 0;

    return m_ActualBlock;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalRawAllocator::InitializeBufferAllocator(void *Block, SAPDB_ULong BlockSize)
{
    SAPDB_Byte *Address = reinterpret_cast<SAPDB_Byte*>(Block);
    // The buffer allocator starts at an offset of GetAlignmentSize() from the block address.
    // This offset is large enough to hold the address (void*) of the next block.
    m_BufferAllocator.Initialize(Address+GetAlignmentSize(), BlockSize-GetAlignmentSize());
}
