/*!***************************************************************************

  @file           RTEMem_UnregisteredAllocator.hpp
  @author         Robin
  @ingroup        Memory Management
  @brief          Implementation of an allocator for special cases (only!) 
                  where registering must be avoided - within RTE_ItemRegister
                  for example. To achieve this, memory allocations are fullfilled
                  from a static byte array. If this array is exhausted, memory
                  allocations are proceeded to RTEMem_RteAllocator - hoping that
                  it can be used.


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




*****************************************************************************/


#ifndef RTE_MEMUNREGISTEREDALLOCATOR_HPP
#define RTE_MEMUNREGISTEREDALLOCATOR_HPP

/*!
    @brief  the size of the statically allocated array used for the first allocations when no usable allocator is available
*/
#define SIZE_OF_STATIC_ARRAY 4096

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  Class: RTEMem_UnregisteredAllocator

  Description: 
 */
class RTEMem_UnregisteredAllocator : public SAPDBMem_IRawAllocator
{
public:
  /*!
    Function: Instance

    description: Return reference to single instance of RTEMem_UnregisteredAllocator

    The InitializeInstance function must have been called before.
    
    Return value: Reference to RTEMem_UnregisteredAllocator instance
   */
  static SAPDBMem_IRawAllocator & Instance( );

  /*! 
    Declaration: AlignType

    Description: Type requiring maximal alignment on the 
      given platform. 
  */
private:
  typedef union { void* p; long l; double d; } AlignType;
public:
  /*! 
    Function: Allocate

    Description: Allocates contiguous memory for 'count' bytes.

      The memory is suitably aligned for all types.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    Arguments: count [in] Number of Bytes to allocate

    Return value: Position where memory is allocated
   */
  virtual void* Allocate(SAPDB_ULong count);

  /*! 
    Function: Allocate

    Description: Allocates contiguous memory for 'count' bytes.
      hint is ignored! Has been added only to fullfill the
      SAPDBMem_IRawAllocator interface

      If possible, the allocator tries to reserve the memory near
      a specified location.

      The memory is suitably aligned for all types.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    Arguments: count [in] Number of Bytes to allocate
               hint [in] Hint where to allocate memory - ignored

    Return value: Position where memory is allocated
  */
  virtual void* Allocate(SAPDB_ULong count, const void * hint){return Allocate(count);};

  /*!
    Function: Deallocate
    
    Description: Frees the memory. 

     The pointer must have been obtained by a previous call to
     'allocate'.

    Arguments: p [in] Pointer to memory to deallocate
  */
  virtual void Deallocate(void * p);

  /*!
    Function: GetIdentifier
    
    Description: Return the identifier of this allocator

    Return value: "RTE_Allocator"
  */
  virtual const SAPDB_UTF8 * GetIdentifier() const 
      { return (const SAPDB_UTF8 *)"RTEMem_UnregisteredAllocator"; }

    /*!----------------------------------------------------------------------
      function: GetBaseAllocatorCallStatistics
      description:  Outputs statistics about the number of Allocate and Deallocate calls of
                    the allocator to the underlying base allocator.
      arguments:    CountAlloc [out]
                    The number of calls of method Allocate of the base allocator.
                    CountDealloc [out]
                    The number of calls of method Deallocate of the base.

      return value: none
      -----------------------------------------------------------------------*/

    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
        SAPDB_ULong &CountDealloc) const{};

    /*!----------------------------------------------------------------------
      Function: CalcStatistics
      Description:  Outputs statistics about the memory usage of the allocator.
      Arguments:    BytesUsed [out]
                    The number of bytes used by this allocater.
                    MaxBytesUsed [out]
                    The maximum of bytes used by this allocator.
                    BytesControlled [out]
                    The number of bytes controlled by this allocator.

                    The default implementation returns GetInvalidSize() for 
                    both values.
      Return value: none
      -----------------------------------------------------------------------*/
    virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &MaxBytesUsed,
                                 SAPDB_ULong &BytesControlled){};

    /*!----------------------------------------------------------------------
      Function: GetCallStatistics
      Description:  Outputs statistics about the number of Allocate and Deallocate calls.
      Arguments:    CountAlloc [out]
                    The number of calls of method Allocate.
                    CountDealloc [out]
                    The number of calls of method Deallocate.

      Return value: none
      -----------------------------------------------------------------------*/

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
       SAPDB_ULong &CountDealloc) const{};

private:
  // private allocator only used once during initialization
  RTEMem_UnregisteredAllocator():m_bytesAllocated(0){};

  static RTEMem_UnregisteredAllocator    *m_Instance;
  // Using a static reference would mean, that initialization function is called before main
  // This we have to prevent for the same reason we prevent static class instances
  // -> dynamic linked RunTime would not get initialized correctly on some OS...
  
    SAPDB_ULong m_bytesAllocated;

    SAPDB_UInt8 m_arrayForFirstAllocations[SIZE_OF_STATIC_ARRAY/8];

    RTESync_Spinlock m_Spinlock;
}; /*! endclass: RTEMem_UnregisteredAllocator*/

#endif  /* RTE_MEMUNREGISTEREDALLOCATOR_HPP */
