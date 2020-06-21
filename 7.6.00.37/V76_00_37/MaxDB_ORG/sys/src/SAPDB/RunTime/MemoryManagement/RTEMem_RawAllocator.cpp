/****************************************************************************

  module      : RTEMem_RawAllocator.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Memory Management
  description : Low Level MemoryManagement

  last changed: 2000-12-06  19:03
  see also    : example.html ...
  first created:2000-02-01  16:14

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <stddef.h>
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

#ifdef SAPDBMem_USE_SYSTEM_ALLOC
#include <stdlib.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define RTE_ALIGN(_val,_align) ((1+(((_val)-1)/(_align)))*(_align))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

union RawAllocHead {
  char MaximumAlignment[SAPDB_ALIGNMENT];
  double DoubleAlign;  // Force maximum alignment
  void * PointerAlign; // Force maximum alignment
  SAPDB_ULong ByteSize;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

RTEMem_RawAllocator * RTEMem_RawAllocator::m_Instance = 0;

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

#ifdef AIX
extern "C" void RTEMem_RawAllocator_Reset()
{
    RTEMem_RawAllocator::Reset();
}
#endif

/*---------------------------------------------------------------------------*/

SAPDBMem_IRawAllocator & RTEMem_RawAllocator::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_RawAllocator));

  if ( !m_Instance )
  {
    m_Instance = new (Space) RTEMem_RawAllocator();
    {
        static RTEMem_AllocatorInfo RawAllocatorInfo((const SAPDB_UTF8 *)"RTE_RawAllocator", m_Instance, (const SAPDB_UTF8 *)"SystemHeap");
        RTEMem_AllocatorRegister::Instance().Register(RawAllocatorInfo);
    }
  }
  return *m_Instance;
}

/*---------------------------------------------------------------------------*/

RTEMem_RawAllocator::RTEMem_RawAllocator() 
  : m_MemoryPageAllocator(RTEMem_BlockAllocator::Instance()),
    m_BytesUsed(0), 
    m_MaxBytesUsed(0),
    m_BytesControlled(0)
{
}

SAPDB_ULong RTEMem_RawAllocator::GetOverhead()
{
    return (SAPDB_ULong)sizeof(RawAllocHead);
}

/*---------------------------------------------------------------------------*/
//
// Allocate the memory using system page allocation
//
// Then initialize block using RawAllocHead and return pointer behind it
// (may loose part of a system memory block)
//
// Optimum allocation for
//   Size % SystemPageSize == (SystemPageSize - SAPDB_ALIGNMENT)
//
// Worst case overhead: 1 SystemPage per allocation !!!
//
void * RTEMem_RawAllocator::Allocate(SAPDB_ULong count)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(count);
#else
  m_CountAlloc.Increment();
  SAPDB_ULong ByteSize = 
    RTE_ALIGN( count + sizeof(RawAllocHead), RTE_ISystem::Instance().GetSystemPageSize() );

  void * StartAddress = RTE_ISystem::Instance().AllocSystemPages(ByteSize);

  if ( !StartAddress )
  {
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
    return (void *)0;
#else
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteSize);
#endif
  }

  SAPDB_ULong OldValue;
  m_BytesUsed.Increment(count, &OldValue);
  if ( OldValue+count > m_MaxBytesUsed.GetValue() ) // DDT: ToDo RTESys_AtomicOperation
    m_MaxBytesUsed.SetValue(OldValue+count);
  m_BytesControlled.Increment(ByteSize);

  RawAllocHead * pHead = reinterpret_cast<RawAllocHead *>(StartAddress);
  
  pHead->ByteSize = count;

  return (pHead+1);
#endif
}

/*---------------------------------------------------------------------------*/

void * RTEMem_RawAllocator::Allocate(SAPDB_ULong count, const void *hint)
{
  return Allocate(count);
}

/*---------------------------------------------------------------------------*/

void RTEMem_RawAllocator::Deallocate(void * p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(p);
#else
  m_CountDealloc.Increment();
  RawAllocHead * pHead = ((reinterpret_cast<RawAllocHead *>(p))-1);
  SAPDB_ULong count = pHead->ByteSize;
  SAPDB_ULong ByteSize = 
    RTE_ALIGN( count + sizeof(RawAllocHead), RTE_ISystem::Instance().GetSystemPageSize() );

  // ignore any error...
  if ( !RTE_ISystem::Instance().FreeSystemPages(pHead, ByteSize) )
  {
    SAPDBERR_ASSERT_STATE("Doubled Free" == "");
  }

  m_BytesUsed.Decrement(count);
  m_BytesControlled.Decrement(ByteSize);
#endif
}

/*!----------------------------------------------------------------------
    function: CalcStatistics
    description:  Outputs statistics about the memory usage of the allocator.
    arguments:    BytesUsed [out]
                 The number of bytes used by this allocater.
                 BytesControlled [out]
                 The number of bytes controlled by this allocator.

                 The default implementation returns GetInvalidSize() for 
                 both values.
    return value: none
-----------------------------------------------------------------------*/
void RTEMem_RawAllocator::CalcStatistics( SAPDB_ULong &BytesUsed,
                                          SAPDB_ULong &MaxBytesUsed,
                                          SAPDB_ULong &BytesControlled )
{
  BytesUsed       = m_BytesUsed;
  MaxBytesUsed    = m_MaxBytesUsed;
  BytesControlled = m_BytesControlled;
}

/*!----------------------------------------------------------------------
     Function: GetBaseAllocatorCallStatistics
     Ddescription:  Outputs statistics about the number of Allocate and Deallocate calls of
                   the allocator to the underlying base allocator.
     Arguments:    CountAlloc [out]
                   The number of calls of method Allocate of the base allocator.
                   CountDealloc [out]
                   The number of calls of method Deallocate of the base.

     Return value: none
-----------------------------------------------------------------------*/

void RTEMem_RawAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
	CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}

/*!----------------------------------------------------------------------
  Function: GetCallStatistics
  Description:  Outputs statistics about the number of Allocate and Deallocate calls.
  Arguments:    CountAlloc [out]
                The number of calls of method Allocate.
                CountDealloc [out]
                The number of calls of method Deallocate.

  Return value: none
----------------------------------------------------------------------*/

void RTEMem_RawAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                            SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_CountAlloc;
    CountDealloc = m_CountDealloc;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
