/*!========================================================================

  @file         RTETask_UnconnectedTasksQueue.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        The queue holds connectable tasks which are unconnected
  
                The requestor is dequeuing elements from this queue 
                to fulfil client connect requests that are not specifying
                a dedicated task.
                If a task has been disconnected from all clients it is 
                enqueuing an common queue element into this queue to signal 
                it is waiting for incomming connect requests.

  @since        2003-09-30  18:10
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

#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/Tasking/RTETask_UnconnectedTasksQueue.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void RTETask_UnconnectedTasksQueue::Enqueue( RTETask_CommonQueueElement  &queueElement,
                                             RTETask_Task                &task )
{
    SAPDBERR_ASSERT_STATE( queueElement.GetType() == RTETask_CommonQueueElement::CommunicationConnectWait );

    queueElement.SetTask( task );
    queueElement.SetSenderTask( task );

    m_SyncQueue.Enqueue(queueElement);
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/