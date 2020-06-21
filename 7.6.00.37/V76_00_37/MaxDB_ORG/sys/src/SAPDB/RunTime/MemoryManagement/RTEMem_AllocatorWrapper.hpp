/*!
  @file           RTEMem_AllocatorWrapper.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Allocate Only Allocator Wrapper for registered allocator usage

   This class allows to wrap SAPDBMem_IRawAllocators with an 
   allocator registering interface. Such an allocator is automatically
   registering and  deregistering it in the RTEMem_AllocatorRegister
   singleton.

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
#ifndef RTEMEM_ALLOCATORWRAPPER_HPP
#define RTEMEM_ALLOCATORWRAPPER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

/*!
  @class RTEMem_AllocatorWrapper
  @brief wrapper to allow synchronized allocate and deallacote of an unsynchronized allocator
 */
class RTEMem_AllocatorWrapper : public SAPDBMem_IRawAllocator
{
public:
   /*!----------------------------------------------------------------------
   @brief ctor
   Constructs an allocator from the provided RawAllocator.

   The Allocator is automatically registered with RTEMem_AllocatorRegister
   
   @param identifier [in] the name for this allocator
   @param baseAllocator [in] the wrapped allocator
   prototypes:  1
   -----------------------------------------------------------------------*/
  
    RTEMem_AllocatorWrapper( 
        const SAPDB_UTF8       * identifier,
        SAPDBMem_IRawAllocator & baseAllocator ) :
        m_AllocatorInfo(identifier),
        m_BaseAllocator(baseAllocator),
        m_AllocatedBytes( (SAPDB_ULong)0 ),
        m_MaxAllocatedBytes( (SAPDB_ULong)0 ),
        m_CountAlloc( (SAPDB_ULong)0 ),
        m_CountDealloc( (SAPDB_ULong)0 )
    {
        m_AllocatorInfo = RTEMem_AllocatorInfo(identifier, this, baseAllocator.GetIdentifier());
        RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
    }
  
   /*!
     @brief Deregister and destruct allocator
    */
    virtual ~RTEMem_AllocatorWrapper()
    {
        RTEMem_AllocatorRegister::Instance().Deregister(m_AllocatorInfo);
    }

   /*!----------------------------------------------------------------------
   @brief  Allocates contiguous memory for given number of bytes.
                 The memory is aligned as indicated by the 'GetAlignmentSize' 
                 method.
                 The memory is not initialised.
   @param ByteCount [in]
                 The size in bytes of the memory to be allocated. This 
                 argument must be greater zero.
   @return [void*] A pointer to the allocated raw memory block of 'ByteCount' 
                 bytes. If exception handling is disabled, a NULL pointer will
                 be returned in case of insufficient memory.
   exceptions:   From 'SAPDBMem_IRawAllocator' derived classes may throw a
                 'SAPDBMem_BadAllocException' if the allocation fails.
   -----------------------------------------------------------------------*/
   virtual void* Allocate(SAPDB_ULong ByteCount)
   {
       void *result = m_BaseAllocator.Allocate(ByteCount);
       if ( 0 != result )
       {
           SAPDB_ULong OldValue;
           SAPDB_ULong Increment = m_BaseAllocator.GetChunkSize(result);
           m_AllocatedBytes.Increment(Increment, &OldValue);
           if ( OldValue+Increment > m_MaxAllocatedBytes.GetValue() ) // DDT: ToDo RTESys_AtomicOperation
    		   m_MaxAllocatedBytes.SetValue(OldValue+Increment);
       }
       m_CountAlloc.Increment();
       return result;
   }

   /*!----------------------------------------------------------------------
   @brief  Allocates contiguous memory for given number of bytes.
                 The memory is aligned as indicated by the 'GetAlignmentSize' 
                 method.
                 The memory is not initialised.
   @param ByteCount [in] The size in bytes of the memory to be allocated. This 
                 argument must be greater zero.
   @param Hint [in] If possible, the allocator tries to reserve the memory at
                 the same position as given by 'Hint'. This is useful for
                 resizing a memory block. If the reallocation was succesfull
                 the return value and 'Hint' are identical.
   @return [void*] A pointer to the allocated raw memory block of 'ByteCount' 
                 bytes. If exception handling is disabled, a NULL pointer will
                 be returned in case of insufficient memory.
   exceptions:   From 'SAPDBMem_IRawAllocator' derived classes may throw a
                 'SAPDBMem_BadAllocException' if the allocation fails.
   -----------------------------------------------------------------------*/
   virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
   {
       void *result = m_BaseAllocator.Allocate(ByteCount,Hint);
       if ( 0 != result )
       {
           SAPDB_ULong OldValue;
           SAPDB_ULong Increment = m_BaseAllocator.GetChunkSize(result);
           m_AllocatedBytes.Increment(Increment, &OldValue);
           if ( OldValue+Increment > m_MaxAllocatedBytes.GetValue() ) // DDT: ToDo RTESys_AtomicOperation
    		   m_MaxAllocatedBytes.SetValue(OldValue+Increment);
       }
       m_CountAlloc.Increment();
       return result;
   }

   /*!----------------------------------------------------------------------
   @brief Frees the memory behind <p>.
                 
   @param p [in] memory chunk to free

   @return none
   -----------------------------------------------------------------------*/
   virtual void Deallocate(void* p)
   {
     if (0 != p)
     {
         m_AllocatedBytes.Decrement(m_BaseAllocator.GetChunkSize(p));
         m_BaseAllocator.Deallocate(p);
     }
     m_CountDealloc.Increment();
   }

   /*!----------------------------------------------------------------------
   @brief Outputs statistics about the memory usage of the allocator.
                 This implementation always returns total number of bytes allocated

   @param BytesUsed [out] The number of bytes used by this allocater.
   @param MaxBytesUsed [out] The maximum number of bytes used by this allocater.
   @param BytesControlled [out] The number of bytes controlled by this allocator.

   @return none
   -----------------------------------------------------------------------*/
   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
	                           SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled)
   { 
      SAPDB_ULong byteCount = m_AllocatedBytes;
      BytesUsed       = byteCount;
	  MaxBytesUsed    = m_MaxAllocatedBytes;
      BytesControlled = byteCount;
   }

   /*!----------------------------------------------------------------------
   @brief Outputs statistics about the number of Allocate and Deallocate calls of
                 the allocator to the underlying base allocator.
   @param CountAlloc [out]
                 The number of calls of method Allocate of the base allocator.
   @param CountDealloc [out]
                 The number of calls of method Deallocate of the base.
   @return none
   -----------------------------------------------------------------------*/

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = m_CountAlloc;
       CountDealloc = m_CountDealloc;
   }

  /*!----------------------------------------------------------------------
   function: GetCallStatistics
   description:  Outputs statistics about the number of Allocate and Deallocate calls.
   arguments:    CountAlloc [out]
                 The number of calls of method Allocate.
                 CountDealloc [out]
                 The number of calls of method Deallocate.

   return value: none
   -----------------------------------------------------------------------*/

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
   {
       CountAlloc = m_CountAlloc;
       CountDealloc = m_CountDealloc;
   }

   /*!----------------------------------------------------------------------
   @brief returns the size of a memory chunk of the allocator.
   @param p [in] the address of the chunk. Must have been returned by a call
                   of method Allocate.
 
   @return the size of the chunk in bytes
   -----------------------------------------------------------------------*/
   virtual SAPDB_ULong GetChunkSize(const void* p)
   {
       return m_BaseAllocator.GetChunkSize(p); 
   }

   /*!----------------------------------------------------------------------
   @brief Returns the number of errors found and corrected by the allocator

   This wrapper is not able to identify errors, i.e. returns 0

   @return always 0
    -----------------------------------------------------------------------*/

   virtual SAPDB_ULong GetErrorCount() const
   {
       return m_BaseAllocator.GetErrorCount(); 
   }

   virtual SAPDB_ULong GetAllocFailedCount() const
   {
       return m_BaseAllocator.GetAllocFailedCount(); 
   }

   /*!
   @brief Return the identifier of this allocator

   @return Value given in constructor
  */
  virtual const SAPDB_UTF8 * GetIdentifier() const 
      { return m_AllocatorInfo.GetIdentifier(); }

private:
   RTEMem_AllocatorInfo                    m_AllocatorInfo;
   SAPDBMem_IRawAllocator                & m_BaseAllocator;
   RTESync_InterlockedCounter<SAPDB_ULong> m_AllocatedBytes;
   RTESync_InterlockedCounter<SAPDB_ULong> m_MaxAllocatedBytes;
   RTESync_InterlockedCounter<SAPDB_ULong> m_CountAlloc;
   RTESync_InterlockedCounter<SAPDB_ULong> m_CountDealloc;
};

#endif
