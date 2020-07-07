/*!
  @file           IFRUtil_StackAllocator.h
  @author         D039759
  @ingroup        IFR_Mem
  @brief          Allocator for stack-only stuff.
  @see            

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
#ifndef IFRUTIL_STACKALLOCATOR_H
#define IFRUTIL_STACKALLOCATOR_H

#include "SAPDB/Interfaces/Runtime/IFR_Types.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

IFR_BEGIN_NAMESPACE

/**
 * Simple allocator that has it's space inside.
 * 
 * The template parameter defines how large the internal buffer is.
 */
template <int blocksize>
class IFRUtil_StackAllocator
    : public SAPDBMem_IRawAllocator
{
public:
    inline IFRUtil_StackAllocator()
    {
        m_buffer = (char *) m_buffer_space;
        m_freelist.size = 0;
        m_freelist.next = (MemoryEntry *) m_buffer;
        m_freelist.prev = (MemoryEntry *) m_buffer;
        MemoryEntry *p = m_freelist.next;
        p->next = &m_freelist;
        p->prev = &m_freelist;
        p->size = sizeof(m_buffer_space);
    }
    
    inline virtual ~IFRUtil_StackAllocator()
    {}

    inline virtual void* Allocate(SAPDB_ULong sz)
    {
        // printf("### ALLOCATE %d\n", sz);
        MemoryEntry *l = m_freelist.next;
        IFR_size_t  alloc_size = ((sz + sizeof(MemoryEntry) + sizeof(double)) / sizeof(double)) 
            * sizeof(double);
        while(l != &m_freelist) {
            if(l->size >= alloc_size) {
                break;
            }
            l = l->next; 
        }
        if(l == &m_freelist) {
            return 0;
        }
        
        if(l->size > alloc_size + sizeof(MemoryEntry)) {
            // size is large, split the buffer
            char *p = (char *)l;
            p += alloc_size;
            MemoryEntry *tmp1 = (MemoryEntry *)p;
            tmp1->size = l->size - alloc_size;
            l->size = alloc_size;
            tmp1->next = l->next;
            l->next = tmp1->next->prev = tmp1;
            tmp1->prev = l;
        }
        
        l->prev->next = l->next;
        l->next->prev = l->prev;
        l->next = l->prev = 0;

        char *result = (char *)l;
        result += sizeof(MemoryEntry);
        return (void *) result;
    }

    inline virtual void *Allocate(SAPDB_ULong sz, const void *hint)
    {
        return 0;
    }

    inline virtual void Deallocate(void *p) 
    {
        // for now, nothing.
    }

    // -- SAPDBMem_IAllocatorInfo methods
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                                SAPDB_ULong &CountDealloc) const
    {
        CountAlloc = CountDealloc = 0;
    }

    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const
    {
        CountDealloc = CountAlloc = 0;
    }

private:
    IFRUtil_StackAllocator(const IFRUtil_StackAllocator&);
    IFRUtil_StackAllocator& operator =(const IFRUtil_StackAllocator&);
    
    struct MemoryEntry 
    {
        IFR_Int8     size;
        MemoryEntry *next;
        MemoryEntry *prev;
    };

    double       m_buffer_space[(blocksize + sizeof(double))/sizeof(double)];
    char        *m_buffer;
    MemoryEntry  m_freelist;
};

IFR_END_NAMESPACE

#endif
