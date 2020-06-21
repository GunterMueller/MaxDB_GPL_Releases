/*!
  @file        SrvTasks_JobCheckData.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobCheckData
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
#include "ServerTasks/SrvTasks_JobCheckData.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Transaction/Trans_Context.hpp"
#include "hbd01.h"   // b01pverify_participant
#include "heo56.h"   // vsleep
//--------------------------------------------------------------------------------------
SrvTasks_JobCheckData::SrvTasks_JobCheckData(SrvTasks_WaitingTask &waitingTask,
                                             const tgg00_FileId   &fileid,
                                             bool                  withUpdate,
                                             bool                  extended)
: SrvTasks_Job(jobCheckData,
               SrvTasks_Job::Attributes(),
               &waitingTask),
  m_FileId     (fileid),
  m_WithUpdate (withUpdate),
  m_Extended   (extended)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCheckData::SrvTasks_JobCheckData", SrvTasks_Trace, 5);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_JobCheckData::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                          Msg_List               &errlist,
                                          SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCheckData::CreateCopy", SrvTasks_Trace, 5);
    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    jobCopy = new (allocator) SrvTasks_JobCheckData(*m_WaitingTask,m_FileId,m_WithUpdate,m_Extended);
    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }
    jobCopy->SetAttributes(m_Attributes);
    return true;
}
//--------------------------------------------------------------------------------------
void SrvTasks_JobCheckData::AppendContextInfo (SAPDB_Char  const * const  filename,
                                               SAPDB_UInt4 const          linenumber,
                                               Msg_List                  &errlist,
                                               SAPDB_Char  const * const  title) const
{
    SrvTasks_Job::AppendContextInfo(filename,linenumber,errlist,title);
	/// m_FileId,m_WithUpdate,m_Extended);
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::Result
SrvTasks_JobCheckData::ExecuteIntern (Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCheckData::ExecuteIntern", SrvTasks_Trace, 5);
    tgg00_BasisError   &trError = trans.OldTrans().trError_gg00;

    b01pverify_participant (trans.OldTrans(), m_FileId, m_WithUpdate?1:0, m_Extended?1:0);
    if ( trError != e_ok )
    {
        AppendContextInfo      (__CONTEXT__, trans.ErrorList(), "b01pverify_participant() failed");
        Kernel_AppendBasisError(trError, trans.ErrorList(), "b01pverify_participant() failed");
        m_Result = trError;
    }
    return m_Result;
}
//--------------------------------------------------------------------------------------
/// This class defines a coordinator for the pascal environment to start the jobs.
class SrvTasks_CheckDataCoordinator
{
public:
    /// initializes all members
    SrvTasks_CheckDataCoordinator(SAPDBMem_IRawAllocator &allocator)
    : m_Allocator  (allocator),
      m_WaitingTask(*RTETask_ITask::Self()),
      m_Jobs       (allocator)
    {}
    /// result of ExecuteJob()
    enum ExecuteJobResult
    {
        executeOk,
        executeNoMemory,
        executeFailed,
        executeNoTasks
    };
    /// This creates and executes one job
    ExecuteJobResult ExecuteJob ( const tgg00_FileId &fileid,
                                  const bool          withUpdate,
                                  const bool          extended,
                                        Msg_List     &errlist )
    {
        SrvTasks_JobCheckData* pNewJob = new(m_Allocator) SrvTasks_JobCheckData(m_WaitingTask, fileid, withUpdate, extended);
        if ( pNewJob == 0 )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
            return executeNoMemory;
        }
        
        switch ( pNewJob->ExecuteByServer(m_WaitingTask.Task(), errlist, &m_WaitingTask) )
        {
            case SrvTasks_Job::executeOK:
                if ( m_Jobs.InsertEnd(pNewJob) )
                    return executeOk;
                else
                {
                    destroy(pNewJob,m_Allocator);
                    errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
                    pNewJob->AppendContextInfo(__CONTEXT__,errlist,"m_Jobs.InsertEnd(pNewJob) failed");
                    return executeNoMemory;
                }
            case SrvTasks_Job::executeNoServerAvailable:
                destroy(pNewJob,m_Allocator);
                errlist.ClearMessageList();
                return executeNoTasks;
            case SrvTasks_Job::executeError:
                // go through
            default:
                destroy(pNewJob,m_Allocator);
                return executeFailed;
        }
    }
    /// The caller waits for the completion of any started job
    bool WaitForAnyJobFinished(tsp00_PageNo         &errorRoot,
                               SrvTasks_Job::Result &result,
                               Msg_List             &errlist)
    {
        SrvTasks_JobList<SrvTasks_JobCheckData>::Iterator  jobiter;

        result = SrvTasks_Job::ok;
    
        if ( ! m_Jobs.WaitForAnyJobFinished(m_WaitingTask, jobiter) )
            return false; // ++++++++ this is not allowed

        result = (*jobiter)->GetResult(errlist);
        
        if ( result != e_ok )
            errorRoot = (*jobiter)->GetRoot();
        else
            errorRoot = NIL_PAGE_NO_GG00;

        destroy(*jobiter, m_Jobs.GetAllocator());
        m_Jobs.RemoveJob(jobiter);

        return true;
    }
    /// abort and wait for all started tasks
    void Abort()
    {
        m_Jobs.AbortAll(m_WaitingTask);
    }
    /// Destroy all started jobs (deallocate)
    void Delete()
    {
        m_Jobs.Delete();
    }
    /// this calls Delete to remove all resources
    ~SrvTasks_CheckDataCoordinator()
    {
        Delete();
    }
private:
    SAPDBMem_IRawAllocator                  &m_Allocator;
    SrvTasks_WaitingTask                     m_WaitingTask;
    SrvTasks_JobList<SrvTasks_JobCheckData>  m_Jobs;
};
//--------------------------------------------------------------------------------------
/// allocate the pascal handler
externPascal void kb900CreateCheckDataCoordinator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  pCheckDataCoordinator)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900CreateCheckDataCoordinator", SrvTasks_Trace, 5);
    SrvTasks_CheckDataCoordinator *pCoordinator = new(Trans_Context::GetContext(trans).Allocator())
                                                  SrvTasks_CheckDataCoordinator(Trans_Context::GetContext(trans).Allocator());
    if ( pCoordinator == 0 )
    {
        trans.trError_gg00 = e_no_more_memory;
        return;
    }
    pCheckDataCoordinator = reinterpret_cast<tsp00_Addr>(pCoordinator);
}
//--------------------------------------------------------------------------------------
/// wrapper to start a job
externPascal void kb900ExecuteCheckDataJob (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr                           pCheckDataCoordinator,
    tgg00_FileId          VAR_VALUE_REF  fileid,
    pasbool                              bWithUpdate,
    pasbool                              bWithExtendedCheck)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900ExecuteCheckDataJob", SrvTasks_Trace, 5);
    SrvTasks_CheckDataCoordinator *pCoordinator = reinterpret_cast<SrvTasks_CheckDataCoordinator*>(pCheckDataCoordinator);
    switch ( pCoordinator->ExecuteJob ( fileid, bWithUpdate!=0, bWithExtendedCheck!=0, Trans_Context::GetContext(trans).ErrorList() ) )
    {
        case SrvTasks_CheckDataCoordinator::executeOk:
            trans.trError_gg00 = e_ok;
            break;
        case SrvTasks_CheckDataCoordinator::executeNoTasks:
            trans.trError_gg00 = e_no_server_task_available;
            break;
        case SrvTasks_CheckDataCoordinator::executeNoMemory:
            // go through
        case SrvTasks_CheckDataCoordinator::executeFailed:
            // go through
        default:
            trans.trError_gg00 = e_start_server_task_error;
            break;
    }
}
//--------------------------------------------------------------------------------------
/// wrapper to wait for any finished job
externPascal void kb900WaitForAnyCheckDataJob (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr                           pCheckDataCoordinator,
    tgg00_BasisError      VAR_VALUE_REF  joberror,
    tsp00_PageNo          VAR_VALUE_REF  errorRoot)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900WaitForAllCheckDataJob", SrvTasks_Trace, 5);
    SrvTasks_CheckDataCoordinator *pCoordinator = reinterpret_cast<SrvTasks_CheckDataCoordinator*>(pCheckDataCoordinator);
	SrvTasks_Job::Result           result;

    trans.trError_gg00 = e_ok;
    
    if ( ! pCoordinator->WaitForAnyJobFinished(errorRoot,result,Trans_Context::GetContext(trans).ErrorList()) )
        trans.trError_gg00 = e_no_more_jobs_to_wait_for;

    joberror = result;
}
//--------------------------------------------------------------------------------------
/// wrapper to abort all running job and to wait for the end
externPascal void kb900AbortCheckDataJobs (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr                           pCheckDataCoordinator)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900AbortCheckDataJobs", SrvTasks_Trace, 5);
    SrvTasks_CheckDataCoordinator *pCoordinator = reinterpret_cast<SrvTasks_CheckDataCoordinator*>(pCheckDataCoordinator);
    pCoordinator->Abort();
}
//--------------------------------------------------------------------------------------
/// wrapper to deallocate the coordinator
externPascal void kb900DeleteCheckDataCoordinator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  pCheckDataCoordinator)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900DeleteCheckDataCoordinator", SrvTasks_Trace, 5);
    if ( pCheckDataCoordinator == 0 )
        return;
    SAPDBMem_IRawAllocator        &allocator    = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    SrvTasks_CheckDataCoordinator *pCoordinator = reinterpret_cast<SrvTasks_CheckDataCoordinator*>(pCheckDataCoordinator);
    destroy(pCoordinator, allocator);
}
