/*!
  @file           SrvTasks_WaitingTask.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines SrvTasks_WaitingTask

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
#ifndef SrvTasks_WaitingTask_HPP
#define SrvTasks_WaitingTask_HPP
//--------------------------------------------------------------------------------------
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
//--------------------------------------------------------------------------------------
class RTETask_ITask;
//--------------------------------------------------------------------------------------
/// This is the context which is used by waiting tasks to wait for a job
class SrvTasks_WaitingTask
{
public:
    /// initialize values
    SrvTasks_WaitingTask(RTETask_ITask &task)
    : m_pTask(&task),
      m_IsWaiting(false)
    {}
    /// a new task is assigned to a waiting context
    void AssignNewTask(RTETask_ITask &task)
    {
        m_pTask = &task;
    }
    /// returns the task
    RTETask_ITask& Task() const
    {
        return *m_pTask;
    }
    /// internal state is set to true
    void SetWaitFlag();
    /// the caller is suspended
    void Wait();
    /// The waiting caller is resumed
    void Resume(RTETask_ITask &task);
private:
    RTETask_ITask    *m_pTask;
    volatile bool     m_IsWaiting;
    RTESync_Spinlock  m_Lock;
};
#endif // SrvTasks_WaitingTask_HPP
