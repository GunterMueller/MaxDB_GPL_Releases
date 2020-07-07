/*!
  @file        SrvTasks_JobDropVolume.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobDropVolume
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
#include "ServerTasks/SrvTasks_JobDropVolume.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Transaction/Trans_Context.hpp"
#include "IOManager/IOMan_IDataManager.hpp"


//--------------------------------------------------------------------------------------

SrvTasks_JobDropVolume::SrvTasks_JobDropVolume(SrvTasks_WaitingTask &waitingTask,
                            Converter_PageNoIterator &pageIter):
        SrvTasks_Job(jobDropVolume, Attributes(), &waitingTask),
        m_pageIter(pageIter)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropVolume::SrvTasks_JobDropVolume", SrvTasks_Trace, 5);
}

//--------------------------------------------------------------------------------------

bool SrvTasks_JobDropVolume::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                          Msg_List               &errlist,
                                          SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropVolume::CreateCopy", SrvTasks_Trace, 5);

    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    jobCopy = new (allocator) SrvTasks_JobDropVolume(*m_WaitingTask, m_pageIter);

    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }

    jobCopy->SetAttributes(m_Attributes);

    return true;
}

//--------------------------------------------------------------------------------------

SrvTasks_Job::Result SrvTasks_JobDropVolume::ExecuteIntern(Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropVolume::ExecuteIntern", SrvTasks_Trace, 5);
    return IOMan_IDataManager::GetInstance().SrvTaskDropVolume( trans.Task(), trans.ErrorList(), m_pageIter)
           ? ok : error;
}
