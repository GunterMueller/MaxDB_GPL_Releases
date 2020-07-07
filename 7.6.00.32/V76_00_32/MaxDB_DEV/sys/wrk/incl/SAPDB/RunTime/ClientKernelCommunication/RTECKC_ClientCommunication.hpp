/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommunication.hpp
  @author       StefanP

  @brief        Client Interface for the Client Kernel Communication

  @since        2003-11-19  13:43
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


#ifndef RTECKC_CLIENTCOMMUNICATION_HPP
#define RTECKC_CLIENTCOMMUNICATION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_HandleManager.hpp"
#include    "RunTime/RTE_MessageList.hpp"
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
class RTECKC_ICommunication;


/*!
    @class	  RTECKC_ClientCommunication  
    @brief    This class provides the interface used by a client applications
              to communicate with the database kernel. 

              Instances of this class represent the connections to the kernel.

              Actually only local connects (Shared Memory) are supported.

              The application can determine at open time the number of packets to be
              used for the connection. To work with more than one packet may increases
              the command execution time significantly because client and server can be 
              utilized more efficiently. Even the use of more than two packets may be 
              meaningfull because of certain load situations of the kernel or client 
              respectively.

              The following constraints must be satisfied:

			   1) Sending data in parallel is not allowed 
               2) Receive data in parallel is not allowed 
               3) Data can be send on a packet i if 
                    a) it is the first send on i or
                    b) reply data were already received for i ('no pending send')
               4) 'Receive' can only be called for a packet i if a reply is outstanding for
				  i (that means that a request was send before on i and
				  no pending 'Receive' exist for i)


              Example:
              
              From the constraints above we get i.e. a following communication sequence:
              
                        Preconditions:
                        
                        n >= 1 packets, 
                        m <= n, 
                        f: {1,...,n} -> {1,...,n} is a one-to-one function

                        Sequence:
                        
                        Send (f(1))
                        ..          
                        ..          
                        ..          
                        Send (f(m))

                        Receive    -+ 
                        Send        |
                        ..          |   'Receive'/'Send' arbitrary often 
                        ..           >  (We receive data always on the most outstanding
                        ..          |   packet. Sending data has to take place on the 
                        Receive     |   last received packet).
                        Send       -+   

                        Receive    -+  
                        ..          | 
                        ..           >    m times
                        ..          |
                        Receive    -+

              
*/

class RTECKC_ClientCommunication
{

public:

        enum CommRC    {NoError = 0,			///<No error
                        Error,                  ///<Error, not specified                       
                        TaskLimit,              ///<No free kernel task available         
                        ClientConnectTimeout,   ///<Timeout while waiting on connect reply of the database
                        ServerTimeout,          ///<Session timeout on database occured
                        ServerCrash,            ///<Database has terminated abnormally     
                        Starting,               ///<Database is currently starting
                        Shutdown,               ///<Shutdown of database in progress   
                        Offline,                ///<Database is offline
                        ServerClose,            ///<Database has closed the connection
                        ClientClose,            ///<Client application has closed the connection
                        WrongState,             ///<Connection is in wrong state
                        ResourceMissing,        ///<A database resource is missing
                        SendPending,            ///<A send request is already pending (see constraint 1) above)     
                        ClosePending,           ///<Close of the connection is pending
                        PacketInUse,            ///<The packet is in use, i.e. data were send but the reply was not read yet (see constraint 3) above)
                        NoPendingRequest,       ///<Receive was called without a outstanding reply (see constraint 4) above)                                                          
                        ReceivePending,         ///<A receive request is already pending (see constraint 2) above)              
                        NoReplyAvailable};      ///<Currently no reply packet is available     

  /*!
     @brief			Constructor   
  */

    RTECKC_ClientCommunication      ()
    :   m_HandleConnection (RTE_HANDLEMANAGER_UNDEF_HANDLE)
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::RTECKC_ClientCommunication", 
                                     CKC_Trace, 9);
        }

  /*!
     @brief			Destructor
  */

    ~RTECKC_ClientCommunication     ();

  /*!
     @brief         Open connection to a new session or reconnect to a session which is already open
                    (depends on the used URI)

     @param         pURI [in]               - Pointer to URI used for the connection
     @param         numPacket [in]          - Number of packets to be used for the communication
     @param         mdu [out]               - Maximum data unit per packet
     @param         messageList [out]		- Message list

     @return        NoError, Error, Offline, ResourceMissing, ClientConnectTimeout, WrongState,
                    ServerClose, ServerTimeout, ServerCrash, Shutdown, TaskLimit, Starting, 
                    NoPendingRequest (no connect request exists)
  */
                                    
    CommRC          Open           (SAPDB_Char const * const        pURI,
                                    SAPDB_UInt4 const               numPacket,
                                    SAPDB_UInt4 &                   mdu,
                                    SAPDBErr_MessageList &          messageList);

  /*!
     @brief         Close connection

     @param         messageList [out]		- Message list

     @return        NoError, Error, ClosePending, ClientClose
  */
                                    
    CommRC          Close           (SAPDBErr_MessageList &          messageList);

  /*!
     @brief         Get packet pointer

     @param         packetIndex [in]        - Index of the packet wanted (0 <= packetIndex < numPacket)
     @param         pPacket [out]           - Pointer to the data part of the wanted packet or NULL if
                                              return does not equal 'NoError'
     @param         messageList [out]		- Message list

     @return        NoError, Error, ClientClose, ClosePending      
  */

    CommRC          GetPacket       (SAPDB_UInt4 const              packetIndex,
                                     SAPDB_Byte * &                 pPacket,
                                     SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief         Get pointer to the first packet

     @param         pPacket [out]           - Pointer to the data part of the first packet or NULL if
                                              return does not equal 'NoError'
     @param         messageList [out]		- Message list

     @return        NoError, Error, ClientClose, ClosePending      
  */

    CommRC          GetFirstPacket  (SAPDB_Byte * &                 pPacket,
                                     SAPDBErr_MessageList &         messageList) const
        {
            return GetPacket ( 0, pPacket, messageList);
        }

  /*!
     @brief         Send data

     @param         pSendData [in]          - Pointer to the packet holding the data to be send
     @param         sendLen [in]            - Length of the data to be send
     @param         messageList [out]		- Message list
                    

     @return        NoError, Error, WrongState, ClientClose, ClosePending, SendPending, 
                    PacketInUse
  */

    CommRC          Send            (SAPDB_Byte const * const       pSendData,
                                     SAPDB_UInt4 const              sendLen,
                                     SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief         Determine if a reply is available in one of the communication packets

     @param         messageList [out]		- Message list

     @return        NoError, Error, NoReplyAvailable, ClientClose, ClosePending
  */

    CommRC          ReplyAvailable  (SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief         Receive
                    Receive data from Kernel. It is ensured that we always receive the most 
                    outstanding packet (First 'Send' - First 'Receive'). 
                    Insofar the output parameter 'pReceiveData' may be useless for the caller.

     @param         pReceiveData [out]      - Pointer to the packet holding the data received
     @param         receiveLen [out]        - Length of the data received
     @param         messageList [out]		- Message list

     @return        NoError, Error, ClientClose, ClosePending, ReceivePending, NoPendingRequest, 
                    ServerClose, ServerTimeout, WrongState, Shutdown, 
                    ServerCrash
  */

    CommRC          Receive         (SAPDB_Byte * &                 pReceiveData,
                                     SAPDB_UInt4 &                  receiveLen,
                                     SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief         Try to cancel the actual command. It is neither guarantied that a specific command 
                    is canceled nor that a command is canceled at all. This always depends on the actual
                    kernel state of the command execution.

     @param         messageList [out]		- Message list

     @return        NoError, Error, ClientClose, ClosePending, ResourceMissing
  */

    CommRC          Cancel          (SAPDBErr_MessageList &         messageList);

private:
  /*!
     @brief			Copy Constructor - By declaring the copy constructor as private we prevent that
					he can be called from outside
  */

	RTECKC_ClientCommunication  (const RTECKC_ClientCommunication & clientComm) {;};

  /*!
     @brief			Assignment operator - By declaring the assignment operator as private we prevent that
					he can be called from outside
  */

    RTECKC_ClientCommunication & operator = (const RTECKC_ClientCommunication& clientComm) 
        {
            return *this;   
        }

  /*!
     @brief         Get the connection object from the handle control

     @param         ppConnection [out]      - Pointer to a pointer to a abstact communication object
     @param         messageList [out]		- Message list

     @return        NoError, ClientClose, ClosePending

  */
    
    CommRC          LeaseConnectionData     (RTECKC_ICommunication ** const ppConnection,
                                             SAPDBErr_MessageList &         messageList) const;
  /*!
     @brief         Release the connection object get via 'LeaseConnectionData'

     @param         pConnection [in]        - Pointer to a abstact communication object
     @param         messageList [out]		- Message list

     @return        NoError, Error, ClientClose

  */

    CommRC          ReleaseConnectionData   (RTECKC_ICommunication *        pConnection,
                                             SAPDBErr_MessageList &         messageList) const;


    RTE_HandleManager::Hdl          m_HandleConnection;                    

};

/*!
  @brief Convert client communication return code into readable string
  @param rc [in] return code to convert
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTECKC_ClientCommunication::CommRC     rc)
{
    switch (rc)
    {
    case RTECKC_ClientCommunication::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTECKC_ClientCommunication::Error: 
        return SAPDB_ToStringClass("ERROR");
    case RTECKC_ClientCommunication::TaskLimit: 
        return SAPDB_ToStringClass("TASKLIMIT");
    case RTECKC_ClientCommunication::ClientConnectTimeout: 
        return SAPDB_ToStringClass("CLIENTCONNECTTIMEOUT");
    case RTECKC_ClientCommunication::ServerTimeout: 
        return SAPDB_ToStringClass("SERVERTIMEOUT");
    case RTECKC_ClientCommunication::ServerCrash: 
        return SAPDB_ToStringClass("SERVERCRASH");
    case RTECKC_ClientCommunication::Starting: 
        return SAPDB_ToStringClass("STARTING");
    case RTECKC_ClientCommunication::Shutdown: 
        return SAPDB_ToStringClass("SHUTDOWN");
    case RTECKC_ClientCommunication::Offline: 
        return SAPDB_ToStringClass("OFFLINE");
    case RTECKC_ClientCommunication::ServerClose: 
        return SAPDB_ToStringClass("SERVERCLOSE");
    case RTECKC_ClientCommunication::ClientClose: 
        return SAPDB_ToStringClass("CLIENTCLOSE");
    case RTECKC_ClientCommunication::WrongState: 
        return SAPDB_ToStringClass("WRONGSTATE");
    case RTECKC_ClientCommunication::ResourceMissing: 
        return SAPDB_ToStringClass("RESOURCEMISSING");
    case RTECKC_ClientCommunication::SendPending: 
        return SAPDB_ToStringClass("SENDPENDING");
    case RTECKC_ClientCommunication::ClosePending: 
        return SAPDB_ToStringClass("CLOSEPENDING");
    case RTECKC_ClientCommunication::PacketInUse: 
        return SAPDB_ToStringClass("PACKETINUSE");
    case RTECKC_ClientCommunication::NoPendingRequest: 
        return SAPDB_ToStringClass("NOPENDINGREQUEST");
    case RTECKC_ClientCommunication::ReceivePending: 
        return SAPDB_ToStringClass("RECEIVEPENDING");
    case RTECKC_ClientCommunication::NoReplyAvailable: 
        return SAPDB_ToStringClass("NOREPLYAVAILABLE");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)rc); 
}


#endif  /* RTECKC_CLIENTCOMMUNICATION_HPP */

