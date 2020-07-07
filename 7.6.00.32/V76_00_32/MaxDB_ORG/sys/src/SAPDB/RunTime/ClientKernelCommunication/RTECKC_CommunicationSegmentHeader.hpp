/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_CommunicationSegmentHeader.hpp
  @author       StefanP

  @brief        Communication Segment Header for the local Client Kernel Communication

  @since        2003-10-30  12:45
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


#ifndef RTECKC_COMMUNICATIONSEGMENTHEADER_HPP
#define RTECKC_COMMUNICATIONSEGMENTHEADER_HPP

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (_WIN32)
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"	/*nocheck*/
#endif

#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegment.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
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

/*---------------------- Communication Segment Header -----------------------*/

/*!
    @class  RTECKC_CommSegHeader  
    @brief  Class for administration of the communication segment header. 

			An object of this class is instantiated by specifying the pointer to
			the memory location of the header within the communication segment.
			Therefore no memory allocation is necessary by the instances of 
			this class.
*/


class RTECKC_CommSegHeader
{
public:

  /*!
     @brief     The event type is used to identifiy the synchronous and 
				asynchronous communication state.    
  */

    enum EventType  {Synchronous = 0,	///<Synchronous events
                     Asynchronous};		///<Asynchronous events

public:

  /*!
     @brief     Communication state.    
  */

    struct CommunicationState
    {
		RTECKC_CommunicationSegment::CommState async; ///<Asynchronous communication state
        RTECKC_CommunicationSegment::CommState sync;  ///<Synchronous communication state
    };  


  /*!
     @brief     Structure of the communication segment header buffer
  */

    struct CommSegHeader 
    {
        SAPDB_UInt2         commSegVersion;			///<Version of the communication segment
        SAPDB_UInt2         numPacket;				///<Number of packets used for the communication
        SAPDB_UInt4         mpu;                    ///<Maximum Packet Unit
        SAPDB_UInt2         offsetFirstOptionPart;  ///<Offset of the first option part (relative to the address of the header)
        SAPDB_UInt2         offsetFirstPacketInfo;  ///<Offset of the first packet info (relative to the address of the header)
        CommunicationState  clientCommState;        ///<used to signal asynchronous events to the other
        CommunicationState  serverCommState;        ///<side (crashes); synchronous events are signaled
                                                    ///<by an messageList and a related reqRespType (ERROR)
                                                    ///<in the RTE header of the packet
                                                    ///<But the commState is set in any case (used as rc
                                                    ///<from the server side)
	};  

public:

  /*!
     @brief			Constructor

     @param         pCommSegBuffer [in]		- Pointer to the memory location where the communication 
										      segment header to be administered resides
     @param         openMode [in]			- RTECKC_Client or RTECKC_Server

  */		
    RTECKC_CommSegHeader    (void * const               pCommSegBuffer,
                             RTECKC_HostType const      openMode)
    :   m_pCommSegHeaderBuffer ((CommSegHeader *)pCommSegBuffer),
        m_OpenMode (openMode)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegHeader::RTECKC_CommSegHeader", 
                                     CKC_Trace, 9);
            if (NULL != m_pCommSegHeaderBuffer && RTECKC_Client == m_OpenMode)
            {
                m_pCommSegHeaderBuffer->commSegVersion = SAPDB_MAX_UINT2;
                m_pCommSegHeaderBuffer->numPacket = 0;
                m_pCommSegHeaderBuffer->mpu = 0;
                m_pCommSegHeaderBuffer->offsetFirstOptionPart = RTE_UNDEF_OFFSET2;
                m_pCommSegHeaderBuffer->offsetFirstPacketInfo = RTE_UNDEF_OFFSET2;
                m_pCommSegHeaderBuffer->clientCommState.async = RTECKC_CommunicationSegment::NoError;
                m_pCommSegHeaderBuffer->clientCommState.sync = RTECKC_CommunicationSegment::NoError;
                m_pCommSegHeaderBuffer->serverCommState.async = RTECKC_CommunicationSegment::NoError;
                m_pCommSegHeaderBuffer->serverCommState.sync = RTECKC_CommunicationSegment::NoError;
            }
        }

  /*!
     @brief			Set the offset of the first option part.
				    (The offset refers to the memory position where the communication segment header 
					lies!)  

     @param         offset [in]				- Offset to be set

     @return        Given offset is returned
  */	

    SAPDB_UInt2 SetFirstOptionPartOffset (SAPDB_UInt2   offset)
                {
                    return (m_pCommSegHeaderBuffer->offsetFirstOptionPart = offset); 
                }

  /*!
     @brief			Get the offset of the first option part ('RTE_UNDEF_OFFSET2' if no
					option part is available).
				    (The offset refers to the memory position where the communication segment header 
					lies!)  
  */	

    SAPDB_UInt2 GetFirstOptionPartOffset    () const
                {
                    return m_pCommSegHeaderBuffer->offsetFirstOptionPart; 
                }
    
  /*!
     @brief			Set the offset of the first packet info.
 				    (The offset refers to the memory position where the communication segment header 
					lies!)  

     @param         offset [in]				- Offset to be set

     @return        Given offset is returned
  */	

    SAPDB_UInt2 SetFirstPacketInfoOffset    (SAPDB_UInt2   offset)
                {
                    return (m_pCommSegHeaderBuffer->offsetFirstPacketInfo = offset); 
                }

  /*!
     @brief			Get the offset to the first packet info ('RTE_UNDEF_OFFSET2' if no
					option part is available).
				    (The offset refers to the memory position where the communication segment header 
					lies!)  
  */	

    SAPDB_UInt2  GetFirstPacketInfoOffset   () const
                {
                    return m_pCommSegHeaderBuffer->offsetFirstPacketInfo; 
                }

  /*!
     @brief			Set the version info

     @param         version [in]			- Version of the communication segment
  */	

    void        SetVersion                  (SAPDB_UInt4 const      version)
                {
                    m_pCommSegHeaderBuffer->commSegVersion = version; 
                }

  /*!
     @brief			Get the version of the communication segment
  */	

    SAPDB_UInt4 GetVersion                  () const
                {
                    return m_pCommSegHeaderBuffer->commSegVersion; 
                }

  /*!
     @brief			Set the number of packets

     @param         numPacket [in]			- Number of packets used for the session
  */	

    void        SetNumberPackets            (SAPDB_UInt4 const      numPacket)
                {
                    m_pCommSegHeaderBuffer->numPacket = numPacket; 
                }

  /*!
     @brief			Get the number of packets used for the session
  */	

    SAPDB_UInt4 GetNumberPackets            () const
                {
                    return m_pCommSegHeaderBuffer->numPacket; 
                }

  /*!
     @brief			Set the maximum packet unit (MPU) 

     @param         mpu [in]				- Maximum packet unit - maximum size (in bytes) 
											  of a packet (header + data)
  */	

    void        SetMPU                      (SAPDB_UInt4 const      mpu)
                {
                    m_pCommSegHeaderBuffer->mpu = mpu; 
                }

  /*!
     @brief			Get the maximum packet unit (MPU) 
  */	

    SAPDB_UInt4 GetMPU                      () const
                {
                    return m_pCommSegHeaderBuffer->mpu; 
                }

  /*!
     @brief			Set the own communication state (i.e. state of the server in open mode
					'RTECKC_Server' or state of the client in open mode 'RTECKC_Client')

     @param         event [in]				- Event type
	 @param			state [in]				- Communication state to be set
  */	

    inline void SetMyCommState              (EventType                                    event,
                                             RTECKC_CommunicationSegment::CommState const state);

  /*!
     @brief			Get the remote communication state (see above)

     @param         event [in]				- Event type

     @return        Synchronous or asynchronous communication state (depends on the requested 
					event type).
  */

    inline RTECKC_CommunicationSegment::CommState   
                GetRemoteCommState          (EventType                  event) const;

  /*!
     @brief			Get the own communication state (see above)

     @param         event [in]				- Event type

     @return        Synchronous or asynchronous communication state (depends on the requested 
					event type).
  */

    inline RTECKC_CommunicationSegment::CommState   
                GetMyCommState              (EventType                  event) const;


  /*!
     @brief			Get the buffer size needed to hold the communication segment header 
  */	

    static SAPDB_UInt4 const  GetCommSegHeaderBufferSize()
        {return (SAPDB_ALIGN_8BYTE (sizeof (RTECKC_CommSegHeader::CommSegHeader)));}

private:
  CommSegHeader *        m_pCommSegHeaderBuffer;
  RTECKC_HostType const  m_OpenMode;
};

/*---------------------------------------------------------------------------*/

inline void        RTECKC_CommSegHeader::SetMyCommState 
                            (EventType                                              event,
                             RTECKC_CommunicationSegment::CommState const           state)
{
    if (RTECKC_Client == m_OpenMode)
    {
        if (Synchronous == event)
        {
            m_pCommSegHeaderBuffer->clientCommState.sync = state;                                                
        }
        else
        {
            m_pCommSegHeaderBuffer->clientCommState.async = state;                                                
        }
    }
    else
    {
        if (Synchronous == event)
        {
            m_pCommSegHeaderBuffer->serverCommState.sync = state;                                                
        }
        else
        {
            m_pCommSegHeaderBuffer->serverCommState.async = state;                                                
        }
    }
}

/*---------------------------------------------------------------------------*/

inline RTECKC_CommunicationSegment::CommState   
                    RTECKC_CommSegHeader::GetRemoteCommState (EventType                  event) 
const
{
    if (RTECKC_Client == m_OpenMode)
    {
        if (Synchronous == event)
        {
            return m_pCommSegHeaderBuffer->serverCommState.sync;                                                
        }
        else
        {
            return m_pCommSegHeaderBuffer->serverCommState.async;                                                
        }
    }
    else
    {
        if (Synchronous == event)
        {
            return m_pCommSegHeaderBuffer->clientCommState.sync;                                                
        }
        else
        {
            return m_pCommSegHeaderBuffer->clientCommState.async;                                                
        }
    }
}

/*---------------------------------------------------------------------------*/

inline RTECKC_CommunicationSegment::CommState   
                    RTECKC_CommSegHeader::GetMyCommState (EventType                  event) 
const
{
    if (RTECKC_Client == m_OpenMode)
    {
        if (Synchronous == event)
        {
            return m_pCommSegHeaderBuffer->clientCommState.sync;                                                
        }
        else
        {
            return m_pCommSegHeaderBuffer->clientCommState.async;                                                
        }
    }
    else
    {
        if (Synchronous == event)
        {
            return m_pCommSegHeaderBuffer->serverCommState.sync;                                                
        }
        else
        {
            return m_pCommSegHeaderBuffer->serverCommState.async;                                                
        }
    }
}


/*------------ Communication Segement Header: Common Option Part ------------*/


/*!
    @class	RTECKC_CommSegHeaderCommonOptionPart
    @brief  Common part for the options (features).
			Each feature is identified by an Id. The server examines the feature list 
			submitted by the client. Those features which were not supported are marked as 
			'Not supported' by the server.
			An object of this class is instantiated by specifying the pointer to
			the memory location of the common option part within the communication segment.
*/

class RTECKC_CommSegHeaderCommonOptionPart
{
public:

  /*!
     @brief     Enumeration type for the id of the feature     
  */
    enum OptionID  {Undefined = 0,		///<
                    LoadBalancing,		///<Load Balancing support
					EventUNIX};			///<Client event resides in the communication segment (UNIX only)

private:

  /*!
     @brief     Structure of the common option part buffer
  */

struct OptionPart
{
    OptionID            optionId;
    SAPDB_Bool          supported;
    SAPDB_UInt2         offsetNextOptionPart;
};   

public:

  /*!
     @brief			Constructor

     @param         pOptionBuffer [in]		- Pointer to the memory location of the common option
											  part
     @param         openMode [in]			- RTECKC_Client or RTECKC_Server
  */		

    RTECKC_CommSegHeaderCommonOptionPart    (void * const               pOptionBuffer,
                                             RTECKC_HostType const      openMode)
    :   m_pOptionPartBuffer ((OptionPart *)pOptionBuffer)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegHeaderCommonOptionPart::RTECKC_CommSegHeaderCommonOptionPart", 
                                     CKC_Trace, 9);

            if (NULL != m_pOptionPartBuffer && RTECKC_Client == openMode)
            {
                m_pOptionPartBuffer->optionId  = Undefined;
                m_pOptionPartBuffer->supported = false;
                m_pOptionPartBuffer->offsetNextOptionPart = RTE_UNDEF_OFFSET2;
            }
        }

  /*!
     @brief			Set common informations for the option/feature 

     @param         id [in]					- Id of the feature
     @param         offsetNextOption [in]	- Offset of the next feature (common part). 
											  The offset refers to the memory position where 
											  the communication segment header lies!  

  */	

    void        SetOption               (OptionID const                 id,
                                         SAPDB_UInt2 const              offsetNextOption = RTE_UNDEF_OFFSET2)
        {
            m_pOptionPartBuffer->optionId = id;
            m_pOptionPartBuffer->offsetNextOptionPart = offsetNextOption;
        }

  /*!
     @brief			Get the feature id
  */	

    OptionID GetOptionId             ()
        {
            return (m_pOptionPartBuffer->optionId);
        }

  /*!
     @brief			Set the feature as 'supported'. 
  */	

    void        SetSupported            ()
        {
            m_pOptionPartBuffer->supported = true;
        }

  /*!
     @brief			Get offset of the common part of the next feature. If no further feature 
					is available 'RTE_UNDEF_OFFSET2' is returned.
					(The offset refers to the memory position where the communication segment header 
					lies!)  
  */

    SAPDB_UInt2 GetOffsetToNextOption   () const
        {
            return m_pOptionPartBuffer->offsetNextOptionPart;
        }

  /*!
     @brief			Get the buffer size needed to hold the common option part 
  */	

  static SAPDB_UInt4 const  GetOptionPartBufferSize()
        {return (SAPDB_ALIGN_8BYTE (sizeof (OptionPart)));}

private:

  OptionPart *  m_pOptionPartBuffer;
};

/*---------------------------------------------------------------------------*/

/*!
  @brief Convert the option ID of the communication segment header option part into a readable string
  @param option [in] Option ID
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTECKC_CommSegHeaderCommonOptionPart::OptionID     option)
{
    switch(option)
    {
    case RTECKC_CommSegHeaderCommonOptionPart::Undefined: 
        return SAPDB_ToStringClass("Undefined");
    case RTECKC_CommSegHeaderCommonOptionPart::LoadBalancing: 
        return SAPDB_ToStringClass("LoadBalancing");
	case RTECKC_CommSegHeaderCommonOptionPart::EventUNIX: 
        return SAPDB_ToStringClass("EventUNIX");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)option); 
}


/*----------- Communication Segment Header: Load Balancing Option -----------*/


/*!
    @class	RTECKC_LoadBalancingOption   
    @brief  If Load Balancing is supported the server has to keep the client up to date about
			the semaphore index of the actual task scheduler the session is running in.

			This is necessary because the session may change to another task scheduler during runtime.

			The semaphore is needed by the client to resume the task scheduler of the session.

			An object of this class is instantiated by specifying the pointer to
			the memory location of the Load Balancing option (index) within the communication segment.
*/


class RTECKC_LoadBalancingOption
{
public:

  /*!
     @brief			Constructor

     @param         pOption [in]			- Pointer to the memory location of 
											  the task scheduler semaphore index
  */	

    RTECKC_LoadBalancingOption    (void * const     pOption)
    :   m_pTaskSchedulerSemaphoreIdx ((SAPDB_UInt4 *)pOption)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_LoadBalancingOption::RTECKC_LoadBalancingOption", 
                                     CKC_Trace, 9);
        }

  /*!
     @brief			Set the task scheduler semaphore index

     @param         idx [in]				- Pointer to the task scheduler semaphore index
  */

    void        SetTaskSchedulerSemaphoreIdx                   (SAPDB_UInt4 const      idx)
                {
                    *m_pTaskSchedulerSemaphoreIdx = idx; 
                }

  /*!
     @brief			Get the task scheduler semaphore index
  */

    SAPDB_UInt4 GetTaskSchedulerSemaphoreIdx                   () const
                {
                    return *m_pTaskSchedulerSemaphoreIdx; 
                }

  /*!
     @brief			Get the buffer size needed to hold this option (index) 
  */

    static SAPDB_UInt4 const  GetLoadBalancingOptionBufferSize()
        {return (SAPDB_ALIGN_8BYTE (sizeof (SAPDB_UInt4)));}

private:
    SAPDB_UInt4 *  m_pTaskSchedulerSemaphoreIdx;
};

#if !defined (_WIN32) && !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) // && defined (_REENTRANT)

/*----------- Communication Segment Header: Client Event Option (UNIX) ------*/
/*!
    @class	RTECKC_ClientEventUNIXOption   
    @brief  If Load Balancing is supported the server has to keep the client up to date about
			the semaphore index of the actual task scheduler the session is running in.

			This is necessary because the session may change to another task scheduler during runtime.

			The semaphore is needed by the client to resume the task scheduler of the session.

			An object of this class is instantiated by specifying the pointer to
			the memory location of the Load Balancing option (index) within the communication segment.
*/


class RTECKC_ClientEventUNIXOption
{
public:

  /*!
     @brief			Constructor

     @param         pOption [in]			- Pointer to the memory location of 
											  the task scheduler semaphore index
  */	

    RTECKC_ClientEventUNIXOption    (void * const     pOption)
		:   m_pClientEventUNIX ((RTESync_BinarySemaphore::EventUNIX *)pOption)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientEventUNIXOption::RTECKC_ClientEventUNIXOption", 
                                     CKC_Trace, 9);
        }

  /*!
     @brief			Get the pointer to the UNIX event buffer which is located in the 
					communication segment
  */

    RTESync_BinarySemaphore::EventUNIX * const GetEventBuffer    () const
		{
            return m_pClientEventUNIX; 
        }

  /*!
     @brief			Get the buffer size needed to hold this option (index) 
  */

    static SAPDB_UInt4 const  GetClientEventUNIXOptionBufferSize()
        {return (SAPDB_ALIGN_8BYTE (sizeof (RTESync_BinarySemaphore::EventUNIX)));}

private:
    RTESync_BinarySemaphore::EventUNIX * const m_pClientEventUNIX;
};

#endif
/*---------------- Communication Segment Header: Packet Info ----------------*/


/*!
    @class	RTECKC_CommSegPacketInfo    
    @brief  Info part of a packet. In particular this class hold the sequence number of the packet   
			and the client and server connection state.

			An object of this class is instantiated by specifying the pointer to
			the memory location of the packet info within the communication segment.
*/


class RTECKC_CommSegPacketInfo
{
public:
    enum    ConnectionStateType  
            {
                Undefined = 0,	///<Undefined state (for initialization)
                Send,			///<Data are send and received by the remote side
                Pending,		///<Data are send but not yet received by the remote side
                Received		///<Data from the remote side are received 
            };

    enum    Location {
                Own = 0,		///<Own side (depends on the popint of view: Client or Kernel side respectively)
                Remote			///<Remote side (depends on the popint of view: Client or Kernel side respectively)
            };

private:

  /*!
     @brief     Structure of the packet info buffer
  */

struct PacketInfo
{
    SAPDB_UInt4             sequenceNumber;             
    ConnectionStateType     clientConnectionState;  //ehemals: clientFlag
    ConnectionStateType     serverConnectionState;  //ehemals: serverFlag    
    SAPDB_UInt2             offsetNextPacketInfo;   //offset or RTE_UNDEF_OFFSET2, if no further packet exists
};   

public:

  /*!
     @brief			Constructor

     @param         openMode [in]			- RTECKC_Client or RTECKC_Server
     @param         pPacketInfoBuffer [in]	- Pointer to the memory location of the packet info 
											  (Default: NULL)
  */	

    RTECKC_CommSegPacketInfo            (RTECKC_HostType const      openMode,
                                         void * const               pPacketInfoBuffer = NULL)
        :   m_OpenMode (openMode),
            m_pPacketInfoBuffer (static_cast <PacketInfo *> (pPacketInfoBuffer))
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegPacketInfo::RTECKC_CommSegPacketInfo", 
                CKC_Trace, 9);
            if (NULL != m_pPacketInfoBuffer && RTECKC_Client == m_OpenMode)
            {
                m_pPacketInfoBuffer->sequenceNumber = 0;
                m_pPacketInfoBuffer->clientConnectionState = Undefined;
                m_pPacketInfoBuffer->serverConnectionState = Undefined;
                m_pPacketInfoBuffer->offsetNextPacketInfo = RTE_UNDEF_OFFSET2;
            }
        }

  /*!
     @brief			Initialize the internal packet info buffer pointer (class attribute) with the given
					memory location.

     @param         pPacketInfoBuffer [in]	- Pointer to the memory location of the packet info
  */

    inline void        SetPacketInfoBuffer     (void * const               pPacketInfoBuffer);

  /*!
     @brief			Get the sequence number.
  */

    SAPDB_UInt4 GetSequenceNumber              ()
        {
            return m_pPacketInfoBuffer->sequenceNumber;
        }

  /*!
     @brief			Set the sequence number.

     @param         sequenceNumber [in]		- Sequence number
  */

    void        SetSequenceNumber              (SAPDB_UInt4                sequenceNumber)
        {
            m_pPacketInfoBuffer->sequenceNumber = sequenceNumber;
        }

  /*!
     @brief			Set the own or remote connection state.

     @param         location [in]			- 'Own' or 'Remote'
     @param         state [in]				- Connection state (see above)
  */

    inline void SetConnectionState      (Location const                 location,
                                         ConnectionStateType const      state);
  /*!
     @brief			Get the own or remote connection state.

     @param         location [in]			- 'Own' or 'Remote'
  */

    inline ConnectionStateType GetConnectionState      (Location const      location) const;

  /*!
     @brief			Get the buffer size needed to hold the packet info buffer
  */

    static SAPDB_UInt2 const  GetPacketInfoBufferSize()
        {return (SAPDB_ALIGN_8BYTE (sizeof (PacketInfo)));}

private:
    PacketInfo *                m_pPacketInfoBuffer;
    RTECKC_HostType const       m_OpenMode;
};


/*---------------------------------------------------------------------------*/

inline void    RTECKC_CommSegPacketInfo::SetPacketInfoBuffer (void * const               pPacketInfoBuffer)
{   
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegPacketInfo::SetPacketInfoBuffer",  CKC_Trace, 9);

    m_pPacketInfoBuffer = static_cast <PacketInfo *> (pPacketInfoBuffer);

    if (NULL != m_pPacketInfoBuffer && RTECKC_Client == m_OpenMode)
    {
        m_pPacketInfoBuffer->sequenceNumber = 0;
        m_pPacketInfoBuffer->clientConnectionState = Undefined;
        m_pPacketInfoBuffer->serverConnectionState = Undefined;
        m_pPacketInfoBuffer->offsetNextPacketInfo = RTE_UNDEF_OFFSET2;
    }
}


/*---------------------------------------------------------------------------*/

inline void    RTECKC_CommSegPacketInfo::SetConnectionState (Location const                 location,
                                                             ConnectionStateType const      state)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegPacketInfo::SetConnectionState", 
        CKC_Trace, 9);

    if (RTECKC_Client == m_OpenMode && Own == location ||
        RTECKC_Server == m_OpenMode && Remote == location)
    {
        m_pPacketInfoBuffer->clientConnectionState = state;
    }
    else
    {
        m_pPacketInfoBuffer->serverConnectionState = state;
    }

    return;
}

/*---------------------------------------------------------------------------*/

inline RTECKC_CommSegPacketInfo::ConnectionStateType 
            RTECKC_CommSegPacketInfo::GetConnectionState (Location const      location) 
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommSegPacketInfo::GetConnectionState", 
        CKC_Trace, 9);

    if (RTECKC_Client == m_OpenMode && Own == location ||
        RTECKC_Server == m_OpenMode && Remote == location)
    {
        return m_pPacketInfoBuffer->clientConnectionState;
    }
    else
    {
        return m_pPacketInfoBuffer->serverConnectionState;
    }
}

/*---------------------------------------------------------------------------*/

/*!
   @brief Convert ConnectionStateType into readable string
   @param state [in] state to convert
*/

inline SAPDB_ToStringClass SAPDB_ToString(RTECKC_CommSegPacketInfo::ConnectionStateType     state)
{
    switch(state)
    {
    case RTECKC_CommSegPacketInfo::Undefined: 
        return SAPDB_ToStringClass("UNDEFINED");
    case RTECKC_CommSegPacketInfo::Send: 
        return SAPDB_ToStringClass("SEND");
    case RTECKC_CommSegPacketInfo::Received: 
        return SAPDB_ToStringClass("RECEIVED");
    case RTECKC_CommSegPacketInfo::Pending: 
        return SAPDB_ToStringClass("PENDING");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)state); 
}

#endif  /* RTECKC_COMMUNICATIONSEGMENTHEADER_HPP */

