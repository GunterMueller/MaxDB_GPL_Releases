/*!
  @file           RTECons_Communication.cpp
  @author         StefanP
  @special area   Console (XCons)
  @brief          Communication
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include  "gos41.h"     /* nocheck */
#include  <stdio.h>
#else                   /*&else*/
#include  "geo50_0.h"   /* nocheck */
#include  "gen41.h"     /* nocheck */
#endif                  /*&endif*/


#include  "hsp77.h"
#include  "hsp100.h"
#include  "heo15.h"

#include  "RunTime/System/RTESys_Spinlock.h"
#include  "RunTime/RTE_MessageList.hpp"
#include  "RunTime/RTE_Message.hpp"
#include  "RunTime/RTE_XCons_Messages.hpp"
#include  "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include  "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include  "RunTime/RTE_ConsoleCommon.hpp"
#include  "RunTime/Console/RTECons_Communication.hpp"
#include  "RunTime/RTE_ConsoleRequestCommunication.hpp"
#include  "RunTime/RTE_KSS.h"
#include  "RunTime/Console/RTECons_Main.h"



extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  DECLARATIONS                                                             *
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
SAPDB_UInt4     RTECons_Communication::m_ConnectionCounter = 0;
RTE_Lock        RTECons_Communication::m_ConnectionCounterSpinlock; 


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTECons_Communication::RTECons_Communication 
(
    SAPDB_Char const * const    szServerDB,
    SAPDB_UInt4 const           communicationType
) :   RTE_ConsoleRequestCommunication (szServerDB, RTE_CONSOLE_CLIENT_MODE),
      RTE_ConsoleDataCommunication    (RTE_CONSOLE_CLIENT_MODE, GenerateConnectionCounter ()), 
      m_header (&m_request.header)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_Communication::RTECons_Communication", 
                             Console_Trace, 9);

    strcpy (m_ServerDB, szServerDB);
    m_request.data.connectData.commType = communicationType;
    strcpy ((char *)m_request.data.connectData.consoleShmID, RTE_CONSOLE_UNDEF_SHM_ID);
#if defined (_WIN32)
    GETPROCESSID (&m_request.data.connectData.pid);
#else
    m_request.data.connectData.pid = (RTE_OSPid)getpid();
#endif
}

/*---------------------------------------------------------------------------*/

RTECons_Communication::~RTECons_Communication () 
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_Communication::~RTECons_Communication", 
                             Console_Trace, 9);
#if defined (_WIN32)
    if (NULL != m_WorldSA.lpSecurityDescriptor)
    {
        FREE_MEM (m_WorldSA.lpSecurityDescriptor);
    }
#endif
}

/*---------------------------------------------------------------------------*/

RTECons_Communication::CommRC  RTECons_Communication::Initialize 
(
    SAPDBErr_MessageList &              messageList
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/

    RTE_SystemRc                                rc       = RTE_SYSTEMRC_NO_ERROR;
    RTE_FileHandle                              hKSS     = 0;
    tsp00_Versionc                              rteVersionString;
    tsp100_VersionResult                        result;  
    SAPDB_UInt4                                 consoleShmSize;
    SAPDB_UInt4                                 consoleSemTimeout;
    RTE_ConsoleRequestCommunication::RequestRC  rcRequest = RTE_ConsoleRequestCommunication::NoError;


    
   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTECons_Communication::Initialize", 
                             Console_Trace, 1);

    RTE_InitIdGenerator();

    //
    // --- allocate and initialize security attributes
    //
#if defined (_WIN32)
    ACCESS_RIGHTS_REC               Access;


    m_WorldSA.nLength             = sizeof(m_WorldSA);
    m_WorldSA.bInheritHandle      = FALSE;
    memset ( &Access, 0, sizeof (Access) );   // - this must be done first!

    Access.World.ulAccess = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    rc = sql49c_alloc_and_init_SD( &Access, &m_WorldSA.lpSecurityDescriptor );

    if ( rc != RTE_SYSTEMRC_NO_ERROR )
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_SECURITY_DESCRIPTOR, 
                                                          SAPDB_ToString(rc));
        return Error;
    }
#else
    m_WorldSA = 0;
#endif  


    //
    // --- Attaching FIFO
    //
    rcRequest = RTE_ConsoleRequestCommunication::Initialize (NULL, messageList);
    if (RTE_ConsoleRequestCommunication::NoError != rcRequest)
    {
        if (RTE_ConsoleRequestCommunication::Offline == rcRequest)
        {
            return Offline;
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_INIT_REQ_COMM);
            return Error;
        }
    }

    //
    // --- Getting Shared Memory and Semaphore ID's
    //

    if (!RTE_GetUID (m_request.data.connectData.consoleShmID))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_GUID, "shared memory");
        return Error;
    }

#if defined (_WIN32)
    //
    // --- check database version
    //

    KERNEL_SHARED_SEG *     pKSS     = NULL;

    rc = sql41c_get_named_shrd_mem ( (void **)&pKSS, SHM_KSS, m_ServerDB, &hKSS );

    if (ERROR_INVALID_NAME == rc)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_OPEN_KSS, m_ServerDB, SAPDB_ToString (rc));
        return Error;
    }
    else if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_OPEN_KSS, m_ServerDB, SAPDB_ToString (rc));
        return Error;
    }

    // --- set the RTE version string
    result = sp100_CompareVersionIDs (&pKSS->RTEVersionID);

    if ( result != IsEqual_esp100)
    {
        // --- set the RTE version string
        if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
        {
            sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, XConsBuildNumber, rteVersionString);
        }

        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_DB_VERSION, 
                                                          pKSS->RTEVersion, rteVersionString);

        rc = sql41c_free_shrd_mem ( pKSS, hKSS );
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_RELEASE_KSS, m_ServerDB, SAPDB_ToString (rc));
        }
            
        return Error;
    }

    strcpy (m_RTEVersion,    pKSS->RTEVersion);
    strcpy (m_KernelVersion, pKSS->KernelVersion);

    consoleShmSize      = pKSS->consoleShmSize;
    consoleSemTimeout   = pKSS->consoleSemTimeout;

    rc = sql41c_free_shrd_mem ( pKSS, hKSS );
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_RELEASE_KSS, m_ServerDB, SAPDB_ToString (rc));
        return Error;
    }

#else
    //Check for database version on UNIX is still done in ven92.c.
    //KGS is already attached in ven92.c
    strcpy (m_RTEVersion,    KGS->RTEVersion);
    strcpy (m_KernelVersion, KGS->KernelVersion);

    consoleShmSize      = KGS->consoleShmSize;
    consoleSemTimeout   = KGS->consoleSemTimeout;
#endif
    //
    // --- Initialize data communication
    //

    if (!RTE_ConsoleDataCommunication::Initialize (m_ServerDB, 
                                                   &m_WorldSA, 
                                                   m_request.data.connectData.consoleShmID, 
                                                   consoleShmSize,
                                                   consoleSemTimeout,
                                                   messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_INIT_DATA_COMMUNICATION);
        return Error;
    }


    SAPDBTRACE_WRITELN(Console_Trace, 1, "Data communication for "
                                         << m_ServerDB
                                         << " initialized: SHM-ID " 
                                         << SAPDB_ToString (m_request.data.connectData.consoleShmID));

    return NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTECons_Communication::SendRequestData 
(
    SAPDB_UInt4 const               reqRespType,
    SAPDB_Bool const                security,
    SAPDB_Byte const * const        pData,
    SAPDB_UInt4 const               size,
    SAPDBErr_MessageList &          messageList
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    SAPDB_UInt4     requestLen;
    SAPDB_UInt4     myRef;
    SAPDB_UInt4     remoteRef;
    SAPDB_UInt4     rc = RTE_CONS_NO_ERROR;


   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTECons_Communication::SendRequestData", 
                             Console_Trace, 1);

    if (security)
    {
        if (!CheckAccessRight (messageList))
        {
            return RTE_CONS_ERROR;
        }
    }

    if (OPEN_REQ == reqRespType)
    {
        requestLen = sizeof (m_request.data.connectData);
    }
    else
    {
        requestLen = sizeof (m_request.data.hConnect);
    }

    myRef       = GetMyRef ();
    remoteRef   = GetRemoteRef ();

    m_header.Set (reqRespType, myRef, remoteRef, requestLen, 
                  Prot_ConsoleFIFO, RTE_CONSOLE_FIFO_PROTOCOL_VERSION, 
                  RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE);

    SAPDBTRACE_WRITELN(Console_Trace, 7, "Connect request: Type: "  << reqRespType                  <<
                                         ", SenderRef: "    << myRef                                <<
                                         ", ReceiverRef: "  << remoteRef                            <<
                                         ", Data size: "    << requestLen                           <<
                                         ", Total: "        << RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE <<
                                         ", ID: "           << Prot_ConsoleFIFO                     <<
                                         ", Version: "      << RTE_CONSOLE_FIFO_PROTOCOL_VERSION);

    if (!PutRequest (m_request, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_PUT_REQUEST, GetDBName());
        return RTE_CONS_ERROR;
    }

    rc = SendData  (pData, size, reqRespType, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        if (RTE_CONS_REMOTE_TIMEOUT == rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_XCONS_TIMEOUT, GetDBName());
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_DATA, GetDBName());
        }
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_Communication::GenerateConnectionCounter (void)    
{
    SAPDBTRACE_ROUTINE_DEBUG("RTECons_Communication::GenerateConnectionCounter", Console_Trace, 1);

    if (0 == m_ConnectionCounter)
    {
        RTESys_InitLock(&m_ConnectionCounterSpinlock);
    }

    while (RTESys_TestAndLock(&m_ConnectionCounterSpinlock))
    {
        ;
    }

    ++m_ConnectionCounter;

    RTESys_Unlock (&m_ConnectionCounterSpinlock);
    return (m_ConnectionCounter);
}  

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTECons_Communication::CheckAccessRight 
(
    SAPDBErr_MessageList           &    messageList
)
{
    RTE_SystemRc   rc           = RTE_SYSTEMRC_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_Communication::CheckAccessRight", 
                             Console_Trace, 1);

#if defined (_WIN32)
    HEV              hevSemCoord  = 0;


   //
   // --- try to open coordinators semaphore to check access rights
   //
    rc = sql41c_open_event_sem (&hevSemCoord, SEM_COORD, m_ServerDB,
                                ERROR_ACCESS_DENIED);

    if (rc == RTE_SYSTEMRC_NO_ERROR )
    {
        sql41c_close_event_sem ( hevSemCoord, "COORD" );
    }
    else if (rc == ERROR_ACCESS_DENIED)
    {
       messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_ACCESS_DENIED, m_ServerDB);
    }
    else if (rc == ERROR_SEM_NOT_FOUND)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_DB_NOT_STARTED, m_ServerDB);
    }
#endif

    return (RTE_SYSTEMRC_NO_ERROR == rc) ? true : false; 
}
