/*!
  @file        SrvTasks_Task.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_Task
  @since       2005-06-13  13:39

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
#include "ServerTasks/SrvTasks_Task.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Coordinator.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "heo55k.h" // RTE   : vbegexcl,vendexcl 
#include "hgg08.h"  // g08servertask
//--------------------------------------------------------------------------------------
SrvTasks_Task::SrvTasks_Task (Trans_Context &context)
: m_Context     (context),
  m_pJob        (0),
  m_Index       (SAPDB_MAX_UINT4),
  m_ReleaseSelf (false),
  m_pTaskWaitingToRegisterJob(0)     // PTS 1134588 UH 2005-03-24
{
    if ( context.Task().Type() != RTETask_ITask::Server)
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_WRONG_TASK_TYPE,
                           Msg_Arg("TYPE",SAPDB_ToString(context.Task().Type()))));

    if (! SrvTasks_Coordinator::Instance().Register(*this, m_Context.ErrorList()))
        RTE_Crash(m_Context.ErrorList());
}
//--------------------------------------------------------------------------------------
static inline void DestroySQLManContext(Trans_Context &context)
{
    if ( context.OldTrans().trAcvPtr_gg00 != 0 )
    {
        SQLMan_Context* pContext = reinterpret_cast<SQLMan_Context*>(context.OldTrans().trAcvPtr_gg00);
        destroy (pContext, context.Allocator());
        context.OldTrans().trAcvPtr_gg00 = 0;
    }
}
//--------------------------------------------------------------------------------------
void SrvTasks_Task::UnRegisterJob()
{
    vbegexcl(m_Context.Task().ID(),g08servertask);
    m_pJob = 0;
    // PTS 1134995 UH 2005-04-19 moved the following into region
    if ( m_pTaskWaitingToRegisterJob != 0 )     // PTS 1134588 UH 2005-03-24
        m_Context.Task().ResumeOther(*m_pTaskWaitingToRegisterJob);
    vendexcl(m_Context.Task().ID(),g08servertask);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Task::CheckAllocatorUsage(const AllocatorStatistics &usageBeforeExecute,
                                        Msg_List                  &errlist)
{
    AllocatorStatistics check;
    m_Context.Allocator().CalcStatistics(check.usedBytes, check.maxUsedBytes, check.controlledBytes);
    if ( check.usedBytes == usageBeforeExecute.usedBytes )
    {
        return true;
    }
    errlist = Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_JOB_HOLDS_MEMORY_OF_SERVERTASK,
                       Msg_Arg(SDBMSGTAG_SRVTASKS_JOB_HOLDS_MEMORY_OF_SERVERTASK__BYTES_USED,
                               SAPDB_ToString(check.usedBytes - usageBeforeExecute.usedBytes)));
    return false;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Task::Run()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Task::Main", SrvTasks_Trace, 5);

    if ( SAPDB_MAX_UINT4 == m_Index )
    {
        m_Context.ErrorList() = Msg_List(Msg_List::Error,	SDBMSG_SRVTASKS_TASK_NOT_PROPERLY_REGISTERED,
                                         Msg_Arg("JOB_TASKID",SAPDB_ToString(m_Context.Task().ID())));
        RTE_Crash(m_Context.ErrorList());
    }
    
    RTE_TaskDisplayName      description;
	RTE_TaskDisplayNameShort shortDescription;
    AllocatorStatistics      usageBeforeJob;

    while ( 1 )
    {
        if ( SrvTasks_Trace.TracesLevel(7) )
        {
            Kernel_VTrace trace;
            WriteToTraceIntern(trace,true);
            trace << " SUSPENDED";
        }
        m_Context.Task().ResetPendingCancel();
        m_Context.Task().Suspend(TSK_VSUSPEND_REASON_NO_WORK);
        m_Context.Task().ResetPendingCancel();

        if ( m_pJob == 0 )
        {
            m_Context.ErrorList() = Msg_List( Msg_List::Error, SDBMSG_SRVTASKS_TASK_STARTED_WITHOUT_JOB,
                                              Msg_Arg(SDBMSGTAG_SRVTASKS_TASK_STARTED_WITHOUT_JOB__TASK_ID,SAPDB_ToString(m_Context.Task().ID())));
            AppendContextInfo(__CONTEXT__, m_Context.ErrorList(), "Task started without a job.");
            Kernel_VTrace trace;
            WriteToTraceIntern (trace);
            trace << " CURRENT SERVERTASK" << FlushLine;
            RTE_Crash(m_Context.ErrorList());
        }
        
        m_Context.Allocator().CalcStatistics(usageBeforeJob.usedBytes, usageBeforeJob.maxUsedBytes, usageBeforeJob.controlledBytes);

        m_pJob->GetDescription( description, shortDescription );
                         
        SrvTasks_Coordinator::Instance().SetTaskStarted (m_Context.Task(), m_Index, description, shortDescription );
        
        m_pJob->BeginExecute(*this);

        m_ReleaseSelf = ! m_pJob->GetAttributes().KeepServerTaskReserved;
    
        m_pJob->Execute();  // The jobs execution is starting here.
        
        if ( m_ReleaseSelf )
            SrvTasks_Coordinator::Instance().SetTaskStopped (m_Context.Task(), m_Index);

        if ( m_pJob->GetWaitingTask() != 0 )
        {
            if ( ! CheckAllocatorUsage(usageBeforeJob, m_Context.ErrorList()) )
            {
                AppendContextInfo(__CONTEXT__,m_Context.ErrorList());
                #ifdef SAPDB_SLOW
                if ( SrvTasks_Check.ChecksLevel(9) )
                    RTE_Crash(m_Context.ErrorList());
                #else
                RTE_Message(m_Context.ErrorList());
                m_Context.ErrorList().ClearMessageList();
                #endif
            }

            // IMPORTANT after this the control over the job is switched back to the user task
            m_pJob->EndExecute();
        }
        else
        {
            if ( ! CheckAllocatorUsage(usageBeforeJob, m_Context.ErrorList()) )
            {
                AppendContextInfo(__CONTEXT__,m_Context.ErrorList());
                m_pJob->AppendContextInfo(__CONTEXT__,m_Context.ErrorList(),"CheckAllocatorUsage failed.");
                #ifdef SAPDB_SLOW
                if ( SrvTasks_Check.ChecksLevel(9) )
                    RTE_Crash(m_Context.ErrorList());
                #else
                RTE_Message(m_Context.ErrorList());
                m_Context.ErrorList().ClearMessageList();
                #endif
            }
            
            // nobody is waiting for this job
            m_pJob->EndExecute();

            // Nobody is waiting so we have to handle any error or message here
            // before destroying all contexts.
            if ( m_pJob->GetResult(m_Context.ErrorList()) == SrvTasks_Job::ok )
            {
                if ( ! m_Context.ErrorList().IsEmpty() )
                    RTE_Message ( m_Context.ErrorList()
                                  +
                                  Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ERRLIST_NOT_EMPTY) );
            }
            else
            {
                m_pJob->AppendContextInfo(__CONTEXT__,m_Context.ErrorList(),"The jobs result code is not OK.");
                RTE_Message (m_Context.ErrorList());
            }
            m_Context.ErrorList().ClearMessageList();
            // PTS 1135617 mb 2005-05-19 m_pJob must be changed in region, thus it cannot be given to destroy.
            // Also destroy has to be called before changing m_pJob because after setting m_pJob to NULL
            // another task may use the transallocator while registering a job.
            SrvTasks_Job *pJobToDestroy = m_pJob; // PTS 1135511 UH 2005-05-12
            destroy(pJobToDestroy,m_Context.Allocator()); // PTS 1135511 UH 2005-05-12
        }
        UnRegisterJob();     // m_pJob is invalidated in region
        if ( m_ReleaseSelf )
        {
            if ( ! SrvTasks_Coordinator::Instance().Release (m_Context.Task(), m_Index) )
            {
                m_Context.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_RELEASE_TASK_FAILED,
                                                       Msg_Arg("JOB_TASKID", SAPDB_ToString(m_Context.Task().ID())),
                                                       Msg_Arg("INDEX",      SAPDB_ToString(m_Index))));
                RTE_Crash(m_Context.ErrorList());
            }
        }
        
        DestroySQLManContext(m_Context);
    } // endwhile
}
//--------------------------------------------------------------------------------------
void SrvTasks_Task::Abort(RTETask_ITask &usertask,
                          bool           inRegion)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Task::Abort", SrvTasks_Trace, 5);
    m_Context.Task().Cancel();
}
//--------------------------------------------------------------------------------------
SrvTasks_Task::RegisterJobResult
SrvTasks_Task::RegisterJob (RTETask_ITask        & task,
                            SrvTasks_Job         & job,
                            Msg_List             & errlist,
                            bool                   waitForTaskToBeFinished,
                            SrvTasks_WaitingTask * waitingTask )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Task::RegisterJob", SrvTasks_Trace, 5);

    vbegexcl(task.ID(),g08servertask);
    if ( m_pJob != 0 )
    {
        if ( waitForTaskToBeFinished )     // PTS 1134588 UH 2005-03-24
        {
            if ( m_pTaskWaitingToRegisterJob != 0 )
            {
                vendexcl(task.ID(),g08servertask);
                return cannotWaitforRegister;
            }
            
            m_pTaskWaitingToRegisterJob = &task;
            
            vendexcl(task.ID(),g08servertask);
            
            task.Suspend(214);

            vbegexcl(task.ID(),g08servertask);

            if ( m_pTaskWaitingToRegisterJob != &task )
            {
                m_pTaskWaitingToRegisterJob = 0;
                AppendContextInfo    (__CONTEXT__, errlist, "After waiting for RegisterJob ");
                job.AppendContextInfo(__CONTEXT__, errlist, "This Job should be executed.");
                Kernel_VTrace trace;
                trace << "ERROR: A servertask should execute two jobs." << FlushLine;
                SrvTasks_Task::WriteToTraceIntern (trace, false);
                trace << FlushLine << "The new jobs is: " << job;
                vendexcl(task.ID(),g08servertask);
                return registerError;
            }

            m_pTaskWaitingToRegisterJob = 0;
        }
        
        if ( m_pJob != 0 )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_JOB_ALREADY_EXISTS);
            AppendContextInfo        (__CONTEXT__, errlist, "This Task should execute two jobs.");
            job.AppendContextInfo    (__CONTEXT__, errlist, "This Job should be executed.");
            Kernel_VTrace trace;
            trace << "ERROR: A servertask should execute two jobs." << FlushLine;
            SrvTasks_Task::WriteToTraceIntern (trace, false);
            trace << FlushLine << "The new jobs is: " << job;
            vendexcl(task.ID(),g08servertask);
            return registerError;
        }
    }
    
    if ( waitingTask != 0 )
        job.SetWaitingTask(*waitingTask);
        
    if ( job.GetWaitingTask() != 0 )
        m_pJob = &job;
    else
    {
        m_Context.Allocator().AssignToTask(task.ID()); // PTS 1133914 UH 2005-02-15 
                                                       // because the servertasks own allocator is used here by another task
        
        if ( ! job.CreateCopy (m_Context.Allocator(), errlist, m_pJob) )
        {
            m_Context.Allocator().AssignToTask(m_Context.Task().ID());

            errlist = errlist + Msg_List(Msg_List::Error,	SDBMSG_SRVTASKS_ALLOCATION_FAILED);
            job.AppendContextInfo(__CONTEXT__,errlist,"Job could not be copied.");
            if ( ! SrvTasks_Coordinator::Instance().Release (m_Context.Task(), m_Index) )
            {
                errlist = Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_RELEASE_TASK_FAILED,
                                   Msg_Arg("JOB_TASKID", SAPDB_ToString(m_Context.Task().ID())),
                                   Msg_Arg("INDEX",      SAPDB_ToString(m_Index)) );
                RTE_Crash(errlist);
            }
            return registerError;
        }

        m_Context.Allocator().AssignToTask(m_Context.Task().ID());
    }
    m_pJob->SetStarted(task);
    if ( SrvTasks_Trace.TracesLevel(7) )
    {
        Kernel_VTrace trace;
        WriteToTraceIntern(trace,true);
        trace << " RESUMED BY T" << task.ID();
    }
    task.ResumeOther(m_Context.Task());
    vendexcl(task.ID(),g08servertask);
    return registered;
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Task::IsAborted() const
{
    SAPDBTRACE_WRITELN(SrvTasks_Trace, 6, (m_Context.Task().CancelPending() ? "CANCELLED" : ""));
    return m_Context.Task().CancelPending();
}
//--------------------------------------------------------------------------------------
const char * SrvTasks_Task::StateString [] =
{
	"init",
    "nowork",
    "running",
    "waiting"
};
//--------------------------------------------------------------------------------------
void SrvTasks_Task::WriteToTraceIntern (Kernel_VTrace &trace,
                                        bool           withJob) const
{
    trace << "ServerTask #" << m_Index
          << " T" << m_Context.Task().ID()
          << " ";
	if ( withJob )
    {
        if ( m_pJob != 0 )
            trace << *m_pJob;
        else
            trace << ", no job";
    }
    else
        trace << (m_pJob != 0 ? ", job exists" : ", no job");
    trace << ", TaskAddr: 0x" << SAPDB_ToString((const void*)this,SAPDB_ToStringClass::hex);
}
//--------------------------------------------------------------------------------------
SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_ITaskForJob &task)
{
    const SrvTasks_Task * const pTask = static_cast<const SrvTasks_Task*>(&task);
    os    << "ServerTask #" << pTask->m_Index
          << " T" << pTask->m_Context.Task().ID()
          << (pTask->m_pJob != 0 ? ", job exists" : ", no job")
          << ", TaskAddr: 0x" << SAPDB_ToString((const void*)&task,SAPDB_ToStringClass::hex)
          << (pTask->m_ReleaseSelf ? ", release self" : "");
     return os;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Task::AppendContextInfo (SAPDB_Char  const * const  filename,
                                       SAPDB_UInt4 const          linenumber,
                                       SAPDBErr_MessageList      &errlist,
                                       SAPDB_Char  const * const  title) const
{
    errlist.AddDetail(SDBMSG_SRVTASKS_SERVER_TASK_INFO,
                      Msg_Arg("SERVER_TASK_ID",SAPDB_ToString(m_Context.Task().ID())),
                      Msg_Arg("SERVER_TASK_INDEX",SAPDB_ToString(m_Index)),
                      Msg_Arg("SERVER_TASK_KIND",(m_ReleaseSelf?"dynamic":"static")),
                      Msg_Arg("DESCRIPTION",(title!=0?title:"")));
}
