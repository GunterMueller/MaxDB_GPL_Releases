/*!
  @file           SAPDBMem_PageAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Raw allocator for allocating memory pages of constant size.

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


#ifndef SAPDBMem_PageAllocator_HPP
#define SAPDBMem_PageAllocator_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*!
   @class          SAPDBMem_PageAllocator
   @brief          This class manages given memory blocks. This blocks are divided
                   into block of common size, called pages. This memory is provided
                   by the SAPDBMem_IRawAllocator interface.
*/


class SAPDBMem_PageAllocator : public SAPDBMem_IRawAllocator
{
private:
    /*!
    @ brief defines a page header
    */
    struct Page {
        Page *NextPage;      //!< pointer to previous page
        Page *PreviousPage;  //!< pointer to next page
    };
    SAPDB_ULong m_PageSize;                 //!< The size of one page.
    SAPDB_ULong m_FreePages;                //!< The number of free pages.
    Page *m_FirstFreePagePtr;               //!< The address of the first free page in the chain.
    SAPDB_ULong m_CountAlloc;               //!< number of Allocate calls.
    SAPDB_ULong m_CountDealloc;             //!< number of Deallocate calls.       
public:
    /*!
       @brief          Constructs an page allocator
       @param          PageSize [in] page size in bytesThe size of one page.
                       The page allocator is empty at the beginning. 
    */

    SAPDBMem_PageAllocator(SAPDB_ULong PageSize);

    /*!
       @brief destructor          
    */

    virtual ~SAPDBMem_PageAllocator() {}

    /*!
       @see SAPDBMem_IRawAllocator::Allocate
     */

    virtual void* Allocate(SAPDB_ULong ByteCount);
    
    /*!
       @see SAPDBMem_IRawAllocator::Allocate
     */

    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
        { return Allocate(ByteCount); }

    /*!
       @see SAPDBMem_IRawAllocator::Deallocate
     */

    virtual void Deallocate(void* p);

    /*!
       @brief          This method lets an memory block be controlled by the page
              allocator. The page allocator splits the memory block into
              smaller pieces of the page size.
       @param          Ptr [in] pointer to the given memory block
       @param          Len [in] number of bytes belonging to the given memory block
       @return         none

     */

    void AddMemoryBlock(void* Ptr, SAPDB_ULong Len);

   /*!
      @brief          Outputs statistics about the number of Allocate and Deallocate calls of
              the allocator to the underlying base allocator.
      @param          CountAlloc [out] The number of calls of method Allocate of the base allocator.
      @param          CountDealloc [out] The number of calls of method Deallocate of the base.
      @return         none

    */


   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!
      @brief          Outputs statistics about the number of Allocate and Deallocate calls.
      @param          CountAlloc [out] The number of calls of method Allocate.
      @param          CountDealloc [out] The number of calls of method Deallocate.
      @return         none

    */


   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;


    /*!
       @brief          Returns the number of free pages, that is the number of
                       subsequent sucessful calls to Allocate().
       @return         SAPDB_ULong

     */

    SAPDB_ULong GetFreePages() const
        { return m_FreePages; }

private:
    /*!
    @brief Private copy constructor avoids copying.
    */
    SAPDBMem_PageAllocator(const SAPDBMem_PageAllocator &InitAlloc)
        : m_PageSize(InitAlloc.m_PageSize), m_FirstFreePagePtr(0)
    {}

    /*!
    @brief Private assignment operator avoids copying.
    */
    SAPDBMem_PageAllocator operator=(SAPDBMem_PageAllocator &)
        { return *this; }
};

#endif //SAPDBMem_PageAllocator_HPP

