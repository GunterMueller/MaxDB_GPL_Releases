/*!
  @file           SrvTasks_JobDropVolume.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines class SrvTasks_JobDropVolume

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
#ifndef SrvTasks_JobDropVolume_HPP
#define SrvTasks_JobDropVolume_HPP

//--------------------------------------------------------------------------------------

#include "ServerTasks/SrvTasks_Job.hpp"
#include "Converter/Converter_PageNoIterator.hpp"
#include "ggg00.h"

//--------------------------------------------------------------------------------------

/// This job is used for drop volume 
class SrvTasks_JobDropVolume : public SrvTasks_Job
{

public: // interface for users

    /// This creates and defines a new job which can be executed by a server task.
    SrvTasks_JobDropVolume(SrvTasks_WaitingTask &waitingTask,
                            Converter_PageNoIterator &pageIter);

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

    Converter_PageNoIterator         &m_pageIter;

};
#endif // SrvTasks_JobDropVolume_HPP
