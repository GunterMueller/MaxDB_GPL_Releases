/*!========================================================================

  @file         RTETask_LocalStorage.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task local storage handling.

                A task can use task local storage (TASKLS) to create a 
                unique copy of a data for each task. If no task switching occurs, 
                task local storage acts exactly the same as thread local storage.
                The class 'RTETask_LocalStorage' implements a mechanism to 
                manipulate the task local stoarage associated with the current task.
                If the task is switched, the task local storage is also switched.

  @since        2004-11-22  11:41
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


#ifndef RTETASK_LOCALSTORAGE_HPP
#define RTETASK_LOCALSTORAGE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! No more task local storage entries available. To get more entries increase TASKLS_MAX_INDEXES */
#define TASKLS_OUT_OF_INDEXES              0xFFFFFFFFUL

/*! Maximum number task local storage entries available */
#define TASKLS_MAX_INDEXES                 10


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;

/*!---------------------------------------------------------------------------
  @class   RTETask_LocalStorage

  @brief   Task local storage handling.

           A task can use task local storage (TASKLS) to create a 
           unique copy of a data for each task. If no task switching occurs, 
           task local storage acts exactly the same as thread local storage.
           This class implements a mechanism to  manipulate the task local 
           stoarage associated with the current task. If the task is switched,
           the task local storage is also switched.

 ----------------------------------------------------------------------------*/

class RTETask_LocalStorage
{
    /*!
    @brief         'Allocate' allocates a task local storage index. 
                    Any task can subsequently use this index to store and retrieve
                    pointer values that are local to the task.

    @return        Task local storage index
    */
    SAPDB_UInt4   Allocate();


    /*!
    @brief         'Free' releases a task local storage index, making it available for reuse.

    @param         index [in]- Index that was allocated by the 'Alloc' methode.

    @return        False, if the index is invalid because its out of range or it has
                   not been allocated by 'Allocate'.
    */
    SAPDB_Bool Free( SAPDB_UInt4 index );


    /*!
    @brief         'SetValue' stores a value in the calling tasks's task local storage slot for 
                   the specified index. Each task has its own slot for each task local 
                   storage index.
    @param         index   [in]- Index that was allocated by the 'Alloc' methode.
    @param         dataPtr [in]- Pointer value to be stored in the task local storage slot 
                                 for the calling task.

    @return        False, if the index is invalid because its out of range or it has
                   not been allocated by 'Allocate'.
    */
    SAPDB_Bool SetValue( SAPDB_UInt4 index,  void* dataPtr )
    {
        if (( index <= TASKLS_MAX_INDEXES ) && ( indexUsed[index-1] != 0 ))
        {
            m_Pool[index-1].indexVersion = indexUsed[index-1];
            m_Pool[index-1].dataPtr      = dataPtr;

            return true;
        }

        return false;
    }


    /*!
    @brief         'GetValue' retrieves the value in the calling tasl's task local 
                   storage slot for the specified index. Each task has its own slot for 
                   each task local storage index.
    @param         index [in]- Index that was allocated by the 'Alloc' methode.

    @return        If the methode succeeds, the return value is the value stored in the 
                   calling tasks's task local storage slot associated with the specified index.

                   If the methode fails, the return value is NULL, because the index is out of 
                   range or it has not been allocated by 'Allocate'.
    */
    void* GetValue( SAPDB_UInt4   index )
    {
        if (( index <= TASKLS_MAX_INDEXES ) && ( indexUsed[index-1] == m_Pool[index-1].indexVersion ))
            return m_Pool[index-1].dataPtr;

        return 0;
    }

private:
    friend struct RTETask_Task;            // only the task class is allowed to create this object

    RTETask_LocalStorage()
    { memset (m_Pool, 0, sizeof(m_Pool)); }


    struct 
    {
        SAPDB_UInt4   indexVersion;       // data pointer version
        void*         dataPtr;            // local storage data pointer
    } m_Pool [TASKLS_MAX_INDEXES];

private:
    static RTESync_Spinlock    lock;                           // used for lock global index access
    static SAPDB_UInt4         indexVersion;                   // current index version
    static SAPDB_UInt4         indexUsed[TASKLS_MAX_INDEXES];  // local storage index used flag
};

#endif  /* RTETASK_LOCALSTORAGE_HPP */
