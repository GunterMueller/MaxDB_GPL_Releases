/*!*****************************************************************************

  module:       SAPDBMem_IncrementalBufferAllocator.cpp

  ------------------------------------------------------------------------------

  responsible:  Stefan Baier

  special area: Memory Management

  description:  Simple implementation of the 'SAPDBMem_IRawAllocator' interface.

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

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp"

#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#include <stdlib.h>
#endif


/*!***************************************************************************
    Implementation of class SAPDBMem_IncrementalBufferAllocator
*****************************************************************************/
  
SAPDBMem_IncrementalBufferAllocator::SAPDBMem_IncrementalBufferAllocator(void* Address, SAPDB_ULong Length, bool Align)
{
    Initialize(Address, Length, Align);
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::Initialize(void* Address, SAPDB_ULong Length, bool Align)
{
    SAPDB_Int Offset = (Align) ? SAPDBMem_IRawAllocator::CalcAlignmentOffset(Address) : 0;
    m_Memory      = reinterpret_cast<SAPDB_Byte*>(Address) + Offset; 
    m_Size        = Length - Offset;
    m_MaxUsedSize = 0;
    m_FreeMemory  = m_Memory;
    m_FreeSize    = m_Size;
    m_LastPointer = 0;
    m_LastSize    = 0;
    m_TotalAllocatedSize = 0;
    m_CountAlloc = 0;
    m_CountDealloc = 0;
}

//------------------------------------------------------------------------------

void* SAPDBMem_IncrementalBufferAllocator::Allocate(SAPDB_ULong ByteCount)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(ByteCount);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#endif
  return p;
#else
    SAPDBERR_ASSERT_STATE(IsAssigned());
    SAPDBERR_ASSERT_ARGUMENT(ByteCount > 0);
    ++m_CountAlloc;
    // Check, if there is enough space left 
    if (ByteCount > m_FreeSize) {
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
        #else
            return 0;
        #endif
    }

    // remember actual free memory pointer
    m_LastPointer = m_FreeMemory;
    m_LastSize = ByteCount;

    // adjust m_FreeMemory and m_FreeSize for the next position suitable aligned for all types
    SAPDB_ULong AlignedSize = (ByteCount + GetAlignmentSize() - 1) / GetAlignmentSize(); 
    AlignedSize *= GetAlignmentSize();
    m_FreeMemory += AlignedSize;
    if (AlignedSize > m_FreeSize) {
        m_FreeSize = 0;
    }
    else {
        m_FreeSize -= AlignedSize;
    }

    m_TotalAllocatedSize += ByteCount;
    return m_LastPointer;
#endif
}

//------------------------------------------------------------------------------

void* SAPDBMem_IncrementalBufferAllocator::Allocate(SAPDB_ULong ByteCount, const void* Hint) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(ByteCount);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#endif
  return p;
#else
    // If the last allocation returned <Hint>, try to reallocate memory at this position
    ++m_CountAlloc;
    if ((Hint != 0) && (Hint == m_LastPointer) && (ByteCount >= m_LastSize)) {
        // Check, if there is enough space left 
        if (ByteCount-m_LastSize > m_FreeSize) {
            #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
                throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
            #else
                return 0;
            #endif
        }
        // Set the free position to the position of the last call to 'Allocate'
        m_FreeMemory = m_LastPointer;
        m_FreeSize += m_LastSize;
        if (m_Size-m_FreeSize > m_MaxUsedSize)
            m_MaxUsedSize = m_Size-m_FreeSize;
        // Allocate the same memory again
        Allocate(ByteCount);
        SAPDBERR_ASSERT_STATE(m_LastPointer == Hint);
        return m_LastPointer;
    }
    else {
        return Allocate(ByteCount);
    }
#endif
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::Deallocate(void* Ptr)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(Ptr);
#else
    // Assure, that <Ptr> came from this allocator.
    SAPDBERR_ASSERT_ARGUMENT(Ptr >= m_Memory);
    SAPDBERR_ASSERT_ARGUMENT(Ptr < m_Memory+m_Size);
    ++m_CountDealloc;
#endif
}

//------------------------------------------------------------------------------

bool SAPDBMem_IncrementalBufferAllocator::CheckConsistency()
{
    if (m_Size < m_FreeSize) {
        #ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
            throw SAPDBMem_ConsistencyException(__CONTEXT__, "FreeSize is larger than buffer size!");
        #else
            return false;
        #endif
    }
    return true;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::CalcStatistics(SAPDB_ULong &BytesUsed,
                                                         SAPDB_ULong &MaxBytesUsed,
                                                         SAPDB_ULong &BytesControlled) 
{
    SAPDBERR_ASSERT_STATE(m_FreeSize <= m_Size);
    BytesUsed       = m_Size-m_FreeSize;
    MaxBytesUsed    = m_MaxUsedSize;
    BytesControlled = m_Size;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
	CountAlloc   = 0;
    CountDealloc = 0;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                                            SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
}

//------------------------------------------------------------------------------

void SAPDBMem_IncrementalBufferAllocator::FreeAll()
{
    m_FreeMemory = m_Memory;
    m_FreeSize = m_Size;
}

