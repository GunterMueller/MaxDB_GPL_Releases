/*!========================================================================

  @file         RTETask_ITask.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel task interface class.

                This class provides the interface for all kernel task related 
                methodes and data.

  @since        2005-11-03  9:26
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


#ifndef RTETASK_ITASK_HPP
#define RTETASK_ITASK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "geo00_2.h" /* to be removed, if compatibility with '_EO00' defines is no longer needed... */
#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/Tasking/RTETask_Task.h"


/* Forward declarations */
class RTETask_ITaskScheduler;        /* defined in "RunTime/Tasking/RTETask_ITaskScheduler.hpp" */
class RTETask_ITaskClientConnection; /* defined in "RunTime/Tasking/RTETask_ITaskClientConnection.hpp" */
class SAPDBErr_MessageList;          /* defined in "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp */
class RTETask_ITask;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  PROTOTYPES ...                                                           *
 *===========================================================================*/

/*!
   @brief Entry point of each kernel task
   @param task [in] the task object associated with the task
 */
void KernelTask( RTETask_ITask * task );

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!---------------------------------------------------------------------------
  @class   RTETask_ITask
  @brief   Kernel task class.

           This class provides all kernel task related 
           methodes and data. If you look for other RTE methodes, 
           that are not task depending look into description of 
           RTE_IGlobal class.
           
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

           The I/O calls inside the kernel code are never issued directly, but
           using the I/O member function of this class. The I/O calls are devided
           into Synchronous I/O calls for accessing volumes and asynchronous I/O
           calls for accessing backup devices.
 ----------------------------------------------------------------------------*/

class RTETask_ITask
{
public:
    /*!
    @brief    Type of task
    */
    enum TaskType 
    {
        Timer            = TT_TI_EO00,
        Logwriter        = TT_AL_EO00,
        Tracewriter      = TT_TW_EO00,
        Datawriter       = TT_DW_EO00,
        Utility          = TT_UT_EO00,
        Event            = TT_EV_EO00,
        GarbageCollector = TT_GC_EO00,
        Backup           = TT_BUP_EO00,
        User             = TT_US_EO00,
        Server           = TT_SV_EO00
    };


public:
    /*!
      @brief Call to retrieve own task object
  
      Must be called inside from inside tasking code, or kernel aborts...

      @return reference to own task object
     */
    static RTETask_ITask* Self();

    /*!
      @brief Call to retrieve task object from given TaskId
      @param taskID [in] the given task id [ 1 ... kgs->maxKernelTasks ]
      @return corresponding task object or NULL if no such task exists 
     */
    static RTETask_ITask* ByTaskID( RTE_TaskId taskID );

    /*! 
    @brief  Signal that the startup phase of the task has finished
    */
    virtual void SetStartupComplete () = 0;

    /*! 
    @brief  Waits for a client connection
            
            Call this methode to join incoming client connects. 
            This methode blocks until a client connect will be 
            joined to this tasks connect request. The returned 
            client connection object is used to handle the new 
            created connection.
            
    @return   Object used to handle the connection
    */
    virtual RTETask_ITaskClientConnection * const AcceptClientConnect() = 0;

    /*! 
    @brief  Check if cancellation request is pending
            
    @return   true if cancel byte is set, false otherwise
    */
    virtual SAPDB_Bool CancelPending() = 0;

    /*! 
    @brief  Set cancel request to false
    */
    virtual void ResetPendingCancel() = 0;

    /*! 
    @brief  Set cancellation request pending
            
    @return   none
    */
    virtual void Cancel() = 0;

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
    virtual void                     YieldTask( SAPDB_Bool allowLoadBalancing = true ) = 0;

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
    virtual void                     LockLoadBalancing()   = 0;

    /*!
    @brief         Include this task to load balancing process.

    This is a counting call which means the same number of locks 
    and  unlock calls must occur to include this task to the 
    load balancing process.

    @return        None
    */
    virtual void                     UnlockLoadBalancing() = 0;

    /*!
    @brief         The tasks unique ID

                   Each task has its own ID, which is unique
                   to the database process.
    @return        Task ID
    */
    virtual RTE_TaskId               ID() const = 0;

    /*!
    @brief         Returns the tasks type

                   The tasks have related to there assignment a 
                   task type information. 

    @return        Task type
    */
    virtual TaskType                 Type() const = 0;

    /*!
    @brief         Set a display name for the task.   
                   If NULL is given for one of the strings both internal names are reseted to empty strings.

    @param         shortName [in] - Short display name or NULL. String is cut off after 7 characters.
                   longName [in]  - Long display name or NULL. String is cut off after 16 characters. 
                   
    @return        None
    */
    virtual void                     SetDisplayName  (SAPDB_Char const * const shortName, 
                                                       SAPDB_Char const * const longName) = 0;

    /*!
    @brief         Returns the tasks state as string

    @return        Task state as string
    */
    virtual char const * const StateAsString() = 0;


    /*!
    @brief         'AllocateLocalStorage' allocates a task local storage index. 
                    Any task can subsequently use this index to store and retrieve
                    pointer values that are local to the task.

    @return        Task local storage index
    */
    virtual SAPDB_UInt4   AllocateLocalStorage() = 0 ;


    /*!
    @brief         'FreeLocalStorage' releases a task local storage index, making it available for reuse.

    @param         index [in]- Index that was allocated by the 'Alloc' methode.

    @return        False, if the index is invalid because its out of range or it has
                   not been allocated by 'AllocateLocalStorage'.
    */
    virtual SAPDB_Bool FreeLocalStorage( SAPDB_UInt4 index ) = 0;


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
    virtual  SAPDB_Bool SetLocalStorageValue( SAPDB_UInt4 index,  void* dataPtr ) = 0;


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
    virtual void* GetLocalStorageValue( SAPDB_UInt4   index ) = 0;


    /*!
    @brief         Get reference to the corresponding task 
                   scheduler interface object.

                   CAUTION:
                     If a task is rescheduled and returns from
                     any task methode call, the corresponding
                     TaskScheduler might have changed because of the
                     load balancing mechanism. So it is a good 
                     practice not to store a pointer or reference
                     to the TaskScheduler over Runtime methode 
                     calls. 

    @return        Reference to TaskScheduler object
    */
    virtual RTETask_ITaskScheduler  &ITaskScheduler() = 0;

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
    virtual void Stop() = 0;

    /*!
    @brief         Continue a task, that stopped itself.
    
       The task is expected to be using Stop(). Issuing this call several times, leads to increase 
       of internal counter in 'otherTask'. That means if called twice for the same task, the task will
       not stop, before the third Stop() call.

       Replaces vcontinue( tsp00_TaskId pid )

    @param otherTask [in] the task that can continue
    */
    static void ContinueOther( RTETask_ITask &otherTask );

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
    virtual void Suspend( SAPDB_UInt4 suspendReason ) = 0;

    /*!
    @brief         The tasks suspends itself specifying a page lock

        Almost identical with Suspend, but additional page information is given for console output.

    Replaces vSuspendForPageLock( tsp00_TaskId pid, tsp00_Int2 suspend_reason, tsp00_Int4 page_no )

    @param suspendReason [in] An 'well defined' index of suspend reasons
    @param pageNumber [in] the page which caller is waiting for
    */
    virtual void SuspendForPageLock( SAPDB_UInt4       suspendReason
                                   , SAPDB_Int4        pageNumber ) = 0;

    /*!
    @brief         Resume a task, that suspended itself.
    
       The task is expected to be using Suspend(). This call is allowing a single outstanding resume
       only. That means if called twice for the same task without an intermediate Suspend() of that task,
       a programming error occured...

       Replaces vresume( tsp00_TaskId pid )

    @param otherTask [in] the task that can continue
    */
    virtual void ResumeOther( RTETask_ITask &otherTask ) = 0;

    /*!
    @brief         Checks if a resume is pending for given task

       The given task is checked for an pending resume

       Replaces visresume( tsp00_TaskId pid)

    @param otherTask [in] the task that can continue
    @return true, if resume pending, false if not
    */
    virtual SAPDB_Bool IsResumePending(RTETask_ITask &otherTask) = 0;

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
    virtual void Wait( SAPDB_Bool1 &signaledFlag ) = 0;

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
                            , SAPDB_Bool1 signaledFlag ) = 0;

    /*!
    @brief         Let the task sleep for a given time period

    The task issues an internal job request to the timer thread and reschedules.

       Replaces vsleep( tsp00_TaskId pid, tsp00_Int2 limit )

    @param timePeriodInSeconds [in] the number of seconds to sleep
    */
    virtual void Sleep( SAPDB_Int2 timePeriodInSeconds ) = 0;

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
                                       , SAPDB_Int4 &RemainMilliSeconds ) = 0;

};


inline SAPDB_ToStringClass SAPDB_ToString(RTETask_ITask::TaskType     type)
{
    switch (type)
    {
    case RTETask_ITask::Timer            : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_TI;
    case RTETask_ITask::Logwriter        : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_AL;
    case RTETask_ITask::Tracewriter      : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_TW;
    case RTETask_ITask::Datawriter       : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_DW;
    case RTETask_ITask::Utility          : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_UT;
    case RTETask_ITask::Event            : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_EV;
    case RTETask_ITask::GarbageCollector : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_GC;
    case RTETask_ITask::Backup           : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_BUP;
    case RTETask_ITask::User             : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_US;
    case RTETask_ITask::Server           : return (SAPDB_ToStringClass)RTE_TSK_TYPE_NAME_SV;
    }

    return (SAPDB_ToStringClass)(RTE_TSK_TYPE_NAME_UNKNOWN);
}


#endif  /* RTETASK_ITASK_HPP */
