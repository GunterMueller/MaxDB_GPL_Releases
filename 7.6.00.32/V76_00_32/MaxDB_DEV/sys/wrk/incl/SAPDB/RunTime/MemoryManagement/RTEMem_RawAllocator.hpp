/*!
  @file           RTEMem_RawAllocator.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          RunTime RawAllocator interface for memory management.

  This is a base allocator used by all other allocators to get memory

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
#ifndef RTEMEM_RAWALLOCATOR_HPP
#define RTEMEM_RAWALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
class SAPDBMem_IBlockAllocator;

/*! 
  @class RTEMem_RawAllocator

  @brief 'Synchronized' allocator interface for 'raw' memory. 

    Class implementing the interface to provide methods for reserving
    and freeing 'raw' memory. The reserved memory always is suitably
    (platform dependent) aligned such that any object type can be put
    there. Implementations shall meet the requirements of the C++
    Standard, par.3.7.3 'Dynamic storage duration
    [basic.stc.dynamic]'.  
 */
class RTEMem_RawAllocator : public SAPDBMem_IRawAllocator
{
public: 
  /*!
    @brief Return reference to single instance of RTEMem_Allocator

    The InitializeInstance function must have been called before.
    
    @return Reference to RTEMem_Allocator instance
   */
    static SAPDBMem_IRawAllocator & Instance();

  /*! 
    @brief Type requiring maximal alignment on the given platform. 
  */
  typedef union {
   void* p; ///< pointer
   long l; ///< long volue
   double d; ///< floating point value
  } AlignType;

  /*! 
    @brief Allocates contiguous memory for 'count' bytes.

      The memory is suitably aligned for all types.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    @param count [in] Number of Bytes to allocate

    @return Position where memory is allocated
   */
    virtual void* Allocate(SAPDB_ULong count);

  /*! 
    @brief Allocates contiguous memory for 'count' bytes,
      while trying to keep locality with a given memory location.

      If possible, the allocator tries to reserve the memory near
      a specified location.

      The memory is suitably aligned for all types.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    @param count [in] Number of Bytes to allocate
    @param hint [in] Hint where to allocate memory

    @return Position where memory is allocated
  */
    virtual void* Allocate(SAPDB_ULong count, const void * hint);

  /*!
    @brief Frees the memory. 

     The pointer must have been obtained by a previous call to
     'allocate'.

    @param p [in] Pointer to memory to deallocate
  */
    virtual void Deallocate(void * p);

  /*!
    @brief Return the identifier of this allocator

    @return "RTE_RawAllocator"
  */
      virtual const SAPDB_UTF8 * GetIdentifier() const 
          { return (const SAPDB_UTF8 *)"RTE_RawAllocator"; }

    /*!----------------------------------------------------------------------
      @brief Return number of bytes needed as overhead to store size info

      This value can be used to optimize access of this allocator, minimizing the
      overhead needed for each allocation.

      @return Minimum number of bytes overhead
      -----------------------------------------------------------------------*/
    static SAPDB_ULong GetOverhead();

    /*!----------------------------------------------------------------------
      @brief  Outputs statistics about the memory usage of the allocator.

      @param BytesUsed [out] The number of bytes used by this allocater.
      @param MaxBytesUsed [out] The maximum number of bytes used by this allocater.
      @param BytesControlled [out] The number of bytes controlled by this allocator.

      @return none
      -----------------------------------------------------------------------*/
    virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &MaxBytesUsed,
                                 SAPDB_ULong &BytesControlled);

	/*!----------------------------------------------------------------------
     @brief  Outputs statistics about the number of Allocate and Deallocate calls of
                   the allocator to the underlying base allocator.
     @param CountAlloc [out] The number of calls of method Allocate of the base allocator.
     @param CountDealloc [out] The number of calls of method Deallocate of the base.

     @return none
     -----------------------------------------------------------------------*/
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

    /*!----------------------------------------------------------------------
      @brief  Outputs statistics about the number of Allocate and Deallocate calls.
      @param  CountAlloc [out] The number of calls of method Allocate.
      @param  CountDealloc [out] The number of calls of method Deallocate.

      @return none
      -----------------------------------------------------------------------*/
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const;

#ifdef AIX
    /*!----------------------------------------------------------------------
    @brief Resets the m_Instance 
    -----------------------------------------------------------------------*/
    static void Reset()
    {
        m_Instance = 0;
    }
#endif

private:
      RTEMem_RawAllocator();

      SAPDBMem_IBlockAllocator & m_MemoryPageAllocator;

      RTESync_InterlockedCounter<SAPDB_ULong> m_BytesUsed;
      RTESync_InterlockedCounter<SAPDB_ULong> m_MaxBytesUsed;
      RTESync_InterlockedCounter<SAPDB_ULong> m_BytesControlled;
      RTESync_InterlockedCounter<SAPDB_ULong> m_CountAlloc;
      RTESync_InterlockedCounter<SAPDB_ULong> m_CountDealloc;

      static RTEMem_RawAllocator *m_Instance;
};
/*!   endclass: RTEMem_RawAllocator */


#endif /* RTEMEM_RAWALLOCATOR_HPP */
