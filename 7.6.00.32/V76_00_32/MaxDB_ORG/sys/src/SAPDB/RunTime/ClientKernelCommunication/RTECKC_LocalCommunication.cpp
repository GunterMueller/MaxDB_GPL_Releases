/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_LocalCommunication.cpp
  @author       StefanP

  @brief        Local Communication

  @since        2006-04-20  17:33
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

#include    "RunTime/System/RTESys_SecurityAttributes.hpp"
#include    "RunTime/IPC/RTEIPC_NamedSharedMemory.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_LocalCommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_Connect.hpp"
#include    "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/RTE_TagFileHandlingUNIX.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include    "RunTime/System/RTESys_ProcessID.hpp"

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

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

RTECKC_LocalCommunication::~RTECKC_LocalCommunication ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::~RTECKC_LocalCommunication", CKC_Trace, 9);

    if (!Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_LOCAL_COMM);
        RTE_Message (messageList, MSG_DIAG_CONSOLE);
    }
}

/*---------------------------------------------------------------------------*/


RTECKC_ClientCommunication::CommRC    
            RTECKC_LocalCommunication::Open (RTEComm_ParseURI const &           parseURIObj,
                                             SAPDB_UInt4 const                  clientRef, 
                                             SAPDB_UInt4 const                  numPacket,
                                             SAPDB_UInt4 &                      mdu,    
                                             SAPDBErr_MessageList &             messageList)
{
    SAPDB_Char *                            pCommSegName;
    RTE_UniqueIdBuffer                      uniqueID;
    SAPDB_UInt4                             lenCommSegName;
    SAPDB_UInt4                             commSegSize;
    RTE_OSPid                               serverPID;
    RTECKC_ClientCommunication::CommRC      rc = RTECKC_ClientCommunication::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::Open", CKC_Trace, 9);

    if (!GetServerDB (parseURIObj, messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

    rc = OpenGlobalCommunicationSegment (messageList);
    if (RTECKC_ClientCommunication::NoError != rc)
    {
        return rc;
    }

    if (!InitServerSemaphores (messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

    //Build Communication Segment Name
    if (!RTE_GetUID (uniqueID))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_GET_UNIQUE_NAME, 
            "Communication Segment");
        return RTECKC_ClientCommunication::Error;
    }

    lenCommSegName = static_cast <SAPDB_UInt4>(strlen (SHM_COMMSEG_NAME_SKELETON) + 
                                               strlen (reinterpret_cast <SAPDB_Char *> (uniqueID)));
 
    pCommSegName = static_cast <SAPDB_Char *> (alloca (lenCommSegName + 1));
    if (NULL == pCommSegName)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_ALLOCATE_COMMSEG_NAME,
            SAPDB_ToString (lenCommSegName + 1));    
        return RTECKC_ClientCommunication::Error;
    }

    strcpy (pCommSegName, SHM_COMMSEG_NAME_SKELETON);
    strcat (pCommSegName, reinterpret_cast <SAPDB_Char *> (uniqueID));

    if (!OpenCommunicationSegment (pCommSegName, 
                                   commSegSize, 
                                   RTESys_SecurityAttributes::Instance().GetWorldSA (),
                                   mdu,
                                   numPacket, 
                                   messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

    if (!InitSequenceCheck (messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

    if (RTECKC_ClientCommunication::NoError != (rc = OpenRequestChannel (messageList)))
    {
        return rc;
    }

    if (!CreateClientSemaphore (uniqueID, messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

    //Initialize the CKC Packet Header object
    m_PacketHeader = RTECKC_PacketHeader (clientRef, m_MPU);

    //Connect to Server
    rc = Connect ((SAPDB_Char *)uniqueID, commSegSize, parseURIObj, messageList);
    if (RTECKC_ClientCommunication::NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CONNECT_TO_SERVER);       
        return rc;
    }

    rc = ConnectReceive (serverPID, messageList);
    if (RTECKC_ClientCommunication::NoError == rc)
    {
        if (!OpenActualServerSemaphore (messageList))
        {
            rc = RTECKC_ClientCommunication::Error;
        }
        else
        {
            rc = OpenServerProcess (serverPID, messageList);
        }
    }

	if (RTECKC_ClientCommunication::NoError != rc)
    {
		m_pCommunicationSegment->SignalCloseToRemote (RTECKC_CommunicationSegment::Crashed);
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_LocalCommunication::PrepareClose (SAPDBErr_MessageList &          messageList)
{
    RTECKC_ClientCommSequenceCheck::SyncRet     rcSeq = RTECKC_ClientCommSequenceCheck::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::PrepareClose", CKC_Trace, 9);

    rcSeq = m_CommSeqCheck.EnterClose (messageList);
    if (RTECKC_ClientCommSequenceCheck::ClosePending == rcSeq)
    {
        return RTECKC_ClientCommunication::ClosePending;
    }

    //At this point no further request (cancel, send, receive, close) are possible.
    //Only pending request could exist such as receive, send or cancel
    
    //Try to release the receive request by posting the client semaphore
    if (RTECKC_ClientCommSequenceCheck::ReceivePending == rcSeq)
    {
        RTESync_BinarySemaphore::SemRC      rcSem = RTESync_BinarySemaphore::NoError;

        rcSem = m_ClientSem.Post (messageList);
        if (RTESync_BinarySemaphore::NoError != rcSem)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_POST_CLIENT_SEM);            
            return RTECKC_ClientCommunication::Error;
        }
    }

    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::Close (SAPDBErr_MessageList &          messageList)
{
    SAPDBErr_MessageList    tmpMessageList1, tmpMessageList2;
    SAPDB_Bool              rc = true;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::Close", CKC_Trace, 9);

    if (!CloseClientSemaphore (tmpMessageList1))
    {
        tmpMessageList2 = tmpMessageList2 + tmpMessageList1;       
        rc = false;
    }

    if (!CloseCommunicationSegment (tmpMessageList1))
    {
        tmpMessageList2 = tmpMessageList2 + tmpMessageList1;            
        rc = false;
    }

    if (!ReleaseRequestChannel (tmpMessageList1))
    {
        tmpMessageList2 = tmpMessageList2 + tmpMessageList1;           
        rc = false;
    }

    if (!ReleaseServerSemaphores (tmpMessageList1))
    {
        tmpMessageList2 = tmpMessageList2 + tmpMessageList1;
        rc = false;
    }

    if (!ReleaseGlobalCommunicationSegment (tmpMessageList1))
    {
        tmpMessageList2 = tmpMessageList2 + tmpMessageList1; 
        rc = false;
    }

	CloseServerProcess ();

    if (NULL != m_ServerDB)
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_ServerDB);
        m_ServerDB = NULL;
    }

	if (NULL != m_SessionURI)
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_SessionURI);
        m_SessionURI = NULL;
    }

    if (!rc)
    {
        messageList = tmpMessageList2;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::Send (SAPDB_Byte const * const   pSendData,
                                             SAPDB_UInt4 const          sendLen,
                                             SAPDBErr_MessageList &     messageList)
{
    RTECKC_CommunicationSegment::CommState  commState = RTECKC_CommunicationSegment::NoError;
    RTESync_BinarySemaphore::SemRC          rcSem = RTESync_BinarySemaphore::NoError;
    RTECKC_ClientCommSequenceCheck::SyncRet rcSeq = RTECKC_ClientCommSequenceCheck::NoError;
    SAPDB_Byte const * const                pPacket = pSendData - m_HeaderSize;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::Send", CKC_Trace, 9);

    rcSeq = m_CommSeqCheck.EnterSend (pPacket, messageList);
    if (RTECKC_ClientCommSequenceCheck::NoError != rcSeq)
    {
        if (rcSeq != RTECKC_ClientCommSequenceCheck::ClosePending)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_ENTER_SEND);
        }

        return (GetErrCodeFromSeqCheckError (rcSeq));
    }

    //At this point we have a valid packet pointer!

    if (!m_PacketHeader.SetRequest ((SAPDB_Byte *) (pPacket), sendLen, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SET_REQUEST_HEADER);
        return RTECKC_ClientCommunication::Error;
    }


    commState = m_pCommunicationSegment->Send (pPacket, messageList);
    if (RTECKC_CommunicationSegment::NoError != commState)
    {
         messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLIENT_SEND,
                                                          SAPDB_ToString (pPacket),
                                                          SAPDB_ToString (sendLen));

        return (GetErrCodeFromCommSegError (commState));
    }
//!!!Temporary Hack
//Das Setzen des CommFlags hat hier zu erfolgen und nicht in m_pCommunicationSegment!!!
//Setzen des CommFlags mittels Allokators und m_CommFlagIdx (Reply vom ConnectReply)
//    if ( pShmConnInfo->pulCommFlag != NULL )
//        *pShmConnInfo->pulCommFlag   = 1;

//An dieser Stelle ist Memory Barrier notwendig!!! (->Matsushita Problem)

#if !defined (CKC_TEST) && defined (WIN32)
        if (m_pCommFlag != NULL)
        {
            *m_pCommFlag = 1;
        }
#endif


    // ---  Wake up the UKT if necessary?

    if (!OpenActualServerSemaphore (messageList))
    {
        return RTECKC_ClientCommunication::Error;
    }

//!!!
//Optimization requires possibly a memory barrier
//    if (m_pGlobalCommSeg->IsUKTSemaphorePosted (m_ActualTaskSchedulerSemaphoreIdx))
    if (true)
    {
        rcSem = (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Post (messageList);
        if (RTESync_BinarySemaphore::NoError != rcSem)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_POST_SERVER_SEM,
                                                             SAPDB_ToString (m_ActualTaskSchedulerSemaphoreIdx));            
            return RTECKC_ClientCommunication::Error;
        }
    }

    m_CommSeqCheck.LeaveSend ();

    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECKC_LocalCommunication::ReplyAvailable ()
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::ReplyAvailable", CKC_Trace, 9);

    return (m_pCommunicationSegment->ReplyAvailable ());
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::Receive (SAPDB_Byte * &            pReceiveData,
                                                SAPDB_UInt4 &             replyLen,
                                                SAPDBErr_MessageList &    messageList)
{
    RTECKC_CommunicationSegment::CommState  commState = RTECKC_CommunicationSegment::NoError;
    RTECKC_ClientCommunication::CommRC      rc = RTECKC_ClientCommunication::NoError;
    RTESync_BinarySemaphore::SemRC          rcSem = RTESync_BinarySemaphore::NoError;
    RTECKC_ClientCommSequenceCheck::SyncRet rcSeq = RTECKC_ClientCommSequenceCheck::NoError;
    RTECKC_PacketHeader::RCPacketHeader     rcHeader = RTECKC_PacketHeader::NoError;
    SAPDB_Byte *                            pPacket = NULL;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::Receive", CKC_Trace, 9);

    pReceiveData = NULL;

    rcSeq = m_CommSeqCheck.EnterReceive (messageList);
    if (RTECKC_ClientCommSequenceCheck::NoError != rcSeq)
    {
        if (rcSeq != RTECKC_ClientCommSequenceCheck::ClosePending)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_ENTER_RECEIVE);
        }

        return (GetErrCodeFromSeqCheckError (rcSeq));
    }


    do 
    {
        commState = m_pCommunicationSegment->Receive (pPacket, messageList);
        if (RTECKC_CommunicationSegment::DataAvailable != commState &&
            RTECKC_CommunicationSegment::DataOutstanding != commState) 
        {
            SAPDB_Byte *    pErrorPacket;

            if (RTECKC_CommunicationSegment::DataMissing != commState && 
                m_pCommunicationSegment->RemotePacketAvailable (pErrorPacket))
            {
				SAPDBErr_MessageList     tmpMessageList;

                if (m_PacketHeader.CheckErrorReply (pErrorPacket, tmpMessageList))
				{
					messageList = messageList + tmpMessageList;
				}
            }
            else if (RTECKC_CommunicationSegment::Closed != commState &&
                     RTECKC_CommunicationSegment::Timeout != commState)
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RECEIVE);
            }

            return (GetErrCodeFromCommSegError (commState));
        }

        if (RTECKC_CommunicationSegment::DataOutstanding == commState)
        {
            rcSem = m_ClientSem.Wait (m_LookupTimeInterval, messageList);
            if (RTESync_BinarySemaphore::NoError != rcSem)
            {
                if (RTESync_BinarySemaphore::Timeout == rcSem)
                {//A timeout may only occur if the server state was last 'Alive' (see above)
                    if (RTESys_ProcessState::Alive != m_ServerProcess.State ())
                    {
                        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SERVERDB_OFFLINE);
                        return RTECKC_ClientCommunication::Offline;
                    }
                }
                else if (RTESync_BinarySemaphore::Removed == rcSem) 
                {
                    return RTECKC_ClientCommunication::ServerClose;
                }
                else
                {
                    messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WAIT_CLIENT_SEM);
                    return RTECKC_ClientCommunication::Error;
                }
            }
        }
    }
    while (RTECKC_CommunicationSegment::DataAvailable != commState);

    //we have received a reply packet
    rcHeader = m_PacketHeader.CheckReply (pPacket, replyLen, messageList);
    if (RTECKC_PacketHeader::NoError != rcHeader)
    {
        if (RTECKC_PacketHeader::ErrorMsgAvailable != rcHeader)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CHECK_REPLY_HEADER);            
        }

        return RTECKC_ClientCommunication::Error;
    }

    pReceiveData = pPacket + m_HeaderSize;

    rcSeq = m_CommSeqCheck.LeaveReceive (pPacket, messageList);
    if (RTECKC_ClientCommSequenceCheck::NoError != rcSeq)
    {
        return (GetErrCodeFromSeqCheckError (rcSeq));
    }

    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::CancelRequest (SAPDBErr_MessageList &         messageList)
{
    RTECKC_Request                              request (m_RequestChannel.GetMaxRequestLength ());
    RTEIPC_CommunicationObject::CommObjRC       rcReq = RTEIPC_CommunicationObject::NoError;
    RTECKC_ClientCommSequenceCheck::SyncRet     rcSeq = RTECKC_ClientCommSequenceCheck::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::CancelRequest", CKC_Trace, 9);
    
    rcSeq = m_CommSeqCheck.EnterCancel (messageList);
    if (RTECKC_ClientCommSequenceCheck::NoError != rcSeq)
    {
        return RTECKC_ClientCommunication::ClosePending;
    }

    if (!request.PutCancelRequest (m_SessionURI, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PUT_CANCEL_REQUEST);
        return RTECKC_ClientCommunication::Error;
    }

    rcReq = m_RequestChannel.Write (request, messageList);

    if (RTEIPC_CommunicationObject::NoError != rcReq)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WRITE_REQUEST_CHANNEL,
                                                         SAPDB_ToString (m_ServerDB));
    }

    return GetErrCodeFromIPCError (rcReq);
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::Connect (SAPDB_Char *                    uniqueID,
                                                SAPDB_UInt4 const               commSegSize,
                                                RTEComm_ParseURI const &        parseURIObj,
                                                SAPDBErr_MessageList &          messageList)
{
    SAPDB_Byte *                                pConnectPacket = m_pCommunicationSegment->GetPacket (RTECKC_CONNECT_PACKET_IDX);
    RTECKC_Connect                              connectData (pConnectPacket + m_HeaderSize, RTECKC_Client); 
    RTECKC_Request                              request (m_RequestChannel.GetMaxRequestLength ());
    RTEIPC_CommunicationObject::CommObjRC       rcReq = RTEIPC_CommunicationObject::NoError;
    RTECKC_CommunicationSegment::CommState      commState = RTECKC_CommunicationSegment::NoError;
    SAPDB_UInt4                                 sendLen;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::Connect", CKC_Trace, 9);

    //Write Connect Request to the communication segment: Has to be done of course before we write
    //to the mailslot

    connectData.SetClientPID (RTESys_ProcessID());
    connectData.SetConnectURI (parseURIObj);
    sendLen = connectData.GetDataLength ();

    if (!m_PacketHeader.SetConnectRequest (pConnectPacket, sendLen, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SET_CONNECT_REQUEST_HEADER);
        return RTECKC_ClientCommunication::Error;
    }

    commState = m_pCommunicationSegment->Send (pConnectPacket, messageList);
    if (RTECKC_CommunicationSegment::NoError != commState)
    {
         messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLIENT_SEND,
                                                          SAPDB_ToString (pConnectPacket),
                                                          SAPDB_ToString (sendLen));

        return (GetErrCodeFromCommSegError (commState));
    }

    if (!request.PutConnectRequest (SHM_COMMSEG_NAME_SKELETON, 
#if defined (_WIN32)
                                    SEM_CLIENT_NAME_SKELETON, 
#elif defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) || !defined (_REENTRANT)
                                    m_ClientSem.GetSemaphoreID (),
#endif
                                    uniqueID,
                                    commSegSize, 
                                    parseURIObj.Path(), 
                                    messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PUT_CONNECT_REQUEST);
        return RTECKC_ClientCommunication::Error;
    }

    rcReq = m_RequestChannel.Write (request, messageList);
    if (RTEIPC_CommunicationObject::NoError != rcReq)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WRITE_REQUEST_CHANNEL,
                                           SAPDB_ToString (m_ServerDB));
    }

    return GetErrCodeFromIPCError (rcReq);
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::ConnectReceive (RTE_OSPid &                    serverPID,
                                                       SAPDBErr_MessageList &         messageList)
{
    RTECKC_CommunicationSegment::CommState  commState = RTECKC_CommunicationSegment::NoError;
    RTESync_BinarySemaphore::SemRC          rcSem = RTESync_BinarySemaphore::NoError;
    SAPDB_Byte *                            pConnectPacket;
    RTECKC_PacketHeader::RCPacketHeader     rcHeader = RTECKC_PacketHeader::NoError;
	SAPDB_ULong								connectTimeout;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::ConnectReceive", CKC_Trace, 9);

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) || !defined(_REENTRANT) // Without pthead system scope mutex events are based on 
                    // IPC semaphores so far and those semaphores do not support timeouts!
	connectTimeout = RTE_INFINITE_WAIT;
#else
	connectTimeout = CONNECTION_TIMEOUT;
#endif

    rcSem = m_ClientSem.Wait (connectTimeout, messageList);
    if (RTESync_BinarySemaphore::NoError != rcSem)
    {
        if (RTESync_BinarySemaphore::Timeout == rcSem)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_TIMEOUT_CLIENT_SEM);
            return RTECKC_ClientCommunication::ClientConnectTimeout;
        }
        else if (RTESync_BinarySemaphore::Removed == rcSem) 
        {
            return RTECKC_ClientCommunication::ServerClose;
        }

        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WAIT_CLIENT_SEM);            
        return RTECKC_ClientCommunication::Error;
    }

    commState = m_pCommunicationSegment->Receive (pConnectPacket, messageList);
    if (RTECKC_CommunicationSegment::DataAvailable != commState)
    {
        SAPDB_Byte *    pErrorPacket;

        if (RTECKC_CommunicationSegment::DataMissing != commState && 
            RTECKC_CommunicationSegment::DataOutstanding != commState &&
            m_pCommunicationSegment->RemotePacketAvailable (pErrorPacket))
        {
            SAPDBErr_MessageList     tmpMessageList;

            if (m_PacketHeader.CheckErrorReply (pErrorPacket, tmpMessageList))
            {
                messageList = messageList + tmpMessageList;
            }
        }

        if (RTECKC_CommunicationSegment::Closed != commState &&
            RTECKC_CommunicationSegment::Timeout != commState)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_RECEIVE);
        }

        return (GetErrCodeFromCommSegError (commState));
    }

    //Lock Communication Segment
    //On Linux the Server has no right to unlink the shared memory filename from 
    //the filesystem. The 'file' is located in '/dev/shm'. The owner
    //of this directory is root and the sticky bit is set for the directory. 
    //This means that only the owner of the file, the owner of the
    //directory and the superuser can unlink/remove that file. 
    //Therefore locking is tried a second time here.

    if (!m_pCommunicationSegment->Lock (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_CKC_LOCK_COMMSEG);            
        RTE_Message (messageList);
    }

    RTECKC_ConnectReply     connectReplyData (pConnectPacket + m_HeaderSize, RTECKC_Client);
    SAPDB_UInt4             dataLen;

    //Evaluate connect reply
    rcHeader = m_PacketHeader.CheckConnectReply (pConnectPacket, dataLen, messageList);
    if (RTECKC_PacketHeader::NoError != rcHeader)
    {
        if (RTECKC_PacketHeader::ErrorMsgAvailable != rcHeader)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CHECK_CONNECT_REPLY_HEADER);            
        }

        return RTECKC_ClientCommunication::Error;
    }

    if (dataLen != connectReplyData.GetDataLength ())
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_REPLY_LENGTH,
                                           SAPDB_ToString (connectReplyData.GetDataLength ()),
                                           SAPDB_ToString (dataLen));            
        return RTECKC_ClientCommunication::Error;
    }

    serverPID = connectReplyData.GetServerPID (); 
#if !defined (CKC_TEST) && defined (_WIN32)
    m_pCommFlag = (SAPDB_ULong *)((SAPDB_Byte *)m_pFCS + m_pFCS->ulCommFlagOffset) + 
        connectReplyData.GetCommFlagIndex ();
#endif

    SAPDB_Char const *  pSessionURI = connectReplyData.GetSessionURI ();
//!!!
//-------------------------------------------------------
//Only for testing:
#if !defined (_WIN32)
    m_ServerSemID = connectReplyData.GetServerSemID ();
#endif
//-------------------------------------------------------

    m_SessionURI = static_cast <SAPDB_Char *> (RTEMem_RteAllocator::Instance().Allocate
                                                    (strlen (pSessionURI) + 1));
    if (NULL == m_SessionURI)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_ALLOCATE_SERVER_URI, 
            SAPDB_ToString (strlen (pSessionURI) + 1));    
        return RTECKC_ClientCommunication::Error;
    }

    strcpy (m_SessionURI, pSessionURI);
    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

void    RTECKC_LocalCommunication::CalculateMDU (SAPDB_UInt4 const              mcu, 
                                                 SAPDB_UInt4 const             granularity,
                                                 SAPDB_UInt4 const             commSegHeaderSize,
                                                 SAPDB_UInt4 &                 mdu)
const
{

    /*
        Formula to calculate the maximum data unit (mdu') from the given mdu:

        H(P) -  Headersize per packet (includes packet header H(RTE) as well as the Communication
                Segment header size H(C) in a proportional manner
        n    -  Number of packets
        G    -  Granularity for the shared memory allocation
        mcu  -  Maximum communication unit as configured

        H(p) := (H(C) + n * H(RTE))/n

        mdu' := ALIGN (min ( min (SAPDB_ALIGN (mdu + H(P), G), mcu) - H(P), mdu ), RTECKC_ALIGNMENT)  

        Optimal MDU:
        min (ALIGN (mdu + H(P), G), mcu) - H(P)
    */

    SAPDB_UInt4     sizeHeaderPerPacket;
    SAPDB_UInt4     headerSize;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::CalculateMDU", 
                              CKC_Trace, 9);

    headerSize = commSegHeaderSize + m_HeaderSize * m_NumPacket;

    sizeHeaderPerPacket = (headerSize + (m_NumPacket - 1)) / m_NumPacket; //round up 

    if (RTE_UNDEF_LEN4 == mdu)
    {
        mdu = mcu - sizeHeaderPerPacket;
    }
    else
    {
        mdu = SAPDB_MIN (SAPDB_MIN (SAPDB_ALIGN (mdu + sizeHeaderPerPacket, granularity), mcu) 
                         - sizeHeaderPerPacket, 
                         mdu);
    }

    mdu = SAPDB_ALIGN_8BYTE (mdu + 1) - 8;  //round off

    return;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::OpenRequestChannel (SAPDBErr_MessageList &    messageList)
{
    RTEIPC_CommunicationObject::CommObjRC          rcReq = RTEIPC_CommunicationObject::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::OpenRequestChannel", CKC_Trace, 9);
    rcReq = m_RequestChannel.Open (RTE_OpenExisting, 
                                    reinterpret_cast <SAPDB_Char *> (m_ServerDB), 
                                    RTESys_SecurityAttributes::Instance().GetNoSA (), 
                                    messageList);
    if (RTEIPC_CommunicationObject::NoError != rcReq)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_REQUEST_CHANNEL);       
    }

    return GetErrCodeFromIPCError (rcReq);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::ReleaseRequestChannel (SAPDBErr_MessageList &    messageList)
{

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::ReleaseRequestChannel", CKC_Trace, 9);

    if (!m_RequestChannel.Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_REQUEST_CHANNEL);            
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::InitServerSemaphores (SAPDBErr_MessageList &    messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::InitServerSemaphores", CKC_Trace, 9);
//!!!Temporary hack
//    m_NumTaskSchedulerSemaphores = m_pGlobalCommSeg->GetNumOfUKT ();
#if defined (CKC_TEST) || !defined (_WIN32)
    m_NumTaskSchedulerSemaphores = 1;
#else
    m_NumTaskSchedulerSemaphores = m_pFCS->ulNumOfSemaphores;
#endif

    if (0 == m_NumTaskSchedulerSemaphores)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_NUM_UKT);       
        return false;
    }
    else
    {
        newarray (m_pServerSem, m_NumTaskSchedulerSemaphores, RTEMem_Allocator::Instance());
        if (NULL == m_pServerSem)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_ARRAY_INST, 
                SAPDB_ToString (m_NumTaskSchedulerSemaphores),
                "server semaphores");
            return false;
        }
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::OpenActualServerSemaphore (SAPDBErr_MessageList &         messageList)
{
    RTESync_BinarySemaphore::SemRC      rcSem = RTESync_BinarySemaphore::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::OpenActualServerSemaphore", CKC_Trace, 9);

//Can be optimized: Ask first if the index has changed. Then ask with inline method 'sem->IsOpen' (inline, has to be implemented)
//if the semaphore is already open.

    m_ActualTaskSchedulerSemaphoreIdx = m_pCommunicationSegment->GetActualTaskSchedulerSemaphoreIdx ();
#if defined (_WIN32) 
#if defined (CKC_TEST)
    rcSem = (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Open ("SDBTech-SEM-Server-NewCommunication", 
                                                   messageList);
#else
//!!!Temporary hack
    RTE_Path    serverSemName;

    strcpy (serverSemName, m_pFCS->szSemPath);
    strcat (serverSemName, (SAPDB_Char *)((PIPC_BASE_OBJ_NAME) ((SAPDB_Byte *) m_pFCS + m_pFCS->ulSemaNameOffset) + m_ActualTaskSchedulerSemaphoreIdx));
    rcSem = (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Open (serverSemName, messageList);
#endif
#else //UNIX

//-------------------------------------------------
//Testweise:
    rcSem = (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Open (m_ServerSemID, 
                                                   KernelUser, reinterpret_cast<SAPDB_Char *> (m_ServerDB), 
                                                   messageList);
//-------------------------------------------------

/*
    rcSem = (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Open (m_pGlobalCommSeg->GetUKTSemaphoreID (m_ActualTaskSchedulerSemaphoreIdx), 
                                                   KernelUser, reinterpret_cast<SAPDB_Char *> (m_ServerDB), 
                                                   messageList);
*/
#endif

    if (RTESync_BinarySemaphore::NoError != rcSem)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_OPEN_SERVER_SEM,
                                                          SAPDB_ToString (m_ActualTaskSchedulerSemaphoreIdx + 1));       
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::ReleaseServerSemaphores (SAPDBErr_MessageList &    messageList)
{
    SAPDB_Bool    rc = true;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::ReleaseServerSemaphores", CKC_Trace, 9);

    if (NULL != m_pServerSem)
    {
        for (SAPDB_UInt4    idx = 0; idx < m_NumTaskSchedulerSemaphores; ++idx)
        {
#if defined (_WIN32) 
            if (RTESync_BinarySemaphore::NoError != m_pServerSem[idx].Close (messageList))
#else
            if (RTESync_BinarySemaphore::NoError != m_pServerSem[idx].Close (messageList))
#endif
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_SERVER_SEM,
                                                                 SAPDB_ToString (idx + 1)); 
                rc = false;
            }
        }

        destroyarray (m_pServerSem, m_NumTaskSchedulerSemaphores, RTEMem_Allocator::Instance());
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC      
            RTECKC_LocalCommunication::OpenServerProcess (RTE_OSPid const                serverPID,
                                                          SAPDBErr_MessageList &         messageList)
{
    RTESys_ProcessState::ProcState  serverOpenState;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::OpenServerProcess", CKC_Trace, 9);


    m_ServerProcess.Open (serverPID);
    serverOpenState = m_ServerProcess.State ();

#if defined (_WIN32) || (!defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) && defined (_REENTRANT)) 
    m_LookupTimeInterval = (RTESys_ProcessState::Unknown == m_ServerProcess.State ()) ?  
                            RTE_INFINITE_WAIT : 
                            LOOKUP_ALIVE_TIME_INTERVAL;
#else
//timeout on Linux or non multithreaded UNIX applications not yet implemented
    m_LookupTimeInterval = RTE_INFINITE_WAIT;
#endif

    if (RTESys_ProcessState::Alive != serverOpenState)
    {
        if (RTESys_ProcessState::Died == serverOpenState)
        {//Server is offline
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_SERVER_STATE,
                                                              SAPDB_ToString (serverPID), 
                                                              SAPDB_ToString (serverOpenState));     
            return RTECKC_ClientCommunication::Offline;
        }
        else //State == Unknown
        {
            //We may not have the permission to open the Process. Therefore this state is absolutely
            //acceptable
            ;
        }
    }

    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

void    RTECKC_LocalCommunication::CloseServerProcess ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::CloseServerProcess", CKC_Trace, 9);

	m_ServerProcess.Close ();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::CreateClientSemaphore (RTE_UniqueIdBuffer              uniqueID,
                                                                SAPDBErr_MessageList &          messageList)
{
    RTESync_BinarySemaphore::SemRC      rcSem = RTESync_BinarySemaphore::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::CreateClientSemaphore", CKC_Trace, 9);

#if defined (_WIN32)
    RTESync_SemaphoreID                     clientSemID;
    SAPDB_UInt4                             lenSemaphoreID;

    //Build Semaphore ID

    lenSemaphoreID = static_cast <SAPDB_UInt4>(strlen (SEM_CLIENT_NAME_SKELETON) + 
                                               strlen (reinterpret_cast <SAPDB_Char *> (uniqueID)));
 
    clientSemID = static_cast <RTESync_SemaphoreID> (alloca (lenSemaphoreID + 1));
    if (NULL == clientSemID)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_ALLOCATE_CLIENT_SEM_NAME,
            SAPDB_ToString (lenSemaphoreID + 1));    
        return RTECKC_ClientCommunication::Error;
    }

    strcpy (clientSemID, SEM_CLIENT_NAME_SKELETON);
    strcat (clientSemID, reinterpret_cast <SAPDB_Char *> (uniqueID));

    rcSem = m_ClientSem.Create  (clientSemID, 
                                 RTESys_SecurityAttributes::Instance().GetWorldSA (), 
                                 messageList);
#elif !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) && defined (_REENTRANT)
	rcSem = m_ClientSem.Create (m_pCommunicationSegment->GetUNIXEventBuffer (), 
								PTHREAD_PROCESS_SHARED, 
								messageList);
#else
    rcSem = m_ClientSem.Create  (ClientUser, 
                                 reinterpret_cast<SAPDB_Char *> (m_ServerDB), 
                                 RTESys_SecurityAttributes::Instance().GetOwnSA (), 
                                 messageList);
#endif
    if (RTESync_BinarySemaphore::NoError != rcSem)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CREATE_CLIENT_SEM);       
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::CloseClientSemaphore (SAPDBErr_MessageList &    messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::CloseClientSemaphore", CKC_Trace, 9);

    if (RTESync_BinarySemaphore::NoError != m_ClientSem.Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_CLIENT_SEM);  
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::GetServerDB (RTEComm_ParseURI const &    parseURIObj,
                                                      SAPDBErr_MessageList &      messageList)
{
    SAPDB_UInt4     lenServerDB;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::GetServerDB", CKC_Trace, 9);

    if (parseURIObj.HasDBPath ())
    {
        lenServerDB = static_cast <SAPDB_UInt4>(strlen ((SAPDB_Char *) (parseURIObj.DBPath()->ServerDB())));
        if (0 != lenServerDB)
        {
            m_ServerDB = static_cast <SAPDB_UTF8 *> (RTEMem_RteAllocator::Instance().Allocate
                                        (lenServerDB * sizeof(SAPDB_UTF8) + 1));
            if (NULL == m_ServerDB)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_ALLOCATE_DB_NAME, 
                                (SAPDB_Char *)(parseURIObj.DBPath ()->ServerDB ()),
                                SAPDB_ToString (lenServerDB * sizeof(SAPDB_UTF8) + 1));    
                return false;
            }

            strcpy (reinterpret_cast<SAPDB_Char *> (m_ServerDB), 
                    (SAPDB_Char *)(parseURIObj.DBPath ()->ServerDB ()));

			for (SAPDB_UInt4 idx = 0; m_ServerDB[idx] != 0; ++idx)
			{
				m_ServerDB[idx] = toupper (m_ServerDB[idx]);
			}
        }
        else
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_MISSING_SERVERDB,
                parseURIObj.URI ());
            return false;
        }
    }
    else
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_MISSING_DB_PATH, 
            parseURIObj.URI ());
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC          
            RTECKC_LocalCommunication::OpenGlobalCommunicationSegment (SAPDBErr_MessageList &    messageList)
{
//!! temporary hack
#if !defined (CKC_TEST) && defined (_WIN32)
    RTE_Path    globalCommSegName;
    RTEIPC_NamedSharedMemory::SHMRet        rcSHM = RTEIPC_NamedSharedMemory::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::OpenGlobalCommunicationSegment", CKC_Trace, 9);

    strcpy (globalCommSegName, SHM_FCS);
    strcat (globalCommSegName, (SAPDB_Char *)m_ServerDB);
    rcSHM = m_GlobalCommSeg.Open (globalCommSegName, 
                                  NULL, 
                                  RTE_OpenExisting, 
                                  RTESys_SecurityAttributes::Instance().GetNoSA (), 
                                  0, 
                                  (void **)(&m_pFCS), 
                                  messageList);
    if (RTEIPC_NamedSharedMemory::NoError != rcSHM)
    {
        if (RTEIPC_NamedSharedMemory::ResourceMissing == rcSHM)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_GLOBAL_COMMSEG_MISSING);
            return RTECKC_ClientCommunication::ResourceMissing;
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_GLOBAL_COMMSEG);
            return RTECKC_ClientCommunication::Error;

        }
    }

/* vos33:
    rc = sql41c_get_named_shrd_mem ( (PPVOID)ppFCS, SHM_FCS,
                                   pszFCSName, phFCS );

  if (( rc == ERROR_FILE_NOT_FOUND ) || ( rc == ERROR_INVALID_NAME ))
    {
    MSGD (( ERR_DATABASE_NOT_STARTED, pszFCSName ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_DATABASE_NOT_STARTED, 0 );

    DBGOUT;
    return ( commErrStartRequired_esp01 );
    }

  if ( rc != NO_ERROR )
    {
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_OR_DB_NOT_ACC, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );
*/

#endif


    //Open Global Communication Segment (formerly known as Flag Communication Segment)
    //OpenGlobalCommunicationSegment (m_ServerDB, m_pGlobalCommSeg);
/*
    m_pGlobalCommSeg = new (RTEMem_Allocator::Instance()) RTECKC_GlobalCommunicationSegment (m_ServerDB);
    if (NULL == m_pGlobalCommSeg)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
            "RTECKC_GlobalCommunicationSegment");       
        return false;
    }
*/
    return RTECKC_ClientCommunication::NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::ReleaseGlobalCommunicationSegment (SAPDBErr_MessageList &    messageList)
{
    SAPDB_Bool    rc = true;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::ReleaseGlobalCommunicationSegment", CKC_Trace, 9);

#if !defined (CKC_TEST) && defined (_WIN32)    
    if (RTEIPC_NamedSharedMemory::NoError != m_GlobalCommSeg.Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_GLOBAL_COMM_SEG);            
        rc = false;
    }

    m_pFCS = NULL;
    m_pCommFlag = NULL;
#endif
   
    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::OpenCommunicationSegment (SAPDB_Char const * const         commSegName,
                                                                   SAPDB_UInt4 &                    commSegSize,
                                                                   RTE_SECURITY_ATTRIBUTES &        secAttribute,   
                                                                   SAPDB_UInt4 &                    mdu,    
                                                                   SAPDB_UInt4 const                numPacket,
                                                                   SAPDBErr_MessageList &           messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::OpenCommunicationSegment", CKC_Trace, 9);

    //Check number of packets: Maximal number of packets should be configurated in XParam and
    //is located in the Global Communication Segment
//!!!
//    if (numPacket > m_pGlobalCommSeg->GetMaxNumPackets ())
    if (numPacket > 2)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_NUM_PACKETS,
//                                            SAPDB_ToString (m_pGlobalCommSeg->GetMaxNumPackets ()),
                                            SAPDB_ToString (2),
                                            SAPDB_ToString (numPacket));
        return false;
    }

    m_NumPacket = numPacket;

    //Calculate maximal possible MPU: Only this method is also used by an info request (XSERVER)
    //Die Granularität im GlobalCommSeg sollte <= der Granularität für den konfigurierten Wert mcu 
    //liegen
//!!!
    CalculateMDU (64 * 1024, 64 * 1024,
                  RTECKC_ClientCommunicationSegment::GetCommSegHeaderSize (m_NumPacket), mdu);

//    CalculateMDU (m_pGlobalCommSeg->GetMCU (), m_pGlobalCommSeg->GetGranularity (),
//                  RTECKC_CommunicationSegment::GetCommSegHeaderSize (m_NumPacket), mdu);

    m_MPU = mdu + m_HeaderSize;
    
    //Communication Segment
    m_pCommunicationSegment = new (RTEMem_Allocator::Instance()) RTECKC_ClientCommunicationSegment 
                                                                        (m_MPU, 
                                                                         m_NumPacket);
    if (NULL == m_pCommunicationSegment)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
            "RTECKC_CommunicationSegment");       
        return false;
    }
//!!!
//    commSegSize = GetCommSegSize (m_pGlobalCommSeg->GetGranularity (),
    commSegSize = GetCommSegSize (64 * 1024,
                                  m_pCommunicationSegment->GetCommSegHeaderSize (m_NumPacket), 
                                  m_MPU);

    if (!m_pCommunicationSegment->Open (commSegName,
                                        commSegSize,
                                        secAttribute,
                                        messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_COMM_SEG);       
        return false;
    }

/*Example code for changing the owner of the shared memory
  
  Is not necessary here because we create the shared memory with world access. For security reasons
  it is locked by the kernel after attaching to the shared memory

#if !defined (_WIN32)
    SAPDB_Int4 dummy, groupId;

    if (!RTE_GetDefaultSapdbUserAndGroupIds(dummy, groupId, messageList) ||
        !m_pCommunicationSegment->ChangeOwner (groupId, messageList))
    {
        RTE_Message (messageList);
        return false;
    }
#endif
*/
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_LocalCommunication::CloseCommunicationSegment (SAPDBErr_MessageList &    messageList)
{
    SAPDB_Bool                          rc = true;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::CloseCommunicationSegment", CKC_Trace, 9);

    if (NULL != m_pCommunicationSegment)
    {
        if (!m_pCommunicationSegment->Close (messageList))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_COMM_SEG);            
            rc = false;
        }
    }
//!!!Temporary Hack
/*
     if ( pShmConnInfo->pulCommFlag != NULL )
       *pShmConnInfo->pulCommFlag  = 1;
*/
#if !defined (CKC_TEST) && defined (_WIN32)
    if (NULL != m_pCommFlag)
    {
        *m_pCommFlag = 1;
    }
#endif

    if (NULL != m_pServerSem)
    {
        if (SAPDB_MAX_UINT4 != m_ActualTaskSchedulerSemaphoreIdx && 
            (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->IsOpen ())
        {
            //Ignoring errors
            (m_pServerSem + m_ActualTaskSchedulerSemaphoreIdx)->Post (messageList);
        }
    }

    destroy (m_pCommunicationSegment, RTEMem_Allocator::Instance());

    return rc;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool    RTECKC_LocalCommunication::InitSequenceCheck (SAPDBErr_MessageList &    messageList)
{
    RTECKC_ClientCommSequenceCheck::SyncRet    rc = RTECKC_ClientCommSequenceCheck::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::InitSequenceCheck", CKC_Trace, 9);

    rc = m_CommSeqCheck.Set (m_pCommunicationSegment->GetPacket (0), m_MPU, m_NumPacket, messageList);
    if (RTECKC_ClientCommSequenceCheck::NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_INIT_SEQ_CHECK);            
        return false;
    }

    return true;
}


/*---------------------------------------------------------------------------*/

inline RTECKC_ClientCommunication::CommRC    
            RTECKC_LocalCommunication::GetErrCodeFromSeqCheckError (RTECKC_ClientCommSequenceCheck::SyncRet const    rcSeq)
const
{
    switch (rcSeq)
    {
    case RTECKC_ClientCommSequenceCheck::NoError:
        return RTECKC_ClientCommunication::NoError;

    case RTECKC_ClientCommSequenceCheck::SendPending:
        return RTECKC_ClientCommunication::SendPending;

    case RTECKC_ClientCommSequenceCheck::PacketInUse:
        return RTECKC_ClientCommunication::PacketInUse;

    case RTECKC_ClientCommSequenceCheck::ClosePending:
        return RTECKC_ClientCommunication::ClosePending;

    case RTECKC_ClientCommSequenceCheck::NoOutstandingPackets:
        return RTECKC_ClientCommunication::NoPendingRequest;

    case RTECKC_ClientCommSequenceCheck::ReceivePending:
        return RTECKC_ClientCommunication::ReceivePending;
    }

    return RTECKC_ClientCommunication::Error;
}

/*---------------------------------------------------------------------------*/

inline RTECKC_ClientCommunication::CommRC    
            RTECKC_LocalCommunication::GetErrCodeFromCommSegError (RTECKC_CommunicationSegment::CommState const    commState)
const
{

    switch (commState)
    {
    case RTECKC_CommunicationSegment::NoError:
        return RTECKC_ClientCommunication::NoError;

    case RTECKC_CommunicationSegment::TaskLimitExceeded:
        return RTECKC_ClientCommunication::TaskLimit;

    case RTECKC_CommunicationSegment::Starting:
        return RTECKC_ClientCommunication::Starting;

    case RTECKC_CommunicationSegment::WrongState:
        return RTECKC_ClientCommunication::WrongState;

    case RTECKC_CommunicationSegment::Timeout:
        return RTECKC_ClientCommunication::ServerTimeout;

    case RTECKC_CommunicationSegment::Shutdown:
        return RTECKC_ClientCommunication::Shutdown;

    case RTECKC_CommunicationSegment::Crashed:
        return RTECKC_ClientCommunication::ServerCrash;

    case RTECKC_CommunicationSegment::Closed:
        return RTECKC_ClientCommunication::ServerClose;

    case RTECKC_CommunicationSegment::DataMissing:
        return RTECKC_ClientCommunication::NoPendingRequest;

    }

    return RTECKC_ClientCommunication::Error;
}

/*---------------------------------------------------------------------------*/

inline RTECKC_ClientCommunication::CommRC    
RTECKC_LocalCommunication::GetErrCodeFromIPCError (RTEIPC_CommunicationObject::CommObjRC const    rcIPC)
const
{
    switch (rcIPC)
    {
    case RTEIPC_CommunicationObject::NoError:
        return RTECKC_ClientCommunication::NoError;

    case RTEIPC_CommunicationObject::ResourceMissing:
        return RTECKC_ClientCommunication::ResourceMissing;
    }

    return RTECKC_ClientCommunication::Error;
}

