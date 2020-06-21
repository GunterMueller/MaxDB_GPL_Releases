/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_Request.cpp
  @author       StefanP

  @brief        Requests used by the request channel between client applications
                and the kernel requestor.

  @since        2004-12-06  12:00
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

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

#include <limits.h>


#include    "RunTime/ClientKernelCommunication/RTECKC_Request.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"

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
/*!
  @brief Convert request type into readable string
  @param type [in] request type
 */

inline SAPDB_ToStringClass SAPDB_ToString (RTECKC_Request::RequestType     type)
{
    switch (type)
    {
    case RTECKC_Request::Undef: 
        return SAPDB_ToStringClass("Undefined");
    case RTECKC_Request::Connect: 
        return SAPDB_ToStringClass("Connect");
    case RTECKC_Request::Cancel: 
        return SAPDB_ToStringClass("Cancel");
    case RTECKC_Request::Dump: 
        return SAPDB_ToStringClass("Dump");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)type); 
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECKC_Request::Init (SAPDBErr_MessageList &      messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::Init", CKC_Trace, 9);

    if (NULL == m_pRequest)
    {
        m_pRequest = static_cast<Request *>(RTEMem_RteAllocator::Instance().Allocate
                                                    (m_MaxRequestSize));
        if (NULL == m_pRequest)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_MEMORY_EXHAUSTED, 
                "Request",
                SAPDB_ToString (m_MaxRequestSize));    
            return false;
        }
    }

    memset (m_pRequest, 0, m_MaxRequestSize);
    m_RequestType = Undef;
    m_parseURIRequest.CleanUp ();
    m_URIBuilder.CleanUp ();

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECKC_Request::InitRequestURI (SAPDB_Char const * const    pDBPath, 
                                            RequestType                 type,
                                            SAPDBErr_MessageList &      messageList)
{
    SAPDB_Char *    pURI;
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::InitRequestURI", CKC_Trace, 9);

    m_pRequest->offsetURI = sizeof (*m_pRequest);
    
    pURI = reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI;

    if (strlen (pDBPath) < m_MaxRequestSize)
    {
        strcpy (pURI, pDBPath);
    }
    else
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_TO_LARGE, 
                                           SAPDB_ToString (m_MaxRequestSize));
        return false;
    }

    const RTEComm_URIBuilder tmpURIBuilder (pURI, m_MaxRequestSize - m_pRequest->offsetURI);
    m_URIBuilder = tmpURIBuilder;


    if (RTEComm_URIBuilder::NoError != m_URIBuilder.Append (OPTION_TYPE, SAPDB_ToString (type), messageList))
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_APPEND_OPTION,
            SAPDB_ToString (OPTION_TYPE), pURI);
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

RTEIPC_CommunicationObject::CommObjRC   
                RTECKC_Request::DumpRequest (RTEIPC_CommunicationObject &       comObject,
                                             SAPDBErr_MessageList &             messageList) const
{   
    RTEIPC_CommunicationObject::CommObjRC       rcIPC = RTEIPC_CommunicationObject::NoError;
    SAPDB_UInt4                                 bytesToWrite;
    SAPDB_UInt4                                 bytesWritten = 0;
    SAPDBErr_MessageList                        tmpMessageList;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::Dump", CKC_Trace, 5);

    if (NULL == m_pRequest || 0 == m_pRequest->offsetURI || 0 == m_pRequest->size)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_INCOMPLETE_REQUEST,
                                                         SAPDB_ToString (m_pRequest));
        return RTEIPC_CommunicationObject::Error;
    }

    bytesToWrite = m_pRequest->size;
    rcIPC = comObject.Write (reinterpret_cast<SAPDB_Byte *> (m_pRequest),
                             bytesToWrite,
                             bytesWritten,
                             tmpMessageList);

    if (RTEIPC_CommunicationObject::NoError != rcIPC || bytesToWrite != bytesWritten)
    {
        tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WRITE_REQUEST,
            reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI,
            SAPDB_ToString (m_pRequest->size),
            SAPDB_ToString (bytesToWrite),
            SAPDB_ToString (bytesWritten));

        messageList = tmpMessageList;

        if (RTEIPC_CommunicationObject::ResourceMissing != rcIPC)
        {
            rcIPC = RTEIPC_CommunicationObject::Error;
        }
    }

    return rcIPC;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::PutConnectRequest   (SAPDB_Char const * const       commSegID, 
#if defined (_WIN32) || defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) || !defined (_REENTRANT)
                                                   RTESync_SemaphoreID const      clientSemID,        
#endif
                                                   SAPDB_Char const * const       uniqueID,
                                                   SAPDB_UInt4 const              commSegSize, 
                                                   SAPDB_Char const * const       pDBPath,
                                                   SAPDBErr_MessageList &         messageList)
{
    SAPDB_Char *    pURI;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::PutConnectRequest", CKC_Trace, 9);

    if (!Init (messageList))
    {
        return false;
    }

    if (!InitRequestURI (pDBPath, Connect, messageList))
    {
        return false;
    }

    if (RTEComm_URIBuilder::NoError != m_URIBuilder.Append (OPTION_COMMSEGID, commSegID, messageList))
    {
        pURI = reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI;
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_APPEND_OPTION,
            SAPDB_ToString (OPTION_COMMSEGID), pURI);
        return false;
    }

#if defined (_WIN32) || defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) || !defined (_REENTRANT)
    if (RTEComm_URIBuilder::NoError !=  m_URIBuilder.Append (OPTION_CLIENTSEMID, clientSemID, messageList))
    {
        pURI = reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI;
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_APPEND_OPTION,
            SAPDB_ToString (OPTION_CLIENTSEMID), pURI);
        return false;
    }
#endif

    if (RTEComm_URIBuilder::NoError !=  m_URIBuilder.Append (OPTION_UNIQUEOBJID, uniqueID, messageList))
    {
        pURI = reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI;
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_APPEND_OPTION,
            SAPDB_ToString (OPTION_UNIQUEOBJID), pURI);
        return false;
    }

    if (RTEComm_URIBuilder::NoError !=  m_URIBuilder.Append (OPTION_COMMSEGSIZE, commSegSize, messageList))
    {
        pURI = reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI;
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_APPEND_OPTION,
            SAPDB_ToString (OPTION_COMMSEGSIZE), pURI);
        return false;
    }

    m_pRequest->size = sizeof (*m_pRequest) +
                       static_cast <SAPDB_UInt4>(strlen (reinterpret_cast<SAPDB_Char *> (m_pRequest) + 
                                                 m_pRequest->offsetURI));
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::PutCancelRequest (SAPDB_Char const * const       pDBPath,
                                                SAPDBErr_MessageList &         messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::PutCancelRequest", CKC_Trace, 9);

    if (!Init (messageList))
    {
        return false;
    }

    if (!InitRequestURI (pDBPath, Cancel, messageList))
    {
        return false;
    }

    m_pRequest->size = sizeof (*m_pRequest) +
                       static_cast <SAPDB_UInt4>(strlen (reinterpret_cast<SAPDB_Char *> (m_pRequest) + 
                                                 m_pRequest->offsetURI));
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::PutDumpRequest (SAPDB_Char const * const       pDBPath,
                                                     SAPDBErr_MessageList &         messageList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::PutDumpRequest", CKC_Trace, 9);

    if (!Init (messageList))
    {
        return false;
    }

    if (!InitRequestURI (pDBPath, Dump, messageList))
    {
        return false;
    }


    m_pRequest->size = sizeof (*m_pRequest) +
                       static_cast <SAPDB_UInt4>(strlen (reinterpret_cast<SAPDB_Char *> (m_pRequest) + 
                                                 m_pRequest->offsetURI));
    return true;
}


/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::LoadRequest (RTEIPC_CommunicationObject &       comObject,
                                           SAPDBErr_MessageList &             messageList)
{   
    SAPDB_UInt4                             bytesRead = 0;
    RTEIPC_CommunicationObject::CommObjRC   rcIPC = RTEIPC_CommunicationObject::NoError;
    RTEComm_URIUtils::URIRet                rcParse = RTEComm_URIUtils::NoError;
    SAPDB_UTF8 const *                      pTypeString;
    SAPDBErr_MessageList                    tmpMessageList;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::Load", CKC_Trace, 5);

    if (!Init (messageList))
    {
        return false;
    }

    rcIPC = comObject.Read (reinterpret_cast<SAPDB_Byte *> (m_pRequest), 
                                          bytesRead,
                                          tmpMessageList);

    if (RTEIPC_CommunicationObject::NoError != rcIPC || 
        bytesRead < sizeof (*m_pRequest) || 
        bytesRead != m_pRequest->size)
    {
        tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_READ_REQUEST,
            SAPDB_ToString (bytesRead), SAPDB_ToString (sizeof (*m_pRequest))); 
        messageList = tmpMessageList;
        return false;
    }

    rcParse = m_parseURIRequest.Parse (reinterpret_cast<SAPDB_Char *> (m_pRequest) + m_pRequest->offsetURI, 
                                 messageList);
    if (RTEComm_URIUtils::NoError != rcParse)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PARSE_CLIENT_REQUEST, 
                                                         SAPDB_ToString (rcParse));
        return false;
    }

    if (!m_parseURIRequest.HasDBPath ())
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_WITHOUT_DB_PATH,
                        m_parseURIRequest.URI());
        return false;
    }

    if (!m_parseURIRequest.FindOption (OPTION_TYPE, pTypeString))
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_MISSING_TYPE,
            m_parseURIRequest.URI());
        return false;
    }

    if (0 == strcmp (reinterpret_cast <SAPDB_Char const *> (pTypeString), 
                     SAPDB_ToString (Connect)))
    {
        m_RequestType = Connect;
    }
    else if (0 == strcmp (reinterpret_cast <SAPDB_Char const *> (pTypeString), 
                          SAPDB_ToString (Cancel)))
    {
        m_RequestType = Cancel;
    }
    else if (0 == strcmp (reinterpret_cast <SAPDB_Char const *> (pTypeString), 
                          SAPDB_ToString (Dump)))
    { 
        m_RequestType = Dump;
    }
    else
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_UNKNOWN_REQUEST, 
            SAPDB_ToString (pTypeString));
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::GetConnectRequest   (SAPDB_Char const * &           commSegID, 
                                                   RTESync_SemaphoreID &          clientSemID,        
                                                   SAPDB_Char const * &           uniqueID,
                                                   SAPDB_UInt4 &                  commSegSize,
                                                   SAPDB_Char const * &           pSessionTyp,
                                                   SAPDB_Char const * &           pSessionID,
                                                   SAPDBErr_MessageList &         messageList)
const
{
    SAPDB_UTF8 const *          option;
    SAPDB_UTF8 const *          optionValue;
    SAPDB_UInt4                 numFoundOptions = 0;
    SAPDB_Bool                  foundSize = false;
    SAPDB_Bool                  foundCommSegID = false;
    SAPDB_Bool                  foundUniqueID = false;
    RTEComm_URI_DBPath const *  pParsedDBPath;
    SAPDB_UTF8 const *          pTmpSessionTyp;
    SAPDB_UTF8 const *          pTmpSessionID;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::GetConnectRequest", CKC_Trace, 9);

    //Get path elements

    pParsedDBPath = m_parseURIRequest.DBPath ();
    if (NULL == pParsedDBPath)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_WITHOUT_DB_PATH,
                                           m_parseURIRequest.URI());
        return false;
    }

    pTmpSessionTyp = pParsedDBPath->FirstOpaquePathSegment ();
	if (NULL == pTmpSessionTyp)
	{
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SESSION_MISSING_TYP,
                                           m_parseURIRequest.URI());
        return false;
	}

    pTmpSessionID = pParsedDBPath->NextOpaquePathSegment (pTmpSessionTyp);

    pSessionTyp = reinterpret_cast <SAPDB_Char const *> (pTmpSessionTyp);
    pSessionID = reinterpret_cast <SAPDB_Char const *> (pTmpSessionID);

    //Get options
    option = m_parseURIRequest.FirstOption (optionValue);

    while (NULL != option && (!foundCommSegID || !foundUniqueID || !foundSize))
    {
        if (!strcmp (reinterpret_cast <SAPDB_Char *> (OPTION_COMMSEGID), reinterpret_cast <SAPDB_Char const *> (option)))
        {
            commSegID = reinterpret_cast <SAPDB_Char const *> (optionValue);   
            foundCommSegID = true;
        }
        else if (!strcmp (reinterpret_cast <SAPDB_Char *> (OPTION_CLIENTSEMID), reinterpret_cast <SAPDB_Char const *> (option)))
        {
#if defined (_WIN32)
            clientSemID = (SAPDB_Char *) (optionValue);   
#else
            clientSemID = static_cast <SAPDB_Int> (strtol (reinterpret_cast <SAPDB_Char const *>(optionValue), NULL, 10));       
#endif
        }
        else if (!strcmp (reinterpret_cast <SAPDB_Char *> (OPTION_UNIQUEOBJID), reinterpret_cast <SAPDB_Char const *> (option)))
        {
            uniqueID = reinterpret_cast <SAPDB_Char const *> (optionValue);   
            foundUniqueID = true;
        }
        else if (!strcmp (reinterpret_cast <SAPDB_Char *> (OPTION_COMMSEGSIZE), reinterpret_cast <SAPDB_Char const *> (option)))
        {
            commSegSize = strtoul (reinterpret_cast <SAPDB_Char const *>(optionValue), NULL, 10);       

            if (0 == commSegSize || ULONG_MAX == commSegSize)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_REQUEST_WRONG_COMMSEGSIZE,
                    m_parseURIRequest.URI (),
                    SAPDB_ToString (commSegSize));
                return false;
            }

            foundSize = true;
        }

        option = m_parseURIRequest.NextOption (option, optionValue);
    }

    if (!foundCommSegID)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_REQUEST_MISSING_OPTION,
            m_parseURIRequest.URI (), SAPDB_ToString (OPTION_COMMSEGID));
        return false;
    }

    if (!foundUniqueID)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_REQUEST_MISSING_OPTION,
            m_parseURIRequest.URI (), SAPDB_ToString (OPTION_UNIQUEOBJID));
        return false;
    }

    if (!foundSize)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECT_REQUEST_MISSING_OPTION,
            m_parseURIRequest.URI (), SAPDB_ToString (OPTION_COMMSEGSIZE));
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTECKC_Request::GetCancelRequest   (SAPDB_Char const * &           pSessionTyp,
                                                  SAPDB_Char const * &           pSessionID,
                                                  SAPDBErr_MessageList &         messageList)
const
{
    RTEComm_URI_DBPath const *  pParsedDBPath;
    SAPDB_UTF8 const *          pTmpSessionTyp;
    SAPDB_UTF8 const *          pTmpSessionID;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::GetCancelRequest", CKC_Trace, 9);

    //Get path elements

    pParsedDBPath = m_parseURIRequest.DBPath ();
    if (NULL == pParsedDBPath)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_WITHOUT_DB_PATH,
                                           m_parseURIRequest.URI());
        return false;

    }

    pTmpSessionTyp = pParsedDBPath->FirstOpaquePathSegment ();
	if (NULL == pTmpSessionTyp)
	{
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SESSION_MISSING_TYP,
                                           m_parseURIRequest.URI());
        return false;
	}

    pTmpSessionID = pParsedDBPath->NextOpaquePathSegment (pTmpSessionTyp);
	if (NULL == pTmpSessionID)
	{
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CANCEL_MISSING_SESSION_ID,
                                           m_parseURIRequest.URI());
        return false;
	}

    pSessionTyp = reinterpret_cast <SAPDB_Char const *> (pTmpSessionTyp);
    pSessionID = reinterpret_cast <SAPDB_Char const *> (pTmpSessionID);

    return true;
}
