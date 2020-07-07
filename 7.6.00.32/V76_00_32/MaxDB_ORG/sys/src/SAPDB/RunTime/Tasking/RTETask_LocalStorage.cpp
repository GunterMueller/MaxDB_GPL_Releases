/*!========================================================================

  @file         RTETask_LocalStorage.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task local storage handling.

                A task can use task local storage (TASKLS) to create a 
                unique copy of a data for each task. If no task switching occurs, 
                task local storage acts exactly the same as thread local storage.
                The class 'RTETask_LocalStorage' implements a mechanism to 
                manipulate the task local stoarage associated with the current task.
                If the task is switched, the task local storage is also switched.

  @since        2004-11-22  14:36
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif

#include "RunTime/Tasking/RTETask_LocalStorage.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

RTESync_Spinlock RTETask_LocalStorage::lock(0);
SAPDB_UInt4      RTETask_LocalStorage::indexVersion = 0; 
SAPDB_UInt4      RTETask_LocalStorage::indexUsed[TASKLS_MAX_INDEXES];

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_UInt4 RTETask_LocalStorage::Allocate()
{
    // Locked scope
    RTESync_LockedScope LockedScope( lock ); 

    for ( SAPDB_UInt4 index = 0; index < TASKLS_MAX_INDEXES; ++index )
    {
        if ( indexUsed[index] == 0 )
        {
            indexUsed[index] = ++indexVersion;
            return index + 1;
        }
    }

    return TASKLS_OUT_OF_INDEXES;
}

/*---------------------------------------------------------------------------*/


SAPDB_Bool RTETask_LocalStorage::Free( SAPDB_UInt4 index )
{
    // Locked scope
    RTESync_LockedScope LockedScope( lock ); 

    if ( ( index <= TASKLS_MAX_INDEXES ) && indexUsed[index-1] != 0 )
    {
        indexUsed[index-1] = 0;
        return true;
    }

    return false;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/