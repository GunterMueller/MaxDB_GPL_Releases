/*!========================================================================

  @file         RTECons_StandardSession.cpp
  @ingroup      Console
  @author       StefanP

  @brief        Standard XCons Session

  @since        2006-03-08  11:37
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
#include    "heo00x.h"
#include    "heo92.h"
#include    "geo00_3.h"
#include    "heo47.h"

#include  "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"
#include  "RunTime/RTE_CompilerFeatures.h"
#include  "RunTime/RTE_MessageList.hpp"
#include  "RunTime/RTE_Message.hpp"
#include  "RunTime/RTE_XCons_Messages.hpp"
#include  "RunTime/Console/RTECons_StandardSession.hpp"
#include  "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include  "SAPDBCommon/SAPDB_sprintf.h"
#include  "RunTime/RTE_ConsoleStandardOutput.h"
#include  "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include  "RunTime/Tasking/RTETask_TaskScheduler.hpp"


extern SAPDBTrace_Topic Console_Trace; ///< global console trace object


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! buffer size for test requests */
#define     BYTES_PER_LINE      81

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

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTECons_StandardSession::RTECons_StandardSession 
(
    RTE_ConsoleRequestRecord &         request
) :   m_CommPort (request.serverDB, RTE_CONS_COM_STANDARD),
      m_request  (request),
      m_FirstCall (true)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::RTECons_StandardSession", 
                             Console_Trace, 9);
}

/*---------------------------------------------------------------------------*/

RTECons_Communication::CommRC  
    RTECons_StandardSession::Create 
(
    SAPDBErr_MessageList &              messageList
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::Open", 
                             Console_Trace, 1);

    return (m_CommPort.Initialize (messageList));
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTECons_StandardSession::Connect
(
    SAPDBErr_MessageList &          messageList
) 
{
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;
    RTE_ConsoleStandardConnectPacket    connect;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::Connect", Console_Trace, 1);

    memset (&connect, 0, sizeof (connect));

    rc = Request (OPEN_REQ, 
                        CON_REQUEST(CON_REQ_OPEN),
                        CON_REQUEST_SECURITY(CON_REQ_OPEN),
                        (SAPDB_Byte *)&connect, 
                        0, 
                        sizeof (connect.hConnect), 
                        messageList);

    if (RTE_CONS_NO_ERROR == rc)
    {
        m_CommPort.SetConnectHandle (connect.hConnect);
        SAPDBTRACE_WRITELN (Console_Trace, 5, "Connection to Server DB " 
                                              << m_request.serverDB 
                                              << " established: Connection handle "
                                              << connect.hConnect);
    }
    else
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_CONNECT);
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::RequestToStandardOutput 
(
    SAPDBErr_MessageList &          messageList 
)
{
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    RTE_ConsoleStandardDataPacket   request;
    SAPDB_UInt                      idx;
    SAPDB_UInt4                     command = CON_REQ_UNDEFINED;

//Test----------------------------------------------
    RTE_ConsoleTestDataPacket       longTestRequest;
//--------------------------------------------------

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::RequestToStandardOutput", Console_Trace, 1);

    memset (&request, 0, sizeof (request));

    switch ( m_request.lCommand )
    {
        //***************************************************************************************
        //Test
        //***************************************************************************************
    case COM_TEST01 :
        srand( (unsigned)time( NULL ) );
        for (idx = 1; idx <= 4; ++idx)
        {
            rc = Request (DATA_REQ, 
                CON_REQUEST(CON_REQ_TEST01),
                CON_REQUEST_SECURITY(CON_REQ_TEST01),
                (SAPDB_Byte *)&request, 
                //sizeof (request.data.test01String),
                0,
                sizeof (request.data.test01String), 
                messageList);
            if (RTE_CONS_NO_ERROR == rc)
            {
                rc = TestRequest (request.data.test01String,  CON_REQ_TEST01, messageList);
            }
            else
            {
                break;
            }

            if (idx == 2)
            {
                SAPDB_UInt4 sleeptime;
                sleeptime = (SAPDB_UInt4)(30000 * ((float) rand () / RAND_MAX));
#if defined (_WIN32)
                Sleep (sleeptime);
#else
                sleep ((SAPDB_UInt4)sleeptime/1000);
#endif
            }
        }

        break;

    case COM_TEST02 :
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_TEST02),
            CON_REQUEST_SECURITY(CON_REQ_TEST02),
            (SAPDB_Byte *)&longTestRequest, 
            0,
            sizeof (longTestRequest.data), 
            messageList);
        if (RTE_CONS_NO_ERROR == rc)
        {
            rc = TestRequest (longTestRequest.data,  CON_REQ_TEST02, messageList);
        }
        else
        {
            break;
        }

        break;

    case COM_TEST03 :
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_TEST01),
            CON_REQUEST_SECURITY(CON_REQ_TEST01),
            (SAPDB_Byte *)&request, 
            //sizeof (request.data.test01String),
            0,
            sizeof (request.data.test01String), 
            messageList);
        if (RTE_CONS_NO_ERROR == rc)
        {
            rc = TestRequest (request.data.test01String,  CON_REQ_TEST01, messageList);
        }
        else
        {
            break;
        }
        break;

        //***************************************************************************************

    case COM_TRACE_FLUSH:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_TRACE_FLUSH), 
            CON_REQUEST_SECURITY(CON_REQ_TRACE_FLUSH),
            (SAPDB_Byte *)&request, 
            0,
            0, 
            messageList);
        break;

    case COM_TRACELEVEL:
        if (PARA_TRACE_XCONS == m_request.lParam)
        {
#ifdef SAPDB_SLOW
            Console_Trace.SetTraceLevel (m_request.lValue);
#endif
        }
        else
        {
            request.data.SetTraceLevel.level = (SAPDB_UInt4)m_request.lValue;
            rc = Request (DATA_REQ, 
                CON_REQUEST(CON_REQ_TRACE_CONSOLE), 
                CON_REQUEST_SECURITY(CON_REQ_TRACE_CONSOLE),
                (SAPDB_Byte *)&request, 
                sizeof (request.data.SetTraceLevel),
                0, 
                messageList);
        }
        break;

    case COM_PAGECACHE_RELEASE:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_PAGECACHE_RELEASE), 
            CON_REQUEST_SECURITY(CON_REQ_PAGECACHE_RELEASE),
            (SAPDB_Byte *)&request, 
            0,
            0, 
            messageList);
        break;

    case COM_DIAGNOSE_PAGECACHE:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_DIAGNOSE_PAGECACHE), 
            CON_REQUEST_SECURITY(CON_REQ_DIAGNOSE_PAGECACHE),
            (SAPDB_Byte *)&request, 
            0,
            0, 
            messageList);
        break;
        
    case COM_SET_LB_CHK:
        request.data.SetCheckInterval.CheckIntervalTime = (ULONG)m_request.lParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_SET_CHECK_INTERVAL), 
            CON_REQUEST_SECURITY(CON_REQ_SET_CHECK_INTERVAL),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.SetCheckInterval),
            sizeof (request.data.SetCheckInterval), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.SetCheckInterval.usReturnCode )
            {
            case CON_NO_ERROR:
                break;

            case CON_WRONG_VALUE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_VALUE);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.SetCheckInterval.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_DEBUG_TASK:
        request.data.DebugTask.ulTask   = (ULONG)m_request.lParam;


        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_DEBUG_TASK), 
            CON_REQUEST_SECURITY(CON_REQ_DEBUG_TASK),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.DebugTask),
            sizeof (request.data.DebugTask), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.DebugTask.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_TASK_INACTIVE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_TASK_INACTIVE);
                rc = RTE_CONS_ERROR;
                break;

            case CON_WRONG_TASK:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.DebugTask.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;
    
    case COM_RESET:
        if (m_request.lParam & PARA_IO)
        {
            command = CON_REQ_RES_ALL;
        }
        else if (m_request.lParam & PARA_IO)
        {
            command = CON_REQ_RES_IO;
        }
        else if (m_request.lParam & PARA_REGIONS)
        {
            command = CON_REQ_RES_REGION;
        }
        else if (m_request.lParam & PARA_QUEUE_STAT)
        {
            command = CON_REQ_RES_QUEUE_STAT;
        }
        else if (m_request.lParam & PARA_TASK_COUNTS)
        {
            command = CON_REQ_RES_TASK_COUNTS;
        }
        else if (m_request.lParam & PARA_UKT_SL_STAT)
        {
            command = CON_REQ_RES_UKT_SLEEPTIME_STAT;
        }
        else if (m_request.lParam & PARA_SUSPENDS)
        {
            command = CON_REQ_RES_SUSPENDS;
        }

        if (CON_REQ_UNDEFINED != command)
        {
            rc = Request (DATA_REQ, 
                CON_REQUEST(command), 
                CON_REQUEST_SECURITY(command),
                (SAPDB_Byte *)&request, 
                0,
                0, 
                messageList);
        }
        break;

    case COM_TIME_MEASURE:
        if (m_request.lParam & PARA_ENABLE)
        {
            command = CON_REQ_ENABLE_TIME_MEASURE;
        }

        if (m_request.lParam & PARA_DISABLE)
        {
            command = CON_REQ_DISABLE_TIME_MEASURE;
        }

        if (CON_REQ_UNDEFINED != command)
        {
            rc = Request (DATA_REQ, 
                CON_REQUEST(command), 
                CON_REQUEST_SECURITY(command),
                (SAPDB_Byte *)&request, 
                0,
                0, 
                messageList);
        }
        break;

#if defined (_WIN32)
    case COM_CANCEL:
        request.data.CancelTaskCtrl.usReturnCode  = CON_NO_ERROR;
        request.data.CancelTaskCtrl.ulTask        = (ULONG)m_request.lParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_CANCEL), 
            CON_REQUEST_SECURITY(CON_REQ_CANCEL),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.CancelTaskCtrl),
            sizeof (request.data.CancelTaskCtrl), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            //eigentlich mit der neuen Technik nicht mehr notwendig: Message sollte schon im console thread
            //in die Messageliste geschrieben werden und mit entsprechenden return code im Datenheader
            //mit SendDateError verschickt werden!!! Die Messageausgabe erfolgt dann bereits schon
            //in RTECons_StandardRequest
            switch (request.data.CancelTaskCtrl.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_TASK_INACTIVE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_TASK_INACTIVE);
                rc = RTE_CONS_ERROR;
                break;

            case CON_WRONG_TASK:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK);
                rc = RTE_CONS_ERROR;
                break;

            case CON_WRONG_TASK_TYPE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK_TYPE);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.CancelTaskCtrl.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_UKT_AFFINITY :
        request.data.UKTAffinity.ulUKTIndex = (ULONG)m_request.lParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_SET_UKT_AFFINITY), 
            CON_REQUEST_SECURITY(CON_REQ_SET_UKT_AFFINITY),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.UKTAffinity),
            sizeof (request.data.UKTAffinity), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.UKTAffinity.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_WRONG_UKT:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_UKT);
                rc = RTE_CONS_ERROR;
                break;

            case CON_ERROR:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_SET_UKT_AFFINITY);
                rc = RTE_CONS_ERROR;
                break;

            case CON_NO_PROCESSOR:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_AFFIN_NO_PROCESSOR);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.UKTAffinity.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_PROCESSOR_MASK :
        request.data.SetProcessorMask.ProcessAffinityMask   = (ULONG)m_request.lParam;
        request.data.SetProcessorMask.TaskType              = (UINT1 )m_request.lExtParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_SET_PROCESSOR_MASK), 
            CON_REQUEST_SECURITY(CON_REQ_SET_PROCESSOR_MASK),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.SetProcessorMask),
            sizeof (request.data.SetProcessorMask), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.SetProcessorMask.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_ERROR:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_SET_UKT_AFFINITY);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.SetProcessorMask.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_DEBUG_BREAK:
        request.data.DebugBreak.ulBreakPosition   = (ULONG)m_request.lValue;

        if (PARA_CLEAR == m_request.lParam)
        {
            request.data.DebugBreak.fClearPosition = TRUE;
        }
        else
        {
            request.data.DebugBreak.fClearPosition = FALSE;
        }

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_DEBUG_BREAK), 
            CON_REQUEST_SECURITY(CON_REQ_DEBUG_BREAK),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.DebugBreak),
            sizeof (request.data.DebugBreak), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.DebugBreak.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_NO_INTERACT_WITH_DECTOP:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_INTERACT_WTH_DECTOP);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.DebugBreak.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_KILL:
        request.data.KillTaskCtrl.ulTask = (ULONG)m_request.lParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_KILL), 
            CON_REQUEST_SECURITY(CON_REQ_KILL),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.KillTaskCtrl),
            sizeof (request.data.KillTaskCtrl), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            switch (request.data.KillTaskCtrl.usReturnCode)
            {
            case CON_NO_ERROR:
                break;

            case CON_TASK_INACTIVE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_TASK_INACTIVE);
                rc = RTE_CONS_ERROR;
                break;

            case CON_WRONG_TASK:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK);
                rc = RTE_CONS_ERROR;
                break;

            case CON_WRONG_TASK_TYPE:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK_TYPE);
                rc = RTE_CONS_ERROR;
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_RET_CODE,
                    SAPDB_ToString (request.data.KillTaskCtrl.usReturnCode));
                rc = RTE_CONS_ERROR;
            }
        }
        break;

    case COM_CRASH_DUMP:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_CRASH_DUMP), 
            CON_REQUEST_SECURITY(CON_REQ_CRASH_DUMP),
            (SAPDB_Byte *)&request, 
            0,
            0, 
            messageList);
        break;

    case COM_MINI_DUMP:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_MINI_DUMP), 
            CON_REQUEST_SECURITY(CON_REQ_MINI_DUMP),
            (SAPDB_Byte *)&request, 
            0,
            0, 
            messageList);
        break;
        
    case COM_DROP:
        break;

    case COM_GETPARAM:
        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_GETPARAM), 
            CON_REQUEST_SECURITY(CON_REQ_GETPARAM),
            (SAPDB_Byte *)&request, 
            0,
            sizeof (request.data.GetParamInfo), 
            messageList);

        if (RTE_CONS_NO_ERROR == rc)
        {
            e92_print_param (&request.data.GetParamInfo.XParam, m_request.pszParam);
        }
        break;

    case COM_ERRIOR:
    case COM_ERRIOW:
        request.data.ForceIOError.ulDevno       = (ULONG)m_request.lParam;
        request.data.ForceIOError.fErrOnRead    = (m_request.lCommand == COM_ERRIOR);

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_FORCE_IO_ERROR), 
            CON_REQUEST_SECURITY(CON_REQ_FORCE_IO_ERROR),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.ForceIOError),
            0, 
            messageList);
        break ;

    case COM_SET_DEBUGLEVEL:
        request.data.SetDebugLevelCtrl.ulDebugLevel = (ULONG)m_request.lParam;

        rc = Request (DATA_REQ, 
            CON_REQUEST(CON_REQ_SET_DEBUGLEVEL), 
            CON_REQUEST_SECURITY(CON_REQ_SET_DEBUGLEVEL),
            (SAPDB_Byte *)&request, 
            sizeof (request.data.SetDebugLevelCtrl),
            0, 
            messageList);
        break;

#endif

#if defined (_WIN32)
    case COM_SHOW_UPDATE:
        printf ( XCONS_SERVERDB, m_request.serverDB);

        while (m_request.lRepeatCount)
        {
            if (m_request.lParam & PARA_VERSIONS )
            {
                printf (XCONS_SHOW_VERS_KERNEL, m_CommPort.GetKernelVersion ());
                printf (XCONS_SHOW_VERS_RTE   , m_CommPort.GetRTEVersion ());
            }

            if (m_request.lParam & PARA_IO )
            {
                rc = ShowIOInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_AIO )
            {
                rc = ShowAIOInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_REGIONS )
            {
                rc = ShowRegionInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_RTE )
            {
                rc = ShowRTEInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_STORAGE )
            {
                rc = ShowStorageInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_TASKS  ||
                m_request.lParam & PARA_ACTIVE ||
                m_request.lParam & PARA_RUNNABLE)
            {
                if ( m_request.lParam & PARA_TASKS )
                    request.data.TaskInfo.StateToShow = SHOW_ALL_EO92;
                else if ( m_request.lParam & PARA_ACTIVE )
                    request.data.TaskInfo.StateToShow = SHOW_ACTIVE_EO92;
                else
                    request.data.TaskInfo.StateToShow = SHOW_RUNNABLE_EO92;

                /* PTS 1106166 */ 
                request.data.TaskInfo.TypeToShow  = 
                    m_request.lParam == PARA_ALL ? SHOW_TT_ALL_EO92 : (UINT1 )m_request.lExtParam;

                rc = ShowTaskInfo (request, messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_UKT_SL_STAT)
            {
                rc = ShowUKTSlInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_THREAD_TIMES)
            {
                rc = ShowThreadTimes (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_SUSPENDS)
            {
                rc = Request (DATA_REQ, 
                    CON_REQUEST(CON_REQ_SHOW_SUSPENDS), 
                    CON_REQUEST_SECURITY(CON_REQ_SHOW_SUSPENDS),
                    (SAPDB_Byte *)&request, 
                    0,
                    sizeof (request.data.SuspendReasons), 
                    messageList);

                if (RTE_CONS_NO_ERROR == rc)
                {
                    ShowSuspends (request.data.SuspendReasons);
                }
                else
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_UKT_QUEUES)
            {
                rc = ShowUKTQueues (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_STATE)
            {
                rc = Request (DATA_REQ, 
                    CON_REQUEST(CON_REQ_STATE), 
                    CON_REQUEST_SECURITY(CON_REQ_STATE),
                    (SAPDB_Byte *)&request, 
                    0,
                    sizeof (request.data.StateInfo), 
                    messageList);

                if (RTE_CONS_NO_ERROR == rc)
                {
                    ShowStateInfo (request.data.StateInfo);
                }
                else
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_UKT_QUEUES)
            {
                rc = ShowQueueInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_TASK_COUNTS)
            {
                request.data.TaskCountsInfo.lNumOfActiveTasks = (LONG)m_request.lExtParam;
                rc = ShowTaskCountInfo (request, messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_TOTAL_COUNTS)
            {
                rc = Request (DATA_REQ, 
                    CON_REQUEST(CON_REQ_TOTAL_COUNTS), 
                    CON_REQUEST_SECURITY(CON_REQ_TOTAL_COUNTS),
                    (SAPDB_Byte *)&request, 
                    0,
                    sizeof (request.data.TotalCountsInfo), 
                    messageList);

                if (RTE_CONS_NO_ERROR == rc)
                {
                    ShowTotalCountInfo (request.data.TotalCountsInfo);
                }
                else
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_AWE)
            {
                rc = ShowAWEInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }
            if (m_request.lParam & PARA_CONNECTIONS)
            {
                rc = ShowConnectionsInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }
#else
    case COM_SHOW_UPDATE:
        while (m_request.lRepeatCount)
        {
#endif
            if ( m_request.lParam & PARA_TASK_MOVE )
            {
                rc = ShowTaskMoveInfo (request, messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_MOVE_INFO)
            {
                rc = ShowMoveInfo (messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lParam & PARA_RWREGIONS)
            {
                if( (PARA_ALL == m_request.lParam) || (0 != m_request.lExtParam) )
                {
                    request.data.RWRegionInfo.info.showAll = true;
                }
                else
                {
                    request.data.RWRegionInfo.info.showAll = false;
                }
                rc = ShowRWRegions (request,messageList);

                if (RTE_CONS_NO_ERROR != rc)
                {
                    break;
                }
            }

            if (m_request.lRepeatCount != -1) // --- don't loop forever?
            {
                m_request.lRepeatCount--;
            }

            if ((m_request.lInterval) && (m_request.lRepeatCount))
            {
#              if defined (_WIN32)
                SLEEP((DWORD)(m_request.lInterval * 1000));
#              else
                sleep( m_request.lInterval );
#              endif
            }
        }

        break;

    default:
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_REQUEST,
                                                          SAPDB_ToString (m_request.lCommand));
        return RTE_CONS_ERROR;
    }

    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_REQUEST_PROCESSING,
                                                          SAPDB_ToString (m_request.lCommand));
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4   RTECons_StandardSession::Release   
(
    SAPDBErr_MessageList &      messageList
)
{
    RTE_ConsoleStandardDataHeader       header;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::Release", Console_Trace, 1);

    memset (&header, 0, sizeof (header));

    rc = Request (CLOSE_REQ, 
                        CON_REQUEST(CON_REQ_CLOSE),
                        CON_REQUEST_SECURITY(CON_REQ_CLOSE),
                        (SAPDB_Byte *)&header, 
                        0, 
                        0, 
                        messageList);

    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_DISCONNECT);
    }

    if (RTE_CONS_NO_ERROR != (rc = m_CommPort.SignalConfirmation (messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SIGNAL_CONFIRMATION); 
        return rc;
    }


    SAPDBTRACE_WRITELN (Console_Trace, 5, "Connection to Server DB " 
                                          << m_request.serverDB
                                          << " released");
    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTECons_StandardSession::GetError 
(
    SAPDB_UInt1 const                     expectedType,
    SAPDB_UInt4                           messageLen,
    SAPDBErr_MessageList &                messageList
) 
{
    SAPDB_Byte          *   pDataBuffer;
    SAPDB_UInt4             bytesRead;
    SAPDB_UInt4             bytesTotal;
    SAPDBErr_MessageList    tmpMessageList;
    teo200_EventList        eventList;
    teo200_EventList        tmpEventList;
    SAPDB_UInt4             rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::GetError", Console_Trace, 1);
    
    pDataBuffer = (SAPDB_Byte *)(RTEMem_RteAllocator::Instance().Allocate (messageLen));
    if (NULL == pDataBuffer)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_EVENT, 
                                                         SAPDB_ToString(messageLen));       
        return RTE_CONS_ERROR;
    }

    if (RTE_CONS_NO_ERROR != (rc = m_CommPort.GetDataContinue (pDataBuffer, expectedType, 
                                                               messageLen, bytesRead, bytesTotal, 
                                                               messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_PROTOCOL_ERROR); 
        RTEMem_RteAllocator::Instance().Deallocate(pDataBuffer);
        return rc;
    }

    if (!eventList.eo200_LoadEventList (pDataBuffer, bytesRead, &tmpEventList))
    {
        SAPDBErr_ConvertEventToMessageList (tmpMessageList, tmpEventList);
        messageList = messageList + tmpMessageList;
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_LOAD_EVENT);
        RTEMem_RteAllocator::Instance().Deallocate(pDataBuffer);
        return RTE_CONS_ERROR;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pDataBuffer);
    SAPDBErr_ConvertEventToMessageList (tmpMessageList, eventList);
    messageList = messageList + tmpMessageList;
    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::SendRequest 
(
    SAPDB_UInt4 const                       reqRespType,
    SAPDB_UInt4 const                       commandType,
    SAPDB_Bool const                        security,
    SAPDB_Byte * const                      pDataPacket,
    SAPDB_UInt4 const                       dataLen,
    SAPDBErr_MessageList &                  messageList
)
{
    RTE_ConsoleStandardDataHeader      *pHeader = (RTE_ConsoleStandardDataHeader *)pDataPacket;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::SendRequest", Console_Trace, 1);

    SAPDBTRACE_WRITELN (Console_Trace, 5, "Connection to Server DB "    << m_request.serverDB
                                              << ", connection handle " << m_CommPort.GetConnectHandle ()
                                              << ": request type "      << reqRespType
                                              << ", command type "      << commandType
                                              << ", bytes to send "     << dataLen);
    memset (pHeader, 0, sizeof (*pHeader));
    pHeader->type     = commandType;
    pHeader->dataLen  = dataLen;
    pHeader->lastSend  = true;

    return (m_CommPort.SendRequestData (reqRespType, security, pDataPacket, sizeof (*pHeader) + dataLen, 
                                                               messageList));
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::GetFirstReplyHeader 
(
    RTE_ConsoleStandardDataHeader &         header,
    SAPDB_UInt4 const                       expectedType,
    SAPDB_UInt4 &                           bytesTotal,
    SAPDBErr_MessageList &                  messageList
)
{
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::GetFirstReplyHeader", Console_Trace, 1);

    SAPDBTRACE_WRITELN (Console_Trace, 5, "Get header from Server DB "  << m_request.serverDB
                                              << ", connection handle " << m_CommPort.GetConnectHandle ()
                                              << ": expected type "     << expectedType);
    
    memset (&header, 0, sizeof (header));

    if (RTE_CONS_NO_ERROR != (rc = m_CommPort.GetData ((SAPDB_Byte *)(&header), expectedType, sizeof (header), 
                                                        bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_PROTOCOL_ERROR); 
        return rc;
    }

    if (bytesRead != sizeof (header))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_DATA_READ,
                                                         m_request.serverDB, 
                                                         SAPDB_ToString (bytesRead), 
                                                         SAPDB_ToString (sizeof (header)));
        return RTE_CONS_ERROR;
    }


    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::GetNextReplyHeader 
(
    RTE_ConsoleStandardDataHeader &         header,
    SAPDB_UInt4 const                       expectedType,
    SAPDB_UInt4 &                           bytesTotal,
    SAPDBErr_MessageList &                  messageList
)
{
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::GetNextReplyHeader", Console_Trace, 1);

    SAPDBTRACE_WRITELN (Console_Trace, 5, "Get header from Server DB "  << m_request.serverDB
                                              << ", connection handle " << m_CommPort.GetConnectHandle ()
                                              << ": expected type "     << expectedType);
    
    memset (&header, 0, sizeof (header));

    if (RTE_CONS_NO_ERROR != (rc = m_CommPort.GetDataNext ((SAPDB_Byte *)(&header), expectedType, sizeof (header), 
                              bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_PROTOCOL_ERROR); 
        return rc;
    }

    if (bytesRead != sizeof (header))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_DATA_READ,
                                                         m_request.serverDB, 
                                                         SAPDB_ToString (bytesRead), 
                                                         SAPDB_ToString (sizeof (header)));
        return RTE_CONS_ERROR;
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::GetReplyData 
(
    RTE_ConsoleStandardDataHeader &                     header,
    SAPDB_UInt4 const                                   expectedType,
    SAPDB_UInt4 const                                   dataLen,
    SAPDB_Byte * const                                  pData,
    SAPDBErr_MessageList &                              messageList
)
{
    SAPDB_UInt4                         bytesRead = 0;
    SAPDB_UInt4                         bytesTotal = 0;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::GetReplyData", Console_Trace, 1);

    SAPDBTRACE_WRITELN (Console_Trace, 5, "Get reply data from Server DB "  << m_request.serverDB
                                              << ", connection handle "     << m_CommPort.GetConnectHandle ()
                                              << ": expected type "         << expectedType
                                              << ", bytes to read "         << dataLen);

    if (RTE_CONS_NO_ERROR != header.rc)
    {
        if (0 != header.dataLen)
        {
            if (RTE_CONS_NO_ERROR != (rc = GetError (expectedType, header.dataLen, messageList)))
            {
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_GET_ERROR,
                                                                  m_request.serverDB);
                if (RTE_CONS_TIMEOUT == rc)
                {
                    return rc;
                }
            }
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_XCONS_GET_DATA,
                                                              m_request.serverDB);
        }
        
        return header.rc;
    }
    else
    {
        if (dataLen != header.dataLen)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_DATA_LEN,
                                                             m_request.serverDB, 
                                                             SAPDB_ToString (dataLen), 
                                                             SAPDB_ToString (header.dataLen));
            return RTE_CONS_FATAL_ERROR;
        }

        if (0 != dataLen)
        {
            memset (pData, 0, dataLen);
            
            if (RTE_CONS_NO_ERROR != (rc = m_CommPort.GetDataContinue (pData, expectedType, dataLen,
                                             bytesRead, bytesTotal, messageList)))
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_PROTOCOL_ERROR); 
                return rc;
            }

            if (bytesRead != bytesTotal - sizeof (header))
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_DATA_READ,
                                                                 m_request.serverDB, 
                                                                 SAPDB_ToString (bytesRead), 
                                                                 SAPDB_ToString (bytesTotal));
                return RTE_CONS_ERROR;
            }
        }
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::Request 
(
    SAPDB_UInt4 const                       reqRespType,
    SAPDB_UInt4 const                       commandType,
    SAPDB_Bool const                        security,
    SAPDB_Byte * const                      pDataPacket,
    SAPDB_UInt4 const                       dataLenOut,
    SAPDB_UInt4 const                       dataLenIn,
    SAPDBErr_MessageList &                  messageList
)
{
    SAPDB_UInt4                         bytesTotal;
    RTE_ConsoleStandardDataHeader      *pHeader = (RTE_ConsoleStandardDataHeader *)pDataPacket;
    SAPDB_Byte                         *pData = NULL;
    SAPDB_UInt4                         expectedType;
    SAPDB_UInt4                         rc;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::Request", Console_Trace, 1);

    SAPDBTRACE_WRITELN (Console_Trace, 5, "Connection to Server DB "    << m_request.serverDB
                                              << ", connection handle " << m_CommPort.GetConnectHandle ()
                                              << ": request type "      << reqRespType
                                              << ", command type "      << commandType
                                              << ", length out "        << dataLenOut
                                              << ", length in "         << dataLenIn);
     
    rc = SendRequest (reqRespType, commandType, security, pDataPacket, dataLenOut, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (commandType),
                                                         m_request.serverDB); 
        return rc;
    }

    switch (reqRespType)
    {
    case OPEN_REQ:
        expectedType = OPEN_REP;
        break;

    case DATA_REQ:
        expectedType = DATA_REP;
        break;

    case CLOSE_REQ:
        expectedType = CLOSE_REP;
        break;

    default:
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_UNKNOWN_REQUEST_TYPE,
                                                         SAPDB_ToString (reqRespType)); 
        return RTE_CONS_FATAL_ERROR;
    }

    rc = GetFirstReplyHeader (*pHeader, expectedType, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (commandType),
                                                         m_request.serverDB);  
        return rc;
    }

    if (0 != dataLenIn)
    {
        pData = (SAPDB_Byte *)(pHeader + 1);
    }

    rc = GetReplyData (*pHeader, expectedType, dataLenIn, pData, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        return rc;
    }


    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::TestRequest 
(
    SAPDB_Byte *                    testString,
    SAPDB_UInt4 const               commandType,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_Char              buffer[BYTES_PER_LINE];
    SAPDB_ULong             bytesToWrite = 0; 
    SAPDB_ULong             numBytes = 0; 


 
    if (commandType == CON_REQ_TEST01)
    {
        numBytes = strlen ((char *)testString);
    }
    else //CON_REQ_TEST02
    {
        numBytes = sizeof (TestLongString);
    }
    
    while (0 != numBytes)
    {
        memset (buffer, 0, sizeof (buffer));
        testString += bytesToWrite;
        bytesToWrite = min (BYTES_PER_LINE - 1, numBytes);
        SAPDB_MemCopyNoCheck (buffer, testString, bytesToWrite);
        
        printf ("%s\n", buffer);
        numBytes -= bytesToWrite;  
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

#if defined (_WIN32)


SAPDB_UInt4     RTECons_StandardSession::ShowIOInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_ULong                     ulDevspaceNo;
    RTE_Path                        szPathName;
    SAPDB_UInt8                     ulReadCnt  = 0;
    SAPDB_UInt8                     ulReadPageCnt  = 0;
    SAPDB_UInt8                     ulWriteCnt = 0;
    SAPDB_UInt8                     ulWritePageCnt = 0;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowIOInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_IO), 
                      CON_REQUEST_SECURITY(CON_REQ_IO), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_IO),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_IO),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_IO));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_IO_INFO_REC   pIOInfo         =   (PCONS_IO_INFO_REC)pData;
        PCONS_DEV_INFO_REC  pDevspaceInfos  =   (PCONS_DEV_INFO_REC)(pData + pIOInfo->ulDevspaceInfoOffset);
        printf (XCONS_SHOW_IO_HEADER);

        for (idx = 0; idx < (SAPDB_UInt4)pIOInfo->lNumOfDevspaces; idx++)
        {
            ulDevspaceNo = pDevspaceInfos[idx].ulDevspaceNo;

            // --- The compress value (44) must be identical to the value
            //     defined in 'XCONS_SHOW_IO_INFO'!!!!!!!!!!!
            eo92_compress_path( szPathName, pDevspaceInfos[idx].DevspaceName, 44 );

            printf (XCONS_SHOW_IO_INFO,
                szPathName,
                ulDevspaceNo,
                pDevspaceInfos[idx].ulReadCnt,
                pDevspaceInfos[idx].ulReadPageCnt,
                pDevspaceInfos[idx].ulWriteCnt,
                pDevspaceInfos[idx].ulWritePageCnt);

            ulReadCnt  += pDevspaceInfos[idx].ulReadCnt;
            ulReadPageCnt  += pDevspaceInfos[idx].ulReadPageCnt;
            ulWriteCnt += pDevspaceInfos[idx].ulWriteCnt;
            ulWritePageCnt += pDevspaceInfos[idx].ulWritePageCnt;
        }

        printf (XCONS_SHOW_IO_INFO_TOTAL,
            XCONS_SHOW_IO_INFO_TOTAL_STR,
            ulReadCnt,
            ulReadPageCnt,
            ulWriteCnt,
            ulWritePageCnt);

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowAIOInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_Char                      szTid[12];
    RTE_Path                        szPathName;
    SAPDB_Char                      szMode[2];
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowAIOInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_AIO), 
                      CON_REQUEST_SECURITY(CON_REQ_AIO), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);   
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_AIO),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_AIO),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_AIO));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_AIO_INFO_REC              pAIOInfo     =   (PCONS_AIO_INFO_REC)pData;
        printf (XCONS_SHOW_ASYNCIO_HEADER);
        if (pAIOInfo->lNumOfASYNCis)
        {
            PCONS_ASYNCI_INFO_REC           pASYNCiInfos =   (PCONS_ASYNCI_INFO_REC)(pData + pAIOInfo->ulASYNCiInfoOffset);

            for ( idx = 0; idx < (SAPDB_UInt4)pAIOInfo->lNumOfASYNCis; idx++ )
            {
                // --- The compress value (20) must be identical to the value
                //     defined in 'XCONS_SHOW_ASYNCIO_DEV_INFO'!!!!!!!!!!!
                eo92_compress_path ( szPathName,
                    pASYNCiInfos[idx].szLogDeviceName,
                    20 );

                szTid[0] = '0';
                szTid[1] = 'x';
                _ultoa ( pASYNCiInfos[idx].Tid, szTid + 2, 16 );
                strupr ( szTid + 2 );

                if ( pASYNCiInfos[idx].fForWriting )
                {
                    szMode[0] = 'W';
                    szMode[1] = '\0';
                }
                else
                {
                    szMode[0] = 'R';
                    szMode[1] = '\0';
                }

                printf ( XCONS_SHOW_ASYNCIO_INFO,
                    szPathName,
                    szTid,
                    szMode,
                    pASYNCiInfos[idx].ulMaxBlockCnt,
                    pASYNCiInfos[idx].ulBlockSize,
                    pASYNCiInfos[idx].DevIOStat.DevRead.IOStat.TimeStat.ulCount,
                    pASYNCiInfos[idx].DevIOStat.DevWrite.IOStat.TimeStat.ulCount,
                    pASYNCiInfos[idx].DevIOStat.ulIOQueLen,
                    pASYNCiInfos[idx].DevIOStat.ulMaxIOQueLen );
            }
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowRegionInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_UInt4                     ix;
    SAPDB_ULong *                   pulWaitTasks;
    SAPDB_Real4                     flPercent;
    SAPDB_Char                      szOwnerTaskIndex[20];
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowRegionInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_REGION), 
                      CON_REQUEST_SECURITY(CON_REQ_REGION), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);   
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_REGION),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_REGION),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_REGION));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_SEM_INFO_REC              pSemInfo        =   (PCONS_SEM_INFO_REC)pData;
        PCONS_SINGLE_SEM_INFO_REC       pSingleSemInfos =   (PCONS_SINGLE_SEM_INFO_REC)(pData + pSemInfo->ulSingleSemInfoOffset);
        printf (XCONS_SHOW_REGION_HEADER);

        for ( idx = 0; idx < (SAPDB_UInt4)pSemInfo->lNumOfSems; idx ++ )
        {
            pulWaitTasks = (SAPDB_ULong *)(pData + pSingleSemInfos[idx].ulWaitTaskOffset);

            if (( ! pSingleSemInfos[idx].lGetCount ) ||
                ( ! pSingleSemInfos[idx].lCollisionCount ))
                flPercent = 0.0f;
            else
                flPercent = (SAPDB_Real4) pSingleSemInfos[idx].lCollisionCount * 100.0f /
                            (SAPDB_Real4) pSingleSemInfos[idx].lGetCount;


            if (pSingleSemInfos[idx].ulOwnerTaskIndex)
            {
                szOwnerTaskIndex[0] = 'T';
                _ultoa ((ULONG)pSingleSemInfos[idx].ulOwnerTaskIndex, szOwnerTaskIndex + 1, 10 );
            }
            else
                szOwnerTaskIndex[0] = '\0';

            printf ( XCONS_SHOW_REGION_INFO,
                pSingleSemInfos[idx].ulSid,
                pSingleSemInfos[idx].szSemaName,
                szOwnerTaskIndex,
                pSingleSemInfos[idx].lGetCount,
                pSingleSemInfos[idx].lBegExclTasCount +
                pSingleSemInfos[idx].lEndExclTasCount,
                pSingleSemInfos[idx].lCollisionCount,
                pSingleSemInfos[idx].lWaitCount,
                pSingleSemInfos[idx].lExclusive,
                flPercent );

            if ( pSingleSemInfos[idx].lNumWaitTasks)
            {
                printf ( XCONS_SHOW_REGION_WAIT_HD );

                for ( ix = 0; ix < pSingleSemInfos[idx].lNumWaitTasks; ix++ )
                {
                    if ( !(ix % TASKS_INFO_P_LINE) && ix)
                        printf (XCONS_SHOW_REGION_WAIT_ILF,
                                pulWaitTasks[ix]);
                    else
                        printf (XCONS_SHOW_REGION_WAIT_I,
                                pulWaitTasks[ix]);
                }
                printf ("\n");
            }

        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
SAPDB_Char *    RTECons_StandardSession::KTStateName 
( 
    SAPDB_ULong const     ulState 
)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::KTStateName", Console_Trace, 9);

    switch ( ulState )
    {
    case KT_INITIALIZED:    return ( XCONS_KT_STATE_INITIALIZED );
    case KT_RUNNING    :    return ( XCONS_KT_STATE_RUNNING     );
    case KT_TERMINATING:    return ( XCONS_KT_STATE_TERMINATING );
    case KT_TERMINATED :    return ( XCONS_KT_STATE_TERMINATED  );
    case KT_SLEEPING   :    return ( XCONS_KT_STATE_SLEEPING    );
    case KT_DETACHED   :    return ( XCONS_KT_STATE_DETACHED    );
    case KT_ATTACHED   :    return ( XCONS_KT_STATE_ATTACHED    );
    }

  return ( XCONS_KT_STATE_UNKNOWN );
}

SAPDB_UInt4     RTECons_StandardSession::ShowRTEInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulDevspaceNo;
    SAPDB_UInt4                     i;
    SAPDB_UInt4                     j;
    SAPDB_Bool                      displayRelationship = false;
    SAPDB_Char                      szTid[12];
    RTE_Path                        szPathName;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowRTEInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_RTE), 
                      CON_REQUEST_SECURITY(CON_REQ_RTE), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);   
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_RTE),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_RTE),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_RTE));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_RTE_INFO_REC          pRTEInfo        = (PCONS_RTE_INFO_REC)pData;
        PCONS_DEV_WORKER_INFO_REC   pDevWorkerInfos = (PCONS_DEV_WORKER_INFO_REC)(pData + pRTEInfo->ulDevWorkerInfoOffset);
        PCONS_UKT_INFO_REC          pUKTInfos       = (PCONS_UKT_INFO_REC) (pData + pRTEInfo->ulUKTInfoOffset);
        PCONS_DEVI_INFO_REC         pDEViInfos      = (PCONS_DEVI_INFO_REC) (pData + pRTEInfo->ulDEViInfoOffset);

        // --- show kernel thread info
        printf ( XCONS_SHOW_RTE_KT_HEADER );

        //
        // --- COORDINATOR
        //
        if ( pRTEInfo->CoordTid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pRTEInfo->CoordTid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_COORD, szTid,
            KTStateName(pRTEInfo->CoordState));

        //
        // --- TIMER
        //
        if (pRTEInfo->TimerTid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa (pRTEInfo->TimerTid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_TIMER, szTid,
            KTStateName(pRTEInfo->TimerState ) );

        //
        // --- CLOCK
        //
        if (pRTEInfo->ClockTid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa (pRTEInfo->ClockTid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_CLOCK, szTid,
            KTStateName(pRTEInfo->ClockState ) );

        //
        // --- DEV0
        //
        if (pRTEInfo->Dev0Tid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pRTEInfo->Dev0Tid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_DEV0, szTid,
            KTStateName( pRTEInfo->Dev0State ) );

        //
        // --- ASYNC0
        //
        if ( pRTEInfo->Async0Tid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pRTEInfo->Async0Tid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_ASYNC0, szTid,
            KTStateName( pRTEInfo->Async0State ) );

        //
        // --- CONSOLE
        //
        if ( pRTEInfo->ConsoleTid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pRTEInfo->ConsoleTid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_CONSOLE, szTid,
            KTStateName( pRTEInfo->ConsoleState ) );

        //
        // --- REQUESTOR
        //
        if ( pRTEInfo->RequestorTid != (TID)UNDEF)
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pRTEInfo->RequestorTid, szTid + 2, 16 );
            strupr ( szTid + 2 );
        }
        else
            szTid[0] = '\0';

        printf ( XCONS_SHOW_RTE_KT_INFO, XCONS_SHOW_RTE_KT_REQUESTOR, szTid,
            KTStateName( pRTEInfo->RequestorState ) );

        if ( pRTEInfo->lNumOfDevWorkers )
        {
            printf ( XCONS_SHOW_RTE_DEV_WORKER_HEADER );

            for ( i = 0; i < (SAPDB_UInt4)pRTEInfo->lNumOfDevWorkers; i++ )
            {
                szTid[0] = '0';
                szTid[1] = 'x';
                _ultoa ( pDevWorkerInfos[i].Tid, szTid + 2, 16 );
                strupr ( szTid + 2 );
                printf ( XCONS_SHOW_RTE_DEV_WORKER_INFO,
                    i,
                    szTid,
                    (char *)(SAPDB_ToString ((RTETask_TaskScheduler::TaskSchedulerState)(pDevWorkerInfos[i].ThrdState))),
                    pDevWorkerInfos[i].ulIOCount );
            }
            printf ( XCONS_SHOW_RTE_CONCURR_DEV_WORKER,
                pRTEInfo->ulNumOfConcurrentDevWorkers );
        }

        printf ( XCONS_SHOW_RTE_UKT_HEADER );

        for ( i = 0; i < (SAPDB_UInt4)pRTEInfo->lNumOfUKTs; i++ )
        {
            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pUKTInfos[i].Tid, szTid + 2, 16 );
            strupr ( szTid + 2 );

            printf ( XCONS_SHOW_RTE_UKT_INFO,
                pUKTInfos[i].ulUKTIndex,
                szTid,
                (char *)(SAPDB_ToString ((RTETask_TaskScheduler::TaskSchedulerState)(pUKTInfos[i].ThrdState))),
                pUKTInfos[i].dDispatcherCalls,
                pUKTInfos[i].dTaskSwitchCalls,
                pUKTInfos[i].ulActiveTasks,
                pUKTInfos[i].ulTotalTasks,
                pUKTInfos[i].szCluster );


            if ( pUKTInfos[i].CPURelationship != UKT_CPU_RELATIONSHIP_NONE )
                displayRelationship = TRUE;
        }

        
        printf ( XCONS_SHOW_RTE_UKT_CPU_HEADER );
        printf ( XCONS_SHOW_RTE_UKT_CPU_INFO, 
                 pRTEInfo->numLogProcessors, 
                 pRTEInfo->numPhysProcessors );

        if ( displayRelationship )
        {
            char         Hd1[64 + 1];
            char         Hd2[64 + 1];
            SAPDB_UInt4  cutoff;
            SAPDB_UInt4  numCpu = pRTEInfo->numLogProcessors;


            if (( numCpu < 20 ) && ( numCpu != 10 ))
            {
                strcpy ( Hd1, "1  4   8   12  16" );
                strcpy ( Hd2, "|  |   |   |   |" );
            }
            else
            {
                strcpy ( Hd1, "1        10        20        30        40        50        60" );
                strcpy ( Hd2, "|        |         |         |         |         |         |" );
            }

            cutoff = (numCpu % 10) && (numCpu != 16) && (numCpu != 12) ? numCpu : numCpu + 1;
            Hd1[cutoff] = '\0';
            Hd2[cutoff] = '\0';

            printf ( XCONS_SHOW_RTE_UKT_REL_HEADER );
            printf ( XCONS_SHOW_RTE_UKT_REL_MASK_HD1, Hd1 );
            printf ( XCONS_SHOW_RTE_UKT_REL_MASK_HD2, Hd2 );
            
            for ( i = 0; i < (SAPDB_UInt4)pRTEInfo->lNumOfUKTs; i++ )
            {
                if ( pUKTInfos[i].CPURelationship != UKT_CPU_RELATIONSHIP_NONE )
                {
                    SAPDB_ULong   ulProcessor = pUKTInfos[i].ulProcessor;
                    char          Mask[(sizeof(ulProcessor)*8) + 1];
                    SAPDB_ULong   idx;

                    for ( idx = 0; idx < numCpu; ulProcessor >>= 1, idx++ )
                        Mask[idx] = ulProcessor & (SAPDB_ULong)0x1 ? '1' : '0';

                    Mask[idx] = '\0'; 

                    if ( pUKTInfos[i].CPURelationship == UKT_CPU_RELATIONSHIP_AFFINITY )
                        printf ( XCONS_SHOW_RTE_UKT_REL_MASK_INFO, pUKTInfos[i].ulUKTIndex, Mask, 'A' );
                    else
                        printf ( XCONS_SHOW_RTE_UKT_REL_MASK_INFO, pUKTInfos[i].ulUKTIndex, Mask, 'I' );
                }
            }
        }

        printf ( XCONS_SHOW_RTE_IO_HEADER );

        for ( i = 0; i < (SAPDB_UInt4)pRTEInfo->lNumOfUKTs; i++ )
        {
            PCONS_DEV_INFO_REC  pDevspaceInfos = (PCONS_DEV_INFO_REC) (pData + (pUKTInfos +i)->ulDevspaceInfoOffset);

            for ( j = 0; j < (SAPDB_UInt4)pUKTInfos[i].lNumOfDevspaces; j++ )
            {
                ulDevspaceNo = pDevspaceInfos[j].ulDevspaceNo;

                // --- The compress value (20) must be identical to the value
                //     defined in 'XCONS_SHOW_RTE_IO_UKT_INFO'!!!!!!!!!!!
                eo92_compress_path( szPathName, pDevspaceInfos[j].DevspaceName, 20 );

                szTid[0] = '0';
                szTid[1] = 'x';
                _ultoa ( pUKTInfos[i].Tid, szTid + 2, 16 );
                strupr ( szTid + 2 );

                printf ( XCONS_SHOW_RTE_IO_UKT_INFO,
                    pUKTInfos[i].ulUKTIndex,
                    szTid,
                    szPathName,
                    ulDevspaceNo,
                    pDevspaceInfos[j].ulReadCnt,
                    pDevspaceInfos[j].ulWriteCnt );
            }
        }

        for ( i = 0; i < (SAPDB_UInt4)pRTEInfo->lNumOfDEVis; i++ )
        {
            ulDevspaceNo = pDEViInfos[i].ulDevspaceNo;

            // --- The compress value (20) must be identical to the value
            //     defined in 'XCONS_SHOW_RTE_IO_DEV_INFO'!!!!!!!!!!!
            eo92_compress_path ( szPathName, pDEViInfos[i].DevspaceName, 20 );

            szTid[0] = '0';
            szTid[1] = 'x';
            _ultoa ( pDEViInfos[i].Tid, szTid + 2, 16 );
            strupr ( szTid + 2 );

            printf ( XCONS_SHOW_RTE_IO_DEV_INFO,
                pDEViInfos[i].ulDEViIndex,
                szTid,
                szPathName,
                ulDevspaceNo,
                pDEViInfos[i].DevIOStat.DevRead.IOStat.TimeStat.ulCount,
                pDEViInfos[i].DevIOStat.DevWrite.IOStat.TimeStat.ulCount,
                pDEViInfos[i].DevIOStat.ulIOQueLen,
                pDEViInfos[i].DevIOStat.ulMaxIOQueLen );
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowStorageInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowStorageInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_STORAGE), 
                      CON_REQUEST_SECURITY(CON_REQ_STORAGE), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_STORAGE),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_STORAGE),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_STORAGE));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_STORAGE_INFO_REC       pStorageInfo    =   (PCONS_STORAGE_INFO_REC)pData;
        PCONS_TASK_STORAGE_INFO_REC  pTaskStorInfos  =   (PCONS_TASK_STORAGE_INFO_REC)(pData + pStorageInfo->ulTaskStorInfoOffset);
        

        printf ( XCONS_SHOW_STOR_ALLOC_STCK, pStorageInfo->lTaskStacks / 1024 );
        printf ( XCONS_SHOW_STOR_KGS, pStorageInfo->ulKGSSize / 1024 );
        printf ( XCONS_SHOW_STOR_KSS, pStorageInfo->ulKSSSize / 1024 );

        printf ( XCONS_SHOW_STOR_TSK_HEADER );

        for ( idx = 0; idx < (SAPDB_UInt4)pStorageInfo->lNumOfTasks; idx++)
        {
            printf ( XCONS_SHOW_STOR_TSK_INFO,
                pTaskStorInfos[idx].ulTaskIndex,
                pTaskStorInfos[idx].displayName,
                (float)pTaskStorInfos[idx].ulStackUsed / 1024.0,
                (float)pTaskStorInfos[idx].ulStackFree / 1024.0 );
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowTaskInfo 
( 
    RTE_ConsoleStandardDataPacket & request,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     i;
    SAPDB_Char                      szWaitingIndex[20];
    SAPDB_Char                      szWaitingItem[20];
    SAPDB_Char                      szApplPid[14];
    SAPDB_Char                      szTimeOut[20];
    SAPDB_Char                      szTid[12];
    SAPDB_ULong                     ulUnconnectedTasks   = 0;
    SAPDB_ULong                     ulUnusedServerTasks  = 0;
    SAPDB_ULong                     ulTimeOut            = 0;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowTaskInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_TASK), 
                      CON_REQUEST_SECURITY(CON_REQ_TASK), 
                      (SAPDB_Byte *)&request, 
                      sizeof (request.data.TaskInfo), 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_TASK),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (request.header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_TASK),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = request.header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_TASK));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (request.header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_TASK_INFO_REC         pTaskInfo   = (PCONS_TASK_INFO_REC)pData;
        PCONS_SINGLE_TASK_INFO_REC  pTaskInfos  = (PCONS_SINGLE_TASK_INFO_REC)(pData + pTaskInfo->ulTaskInfoOffset);

        if ( m_FirstCall || SHOW_TT_ALL_EO92 == request.data.TaskInfo.TypeToShow)
        {
            printf ( XCONS_SHOW_TASK_HEADER );
            m_FirstCall = false;
        }
        else
            printf ("\n" ) ;

        if ( pTaskInfo->lNumOfActiveTasks == 0 )
            return ( NO_ERROR );

        for ( i = 0; i < (SAPDB_UInt4)pTaskInfo->lNumOfActiveTasks; i++)
        {
            if ( pTaskInfos[i].ApplPid != RTE_UNDEF_OSPID)
            {
                sql47_ltoa ( (long)pTaskInfos[i].ApplPid,
                    szApplPid, 10 );
                if ( pTaskInfos[i].szClientNode[0] != '\0' )
                {
                    strcat(szApplPid, "*");
                }
            }
            else
                szApplPid[0] = '\0';

            /* PTS 1115172 */
            szWaitingIndex[0] = '\0';
            if ( pTaskInfos[i].TaskType == TT_US_EO00 
                && ( pTaskInfos[i].TaskState == TSK_VSUSPEND_EO00 || pTaskInfos[i].TaskState == TSK_VWAIT_EO00 ) )
            {
                if ( 0 < pTaskInfos[i].lWaitForTask )
                {
                    _ltoa ( (LONG)pTaskInfos[i].lWaitForTask, szWaitingIndex, 10 );
                }
            }
            else if ( pTaskInfos[i].TaskState == TSK_IO_READ_EO00 || pTaskInfos[i].TaskState == TSK_IO_WRITE_EO00 ) 
            {
                _ltoa ( (LONG)pTaskInfos[i].lWaitForDevno, szWaitingIndex, 10 );
            }
            else if ( pTaskInfos[i].TaskState == TSK_ENTERRWREGION_EO00 ) 
            {
                SAPDB_sprintf(szWaitingIndex,sizeof(szWaitingIndex),"(%c)",
                    pTaskInfos[i].WaitingForExclusiveAccess ? 'w' : 'r');
            }
            else if ( pTaskInfos[i].TaskState == TSK_YIELDING_ON_SPINLOCK_EO00 )
            {
                strncpy(&szWaitingIndex[0], &pTaskInfos[i].NameOfSpinlockYieldingFor[0], sizeof(szWaitingIndex)-1);
                szWaitingIndex[sizeof(szWaitingIndex)-1] = 0;
            }
            else
            {
                if ( 0 < pTaskInfos[i].Sid )
                {
                    _ultoa ( pTaskInfos[i].Sid, szWaitingIndex, 10 );
                }
            }
            szWaitingItem[0] = 0;
            if ( (pTaskInfos[i].TaskType == TT_US_EO00 || pTaskInfos[i].TaskType == TT_SV_EO00 || pTaskInfos[i].TaskType == TT_GC_EO00) // PTS 1130342
                && pTaskInfos[i].TaskState == TSK_VSUSPEND_EO00) /* PTS 1115172 */
            {
                if ( pTaskInfos[i].LockedPageNo >= 0 && pTaskInfos[i].LockedPageNo != MAX_INT4_SP00 )
                {
                    _ultoa( pTaskInfos[i].LockedPageNo, szWaitingItem, 10 );
                }
                else if ( pTaskInfos[i].lWaitForRoot >= 0 && pTaskInfos[i].lWaitForRoot != MAX_INT4_SP00 )
                {
                    _ltoa( (LONG)pTaskInfos[i].lWaitForRoot, szWaitingItem, 10 );
                }
            }
            else if ( pTaskInfos[i].TaskState == TSK_ENTERRWREGION_EO00 ) 
            {
                SAPDB_sprintf(szWaitingItem,sizeof(szWaitingItem),"%d",
                    pTaskInfos[i].Sid);
            }
            else
            {
                if ( pTaskInfos[i].WaitTime > 0 )
                {
                    _ltoa( (LONG)pTaskInfos[i].WaitTime, szWaitingItem, 10 );
                }
            }

            if ( pTaskInfos[i].TaskState == TSK_VRECEIVE_EO00 )
            {
                if ( pTaskInfos[i].ulTaskTimeOut == (tsp00_Longuint) UNDEF )
                    strncpy (szTimeOut, XCONS_SHOW_TASK_NO_TIMEOUT, 10);
                else if (pTaskInfos[i].ulTaskTimeOut < (tsp00_Longuint)time(NULL))
                    ulTimeOut = 0;
                else
                    ulTimeOut = (ULONG)(pTaskInfos[i].ulTaskTimeOut - time (NULL));

                if ( pTaskInfos[i].ulTaskTimeOut != (tsp00_Longuint) UNDEF )
                    _ultoa ( (ULONG)ulTimeOut, szTimeOut, 10 );
                else
                    // strncpy (szTimeOut, XCONS_SHOW_TASK_NO_TIMEOUT, 10);

                    szTimeOut[10] = '\0';
            }
            else
            {
                _ultoa ( (ULONG)pTaskInfos[i].ulDynPrio, szTimeOut, 10 );
            }

            if (( pTaskInfos[i].TaskState == TSK_INITIALIZED_EO00         ||
                pTaskInfos[i].TaskState == TSK_INACTIVE_EO00            ||
                pTaskInfos[i].TaskState == TSK_MARKED_FOR_STARTUP_EO00  ||
                pTaskInfos[i].TaskState == TSK_CONNECTWAIT_EO00       ) &&
                pTaskInfos[i].TaskType  != TT_UT_EO00 )
            {
                if ( pTaskInfos[i].TaskType == TT_SV_EO00 )
                    ulUnusedServerTasks++;
                else
                    ulUnconnectedTasks++;
            }
            else
            {
                szTid[0] = '0';
                szTid[1] = 'x';
                _ultoa ( pTaskInfos[i].Tid, szTid + 2, 16 );
                strupr ( szTid + 2 );

                printf ( XCONS_SHOW_TASK_INFO,
                         pTaskInfos[i].ulTaskIndex,
                         pTaskInfos[i].ulUktIndex,
                         szTid,
                         pTaskInfos[i].displayName,
                         szApplPid,
                         pTaskInfos[i].QueLocation == TASK_IS_IN_RUN_QUEUE_EO00 ? '*' : ' ' ,
                         eo92GetTaskStateName (pTaskInfos[i].TaskState,(SAPDB_Int4)pTaskInfos[i].ulStateReason) ,
                         ! pTaskInfos[i].OmsCallbackActive ? ' ' : '&', /* CR 1106969 */
                         ! pTaskInfos[i].CancelFlag ? ' ' : '!',        /* CR 1103476 */
                         ! pTaskInfos[i].PrioFlag   ? ' ' :
                         '0' + pTaskInfos[i].PrioFlag
                         + pTaskInfos[i].SavePrioFlag ,
                         szTimeOut,
                         pTaskInfos[i].ulBegExclCnt,
                         szWaitingIndex,
                         szWaitingItem,
                         pTaskInfos[i].UktSleepCount,
                         pTaskInfos[i].UktSleepingFlag ? 's' : 'r' );
            }
        }
        if ( ulUnusedServerTasks )
        {
            printf ( XCONS_SHOW_SERVER_UNUSED, ulUnusedServerTasks );
        }

        if ( ulUnconnectedTasks )
        {
            printf ( XCONS_SHOW_TASK_CONN_INFO,
                ulUnconnectedTasks, XCONS_TSK_STATE_CONNECTWAIT );
        }


        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
SAPDB_UInt4     RTECons_StandardSession::ShowUKTSlInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_Char                      IdleTimeBuf[20];             /* ddd:hh:mm:ss: */
    SAPDB_Char                      IOWaitTimeBuf[20];           /* ddd:hh:mm:ss: */
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowUKTSlInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_UKT_SLEEPTIME_STAT), 
                      CON_REQUEST_SECURITY(CON_REQ_UKT_SLEEPTIME_STAT), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_UKT_SLEEPTIME_STAT),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_UKT_SLEEPTIME_STAT),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_UKT_SLEEPTIME_STAT));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_UKT_SLEEPTIME_REC pSleepInfo = (PCONS_UKT_SLEEPTIME_REC)pData;

        if ( pSleepInfo->lNumOfActiveUKTs > 0 )
        {
            PCONS_SINGLE_UKT_SL_STAT_INFO_REC   pSleepStatInfo = (PCONS_SINGLE_UKT_SL_STAT_INFO_REC)(pData + pSleepInfo->ulSleepStatInfoOffset);        

            printf ( XCONS_SHOW_UKT_SLEEP_STAT_HEADER ) ;

            for ( idx = 0; idx < (SAPDB_UInt4)pSleepInfo->lNumOfActiveUKTs; idx++)
            {
                /* CR 1000097 */
                eo92_SecToDDHHMMSS( (ULONG)pSleepStatInfo[idx].UKTIdleStat.ulAbsMicroSeconds/1000000, IdleTimeBuf, sizeof(IdleTimeBuf) ) ;

                eo92_SecToDDHHMMSS( (ULONG)pSleepStatInfo[idx].UKTIOWaitStat.ulAbsMicroSeconds/1000000, IOWaitTimeBuf, sizeof(IOWaitTimeBuf) ) ; 

                printf( XCONS_SHOW_UKT_SLEEP_STAT_INFO ,
                    pSleepStatInfo[idx].ulUKTIndex ,
                    pSleepStatInfo[idx].UKTIdleStat.ulCount,
                    IdleTimeBuf,
                    ((double) (tsp00_Int8)pSleepStatInfo[idx].UKTIdleStat.ulAbsMicroSeconds) * 100. /
                    (double) (tsp00_Int8)pSleepStatInfo[idx].measuredMicroSeconds,
                    pSleepStatInfo[idx].UKTIOWaitStat.ulCount,
                    IOWaitTimeBuf,
                    ((double) (tsp00_Int8)pSleepStatInfo[idx].UKTIOWaitStat.ulAbsMicroSeconds) * 100. /
                    (double) (tsp00_Int8)pSleepStatInfo[idx].measuredMicroSeconds,
                    pSleepStatInfo[idx].ulRUNQueueLen );
            }
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTECons_StandardSession::ConvThreadTime 
( 
    FILETIME        *pThreadTime ,
    XC_THREAD_TIMES *pTime ,
    XC_THREAD_TIMES *pSumTime 
)
const
{
    #define MAX_SECS_PER_LOW_WORD ((SAPDB_Real8 )429.5)

    SAPDB_Real8 flHigh = (SAPDB_Real8) ( pThreadTime->dwHighDateTime) ;
    SAPDB_Real8 flLow  = (SAPDB_Real8) ( pThreadTime->dwLowDateTime ) ;
    SAPDB_Real8 flTotalSec  ;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ConvThreadTime", Console_Trace, 9);

    flTotalSec = flHigh * MAX_SECS_PER_LOW_WORD + flLow / 10000000.0 ;

    pTime->ulDays = ( ULONG ) floor ( flTotalSec / 3600.0 / 24.0 );
    flTotalSec   -= (SAPDB_Real8)(pTime->ulDays) * 3600.0 * 24.0 ;

    pTime->ulHr   = ( ULONG ) floor ( flTotalSec / 3600.0 );
    flTotalSec   -= (SAPDB_Real8)(pTime->ulHr) * 3600.0 ;

    pTime->ulMin = ( ULONG ) floor ( flTotalSec / 60.0 );
    flTotalSec   -= (SAPDB_Real8)(pTime->ulMin) * 60.0 ;

    pTime->ulSec  = ( ULONG ) floor ( flTotalSec  );
    pTime->ulMsec = ( pThreadTime->dwLowDateTime % 10000000 ) / 10000 ;

    pSumTime->ulMsec  += pTime->ulMsec ;
    pSumTime->ulSec   += pTime->ulSec + (pSumTime->ulMsec / 1000) ;
    pSumTime->ulMsec  %= 1000 ;
    pSumTime->ulMin   += pTime->ulMin + (pSumTime->ulSec  /   60) ;
    pSumTime->ulSec   %= 60 ;
    pSumTime->ulHr    += pTime->ulHr  + (pSumTime->ulMin  /   60) ;
    pSumTime->ulMin   %= 60 ;
    pSumTime->ulDays  += pTime->ulDays+ (pSumTime->ulHr   /   24) ;
    pSumTime->ulHr    %= 24 ;
    return ;
}

SAPDB_UInt4     RTECons_StandardSession::ShowThreadTimes 
( 
    SAPDBErr_MessageList &          messageList
)
{

    SAPDB_UInt4                     actThrdIndex ;
    XC_THREAD_TIMES                 SysTime, UsrTime ;
    XC_THREAD_TIMES                 TotSysTime, TotUsrTime ;
    SAPDB_Char                      szSysDays[5], szUsrDays[5] ;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowThreadTimes", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_THREAD_TIMES), 
                      CON_REQUEST_SECURITY(CON_REQ_THREAD_TIMES), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_THREAD_TIMES),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_THREAD_TIMES),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_THREAD_TIMES));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_THREAD_TIMES_INFO_REC         pThreadInfo      = (PCONS_THREAD_TIMES_INFO_REC)pData;
        PCONS_SINGLE_THREAD_TIMES_INFO_REC  pThreadTimesInfo = (PCONS_SINGLE_THREAD_TIMES_INFO_REC)
            (pData + pThreadInfo->ulThreadTimesOffset);        

        memset ( &SysTime   , 0, sizeof (SysTime) ) ;
        memset ( &UsrTime   , 0, sizeof (UsrTime) ) ;
        memset ( &TotSysTime, 0, sizeof (TotSysTime) );
        memset ( &TotUsrTime, 0, sizeof (TotUsrTime) );

        printf ( XCONS_SHOW_THREAD_TIMES_HEADER );

        for ( actThrdIndex = 0; actThrdIndex < (SAPDB_UInt4)pThreadInfo->lNumOfUKTs; 
              actThrdIndex++, pThreadTimesInfo++ )
        {
            ConvThreadTime ( &pThreadTimesInfo->SystemTime, &SysTime , &TotSysTime ) ;

            ConvThreadTime ( &pThreadTimesInfo->UserTime,   &UsrTime , &TotUsrTime ) ;

            printf ( XCONS_FORMAT_SHOW_THREAD_TIMES ,
                pThreadTimesInfo->szThreadName,
                pThreadTimesInfo->Tid,
                pThreadTimesInfo->szCluster, 
                SysTime.ulDays ? _ultoa ( SysTime.ulDays, szSysDays, 10 ) : "",
                SysTime.ulHr, SysTime.ulMin, SysTime.ulSec, SysTime.ulMsec ,
                UsrTime.ulDays ? _ultoa ( UsrTime.ulDays, szUsrDays, 10 ) : "",
                UsrTime.ulHr, UsrTime.ulMin, UsrTime.ulSec, UsrTime.ulMsec ) ; 
        }

        printf ( XCONS_FORMAT_SHOW_THREAD_TOT_TIMES ,
            TotSysTime.ulDays ? _ultoa ( TotSysTime.ulDays, szSysDays, 10 ) : "",
            TotSysTime.ulHr, TotSysTime.ulMin, TotSysTime.ulSec, TotSysTime.ulMsec ,
            TotUsrTime.ulDays ? _ultoa ( TotUsrTime.ulDays, szSysDays, 10 ) : "",
            TotUsrTime.ulHr, TotUsrTime.ulMin, TotUsrTime.ulSec, TotUsrTime.ulMsec ) ; 


        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTECons_StandardSession::ShowSuspends 
(
    CONS_SHOW_SUSPEND_REC &  suspReasons
)
const
{
    SAPDB_UInt4             idx;
    SAPDB_ULong             ulTotalSuspends = 0 ;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowSuspends", Console_Trace, 1);

    printf ( XCONS_SHOW_SUSPEND_HEADER );

    for (idx = 0; idx < MAX_SUSPEND_REASONS ; idx++ )
        ulTotalSuspends += suspReasons.ulSuspends [idx] ;

    printf ( XCONS_SHOW_SUSPEND_TOTAL , ulTotalSuspends ) ;

    for (idx = 0; idx < MAX_SUSPEND_REASONS; idx++)
    {
        if ( suspReasons.ulSuspends [idx] )
        {
            printf( XCONS_SHOW_SUSPEND_CNT_PERCENT,
                SuspendReasonText[idx].szShortReason ,
                suspReasons.ulSuspends [idx] ,
                (float ) suspReasons.ulSuspends [idx] * 100.0 /
                (float ) ulTotalSuspends ,
                SuspendReasonText[idx].szLongReason) ;
        }
    }
}

/*---------------------------------------------------------------------------*/

void    RTECons_StandardSession::ShowUKTQueue 
( 
    SAPDB_ULong const           ulUKTQueueLen,
    PCONS_SINGLE_QUEUE_INFO_REC pUKTUKTQueue
)
const
{
    SAPDB_ULong ulQueuElem;
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowUKTQueue", Console_Trace, 9);

    if ( ulUKTQueueLen > 0 )
    { 
        printf ( XCONS_SHOW_UKT_QUEUE_HEAD ) ;
        for ( ulQueuElem = 0;
            ulQueuElem < ulUKTQueueLen ;
            ulQueuElem++, pUKTUKTQueue++ )
        {
            if ( pUKTUKTQueue->PrioFlag )
            {
                printf( XCONS_SHOW_UKT_QUEUE_PRIO_ELEMENT,
                pUKTUKTQueue->ulTaskIndex ,
                pUKTUKTQueue->PrioFlag ) ;
            }
            else
            {
                printf( XCONS_SHOW_UKT_QUEUE_ELEMENT,
                pUKTUKTQueue->ulTaskIndex ) ;
            }
        }
    }
}


void    RTECons_StandardSession::ShowCOMQueue 
( 
    SAPDB_ULong const           ulCOMQueueLen,
    PCONS_SINGLE_QUEUE_INFO_REC pUKTCOMQueue
)
const
{
    SAPDB_ULong ulQueuElem;
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowCOMQueue", Console_Trace, 9);

    if ( ulCOMQueueLen > 0 )
    { 
        printf ( XCONS_SHOW_COM_QUEUE_HEAD ) ;
        for ( ulQueuElem = 0;
            ulQueuElem < ulCOMQueueLen ;
            ulQueuElem++, pUKTCOMQueue++ )
        {
            if ( pUKTCOMQueue->PrioFlag )
            {
                printf( XCONS_SHOW_COM_QUEUE_PRIO_ELEMENT,
                pUKTCOMQueue->ulTaskIndex ,
                pUKTCOMQueue->PrioFlag ) ;
            }
            else
            {
                printf( XCONS_SHOW_COM_QUEUE_ELEMENT,
                pUKTCOMQueue->ulTaskIndex ) ;
            }
        }
    }
}

SAPDB_Char *  RTECons_StandardSession::ReqTypeName 
( 
    SAPDB_ULong const     ulReqType 
)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ReqTypeName", Console_Trace, 9);

    switch ( ulReqType )
    {
    case REQ_VATTACH                 : return ( XCONS_REQ_VATTACH ) ;
    case REQ_VBLOCKIO                : return ( XCONS_REQ_VBLOCKIO ) ;
    case REQ_VDETACH                 : return ( XCONS_REQ_VDETACH ) ;
    case REQ_INITOK                  : return ( XCONS_REQ_INITOK ) ;
    case REQ_VRESUME                 : return ( XCONS_REQ_VRESUME ) ;
    case REQ_VSIGNAL                 : return ( XCONS_REQ_VSIGNAL ) ;
    case REQ_CONNECT                 : return ( XCONS_REQ_CONNECT );
    case REQ_VRECEIVE                : return ( XCONS_REQ_VRECEIVE) ;
    case REQ_SHUTDOK                 : return ( XCONS_REQ_SHUTDOK ) ;
    case REQ_SENDER_JOB_FREE         : return ( XCONS_REQ_SENDER_JOB_FREE) ;
    case REQ_VENDEXCL                : return ( XCONS_REQ_VENDEXCL ) ;
    case REQ_RESCHEDULE              : return ( XCONS_REQ_RESCHEDULE ) ;
    case REQ_YIELDING_EO00           : return ( XCONS_REQ_YIELDING ) ;
    case REQ_SHUTDOWN_RESUME_TW      : return ( XCONS_REQ_SHUTDOWN_RESUME_TW);
    case REQ_ASYNOPEN                : return ( XCONS_REQ_ASYNOPEN ) ;
    case REQ_ASYNCLOSE               : return ( XCONS_REQ_ASYNCLOSE ) ;
    case REQ_ASYNIO                  : return ( XCONS_REQ_ASYNIO ) ;
    case REQ_ASYNWAIT                : return ( XCONS_REQ_ASYNWAIT ) ;
    case REQ_VNSHUTDOWN_REPLY        : return ( XCONS_REQ_VNSHUTDOWN_REPLY) ;
    case REQ_OUTGOING_CONNECT        : return ( XCONS_REQ_OUTGOING_CONNECT) ;
    case REQ_OUTGOING_CONNECT_REPLY  : return ( XCONS_REQ_OUTGOING_CONNECT_REPLY) ;
    case REQ_OPEN_WAKEUP_SD          : return ( XCONS_REQ_OPEN_WAKEUP_SD ) ;
    case REQ_CONNECT_RC              : return ( XCONS_REQ_CONNECT_RC ) ;
    case REQ_RELEASE_RC              : return ( XCONS_REQ_RELEASE_RC ) ;
    case REQ_CLOSE_OUTGOING          : return ( XCONS_REQ_CLOSE_OUTGOING ) ;
    case REQ_CLOSE_OUTGOING_REPLY    : return ( XCONS_REQ_CLOSE_OUTGOING_REPLY) ;
    case REQ_FINISH_SV               : return ( XCONS_REQ_FINISH_SV ) ;
    case REQ_ASYNCNTL                : return ( XCONS_REQ_ASYNCNTL ) ;
    }

    return ( XCONS_REQ_TYPE_UNKNOWN );
}

void    RTECons_StandardSession::ShowRUNQueue 
( 
    SAPDB_ULong const           ulRUNQueueLen,
    PCONS_SINGLE_QUEUE_INFO_REC pUKTRUNQueue
)
const
{
    SAPDB_ULong ulQueuElem;
    SAPDB_Char  szFromTask[10];

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowRUNQueue", Console_Trace, 1);

    if ( ulRUNQueueLen > 0 )
    { 
        printf ( XCONS_SHOW_RUN_QUEUE_HEAD ) ;
        for ( ulQueuElem = 0;
            ulQueuElem < ulRUNQueueLen ;
            ulQueuElem++, pUKTRUNQueue++ )
        {   
            if ( pUKTRUNQueue->ulFromTask != UNDEF )
            {
                SAPDB_sprintf (szFromTask, sizeof (szFromTask), 
                               "(T%d)", pUKTRUNQueue->ulFromTask);
            }
            else
            {
                szFromTask[0] = '\0' ;
            }

            if ( pUKTRUNQueue->ulReqType != UNDEF )
            {
                printf( XCONS_SHOW_RUN_QUEUE_ELEMENT_REQ,
                    pUKTRUNQueue->ulTaskIndex ,
                    eo92GetTaskStateName ( pUKTRUNQueue->TaskState,0 ),
                    pUKTRUNQueue->ulDynPrio ,
                    ReqTypeName ( pUKTRUNQueue->ulReqType ),
                    szFromTask) ;
            }
            else
            {
                printf( XCONS_SHOW_RUN_QUEUE_ELEMENT,
                    pUKTRUNQueue->ulTaskIndex ,
                    eo92GetTaskStateName ( pUKTRUNQueue->TaskState,0 ),
                    pUKTRUNQueue->ulDynPrio , szFromTask) ;
            }
        }
    }
}


SAPDB_UInt4     RTECons_StandardSession::ShowUKTQueues 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulUKT;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowUKTQueues", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_UKT_QUEUES), 
                      CON_REQUEST_SECURITY(CON_REQ_UKT_QUEUES), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_UKT_QUEUES),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_UKT_QUEUES),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_UKT_QUEUES));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_UKT_QUEUES_REC                pUKTQueueInfo    = (PCONS_UKT_QUEUES_REC)pData;

        if ( pUKTQueueInfo->lNumOfActiveUKTs > 0 )
        {
            PCONS_SINGLE_UKT_QUEUES_INFO_REC    pUKTQueues = (PCONS_SINGLE_UKT_QUEUES_INFO_REC)
                (pData + pUKTQueueInfo->ulUKTQueuesOffset);        

            for ( ulUKT = 0; ulUKT < (SAPDB_ULong)pUKTQueueInfo->lNumOfActiveUKTs;
                ulUKT++, pUKTQueues++ )
                if ( pUKTQueues->ulUKTQueueLen ||
                    pUKTQueues->ulCOMQueueLen ||
                    pUKTQueues->ulRUNQueueLen )
                {
                    printf ( XCONS_SHOW_QUEUES_HEAD,
                        pUKTQueues->ulUKTIndex ,
                        pUKTQueues->Tid ) ;

                    ShowUKTQueue ( pUKTQueues->ulUKTQueueLen,
                        (PCONS_SINGLE_QUEUE_INFO_REC )
                        (pData + pUKTQueues->ulUKTQueueOffset)) ;

                    ShowCOMQueue ( pUKTQueues->ulCOMQueueLen,
                        (PCONS_SINGLE_QUEUE_INFO_REC )
                        (pData + pUKTQueues->ulCOMQueueOffset)) ;

                    ShowRUNQueue ( pUKTQueues->ulRUNQueueLen,
                        (PCONS_SINGLE_QUEUE_INFO_REC )
                        (pData + pUKTQueues->ulRUNQueueOffset)) ;
                }
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTECons_StandardSession::ShowStateInfo 
( 
    CONS_STATE_INFO_REC &    stateInfo 
)
const
{
    SAPDB_Char *      pszStateStr;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowStateInfo", Console_Trace, 1);

    switch (stateInfo.DatabaseState)
    {
    case SERVER_STARTING      : pszStateStr = XCONS_SHOW_STATE_STARTING ; break;
    case SERVER_COLD          : pszStateStr = XCONS_SHOW_STATE_COLD     ; break;
    case SERVER_STANDBY_EO00  : pszStateStr = XCONS_SHOW_STATE_STANDBY  ; break;
    case SERVER_WARM          : pszStateStr = XCONS_SHOW_STATE_WARM     ; break;
    case SERVER_SHUTDOWN      : pszStateStr = XCONS_SHOW_STATE_SHTDWN   ; break;
    case SERVER_SHUTDOWNREINIT: pszStateStr = XCONS_SHOW_STATE_SHTDWNREI; break;
    case SERVER_SHUTDOWNKILL  : pszStateStr = XCONS_SHOW_STATE_SHTDWNKIL; break;
    case SERVER_STOP          : pszStateStr = XCONS_SHOW_STATE_STOP     ; break;
    case SERVER_KILL          : pszStateStr = XCONS_SHOW_STATE_KILL     ; break;
    case SERVER_ABORT         : pszStateStr = XCONS_SHOW_STATE_ABORT    ; break;
    default                   : pszStateStr = XCONS_SHOW_STATE_UNKNOWN  ; break;
    }

    if (stateInfo.ulTerminationTimeOut != 0)
    {
        printf ( XCONS_SHOW_STATE_INFO_TERM_TIMEOUT, pszStateStr,
            stateInfo.ulTerminationTimeOut );
    }
    else
    {
        printf ( XCONS_SHOW_STATE_INFO, pszStateStr );
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowQueueInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{

    SAPDB_UInt4                     idx;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowQueueInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_QUEUE_STAT), 
                      CON_REQUEST_SECURITY(CON_REQ_QUEUE_STAT), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_QUEUE_STAT),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_QUEUE_STAT),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_QUEUE_STAT));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_QUEUE_STAT_INFO_REC         pQueueInfo   = (PCONS_QUEUE_STAT_INFO_REC)pData;
        PCONS_SINGLE_QUEUE_STAT_INFO_REC  pQueStatInfo = (PCONS_SINGLE_QUEUE_STAT_INFO_REC)
            (pData + pQueueInfo->ulQueStatInfoOffset);    

        printf ( XCONS_SHOW_QUEUE_STAT_HEADER );

        for ( idx = 0; idx < (SAPDB_UInt4)pQueueInfo->lNumOfActiveTasks; idx++)
        {
            printf ( XCONS_SHOW_QUEUE_STAT_INFO ,
                pQueStatInfo[idx].ulTaskIndex,
                pQueStatInfo[idx].QueStat.Resetable.ulUToUQueCount,
                pQueStatInfo[idx].QueStat.Resetable.ulUKTQueCount,
                pQueStatInfo[idx].QueStat.Resetable.ulCOMQueCount ) ;
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowTaskCountInfo 
( 
    RTE_ConsoleStandardDataPacket & request,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_Char                      szApplPid[12];
    teo52_TaskStateStatisticRec *   pTaskStateStat;
    teo52_TaskPrioStatistic *       pTaskPrioStat;
    BOOL                            fTimeMeasureActive;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowTaskCountInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_TASK_COUNTS), 
                      CON_REQUEST_SECURITY(CON_REQ_TASK_COUNTS), 
                      (SAPDB_Byte *)&request, 
                      sizeof (request.data.TaskCountsInfo), 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_TASK_COUNTS),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (request.header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_TASK_COUNTS),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = request.header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_TASK_COUNTS));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (request.header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_TASK_COUNTS_INFO_REC           pTaskInfo       = (PCONS_TASK_COUNTS_INFO_REC)pData;

        /* PTS 1104727 */
        if ( pTaskInfo->usReturnCode == CON_WRONG_TASK )
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_WRONG_TASK);
            return RTE_CONS_ERROR;
        }

        PCONS_SINGLE_TASK_COUNTS_INFO_REC    pTaskCountsInfo = (PCONS_SINGLE_TASK_COUNTS_INFO_REC)
            (pData + pTaskInfo->ulTaskCountsInfoOffset);

        fTimeMeasureActive = pTaskInfo->fTimeMeasureActive;

        for ( idx = 0;
            idx < (SAPDB_UInt4)pTaskInfo->lNumOfActiveTasks;
            idx++, pTaskCountsInfo++ )
        {
            pTaskStateStat  = &pTaskCountsInfo->StateStat;
            pTaskPrioStat   = &pTaskCountsInfo->PrioStat;

            if ( pTaskCountsInfo->ApplPid != RTE_UNDEF_OSPID )
            {
                sql47_ltoa ( (long)pTaskCountsInfo->ApplPid, szApplPid, 10 );
            }
            else
            {
                szApplPid[0] = '?';
                szApplPid[1] = '\0';
            }


            printf ( XCONS_SHOW_TASK_COUNT_HEADER,
                XCONS_INFO_LINE ,
                pTaskCountsInfo->ulTaskIndex,
                pTaskCountsInfo->displayName,
                szApplPid,
                XCONS_INFO_LINE ) ;

            if ( pTaskCountsInfo->szClientNode[0] )
            {
                printf ( XCONS_SHOW_TASK_COUNT_REM_NODE_AND_PID ,
                    XCONS_SHOW_TASK_COUNT_REM_NODE ,
                    pTaskCountsInfo->szClientNode ,
                    XCONS_SHOW_TASK_COUNT_REM_PID  ,
                    szApplPid );
            }

            eo92PrintTaskOMSStatistics( &pTaskCountsInfo->TaskOMSStat ) ;

            printf ( XCONS_SHOW_TASK_COUNT_DISP_COMMAND_CNT    ,
                XCONS_SHOW_TASK_COUNT_KT_DISP_CALL_CNT,
                pTaskCountsInfo->ulDispatcherCalls,
                XCONS_SHOW_TASK_COUNT_KT_COMMAND_CNT,
                pTaskCountsInfo->ulCommands );

            printf ( XCONS_SHOW_TASK_COUNT_EXCL_SELF_SUSP_CNT  ,
                XCONS_SHOW_TASK_COUNT_KT_EXCLUSIVE_CNT ,
                pTaskCountsInfo->ulBegExclCalls ,
                XCONS_SHOW_TASK_COUNT_KT_SELF_SUSP_CNT ,
                pTaskCountsInfo->ulSelfSuspCount );

            if ( pTaskCountsInfo->totalResumerCount > 0 )
            {
                SAPDB_UInt4     j2;
                SAPDB_UInt4     j;
                printf ( XCONS_SHOW_TASK_COUNT_RESUME_CNT,
                    (int)pTaskCountsInfo->lResumeCount, (int)pTaskCountsInfo->totalResumerCount);
                j2 = pTaskCountsInfo->totalResumerCount;
                j = MAX_RESUMER_PIDS;
                while( (j2-- > 0) && (j-- > 0) )
                {
                    printf ( " T%d", (int)pTaskCountsInfo->resumerPid[j2%MAX_RESUMER_PIDS]);
                }
                printf ( " ]\n" );
            }

            /* PTS 1106969 */
            if ( *pTaskCountsInfo->DevNameWaitingFor )
                printf ( XCONS_SHOW_TASK_COUNT_WAIT_FOR_DEV, pTaskCountsInfo->DevNameWaitingFor) ;

            eo92PrintTaskCounts( (tsp00_Int4 ) fTimeMeasureActive, pTaskStateStat ) ;

            if ( pTaskPrioStat->Resetable.ulTotalCount )
                printf ( XCONS_SHOW_TASK_COUNT_PRIO_TOT_OTH,
                XCONS_SHOW_TASK_COUNT_KT_PRIO_CNT ,
                pTaskPrioStat->Resetable.ulTotalCount   ,
                XCONS_SHOW_TASK_COUNT_KT_PRIO_FROM_OTHER,
                pTaskPrioStat->Resetable.ulOtherUkt ) ;

            printf ( XCONS_SHOW_TASK_COUNT_TRAILER ) ;
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTECons_StandardSession::ShowTotalCountInfo 
( 
    CONS_TOTAL_COUNTS_INFO_REC &    totalCntInfo 
)
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowTotalCountInfo", Console_Trace, 1);

    printf ( XCONS_SHOW_TOTAL_STAT_HEADER ) ;

    printf ( XCONS_SHOW_TOTAL_DISP_COMMAND_CNT ,
        XCONS_SHOW_TOTAL_KT_DISP_CALL_CNT ,
        totalCntInfo.ulTotalDispCalls,
        XCONS_SHOW_TOTAL_KT_COMMAND_CNT ,
        totalCntInfo.ulTotalCommands);

    printf ( XCONS_SHOW_TOTAL_KT_STATE_VWAIT_VSUSP_CNT ,
        XCONS_SHOW_TOTAL_KT_STATE_VWAIT_CNT,
        totalCntInfo.ulTotalVwait,
        XCONS_SHOW_TOTAL_KT_STATE_VSUSP_CNT,
        totalCntInfo.ulTotalVsusp );

    printf ( XCONS_SHOW_TOTAL_KT_STATE_VSLEEP,
        XCONS_SHOW_TOTAL_KT_STATE_VSLEEP_CNT,
        totalCntInfo.ulTotalVsleep);

    printf ( XCONS_SHOW_TOTAL_SELFIO_DEVIO_CNT ,
        XCONS_SHOW_TOTAL_KT_SELFIO_CNT ,
        totalCntInfo.ulTotalSelfIO,
        XCONS_SHOW_TOTAL_KT_DEVIO_CNT,
        totalCntInfo.ulTotalDevIO);

    printf ( XCONS_SHOW_TOTAL_REG_ACC_BEGEXCL_COLL,
        XCONS_SHOW_TOTAL_KT_REG_ACCESS,
        totalCntInfo.ulTotalRegAccess,
        XCONS_SHOW_TOTAL_KT_BEGEXCL_COLL,
        totalCntInfo.ulTotalBegExclTasCount);

    printf ( XCONS_SHOW_TOTAL_REG_COLL_ENDEXCL_COLL,
        XCONS_SHOW_TOTAL_KT_REG_COLL,
        totalCntInfo.ulTotalRegColl,
        XCONS_SHOW_TOTAL_KT_ENDEXCL_COLL,
        totalCntInfo.ulTotalEndExclTasCount);

    printf ( XCONS_SHOW_TOTAL_KT_REG_WAIT ,
        XCONS_SHOW_TOTAL_KT_REG_WAIT_CNT ,
        totalCntInfo.ulTotalRegWait);

    if ( totalCntInfo.TotalPrioStat.Resetable.ulTotalCount )
    { 
        printf ( XCONS_SHOW_TOTAL_PRIO_CNT_AND_OTHER,
            XCONS_SHOW_TOTAL_KT_PRIO_CNT,
            totalCntInfo.TotalPrioStat.Resetable.ulTotalCount,
            XCONS_SHOW_TOTAL_KT_PRIO_OTHER ,
            totalCntInfo.TotalPrioStat.Resetable.ulOtherUkt);
    }

    printf ( XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCKS ,
        XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCK_CNT ,
        totalCntInfo.ulTotalSymbolResolutionLocks);
}


/*---------------------------------------------------------------------------*/
SAPDB_UInt4     RTECons_StandardSession::ShowAWEInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;

    RTE_ConsoleStandardDataHeader   header;
    PCONS_AWE_INFO_REC              pAWEInfo;
    SAPDB_ULong                     numInfosToRead = 0;
    SAPDB_ULong                     numPhysPageInfos = 0;
    SAPDB_ULong                     numMapPageInfos = 0;
    SAPDB_ULong                     ix;
    PCONS_AWE_MAP_PAGE_INFO_REC     pAWEMapPageInfos;
    PCONS_AWE_PHYS_PAGE_INFO_REC    pAWEPhysPageInfos;
    SAPDB_Bool                      aweAvailable = false;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowAWEInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_AWE), 
                      CON_REQUEST_SECURITY(CON_REQ_AWE), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_AWE),
                                                         m_request.serverDB); 
        return rc;
    }


    for (idx = 1; idx <= 3; ++idx)
    {
        do
        {
            if (1 == idx)
            {
                rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
                if (RTE_CONS_NO_ERROR != rc)
                {
                    messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                        SAPDB_ToString (CON_REQ_AWE),
                        m_request.serverDB);  
                    return rc;
                }
            }
            else
            {
                rc = GetNextReplyHeader (header, DATA_REP, bytesTotal, messageList);
                if (RTE_CONS_NO_ERROR != rc)
                {
                    messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                        SAPDB_ToString (CON_REQ_AWE),
                        m_request.serverDB);  
                    return rc;
                }
            }


            if (0 != header.dataLen)
            {
                SAPDB_Byte *                    pData;

                if (1 == idx)
                {
                    pAWEInfo = (PCONS_AWE_INFO_REC) RTEMem_RteAllocator::Instance().Allocate (header.dataLen);
                    if (NULL == pAWEInfo)
                    {
                        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                            SAPDB_ToString (header.dataLen),
                            m_request.serverDB,
                            SAPDB_ToString (CON_REQ_AWE));
                        return RTE_CONS_FATAL_ERROR;
                    }

                    rc = GetReplyData (header, DATA_REP, header.dataLen, (SAPDB_Byte *)pAWEInfo, messageList);
                    if (RTE_CONS_NO_ERROR != rc)
                    {
                        RTEMem_RteAllocator::Instance().Deallocate(pAWEInfo);
                        return rc;
                    }
                }
                else
                {
                    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (header.dataLen);
                    if (NULL == pData)
                    {
                        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                            SAPDB_ToString (header.dataLen),
                            m_request.serverDB,
                            SAPDB_ToString (CON_REQ_AWE));
                        return RTE_CONS_FATAL_ERROR;
                    }


                    rc = GetReplyData (header, DATA_REP, header.dataLen, pData, messageList);
                    if (RTE_CONS_NO_ERROR != rc)
                    {
                        RTEMem_RteAllocator::Instance().Deallocate(pData);
                        return rc;
                    }
                }

                switch (idx)
                {
                case 1:
                    printf ( XCONS_SHOW_AWE_INFO, 
                        pAWEInfo->NumOfPhysPages,
                        pAWEInfo->NumOfFreePhysPages,
                        pAWEInfo->NumOfMapAreaPages,
                        pAWEInfo->NumOfFreeMapAreaPages,
                        pAWEInfo->NumOfMaps,
                        pAWEInfo->NumOfUnMaps );
                    aweAvailable = pAWEInfo->AWEAvailable;
                    RTEMem_RteAllocator::Instance().Deallocate(pAWEInfo);
                    break;

                case 2:
                    pAWEMapPageInfos = (PCONS_AWE_MAP_PAGE_INFO_REC)pData; 
                    if (0 == numMapPageInfos)
                    {
                        printf (XCONS_SHOW_AWE_MAP_PAGE_HEADER);
                    }
 
                    numInfosToRead = header.dataLen / sizeof (*pAWEMapPageInfos);
                    numMapPageInfos += numInfosToRead;
                    for (ix = 0; ix < numInfosToRead; ++ix)
                    {
                        if (( pAWEMapPageInfos[ix].PhysPageNo ==  -1 ) ||
                            ( pAWEMapPageInfos[ix].Reserved   == false ))
                            printf ( XCONS_SHOW_AWE_UNMAP_PAGE_INFO,
                            pAWEMapPageInfos[ix].pMappingPageAddr,
                            pAWEMapPageInfos[ix].Reserved ? "YES": "NO",
                            pAWEMapPageInfos[ix].MapCount );
                        else
                            printf ( XCONS_SHOW_AWE_MAP_PAGE_INFO,
                            pAWEMapPageInfos[ix].pMappingPageAddr,
                            pAWEMapPageInfos[ix].PhysPageNo,
                            pAWEMapPageInfos[ix].MapCount );
                    }

                    break;

                case 3:
                    pAWEPhysPageInfos   = (PCONS_AWE_PHYS_PAGE_INFO_REC)pData; 
                    if (0 == numPhysPageInfos)
                    {
                        printf (XCONS_SHOW_AWE_MAP_PAGE_HEADER);
                    }
 
                    numInfosToRead = header.dataLen / sizeof (*pAWEPhysPageInfos);

                    for (ix = 0; ix < numInfosToRead; ++ix, ++numPhysPageInfos)
                    {
                        printf ( XCONS_SHOW_AWE_PHYS_PAGE_INFO,
                            numPhysPageInfos,
                            pAWEPhysPageInfos[ix].Reserved ? "YES" : "NO",
                            pAWEPhysPageInfos[ix].MapCount );
                    }

                    break;
                }

                if (1 != idx)
                {
                    RTEMem_RteAllocator::Instance().Deallocate(pData);
                }
            }
        } while (1 != idx && !header.lastSend);

        if (!aweAvailable)
        {
            break;
        }
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowConnectionsInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    # define MAX_NAME_BUFFER_SIZE 128

    SAPDB_ULong                     actConnectionIndex;
    SAPDB_Char                      buffer[MAX_NAME_BUFFER_SIZE];
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowConnectionsInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_CONNECTIONS), 
                      CON_REQUEST_SECURITY(CON_REQ_CONNECTIONS), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_CONNECTIONS),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_CONNECTIONS),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;

    if (0 != bytesLeft)
    {
        SAPDB_Byte *                    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                SAPDB_ToString (bytesLeft),
                m_request.serverDB,
                SAPDB_ToString (CON_REQ_CONNECTIONS));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);
        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }


        PCONS_CONNECTION_INFO_REC        pConnect  = (PCONS_CONNECTION_INFO_REC)pData;
        PCONS_SINGLE_CONNECTION_INFO_REC  pConnectionInfo  = (PCONS_SINGLE_CONNECTION_INFO_REC)
            (pData + pConnect->ulConnectionInfoOffset);   


        for (actConnectionIndex = 0; actConnectionIndex < (SAPDB_ULong)pConnect->lNumOfConnections; 
            actConnectionIndex++, pConnectionInfo++ )
        {
            eo92GetConnectStateName( pConnectionInfo->connState, buffer, MAX_NAME_BUFFER_SIZE );

            printf ( XCONS_FORMAT_SHOW_CONNECTION_TASK ,
                pConnectionInfo->taskIndex,
                pConnectionInfo->uktIndex,
                pConnectionInfo->displayName);

            if ( pConnectionInfo->connectTime == 0 )
            {
                printf ( XCONS_FORMAT_SHOW_CONNECTION_STATE_NOT_CONNECTED, buffer );
            } 
            else 
            {
                struct tm *pTime = localtime( &pConnectionInfo->connectTime );

                if ( pTime )
                {
                    printf ( XCONS_FORMAT_SHOW_CONNECTION_STATE_TIME,
                        buffer, asctime(pTime) );
                } 
                else 
                {
                    printf ( XCONS_FORMAT_SHOW_CONNECTION_STATE_DUMPTIME,
                        buffer, pConnectionInfo->connectTime );
                }
            }

            if ( pConnectionInfo->clientNode[0] == 0 || pConnectionInfo->clientNode[0] == ' ' )
            {
                printf ( XCONS_FORMAT_SHOW_CONNECTION_LOCAL, pConnectionInfo->peerPid );
            } 
            else 
            {
                sprintf(buffer, "%I64d", pConnectionInfo->remotePid );

                printf ( XCONS_FORMAT_SHOW_CONNECTION_REMOTE ,
                    pConnectionInfo->peerPid,
                    buffer,
                    pConnectionInfo->clientNode );
            }
        }

        if ( !pConnect->lNumOfConnections )
            printf ( XCONS_FORMAT_SHOW_CONNECTION_NO_TASK_CONNECTED );

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}
#endif

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowTaskMoveInfo 
( 
    RTE_ConsoleStandardDataPacket & request,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     i;
    SAPDB_ULong                     ulUnconnectedTasks   = 0;
    SAPDB_ULong                     ulUnusedServerTasks  = 0;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;
    RTE_ConsoleStandardDataHeader   header;

    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowTaskMoveInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_TASK_MOVE), 
                      CON_REQUEST_SECURITY(CON_REQ_TASK_MOVE), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_TASK_MOVE),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (request.header, DATA_REP, bytesTotal, messageList);

    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_TASK_MOVE),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = request.header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte *    pData;

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                                                             SAPDB_ToString (bytesLeft),
                                                             m_request.serverDB,
                                                             SAPDB_ToString (CON_REQ_TASK_MOVE));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (request.header, DATA_REP, bytesLeft, pData, messageList);

        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }

        PCONS_TASK_MOVE_INFO_REC         pTaskInfo   = (PCONS_TASK_MOVE_INFO_REC)pData;
        PCONS_SINGLE_TASK_MOVE_INFO_REC  pTaskInfos  = (PCONS_SINGLE_TASK_MOVE_INFO_REC)(pData + pTaskInfo->ulTaskMoveInfoOffset);

        printf ( XCONS_SHOW_TASK_MOVE_HEADER );

        if ( pTaskInfo->lNumOfActiveTasks == 0 )
            return ( NO_ERROR );

        for ( i = 0; i < (SAPDB_UInt4)pTaskInfo->lNumOfActiveTasks; i++)
        {
          if (( pTaskInfos[i].TaskState == TSK_INITIALIZED_EO00         ||
                pTaskInfos[i].TaskState == TSK_INACTIVE_EO00            ||
                pTaskInfos[i].TaskState == TSK_MARKED_FOR_STARTUP_EO00  ||
                pTaskInfos[i].TaskState == TSK_CONNECTWAIT_EO00       ) &&
                pTaskInfos[i].TaskType  != TT_UT_EO00 )
            {
                if ( pTaskInfos[i].TaskType == TT_SV_EO00 )
                    ulUnusedServerTasks++;
                else
                    ulUnconnectedTasks++;
            }
            else
            {
                char   szLock[12];
                szLock[sizeof(szLock) - 1] = '\0';

                if( pTaskInfos[i].LongTermMoveLock )
                {
                    strcpy ( szLock, "LT:" );
                    sql47_ltoa( pTaskInfos[i].LongTermMoveLock, szLock + 3, sizeof(szLock) - 3 - 1 );
                }
                else
                    sql47_ltoa( pTaskInfos[i].TemporaryMoveLock, szLock, sizeof(szLock) - 1 );

                printf ( XCONS_SHOW_TASK_MOVE_INFO,
                        pTaskInfos[i].ulTaskIndex,
                        pTaskInfos[i].ulUktIndex,
                        pTaskInfos[i].displayName,
                        eo92GetTaskStateName (pTaskInfos[i].TaskState,(SAPDB_Int4)pTaskInfos[i].ulStateReason),
                        pTaskInfos[i].MaxRunnableMicroSeconds / 1000000,
                        pTaskInfos[i].MaxRunnableMicroSeconds % 1000000,
                        pTaskInfos[i].RunnableMicroSeconds / 1000000,
                        pTaskInfos[i].RunnableMicroSeconds % 1000000,
                        szLock );
            }
        }
        if ( ulUnusedServerTasks )
        {
            printf ( XCONS_SHOW_SERVER_UNUSED, ulUnusedServerTasks );
        }

        if ( ulUnconnectedTasks )
        {
            printf ( XCONS_SHOW_TASK_CONN_INFO,
                ulUnconnectedTasks, XCONS_TSK_STATE_CONNECTWAIT );
        }


        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowMoveInfo 
( 
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     idx;
    SAPDB_Char                      TimeBuf[20];             /* ddd:hh:mm:ss: */  
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;

    RTE_ConsoleStandardDataHeader   header;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::ShowMoveInfo", Console_Trace, 1);
                 
    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_MOVE_INFO), 
                      CON_REQUEST_SECURITY(CON_REQ_MOVE_INFO), 
                      (SAPDB_Byte *)&header, 
                      0, 
                      messageList);    
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_MOVE_INFO),
                                                         m_request.serverDB); 
        return rc;
    }

    rc = GetFirstReplyHeader (header, DATA_REP, bytesTotal, messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                         SAPDB_ToString (CON_REQ_MOVE_INFO),
                                                         m_request.serverDB);  
        return rc;
    }

    bytesLeft = header.dataLen;
    
    if (0 != bytesLeft)
    {
        SAPDB_Byte * pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (bytesLeft);

        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                                                              SAPDB_ToString (bytesLeft),
                                                              m_request.serverDB,
                                                              SAPDB_ToString (CON_REQ_MOVE_INFO));
            return RTE_CONS_FATAL_ERROR;
        }

        rc = GetReplyData (header, DATA_REP, bytesLeft, pData, messageList);

        if (RTE_CONS_NO_ERROR != rc)
        {
            RTEMem_RteAllocator::Instance().Deallocate(pData);
            return rc;
        }


        PCONS_MOVE_INFO_REC             pTaskInfo  = (PCONS_MOVE_INFO_REC)pData;
        PCONS_SINGLE_UKT_MOVE_INFO_REC  pUKTInfos  = (PCONS_SINGLE_UKT_MOVE_INFO_REC)
                                                     (pData + pTaskInfo->ulUKTInfoOffset);   
        PCONS_LAST_MOVES_ENTRY          pLastMoves = (PCONS_LAST_MOVES_ENTRY)
                                                     (pData + pTaskInfo->ulLastMovesInfoOffset); 

        if (pTaskInfo->TaskMoveIntervalStartTime )
            eo92_SecToDDHHMMSS( (long)(time(NULL) - pTaskInfo->TaskMoveIntervalStartTime), 
            TimeBuf, (int)sizeof(TimeBuf) ) ;
        else
            TimeBuf[0] = 0;

        printf ( XCONS_SHOW_MOVE_INFO,
            pTaskInfo->CheckIntervalTime ? "ENABLED" : "DISABLED",
            pTaskInfo->CheckIntervalTime,
            TimeBuf,
            pTaskInfo->TaskMoveMeasIntervals,
            pTaskInfo->MinimumDiff,
            pTaskInfo->ValueEquality );


        if (pTaskInfo->lNumOfLastRecs)
        {
            printf ( XCONS_SHOW_LAST_TASK_MOVES_HEADER );

            for (idx = 0; idx < (SAPDB_UInt4)pTaskInfo->lNumOfLastRecs; idx++)
            {
                eo92_SecToDDHHMMSS( long(time(NULL) - pLastMoves[idx].MoveTime), TimeBuf, (int)sizeof(TimeBuf));
                printf ( XCONS_SHOW_LAST_TASK_MOVES_INFO,
                    TimeBuf,
                    pLastMoves[idx].ulTaskIndex,
                    pLastMoves[idx].TaskMaxRunnableMicroSeconds / 1000000,
                    pLastMoves[idx].TaskMaxRunnableMicroSeconds % 1000000,
                    pLastMoves[idx].ulSrcUKTIndex,
                    pLastMoves[idx].SrcMaxRunnableMicroSeconds / 1000000,
                    pLastMoves[idx].SrcMaxRunnableMicroSeconds % 1000000,
                    pLastMoves[idx].ulDstUKTIndex,
                    pLastMoves[idx].DstMaxRunnableMicroSeconds / 1000000,
                    pLastMoves[idx].DstMaxRunnableMicroSeconds % 1000000 );
            }
        }

        printf ( XCONS_SHOW_MOVE_UKT_HEADER );

        for (idx = 0; idx < (SAPDB_UInt4)pTaskInfo->lNumOfUKTs; idx++)
        {
            if (pTaskInfo->TaskMoveIntervalStartTime )
            {
                eo92_SecToDDHHMMSS( (long)(time(NULL) - pUKTInfos[idx].TaskMoveIntervalStartTime), 
                                    TimeBuf, (int)sizeof(TimeBuf));
            }
            else
            {
                TimeBuf[0] = 0;
            }

            printf ( XCONS_SHOW_MOVE_UKT_INFO,
                pUKTInfos[idx].ulUKTIndex,
                pUKTInfos[idx].NumOfMovableTasks,
                TimeBuf,
                pUKTInfos[idx].MaxRunnableMicroSeconds / 1000000,
                pUKTInfos[idx].MaxRunnableMicroSeconds % 1000000,
                pUKTInfos[idx].RunnableMicroSeconds / 1000000,
                pUKTInfos[idx].RunnableMicroSeconds % 1000000,
                pUKTInfos[idx].UKTSleepTimeMicroSeconds / 1000000,
                pUKTInfos[idx].UKTSleepTimeMicroSeconds % 1000000 );
        }

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}



/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECons_StandardSession::ShowRWRegions 
( 
    RTE_ConsoleStandardDataPacket & request,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                     bytesTotal;
    SAPDB_UInt4                     bytesLeft;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDB_Bool                      firstPacket = true;
    SAPDB_Byte *                    pData = NULL;

    SAPDB_Bool                      spinlockPoolHeaderHasBeenWritten = false;
    SAPDB_Bool                      rwRegionHeaderHasBeenWritten = false;

    SAPDB_Bool                      showAllRWRegions;

    SAPDB_ULong                     spinlockPoolCounter = 0;

    rc = SendRequest (DATA_REQ, 
                      CON_REQUEST(CON_REQ_RWREGIONS),
                      CON_REQUEST_SECURITY(CON_REQ_RWREGIONS),
                      (SAPDB_Byte *)&request,
                      sizeof(request.data.RWRegionInfo),
                      messageList);
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_SEND_REQUEST,
                                                         SAPDB_ToString (CON_REQ_RWREGIONS),
                                                         m_request.serverDB); 
        return rc;
    }

    do
    {
        if(firstPacket)
        {
            rc = GetFirstReplyHeader (request.header, DATA_REP, bytesTotal, messageList);
            firstPacket = false;
            // only the last incoming packet may not be filled completely
            // that means that there cannot be a packet with a dataLen bigger than that of the first one
            // so it is okay to allocate the buffer memory regarding the dataLen of the first packet
            pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (request.header.dataLen);
            if (NULL == pData)
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_ALLOC_DATA,
                                                                SAPDB_ToString (request.header.dataLen),
                                                                m_request.serverDB,
                                                                SAPDB_ToString (CON_REQ_RWREGIONS));
                return RTE_CONS_FATAL_ERROR;
            }
            showAllRWRegions = request.data.RWRegionInfo.info.showAll;
        }
        else
        {
            rc = GetNextReplyHeader (request.header, DATA_REP, bytesTotal, messageList);
        }

        if (RTE_CONS_NO_ERROR != rc)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_XCONS_GET_HEADER,
                                                            SAPDB_ToString (CON_REQ_RWREGIONS),
                                                            m_request.serverDB);  
            return rc;
        }

        bytesLeft = request.header.dataLen;
        
        if (0 != bytesLeft)
        {
            rc = GetReplyData (request.header, DATA_REP, bytesLeft, pData, messageList);

            if (RTE_CONS_NO_ERROR != rc)
            {
                RTEMem_RteAllocator::Instance().Deallocate(pData);
                return rc;
            }
            PCONS_RWREGION_INFO_REC pRWRegionHeader = (PCONS_RWREGION_INFO_REC)pData;
            PCONS_SINGLE_SPINLOCKPOOL_INFO_REC pCurrentSpinlockPoolInfo = (PCONS_SINGLE_SPINLOCKPOOL_INFO_REC)(pData + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT));

            if( (pRWRegionHeader->info.outParams.numberOfSpinlockPools > 0) && !spinlockPoolHeaderHasBeenWritten )
            {
                printf("Spinlock Pools:\n");
                spinlockPoolHeaderHasBeenWritten = true;
            }

            for ( SAPDB_UInt spinlockCounter = 0; spinlockCounter < pRWRegionHeader->info.outParams.numberOfSpinlockPools; spinlockCounter++)
            {
                printf("%d: %s\n",spinlockPoolCounter++,pCurrentSpinlockPoolInfo->m_Name);
                pCurrentSpinlockPoolInfo++;
            }

            if( (pRWRegionHeader->info.outParams.ulInfoNum > 0) && !rwRegionHeaderHasBeenWritten )
            {
                if(showAllRWRegions)
                    printf("\nRWRegions:\n");
                else
                    printf("\nRWRegions used:\n");
                printf("Address   ");
#ifdef BIT64
                printf("        ");
#endif
                printf("Pool Task ");
                printf("     Access ");
                printf("Collisions ");
                printf("ExclAccess ");
                printf("NonEAccess ");
                printf("Waiters");
                printf("\n");
                rwRegionHeaderHasBeenWritten = true;
            }

            PCONS_SINGLE_RWREGION_INFO_REC pCurrentRWRegionInfo = (PCONS_SINGLE_RWREGION_INFO_REC)(pData 
                                                                                                  + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                                                                  + SAPDB_ALIGN( sizeof(*pCurrentSpinlockPoolInfo) * pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT));
            for ( SAPDB_UInt rwRegionCounter = 0; rwRegionCounter < pRWRegionHeader->info.outParams.ulInfoNum; rwRegionCounter++)
            {
                // for the hexadecimal output of an address with leading "0X", sizeof(void*)*2+2 characters are needed (10 on 32bit, 18 on 64bit)
                printf("%s ",(SAPDB_Char *)SAPDB_ToString(pCurrentRWRegionInfo->pAddress,sizeof(void*)*2+2,SAPDB_ToStringClass::showpoint,SAPDB_ToStringClass::showbase));

                printf("(%d) ",pCurrentRWRegionInfo->spinlockPoolIndex);

                switch( pCurrentRWRegionInfo->taskID )
                {
                case -1:
                    printf("???? ");
                    break;
                case 0:
                    printf("none ");
                    break;
                default:
                    printf("%4d ",pCurrentRWRegionInfo->taskID);
                    break;
                }
                switch(pCurrentRWRegionInfo->access)
                {
                case CONS_SINGLE_RWREGION_INFO_REC::holdingExclusive:
                    printf("   holdExcl ");
                    break;
                case CONS_SINGLE_RWREGION_INFO_REC::holdingNonExclusive:
                    printf("holdNonExcl ");
                    break;
                case CONS_SINGLE_RWREGION_INFO_REC::waitingForNonExclusive:
                    printf("waitNonExcl ");
                    break;
                case CONS_SINGLE_RWREGION_INFO_REC::waitingForExclusive:
                    printf("   waitExcl ");
                    break;
                default:
                    printf("            ");
                    break;
                }
                printf("%10d ",pCurrentRWRegionInfo->collisionCounter);
                printf("%10d ",pCurrentRWRegionInfo->exclusiveAccessCounter);
                printf("%10d ",pCurrentRWRegionInfo->nonExclusiveAccessCounter);
                printf("%7d",pCurrentRWRegionInfo->waitCounter);
                printf("\n");
                pCurrentRWRegionInfo++;
            }
        }
    }
    while( ! request.header.lastSend );

    if( pData )
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

