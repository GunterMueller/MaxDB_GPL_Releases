/*!========================================================================

  @file         RTETask_ExternalRequestsQueue.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler external requests queue handling.

                This queue is part of the scheduler mechanism and 
                contains the queue elements of non task scheduler
                internal requests.

  @since        2003-09-26  12:49
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
#include    "RunTime/Tasking/RTETask_ExternalRequestsQueue.hpp"


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

void RTETask_ExternalRequestsQueue
           ::EnqueueAndWakeUp( RTETask_CommonQueueElement &queueElement,
                               RTETask_Task               &task )
{
    // Check if task is in this TaskScheduler
    SAPDBERR_ASSERT_STATE( m_pTaskScheduler == &task.TaskScheduler() );

    queueElement.SetTask( task.LegacyTaskCtrl() );
    m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInUKTQueue ( queueElement, task.LegacyTaskCtrl() );
    m_pTaskScheduler->WakeUP();
}

/*---------------------------------------------------------------------------*/

void RTETask_ExternalRequestsQueue
           ::EnqueueAndWakeUp( RTETask_CommonQueueElement &queueElement,
                               RTETask_Task               &task,
                               RTETask_Task               &senderTask )
{
    // Check if task is in this TaskScheduler
    SAPDBERR_ASSERT_STATE( m_pTaskScheduler == &task.TaskScheduler() );

    queueElement.SetTask( task.LegacyTaskCtrl() );
    queueElement.SetSenderTask( senderTask.LegacyTaskCtrl() );

    m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInUKTQueue ( queueElement, task.LegacyTaskCtrl() );
    m_pTaskScheduler->WakeUP();
}

/*---------------------------------------------------------------------------*/

void RTETask_ExternalRequestsQueueTS
           ::EnqueueAndWakeUp( RTETask_CommonQueueElement   &queueElement )
{
    m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInUKTQueue ( queueElement );
    m_pTaskScheduler->WakeUP();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/