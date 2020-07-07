/*!
  @file           RTE_ConsoleDataCommunication.cpp
  @author         StefanP
  @brief          Common Communication Class
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

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined (SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined (_WIN32)                                        /*&if $OSSPEC = WIN32*/ 
#include  "gos41.h"                                         /* nocheck */
#else                                                       /*&else*/
#include  "gen41.h"                                         /* nocheck */
#include  "RunTime/RTE_UNIXAuthenticate.hpp"                /* nocheck */
#endif                                                      /*&endif*/

#include  "heo41.h"

#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Crash.hpp"
#include    "RunTime/RTE_Console_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/RTE_ConsoleDataCommunication.hpp"
#include    "RunTime/RTE_ConsoleStandardDataRequests.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"



#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)            /*&if ($OSSPEC = LINUX && $__HAS_NPTL != 1) || $MACH = SUN || $OSSPEC == SOLARIS */
#include    "RunTime/RTE_ConsoleSemaphoreTimeoutList.hpp"   /* nocheck */
#include    "RunTime/RTE_saveUNIXcalls.h"                   /* nocheck */
#endif                                                      /*&endif*/        




//Test Tracing
//#include    "RunTime/RTE_ConsoleTraceTopic.hpp"



extern SAPDBTrace_Topic Console_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/* external defined trace topic */
extern SAPDBTrace_Topic Console_Trace;

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/



RTE_ConsoleDataCommunication::RTE_ConsoleDataCommunication 
(
    SAPDB_UInt4 const           openMode,
    SAPDB_UInt4 const           myRef,
    RTE_OSPid   const           remotePID,
    SAPDB_UInt4 const           remoteRef
)  :    m_MyRef             (myRef),
        m_RemoteRef         (remoteRef), 
        m_OpenMode          (openMode),
        m_pShm              (NULL),
        m_pDataPart         (NULL),
        m_RemotePID         (remotePID),
        m_hShm              (RTE_UNDEF_HANDLE),
#if defined (_WIN32)
        m_hMySem            (RTE_UNDEF_HANDLE),
        m_hRemoteSem        (RTE_UNDEF_HANDLE),
#endif
#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
        m_ThreadIndex       (0),
#endif

        m_pMySem            (NULL),
        m_pRemoteSem        (NULL),
        m_BytesLeftPacket   (0),
        m_NumResidualPackets(RTE_CONSOLE_UNDEF_RES_PACKETS)
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::RTE_ConsoleDataCommunication", 
                             Console_Trace, 9);

    memset (m_ServerDB, 0, sizeof (m_ServerDB));
    strcpy ((char *)m_ShmID, RTE_CONSOLE_UNDEF_SHM_ID);
}

/*---------------------------------------------------------------------------*/

RTE_ConsoleDataCommunication::~RTE_ConsoleDataCommunication ()
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
//    SAPDB_Bool      removeResources = true;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::~RTE_ConsoleDataCommunication", 
                             Console_Trace, 9);

#if defined (_WIN32)
    RemoveSem (m_hMySem, m_pMySem);
    RemoveSem (m_hRemoteSem, m_pRemoteSem);
//    RemoveShm (removeResources);
#else
/*
    while (RTESys_TestAndLock(&m_pShm->lock))
    {
        ;
    }

    if (--m_pShm->openCount > 0)
    {
        removeResources = false;
    }

    RTESys_Unlock (&m_pShm->lock);

    if (removeResources)
*/
    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {
        RemoveSem (m_pMySem);
        RemoveSem (m_pRemoteSem);
//        RemoveShm (removeResources);
    }
#endif

    RemoveShm ();

    if ( m_RemotePID != RTE_UNDEF_OSPID )
       RTEProc_Close (m_hRemoteSide);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTE_ConsoleDataCommunication::Initialize 
(
    RTE_DBName const                szServerDB,
    RTE_SECURITY_ATTRIBUTES * const pSA,
    RTE_UniqueIdBuffer const        shmID,
    SAPDB_UInt4                     shmSize,
    SAPDB_UInt4                     semTimeout,
    SAPDBErr_MessageList &          messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_UInt4         dataOffset;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::Initialize", 
                             Console_Trace, 1);

    RTEProc_Open (m_RemotePID, &m_hRemoteSide);

    /*--- Open shared memory ------------------------------------------------*/
    strcpy (m_ServerDB, szServerDB);
    strcpy ((char *)m_ShmID, (char *)shmID);
    m_ShmSize       = shmSize;
    m_SemTimeout    = semTimeout;

    if (!CreateShm (pSA, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SHM);
        return false;
    }
    
    m_HeaderOffset      = SAPDB_ALIGN (2 * sizeof (RTE_ConsoleSemaphore), SAPDB_ALIGNMENT);
    dataOffset          = m_HeaderOffset + 
                          SAPDB_ALIGN (RTEComm_PacketHeader::PacketHeaderBufferSize(), SAPDB_ALIGNMENT);

    m_pDataPart         = m_pShm + dataOffset; 
    m_MaxPacketLen      = m_ShmSize - dataOffset + RTEComm_PacketHeader::PacketHeaderBufferSize();

    m_header.SetPacketHeaderBuffer (m_pShm + m_HeaderOffset);

    if (RTE_CONSOLE_CLIENT_MODE == m_OpenMode)  
    {
        //Client: Creator of shared memory and semaphores
        m_pMySem = (RTE_ConsoleSemaphore *) m_pShm;
        m_pRemoteSem = (RTE_ConsoleSemaphore *) (m_pShm + sizeof (RTE_ConsoleSemaphore));
#if defined (_WIN32)
        if (!CreateSem (RTE_CONSOLE_CLIENT_SEM_NAME_SKELETON, m_hMySem, 
                        m_pMySem, pSA, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "my");
            return false;
        }

        if (!CreateSem (RTE_CONSOLE_SERVER_SEM_NAME_SKELETON, m_hRemoteSem, 
                        m_pRemoteSem, pSA, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "remote");
            return false;
        }
#else
        if (!CreateSem (m_pMySem, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "my");
            return false;
        }

        if (!CreateSem (m_pRemoteSem, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "remote");
            return false;
        }
#endif
        //Initialize the packet header
        m_header.Set (UNDEF_TYPE, m_MyRef, m_RemoteRef, 0, Prot_ConsoleSM, 
                      RTE_CONSOLE_SM_PROTOCOL_VERSION, m_MaxPacketLen);

    }
    else    //Server mode
    {
        m_pMySem = (RTE_ConsoleSemaphore *) (m_pShm + sizeof (RTE_ConsoleSemaphore));
        m_pRemoteSem = (RTE_ConsoleSemaphore *) m_pShm;

#if defined (_WIN32)
        if (!CreateSem (RTE_CONSOLE_SERVER_SEM_NAME_SKELETON, m_hMySem, 
                        m_pMySem, pSA, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "my");
            return false;
        }

        if (!CreateSem (RTE_CONSOLE_CLIENT_SEM_NAME_SKELETON, m_hRemoteSem, 
                        m_pRemoteSem, pSA, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "remote");
            return false;
        }
#else
        if (!CreateSem (m_pMySem, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "my");
            return false;
        }

        if (!CreateSem (m_pRemoteSem, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_SEM, "remote");
            return false;
        }
#endif
    }

    return true;
}

/*---------------------------------------------------------------------------*/

void  RTE_ConsoleDataCommunication::ResetPacketData ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::ResetPacketData", 
                             Console_Trace, 1);
    m_BytesLeftPacket = 0;
    m_NumResidualPackets = RTE_CONSOLE_UNDEF_RES_PACKETS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTE_ConsoleDataCommunication::CheckHeader
(
    SAPDB_UInt1                 const   expectedType,
    SAPDB_UInt1             &           reqRespType,
    SAPDB_UInt4             &           numBytesTotal,
    SAPDB_UInt4             &           currPacketDataLen,
    SAPDB_UInt4             &           senderRef,
    SAPDBErr_MessageList    &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTEComm_ProtocolID      protocolID;
    SAPDB_UInt1             protocolVersion;
    SAPDB_UInt4             receiverRef;
    SAPDBErr_MessageList    tmpMessageList;
        

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::CheckHeader", 
                             Console_Trace, 1);
    
    if (!m_header.Get (&reqRespType, &senderRef, &receiverRef, &numBytesTotal, 
                       &currPacketDataLen, &protocolID, &protocolVersion, tmpMessageList))
    {
        messageList = messageList + tmpMessageList;
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_GET_PACKET_HEADER); 
        return false;
    }

    SAPDBTRACE_WRITELN(Console_Trace, 7, "Type: "           << reqRespType        <<
                                         ", SenderRef: "    << senderRef          <<
                                         ", ReceiverRef: "  << receiverRef        <<
                                         ", Total: "        << numBytesTotal      <<
                                         ", Current: "      << currPacketDataLen  <<
                                         ", ID: "           << protocolID         <<
                                         ", Version: "      << protocolVersion);

    if (RTE_CONSOLE_SM_PROTOCOL_VERSION < protocolVersion)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_SM_PROTOCOL_VERSION, 
                                                          SAPDB_ToString (RTE_CONSOLE_SM_PROTOCOL_VERSION), 
                                                          SAPDB_ToString (protocolVersion));
        return false;
    }

    if (Prot_ConsoleSM != protocolID)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_SM_PROTOCOL_ID, 
                                                          SAPDB_ToString (Prot_ConsoleSM), 
                                                          SAPDB_ToString (protocolID));
        return false;
    }

    if (0 == (reqRespType & expectedType))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_RESP_TYPE, 
                                                          SAPDB_ToString (expectedType),
                                                          SAPDB_ToString (reqRespType));
        return false;
    }

    if (OPEN_REQ != expectedType && receiverRef != m_MyRef)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_RECEIVER_REF, 
                                                          SAPDB_ToString (m_MyRef),
                                                          SAPDB_ToString (receiverRef));
        return false;
    }

    if (OPEN_REP != expectedType && senderRef != m_RemoteRef)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_SENDER_REF, 
                                                          SAPDB_ToString (m_RemoteRef),
                                                          SAPDB_ToString (senderRef));
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTE_ConsoleDataCommunication::TimeoutOccurred 
(
    SAPDBErr_MessageList    &           messageList
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    SAPDB_UInt1             reqRespType;

   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/

    if (!m_header.Get (&reqRespType, NULL, NULL, NULL, NULL, NULL, NULL, messageList))
    {
        return RTE_CONS_ERROR;
    }

    if (TIMEOUT == reqRespType)
    {
        return RTE_CONS_REMOTE_TIMEOUT;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::GetData 
(
    SAPDB_Byte              *   const   pBuffer,
    SAPDB_UInt1                 const   expectedType,
    SAPDB_UInt4                 const   bufferSize,
    SAPDB_UInt4             &           numBytesRead,
    SAPDB_UInt4             &           numBytesTotal,
    SAPDBErr_MessageList    &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::GetData", 
                             Console_Trace, 1);

    ResetPacketData ();

    return (GetDataContinue (pBuffer, expectedType, bufferSize, numBytesRead, 
                             numBytesTotal, messageList));
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::GetDataNext 
(
    SAPDB_Byte              *   const   pBuffer,
    SAPDB_UInt1                 const   expectedType,
    SAPDB_UInt4                 const   bufferSize,
    SAPDB_UInt4             &           numBytesRead,
    SAPDB_UInt4             &           numBytesTotal,
    SAPDBErr_MessageList    &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_UInt4             rc = RTE_CONS_NO_ERROR;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::GetDataNext", 
                             Console_Trace, 1);

    //Trigger send on the remote side
#if defined (_WIN32)
    SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore: " << SAPDB_ToString (m_pRemoteSem->semID));
    if (RTE_CONS_NO_ERROR != (rc = PostSem (m_hRemoteSem, m_pRemoteSem, messageList)))
#else
    SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore");
    if (RTE_CONS_NO_ERROR != (rc = PostSem (m_pRemoteSem, messageList)))
#endif
    {
        return rc;
    }

    return (GetData (pBuffer, expectedType, bufferSize, numBytesRead, numBytesTotal, 
                     messageList));
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::GetDataContinue
(
    SAPDB_Byte              *   const   pData,
    SAPDB_UInt1                 const   expectedType,
    SAPDB_UInt4                 const   bufferSize,
    SAPDB_UInt4             &           numBytesRead,
    SAPDB_UInt4             &           numBytesTotal,
    SAPDBErr_MessageList    &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_UInt4             bytesLeftBuffer;
    SAPDB_UInt4             bytesToRead;
    SAPDB_UInt4             currPacketDataLen;
    SAPDB_UInt4             senderRef;
    SAPDB_UInt1             reqRespType;
    SAPDB_Byte          *   pDataBuffer = pData;
    SAPDB_UInt4             rc = RTE_CONS_NO_ERROR;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::GetDataContinue", 
                             Console_Trace, 1);
    numBytesRead = 0;
    bytesLeftBuffer = bufferSize;

    if (0 == m_BytesLeftPacket && 0 != m_NumResidualPackets)
    {

#if defined (_WIN32)
        SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on my semaphore: " << SAPDB_ToString (m_pMySem->semID));
        if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_hMySem, m_pMySem, messageList)))
#else
        SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on own semaphore");
        if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_pMySem, messageList)))
#endif
        {
            return rc;
        }
    }

    if (!CheckHeader (expectedType, reqRespType, numBytesTotal, currPacketDataLen, senderRef, 
                      messageList))
    {
        return RTE_CONS_ERROR;
    }

    if (0 == m_BytesLeftPacket && 0 != m_NumResidualPackets)
    {
        m_BytesLeftPacket = currPacketDataLen;
        m_NumResidualPackets = m_header.ResidualPacketSegs ();
    }

    SAPDBTRACE_WRITELN (Console_Trace, 7, "Bytes left buffer: "     << bytesLeftBuffer      <<
                                          ", Bytes left packet: "   << m_BytesLeftPacket    <<
                                          ", Residual packets: "    << m_NumResidualPackets);
        
    if (OPEN_REP == reqRespType)
    {
        m_RemoteRef = senderRef;
    }
    
    while (bytesLeftBuffer > 0 && m_BytesLeftPacket > 0)
    {
        bytesToRead = min (bytesLeftBuffer, m_BytesLeftPacket);
        SAPDB_MemCopyNoCheck (pDataBuffer + numBytesRead, m_pDataPart + currPacketDataLen - m_BytesLeftPacket, 
                bytesToRead);
        numBytesRead += bytesToRead;
        bytesLeftBuffer -= bytesToRead;
        m_BytesLeftPacket -= bytesToRead;

        SAPDBTRACE_WRITELN (Console_Trace, 7, "Bytes read: "            << numBytesRead     <<
                                              ", Bytes left buffer: "   << bytesLeftBuffer  <<
                                              ", Bytes left packet: "   << m_BytesLeftPacket);

        /* remember following conclusions are valid at this point */
        /* => bytesLeftBuffer == 0 || bytesLeftPacket == 0 */
        /* bytesLeftBuffer > 0 => bytesLeftPacket == 0 */
        if (0 == m_BytesLeftPacket && 0 != m_NumResidualPackets)
        {


            /* Reading next package from client even if the buffer is already filled up completely. */
            /* This is done to guarantee a new package for the next call of this function */
#if defined (_WIN32)
            SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore: " << SAPDB_ToString (m_pRemoteSem->semID));
            if (RTE_CONS_NO_ERROR != (rc = PostSem (m_hRemoteSem, m_pRemoteSem, messageList)))
#else
            SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore");
            if (RTE_CONS_NO_ERROR != (rc = PostSem (m_pRemoteSem, messageList)))
#endif
            {
                return rc;
            }

            if (0 != bytesLeftBuffer)
            {
#if defined (_WIN32)
                SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on my semaphore: " << SAPDB_ToString (m_pMySem->semID));
                if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_hMySem, m_pMySem, messageList)))
#else
                SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on own semaphore");
                if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_pMySem, messageList)))
#endif
                {
                    return rc;
                }

                if (!CheckHeader (expectedType, reqRespType, numBytesTotal, currPacketDataLen, senderRef, 
                                  messageList))
                {
                    return RTE_CONS_ERROR;
                }

                m_BytesLeftPacket = currPacketDataLen;
                m_NumResidualPackets = m_header.ResidualPacketSegs ();

                SAPDBTRACE_WRITELN (Console_Trace, 7, "Bytes left packet: "     << m_BytesLeftPacket    <<
                                                      ", Residual packets: "    << m_NumResidualPackets);
            }
        }
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTE_ConsoleDataCommunication::SendData
(
    SAPDB_Byte              const   *   const   pData,
    SAPDB_UInt4             const               size,
    SAPDB_UInt1             const               respType,
    SAPDBErr_MessageList            &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_UInt4             bytesToWrite;
    SAPDB_UInt4             offset = 0;
    SAPDB_UInt4             headerLen;
    SAPDB_UInt4             currPacketLen;
    SAPDB_UInt4             numResPacket;
    SAPDB_UInt4             rc = RTE_CONS_NO_ERROR;
    
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::SendData", 
                             Console_Trace, 1);

    rc = TimeoutOccurred (messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        return rc;
    }

    //Initialize shared memory
    memset ((SAPDB_Byte *)m_pShm + m_HeaderOffset, 0, m_ShmSize - m_HeaderOffset); 
                                                                       
    headerLen = m_header.PacketHeaderBufferSize ();
    currPacketLen = m_header.Set (respType, m_MyRef, m_RemoteRef, size, Prot_ConsoleSM, 
                                  RTE_CONSOLE_SM_PROTOCOL_VERSION, m_MaxPacketLen);

    SAPDBTRACE_WRITELN(Console_Trace, 7, "Type: "           << respType                         <<
                                         ", SenderRef: "    << m_MyRef                          <<
                                         ", ReceiverRef: "  << m_RemoteRef                      <<
                                         ", Data size: "    << size                             <<
                                         ", Total: "        << m_MaxPacketLen                   <<
                                         ", ID: "           << Prot_ConsoleSM                   <<
                                         ", Version: "      << RTE_CONSOLE_SM_PROTOCOL_VERSION);

    do
    {

        bytesToWrite = currPacketLen - headerLen;

        SAPDBTRACE_WRITELN (Console_Trace, 7, "Current packet length: " << currPacketLen    <<
                                              ", Bytes to write: "      << bytesToWrite     <<
                                              ", Offset: "              << m_BytesLeftPacket);

        if (0 != size)
        {
            SAPDB_MemCopyNoCheck (m_pDataPart, pData + offset, bytesToWrite);
        }

        offset += bytesToWrite;

        /*--- Signal Remote about the reply --------------------------------------*/
     
        //Has to be done before posting the remote side!!!
        numResPacket = m_header.ResidualPacketSegs ();
#if defined (_WIN32)
        SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore: " << SAPDB_ToString (m_pRemoteSem->semID));
        if (RTE_CONS_NO_ERROR != (rc = PostSem (m_hRemoteSem, m_pRemoteSem, messageList)))
#else
        SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore");
        if (RTE_CONS_NO_ERROR != (rc = PostSem (m_pRemoteSem, messageList)))
#endif
        {
            return rc;
        }

  //      if (0 != m_header.ResidualPacketSegs ())
        if (0 != numResPacket)
        {
#if defined (_WIN32)
            SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on my semaphore: " << SAPDB_ToString (m_pMySem->semID));
            if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_hMySem, m_pMySem, messageList)))
#else
            SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on own semaphore");
            if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_pMySem, messageList)))
#endif
            {
                return rc;
            }

            currPacketLen = m_header.NextSendPacketSeg ();
        }
        else
        {
            currPacketLen = 0;
            SAPDBTRACE_WRITELN (Console_Trace, 7, "Current packet length: " << currPacketLen);
        }
    }
    while (0 != currPacketLen);

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTE_ConsoleDataCommunication::SendDataNext
(
    SAPDB_Byte              const   *   const   pData,
    SAPDB_UInt4             const               size,
    SAPDB_UInt1             const               respType,
    SAPDBErr_MessageList            &           messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_UInt4             rc = RTE_CONS_NO_ERROR;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::SendDataNext", 
                             Console_Trace, 1);

#if defined (_WIN32)
    SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on my semaphore: " << SAPDB_ToString (m_pMySem->semID));
    if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_hMySem, m_pMySem, messageList)))
#else
    SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on own semaphore");
    if (RTE_CONS_NO_ERROR != (rc = WaitSem (m_pMySem, messageList)))
#endif
    {
        return rc;
    }

    return (SendData (pData, size, respType, messageList));
}

/*---------------------------------------------------------------------------*/
#if defined (_WIN32)  //NT Semaphores: Events

SAPDB_Bool  RTE_ConsoleDataCommunication::CreateSem 
(
    RTE_Filename const                  nameSkeleton,
    RTE_SemaphoreHandle &                hSem,
    RTE_ConsoleSemaphore *              pSem, 
    RTE_SECURITY_ATTRIBUTES * const     pSA,
    SAPDBErr_MessageList &              messageList
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::CreateSem", 
                             Console_Trace, 9);
    hSem = RTE_UNDEF_HANDLE;
    
    RTE_SystemRc        rc;
    
    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {
        rc = sql41c_open_event_sem (&hSem, 
                                    (char *)nameSkeleton, 
                                    (char *)pSem->semID, 
                                    RTE_SYSTEMRC_NO_ERROR);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            hSem = RTE_UNDEF_HANDLE;
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ATTACH_SEM, 
                                                 SAPDB_ToString (pSem->semID), SAPDB_ToString (rc));   
            return false;
        }

    }
    else //RTE_CONSOLE_CLIENT_MODE
    {

        if (!RTE_GetUID (pSem->semID))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_GUID, "semaphore");
            return false;
        }

        rc = sql41c_create_event_sem (&hSem, 
                                      (char *)nameSkeleton, 
                                      (char *)pSem->semID, 
                                      0,            //dummy OS/2 
                                      0,            //dummy OS/2
                                      pSA);

        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            hSem = RTE_UNDEF_HANDLE;
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_SEM,  
                                                 SAPDB_ToString (pSem->semID), SAPDB_ToString (rc));
            return false;
        }
    }

    return true;
} 
 
/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::PostSem 
(
    RTE_SemaphoreHandle const            hSem,
    RTE_ConsoleSemaphore const * const  pSem,
    SAPDBErr_MessageList &              messageList
) 
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::PostSem", 
                             Console_Trace, 9);
    RTE_SystemRc        rc;
    rc = sql41c_post_event_sem (hSem, (char *)pSem->semID);
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_POST_SEM, 
                                                          SAPDB_ToString (pSem->semID), SAPDB_ToString (rc));
        return RTE_CONS_ERROR;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::WaitSem 
(
    RTE_SemaphoreHandle const            hSem,
    RTE_ConsoleSemaphore const * const  pSem,
    SAPDBErr_MessageList &              messageList
) 
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc    rc;
    SAPDBErr_MessageList    tmpMessage;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::WaitSem", 
                             Console_Trace, 9);

    if (RTE_SYSTEMRC_NO_ERROR 
        != (rc = sql41c_wait_event_sem (hSem, m_SemTimeout, (char *)pSem->semID)))
    {
        if (ERROR_TIMEOUT == rc && RTE_CONSOLE_SERVER_MODE == m_OpenMode)
        {
            //Is client still busy or already died?
            RTEProc_State state = RTEProc_GetState (m_hRemoteSide);

            switch (state)
            {
            case Alive:
                rc = sql41c_wait_event_sem (hSem, 10 * m_SemTimeout, (char *)pSem->semID);//Waiting 10 times longer than before
                break;

            case Died:
                tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_CLIENT_DEAD, 
                                                   SAPDB_ToString (m_hRemoteSide.m_Pid));
                break;

            default:
                tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CLIENT_STATE, 
                                                   SAPDB_ToString (m_hRemoteSide.m_Pid));
            }
        }

        if (ERROR_TIMEOUT == rc)
        {
            tmpMessage = tmpMessage + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_SEM_TIMEOUT, 
                                                              SAPDB_ToString (pSem->semID));
            RTE_Message (tmpMessage);
            messageList = messageList + tmpMessage;
            return RTE_CONS_TIMEOUT;
        }
        else if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_WAIT_SEM, 
                                                              SAPDB_ToString (pSem->semID), SAPDB_ToString (rc));
            return RTE_CONS_ERROR;
        }
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTE_ConsoleDataCommunication::RemoveSem 
(
    RTE_SemaphoreHandle                      hSem,
    RTE_ConsoleSemaphore const * const      pSem
)     
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::RemoveSem", 
                             Console_Trace, 9);
    if (RTE_UNDEF_HANDLE != hSem && NULL != pSem )
    {
        sql41c_close_event_sem (hSem, (char *)(pSem->semID));
    }
}

#else //UNIX: semaphores

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTE_ConsoleDataCommunication::CreateSem 
(
    RTE_ConsoleSemaphore *      pSem,
    SAPDBErr_MessageList &      messageList
) 
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::CreateSem", 
                             Console_Trace, 9);

    if (RTE_CONSOLE_CLIENT_MODE == m_OpenMode)
    {
        pSem->state = RTE_CONSOLE_SEM_RUNNING;
#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
        char      ipcPraefix[] = "us";

        pSem->handle = sql41_create_sem (0, 
                                       0, 
                                       0,       //not used anymore
                                       0666,    //?
                                       getuid (), 
                                       ipcPraefix, 
                                       m_ServerDB);

        if (RTE_UNDEF_HANDLE == pSem->handle)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_SEM_UNIX);
            return false;	
        }

        RTESys_InitLock(&pSem->lock);

#else
        RTE_SystemRc            rc;

        //the attribute objects have not to be stored in shared memory. They were only used
        //to initialize the mutex and condition respectively. 
        pthread_mutexattr_t     mutexAttribute;
        pthread_condattr_t      conditionAttribute;

        /* MUTEX */

        /*
        Setting scope of mutex to system (interprocess) 
        */

        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_init (&mutexAttribute)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_ATTR_INIT, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            return false;
        }

        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_setpshared (&mutexAttribute,
            PTHREAD_PROCESS_SHARED)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_ATTR_SET, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            return false;
        }


        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_init (&pSem->mutex, 
            &mutexAttribute)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_INIT, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            //not necessary:
            //        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_destroy (&mutexAttribute)))
            //        {
            //            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_ATTR_DESTROY, 
            //                                                              SAPDB_ToString (rc), ConsoleError (rc));
            //        }

            return false;
        }

        //not necessary:
        //    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_destroy (&mutexAttribute)))
        //    {
        //        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_ATTR_DESTROY, 
        //                                                          SAPDB_ToString (rc), ConsoleError (rc));
        //    }


        /* CONDITION */

        /*
        Setting scope of mutex to system (interprocess) 
        */
        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_init (&conditionAttribute)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_ATTR_INIT, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            return false;
        }


        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_setpshared (&conditionAttribute, 
            PTHREAD_PROCESS_SHARED)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_ATTR_SET, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            return false;
        }


        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_cond_init (&pSem->condition, 
            &conditionAttribute)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_INIT, 
                SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            //not necessary:
            //        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_destroy (&conditionAttribute)))
            //        {
            //            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_ATTR_DESTROY, 
            //                                                              SAPDB_ToString (rc), ConsoleError (rc));
            //        }
            return false;
        }

        //not necessary:
        //    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_destroy (&conditionAttribute)))
        //    {
        //        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_ATTR_DESTROY, 
        //                                                          SAPDB_ToString (rc), ConsoleError (rc));
        //    }
#endif // HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
    }

    return true;
}

/*---------------------------------------------------------------------------*/

void    RTE_ConsoleDataCommunication::RemoveSem 
(
    RTE_ConsoleSemaphore * const    pSem
) 
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::RemoveSem", 
                             Console_Trace, 9);

    if (NULL != pSem)
    {
#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
        char      ipcPraefix[] = "us";
        sql41_remove_sem (&pSem->handle, ipcPraefix, m_ServerDB);
#else
        pthread_mutex_destroy (&pSem->mutex);
        pthread_cond_destroy  (&pSem->condition);
#endif
    }
}
   
/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::PostSem 
(
    RTE_ConsoleSemaphore * const    pSem,         
    SAPDBErr_MessageList &          messageList
) 
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc    rc;    
    SAPDB_UInt4     retCode = RTE_CONS_NO_ERROR;



/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::PostSem", 
                             Console_Trace, 9);

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
    SAPDB_Bool      post = false;

    while (RTESys_TestAndLock(&pSem->lock))
    {
        ;
    }

    if (RTE_CONSOLE_SEM_TIMEOUT != pSem->state)
    {
        if (RTE_CONSOLE_SEM_WAIT == pSem->state)
        {
            post = true;
        }

        pSem->state = RTE_CONSOLE_SEM_POST;
    }
    else
    {
        retCode = RTE_CONS_TIMEOUT;
    }

 
    RTESys_Unlock (&pSem->lock);

    if (post)
    {
        union semun   arg ;
	    arg.val = 1 ;
	    rc = RTE_save_semctl (pSem->handle, 0 , SETVAL , SEMCTL_SETVAL(arg));
        if ( rc == -1)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_POST_SEM_UNIX, 
                                                              SAPDB_ToString (pSem->handle), RTE_ConsoleUNIXError (errno));
            retCode = RTE_CONS_ERROR;
        }
    }

#else

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_lock (&pSem->mutex)))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_LOCK, 
                                                          SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
        return RTE_CONS_ERROR;
    }

    if (RTE_CONSOLE_SEM_TIMEOUT != pSem->state)
    {
        if (RTE_CONSOLE_SEM_WAIT == pSem->state 
            && RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_cond_signal (&pSem->condition)))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_SIGNAL, 
                                                              SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
            retCode = RTE_CONS_ERROR;

        }

        pSem->state = RTE_CONSOLE_SEM_POST;
    }
    else
    {
        retCode = RTE_CONS_TIMEOUT;
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_unlock (&pSem->mutex)))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_UNLOCK, 
                                                          SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
        retCode = RTE_CONS_ERROR;
    }

#endif

    return retCode;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTE_ConsoleDataCommunication::WaitSem 
(
    RTE_ConsoleSemaphore * const    pSem,       
    SAPDBErr_MessageList &          messageList
) 
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc;    
    SAPDB_UInt4             retCode = RTE_CONS_NO_ERROR;
    SAPDBErr_MessageList    tmpMessage;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::WaitSem", 
                             Console_Trace, 9);

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
    SAPDB_Bool              wait = false;
    SAPDB_Bool              timedOut = false;

    while (RTESys_TestAndLock(&pSem->lock))
    {
        ;
    }
    if (RTE_CONSOLE_SEM_POST != pSem->state)
    {
        wait = true;
        pSem->state = RTE_CONSOLE_SEM_WAIT;
    }

    RTESys_Unlock (&pSem->lock);


    if (wait)
    {
        struct sembuf       semWait;

        semWait.sem_num = 0;
        semWait.sem_op  = -1;
        semWait.sem_flg = SEM_UNDO;

        // sm_SemTimeout is given in msec. Therefore
        // we have to transform it to seconds first
        
        if (!RTE_ConsoleSemaphoreTimeoutList::GetInstance ().EnqueueSemaphore (pSem, 
                                                                               m_SemTimeout/1000, 
                                                                               m_ThreadIndex,
                                                                               RTE_CONSOLE_CLIENT_MODE == m_OpenMode ? true : false,
                                                                               messageList))
        {//Fatale Error   
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ENQUEUE_SEMAPHORE);
            RTE_Crash (messageList);
        }

        rc = RTE_save_semop (pSem->handle, &semWait, 1); 
        if ( rc == -1 )
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_WAIT_SEM_UNIX, 
                                                            SAPDB_ToString (pSem->handle), 
                                                            RTE_ConsoleUNIXError (errno));
            retCode = RTE_CONS_ERROR;
        }

        if (!RTE_ConsoleSemaphoreTimeoutList::GetInstance ().DequeueSemaphore (m_ThreadIndex, 
                                                                               timedOut,
                                                                               messageList))
        {//Fatale Error
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_DEQUEUE_SEMAPHORE);
            RTE_Crash (messageList);
        }

        if (RTE_CONSOLE_SERVER_MODE == m_OpenMode && timedOut)
        {
            //Is client still busy or has he already died?
            RTEProc_State state = RTEProc_GetState (m_hRemoteSide);

            switch (state)
            {
            case Alive:
                wait = false;
                while (RTESys_TestAndLock(&pSem->lock))
                {
                    ;
                }
                if (RTE_CONSOLE_SEM_POST != pSem->state)
                {
                    wait = true;
                    pSem->state = RTE_CONSOLE_SEM_WAIT;
                }

                RTESys_Unlock (&pSem->lock);
                if (wait)
                {
                    //Waiting 10 times longer than before
                    if (!RTE_ConsoleSemaphoreTimeoutList::GetInstance ().EnqueueSemaphore (pSem, 
                                                                                           m_SemTimeout/100, 
                                                                                           m_ThreadIndex, 
                                                                                           true, 
                                                                                           messageList))
                    {//Fatale Error   
                        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ENQUEUE_SEMAPHORE);
                        RTE_Crash (messageList);
                    }

                    rc = RTE_save_semop (pSem->handle, &semWait, 1); 
                    if ( rc == -1 )
                    {
                        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_WAIT_SEM_UNIX, 
                            SAPDB_ToString (pSem->handle), 
                            RTE_ConsoleUNIXError (errno));
                        retCode = RTE_CONS_ERROR;
                    }

                    if (!RTE_ConsoleSemaphoreTimeoutList::GetInstance ().DequeueSemaphore (m_ThreadIndex, 
                                                                                           timedOut,
                                                                                           messageList))
                    {//Fatale Error
                        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_DEQUEUE_SEMAPHORE);
                        RTE_Crash (messageList);
                    }
                }
                break;

            case Died:
                if (RTE_CONSOLE_SEM_POST != pSem->state)
                {
                    pSem->state = RTE_CONSOLE_SEM_TIMEOUT; 
                    tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_CLIENT_DEAD, 
                                                                SAPDB_ToString (m_hRemoteSide.m_Pid));
                }
                
                break;

            default:
                pSem->state = RTE_CONSOLE_SEM_TIMEOUT; 
                tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CLIENT_STATE, 
                                                                SAPDB_ToString (m_hRemoteSide.m_Pid));
            }
        }
                
        if (RTE_CONSOLE_SEM_TIMEOUT == pSem->state)
        {
            tmpMessage = tmpMessage + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_SEM_TIMEOUT_UNIX);
            RTE_Message (tmpMessage);
            messageList = messageList + tmpMessage;
            retCode = RTE_CONS_TIMEOUT;
        }
    }

#else

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_lock (&pSem->mutex)))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_LOCK, 
                                                          SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
        return RTE_CONS_ERROR;
    }

    SAPDBTRACE_WRITELN (Console_Trace, 5, "State of my semaphore: " << SAPDB_ToString (pSem->state));

    if (RTE_CONSOLE_SEM_POST != pSem->state)
    {
        struct timespec timeout;

        pSem->state = RTE_CONSOLE_SEM_WAIT;

        timeout.tv_sec = time (NULL) + m_SemTimeout/1000; // m_SemTimeout is given in msec. Therefore
                                                          // we have to transform it to seconds first
        timeout.tv_nsec = 0;

        if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_cond_timedwait (&pSem->condition, 
                                                                    &pSem->mutex, 
                                                                    &timeout)))
        {
            SAPDBTRACE_WRITELN (Console_Trace, 5, "pthread_cond_timedwait: " << SAPDB_ToString (rc));

            //The semaphore may be posted even if a timeout has occurred
            if (ETIMEDOUT == rc && RTE_CONSOLE_SERVER_MODE == m_OpenMode 
                && RTE_CONSOLE_SEM_POST != pSem->state)
            {
                //Is client still busy or has he already died?
                RTEProc_State state = RTEProc_GetState (m_hRemoteSide);

                switch (state)
                {
                case Alive:
                    if (RTE_CONSOLE_SEM_POST != pSem->state)
                    {
                        pSem->state = RTE_CONSOLE_SEM_WAIT;
                        timeout.tv_sec = time (NULL) + (m_SemTimeout/100); //Waiting 10 times longer than before
                        rc =  pthread_cond_timedwait (&pSem->condition, &pSem->mutex, &timeout);
                        SAPDBTRACE_WRITELN (Console_Trace, 5, "pthread_cond_timedwait: " << SAPDB_ToString (rc));
                    }
    
                    break;

                case Died:
                    tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_CLIENT_DEAD, 
                                                                      SAPDB_ToString (m_hRemoteSide.m_Pid));
                    break;

                default:
                    tmpMessage = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CLIENT_STATE, 
                                                                      SAPDB_ToString (m_hRemoteSide.m_Pid));
                }
            }
            
            if (ETIMEDOUT == rc)
            {
                if (RTE_CONSOLE_SEM_POST != pSem->state)
                {
                    pSem->state = RTE_CONSOLE_SEM_TIMEOUT;
                    tmpMessage = tmpMessage + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CONSOLE_SEM_TIMEOUT_UNIX);
                    RTE_Message (tmpMessage);
                    messageList = messageList + tmpMessage;
                    retCode = RTE_CONS_TIMEOUT;
                }
            }
            else if (RTE_SYSTEMRC_NO_ERROR != rc)
            {
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PCONDITION_WAIT_SEM, 
                                                                  SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
                retCode = RTE_CONS_ERROR;
            }
        }
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_unlock (&pSem->mutex)))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_PMUTEX_UNLOCK, 
                                                          SAPDB_ToString (rc), RTE_ConsoleUNIXError (rc));
        retCode = RTE_CONS_ERROR;
    }

#endif

    if (RTE_CONSOLE_SEM_TIMEOUT != pSem->state)
    {
        pSem->state = RTE_CONSOLE_SEM_RUNNING;
    }

    return retCode;
}

#endif //UNIX

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTE_ConsoleDataCommunication::WaitForConfirmation      
(
    SAPDBErr_MessageList &          messageList
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::ReceiveReleaseConfirmation", 
                             Console_Trace, 1);
#if defined (_WIN32)
        SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on my semaphore: " << SAPDB_ToString (m_pMySem->semID));
        return (WaitSem (m_hMySem, m_pMySem, messageList));
#else
        SAPDBTRACE_WRITELN (Console_Trace, 5, "Wait on own semaphore");
        return (WaitSem (m_pMySem, messageList));
#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTE_ConsoleDataCommunication::SignalConfirmation      
(
    SAPDBErr_MessageList &          messageList
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::SendReleaseConfirmation", 
                             Console_Trace, 1);

#if defined (_WIN32)
    SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore: " << SAPDB_ToString (m_pRemoteSem->semID));
    return (PostSem (m_hRemoteSem, m_pRemoteSem, messageList));
#else
    SAPDBTRACE_WRITELN(Console_Trace, 5, "Posting remote semaphore");
    return (PostSem (m_pRemoteSem, messageList));
#endif
}

/*---------------------------------------------------------------------------*/

void    RTE_ConsoleDataCommunication::GetShmPath 
(
    RTE_Path                     path
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::GetShmPath", 
                             Console_Trace, 9);
 
#if !(defined (_WIN32))
    SAPDB_UInt4         lenPath;

    sql41_get_ipc_dir (*(tsp00_Pathc *)path);
    lenPath = strlen (path);
    SAPDB_sprintf (path + lenPath, sizeof(RTE_Path) - lenPath, "us:%s/" , m_ServerDB);
#endif

    strcpy (path + strlen (path), RTE_CONSOLE_SHM_NAME_SKELETON);
    strcpy (path + strlen (path), (char *)m_ShmID);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTE_ConsoleDataCommunication::CreateShm 
(
    RTE_SECURITY_ATTRIBUTES * const     pSA,            //NT only
    SAPDBErr_MessageList &              messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::CreateShm", 
                             Console_Trace, 9);

#if defined (_WIN32)
    RTE_SystemRc        rc;

    SAPDBTRACE_WRITELN (Console_Trace, 1, "Name of shared memory: " << RTE_CONSOLE_SHM_NAME_SKELETON
                                                                    << SAPDB_ToString (m_ShmID));

    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {
        rc = sql41c_get_named_shrd_mem ((PVOID *)(&m_pShm),
                                        RTE_CONSOLE_SHM_NAME_SKELETON, 
                                        (char *)m_ShmID,
                                        &m_hShm);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ATTACH_SHM, 
                                                              SAPDB_ToString (m_ShmID), SAPDB_ToString (rc));       
            return (false);
        }
    }
    else //RTE_CONSOLE_CLIENT_MODE
    {
        rc = sql41c_create_shrd_mem ((PVOID *)(&m_pShm),
                                     m_ShmSize, 
                                     RTE_CONSOLE_SHM_NAME_SKELETON,
                                     (char *)m_ShmID,
                                     pSA, 
                                     &m_hShm);

        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_SHM, 
                                                              SAPDB_ToString (m_ShmID), SAPDB_ToString (rc));       
            return (false);
        }

        memset (m_pShm, 0, m_ShmSize);
    }
#else    
    RTE_Path            shmPath;
    mode_t              umask_old;
    SAPDB_Int4          dummy;
    SAPDB_Int4          groupId;


    /* Determining path to memory mapped file */
    memset (shmPath, 0, sizeof (RTE_Path));
    GetShmPath (shmPath);
    SAPDBTRACE_WRITELN (Console_Trace, 1, "Path of shared memory: " << shmPath);

    umask_old = umask (0);
    umask (0007 & umask_old);
    if (NULL == (m_pShm = (SAPDB_Byte *)sqlAllocSharedMem (shmPath, m_ShmSize)))
    {
        if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
        {
//          SAPDBTRACE_WRITELN (ConsoleThread, 1, "Allocating Memory for MMAP file failed");
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ATTACH_SHM_UNIX, 
                                                              (SAPDB_Char *)m_ShmID);       
        }
        else    //RTE_CONSOLE_CLIENT_MODE 
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_SHM_UNIX, 
                                                              (SAPDB_Char *)m_ShmID);       
        }

        umask (umask_old);
        return false;
    }

    umask (umask_old);
    if (RTE_CONSOLE_CLIENT_MODE == m_OpenMode)
    {
        if (!RTE_GetDefaultSapdbUserAndGroupIds (dummy, groupId, messageList))
        {
            return false;
        }

        if (!RTE_SetFileOwnerAndGroup (shmPath, getuid (), groupId, messageList))
        {
            return false;
        }

        memset (m_pShm, 0, m_ShmSize);
    }

//    ++m_pShm->openCount;
#endif


    return true;
}

/*---------------------------------------------------------------------------*/

void    RTE_ConsoleDataCommunication::RemoveShm () 
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleDataCommunication::RemoveShm", 
                             Console_Trace, 9);
    if (NULL != m_pShm)
    {
#if defined (_WIN32)
        sql41c_free_shrd_mem (m_pShm, m_hShm);                              
#else
        RTE_Path            shmPath;

        memset (shmPath, 0, sizeof (RTE_Path));
        GetShmPath (shmPath);

        sqlFreeSharedMem(m_pShm, m_ShmSize);
        if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
        {
            unlink (shmPath) ;
        }
#endif
    }
}

/*---------------------------------------------------------------------------*/
