/*!
  @file           SAPDBMem_SynchronizedRawAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Implementation of a general purpose allocator.

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


#ifndef SAPDBMEM_SYNCHRONIZEDRAWALLOCATOR_HPP
#define SAPDBMEM_SYNCHRONIZEDRAWALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

/*!
  @brief          provides a synchronized SAPDBMem_RawAllocator, i.e. an
                  allocator that can be used by serveral tasks simultaneously
 */

class SAPDBMem_SynchronizedRawAllocator : public SAPDBMem_RawAllocator
{
  public :

    typedef SAPDBMem_RawAllocator::FreeRawExtendsEnum FreeRawExtendsEnum; //!< abbreviation      

    /*!
       @brief Constructs an synchronized raw allocator
       @param          Identifier [in] the name of the allocator
       @param          BlockAllocator [in] The underlying block allocator
       @param          FirstBlockSize [in] The size of the memory block in bytes that the RawAllocator starts with.
       @param          SupplementBlockSize [in] The size of the memory blocks in bytes that are allocated, 
                       if the free memory is not sufficient to fulfill the next allocation.
       @param          FreeRawExtends [in] If FREE_RAW_EXTENDS, memory provided by RawAllocator is given back, whenever possible.
       @param          MaxSize [in] The maximal size of the total memory in bytes controlled by the RawAllocator.
     */

    SAPDBMem_SynchronizedRawAllocator (
        const SAPDB_UTF8       *   Identifier,
        SAPDBMem_IBlockAllocator & BlockAllocator,
        SAPDB_ULong                FirstBlockSize,
        SAPDB_ULong                SupplementBlockSize,
        FreeRawExtendsEnum         FreeRawExtends,
        SAPDB_ULong                MaxSize = SAPDBMEM_ALLOCATOR_UNLIMITED
        ) 
        : SAPDBMem_RawAllocator(Identifier, BlockAllocator, &m_lock, 
            FirstBlockSize, SupplementBlockSize, FreeRawExtends, MaxSize)
        , m_lock(Identifier)
    {
    }

    /*!
       @brief destructor, returns all memory to base allocator
    */

    virtual ~SAPDBMem_SynchronizedRawAllocator()
    {
    }
  private :
    RTESync_NamedSpinlock m_lock; //!< synchronisation object
};



#endif
