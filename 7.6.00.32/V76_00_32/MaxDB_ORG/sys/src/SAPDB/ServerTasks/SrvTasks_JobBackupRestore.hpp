/*!
  @file           SrvTasks_JobBackupRestore.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines class SrvTasks_JobBackupRestore

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
#ifndef SrvTasks_JobBackupRestore_HPP
#define SrvTasks_JobBackupRestore_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ggg00.h"
//--------------------------------------------------------------------------------------
/// This job is used for all backup/restore tasks
class SrvTasks_JobBackupRestore : public SrvTasks_Job
{
public: // interface for users
    /*!
    @brief This creates and defines a new job which can be executed by a server task.
    @param type        [in]  what kind of execution should be done
    @param taskno      [in]  the backup taskno used by backup code
    @param waitingTask [in]  optional waiting task context
    */
    SrvTasks_JobBackupRestore
                ( SrvTasks_JobTypes      type,
                  SAPDB_UInt             taskno,
                  SrvTasks_WaitingTask * waitingTask = 0);
    virtual void Deallocate() {}
protected:
    virtual Result ExecuteIntern (Trans_Context &trans);
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const;
private:
    /// type of concrete action to be executed
    SrvTasks_JobTypes m_Type;
};
#endif // SrvTasks_JobBackupRestore_HPP
