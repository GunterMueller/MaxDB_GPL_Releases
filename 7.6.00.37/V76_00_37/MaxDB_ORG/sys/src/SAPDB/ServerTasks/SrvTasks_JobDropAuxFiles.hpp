/*!
  @file           SrvTasks_JobDropAuxFiles.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines class SrvTasks_JobDropAuxFiles.

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
#ifndef SrvTasks_JobDropAuxFiles_HPP
#define SrvTasks_JobDropAuxFiles_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ggg00.h"  // tgg91_TransNo,tgg00_TransContext
//--------------------------------------------------------------------------------------
class SAPDBMem_IRawAllocator;
class Msg_List;
class SrvTasks_ITaskForJob;
class RTETask_ITask;
//--------------------------------------------------------------------------------------
/// This job is used to asynchronous remove file which were deleted before by a user.
class SrvTasks_JobDropAuxFiles : public SrvTasks_Job
{
public:
    /// This creates and defines a new job which can be executed by a server task.
    SrvTasks_JobDropAuxFiles(const tgg91_TransNo &transno);
    /// The job is immediately executed, false is returned on any error.
    static SrvTasks_Job::Result ExecuteByRedoTask (tgg00_TransContext &trans,
                                                   Msg_List           &errlist);
    /// used by timeout to remove aux files
    static void CheckAndStartDropServer (tsp00_TaskId taskid);
    virtual void Deallocate() {}
protected:
    virtual Result ExecuteIntern (Trans_Context &trans);
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const;
private:
    tgg91_TransNo m_TransId;
};
#endif // SrvTasks_JobDropAuxFiles_HPP
