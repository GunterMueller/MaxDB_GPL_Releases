/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommunication.cpp
  @author       StefanP

  @brief        Local Communication (uses Client Communication)

  @since        2003-11-12  16:29
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

#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_LocalCommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ICommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_GlobalClientInstance.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include    "RunTime/RTE_ISystem.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"


//Testing
//#include    "RTECKC_TestClient.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     OPEN_RETRYCOUNT          3	///<Number of retries while opening the connection

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

RTECKC_ClientCommunication::~RTECKC_ClientCommunication ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::~RTECKC_ClientCommunication", CKC_Trace, 9);

    if (NoError != Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_CLIENT_COMM);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::Open (SAPDB_Char const * const        pURI,
                                              SAPDB_UInt4 const               numPacket,
                                              SAPDB_UInt4 &                   mdu,  //Vorschlagswert vom User, könnte man machen,
                                                                                    //wird aber erstmal nicht unterstützt; kann später leicht nachgerüstet werden  
                                              SAPDBErr_MessageList &          messageList)
{
    RTE_HandleManager &                         handleManager = RTECKC_GlobalClientInstance::Instance().GetHandleManager();

    CommRC                                      rc = NoError;
    RTE_HandleManager::HdlRet                   rcHdl = RTE_HandleManager::NoError;
    SAPDB_UInt4                                 retry;
    RTECKC_ICommunication *                     pConnection;
    RTEComm_URIUtils::URIRet                    rcParse = RTEComm_URIUtils::NoError;
    RTEComm_ParseURI                            parseURIObj;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::Open", CKC_Trace, 9);

    if (RTE_HANDLEMANAGER_UNDEF_HANDLE != m_HandleConnection)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CONNECTION_ALREADY_OPEN); 
        RTE_Message (messageList);
        return Error;
    }

    rcParse = parseURIObj.Parse (pURI, messageList);
    if (RTEComm_URIUtils::NoError != rcParse)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PARSE_URI_NEW_CONNECTION, 
            SAPDB_ToString (rcParse));
        RTE_Message (messageList);
        return Error;
    }

    /* May be substituted by the parsed protocol if other protocols than local are supported */

    mdu = RTE_UNDEF_LEN4;

  // --- Try to open
    for (retry = 0, rc = TaskLimit; TaskLimit == rc && retry <= OPEN_RETRYCOUNT; ++retry)
    {
        if (parseURIObj.IsLocalLocation ())
        {
            pConnection = new (RTEMem_Allocator::Instance()) RTECKC_LocalCommunication ();
            if (NULL == pConnection)
            {
                messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                    "RTECKC_LocalCommunication");       
                RTE_Message (messageList);
                return Error;
            }

			rc = (static_cast <RTECKC_LocalCommunication *>(pConnection))->Open 
						(parseURIObj, 
						 static_cast <SAPDB_UInt4> (m_HandleConnection), 
						 numPacket,
						 mdu,
						 messageList);
        }
        else
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PROTOCOL_NOT_IMPLEMENTED,
                SAPDB_ToString (parseURIObj.Protocol ())); 
            RTE_Message (messageList);
            return Error;
        }

        if (TaskLimit == rc)
        {
//Defines!!!
			RTE_ISystem::DoSleep(3000);
        }
    }

    if (NoError != rc)
    {
        if (ServerClose == rc || ServerTimeout == rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_OPEN_NEW_CONNECTION, 
                                                             pURI, SAPDB_ToString (rc));       
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_OPEN_NEW_CONNECTION, pURI);       
        }
    }
	else
	{
		rcHdl = handleManager.NewHandle (m_HandleConnection, pConnection);
		if (RTE_HandleManager::NoError != rcHdl)
		{
			messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_NEW_HANDLE_FOR_CONNECTION,
				SAPDB_ToString (rcHdl));
			rc = Error;
		}
	}

	if (NoError != rc)
	{
		SAPDBErr_MessageList	tmpMessageList;

        if (!pConnection->Close (tmpMessageList))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_CONNECTION);
			RTE_Message (tmpMessageList);
        }
		else
		{
			messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CONNECTION_CLOSED);
		}

        m_HandleConnection = RTE_HANDLEMANAGER_UNDEF_HANDLE; 
        destroy (pConnection, RTEMem_Allocator::Instance()); 
        RTE_Message (messageList);
	}

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::Close   (SAPDBErr_MessageList &          messageList)
{
    RTE_HandleManager &         handleManager = RTECKC_GlobalClientInstance::Instance().GetHandleManager();
    RTE_HandleManager::HdlRet   rcHdl = RTE_HandleManager::NoError;
    RTECKC_ICommunication *     pConnection;
    CommRC                      rc = NoError;
    CommRC                      rcRelease = NoError;
    SAPDBErr_MessageList        tmpMessageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::Close", CKC_Trace, 9);

    rc = LeaseConnectionData (&pConnection, messageList);
    if (NoError != rc)
    {
        RTE_Message (messageList);

        if (ClientClose == rc)
        {
            m_HandleConnection = RTE_HANDLEMANAGER_UNDEF_HANDLE;
            return NoError;
        }

        return rc;
    }

    rc = pConnection->PrepareClose (messageList);
    if (NoError != rc && ClosePending != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PREPARE_CLOSE_CONNECTION);       
    }

    rcRelease = ReleaseConnectionData (pConnection, tmpMessageList);
    if (NoError != rcRelease)
    {
        if (ClientClose == rcRelease)
        {
            m_HandleConnection = RTE_HANDLEMANAGER_UNDEF_HANDLE;
            return NoError;
        }

        tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
        if (NoError != rc)
        {
            messageList = messageList + tmpMessageList; 
        }
        else
        {
            messageList = tmpMessageList;
        }

        RTE_Message (messageList);
        return rcRelease;
    }

    if (NoError != rc)
    {
        if (ClosePending != rc)
        {
            RTE_Message (messageList);
        }

        return rc;
    }

    rcHdl = handleManager.FreeHandle (m_HandleConnection);
    if (RTE_HandleManager::NoError != rcHdl)
    {   //LeasePending or InvalidHandle: actually not possible
        //We do not monitor that the connection is already closed. But we have done everything
        //to ensure that it will be closed shortly by the request which holds actually the connection 
        //(LeaseHandleData) 
        rc = ClosePending;        
    }
    else
    {   //Connection is to close
        if (!pConnection->Close (messageList))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CLOSE_CONNECTION);
            rc = Error;
        }
		else
		{
			messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CONNECTION_CLOSED);
			rc = ClientClose;
		}

        m_HandleConnection = RTE_HANDLEMANAGER_UNDEF_HANDLE; 
        destroy (pConnection, RTEMem_Allocator::Instance()); 
        RTE_Message (messageList);
    }

    return rc;
}

/*---------------------------------------------------------------------------*/
RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::GetPacket (SAPDB_UInt4 const              packetIndex,
                                                   SAPDB_Byte * &                 pPacket,
                                                   SAPDBErr_MessageList &         messageList)
const
{
    RTECKC_ICommunication *     pConnection;
    CommRC                      rc = NoError;
    CommRC                      rcRelease = NoError;
    SAPDBErr_MessageList        tmpMessageList;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::GetPacket", CKC_Trace, 9);

    pPacket = NULL;

    rc = LeaseConnectionData (&pConnection, messageList);
    if (NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_LEASE_CONNECTION_DATA); 
        RTE_Message (messageList);
        return rc;
    }


    pPacket = pConnection->GetDataPacket (packetIndex);
    if (NULL == pPacket)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_ILLEGAL_PACKET_INDEX,
                                           SAPDB_ToString (packetIndex)); 
        rc = Error;
    }

    rcRelease = ReleaseConnectionData (pConnection, tmpMessageList);

    if (NoError != rcRelease)
    {
        if (ClientClose != rcRelease)
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
            if (NoError != rc)
            {
                messageList = messageList + tmpMessageList; 
            }
            else
            {
                messageList = tmpMessageList;
            }

            RTE_Message (messageList);
        }

        pPacket = NULL;
        return rcRelease;
    }

    if (NoError != rc)
    {
        RTE_Message (messageList);
        return rc;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::Send    (SAPDB_Byte const * const       pSendData,
                                                 SAPDB_UInt4 const              sendLen,
                                                 SAPDBErr_MessageList &         messageList)
const
{
    RTECKC_ICommunication *     pConnection;
    CommRC                      rc = NoError;
    CommRC                      rcRelease = NoError;
    SAPDBErr_MessageList        tmpMessageList;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::Send", CKC_Trace, 9);

    rc = LeaseConnectionData (&pConnection, messageList);
    if (NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_LEASE_CONNECTION_DATA); 
        RTE_Message (messageList);
        return rc;
    }


    rc = pConnection->Send (pSendData, sendLen, messageList);
    if (NoError != rc)
    {
        if (ClosePending == rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_SEND_DATA, 
                                                             SAPDB_ToString (rc)); 
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_SEND_DATA); 
        }
    }

    rcRelease = ReleaseConnectionData (pConnection, tmpMessageList);

    if (NoError != rcRelease)
    {
        if (ClientClose != rcRelease)
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
            if (NoError != rc)
            {
                messageList = messageList + tmpMessageList; 
            }
            else
            {
                messageList = tmpMessageList;
            }

            RTE_Message (messageList);
        }

        return rcRelease;
    }

    if (NoError != rc)
    {
        RTE_Message (messageList);
        return rc;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::ReplyAvailable (SAPDBErr_MessageList &         messageList)
const
{
    RTECKC_ICommunication *     pConnection;
    SAPDB_Bool                  rc = true;
    CommRC                      commRC = NoError;
    CommRC                      rcRelease = NoError;
        
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::ReplyAvailable", CKC_Trace, 9);

    commRC = LeaseConnectionData (&pConnection, messageList);
    if (NoError != commRC)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_LEASE_CONNECTION_DATA); 
        RTE_Message (messageList);
        return commRC;
    }

    rc = pConnection->ReplyAvailable ();

    rcRelease = ReleaseConnectionData (pConnection, messageList);

    if (NoError != rcRelease)
    {
        if (ClientClose != rcRelease)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
            RTE_Message (messageList);
        }

        return rcRelease;
    }

    if (!rc)
    {
        return NoReplyAvailable;
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::Receive (SAPDB_Byte * &                 pReceiveData,
                                                 SAPDB_UInt4 &                  receiveLen,
                                                 SAPDBErr_MessageList &         messageList)
const
{
    RTECKC_ICommunication *      pConnection;
    CommRC                      rc = NoError;
    CommRC                      rcRelease = NoError;
    SAPDBErr_MessageList        tmpMessageList;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::Receive", CKC_Trace, 9);

    rc = LeaseConnectionData (&pConnection, messageList);
    if (NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_LEASE_CONNECTION_DATA); 
        RTE_Message (messageList);
        return rc;
    }

    rc = pConnection->Receive (pReceiveData, receiveLen, messageList);
    if (NoError != rc)
    {
        if (ClosePending == rc || ServerTimeout == rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_RECEIVE_DATA,
                                                             SAPDB_ToString (rc)); 
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RECEIVE_DATA); 
        }
    }

    rcRelease = ReleaseConnectionData (pConnection, tmpMessageList);

    if (NoError != rcRelease)
    {
        if (ClientClose != rcRelease)
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
            if (NoError != rc)
            {
                messageList = messageList + tmpMessageList; 
            }
            else
            {
                messageList = tmpMessageList;
            }

            RTE_Message (messageList);
        }

        return rcRelease;
    }

    if (NoError != rc)
    {
        RTE_Message (messageList);
        return rc;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::Cancel  (SAPDBErr_MessageList &         messageList)
{
    RTECKC_ICommunication *     pConnection;
    CommRC                      rc = NoError;
    CommRC                      rcRelease = NoError;
    SAPDBErr_MessageList        tmpMessageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::Cancel", CKC_Trace, 9);


    rc = LeaseConnectionData (&pConnection, messageList);
    if (NoError != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_LEASE_CONNECTION_DATA); 
        RTE_Message (messageList);
        return rc;
    }

    rc = pConnection->CancelRequest (messageList);
    if (NoError != rc)
    {
        if (ClosePending == rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CANCEL_REQUEST,
                                                             SAPDB_ToString(rc)); 
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_CANCEL_REQUEST); 
        }
    }

    rcRelease = ReleaseConnectionData (pConnection, tmpMessageList);
    if (NoError != rcRelease)
    {
        if (ClientClose != rcRelease)
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_RELEASE_CONNECTION_DATA); 
            if (NoError != rc)
            {
                messageList = messageList + tmpMessageList; 
            }
            else
            {
                messageList = tmpMessageList;
            }

            RTE_Message (messageList);
        }

        return rcRelease;
    }

    if (NoError != rc)
    {
        RTE_Message (messageList);
        return rc;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::LeaseConnectionData (RTECKC_ICommunication ** const     ppConnection,
                                                             SAPDBErr_MessageList &             messageList)
const
{
    RTE_HandleManager &         handleManager = RTECKC_GlobalClientInstance::Instance().GetHandleManager();
    RTE_HandleManager::HdlRet   rcHdl = RTE_HandleManager::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::LeaseConnectionData", CKC_Trace, 9);

    rcHdl = handleManager.LeaseHandleData (m_HandleConnection, reinterpret_cast <void **>(ppConnection));
    if (RTE_HandleManager::NoError != rcHdl)
    {
        if (RTE_HandleManager::InvalidHandle == rcHdl)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CKC_CONNECTION_IS_NOT_OPENED); 
            return ClientClose;
        }
        else //FreePending
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CKC_CLOSE_PENDING); 
            return ClosePending;
        }
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommunication::CommRC    
            RTECKC_ClientCommunication::ReleaseConnectionData (RTECKC_ICommunication *      pConnection,
                                                               SAPDBErr_MessageList &       messageList)
const
{
    RTE_HandleManager &         handleManager = RTECKC_GlobalClientInstance::Instance().GetHandleManager();
    RTE_HandleManager::HdlRet   rcHdl = RTE_HandleManager::NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunication::ReleaseConnectionData", CKC_Trace, 9);

    rcHdl = handleManager.ReleaseHandleData (m_HandleConnection);
    if (RTE_HandleManager::NoError != rcHdl)
    {
        if (RTE_HandleManager::InvalidHandle == rcHdl)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_REL_HANDLE_DATA); 
            return Error;
        }
        else //HandleFreed
        {
            SAPDBErr_MessageList        tmpMessageList;

            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CKC_CONNECTION_FREED); 
            if (NoError != pConnection->Close (tmpMessageList))
            {
                tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, 
                                                                        RTEERR_CKC_CLOSE_CONNECTION);
                messageList = messageList + tmpMessageList;
                return Error;
            }
            else
            {
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_CKC_CONNECTION_CLOSED); 
                destroy (pConnection, RTEMem_Allocator::Instance()); 
                return ClientClose;
            }
        }
    }

    return NoError;
}

