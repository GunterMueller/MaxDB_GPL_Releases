/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_STACKALLOCATOR_HPP
#define __OMS_STACKALLOCATOR_HPP

#include "ggg01.h"

class SAPDBMem_IRawAllocator;

#define OMS_DATA_ALIGNMENT  sizeof(void*)

//struct OMS_StackAllocatorMemChunk {
//  OMS_StackAllocatorMemChunk*  m_next;
//  unsigned char m_mem[OMS_STACK_CHUNK_SIZE - sizeof(OMS_StackAllocatorMemChunk*)];
//  long memSize() { return sizeof(m_mem); }
//};

struct OMS_StackAllocatorMemChunk {
  OMS_StackAllocatorMemChunk*  m_next;
  unsigned char m_mem;
  static size_t headerSize() { return sizeof(OMS_StackAllocatorMemChunk)-1; }
};

class OMS_StackAllocator {
public:
  OMS_StackAllocator() : 
       m_pAlloc(NULL), m_usedChunks(NULL), m_rest(0), m_used(0) {}
       void* Malloc( size_t requiredSize);
       OMS_StackAllocatorMemChunk* MallocExtend(size_t memChunkSize);
       void  Free();
       tsp00_8ByteCounter HeapUsed() const { return m_used; } /* PTS 1107731 */
       void  InitStackAllocator(SAPDBMem_IRawAllocator *pAlloc, size_t memChunkSize);

       ~OMS_StackAllocator(){Free();}
       inline size_t AlignSize(size_t val);

private :
  SAPDBMem_IRawAllocator* m_pAlloc;  
  OMS_StackAllocatorMemChunk* m_usedChunks;
  unsigned char*     m_curr;
  size_t             m_rest;
  tsp00_8ByteCounter m_used; /* PTS 1107731 */
  size_t             m_memChunkSize;
};

/*----------------------------------------------------------------------*/

inline size_t OMS_StackAllocator::AlignSize(size_t val)
{
  return ((val + (OMS_DATA_ALIGNMENT - 1)) & ~(OMS_DATA_ALIGNMENT - 1)); 
}

/*----------------------------------------------------------------------*/

inline void* OMS_StackAllocator::Malloc(size_t requiredSize) 
{
  void * p;
  size_t alignedSize = AlignSize(requiredSize);
  if (m_rest >= alignedSize) {
    m_rest -= alignedSize;
    p       = m_curr;
    m_curr += alignedSize;
    return p;
  }
  else {
    OMS_StackAllocatorMemChunk* c;
    if( alignedSize > m_memChunkSize)
    {
      c       = MallocExtend(alignedSize);
      m_used += alignedSize;
    }
    else
    {
      c       = MallocExtend(m_memChunkSize);
      m_used += m_memChunkSize; /* PTS 1107731 */
      m_rest  = m_memChunkSize - alignedSize;
      m_curr  = &c->m_mem + alignedSize;
    }
    c->m_next    = m_usedChunks;
    m_usedChunks = c;
    
    return &c->m_mem;
  }
}

#endif // __OMS_STACKALLOCATOR_HPP
