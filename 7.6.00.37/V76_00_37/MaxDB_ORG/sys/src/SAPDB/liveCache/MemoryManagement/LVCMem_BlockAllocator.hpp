/*!
  @file           LVCMem_BlockAllocator.hpp
  @author         ThomasA
  @ingroup        OMS memory management
  @brief          Block Allocator for OMS
   
                  This Allocator is a singleton, that provides memory for the
                  OMS allocator. It's main purpose is superwising the OMS heap limit. 

\if EMIT_LICENCE

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



\endif
*/



#ifndef LVCMEM_BLOCKALLOCATOR_HPP
#define LVCMEM_BLOCKALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class    LVCMem_BlockAllocator
  @brief    Defines a block allocator as base of the OMS alloctor.
            
            Only a singleton instance of this class is creatable. It is used
            to allocate MemoryPages for the OMS allocator.
            The allocator is allocating blocks of memory from the RTEMem_BlockAllocator.
            The allocator supervises the OMS heap limit and caches free blocks of memory
            for following requests. To be able to cache free blocks, a default request
            size has to be defined. Only free blocks of this size are cached. All other
            blocks are returend to the underlying RTEMem_BlockAllocator immediatly.
 */


class LVCMem_BlockAllocator : public SAPDBMem_IBlockAllocator
{
public :

  /*!
     @brief    returns a reference to the single instance of LVCMem_BlockAllocator.
               The first call of Instance will construct the singleton.
     @return value   reference to the instance
   */

  static LVCMem_BlockAllocator & Instance();

  /*!
     @brief    Returns the number of blocks needed to allocate given number of bytes

               Since Allocate() needs the number of blocks instead of number of bytes, this helper 
               routine exists to calculate the right argument. When used calling the Routine 
               GetBlockSize() is not needed.
     @param          NumberOfBytes [in] Number of bytes instead of number of blocks
     @return value   The number of system memory page needed for given number of bytes
   */

  virtual SAPDB_ULong CalcBlockCount(SAPDB_ULong NumberOfBytes) const;

  /*!
     @brief    Outputs statistics about the memory usage of the allocator.
     @param    BytesUsed [out] The number of bytes used by this allocater.
     @param    BytesControlled [out] The number of bytes controlled by this allocator.
               The default implementation returns GetInvalidSize() for both values.
     @return value   none
   */
 
  virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled);

  /*!
     @brief  Returns the size in bytes of the blocks that can be allocated by this allocator.
             Returns blocksize setup by the constructor. Returns size of a system memory page in bytes.
     @return value   The system memory page in bytes used for block count parameter
   */


  virtual SAPDB_ULong GetBlockSize() const;

  /*!
     @brief    Returns the alignment size in bytes of the blocks
               that can be allocated by this allocator.

               An implementation must ensure that the alignment size A and
               the block size B satisfy B = N * A, where N is a non-zero
               integer number.
     @return value   The system page size as defined by the underlying operating system
  */


  virtual SAPDB_ULong GetAlignmentSize() const;

  /*!
     @brief    Outputs statistics about the number of Allocate and Deallocate calls of
               the allocator to the underlying base allocator.
     @param    CountAlloc [out] The number of calls of method Allocate of the base allocator.
     @param    CountDealloc [out] The number of calls of method Deallocate of the base.
     @return   none
   */

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

  /*!
     @brief    Outputs statistics about the number of Allocate and Deallocate calls.
     @param    CountAlloc [out] The number of calls of method Allocate.
     @param    CountDealloc [out] The number of calls of method Deallocate.
     @return   none
  */

  virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                 SAPDB_ULong &CountDealloc) const;

  /*!
     @brief    Returns the allocators name
     @return value   const SAPDB_UTF8 * "LVCMem_BlockAllocator"
   */

  virtual const SAPDB_UTF8 * GetIdentifier() const;

  /*!
     @brief    Allocates contiguous memory for 'BlockCount' blocks of memory.

               An implementation grants that a pointer returned will be an
               integral multiple of the system page size.
               The memory is not initialised.  
               (void *)0 is returned on failure.
     @param          BlockCount [ in ] Number of blocks (NOT bytes!) to allocate.
     @return value   pointer to the block allocated
   */

  virtual void* Allocate(SAPDB_ULong BlockCount);

   /*!
      @brief          Allocates contiguous memory for 'count' blocks of
                      memory.
                      An implementation grants that a pointer returned will be an
                      integral multiple of the alignment size. 
                      The memory is not initialised.
      @param          BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
      @param          messageList [out] filled if memory not available   
      @return         void*
                      A pointer to the allocated raw memory block of &lt;BlockCount> 
                      blocks. If exception handling is disabled, a NULL pointer will
                      be returned in case of insufficient memory.
      @exception      From 'SAPDBMem_IBlockAllocator' derived classes may 
                      throw a 'SAPDBMem_BadAllocException' if the allocation fails.

    */

   virtual void * Allocate(SAPDB_ULong BlockCount, Msg_List &messageList)
   {
       return Allocate(BlockCount);
   }

  /*!
     @brief    Frees the memory.

               None of the objects in the memory block will be deinitialised
               by this call.  The size argument must match the value passed
               to 'allocate' to obtain this memory. The pointer argument
               must not be null.
               On failure, an exception is thrown.
     @param    p [ in ] the pointer returned by a previous allocate call.
     @param    BlockCount [ in ] Number of blocks (NOT bytes!) to deallocate.

   */

 
  virtual void Deallocate(void* p, SAPDB_ULong BlockCount);

  /*!
     @brief        defines the maximal amount of memory provided by this allocator.
     @return value none

   */

  void SetLimit(SAPDB_ULong limit); 

  /*!
     @brief        defines the default block size, i.e. the amount of memory, that 
                   normaly is required by the client.
                   Only free blocks of this size can be cached by the allocator and
                   will be reused by following requests.
     @return value none
   */
  
  void SetDefaultBlockCount(SAPDB_ULong BlockCount);

private :
  void ReleaseFreeList(); //!< releases all chunks of the free list

  /*
  @brief header of a free chunk
  */
  struct NextPointer 
  {
    NextPointer* m_next; //!< pointer to next free chunk
  }; 
  LVCMem_BlockAllocator(); //!< default constructor
  SAPDBMem_IBlockAllocator&     m_allocator;           //!< underlying Block Allocator
  SAPDB_ULong                   m_defaultBlockCount;   //!< default block size
  SAPDB_ULong                   m_limit;               //!< upper heap limit
  SAPDB_ULong                   m_allocated;           //!< number of allocated blocks
  SAPDB_ULong                   m_maxAllocated;        //!< maximal number of allocated blocks
  SAPDB_ULong                   m_controlled;          //!< number of controlled blocks
  SAPDB_ULong                   m_CountAlloc;          //!< number of calls of Allocate
  SAPDB_ULong                   m_CountDealloc;        //!< number of calls of Deallocate
  SAPDB_ULong                   m_CountBaseAlloc;      //!< number of calls of Allocate to base allocator
  SAPDB_ULong                   m_CountBaseDealloc;    //!< number of calls of Deallocate to base allocator
  NextPointer*                  m_firstFree;           //!< pointer to first free block
  static LVCMem_BlockAllocator* m_Instance;            //!< pointer to the singelton
  RTESync_NamedSpinlock         m_spinlock;            //!< synchronization object
  bool                          m_writeHeapLimitOpMsg; //!< true, if OMS_HEAP_LIMIT opmsg has to be written, if heap limit is reached
};



#endif