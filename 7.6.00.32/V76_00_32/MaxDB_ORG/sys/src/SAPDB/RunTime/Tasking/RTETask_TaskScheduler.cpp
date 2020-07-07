/*!========================================================================

  @file         RTETask_TaskScheduler.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        The kernel task scheduler.

                The kernel task scheduler is a unit of execution where the 
                kernel tasks are runninig in. From a system standpoint, a 
                task assumes the identity of a thread that runs it.
                At least one task is runninig in a task scheduler context. 
                One ore more TaskScheduler are running in each database 
                instance.

  @since        2003-09-25  17:30
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

#include    "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_LegacyUKTCtrl.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"

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

inline SAPDB_Bool RTETask_TaskScheduler::IgnoreThisRequestOnSystemEnd( RTETask_CommonQueueElement  &queueElement )
{
    if (( m_KGS.DBState()                >= RTE_KGS::RTEKGS_StateShutdown) && 
        ( queueElement.Task()->Type() != RTETask_ITask::Tracewriter ))
    {
        queueElement.Task()->State() = RTETask_Task::TaskLocked;
        return true;
    }
    return false;
}


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTETask_TaskScheduler* RTETask_TaskScheduler::Self()
{
    RTETask_LegacyUKTCtrl* pLegacyUKTCtrl;

    pLegacyUKTCtrl = RTETask_LegacyUKTCtrl::Self();

    return (0 == pLegacyUKTCtrl) ? 0 : pLegacyUKTCtrl->TaskSchedulerObject();
}

/*---------------------------------------------------------------------------*/

RTE_TaskSchedulerId RTETask_TaskScheduler::ID()
{
    return m_LegacyUKTCtrl.UKTID();
}

/*---------------------------------------------------------------------------*/

tsp00_Longuint RTETask_TaskScheduler::SearchForCommunicationRequests( tsp00_Longuint  currTime )
{
    tsp00_Longuint                 waitTime;
    RTETask_CommonQueueElement*    pQueueElement;

    if ( m_CommQueue.QueueIsNotEmpty() )
    {
        switch ( m_KGS.DBState() )
        {
        case RTE_KGS::RTEKGS_StateStarting:
        case RTE_KGS::RTEKGS_StateAdmin   :
        case RTE_KGS::RTEKGS_StateStandby :
        case RTE_KGS::RTEKGS_StateOnline  :
            break;
        default :
            currTime = MAXTIMEVALUE;    // --- force timeout break;
            break;
        }
        waitTime = m_CommQueue.Dequeue( currTime, pQueueElement );

        if ( 0 != pQueueElement )
        {
            if ( false == IgnoreThisRequestOnSystemEnd(  *pQueueElement ) )
                m_ReadyToRunQueue.Enqueue( *pQueueElement, RTETask_ReadyToRunQueueTS::CommunicationPrio );
        }
    }
    else
        waitTime = MAXTIMEVALUE;

    return waitTime;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/