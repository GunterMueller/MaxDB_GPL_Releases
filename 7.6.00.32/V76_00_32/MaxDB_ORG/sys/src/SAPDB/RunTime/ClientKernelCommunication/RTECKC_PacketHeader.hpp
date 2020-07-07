/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_PacketHeader.hpp
  @author       StefanP

  @brief        RTE Header handling for the Client Kernel Communication

  @since        2003-11-21  12:19
  @sa           


  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTECKC_PACKETHEADER_HPP
#define RTECKC_PACKETHEADER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/Communication/RTEComm_PacketHeader.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"



extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
    @class	RTECKC_PacketHeader      
    @brief	This class extends the general class RTEComm_PacketHeader for the RTE Header
			by special needs for the Client Kernel Communication. These are basically
			methods for setting and checking the header for the different kinds of requests
			and replies (see 'RequestType' below).
			Instances of that class always work an given memory. Therefore no memory allocation
			is done by this class.
*/

class RTECKC_PacketHeader
{
public:

    enum RCPacketHeader {NoError,				///<No error
                         Error,					///<Error, not specified
                         ErrorMsgAvailable,		///<An error message (messagelist) is included in the data part
						 NoErrorMsgAvailable};	///<No error message is included in the data part (RequestType 'ErrorData')


private:
#define     PROTOCOL_VERSION        1	///<Protocol version for the local client kernel communication

    enum RequestType    {Undef = 0,
                         ConnectRequest,	///<Connect request from client to server
                         ConnectReply,		///<Connect reply from server to client
                         DataRequest,		///<Data request from client to server
                         DataReply,			///<Data reply request from client to server
                         ErrorData			///<Error reply (from server to client)
                        };

public:

  /*!
     @brief			Constructor

     @param         myRef [in]				- Own reference to be used for the communication
     @param         packetLen [in]			- Length of a packet (including header)
  */	

    RTECKC_PacketHeader      (SAPDB_UInt4 const         myRef = RTE_COMM_UNDEF_REF,
                              SAPDB_UInt4 const         packetLen = 0)
    :   m_MyRef (myRef),
        m_RemoteRef (RTE_COMM_UNDEF_REF),
        m_HeaderSize ((SAPDB_UInt4)(SAPDB_ALIGN_8BYTE (RTEComm_PacketHeader::PacketHeaderBufferSize ()))),
        m_PacketLen (packetLen),
        m_MaxDataLen (packetLen - m_HeaderSize)    {;}


  /*!
     @brief			Check the RTE header on the client for a connect reply send by the server.

     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         dataLen [in]			- Length of the data received (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        NoError, Error, ErrorMsgAvailable
  */	

    inline RCPacketHeader   CheckConnectReply   (SAPDB_Byte * const             pPacket,
                                                 SAPDB_UInt4 &                  dataLen,
                                                 SAPDBErr_MessageList &         messageList);

  /*!
     @brief			Check the RTE header on the client for a data reply send by the server.

     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         dataLen [in]			- Length of the data received (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        NoError, Error, ErrorMsgAvailable
  */	

    inline RCPacketHeader   CheckReply      (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4 &                      dataLen,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Check the RTE header on the server for a connect request send by the client.
					
     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         dataLen [in]			- Length of the data received (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true: No error 
					false: Error
  */	

    inline SAPDB_Bool   CheckConnectRequest (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4 &                      dataLen,
                                             SAPDBErr_MessageList &             messageList);

  /*!
     @brief			Check the RTE header on the server for a data request send by the client.
					
     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         dataLen [in]			- Length of the data received (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true: No error 
					false: Error
  */	

    inline SAPDB_Bool   CheckRequest        (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4 &                      dataLen,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Check the RTE header on the client for an error reply send by the server.
					If an error message was send it is returned by the message list.

     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         messageList [out]		- Message list

     @return        true: An error was received or an error occurred while analyzing the header
						  (in both cases the message list includes the concerning error message)
					false: No error was received (the returned messageList remains empty!!!)
  */	

    inline  SAPDB_Bool  CheckErrorReply     (SAPDB_Byte * const                 pPacket,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Set the RTE header on the client for a connect request to the server.

     @param         pPacket [in]			- Pointer to the packet header to be set
     @param         dataLen [in]			- Length of the data to be send (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true/false
  */

    inline SAPDB_Bool   SetConnectRequest   (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4  const                 dataLen,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Set the RTE header on the client for a data request to the server.

     @param         pPacket [in]			- Pointer to the packet header to be set
     @param         dataLen [in]			- Length of the data to be send (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true/false:
  */

    inline SAPDB_Bool   SetRequest          (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4  const                 dataLen,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Set the RTE header on the server for a connect reply to the client.

     @param         pPacket [in]			- Pointer to the packet header to be set
     @param         dataLen [in]			- Length of the data to be send (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true/false:
  */

    inline SAPDB_Bool   SetConnectReply     (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4 const                  dataLen,
                                             SAPDBErr_MessageList &             messageList) const;
  /*!
     @brief			Set the RTE header on the server for a data reply to the client.

     @param         pPacket [in]			- Pointer to the packet header to be set
     @param         dataLen [in]			- Length of the data to be send (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true/false:
  */

    inline SAPDB_Bool   SetReply            (SAPDB_Byte * const                 pPacket,
                                             SAPDB_UInt4 const                  dataLen,
                                             SAPDBErr_MessageList &             messageList) const;

  /*!
     @brief			Get the size of the packet header
  */

    static SAPDB_UInt4 const  GetPacketHeaderSize()
        {return ((SAPDB_UInt4)(SAPDB_ALIGN_8BYTE (RTEComm_PacketHeader::PacketHeaderBufferSize ())));}


private:

  /*!
     @brief			Check the RTE header received by the remote.
					
     @param         pPacket [in]			- Pointer to the packet (packet header) to be analyzed
     @param         expectedType [in]		- Expected request type
     @param         dataLen [in]			- Length of the data received (exluding header, in bytes)
     @param         remoteRef [out]			- Only relevant for connect request or connect reply
										      (reference of the remote side)
     @param         messageList [out]		- Message list

     @return        NoError, Error, ErrorMsgAvailable, NoErrorMsgAvailable
  */	

    RCPacketHeader  Check                   (SAPDB_Byte * const                 pPacket,
                                             RequestType const                  expectedType,
                                             SAPDB_UInt4 &                      dataLen,
                                             SAPDB_UInt4 &                      remoteRef,
                                             SAPDBErr_MessageList &             messageList) const;
  /*!
     @brief			Set the RTE header for a reply to the remote side.

     @param         pPacket [in]			- Pointer to the packet header to be set
     @param         reqType [in]			- Request type
     @param         dataLen [in]			- Length of the data to be send (exluding header, in bytes)
     @param         messageList [out]		- Message list

     @return        true/false:
  */

    SAPDB_Bool      Set                     (SAPDB_Byte * const                 pPacket,
                                             RequestType const                  reqType,
                                             SAPDB_UInt4 const                  dataLen,
                                             SAPDBErr_MessageList &             messageList) const;


    SAPDB_UInt4             m_HeaderSize;
    SAPDB_UInt4             m_MyRef;
    SAPDB_UInt4             m_RemoteRef;
    SAPDB_UInt4             m_MaxDataLen;
    SAPDB_UInt4             m_PacketLen;
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool   RTECKC_PacketHeader::SetConnectRequest (SAPDB_Byte * const		pPacket,
                                                            SAPDB_UInt4 const       dataLen,
                                                            SAPDBErr_MessageList &  messageList)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::SetConnectRequest", CKC_Trace, 7);

    return (Set (pPacket, ConnectRequest, dataLen, messageList));
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool   RTECKC_PacketHeader::SetRequest (SAPDB_Byte * const             pPacket,
                                                     SAPDB_UInt4 const              dataLen,
                                                     SAPDBErr_MessageList &         messageList)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::SetRequest", CKC_Trace, 7);

    return (Set (pPacket, DataRequest, dataLen, messageList));
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool   RTECKC_PacketHeader::SetConnectReply (SAPDB_Byte * const        pPacket,
                                                          SAPDB_UInt4 const         dataLen,
                                                          SAPDBErr_MessageList &    messageList)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::SetConnectReply", CKC_Trace, 7);

    return (Set (pPacket, ConnectReply, dataLen, messageList));
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool   RTECKC_PacketHeader::SetReply (SAPDB_Byte * const				pPacket,
                                                   SAPDB_UInt4 const                dataLen,
                                                   SAPDBErr_MessageList &           messageList)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::SetReply", CKC_Trace, 7);

    return (Set (pPacket, DataReply, dataLen, messageList));
}

/*---------------------------------------------------------------------------*/

inline  RTECKC_PacketHeader::RCPacketHeader  
        RTECKC_PacketHeader::CheckConnectReply (SAPDB_Byte * const					pPacket,
                                                SAPDB_UInt4 &						dataLen,
                                                SAPDBErr_MessageList &				messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::CheckConnectReply", CKC_Trace, 9);

    return Check (pPacket, ConnectReply, dataLen, m_RemoteRef, messageList);
}

/*---------------------------------------------------------------------------*/

inline  RTECKC_PacketHeader::RCPacketHeader  
        RTECKC_PacketHeader::CheckReply (SAPDB_Byte * const							pPacket,
                                         SAPDB_UInt4 &								dataLen,
                                         SAPDBErr_MessageList &						messageList)
const
{
    SAPDB_UInt4     dummyRef;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::CheckReply", CKC_Trace, 9);
     
    return Check (pPacket, DataReply, dataLen, dummyRef, messageList);
}

/*---------------------------------------------------------------------------*/

inline  SAPDB_Bool  RTECKC_PacketHeader::CheckConnectRequest (SAPDB_Byte * const		pPacket,
                                                              SAPDB_UInt4 &             dataLen,
                                                              SAPDBErr_MessageList &    messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::CheckConnectRequest", CKC_Trace, 9);

    return (NoError == Check (pPacket, ConnectRequest, dataLen, m_RemoteRef, messageList));
}

/*---------------------------------------------------------------------------*/

inline  SAPDB_Bool  RTECKC_PacketHeader::CheckRequest (SAPDB_Byte * const           pPacket,
                                                       SAPDB_UInt4 &                dataLen,
                                                       SAPDBErr_MessageList &       messageList)
const
{
    SAPDB_UInt4     dummyRef;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::CheckRequest", CKC_Trace, 9);

    return (NoError == Check (pPacket, DataRequest, dataLen, dummyRef, messageList));
}

/*---------------------------------------------------------------------------*/

inline  SAPDB_Bool  RTECKC_PacketHeader::CheckErrorReply (SAPDB_Byte * const        pPacket,
                                                          SAPDBErr_MessageList &    messageList)
const
{
    SAPDB_UInt4     dummyRef;
    SAPDB_UInt4     dummyLen;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_PacketHeader::CheckReply", CKC_Trace, 9);
     
    return (NoErrorMsgAvailable != Check (pPacket, ErrorData, dummyLen, dummyRef, messageList));
}

/*---------------------------------------------------------------------------*/


#endif  /* RTECKC_PACKETHEADER_HPP */

