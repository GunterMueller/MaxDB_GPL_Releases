/*!========================================================================

  @file         RTETask_CommunicationQueue.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler communication queue handling.

                The communication queue is part of the client-kernel
                -communication mechanism. The queue contains a list of 
                elements, one per task, which is waiting for a communication
                packet to be arrived. 

  @since        2005-11-15  16:18
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Tasking/RTETask_CommunicationQueue.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/RTE_KGS.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static SAPDB_ULong const maxTimeoutVal = (SAPDB_ULong)-1;


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void 
    RTETask_CommunicationQueue::Enqueue( RTETask_CommonQueueElement  &queueElement,
                                         RTETask_Task                &task )
{
    // Check if task is in this TaskScheduler
    SAPDBERR_ASSERT_STATE( m_pTaskScheduler == &task.TaskScheduler() );
    SAPDBERR_ASSERT_STATE( queueElement.GetType() == RTETask_CommonQueueElement::CommunicationReceiveWait );

    // provide legacy values
    //
    RTETask_LegacyTaskCtrl &LegacyTaskCtrl = task.LegacyTaskCtrl();
    LegacyTaskCtrl.QueueLocation()                      = TASK_IS_IN_COM_QUEUE_EO00;
    LegacyTaskCtrl.CommQueueCount()                    += 1;
    m_pTaskScheduler->LegacyUKTCtrl().CommQueueCount() += 1;
    m_CommQueueCount                                   += 1;

    queueElement.SetTask( task );
    queueElement.SetSenderTask( task );

    // insert into doubly linked list
    m_DLList.InsertTail( queueElement );

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_ULong 
    RTETask_CommunicationQueueTS::Dequeue( SAPDB_ULong                   currTime,
                                           RTETask_CommonQueueElement*  &pQueueElement )
{
    SAPDB_ULong      timeOutVal = maxTimeoutVal;

    // Find first element in COM queue which waits for
    // for command or had a timeout
    pQueueElement = FirstInQueue();

    while ( pQueueElement )
    {
        RTETask_CQE_CommunicationReceiveWaitPart&  
              elementDataPart = pQueueElement->CommunicationReceiveWaitPart();

        //  Communication packet available?
        if ( elementDataPart.CommunicationDataAvailable() )
        {
            //  remove task from doubly linked list
            m_DLList.Remove( *pQueueElement );

            // provide legacy values
            pQueueElement->Task()->LegacyTaskCtrl().QueueLocation() = TASK_IS_IN_NON_QUEUE_EO00;
            pQueueElement->Task()->LegacyTaskCtrl().UnlockLoadBalancing(WAITING_FOR_NEW_COMMAND);
            break;
        } 
        else
        {
            // timeout occured ?
            if ( elementDataPart.SessionTimeOutAt() < currTime )
            {
                // signal that a timeout has occured 
                elementDataPart.SetSessionTimeoutOccurred();

                //  remove task from doubly linked list
                m_DLList.Remove( *pQueueElement );

                // provide legacy values
                pQueueElement->Task()->LegacyTaskCtrl().QueueLocation() = TASK_IS_IN_NON_QUEUE_EO00;
                pQueueElement->Task()->LegacyTaskCtrl().UnlockLoadBalancing(WAITING_FOR_NEW_COMMAND);
                break;
            }

            // calculate minimum timout value
            if ( elementDataPart.SessionTimeOutAt() < timeOutVal )
            {
                // set new minimum timout value
                timeOutVal = elementDataPart.SessionTimeOutAt();
            }
            pQueueElement = pQueueElement->Next();
        }
    }
    return timeOutVal;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
