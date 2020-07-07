/*!========================================================================

  @file         RTETask_CommonQueueElement.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Common tasking queue elements

                The following class provides methodes to access
                common tasking queue elements. These queue elements
                are a essential part of the sheduling. A task waiting
                for a operation to finish, doing a reschedule (yield) or
                waiting for a wake up triggered by another task is doing
                a 'WaitForReschedule' call. A respective queue element is
                enqueued in one of the TaskSchedulers queues to set the 
                taks free from it's wait call.

  @since        2005-11-22  12:35
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


#ifndef RTETASK_COMMONQUEUEELEMENT_HPP
#define RTETASK_COMMONQUEUEELEMENT_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif
#include    "RunTime/RTE_Types.h"
#include    "geo50_0.h"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/Tasking/RTETask_LegacyQueueElement.h"
#include    "RunTime/Tasking/RTETask_DoublyLinkedListElement.hpp"
#include    "RunTime/Tasking/RTETask_ICommonQueue.hpp"


#include <memory.h>


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

typedef DLQ_ARGS    RTETask_CQEDataPart;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;

/*!---------------------------------------------------------------------------
  @class   RTETask_CommonQueueElement
  @brief   Common tasking queue elements

           The following class provides methodes to access
           common tasking queue elements. These queue elements
           are a essential part of the sheduling. A task waiting 
           for a operation to finish, doing a reschedule (yield) or
           waiting for a wake up triggered by another task is doing
           a 'WaitForReschedule' call. A respective queue element is
           enqueued in one of the TaskSchedulers queues to set the 
           taks free from it's wait call.

           A task sending a request (e.g. IO request) to another thread
           enqueues a queue element into the threads job queue before 
           doing the 'WaitForReschedule' call. If the requested thread
           has finished the job it stores a ready queue element into
           the 'TaskSchedulers' 'ExternalRequestsQueue' to set the 
           waiting task free.

           In some situatiuons the enqueued element is not task 
           releated, it contains a request for the TaskScheduler.
           Most of the elements have a special data part which
           can be accessed via the type related data methodes like
           'm_CommunicationReceiveWaitPart'.

           A comon queue element might belong to a free list where it must be 
           released when it's not longer used. Each queue element should be 
           released by calling the elements methode Release().

 ----------------------------------------------------------------------------*/

struct RTETask_CommonQueueElement : protected RTETask_DoublyLinkedListElement
{
public:
    /*! 
    @brief    Type of the element 

              It's necessary to distinguish the different types
              of element contents.
    */
    enum ElementType 
    {
        CommunicationReceiveWait  = REQ_COMMUNICATION_RECEIVE_WAIT,   // communication is waiting for a new packet
        CommunicationConnectWait  = REQ_COMMUNICATION_CONNECT_WAIT,   // communication is waiting for a new connection
        
        Vattach                   = REQ_VATTACH_EO00,
        VblockIO                  = REQ_VBLOCKIO_EO00,
        Vdetach                   = REQ_VDETACH_EO00,
        InitOk                    = REQ_INITOK_EO00,
        Vresume                   = REQ_VRESUME_EO00,
        VSignal                   = REQ_VSIGNAL_EO00,
        VSleep                    = REQ_VSLEEP_EO00,
        Connect                   = REQ_CONNECT_EO00,
        ConnectRequest            = REQ_CONNECT_REQUEST_EO00,
        Receive                   = REQ_VRECEIVE_EO00,
        ShutDown                  = REQ_SHUTDOK_EO00,
        SenderJobFree             = REQ_SENDER_JOB_FREE_EO00,
        AsyncOpen                 = REQ_ASYNOPEN_EO00,
        AsyncClose                = REQ_ASYNCLOSE_EO00,
        AsyncIO                   = REQ_ASYNIO_EO00,
        AsyncControl              = REQ_ASYNCNTL_EO00,
        AsyncWait                 = REQ_ASYNWAIT_EO00,
        Vstop                     = REQ_VSTOP_EO00,
        VvectorIO                 = REQ_VVECTORIO_EO00,
        DebugTask                 = REQ_DEBUG_TASK_EO00,
        InsertMovingTask          = REQ_INSERT_MOVING_TASK_E000,
        MoveTaskMeasInterval      = REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00,
        MoveTask                  = REQ_MOVE_TASK_EO00,
        NewCommand                = REQ_NEW_COMMAND_EO00,
        RemoveTaskFromScheduler   = REQ_REMOVE_TASK_FROM_UKT_EO00,
        ResumeTraceWriter         = REQ_RESUME_TW_EO00,
#       if defined (_WIN32)
         ShutdownResumeTraceWr    = REQ_SHUTDOWN_RESUME_TW_EO00,
         Vendexcl                 = REQ_VENDEXCL_EO00,
         OverlapppedIOFinished    = REQ_OVERLAPPED_IO_FINISHED_EO00,
         MarkVolumeAsBad          = REQ_MARK_BAD_DEVSPACE_EO00,
         FinishServerTask         = REQ_FINISH_SV_EO00,
#       else 
         Exclusive                = REQ_EXCLUSIVE_EO00,
         TestAndSet               = REQ_TEST_AND_SET_EO00,
         IOFinished               = REQ_IO_FINISHED_EO00,
         BigCommsegConnect        = REQ_BIG_CONNECT_EO00,
         AddressConnect           = REQ_ADDRESS_CONNECT_EO00,
         BigAddressConnect        = REQ_BIG_ADDR_CONNECT_EO00,
         MarkVolumeAsBad          = REQ_VMARK_BAD_DEV_EO00,
#       endif
        Reschedule                = REQ_RESCHEDULE_EO00,
        TaskSleep                 = REQ_TASK_SLEEP_EO00,
        LeaveRWRegion             = REQ_LEAVE_RWREGION_EO00,
        CancelTask                = REQ_CANCEL_TASK_EO00,
        RequestTimedOut           = REQ_REQUEST_TIMED_OUT_EO00,
        Yielding                  = REQ_YIELDING_EO00
    };

    /*! 
    @brief    Constructor 

    @param    queueElement  [in]   - Reference to an external queue element
                                      If no pointer to an external 
                                      queue element is given the member
                                      queue element is used
    */
    RTETask_CommonQueueElement() : m_pFreeList(0)
    {
        Clear();
        m_QueueElement.pQueueElementObject = this;
    }


    /*!
    @brief    Release element

              An element might belong to a free list where it must be released
              when it's not longer used. After an element is not longer used
              this release methode should be used. 

              IT'S GOOD PRACTICE TO DO THIS EVEN IF THE QUEUE ELEMENT DOES NOT
              BELONG TO A FREELIST!
    */

    void Release()
    {
        if ( m_pFreeList )
            m_pFreeList->Enqueue( *this );
    }

   /*!
    @brief    Initialize queue element

    @param    type         [in]   - Queue element type
    */
    void Init( ElementType type )
    {
        SetType( type );
        SetTask(0);        // will be set by the enqueue methodes
        SetSenderTask(0);  // will be set by the enqueue methodes
    }

    /*!
    @brief    Get type of queue element
    */
    ElementType GetType()
    {
#       if defined (_WIN32)
         return (ElementType)m_QueueElement.ulReqType;
#       else
         return (ElementType)m_QueueElement.req_type;
#       endif
    }


    /*!
    @brief    Get the task sending this queue element
              Null if none
    */
    RTETask_Task* SenderTask() const
    {
        TASK_CTRL pTaskCtrl;
#       if defined (_WIN32)
         pTaskCtrl = m_QueueElement.pFromTaskCtrl;
#       else
         pTaskCtrl = m_QueueElement.fromtaskid;
#       endif

        return 0 == pTaskCtrl ? 0 : pTaskCtrl->pTaskObject;
    }

   /*!
    @brief    Get task owner of this queue element
              Null if none
    */
    RTETask_Task* Task() const
    {
        TASK_CTRL pTaskCtrl;
#       if defined (_WIN32)
         pTaskCtrl = m_QueueElement.pTaskCtrl;
#       else
         pTaskCtrl = m_QueueElement.taskid;
#       endif

        return 0 == pTaskCtrl ? 0 : pTaskCtrl->pTaskObject;
    }

    /*!
    @brief    Push original request type to backup area
              (used by load balancing)

    @param    type  [in]   - new queue element type
    */
    void PushElementType( ElementType type )
    {
#       if defined (_WIN32)
         m_QueueElement.ulOrgReqType = m_QueueElement.ulReqType;
         m_QueueElement.ulReqType    = type;
#       else
         m_QueueElement.req_type_org = m_QueueElement.req_type;
         m_QueueElement.req_type     = type;
#       endif
    }

    /*!
    @brief    Pop original request type (used by load balancing)
    */
    void PopElementType()
    {
#       if defined (_WIN32)
         m_QueueElement.ulReqType = m_QueueElement.ulOrgReqType;
#       else
         m_QueueElement.req_type  = m_QueueElement.req_type_org;
#       endif
    }


    /*!
    @brief          Get access to communication receive wait part
    */
    RTETask_CQE_CommunicationReceiveWaitPart& CommunicationReceiveWaitPart()
    {
         return DataPart().m_CommunicationReceiveWaitPart;
    }

    /*!
    @brief          Get access to connect request part
    */
    RTETask_CQE_ConnectRequest& ConnectRequestPart()
    {
         return DataPart().m_ConnectRequest;
    }

    /*!
    @brief          Get access to task sleep part
    */
    RTETask_CQE_TaskSleep& TaskSleepPart()
    {
         return DataPart().m_TaskSleep;
    }

    /*!
    @brief          Get direct access to queue elements data part
    @return         Pointer to data part union
    */
    RTETask_CQEDataPart &DataPart()
    {
#       if defined (_WIN32)
         return m_QueueElement.ReqArgs;
#       else
         return m_QueueElement.args;
#       endif
    }

    /*
    @brief          Get direct access to queue element
    */
    operator DLQ_ELEMENT*()
    {
        return &m_QueueElement;
    }

private:
    /*!
    @brief    Clear the queue element content
    */
    void Clear()
    { 
        memset ( &m_QueueElement, 0 , sizeof(m_QueueElement));
        m_QueueElement.pQueueElementObject = this; 
    }

   /*!
    @brief    Set queue element type

    @param    type  [in]   - queue element type
    */
    void SetType( ElementType type )
    {
#       if defined (_WIN32)
         m_QueueElement.ulReqType  = type;
#       else
         m_QueueElement.req_type   = type;
#       endif
    }

private:
    // the following methods and the protected base class 'RTETask_DoublyLinkedListElement' 
    // is accessed by:
    friend class RTETask_ReadyToRunQueue;
    friend class RTETask_ReadyToRunQueueTS;
    friend class RTETask_ExternalRequestsQueue;
    friend class RTETask_ExternalRequestsQueueTS;
    friend class RTETask_CommunicationQueue;
    friend class RTETask_CommunicationQueueTS;
    friend class RTETask_CommonQueue;
    friend class RTETask_SynchronizedCommonQueue;
    friend class RTETask_UnconnectedTasksQueue;

    /*!
    @brief    Set pointer to the free list this element belongs to
              (will be set by the constructor of the free list classes).

    @param    pFreeList  [in]   - pointer to the free list
    */
    void SetFreeList( RTETask_ICommonQueue  &freeList )
    {
        m_pFreeList = &freeList;
    }

    /*!
    @brief    Set owner of this queue element
              (The enqueue methode will do this for us )

    @param    pTaskCtrl  [in]   - task control
    */
    void SetTask( TASK_CTRL  pTaskCtrl )
    {
#       if defined (_WIN32)
         m_QueueElement.pTaskCtrl = pTaskCtrl;
#       else
         m_QueueElement.taskid    = pTaskCtrl;
#       endif
    }

    /*!
    @brief    Set sending task of this queue element

    @param    pTaskCtrl  [in]   - sending task
    */
    void SetSenderTask( TASK_CTRL  pTaskCtrl )
    {
#       if defined (_WIN32)
         m_QueueElement.pFromTaskCtrl = pTaskCtrl;
#       else
         m_QueueElement.fromtaskid    = pTaskCtrl;
#       endif
    }

    /*!
       @brief          Returns the next element
       @return         Next element
     */
    RTETask_CommonQueueElement * Next() const
    {
        return reinterpret_cast<RTETask_CommonQueueElement*>(NextElement());
    }

private:
    // don't allow copies
    inline   RTETask_CommonQueueElement( const RTETask_CommonQueueElement &c ) {;}

private:
    DLQ_ELEMENT                             m_QueueElement;
    RTETask_ICommonQueue*                   m_pFreeList;
};

/*!
  @brief convert RTETask_CommonQueueElement::ElementType into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTETask_CommonQueueElement::ElementType ElementType )
{
    switch (ElementType)
    {
    case RTETask_CommonQueueElement::Vattach                 : return SAPDB_ToStringClass("Vattach");
    case RTETask_CommonQueueElement::Vdetach                 : return SAPDB_ToStringClass("Vdetach");
    case RTETask_CommonQueueElement::InitOk                  : return SAPDB_ToStringClass("InitOk");
    case RTETask_CommonQueueElement::Vresume                 : return SAPDB_ToStringClass("Vresume");
    case RTETask_CommonQueueElement::VSignal                 : return SAPDB_ToStringClass("VSignal");
    case RTETask_CommonQueueElement::VSleep                  : return SAPDB_ToStringClass("VSleep");
    case RTETask_CommonQueueElement::TaskSleep               : return SAPDB_ToStringClass("TaskSleep");
    case RTETask_CommonQueueElement::Connect                 : return SAPDB_ToStringClass("Connect");
    case RTETask_CommonQueueElement::Receive                 : return SAPDB_ToStringClass("Receive");
    case RTETask_CommonQueueElement::ShutDown                : return SAPDB_ToStringClass("ShutDown");
    case RTETask_CommonQueueElement::SenderJobFree           : return SAPDB_ToStringClass("SenderJobFree");
    case RTETask_CommonQueueElement::AsyncOpen               : return SAPDB_ToStringClass("AsyncOpen");
    case RTETask_CommonQueueElement::AsyncClose              : return SAPDB_ToStringClass("AsyncClose");
    case RTETask_CommonQueueElement::AsyncIO                 : return SAPDB_ToStringClass("AsyncIO");
    case RTETask_CommonQueueElement::AsyncControl            : return SAPDB_ToStringClass("AsyncControl");
    case RTETask_CommonQueueElement::AsyncWait               : return SAPDB_ToStringClass("AsyncWait");
    case RTETask_CommonQueueElement::Vstop                   : return SAPDB_ToStringClass("Vstop");
    case RTETask_CommonQueueElement::VvectorIO               : return SAPDB_ToStringClass("VvectorIO");
    case RTETask_CommonQueueElement::DebugTask               : return SAPDB_ToStringClass("DebugTask");
    case RTETask_CommonQueueElement::InsertMovingTask        : return SAPDB_ToStringClass("InsertMovingTask");
    case RTETask_CommonQueueElement::MoveTaskMeasInterval    : return SAPDB_ToStringClass("MoveTaskMeasInterval");
    case RTETask_CommonQueueElement::MoveTask                : return SAPDB_ToStringClass("MoveTask");
    case RTETask_CommonQueueElement::NewCommand              : return SAPDB_ToStringClass("NewCommand");
    case RTETask_CommonQueueElement::RemoveTaskFromScheduler : return SAPDB_ToStringClass("RemoveTaskFromScheduler");
    case RTETask_CommonQueueElement::ResumeTraceWriter       : return SAPDB_ToStringClass("ResumeTraceWriter");
#   if defined (_WIN32)
     case RTETask_CommonQueueElement::ShutdownResumeTraceWr  : return SAPDB_ToStringClass("ShutdownResumeTraceWr");
     case RTETask_CommonQueueElement::Vendexcl               : return SAPDB_ToStringClass("Vendexcl");
     case RTETask_CommonQueueElement::OverlapppedIOFinished  : return SAPDB_ToStringClass("OverlapppedIOFinished  ");
     case RTETask_CommonQueueElement::MarkVolumeAsBad        : return SAPDB_ToStringClass("MarkVolumeAsBad");
     case RTETask_CommonQueueElement::FinishServerTask       : return SAPDB_ToStringClass("FinishServerTask");
#   else 
     case RTETask_CommonQueueElement::Exclusive              : return SAPDB_ToStringClass("Exclusive");
     case RTETask_CommonQueueElement::TestAndSet             : return SAPDB_ToStringClass("TestAndSet");
     case RTETask_CommonQueueElement::IOFinished             : return SAPDB_ToStringClass("IOFinished");
     case RTETask_CommonQueueElement::BigCommsegConnect      : return SAPDB_ToStringClass("BigCommsegConnect");
     case RTETask_CommonQueueElement::AddressConnect         : return SAPDB_ToStringClass("AddressConnect");
     case RTETask_CommonQueueElement::BigAddressConnect      : return SAPDB_ToStringClass("BigAddressConnect");
     case RTETask_CommonQueueElement::MarkVolumeAsBad        : return SAPDB_ToStringClass("MarkVolumeAsBad");
#   endif
    case RTETask_CommonQueueElement::Reschedule              : return SAPDB_ToStringClass("Reschedule");
    case RTETask_CommonQueueElement::LeaveRWRegion           : return SAPDB_ToStringClass("LeaveRWRegion");
    case RTETask_CommonQueueElement::CancelTask              : return SAPDB_ToStringClass("CancelTask");
    case RTETask_CommonQueueElement::RequestTimedOut         : return SAPDB_ToStringClass("RequestTimedOut");
    case RTETask_CommonQueueElement::Yielding                : return SAPDB_ToStringClass("Yielding");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)ElementType);
}

#endif  /* RTETASK_COMMONQUEUEELEMENT_HPP */
