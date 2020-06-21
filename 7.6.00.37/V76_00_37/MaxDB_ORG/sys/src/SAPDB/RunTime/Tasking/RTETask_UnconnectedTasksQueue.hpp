/*!========================================================================

  @file         RTETask_UnconnectedTasksQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        The queue holds connectable tasks which are unconnected
  
                The requestor is dequeuing elements from this queue 
                to fulfil client connect requests that are not specifying
                a dedicated task.
                If a task has been disconnected from all clients it is 
                enqueuing an common queue element into this queue to signal 
                it is waiting for incomming connect requests.

  @since        2003-09-30  18:11
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


#ifndef RTETASK_UNCONNECTEDTASKSQUEUE_HPP
#define RTETASK_UNCONNECTEDTASKSQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Tasking/RTETask_CommonQueue.hpp"

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
  @class   RTETask_UnconnectedTasksQueue
  @brief   The queue holds connectable tasks which are unconnected.

           The requestor is dequeuing elements from this queue 
           to fulfil client connect requests that are not specifying
           a dedicated task.
           If a task has been disconnected from all clients it is 
           enqueuing an common queue element into this queue to signal 
           it is waiting for incomming connect requests.
 ----------------------------------------------------------------------------*/

class  RTETask_UnconnectedTasksQueue
{
public:
    /*!
    @brief         Constructor.

                   Contructor to create a unconnected tasks queue

    @param         queueName         [in] - Give the queue a name for error messages
    */
    RTETask_UnconnectedTasksQueue( SAPDB_Char const * const  queueName ) 
        : m_SyncQueue(queueName) 
    {
    }

    /*!
    @brief         Enqueue a task queue element of a task that is waiting for 
                   a incomming connect request.

    The task which has released ALL connections and is going to wait for a connect 
    request has to get a common queue element from the tasks free list.  This element
    must get the element type 'RTETask_CommonQueueElement::CommunicationConnectWait'.
    The Enqueue() methode is used to insert such an element in to the unconnected
    tasks queue to signal a task is unconnected and is waiting for a connection.

    @param         queueElement [in] - Connect wait queue element.
    @param         task         [in] - The task which waits for a connect request.
    */
    void Enqueue( RTETask_CommonQueueElement  &queueElement,
                  RTETask_Task                &task );

   /*!
    @brief         Number of unconnected tasks in queue.
    */
    SAPDB_UInt4 NumberOfWaitingTasks()
    {
        return m_SyncQueue.NumberOfQueueElements();
    }

    /*!
    @brief         Dequeue a common queue element of a unconnected task.
    
    This queue element contains the task object pointer of a unconnected 
    task, the element type is 'RTETask_CommonQueueElement::CommunicationConnectWait'.

    @return        Common queue element of a unconnected task.
    */
    RTETask_CommonQueueElement* Dequeue()
    {
        return m_SyncQueue.Dequeue();
    }

private:
    RTETask_SynchronizedCommonQueue           m_SyncQueue;
};

#endif  /* RTETASK_UNCONNECTEDTASKSQUEUE_HPP */
