/****************************************************************************

  module      : RTEMem_Allocator.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Common Allocator for small objects in RTE context (replaces malloc/free)
  description : Allocator based on SAPDBMem_RawAllocator implementation

                This allocator wrapps internally a SAPDBMem_RawAllocator that is using
                the RTEMem_RawAllocator instance. It is also a singleton.
                Specialty of this allocator is its synchronisation! This allocator can be
                used from within any thread 

  last changed: 2000-12-06  19:05
  see also    : RTEMem_Services.html
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

#ifdef SAPDB_DEBUG_ALLOCATOR
#  if  defined(_WIN32)
#    include <windows.h>
#  else
#    if defined(LINUX) || defined(FREEBSD)
/* With RedHat6.2 the mmap() prototype was fixed. This prevented the kernel from */
/* starting. The message given was 'invalid argument'. This Message was given due */
/* to the last argument of mmap() which was defined as 'off_t' and later 'off64_t' */
/* So on RedHat6.2 the last argument was expected to be a 64bit value, therefore */
/* the extra 32bits are collected from the stack. Such an offset is not PAGESIZE */
/* aligned, so it leads to 'invalid argument'... */
#        undef __USE_FILE_OFFSET64
#        define __USE_LARGEFILE64
#    endif /* LINUX */
#    include <sys/mman.h>
#  endif
#endif /* SAPDB_DEBUG_ALLOCATOR */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifdef SAPDB_DEBUG_ALLOCATOR
#define RTE_ALIGN(_val,_align) ((1+(((_val)-1)/(_align)))*(_align))
#endif
/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

#ifdef SAPDB_DEBUG_ALLOCATOR
union RawAllocHead {
  char MaximumAlignment[SAPDB_ALIGNMENT];
  double DoubleAlign;  // Force maximum alignment
  void * PointerAlign; // Force maximum alignment
  SAPDB_ULong ByteSize;
};
#endif

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
//
// This is expected to be called during a phase where multithreading does not is a problem!!!!
//
void RTEMem_Allocator::Initialize( SAPDB_ULong firstAlloc,
                                   SAPDB_ULong supplementAlloc,
                                   SAPDB_ULong maxAlloc )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_Allocator));

  if ( !m_Instance )
  {
    if ( maxAlloc == RTE_ISystem::UnlimitedMemory )
    {
      m_Instance = new (Space) RTEMem_Allocator(firstAlloc, supplementAlloc);
    }
    else
    {
      m_Instance = new (Space) RTEMem_Allocator(firstAlloc, supplementAlloc, maxAlloc);
    }
  }
}

/*---------------------------------------------------------------------------*/

#ifdef AIX
extern "C" void RTEMem_Allocator_Reset()
{
    RTEMem_Allocator::Reset();
}
#endif

/*---------------------------------------------------------------------------*/

  /*! 
    function: Instance

    description: Return reference to single instance of RTEMem_Allocator

    If never called before, default parameters are used to setup allocator.

    return value: Reference to RTEMem_Allocator instance
   */
SAPDBMem_IRawAllocator & RTEMem_Allocator::Instance( )
{
  RTEMem_Allocator::Initialize( (SAPDB_ULong)(1024*1024),
                                (SAPDB_ULong)(1024*1024),
                                RTE_ISystem::UnlimitedMemory );

  SAPDBERR_ASSERT_STATE(m_Instance != NULL);

  return *m_Instance;
}

RTEMem_Allocator                  * RTEMem_Allocator::m_Instance     = NULL;
SAPDBMem_SynchronizedRawAllocator * RTEMem_Allocator::m_Allocator    = NULL;
/*---------------------------------------------------------------------------*/

// private allocator only used once during initialization
RTEMem_Allocator::RTEMem_Allocator( SAPDB_ULong firstAlloc, 
                                    SAPDB_ULong supplementAlloc,
                                    SAPDB_ULong maxAlloc )
#ifdef SAPDB_DEBUG_ALLOCATOR
                                    : m_Spinlock((const SAPDB_UTF8 *)"RTEMem_Allocator")
#endif
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  m_Allocator = 0;
  {
      static RTEMem_AllocatorInfo AllocatorInfo((const SAPDB_UTF8 *)"RTE_Allocator", this, (const SAPDB_UTF8 *)"SystemHeap");
      RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
  }
#else
  SAPDBERR_ASSERT_STATE(m_Allocator == NULL);

  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(SAPDBMem_SynchronizedRawAllocator));

  m_Allocator = new (Space) SAPDBMem_SynchronizedRawAllocator(
       this->GetIdentifier(),
       RTEMem_BlockAllocator::Instance(), 
	   firstAlloc,
       supplementAlloc,
       SAPDBMem_SynchronizedRawAllocator::FREE_RAW_EXTENDS,
	   maxAlloc); 
#endif
}

// private allocator only used once during initialization
RTEMem_Allocator::RTEMem_Allocator( SAPDB_ULong firstAlloc, 
                                    SAPDB_ULong supplementAlloc)
#ifdef SAPDB_DEBUG_ALLOCATOR
                                    : m_Spinlock((const SAPDB_UTF8 *)"RTEMem_Allocator")
#endif
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  m_Allocator = 0;
  {
      static RTEMem_AllocatorInfo AllocatorInfo((const SAPDB_UTF8 *)"RTE_Allocator", this, (const SAPDB_UTF8 *)"SystemHeap");
      RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
  }
#else
  SAPDBERR_ASSERT_STATE(m_Allocator == NULL);

  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(SAPDBMem_SynchronizedRawAllocator));

  m_Allocator = new (Space) SAPDBMem_SynchronizedRawAllocator(
       this->GetIdentifier(),
       RTEMem_BlockAllocator::Instance(), 
	   firstAlloc,
       supplementAlloc,
       SAPDBMem_SynchronizedRawAllocator::FREE_RAW_EXTENDS);
#endif
}

/*!
  function: Allocate

  description: Allocates contiguous memory for 'count' bytes.

    The memory is suitably aligned for all types.

    The memory is not initialised.  
  arguments: count [in] Number of Bytes to allocate
  return value: Position where memory is allocated
 */
void * RTEMem_Allocator::Allocate(SAPDB_ULong count) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(count);
#ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  if (!p) 
    throw SAPDBMem_BadAllocException(__CONTEXT__, ByteSize);
#endif
  return p;
#else
#ifdef SAPDB_DEBUG_ALLOCATOR
  RTESync_LockedScope Lock(m_Spinlock);
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

  m_BytesUsed += count;
  if (m_BytesUsed > m_MaxBytesUsed)
      m_MaxBytesUsed = m_BytesUsed;
  m_BytesControlled += ByteSize;
  ++m_CountAlloc;

  RawAllocHead * pHead = reinterpret_cast<RawAllocHead *>(StartAddress);
  
  pHead->ByteSize = count;

  return (pHead+1);
#else
  return m_Allocator->Allocate(count);
#endif
#endif
}

/*! 
  function: Allocate

  description: Allocates contiguous memory for 'count' bytes,
    while trying to keep locality with a given memory location.

    If possible, the allocator tries to reserve the memory near
    a specified location.

    The memory is suitably aligned for all types.

    The memory is not initialised.  
  arguments: count [in] Number of Bytes to allocate
             hint [in] Hint where to allocate memory
  return value: Position where memory is allocated
*/
void* RTEMem_Allocator::Allocate(SAPDB_ULong count, const void * hint)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(count);
#else
#ifdef SAPDB_DEBUG_ALLOCATOR
  return Allocate(count);
#else
  return m_Allocator->Allocate(count, hint);
#endif
#endif
}

/*! 
  function: deallocate
  
  description: Frees the memory. 

   The pointer must have been obtained by a previous call to
   'allocate'.
  arguments: p [in] Pointer to memory to deallocate
*/
void RTEMem_Allocator::Deallocate(void * p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(p);
#else
#ifdef SAPDB_DEBUG_ALLOCATOR
  RTESync_LockedScope Lock(m_Spinlock);
  ++m_CountDealloc;
  RawAllocHead * pHead = ((reinterpret_cast<RawAllocHead *>(p))-1);
  SAPDB_ULong count = pHead->ByteSize;
  SAPDB_ULong ByteSize = 
    RTE_ALIGN( count + sizeof(RawAllocHead), RTE_ISystem::Instance().GetSystemPageSize() );

#if defined(_WIN32)
  DWORD oldProtect;
  VirtualProtect(pHead, ByteSize, PAGE_NOACCESS, &oldProtect);
#else
  if ( mprotect(pHead, ByteSize, PROT_NONE) )
  {
	perror("mprotect freed area");
  }
#endif
/*
  // ignore any error...
  if ( !RTE_ISystem::Instance().FreeSystemPages(pHead, ByteSize) )
  {
    SAPDBERR_ASSERT_STATE("Doubled Free" == "");
  }
 */
  m_BytesUsed -= count;
  m_BytesControlled -= ByteSize;
#else
  m_Allocator->Deallocate(p);
#endif
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_Allocator::CalcStatistics( SAPDB_ULong &BytesUsed,
                                       SAPDB_ULong &MaxBytesUsed,
                                       SAPDB_ULong &BytesControlled) 
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  RTESync_LockedScope Lock(m_Spinlock);
  BytesUsed       = m_BytesUsed;
  MaxBytesUsed    = m_MaxBytesUsed;
  BytesControlled = m_BytesControlled;
#else
  m_Allocator->CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_Allocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                                      SAPDB_ULong &CountDealloc) const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  CountAlloc   = 0;
  CountDealloc = 0;
#else
  m_Allocator->GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_Allocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                         SAPDB_ULong &CountDealloc) const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
#else
  m_Allocator->GetCallStatistics(CountAlloc, CountDealloc);
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTEMem_Allocator::GetChunkSize(const void* p)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  // TODO: we don't know chunk sizes
  return 1;
#endif
#ifdef SAPDB_DEBUG_ALLOCATOR
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
  return SAPDBMem_IRawAllocator::GetInvalidSize();
#else
  return m_Allocator->GetChunkSize(p);
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTEMem_Allocator::GetErrorCount() const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  return 0;
#else
  return m_Allocator->GetErrorCount();
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTEMem_Allocator::GetAllocFailedCount() const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  return 0;
#else
  return m_Allocator->GetAllocFailedCount();
#endif
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
