/*!
  @file           SrvTasks_JobCheckData.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines class SrvTasks_JobCheckData

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
#ifndef SrvTasks_JobCheckData_HPP
#define SrvTasks_JobCheckData_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ggg00.h"
//--------------------------------------------------------------------------------------
/// This job is used for asynchronous Check Data tasks
class SrvTasks_JobCheckData : public SrvTasks_Job
{
public: // interface for users
    /// This creates and defines a new job which can be executed by a server task.
    SrvTasks_JobCheckData(SrvTasks_WaitingTask &waitingTask,
                          const tgg00_FileId   &fileid,
                          bool                  withUpdate,
                          bool                  extended);
	/// returns the root of the current tree
    tsp00_PageNo GetRoot() const
    {
        return m_FileId.fileRoot_gg00();
    }
    /// Appends some context information
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    Msg_List                  &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    virtual void Deallocate() {}
protected:
    /// This executes the job and must be overlayed
    virtual Result ExecuteIntern (Trans_Context &trans);
    /// This creates a copy of the job (including all pointers)
    /// This is used by servertasks if nobody wants to wait for the jobs end
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const;
private:
    /// The fileid of the file to be checked
    tgg00_FileId m_FileId;
    /// Used in admin mode to maintane the converter
    bool         m_WithUpdate;
    /// use an extended check
    bool         m_Extended;
};
#endif // SrvTasks_JobCheckData_HPP
