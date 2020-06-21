/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_PacketHeader.cpp
  @author       StefanP

  @brief        Checks the RTE Header for consistency

  @since        2003-11-03  12:54
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
#include    "RunTime/ClientKernelCommunication/RTECKC_PacketHeader.hpp"
#include    "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"



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



RTECKC_PacketHeader::RCPacketHeader  
                RTECKC_PacketHeader::Check (SAPDB_Byte * const                 pPacket,
                                            RequestType const                  expectedType,
                                            SAPDB_UInt4 &                      dataLen,
                                            SAPDB_UInt4 &                      remoteRef,
                                            SAPDBErr_MessageList &             messageList)
const
{
    SAPDB_UInt1                 reqRespType;
    RTEComm_ProtocolID          protocolID;
    SAPDB_UInt1                 protocolVersion;
    SAPDB_UInt4                 senderRef;
    SAPDB_UInt4                 receiverRef;
    SAPDB_UInt4                 numBytesTotal;
    teo200_EventList            eventList;
    teo200_EventList            tmpEventList;
    RTEComm_PacketHeader        header (pPacket);
    SAPDB_Byte  * const         pData = pPacket + m_HeaderSize;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::Check", CKC_Trace, 7);

    if (!header.Get (&reqRespType, &senderRef, &receiverRef, &numBytesTotal, 
                     &dataLen, &protocolID, &protocolVersion, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_GET_PACKET_HEADER); 
        return Error;
    }

    SAPDBTRACE_WRITELN(CKC_Trace, 7, "Type: "               << reqRespType        <<
                                         ", SenderRef: "    << senderRef          <<
                                         ", ReceiverRef: "  << receiverRef        <<
                                         ", Total: "        << numBytesTotal      <<
                                         ", Current: "      << dataLen            <<
                                         ", ID: "           << protocolID         <<
                                         ", Version: "      << protocolVersion);

    if (PROTOCOL_VERSION < protocolVersion)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_PROTOCOL_VERSION, 
                                            SAPDB_ToString (PROTOCOL_VERSION), 
                                            SAPDB_ToString (protocolVersion));
        return Error;
    }

    if (Prot_CommSeg != protocolID)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_PROTOCOL_ID, 
                                            SAPDB_ToString (Prot_CommSeg), 
                                            SAPDB_ToString (protocolID));
        return Error;
    }

 
    if (numBytesTotal != dataLen || dataLen > m_MaxDataLen)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_PROTOCOL_DATA_LEN, 
                                            SAPDB_ToString (numBytesTotal), 
                                            SAPDB_ToString (dataLen),
                                            SAPDB_ToString (m_MaxDataLen));
        return Error;
    }

    if (reqRespType != (SAPDB_UInt1)expectedType || ErrorData == expectedType)
    {
        if ((SAPDB_UInt1)ErrorData == reqRespType)
        {
            if (!eventList.eo200_LoadEventList (pData, dataLen, &tmpEventList))
            {
                SAPDBErr_ConvertEventToMessageList (messageList, tmpEventList);
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_LOAD_EVENT);
            }
            else
            {
                SAPDBErr_ConvertEventToMessageList (messageList, eventList);
            }

            return ErrorMsgAvailable;
        }
        else
        {
            if (ErrorData != expectedType)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_RESPONSE_TYPE, 
                                                    SAPDB_ToString (expectedType),
                                                    SAPDB_ToString (reqRespType));
				return Error;
            }
			else //ErrorData == expectedType && ErrorData != reqRespType
			{
				return NoErrorMsgAvailable;
			}
        } 
    }

    if ( receiverRef != m_MyRef && ConnectRequest != expectedType )
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_RECEIVER_REF, 
                                            SAPDB_ToString (m_MyRef),
                                            SAPDB_ToString (receiverRef));
        return Error;
    }

    if ( ConnectRequest != expectedType && ConnectReply != expectedType )
    {
        if ( senderRef != m_RemoteRef )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_SENDER_REF, 
                                                SAPDB_ToString (m_RemoteRef),
                                                SAPDB_ToString (senderRef) );
            return Error;
        }
    }
    else
    {
        remoteRef = senderRef;
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool   RTECKC_PacketHeader::Set (SAPDB_Byte * const                  pPacket,
                                       RequestType const                   reqType,
                                       SAPDB_UInt4 const                   dataLen,
                                       SAPDBErr_MessageList &              messageList)
const
{
    RTEComm_PacketHeader        header (pPacket);

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::Set", CKC_Trace, 7);

    if (dataLen > m_MaxDataLen)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SET_DATA_LEN,
                                           SAPDB_ToString (dataLen),
                                           SAPDB_ToString (m_MaxDataLen));
        return false;
    }

    header.Set ((SAPDB_UInt1)reqType, m_MyRef, m_RemoteRef, dataLen, 
                Prot_CommSeg, PROTOCOL_VERSION, m_PacketLen);

    return true;
}

