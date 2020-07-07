/*!
  @file           RTEThread_ConsoleRequestor.cpp
  @author         StefanP
  @special area   Request Communication XCons - Console Thread
  @brief          Singleton on Kernel side
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_KGS.hpp"
#include    "RunTime/RTE_CompilerFeatures.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/Communication/RTEComm_PacketHeader.h"
#include    "RunTime/Threading/RTEThread_ConsoleRequestor.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"



extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

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
RTEThread_ConsoleRequestor   *RTEThread_ConsoleRequestor::m_Instance = NULL;


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/



RTEThread_ConsoleRequestor & RTEThread_ConsoleRequestor::Instance()
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    SAPDBErr_MessageList    messageList;

   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
    SAPDBTRACE_ROUTINE_DEBUG("RTEThread_ConsoleRequestor::Instance", Console_Trace, 9);

    if (NULL == m_Instance)
    {
#if defined (_WIN32) 
        RTE_SECURITY_ATTRIBUTES *  pWorldSA = &RTE_KGS::Instance().GetKGS()->WorldSA;
        m_Instance = new (RTEMem_Allocator::Instance()) RTEThread_ConsoleRequestor(RTE_KGS::Instance ().GetKGS()->szServerDB);
#else
        RTE_SECURITY_ATTRIBUTES *  pWorldSA = NULL;
        m_Instance = new (RTEMem_Allocator::Instance()) RTEThread_ConsoleRequestor(RTE_KGS::Instance ().GetKGS()->serverdb);
#endif
        SAPDBTRACE_WRITELN(Console_Trace, 1, "Creation of Console Request instance");


        if (NULL == m_Instance)
        {
            RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, "RTEThread_ConsoleRequestor"));
        }

        if (RTE_ConsoleRequestCommunication::NoError != 
                m_Instance->RTE_ConsoleRequestCommunication::Initialize (pWorldSA, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_INIT_REQUESTOR);
            RTE_Crash (messageList);
        }
    }
 
    return *m_Instance;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTEThread_ConsoleRequestor::ReadCommand 
(
    RTEThread_ConsoleCommand    &    command,
    SAPDBErr_MessageList        &    messageList
)
{
    RTEComm_ProtocolID          protocolID;
    SAPDB_UInt1                 protocolVersion;
    SAPDBErr_MessageList        tmpMessageList;
    SAPDB_UInt4                 expectedLength;
    SAPDB_UInt4                 numBytesTotal;
    SAPDB_UInt4                 currPacketDataLen;
    SAPDB_UInt1                 reqType;
    SAPDB_UInt4                 senderRef;
    SAPDB_UInt4                 receiverRef;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleRequestor::ReadRequest", 
                             Console_Trace, 1);

    if (!GetRequest (m_request, messageList))
    {
        return false;
    }
    
    if (!m_header.Get (&reqType, 
                       &senderRef, 
                       &receiverRef, 
                       &numBytesTotal, 
                       &currPacketDataLen, 
                       &protocolID, 
                       &protocolVersion, 
                       tmpMessageList))
    {
        messageList = messageList + tmpMessageList;
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_FIFO_PACKET_HEADER); 
        return (false);
    }

    SAPDBTRACE_WRITELN(Console_Trace, 7, "Type: "           << reqType       <<
                                         ", SenderRef: "    << senderRef     <<
                                         ", ReceiverRef: "  << receiverRef   <<
                                         ", Total: "        << numBytesTotal                <<
                                         ", Current: "      << currPacketDataLen            <<
                                         ", ID: "           << protocolID                   <<
                                         ", Version: "      << protocolVersion);

    if (RTE_CONSOLE_FIFO_PROTOCOL_VERSION < protocolVersion)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_FIFO_PROT_VERSION, 
                                                          SAPDB_ToString (RTE_CONSOLE_FIFO_PROTOCOL_VERSION), 
                                                          SAPDB_ToString (protocolVersion));
        return false;
    }

    if (Prot_ConsoleFIFO != protocolID)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_FIFO_PROT_ID, 
                                                          SAPDB_ToString (Prot_ConsoleFIFO), 
                                                          SAPDB_ToString (protocolID));
        return false;
    }

    expectedLength = (OPEN_REQ == reqType) ? sizeof (m_request.data.connectData)
                                                          : sizeof (m_request.data.hConnect);
    if (numBytesTotal != expectedLength || currPacketDataLen != expectedLength)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_FIFO_DATA_LEN, 
                                                          SAPDB_ToString (sizeof (m_request.data)), 
                                                          SAPDB_ToString (currPacketDataLen),
                                                          SAPDB_ToString (numBytesTotal));
        return false;
    }

    // fill command object
    command = RTEThread_ConsoleCommand( reqType, senderRef, receiverRef, m_request.data );

    return true;
}


