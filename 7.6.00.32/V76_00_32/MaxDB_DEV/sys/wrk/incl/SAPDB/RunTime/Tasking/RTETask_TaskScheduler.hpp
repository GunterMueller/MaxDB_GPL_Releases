/*!========================================================================

  @file         RTETask_TaskScheduler.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        The kernel task scheduler.

                The kernel task scheduler is a unit of execution where the 
                kernel tasks are runninig in. From a system standpoint, a 
                task assumes the identity of a thread that runs it.
                At least one task is runninig in a task scheduler context. 
                One ore more TaskScheduler are running in each database 
                instance.

  @since        2005-01-21  10:20
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


#ifndef RTETASK_TASKSCHEDULER_HPP
#define RTETASK_TASKSCHEDULER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_ITask.hpp"
#include    "RunTime/Tasking/RTETask_ITaskScheduler.hpp"
#include    "RunTime/Tasking/RTETask_CommunicationQueue.hpp"
#include    "RunTime/Tasking/RTETask_LegacyUKTCtrl.hpp"
#include    "RunTime/Tasking/RTETask_ReadyToRunQueue.hpp"
#include    "RunTime/Tasking/RTETask_ExternalRequestsQueue.hpp"
#include    "RunTime/Tasking/RTETask_SchedulerToSchedulerQueue.hpp"
#include    "RunTime/Tasking/RTETask_UnconnectedTasksQueue.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define RTE_TSK_SCHEDULER_STATE_INITIALIZE         "Initialized"
#define RTE_TSK_SCHEDULER_STATE_RUNNING            "Running    "
#define RTE_TSK_SCHEDULER_STATE_TERMINATED         "Terminated "
#define RTE_TSK_SCHEDULER_STATE_SLEEPING           "Sleeping   "
#define RTE_TSK_SCHEDULER_STATE_UNKNOWN            "UNKNOWN    "


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;


/*!---------------------------------------------------------------------------
  @class   RTETask_TaskScheduler
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

/* 
 We use a struct here to prevent casting caused by a C/C++ transition
*/
struct RTETask_TaskScheduler : public RTETask_ITaskScheduler
{
public:

    enum TaskSchedulerState
	{Initialized = KT_INITIALIZED,			///<Task Scheduler is initialized
     Running	 = KT_RUNNING,              ///<Task Scheduler is running                       
     Terminated  = KT_TERMINATED,           ///<Task Scheduler is terminated
	 Sleeping    = KT_SLEEPING				///<Task Scheduler is sleeping
	};

    /*!
    @brief         Constructor of the task scheduler object.

    Creates a task scheduler object. This is currently done during the 
    legacy UKT creation and does not lead to a new physical thread or 
    thread group. It's only attaching the legacy/old ukt control 
    structure to deal with it.

    @param    pUKTCtrl [in]     - Pointer to old UKT control
    @param    pNextInList [in]  - Pointer to the next TaskScheduler of a linked list
    */

    RTETask_TaskScheduler( UKT_CTRL                pUKTCtrl,
                           RTETask_TaskScheduler*  pNextInList ) 
         : m_KGS(RTE_KGS::Instance()),  // set shortcut to kgs
           m_LegacyUKTCtrl(pUKTCtrl),
           m_NextInList(pNextInList),
           m_pCurrentTask(0),
           m_UnconnectedCommonTasks("Unconnected common tasks"),
           m_UnconnectedSpecialTasks("Unconnected special tasks"),
           m_IsWaitingForNextScheduledTask(false)
    {
        m_CommQueue.SetTaskScheduler(this);
        m_ExternalRequestsQueue.SetTaskScheduler(this);
        m_ReadyToRunQueue.SetTaskScheduler(this);
        m_SchedulerToSchedulerQueue.SetTaskScheduler(this);
        m_SemaphoreIndex = ID() - 1;
    }

    /*!
    @brief         The tasks scheduler unique ID

                   Each task scheduler (UKT) has its own ID, 
                   which is unique to the database process.
    @return        task scheduler ID
    */
    virtual RTE_TaskSchedulerId ID();

    /*!
    @brief          Returns true if task scheduler waits for
                    the next scheduled task
    */
    SAPDB_Bool IsWaitingForNextScheduledTask()
    {
        return m_IsWaitingForNextScheduledTask;
    }


    /*!
    @brief          Returns the next task to be scheduled

    'NextScheduledTask' is normaly called by the task objects
    'WaitForReschedule' methode which switches to the next
    scheduled task. This methode is also be used during 
    task startup and finish operations.
    . 
    */
    RTETask_Task& NextScheduledTask();

    /*!
    @brief          Returns the number of task switches
    */
    SAPDB_UInt8  TaskSwitches()
    {
        return m_LegacyUKTCtrl.taskSwitches();
    }

    /*!
    @brief          Returns next task scheduler of the linked list of TaskScheduler objects.
    */
    RTETask_TaskScheduler* Next()
    {
      return m_NextInList; 
    }

   /*!
    @brief          Wake up task scheduler from semaphore
    */
    void WakeUP()
    {
        m_LegacyUKTCtrl.WakeUP();
    }

    /*!
    @brief   Get a reference to the task schedulers communication queue.
    */
    RTETask_CommunicationQueue&  CommQueue() 
    {
        return m_CommQueue;
    }

    /*!
    @brief   Get a reference to the task schedulers ready to run queue.
    */

    RTETask_ReadyToRunQueue&  ReadyToRunQueue()
    {
        return m_ReadyToRunQueue;
    }

    /*!
    @brief   Get a reference to the task schedulers ready to run queue.
    */

    RTETask_ExternalRequestsQueue&  ExternalRequestsQueue()
    {
        return m_ExternalRequestsQueue;
    }

    /*!
    @brief   Get a reference to the task schedulers 
             scheduler to scheduler queue.
    */

    RTETask_SchedulerToSchedulerQueue&  SchedulerToSchedulerQueue()
    {
        return m_SchedulerToSchedulerQueue;
    }

    
    /*!
    @brief         Get pointer to the task scheduler object we
                   are running in.

    USE ONLY IF NO ANCHOR IS AVAILABLE! 

    If a pointer to the current task object is known use the methode
    'TaskScheduler' of this task object. 

    @return         Pointer to the task scheduler object or 
                    null if not runínig in task context.
    */
    static RTETask_TaskScheduler* Self();

    /*!
    @brief   Get the current task object.
     */
    RTETask_Task&   CurrentTask()
    {
        return *m_pCurrentTask;
    }


    /*!
    @brief         Get pointer to the legacy UKT control object
    @return        Pointer to the legacy UKT control object
    */
    RTETask_LegacyUKTCtrl & LegacyUKTCtrl()
    {
        return m_LegacyUKTCtrl;
    }

    /*!
    @brief   Load current task object from legacy UKT control.
           
             We access the Task object via CurrentTaskObject() methode 
             of the legacy control object.
     */
    void  LoadCurrentTask()
    {
        m_pCurrentTask = m_LegacyUKTCtrl.CurrentTaskObject();
    }

    /*!
    @brief   The queue that holds all connectable tasks which are unconnected.

    This methode returns the pointer to a synchronized queue which is 
    holding all connectable tasks that have released all client connections.

    The requestor is dequeuing elements from this queue to fulfil client
    connect requests that are not specifying a dedicated task. If a task 
    has been disconnected from all clients it is enqueuing an common queue
    element into this queue to signal it is waiting for incomming connect
    requests.

    @param         taskType [in] - Type of task, like UserTask or EventTask.
                                   TaskType must be one of the connectable tasks
                                   otherwise this methode returns a null value.

    @return        Pointer to the unconnected common task queue. 
                   returns null if task type has no queue relatedness.
     */
    RTETask_UnconnectedTasksQueue* UnconnectedTasksQueue( RTETask_ITask::TaskType taskType )
    {
        switch (taskType) 
        {
        case RTETask_ITask::User:  return &m_UnconnectedCommonTasks;
        case RTETask_ITask::Event: return &m_UnconnectedSpecialTasks;
        }
        return 0;
    }

    /*!
    @brief         The tasks schedulers semaphore info index

                   Each task scheduler (UKT) holds its own 
                   semaphore information, inside a global shared
                   memory segment.This methode returns the slot 
                   index of the list of TaskScheduler semaphores.
                   The semaphore informations a part of the 
                   client kernel communication.

    @return        task scheduler semaphore info slot
    */
    virtual SAPDB_UInt4  SemaphoreIndex()
    {
        return m_SemaphoreIndex;
    }

    /*!
    @brief         Returns the task scheduler state

    @return        Task scheduler state
    */
    TaskSchedulerState &State()
    {
         return (TaskSchedulerState&)m_LegacyUKTCtrl.UKTState();
    }

    /*!
    @brief          Get the current queue element which woke up the task
                    from dispatcher.
    */
    RTETask_CommonQueueElement* WakeUPQueueElement() const
    {
        return ( 0 == m_LegacyUKTCtrl.WakeUPRequest() ) ? 0 : m_LegacyUKTCtrl.WakeUPRequest()->pQueueElementObject;
    }

private:
    /*!
    @brief         Search for tasks that have a communication packet received

                   A task waiting for communictaion has been insterted into
                   the communication queue. This methodes searches for tasks 
                   in the communication queue that have received a 
                   communictaion packet.  They will be insterted into the ready
                   to run queue.

    @param         currTime  [in]        - current time needed to check command timeouts.

    @return        TaskScheduler (semaphore) sleep time.
                   The maximum wait time until the next command timeout occures
                   (maximum TaskScheduler sleep time). 
    */
    tsp00_Longuint  SearchForCommunicationRequests( tsp00_Longuint  currTime );


    /*!
    @brief         This methode checks if this queue element must be ignored (on system end)

    @param         queueElement  [in] - queue element that has be checked.

    @return        Ignore this queue element do not enqueue into the ready to run queue.
    */
    inline SAPDB_Bool IgnoreThisRequestOnSystemEnd( RTETask_CommonQueueElement  &queueElement );

#if defined (_WIN32)
    /*!
    @brief         Search for requests from outside of the TaskScheduler context.

                   Any 'external' thread use this queue type to send a request 
                   (common queue element) to a task or to the TaskScheduler.
    */
    void SearchExternalRequestsQueue();
#endif

private:
    // don't allow copies
	inline   RTETask_TaskScheduler( const RTETask_TaskScheduler &c )
               : m_KGS(RTE_KGS::Instance()),
                 m_LegacyUKTCtrl(c.m_LegacyUKTCtrl),
                 m_UnconnectedCommonTasks("Unconnected common tasks"),
                 m_UnconnectedSpecialTasks("Unconnected special tasks"),
                 m_IsWaitingForNextScheduledTask(false)
                  {         
                      m_CommQueue.SetTaskScheduler(this);
                      m_ExternalRequestsQueue.SetTaskScheduler(this);
                      m_ReadyToRunQueue.SetTaskScheduler(this);
                      m_SchedulerToSchedulerQueue.SetTaskScheduler(this);
                  }

private:
  RTE_KGS                               &m_KGS;           // Shortcut to kgs
  RTETask_LegacyUKTCtrl                 m_LegacyUKTCtrl;  // methodes to access  the old ukt control
  RTETask_TaskScheduler*                m_NextInList;     // next in linked list of TaskScheduler

  RTETask_CommunicationQueueTS          m_CommQueue;
  RTETask_ReadyToRunQueueTS             m_ReadyToRunQueue;
  RTETask_ExternalRequestsQueueTS       m_ExternalRequestsQueue;
  RTETask_SchedulerToSchedulerQueueTS   m_SchedulerToSchedulerQueue;

  RTETask_UnconnectedTasksQueue         m_UnconnectedCommonTasks;
  RTETask_UnconnectedTasksQueue         m_UnconnectedSpecialTasks;

  RTETask_Task*                         m_pCurrentTask;
  SAPDB_Bool                            m_IsWaitingForNextScheduledTask;
  SAPDB_UInt4                           m_SemaphoreIndex;
};

/*!
  @brief Convert task scheduler state into readable string
  @param state [in] task scheduler state
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTETask_TaskScheduler::TaskSchedulerState     state)
{
    switch (state)
    {
    case RTETask_TaskScheduler::Initialized: 
        return SAPDB_ToStringClass(RTE_TSK_SCHEDULER_STATE_INITIALIZE);
    case RTETask_TaskScheduler::Running: 
        return SAPDB_ToStringClass(RTE_TSK_SCHEDULER_STATE_RUNNING);
    case RTETask_TaskScheduler::Terminated: 
        return SAPDB_ToStringClass(RTE_TSK_SCHEDULER_STATE_TERMINATED);
    case RTETask_TaskScheduler::Sleeping: 
        return SAPDB_ToStringClass(RTE_TSK_SCHEDULER_STATE_SLEEPING);
    default:                          
        break;
    }
    return SAPDB_ToStringClass(RTE_TSK_SCHEDULER_STATE_UNKNOWN); 
}
#endif  /* RTETASK_TASKSCHEDULER_HPP */
