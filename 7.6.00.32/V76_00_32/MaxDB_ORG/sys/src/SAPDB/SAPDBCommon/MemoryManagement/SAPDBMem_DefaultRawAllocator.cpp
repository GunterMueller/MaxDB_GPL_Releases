/*!*****************************************************************************

  module:       SAPDBMem_DefaultRawAllocator.cpp

  ------------------------------------------------------------------------------

  responsible:  Stefan Baier

  special area: Memory management

  description:  Memory allocator using standard malloc/free functions.
                     
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

#include "SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include <stdlib.h> /* for malloc() and free() */

// Initialization of the static instance member
SAPDBMem_DefaultRawAllocator* SAPDBMem_DefaultRawAllocator::m_Instance = 0;


void* SAPDBMem_DefaultRawAllocator::Allocate(SAPDB_ULong Count)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(Count);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, Count);
#endif
  return p;
#else
    SAPDBERR_ASSERT_ARGUMENT(Count > 0);
    ++m_CountAlloc;
    SAPDB_Byte *Ptr = (SAPDB_Byte*) malloc(Count+SAPDBMem_IRawAllocator::GetAlignmentSize());

    if (Ptr == 0) {
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_BadAllocException(__CONTEXT__, Count);
        #else
            return 0;
        #endif
    }

    // Assure that the memory returned is always aligned properly
    // and remember the original pointer.
    SAPDB_ULong Offset = CalcAlignmentOffset(Ptr);
    SAPDBERR_ASSERT_STATE(Offset >= sizeof(void*)); // This should always be.
    *(reinterpret_cast<SAPDB_Byte**>(Ptr+Offset-sizeof(void*))) = Ptr;
    return Ptr + Offset;
#endif
}


void SAPDBMem_DefaultRawAllocator::Deallocate(void *p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(p);
#else
    SAPDBERR_ASSERT_ARGUMENT(p != 0);
    ++m_CountDealloc;
    SAPDB_Byte *OrgPtr;
    // Get the original pointer ...
    OrgPtr = reinterpret_cast<SAPDB_Byte*>
                    (*(reinterpret_cast<void**>(reinterpret_cast<SAPDB_Byte*>(p)-sizeof(void*))));
    // ... and delete it.
    free(OrgPtr);
#endif
}


SAPDBMem_DefaultRawAllocator& SAPDBMem_DefaultRawAllocator::Instance()
{
    // Static memory for the m_Instance member
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(SAPDBMem_DefaultRawAllocator));

    // If the Singleton is not yet constructed, do it
    if (m_Instance == 0) {
        m_Instance = new(Space) SAPDBMem_DefaultRawAllocator;
    }
    return *m_Instance;
}

void SAPDBMem_DefaultRawAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                    SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}

void SAPDBMem_DefaultRawAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                                     SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}
