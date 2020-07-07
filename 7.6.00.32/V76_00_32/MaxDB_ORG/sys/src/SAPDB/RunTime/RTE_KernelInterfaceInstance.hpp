/*!
  @file    RTE_KernelInterfaceInstance.hpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface implementation for use in liboms.

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

#ifndef RTE_KERNELINTERFACEINSTANCE_HPP
#define RTE_KERNELINTERFACEINSTANCE_HPP

#include "RunTime/RTE_CommonInterfaceInstance.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class RTE_KernelInterfaceInstance
  @brief The exported kernel runtime interface
  
  This class contains all functions provided to all threads or tasks inside a livecache kernel or liboms
 */
class RTE_KernelInterfaceInstance : public RTE_CommonInterfaceInstance
{
public:
    /*!
      @brief get a procedure address from given shared library handle
     */
    virtual RTE_SharedLibProcAddress GetProcAddress( RTE_SharedLibHandle hLib,
                                                     SAPDB_Char const *szSqlFunction, 
                                                     SAPDB_Char *pErrText,
                                                     SAPDB_UInt4 lenErrTextInBytes);
    /*!
      @brief set the task state to yield on spinlock, if not already set

      A task can indicate by this interface, that it is yielding to acquiere the given spinlock.
      This call is part of the interface, to allow liboms to indicate its spinlock yield entry.

      @param pLock [in] pointer to spinlock 
      @param beginning [in] flag to indicate yield begins or ends
      @return none
     */
    virtual void SetTaskStateToYieldOnSpinlock(RTESync_Spinlock const *pLock, bool beginning);

    /*!
      @brief yield while potentially inside task code

      NOTE: This call produces extra overhead to detect if runnning inside a task or runtime thread.
      If you already know you are running in a task, you can specify a task id. If you know you are not
      running in a task, better call GiveUpTimeSlice() directly !

      This call allows to dispatch next task/thread/process. Used if block i.e. by a spinlock
      param taskId [in] taskid currently running in ( optional, but performance relevant...)
     */
    virtual void TaskYield(RTE_TaskId taskId=0, SAPDB_Bool allowLoadBalancing = false);
}; /* endclass RTE_KernelInterfaceInstance */

#endif  /* RTE_KERNELINTERFACEINSTANCE_HPP */
