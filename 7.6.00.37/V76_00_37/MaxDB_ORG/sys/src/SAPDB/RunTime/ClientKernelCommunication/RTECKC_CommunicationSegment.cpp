/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_CommunicationSegment.cpp
  @author       StefanP

  @brief        Communication Segment for the Client Kernel Communication

  @since        2003-11-13  15:15
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
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegmentHeader.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegment.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"

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
RTECKC_CommunicationSegment::~RTECKC_CommunicationSegment ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::~RTECKC_CommunicationSegment", CKC_Trace, 9);

    if (!Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_COMM_SEG);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECKC_CommunicationSegment::Open (SAPDB_Char const * const         commSegName,
                                               SAPDB_UInt4 const                commSegSize,
                                               RTE_SECURITY_ATTRIBUTES &        secAttribute,   
                                               SAPDBErr_MessageList &           messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::Open", 
        CKC_Trace, 9);

    RTEIPC_NamedSharedMemory::SHMRet        rcSHM = RTEIPC_NamedSharedMemory::NoError;
    SAPDB_Byte *                            pShm = NULL;
    SAPDB_UInt2                             offsetHeader = 0;
    SAPDB_UInt4                             offsetPacket = 0;
    RTECKC_CommSegHeaderCommonOptionPart *  pOptionPart;
    SAPDB_UInt4                             idx;


    rcSHM = RTEIPC_NamedSharedMemory::Open (commSegName, 
                                            NULL,
                                            (m_OpenMode == RTECKC_Client) ? RTE_OpenNew: RTE_OpenExisting,
                                            secAttribute,   
                                            commSegSize, 
                                            reinterpret_cast <void **> (&pShm), 
                                            messageList);

    if (RTEIPC_NamedSharedMemory::NoError != rcSHM)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_NAMED_SHM);
        return false;
    }

    //Communication segment header 
    m_pHeader = new (RTEMem_Allocator::Instance()) RTECKC_CommSegHeader (pShm, m_OpenMode);
    if (NULL == m_pHeader)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                           "RTECKC_CommSegHeader");       
        return false;
    }

    if (RTECKC_Client == m_OpenMode)
    {
        m_pHeader->SetVersion (COMMUNICATION_SEGMENT_VERSION);
        m_pHeader->SetNumberPackets (m_NumPacket);
        m_pHeader->SetMPU (m_MPU);

        //Option parts
        offsetHeader = m_pHeader->SetFirstOptionPartOffset (RTECKC_CommSegHeader::GetCommSegHeaderBufferSize());

		
		//Load Balancing
        pOptionPart = new (RTEMem_Allocator::Instance()) 
            RTECKC_CommSegHeaderCommonOptionPart (pShm + offsetHeader, m_OpenMode);
        if (NULL == pOptionPart)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                "RTECKC_CommSegHeaderCommonOptionPart");       
            return false;
        }

#if !defined (_WIN32) && !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) //&& defined (_REENTRANT)
		pOptionPart->SetOption (RTECKC_CommSegHeaderCommonOptionPart::LoadBalancing,
								RTECKC_LoadBalancingOption::GetLoadBalancingOptionBufferSize()
								+ RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize ());
#else
        pOptionPart->SetOption (RTECKC_CommSegHeaderCommonOptionPart::LoadBalancing);
#endif
        destroy (pOptionPart, RTEMem_Allocator::Instance());

        offsetHeader += RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize ();

        m_pOptionLoadBalancing = new (RTEMem_Allocator::Instance()) 
                                        RTECKC_LoadBalancingOption (pShm + offsetHeader);
        if (NULL == m_pOptionLoadBalancing)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                                             "RTECKC_LoadBalancingOption");       
            return false;
        }

        offsetHeader += RTECKC_LoadBalancingOption::GetLoadBalancingOptionBufferSize();

		//UNIX Event
#if !defined (_WIN32) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) // && defined (_REENTRANT)
        pOptionPart = new (RTEMem_Allocator::Instance()) 
            RTECKC_CommSegHeaderCommonOptionPart (pShm + offsetHeader, m_OpenMode);
        if (NULL == pOptionPart)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
				"RTECKC_CommSegHeaderCommonOptionPart");       
            return false;
        }

		pOptionPart->SetOption (RTECKC_CommSegHeaderCommonOptionPart::EventUNIX);
        destroy (pOptionPart, RTEMem_Allocator::Instance());
        offsetHeader += RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize ();

        m_pOptionClientEventUNIX = new (RTEMem_Allocator::Instance()) 
                                        RTECKC_ClientEventUNIXOption (pShm + offsetHeader);
        if (NULL == m_pOptionClientEventUNIX)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                                             "RTECKC_ClientEventUNIXOption");       
            return false;
        }

		offsetHeader += RTECKC_ClientEventUNIXOption::GetClientEventUNIXOptionBufferSize();
#endif

        m_pHeader->SetFirstPacketInfoOffset (offsetHeader);

    }
    else    //Server
    {
        SAPDB_UInt2     offsetToNextOption;

        if (m_pHeader->GetVersion () > COMMUNICATION_SEGMENT_VERSION)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_COMMSEG_VERSION,
                                                             SAPDB_ToString (COMMUNICATION_SEGMENT_VERSION),
                                                             SAPDB_ToString (m_pHeader->GetVersion ()));
            return false;
        }

        m_NumPacket = m_pHeader->GetNumberPackets ();
        m_MPU = m_pHeader->GetMPU ();

        //Option parts
        offsetToNextOption = m_pHeader->GetFirstOptionPartOffset ();

        while (RTE_UNDEF_OFFSET2 != offsetToNextOption)
        {
            offsetHeader += offsetToNextOption;
            pOptionPart = new (RTEMem_Allocator::Instance()) 
                                RTECKC_CommSegHeaderCommonOptionPart (pShm + offsetHeader, m_OpenMode);
            if (NULL == pOptionPart)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                    "RTECKC_CommSegHeaderCommonOptionPart");    
                return false;
            }

            switch (pOptionPart->GetOptionId ())
            {
            case RTECKC_CommSegHeaderCommonOptionPart::LoadBalancing:
                m_pOptionLoadBalancing = new (RTEMem_Allocator::Instance()) 
                                                RTECKC_LoadBalancingOption (pShm +
                                                                            offsetHeader +
                                                                            RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize ());
                if (NULL == m_pOptionLoadBalancing)
                {
                    messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                                                     "RTECKC_LoadBalancingOption");       
                    destroy (pOptionPart, RTEMem_Allocator::Instance());
                    return false;
                }

                pOptionPart->SetSupported ();
                break;

#if !defined (_WIN32) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) // && defined (_REENTRANT)
            case RTECKC_CommSegHeaderCommonOptionPart::EventUNIX:
                m_pOptionClientEventUNIX = new (RTEMem_Allocator::Instance()) 
                                                RTECKC_ClientEventUNIXOption (pShm +
                                                                              offsetHeader +
                                                                              RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize ());
                if (NULL == m_pOptionClientEventUNIX)
                {
                    messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                                                     "RTECKC_ClientEventUNIXOption");       
                    destroy (pOptionPart, RTEMem_Allocator::Instance());
                    return false;
                }

                pOptionPart->SetSupported ();
                break;
#endif
            default:
                RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_OPTION_NOT_SUPPORTED,
                    SAPDB_ToString (pOptionPart->GetOptionId ())));
            }

            offsetToNextOption = pOptionPart->GetOffsetToNextOption ();

            destroy (pOptionPart, RTEMem_Allocator::Instance());
        }

        offsetHeader = m_pHeader->GetFirstPacketInfoOffset ();

    }

    newarray (m_pPacketInfo, m_NumPacket, RTEMem_Allocator::Instance(), RTECKC_CommSegPacketInfo (m_OpenMode));
    if (NULL == m_pPacketInfo)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_ARRAY_INST, 
            SAPDB_ToString (m_NumPacket),
            "RTECKC_CommSegPacketInfo");
        return false;
    }

    //Packet infos
    for (idx = 0; idx < m_NumPacket; ++idx)
    {
        m_pPacketInfo[idx].SetPacketInfoBuffer (pShm + offsetHeader);
        offsetHeader += RTECKC_CommSegPacketInfo::GetPacketInfoBufferSize();
    }

    if (RTECKC_Client == m_OpenMode)
    {
        for (idx = 0; idx < m_NumPacket; ++idx)
        {
            m_pPacketInfo[idx].SetConnectionState (RTECKC_CommSegPacketInfo::Own, 
                                                   RTECKC_CommSegPacketInfo::Received);
            m_pPacketInfo[idx].SetConnectionState (RTECKC_CommSegPacketInfo::Remote, 
                                                   RTECKC_CommSegPacketInfo::Send);
        }
    }

    m_pFirstPacket = pShm + offsetHeader;

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECKC_CommunicationSegment::Close (SAPDBErr_MessageList &	messageList)
{
    RTEIPC_NamedSharedMemory::SHMRet        rcSHM = RTEIPC_NamedSharedMemory::NoError;
    SAPDB_Bool                              rc = true;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::Close", CKC_Trace, 9);

	SignalCloseToRemote ();

    rcSHM = RTEIPC_NamedSharedMemory::Close (messageList);
    if (RTEIPC_NamedSharedMemory::NoError != rcSHM)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_NAMED_SHM);
        rc = false;
    }

    // Destroy Load Balancing Option
    destroy (m_pOptionLoadBalancing, RTEMem_Allocator::Instance());

#if !defined (_WIN32) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) // && defined (_REENTRANT)
	// Destroy UNIX Event Option
    destroy (m_pOptionClientEventUNIX, RTEMem_Allocator::Instance());
#endif

    // Destroy Communication Segment Header
    destroy (m_pHeader, RTEMem_Allocator::Instance());

    // Destroy array of packet infos
    destroyarray (m_pPacketInfo, m_NumPacket, RTEMem_Allocator::Instance());

    m_LastSend = 0;
    m_NextReceive = 0;
    m_ActualReceivePacket = RTECKC_UNDEF_COMMUNICATION_PACKET_IDX;
    m_ActualSendPacket = RTECKC_CONNECT_PACKET_IDX;
	m_CloseSignaled = false;

    return rc;
}

/*---------------------------------------------------------------------------*/

void	RTECKC_CommunicationSegment::SignalCloseToRemote  (CommState const		state)
{
	if (NULL != m_pHeader && !m_CloseSignaled)
	{
		m_CloseSignaled = true;
		m_pHeader->SetMyCommState (RTECKC_CommSegHeader::Synchronous, state);
	}
}


/*---------------------------------------------------------------------------*/

RTECKC_CommunicationSegment::CommState     
                RTECKC_CommunicationSegment::Send (SAPDB_Byte const * const   pSendPacket,
                                                   SAPDBErr_MessageList &     messageList)
{
    RTECKC_CommSegPacketInfo::ConnectionStateType   remoteState, myState;
    SAPDB_UInt4                                     packetIdx;



    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::Send", 
                              CKC_Trace, 5);

    //Check packet pointer
    packetIdx = GetPacketIdx (pSendPacket);

    if (RTECKC_UNDEF_COMMUNICATION_PACKET_IDX == packetIdx)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_INVALID_SEND_PACKET,
                                           SAPDB_ToString (pSendPacket));
        return Error;
    }

    remoteState = m_pPacketInfo[packetIdx].GetConnectionState (RTECKC_CommSegPacketInfo::Remote);
    myState     = m_pPacketInfo[packetIdx].GetConnectionState (RTECKC_CommSegPacketInfo::Own);
    //Client state needs not to be verified because this has already be done by the layer above.
    //It is only be done for symmetric reasons
    if (RTECKC_CommSegPacketInfo::Send != remoteState ||
        RTECKC_CommSegPacketInfo::Received != myState)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WRONG_CONNECTION_STATE,
            "Send", 
            SAPDB_ToString (myState),
            SAPDB_ToString (remoteState));
        return WrongState;
    }

    m_ActualSendPacket = packetIdx;
    if (RTECKC_Client == m_OpenMode)
    {
        SetNextSendPacket (m_ActualSendPacket);
    }
    else
    {
        //Checks if we try to reply on the right packet 
        if (!CheckNextSendPacket (m_ActualSendPacket, messageList))
        {
            return Error;
        }
    }

    m_pPacketInfo[m_ActualSendPacket].SetConnectionState (RTECKC_CommSegPacketInfo::Own, 
            RTECKC_CommSegPacketInfo::Pending);
    return NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_CommunicationSegment::CommState     
                RTECKC_CommunicationSegment::Receive (SAPDB_Byte * &            pReceivePacket,
                                                      SAPDBErr_MessageList &    messageList)
{
    CommState                                       commRemoteStateAsync, commRemoteStateSync;
    CommState                                       commMyStateAsync;
    RTECKC_CommSegPacketInfo::ConnectionStateType   remoteState;
    SAPDB_Bool                                      rc = true;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::Receive", 
                              CKC_Trace, 5);

    pReceivePacket = NULL;

    //connection was terminated asynchronously?
    commRemoteStateAsync = m_pHeader->GetRemoteCommState (RTECKC_CommSegHeader::Asynchronous);
    commRemoteStateSync  = m_pHeader->GetRemoteCommState (RTECKC_CommSegHeader::Synchronous);
    commMyStateAsync = m_pHeader->GetMyCommState (RTECKC_CommSegHeader::Asynchronous);

    if (NoError != commRemoteStateAsync || NoError != commRemoteStateSync ||
        NoError != commMyStateAsync)
    {
        if (NoError != commMyStateAsync)
        {
            if (Timeout == commMyStateAsync)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_COMMAND_TIMEOUT);
                return commMyStateAsync;
            }
            else
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECTION_BROKEN,
                    "Receive",
                    SAPDB_ToString (m_pHeader->GetMyCommState (RTECKC_CommSegHeader::Synchronous)),
                    SAPDB_ToString (commMyStateAsync));
                return commMyStateAsync;
            }
        }
        else
        {
            if (Closed == commRemoteStateSync)
            {//--- the remote side has released the connection
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CONNECTION_RELEASED);
                return commRemoteStateSync;
            }

            if (Timeout == commRemoteStateSync)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_COMMAND_TIMEOUT);
                return commRemoteStateSync;
            }
            else
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECTION_BROKEN,
                    "Receive",
                    SAPDB_ToString (commRemoteStateSync),
                    SAPDB_ToString (commRemoteStateAsync));
                if (NoError != commRemoteStateAsync)
                {
                    return commRemoteStateAsync;
                }
                else
                {
                    return commRemoteStateSync;
                }
            }
        }
    }
    
    if (RTECKC_UNDEF_COMMUNICATION_PACKET_IDX == m_ActualReceivePacket)
    {
        rc = GetNextReceivePacket ();
		if (!rc)
		{
			if (RTECKC_Client == m_OpenMode)
			{
				return DataMissing;
			}

			//We still wait for the request from the client
			return DataOutstanding;
		}
    }


    remoteState = m_pPacketInfo[m_ActualReceivePacket].GetConnectionState (RTECKC_CommSegPacketInfo::Remote);
    if (RTECKC_CommSegPacketInfo::Pending != remoteState)
    {
        return DataOutstanding;
    }

    pReceivePacket = m_pFirstPacket + m_ActualReceivePacket * m_MPU;

    m_pPacketInfo[m_ActualReceivePacket].SetConnectionState (RTECKC_CommSegPacketInfo::Own, 
            RTECKC_CommSegPacketInfo::Received);

    m_pPacketInfo[m_ActualReceivePacket].SetConnectionState (RTECKC_CommSegPacketInfo::Remote, 
            RTECKC_CommSegPacketInfo::Send);

    m_ActualReceivePacket = RTECKC_UNDEF_COMMUNICATION_PACKET_IDX;

    return DataAvailable;
}

/*---------------------------------------------------------------------------*/

void			RTECKC_CommunicationSegment::SetNextSendPacket       (SAPDB_UInt4 const          packetIdx)
{
	SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::SetNextSendPacket", 
							 CKC_Trace, 9);

	m_pPacketInfo[packetIdx].SetSequenceNumber (INCREMENT_UINT4(m_LastSend));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTECKC_CommunicationSegment::CheckNextSendPacket (SAPDB_UInt4 const          packetIdx,
    									    				 SAPDBErr_MessageList &     messageList)
{
	SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::CheckNextSendPacket", 
		CKC_Trace, 9);

	if (m_pPacketInfo[packetIdx].GetSequenceNumber () != INCREMENT_UINT4(m_LastSend))
	{
		messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_WRONG_SERVER_SEND_PACKET,
			SAPDB_ToString (m_LastSend),
			SAPDB_ToString (m_pPacketInfo[packetIdx].GetSequenceNumber()));
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------*/


SAPDB_Bool  RTECKC_CommunicationSegment::GetNextReceivePacket ()
{
    SAPDB_UInt4 idx = 0;
    SAPDB_UInt4 oldNextReceive = m_NextReceive;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::GetNextReceivePacket", 
                              CKC_Trace, 9);

    INCREMENT_UINT4(m_NextReceive);
    //Optimization for one packet
    if (1 == m_NumPacket)
    {
        if (m_pPacketInfo[idx].GetSequenceNumber () != m_NextReceive)
        {
            m_NextReceive = oldNextReceive;
            return false;
        }
    }
    else
    {
        for (idx = 0; idx < m_NumPacket && m_pPacketInfo[idx].GetSequenceNumber () != m_NextReceive; ++idx)
        {;}

        if (idx == m_NumPacket)
        {
            m_NextReceive = oldNextReceive;
            return false;
        }
    }

    m_ActualReceivePacket = idx;
    return true;
}

/*---------------------------------------------------------------------------*/

inline  SAPDB_UInt4     RTECKC_CommunicationSegment::GetPacketIdx (SAPDB_Byte const * const   packet)
const
{ 
    SAPDB_Long packetDiff = (SAPDB_Long)(packet - m_pFirstPacket);


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::GetPacketIdx", 
                              CKC_Trace, 9);

    if (( packetDiff < 0 ) || ( 0 != (packetDiff % m_MPU )))
        return RTECKC_UNDEF_COMMUNICATION_PACKET_IDX;    

    return (SAPDB_UInt4)(packetDiff / m_MPU);
}

/*---------------------------------------------------------------------------*/

#if !defined (_WIN32) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) //&& defined (_REENTRANT)

RTESync_BinarySemaphore::EventUNIX * const   RTECKC_CommunicationSegment::GetUNIXEventBuffer ()
const
{
	if (NULL != m_pOptionClientEventUNIX)
	{
		return m_pOptionClientEventUNIX->GetEventBuffer ();
	}

	return NULL;
}
#endif

