/*!========================================================================

  @file         RTETask_ReadyToRunQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler ready to run queue handling.

                This queue is part of the scheduler mechanism and 
                contains the queue elements of the tasks are waiting
                to be scheduled.

  @since        2003-09-26  11:10
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


#ifndef RTETASK_READYTORUNQUEUE_HPP
#define RTETASK_READYTORUNQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTETask_ReadyToRunQueue
  @brief   TaskScheduler ready to run queue handling.

           This queue is part of the task scheduler mechanism and 
           contains the common queue elements for the 
           tasks are waiting to be rescheduled (Refer to 
           RTETask_CommonQueueElement). 

           THIS QUEUE CAN ONLY BE USED FOR THE ENQUEUING TASK ITSELF
           OR A TASK RUNNING IN THE SAME TASKSCHEDULER!

           These queue elements are a essential part of the sheduling.
           A task waiting for a operation to finish, doing a reschedule
           (yield) or waiting for a wake up triggered by another task is
           doing a 'WaitForReschedule' call. A respective queue element 
           is enqueued in one of the TaskSchedulers queues to set the 
           taks free from it's wait call.

           Each TaskScheduler has its own ready to run queue which means
           that no locked scopes are needed to handle this queue.

 ----------------------------------------------------------------------------*/

class  RTETask_ReadyToRunQueue
{
public:
    /*!
    @brief    Base priority needed for scheduling

              Based on kernel parameters (XPARAM) so it can
              be modified ( prior call BASE_PRIO_REX and BASE_PRIO_RAV )

              DON'T FORGET SAPDB_ToStringClass BELOW!!!!
    */
    enum BasePriority
    {
        YieldPrio,           // priority is normaly low and is used for yield situation.
        WakeUpPrio           // priority is normaly above yield priority and 
                             // is used for task wake ups like resume, signal etc.
    };

    /*!
    @brief    Constructor
    */
    RTETask_ReadyToRunQueue() : m_pTaskScheduler(0) {;}

    /*!
    @brief          Enqueue a common queue element to the 
                    ready to run queue

    @param    queueElement [in]   - Reference to queue element
    @param    task         [in]   - The task which receives that queue element
    @param    senderTask   [in]   - The task which sends that queue element
    @param    basePrio     [in]   - Task priority.
    */
    void Enqueue( RTETask_CommonQueueElement &queueElement,
                  RTETask_Task               &task,
                  RTETask_Task               &senderTask,
                  BasePriority               basePrio = WakeUpPrio );

private:
    // don't allow copies
    inline   RTETask_ReadyToRunQueue( const RTETask_ReadyToRunQueue &c )
                : m_pTaskScheduler(c.m_pTaskScheduler) {;}; 

protected:
    RTETask_TaskScheduler* m_pTaskScheduler; 
};

/*---------------------------------------------------------------------------*/

/*!
  @brief convert RTETask_ReadyToRunQueue::BasePriority into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTETask_ReadyToRunQueue::BasePriority basePrio )
{
    switch (basePrio)
    {
    case RTETask_ReadyToRunQueue::YieldPrio            : return SAPDB_ToStringClass("Yield priority");
    case RTETask_ReadyToRunQueue::WakeUpPrio           : return SAPDB_ToStringClass("Wake priority");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)basePrio);
}

/*!---------------------------------------------------------------------------
  @class   RTETask_ReadyToRunQueueTS

  @brief   TaskScheduler ready to run queue handling.

  This derived calls contains all methodes which are
  access by the TaskScheduler only

 ----------------------------------------------------------------------------*/

class  RTETask_ReadyToRunQueueTS : public RTETask_ReadyToRunQueue
{
public:
    /*!
    @brief    Internal base priority needed for scheduling


    */
    enum InternalBasePriority
    {
        CommunicationPrio,       // request received via communication queue
        ExternalRequestsPrio,    // request received via external requests queue
        SchedulerToSchedulerPrio,// request received via scheduler to scheduler queue
        DebugTaskPrio            // maximum priority task is scheduled immediatly
    };

public:
    /*!
    @brief          Enqueue a common queue element to the 
                    ready to run queue

    @param    queueElement         [in]   - Reference to queue element
    @param    InternalBasePriority [in]   - Task priority.
    */
    void Enqueue( RTETask_CommonQueueElement   &queueElement,
                  InternalBasePriority         internalBasePrio );

    /*!
    @brief          Enqueue a common queue element to the 
                    ready to run queue

                    This enqueue methode alters the task and sender task entries
                    of the queue element. This methode should be used in some rare
                    situation.

    @param    queueElement         [in]   - Reference to queue element
    @param    task                 [in]   - The task which receives that queue element
    @param    senderTask           [in]   - The task which sends that queue element
    @param    InternalBasePriority [in]   - Task priority.
    */
    void Enqueue( RTETask_CommonQueueElement   &queueElement,
                  RTETask_Task                 &task,
                  RTETask_Task                 &senderTask,
                  InternalBasePriority         internalBasePrio );

   /*!
    @brief         Set pointer to the TaskScheduler the queue belongs to.

    @param         pTaskScheduler [in]     - pointer to the TaskScheduler object
    */
    void SetTaskScheduler( RTETask_TaskScheduler* pTaskScheduler ) 
    {
        m_pTaskScheduler = pTaskScheduler;
    }

};

/*---------------------------------------------------------------------------*/

/*!
  @brief convert RTETask_ReadyToRunQueueTS::InternalBasePriority into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTETask_ReadyToRunQueueTS::InternalBasePriority basePrio )
{
    switch (basePrio)
    {
    case RTETask_ReadyToRunQueueTS::CommunicationPrio        : return SAPDB_ToStringClass("Communication priority");
    case RTETask_ReadyToRunQueueTS::ExternalRequestsPrio     : return SAPDB_ToStringClass("ExternalRequests priority");
    case RTETask_ReadyToRunQueueTS::SchedulerToSchedulerPrio : return SAPDB_ToStringClass("SchedulerToScheduler priority");
    case RTETask_ReadyToRunQueueTS::DebugTaskPrio            : return SAPDB_ToStringClass("DebugTask priority");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)basePrio);
}

#endif  /* RTETASK_READYTORUNQUEUE_HPP */
