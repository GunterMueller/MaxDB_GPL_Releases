/*!
  @file           LVCMem_EmergencyAllocator.hpp
  @author         ThomasA
  @ingroup        OMS memory management
  @brief          emergency allocator which provides memory for clean up tasks.
                  This allocator wrapps internally to a SAPDBMem_DougLeaAllocator 
                  It is also a singleton.
                

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



#ifndef LVCMEM_EMERGENCYALLOCATOR_HPP
#define LVCMEM_EMERGENCYALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "gsp00.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @brief    implements the OMS emergency allocator. This allocator
             is attached to a user task, if a memory request of the
             OMS allocator fails due to lack of memory. All following
             memory requests of that task are fulfilled by the emergeny allocator
             until the user tasks leaves the calling db-procedure.
             Note : At one time at most one user task uses the emergeny allocator.
                    This is realized by use of a region.

 */

class LVCMem_EmergencyAllocator : public SAPDBMem_RawAllocator
{
public:
  typedef tsp00_TaskId TaskId; //!< task identification

  /*!
     @brief          Returns reference to single instance of LVCMem_EmergencyAllocator
     @return value   Reference to LVCMem_EmergencyAllocator instance
  */

  static LVCMem_EmergencyAllocator& Instance();

  /*!
     @brief Allocates contiguous memory for 'count' bytes.

            The memory is suitably aligned for all types.
            The memory is not initialised.  
            std::bad_alloc is thrown on failure.
     @param          count [in] Number of Bytes to allocate
     @return value   Position where memory is allocated
   */

  virtual void* Allocate(SAPDB_ULong count);

  /*!
     @brief same as first variant. hint has no meaning

     @param          count [in] Number of Bytes to allocate
     @param          hint [in] has no meaning
     @return value   Position where memory is allocated
   */

  virtual void* Allocate(SAPDB_ULong count, const void* hint)
  {
    return Allocate(count);
  }

  /*!
     @brief          returns allocator statistics
     @param          BytesUsed [out] bytes in use
     @param          BytesControlled [out] memory controlled by this allocator
     @param          BytesControlled [out] max(BytesUsed)
     @return value   none
   */

  void GetEmergencyAllocatorInfo(SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &BytesControlled,
                                 SAPDB_ULong &MaxBytesUsed) const; 

  /*!
     @brief  releases the emergency allocator. 
             If the given taskId identifies the owner of the emergency
             allocator, the region blocking the emergeny alloctor
             is unlocked.
     @param  taskId [in] taskId of the caller
     @return value   none
   */

  void Release(TaskId taskId);

  /*!
     @brief          reserves the emergency allocator for the specified task.
     @param          taskId [in] taskId of the caller
     @return value   true, if emergency allocator has been granted to task, false
                     if task has already been owner of emergency allocator
   */

  bool Reserve(TaskId taskId);

private:
  LVCMem_EmergencyAllocator(); //!< default constructor
  TaskId                            m_Owner;     //!< current owner of emergency allocator
  static LVCMem_EmergencyAllocator* m_Instance;  //!< the emergency allocator instance
}; 

#endif



