/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_Connect.hpp
  @author       StefanP

  @brief        Connect and Connect Reply for the Client Kernel Communication

  @since        2003-11-04  10:34
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


#ifndef RTECKC_CONNECT_HPP
#define RTECKC_CONNECT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     RTE_UNDEF_OFFSET4    SAPDB_MAX_UINT4	///<Undefined offset used for initialization (4 byte length) 

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    @class	RTECKC_Connect          
    @brief  This class handles the data part of the connect request packet.  

			An object of this class is instantiated by specifying a pointer to
			the memory location of the data part of the connect packet.	
			Therefore no memory allocation is necessary by instances of this class.
*/

class RTECKC_Connect
{
private:

  /*!
     @brief			Structure of the connect buffer
  */

    typedef struct  _Connect
    {
        RTE_OSPid           clientPID;
        SAPDB_UInt4         offsetConnectURI;
    } Connect;

public:

  /*!
     @brief			Constructor

     @param         pConnectBuffer [in]		- Pointer to the memory location of the data part
											  of the connect packet
     @param         openMode [in]			- RTECKC_Client or RTECKC_Server
  */	

    RTECKC_Connect    (SAPDB_Byte * const     pConnectBuffer,
                       RTECKC_HostType const  openMode)
        :   m_pConnectBuffer ((Connect *) pConnectBuffer)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::RTECKC_Connect", 
                                     CKC_Trace, 9);
            if (NULL != m_pConnectBuffer && RTECKC_Client == openMode)
            {
                m_pConnectBuffer->clientPID = RTE_UNDEF_OSPID;
                m_pConnectBuffer->offsetConnectURI = RTE_UNDEF_OFFSET4;
            }
        }

  /*!
     @brief			Set the process ID of the client in the connect packet.   

     @param         pid [in]				- Process ID
  */

    inline void         SetClientPID         (RTE_OSPid                    pid);

  /*!
     @brief			Get the process ID of the client from the connect packet   
  */

    inline RTE_OSPid    GetClientPID         () const;

  /*!
     @brief			Set the connection URI in the connect packet

     @param			parseURIObj [in]		- Parsed connection URI
  */

    inline void         SetConnectURI           (RTEComm_ParseURI const &   parseURIObj);

  /*!
     @brief			Get the connection URI from the connect packet 

     @param         parseURIObj [in]		- Parsed connection URI
     @param         messageList [out]		- Message list

     @return        true/false
  */

    inline  SAPDB_Bool    GetConnectURI           (RTEComm_ParseURI &           parseURIObj,
                                                   SAPDBErr_MessageList &       messageList) const;

  /*!
     @brief			Get length of the connect packet (excluding the header)   
  */

    inline  SAPDB_UInt4 GetDataLength         () const;



private:
    Connect *      m_pConnectBuffer;
};

/*---------------------------------------------------------------------------*/

inline void     RTECKC_Connect::SetClientPID (RTE_OSPid     pid)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::SetClientPID", CKC_Trace, 9);

    m_pConnectBuffer->clientPID = pid; 
}
/*---------------------------------------------------------------------------*/

inline RTE_OSPid   RTECKC_Connect::GetClientPID () 
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::GetClientPID", CKC_Trace, 9);

    return m_pConnectBuffer->clientPID; 
}

/*---------------------------------------------------------------------------*/

inline void     RTECKC_Connect::SetConnectURI (RTEComm_ParseURI const &   parseURIObj)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::SetConnectURI", CKC_Trace, 9);

    m_pConnectBuffer->offsetConnectURI = sizeof (Connect);
    strcpy (reinterpret_cast<SAPDB_Char *>(m_pConnectBuffer) + m_pConnectBuffer->offsetConnectURI, 
            parseURIObj.URI());
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool   RTECKC_Connect::GetConnectURI (RTEComm_ParseURI &          parseURIObj,
                                                   SAPDBErr_MessageList &      messageList) 
const
{
    RTEComm_URIUtils::URIRet    rcParse = RTEComm_URIUtils::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::GetConnectURI", CKC_Trace, 9);

    rcParse = parseURIObj.Parse (reinterpret_cast<SAPDB_Char *>(m_pConnectBuffer) + m_pConnectBuffer->offsetConnectURI,
                    messageList);
    if (RTEComm_URIUtils::NoError != rcParse)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PARSE_CONNECT_URI,
                                                    SAPDB_ToString (rcParse));
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4    RTECKC_Connect::GetDataLength () 
const
{
    SAPDB_UInt4     length = sizeof (*m_pConnectBuffer);
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Connect::GetDataLength", CKC_Trace, 9);

    if (RTE_UNDEF_OFFSET4 != m_pConnectBuffer->offsetConnectURI)
    {
        length += static_cast <SAPDB_UInt4>(strlen (reinterpret_cast<SAPDB_Char *>(m_pConnectBuffer) + 
                                            m_pConnectBuffer->offsetConnectURI)) + 1;
    }

    return length;
}

/*---------------------------------------------------------------------------*/

/*!
    @class	RTECKC_ConnectReply          
    @brief  This class handles the data part of the connect reply packet.  

			An object of this class is instantiated by specifying a pointer to
			the memory location of the data part of the connect packet.
*/

class RTECKC_ConnectReply
{
private:

  /*!
     @brief     Structure of the connect buffer
  */
    typedef struct  _ConnectReply
    {
		RTE_OSPid           serverPID;
		SAPDB_UInt4         commFlagIdx;	
//!!!
//-------------------------------------------------
//Testweise:
#if !defined (_WIN32)    
    RTESync_SemaphoreID serverSemID;
#endif
//-------------------------------------------------
		SAPDB_UInt4         offsetSessionURI;	
    } ConnectReply;

public:

  /*!
     @brief			Constructor

     @param         pConnectReplyBuffer [in]- Pointer to the memory location of the data part
											  of the connect reply packet
     @param         openMode [in]			- RTECKC_Client or RTECKC_Server
  */	

    RTECKC_ConnectReply    (SAPDB_Byte * const      pConnectReplyBuffer,
                            RTECKC_HostType const   openMode)
    :   m_pConnectReplyBuffer ((ConnectReply *)pConnectReplyBuffer)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::RTECKC_ConnectReply", 
                                     CKC_Trace, 9);
            if (NULL != m_pConnectReplyBuffer && RTECKC_Server == openMode)
            {
                m_pConnectReplyBuffer->serverPID = RTE_UNDEF_OSPID;
                m_pConnectReplyBuffer->commFlagIdx = SAPDB_MAX_UINT4;
                m_pConnectReplyBuffer->offsetSessionURI = RTE_UNDEF_OFFSET4;
            }
        }

  /*!
     @brief			Set the process ID of server process to the connect reply packet

     @param			pid [in]				- Process ID of the server
  */

    inline void         SetServerPID            (RTE_OSPid                    pid);

  /*!
     @brief			Get the process ID of the server from the connect reply packet   
  */

    inline RTE_OSPid    GetServerPID            () const;

  /*!
     @brief			Set the index of the communication flag in the connect reply packet.
					(Background: 
					By means of that index the flag can be located in a shared memory 
					segement. 
					The client signals new requests (communication)	for the related task 
					by that flag. The communication flags are stored centrally in a common 
					shared memory segment. Therefore the access to the flags during scan 
					for communication requests by the task scheduler is optimized 
					(compared to that the task scheduler had to access several shared memory 
					segments if the flags would be stored in the communication segments).

     @param			idx [in]				- Index of the communication flag
  */

    inline void         SetCommFlagIndex        (SAPDB_UInt4                idx);

  /*!
     @brief			Get the index of the communication flag from the connect reply packet   
  */

    inline SAPDB_UInt4  GetCommFlagIndex        () const;

//!!!
//-------------------------------------------------
//Testweise:
#if !defined (_WIN32)   
  /*!
     @brief			Only for test purposes
  */

    void          SetServerSemID        (RTESync_SemaphoreID serverSemID) 
        const
        {
            m_pConnectReplyBuffer->serverSemID = serverSemID;
        }

  /*!
     @brief			Only for test purposes  
  */

    RTESync_SemaphoreID  GetServerSemID        ()
        const
        {
            return m_pConnectReplyBuffer->serverSemID;
        }
#endif
//-------------------------------------------------

  /*!
     @brief			Set the session URI in the connect reply packet.
					(The session URI identifies the session (task) on the server side.
					This URI is set by the server and is needed by the client e.g for the 
					cancel request.)

     @param			pSessionURI [in]		- Pointer to session URI (pointer to character string)
  */

    inline void                 SetSessionURI  (SAPDB_Char const * const     pSessionURI);

  /*!
     @brief			Get the pointer to the session URI (pointer to character string)
					located in the connect reply packet.
  */

    inline  SAPDB_Char const *  GetSessionURI   () const;

  /*!
     @brief			Get length of the connect reply packet (excluding the header)   
  */

    inline  SAPDB_UInt4 GetDataLength         () const;


private:
  ConnectReply *  m_pConnectReplyBuffer;
};

/*---------------------------------------------------------------------------*/

inline  void    RTECKC_ConnectReply::SetServerPID (RTE_OSPid    pid)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::SetServerPID", CKC_Trace, 9);

    m_pConnectReplyBuffer->serverPID = pid; 
}

/*---------------------------------------------------------------------------*/

inline  RTE_OSPid   RTECKC_ConnectReply::GetServerPID () 
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::GetServerPID", CKC_Trace, 9);

    return m_pConnectReplyBuffer->serverPID; 
}

/*---------------------------------------------------------------------------*/
//!!!
inline  void    RTECKC_ConnectReply::SetCommFlagIndex (SAPDB_UInt4                idx)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::SetCommFlagIndex", CKC_Trace, 9);

    m_pConnectReplyBuffer->commFlagIdx = idx; 
}

/*---------------------------------------------------------------------------*/
//!!!
inline  SAPDB_UInt4 RTECKC_ConnectReply::GetCommFlagIndex () 
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::GetCommFlagIndex", CKC_Trace, 9);

    return m_pConnectReplyBuffer->commFlagIdx; 
}

/*---------------------------------------------------------------------------*/

inline  void    RTECKC_ConnectReply::SetSessionURI (SAPDB_Char const * const     pSessionURI)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::SetSessionURI", CKC_Trace, 9);

    m_pConnectReplyBuffer->offsetSessionURI = sizeof (*m_pConnectReplyBuffer);
    strcpy (reinterpret_cast<SAPDB_Char *>(m_pConnectReplyBuffer) + 
        m_pConnectReplyBuffer->offsetSessionURI, pSessionURI);
}

/*---------------------------------------------------------------------------*/

inline  SAPDB_Char const *  RTECKC_ConnectReply::GetSessionURI () 
const
{

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::GetSessionURI", CKC_Trace, 9);
    
    return (reinterpret_cast<SAPDB_Char *>(m_pConnectReplyBuffer) + 
                m_pConnectReplyBuffer->offsetSessionURI);
}

/*---------------------------------------------------------------------------*/


inline SAPDB_UInt4    RTECKC_ConnectReply::GetDataLength () 
const
{
    SAPDB_UInt4     length = sizeof (*m_pConnectReplyBuffer);

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ConnectReply::GetDataLength", CKC_Trace, 9);

    if (RTE_UNDEF_OFFSET4 != m_pConnectReplyBuffer->offsetSessionURI)
    {
        length += static_cast <SAPDB_UInt4>(strlen (reinterpret_cast<SAPDB_Char *>(m_pConnectReplyBuffer) + 
                                            m_pConnectReplyBuffer->offsetSessionURI)) + 1;
    }

    return length;
}

#endif  /* RTECKC_CONNECT_HPP */

