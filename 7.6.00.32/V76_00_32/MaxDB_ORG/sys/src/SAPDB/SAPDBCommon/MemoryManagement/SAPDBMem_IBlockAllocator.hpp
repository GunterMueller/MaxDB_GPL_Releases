/*!
  @file           SAPDBMem_IBlockAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Block allocator interface for memory management of blocked memory.

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


#ifndef SAPDBMem_IBlockAllocator_HPP
#define SAPDBMem_IBlockAllocator_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IAllocatorInfo.hpp"

class Msg_List;
/*!
   @interface      SAPDBMem_IBlockAllocator
   @brief          Allocator interface for aligned blocks of memory of
                   fixed size.

     Classes implementing this interface provide methods for reserving
     and freeing memory in integral multiples of a fixed block
     size. The reserved memory is always aligned to integral multiples
     of a fixed alignment size, which may differ from the block size.
     The interface serves the abstraction from low level allocation
     mechanisms like, e.g., the 'mmap' system call on UNIX platforms.
     Application programmers will most likely prefer STL allocators or
     raw allocators in the sense of the C++ Standard.
     While this interface is similar to the one required by the
     Standard Template Library, the block size and the alignment size
     is not determined by a C++ data type at compile time, but instead
     is determined at runtime.
     Implementations of this interface must not be confused neither
     with STL allocators nor with raw storage allocation functions as
     described in the C++ Standard, par.3.7.3 'Dynamic storage duration
     [basic.stc.dynamic]'.
     Every class implementing this interface must assure, that the
     copy constructor and the assignment operator is inaccessible.

 */


class SAPDBMem_IBlockAllocator : public SAPDBMem_IAllocatorInfo
{
public:
   /*!
      @brief          Returns the number of blocks needed to allocate 
                      NumberOfBytes bytes.

                      Since Allocate() needs the number of blocks instead of number 
                      of bytes, this helper routine exists to calculate the right 
                      argument. When used calling the Routine GetBlockSize() is not needed.
      @param          NumberOfBytes [in] Number of bytes instead of number of blocks.
      @return         SAPDB_ULong
                      Number of blocks needed for a given number of bytes.

    */

   virtual SAPDB_ULong CalcBlockCount(SAPDB_ULong NumberOfBytes) const
      { return ( NumberOfBytes + GetBlockSize() - 1 ) / GetBlockSize(); }

   /*!
      @brief          Returns the size in bytes of the blocks that can
                      be allocated by this allocator.

    */

   virtual SAPDB_ULong GetBlockSize() const = 0;

   /*!
      @brief          Returns the alignment size (in bytes) of the blocks
                      that can be allocated by this allocator.

    
                      An implementation must ensure that the alignment size A and
                      the block size B satisfy B = N * A, where N is a non-zero
                      integer number.
    */

   virtual SAPDB_ULong GetAlignmentSize() const = 0;

   /*!
      @brief          Allocates contiguous memory for 'count' blocks of
                      memory.
                      An implementation grants that a pointer returned will be an
                      integral multiple of the alignment size. 
                      The memory is not initialised.
      @param          BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;BlockCount> 
                      blocks. If exception handling is disabled, a NULL pointer will
                      be returned in case of insufficient memory.
      @exception      From 'SAPDBMem_IBlockAllocator' derived classes may 
                      throw a 'SAPDBMem_BadAllocException' if the allocation fails.

    */

   virtual void* Allocate(SAPDB_ULong BlockCount) = 0;

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
      @brief          Frees the memory.
                      None of the objects in the memory block will be deinitialised
                      by this call.  The size argument must match the value passed
                      to 'allocate' to obtain this memory. The pointer argument
                      must not be null.
      @param          p [in] pointer to memory to be freed
      @param          BlockCount [in] number of blocks to be freed                
    */

   virtual void Deallocate(void* p, SAPDB_ULong BlockCount) = 0;

};



#endif // SAPDBMem_IBlockAllocator_HPP
