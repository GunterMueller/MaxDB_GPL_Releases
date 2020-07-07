/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_GlobalClientInstance.cpp
  @author       StefanP
  @brief        Global Client Instance

  @since        2003-11-12  16:00
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
#include    "RunTime/RTE_HandleManager.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_GlobalClientInstance.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ICommunication.hpp"
#include    "RunTime/RTE_UniqueId.h"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"

#if !defined (_WIN32)                                               /*&if $OSSPEC!=WIN32*/
#include "gen22.h"                                                  /*nocheck*/
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"   /*nocheck*/
#endif                                                              /*&endif*/

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
RTECKC_GlobalClientInstance   *RTECKC_GlobalClientInstance::m_Instance = NULL; 

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTECKC_GlobalClientInstance & RTECKC_GlobalClientInstance::Instance()
{
    SAPDBTRACE_ROUTINE_DEBUG("RTECKC_GlobalClientInstance::Instance", CKC_Trace, 9);
    
    if (NULL == m_Instance)
    {
        m_Instance = new (RTEMem_Allocator::Instance()) RTECKC_GlobalClientInstance ();
        if (NULL == m_Instance)
        {
            RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, "RTECKC_GlobalClientInstance"));
        }

        RTE_InitIdGenerator();
    }
   
    return *m_Instance;
}

//--------------------------------------------------------------------------------

RTECKC_GlobalClientInstance::~RTECKC_GlobalClientInstance    ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_GlobalClientInstance::~RTECKC_GlobalClientInstance", CKC_Trace, 9);
    if (!CloseAllConnections (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_ALL_CONNECTIONS);
        RTE_Message (messageList);
    }
}

//--------------------------------------------------------------------------------

SAPDB_Bool  RTECKC_GlobalClientInstance::CloseAllConnections  (SAPDBErr_MessageList &         messageList)
{
    RTE_HandleManager::HdlRet           rcHdl = RTE_HandleManager::NoError;
    RTE_HandleManager::Hdl              handle;
    RTECKC_ICommunication *             pConnection;
    SAPDB_Bool                          tryAgain = true;
    SAPDB_UInt4                         numTrials;
    RTECKC_ClientCommunication::CommRC  rc = RTECKC_ClientCommunication::NoError;
    SAPDB_UInt4                         idx;


    SAPDBTRACE_METHOD_DEBUG ("RTECKC_GlobalClientInstance::Release", CKC_Trace, 9);


    // --- search for open connections
    rcHdl = m_HandleManager.FindFirstHandle (handle);

    while (RTE_HandleManager::NoHandleFound != rcHdl)
    {
        rcHdl = m_HandleManager.LeaseHandleData (handle, reinterpret_cast <void **>(&pConnection));
        if (RTE_HandleManager::NoError != rcHdl)
        {
            break;  //Connection already closed or FreeHandle is already pending
        }

        rc = pConnection->PrepareClose (messageList);
        if (RTECKC_ClientCommunication::NoError != rc && 
            RTECKC_ClientCommunication::ClosePending != rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_PREPARE_CLOSE_CONNECTION);       
            RTE_Message (messageList);
            return false;
        }

        rcHdl = m_HandleManager.ReleaseHandleData (handle);
        if (RTE_HandleManager::NoError != rcHdl)
        {
            if (RTE_HandleManager::InvalidHandle == rcHdl)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_REL_HANDLE_DATA); 
                RTE_Message (messageList);
                return false;
            }
            else//HandleFreed
            {//we are able to close the the connection by ourselves
                if (!pConnection->Close (messageList))
                {
                    messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, 
                                                                     RTEERR_CKC_CLOSE_CONNECTION);
                    RTE_Message (messageList);
                    return false;
                }

                destroy (pConnection, RTEMem_Allocator::Instance()); 
            }
        }
        else //NoError
        {//we can try to free the handle
            rcHdl = m_HandleManager.FreeHandle (handle);
            if (RTE_HandleManager::NoError == rcHdl)
            {   
                if (!pConnection->Close (messageList))
                {
                    messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, 
                                                                     RTEERR_CKC_CLOSE_CONNECTION);
                    RTE_Message (messageList);
                    return false;
                }

                destroy (pConnection, RTEMem_Allocator::Instance()); 
            }
            //else:LeasePending or InvalidHandle: actually not possible
            //We have done everything to ensure that the connection will be closed shortly 
            //by the request which holds actually the connection (LeaseHandleData).
            //We will also monitor that the connection is closed (see below). 
        }

        rcHdl = m_HandleManager.FindNextHandle (handle);
    }

    numTrials = MAX_WAITING_PERIOD_FOR_CONNECTION_CLOSE / CLOSE_REPEAT_INTERVAL;
    for (idx = 0; idx < numTrials; ++idx)
    {
        rcHdl = m_HandleManager.FindFirstPendingFree (handle);

        if (RTE_HandleManager::NoHandleFound == rcHdl)
        {
            break;
        }
#if defined(_WIN32)
    	Sleep(CLOSE_REPEAT_INTERVAL);
#else
	    sleep(CLOSE_REPEAT_INTERVAL/1000);
#endif 
    }

    if (idx == numTrials)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CKC_CLOSE_ALL_CONNECTIONS);
        RTE_Message (messageList);
        return false;
    }

    m_HandleManager.Finish ();

    return true;
}
