/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Test.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#define MALLOC_ALIGN_MASK      (7)

inline static SAPDB_UInt4 AlignedSize(SAPDB_UInt4 req) {
  return  
  ((req) + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK);
}

namespace SAPDBMem_TestSpace
{
    struct ChunkInfo {
        int   m_size;
        char* m_ptr;
    };
    struct ChunkHeader {
        ChunkHeader() : m_next(NULL), m_cnt(0) {}
        ChunkInfo    m_chunk[256];
        ChunkHeader* m_next;
        int          m_cnt;
    };
};

using namespace SAPDBMem_TestSpace;

//-----------------------------------------------------------------------------

void SAPDBMem_Test::OneRandomTest (SAPDBMem_IRawAllocator& allocator, int inKB)
{

    bool         error = false;
    ChunkHeader* pHeader   = new(allocator) ChunkHeader();
    ChunkHeader* last      = pHeader;
    SAPDB_Int8   all       = sizeof(ChunkHeader);
    int          cntChunkHeader = 1;
    while (1) {
        size_t required = (rand() + 1) % 1000;
        all         += (SAPDB_Int8) AlignedSize(required);
        last->m_chunk[last->m_cnt].m_ptr  = (char*) allocator.Allocate(required);
        if (!last->m_chunk[last->m_cnt].m_ptr) break;
        memset (last->m_chunk[last->m_cnt].m_ptr, 0xFA, required);
        last->m_chunk[last->m_cnt].m_size = AlignedSize(required);
        ++last->m_cnt;
        if (last->m_cnt == 256) {
            if (all + sizeof(ChunkHeader) > inKB * 1024)
            {
                break;
            }
            last->m_next = new(allocator) ChunkHeader();
            if (0 == last->m_next)
            {
                break;
            }
            ++cntChunkHeader;
            all         += sizeof(ChunkHeader);
            last         = last->m_next;
        }
        if (all > inKB * 1024)
        {
            break;
        }
    }
    for (int loop = 0; loop < 10000; ++loop)
    {
        int headerIdx = rand() % cntChunkHeader;
        ChunkHeader* pCurr = pHeader;
        while (headerIdx > 0)
        {
            --headerIdx;
            pCurr = pCurr->m_next;
        }
        int idx = rand() % pCurr->m_cnt;
        if (pCurr->m_chunk[idx].m_ptr)
        {
            allocator.Deallocate (pCurr->m_chunk[idx].m_ptr);
            all -= pCurr->m_chunk[idx].m_size;
            pCurr->m_chunk[idx].m_size = 0;
            pCurr->m_chunk[idx].m_ptr  = 0;
        }
        int required = rand() % 10000;
        int alignedSize = AlignedSize(required);
        if (all + alignedSize > inKB * 1024)
        {
            required = (int) (inKB * 1024 - all);
            if (required <= 0) continue;
        }
        pCurr->m_chunk[idx].m_ptr  = (char*) allocator.Allocate(required);
        if (!pCurr->m_chunk[idx].m_ptr) break;
        pCurr->m_chunk[idx].m_size = alignedSize;
        memset (pCurr->m_chunk[idx].m_ptr, 0xFA, required);
        all += alignedSize;
    }
    all /= 1024;
    ChunkHeader* curr       = pHeader;
    ChunkHeader* pAuxHeader = pHeader;
    do {
        for (int ix = 0; ix < curr->m_cnt; ++ix) {
            if (curr->m_chunk[ix].m_ptr)
            {
                allocator.Deallocate (curr->m_chunk[ix].m_ptr);
            }
        }
        pHeader    = curr;
        pAuxHeader = pHeader;
        curr       = curr->m_next;
        destroy (pHeader, allocator);
    }
    while (pAuxHeader != last);
}

//-----------------------------------------------------------------------------

void SAPDBMem_Test::RandomTest()
{
    {
        // no check level
        SAPDBMem_RawAllocator allocator ((SAPDB_UTF8*) "TestAllocator", RTEMem_Allocator::Instance(), 16 * 1024, 16 * 1024, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS);
        allocator.SetCheckLevel (0);
        OneRandomTest (allocator, 1024);
    }
    for (int checkLevel = 1; checkLevel <= 32; checkLevel <<= 1)
    {
        SAPDBMem_RawAllocator allocator ((SAPDB_UTF8*) "TestAllocator", RTEMem_Allocator::Instance(), 16 * 1024, 16 * 1024, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS);
        allocator.SetCheckLevel (checkLevel);
        OneRandomTest (allocator, 1024);
    }
    {
        // all check levels
        SAPDBMem_RawAllocator allocator ((SAPDB_UTF8*) "TestAllocator", RTEMem_Allocator::Instance(), 16 * 1024, 16 * 1024, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS);
        allocator.SetCheckLevel (255);
        OneRandomTest (allocator, 1024);
    }
}

//-----------------------------------------------------------------------------

void SAPDBMem_Test::SizeTest()
{
    SAPDBMem_RawAllocator allocator ((SAPDB_UTF8*) "TestAllocator", RTEMem_Allocator::Instance(), 16 * 1024, 16 * 1024, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS);
    allocator.SetCheckLevel (1);
    void* pArr[1000];
    int ix;
    for (ix = 0; ix < 1000; ++ix)
    {
        pArr[ix] = 0;
    }
    for (ix = 0; ix < 1000; ++ix)
    {
        pArr[ix] = allocator.Allocate(ix);
        if (!pArr[ix])
        {
            break;
        }
        memset (pArr[ix], 0xFA, ix);
    }
    for (ix = 0; ix < 1000; ++ix)
    {
        allocator.Deallocate(pArr[ix]);
    }
}

//-----------------------------------------------------------------------------

void SAPDBMem_Test::Execute (int TestCase)
{
    SizeTest();
    RandomTest ();
}