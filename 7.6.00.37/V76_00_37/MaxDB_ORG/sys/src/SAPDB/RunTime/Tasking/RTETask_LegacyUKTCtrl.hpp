/*!========================================================================

  @file         RTETask_LegacyUKTCtrl.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access old UKT control

                The following class provides methodes to access 
                old UKT control.

  @since        2006-03-06  11:40
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


#ifndef RTETASK_LEGACYUKTCTRL_HPP
#define RTETASK_LEGACYUKTCTRL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"
#include    "geo50_0.h"       // old task control

#if defined (_WIN32)
# include   "gos74.h"         /* nocheck */
#else
# include   "gen71.h"         /* nocheck */
# include   "gen72.h"         /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

# define PEER_FUNCTION_PTR    PFNREMOTEFUNC


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;
struct RTETask_TaskScheduler;
class  RTETask_ReadyToRunQueue;
class  RTETask_ReadyToRunQueueTS;

/*!---------------------------------------------------------------------------
  @class   RTETask_LegacyUKTCtrl
  @brief   Access old UKT control

           The following class provides methodes to access the old UKT
           control. The most variables of the old UKT control 
           structures have different writings on various 
           platforms like UNIX and Windows.
           To hide this differences and to provide access to these
           legacy values in new classes the following class should be used.
 ----------------------------------------------------------------------------*/

struct RTETask_LegacyUKTCtrl
{
public:
    /*!  
    @brief          Get the current legacy ukt control object

                    USE ONLY IF NO ANCHOR IS AVAILABLE! 

                    If a pointer to the current task object or to the
                    task control is known use tis way to get the current 
                    ukt control !

    @return         Pointer to the current legacy ukt control object or 
                    null if we are not running in UKT context.
     */
    static RTETask_LegacyUKTCtrl* Self();

    /*!
    @brief          Returns the number of currently active tasks
    */
    SAPDB_UInt4 &activeTasks(void)
    {
#       if defined (_WIN32)
         return (SAPDB_UInt4&)m_pUKTCtrl->ulActiveTasks;
#       else
         return m_pUKTCtrl->activeTasks;
#       endif
    }

    /*!
    @brief          Returns the number of task switches
    */
    SAPDB_UInt8 &taskSwitches(void)
    {
        return m_pUKTCtrl->taskSwitches;
    }

    /*!
    @brief          Returns the number of prioritized tasks 
                    currently in the run queue
    */
    SAPDB_UInt4  & prioritizedTasksInRunqueue (void)
    {
#       if defined (_WIN32)
         return (SAPDB_UInt4&)m_pUKTCtrl->ulImmediateDispatch;
#       else
         return m_pUKTCtrl->SelfDispatch;
#       endif
    }

    /*!
    @brief          Get the communication queue statistic counter
    */
    SAPDB_UInt8 &CommQueueCount()
    {
#       if defined (_WIN32)
         return m_DummyCmdCounter;
#       else
         return m_pUKTCtrl->cmd_counter;
#       endif
    }


    /*!
    @brief          Get the current request which woke up the task
                    from dispatcher.
    */
    DLQ_ELEMENT* WakeUPRequest() const
    {
#       if defined (_WIN32)
         return m_pUKTCtrl->pCurrReq;
#       else
         return m_pUKTCtrl->curr_quu;
#       endif
    }


    /*!
    @brief         Get the UKT ID (index)
    */
#   if defined (_WIN32)
     SAPDB_UInt4 &UKTID()
     {
         return (SAPDB_UInt4 &)m_pUKTCtrl->ulUKTIndex;
     }
#   else
     SAPDB_Int4  &UKTID()          
     {
         return m_pUKTCtrl->index;
     }
#   endif

    /*!
    @brief          The number of tasks are waiting for a region 
    */
    SAPDB_UInt4 &numRegWait(void)
    {
#       if defined (_WIN32)
         return (SAPDB_UInt4&)m_pUKTCtrl->ulNumRegWait;
#       else
         return m_pUKTCtrl->NumRegWait;
#       endif
    }

    /*!
    @brief          Returns the number of dispatcher calls made
    */
    SAPDB_UInt8 &dispatcherCalls(void)
    {
#       if defined (_WIN32)
         return m_pUKTCtrl->DispatcherCalls;
#       else
         return m_pUKTCtrl->disp_counter;
#       endif
    }


    /*!
    @brief          Get direct access to UKT control structure
    */
    operator UKT_CTRL() const
    {
        return m_pUKTCtrl;
    }

    /*!
    @brief          Enter dispatcher
    */
    void Dispatcher()
    {
#       if defined (_WIN32)
         sql74k_dispatcher ( m_pUKTCtrl );
#       else
         en71Dispatcher ( m_pUKTCtrl );
#       endif
    }


   /*!
    @brief          Wake up UKT from semaphore
    */
    void WakeUP() 
    {
#       if defined (_WIN32)
         sql72k_wake_ukt ( m_pUKTCtrl );
#       else
        e72_wake ( m_pUKTCtrl );
#       endif
    }

    /*!
    @brief          The current running task of the UKT
    */
    TASK_CTRL CurrTaskCtrl()
    {
#       if defined (_WIN32)
         return m_pUKTCtrl->pCTask;
#       else
         return m_pUKTCtrl->curr_task;
#       endif
    }

    /*! 
    @brief          Return the Task object. 
      
                    WE SHOULD NOT STORE THE THE TASKCTRL OR TASK
                    IN MEMBER VARIABLES BECAUSE IT MIGHT CHANGE DURING
                    A SCHEDULE OPERATION (LOAD BALANCING)
     */
    RTETask_Task*   CurrentTaskObject()
    {
        return CurrTaskCtrl()->pTaskObject;
    }

    /*!
    @brief          Return the TaskScheduler previously called UKT.
      
                    WE SHOULD NOT STORE THE THE UKT OR TASKSCHEDULER 
                    IN MEMBER VARIABLES BECAUSE IT MIGHT CHANGE DURING
                    A SCHEDULE OPERATION (LOAD BALANCING)
     */
    RTETask_TaskScheduler*  TaskSchedulerObject()
    {
        return m_pUKTCtrl->pTaskSchedulerObject;
    }

    /*!
    @brief         Get the state of UKT
    */
    volatile SAPDB_Int4 &UKTState()
    {
#       if defined (_WIN32)
         return m_pUKTCtrl->ThrdCtrlHeader.ThrdState;
#       else
         return m_pUKTCtrl->state;
#       endif
    }


private:
    // the following methodes are accessed by 'RTETask_SchedulerToSchedulerQueue'
    friend class RTETask_SchedulerToSchedulerQueue;
    /*!
    @brief          Calls a function in peer UKT related to a peer task

    @param    pTaskCtrl      [in]   - Pointer to peer task control.
    @param    pFuncToExecute [in]   - Function which is called in peer UKT.
    @param    pFuncParam     [in]   - Function parameter.
    */
    void EnqueueElementInUKTToUKTQueue ( TASK_CTRL          pTaskCtrl,
                                         PEER_FUNCTION_PTR  pFuncToExecute,
                                         void*              pFuncParam = 0 )
    {
#       if defined (_WIN32)
         sql73k_UToU_enqu( m_pUKTCtrl, CurrTaskCtrl(), pTaskCtrl->pUKT, pTaskCtrl, pFuncToExecute, pFuncParam );
#       else
         en71EnqueueUkt2UktAndWake ( m_pUKTCtrl, pTaskCtrl, CurrTaskCtrl(), pFuncToExecute, pFuncParam);
#       endif
    }

    /*!
    @brief          Enqueue a doubly linked queue element to run queue

                    Task is yielding suppress time measurement.

    @param    pQueueElement      [in]   - Pointer to queue element
    @param    pTaskControl       [in]   - Receiving task control.
    @param    pSenderTaskControl [in]   - Task control of the task enqueuing this element.
    @param    basePrio           [in]   - Task priority.
    */
    void EnqueueElementInRunQueueTaskYield ( DLQ_ELEMENT*       pQueueElement,
                                             TASK_CTRL          pTaskControl,
                                             TASK_CTRL          pSenderTaskControl,
                                             SAPDB_UInt4        basePrio )
    {
#       if defined (_WIN32)
         sql74k_YieldTaskRunEnqu ( m_pUKTCtrl, pTaskControl, pQueueElement, basePrio );
#       else
         e71_YieldTaskRunEnqu ( m_pUKTCtrl, pTaskControl, pQueueElement, basePrio  );
#       endif
    }


private:
    // the following methodes are accessed by 'RTETask_ReadyToRunQueue'
    friend class RTETask_ReadyToRunQueueTS;
    friend class RTETask_ReadyToRunQueue;
    /*!
    @brief          Enqueue a doubly linked queue element to run queue

    @param    pQueueElement      [in]   - Pointer to queue element
    @param    pTaskControl       [in]   - Receiving task control.
    @param    pSenderTaskControl [in]   - Task control of the task enqueuing this element.
    @param    basePrio           [in]   - Task priority.
    */
    void EnqueueElementInRunQueue ( DLQ_ELEMENT*       pQueueElement,
                                    TASK_CTRL          pTaskControl,
                                    TASK_CTRL          pSenderTaskControl,
                                    SAPDB_UInt4        basePrio )
    {
#       if defined (_WIN32)
         sql74k_RUN_enqu ( m_pUKTCtrl, pTaskControl, pSenderTaskControl, pQueueElement, basePrio );
#       else
         e71_EnquRun ( m_pUKTCtrl, pTaskControl, pSenderTaskControl, pQueueElement, basePrio  );
#       endif
    }


private:
    // the following methodes are accessed by 'RTETask_ExternalRequestsQueue'
    friend class RTETask_ExternalRequestsQueueTS;
    friend class RTETask_ExternalRequestsQueue;
    /*!
    @brief          Enqueue a doubly linked queue element to UKT queue

    @param    pQueueElement [in]  - Pointer to queue element
    @param    pTaskControl  [in]  - Receiving task control.
    */
    void EnqueueElementInUKTQueue ( DLQ_ELEMENT*       pQueueElement,
                                    TASK_CTRL          pTaskControl )
    {
#       if defined (_WIN32)
         sql73k_UKT_enqu ( m_pUKTCtrl, pTaskControl, pQueueElement );
#       else
         en71_ioc ( m_pUKTCtrl, pTaskControl, pQueueElement );
#       endif
    }

    /*!
    @brief          Enqueue a doubly linked queue element to UKT queue

    This queue element is UKT related and has no relation to a task!

    @param    pQueueElement [in]   - Pointer to queue element
    */
    void EnqueueElementInUKTQueue ( DLQ_ELEMENT*       pQueueElement )
    {
#       if defined (_WIN32)
         sql73k_UKT_enqu_no_task ( m_pUKTCtrl, pQueueElement );
#       else
         en71_iocNoTask( m_pUKTCtrl, pQueueElement );
#       endif
    }
private:
    /*! @brief    The following methodes can also be 
                  used by the task scheduler class */
    friend struct RTETask_TaskScheduler;

    RTETask_LegacyUKTCtrl(UKT_CTRL pUKTCtrl)
        : m_pUKTCtrl(pUKTCtrl) 
    {
        pUKTCtrl->pLegacyUKTCtrlObject = this;
    }

private:
    // don't allow copies
	inline   RTETask_LegacyUKTCtrl( const RTETask_LegacyUKTCtrl&c ) {;}; 

private:
# if defined (_WIN32)
    SAPDB_UInt8                  m_DummyCmdCounter;
# endif

  UKT_CTRL                       m_pUKTCtrl;  // old UKT control
};

#endif  /* RTETASK_LEGACYUKTCTRL_HPP */
