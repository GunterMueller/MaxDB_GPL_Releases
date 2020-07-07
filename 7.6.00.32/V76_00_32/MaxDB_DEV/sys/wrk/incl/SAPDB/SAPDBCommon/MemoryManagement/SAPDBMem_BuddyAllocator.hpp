/*!
  @file           SAPDBMem_BuddyAllocator.hpp
  @author         ThomasA
  @ingroup        Memory management
  @brief          Implementation of a 'buddy allocator'.

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


#ifndef SAPDBMEM_BUDDYALLOCATOR_HPP
#define SAPDBMEM_BUDDYALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_BuddyHeader.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_BuddyBlock.hpp"
#include "SAPDBCommon/SAPDB_IntegerArithmetic.hpp"

/*!
  @class          SAPDBMem_BuddyAllocator
  @brief          Implementation of the 'SAPDBMem_IRawAllocator'
  interface building on an 'SAPDBMem_IBlockAllocator' 
  backend allocator.


     This class provides an implementation of a 'buddy allocator' as
     described in the book 'Data Structures and Algorithms with
     Object-Oriented Design Patterns in C++' by Bruno R. Preiss, 1999.
     A 'buddy allocator' aims at fast allocation of memory.  The
     drawback is possibly inefficient use of memory. The sizes of
     memory chunks always are powers of 2. Thus, in the worst case an
     allocated memory chunk might be approximately twice as large as
     required.
     History: 
   - Fri Feb 11 12:09:53 2000      changed 'back end' allocator type from     'IRawAllocator' to 'IBlockAllocator'. the later interface is     easier to implement.
   - Wed Jan 19 13:59:16  2000      fixed allocation of 0 bytes. was incorrect.     some improvements on performance and readability.
   - 2000-01-05     fixed computation of buddy address. was incorrect. 
   - Wed Nov 10 14:35:56  1999
        The following improvements over B.Preiss' suggestion have been made:
       provided backend allocation mechanism via an interface class. 
    was using 'new'/'delete'.
         inserted alignment member into 'BuddyPool::Block::userPart'
         union. was alignment problem.
         converted 'BuddyPool::Header::status' member from type
         'Status' to type 'unsigned int'. was signed/unsigned problem.  

 */


class SAPDBMem_BuddyAllocator : public SAPDBMem_IRawAllocator
{
private: 
   /*! type definition as abbreviation */
   typedef SAPDBMem_BuddyBlock<SAPDBMem_BuddyHeader> BuddyBlock;

   /*!
   The 'back end' allocator. The memory region managed by this buddy allocator 
   is taken from the allocator being referenced.
   */
   SAPDBMem_IBlockAllocator& m_BlockAllocator;

   /*!
   The number of blocks allocated from the back end allocator.
   */
   SAPDB_ULong m_BlockCount;
      
   /*!
   The base 2 logarithm of the maximal size (in bytes) of a memory chunk.
   */
   unsigned int m_Log2MaxSize;

   /*!
   Pointer to the array of memory blocks in the memory region.
   */
   BuddyBlock* m_Blocks;

   /*!
   Pointer to the array of sentinel blocks of free-lists in the memory region.
   */
   BuddyBlock* m_Sentinels;

   /*!
   Statistics information about memory usage.
   */
   SAPDB_ULong m_BytesUsed;
   SAPDB_ULong m_MaxBytesUsed;

   /* enddeclaration: */
      
   /*!
   @brief   Returns the log2 of the required size in bytes. 

   @param   ByteCount [in] The required size is the size this allocator needs in
       order to provide the requested amount of memory to the client.
   @return  log2 of required size in bytes.
   */
   unsigned int GetLog2RequiredSize(SAPDB_ULong ByteCount) const;

public: 
   /*!
      @brief          Constructs a buddy allocator from the provided
                      raw allocator.
      @param          ByteCount [in] The usable size of the allocator.
      @param          BlockAllocator [in] The backend block allocator.


                   The buddy allocator allocates a single memory region from
                   the block allocator and provides the user with memory chunks
                   thereof. The usable size of this region is at least as large
                   as specified. The size allocated from the back end allocator
                   is actually larger.

    */

   SAPDBMem_BuddyAllocator( 
      SAPDB_ULong ByteCount, 
      SAPDBMem_IBlockAllocator& BlockAllocator
   );
      
   /*!
      @brief        Virtual destructor.


                   Frees all the memory allocated from the backend raw allocator.

    */

   virtual ~SAPDBMem_BuddyAllocator();

   /*!
      @sa SAPDBMem_IRawAllocator::Allocate

   */

   virtual void* Allocate(SAPDB_ULong ByteCount);

   /*!
      @sa SAPDBMem_IRawAllocator::Allocate
   */
   
   virtual void * Allocate(SAPDB_ULong ByteCount, const void * hint)
      { return Allocate(ByteCount); }

   /*!
      @brief          See 'SAPDBMem_IRawAllocator

    */

   virtual void Deallocate(void* p);

   /*!
      @brief          See 'SAPDBMem_IRawAllocator

    */

   virtual bool CheckConsistency();

   /*!
      @brief          See 'SAPDBMem_IRawAllocator

    */

   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled);

private:
   /*!
	@brief copy constructor
           Private copy constructor avoids copying.
   */
   SAPDBMem_BuddyAllocator(const SAPDBMem_BuddyAllocator& Buddy)
      : m_BlockAllocator(Buddy.m_BlockAllocator)
      {}

   /*!
	@brief  Private assignment operator avoids copying.
   */
   SAPDBMem_BuddyAllocator &operator=(SAPDBMem_BuddyAllocator &)
      { return *this; }

};



#endif
