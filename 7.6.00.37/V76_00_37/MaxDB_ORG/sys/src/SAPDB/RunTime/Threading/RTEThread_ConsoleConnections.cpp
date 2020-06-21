/*!========================================================================

  @file         RTEThread_ConsoleConnections.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Console connection object

                Handles all console connections

  @since        2005-06-22  12:53
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined (SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

#include "RunTime/RTE_CompilerFeatures.h"
#include "RunTime/Threading/RTEThread_ConsoleConnections.h"
#include "RunTime/Threading/RTEThread_ConsoleConnections.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Threading/RTEThread_ConsoleStandardWorker.hpp"
#include "RunTime/RTE_Console_Thread_Messages.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"

extern SAPDBTrace_Topic Console_Trace; ///< global console trace object


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTE_MAX_CONSOLE_IDLE_TIME      300    /* seconds; After this period of inactivity */
                                              /* time the connection is disconnected */


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

RTEThread_ConsoleConnections::RTEThread_ConsoleConnections ()  
                             : m_Connections()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleConnections::RTEThread_ConsoleConnections", 
                              Console_Trace, 9);
}

/*---------------------------------------------------------------------------*/

RTEThread_ConsoleConnections::~RTEThread_ConsoleConnections()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleConnections::~RTEThread_ConsoleConnections", 
                             Console_Trace, 9);

    RTE_HandleManager::Hdl    hdl;
    RTE_HandleManager::HdlRet hdlRet;
   
    hdlRet = m_Connections.FindFirstHandle ( hdl );

    while ( RTE_HandleManager::NoError != hdlRet )
    {
        RTEThread_ConsoleWorkerBase*  pWorker;

        hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

        if ( RTE_HandleManager::NoError == hdlRet )
        {   
            SAPDBErr_MessageList   messageList;
            destroy (pWorker, RTEMem_Allocator::Instance());
        }
        hdlRet = m_Connections.FindNextHandle( hdl );
    }

    m_Connections.Finish();
}

/*---------------------------------------------------------------------------*/

RTEThread_ConsoleConnections * RTEThread_ConsoleConnections::m_Instance = NULL;

RTEThread_ConsoleConnections&  RTEThread_ConsoleConnections::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEThread_ConsoleConnections));

    SAPDBTRACE_ROUTINE_DEBUG("RTEThread_ConsoleConnectionst::Instance", Console_Trace, 9);

    if ( m_Instance == NULL )
    {
        m_Instance = new (Space) RTEThread_ConsoleConnections();
        SAPDBERR_ASSERT_STATE(m_Instance != NULL);
    }

    return *m_Instance;
}


/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleConnections::CreateWorker( RTEThread_ConsoleCommand     &command,
                                                        RTEThread_ConsoleWorkerBase* &pWorker,
                                                        SAPDBErr_MessageList&        messageList )
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::CreateWorker", 
                              Console_Trace, 1);

    switch (command.Data().connectData.commType)
    {
    case RTE_CONS_COM_STANDARD:
        pWorker = new (RTEMem_Allocator::Instance()) RTEThread_ConsoleStandardWorker( RTE_CONSOLE_SERVER_MODE );

        if ( 0 == pWorker )
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                               "RTEThread_ConsoleStandardWorker");       
            return RTE_CONS_ERROR;
        }

        break;
        /* Example:
           A support for a other communication types, e.g. XML, may be included at this point
           case RTE_CONS_COM_XML:
           break;
        */
    default:
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_COMMUNICATION_TYPE, 
                                            SAPDB_ToString (command.Data().connectData.commType));
        return RTE_CONS_ERROR;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void  RTEThread_ConsoleConnections::OpenReject ( RTEThread_ConsoleCommand       &command,
                                                 RTE_HandleManager::Hdl         hdl,
                                                 RTEThread_ConsoleWorkerBase*   pWorker,
                                                 SAPDBErr_MessageList&          sendMessageList )
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::OpenReject", 
                              Console_Trace, 1);
    SAPDBErr_MessageList  tmpMessageList;

    SAPDBTRACE_WRITELN(Console_Trace, 5, "Client PID " << (SAPDB_UInt8)command.Data().connectData.pid
                       << ", Ref. "  << command.SenderRef() << ":"      
                       << " Connection rejected, Handle/Server Ref:" << hdl );

    if ( !pWorker->IsInitialized() && 
         !pWorker->Initialize ( command.Data().connectData.consoleShmID, 
                                command.Data().connectData.pid, hdl, 
                                command.SenderRef(), tmpMessageList ))
    {
        return;
    }

    pWorker->RejectConnectRequest( sendMessageList );

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleConnections::OpenConnection( RTEThread_ConsoleCommand    &command,
                                                          SAPDB_UInt4 const           threadIndex,
                                                          SAPDBErr_MessageList&       messageList )
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::OpenConnection", 
                              Console_Trace, 1);

    SAPDB_UInt4                   rc;
    RTEThread_ConsoleWorkerBase*  pWorker;

    SAPDBTRACE_WRITELN(Console_Trace, 5, "Client PID " << (SAPDB_UInt8)command.Data().connectData.pid
                        << ", Ref. "  << command.SenderRef() << ":"      
                        << " Create connection" );
        
    rc = CreateWorker( command, pWorker, messageList );

    if ( RTE_CONS_NO_ERROR == rc )
    {
        RTE_HandleManager::Hdl    hdl;
        RTE_HandleManager::HdlRet hdlRet;

        // get new handle from handle manager
        hdlRet = m_Connections.NewHandle( hdl, pWorker );

        if ( RTE_HandleManager::NoError != hdlRet )
        {   
            // new handle failed!
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_CONS_NO_FREE_CONNECTION,
                                                SAPDB_ToString (command.Data().connectData.pid), 
                                                SAPDB_ToString (command.SenderRef()) );
            // send an error to the client
            OpenReject ( command, hdl, pWorker, messageList );
            destroy (pWorker, RTEMem_Allocator::Instance());

            return RTE_CONS_ERROR;
        }

        // we have to lease the pWorker pointer to access this object!
        hdlRet = m_Connections.LeaseHandleData( hdl, reinterpret_cast<void**>(&pWorker) );

        if ( RTE_HandleManager::NoError != hdlRet )
        {    
            // oops... (InvalidHandle or FreePending) anybody else has dropped the connection!
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_THREAD_CONS_CONNECT_EXISTING, 
                                                SAPDB_ToString (hdl), SAPDB_ToString(0)); 
            return RTE_CONS_ERROR;
        }

        // Initialize worker
        if ( !pWorker->Initialize ( command.Data().connectData.consoleShmID, 
                                    command.Data().connectData.pid, hdl, 
                                    command.SenderRef(), messageList ))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_INIT_WORKER);

            // Because of initialize failed, we cannot send any error data to the client!
            //
            m_Connections.ReleaseHandleData(hdl);

            hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

            if ( RTE_HandleManager::LeasePending != hdlRet ) // Is anybody else using the connection?
                destroy (pWorker, RTEMem_Allocator::Instance());

            return RTE_CONS_ERROR;
        }

        // Connect to client
        rc = pWorker->ConnectRequest ( hdl, messageList );

        if ( RTE_CONS_NO_ERROR != rc )
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_CONNECT_FAILED); 

            m_Connections.ReleaseHandleData(hdl);

            hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

            if ( RTE_HandleManager::LeasePending != hdlRet ) // Is anybody else using the connection?
                destroy (pWorker, RTEMem_Allocator::Instance());

            return RTE_CONS_ERROR;
        }

#       if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
         pWorker->SetThreadIndex (threadIndex);
#       endif

        m_NumConnections.Increment();

        SAPDBTRACE_WRITELN(Console_Trace, 5, "Client PID " << (SAPDB_UInt8)pWorker->GetRemotePID()
                            << ", Ref. "  << command.SenderRef() << ":"      
                            << " Connection established, Handle/Server Ref:" << hdl );

        hdlRet = m_Connections.ReleaseHandleData(hdl);

        if ( RTE_HandleManager::NoError != hdlRet )
        {   
            // oops... (InvalidHandle or FreePending) anybody else has dropped the connection!
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_THREAD_CONS_CONNECT_EXISTING, 
                                                SAPDB_ToString (hdl), SAPDB_ToString(0)); 
            ReleaseWorker( pWorker );
            return RTE_CONS_ERROR;
        }
    }
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleConnections::RequestProcessing( RTEThread_ConsoleCommand    &command,
                                                             SAPDB_UInt4 const           threadIndex,
                                                             SAPDBErr_MessageList&       messageList )
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::RequestProcessing", 
                              Console_Trace, 1);

    SAPDB_UInt4                   rc;
    RTEThread_ConsoleWorkerBase*  pWorker;
    RTE_HandleManager::HdlRet     hdlRet;
    RTE_HandleManager::Hdl        hdl = command.ReceiverRef();

    // we have to lease the pWorker pointer to access this object!
    hdlRet = m_Connections.LeaseHandleData( hdl, reinterpret_cast<void**>(&pWorker) );

    if ( RTE_HandleManager::NoError != hdlRet )
    {   
        // oops...  invalid handle or anybody else has dropped the connection!
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_THREAD_CONS_CONNECT_EXISTING, 
                                           SAPDB_ToString (hdl), SAPDB_ToString(0)); 

        return RTE_CONS_ERROR;
    }

    SAPDBTRACE_WRITELN(Console_Trace, 5, "Client PID " << (SAPDB_UInt8)pWorker->GetRemotePID()
                       << " Request processing, Handle/Server Ref:" << hdl );

#   if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
     pWorker->SetThreadIndex (threadIndex);
#   endif

    /* Process request from XCons */
    rc = pWorker->ProcessRequest ( messageList );

    hdlRet = m_Connections.ReleaseHandleData(hdl);

    if ( RTE_HandleManager::NoError != hdlRet )
    {   
        // oops... anybody else has dropped the connection!
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_THREAD_CONS_CONNECT_EXISTING, 
                                            SAPDB_ToString (hdl), SAPDB_ToString(0)); 
        ReleaseWorker( pWorker );
        return RTE_CONS_ERROR;
    }

    if ( RTE_CONS_NO_ERROR != rc )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_REQUEST);

        hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

        if ( RTE_HandleManager::NoError == hdlRet )
          ReleaseWorker( pWorker );
    }


    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleConnections::CloseConnection ( RTEThread_ConsoleCommand    &command,
                                                            SAPDB_UInt4 const           threadIndex,
                                                            SAPDBErr_MessageList        &messageList )
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::Release", 
                              Console_Trace, 1);

    RTEThread_ConsoleWorkerBase*  pWorker;
    RTE_HandleManager::HdlRet     hdlRet;
    RTE_HandleManager::Hdl        hdl = command.ReceiverRef();

    // we will receive a pointer to access the worker object!
    hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

    if ( RTE_HandleManager::NoError != hdlRet )
    {   
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_THREAD_CONS_CONNECT_EXISTING, 
                                            SAPDB_ToString (hdl), SAPDB_ToString(0)); 
        return RTE_CONS_ERROR;
    }

#   if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
     pWorker->SetThreadIndex (threadIndex);
#   endif

    SAPDBTRACE_WRITELN(Console_Trace, 5, "Client PID " << (SAPDB_UInt8)pWorker->GetRemotePID()
                       << " Releasing connection, Handle/Server Ref:" << command.ReceiverRef() );

    return ReleaseWorker( pWorker, &messageList );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleConnections::ReleaseWorker ( RTEThread_ConsoleWorkerBase*   pWorker,
                                                          SAPDBErr_MessageList*          pMessageList )

{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::ReleaseWorker", 
                              Console_Trace, 1);
    SAPDB_UInt4                 rc;
    SAPDBErr_MessageList        tmpMessageList;

    if ( 0 == pMessageList )
         pMessageList = &tmpMessageList;

    rc = pWorker->DisconnectRequest( (*pMessageList) ); //Getting and sending Data from/to XCons

    if ( RTE_CONS_NO_ERROR != rc )
        (*pMessageList) = (*pMessageList) + SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_CONS_DISCONNECT_FAILED );

    m_NumConnections.Decrement();
    destroy (pWorker, RTEMem_Allocator::Instance());

    return rc;
}

/*---------------------------------------------------------------------------*/

void   RTEThread_ConsoleConnections::ScanConnectionsForTimeout ()
{
    SAPDBTRACE_METHOD_DEBUG ( "RTEThread_ConsoleConnections::ScanConnectionsForTimeout", 
                              Console_Trace, 1);

    SAPDBErr_MessageList      messageList;
    RTE_HandleManager::Hdl    hdl;
    RTE_HandleManager::HdlRet hdlRet;
    SAPDB_UInt8               currentTime = RTESys_Time ();
   
    hdlRet = m_Connections.FindFirstHandle ( hdl );

    while ( RTE_HandleManager::NoError == hdlRet )
    {
        RTEThread_ConsoleWorkerBase*  pWorker;

        // we have to lease the pWorker pointer to access this object!
        hdlRet = m_Connections.LeaseHandleData( hdl, reinterpret_cast<void**>(&pWorker) );

        if ( RTE_HandleManager::NoError == hdlRet )
        {   
            SAPDB_Bool   timeOut    = false;
            SAPDB_Bool   clientDied = false;
            SAPDB_UInt8  lastRequestProcessingTime =  pWorker->GetLastRequestProcessingTime();
            
            if ( pWorker->IsInitialized() )
            {
                if ( !pWorker->IsRequestPendig () && 
                    ( lastRequestProcessingTime < currentTime ) &&
                    ( currentTime - lastRequestProcessingTime > RTE_MAX_CONSOLE_IDLE_TIME ))
                {
                    timeOut = true;
                }
                else
                {
                    //Check if the client is still alive
                    RTESys_ProcessState     clientProcess (pWorker->GetRemotePID());

                    if (RTESys_ProcessState::Alive != clientProcess.State ())
                        clientDied = true;
                }
            }

            hdlRet = m_Connections.ReleaseHandleData(hdl);

            if ( RTE_HandleManager::NoError != hdlRet )
            {   
                // oops... anybody else has dropped the connection!
                ReleaseWorker( pWorker );
            }
            else if ( timeOut || clientDied )
            {
                // We will receive a new pointer to access the worker object!
                hdlRet = m_Connections.FreeHandle( hdl, reinterpret_cast<void**>(&pWorker) );

                if ( RTE_HandleManager::NoError == hdlRet )
                {   

                    if ( clientDied )
                    {
                        RTE_Message (SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_THREAD_CONS_CLIENT_DIED, 
                                                           SAPDB_ToString (pWorker->GetServerReference()),
                                                           SAPDB_ToString (pWorker->GetRemotePID ()),
                                                           SAPDB_ToString (pWorker->GetClientReference ()),
                                                           SAPDB_ToString (hdl) ));
                    }
                    else
                    {
                        //Send timeout to XCons
                        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_THREAD_CONS_TIMEOUT, 
                                                            SAPDB_ToString (pWorker->GetServerReference()),
                                                            SAPDB_ToString (pWorker->GetRemotePID ()),
                                                            SAPDB_ToString (pWorker->GetClientReference ()),
                                                            SAPDB_ToString (hdl) );

                        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        //The following send may not be blocking! Since XCons is not awaiting an answer
                        //at the moment we have to be sure that all error data fit in one packet.
                        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                        if ( !pWorker->SendError ( TIMEOUT, RTE_CONS_REMOTE_TIMEOUT, messageList ) )
                        {
                            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_ERR);
                            RTE_Message ( messageList );
                        }
                    }
                    ReleaseWorker( pWorker );
                }
            }
        }

        hdlRet = m_Connections.FindNextHandle( hdl );
    }

}

/*---------------------------------------------------------------------------*/

externC void   RTEThread_ConsoleScanConnectionsForTimeout()
{
    RTEThread_ConsoleConnections::Instance().ScanConnectionsForTimeout ();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
