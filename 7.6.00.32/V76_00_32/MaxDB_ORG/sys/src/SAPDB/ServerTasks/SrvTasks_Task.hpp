/*!
  @file           SrvTasks_Task.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines what a Job is.

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
#ifndef SrvTasks_Task_HPP
#define SrvTasks_Task_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "ServerTasks/SrvTasks_ITask.hpp"
//--------------------------------------------------------------------------------------
class RTETask_ITask;
class SrvTasks_Job;
class Msg_List;
class SAPDBMem_IRawAllocator;
class Kernel_VTrace;
class Trans_Context;
//--------------------------------------------------------------------------------------
/// This implements one server task
class SrvTasks_Task : public SrvTasks_ITaskForJob, public SrvTasks_ITask
{
public: // SrvTasks_ITask USED BY USERS
    virtual RegisterJobResult RegisterJob (RTETask_ITask        & requestortask,
                                           SrvTasks_Job         & job,
                                           Msg_List             & errlist,
                                           bool                   waitForTaskToBeFinished,
                                           SrvTasks_WaitingTask * waitingTask = 0);
    virtual void Abort(RTETask_ITask &task,
                       bool           inRegion = false);
public: // ONLY USED BY SrvTasks_Coordinator
    /// constructor which is called when server task begins to run
    /// Only a server task may construct such an object
    SrvTasks_Task (Trans_Context &context);
    /// This executes the servertask. At first the servertask suspsends with NoWork.
    void Run();
    /// Give some context information
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDBErr_MessageList      &errlist,
                            SAPDB_Char  const * const  title = 0) const;
public: // SrvTasks_ITaskForJob USED BY JOBS
    
    virtual bool IsAborted() const;
    virtual Trans_Context& GetContext()
    {
        return m_Context;
    }
    /// This is the reference back to the coordinator
    void SetIndex (SAPDB_UInt index)
    {
        m_Index = index;
    }
private:
    /// the possible states of a servertask
    enum State
    {
        init,
        nowork,
        running,
        waiting,
        invalidState_DONT_USE
    };
    /// this is used to have a nice print out
    static const char * StateString[invalidState_DONT_USE];
    /// write some trace, withJob is ONLY SAFE if called by this class itself
    void WriteToTraceIntern (Kernel_VTrace &trace,
                             bool           withJob = true) const;
	/// This is used by the servertask to disconnect from the job
    void UnRegisterJob();
    /// used to verify if a job is not using the servertasks allocator anymore after Execute()
    struct AllocatorStatistics
    {
        SAPDB_ULong usedBytes;
        SAPDB_ULong maxUsedBytes;
        SAPDB_ULong controlledBytes;
    };
    /// Checks if current usage is different from the given value, if so false is returned.
    bool CheckAllocatorUsage(const AllocatorStatistics &usageBeforeExecute,
                             Msg_List                  &errlist);
    
private:
    /// the trans context of the servertask
    Trans_Context &m_Context;
    /// the job to do if not null
    SrvTasks_Job  *m_pJob;
    /// This is an index which can be used by the coordinator to find the control structure
    SAPDB_UInt m_Index;
    /// if this is true the servertask tries to release itself before suspending with NoWork
    bool       m_ReleaseSelf;
    /// if this task pointer is assigned there exists a task which waits for this dedicated task (e.g. Savepoint)
    RTETask_ITask *m_pTaskWaitingToRegisterJob;
private:
    /// << operator is a friend to access protected members for output
    friend SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_ITaskForJob &task);
};
#endif // SrvTasks_Task_HPP
