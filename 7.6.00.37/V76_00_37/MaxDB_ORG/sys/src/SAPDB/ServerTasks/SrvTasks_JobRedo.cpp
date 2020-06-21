/*!
  @file        SrvTasks_JobRedo.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobRedo
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
#include "ServerTasks/SrvTasks_JobRedo.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "ggg00.h" // tgg00_TransContext,tgg00_MessBlock
#include "Restart/Rst_RedoManager.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"

//--------------------------------------------------------------------------------------
SrvTasks_JobRedo::SrvTasks_JobRedo
                          (      SrvTasks_JobTypes      type,
                           const Attributes             attributes,
                                 SrvTasks_WaitingTask * waitingTask )
: SrvTasks_Job(type, attributes, waitingTask)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobRedo::SrvTasks_JobRedo", SrvTasks_Trace, 5);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_JobRedo::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                          Msg_List               &errlist,
                                          SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobRedo::CreateCopy", SrvTasks_Trace, 5);
    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    jobCopy = new (allocator) SrvTasks_JobRedo(m_JobType,m_Attributes,m_WaitingTask);
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
SrvTasks_JobRedo::ExecuteIntern (Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobRedo::ExecuteIntern", SrvTasks_Trace, 5);

    Msg_List          &errlist     = trans.ErrorList();
    tgg00_BasisError  &trError     = trans.OldTrans().trError_gg00;
    Rst_RedoManager   &redoManager = Kernel_IAdminRestartShutdown::Instance().GetRedoManager();

    switch ( m_JobType )
    {
        case jobRedoLogReader:
        {
            if ( redoManager.ReadLogAndCreateRedoFiles (trans.Task().ID(),trans.OldTrans(),errlist) )
            {
                if ( trError != e_ok )
                {
                    if (e_cancelled != trError) // PTS 1132981 mb 2004-12-15
                    {
                        Kernel_AppendBasisError (trError,errlist,"Result is OK but trError is not.");
                        AppendContextInfo(__CONTEXT__, errlist, "Result is OK but trError is not.");
                        m_Result = error;
                    }
                    redoManager.Abort (trans.Task().ID());
                }
            }
            else
            {
                m_Result = error;
                redoManager.Abort (trans.Task().ID());
            }
            break;
        }
        case jobRedoLogExecutor:
        {
            if ( redoManager.GetRedoTrafficControl() != 0
                 &&
                 redoManager.GetRedoTrafficControl()->ExecuteJobs (trans.Task().ID(),trans.OldTrans(),errlist) )
            {
                if ( trError != e_ok )
                {
                    Kernel_AppendBasisError (trError,errlist,"Result is OK but trError is not.");
                    AppendContextInfo(__CONTEXT__, errlist, "Result is OK but trError is not.");
                    m_Result = error;
                    redoManager.Abort (trans.Task().ID());
                }
            }
            else
            {
                m_Result = error;
                redoManager.Abort (trans.Task().ID());
            }
            break;
        }
        case jobStandbyRestartCoordinator:
        {
            if ( Kernel_IAdminHotStandby::Instance().RestartStandbyServer(trans.Task().ID(),trans.OldTrans(),errlist) )
            {
                if ( trError != e_ok )
                {
                    Kernel_AppendBasisError (trError,errlist,"Result is OK but trError is not.");
                    AppendContextInfo(__CONTEXT__, errlist, "Result is OK but trError is not.");
                    m_Result = error;
                    Kernel_IAdminRestartShutdown::Instance().Offline(trError);
                }
            }
            else
            {
                m_Result = error;
                Kernel_IAdminRestartShutdown::Instance().Offline(errlist);
            }
            break;
        }
        case jobStandbySynchronizeServer:
        {
            Kernel_IAdminHotStandby::Instance().SynchronizeServer(trans.Task().ID(), trans.Allocator());
            break;
        }
        default:
        {
            m_Result = error;
            AppendContextInfo(__CONTEXT__, errlist, "ServerTask Type not supported for execution");
        }
    }
    return m_Result;
}
