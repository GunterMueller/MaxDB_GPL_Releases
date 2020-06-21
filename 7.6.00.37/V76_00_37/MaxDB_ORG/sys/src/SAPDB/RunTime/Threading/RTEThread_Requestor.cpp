/*!========================================================================

  @file         RTEThread_Requestor.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Handle incomming communication requests

                This thread class manages incomming communication 
                requests from clients application like connect, cancel
                and kill requests.

  @since        2005-07-19  11:16
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

#include    "RunTime/RTE_KGS.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "RunTime/Threading/RTEThread_Requestor.hpp"
#include    "SAPDBCommon/SAPDB_StrUtils.hpp"
#include    "RunTime/Tasking/RTETask_TaskClientConnections.hpp"
#include    "RunTime/Tasking/RTETask_TaskClientConnection.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/RTE_Crash.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include    "RunTime/RTE_ISystem.hpp"
#include    "RunTime/System/RTESys_SecurityAttributes.hpp"

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


SAPDB_Int4 RTEThread_Requestor::KernelThreadMain()
{

    SAPDB_Int4               returnCode = 0;
    SAPDBErr_MessageList     messageList;
    RTECKC_Request           request (m_requestChannel.GetMaxRequestLength ());

    // create the requestors IPC resources
    CreateIPCResources();

    for (;;)
    {
        SAPDB_Bool Ok = m_requestChannel.Read( request, messageList );
        
        // is the database instance stopping?
        if ( m_KGS.DBState() >= RTE_KGS::RTEKGS_StateShutdown )
        {
            returnCode = m_KGS.DBState();
            break;
        }

        if ( false == Ok )
        {
            RTE_Message (messageList, MSG_DIAG );
            continue;
        }

        if ( request.IsCancel())
        {
            HandleCancelRequest( &request );
        }
        else if ( request.IsDump())
        {
            // not yet implemented
        }
        else
        {
            HandleConnectRequest( &request );
        }
    }

    m_requestChannel.Close( messageList );

    return returnCode;
}

/*---------------------------------------------------------------------------*/

void RTEThread_Requestor::CreateIPCResources()
{
    RTEIPC_CommunicationObject::CommObjRC   rcReq;
    SAPDBErr_MessageList                    messageList;

    //Create Request Channel
    rcReq = m_requestChannel.Open (RTE_OpenNew, 
                                   (SAPDB_Char*)m_KGS.DBName(),
#if defined (_WIN32)
                                   m_KGS.GetKGS()->WorldSA,
#else
//Kernel umask is not considered in this open call. Therefore the given permissions is used
//as absolut permissions!
                                   RTESys_SecurityAttributes::Instance().GetWorldGroupWriteSA (),
#endif  
                                   messageList);

    if (RTEIPC_CommunicationObject::NoError != rcReq)
    {
        messageList =  messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_REQUEST_CHANNEL );
        RTE_Crash( messageList );
    }
}

/*---------------------------------------------------------------------------*/

void RTEThread_Requestor::HandleCancelRequest ( RTECKC_Request*     pRequest )
{
}

/*---------------------------------------------------------------------------*/

void RTEThread_Requestor::HandleConnectRequest ( RTECKC_Request*    pRequest )
{
    SAPDB_UInt4              commSegSize;
    SAPDB_Char const *       pSessionTyp;
    SAPDB_Char const *       pSessionID;
    SAPDBErr_MessageList     messageList;
    SAPDB_Char const *       pCommSegID;
    SAPDB_Char const *       pUniqueObjID;
    RTESync_SemaphoreID      clientSemID = RTE_SYNC_UNDEF_SEM_ID;
    
    // Interpret the connect request packet 
    if (!pRequest->GetConnectRequest ( pCommSegID, 
									   clientSemID, 
									   pUniqueObjID,
                                       commSegSize, 
									   pSessionTyp, 
									   pSessionID, 
									   messageList ))
    {
        RTE_Message (messageList, MSG_DIAG );
        return;
    }

    if ( 0 != pSessionID ) // Session id request?
    {   //
        // A connect to an exisiting session was requested!
        //
        if ( false == SAPDB_StringsIdentical( pSessionTyp, "session" ) )
        {
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, 
																 pCommSegID, 
																 clientSemID, 
																 commSegSize );
            RTE_Message (SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_INVALID_SESSIONID_TYPE, pSessionTyp), MSG_DIAG );
            return;
        }

        RTETask_TaskClientConnections* pTaskClientConnections;
        
        pTaskClientConnections = RTETask_TaskClientConnections::ParseSessionIdentifier( pSessionID, messageList );

        if ( 0 == pTaskClientConnections )
        {
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }
        
        RTETask_TaskClientConnection* pConnection;

        pConnection = pTaskClientConnections->CreateConnectionObject( messageList );

        if ( 0 == pConnection )
        {
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }

        if ( false == pConnection->OpenClientConnection( pUniqueObjID, pCommSegID, clientSemID, commSegSize, messageList ))
        {
            pTaskClientConnections->ReleaseConnectionObject( pConnection );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }

        RTETask_CommonQueueElement* pQueueElement = m_KGS.GlobalFreeList()->Dequeue();

        if ( 0 == pQueueElement )  
        {   // all tasks are busy no free connections
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_CKC_ALL_TASKS_ARE_BUSY, "Session");
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize,
                                                                 RTECKC_CommunicationSegment::TaskLimitExceeded );
            return;
        }

        SendConnectionToTask( pConnection, pQueueElement );
    }
    else  if ( 0 != pSessionTyp )
    {
        //
        // Create a new connection
        //
        RTETask_ITask::TaskType taskType;

        if ( SAPDB_StringsIdentical( pSessionTyp, "user" ) )
            taskType = RTETask_ITask::User;
        else if ( SAPDB_StringsIdentical( pSessionTyp, "event" ) )
            taskType = RTETask_ITask::Event;
        else
        {
            RTE_Message (SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_UNKNOWN_SESSION_TYPE, pSessionTyp ), MSG_DIAG );
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
            return;
        }

        RTECKC_KernelCommunication*    pKernelCommunication = m_KGS.KernelCommunication();
        RTETask_CommonQueueElement*    pQueueElement;
        SAPDB_UInt4                    retry  = 2;

        do
        {
          // we try to get a task from the unconnected task queue
            pQueueElement = pKernelCommunication->UnconnectedTaskPool().GetTask( taskType );

            if ( 0 == pQueueElement )
            {
                RTETask_TaskClientConnections::ReleaseOrphanedConnections();
                RTE_ISystem::DoSleep(3000); // sleep a moment and retry find free task
            }
        }
        while(( 0 == pQueueElement ) && retry--);

        if ( 0 == pQueueElement )  
        {   // all tasks are busy no free connections
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_CKC_ALL_TASKS_ARE_BUSY, SAPDB_ToString(taskType));
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize,
                                                                 RTECKC_CommunicationSegment::TaskLimitExceeded );
            return;
        }

        RTETask_Task&                   task                  = *pQueueElement->Task();
        RTETask_TaskClientConnections&  taskClientConnections = task.ClientConnections();
        RTETask_TaskClientConnection*   pConnection;

        pConnection = taskClientConnections.CreateConnectionObject( messageList );

        if ( 0 == pConnection )
        {
            pQueueElement->Release();
            task.EnqueueIntoUnconnectedTasksQueue();
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }

        // If task is inactive startup the task
        if ( false == task.Startup( messageList ))
        {
            pQueueElement->Release();
            task.EnqueueIntoUnconnectedTasksQueue();
            RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }

        // --- open connection to the client process
        if ( false == pConnection->OpenClientConnection( pUniqueObjID, pCommSegID, clientSemID, commSegSize, messageList ))
        {
            pQueueElement->Release();
            task.EnqueueIntoUnconnectedTasksQueue();
            taskClientConnections.ReleaseConnectionObject( pConnection );
            RTE_Message ( messageList, MSG_DIAG );
            return;
        }

        SendConnectionToTask( pConnection, pQueueElement );
    }
    else
    {
        RTE_Message (SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_SESSION_MISSING_TYP, pRequest->GetRequestURI().URI() ), MSG_DIAG );

        RTETask_TaskClientConnection::RejectConnectRequest ( pUniqueObjID, pCommSegID, clientSemID, commSegSize );
        return;
    }

}

/*---------------------------------------------------------------------------*/

void RTEThread_Requestor::SendConnectionToTask( RTETask_TaskClientConnection* pConnection,
                                                RTETask_CommonQueueElement*   pQueueElement )
{

    RTETask_Task&    task = *pConnection->Task();

    pQueueElement->Init( RTETask_CommonQueueElement::ConnectRequest );
    pQueueElement->ConnectRequestPart().Init( pConnection );
    task.EnqueueIntoExternalRequestsQueueAndWakeUP( *pQueueElement );

    return;
}

/*---------------------------------------------------------------------------*/

RTEThread_Requestor & RTEThread_Requestor::Instance( )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEThread_Requestor));

  if ( m_Instance == NULL )
    m_Instance = new (Space) RTEThread_Requestor();

  return *m_Instance;
}

RTEThread_Requestor * RTEThread_Requestor::m_Instance = NULL;

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/