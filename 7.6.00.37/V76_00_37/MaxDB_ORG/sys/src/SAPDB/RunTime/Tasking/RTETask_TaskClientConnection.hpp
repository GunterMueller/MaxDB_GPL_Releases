/*!========================================================================

  @file         RTETask_TaskClientConnection.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Single client kernel connection

                This class conatains all information of a 
                single client kernel connection. 
                Each task connected to client application(s)
                has at least one task client connection object 
                in it's client connection list (Refer to 
                'RTETask_TaskClientConnection'). 
                A new connection to a client leads to new
                object in this list. If a connection is 
                released via 'Release' this object 
                is removed from list and destroyed.
                Any further access to this object is a 
                fatal error!!!

  @since        2005-07-19  12:29
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


#ifndef RTETASK_TASKCLIENTCONNECTION_HPP
#define RTETASK_TASKCLIENTCONNECTION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Tasking/RTETask_DoublyLinkedListElement.hpp"
#include    "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommunicationSegment.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommSequenceCheck.hpp"
#include    "RunTime/RTE_KGS.hpp"
#include    "RunTime/System/RTESys_ProcessState.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_PacketHeader.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;
class RTECKC_KernelCommunication;          // Used to reference global kernel communication objects

/*!---------------------------------------------------------------------------
  @class  RTETask_TaskClientConnection
  @brief Single client kernel (task) connection

           This class contains all information of a 
           single client kernel connection. 
           Each task connected to client application(s)
           has at least one task client connection object 
           in it's client connection list (Refer to 
           'RTETask_TaskClientConnection'). 
           A new connection to a client leads to new
           object in this list. If a connection is 
           released via 'Release' this object 
           is removed from list and destroyed.
           Any further access to this object is a 
           fatal error!!!
 ----------------------------------------------------------------------------*/

/* 
  We use a struct here to prevent casting caused by a C/C++ transition
*/
struct RTETask_TaskClientConnection : public     RTETask_ITaskClientConnection,
                                      protected  RTETask_DoublyLinkedListElement
{
public:
    /*!
    @description    Open a new client task connection

                    A new connection to a client application is 
                    established.
                    
    @param      pUniqueObjID [in]  - Each client connection has its own 
                                     identfier (pUniqueObjID) to 
                                     distinguish there IPC resource names
                                     from other connections. Its a part 
                                     of the IPC resource names.

    @param      pCommSegID [in]    - It's a substring of the communication
                                     segment name to identify the kind of
                                     segment. To build up the complete 
                                     name the 'pUniqueObjID' must be appended.

    @param      clientSemID [in]   - Used to identifies the clients 
                                     communication semaphore.

    @param      commSegSize  [in]  - The communication segment size need for
                                         checks.
    @param      messageList  [out] - Error messages

    @return     true, if no error
    */
    SAPDB_Bool   OpenClientConnection( SAPDB_Char const *      pUniqueObjID,
                                       SAPDB_Char const *      pCommSegID,
                                       RTESync_SemaphoreID    &clientSemID,
                                       SAPDB_UInt4             commSegSize,
                                       SAPDBErr_MessageList    &messageList );


    /*!
    @description    Get the  maximum allowed send data length.
                    
                    After receiving data via 'Receive' a pointer
                    to a transfer buffer has been return holding 
                    the incomming data. This buffer has to be 
                    reused to reply date to the client via 'Send'.
                    The maximum allowed data that can be written
                    into this buffer is returned by 'MaxSendDataLength'.

                    WARNING:
                     Writting more than this lenght of data 
                     into the buffer might lead to a fatal
                     program error.

    @return     maximum data length that is allowed to put into 
                the transfer buffer. 
    */
    virtual SAPDB_UInt4  MaxSendDataLength();


    /*!
    @description    Receive data from the client application.
                    
                    After receiving data via 'Receive', a pointer
                    to a transfer buffer (pData) has been returned 
                    holding the incomming data. If the given timeout
                    value has expired before any data has been 
                    received, this methode returns with the error 
                    code 'SessionTimeout'.

    @param          timeOut    [in]  - Session timeout in seconds.
                                       If no data is received during 
                                       this period of time, the methode
                                       returns with the error code 
                                       'SessionTimeout'. Use the
                                       constant 'NoSessionTimeout' as
                                       input if no session timeout is 
                                       required.

    @param          pData      [out] - Pointer to the received data.
                                       This pointer has to be used for 
                                       subsequent 'Send' operation. 
                                       The methode 'MaxSendDataLength'
                                       allows to determine the maximum
                                       send data length.

    @param          dataLen    [out] - Received data length.
    @param          messageList[out] - Error messages

    @return     NoError, if no error is occured.
    */
    virtual ConnRet Receive( SAPDB_UInt4          timeOut, 
                             SAPDB_Byte * &       pData,
                             SAPDB_UInt4 &        dataLen,
                             SAPDBErr_MessageList &messageList );

    /*!
    @description    Send data to the client application.
                    
                    After receiving data via 'Receive', a pointer
                    to a transfer buffer (pData) has been returned 
                    holding the incomming data.  This buffer has to be 
                    reused to reply date to the client via 'Send'.
                    The maximum allowed data that can be written
                    into this buffer is returned by 'MaxSendDataLength'.


    @param          pData       [in] - Pointer to the send data.
                                       This pointer must be the same 
                                       pointer obtained by the prior 
                                       'Receive' call.
                                       The methode 'MaxSendDataLength'
                                       should be called to determine
                                       the maximum data length allowed
                                       to store into this buffer. 

    @param          dataLen     [in] - Received data length.
    @param          messageList[out] - Error messages

    @return     NoError, if no error is occured.
    */
    virtual ConnRet Send( SAPDB_Byte * const   pData,
                          SAPDB_UInt4 const    dataLen,
                          SAPDBErr_MessageList &messageList );

    /*!
    @description    Close the current connection and destroy 
                    this object. Any further access to this object
                    is a fatal error!!!!!
    */
    virtual void   Release();


    /*!
    @description    Check if the client process died

    @return         true, if the client died
    */
    SAPDB_Bool   ClientDied()
    {
        return RTESys_ProcessState::Died == m_ClientProcessState.State();
    }


    /*!
    @description    Reject a client task connection  

                    A new connection to a client application is 
                    rejected.
                    
    @param          pUniqueObjID [in]  - Each client connection has its own 
                                         identfier (pUniqueObjID) to 
                                         distinguish there IPC resource names
                                         from other connections. Its a part 
                                         of the IPC resource names.

    @param          pCommSegID [in]    - It's a substring of the communication
                                         segment name to identify the kind of
                                         segment. To build up the complete 
                                         name the 'pUniqueObjID' must be appended.

    @param          clientSemID [in]   - USed to identifies the clients 
                                         communication semaphore.

                                         WINDOWS:
                                            To build up the complete name the 'pUniqueObjID'
                                            must be appended.

    @param          commSegSize  [in]  - The communication segment size need for
                                         checks.

    @param          commState    [in]  - The communication state that is send back to 
                                         the client.
    */
    static void RejectConnectRequest( SAPDB_Char const *                     pUniqueObjID,
                                      SAPDB_Char const *                     pCommSegID,
                                      RTESync_SemaphoreID                   &clientSemID,
                                      SAPDB_UInt4                            commSegSize,
                                      RTECKC_CommunicationSegment::CommState commState = 
                                            RTECKC_CommunicationSegment::Error );


    /*!
    @description    Get the task this connection belongs to

    @return         Pointer to the task object
    */
    RTETask_Task*  Task()
    {
        return m_pTask;
    }

    /*!
    @description    Update the clients task scheduler semaphore index

                    The load balancing mechanism allows the 
                    TaskScheduler to move a task to another 
                    TaskScheduler. Because the client needs the 
                    correct TaskScheduler semaphore to signal
                    the task a new packet arrival. The TaskScheduler
                    has to update client semaphore index after
                    a task movement.
    */
    void UpdateClientsTaskSchedulerSemaphoreIndex();

    /*!
    @description    Constructor
                    Should only be used by 'RTETask_TaskClientConnections'
    */
    RTETask_TaskClientConnection ( RTETask_Task* pTask ) 
         : m_pTask(pTask),
           m_CommunicationSegment(),
           m_KGS(RTE_KGS::Instance()),
           m_HandleLegacyConnection(false),
           m_pKernelCommunication(RTE_KGS::Instance().KernelCommunication() )
    {
    }

    /*!
    @description    Destructor
    */
    virtual ~RTETask_TaskClientConnection() {};

private:
    friend class RTETask_TaskClientConnections;

    /*!
    @description    Check if database state allows a connection 

    @return          NoError, connecting to kernel is allowed
                     any other state will be send to the client
    */
    RTECKC_CommunicationSegment::CommState CheckDBState ();

    /*!
    @description    Cleanup the resources on open clinet connection error

    First of all this methode write a message list to the 
    diagnostic file. After this a communication state is 
    send to the client (if not equal NoError) and all communication
    resources will be closed.

    @param          commstate    [in] - Optional communication state which 
                                        is send to the client, if not equal 
                                        NoError.
    */
    inline void OpenErrorCleanUp( RTECKC_CommunicationSegment::CommState  commstate = 
                                                                          RTECKC_CommunicationSegment::Error );



    /*!
    @description    Open the clients IPC resources

                    Opens all client IPC resource needed for the communication.
                    Must be the first operation before establishing or rejecting
                    the client connect request.

    @param      pUniqueObjID [in]  - Each client connection has its own 
                                     identfier (pUniqueObjID) to 
                                     distinguish there IPC resource names
                                     from other connections. Its a part 
                                     of the IPC resource names.

    @param      pCommSegID [in]    - It's a substring of the communication
                                     segment name to identify the kind of
                                     segment. To build up the complete 
                                     name the 'pUniqueObjID' must be appended.

    @param      clientSemID [in]   - Used to identifies the clients 
                                     communication semaphore.

                                     WINDOWS:
                                      To build up the complete name the 'pUniqueObjID'
                                       must be appended.

    @param      commSegSize  [in]  - The communication segment size need for
                                     checks.

    @param      clientSemaphore[in/out]      - Reference to the client semaphore to be opened
    @param      communicationSegment[in/out] - Reference to the communication segment to be opened
    @param      messageList         [out]    - Error messages

    @return     true if no error
    */
    static SAPDB_Bool OpenClientIPCResources( SAPDB_Char const *                pUniqueObjID,
                                              SAPDB_Char const *                pCommSegID,
                                              RTESync_SemaphoreID              &clientSemID,
                                              SAPDB_UInt4                       commSegSize,
                                              RTESync_BinarySemaphore           &clientSemaphore,
                                              RTECKC_KernelCommunicationSegment &communicationSegment,
                                              SAPDBErr_MessageList              &messageList );
 
    /*!
    @description    Close the current connection

    @param          messageList  [out] - Error messages

    @return         true if no error
    */
    SAPDB_Bool  CloseClientConnection( SAPDBErr_MessageList &messageList );

private:
    RTETask_Task*                      m_pTask;
    RTESync_BinarySemaphore            m_ClientSemaphore;
    RTECKC_KernelCommunicationSegment  m_CommunicationSegment;
    RTEComm_ParseURI                   m_URI;
    RTE_KGS                            &m_KGS;            // Shortcut to kgs
    RTESys_ProcessState                m_ClientProcessState;
    RTE_OSPid                          m_ClientPID;
    RTECKC_KernelCommSequenceCheck     m_SequenceCheck;
    SAPDB_UInt4 *                      m_pCommFlag;
    SAPDB_UInt4                        m_CommFlagNo;
    RTECKC_PacketHeader                m_PacketHeader;
    SAPDB_Bool                         m_HandleLegacyConnection;
    RTECKC_KernelCommunication *       m_pKernelCommunication;
};


#endif  /* RTETASK_TASKCLIENTCONNECTION_HPP */
