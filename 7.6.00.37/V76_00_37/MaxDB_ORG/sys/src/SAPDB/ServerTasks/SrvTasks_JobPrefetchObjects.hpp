/*!
  @file           SrvTasks_JobPrefetchObjects.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines the class SrvTasks_JobPrefetchObjects.

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
#ifndef SrvTasks_JobPrefetchObjects_HPP
#define SrvTasks_JobPrefetchObjects_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "ggg00.h" // tgg00_TransContext,tgg00_FileId
#include "hkb53.h" // k53StartInternalSession,k53StopInternalSession
#include "hbd04.h" // bd04OmsReadAheadParticipant
//--------------------------------------------------------------------------------------
/// This job uis used to prefetch objects after restart.
class SrvTasks_JobPrefetchObjects : public SrvTasks_Job
{
public:
    /// initializes member
    SrvTasks_JobPrefetchObjects( const tgg00_FileId   &fileid,
                                 SrvTasks_WaitingTask &waitingTask )
    : SrvTasks_Job(jobPrefetchObjects,
                   Attributes(fileid.fileRoot_gg00()),
                   &waitingTask),
      m_FileId(fileid)
    {}
    virtual void Deallocate() {}
protected:
    virtual Result ExecuteIntern (Trans_Context &trans)
    {
        tgg00_TransContext auxTrans;
        k53StartInternalSession     (trans.OldTrans(), auxTrans, false, false);
        bd04OmsReadAheadParticipant (trans.OldTrans(), m_FileId);
        k53StopInternalSession      (trans.OldTrans(), auxTrans, m_commit);
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
        jobCopy = new (allocator) SrvTasks_JobPrefetchObjects(m_FileId,*m_WaitingTask);
        if ( 0 == jobCopy )
        {
            AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
            return false;
        }
        jobCopy->SetAttributes(m_Attributes);
        return true;
    }
private:
    tgg00_FileId m_FileId;
};
#endif // SrvTasks_JobPrefetchObjects_HPP
