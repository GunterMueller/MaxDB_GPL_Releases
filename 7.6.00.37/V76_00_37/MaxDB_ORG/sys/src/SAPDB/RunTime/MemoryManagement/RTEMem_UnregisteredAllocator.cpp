
/*!***************************************************************************

  module      : RTEMem_UnregisteredAllocator.cpp

  -------------------------------------------------------------------------

  responsible : Robin

  special area: RTE
  description : 
  

  last changed: 2004-01-22  12:00
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/MemoryManagement/RTEMem_UnregisteredAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/




  /*! 
    function: Allocate

    description: allocate memory: If the byte array belonging to the class is not yet
                 exhauted, take it from there. Else get the memory from RTEMem_RteAllocator

    return value: a pointer to a memory area of the desired size
   */
void *RTEMem_UnregisteredAllocator::Allocate(SAPDB_ULong size)
{
    SAPDB_ULong alignedSize = SAPDB_ALIGN(size,8);
    // check if the allocation request can be fullfilled from my static array.
    // If not, use the RTEMem_RteAllocator, hoping that it has been initialized yet...
    RTESync_LockedScope lock(m_Spinlock);
    if((m_bytesAllocated + alignedSize) > SIZE_OF_STATIC_ARRAY)
    {
        return RTEMem_RteAllocator::Instance().Allocate(alignedSize);
    }
    else
    {
        SAPDB_Byte *addressToReturn = (SAPDB_Byte *)m_arrayForFirstAllocations + m_bytesAllocated;
        m_bytesAllocated += alignedSize;
        return addressToReturn;
    }
}

  /*! 
    function: Deallocate

    description: give back a portion of memory that has been allocated by the Allocate() method.
                 if the memory portion had been allocated in the (static) byte array belonging to the class,
                 the method has no effect. So implementation of any control structures can be avoided ;-) 
                 This is acceptable, because it is intended to fullfill only the memory allocations for
                 singletons from the static array.

   */
void RTEMem_UnregisteredAllocator::Deallocate (void *space)
{
    // check if the memory area to be deallocated is in my static array.
    // If so, do nothing (well... the intention is to use the static array
    // for the memory for the RTEMem_* allocator-singletons only.)
    // If the memory area is not in my static array, return it to
    // the RTEMem_RteAllocator
    RTESync_LockedScope lock(m_Spinlock);
    if(!(space >= (void *)m_arrayForFirstAllocations && space <= (void *)((SAPDB_Byte *)m_arrayForFirstAllocations+m_bytesAllocated)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(space);
    }
}

  /*! 
    function: Instance

    description: Return reference to single instance of RTEMem_UnregisteredAllocator

    return value: Reference to RTEMem_UnregisteredAllocator instance
   */
SAPDBMem_IRawAllocator & RTEMem_UnregisteredAllocator::Instance( )
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_UnregisteredAllocator));

    if ( !m_Instance )
    {
        m_Instance = new (Space) RTEMem_UnregisteredAllocator();
    }

    SAPDBERR_ASSERT_STATE(m_Instance != NULL);

    return *m_Instance;
}

RTEMem_UnregisteredAllocator    * RTEMem_UnregisteredAllocator::m_Instance     = NULL;
