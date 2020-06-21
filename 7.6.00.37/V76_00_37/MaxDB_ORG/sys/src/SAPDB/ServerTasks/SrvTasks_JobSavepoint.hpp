/*!
  @file           SrvTasks_JobSavepoint.hpp
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
#ifndef SrvTasks_JobSavepoint_HPP
#define SrvTasks_JobSavepoint_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ggg00.h" // tgg00_TransContext,tgg00_MessBlock
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
//--------------------------------------------------------------------------------------
/// This job is used to be the savepoint coordinator.
class SrvTasks_JobSavepoint : public SrvTasks_Job
{
public:
    /// The savepoint coordinator job is defined here
    SrvTasks_JobSavepoint(SAPDB_UInt converterversion)
    : SrvTasks_Job(jobSavepoint, Attributes(converterversion))
    {
        m_Attributes.IgnoreDBFullSecureLimit = true;
        m_Attributes.KeepServerTaskReserved  = true;
    }
    virtual void Deallocate() {}
protected:
    virtual Result ExecuteIntern (Trans_Context &trans)
    {
        Log_Savepoint::Instance().Coordinator(trans.OldTrans());
        if ( trans.OldTrans().trError_gg00 != e_ok )
        {
            AppendContextInfo(__CONTEXT__, trans.ErrorList(), "Error occured");
            Kernel_AppendBasisError (trans.OldTrans().trError_gg00,trans.ErrorList(),"Error occured");
        }
        return trans.OldTrans().trError_gg00;
    }
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const
    {
        jobCopy = new (allocator) SrvTasks_JobSavepoint(m_Attributes.Id);
        if ( 0 == jobCopy )
        {
            AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
            return false;
        }
        jobCopy->SetAttributes(m_Attributes);
        return true;
    }
};
#endif // SrvTasks_JobSavepoint_HPP
