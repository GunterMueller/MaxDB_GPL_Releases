/*!
  @file           SrvTasks_ITask.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          implements interface SrvTasks_ITask

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
#ifndef SrvTasks_ITask_HPP
#define SrvTasks_ITask_HPP
class RTETask_ITask;
class SrvTasks_Job;
class SrvTasks_WaitingTask;
class Msg_List;
/*!
@interface SrvTasks_ITask
@brief     This defines the interface of a server task which can be used by users
*/
class SrvTasks_ITask
{
public:
    /// this is the result of RegisterJob()
    enum RegisterJobResult     // PTS 1134588 UH 2005-03-24
    {
        registered,              ///< the given is is successful registered to the task
        cannotWaitforRegister,   ///< there exists already another task which waits to register - this case must be handled outside
        registerError            ///< any other error occured such as allocation problems
    };
    /// This is called when a new job should be executed by this server task.
    virtual RegisterJobResult RegisterJob (RTETask_ITask        & requestortask,     // PTS 1134588 UH 2005-03-24
                                           SrvTasks_Job         & job,
                                           Msg_List             & errlist,
                                           bool                   waitForTaskToBeFinished,
                                           SrvTasks_WaitingTask * waitingTask = 0) = 0;
    /// This is called when the server task should stop executing
    virtual void Abort(RTETask_ITask &task,
                       bool           inRegion = false) = 0;
};
#endif // SrvTasks_ITask_HPP
