/*!
  @file        SrvTasks_WaitingTask.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_Job
  @since       2005-01-04  16:09

\if EMIT_LICENCE
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

\endif
*/
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"

//--------------------------------------------------------------------------------------
void SrvTasks_WaitingTask::Wait()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_WaitingTask::Wait", SrvTasks_Trace, 5);
    if ( SrvTasks_Trace.TracesLevel(7) )
        Kernel_VTrace() << "TASK T" << m_pTask->ID() << " is waiting for a job";
    m_pTask->Suspend(TSK_VSUSPEND_REASON_WAIT_FOR_JOB_END);
}
//--------------------------------------------------------------------------------------
void SrvTasks_WaitingTask::SetWaitFlag()
{
	m_Lock.Lock();
    m_IsWaiting = true;
	m_Lock.Unlock();
}
//--------------------------------------------------------------------------------------
void SrvTasks_WaitingTask::Resume(RTETask_ITask &task)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_WaitingTask::Resume", SrvTasks_Trace, 5);
    bool resumeWaitingTask = false;
	m_Lock.Lock();
    if ( m_IsWaiting )
    {
        resumeWaitingTask = true;
        m_IsWaiting       = false;
    }
	m_Lock.Unlock();

    if ( SrvTasks_Trace.TracesLevel(7) )
        Kernel_VTrace() << "TASK T" << m_pTask->ID()
                        << (resumeWaitingTask ? " is resumed by T" : " would be resumed by T")
                        << task.ID();

    if ( resumeWaitingTask )
        task.ResumeOther(*m_pTask);
}
