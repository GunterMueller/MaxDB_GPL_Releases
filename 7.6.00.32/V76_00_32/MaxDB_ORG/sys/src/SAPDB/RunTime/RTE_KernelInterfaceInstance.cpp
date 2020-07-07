/*!
  @file    RTE_KernelInterfaceInstance.cpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface for use in liboms.

\if EMIT_LICENCE

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

\endif
*/

#include "RunTime/RTE_KernelInterfaceInstance.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "heo02x.h"
#ifndef TESTSIMUL_SMALL
#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_Task.hpp" /* &variant -simul */
#endif
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

RTE_IInterface *RTE_IInterface::it = 0;

RTE_IInterface *RTE_IInterface::Initialize()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_KernelInterfaceInstance));
    if (0 == RTE_IInterface::it )
    {
        RTE_IInterface::it = new(Space) RTE_KernelInterfaceInstance;
    }
    return RTE_IInterface::it;
}

    /*!
      @brief get a procedure address from given shared library handle
     */
RTE_SharedLibProcAddress RTE_KernelInterfaceInstance::GetProcAddress( RTE_SharedLibHandle hLib,
                                                                     SAPDB_Char const *szSqlFunction, 
                                                                     SAPDB_Char *pErrText,
                                                                     SAPDB_UInt4 lenErrTextInBytes)
{
    return (RTE_SharedLibProcAddress)sqlGetProcAddress( hLib
                                                      , szSqlFunction
                                                      , pErrText
                                                      , lenErrTextInBytes );
}

/*!
  @brief set the task state to yield on spinlock, if not already set

  A task can indicate by this interface, that it is yielding to acquiere the given spinlock.
  This call is part of the interface, to allow liboms to indicate its spinlock yield entry.

  @param pLock [in] pointer to spinlock 
  @param beginning [in] flag to indicate yield begins or ends
  @return none
 */
void RTE_KernelInterfaceInstance::SetTaskStateToYieldOnSpinlock(RTESync_Spinlock const *pLock, bool beginning)
{
#ifndef TESTSIMUL_SMALL
    RTETask_Task::SetTaskStateToYieldOnSpinlock(pLock, beginning);    
#endif
}

/*!
  @brief yield while potentially inside task code

  NOTE: This call produces extra overhead to detect if runnning inside a task or runtime thread.
  If you already know you are running in a task, you can specify a task id. If you know you are not
  running in a task, better call GiveUpTimeSlice() directly !

  This call allows to dispatch next task/thread/process. Used if block i.e. by a spinlock
  @param taskId [in] taskid currently running in ( optional, but performance relevant...)
  @param allowLoadBalancing [in] select to allow/forbid load balancing if yielding in tasking code
 */
void RTE_KernelInterfaceInstance::TaskYield(RTE_TaskId taskId, SAPDB_Bool allowLoadBalancing)
{
#ifndef TESTSIMUL_SMALL
    RTETask_Task *pTask;

    if ( taskId != 0 )
    {
        pTask = RTETask_Task::ByTaskID(taskId);
    }
    else
    {
        pTask = RTETask_Task::Self();
    }

    if ( !pTask )
    {
#endif
        GiveUpTimeSlice(); 
#ifndef TESTSIMUL_SMALL
    }
    else
    {
        // during thread startup with USE_COROUTINES=NO message are produced too!
        // These messages may collide on a spinlock. This spinlock leads to calling 
        // this function, but not with 'CurrentTask' but in a thread for another
        // task in the task group...
        if ( pTask != &(pTask->TaskScheduler().CurrentTask()) )
        {
            GiveUpTimeSlice(); 
        }
        else
        {
            pTask->YieldTask(allowLoadBalancing);
        }
    }
#endif
}
