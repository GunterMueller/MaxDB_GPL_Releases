/*!========================================================================

  @file         RTETask_SchedulerToSchedulerQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        TaskScheduler to TaskScheduler queue handling.

                This queue is part of the scheduler mechanism and 
                contains the queue elements with requests to
                execute functions in other schedulers context.

  @since        2003-09-11  15:26
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


#ifndef RTETASK_SCHEDULERTOSCHEDULERQUEUE_HPP
#define RTETASK_SCHEDULERTOSCHEDULERQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"



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
  @class   RTETask_SchedulerToSchedulerQueue
  @brief   TaskScheduler to TaskScheduler queue handling.

           This queue is part of the scheduler mechanism and 
           contains the queue elements with requests to
           execute functions in other schedulers context.
           These requests can only be send between the schedulers.

 ----------------------------------------------------------------------------*/

class  RTETask_SchedulerToSchedulerQueue
{
public:
    /*!
    @brief    Constructor
    */
    RTETask_SchedulerToSchedulerQueue() : m_pTaskScheduler(0) {;}


    /*!
    @brief          Enqueue a function call request

    After enqueuing a wake up request is send to TaskScheduler.
    
    @param    task           [in]   - The task which receives that queue element
    @param    pFuncToExecute [in]   - Function which is called by the TaskSceduler
    @param    pFuncParam     [in]   - Function parameter
    */
    void EnqueueAndWakeUp ( RTETask_Task           &task,
                            PEER_FUNCTION_PTR      pFuncToExecute,
                            void*                  pFuncParam = 0 );

private:
    // don't allow copies
    inline   RTETask_SchedulerToSchedulerQueue( const RTETask_SchedulerToSchedulerQueue &c )
                : m_pTaskScheduler(c.m_pTaskScheduler) {;}; 

protected:
    RTETask_TaskScheduler* m_pTaskScheduler; // Refer to class RTETask_SchedulerToSchedulerQueueTS
};

/*!---------------------------------------------------------------------------
  @class   RTETask_SchedulerToSchedulerQueueTS

  @brief   TaskScheduler to TaskScheduler queue handling.

  This derived calls contains all methodes which are
  access by the TaskScheduler only

 ----------------------------------------------------------------------------*/

class  RTETask_SchedulerToSchedulerQueueTS : public RTETask_SchedulerToSchedulerQueue
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
};

#endif  /* RTETASK_SCHEDULERTOSCHEDULERQUEUE_HPP */
