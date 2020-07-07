/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_RequestChannel.cpp
  @author       StefanP

  @brief        Request Channel

                Implementation of the communication channel between client
                applications and the kernel requestor

  @since        2003-11-13  14:59
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

#include    "RunTime/ClientKernelCommunication/RTECKC_RequestChannel.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_TagFileHandlingUNIX.hpp"


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

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

RTECKC_RequestChannel::~RTECKC_RequestChannel ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_RequestChannel::~RTECKC_RequestChannel", CKC_Trace, 9);

    if (!Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_REQUEST_CHANNEL);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

RTEIPC_CommunicationObject::CommObjRC
             RTECKC_RequestChannel::Open (RTE_OpenMode                      openMode,
                                          SAPDB_Char *                      serverDB, 
                                          RTE_SECURITY_ATTRIBUTES  &        secAttribute,   
                                          SAPDBErr_MessageList &            messageList)
{
    RTE_Path                                    nameReqChannel;
    RTEIPC_CommunicationObject::CommObjRC       rcIPC = RTEIPC_CommunicationObject::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_RequestChannel::Open", CKC_Trace, 9);


#if defined (_WIN32)
    strcpy (nameReqChannel, RTECKC_REQUEST_CHANNEL_PATH);
    strcat (nameReqChannel, RTECKC_REQUEST_CHANNEL_NAME);
    strcat (nameReqChannel, serverDB);
#else
    if (!RTE_GetIPCFileName (nameReqChannel, 
                             KernelUser, 
                             serverDB, 
                             Mailslot, 
                             RTECKC_REQUEST_CHANNEL_NAME,
                             messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_FILENAME,
                                                          SAPDB_ToString (KernelUser),
                                                          SAPDB_ToString (Mailslot),
                                                          SAPDB_ToString (serverDB),
                                                          RTECKC_REQUEST_CHANNEL_NAME);

        return RTEIPC_CommunicationObject::Error;
    }

#endif

    rcIPC = m_Mailslot.Open (nameReqChannel, openMode, secAttribute, messageList);

    if (RTEIPC_CommunicationObject::NoError != rcIPC)
    {
        if (RTEIPC_CommunicationObject::ResourceMissing == rcIPC)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_MAILSLOT_MISSING,
                SAPDB_ToString (serverDB));
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_MAILSLOT,
                SAPDB_ToString (serverDB));
        }
    }

    return rcIPC;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool      RTECKC_RequestChannel::Close (SAPDBErr_MessageList &         messageList)
{
    if (!m_Mailslot.Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_MAILSLOT);            
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool   RTECKC_RequestChannel::Read   
(
    RTECKC_Request &            request,
    SAPDBErr_MessageList &      messageList
)
{

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_RequestChannel::Read", CKC_Trace, 1);

    if (!request.LoadRequest (m_Mailslot, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_LOAD_REQUEST);
        return false;
    }

 
    return true;
}

/*---------------------------------------------------------------------------*/

RTEIPC_CommunicationObject::CommObjRC
            RTECKC_RequestChannel::Write   
(
    RTECKC_Request const &      request,
    SAPDBErr_MessageList &      messageList
)
{
    RTEIPC_CommunicationObject::CommObjRC       rcIPC = RTEIPC_CommunicationObject::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_RequestChannel::Write", CKC_Trace, 1);

    // --- send a request
/*
    SAPDBTRACE_WRITELN(CKC_Trace, 7, "Write request: Type: "    << requestType      <<
                                     ", URI: "                  << uri              <<
                                     ", client object name: "   << clientObjName);
*/

    rcIPC = request.DumpRequest (m_Mailslot, messageList);
    if (RTEIPC_CommunicationObject::NoError != rcIPC)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_DUMP_REQUEST);
        if (RTEIPC_CommunicationObject::ResourceMissing == rcIPC)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_REQUEST_CHANNEL_MISSING);
        }
    }

    return rcIPC;
}

