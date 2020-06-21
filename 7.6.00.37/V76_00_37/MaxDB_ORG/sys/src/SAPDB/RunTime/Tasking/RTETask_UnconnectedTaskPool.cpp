/*!========================================================================

  @file         RTETask_UnconnectedTaskPool.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Pool deals with unconnected tasks.

                The requestor uses this pool to fulfil incomming
                connect requests for a specific task type

  @since        2003-10-01  14:41
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

#include    "RunTime/Tasking/RTETask_UnconnectedTaskPool.hpp"
#include    "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include    "RunTime/RTE_PendingEventHandler.h"


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

RTETask_CommonQueueElement*   
    RTETask_UnconnectedTaskPool::GetTask( RTETask_ITask::TaskType taskType )
{
    RTETask_TaskScheduler* pBestTaskScheduler;
    SAPDB_Bool almostOutOfSessions;

    for ( SAPDB_UInt4 retry = 2; retry > 0; --retry )
    {
        pBestTaskScheduler = FindBestTaskScheduler( taskType, almostOutOfSessions );

        if ( RTETask_ITask::User == taskType && almostOutOfSessions )
        {
            // allow callback different behaviour if no more task is found !
            (*m_SearchUnreleasedConnectionsCallback)(pBestTaskScheduler != 0);
        }
        else
        {
            if ( pBestTaskScheduler != 0 )
            {
                break;
            }
        }
    }

    if ( RTETask_ITask::User == taskType )
    {
        m_Lock.Lock();
            if ( almostOutOfSessions && !m_OutOfSessionEventRegistered )
            {
                tsp31_event_description Event;
                memset(&Event, 0, sizeof(tsp31_event_description));

                Event.sp31ed_ident.becomes(sp31ei_outofsessions);
                Event.sp31ed_priority.becomes(sp31ep_high);
                strncpy((char *)&(Event.sp31ed_text_value[0]), "Out of user sessions", sizeof(Event.sp31ed_text_value) );
	            Event.sp31ed_text_len = Event.sp31ed_text_value.length();
                Event.sp31ed_value_1  = MAX_INT4_SP00;
                Event.sp31ed_value_2  = MAX_INT4_SP00;

                RTE_PendingEventHandler::Instance().Register(Event);
                m_OutOfSessionEventRegistered = true;
            }
        m_Lock.Unlock();
    }

    return ( pBestTaskScheduler ? pBestTaskScheduler->UnconnectedTasksQueue(taskType)->Dequeue() : 0 );
}

/*---------------------------------------------------------------------------*/

void RTETask_UnconnectedTaskPool::AddTask()
{
    m_Lock.Lock();
        if ( m_OutOfSessionEventRegistered )
        {
            m_OutOfSessionEventRegistered = false;
            RTE_PendingEventHandler::Instance().Deregister(sp31ei_outofsessions);
        }
    m_Lock.Unlock();
}

/*---------------------------------------------------------------------------*/

inline RTETask_TaskScheduler* RTETask_UnconnectedTaskPool::FindBestTaskScheduler( RTETask_ITask::TaskType taskType, SAPDB_Bool &almostOutOfSessions )
{
  RTETask_TaskScheduler* pTaskScheduler     = m_KGS.TaskSchedulerList();
  RTETask_TaskScheduler* pBestTaskScheduler = 0;
  SAPDB_UInt4 totalUnconnectedTaskCount = 0;

  if (  m_strategy == Equalize )
  {
      SAPDB_UInt4 numOfTasks, bestNumOfTasks = 0;

      while ( pTaskScheduler )
      {
          numOfTasks = pTaskScheduler->UnconnectedTasksQueue(taskType)->NumberOfWaitingTasks();
          totalUnconnectedTaskCount += numOfTasks;

          if  ( bestNumOfTasks < numOfTasks )
          {
              bestNumOfTasks     = numOfTasks;
              pBestTaskScheduler = pTaskScheduler;
          }
          pTaskScheduler = pTaskScheduler->Next();
      }
  }
  else // m_strategy == Compress
  {
      while ( pTaskScheduler )
      {
          SAPDB_UInt4 numOfTasks;
          
          numOfTasks = pTaskScheduler->UnconnectedTasksQueue(taskType)->NumberOfWaitingTasks();
          totalUnconnectedTaskCount += numOfTasks;
          
          if ( numOfTasks != 0 )
          {
              if ( !pBestTaskScheduler )
              {
                  pBestTaskScheduler = pTaskScheduler;
              }

              if ( 1 < totalUnconnectedTaskCount ) // prevent from issuing event if single UKT is almost filled... 
              {
                  break;
              }
          }
          pTaskScheduler = pTaskScheduler->Next();
      }
  }

  almostOutOfSessions = ( 1 == totalUnconnectedTaskCount );

  return pBestTaskScheduler;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/