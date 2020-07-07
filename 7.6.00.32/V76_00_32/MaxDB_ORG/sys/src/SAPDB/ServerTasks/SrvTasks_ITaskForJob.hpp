/*!
  @file           SrvTasks_ITaskForJob.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines the interface a job can call

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
#ifndef SrvTasks_ITaskForJob_HPP
#define SrvTasks_ITaskForJob_HPP
#include "RunTime/Tasking/RTETask_ITask.hpp"
class SAPDBMem_IRawAllocator;
class Kernel_VTrace;
class SAPDB_OStream;
class Trans_Context;
/*!
@interface SrvTasks_ITaskForJob
@brief     This is used to define task related methods callable by jobs
*/
class SrvTasks_ITaskForJob
{
public: 
    /// This is called when the server task should stop executing
    virtual void Abort(RTETask_ITask &task,
                       bool           inRegion) = 0;
    /// This returns true, if the job should stop executing
    virtual bool IsAborted() const = 0;
    /// returns the kernel or trans context of the executing server task
    virtual Trans_Context& GetContext() = 0;
};
/// This is used to write the task context to the knltrace.
SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_ITaskForJob &task);
#endif // SrvTasks_ITaskForJob_HPP
