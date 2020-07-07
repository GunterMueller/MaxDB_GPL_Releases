/*!========================================================================

  @file         RTETask_ExternalRequestsQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler external requests queue handling.

                This queue is part of the scheduler mechanism and 
                contains the queue elements of non task scheduler
                internal requests.
                

  @since        2003-09-26  12:41
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_EXTERNALREQUESTSQUEUE_HPP
#define RTETASK_EXTERNALREQUESTSQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_TaskScheduler;

/*!---------------------------------------------------------------------------
  @class   RTETask_ExternalRequestsQueue
  @brief   Task scheduler external requests queue handling.

           This queue is part of the task scheduler mechanism and 
           receives external requests shaped as doubly linked 
           queue elements. External means all code runninig not
           in TaskScheduler thread context like the timer thread or
           the IO completion tread (IOWorker). These requests
           are TaskScheduler related (e.g. start a new load 
           balancing measurement interval) or task related which
           normaly forces a task scheduling.

           Refer to RTETask_CommonQueueElement

           The queue is fully synchronized via locked scopes.

 ----------------------------------------------------------------------------*/

class  RTETask_ExternalRequestsQueue
{
public:
    /*!
    @brief          Enqueue a common queue element to the 
                    external requests queue

    This enqueue methode also wakes up the TaskScheduler from its
    semaphore wait.

    @param    queueElement [in]   - Reference to queue element
    @param    task         [in]   - The task which receives that queue element
    @param    senderTask   [in]   - The task which sends that queue element
    */
    void EnqueueAndWakeUp( RTETask_CommonQueueElement   &queueElement,
                           RTETask_Task                 &task,
                           RTETask_Task                 &senderTask );

    /*!
    @brief          Enqueue a common queue element to the 
                    external requests queue

    This enqueue methode also wakes up the TaskScheduler from its
    semaphore wait. This methode should not be used by tasks. Tasks should
    use the methode above which gets a sender task information. 

    Only threads like the requestor use this methode!

    @param    queueElement [in]   - Reference to queue element
    @param    task         [in]   - The task which receives that queue element
    */
    void EnqueueAndWakeUp( RTETask_CommonQueueElement   &queueElement,
                           RTETask_Task                 &task );
protected:
    /*!
    @brief    Constructor
    */
    RTETask_ExternalRequestsQueue() : m_pTaskScheduler(0) {;}


    // don't allow copies
    inline   RTETask_ExternalRequestsQueue( const RTETask_ExternalRequestsQueue &c )
                : m_pTaskScheduler(c.m_pTaskScheduler) {;}; 

protected:
    RTETask_TaskScheduler* m_pTaskScheduler; 
};

/*!---------------------------------------------------------------------------
  @class   RTETask_ExternalRequestsQueueTS

  @brief        Task scheduler external requests queue handling.

  This derived calls contains all methodes which are
  access by the TaskScheduler only

 ----------------------------------------------------------------------------*/

class  RTETask_ExternalRequestsQueueTS : public RTETask_ExternalRequestsQueue
{
public:
    /*!
    @brief          Enqueue a common queue element to the 
                    external requests queue

    This enqueue methodes also wakes up the TaskScheduler from its
    semaphore wait.

    @param    queueElement [in]   - Reference to queue element
    */
    void EnqueueAndWakeUp( RTETask_CommonQueueElement   &queueElement );

   /*!
    @brief         Set pointer to the TaskScheduler the queue belongs to.

    @param         pTaskScheduler [in]     - pointer to the TaskScheduler object
    */
    void SetTaskScheduler( RTETask_TaskScheduler* pTaskScheduler ) 
    {
        m_pTaskScheduler = pTaskScheduler;
    }
};

#endif  /* RTETASK_EXTERNALREQUESTSQUEUE_HPP */
