/*!========================================================================

  @file         RTETask_WorkerPool.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Pool of System Independend Task Worker Threads.

                System Independend Worker Threads For Time Consuming Or Blocking Task Work
                Needed i.e. during initialization of HotStandby 


  @since        2005-10-14  15:36
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/


#ifndef RTETASK_WORKERPOOL_HPP
#define RTETASK_WORKERPOOL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/Tasking/RTETask_IWorkerJob.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTETask_Worker;
class SAPDBErr_MessageList;

/*!---------------------------------------------------------------------------
  @class   RTETask_WorkerPool
  @brief   pool of worker threads

           The rest of the description appears as detailed description
 ----------------------------------------------------------------------------*/

class RTETask_WorkerPool
{
public:
    /*!
      @brief ctor
     */
    RTETask_WorkerPool() : m_NumOfWorker(0), m_pWorkerPool(0), m_WorkerJobThreshold(0) {}

    /*!
      @brief get singleton instance
      @return the reference
     */
    static RTETask_WorkerPool&  Instance();

    /*!
      @brief add a job for the workers
      @param job [in] worker job
     */
    void AddJob( RTETask_IWorkerJob &job );

    /*!
      @brief create all worker
      @param numOfWorker        [in]  number of workers wanted
      @param workerStackSize    [in]  individual worker thread stack size in bytes
      @param workerJobThreshold [in]  minimum job queue length before using another thread
      @param messageList        [out] message list
      @return number of workers created or 0
     */
    SAPDB_UInt4 CreateAllWorker ( SAPDB_UInt4            numOfWorker,
                                  SAPDB_UInt4            workerStackSize,
                                  SAPDB_UInt4            workerJobThreshold,
                                  SAPDBErr_MessageList   &messageList );

private:
    RTETask_Worker**           m_pWorkerPool;
    SAPDB_UInt4                m_NumOfWorker;
    static RTETask_WorkerPool* m_pInstance;
    SAPDB_UInt4                m_WorkerJobThreshold;
};


/*!
  @brief C wrapper for create worker thread
  @param workerStackSize [in] size of stack in bytes
 */
extern "C" void RTETask_CWrapperForCreateWorkerThread( SAPDB_Int4 workerStackSize );

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

#endif  /* RTETASK_WORKERPOOL_HPP */
