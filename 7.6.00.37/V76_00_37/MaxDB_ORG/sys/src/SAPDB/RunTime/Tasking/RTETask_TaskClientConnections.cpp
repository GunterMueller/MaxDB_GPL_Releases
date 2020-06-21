/*!========================================================================

  @file         RTETask_TaskClientConnections.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task related client kernel connection informations

                Class which collects all connection related methodes and 
                data that belongs to a task that is doing client kernel 
                communication.
 
                This object belongs to a task object (RTETask_Task).

  @since        2004-10-12  10:54
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

#include    "RunTime/Tasking/RTETask_TaskClientConnections.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include    "RunTime/Tasking/RTETask_TaskClientConnection.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "RunTime/RTE_Comm_Messages.hpp"
#include    "SAPDBCommon/SAPDB_StrUtils.hpp"
#include    "RunTime/RTE_Crash.hpp"

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


void RTETask_TaskClientConnections::Initialize( RTETask_Task* pTask,
                                                SAPDB_Bool    isConnectable )
{
    m_pTask         = pTask;
    m_IsConnectable = isConnectable;

    SAPDBErr_MessageList messageList;

    if ( false == CreateSessionURIString( messageList ))
    {   // we are expecting that we have enough memory during kernel startup
        RTE_Crash( messageList );
    }
}

/*---------------------------------------------------------------------------*/

RTETask_TaskClientConnection * const
    RTETask_TaskClientConnections::CreateConnectionObject( SAPDBErr_MessageList &messageList )
{
    RTETask_TaskClientConnection *  pConnection;

    pConnection = new (RTEMem_Allocator::Instance()) RTETask_TaskClientConnection(m_pTask);

    if ( 0 == pConnection )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_ALLOC_INST,
                                            "RTETask_TaskClientConnection");
    }

    return pConnection;
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnections::UpdateClientsTaskSchedulerSemaphoreIndexes()
{
    RTETask_TaskClientConnection *  pConnection;

    // Step through the connection list and update clients task scheduler semaphore 
    // indexes. This is normaly done by the TaskScheduler after a task movement.
    pConnection = static_cast<RTETask_TaskClientConnection*>(m_ConnectionList.FirstElement());

    while ( pConnection )
    {
        pConnection->UpdateClientsTaskSchedulerSemaphoreIndex();

        pConnection = static_cast<RTETask_TaskClientConnection*>(pConnection->NextElement());
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnections::ReleaseAllConnections()
{
    RTETask_TaskClientConnection *  pConnection;

    // Step through the connection list and relase all open connections
    pConnection = static_cast<RTETask_TaskClientConnection*>(m_ConnectionList.FirstElement());

    while ( pConnection )
    {
        pConnection->Release();

        pConnection = static_cast<RTETask_TaskClientConnection*>(m_ConnectionList.FirstElement());
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnections::ReleaseConnectionObject( RTETask_TaskClientConnection*  pConnection )
{
    m_ConnectionList.Remove( *pConnection );

    if (( m_ConnectionList.IsEmpty() ) &&  // no one else connected?
        ( false == m_pTask->LegacyTaskCtrl().IsConnected() ))
    {         
        NewSessionURIString();
    }

    destroy( pConnection, RTEMem_Allocator::Instance() );
}

/*---------------------------------------------------------------------------*/

RTETask_ITaskClientConnection * const RTETask_TaskClientConnections::AcceptClientConnect()
{
    RTETask_ScopeRelatedTaskState taskState(m_pTask, RTETask_Task::TaskWaitingForConnect );

    RTETask_TaskClientConnection *  pConnection;

    // wait for a incomming connect request
    RTETask_CommonQueueElement*  pQueueElement = WaitForConnectRequest( RTETask_CommonQueueElement::ConnectRequest );

    // get connection object from queue element
    pConnection = pQueueElement->ConnectRequestPart().TaskClientConnection();

    // Insert new connection into tasks connection list
    m_ConnectionList.InsertTail( *pConnection );

    // release queue element to free list
    pQueueElement->Release();

    // Because of task load balancing the TaskScheduler might has changed
    UpdateClientsTaskSchedulerSemaphoreIndexes();

    return pConnection;
}

/*---------------------------------------------------------------------------*/

RTETask_CommonQueueElement* RTETask_TaskClientConnections::WaitForConnectRequest( RTETask_CommonQueueElement::ElementType  queueElemType )
{
    RTETask_CommonQueueElement*     pQueueElement;

    if ( m_ConnectionRequestQueue.QueueIsEmpty() )
        pQueueElement = &m_pTask->WaitForReschedule();
    else // Connect request already received!
        pQueueElement = m_ConnectionRequestQueue.Dequeue();


    if ( pQueueElement->GetType() != queueElemType )
    {
        RTE_Crash( SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_UNEXPECTED_QUEUE_ELEM,
                                         SAPDB_ToString(pQueueElement->GetType()),
                                         SAPDB_ToString(queueElemType) ));
    }

    return pQueueElement;
}

/*---------------------------------------------------------------------------*/

void  RTETask_TaskClientConnections::ReleaseOrphanedConnections()
{
#    if defined (_WIN32)
      // try release old orphaned connections
      sql53k_remove_unrel_connnections ( );
#    else
#    endif
}

/*---------------------------------------------------------------------------*/

RTETask_TaskClientConnections *  const
    RTETask_TaskClientConnections:: ParseSessionIdentifier( SAPDB_Char const * const sessionIdentifier,
                                                            SAPDBErr_MessageList     &messageList )
{
    if ( strlen( sessionIdentifier ) != SESSION_IDENTIFIER_LEN )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_INVALID_SESSION_ID,
                                            sessionIdentifier );
        return 0;
    }

    // extract task id
    SAPDB_Char taskID[(SESSION_IDENTIFIER_LEN / 2 ) + 1];
    strcpy( taskID, sessionIdentifier + (SESSION_IDENTIFIER_LEN / 2) );

    RTETask_TaskClientConnections*   pConnections = 0;

    // get task object via the extracted task id string
    RTETask_Task* pTask = RTETask_Task::ByTaskID( (RTE_TaskId)atoi(taskID) );

    if ( pTask != 0 ) // task found?
    {
        // compare the session identifier
        if ( SAPDB_StringsIdentical ( pTask->ClientConnections().m_SessionIdentifier,
                                      sessionIdentifier ))
        {   // identifier is valid
            pConnections = &pTask->ClientConnections();

            if ( false == pConnections->IsConnectable())
            {   // Oops it's not allowed to connect this task!
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_INVALID_SESSION_ID,
                                                    sessionIdentifier );
                pConnections = 0;
            }

        }
    }

    if ( 0 == pConnections )
    {   
        // identifier is wrong, writer a error message ...
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_INVALID_SESSION_ID,
                                            sessionIdentifier );
    }

    return pConnections;
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnections::CreateNewSessionIdentifier()
{
    m_SessionIdentifierCounter++;
    SAPDB_sprintf( m_SessionIdentifier, 
                   sizeof(m_SessionIdentifier),
                   "%0*u%0*u",
                   SESSION_IDENTIFIER_LEN/2, m_SessionIdentifierCounter,
                   SESSION_IDENTIFIER_LEN/2, m_pTask->ID());
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnections::NewSessionURIString()
{
    if ( 0  != m_SessionURIString )
    {
        CreateNewSessionIdentifier();

        // we are expecting the format "/database/<serverdb>/session/<sessionid>"
        SAPDB_Char* 
            pUpdatePosition =  m_SessionURIString + 
                               strlen(m_SessionURIString) - 
                               strlen(m_SessionIdentifier);
        strcpy( pUpdatePosition, m_SessionIdentifier );
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool 
  RTETask_TaskClientConnections::CreateSessionURIString( SAPDBErr_MessageList     &messageList )
{
    // Do we already have one?
    if ( 0  != m_SessionURIString )
        return true;

    CreateNewSessionIdentifier();

    // Calculate the session URI string length
    SAPDB_Char const * pServerDB  = m_KGS.DBName();
    SAPDB_UInt4        len        = strlen("/database/")        + 
                                    strlen(pServerDB)           +
                                    strlen("/session/")         +
                                    strlen(m_SessionIdentifier) +
                                    1;                             // '\0'

    // Allocate the session URI string buffer
    m_SessionURIString = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate(len * sizeof(SAPDB_Char));

    if ( 0 == m_SessionURIString )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
        return false;
    }

    // Build the session URI string
    strcpy( m_SessionURIString, "/database/" );
    strcat( m_SessionURIString, pServerDB );
    strcat( m_SessionURIString, "/session/" );
    strcat( m_SessionURIString, m_SessionIdentifier );

    return true;
}

/*---------------------------------------------------------------------------*/

RTETask_TaskClientConnections::~RTETask_TaskClientConnections()
{
    if ( 0  != m_SessionURIString )
        RTEMem_RteAllocator::Instance().Deallocate(m_SessionURIString);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/