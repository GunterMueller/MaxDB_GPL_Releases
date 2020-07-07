/*!========================================================================

  @file         RTETask_ITaskScheduler.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        The kernel task scheduler.

                The kernel task scheduler is a unit of execution where the 
                kernel tasks are runninig in. From a system standpoint, a 
                task assumes the identity of a thread that runs it.
                At least one task is runninig in a task scheduler context. 
                One ore more TaskScheduler are running in each database 
                instance.

  @since        2003-10-17  14:16
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


#ifndef RTETASK_ITASKSCHEDULER_HPP
#define RTETASK_ITASKSCHEDULER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/RTE_MessageList.hpp"

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
  @class   RTETask_ITaskScheduler
  @brief   The kernel task scheduler object.

           The kernel task scheduler is a unit of execution where the 
           kernel tasks are runninig in. From a system standpoint, a 
           task assumes the identity of a thread that runs it.
           At least one task is runninig in a task scheduler context. 
           One ore more TaskScheduler are running in each database 
           instance.
           The system still schedules the TaskScheduler to run. The 
           currently runninig task runs when its TaskScheduler 
           runs.
           Tasks are not preemptively scheduled. Scheduling a task is
           done by switching to it from another task. Which task is
           scheduled next is determined by the TaskScheduler.

           To optimize the the system work load the tasks can move
           from one task scheduler to another.
 ----------------------------------------------------------------------------*/

class RTETask_ITaskScheduler
{
public:

    /*!
    @brief         The tasks scheduler unique ID

                   Each task scheduler (UKT) has its own ID, 
                   which is unique to the database process.
    @return        task scheduler ID
    */
    virtual RTE_TaskSchedulerId ID() = 0;
};


#endif  /* RTETASK_ITASKSCHEDULER_HPP */
