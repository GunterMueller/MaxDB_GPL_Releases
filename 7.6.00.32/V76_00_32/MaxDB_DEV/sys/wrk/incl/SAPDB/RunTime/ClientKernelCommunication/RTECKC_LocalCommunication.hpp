/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_LocalCommunication.hpp
  @author       StefanP

  @brief        Local Communication Class for Client Applications

  @since        2003-11-19  13:40
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


#ifndef RTECKC_LOCALCOMMUNICATION_HPP
#define RTECKC_LOCALCOMMUNICATION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/System/RTESys_ProcessState.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_RequestChannel.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ICommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommSequenceCheck.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunicationSegment.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_PacketHeader.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include    "RunTime/RTE_UniqueId.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#if !defined (CKC_TEST) && defined (_WIN32) 
#include    "gos00.h"   /* no check */
#endif


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     RTE_UNDEF_LEN4       SAPDB_MAX_UINT4	///<Undefined length (4 bytes)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    @class  RTECKC_LocalCommunication          
    @brief  This class is derived from the abstract class 'RTECKC_ICommunication'.
            It implements the the local communication which is based on shared memory
            communication.
            The interface is used by client applications to implement a local 
            communication with the database kernel.
*/

class RTECKC_LocalCommunication : public RTECKC_ICommunication
{

public:

  /*!
     @brief     Constructor
  */

    RTECKC_LocalCommunication                   ()
    :   m_NumPacket (0), 
        m_MPU (RTE_UNDEF_LEN4),
        m_HeaderSize (SAPDB_ALIGN_8BYTE (RTECKC_PacketHeader::GetPacketHeaderSize ())),
#if !defined (CKC_TEST) && defined (_WIN32)
        m_pFCS (NULL),
        m_pCommFlag (NULL),
#endif
        m_pCommunicationSegment (NULL),
        m_LookupTimeInterval (RTE_INFINITE_WAIT),
        m_pServerSem (NULL),
        m_ServerDB (NULL),
        m_SessionURI (NULL),
        m_ActualTaskSchedulerSemaphoreIdx (SAPDB_MAX_UINT4)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::RTECKC_LocalCommunication", 
                                     CKC_Trace, 9);
        }


  /*!
     @brief     Destructor
  */

    virtual ~RTECKC_LocalCommunication          ();

  /*!
     @brief     Open
                Open connection to a new database session or reconnect to an existing database 
                session (depending on the given URI)

     @param     parseURIObj [in]        - Parsed URI which is to be used for the connection
     @param     clientRef [in]          - Unique reference of the client to be used for the connection 
     @param     numPacket [in]          - Number of packets to be used for the communication
     @param     mdu [out]               - Maximum data unit per packet
     @param     messageList [out]       - Message list

     @return    NoError, Error, Offline, ResourceMissing, ClientConnectTimeout, WrongState,
                ServerClose, ServerTimeout, ServerCrash, Shutdown, TaskLimit, Starting, 
                NoPendingRequest (no connect request exists)
  */


    RTECKC_ClientCommunication::CommRC  
                    Open                        (RTEComm_ParseURI const &         parseURIObj,    
                                                 SAPDB_UInt4 const                clientRef,
                                                 SAPDB_UInt4 const                numPacket,
                                                 SAPDB_UInt4 &                    mdu,    
                                                 SAPDBErr_MessageList &           messageList);
    
  /*!
     @brief     PrepareClose
                This method guaranties that further requests (send, receive, 
                close, cancel) are not possible afterwards. But pending requests may still 
                exist. Those requests will normally return shortly except the receive request. 
                Therefore the client semaphore is triggered to ensure that even a pending
                receive requests will return immediately.

                If different requests for a connection may exists in parallel it has to be 
                guarantied that the last 'user' of the connection calls the 'Close' method.
                (This may be realized by using the 'Handle Manager').

     @param     messageList [out]    - Message list

     @return    NoError, Error, ClosePending
  */

    virtual RTECKC_ClientCommunication::CommRC    
                    PrepareClose                (SAPDBErr_MessageList &     messageList);

    
  /*!
     @brief     Close
                Close the local connection. All resources are destroyed.

                Caution:  This method may only be called if the connection is not used by anyone.
                          This means particulary that the client application may not use the
                          communication packets anymore!!!

     @param     messageList [out]    - Message list

     @return    true/false
  */
                                    
    virtual SAPDB_Bool      Close                       (SAPDBErr_MessageList &          messageList);

  /*!
     @brief     GetDataPacket
                Get pointer to a communication packet (data part) 

     @param     packetIdx [in]		- Index of the packet wanted (0 <= packetIndex < m_NumPacket)

     @return    Pointer to communication packet wanted or NULL when index is illegal 
                (out of range)
  */

    virtual inline SAPDB_Byte * const   GetDataPacket (SAPDB_UInt4 const          packetIdx) const;

  /*!
     @brief     Send
                Send data to Kernel

     @param     pSendData [in]      - Pointer to the data to be send (the pointer must be
                                      received by 'GetDataPacket') 
     @param     sendLen [in]        - Data length
     @param     messageList [out]   - Message list

     @return    NoError, Error, WrongState,  SendPending, PacketInUse, ClosePending
  */

    virtual RTECKC_ClientCommunication::CommRC  
                    Send                        (SAPDB_Byte const * const       pSendData,
                                                 SAPDB_UInt4 const              sendLen,
                                                 SAPDBErr_MessageList &         messageList);

  /*!
     @brief     ReplyAvailable
                Determine if a reply is available in one of the communication packets

     @return    true/false
  */

    virtual SAPDB_Bool  ReplyAvailable          () const;

  /*!
     @brief     Receive
                Receive data from the Kernel. It is ensured that we always receive the most 
                outstanding packet (First 'Send' - First 'Receive'). Insofar the 
                output parameter 'pReceiveData' may be useless for the caller.


     @param     pReceiveData [out]  - Pointer to the packet holding the data received
     @param     replyLen [out]      - Length of data received
     @param     messageList [out]   - Message list


     @return    NoError, Error, ClosePending, ReceivePending, NoPendingRequest, ServerClose, 
                ServerTimeout, WrongState, Shutdown, ServerCrash
  */

    virtual RTECKC_ClientCommunication::CommRC  
                    Receive                     (SAPDB_Byte * &                 pReceiveData,
                                                 SAPDB_UInt4 &                  replyLen,
                                                 SAPDBErr_MessageList &         messageList);

  /*!
     @brief     CancelRequest
                Cancel the actual command of the Kernel session we are connected to. 
                Since this request is send asynchronously to the Kernel (Requestor)
                it is not guarantied which command is actually canceled. 
     @param     messageList [out]   - Message list

     @return    NoError, Error, ResourceMissing, ClosePending
  */

    virtual RTECKC_ClientCommunication::CommRC  
                    CancelRequest               (SAPDBErr_MessageList &         messageList);


private:


#define SEM_CLIENT_NAME_SKELETON        "SDBTech-SEM-Client-"	///<First fixed part of the client semaphore name 
#define CONNECTION_TIMEOUT              120000					///<Connection timeout (in milliseconds)
//For testing: Infinite Wait
//#define CONNECTION_TIMEOUT            0xFFFFFFFF //Milliseconds

#define LOOKUP_ALIVE_TIME_INTERVAL      120000					///<While waiting on a server reply we are looking
																///<periodically for the server process state 
																///<(in milliseconds)

    // It is not allowed to call the Copy Constructor from outside
	inline   RTECKC_LocalCommunication          (const RTECKC_LocalCommunication & localComm) {;};

  /*!
     @brief     Connect

     @return    NoError, Error, ResourceMissing
  */

    RTECKC_ClientCommunication::CommRC  
                    Connect                     (SAPDB_Char *                    uniqueID,
                                                 SAPDB_UInt4 const               commSegSize,
                                                 RTEComm_ParseURI const &        parseURIObj,
                                                 SAPDBErr_MessageList &          messageList);
  /*!
     @brief     ConnectReceive
     @return    NoError, Error, ClientConnectTimeout, WrongState, ServerClose, ServerTimeout, 
                ServerCrash, Shutdown, TaskLimit, Starting, NoPendingRequest (connect request exists)
  */

    RTECKC_ClientCommunication::CommRC  
                    ConnectReceive              (RTE_OSPid &                    serverPID,
                                                 SAPDBErr_MessageList &         messageList);


  /*!
     @brief     CalculateMDU
                Determine the maximum data unit
     @return    Maximum packet unit
  */

    void            CalculateMDU                (SAPDB_UInt4 const              mcu, 
                                                 SAPDB_UInt4 const              granularity,
                                                 SAPDB_UInt4 const              commSegHeaderSize,
                                                 SAPDB_UInt4 &                  mdu) const;

  /*!
     @brief     GetCommSegSize
                Determine the communication segment size
     @return    Communication segment size
  */

    SAPDB_UInt4     GetCommSegSize              (SAPDB_UInt4 const              granularity,
                                                 SAPDB_UInt4 const              commSegHeaderSize,
                                                 SAPDB_UInt4 const              mpu) 
        const
        {
            return (SAPDB_ALIGN (commSegHeaderSize + m_NumPacket * mpu, granularity));
        }

  /*!
     @brief         GetServerDB
     @return        true/false
  */

    SAPDB_Bool      GetServerDB                 (RTEComm_ParseURI const &    parseURIObj,
                                                 SAPDBErr_MessageList &      messageList);

  /*!
     @brief         OpenRequestChannel
     @return        NoError, Error, ResourceMissing

  */

    RTECKC_ClientCommunication::CommRC  
                    OpenRequestChannel          (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         ReleaseRequestChannel
     @return        true/false
  */

    SAPDB_Bool      ReleaseRequestChannel       (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         InitServerSemaphores
     @return        true/false
  */

    SAPDB_Bool      InitServerSemaphores        (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         OpenActualServerSemaphore
     @return        true/false
  */

    SAPDB_Bool      OpenActualServerSemaphore   (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         ReleaseServerSemaphores
     @return        true/false
  */

    SAPDB_Bool      ReleaseServerSemaphores     (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         OpenServerProcess
     @return        NoError, Offline
  */

    RTECKC_ClientCommunication::CommRC  
                    OpenServerProcess           (RTE_OSPid const                serverPID,
                                                 SAPDBErr_MessageList &         messageList);

  /*!
     @brief         CloseServerProcess
     @return        true/false
  */

    void			CloseServerProcess           ();

  /*!
     @brief         CreateClientSemaphore
     @return        true/false
  */

    SAPDB_Bool      CreateClientSemaphore       (RTE_UniqueIdBuffer                 uniqueID,
                                                 SAPDBErr_MessageList &             messageList);
  /*!
     @brief         CloseClientSemaphore
     @return        true/false
  */

    SAPDB_Bool      CloseClientSemaphore        (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         OpenGlobalCommunicationSegment
     @return        true/false
  */

    RTECKC_ClientCommunication::CommRC      
                    OpenGlobalCommunicationSegment      (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         ReleaseGlobalCommunicationSegment
     @return        true/false
  */

    SAPDB_Bool      ReleaseGlobalCommunicationSegment   (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         OpenCommunicationSegment
     @return        true/false
  */

    SAPDB_Bool      OpenCommunicationSegment    (SAPDB_Char const * const           commSegName,
                                                 SAPDB_UInt4 &                      commSegSize,
                                                 RTE_SECURITY_ATTRIBUTES &          secAttribute,   
                                                 SAPDB_UInt4 &                      mdu,    
                                                 SAPDB_UInt4 const                  numPacket,
                                                 SAPDBErr_MessageList &             messageList);

  /*!
     @brief         CloseCommunicationSegment
     @return        true/false
  */

    SAPDB_Bool      CloseCommunicationSegment   (SAPDBErr_MessageList &    messageList);

  /*!
     @brief         InitSequenceCheck
     @return        true/false
  */

    inline SAPDB_Bool   InitSequenceCheck       (SAPDBErr_MessageList &    messageList);

    inline RTECKC_ClientCommunication::CommRC    
                    GetErrCodeFromSeqCheckError (RTECKC_ClientCommSequenceCheck::SyncRet const    rcSeq) const;

    inline RTECKC_ClientCommunication::CommRC    
                    GetErrCodeFromCommSegError  (RTECKC_CommunicationSegment::CommState const     rcComm) const;

    inline RTECKC_ClientCommunication::CommRC    
                    GetErrCodeFromIPCError      (RTEIPC_CommunicationObject::CommObjRC const      rcIPC) const;


    RTECKC_ClientCommSequenceCheck      m_CommSeqCheck;

    SAPDB_UInt4                         m_NumPacket;    

//!!! Temporary hack:
#if !defined (CKC_TEST) && defined (_WIN32)
    RTEIPC_NamedSharedMemory            m_GlobalCommSeg;
    PFLAG_COMM_SEG                      m_pFCS;
    SAPDB_ULong *                       m_pCommFlag;
#endif

    RTECKC_ClientCommunicationSegment * m_pCommunicationSegment;
    SAPDB_UTF8 *                        m_ServerDB; 

    SAPDB_Char *                        m_SessionURI;           //URI which is returned by the connect reply;
                                                                //Is used for further requests such as the cancel request

    RTESync_BinarySemaphore             m_ClientSem;
    SAPDB_ULong                         m_LookupTimeInterval;   //Timeout intervall for client semaphore

    SAPDB_UInt4                         m_NumTaskSchedulerSemaphores;
    SAPDB_UInt4                         m_ActualTaskSchedulerSemaphoreIdx; 
                                                                //Due to the Load Balancing the
                                                                //session may move to other task
                                                                //schedulers during connection time.
                                                                //Therefore the actual task scheduler
                                                                //semaphore idx must be updated 
                                                                //frequently.  
                                                                //This is done before we post the 
                                                                //semaphore while sending data.
    RTESync_BinarySemaphore *           m_pServerSem;           //Task scheduler semaphore
//!!!
//Testweise:
#if !defined (_WIN32)    
    RTESync_SemaphoreID                 m_ServerSemID;
#endif

    RTECKC_RequestChannel               m_RequestChannel;
    RTESys_ProcessState                 m_ServerProcess;
    SAPDB_UInt4                         m_HeaderSize;
    RTECKC_PacketHeader                 m_PacketHeader;
    SAPDB_UInt4                         m_MPU;
};


/*---------------------------------------------------------------------------*/

inline SAPDB_Byte * const   RTECKC_LocalCommunication::GetDataPacket (SAPDB_UInt4 const          packetIdx)
const
{
    SAPDB_Byte *    pPacket;
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_LocalCommunication::GetDataPacket", CKC_Trace, 9);

    pPacket = m_pCommunicationSegment->GetPacket (packetIdx);
    return (NULL != pPacket ? pPacket + m_HeaderSize : NULL);
}


#endif  /* RTECKC_LOCALCOMMUNICATION_HPP */

