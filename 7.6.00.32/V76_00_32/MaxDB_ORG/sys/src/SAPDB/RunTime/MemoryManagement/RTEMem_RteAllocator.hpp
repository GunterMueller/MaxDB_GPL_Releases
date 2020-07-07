/*!
  @file           RTEMem_RteAllocator.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Common Allocator for small objects in RTE context (replaces malloc/free)

  Allocator based on SAPDBMem_RawAllocator implementation

  This allocator wrapps internally a SAPDBMem_RawAllocator that is using
  the RTEMem_RawAllocator instance. It is also a singleton.

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
#ifndef RTEMEM_RTEALLOCATOR_HPP
#define RTEMEM_RTEALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  @class RTEMem_RteAllocator

  @brief Basic synchronised allocator replacing global 'malloc'/'free' in RTE context
 */
class RTEMem_RteAllocator: public SAPDBMem_IRawAllocator
{
public:
  /*!
    @brief Return reference to single instance of RTEMem_Allocator

    The InitializeInstance function must have been called before.
    
    @return Reference to RTEMem_Allocator instance
   */
  static SAPDBMem_IRawAllocator & Instance( );

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

    @return "RTEMem_RteAllocator"
  */
  virtual const SAPDB_UTF8 * GetIdentifier() const 
      { return (const SAPDB_UTF8 *)"RTEMem_RteAllocator"; }

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
      @brief  Outputs statistics about the number of Allocate and Deallocate calls.
      @param  CountAlloc [out] The number of calls of method Allocate.
      @param  CountDealloc [out] The number of calls of method Deallocate.

      @return none
      -----------------------------------------------------------------------*/
   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!----------------------------------------------------------------------
      @brief  Returns the number of errors found and corrected by the allocator

       The derived class is responsible to implement this feature (but must not do so...)

      @return Number of errors
   -----------------------------------------------------------------------*/
   virtual SAPDB_ULong GetErrorCount() const;

   /*!
      @brief          Returns the number of failed allocation attemps
                      The derived class is responsible to implement this feature (but must not do so...)
      @return         integer
    */
   virtual SAPDB_ULong GetAllocFailedCount() const;

   /*!
   @brief          Checks the integrity of the allocator.
   @return         bool (only in the non-exception case)
   */

  virtual bool CheckConsistency()
  {
      return m_Allocator->CheckConsistency();
  }

  /*!
     @brief enables all check levels of the allocator according to parameter HEAP_CHECK_LEVEL.
  */

  void SetCheckLevel()
  {
      m_Allocator->SetCheckLevel(); 
  }

   /*!
      @brief          Checks that allows message output routines to ignore this allocator
      
      The message output routine needs memory. If a problem occurs inside a locked section of the
      allocator it would lead to deadlock if the message output would try to use it to show the
      message. Therefore the message output routine uses this function to decide to switch to its 
      own 'Emergency allocator'...

      @return         bool (if true only message output code will change its behaviour...)

      The default implementation does always indicates usabilaty for message output

    */

  virtual bool DoNotUseForMessageOutputAllocation() const { return m_Allocator->DoNotUseForMessageOutputAllocation(); }

#ifdef AIX
    /*!----------------------------------------------------------------------
    @brief Resets the m_Instance 
    -----------------------------------------------------------------------*/
    static void Reset()
    {
        m_Instance = 0;
        m_Allocator = 0;
    }
#endif

private:
  // private allocator only used once during initialization
  RTEMem_RteAllocator( SAPDB_ULong firstAlloc, 
                    SAPDB_ULong supplementAlloc,
                    SAPDB_ULong maxAlloc );

  // private allocator only used once during initialization
  RTEMem_RteAllocator( SAPDB_ULong firstAlloc, 
                    SAPDB_ULong supplementAlloc);

  static void Initialize( SAPDB_ULong firstAlloc,
                          SAPDB_ULong supplementAlloc,
                          SAPDB_ULong maxAlloc );

  static SAPDBMem_SynchronizedRawAllocator *m_Allocator;
  static RTEMem_RteAllocator               *m_Instance;
  // Using a static reference would mean, that initialization function is called before main
  // This we have to prevent for the same reason we prevent static class instances
  // -> dynamic linked RunTime would not get initialized correctly on some OS...
  
#ifdef SAPDB_DEBUG_ALLOCATOR
  RTESync_NamedSpinlock m_Spinlock;
  SAPDB_ULong           m_BytesUsed;
  SAPDB_ULong           m_MaxBytesUsed;
  SAPDB_ULong           m_BytesControlled;
  SAPDB_ULong           m_CountAlloc;
  SAPDB_ULong           m_CountDealloc;
#endif
}; /*! endclass: RTEMem_RteAllocator */

#define RTEMEM_NEW(_class) new ( RTEMem_RteAllocator::Allocate(sizeof(_class)) ) _class

#endif  /* RTEMEM_ALLOCATOR_HPP */
