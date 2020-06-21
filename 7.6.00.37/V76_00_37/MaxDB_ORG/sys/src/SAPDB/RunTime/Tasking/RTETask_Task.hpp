/*!========================================================================

  @file         RTETask_Task.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel task class.

                This class provides all kernel task related 
                methodes and data.

  @since        2006-03-14  17:34
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


#ifndef RTETASK_TASK_HPP
#define RTETASK_TASK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "RunTime/Tasking/RTETask_Task.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include "RunTime/Tasking/RTETask_TaskClientConnections.hpp"
#include "heo56.h" /* obsolete if own implementation for v... function... */
#include "RunTime/Tasking/RTETask_LocalStorage.hpp"


#if defined (_WIN32)
#else
# include   "gen88.h"  /* no check */
#endif

class RTESync_Spinlock;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! Fixed number of queue elements for internal requests (for special element 1 + 2 / qa + qb, task yield */
#define TASK_NUM_OF_QUEUE_ELEMENTS        4
#define HANDLE_REQUEST_TIMEOUT_INTERVAL   15


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SAPDBErr_MessageList;
class RTESync_IRWRegion;
class RTECKC_KernelCommunication;          // Used to reference global kernel communication objects


/*!---------------------------------------------------------------------------
  @class   RTETask_Task
  @brief   The kernel task object.

           The kernel task is a unit of execution where the database 
           kernel code runs in. It must be manually scheduled by the
           kernel code via a yield or sleep call or is implicit scheduled
           by one of the task related runtime methodes. From a system 
           standpoint, a task assumes the identity of a thread that runs
           it. This thread or group of threads (one thread per task, if 
           group of threads) is called TaskScheduler and is the execution
           unit for at least one task. If a task accesses TaskScheduler
           related  data, it is accessing the data of the TaskScheduler 
           that is running it. However, a task does not have all the same
           state information associated with it as that associated with a 
           TaskScheduler. The only state information maintained for a task
           is its stack,  a subset of its registers, and the task data
           provided by the task object. Tasks are not preemptively
           scheduled. Scheduling a task is done by switching to it from 
           another task. The system still schedules the TaskScheduler 
           to run. The currently runninig task runs when its TaskScheduler 
           runs.

           A task waiting for a operation to finish, doing a reschedule
           (yield) or waiting for a wake up triggered by another task 
           is doing a 'WaitForReschedule' call. A respective queue 
           element is enqueued in one of the TaskSchedulers queues to 
           set the taks free from it's wait call.

           A task sending a request (e.g. IO request) to another thread
           enqueues a queue element into the threads job queue before 
           doing the 'WaitForReschedule' call. If the requested thread
           has finished the job it stores a ready queue element into
           the 'TaskSchedulers' 'ExternalRequestsQueue' to set the 
           waiting task free.

 ----------------------------------------------------------------------------*/

/* 
  We use a struct here to prevent casting caused by a C/C++ transition
*/
struct RTETask_Task : public RTETask_ITask
{
public:
    /*!
    @brief    State of task
    */
    enum TaskState
    {
    TaskInitialized       = TSK_INITIALIZED_EO00,        // Task is initialized but not started
    TaskInactive          = TSK_INACTIVE_EO00,
    TaskMarkedForStartup  = TSK_MARKED_FOR_STARTUP_EO00, // Server task has been marked for startup
    TaskIsRunning         = TSK_RUNNING_EO00,            // Task is running
    TaskLCApplObjCalled   = TSK_DCOM_OBJ_CALLED_EO00,    // liveCache Application object called
    TaskLocked            = TSK_LOCKED_EO00,             // Task has been locked during kernel stop phase to prevent rescheduling
    TaskTerminated        = TSK_TERMINATED_EO00,         // Task has finised work.
    TaskWaitingForConnect = TSK_CONNECTWAIT_EO00,        // Task is waiting for connection
    TaskReceive           = TSK_VRECEIVE_EO00,
    TaskSend              = TSK_VREPLY_EO00,
    TaskCloseConnection   = TSK_VRELEASE_EO00,

    // New kernel interface calls...


    // Old kernel interface calls...
    TaskVDEVSIZE          = TSK_VDEVSIZE_EO00,
    TaskVATTACH           = TSK_VATTACH_EO00,
    TaskVDETACH           = TSK_VDETACH_EO00,
    TaskVFOPEN            = TSK_VFOPEN_EO00,
    TaskVFWRITE           = TSK_VFWRITE_EO00,
    TaskVFCLOSE           = TSK_VFCLOSE_EO00,
    TaskVSHUTDOWN         = TSK_VSHUTDOWN_EO00,
    TaskVBEGEXCL          = TSK_VBEGEXCL_EO00,
    TaskRunnable          = TSK_RUNNABLE_EO00,           // Task has done a reschedule but is still runnable. It's
                                                         // done to prevent a disadvantage of other task (refer to vendenxcl).
    TaskVENDEXCL          = TSK_VENDEXCL_EO00,
    TaskVSUSPEND          = TSK_VSUSPEND_EO00,
    TaskVWAIT             = TSK_VWAIT_EO00,
    TaskVSLEEP            = TSK_SLEEP_EO00,
    TaskVDIAGINIT         = TSK_VDIAGINIT_EO00,
    TaskVOPMSG            = TSK_VOPMSG_EO00,
    TaskASYNOPEN          = TSK_ASYNOPEN_EO00,
    TaskASYNCLOSE         = TSK_ASYNCLOSE_EO00,
    TaskASYNIO            = TSK_ASYNIO_EO00,
    TaskASYNWAITRead      = TSK_ASYNWAIT_READ_EO00,
    TaskASYNWAITWrite     = TSK_ASYNWAIT_WRITE_EO00,
    TaskIORead            = TSK_IO_READ_EO00,
    TaskIOWrite           = TSK_IO_WRITE_EO00,
    TaskASYNCNTL          = TSK_ASYNCNTL_EO00,
    TaskVRESCHEDULE_MSEC  = TSK_RESCHEDULE_MSEC_EO00,
    TaskVVECTORIO         = TSK_VVECTORIO_EO00,
    TaskVDUALVECTORIO     = TSK_VDUALVECTORIO_EO00,
    TaskVSTOP             = TSK_STOPPED_EO00,
    TaskEnteringRWRegion  = TSK_ENTERRWREGION_EO00,
    TaskLeavingRWRegion   = TSK_LEAVERWREGION_EO00,
    TaskIsYieldingOnSpinlock = TSK_YIELDING_ON_SPINLOCK_EO00
    };

public:
    /*!
    @brief         Constructor of the task object.

    Creates a task object. This is currently done during the legacy 
    task creation and does not lead to a new physical task. It's
    only attaching the legacy/old task control structure to deal with it.

    @param         id        [in]     - The tasks unique ID
    @param         type      [in]     - The tasks type
    @param         pTaskCtrl [in]     - Pointer to the old task control
    */
    RTETask_Task(  RTE_TaskId   id,
                   TaskType     type,
                   TASK_CTRL    pTaskCtrl );

    /*!
    @brief         The tasks unique ID

                   Each task has its own ID, which is unique
                   to the database process.
    @return        Task ID
    */
    virtual RTE_TaskId     ID() const { return m_ID; };

    /*!
    @brief         Returns the tasks type

                   The tasks have related to there assignment a 
                   task type information. 

    @return        Task type
    */
    virtual TaskType       Type() const { return m_Type; }


    /*!
    @brief         Set a display name for the task.   
                   If NULL is given for one of the strings both internal names are reseted to empty strings.

    @param         shortName [in] - Short display name or NULL. String is cut off after 7 characters.
                   longName [in]  - Long display name or NULL. String is cut off after 16 characters. 
                   
    @return        None
    */
    virtual void                    SetDisplayName  (SAPDB_Char const * const shortName = NULL, 
                                                     SAPDB_Char const * const longName = NULL);

    /*!
    @brief         Returns the tasks type as string
    @param         longVersion [in] - true (default): Type name is given as long version

    @return        Task type as string
    */
    inline char const * const       TypeAsString    (SAPDB_Bool     longVersion = true) const;

    /*!
    @brief         Returns the tasks state

                   The tasks state is the information about
                   the runtime code location the task has entered.

    @return        Task state
    */
    TaskState &State() 
    {
         return (TaskState&)m_LegacyTaskCtrl.TaskState();
    }


    /*!
    @brief         Returns the tasks state as string

    @return        Task state as string
    */
    virtual char const * const StateAsString();


    /*!
    @brief         'AllocateLocalStorage' allocates a task local storage index. 
                    Any task can subsequently use this index to store and retrieve
                    pointer values that are local to the task.

    @return        Task local storage index
    */
    virtual SAPDB_UInt4   AllocateLocalStorage();


    /*!
    @brief         'FreeLocalStorage' releases a task local storage index, making it available for reuse.

    @param         index [in]- Index that was allocated by the 'Alloc' methode.

    @return        False, if the index is invalid because its out of range or it has
                   not been allocated by 'AllocateLocalStorage'.
    */
    virtual SAPDB_Bool FreeLocalStorage( SAPDB_UInt4 index );


    /*!
    @brief         'SetLocalStorageValue' stores a value in the calling tasks's task local storage slot for 
                   the specified index. Each task has its own slot for each task local 
                   storage index.
    @param         index   [in]- Index that was allocated by the 'Alloc' methode.
    @param         dataPtr [in]- Pointer value to be stored in the task local storage slot 
                                 for the calling task.

    @return        False, if the index is invalid because its out of range or it has
                   not been allocated by 'AllocateLocalStorage'.
    */
    virtual SAPDB_Bool SetLocalStorageValue( SAPDB_UInt4 index,  void* dataPtr );


    /*!
    @brief         'GetLocalStorageValue' retrieves the value in the calling tasl's task local 
                   storage slot for the specified index. Each task has its own slot for 
                   each task local storage index.
    @param         index [in]- Index that was allocated by the 'Alloc' methode.

    @return        If the methode succeeds, the return value is the value stored in the 
                   calling tasks's task local storage slot associated with the specified index.

                   If the methode fails, the return value is NULL, because the index is out of 
                   range or it has not been allocated by 'AllocateLocalStorage'.
    */
    virtual void* GetLocalStorageValue( SAPDB_UInt4   index );


    /*!
    @brief         Returns the next task is the wait queue
                   Used for RWRegions only

    @return        pointer to the next task
    */
    RTETask_Task *&NextTaskInWaitQueue(void)
    {
        return m_nextTaskInWaitQueue;
    }

    /*!
    @brief         Returns true if the task is waiting for exclusive access to a RWRegion

    @return        true if the task is waiting for exclusive access, false if not
    */
    SAPDB_Bool   &TaskIsWaitingForExclusiveAccess(void)
    {
        return m_taskIsWaitingForExclusiveAccess;
    }

    /*!
    @brief         Returns true if the task is waiting for exclusive access to a RWRegion

    @return        true if the task is waiting for exclusive access, false if not
    */
    RTESync_IRWRegion *&RWRegion(void)
    {
        return m_pRWRegion;
    }

    /*!
    @brief         Force a reschedule task

                   The current task which is calling this methode,
                   enters the TaskScheduler that determines the next
                   to be scheduled.

    @param         allowLoadBalancing [in]     - The load balancing mechanism allows
                                                 the TaskScheduler to move a task to 
                                                 another TaskScheduler. To prevent this
                                                 operation during the Yield call, set this
                                                 parameter to false. This is necessary if
                                                 the surrounding code expecting the task
                                                 in the same TaskScheduler after this call.
    @return        None
    */
    virtual void                    YieldTask( SAPDB_Bool allowLoadBalancing = true );

    /*!
    @brief         Exclude this task from load balancing.

    The load balancing mechanism allows the TaskScheduler to move a 
    task to another TaskScheduler. To prevent this operation this
    methode is used. This is necessary if the code expecting the 
    task in the same TaskScheduler during the following operation.

    This is a counting call which means the same number of unlock
    calls must occur to include this task to the load balancing 
    process.

    @return        None
    */
    virtual void                    LockLoadBalancing();

    /*!
    @brief         Include this task to load balancing process.

    This is a counting call which means the same number of locks 
    and  unlock calls must occur to include this task to the 
    load balancing process.

    @return        None
    */
    virtual void                    UnlockLoadBalancing();

    /*!
    @brief         Get reference to the corresponding task 
                   scheduler interface object.

                   CAUTION:
                     If a task is rescheduled and is woken up from
                     'WaitForReschedule' call, the corresponding
                     TaskScheduler might have changed because of the
                     load balancing mechanism. So it is a good 
                     practice not to store a pointer or reference
                     to the TaskScheduler over the 'WaitForReschedule'
                     call. Optional there is a 'WaitForReschedule' 
                     variant, expecting a pointer to a TaskScheduler 
                     object reference which is modified if the 
                     TaskScheduler has changed.

    @return        Reference to TaskScheduler object
    */
    virtual RTETask_ITaskScheduler &ITaskScheduler();


    /*!
    @brief         Wait for rescheduling

    A task waiting for a operation to finish, doing a reschedule
    (yield) or waiting for a wake up triggered by another task 
    is doing a 'WaitForReschedule' call. A respective queue 
    element is enqueued in one of the TaskSchedulers queues to 
    set the taks free from it's wait call.

    A task sending a request (e.g. IO request) to another thread
    enqueues a queue element into the threads job queue before 
    doing the 'WaitForReschedule' call. If the requested thread
    has finished the job it stores a ready queue element into
    the 'TaskSchedulers' 'ExternalRequestsQueue' to set the 
    waiting task free.

    CAUTION:
      If a task is rescheduled and is woken up from
      'WaitForReschedule' call, the corresponding
      TaskScheduler might have changed because of the
      load balancing mechanism. So it is a good 
      practice not to store a pointer or reference
      to the TaskScheduler over the 'WaitForReschedule'
      call. Optional use the 'WaitForReschedule' variant 
      below which is expecting a pointer to a TaskScheduler 
      object reference that is modified if the TaskScheduler
      has changed.

    @return        The queue element which woke the task
                   (Task object member element, don't
                    compare pointers to check equality)
    */
    inline RTETask_CommonQueueElement& WaitForReschedule( SAPDB_Bool       cancelable = false,
                                                          SAPDB_ULong      timeout    = 0 );
    /*!
    @brief         Wait for rescheduling

    CAUTION:
      If a task is rescheduled and is woken up from
      'WaitForReschedule' call, the corresponding
      TaskScheduler might have changed because of the
      load balancing mechanism. If a local copy of 
      a TaskScheduler reference exists, needed over
      the 'WaitForReschedule' call, use this type of
      methode which modifies this reference if the
      TaskScheduler changes.

    @return        The queue element which woke the task
                   (Task object member element, don't
                    compare pointers to check equality)
    */
    inline RTETask_CommonQueueElement& WaitForReschedule( struct RTETask_TaskScheduler* &pTaskScheduler,
                                                          SAPDB_Bool                    cancelable = false,
                                                          SAPDB_ULong                   timeout    = 0  );

    /*!
    @brief         Get pointer to the current task object.

    USE ONLY IF NO ANCHOR IS AVAILABLE! 

    If a pointer to the current task scheduler object 
    is known use the methode  'CurrentTask' of this task
    scheduler object. 

    @return         Pointer to the current task object or 
                    null if not runínig in task context.
    */
    static RTETask_Task* Self()
    {
        TASK_CTRL  pTaskCtrl; 

#   if defined (_WIN32)
        PUKT_CTRL_REC  pUKT = THIS_UKT_CTRL;

        if (( pUKT ) && ( pUKT->pCTask ))
            pTaskCtrl = pUKT->pCTask;
        else
            return NULL; /* called from other type of thread */
#   else
        pTaskCtrl = RTE_GetOwnTask();
        if(0 == pTaskCtrl)
            return NULL;
#   endif

#   ifdef SAPDB_SLOW
        if(NULL == pTaskCtrl->pLegacyTaskCtrlObject)
            return NULL;
#   endif
        return pTaskCtrl->pLegacyTaskCtrlObject->TaskObject();
    }

    /*!  
    @brief          Get the task object of a given taskid

    @return         Pointer to the task object or 
                    null if unknown.
    */
    static RTETask_Task* ByTaskID(  RTE_TaskId taskID )
    {
        if(0 != taskID)
        {
            TASK_CTRL  pTaskCtrl;

#   if defined (_WIN32)
            pTaskCtrl = &(kgs.pFirstTaskCtrl[taskID - 1]) ;
#   else
            pTaskCtrl = kgs->pFirstTaskCtrl + taskID - 1 ;
#   endif
            return pTaskCtrl->pTaskObject;
        }
        else
            return 0;
    }

    /*!
      @brief set the task state to yield on spinlock, if not already set

      A task can indicate by this interface, that it is yielding to acquiere the given spinlock.
      This call is part of the interface, to allow liboms to indicate its spinlock yield entry.

      @param pLock [in] pointer to spinlock 
      @param beginning [in] flag to indicate yield begins or ends
      @return none
     */
    static void SetTaskStateToYieldOnSpinlock(RTESync_Spinlock const *pLock, bool beginning)
    {
        RTETask_Task *pTask = RTETask_Task::Self();
        if ( pTask )
        {
            if ( beginning )
            {
                if ( TaskIsRunning == pTask->State() )
                {
                    *pTask->m_LegacyTaskCtrl.NameOfSpinlockYieldingFor() = 0;
                    if ( pLock )
                    {
                        if ( pLock->GetName() )
                        {
                            SAPDB_strncpy_zero_terminated( pTask->m_LegacyTaskCtrl.NameOfSpinlockYieldingFor()
                                , (SAPDB_Char const *)pLock->GetName()
                                , 20);
                        }
                        else
                        {
                            SAPDB_strncpy_zero_terminated( pTask->m_LegacyTaskCtrl.NameOfSpinlockYieldingFor()
                                , (const SAPDB_Char *)SAPDB_ToString((const void *)pLock, _T_p)
                                , 20);
                        }
                    }
                    else
                    {
                        *pTask->m_LegacyTaskCtrl.NameOfSpinlockYieldingFor() = 0;
                    }
                    pTask->State() = TaskIsYieldingOnSpinlock;
                }
            }
            else
            {
                if ( TaskIsYieldingOnSpinlock == pTask->State() )
                {
                    *pTask->m_LegacyTaskCtrl.NameOfSpinlockYieldingFor() = 0;
                    pTask->State() = TaskIsRunning;
                }
            }
        }
    }

    /*!
    @brief         Get reference to the corresponding task 
                   scheduler object.

                   CAUTION:
                     If a task is rescheduled and is woken up from
                     'WaitForReschedule' call, the corresponding
                     TaskScheduler might have changed because of the
                     load balancing mechanism. So it is a good 
                     practice not to store a pointer or reference
                     to the TaskScheduler over the 'WaitForReschedule'
                     call. Optional there is a 'WaitForReschedule' 
                     variant, expecting a pointer to a TaskScheduler 
                     object reference which is modified if the 
                     TaskScheduler has changed.

    @return        Reference toTaskScheduler object
    */
    RTETask_TaskScheduler&   TaskScheduler()
    {
        return *m_pTaskScheduler;
    }

    /*!
    @brief         Get pointer to the legacy task control object
    @return        Pointer to the legacy task control object
    */
    RTETask_LegacyTaskCtrl & LegacyTaskCtrl()
    {
        return m_LegacyTaskCtrl;
    }

    /*!
    @brief          Get direct access to task control structure
    */
    operator TASK_CTRL()
    {
        return (TASK_CTRL)m_LegacyTaskCtrl;
    }

    /*!
    @brief         Get reference to a task related common 
                   queue element.

                   This queue elements should only be
                   used for task related operations. 

                   THIS METHODE USES THE TASKS COMMON QUEUE 
                   ELEMENT FREE LIST WHICH IS NOT SYNCHRONIZED
                   AND SHOULD ONLY BE ACCESSED BY THE TASK ITSELF
                   OR A TASK THAT BELONGS TO THE SAME TASK SCHEDULER.

                   If a none task scheduler thread needs to wakeup/request
                   a task the global free list of the KGS object has to be
                   used!

                   After the call to 'WaitForReschedule' the task 
                   has to release the wakeup queue element by calling
                   the queue elements methode 'Release'.

    @return        Common task queue element.
    */
    RTETask_CommonQueueElement&  CommonQueueElementFromFreeList()
    {
        SAPDBERR_ASSERT_STATE( m_pTaskScheduler == RTETask_TaskScheduler::Self() );

        // We can cast it to a reference because the free list crashes 
        // if no element is available
        return *m_FreeList.Dequeue();
    }

    /*!
    @brief   Load TaskScheduler object the task belongs to.
           
             This methode will be called by the constructor and 
             by the TaskScheduler during task movement.

             We access the TaskScheduler object via 
             TaskSchedulerObject() methode  of the legacy 
             control object.
      
             WE SHOULD NOT STORE THE THE UKTCTRL OR TASKSCHEDULER 
             OBJECT IN MEMBER VARIABLES BECAUSE IT MIGHT CHANGE DURING
             A SCHEDULE OPERATION (LOAD BALANCING)
     */
    void  LoadTaskScheduler()
    {
        m_pTaskScheduler = m_LegacyTaskCtrl.TaskSchedulerObject();
    }

    /*!
    @brief          Enqueue a doubly linked queue element for this task 
                    to the external requests queue

    After enqueuing a wake up request is send to TaskScheduler.

    @param    queueElement [in]   - Reference to queue element
    @param    senderTask   [in]   - The task which sends that queue element
    */
    void EnqueueIntoExternalRequestsQueueAndWakeUP( RTETask_CommonQueueElement   &queueElement,
                                                    RTETask_Task                 &senderTask )
    {
        m_pTaskScheduler->ExternalRequestsQueue().EnqueueAndWakeUp( queueElement, *this, senderTask );
    }

    /*!
    @brief          Enqueue a doubly linked queue element for this task 
                    to the external requests queue

    After enqueuing a wake up request is send to TaskScheduler.
    This methode should not be used by tasks. Tasks should
    use the methode above which gets a sender task information. 

    Only threads like the requestor use this methode!

    @param    queueElement [in]   - Reference to queue element
    */
    void EnqueueIntoExternalRequestsQueueAndWakeUP( RTETask_CommonQueueElement   &queueElement )
    {
        m_pTaskScheduler->ExternalRequestsQueue().EnqueueAndWakeUp( queueElement, *this );
    }

    /*!
    @brief          Enqueue a common queue element for this task 
                    to the ready to run queue

    @param    queueElement [in]   - Reference to queue element
    @param    senderTask   [in]   - The task which sends that queue element
    @param    basePrio     [in]   - Task priority.
    */
    void EnqueueIntoReadyToRunQueue( RTETask_CommonQueueElement             &queueElement,
                                     RTETask_Task                           &senderTask,
                                     RTETask_ReadyToRunQueue::BasePriority   basePrio = RTETask_ReadyToRunQueue::WakeUpPrio )
    {
        SAPDBERR_ASSERT_STATE( m_pTaskScheduler == RTETask_TaskScheduler::Self() );

        m_pTaskScheduler->ReadyToRunQueue().Enqueue( queueElement, *this, senderTask, basePrio );
    }

    /*!
    @brief         Enqueue a common queue element for this task 
                   to the communication queue.
    @param         queueElement [in] - Common queue element.
    */
    void EnqueueIntoCommunicationQueue( RTETask_CommonQueueElement  &queueElement )
    {
        m_pTaskScheduler->CommQueue().Enqueue( queueElement, *this );
    }
    /*!
    @brief          Enqueue a function call request

    After enqueuing a wake up request is send to TaskScheduler.
    
    @param    pFuncToExecute [in]   - Function which is called by the TaskSceduler
    @param    pFuncParam     [in]   - Function parameter
    */
    void EnqueueIntoSchedulerToSchedulerQueueAndWakeUp( PEER_FUNCTION_PTR      pFuncToExecute,
                                                        void*                  pFuncParam = 0 )
    {
        m_pTaskScheduler->SchedulerToSchedulerQueue().EnqueueAndWakeUp( *this,
                                                                        pFuncToExecute,
                                                                        pFuncParam );
    }

    /*!
    @brief          Enqueue into the unconnect tasks queue

    Enqueue task into the queue of unconnected tasks of the TaskScheduler. If the
    task type is not connectable this call is ignored
    */
    void EnqueueIntoUnconnectedTasksQueue();


    /*!
    @brief         Get reference to the client connection object

                   This object collects all connection related methodes and 
                   data that belongs to a task that is doing client kernel 
                   communication.

    @return        Reference to client connection object.
    */
    RTETask_TaskClientConnections&  ClientConnections()
    {
        return m_ClientConnections;
    }

   /*! 
    @brief  Waits for a client connection
            
            Call this methode to join incoming client connects. 
            This methode blocks until a client connect will be 
            joined to this tasks connect request. The returned 
            client connection object is used to handle the new 
            created connection.
            
    @return   Object used to handle the connection
    */
    virtual RTETask_ITaskClientConnection * const AcceptClientConnect();

    /*! 
    @brief    Prepare the enqueue phase of a interruptible request.

              If the request that has to be enqueued is interruptible by a cancel or timeout
              (refer to RW-Regions), this methode has to be called to decide if the 
              enqueue operation is allowed.
              
    @return   true:  Prepare succeeded, enqueuing the request is allowed.
              false: A cancel or timeout request has been inserted
                     into the run queuue. Don't send the request!

    */
    inline SAPDB_Bool PrepareEnqueueOfInterruptibleRequest();

    /*! 
    @brief  Signal that a cancel, timeout or interruptible request has been handled.

            Call this metode after the 'WaitForReschedule' has returned a request 
            and all internal states are signaling we are not longer ready to receive
            a request!!!!
    */
    inline void EndOfInterruptibleRequest();

    /*! 
    @brief    Prepare the enqueue phase of a cancel request.

              This methode is used by the TaskScheduler to prepare 
              the enqueue opertation of cancel request.
              
    @return   true:  Prepare succeeded, enqueuing the cancel
                     request is allowed.
              false: The standard request has been inserted into the run queuue.
                     Don't send the cancel or timeout request!

    */
    inline SAPDB_Bool PrepareEnqueueOfCancelRequest();


    /*! 
    @brief    Prepare the enqueue phase of a timeout.

              This methode is used by the TaskScheduler to prepare 
              the enqueue opertation of timeout request.
              
    @return   true:  Prepare succeeded, enqueuing the timeout 
                     request is allowed.
              false: The standard request has been inserted into the run queuue.
                     Don't send the cancel or timeout request!

    */
    inline SAPDB_Bool PrepareEnqueueOfTimeoutRequest();

     /*! 
    @brief    Check if a request timed out. Used by the timer thread to check if 
              a timeout request should be send.
            
    @return   true if the request timed out
    */
    inline void HandleRequestTimeout();


     /*! 
    @brief  Check if cancellation request is pending
            
    @return   true if cancel byte is set, false otherwise
    */
    virtual SAPDB_Bool CancelPending()
    {
        return m_LegacyTaskCtrl.CancelPending();
    }

   /*! 
    @brief  Set cancel request to true
    */
    virtual void Cancel()
    {
        if ( !m_LegacyTaskCtrl.CancelPending() )
        {
            m_LegacyTaskCtrl.Cancel();

            RTETask_CommonQueueElement* pQueueElement = m_KGS.GlobalFreeList()->Dequeue();
            pQueueElement->Init( RTETask_CommonQueueElement::CancelTask );
            EnqueueIntoExternalRequestsQueueAndWakeUP( *pQueueElement );
        }
    }

    /*! 
    @brief  Set cancel request to false
    */
    virtual void ResetPendingCancel()
    {
        m_LegacyTaskCtrl.ResetPendingCancel();
    }


   /*! 
    @brief      Call this methode to check tasks inactive state.
    @return     true if task not active
    */
    SAPDB_Bool TaskIsInactive()
    {
#if defined (_WIN32)
        return  ( TaskInactive == State() ) || ( TaskInitialized == State() );
#else
        return  ( TaskInactive == State() ) || ( TaskInitialized == State() ) || ( TaskMarkedForStartup == State() );
#endif
    }

   /*! 
    @brief  Startup the task if inactive
            
            Call this methode to startup a task.
            If the task is not inactive this call does nothing.

    @param          messageList  [out] - Error messages

    @return         true if no error
    */
    SAPDB_Bool Startup( SAPDBErr_MessageList &messageList );

    /*! 
    @brief  Signal that the startup phase of the task has finished.
            Has to be called by the kernel main code.
    */
    virtual void SetStartupComplete () 
    {
        m_StartupComplete = true;
    }

    /*! 
    @brief  Check if the startup phase of the task has finished.
    @return true if the startup phase has finished, false otherwise
    */
    SAPDB_Bool StartupComplete () 
    {
        return m_StartupComplete;
    }


    /*!
    @name  Replacement for old task synchronization primitives
    @{
     */

    /*! @todo Discussion needed for 'SAPDB_Bool + SAPDBErr_MessageList &errList'... The kernel implementation
        currently does not know about 'errors....' and expect the runtime to ABORT() in such a case...
        This actually may happen controlled only in slowknl, but crashes normally the fast kernel... */
    /*!
    @brief         The tasks stops itself

       The task will continue only, when the RTETask_ITask::ContinueOther(otherTask) is called where
       otherTask references this task. An internal counter is used for ContinueOther() which means that
       if the 'ContinueOther()' was called before the Stop() was called, the Stop() immediatly
       returns without blocking.

       Replaces vstop( tsp00_TaskId pid )
    */
    virtual void Stop()
    {
        vstop(ID());
    }

    /*!
    @brief         The tasks suspends itself

       The task will suspend until the ResumeOther(otherTask) is called where
       otherTask references this task. An internal ternary flag is used for each task,
       which means that if the 'ResumeOther()' was called before the Suspend() was called,
       the Suspend() immediatly returns without blocking, but two ResumeOther() specifing
       the same task would fail.

       Replaces vsuspend( tsp00_TaskId pid, tsp00_Int2 suspend_reason )

    @param suspendReason [in] An 'well defined' index of suspend reasons
    */
    virtual void Suspend( SAPDB_UInt4 suspendReason )
    {
        vsuspend(ID(), (SAPDB_Int2) suspendReason);
    }

    /*!
    @brief         The tasks suspends itself specifying a page lock

        Almost identical with Suspend, but additional page information is given for console output.

    Replaces vSuspendForPageLock( tsp00_TaskId pid, tsp00_Int2 suspend_reason, tsp00_Int4 page_no )

    @param suspendReason [in] An 'well defined' index of suspend reasons
    @param pageNumber [in] the page which caller is waiting for
    */
    virtual void SuspendForPageLock( SAPDB_UInt4        suspendReason
                                   , SAPDB_Int4         pageNumber )
    {
        vSuspendForPageLock(ID(), (SAPDB_Int2) suspendReason, pageNumber);
    }

    /*!
    @brief         Resume a task, that suspended itself.
    
       The task is expected to be using Suspend(). This call is allowing a single outstanding resume
       only. That means if called twice for the same task without an intermediate Suspend() of that task,
       a programming error occured...

       Replaces vresume( tsp00_TaskId pid )

    @param otherTask [in] the task that can continue
    */
    virtual void ResumeOther( RTETask_ITask &otherTask )
    {
        vresume(otherTask.ID());
    }

    /*!
    @brief         Checks if a resume is pending for given task

       The given task is checked for an pending resume

       Replaces visresume( tsp00_TaskId pid)

    @param otherTask [in] the task that can continue
    @return true, if resume pending, false if not
    */
    virtual SAPDB_Bool IsResumePending(RTETask_ITask &otherTask)
    {
        return (0 != visresume(otherTask.ID()));
    }

    /*!
    @brief         The tasks waits until a signal is received

       The task will suspend until the SignalOther(otherTask,value) is called where
       otherTask references this task. An internal ternary flag is used for each task,
       which means that if the 'SignalOther()' was called before the Wait() was called,
       the Wait() immediatly returns without blocking, but two SignalOther() specifing
       the same task would fail.

       Replaces vwait( tsp00_TaskId pid, tsp00_Bool *pOk)

    @param signaledFlag [out] returns the signaled value
    */
    virtual void Wait( SAPDB_Bool1 &signaledFlag )
    {
        vwait(ID(), signaledFlag);
    }

    /*!
    @brief         Signal another task which is waiting

       The task is expected to be using Wait(). This call is allowing a single outstanding wait
       only. That means if called twice for the same task without an intermediate Wait() of that task,
       a programming error occured...

       Replaces vsignal( tsp00_TaskId pid, tsp00_Bool ok )

    @param otherTask [in] the task that is signaled
    @param signaledFlag [in] returns the signaled value
    */
    virtual void SignalOther( RTETask_ITask &otherTask
                                , SAPDB_Bool1 signaledFlag )
    {
        vsignal(otherTask.ID(), signaledFlag);
    }

    /*!
    @brief         Let the task sleep for a given time period

    The task issues an internal job request to the timer thread and reschedules.

       Replaces vsleep( tsp00_TaskId pid, tsp00_Int2 limit )

    @param timePeriodInSeconds [in] the number of seconds to sleep
    */
    virtual void Sleep( SAPDB_Int2 timePeriodInSeconds )
    {
        vsleep(ID(), timePeriodInSeconds);
    }

    /*!
    @brief         Let the task reschedule and measure time it needed

    The task issues an internal reschedule request.

    If StartMilliSeconds was given as 0, the actual time before reschedule is stored in StartMilliSeconds
    After rescheduling the actual time is taken.

    If the difference between actual time and StartMilliSeconds is larger than RemainMilliSeconds or the 
    actual time is less than StartMilliSeconds, RemainMilliSeconds is returned as zero.

    Otherwise that difference is subtraced from RemainMilliSeconds and StartMilliSeconds is set to the actual
    time after reschedule.

    The idea is to do allow 'active' polling for a given time period specified in RemainMilliSeconds and to loop
    until RemainMilliSeconds is returned as zero.

    @note This call does NOT allow an exact waiting for a specified number of milliseconds.

       Replaces vreschedule_msec( tsp00_TaskId pid, tsp00_Int4 *StartMsec, tsp00_Int4 *RemainMsec )

    @param StartMilliSeconds [inout] the number of seconds to sleep
    @param RemainMilliSeconds [inout] the remaining time im milliseconds
    */
    virtual void RescheduleMilliSeconds( SAPDB_Int4 &StartMilliSeconds
                                       , SAPDB_Int4 &RemainMilliSeconds )
    {
        vreschedule_msec(ID(), StartMilliSeconds, RemainMilliSeconds);
    }


    /*!
    @brief         Same methods as 'Type ()' method beside that this method can be called in the UNIX console
                   because it is not virtual
    */
    TaskType                GetTaskType() const
    {
        return m_Type;
    }


    /*!
    @brief         Same methods as 'ID ()' method beside that this method can be called in the UNIX console
                   because it is not virtual
    */
    RTE_TaskId     GetTaskID() const { return m_ID; };

private:
    // private, don't allow copies
    RTETask_Task( RTETask_Task &c ) 
               : m_KGS(RTE_KGS::Instance()),
                 m_LegacyTaskCtrl(c.m_LegacyTaskCtrl),
                 m_pTaskScheduler(0),
                 m_FreeList( m_CommonQueueElement, 
                             TASK_NUM_OF_QUEUE_ELEMENTS, 
                             "TaskCommonQueueFreeList" ) {;}

    // private, don't allow copies
    RTETask_Task & operator=(const RTETask_Task& c ) { return *this;}


#if ASSERT_STATE_OPTION != SAPDBERR_OFF
public:
    RTETask_CommonQueueElement& StartupElement() { return m_StartupElement; }
#endif

private:
    RTE_TaskId                          m_ID;                                             ///< Task specific identfier
    TaskType                            m_Type;                                           ///< The tasks have related to there assignment a task type information.

    RTETask_TaskClientConnections       m_ClientConnections;                              ///< Client connection information
    RTE_KGS &                           m_KGS;                                            ///< Shortcut to kgs
    RTETask_TaskScheduler*              m_pTaskScheduler;                                 ///< The TaskScheduler the task belongs to.
    RTETask_LegacyTaskCtrl              m_LegacyTaskCtrl;                                 ///< Methodes to access old task control

    RTETask_CommonQueueElement          m_CommonQueueElement[TASK_NUM_OF_QUEUE_ELEMENTS]; ///< Common queue elements of the following free list
    RTETask_CommonQueueElement          m_StartupElement;                                 ///< Common queue element used for task startup
    RTETask_CommonQueue                 m_FreeList;                                       ///< Unsynchronized task related common queue element free list

    RTETask_Task *                      m_nextTaskInWaitQueue;
    SAPDB_Bool                          m_taskIsWaitingForExclusiveAccess;
    RTESync_IRWRegion *                 m_pRWRegion;

    RTE_TaskDisplayName                 m_DisplayName;                                    ///< Long display name for the task type (only available for server tasks) 
    RTE_TaskDisplayNameShort            m_DisplayShortName;                               ///< Short display name for the task type (only available for server tasks) 

    RTETask_LocalStorage                m_TaskLocalStorage;                               ///< Allows to store unique copy of a data for each task
    SAPDB_Bool                          m_StartupComplete;
    SAPDB_Bool                          m_AlreadyYielding;

    SAPDB_Bool                          m_TimeoutRequestPending;
    SAPDB_ULong                         m_SendTimeOutRequestAt;
    SAPDB_Bool                          m_CancelRequestAllowed;
    SAPDB_Bool                          m_TimeoutRequestAllowed;
    SAPDB_Bool                          m_CancelOrTimeoutRequestEnqueued;
    SAPDB_Bool                          m_InterruptibleRequestEnqueued;
    RTECKC_KernelCommunication *        m_pKernelCommunication;
};


/*---------------------------------------------------------------------------*/

inline char const * const RTETask_Task::TypeAsString(SAPDB_Bool     longVersion) const
{
    if ( longVersion )
    {
        switch ( m_Type )
        {
        case Timer            : return RTE_TSK_TYPE_NAME_TI;
        case Logwriter        : return RTE_TSK_TYPE_NAME_AL;
        case Tracewriter      : return RTE_TSK_TYPE_NAME_TW;
        case Datawriter       : return RTE_TSK_TYPE_NAME_DW;
        case Utility          : return RTE_TSK_TYPE_NAME_UT;
        case Event            : return RTE_TSK_TYPE_NAME_EV;
        case GarbageCollector : return RTE_TSK_TYPE_NAME_GC;
        case Backup           : return RTE_TSK_TYPE_NAME_BUP;
        case User             : return RTE_TSK_TYPE_NAME_US;
        case Server           : 
            {
                if ('\0' != m_DisplayName[0])
                {
                    return m_DisplayName + 0;
                }
                else
                {
                    return RTE_TSK_TYPE_NAME_SV;
                }
            }
        }
    }
    else
    {
        switch ( m_Type )
        {
        case Timer            : return RTE_TSK_TYPE_SHORTNAME_TI;
        case Logwriter        : return RTE_TSK_TYPE_SHORTNAME_AL;
        case Tracewriter      : return RTE_TSK_TYPE_SHORTNAME_TW;
        case Datawriter       : return RTE_TSK_TYPE_SHORTNAME_DW;
        case Utility          : return RTE_TSK_TYPE_SHORTNAME_UT;
        case Event            : return RTE_TSK_TYPE_SHORTNAME_EV;
        case GarbageCollector : return RTE_TSK_TYPE_SHORTNAME_GC;
        case Backup           : return RTE_TSK_TYPE_SHORTNAME_BUP;
        case User             : return RTE_TSK_TYPE_SHORTNAME_US;
        case Server           : 
            {
                if ('\0' != m_DisplayShortName[0])
                {
                    return m_DisplayShortName + 0;
                }
                else
                {
                    return RTE_TSK_TYPE_SHORTNAME_SV;
                }
            }
        }
    }
    return RTE_TSK_TYPE_NAME_UNKNOWN;
}


/*!---------------------------------------------------------------------------
  @class   RTETask_ScopeRelatedTaskState
  @brief   Set task state scope related

           The class allows to set the task state scope dependent.
           Creating an instance of this class inside a code scope 
           will restore the prior task state when the scope is left. 

           Example : 

           {
           RTETask_ScopeRelatedTaskState tempState (task, RTETask_Task::TaskReceive );

           ....
           } // now the tasks state has been retored

 ----------------------------------------------------------------------------*/

class RTETask_ScopeRelatedTaskState
{
public:
    /*!
    @brief         Constructor.

                   Creating an instance of this class inside a code scope 
                   will restore the prior task state when the scope is left. 

    @param         pTask [in]      - task object
    @param         newState [in]   - the temporary task state
    */
    RTETask_ScopeRelatedTaskState( RTETask_Task*           pTask,
                                   RTETask_Task::TaskState newState ) 
        : m_OldState(pTask->State()),
          m_pTask(pTask)
    {
        m_pTask->State() = newState;
    }

    /*! 
    @brief Destructor retores the old task state
    */
    ~RTETask_ScopeRelatedTaskState() 
    {
        m_pTask->State() = m_OldState;
    }
private:
    RTETask_Task::TaskState  m_OldState;
    RTETask_Task*            m_pTask;
};


/*---------------------------------------------------------------------------*/

inline RTETask_CommonQueueElement& RTETask_Task::WaitForReschedule( SAPDB_Bool       cancelable,
                                                                    SAPDB_ULong      timeout )
{
    if ( cancelable )
    {
        if ( !m_InterruptibleRequestEnqueued )  // standard request already enqueued?
        {
            if ( CancelPending() ) // cancel already set?
            {
                RTETask_CommonQueueElement& element = CommonQueueElementFromFreeList();
                element.Init( RTETask_CommonQueueElement::CancelTask );
                return  element;
            }
            m_CancelRequestAllowed = true;  // allow cancel or timeout requests
        }
    }
    if ( timeout )
    {
        m_SendTimeOutRequestAt  = m_KGS.CurrTime() + timeout;
        m_TimeoutRequestAllowed = true;
        m_TimeoutRequestPending = false;
    }

    m_LegacyTaskCtrl.GotoDispatcher( m_pTaskScheduler->LegacyUKTCtrl() );

    if ( cancelable )
        m_CancelRequestAllowed = false;  // we have finished, disable cancelation

    if ( timeout )
    {
        m_TimeoutRequestAllowed = false;
        m_SendTimeOutRequestAt  = 0;
    }

    return *m_pTaskScheduler->LegacyUKTCtrl().WakeUPRequest()->pQueueElementObject;
}

/*---------------------------------------------------------------------------*/

inline RTETask_CommonQueueElement& RTETask_Task::WaitForReschedule( struct RTETask_TaskScheduler* &pTaskScheduler,
                                                                    SAPDB_Bool                    cancelable,
                                                                    SAPDB_ULong                   timeout )
{
    if ( cancelable )
    {
        if ( !m_InterruptibleRequestEnqueued )  // standard request already enqueued?
        {
            if ( CancelPending() ) // cancel already set?
            {
                RTETask_CommonQueueElement& element = CommonQueueElementFromFreeList();
                element.Init( RTETask_CommonQueueElement::CancelTask );
                return  element;
            }
            m_CancelRequestAllowed = true;  // allow cancel or timeout requests
        }
    }
    if ( timeout )
    {
        m_SendTimeOutRequestAt  = m_KGS.CurrTime() + timeout;
        m_TimeoutRequestAllowed = true;
        m_TimeoutRequestPending = false;
    }

    m_LegacyTaskCtrl.GotoDispatcher( m_pTaskScheduler->LegacyUKTCtrl() );

    pTaskScheduler = m_pTaskScheduler;

    if ( cancelable )
        m_CancelRequestAllowed = false;  // we have finished, disable cancelation

    if ( timeout )
    {
        m_TimeoutRequestAllowed = false;
        m_SendTimeOutRequestAt  = 0;
    }

    return *m_pTaskScheduler->LegacyUKTCtrl().WakeUPRequest()->pQueueElementObject;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTETask_Task::PrepareEnqueueOfInterruptibleRequest()
{
    if ( !m_CancelOrTimeoutRequestEnqueued )
    {
        m_InterruptibleRequestEnqueued  = true;
        return true;
    }
    return false;
}

/*---------------------------------------------------------------------------*/

inline void RTETask_Task::EndOfInterruptibleRequest()
{
    m_CancelOrTimeoutRequestEnqueued = false;
    m_InterruptibleRequestEnqueued   = false;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTETask_Task::PrepareEnqueueOfCancelRequest()
{
    if ( m_CancelRequestAllowed && !m_InterruptibleRequestEnqueued && !m_CancelOrTimeoutRequestEnqueued )
    {
        m_CancelOrTimeoutRequestEnqueued = true;  // enqueue request into the runqueue!
        return true;
    }
    return false;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTETask_Task::PrepareEnqueueOfTimeoutRequest()
{
    if ( m_TimeoutRequestAllowed && !m_InterruptibleRequestEnqueued && !m_CancelOrTimeoutRequestEnqueued )
    {
        if ( m_SendTimeOutRequestAt < m_KGS.CurrTime() )  // valid request?
        {
            m_CancelOrTimeoutRequestEnqueued = true;  // enqueue request into the runqueue!
            return true;
        }
        m_TimeoutRequestPending = false; // drop this request, wait for another one
    }

    return false;
}

/*---------------------------------------------------------------------------*/

inline void RTETask_Task::HandleRequestTimeout()
{
    if ( m_SendTimeOutRequestAt && !m_TimeoutRequestPending && 
        ( m_SendTimeOutRequestAt < m_KGS.CurrTime() ))
    {
        m_TimeoutRequestPending = true;
        RTETask_CommonQueueElement* pQueueElement = m_KGS.GlobalFreeList()->Dequeue();
        pQueueElement->Init( RTETask_CommonQueueElement::RequestTimedOut );
        EnqueueIntoExternalRequestsQueueAndWakeUP( *pQueueElement );
    }
}

#endif  /* RTETASK_TASK_HPP */
