/*!
  @file           LVCMem_DefaultAllocator.hpp
  @author         ThomasA
  @ingroup        OMS Memory management
  @brief          default allocator of liboms
                  LVCMem_DefaultAllocator becomes a singleton by call of
                  method Instance().


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


#ifndef LVCMEM_DEFAULTALLOCATOR_HPP
#define LVCMEM_DEFAULTALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*!
   @class          LVCMem_DefaultAllocator
   @brief          Default allocator of liboms.
                   This allocator is active, until a call of OMS_LibOmsInterfaceInstance::InitLibOms
                   has been made. This call is made only, if the liboms runs in the kernel environment
 */

class LVCMem_DefaultAllocator : public SAPDBMem_IRawAllocator
{
public:

   /*!
	 @brief allocates memory
     @see   SAPDBMem_IRawAllocator::Allocate
   */
   virtual void* Allocate(SAPDB_ULong Count);

   /*!
	 @brief allocates memory
     @see   SAPDBMem_IRawAllocator::Allocate
   */
   virtual void* Allocate(SAPDB_ULong Count, const void* Hint)
      { return Allocate(Count); }

   /*!
      @brief frees memory.
      @see   SAPDBMem_IRawAllocator::Deallocate
    */

   virtual void Deallocate(void *p);

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
      @brief Default constructor
	*/
    LVCMem_DefaultAllocator();

private:
   /*!
   @brief Private assignment operator avoids copying.
   */
   LVCMem_DefaultAllocator operator=(LVCMem_DefaultAllocator &)
      { return *this; }
};

#endif // LVCMem_DefaultAllocator_HPP
