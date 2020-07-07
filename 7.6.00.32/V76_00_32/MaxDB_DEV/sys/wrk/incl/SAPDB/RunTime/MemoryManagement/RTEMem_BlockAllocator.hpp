/*!***************************************************************************

  module      : RTEMem_BlockAllocator.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Block Allocator
  Description : Allocator used to allocate blocks usable for block I/O etc.

  last changed: 2000-12-14  13:40
  first created:2000-02-28  15:35

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


#ifndef RTEMEM_BLOCKALLOCATOR_HPP
#define RTEMEM_BLOCKALLOCATOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  Class:       RTEMem_BlockAllocator
  
  Description: Defines base allocators used for all memory page block allocation

    Only a singleton instances of the class is creatable. It is used
    to allocate MemoryPages. The size of a memory page is System dependend!
    
    The allocator is allocating blocks of memory in SystemMemoryPage
    size. These blocks are used for fast I/O operations. DataCache and Converter
    fill their memory needs using it.

    To get an RTEMem_BlockAllocator::Instance() no argument is needed.
 */
class RTEMem_BlockAllocator : public SAPDBMem_IBlockAllocator
{
public:
  /*! 
    Function:    Instance
  
    Description: Returns the singleton RTE Block Allocator working with system memory pages

    This returns system defined memory pages. Size may vary from system to system, but overhead
    is guarantuid to be minimized!

    Return value: The block allocator instance for working with memory pages
   */
  static SAPDBMem_IBlockAllocator & Instance();

  /*! 
    Function: GetBlockSize
	
    Description: Returns the size in bytes of the blocks that can be allocated by this allocator.

     Returns blocksize setup by the constructor. Returns size of a system memory page in bytes.
                 
    Return value: The system memory page in bytes used for block count parameter
   */
  virtual SAPDB_ULong GetBlockSize() const { return m_BlockSize; }

  /*! 
    Function: CalcBlockCount
	
    Description: Returns the number of blocks needed to allocate given number of bytes
    
     Since Allocate() needs the number of blocks instead of number of bytes, this helper 
     routine exists to calculate the right argument. When used calling the Routine 
     GetBlockSize() is not needed.

    Arguments: NumberOfBytes [in] Number of bytes instead of number of blocks

    Return value: The number of system memory page needed for given number of bytes  
  */
  virtual SAPDB_ULong CalcBlockCount(SAPDB_ULong NumberOfBytes) const
  { return ( NumberOfBytes + m_BlockSize - 1 ) / m_BlockSize; }

  /*!
    Function: CalcStatistics
    Description:  Outputs statistics about the memory usage of the allocator.
    Arguments:    BytesUsed [out]
                  The number of bytes used by this allocater.
                  MaxBytesUsed [out]
                  The maximum number of bytes used by this allocator so far
                  BytesControlled [out]
                  The number of bytes controlled by this allocator.

                  The default implementation returns GetInvalidSize() for 
                  both values.
    Return value: none
  */
  virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled);

  /*! 
    Function: GetAlignmentSize
    
    Description: Returns the alignment size in bytes of the blocks
      that can be allocated by this allocator.

      An implementation must ensure that the alignment size A and
      the block size B satisfy B = N * A, where N is a non-zero
      integer number.

    Return value: The system page size as defined by the underlying operating system
   */
  virtual SAPDB_ULong GetAlignmentSize() const { return m_AlignmentSize; }

  /*! 
    Function: Allocate

    Description: Allocates contiguous memory for 'count' blocks of memory.

      An implementation grants that a pointer returned will be an
      integral multiple of the system page size.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    Arguments: BlockCount [ in ] Number of blocks (NOT bytes!) to allocate.
    
    Return value: pointer to the block allocated
   */
  virtual void * Allocate(SAPDB_ULong BlockCount);

  /*! 
    Function: Allocate

    Description: Allocates contiguous memory for 'count' blocks of memory.

      An implementation grants that a pointer returned will be an
      integral multiple of the system page size.

      The memory is not initialised.  

      If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 is returned on failure.

      If compiled without this switch an SAPDBMem_BadAllocException exception is thrown on failure.

    Arguments: BlockCount [ in ] Number of blocks (NOT bytes!) to allocate.
               messageList [out] filled if memory not available
    Return value: pointer to the block allocated
   */
  virtual void * Allocate(SAPDB_ULong BlockCount, Msg_List &messageList);

  /*! 
    Function: Deallocate
    
    Description: Frees the memory. 

     None of the objects in the memory block will be deinitialised
     by this call.  The size argument must match the value passed
     to 'allocate' to obtain this memory. The pointer argument
     must not be null.

     On failure, an exception is thrown.

    Arguments: BlockStartAddress [ in ] the pointer returned by a previous allocate call.
               BlockCount [ in ] Number of blocks (NOT bytes!) to deallocate.
   */
  virtual void Deallocate(
     void * BlockStartAddress, 
     SAPDB_ULong BlockCount );

  /*!
   function: GetBaseAllocatorCallStatistics
   description:  Outputs statistics about the number of Allocate and Deallocate calls of
                 the allocator to the underlying base allocator.
   arguments:    CountAlloc [out]
                 The number of calls of method Allocate of the base allocator.
                 CountDealloc [out]
                 The number of calls of method Deallocate of the base.

   return value: none
   */

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

  /*!
   Function: GetCallStatistics
   Description:  Outputs statistics about the number of Allocate and Deallocate calls.
   Arguments:    CountAlloc [out]
                 The number of calls of method Allocate.
                 CountDealloc [out]
                 The number of calls of method Deallocate.

   return value: none
   */

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!
    Function: GetIdentifier
    Description: Return identifier
    Return value: const SAPDB_UTF8 * "RTE_RawAllocator"
   */

   virtual const SAPDB_UTF8 * GetIdentifier() const 
          { return (const SAPDB_UTF8 *)"RTEMem_BlockAllocator"; }

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

  RTEMem_BlockAllocator(SAPDB_ULong BlockSize=0);  // Private constructor

  SAPDB_ULong m_BlockSize;                       // The size of a block defined by the user
  SAPDB_ULong m_AlignmentSize;                   // The alignment size defined by the system
  RTESync_InterlockedCounter<SAPDB_ULong> m_BytesControlled;
  RTESync_InterlockedCounter<SAPDB_ULong> m_CountAlloc;
  RTESync_InterlockedCounter<SAPDB_ULong> m_CountDealloc;
  static RTEMem_BlockAllocator *m_Instance; // The Memory Page Block allocator instance
  RTEMem_AllocatorInfo          m_AllocatorInfo;
  static RTEMem_AllocatorInfo  *m_pAllocatorInfo;
};
/*! Endclass: RTEMem_BlockAllocator */

#endif  /* RTEMEM_BLOCKALLOCATOR_HPP */
