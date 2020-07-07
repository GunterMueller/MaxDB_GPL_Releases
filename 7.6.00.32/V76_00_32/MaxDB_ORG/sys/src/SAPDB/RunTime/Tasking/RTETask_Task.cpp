/*!========================================================================

  @file         RTETask_Task.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel task class.

                This class provides all kernel task related 
                methodes and data. If you look for other RTE methodes, 
                that are not task depending look into description of 
                RTE_IGlobal class. This defines i.e. the 
                GetCurrentTask() function to retrieve the the currently 
                running task, which then can be used to access
                the task object.

  @since        2006-03-14  17:36
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

/*! temporary defined */
#define ASSERTION_CRASH_CODE

#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_LegacyTaskCtrl.hpp"
#include    "RunTime/RTE_ConsoleStandardOutput.h"
#include    "heo00x.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Task_Messages.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include    "KernelCommon/Kernel_Main.hpp"
#include    "KernelCommon/Kernel_Initialization.hpp"

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

RTETask_Task::RTETask_Task( RTE_TaskId   id,
                            TaskType     type,
                            TASK_CTRL    pTaskCtrl ) 
             : m_KGS(RTE_KGS::Instance()),  // set shortcut to kgs
               m_LegacyTaskCtrl(pTaskCtrl),
               m_pKernelCommunication(RTE_KGS::Instance().KernelCommunication()),
               m_pTaskScheduler(0),
               m_ID(id),
               m_Type(type),
               m_StartupComplete(false),
               m_AlreadyYielding(false),
               m_CancelRequestAllowed(false),
               m_TimeoutRequestAllowed(false),
               m_TimeoutRequestPending(false),
               m_SendTimeOutRequestAt(0),
               m_CancelOrTimeoutRequestEnqueued(false),
               m_InterruptibleRequestEnqueued(false),
               m_FreeList( m_CommonQueueElement, 
                           TASK_NUM_OF_QUEUE_ELEMENTS, 
                           "TaskCommonQueueFreeList" )
{
    SAPDBERR_ASSERT_STATE( 0 != pTaskCtrl );

    LoadTaskScheduler();
    EnqueueIntoUnconnectedTasksQueue();

    memset (m_DisplayName, 0, sizeof (m_DisplayName));
    memset (m_DisplayShortName, 0, sizeof (m_DisplayShortName));
    m_ClientConnections.Initialize( this, m_LegacyTaskCtrl.IsConnectable() );
}

/*---------------------------------------------------------------------------*/

void    RTETask_Task::SetDisplayName  (SAPDB_Char const * const shortName, 
                                       SAPDB_Char const * const longName)
{
    if (NULL != shortName && NULL != longName)
    {
        strncpy (m_DisplayName, longName, sizeof (m_DisplayName) - 1);
        strncpy (m_DisplayShortName, shortName, sizeof (m_DisplayShortName) - 1);
    }
    else
    {
        memset (m_DisplayName, 0, sizeof (m_DisplayName));
        memset (m_DisplayShortName, 0, sizeof (m_DisplayShortName));
    }
}

/*---------------------------------------------------------------------------*/

RTETask_ITaskClientConnection* const RTETask_Task::AcceptClientConnect()
{
    return m_ClientConnections.AcceptClientConnect();
}

/*---------------------------------------------------------------------------*/

void RTETask_Task::EnqueueIntoUnconnectedTasksQueue()
{
    RTETask_UnconnectedTasksQueue* pUnconnectedTasksQueue;

    pUnconnectedTasksQueue = m_pTaskScheduler->UnconnectedTasksQueue(Type());

    if ( 0 != pUnconnectedTasksQueue )
    {
        // enqueue server task into the unconnected tasks queue where it's
        // waiting for a connect request
        RTETask_CommonQueueElement* pElement = m_FreeList.Dequeue();
        pElement->Init( RTETask_CommonQueueElement::CommunicationConnectWait );

        if ( User == Type() ) 
        {
            m_pKernelCommunication->UnconnectedTaskPool().AddTask();
        }
        pUnconnectedTasksQueue->Enqueue(*pElement, *this);
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_Task::YieldTask( SAPDB_Bool allowLoadBalancing )
{
    SAPDBERR_ASSERT_STATE( this == RTETask_Task::Self() );

    // If we are waiting for the next task, which means
    // we are currently running inside the scheduler wait methode 
    // (dispatcher) ...
    if ( true == TaskScheduler().IsWaitingForNextScheduledTask() || m_AlreadyYielding )
    {   // ... we have to reschedule the OS thread.
        RTESys_GiveUpTimeSlice();
    }
    else
    {   // If we are outside the scheduler wait methode (dispatcher) 
        // and we are not already yielding we can reschedule the task...

        // To avoid recursive YieldTask calls, ... 
        m_AlreadyYielding = true; // ... signal that we already yielding

        // load balancing allowed?
        if ( false == allowLoadBalancing )  
        {   // task should not move to another task scheduler 
            // during reschedule
            LockLoadBalancing();  
        }

        // get the number of task switches occured in current scheduler
        tsp00_Uint8 taskSwitches = TaskScheduler().TaskSwitches();

        // add tasks queue element to the runqueue to force a reschedule
        RTETask_CommonQueueElement* pElement = m_FreeList.Dequeue();

        pElement->Init( RTETask_CommonQueueElement::Yielding );

        EnqueueIntoReadyToRunQueue( *pElement, *this, RTETask_ReadyToRunQueue::YieldPrio );

        // wait until we are avoke by the prior reschedule element  
        RTETask_CommonQueueElement& returnElement = WaitForReschedule();

        SAPDBERR_ASSERT_STATE( returnElement == *pElement );

        // we don't need the queue element release it
        pElement->Release();

        if ( false == allowLoadBalancing )  
            UnlockLoadBalancing();

        // if number of task switches did, not change, 
        // we can savely reschedule the thread... 
        if ( taskSwitches == TaskScheduler().TaskSwitches() )
        {   
            RTESys_GiveUpTimeSlice();  // reschedule the OS thread
        }

        m_AlreadyYielding = false;
    }
}


/*---------------------------------------------------------------------------*/

void  RTETask_Task::LockLoadBalancing()
{
    m_LegacyTaskCtrl.LockLoadBalancing();
}

/*---------------------------------------------------------------------------*/

void  RTETask_Task::UnlockLoadBalancing()
{
    m_LegacyTaskCtrl.UnlockLoadBalancing();
}

/*---------------------------------------------------------------------------*/

RTETask_ITaskScheduler  &RTETask_Task::ITaskScheduler()
{
    return *m_pTaskScheduler;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_Task::Startup( SAPDBErr_MessageList &messageList )
{
    SAPDBERR_ASSERT_STATE( 0 != ((TASK_CTRL)m_LegacyTaskCtrl) );

    // Is the task inactive?
    if (TaskIsInactive())
    {   
#       if defined (_WIN32)
         // startup task
         if ( NO_ERROR != sql88k_create_task_or_fiber ( m_LegacyTaskCtrl ) )
         {
             messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_TASK_COULD_NOT_START, SAPDB_ToString(TypeAsString()));
             return false;
         }
#       else
#       endif

        m_StartupElement.Init( RTETask_CommonQueueElement::InitOk );
        EnqueueIntoExternalRequestsQueueAndWakeUP( m_StartupElement );
    }
    return true;
}

/*---------------------------------------------------------------------------*/

char const * const RTETask_Task::StateAsString() 
{
    switch ( State() )
    {
    case TaskInitialized       : return XCONS_TSK_STATE_INITIALIZE;
    case TaskInactive          : return XCONS_TSK_STATE_INACTIVE;
    case TaskMarkedForStartup  : return XCONS_TSK_STATE_MARKED_FOR_STRT;
    case TaskIsRunning         : return XCONS_TSK_STATE_RUNNING;
    case TaskWaitingForConnect : return XCONS_TSK_STATE_CONNECTWAIT;
    case TaskVDEVSIZE          : return XCONS_TSK_STATE_VDEVSIZE;
    case TaskVATTACH           : return XCONS_TSK_STATE_VATTACH;
    case TaskVDETACH           : return XCONS_TSK_STATE_VDETACH;
    case TaskVFOPEN            : return XCONS_TSK_STATE_VFOPEN;
    case TaskVFWRITE           : return XCONS_TSK_STATE_VFWRITE;
    case TaskVFCLOSE           : return XCONS_TSK_STATE_VFCLOSE;
    case TaskCloseConnection   : return XCONS_TSK_STATE_VRELEASE;
    case TaskVSHUTDOWN         : return XCONS_TSK_STATE_VSHUTDOWN;
    case TaskReceive           : return XCONS_TSK_STATE_VRECEIVE;
    case TaskSend              : return XCONS_TSK_STATE_VREPLY;
    case TaskVBEGEXCL          : return XCONS_TSK_STATE_VBEGEXCL;
    case TaskVENDEXCL          : return XCONS_TSK_STATE_VENDEXCL;
    case TaskVSUSPEND          : 
        if ( m_LegacyTaskCtrl.TaskStateReason() > 0 )
            return (SuspendReasonText[m_LegacyTaskCtrl.TaskStateReason()].szShortReason);
        else            
            return XCONS_TSK_STATE_VSUSPEND;
        break ;
    case TaskVWAIT             : return XCONS_TSK_STATE_VWAIT;
    case TaskVSLEEP            : return XCONS_TSK_STATE_VSLEEP;
    case TaskVDIAGINIT         : return XCONS_TSK_STATE_VDIAGINIT;
    case TaskVOPMSG            : return XCONS_TSK_STATE_VOPMSG;
    case TaskLocked            : return XCONS_TSK_STATE_LOCKED;
    case TaskTerminated        : return XCONS_TSK_STATE_TERMINATED;
    case TaskRunnable          : return XCONS_TSK_STATE_RUNNABLE;
    case TaskASYNOPEN          : return XCONS_TSK_STATE_ASYNOPEN;
    case TaskASYNCLOSE         : return XCONS_TSK_STATE_ASYNCLOSE;
    case TaskASYNIO            : return XCONS_TSK_STATE_ASYNIO;
    case TaskASYNWAITRead      : return XCONS_TSK_STATE_ASYNWAIT_READ;
    case TaskASYNWAITWrite     : return XCONS_TSK_STATE_ASYNWAIT_WRITE;
    case TaskIORead            : return XCONS_TSK_STATE_IO_READ;
    case TaskIOWrite           : return XCONS_TSK_STATE_IO_WRITE;
    case TaskASYNCNTL          : return XCONS_TSK_STATE_ASYNCNTL;
    case TaskLCApplObjCalled   : return XCONS_TSK_STATE_DCOM_OBJ_CALLED;
    case TaskVRESCHEDULE_MSEC  : return XCONS_TSK_STATE_RESCHEDULE_MSEC;
    case TaskVSTOP             : return XCONS_TSK_STATE_STOPPED;
    case TaskVVECTORIO         : return XCONS_TSK_STATE_VVECTORIO;
    case TaskVDUALVECTORIO     : return XCONS_TSK_STATE_VDUALVECTORIO;
    case TaskEnteringRWRegion  : return XCONS_TSK_STATE_VENTERRWREGION;
    case TaskLeavingRWRegion   : return XCONS_TSK_STATE_VLEAVERWREGION;
    case TaskIsYieldingOnSpinlock : return XCONS_TSK_STATE_YIELDING_ON_SPINLOCK;
    default: break;
    }
    return ( XCONS_TSK_STATE_UNKNOWN );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTETask_Task::AllocateLocalStorage()
{ 
    return m_TaskLocalStorage.Allocate(); 
} 

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_Task::FreeLocalStorage( SAPDB_UInt4 index )
{ 
    return m_TaskLocalStorage.Free( index ); 
} 

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_Task::SetLocalStorageValue( SAPDB_UInt4 index,  void* dataPtr )
{ 
    return m_TaskLocalStorage.SetValue(index,dataPtr); 
} 

/*---------------------------------------------------------------------------*/

void* RTETask_Task::GetLocalStorageValue( SAPDB_UInt4   index )
{ 
    return m_TaskLocalStorage.GetValue( index ); 
} 

/*---------------------------------------------------------------------------*/

RTETask_ITask *RTETask_ITask::Self()
{
    return RTETask_Task::Self();
}

/*---------------------------------------------------------------------------*/

RTETask_ITask* RTETask_ITask::ByTaskID( RTE_TaskId taskID )
{
    return RTETask_Task::ByTaskID( taskID );
}

/*---------------------------------------------------------------------------*/

void RTETask_ITask::ContinueOther( RTETask_ITask &otherTask )
{
    vcontinue(otherTask.ID());
}

/*---------------------------------------------------------------------------*/

extern "C" void RTETask_TaskMain()
{
    RTETask_Task &task = *RTETask_Task::Self();

    if ( RTETask_ITask::Timer == task.Type() )
        Kernel_Init( task );  // It has to be called before any kernel tasks are running

    Kernel_Main( task );
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
