/*!
  @file        SrvTasks_Job.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_Job
  @since       2004-06-08

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
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Transaction/Trans_Context.hpp"
#include "ggg00.h"  // tgg00_TransContext
#include "hkb53.h"  // k53child_trans_build
#include "heo55k.h" // RTE   : vbegexcl,venexcl 
#include "hgg08.h"  // g08servertask
#include "vsp002.h" // sp2pt_server

//--------------------------------------------------------------------------------------
const char * SrvTasks_Job::StateString [] =
{
    "prepared",
    "started",
    "executing",
    "finished",
    "aborted",
    "confirmed"
};
//--------------------------------------------------------------------------------------
SrvTasks_Job::SrvTasks_Job(const RTE_TaskDisplayName      description,
                           const RTE_TaskDisplayNameShort shortdescription,
                           const Attributes               attributes,
                                 SrvTasks_WaitingTask   * waitingTask )
: m_Result            (ok),
  m_Attributes        (attributes),
  m_State             (prepared),
  m_JobType           (jobUnspecified),
  m_WaitingTask       (waitingTask),
  m_ServerTaskContext (0)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::SrvTasks_Job", SrvTasks_Trace, 5);
    SAPDB_strncpy_zero_terminated ( m_Description,      description,      sizeof(RTE_TaskDisplayName)      );
    SAPDB_strncpy_zero_terminated ( m_ShortDescription, shortdescription, sizeof(RTE_TaskDisplayNameShort) );
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::SrvTasks_Job(      SrvTasks_JobTypes        jobtype,
                           const Attributes               attributes,
                                 SrvTasks_WaitingTask   * waitingTask )
: m_Result            (ok),
  m_Attributes        (attributes),
  m_State             (prepared),
  m_JobType           (jobtype),
  m_WaitingTask       (waitingTask),
  m_ServerTaskContext (0)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::SrvTasks_Job", SrvTasks_Trace, 5);
    SAPDB_strncpy_zero_terminated ( m_Description,      SrvTasks_JobTypesString[m_JobType],      sizeof(RTE_TaskDisplayName)      );
    SAPDB_strncpy_zero_terminated ( m_ShortDescription, SrvTasks_JobTypesStringShort[m_JobType], sizeof(RTE_TaskDisplayNameShort) );
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::Abort(RTETask_ITask &task)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::Abort", SrvTasks_Trace, 5);
	vbegexcl(task.ID(),g08servertask);
    if ( m_ServerTaskContext != 0 )
        m_ServerTaskContext->Abort(task, true);
	vendexcl(task.ID(),g08servertask);
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::SetStarted(RTETask_ITask &usertask)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::SetStarted", SrvTasks_Trace, 5);
    visexcl(usertask.ID(),g08servertask);
    if ( m_State != prepared )
    {
        m_ErrList.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_WRONG_JOB_STATE,
                                            Msg_Arg("STATE",m_State < invalidState_DONT_USE ? StateString[m_State] : SAPDB_ToString(m_State))));
        AppendContextInfo (__CONTEXT__, m_ErrList, "m_State != prepared");
        RTE_Crash(m_ErrList);
    }
    m_State = started;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::AppendContextInfo (SAPDB_Char  const * const  filename,
                                      SAPDB_UInt4 const          linenumber,
                                      Msg_List                  &errlist,
                                      SAPDB_Char  const * const  title) const
{
    errlist.AddDetail(   SDBMSG_SRVTASKS_JOB_INFO_TITLE,Msg_Arg("DESCRIPTION",(title!=0?title:"")));

    if (m_WaitingTask != 0)
    {
        errlist.AddDetail(   SDBMSG_SRVTASKS_JOB_INFO,
                             Msg_Arg("JOB_ID",            SAPDB_ToString(m_Attributes.Id)),
                             Msg_Arg("DESCRIPTION",       m_Description),
                             Msg_Arg("JOB_STATE",         m_State < invalidState_DONT_USE ? StateString[m_State] : SAPDB_ToString(m_State)),
                             Msg_Arg("JOB_WAITING_TASKID",SAPDB_ToString(m_WaitingTask->Task().ID())),
                             Msg_Arg("JOB_RESULT",        m_Result==ok?"OK":static_cast<const char*>(SAPDB_ToString(m_Result)) ));
    }
    else
    {
        errlist.AddDetail(   SDBMSG_SRVTASKS_JOB_INFO,
                             Msg_Arg("JOB_ID",            SAPDB_ToString(m_Attributes.Id)),
                             Msg_Arg("DESCRIPTION",       m_Description),
                             Msg_Arg("JOB_STATE",         m_State < invalidState_DONT_USE ? StateString[m_State] : SAPDB_ToString(m_State)),
                             Msg_Arg("JOB_WAITING_TASKID","--"),
                             Msg_Arg("JOB_RESULT",        m_Result==ok?"OK":static_cast<const char*>(SAPDB_ToString(m_Result)) ));
    }
}
//--------------------------------------------------------------------------------------
/// This is used to write a job to the knltrace.
SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_Job &job)
{
    os    << "JOBID "
          << job.m_Attributes.Id
          << " (" << job.m_Description << ") "
          << (job.m_State < SrvTasks_Job::invalidState_DONT_USE ? job.StateString[job.m_State] : SAPDB_ToString(job.m_State))
          << " WaitT" << (job.m_WaitingTask != 0 ? job.m_WaitingTask->Task().ID() : -1)
          << ", Result: "  << job.m_Result;
    os    << ", JobAddr: 0x" << SAPDB_ToString((const void*)&job,SAPDB_ToStringClass::hex);
//    if ( job.m_ServerTaskContext != 0 )
//        os << ", task:  " << *(job.m_ServerTaskContext);
    return os;
}
//--------------------------------------------------------------------------------------
Trans_Context& SrvTasks_Job::GetTransContext()
{
    if ( 0 == m_ServerTaskContext )
    {
        m_Result = error;
        AppendContextInfo(__CONTEXT__,m_ErrList,"Session context not found");
        RTE_Crash(m_ErrList);
    }
    return m_ServerTaskContext->GetContext();
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::BeginExecute (SrvTasks_ITaskForJob &context)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::BeginExecute", SrvTasks_Trace, 5);
	vbegexcl(context.GetContext().Task().ID(),g08servertask);
    m_Result            = ok;
    m_State             = executing;
    m_ServerTaskContext = &context;

    Trans_Context &trans = GetTransContext();

    trans.ServerTaskInit( m_JobType, m_Attributes.Id, m_Attributes.IgnoreDBFullSecureLimit );

    if ( SrvTasks_Trace.TracesLevel(4) )
        Kernel_VTrace() << "Begin: " << *this;

    vendexcl(context.GetContext().Task().ID(),g08servertask);
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::EndExecute ()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::EndExecute", SrvTasks_Trace, 5);

    Trans_Context &servertrans = GetTransContext();
    
    vbegexcl(servertrans.Task().ID(), g08servertask);

    m_State = m_ServerTaskContext->IsAborted() ? aborted : finished;
    m_ErrList = servertrans.ErrorList();
    servertrans.ErrorList().ClearMessageList();
    
    if ( GetWaitingTask() != 0 )
        GetWaitingTask()->Resume(servertrans.Task());

    if ( SrvTasks_Trace.TracesLevel(4) )
        Kernel_VTrace() << "End: " << *this;

    m_ServerTaskContext = 0;

    vendexcl(servertrans.Task().ID(), g08servertask);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Job::WaitForResult ( SrvTasks_WaitingTask &waitingTask,
                                   Result               &jobResult,
                                   Msg_List             &jobErrList)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::WaitForResult", SrvTasks_Trace, 5);
    if ( &waitingTask != m_WaitingTask )
    {
        m_ErrList.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_WRONG_WAITING_CONTEXT));
        AppendContextInfo (__CONTEXT__, m_ErrList, "&waitingTask != m_WaitingTask");
        RTE_Crash(m_ErrList);
    }
    if ( m_State == confirmed )
        return false;
    waitingTask.Wait();
    jobResult = GetResult(jobErrList);
    return true;
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::ExecuteByServerResult
SrvTasks_Job::ExecuteByServer ( RTETask_ITask        &task,
                                Msg_List             &errlist,
                                SrvTasks_WaitingTask *waitingTask )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::ExecuteByServer", SrvTasks_Trace, 5);

    SrvTasks_RequestedTask servertask;

    switch ( SrvTasks_ICoordinator::Instance().Reserve (task, servertask, errlist, m_Description, m_ShortDescription) )
    {
        case SrvTasks_ICoordinator::reserveOK:
        {
            if ( servertask.pTask->RegisterJob (task, *this, errlist, false, waitingTask) 
                 ==
                 SrvTasks_ITask::registered )
                return executeOK;
            else
            {
                Kernel_VTrace() << "ExecuteByServer failed (servertask.pTask->RegisterJob): " << *this;
                AppendContextInfo (__CONTEXT__, errlist, "executeError");
                return executeError;
            }
        }
        case SrvTasks_ICoordinator::reserveNoServerAvailable:
        {
            Kernel_VTrace() << "ExecuteByServer failed (reserveNoServerAvailable): " << *this;
            AppendContextInfo (__CONTEXT__, errlist, "reserveNoServerAvailable");
            return executeNoServerAvailable;
        }
        case SrvTasks_ICoordinator::reserveFailed:
        {
            Kernel_VTrace() << "ExecuteByServer failed (reserveFailed): " << *this;
            AppendContextInfo (__CONTEXT__, errlist, "executeError");
            return executeError;
        }
        default:
        {
            Kernel_VTrace() << "ExecuteByServer failed: unknown result code" << *this;
            AppendContextInfo (__CONTEXT__, errlist, "executeError");
            return executeError;
        }
    }
}
//--------------------------------------------------------------------------------------
void SrvTasks_Job::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_Job::Execute", SrvTasks_Trace, 5);

    if ( GetTransContext().Task().Type() != RTETask_ITask::Server )
    {
        m_Result = error;
        m_ErrList.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_WRONG_TASK_TYPE,
                                   Msg_Arg("TYPE",SAPDB_ToString(GetTransContext().Task().Type()))));
        return;
    }
    ExecuteDirectly (GetTransContext());
}
