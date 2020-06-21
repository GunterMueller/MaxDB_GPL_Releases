/*!
  @file           SAPDBMem_DefaultRawAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Memory allocator using standard 'operator new'
                  and 'operator delete' functions. 
                  Caution : this allocator must not be used in kernel context


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


#ifndef SAPDBMem_DefaultRawAllocator_HPP
#define SAPDBMem_DefaultRawAllocator_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*!
  @class          SAPDBMem_DefaultRawAllocator
  @brief          Simple implementation of the 'SAPDBMem_IRawAllocator'
                  interface.

                  This class uses the 'operator new' and 'operator delete' functions
                  for memory management. It's purpose is to have a simple
                  implementation of the 'SAPDBMem_IRawAllocator' interface at hand
                  that can be used for testing purposes.
                  Caution : must not be called in kernel context
*/

class SAPDBMem_DefaultRawAllocator : public SAPDBMem_IRawAllocator
{
public:
   /*!
      @brief Returns a reference to the singleton of this class
    */

   static SAPDBMem_DefaultRawAllocator& Instance(); 

   /*!
      @brief allocates memory using standard operator new
      @see   SAPDBMem_IRawAllocator::Allocate
   */

   virtual void* Allocate(SAPDB_ULong Count);

   /*!
      @brief allocates memory using standard operator new
      @see   SAPDBMem_IRawAllocator::Allocate
   */

   virtual void* Allocate(SAPDB_ULong Count, const void* Hint)
      { return Allocate(Count); }

   /*!
      @brief frees memory using standard operator delete.
    */

   virtual void Deallocate(void *p);

   /*!
      @brief   Outputs statistics about the number of Allocate and Deallocate calls of
               the allocator to the underlying base allocator.
      @param   CountAlloc [out] The number of calls of method Allocate of the base allocator.
      @param   CountDealloc [out] The number of calls of method Deallocate of the base.
      @return  none
    */

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!
      @brief   Outputs statistics about the number of Allocate and Deallocate calls.
      @param   CountAlloc [out] The number of calls of method Allocate.
      @param   CountDealloc [out] The number of calls of method Deallocate.
      @return  none
    */

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

private:
   /*!
      @brief Does nothing. This constructor is private, because this class is a singleton.
   */
    SAPDBMem_DefaultRawAllocator() 
        : m_CountAlloc(0)
        , m_CountDealloc(0)
    {}
   
   /*!
      @brief Private assignment operator avoids copying.
   */
   SAPDBMem_DefaultRawAllocator operator=(SAPDBMem_DefaultRawAllocator &)
      { return *this; }

   SAPDB_ULong m_CountAlloc;   //!< number of Allocate calls
   SAPDB_ULong m_CountDealloc; //!< number of Deallocate calls

   static SAPDBMem_DefaultRawAllocator* m_Instance; //!< pointer to the singleton instance
};

#endif // SAPDBMem_DefaultRawAllocator_HPP
