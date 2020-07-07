/*!
  @file        SrvTasks_JobBackupRestore.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobBackupRestore
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
#include "ServerTasks/SrvTasks_JobBackupRestore.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/Messages/SDBMsg_Recovery.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Transaction/Trans_Context.hpp"
#include "heo55k.h"  // vbegexcl/vendexcl/visexcl
#include "hgg08.h"   // g08save0
#include "hkb03_1.h" // kb03Check
#include "hkb38.h"   // k38st_autosave_start
#include "hkb39.h"   // k39run_backup_task
#include "ggg00.h"   // tgg00_TransContext,tgg00_MessBlock
#include "hkb900.h"  // Pascal Interface

//--------------------------------------------------------------------------------------
SrvTasks_JobBackupRestore::SrvTasks_JobBackupRestore
                          (SrvTasks_JobTypes      type,
                           SAPDB_UInt             taskno,
                           SrvTasks_WaitingTask * waitingTask )
: SrvTasks_Job(type, Attributes(taskno), waitingTask),
  m_Type(type)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobBackupRestore::SrvTasks_JobBackupRestore", SrvTasks_Trace, 5);
	m_Attributes.KeepServerTaskReserved = true;
}
//--------------------------------------------------------------------------------------
bool SrvTasks_JobBackupRestore::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                             Msg_List               &errlist,
                                             SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobBackupRestore::CreateCopy", SrvTasks_Trace, 5);
    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    jobCopy = new (allocator) SrvTasks_JobBackupRestore(m_Type,m_Attributes.Id,m_WaitingTask);
    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }
    jobCopy->SetAttributes(m_Attributes);
    return true;
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::Result
SrvTasks_JobBackupRestore::ExecuteIntern(Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobBackupRestore::ExecuteIntern", SrvTasks_Trace, 5);

    tgg00_BasisError &trError = trans.OldTrans().trError_gg00;

    if ( m_Type == jobAutosaveCoordinator )
        k38st_autosave_start (trans.OldTrans());
    else
        k39run_backup_task (trans.OldTrans(), (m_Type == jobAutosaveReader ? 1 : 0), 0 );

    m_Result = trError;
    
    if ( trError != e_ok
         &&
         trError != e_cancelled
         &&
         trError != e_write_task_crash )
    {
        AppendContextInfo(__CONTEXT__, trans.ErrorList(), "Error in backup task occured");
        Kernel_AppendBasisError (trError,trans.ErrorList(),"Error in backup task occured");
    }
    return m_Result;
}
/// This indicates if a servertask should be released at the end or if it should be kept
#define FOR_SYSTEM_USAGE true
/// This class defines a coordinator for the pascal environment to start the jobs.
class SrvTasks_BackupServer
{
public:
    /// initializes all members
    SrvTasks_BackupServer (SAPDBMem_IRawAllocator &allocator)
    : m_WaitingTask            (*RTETask_ITask::Self()),
      m_Jobs                   (allocator),
      m_ServerTasks            (allocator)
    {}
    /// reserves the needed servertasks
    bool Initialize(RTETask_ITask &task,
                    SAPDB_UInt     maxMedias,
                    SAPDB_UInt     maxVolumes,
                    Msg_List      &errlist)
    {
        SAPDBTRACE_METHOD_DEBUG ("SrvTasks_BackupServer::Initialize", SrvTasks_Trace, 5);

        SrvTasks_ICoordinator &stc = SrvTasks_ICoordinator::Instance();
        
        if ( ! m_ServerTasks.Initialize(maxMedias + maxVolumes) )
            return false;
        
        for ( SAPDB_UInt reservedTaskNo = 0; reservedTaskNo < m_ServerTasks.GetSize(); ++reservedTaskNo )
        {
            if ( stc.Reserve (task, m_ServerTasks[reservedTaskNo], errlist, "Backup or restore", "Backup ", FOR_SYSTEM_USAGE)
                 !=
                 SrvTasks_ICoordinator::reserveOK )
            {
                return false;
            }
        }
        RTE_Message(Msg_List(Msg_List::Info,SDBMSG_SRVTASKS_SERVERTASKS_PERMANENTLY_RESERVED,
                             Msg_Arg("TASKS_WANTED",SAPDB_ToString(m_ServerTasks.GetSize())),
                             Msg_Arg("DESCRIPTION", "Backup / Restore")));
        return true;
    }
    /// Execute a backup task
    bool Execute (RTETask_ITask &task,
                  Msg_List      &errlist,
                  bool           forAutosave,
                  SAPDB_Int4     region,
                  SAPDB_Int2     taskNo,
                  SAPDB_Int4     mediaNo,
                  SAPDB_Int4     volumeNo)
    {
        SAPDBTRACE_METHOD_DEBUG ("SrvTasks_BackupServer::Execute", SrvTasks_Trace, 5);

        if ( kb03Check.chkRegion_kb00 )
            g08excl_check (task.ID(), g08save0 + region);
        
        SrvTasks_JobTypes                 jobtype;
        SrvTasks_ITask::RegisterJobResult result;
        

        if ( taskNo < 1  || static_cast<SAPDB_UInt2>(taskNo) > m_ServerTasks.GetSize() )
        {
            errlist.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_ILLEGAL_TASKNO,
                                              Msg_Arg("JOB_ID",SAPDB_ToString(taskNo))));
            return false;
        }

        if ( mediaNo > 0 && volumeNo > 0 )
        {
            errlist.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_EITHER_MEDIA_OR_VOLUME,
                                              Msg_Arg("MEDIA_NO",SAPDB_ToString(mediaNo)),
                                              Msg_Arg("VOLUME_NO",SAPDB_ToString(volumeNo))));
            return false;
        }

        if ( forAutosave )
            jobtype = mediaNo > 0 ? jobAutosaveCoordinator : jobAutosaveReader;
        else
            jobtype = mediaNo > 0 ? jobBackupMediumTask : jobBackupVolumeTask;

        if ( jobtype == jobAutosaveCoordinator )
        {
            SrvTasks_JobBackupRestore newJob(jobtype, taskNo);
            vendexcl(task.ID(), g08save0 + region);
            result = m_ServerTasks[taskNo-1].pTask->RegisterJob ( task, newJob, errlist, true);
            vbegexcl(task.ID(), g08save0 + region);
        }
        else
        {
            SrvTasks_JobBackupRestore *pNewJob = 0;
            m_WaitingTask.AssignNewTask(task);// should always assign the same

            pNewJob = new (m_Jobs.GetAllocator()) SrvTasks_JobBackupRestore (jobtype, taskNo, &m_WaitingTask);
            if ( pNewJob == 0 )
            {
                errlist.AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_ALLOCATION_FAILED,
                                                  Msg_Arg("DESCRIPTION","new SrvTasks_JobBackupRestore")));
                return false;
            }
            if( ! m_Jobs.InsertEnd(pNewJob) )
            {
                errlist = Msg_List(Msg_List::Error,SDBMSG_RECOVERY_ALLOCATION_FAILED,
                                   Msg_Arg("DESCRIPTION","m_Jobs.InsertEnd(pNewJob)"));
                pNewJob->AppendContextInfo (__CONTEXT__, errlist, "m_Jobs.InsertEnd(pNewJob) failed");
                return false;
            }
            vendexcl(task.ID(), g08save0 + region);
            result = m_ServerTasks[taskNo-1].pTask->RegisterJob ( task, *pNewJob, errlist, true, &m_WaitingTask );
            vbegexcl(task.ID(), g08save0 + region);
        }

        if ( result != SrvTasks_ITask::registered )
        {
            errlist.AddDetail (SDBMSG_SRVTASKS_BACKUP_JOB_INFO,
                               Msg_Arg("IS_AUTOSAVE",     forAutosave?"True":"False" ),
                               Msg_Arg("BACKUP_TASK_NO",  SAPDB_ToString(taskNo)     ),
                               Msg_Arg("BACKUP_MEDIA_NO", SAPDB_ToString(mediaNo)    ),
                               Msg_Arg("BACKUP_VOLUME_NO",SAPDB_ToString(volumeNo)   ) );
            return false;
        }
        return true;
    }
    /// wait for any finished backup task
    bool WaitForAnyJobFinished ( RTETask_ITask    &task,
                                 Msg_List         &errlist,
                                 SAPDB_Int4        region,
                                 SAPDB_Int2       &taskno,
                                 tgg00_BasisError &joberror )
	{
        SAPDBTRACE_METHOD_DEBUG ("SrvTasks_BackupServer::WaitForAnyJobFinished", SrvTasks_Trace, 5);

        SrvTasks_JobList<SrvTasks_JobBackupRestore>::Iterator jobiter;
        bool                                                  result;
        
        vendexcl(task.ID(), g08save0 + region);

        result = m_Jobs.WaitForAnyJobFinished (m_WaitingTask, jobiter, true);
        
        vbegexcl(task.ID(), g08save0 + region);

        if ( result )
        {
            SrvTasks_Job::Result    jobResult;
            taskno    = (*jobiter)->GetId();
            jobResult = (*jobiter)->GetResult(errlist);
    
            joberror = jobResult;
        
            if ( joberror == e_ok
                 &&
                 (*jobiter)->GetState() == SrvTasks_Job::aborted )
            {
                joberror = e_cancelled;
            }

            m_Jobs.RemoveJob(jobiter);

            return true;
        }
        else
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_RECOVERY_NO_JOB_FOUND_IN_WAITLIST);
            taskno   = 0;
            joberror = e_ok;
            return false;
        }
    }
    /// deallocate all known jobs
    void Delete()
    {
        m_Jobs.Delete();
    }    
private:
    SrvTasks_WaitingTask                         m_WaitingTask;
    SrvTasks_JobList<SrvTasks_JobBackupRestore>  m_Jobs;
	Container_Vector<SrvTasks_RequestedTask>     m_ServerTasks;
};
//--------------------------------------------------------------------------------------
/// wrapper to create pascal handler
externPascal pasbool kb900CreateBackupServer (
    void* &                              pSrvTasks_BackupServer,
    pasbool                              forAutosave,
    tsp00_Int4                           maxMedias,
    tsp00_Int4                           maxVolumes,
    tsp00_TaskId                         taskid,
    tgg00_VoidPtr                        errorlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900CreateBackupServer", SrvTasks_Trace, 5);

    if ( pSrvTasks_BackupServer != 0 )
        return false;
        
    RTETask_ITask         &task = *RTETask_ITask::ByTaskID(taskid);
    Msg_List               localErrlist;
    Msg_List              *auxErrlist = errorlist != 0 ? reinterpret_cast<Msg_List*>(errorlist)
                                                       : &localErrlist;

    pSrvTasks_BackupServer = new(RTEMem_Allocator::Instance())
                                SrvTasks_BackupServer(RTEMem_Allocator::Instance());
    
    if ( pSrvTasks_BackupServer == 0 )
    {
        auxErrlist->AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_ALLOCATION_FAILED,
                                              Msg_Arg("DESCRIPTION","new SrvTasks_BackupServer")));
        if ( ! localErrlist.IsEmpty() )
            RTE_Message(localErrlist);
        return false;
    }

    SrvTasks_BackupServer &bs = *(reinterpret_cast<SrvTasks_BackupServer*>(pSrvTasks_BackupServer));

    if ( ! bs.Initialize(task, maxMedias, maxVolumes, *auxErrlist) )
    {
        if ( ! localErrlist.IsEmpty() )
            RTE_Message(localErrlist);
        return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------
externPascal void kb900ExecuteBackupJob (
    Trans_Context&                       trans,
    void*                                pSrvTasks_BackupServer,
    tsp00_Int4                           region,
    pasbool                              forAutosave,
    tsp00_Int2                           task_no,
    tsp00_Int4                           media_no,
    tsp00_Int4                           vol_no)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900ExecuteBackupJob", SrvTasks_Trace, 5);
    if ( pSrvTasks_BackupServer == 0 )
    {
        trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_BACKUP_SERVER_NOT_CREATED));
        trans.OldTrans().trError_gg00 = e_internal_error;
    }
    SrvTasks_BackupServer &bs = *(reinterpret_cast<SrvTasks_BackupServer*>(pSrvTasks_BackupServer));
    if ( ! bs.Execute (trans.Task(),trans.ErrorList(),forAutosave!=0,region,task_no, media_no, vol_no) )
        trans.OldTrans().trError_gg00 = e_start_server_task_error;
}
//--------------------------------------------------------------------------------------
/// wrapper to wait for any backup job
externPascal void kb900WaitForAnyBackupJobFinished (
    Trans_Context&                       trans,
    void*                                pSrvTasks_BackupServer,
    tsp00_Int4                           region,
    tsp00_Int2            VAR_VALUE_REF  task_no,
    tgg00_BasisError      VAR_VALUE_REF  joberror)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900WaitForAnyBackupJobFinished", SrvTasks_Trace, 5);
    if ( pSrvTasks_BackupServer == 0 )
    {
        trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_RECOVERY_BACKUP_SERVER_NOT_CREATED));
        trans.OldTrans().trError_gg00 = e_internal_error;
    }
    SrvTasks_BackupServer &bs = *(reinterpret_cast<SrvTasks_BackupServer*>(pSrvTasks_BackupServer));
    if ( ! bs.WaitForAnyJobFinished (trans.Task(), trans.ErrorList(), region, task_no, joberror) )
        trans.OldTrans().trError_gg00 = e_no_more_jobs_to_wait_for;
	else
        if ( e_cancelled == joberror ) 
            trans.OldTrans().trError_gg00 = joberror;
}
