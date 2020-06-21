/*!
  @file           SAPDBMem_BuddyBlock.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Part of implementation of a 'buddy allocator'.

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

#ifndef SAPDBMEM_BUDDYBLOCK_HPP
#define SAPDBMEM_BUDDYBLOCK_HPP


#include <stddef.h>

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*!
   @class          SAPDBMem_BuddyBlock
   @brief          BuddyHeader class used by the 'SAPDBMem_BuddyAllocator'.
                   template arguments
                   BuddyHeader 
                   The header class for the memory chunks. The class
                   'SAPDBMem_BuddyBlock' is derived from BuddyHeader. Thus
                   every 'SAPDBMem_BuddyBlock' is a BuddyHeader and has a
                   the members of BuddyHeader as first members.

 */


template <class BuddyHeader>
class SAPDBMem_BuddyBlock : public BuddyHeader
{
public:
   /*! Type definition for both pointer in a doubly linked list
       Type must be public due to a compiler error on sun32
   */
   struct Links
   {
	   SAPDBMem_BuddyBlock<BuddyHeader> * Previous; //!< pointer to previous list element
	   SAPDBMem_BuddyBlock<BuddyHeader> * Next;     //!< pointer to next list element
   };

private:
   
   typedef SAPDBMem_BuddyBlock<BuddyHeader> BuddyBlock; //!< Type definition as abbreviation

   /*! Union of links and data part. A block either is free, then it has valid links, or else
       it is reserved, then the body contains data.
    */

   union
   {
      
      Links m_Links;                             //! Links to previous and next block (doubly linked list)
      unsigned char m_Body[sizeof(BuddyHeader)]; //! A body has at least sizeof(BuddyHeader) bytes
      SAPDBMem_IRawAllocator::AlignType m_dummy; //! Ensure alignment of the 'm_Body' member. 
   };
   

public: 
   /*!
      @brief          Default constructor

    */

   SAPDBMem_BuddyBlock();

   /*!
      @brief          Returns the offset of the block's usable body.

    */

   static SAPDB_ULong GetOffsetOfBody()
      { return offsetof(BuddyBlock, m_Body); }

   /*!
      @brief          Converts a pointer to a block's body into 
             a pointer to the block.

    */

   static BuddyBlock* GetBlock(void* Body)
   {
      return reinterpret_cast<BuddyBlock*>(
        reinterpret_cast<unsigned char *>(Body) - BuddyBlock::GetOffsetOfBody()
      );
   }

   /*!
      @brief          Returns a pointer to the block's usable body.

    */

   void* GetBody()
      { return m_Body; }

   /*!
      @brief          Initializes the linked list part of the block to point to itself.

    */

   void Initialize();

   /*!
      @brief          Inserts the block into a list.

    */

   void Insert(BuddyBlock& Precessor);
      
   /*!
      @brief          Removes the block from the list.

    */

   void Remove();

   /*!
      @brief          Returns the next block from the list.

    */

   BuddyBlock* GetNext() const
      { return m_Links.Next; }

   /*!
      @brief          Returns the previous block from the list.

    */

   BuddyBlock* GetPrevious() const
      { return m_Links.Previous; }

   /*!
      @brief          Returns a pointer to the buddy of this memory block.


              The address of the buddy depends on the start address of
              the memory region, the offset of this block with respect
              to the start address and the size of this block.

    */

   BuddyBlock* GetBuddy(BuddyBlock& StartBlock) const;
      
};



/*!
@brief let the block point to itself
*/ 
template <class BuddyHeader>
inline void SAPDBMem_BuddyBlock<BuddyHeader>::Initialize()
{
   m_Links.Previous = this;
   m_Links.Next     = this;
}

/*!
@brief remove the block from doubly linked list
*/

template <class BuddyHeader>
inline void SAPDBMem_BuddyBlock<BuddyHeader>::Remove()
{
   (m_Links.Previous)->m_Links.Next = m_Links.Next;
   (m_Links.Next)->m_Links.Previous = m_Links.Previous;
}

/*!
@brief insert a block into the doubly linked list
*/

template <class BuddyHeader>
void SAPDBMem_BuddyBlock<BuddyHeader>::Insert(BuddyBlock& Precessor)
{
   // insert a block into the doubly linked list
   BuddyBlock* const Successor = Precessor.m_Links.Next; 
   Precessor.m_Links.Next = this;
   m_Links.Previous = &(Precessor);
   m_Links.Next = Successor;
   Successor->m_Links.Previous = this;
}

/*!
@brief a chunk of size 2**k lies at an offset that is a multiple of
       2**k.  so the lower k bits of its address are zero. so the
       buddies (of size 2**(k-1)) inside of the 2**k - sized chunk are
       distinguished by bit number k-1 (counting from 0).
*/

template <class BuddyHeader>
inline SAPDBMem_BuddyBlock<BuddyHeader>*
SAPDBMem_BuddyBlock<BuddyHeader>::GetBuddy(BuddyBlock& StartBlock) const
{
   // a chunk of size 2**k lies at an offset that is a multiple of
   // 2**k.  so the lower k bits of its address are zero. so the
   // buddies (of size 2**(k-1)) inside of the 2**k - sized chunk are
   // distinguished by bit number k-1 (counting from 0).
   
   // offset of this block in bytes
   ptrdiff_t Offset = reinterpret_cast<const unsigned char *>(this) - 
                      reinterpret_cast<const unsigned char *>(&StartBlock);
   
   // Offset must be an integral multiple of GetSize()
   SAPDBERR_ASSERT_STATE((Offset & (BuddyHeader::GetSize() - 1)) == 0);

   // Toggle the 'size' bit to get the offset for the buddy block
   Offset = (Offset ^ BuddyHeader::GetSize());
   
   // Construct the address of the buddy block and return it
   return reinterpret_cast<BuddyBlock*>(reinterpret_cast<unsigned char *>(&StartBlock) + Offset);
}

#endif

