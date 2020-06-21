/*!========================================================================

  @file         RTETask_LegacyTaskCtrl.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access old task control

                The following class provides methodes to access 
                old task control.

  @since        2005-11-08  13:51
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
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include    "RunTime/Tasking/RTETask_LegacyTaskCtrl.hpp"
#include    "geo50_0.h"
#include    "heo670.h"
#include    "SAPDBCommon/Messages/SDBMsg_RTETask.h"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include    "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

#if defined (_WIN32)
#else
# include    "gen88.h"  /* no check */
#endif


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

RTETask_ITaskClientConnection::ConnectionType   RTETask_LegacyTaskCtrl::m_ConnectTypeByInstance = RTETask_ITaskClientConnection::UnknownConnection;


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTETask_LegacyTaskCtrl*  RTETask_LegacyTaskCtrl::Self()
{
    TASK_CTRL  pTaskCtrl; 

#   if defined (_WIN32)
     PUKT_CTRL_REC  pUKT = THIS_UKT_CTRL;

     if (( pUKT ) && ( pUKT->pCTask ))
         pTaskCtrl = pUKT->pCTask;
     else
         pTaskCtrl = 0; /* called from other type of thread */
#   else
     pTaskCtrl = RTE_GetOwnTask();
#   endif

    return  (0 == pTaskCtrl) ? 0 : pTaskCtrl->pLegacyTaskCtrlObject;
}

/*---------------------------------------------------------------------------*/

RTETask_LegacyTaskCtrl*  RTETask_LegacyTaskCtrl::ByTaskID(  RTE_TaskId taskID )
{
    TASK_CTRL  pTaskCtrl = 0;

#   if defined (_WIN32)
     if ( taskID >= 1 && taskID <= (SAPDB_Long)kgs.ulNumOfTasks )
         pTaskCtrl = &(kgs.pFirstTaskCtrl[taskID - 1]) ;
#   else
     if ( taskID >= 1 && taskID <= kgs->ulNumOfTasks )
         pTaskCtrl = kgs->pFirstTaskCtrl + taskID - 1 ;
#   endif

    return  (0 == pTaskCtrl) ? 0 : pTaskCtrl->pLegacyTaskCtrlObject;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_LegacyTaskCtrl::IsUtilityTaskConnected()
{
#       if defined (_WIN32)
    return  ( kgs.pUT != 0 
           && kgs.pUT->pUserCommCtrl != 0 
           && 0 != kgs.pUT->pUserCommCtrl->ulConnected );
#       else
    return  ( KGS != 0 
           && KGS->ut != 0 
           && KGS->ut->connection != 0 
           && 0 != KGS->ut->connection->ci_connect_time );
#       endif
}


/*---------------------------------------------------------------------------*/


RTETask_ITaskClientConnection::ConnectionType RTETask_LegacyTaskCtrl::ConnectionType() 
const
{
    RTETask_ITaskClientConnection::ConnectionType   connectionType = RTETask_ITaskClientConnection::UnknownConnection;

#ifdef _WIN32
    if (NULL != m_pTaskCtrl->pUserCommCtrl)
    {
        switch (m_pTaskCtrl->pUserCommCtrl->ulServiceType)
#else
    if (NULL != m_pTaskCtrl->connection)
    {
        switch (m_pTaskCtrl->connection->ci_service)
#endif
        {
        case srvPing_esp01:
            connectionType = RTETask_ITaskClientConnection::Ping;
            break;
        case srvUtility_esp01:
            connectionType = RTETask_ITaskClientConnection::Utility;
            break;
        case srvEvent_esp01:
            connectionType = RTETask_ITaskClientConnection::Event;
            break;
        case srvOLTPUser_esp01:
            connectionType = RTETask_ITaskClientConnection::OLTP;
            break;
        case srvLVCUser_esp01:
        case srvUser_esp01:
            /* We determine the connection type only by the instance type */
            if (RTETask_ITaskClientConnection::UnknownConnection == m_ConnectTypeByInstance)
            {
                RTEConf_Parameter::String   paramValue;
                SAPDBErr_MessageList        dummy;

                if (RTEConf_ParameterAccess::Instance()->GetString(UTF8( "INSTANCE_TYPE" ), paramValue, dummy ))
                {/* Get the information from the configuration */
                    if (0 == strcmp( "LVC", (SAPDB_Char *)paramValue))
                    {
                        m_ConnectTypeByInstance = RTETask_ITaskClientConnection::LiveCache;
                    }
                    else
                    {
                        m_ConnectTypeByInstance = RTETask_ITaskClientConnection::OLTP;
                    }
                }
                else
                {
                    m_ConnectTypeByInstance = RTETask_ITaskClientConnection::LiveCache;
                }
            }

            connectionType = m_ConnectTypeByInstance;
            break;

        default:
            connectionType = RTETask_ITaskClientConnection::UnknownConnection;
        }
    }

    return connectionType;
}


/*---------------------------------------------------------------------------*/

externC DLQ_ELEMENT*  GetTaskDLQElement1( TASK_CTRL   pTaskCtrl )
{
    // later on, when the legacy code returns the leased queue elements
    // we should use the free list of the task object
    return pTaskCtrl->pLegacyTaskCtrlObject->CommonQueueElement1();
}

/*---------------------------------------------------------------------------*/

externC DLQ_ELEMENT*  GetTaskDLQElement2( TASK_CTRL   pTaskCtrl )
{
    // later on, when the legacy code returns the leased queue elements
    // we should use the free list of the task object
    return pTaskCtrl->pLegacyTaskCtrlObject->CommonQueueElement2();
}

/*------------------------------*/
#ifdef _WIN32

typedef WINBASEAPI BOOL (WINAPI tIsDbgPresent)( VOID );

static BOOL RTETask_IsDebuggerPresent ()
  { 
#  undef  MF__
#  define MF__ MOD__"os74kIsDebuggerPresent"

   BOOL                  DbgPresent = FALSE;
   tIsDbgPresent *       IsDbgPresent;
   HINSTANCE             Knl32;

   
   Knl32 = LoadLibrary("KERNEL32.DLL");
 
   if ( Knl32 != NULL )
   {
     IsDbgPresent  = (tIsDbgPresent *) GetProcAddress(Knl32, "IsDebuggerPresent");

     if ( IsDbgPresent != NULL )
       DbgPresent = (TRUE == IsDbgPresent());

     FreeLibrary( Knl32 );
   }
   
   return DbgPresent;
  };
#endif

extern "C" void RTETask_ProcessDebugTaskRequest ( UKT_CTRL *ppUKT )
{
    do
    {
#ifdef _WIN32        
        if ( RTETask_IsDebuggerPresent() )
            DebugBreak();
        else
#endif
        {
            RTE_Message(
                Msg_List(
	                Msg_List::Error,
	                SDBMSG_RTETASK_DUMP_CALLSTACK_TASK,
	                Msg_Arg(SDBMSGTAG_RTETASK_DUMP_CALLSTACK_TASK__TASK_ID,(*ppUKT)->pLegacyUKTCtrlObject->CurrentTaskObject()->ID())
                )
            );
//            MSGD(( ERR_DUMP_CALLSTACK_TASK, (*ppUKT)->pLegacyUKTCtrlObject->CurrentTaskObject()->ID() ));
            eo670_CTraceStack();
        }

        RELEASE_FREELIST_ELEM ((*ppUKT)->pLegacyUKTCtrlObject->WakeUPRequest());
        (*ppUKT)->pLegacyUKTCtrlObject->CurrTaskCtrl()->fDebugTask = FALSE;
        __GOTO_DISPATCHER( ppUKT );
    } 
    while ( (*ppUKT)->pLegacyUKTCtrlObject->CurrTaskCtrl()->fDebugTask );

    return;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
