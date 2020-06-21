/*!========================================================================

  @file         RTETask_CommunicationQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler communication queue handling.

                The communication queue is part of the client-kernel
                -communication mechanism. The queue contains a list of 
                elements, one per task, which is waiting for a communication
                packet to be arrived. 

  @since        2005-04-27  17:03
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


#ifndef RTETASK_COMMUNICATIONQUEUE_HPP
#define RTETASK_COMMUNICATIONQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif
#include    "RunTime/RTE_KGS.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "RunTime/Tasking/RTETask_DoublyLinkedList.hpp"


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
  @class   RTETask_CommunicationQueue
  @brief   Task scheduler communication queue handling.

                The communication queue is part of the client-kernel
                -communication mechanism. 

                The queue contains a list of elements, one per task, 
                which is waiting for a communication packet to be arrived. 

                Each element holds a pointer to a communication flag which
                is part of a shared memory segment. The client sets this flag
                to true if a communication packet has been send. The communication
                queue uses this flag as criterion to dequeue the communication queue
                element and to signal that a communiction packet has arrived.

                Each UKT has its own communication queue which means
                that no locked scopes are needed to handle this queue.

 ----------------------------------------------------------------------------*/

class  RTETask_CommunicationQueue
{
public:
    /*!
    @brief         Enqueue a communication queue element.

    The task which is going to wait for a client communication packed has to get
    a common queue element from the tasks free list first. This element must have
    the element type 'RTETask_CommonQueueElement::CommunicationReceiveWait' and 
    its type related part data describes the wait criteria like command timeout
    and contains the pointer to a communication flag. This flag is set to true if
    //the client has sent a packet.
    The Enqueue() methode is used to insert such an element in to the communication
    queue to signal a tasks communication wait state. After this the task enters 
    the sheduler (dispatcher) which dequeues the elements via Dequeue().

    @param         queueElement [in] - Communication queue element.
    @param         task         [in] - The task which receives that queue element.
                                       It must be the same as the sending task. This
                                       queue can only be used by the receiving task
                                       itself.
    */
    void Enqueue( RTETask_CommonQueueElement  &queueElement,
                  RTETask_Task                &task );

   /*!
    @brief         Number of queue element in comm queue.
    */
    SAPDB_UInt4 NumberOfQueueElements()
    {
        return m_DLList.ListLen();
    }

public:
    /* The following methodes are used by the console standard worker */

   /*!
    @brief         First queue element in comm queue.

    Returns the first queue element of task which is waiting 
    for a client communication packed. 

    @return        pointer to the first element in queue.
    */
    RTETask_CommonQueueElement*  FirstInQueue()
    {
        return reinterpret_cast<RTETask_CommonQueueElement*>( m_DLList.FirstElement() );
    }

   /*!
    @brief         Next queue element in comm queue.

    Returns the next queue element of the communication queue 
    related to the given queue element 

    @param         Element [in]  - the queue element where we 
                                   are starting the next search.

    @return        pointer to the next element in queue.
    */
    RTETask_CommonQueueElement*  NextInQueue( RTETask_CommonQueueElement  &Element )
    {
        RTETask_CommonQueueElement* next = Element.Next();

        // ... still in this queue?
        if ( Element.GetType() == RTETask_CommonQueueElement::CommunicationReceiveWait )
           return next;

        return 0;
    }

   /*!
    @brief         Returns the communication queue count.

    Returns the the communication queue enque operations

    @return        communication queue enque operations.
    */
    SAPDB_UInt8  CommQueueCount() const
    {
        return m_CommQueueCount;
    }

   /*!
    @brief         Reset the communication queue count.

    Resets the the communication queue enque operation counter

    @return        none
    */
    void ResetCommQueueCount()
    {
        m_CommQueueCount = 0;
    }

protected:
    /*!
      @brief         constructor.

                     Build up a communication queue. The is filled via Enque() methode.
    */
    RTETask_CommunicationQueue() : m_KGS(RTE_KGS::Instance()), // set shortcut to kgs
                                   m_pTaskScheduler(0),
                                   m_CommQueueCount(0)
    {}

    /*!
      @brief         destructor.
  
                     Destroy communication queue element. 
    */
    ~RTETask_CommunicationQueue() {}

protected:
    // don't allow copies
    inline   RTETask_CommunicationQueue( const RTETask_CommunicationQueue &c ) 
               : m_KGS(RTE_KGS::Instance()) {;}

protected:
    SAPDB_UInt8                                                      m_CommQueueCount;
    RTE_KGS                                                          &m_KGS;            // Shortcut to kgs
    RTETask_TaskScheduler*                                           m_pTaskScheduler;  // Task scheduler
    RTETask_DoublyLinkedList<DummyLockedScopeDLList,DummyLockDLList> m_DLList;          // container for the queue elements
};

/*!---------------------------------------------------------------------------
  @class   RTETask_CommunicationQueueTS

  @brief        Task scheduler communication queue handling.

  This derived calls contains all methodes which are
  access by the TaskScheduler itself

 ----------------------------------------------------------------------------*/

class  RTETask_CommunicationQueueTS : public RTETask_CommunicationQueue
{
public:
   /*!
    @brief         Set pointer to the TaskScheduler the queue belongs to.

    @param         pTaskScheduler [in]     - pointer to the TaskScheduler object
    */
    void SetTaskScheduler( RTETask_TaskScheduler* pTaskScheduler ) 
    {
        m_pTaskScheduler = pTaskScheduler;
    }

   /*!
    @brief         Dequeue a communication queue element.

    The task which is waiting for a client communication packed has inserted a 
    common queue element into the communication queue. This element describes
    the wait criteria like command timeout and contains the pointer to a communication
    flag. This flag is set to true if the client has sent a packet.

    The scheduler calls the Dequeue() methode to get the next common queue element of
    the task where a communication packet has arrived or command timeout has occurred. 
    The element pointer is null if none of these criterias are true.

    @param         currTime  [in]        - current time needed to check command timeouts.
    @param         pQueueElement [out]   - pointer to the queue element of a task where
                                           a communication packet arrived. If the return
                                           value of Dequeue() is zero, a  task command 
                                           timeout occurred.

    @return        If no queue element is returned it is the maximum wait time 
                   until the next command timeout occures
                   (maximum scheduler sleep time). 
    */
    SAPDB_ULong Dequeue( SAPDB_ULong                  currTime,
                         RTETask_CommonQueueElement*  &pQueueElement );

   /*!
    @brief         Check if the communication queue is empty.

    @return        True, if queue contains no element.
    */
    SAPDB_Bool  QueueIsEmpty() 
    { 
        return m_DLList.IsEmpty(); 
    }

   /*!
    @brief         Check if the communication queue is not empty.

    @return        True, if queue contains at least one element.
    */
    SAPDB_Bool  QueueIsNotEmpty()
    { 
        return m_DLList.IsEmpty() == false;
    }
};


#endif  /* RTETASK_COMMUNICATIONQUEUE_HPP */
