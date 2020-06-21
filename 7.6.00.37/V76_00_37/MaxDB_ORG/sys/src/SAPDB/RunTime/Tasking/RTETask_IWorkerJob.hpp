/*!
  @file           RTETask_IWorkerJob.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Scheduling

  System Independend Worker Thread For Time Consuming Or Blocking Task Work
  Needed i.e. during initialization of HotStandby 

\if EMIT_LICENCE

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
#ifndef RTETASK_IWORKERJOB_HPP
#define RTETASK_IWORKERJOB_HPP

#include "RunTime/RTE_Types.h" // for RTE_TaskId only

/*!
  @class RTETask_IWorkerJob
  @brief This class is the base interface class for a worker job. 
  
  The implementator of the job must derive from the jobs implementation from it.

  The worker thread dequeues a job from its internal job queue and calls the 'DoJob()' 
  function. After it is completed, the wroker thread uses the 'GetTaskId()' call to
  inform the dispatcher, that the job is complete.
 */
class RTETask_IWorkerJob
{
public:
    /*!
      @brief Return the worker job orderer task id
      @return the task id of the task to wake up after job is done
     */
    virtual RTE_TaskId const GetTaskId() const = 0;

    /*!
      @brief Do the job
      This is executed by the worker thread
     */
    virtual void DoJob() = 0;
};

/*!
  @brief Enqueue worker job and wait for completion

  This call enqueues the task in a waiting queue and calls the dispatcher to allow the next
  task in the task group to execute. Only if no more tasks are runnable, the task will suspend itself.
  It will do so by internally calling a 'vstop()'. The worker thread will issue the corresponding 'vcontinue()'.
  If the worker thread is running, will find the job in the queue, otherwise it will be awoken while sleeping
  on its internal semaphore.

  @param job [in] the job to enqueue
 */
extern "C" void RTETask_CallWorker(RTETask_IWorkerJob &job);

#endif  /* RTETASK_IWORKERJOB_HPP */
