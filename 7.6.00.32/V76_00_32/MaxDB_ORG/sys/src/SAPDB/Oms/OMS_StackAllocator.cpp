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
#include "Oms/OMS_StackAllocator.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*----------------------------------------------------------------------*/

OMS_StackAllocatorMemChunk* OMS_StackAllocator::MallocExtend(size_t extendSize) 
{
    OMS_StackAllocatorMemChunk* c = REINTERPRET_CAST(OMS_StackAllocatorMemChunk*, 
          m_pAlloc->Allocate(extendSize + OMS_StackAllocatorMemChunk::headerSize()));
    if (c == NULL){ 
      OMS_Globals::Throw(e_new_failed, "OMS_StackAllocator::Malloc", __MY_FILE__, __LINE__);
    }
    return c;
}

/*----------------------------------------------------------------------*/

void OMS_StackAllocator::InitStackAllocator(SAPDBMem_IRawAllocator *pAlloc, size_t memChunkSize)
{
  m_pAlloc = pAlloc;
  m_memChunkSize = memChunkSize - OMS_StackAllocatorMemChunk::headerSize();
}

/*----------------------------------------------------------------------*/

void OMS_StackAllocator::Free() 
{
  OMS_StackAllocatorMemChunk* pCurrChunk = m_usedChunks;
  if( pCurrChunk != NULL){
    // deallocate all chunks except one
    while (NULL != pCurrChunk->m_next)
    {
      OMS_StackAllocatorMemChunk* pDelete = pCurrChunk;
      pCurrChunk = pCurrChunk->m_next;
      m_pAlloc->Deallocate(pDelete);
    }
    m_usedChunks = pCurrChunk;
    m_rest       = m_memChunkSize - OMS_StackAllocatorMemChunk::headerSize();
    m_curr       = &pCurrChunk->m_mem;
    m_used       = 0; /* PTS 1107731 */
  }
  else{
    // no chunks allocated, reset
    m_usedChunks = NULL;
    m_rest       = 0;
    m_used       = 0; /* PTS 1107731 */
  }
}


