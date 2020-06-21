/*!
  @file        SrvTasks_JobCollection.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobCollection
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
#include "ServerTasks/SrvTasks_JobCollection.hpp"
#include "ServerTasks/SrvTasks_JobPrefetchObjects.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Transaction/Trans_Context.hpp"
#include "hbd01.h" // b01niltree_id, bd01UsableServerForParallelExecution
#include "hbd02.h" // b02prefetch, b02find_leaf
#include "hbd91.h" // bd91FDirNextObjFileId
#include "hbd05.h" // bd05PrefetchLob
#include "hkb53.h" // k53StartInternalSession,k53StopInternalSession
#include "ggg00.h" // tgg00_TransContext

//--------------------------------------------------------------------------------------
void SrvTasks_JobCollection::InitMember()
{
    m_FileId            = b01niltree_id;
	m_LobPosition       = 0;
    m_LobBytesToRead    = 0;
	m_PageNo            = NIL_PAGE_NO_GG00;
    m_Key.keyLen_gg00() = 0;

}
//--------------------------------------------------------------------------------------
SrvTasks_JobCollection::SrvTasks_JobCollection ()
: SrvTasks_Job(jobPrefetchObjetcsCoordinator, SrvTasks_Job::Attributes())
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::SrvTasks_JobCollection.prefetchObjetcsCoordinator", SrvTasks_Trace, 5);
    InitMember();
}
//--------------------------------------------------------------------------------------
SrvTasks_JobCollection::SrvTasks_JobCollection (const tgg00_FileId &lobFileId,
                                                const SAPDB_UInt    lobPosition,
                                                const SAPDB_UInt    lobBytesToRead )
: SrvTasks_Job(jobPrefetchBlob, SrvTasks_Job::Attributes())
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::SrvTasks_JobCollection.prefetchBlob", SrvTasks_Trace, 5);
    InitMember();
    m_FileId         = lobFileId;
    m_LobPosition    = lobPosition;
    m_LobBytesToRead = lobBytesToRead;
}
//--------------------------------------------------------------------------------------
SrvTasks_JobCollection::SrvTasks_JobCollection (const tgg00_FileId &fileid,
                                                      tsp00_PageNo  pageno)
: SrvTasks_Job(jobPrefetchPages, SrvTasks_Job::Attributes())
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::SrvTasks_JobCollection.prefetchPages", SrvTasks_Trace, 5);
    InitMember();
    m_FileId = fileid;
    m_PageNo = pageno;
}
//--------------------------------------------------------------------------------------
SrvTasks_JobCollection::SrvTasks_JobCollection (const tgg00_FileId &fileid,
                                                const tgg00_Lkey   &key)
: SrvTasks_Job(jobPrefetchSelect, SrvTasks_Job::Attributes())
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::SrvTasks_JobCollection.prefetchSelect", SrvTasks_Trace, 5);
    InitMember();
    m_FileId = fileid;
    m_Key    = key;
}
//--------------------------------------------------------------------------------------
bool SrvTasks_JobCollection::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                          Msg_List               &errlist,
                                          SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::CreateCopy", SrvTasks_Trace, 5);
    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    switch(m_JobType)
    {
        case jobPrefetchObjetcsCoordinator:
            jobCopy = new (allocator) SrvTasks_JobCollection();
            break;
        case jobPrefetchBlob:
            jobCopy = new (allocator) SrvTasks_JobCollection(m_FileId, m_LobPosition, m_LobBytesToRead );
            break;
        case jobPrefetchPages:
            jobCopy = new (allocator) SrvTasks_JobCollection(m_FileId,m_PageNo);
            break;
        case jobPrefetchSelect:
            jobCopy = new (allocator) SrvTasks_JobCollection(m_FileId,m_Key);
            break;
        default:
            AppendContextInfo (__CONTEXT__, errlist, "Wrong Job Type for this job handler");
            return false;
    }
    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }
    jobCopy->SetAttributes(m_Attributes);
    return true;
}
//--------------------------------------------------------------------------------------
SrvTasks_JobCollection::Result
SrvTasks_JobCollection::ExecuteIntern (Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::Execute", SrvTasks_Trace, 5);

    switch ( m_JobType )
    {
        case jobPrefetchObjetcsCoordinator:
        {
            OmsReadAheadCoordinator();
            break;
        }
        case jobPrefetchBlob:
        {
            bd05PrefetchLob (trans.OldTrans(), m_FileId, m_LobPosition, m_LobBytesToRead );
            break;
        }
        case jobPrefetchPages:
        {
            b02prefetch (trans.OldTrans(), m_FileId, m_PageNo);
            break;
        }
        case jobPrefetchSelect:
        {
            b02find_leaf (trans.OldTrans(), m_FileId, m_Key);
            break;
        }
        default:
        {
            m_Result = error;
            AppendContextInfo(__CONTEXT__, trans.ErrorList(),
                              "ServerTask Type not supported for execution in this context");
        }
    }
    if ( trans.OldTrans().trError_gg00 != e_ok )
    {
        AppendContextInfo(__CONTEXT__, trans.ErrorList(), "Error occured");
        Kernel_AppendBasisError (trans.OldTrans().trError_gg00,trans.ErrorList(),"Error occured");
        m_Result = trans.OldTrans().trError_gg00;
    }
    return m_Result;
}
//--------------------------------------------------------------------------------------
void SrvTasks_JobCollection::OmsReadAheadCoordinator ()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobCollection::OmsReadAheadCoordinator", SrvTasks_Trace, 5);

    Trans_Context      &context = m_ServerTaskContext->GetContext();
    tgg00_TransContext  auxTrans;

    k53StartInternalSession (context.OldTrans(), auxTrans, false, false);

    tgg00_FileId      fileid;
    tsp00_PageNo      errorRoot    = NIL_PAGE_NO_GG00;
    FileDir_Iterator *pObjFileIter = 0;

    fileid = b01niltree_id;
    fileid.fileTfn_gg00().becomes(tfnObj_egg00);

    const SAPDB_UInt wantedServerTasks = bd01UsableServerForParallelExecution();
    const SAPDB_UInt MaxRetryCount = 10;
    SAPDB_UInt       startedServerTasks = 0;
    SAPDB_UInt       retryCount = 0;
    bool             serverTaskStarted = false;

    SrvTasks_WaitingTask                          waitingTask (context.Task());
    SrvTasks_JobList<SrvTasks_JobPrefetchObjects> jobs        (context.Allocator());
    
    SrvTasks_JobPrefetchObjects *pNewJob = 0;
    
    while ( m_Result == SrvTasks_Job::ok )
    {
        bd91FDirNextObjFileId (context.OldTrans(), pObjFileIter, fileid);
        
        tgg00_BasisError &trError = context.OldTrans().trError_gg00;
        
        if ( trError == e_file_not_found )
        {
            // normal end found
            trError = e_ok;
            break;
        }
        if ( trError != e_ok )
        {
            trError = e_ok;
            m_Result = error;
            Kernel_AppendBasisError(trError, context.ErrorList());
            AppendContextInfo (__CONTEXT__, context.ErrorList(), "bd91FDirNextObjFileId failed");
            break;
        }
        if ( 0 == retryCount && 0 == pNewJob )
        {
            pNewJob = new (context.Allocator()) SrvTasks_JobPrefetchObjects( fileid, waitingTask );
            if ( pNewJob == 0 )
            {
                m_Result = error;
                AppendContextInfo (__CONTEXT__, context.ErrorList(), "jobs.Initialize(wantedServerTasks) failed");
                break;
            }
        }

        serverTaskStarted = pNewJob->ExecuteByServer(context.Task(), context.ErrorList())
                            ==
                            SrvTasks_Job::executeOK;

        if ( serverTaskStarted )
        {
            retryCount = 0;
            ++startedServerTasks;
            if ( ! jobs.InsertEnd(pNewJob) )
            {
                m_Result = error;
                AppendContextInfo (__CONTEXT__, context.ErrorList(), "jobs.InsertEnd(pNewJob) failed");
                break;
            }
            pNewJob = 0;
        }
        else
        {
            RTE_Message(context.ErrorList());
            context.ErrorList().ClearMessageList();
            ++retryCount;
        }

        if ( startedServerTasks == wantedServerTasks
             ||
             (! serverTaskStarted && startedServerTasks > 0) )
        {
            bool isAborted = false;
            jobs.WaitForAll (waitingTask, isAborted, m_Result, context.ErrorList());
            jobs.RemoveAllJobs();
            startedServerTasks = 0;
        }
        
        if ( ! serverTaskStarted
             &&
             startedServerTasks == 0
             &&
             retryCount >= MaxRetryCount )
        {
            retryCount = 0;
            pNewJob->ExecuteDirectly(context);
        }
        
        if ( m_ServerTaskContext->IsAborted() )
            break;
    }//endwhile
    
    if ( startedServerTasks > 0 )
    {
        bool isAborted = false;
        jobs.WaitForAll (waitingTask, isAborted,m_Result,context.ErrorList());
        jobs.RemoveAllJobs();
    }

    k53StopInternalSession  (context.OldTrans(), auxTrans, m_commit);
}
//--------------------------------------------------------------------------------------
/// wrapper to start a prefetch job
externPascal void kb900PrefetchLob (
    Trans_Context         VAR_VALUE_REF  context,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    tsp00_Int4                           lobPosition,
    tsp00_Int4                           lobBytesToRead )
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900PrefetchLob", SrvTasks_Trace, 5);
    const SAPDB_UInt    auxLobPosition = lobPosition;
    SrvTasks_JobCollection job (fileId, auxLobPosition, lobBytesToRead );
    switch ( job.ExecuteByServer(context.Task(), context.ErrorList()) )
    {
        case SrvTasks_Job::executeOK:
            break;
        case SrvTasks_Job::executeNoServerAvailable:
            context.ErrorList().ClearMessageList();
            break;
        case SrvTasks_Job::executeError:
            context.OldTrans().trError_gg00 = e_start_server_task_error;
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------------------------------
/// wrapper to start a prefetch job
externPascal void kb900PrefetchLeaves (
    Trans_Context         VAR_VALUE_REF  context,
    tgg00_FileId          VAR_VALUE_REF  fileid,
    tsp00_PageNo                         pno)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900PrefetchLeaves", SrvTasks_Trace, 5);
    SrvTasks_JobCollection job (fileid, pno);
    switch ( job.ExecuteByServer(context.Task(), context.ErrorList()) )
    {
        case SrvTasks_Job::executeOK:
            break;
        case SrvTasks_Job::executeNoServerAvailable:
            context.ErrorList().ClearMessageList();
            break;
        case SrvTasks_Job::executeError:
            context.OldTrans().trError_gg00 = e_start_server_task_error;
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------------------------------
/// wrapper to start a prefetch job
externPascal void kb900PrefetchJoin (
    Trans_Context         VAR_VALUE_REF  context,
    tgg00_FileId          VAR_VALUE_REF  fileid,
    tgg00_Lkey            VAR_VALUE_REF  key)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900PrefetchJoin", SrvTasks_Trace, 5);
    SrvTasks_JobCollection job (fileid, key);
    switch ( job.ExecuteByServer(context.Task(), context.ErrorList()) )
    {
        case SrvTasks_Job::executeOK:
            break;
        case SrvTasks_Job::executeNoServerAvailable:
            context.ErrorList().ClearMessageList();
            break;
        case SrvTasks_Job::executeError:
            context.OldTrans().trError_gg00 = e_start_server_task_error;
            break;
        default:
            break;
    }
}