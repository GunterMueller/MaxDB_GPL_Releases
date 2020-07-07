/*!========================================================================

  @file         RTETask_ReadyToRunQueue.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task scheduler ready to run queue handling.

                This queue is part of the scheduler mechanism and 
                contains the queue elements of the tasks are waiting
                to be scheduled.

  @since        2006-03-09  14:03
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
#include    "RunTime/Tasking/RTETask_ReadyToRunQueue.hpp"

#include    "geo002.h" // BASE_PRIO_REX, BASE_PRIO_RAV


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

void RTETask_ReadyToRunQueue::Enqueue( RTETask_CommonQueueElement   &queueElement,
                                       RTETask_Task                 &task,
                                       RTETask_Task                 &senderTask,
                                       BasePriority                 basePrio )
{
    // Check if task is in this TaskScheduler
    SAPDBERR_ASSERT_STATE( m_pTaskScheduler == &task.TaskScheduler() );

    queueElement.SetTask( task.LegacyTaskCtrl() );
    queueElement.SetSenderTask( senderTask.LegacyTaskCtrl() );


    TASK_CTRL pSenderTaskCtrl = 0;

    if ( queueElement.SenderTask() )
       pSenderTaskCtrl = queueElement.SenderTask()->LegacyTaskCtrl();

    switch ( basePrio )
    {
    case YieldPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueueTaskYield( queueElement,
                                                                             task.LegacyTaskCtrl(),
                                                                             pSenderTaskCtrl,
                                                                             BASE_PRIO_REX );
        break;
    case WakeUpPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    task.LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    BASE_PRIO_RAV );
        break;
    default:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    task.LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    BASE_PRIO_REX );
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_ReadyToRunQueueTS::Enqueue( RTETask_CommonQueueElement   &queueElement,
                                         InternalBasePriority         internalBasePrio )
{
    RTETask_Task* pSenderTask = queueElement.SenderTask();

    TASK_CTRL pSenderTaskCtrl = 0;

    if ( queueElement.SenderTask() )
       pSenderTaskCtrl = queueElement.SenderTask()->LegacyTaskCtrl();

    switch ( internalBasePrio )
    {
    case CommunicationPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    queueElement.Task()->LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    BASE_PRIO_COM );
        break;
    case SchedulerToSchedulerPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    queueElement.Task()->LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    BASE_PRIO_U2U );
        break;
    case ExternalRequestsPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    queueElement.Task()->LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    BASE_PRIO_UKT );
        break;
    case DebugTaskPrio:
        m_pTaskScheduler->LegacyUKTCtrl().EnqueueElementInRunQueue( queueElement,
                                                                    queueElement.Task()->LegacyTaskCtrl(),
                                                                    pSenderTaskCtrl,
                                                                    (SAPDB_UInt4)-1 );
        break;
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_ReadyToRunQueueTS::Enqueue( RTETask_CommonQueueElement   &queueElement,
                                         RTETask_Task                 &task,
                                         RTETask_Task                 &senderTask,
                                         InternalBasePriority         internalBasePrio )
{
    queueElement.SetSenderTask( senderTask.LegacyTaskCtrl() );
    queueElement.SetTask( task.LegacyTaskCtrl() );

    Enqueue( queueElement, internalBasePrio );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/