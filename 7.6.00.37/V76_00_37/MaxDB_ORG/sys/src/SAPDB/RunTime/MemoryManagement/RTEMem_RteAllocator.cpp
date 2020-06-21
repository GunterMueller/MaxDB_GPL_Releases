/****************************************************************************

  module      : RTEMem_RteAllocator.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Common Allocator for small objects in RTE context (replaces malloc/free)
  description : Allocator based on DoagLea Allocator implementation

                This allocator wrapps internally a SAPDBMem_DougLeaAllocator that is using
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
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
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

#if defined(_REENTRANT)
// for sqlgetthreadid() while RTEThread::Self may sometimes return NULL...
#include "heo07.h" /* nocheck */
#endif

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

#else

/*! The size of the emergency space */
#define RTEMEM_EMERGENCY_SPACE (256*1024)
SAPDB_ULong RTEMem_EmergencySpace[RTEMEM_EMERGENCY_SPACE/sizeof(SAPDB_ULong)];

/*!
  @class RTEMem_EmergencyAllocator
  @brief message list common allocator

  The specialty of this allocator is the emergency page, which is static memory on heap.
 */
class RTEMem_EmergencyAllocator : public SAPDBMem_IRawAllocator
{
public:
   /*!
      @brief          Does nothing

    */
   virtual ~RTEMem_EmergencyAllocator() {}

   static SAPDBMem_IRawAllocator & Instance(SAPDBMem_SynchronizedRawAllocator *baseAllocator)
   {
       SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_EmergencyAllocator));
       if ( !m_Instance )
       {
           m_Instance = new (Space) RTEMem_EmergencyAllocator(baseAllocator);
       }
       return *m_Instance;
   }

   /*!
      @brief          Allocates contiguous memory for &lt;ByteCount> bytes.
              The memory is aligned as indicated by the 'GetAlignmentSize' 
              method.
              The memory is not initialised.
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.
      @exception      From 'SAPDBMem_IRawAllocator' derived classes may throw a
                      'SAPDBMem_BadAllocException' if the allocation fails.

    */
   virtual void* Allocate(SAPDB_ULong ByteCount)
   {
       void *addr = 0;

       if ( !m_Allocator->DoNotUseForMessageOutputAllocation()
#if defined(_REENTRANT)
          || m_Allocator->BlockingThreadId() != (SAPDB_UInt4)sqlgetthreadid() 
#endif
          )
       {
           addr = m_Allocator->Allocate(ByteCount);
       }

       if ( !addr )
       {
           addr = EmergencyAllocate(ByteCount);
       }
       return addr;
   }

   /*!
      @brief          works like realloc
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @param          Hint [in] If possible, the allocator tries to reserve the memory at
                      the same position as given by Hint. This is useful for
                      resizing a memory block. If the reallocation was succesfull
                      the return value and Hint are identical.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.          
   */

   virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
   {
#if 1
       void *addr = ( m_Allocator->DoNotUseForMessageOutputAllocation() ? (void *)0 : m_Allocator->Allocate(ByteCount, Hint) );
#else
       void *addr = m_Allocator->Allocate(ByteCount, Hint);
#endif
       if ( !addr )
       {
           addr = EmergencyAllocate(ByteCount);
       }
       return addr;
   }

   /*!
      @brief          Frees the memory behind &lt;p>.
      @param          p [in] 
      @return         none


                    The memory must have been allocated by a previous call to
                    'Allocate'. This is checked by assertions.
                    !!! An implementation of this method may not throw an 
                    exception !!!

    */

   virtual void Deallocate(void* p)
   {
       if ( !EmergencyDeallocate(p) )
       {
           m_Allocator->Deallocate(p);
       }
   }

   /*!
      @brief          Checks the integrity of the allocator.
      @return         bool (only in the non-exception case)
      @exception      SAPDBMem_ConsistencyException 


                    This function checks the inner state of the allocator 
                    concerning consistency. Therefore, the total memory controlled 
                    by this allocator is checked.
                    If an inconsistency is detected, an excpetion is thrown.
                    The default implementation does nothing.

    */
   virtual bool CheckConsistency() { return m_Allocator->CheckConsistency(); }

   /*!
      @brief          returns the size af a chunk.
      @param          p [int] The address of the chunk. This must be the result of
                      a previous call of the Allocate method of the allocator.
                      The default implementation returns GetInvalidSize().
      @return         the internal size of the chunk in bytes. The size may differ
              from the required size of the corresponding Allocate call.

    */
   virtual SAPDB_ULong GetChunkSize(const void* p) { return m_Allocator->GetChunkSize(p); }

   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled)
   {
       BytesUsed = (m_NextEmergencySpacePointer - m_EmergencySpace);
       MaxBytesUsed = m_EmergencyMaxBytesUsed;
       BytesControlled = RTEMEM_EMERGENCY_SPACE;
   }

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                               SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = 0;
       CountDealloc = 0;
   }

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = m_EmergencyAllocs;
       CountDealloc = m_EmergencyDeallocs;
   }

   virtual SAPDB_ULong GetAllocFailedCount() const { return m_EmergencyAllocFailCount; }

   virtual SAPDB_ULong GetErrorCount() const { return 0; }

   virtual const SAPDB_UTF8 *GetIdentifier() const { return (SAPDB_UTF8 const *)"RTEMem_EmergencyAllocator"; }
private:
    /*!
      @brief ctor based on baseAllocator
      @return none
     */
   RTEMem_EmergencyAllocator(SAPDBMem_SynchronizedRawAllocator *baseAllocator)
   : m_Allocator(baseAllocator)
   , m_EmergencyAllocs(0)
   , m_EmergencyDeallocs(0)
   , m_EmergencyAllocFailCount(0)
   , m_EmergencyMaxBytesUsed(0)
   , m_EmergencyChunks(0)
   , m_EmergencySpace((SAPDB_Byte *)&RTEMem_EmergencySpace[0])
   , m_NextEmergencySpacePointer((SAPDB_Byte *)&RTEMem_EmergencySpace[0])
   {
      static RTEMem_AllocatorInfo AllocatorInfo((const SAPDB_UTF8 *)"RTEMem_EmergencyAllocator", this, (const SAPDB_UTF8 *)"");
      RTEMem_AllocatorRegister::Instance().Register(AllocatorInfo);
   }

    /*!
      @brief Emergency deallocation does not return memory...
      @param p [in] the candidate
      @return pointer to emergency space or 0 if emptied
     */
    void *EmergencyAllocate(SAPDB_ULong ByteCount)
    {
        RTE_IInterface::Instance().AtomicModify(m_EmergencyAllocs, (SAPDB_Int4 const)1);
        while ( ByteCount > 0 )
        {
            SAPDB_Byte *addr = m_NextEmergencySpacePointer;
            SAPDB_UInt4 totalBytesUsed = addr - m_EmergencySpace;
            if ( ByteCount <= (RTEMEM_EMERGENCY_SPACE - totalBytesUsed) )
            {
                RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, (SAPDB_Int4 const)1);
                SAPDB_Byte *nextAddr = addr + ByteCount + CalcAlignmentOffset(addr+ByteCount);
                if ( RTE_IInterface::Instance().AtomicCompareAndExchange( (void **)&m_NextEmergencySpacePointer
                                               , addr
                                               , nextAddr
                                               , (void **)&addr) )
                {
                    if ( m_EmergencyMaxBytesUsed < totalBytesUsed )
                    {
                        m_EmergencyMaxBytesUsed = totalBytesUsed;
                    }
                    return addr;
                }
                RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, (SAPDB_Int4 const)-1);
            }
            else
            {
                ++m_EmergencyAllocFailCount;
                break;
            }
        }
        return 0;
    }

    /*!
      @brief Emergency deallocation does not return memory...
      @param p [in] the candidate
      @return true if candidate is in emergency allocation range, false if outside
     */
    SAPDB_Bool EmergencyDeallocate(void* p)
    {
        if (  (((SAPDB_Byte *)p) >= m_EmergencySpace)
              && (((SAPDB_Byte *)p) <= (m_EmergencySpace+RTEMEM_EMERGENCY_SPACE)) )
        {
            RTE_IInterface::Instance().AtomicModify(m_EmergencyDeallocs, (SAPDB_Int4 const)1);   
            RTE_IInterface::Instance().AtomicModify(m_EmergencyChunks, (SAPDB_Int4 const)-1);
            SAPDB_Byte *addr = m_NextEmergencySpacePointer;

            RTE_IInterface::Instance().ReadMemoryBarrier();  // protect against some other threads parallel emergency allocate...

            // if last chunk was released, reset emergency next chunk pointer to start of emergency area
            if ( m_EmergencyChunks == 0 )
            {
                RTE_IInterface::Instance().AtomicCompareAndExchange( (void **)&m_NextEmergencySpacePointer
                                         , addr
                                         , m_EmergencySpace
                                         , (void **)&addr );
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    SAPDBMem_SynchronizedRawAllocator *m_Allocator; //!< the allocator used for normal allocation
    SAPDB_UInt4  m_EmergencyAllocs;           //!< the number of emergency allocs
    SAPDB_UInt4  m_EmergencyDeallocs;         //!< the number of emergency deallocs
    SAPDB_UInt4  m_EmergencyAllocFailCount;   //!< the number of failed emergency allocation attempts
    SAPDB_UInt4  m_EmergencyMaxBytesUsed;     //!< the maximum number of bytes used from emergency chunk
    SAPDB_UInt4  m_EmergencyChunks;           //!< the number of emergency chunks in use
    SAPDB_Byte  *m_EmergencySpace;            //!< the emergency space used
    SAPDB_Byte  *m_NextEmergencySpacePointer; //!< the next emergency space to return
    static RTEMem_EmergencyAllocator *m_Instance;   //!< singleton hook
}; /*!  @endclass RTEMem_EmergencyAllocator */

RTEMem_EmergencyAllocator * RTEMem_EmergencyAllocator::m_Instance = 0;

#endif

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

//
// This is expected to be called during a phase where multithreading does not is a problem!!!!
//
void RTEMem_RteAllocator::Initialize( SAPDB_ULong firstAlloc,
                                   SAPDB_ULong supplementAlloc,
                                   SAPDB_ULong maxAlloc )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_RteAllocator));

  if ( !m_Instance )
  {
    if ( maxAlloc == RTE_ISystem::UnlimitedMemory )
    {
      m_Instance = new (Space) RTEMem_RteAllocator(firstAlloc, supplementAlloc);
    }
    else
    {
      m_Instance = new (Space) RTEMem_RteAllocator(firstAlloc, supplementAlloc, maxAlloc);
    }
  }
}

/*---------------------------------------------------------------------------*/

  /*! 
    function: Instance

    description: Return reference to single instance of RTEMem_RteAllocator

    If never called before, default parameters are used to setup allocator.

    return value: Reference to RTEMem_RteAllocator instance
   */
SAPDBMem_IRawAllocator & RTEMem_RteAllocator::Instance( )
{
  RTEMem_RteAllocator::Initialize( (SAPDB_ULong)(1024*1024),
                                (SAPDB_ULong)(1024*1024),
                                RTE_ISystem::UnlimitedMemory );

  SAPDBERR_ASSERT_STATE(m_Instance != NULL);

  return *m_Instance;
}

RTEMem_RteAllocator               * RTEMem_RteAllocator::m_Instance     = NULL;
SAPDBMem_SynchronizedRawAllocator * RTEMem_RteAllocator::m_Allocator    = NULL;
/*---------------------------------------------------------------------------*/

#ifdef AIX
extern "C" void RTEMem_RteAllocator_Reset()
{
    RTEMem_RteAllocator::Reset();
}
#endif

/*---------------------------------------------------------------------------*/

// private allocator only used once during initialization
RTEMem_RteAllocator::RTEMem_RteAllocator( SAPDB_ULong firstAlloc, 
                                    SAPDB_ULong supplementAlloc,
                                    SAPDB_ULong maxAlloc )
#ifdef SAPDB_DEBUG_ALLOCATOR
                                    : m_Spinlock((const SAPDB_UTF8 *)"RTEMem_RteAllocator")
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
RTEMem_RteAllocator::RTEMem_RteAllocator( SAPDB_ULong firstAlloc, 
                                    SAPDB_ULong supplementAlloc)
#ifdef SAPDB_DEBUG_ALLOCATOR
                                    : m_Spinlock((const SAPDB_UTF8 *)"RTEMem_RteAllocator")
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
void * RTEMem_RteAllocator::Allocate(SAPDB_ULong count) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(count);
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
  m_BytesControlled += ByteSize;
  ++m_CountAlloc;

  RawAllocHead * pHead = reinterpret_cast<RawAllocHead *>(StartAddress);
  
  pHead->ByteSize = count;

  return (pHead+1);
#else
  /*--------------------------------------------------------*/
  return RTEMem_EmergencyAllocator::Instance(m_Allocator).Allocate(count);
  /*--------------------------------------------------------*/
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
void* RTEMem_RteAllocator::Allocate(SAPDB_ULong count, const void * hint)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(count);
#else
#ifdef SAPDB_DEBUG_ALLOCATOR
  return Allocate(count);
#else
  /*--------------------------------------------------------*/
  return RTEMem_EmergencyAllocator::Instance(m_Allocator).Allocate(count, hint);
  /*--------------------------------------------------------*/
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
void RTEMem_RteAllocator::Deallocate(void * p)
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
  /*--------------------------------------------------------*/
  RTEMem_EmergencyAllocator::Instance(m_Allocator).Deallocate(p);
  /*--------------------------------------------------------*/
#endif
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_RteAllocator::CalcStatistics( SAPDB_ULong &BytesUsed,
                                          SAPDB_ULong &MaxBytesUsed,
                                          SAPDB_ULong &BytesControlled) 
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  RTESync_LockedScope Lock(m_Spinlock);
  BytesUsed       = m_BytesUsed;
  MaxBytesUsed    = m_MaxBytesUsed;
  BytesControlled = m_BytesControlled;
#else
  SAPDB_ULong baseBytesUsed = 0;
  SAPDB_ULong baseMaxBytesUsed = 0;
  SAPDB_ULong baseBytesControlled = 0;
  m_Allocator->CalcStatistics(baseBytesUsed, baseMaxBytesUsed, baseBytesControlled);
  RTEMem_EmergencyAllocator::Instance(m_Allocator).CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
  BytesUsed += baseBytesUsed;
  MaxBytesUsed += baseMaxBytesUsed;
  BytesControlled += baseBytesControlled;
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_RteAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                                         SAPDB_ULong &CountDealloc) const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  CountAlloc   = 0;
  CountDealloc = 0;
#else
  SAPDB_ULong baseAlloc = 0;
  SAPDB_ULong baseDealloc = 0;
  m_Allocator->GetBaseAllocatorCallStatistics(baseAlloc, baseDealloc);
  RTEMem_EmergencyAllocator::Instance(m_Allocator).GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
  CountAlloc += baseAlloc;
  CountDealloc += baseDealloc;
#endif
}

/*---------------------------------------------------------------------------*/

void RTEMem_RteAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                         SAPDB_ULong &CountDealloc) const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
#else
  RTEMem_EmergencyAllocator::Instance(m_Allocator).GetCallStatistics(CountAlloc, CountDealloc);
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTEMem_RteAllocator::GetErrorCount() const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  return 0;
#else
  return m_Allocator->GetErrorCount() + RTEMem_EmergencyAllocator::Instance(m_Allocator).GetAllocFailedCount();
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong RTEMem_RteAllocator::GetAllocFailedCount() const
{
#ifdef SAPDB_DEBUG_ALLOCATOR
  return 0;
#else
  return m_Allocator->GetAllocFailedCount() + RTEMem_EmergencyAllocator::Instance(m_Allocator).GetAllocFailedCount();
#endif
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
