/*!========================================================================

  @file         RTETask_TaskClientConnection.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Single client kernel connection

                This class conatains all information of a 
                single client kernel connection. 
                Each task connected to client application(s)
                has at least one connection object in  
                it's task client connection list. 
                A new connection to a client leads to new
                object in this list. If a connection is 
                dropped its object is removed from list.

  @since        2005-09-21  14:13
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

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/Tasking/RTETask_TaskClientConnection.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_Connect.hpp"
#include    "RunTime/System/RTESys_ProcessID.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/System/RTESys_SecurityAttributes.hpp" 
#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#define MINIMUM_COMMAND_TIMEOUT                30  // - sec
#define INFINIT_COMMAND_TIMEOUT   (SAPDB_ULong)-1

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


SAPDB_Bool   
    RTETask_TaskClientConnection::OpenClientConnection ( SAPDB_Char const *      pUniqueObjID,
                                                         SAPDB_Char const *      pCommSegID,
                                                         RTESync_SemaphoreID    &clientSemID,
                                                         SAPDB_UInt4             commSegSize,
                                                         SAPDBErr_MessageList    &messageList )
{
    RTECKC_CommunicationSegment::CommState  commState;

    // Open IPC resources
    if ( false == OpenClientIPCResources( pUniqueObjID, pCommSegID, clientSemID, commSegSize,
                                          m_ClientSemaphore, m_CommunicationSegment, messageList ))
    {
        // OpenErrorCleanUp makes no sense because we 've no IPC resources
        return false;
    }

    // check the database state
    commState = CheckDBState ();

    if ( RTECKC_CommunicationSegment::NoError != commState )
    {   // its currently not allowed to connect to this database instance reject connection
        OpenErrorCleanUp( commState );
        return false;
    }

    // Receive connect request packet
    SAPDB_Byte *  pConnectPacket;

    commState = m_CommunicationSegment.Receive (pConnectPacket, messageList );

    if ( RTECKC_CommunicationSegment::DataAvailable != commState )
    {
        OpenErrorCleanUp();
        return false;
    }

    // Check communication packet header
    SAPDB_UInt4   headerSize = RTECKC_PacketHeader::GetPacketHeaderSize();
    SAPDB_UInt4   dataLength;

    // create a new packet header object
    m_PacketHeader = RTECKC_PacketHeader ( m_pTask->ID(), m_CommunicationSegment.GetMPU () );

    if (!m_PacketHeader.CheckConnectRequest ( pConnectPacket, dataLength, messageList) )
    {
        OpenErrorCleanUp();
        return false;
    }

    // Extract the packets data content
    RTECKC_Connect connectData (pConnectPacket + headerSize, RTECKC_Server); 

    if ( dataLength != connectData.GetDataLength ())
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_CONNECT_REQUEST_LENGTH,
                                            SAPDB_ToString(dataLength), 
                                            SAPDB_ToString(connectData.GetDataLength ()));
        OpenErrorCleanUp();
        return false;
    }

    // Get the client process id
    m_ClientPID = connectData.GetClientPID();
#if defined (_WIN32)
    m_CommFlagNo   = ((PTASK_CTRL_REC)m_pTask->LegacyTaskCtrl())->ulCommFlagNo;
    m_pCommFlag    = (SAPDB_UInt4*)&m_KGS.GetKGS()->pulCommFlags[m_CommFlagNo];
    m_pCommFlag[0] = 0;  // reset communication flag
#endif

    // open client state object to detect clients constitution
    m_ClientProcessState.Open( m_ClientPID );

    // Probably, the client died in between because of too slow response
    if ( RTESys_ProcessState::Died == m_ClientProcessState.State() )
    { 
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEWARN_CKC_CLIENT_DIED,
                                            SAPDB_ToString(m_ClientPID));
        OpenErrorCleanUp();
        return false;
    }
    // do we know the state?
    if ( RTESys_ProcessState::Unknown == m_ClientProcessState.State() )
    {   // .. no but write at least a message
        RTE_Message (SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_CKC_UNKNOWN_CLIENT_STATE,
                                           SAPDB_ToString(m_ClientPID)), MSG_DIAG_CONSOLE);
    }

    // parse and store complete connect URI 
    if ( false == connectData.GetConnectURI( m_URI, messageList ) )
    {
        OpenErrorCleanUp(commState );
        return false;
    }

    // initialize sequence check object
    if ( m_SequenceCheck.Set( m_CommunicationSegment.GetPacket(0), 
                              m_CommunicationSegment.GetMPU(),
                              m_CommunicationSegment.GetNumberPackets(),
                              messageList ) != RTECKC_KernelCommSequenceCheck::NoError )
    {
        OpenErrorCleanUp();
        return false;
    }

    //
    // Handle connect reply
    //
    RTECKC_ConnectReply connectReplyData (pConnectPacket + headerSize, RTECKC_Server);

    connectReplyData.SetServerPID    ( RTESys_ProcessID() );
    connectReplyData.SetCommFlagIndex( m_CommFlagNo );
    connectReplyData.SetSessionURI   ( m_pTask->ClientConnections().SessionURIString() );

    if (!m_PacketHeader.SetConnectReply ( pConnectPacket, connectReplyData.GetDataLength (), messageList ))
    {
        OpenErrorCleanUp();
        return false;
    }

    m_CommunicationSegment.SetActualTaskSchedulerSemaphoreIdx( m_pTask->TaskScheduler().SemaphoreIndex() );

    // Send connect reply via communication segment ....
    commState = m_CommunicationSegment.Send (pConnectPacket, messageList );
    if ( RTECKC_CommunicationSegment::NoError != commState )
    {
        OpenErrorCleanUp( commState );
        return false;
    }

    // ... and wake up the client
    RTESync_BinarySemaphore::SemRC rcSem = m_ClientSemaphore.Post ( messageList );

    if ( RTESync_BinarySemaphore::NoError != rcSem )
    {
        OpenErrorCleanUp();
        return false;
    }

    // Check if we have to deal with the legacy connection.
    if ( false == m_pTask->LegacyTaskCtrl().IsConnected() )
    {
        m_pTask->LegacyTaskCtrl().SetConnected();
        m_HandleLegacyConnection = true; 
    }

    // Update connection state
    m_pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

    return true;
}

/*---------------------------------------------------------------------------*/

inline void 
    RTETask_TaskClientConnection::OpenErrorCleanUp( RTECKC_CommunicationSegment::CommState commstate )
{
    SAPDBErr_MessageList    tempMessageList;

    if ( RTECKC_CommunicationSegment::NoError != commstate )
    {
        // send this error to the client
        m_CommunicationSegment.SignalCloseToRemote (RTECKC_CommunicationSegment::Error);
    }

    m_ClientSemaphore.Post (tempMessageList);
    m_ClientSemaphore.Close (tempMessageList);
    m_CommunicationSegment.Close (tempMessageList);
}

/*---------------------------------------------------------------------------*/

void 
  RTETask_TaskClientConnection::RejectConnectRequest ( SAPDB_Char const *                     pUniqueObjID,
                                                       SAPDB_Char const *                     pCommSegID,
                                                       RTESync_SemaphoreID                   &clientSemID,
                                                       SAPDB_UInt4                            commSegSize,
                                                       RTECKC_CommunicationSegment::CommState commState )
{
    RTECKC_KernelCommunicationSegment  communicationSegment;
    RTESync_BinarySemaphore            clientSemaphore;
    SAPDBErr_MessageList               messageList;

    // Open IPC resources
    if ( true == OpenClientIPCResources( pUniqueObjID, pCommSegID, clientSemID, commSegSize,
                                         clientSemaphore, communicationSegment, messageList ))
    {
      communicationSegment.SignalCloseToRemote (commState);
      clientSemaphore.Post (messageList);
      clientSemaphore.Close (messageList);
      communicationSegment.Close (messageList);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool 
    RTETask_TaskClientConnection::CloseClientConnection( SAPDBErr_MessageList &messageList )
{
    RTETask_ScopeRelatedTaskState taskState(m_pTask, RTETask_Task::TaskCloseConnection);

    SAPDB_Bool ok = true;

    if ( RTESync_BinarySemaphore::NoError != m_ClientSemaphore.Close( messageList ) )
    {
        ok = false;
        RTE_Message ( messageList, MSG_DIAG_CONSOLE );
    }

    if ( false == m_CommunicationSegment.Close( messageList ))
    {
        ok = false;
        RTE_Message ( messageList, MSG_DIAG_CONSOLE );
    }

    // Check if we have to deal with the legacy connection.
    if ( m_HandleLegacyConnection )
    {
        m_pTask->LegacyTaskCtrl().SetNotConnected();
        m_HandleLegacyConnection = false; 
    }

    // Update connection state
    m_pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

    return ok;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTETask_TaskClientConnection::MaxSendDataLength()
{
    return m_CommunicationSegment.GetMPU() - 
           RTECKC_PacketHeader::GetPacketHeaderSize();
}

/*---------------------------------------------------------------------------*/
    
RTETask_ITaskClientConnection::ConnRet
    RTETask_TaskClientConnection::Send( SAPDB_Byte * const   pData,
                                        SAPDB_UInt4 const    dataLen,
                                        SAPDBErr_MessageList &messageList )
{
    RTETask_ScopeRelatedTaskState taskState(m_pTask, RTETask_Task::TaskSend );

    SAPDB_Byte * pPacket = pData - RTECKC_PacketHeader::GetPacketHeaderSize();

    if ( m_SequenceCheck.EnterSend( pPacket, messageList ) != RTECKC_KernelCommSequenceCheck::NoError )
        return RTETask_ITaskClientConnection::SequenceError;

    if (!m_PacketHeader.SetReply (pPacket, dataLen, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SET_REQUEST_HEADER);
        return RTETask_ITaskClientConnection::Error;
    }

    m_pKernelCommunication->UpdateLastSendTime();

    RTECKC_CommunicationSegment::CommState
        commState = m_CommunicationSegment.Send ( pPacket, messageList );

    if ( RTECKC_CommunicationSegment::NoError != commState )
    {
         messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLIENT_SEND,
                                                          SAPDB_ToString (pPacket),
                                                          SAPDB_ToString (dataLen));

        return RTETask_ITaskClientConnection::Error;
    }

    RTESync_BinarySemaphore::SemRC
      rcSem = m_ClientSemaphore.Post (messageList);

    if ( RTESync_BinarySemaphore::NoError != rcSem )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_POST_CLIENT_SEM);            
        return RTETask_ITaskClientConnection::Error;
    }

    return RTETask_ITaskClientConnection::NoError;
}

/*---------------------------------------------------------------------------*/
    
RTETask_ITaskClientConnection::ConnRet
    RTETask_TaskClientConnection::Receive( SAPDB_UInt4          timeOut, 
                                           SAPDB_Byte * &       pData,
                                           SAPDB_UInt4 &        dataLen,
                                           SAPDBErr_MessageList &messageList )
{
    RTETask_ScopeRelatedTaskState taskState(m_pTask, RTETask_Task::TaskReceive );

    if ( m_SequenceCheck.EnterReceive(messageList) != RTECKC_KernelCommSequenceCheck::NoError )
        return RTETask_ITaskClientConnection::SequenceError;

    // Get the time we have entered...
    SAPDB_ULong enterTime = m_KGS.CurrTime();

    RTECKC_CommunicationSegment::CommState  commState;
    SAPDB_Byte*                             pPacket;

    // reset the communication flag;
//    m_pCommFlag[0] = 0;

    do 
    {
        // Update connection state
        m_pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

        // Try to receive new data ....
        commState = m_CommunicationSegment.Receive ( pPacket, messageList );

        // Update connection state
        m_pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

        if ( RTECKC_CommunicationSegment::DataOutstanding == commState )
        { 
           // ... there is no data available, we have to wait....
           // 

            // calculate timeout..
            //   if the requested timeout is 'NoSessionTimeout' or 
            //   if the ServerDB is not online the task might get a 
            //   infinit timeout value.
            if (( timeOut == RTETask_ITaskClientConnection::NoSessionTimeout ) || 
                (( RTE_KGS::RTEKGS_StateOnline != m_KGS.DBState()) &&
                 ( m_pTask->Type() == RTETask_ITask::Utility )))
            {  
                m_pTask->ClientConnections().SetSessionTimeOut(INFINIT_COMMAND_TIMEOUT);
            }
            else
            {
                if ( timeOut <= MINIMUM_COMMAND_TIMEOUT )
                    m_pTask->ClientConnections().SetSessionTimeOut(enterTime + MINIMUM_COMMAND_TIMEOUT);
                else
                    m_pTask->ClientConnections().SetSessionTimeOut(enterTime + timeOut);
            }

            // Get queue element from tasks freelist
            RTETask_CommonQueueElement& queueElement = m_pTask->CommonQueueElementFromFreeList();

            // Initialize communication queue element
            queueElement.Init( RTETask_CommonQueueElement::CommunicationReceiveWait );
            queueElement.CommunicationReceiveWaitPart().Init( m_pTask->ClientConnections().GetSessionTimeOut(),
                                                              m_pCommFlag[0] );

            m_pTask->EnqueueIntoCommunicationQueue( queueElement );

            // wait until we are avoke by the prior receive wait element
            RTETask_CommonQueueElement& returnElement = m_pTask->WaitForReschedule();

            SAPDBERR_ASSERT_STATE( returnElement == queueElement );

            if ( queueElement.CommunicationReceiveWaitPart().SessionTimeoutOccurred() )
            {
                queueElement.Release(); // we don't need the queue element release it
                return RTETask_ITaskClientConnection::SessionTimeout;
            }

            queueElement.Release(); // we don't need the queue element release it
        }
        else if ( RTECKC_CommunicationSegment::DataAvailable != commState )
        {
            if ( RTECKC_CommunicationSegment::Closed == commState )
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CLIENT_CLOSED_CONNECTION );
                return RTETask_ITaskClientConnection::ConnectionClosed;
            }

            return RTETask_ITaskClientConnection::Error;
        }

    } while (RTECKC_CommunicationSegment::DataAvailable != commState);

    if (!m_PacketHeader.CheckRequest (pPacket, dataLen, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CHECK_REPLY_HEADER);
        return RTETask_ITaskClientConnection::Error;
    }

    pData = pPacket + RTECKC_PacketHeader::GetPacketHeaderSize ();

    if ( m_SequenceCheck.LeaveReceive( pPacket, messageList) != RTECKC_KernelCommSequenceCheck::NoError )
        return RTETask_ITaskClientConnection::SequenceError;

    return RTETask_ITaskClientConnection::NoError;
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnection::Release()
{
    SAPDBErr_MessageList    messageList;

    CloseClientConnection( messageList );
    
    // Attention: THIS object is removed from heap, it must be the last
    //            operation in this methode. Using the object after
    //            this call is a fatal error !!!!!
    m_pTask->ClientConnections().ReleaseConnectionObject(this);
}

/*---------------------------------------------------------------------------*/

void RTETask_TaskClientConnection::UpdateClientsTaskSchedulerSemaphoreIndex()
{
    m_CommunicationSegment.SetActualTaskSchedulerSemaphoreIdx( m_pTask->TaskScheduler().SemaphoreIndex() );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool 
    RTETask_TaskClientConnection::OpenClientIPCResources( SAPDB_Char const *                pUniqueObjID,
                                                          SAPDB_Char const *                pCommSegID,
                                                          RTESync_SemaphoreID               &clientSemID,
                                                          SAPDB_UInt4                       commSegSize,
                                                          RTESync_BinarySemaphore           &clientSemaphore,
                                                          RTECKC_KernelCommunicationSegment &communicationSegment,
                                                          SAPDBErr_MessageList              &messageList )
{
    SAPDB_Char *                    pCommSegName;
    SAPDB_Bool                      Ok = true;

    // Build up the communication segment name first
    pCommSegName = static_cast <SAPDB_Char*>(alloca (strlen (pCommSegID) + strlen (pUniqueObjID) + 1));

    strcpy ( pCommSegName, pCommSegID );
    strcat ( pCommSegName, pUniqueObjID );

    // Open communication segment
    if (Ok = communicationSegment.Open(pCommSegName, 
                                       commSegSize, 
                                       RTESys_SecurityAttributes::Instance().GetNoSA (), 
                                       messageList))
    {
        //Try to lock Communication Segment
        //On LINUX the Kernel has no right to unlink the shared memory filename from 
        //the filesystem. The 'file' is located in '/dev/shm'. The owner
        //of this directory is root and the sticky bit is set for the directory. 
        //This means that only the owner of the file, the owner of the
        //directory and the superuser can unlink/remove that file. 
        //Therefore locking is tried a second time on the client side.
        communicationSegment.Lock (messageList);

#       if defined (_WIN32)
        SAPDB_Char*   pClientSemName;

        pClientSemName = static_cast <SAPDB_Char*>(alloca (strlen (clientSemID) + strlen (pUniqueObjID) + 1));

        strcpy ( pClientSemName, clientSemID );
        strcat ( pClientSemName, pUniqueObjID );

        Ok = clientSemaphore.Open( pClientSemName, messageList ) == RTESync_BinarySemaphore::NoError;
#       else // UNIX
#		if !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
		//Event is located in the communication segment

		//With 'clientSemID != RTE_SYNC_UNDEF_SEM_ID' one may check if we have to open a IPC semaphore
		//or a UNIX event (pthread mutex). This is not used since we do not support single threaded 
		//clients yet. 
		//Currently we have a strict separation between HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE (IPC) und !HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE (pthread_mutex)!
		Ok = clientSemaphore.Open (communicationSegment.GetUNIXEventBuffer (), messageList)
			 == RTESync_BinarySemaphore::NoError;
#		else
        Ok = clientSemaphore.Open (clientSemID,ClientUser, RTE_KGS::Instance().DBName(), 
                                    messageList) == RTESync_BinarySemaphore::NoError;
#       endif
#		endif
    }

    return Ok;
}

/*---------------------------------------------------------------------------*/

RTECKC_CommunicationSegment::CommState
    RTETask_TaskClientConnection::CheckDBState ()
{
    switch ( m_KGS.DBState() )
    {
    case RTE_KGS::RTEKGS_StateStarting      :
        return RTECKC_CommunicationSegment::Starting;

    case RTE_KGS::RTEKGS_StateShutdown      :
    case RTE_KGS::RTEKGS_StateShutdownReinit:
    case RTE_KGS::RTEKGS_StateShutdownKill  :
        return RTECKC_CommunicationSegment::Shutdown;

    case RTE_KGS::RTEKGS_StateAdmin         :
    case RTE_KGS::RTEKGS_StateOnline        :
    case RTE_KGS::RTEKGS_StateStandby       :
        break;

    default                                 :
        return RTECKC_CommunicationSegment::Crashed;
    }

    return RTECKC_CommunicationSegment::NoError;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
