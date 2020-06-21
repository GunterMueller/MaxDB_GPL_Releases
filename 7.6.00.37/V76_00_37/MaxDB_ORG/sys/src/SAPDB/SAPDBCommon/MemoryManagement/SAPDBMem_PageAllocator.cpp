/*!*****************************************************************************

  module:        SAPDBMem_PageAllocator.cpp

  ------------------------------------------------------------------------------

  responsible:  Stefan Baier

  special area: Memory management

  description:  Raw allocator for allocating memory pages of constant size.
                     
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
#if defined(SAPDBMem_USE_SYSTEM_ALLOC) && defined(S390X)
#include <stdlib.h>
#endif

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_PageAllocator.hpp"


/*!***************************************************************************
    Implementation of class SAPDBMem_PageAllocator
*****************************************************************************/

SAPDBMem_PageAllocator::SAPDBMem_PageAllocator(SAPDB_ULong PageSize)
 : m_PageSize(PageSize), m_FreePages(0), m_FirstFreePagePtr(0), m_CountAlloc(0), m_CountDealloc(0)
{
    // Check that two pointer fit into one page
    SAPDBERR_ASSERT_ARGUMENT(PageSize >= 2*sizeof(void*));
}
 
 
void* SAPDBMem_PageAllocator::Allocate(SAPDB_ULong ByteCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#ifdef WIN32
  char *p = (char*) malloc(ByteCount + 4096 + sizeof(int));
  if (p) {
    int delta = 4096 - (((int) p + sizeof(int)) & 4095);
    p += delta + sizeof(int);
    int *cptr = (int*) p;
    cptr[-1] = delta + sizeof(int);
  }
#else
  void *p = malloc(ByteCount);
#endif
  
  // initialize pages used for datacache to prevent false uninitialized mem warnings in Purify
  if (p && ByteCount >= 8192) {
    int *ptr = (int*) p;
    int cnt = ByteCount / sizeof(int);
    while (cnt--) {
      *ptr++ = 0xeffeabba;
    }
  }
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#endif
  return p;
#else
    Page* Ptr;

    ++m_CountAlloc;
    // Get the pointer of the first free page in the chain.
    Ptr = m_FirstFreePagePtr;

    // If <ByteCount> is not equal to the page size, don't return memory
    if (ByteCount != m_PageSize) {
        Ptr = 0;
    }

    // Check for insufficient memory.
    if (Ptr == 0) {
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
        #else
            return 0;
        #endif
    }

    // Update the chain of free pages.
    m_FirstFreePagePtr = Ptr->NextPage;
    if (m_FirstFreePagePtr != 0) {
        m_FirstFreePagePtr->PreviousPage = 0;
    }
    Ptr->NextPage = 0;

    // Decrement the number of free pages
    --m_FreePages;

    return Ptr;
#endif
}


void SAPDBMem_PageAllocator::Deallocate(void *Ptr)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#ifdef WIN32
  if (Ptr) {
    int *cptr = (int*) Ptr;
    char *fptr = (char*) Ptr;
    free(fptr - cptr[-1]);
  }
#else
  free(Ptr);
#endif
#else
    ++m_CountDealloc;

    Page* PagePtr = reinterpret_cast<Page*>(Ptr);

    // Chain the free page into the list.
    PagePtr->NextPage = m_FirstFreePagePtr;
    PagePtr->PreviousPage = 0;
    if (m_FirstFreePagePtr != 0)
        m_FirstFreePagePtr->PreviousPage = PagePtr;
    m_FirstFreePagePtr = PagePtr;

    // Increment the number of free pages
    ++m_FreePages;
#endif
}


void SAPDBMem_PageAllocator::AddMemoryBlock(void *Ptr, SAPDB_ULong Size)
{
    SAPDB_Byte *NextPtr;
    Page *PagePtr, *PrevPtr;

    // Check that <Size> is a multiple of m_PageSize
    SAPDBERR_ASSERT_ARGUMENT((Size % m_PageSize) == 0);

    // Divide the memory block into pages and chain them.
    NextPtr = reinterpret_cast<SAPDB_Byte*>(Ptr);
    PrevPtr = 0;
    for (SAPDB_ULong i=0; i<(Size/m_PageSize); ++i) {
        PagePtr = reinterpret_cast<Page*>(NextPtr);
        NextPtr += m_PageSize;
        PagePtr->NextPage = reinterpret_cast<Page*>(NextPtr);
        PagePtr->PreviousPage = PrevPtr;
        PrevPtr = reinterpret_cast<Page*>(PagePtr);
    }
    // Chain the new block into the free list.
    PagePtr->NextPage = m_FirstFreePagePtr;
    if (m_FirstFreePagePtr != 0) {
        m_FirstFreePagePtr->PreviousPage = PagePtr;
    }
    m_FirstFreePagePtr = reinterpret_cast<Page*>(Ptr);

    // Fit the number of free pages.
    m_FreePages += Size/m_PageSize;
}

void SAPDBMem_PageAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
	CountAlloc   = 0;
    CountDealloc = 0;
}

void SAPDBMem_PageAllocator::GetCallStatistics
(
 SAPDB_ULong &CountAlloc,
 SAPDB_ULong &CountDealloc
 ) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}


