/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_CommunicationSegment.hpp
  @author       StefanP

  @brief        Communication Segment for the local Client Kernel Communication

  @since        2003-11-13  15:18
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


#ifndef RTECKC_COMMUNICATIONSEGMENT_HPP
#define RTECKC_COMMUNICATIONSEGMENT_HPP

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (_WIN32)
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"	/*nocheck*/
#endif

#include    "RunTime/IPC/RTEIPC_NamedSharedMemory.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTECKC_UNDEF_COMMUNICATION_PACKET_IDX   SAPDB_MAX_UINT4 ///<Packet index used for initialization
#define RTECKC_CONNECT_PACKET_IDX               0				///<Packet Index for the connect packet

#define RTE_UNDEF_OFFSET2						SAPDB_MAX_UINT2 ///<Undefined offset used for initialization (2 byte length) 

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class RTECKC_CommSegHeader;
class RTECKC_LoadBalancingOption;
class RTECKC_ClientEventUNIXOption;
class RTECKC_CommSegPacketInfo;



  /*!
     @brief   Enumeration type to identify the host type      
  */

typedef enum {RTECKC_Client = 0,		///<Client (Application)
              RTECKC_Server				///<Server (Database)
} RTECKC_HostType;


/*!
    @class	RTECKC_CommunicationSegment         
    @brief	This class administers the communication segment and which is implemented as 
			shared memory. The communication segment serves as media for the 
			local communication between client applications and the database kernel.
	
			This class is used as base class for the client and kernel specific classes
			'RTECKC_ClientCommunicationSegment' and	'RTECKC_KernelCommunicationSegment'
			respectively.

			No instance can be instantiated from this class since the constructor is
			declared protected.

			The communication segment has the following structure 
			(see also RTECKC_CommunicationSegmentHeader.hpp for further details):

			Example: n packets, k options

\verbatim      
			   +--------------------------------------------+
			+--|     RTECKC_CommSegHeader                   |----+
			|  +--------------------------------------------+    |
			+->|     RTECKC_CommSegHeaderCommonOptionPart(1)|--+ |
			   + ------------------------------------------ +  | |
			   |     RTECKC_LoadBalancingOption             |  | |
			   +--------------------------------------------+  | |
			   |     ...                                    |<-+ |
			   |                                            |--+ |
			   +--------------------------------------------+  | |
			   |     RTECKC_CommSegHeaderCommonOptionPart(k)|<-+ |
			   + ------------------------------------------ +    |
			   |     RTECKC_xxxOption                       |    |
			   +--------------------------------------------+    |
			+--|     RTECKC_PacketInfo (1)                  |<---+
			|  +--------------------------------------------+
			|  .                                            .
			|  .                                            .
			|  +--------------------------------------------+
			+->|     RTECKC_PacketInfo (n)                  |
			   +--------------------------------------------+
			   |     Packet (1)  (Header + Data)            |       it is guarantied that there is no gap
			   +--------------------------------------------+       in between Header and Data part!!!
			   .                                            |
			   .                                            |
			   +--------------------------------------------+
			   |     Packet (n)  (Header + Data)            |
			   +--------------------------------------------+


			As one can see there are k options which may follow up the header part. These options
			serve as a kind of desription of the supported features (feature list). Client and server 
			must not support the same features. Therewith we achieve a high degree of flexibility for 
			extensions of the communication segment. Client and server inform each other about the 
			supported features. 

			Example: Client submits the features 'X', 'Y' and 'Z' to tht server. But the server supports
					 only X and Z
				
					 0 - feature not supported
					 1 - feature supported

			1) Communication Segment submitted to the server by the client

			|  ...
			|  +-+------------------------------------------+ 
			+..|0| RTECKC_CommSegHeaderCommonOptionPart(n)  |--+ 
			   +-+------------------------------------------+  |                                                      
			   |   RTECKC_XOption                           |  |                                      
			   +-+------------------------------------------+  |                                                          
			+--|0| RTECKC_CommSegHeaderCommonOptionPart(n+1)|<-+ 
			|  +-+------------------------------------------+                                                         
			|  |   RTECKC_YOption                           |     
			|  +--------------------------------------------+     
			+->|0| RTECKC_CommSegHeaderCommonOptionPart(n+2)|--+   
			   +-+------------------------------------------+  |   
			   |   RTECKC_ZOption                           |  |                                     
			   +--------------------------------------------+  |    
			   ...

			2) Communication Segment adapted by the server ('Y' not supported)

			|  ...
			|  +-+------------------------------------------+ 
			+->|1| RTECKC_CommSegHeaderCommonOptionPart(n)  |--+ 
			   +-+------------------------------------------+  |                                                      
			   |   RTECKC_XOption                           |  |                                      
			   +-+------------------------------------------+  |                                                          
			+--|0| RTECKC_CommSegHeaderCommonOptionPart(n+1)|<-+ 
			|  +-+------------------------------------------+                                                         
			|  |   RTECKC_YOption                           |     
			|  +--------------------------------------------+     
			+->|1| RTECKC_CommSegHeaderCommonOptionPart(n+2)|     
			   +-+------------------------------------------+     
			   |   RTECKC_ZOption                           |                                       
			   +--------------------------------------------+      
			   ...


			The description of 'RTECKC_ClientCommunication' already contains the constraints concerning 
			the right send/receive order on the client side.

			On the server side the following rules have to be complied:

			 1) The server has to receive the packets in the same order as the client has send them
			 2) The server has to send the data (reply) in the same order as he received the requests (FiFo)
			 3) In contrast to the client side the server has to comply a more restrictive send/receive 
			    sequence: 'Receive' and 'Send' have always to be called alternately starting with 'Receive'.

			The constraints 1) and 2) are observed by means of a sequence number related to each
			packet:

			Each packet which is send by the client gets a sequence number. This sequence number is 
			registered in the releated packet info. The server always knows the sequence number of
			the next awaited request. All packet infos are scanned for this number. After the packet is
			received and processed the kernel has to reply ('Send') to that request before the next
			'Receive' is called. Of course this is done on the same packet. The request is completely
			overwritten. The sequence number remains unchanged.

			Back on the client side we are awaiting the replies ('Receive') in the same order as we send
			the requests, i.e. if we were send the packets with the sequence numbers n, n+1, n+2,... 
			we are awaiting the reply packets in the same order. 

			The responsibility of observing the constraint 3) is left to the server and is not checked here.

			Sequence:

			Example of a sequence with n packets:

			Client:
			-------
			        m_LastSend      == 5
			        m_NextReceive   == 5
			                                                .
			                                                .
			        Send (packet i < n):                +-+-----+
			                                            |6|     |
			        Sequence number = ++m_LastSend      +-+     |    packet i
			                        = 6                 |       |
			                                            +-------+
			                                                .
			        => m_LastSend    == 6                   .
			           m_NextReceive == 5

			Server:
			-------
			        m_LastSend......= 5
			        m_NextReceive...= 5
			                                                .
			                                                .
			        Receive:                            +-+-----|
			                                            |6|     |
			        Get packet with sequence            +-+     |    packet i
			        number ++m_NextReceive              |       |
			        Sequence number: 5 + 1              +-------+
			                                                .
			        => m_LastSend    == 5                   .
			           m_NextReceive == 6

			Server:
			-------
			        m_LastSend      = 5
			        m_NextReceive   = 6
			                                                .
			                                                .
			        Send (packet i < n):                +-+-----+
			                                            |6|     |
			        Check sequence number of            +-+     |    packet i
			        packet i: 6 == ++m_LastSend ?       |       |
			                                            +-------+
			                                                .
			        => m_LastSend    == 6                   .
			           m_NextReceive == 6

			Client:
			-------
			        m_LastSend      = 6
			        m_NextReceive   = 5
			                                                .
			                                                .
			        Receive:                            +-+-----+
			                                            |6|     |
			        Get packet with sequence            +-+     |    packet i
			        number ++m_NextReceive              |       |
			                                            +-------+
			                                                .
			        => m_LastSend    == 6                   .
			           m_NextReceive == 6


//(Lastsituationen auf Kernel und Client bzgl. mehrerer Pakete lassen sich mit Powerpoint annimieren


			For each packet a state information is recorded in the communication segment 
			(cf. RTECKC_CommunicationSegmentHeader.hpp: RTECKC_CommSegPacketInfo) for 
			the client and server respectively which describes the connection/session state as follow:


			c - Client              0: Undefined
			s - Server              1: Send
			                        2: Pending
			                        3: Received


				      c s
				      -+-	(initialized by the constructor of RTECKC_CommSegPacketInfo)
				      0 0
				         \
				    ClientConnectSend
				           \
				            c s
				            -+-
				            2 0
				               \
				         ServerConnectReceive
				                 \
				                  c s
				                  -+-
				                  1 3
				                 /   \
				                /     \
				               /       \
				  Receive (Server)    ServerConnectSend/Send (Server)
				             /           \
				            /             \
				           /               \
				        c s                 c s
				        -+-                 -+-
				        2 1                 1 2
				           \               /
				            \             /
				             \           /
				     Send (Client)    ClientConnectReceive/Receive (Client)
				               \       /
				                \     /
				                 \   /
				                  c s
				                  -+-
				                  3 1

\endverbatim      

			These connection states are used by the 'Send' and 'Receive' methods to verify if
			the actual action is valid (at 'Send') or that an expected state (remote side in 'Pending' 
			state) is reached (at 'Receive').


			The communication segment header (cf. RTECKC_CommunicationSegmentHeader.hpp: RTECKC_CommSegHeader)
			holds a communication state for the client and the server side. Each state consists
			of a synchronous and a asynchronous state entry. With these states each side can signal the 
			other side special events as for example the server detects a session timeout.

			Synchronous events are events that happen during the normal communication which is proceeded
			always sequentially. In contrast to the synchronous events the asynchronous events are used
			by foreign threads (currently used by the server only) e.g to signal a database crash or 
			shutdown. Therewith synchronous and asynchronous events do not owerwrite their states.
*/


class RTECKC_CommunicationSegment : public RTEIPC_NamedSharedMemory
{
public:

  /*!
     @brief   Enumeration type for the different states of communication
  */

    enum CommState {Undefined = 0,		///<No valid state	
                    NoError,            ///<No error
                    Error,              ///<Error, not specified                       
                    Closed,				///<Database or application has released the session
                    Timeout,            ///<Session timeout on database side
                    Crashed,            ///<Database or application has terminated abnormally
                    Shutdown,           ///<Shutdown of database in progress
                    TaskLimitExceeded,  ///<No free database task available
                    Starting,           ///<Database is currently starting
                    WrongState,         ///<Session is in a wrong state
                    DataMissing,        ///<Application has called 'Receive', but there is no outstanding request ('Send')
                    DataOutstanding,    ///<Database or application has called 'Receive', but the data are still outstanding
                    DataAvailable       ///<Data are available from the remote side (database or application respectively)
    };


  /*!
     @brief   Destructor
  */

    virtual ~RTECKC_CommunicationSegment    ();

  /*!
     @brief			Create or open the communication segment in client (m_OpenMode == RTECKC_Client) or 
					server mode (m_OpenMode == RTECKC_Server) respectively. This includes especially 
					the setup of the communication segment header.

     @param			commSegName [in]		- Full name of the communication segment to be opened/created
	 @param			commSegSize [in]		- Size of the communication segment to be opened/created
	 @param			secAttribute [in]		- Security attribute for creating the communication segment.

											  Is not relevant for opening. In that case 
											  RTESys_SecurityAttributes::Instance().GetNoSA () can be 
											  given.
     @param         messageList [out]		- Message list

     @return		true/false
  */
    
    SAPDB_Bool      Open        (SAPDB_Char const * const             commSegName,  
                                 SAPDB_UInt4 const                    commSegSize,
                                 RTE_SECURITY_ATTRIBUTES &            secAttribute,   
                                 SAPDBErr_MessageList &               messageList);


  /*!
     @brief			Close the communication segment: The synchronous communication state
					of the client is set to the given state (Default: 'Closed' and all internal
					resources and attributes of the object are destroyed and reinitialized 
					respectively.

     @param         messageList [out]		- Message list  
	 @param			state [in]				- A state is left behind in the communication segment header
											  to inform the communication partner about the reason of 
											  closing the segment.
											  Default: 'Closed'.

     @return		true/false

  */
                                    
    SAPDB_Bool      Close       (SAPDBErr_MessageList &                 messageList);


  /*!
     @brief         Signals the close of the communication segment to the remote side

	 @param			state [in]				- A state is left behind in the communication segment header
											  to inform the communication partner about the reason of 
											  closing the segment.
											  Default: 'Closed'.
  */

    void			SignalCloseToRemote  (CommState const               state  = Closed);

  /*!
     @brief			Send packet (header + data) to the communication partner 

//Aktionen beschreiben, die durchgeführt werden!!!
//Beim Senden keine Fehlerüberprüfung. Fehler werden erst beim anschließendem Receive dedektiert.

//Comment header file:
//Kernel code has to ensure that the kernel takes notice of the right send/receive order.
//Send can only be called if a receive is pending.


     @param         pSendPacket [in]		- Pointer to the packet to be send
     @param         messageList [out]		- Message list  


     @return        NoError, Error, WrongState, 

  */

    CommState       Send                    (SAPDB_Byte const * const               pSendPacket,
                                             SAPDBErr_MessageList &                 messageList);

  /*!
     @brief			Receive packet from the communication partner

//Aktionen beschreiben, die durchgeführt werden!!!

//Comment header file:
//Kernel code has to ensure that the kernel takes notice of the right send/receive order.
//Receive and send have always to be called one after the other. Sequential calls to receive
//are not supported!!


     @param         pReceivePacket [out]	- Pointer to the packet holding the data received
     @param         messageList [out]		- Message list  

     @return        DataOutstanding, DataAvailable, Crashed, Closed, Error
                    Only in client mode (m_OpenMode == RTECKC_Client): DataMissing, Timeout, Shutdown
                    
  */

    CommState       Receive                 (SAPDB_Byte * &                         pReceivePacket,
                                             SAPDBErr_MessageList &                 messageList);


#if !defined (_WIN32) && !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) //&& defined (_REENTRANT)

  /*!
     @brief			Get the memory location for the UNIX event buffer 

     @return		Memory location or NULL if no event buffer is defined.
  */

    RTESync_BinarySemaphore::EventUNIX * const   GetUNIXEventBuffer   () const;
#endif

  /*!
     @brief			Determine packet pointer out of a given packet index

     @param         packetIdx [in]			- 0 <= packetIdx < m_NumPacket

     @return        Pointer to wanted packet or NULL on error.
  */

    inline  SAPDB_Byte * const   GetPacket          (SAPDB_UInt4 const          packetIdx) const;

  /*!
     @brief			Get pointer to the first packet

     @return        Pointer to the first packet or NULL on error.
  */

    SAPDB_Byte * const   GetFirstPacket     () const
        {
            return GetPacket (0);
        }

  /*!
     @brief			Get number of packets

     @return        Number of packets
  */

    SAPDB_UInt4     GetNumberPackets        () const
        {
            return m_NumPacket; 
        }

  /*!
     @brief			Get maximum packet unit (MPU)

     @return        Maximum packet unit - Maximum size (in bytes) of a packet (header + data)
  */

    SAPDB_UInt4     GetMPU                  () const
        {
            return m_MPU; 
        }


protected:

  /*!
     @brief			Constructor
					Constructor is protected. Therewith the instantiation of an objects of this class is not 
					possible from outside.

     @param         openMode [in]			- RTECKC_Client or RTECKC_Server
     @param         mpu [in]				- Maximum packet unit - Maximum size (in bytes) of a packet (header + data)
											  (To be specify by the client only)
     @param         numPacket [in]			- Number of packets to be used for the communication
											  (To be specify by the client only)
  */									      

    RTECKC_CommunicationSegment     (RTECKC_HostType const          openMode,
                                     SAPDB_UInt4 const              mpu = SAPDB_MAX_UINT4, 
                                     SAPDB_UInt4 const              numPacket = 0)         
        :   m_OpenMode (openMode),
            m_NumPacket (numPacket),
            m_pFirstPacket (NULL),
            m_MPU (mpu),
            m_pHeader (NULL),
            m_pOptionLoadBalancing (NULL),
#if !defined (_WIN32) 
			m_pOptionClientEventUNIX (NULL),
#endif
            m_pPacketInfo (NULL),
            m_LastSend (0),
            m_NextReceive (0),
            m_ActualReceivePacket (RTECKC_UNDEF_COMMUNICATION_PACKET_IDX),
            m_ActualSendPacket (RTECKC_CONNECT_PACKET_IDX),
			m_CloseSignaled (false)

        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::RTECKC_CommunicationSegment", 
                                     CKC_Trace, 9);
        }



  /*!
     @brief			The given packet is marked as next send packet (This is done by register a
					sequence number in the related packet info). 
					Set the next send packet
					(This method is only used by the client)

     @param         packetIdx [in]			- Index of the packet to be send  

  */									      

    void			SetNextSendPacket       (SAPDB_UInt4 const          packetIdx);

  /*!
     @brief			Check if we try to send/reply on the right packet (This is done by checking if the
					packet holds the right sequence number).
					(This method is only used by the server)


     @param         packetIdx [in]			- Index of the packet to be send  
     @param         messageList [out]		- Message list  

     @return		true:	A receive packet is available ('m_ActualReceivePacket' is set to the packet index)
					false:	No packet is available
  */									      

    SAPDB_Bool		CheckNextSendPacket		(SAPDB_UInt4 const          packetIdx,
                                             SAPDBErr_MessageList &     messageList);


  /*!
     @brief			Determine if a receive packet is available (This is done by looking for a packet
					which holds the expected sequence number in the releated packet info).

     @return        true:	A receive packet is available ('m_ActualReceivePacket' is set to the packet index)
					false:	No packet is available
  */									      

    SAPDB_Bool      GetNextReceivePacket            ();


  /*!
     @brief			Get the index of the actual send packet
  */									      

    SAPDB_UInt4     GetActualSendPacket				() const
        {
            return m_ActualSendPacket;
        }

  /*!
     @brief			Get the index of the actual receive packet
  */									      

    SAPDB_UInt4     GetActualReceivePacket			() const
        {
            return m_ActualReceivePacket;
        }

  /*!
     @brief			Set the index of the actual receive packet
  */									      

    void            SetActualReceivePacket			(SAPDB_UInt4 const          packetIdx)
        {
            m_ActualReceivePacket = packetIdx;
        }

  /*!
     @brief			Get the pointer to the Packet Info Array
  */									      

    RTECKC_CommSegPacketInfo * const GetPacketInfo  () const
        {
            return m_pPacketInfo;
        }

  /*!
     @brief			Get the pointer to the Communication Segment Header
  */									      

    RTECKC_CommSegHeader * const     GetHeader		() const
        {
            return m_pHeader;
        }

  /*!
     @brief			Get the pointer to the Load Balancing Option
  */									      

    RTECKC_LoadBalancingOption * const  GetLoadBalancingOption  () const
        {
			return m_pOptionLoadBalancing;
        }

private:

  /*!
     @brief			Copy Constructor - By declaring the copy constructor as private we prevent that
					he can be called from outside
  */

	inline  RTECKC_CommunicationSegment             (const RTECKC_CommunicationSegment & commSeg)
        : m_OpenMode (commSeg.m_OpenMode),
          m_NumPacket (commSeg.m_NumPacket),
          m_MPU (commSeg.m_MPU),
          m_pFirstPacket (NULL),
          m_pHeader (NULL),
          m_pOptionLoadBalancing (NULL),
#if !defined (_WIN32) 
		  m_pOptionClientEventUNIX (NULL),
#endif
          m_pPacketInfo (NULL),
          m_LastSend (0),
          m_NextReceive (0),
          m_ActualReceivePacket (RTECKC_UNDEF_COMMUNICATION_PACKET_IDX),
          m_ActualSendPacket (RTECKC_CONNECT_PACKET_IDX) {;};


  /*!
     @brief			Determine the packet index from a given packet pointer

     @return		Index of the packet or RTECKC_UNDEF_COMMUNICATION_PACKET_IDX on error       
  */

    inline  SAPDB_UInt4     GetPacketIdx            (SAPDB_Byte const * const packet) const;


  /*!
     @brief			Increment given value by one as long as the value is smaller than SAPDB_MAX_UINT4.
	 				Otherwise the value is set to zero again.
  */

#define INCREMENT_UINT4(_seq)       ((_seq) < SAPDB_MAX_UINT4 ? ++(_seq) : (_seq) = 0)

  /*!
     @brief			Version of the communication segment
  */

#define COMMUNICATION_SEGMENT_VERSION   1

    RTECKC_HostType const                   m_OpenMode;
    RTECKC_CommSegHeader *                  m_pHeader;
#if !defined (_WIN32) 
	RTECKC_ClientEventUNIXOption *			m_pOptionClientEventUNIX;
#endif
    RTECKC_LoadBalancingOption *            m_pOptionLoadBalancing;
    RTECKC_CommSegPacketInfo *              m_pPacketInfo;
    SAPDB_UInt4                             m_MCU;						///<Maximum Communication Unit
    SAPDB_UInt4                             m_MPU;						///<Maximum Packet Unit
    SAPDB_UInt4                             m_NumPacket;				
    SAPDB_Byte *                            m_pFirstPacket;
    SAPDB_UInt4                             m_LastSend;					///<Sequence Number of the packet last send
    SAPDB_UInt4                             m_NextReceive;				///<Sequence Number of the packet last received or
																		///<to be received shortly 
    SAPDB_UInt4                             m_ActualReceivePacket;		///<index of packet actually received
    SAPDB_UInt4                             m_ActualSendPacket;			///<index of packet actually to be send
	SAPDB_Bool								m_CloseSignaled;
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Byte * const   RTECKC_CommunicationSegment::GetPacket (SAPDB_UInt4 const          packetIdx)
const
{
    if (packetIdx >= m_NumPacket)
    {
        return NULL;
    }

    return (m_pFirstPacket + packetIdx * m_MPU);
}


/*---------------------------------------------------------------------------*/

/*!
  @brief Convert CommState into readable string
  @param state [in] state to convert
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTECKC_CommunicationSegment::CommState     state)
{
    switch (state)
    {
    case RTECKC_CommunicationSegment::Undefined: 
        return SAPDB_ToStringClass("UNDEFINED");
    case RTECKC_CommunicationSegment::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTECKC_CommunicationSegment::Error: 
        return SAPDB_ToStringClass("ERROR");
    case RTECKC_CommunicationSegment::Closed: 
        return SAPDB_ToStringClass("CLOSED");
    case RTECKC_CommunicationSegment::Timeout: 
        return SAPDB_ToStringClass("TIMEOUT");
    case RTECKC_CommunicationSegment::Crashed: 
        return SAPDB_ToStringClass("CRASHED");
    case RTECKC_CommunicationSegment::Shutdown: 
        return SAPDB_ToStringClass("SHUTDOWN");
    case RTECKC_CommunicationSegment::TaskLimitExceeded: 
        return SAPDB_ToStringClass("TASKLIMITEXCEED");
    case RTECKC_CommunicationSegment::Starting: 
        return SAPDB_ToStringClass("STARTING");
    case RTECKC_CommunicationSegment::WrongState: 
        return SAPDB_ToStringClass("WRONGSTATE");
    case RTECKC_CommunicationSegment::DataMissing: 
        return SAPDB_ToStringClass("DATAMISSING");
    case RTECKC_CommunicationSegment::DataOutstanding: 
        return SAPDB_ToStringClass("DATAOUTSTANDING");
    case RTECKC_CommunicationSegment::DataAvailable: 
        return SAPDB_ToStringClass("DATAAVAILABLE");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)state); 
}



#endif  /* RTECKC_COMMUNICATIONSEGMENT_HPP */

