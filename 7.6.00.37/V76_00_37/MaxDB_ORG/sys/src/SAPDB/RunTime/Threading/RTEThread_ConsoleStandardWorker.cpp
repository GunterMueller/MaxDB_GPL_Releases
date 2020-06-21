/*!
  @file           RTEThread_ConsoleStandardWorker.cpp
  @author         StefanP
  @special area   Kernel Console Thread
  @brief          Standard XCons Request Processing
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

#include    "RunTime/MemoryManagement/RTEMem_AWEAllocator.hpp"
#include    "RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp"

#include    "RunTime/RTE_KGS.hpp"
#include    "heo670.h"
#include    "heo92.h"
#include    "geo007_1.h"
#include    "heo54k.h" //DEV_READ, DEV_WRITE
#include    "geo00_1.h"

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/
#include    "gos003.h"  /* nocheck */
#include    "gos00k.h"  /* nocheck */
#else
#include    "gen81.h"  /* nocheck */
#include    "gen71.h"  /* nocheck */
#include    "gen72.h"  /* nocheck */
#include    "gen73.h"  /* nocheck */
#endif                  /*&endif*/

#include    "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"
#include    "RunTime/RTE_CompilerFeatures.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleStandardWorker.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/RTE_KSS.h"
#include    "RunTime/System/RTESys_MicroTime.h"
#include    "RunTime/Tasking/RTETask_CommunicationQueue.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include    "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include    "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include    "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include    "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h"
#include    "SAPDBCommon/SAPDB_Names.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#define EXCEPT_MINI_DUMP              0xE000BAFF
#define DT_CRASH_DUMP                 0x01
#define DT_MINI_DUMP                  0x02

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define OFFSET_OFF(_strct,_elem)   ((size_t)&(((_strct *)0)->_elem))

#define QW_TO_DBL(_ulHigh,_ulLow) (((double)_ulHigh * 4294967296.) +    \
                                    (double)_ulLow)


//===========
#if defined(_WIN32)
//===========
#define IS_DEVICE_ATTACHED(DevNo_) ( KGS->Dev0.pIOCtrl[DevNo_].ulAttached != 0 \
                                  && KGS->Dev0.pIOCtrl[DevNo_].ulDetachPending == 0 )

#define FIRST_DEV                (KGS->pFirstDeviCtrl)
#define FIRST_UKT                (kgs.pFirstUKTCtrl)
#define LAST_UKT                 (kgs.pLastUKTCtrl)
#define LOCK_OF_UKT( pUKTCtrl_ )   &(pUKTCtrl_)->lExclusive
#define LOCK_OF_DEVI( pDeviCtrl_ ) &(pDeviCtrl_)->lExclusive
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED )
#define DEVI_PER_VOLUME          (XPARAM(lDeviPerDevspace))
//===========
#else
//===========
#define IS_DEVICE_ATTACHED(DevNo_) ( KGS->dev0.dev[DevNo_].devspace[0] != 0 \
                                  && KGS->dev0.dev[DevNo_].ulDetachPending == 0 )
typedef struct DEV_PCB      *PDEVI_CTRL_REC ;
typedef struct DEV_PCB      *PASYNCI_CTRL_REC;
#define FIRST_DEV                (KGS->first_dev)
#define FIRST_UKT                (kgs->first_kp)
#define LAST_UKT                 (kgs->last_kp)
#define LOCK_OF_UKT( pUKTCtrl_ )   (pUKTCtrl_)->exclusive
#define LOCK_OF_DEVI( pDeviCtrl_ ) (pDeviCtrl_)->alone
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->state != KT_INITIALIZED )
#define DEVI_PER_VOLUME          (KGS->devpsets)

#define PUKT_CTRL_REC struct ten50_UKT_Control *

//===========
#endif
//===========



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDB_Bool   RTEThread_ConsoleStandardWorker::SendError
(
    SAPDB_UInt1 const                   respType,
    SAPDB_UInt4 const                   errCode,
    SAPDBErr_MessageList &              messageListToSend
)
{
    teo200_EventList                    tmpEventList;
    SAPDBErr_MessageList                tmpMessageList;
    teo200_EventList                    eventList;
    SAPDB_UInt4                         bufferSize = 0;
    SAPDB_UInt4                         eventDataSize = 0;
    SAPDB_UInt4                         minEventDataSize;
    SAPDB_Byte                         *pError;
    RTE_ConsoleStandardDataHeader      *pHeader;
    SAPDB_Byte                         *pMsgText;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::SendError",
                             Console_Trace, 1);

    SAPDBErr_ConvertMessageToEventList (messageListToSend, eventList);

    if (!eventList.IsEmpty ())
    {
        bufferSize = eventList.eo200_DataBufferSize (&minEventDataSize);
    }

    pError = (SAPDB_Byte *)(RTEMem_RteAllocator::Instance().Allocate (bufferSize
                                                                + sizeof (RTE_ConsoleStandardDataHeader)));
    if (NULL == pError)
    {
        RTE_ConsoleStandardDataHeader   header;

        messageListToSend = messageListToSend + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_EVENT,
                                                                     SAPDB_ToString (bufferSize));
        header.rc = errCode;
        header.dataLen = 0;
        header.lastSend = true;

        if (RTE_CONS_NO_ERROR != SendReplyData ((SAPDB_Byte *)&header, sizeof (header),
                                                      respType, messageListToSend))
        {
            messageListToSend = messageListToSend + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
            return false;
        }

        return true;
    }

    pHeader = (RTE_ConsoleStandardDataHeader *) pError;
    pHeader->rc           = errCode;
    pHeader->lastSend     = true;

    pMsgText = pError + sizeof (*pHeader);

    //Attention: Memory has to completely deallocated after sending!!!
    if (!eventList.eo200_StoreEventList (pMsgText, bufferSize, eventDataSize, true,
                                         &tmpEventList))
    {
        SAPDBErr_ConvertEventToMessageList (tmpMessageList, tmpEventList);
        messageListToSend = messageListToSend + tmpMessageList;
        messageListToSend = messageListToSend + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_STORE_EVENT);

        pHeader->dataLen = 0;

        if (RTE_CONS_NO_ERROR != SendReplyData ((SAPDB_Byte *)pHeader, sizeof (*pHeader),
                                                      respType, messageListToSend))
        {
            RTEMem_RteAllocator::Instance().Deallocate(pError);
            messageListToSend = messageListToSend + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
            return false;
        }

        RTEMem_RteAllocator::Instance().Deallocate(pError);
        return true;
    }

    pHeader->dataLen = eventDataSize;

    if (RTE_CONS_NO_ERROR != SendReplyData (pError, sizeof (*pHeader) + eventDataSize,
                                            respType, messageListToSend))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pError);
        messageListToSend = messageListToSend + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return false;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pError);

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::Connect
(
    RTE_ConsoleHandle const         hConnect,
    SAPDBErr_MessageList &          messageList
)
{
    RTE_ConsoleStandardConnectPacket    connect;
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         bytesTotal;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::Connect",
                             Console_Trace, 1);

    memset (&connect, 0, sizeof (connect));


    if (RTE_CONS_NO_ERROR != (rc = GetRequestData ((SAPDB_Byte *)(&connect.header), OPEN_REQ, sizeof (connect.header),
                                                    bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }

    if (CON_REQUEST (CON_REQ_OPEN) != connect.header.type)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_REQ_TYPE,
                                                         SAPDB_ToString (CON_REQ_OPEN),
                                                         SAPDB_ToString (connect.header.type));
        if (!SendError (OPEN_REP, RTE_CONS_ERROR, messageList))
        {
            messageList = messageList
                          + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_ERR);
        }

        return RTE_CONS_ERROR;
    }

    memset (&connect, 0, sizeof (connect));

   /* Sending Connection Handle back to client (XCons) */
    connect.header.rc             =   RTE_CONS_NO_ERROR;
    connect.header.lastSend       =   true;
    connect.header.dataLen        =   sizeof (connect.hConnect);

    connect.hConnect = hConnect;

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData ((SAPDB_Byte *)&connect, sizeof (connect), OPEN_REP, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ConnectReject( SAPDBErr_MessageList&        sendMessageList )
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ConnectReject",
                             Console_Trace, 1);

    RTE_ConsoleStandardConnectPacket        connect;
    SAPDB_UInt4                             bytesRead;
    SAPDB_UInt4                             bytesTotal;
    SAPDBErr_MessageList                    tmpMessageList;
    SAPDB_UInt4                             rc;

    rc = GetRequestData ( (SAPDB_Byte *)(&connect.header), OPEN_REQ,
                          sizeof (connect.header), bytesRead, bytesTotal,
                          tmpMessageList );

    if ( RTE_CONS_NO_ERROR != rc )
        return;

    SendError (OPEN_REP, RTE_CONS_FATAL_ERROR, sendMessageList );

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::Disconnect
(
    SAPDBErr_MessageList &      messageList
)
{
    RTE_ConsoleStandardDataHeader       header;
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         bytesTotal;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::Disconnect",
                             Console_Trace, 1);

    memset (&header, 0, sizeof (header));

    if (RTE_CONS_NO_ERROR != (rc = GetRequestData ((SAPDB_Byte *)&header, CLOSE_REQ, sizeof (header),
                                                    bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }


    if (CON_REQUEST(CON_REQ_CLOSE) == header.type)
    {
        memset (&header, 0, sizeof (header));

        header.rc             =   RTE_CONS_NO_ERROR;
        header.lastSend       =   true;
        header.dataLen        =   0;


        /* Sending reply to client (XCons): only header and no data at the moment */
        if (RTE_CONS_NO_ERROR != (rc = SendReplyData ((SAPDB_Byte *)&header, sizeof (header), CLOSE_REP, messageList)))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
            return rc;
        }

        if (RTE_CONS_NO_ERROR != (rc = WaitForReleaseConfirmation (messageList)))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_WAIT_CONFIRMATION);
            return rc;
        }
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::RequestProcessing
(
    SAPDBErr_MessageList &      messageList
)
{
    SAPDB_UInt4                     bytesRead;
    SAPDB_UInt4                     bytesTotal;
    RTE_ConsoleStandardDataHeader   header;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::RequestProcessing",
                             Console_Trace, 1);

    if (RTE_CONS_NO_ERROR != (rc = GetRequestData ((SAPDB_Byte *)&header, DATA_REQ, sizeof (header),
                                                    bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }
    else
    {
        SAPDBTRACE_WRITELN(Console_Trace, 5, "Connection slot "
                                             << GetServerReference ()
                                             << ": Processing request type "
                                             << header.type);

        switch (header.type)
        {
            /*===========================================================================*
             *  'Normal' requests to the database                                        *
             *===========================================================================*/

            case CON_REQUEST(CON_REQ_TEST02):
                rc = ConsoleLongTestRequest (header.type, messageList);
                break;

            case CON_REQUEST(CON_REQ_TEST01):
            case CON_REQUEST(CON_REQ_TRACE_FLUSH):
            case CON_REQUEST(CON_REQ_TRACE_CONSOLE):
            case CON_REQUEST(CON_REQ_PAGECACHE_RELEASE):
            case CON_REQUEST(CON_REQ_DIAGNOSE_PAGECACHE):
            case CON_REQUEST(CON_REQ_SET_CHECK_INTERVAL):
            case CON_REQUEST(CON_REQ_DEBUG_TASK):
            case CON_REQUEST(CON_REQ_ENABLE_TIME_MEASURE):
            case CON_REQUEST(CON_REQ_DISABLE_TIME_MEASURE):
            case CON_REQUEST(CON_REQ_RES_IO):
            case CON_REQUEST(CON_REQ_RES_REGION):
            case CON_REQUEST(CON_REQ_RES_TASK_COUNTS):
            case CON_REQUEST(CON_REQ_RES_UKT_SLEEPTIME_STAT):
            case CON_REQUEST(CON_REQ_RES_QUEUE_STAT):
            case CON_REQUEST(CON_REQ_RES_SUSPENDS):
            case CON_REQUEST(CON_REQ_RES_ALL):
#if defined (_WIN32)
            case CON_REQUEST(CON_REQ_CANCEL):
            case CON_REQUEST(CON_REQ_SET_UKT_AFFINITY):
            case CON_REQUEST(CON_REQ_SET_PROCESSOR_MASK):
            case CON_REQUEST(CON_REQ_DEBUG_BREAK):
            case CON_REQUEST(CON_REQ_KILL):
            case CON_REQUEST(CON_REQ_MINI_DUMP):
            case CON_REQUEST(CON_REQ_CRASH_DUMP):
            case CON_REQUEST(CON_REQ_GETPARAM):
            case CON_REQUEST(CON_REQ_FORCE_IO_ERROR):
            case CON_REQUEST(CON_REQ_SET_DEBUGLEVEL):
            case CON_REQUEST(CON_REQ_TOTAL_COUNTS):
            case CON_REQUEST(CON_REQ_SHOW_SUSPENDS):
            case CON_REQUEST(CON_REQ_STATE):
#endif
                rc = ConsoleSimpleRequests (header.type, messageList);
                break;

#if defined (_WIN32)
            case CON_REQUEST(CON_REQ_TASK):
                rc = GetTaskInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_CONNECTIONS):
                rc = GetConnectionsInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_IO):
                rc = GetIOInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_AIO):
                rc = GetAIOInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_REGION):
                rc = GetRegionInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_RTE):
                rc = GetRteInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_STORAGE):
                rc = GetStorageInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_UKT_SLEEPTIME_STAT):
                rc = GetUKTSlStatInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_THREAD_TIMES):
                rc = GetThreadTimes (messageList);
                break;

            case CON_REQUEST(CON_REQ_UKT_QUEUES):
                rc = GetUKTQueues (messageList);
                break;

            case CON_REQUEST(CON_REQ_QUEUE_STAT):
                rc = GetQueueInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_TASK_COUNTS):
                rc = GetTaskCountInfo (messageList);
                break;


            case CON_REQUEST(CON_REQ_AWE):
                rc = GetAWEInfo (messageList);
                break;
#endif
            case CON_REQUEST(CON_REQ_TASK_MOVE):
                rc = GetTaskMoveInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_MOVE_INFO):
                rc = GetMoveInfo (messageList);
                break;

            case CON_REQUEST(CON_REQ_RWREGIONS):
                rc = GetRWRegions (messageList);
                break;

            default:
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_REQ_TYPE_UNKNOWN,
                                                    SAPDB_ToString (header.type));
                if (!SendError (DATA_REP, RTE_CONS_ERROR, messageList))
                {
                    messageList = messageList
                                  + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_ERR);
                    return RTE_CONS_ERROR;
                }
        }

        if (RTE_CONS_ERROR == rc)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_THREAD_CONS_REQ_PROCESSING,
                                                              SAPDB_ToString (header.type));
        }
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleStandardWorker::ConsoleSimpleRequests
(
    SAPDB_UInt4 const               type,
    SAPDBErr_MessageList &          messageList
)
{
    RTE_ConsoleStandardDataPacket       reply;
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         bytesTotal;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ConsoleSimpleRequests",
                             Console_Trace, 1);

    memset (&reply, 0, sizeof (reply));


    if (RTE_CONS_NO_ERROR != (rc = GetRequestDataContinue ((SAPDB_Byte *)&reply.data, DATA_REQ, sizeof (reply.data),
                                                           bytesRead, bytesTotal, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }


    switch (type)
    {
    //TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    case CON_REQUEST(CON_REQ_TEST01):
        ConsoleTest01 (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.test01String);
        break;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    case CON_REQUEST(CON_REQ_TRACE_FLUSH):

        if (!TraceFlush (messageList))
        {
            if (!SendError (DATA_REP, RTE_CONS_ERROR, messageList))
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_ERR);
                return RTE_CONS_ERROR;
            }

            return RTE_CONS_NO_ERROR;
        }

        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_TRACE_CONSOLE):
        Console_Trace.SetTraceLevel (reply.data.SetTraceLevel.level);
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_PAGECACHE_RELEASE):
        RTEMem_SystemPageCache::Instance().ReleaseFreeBlocks();
        break;

    case CON_REQUEST(CON_REQ_DIAGNOSE_PAGECACHE):
        RTEMem_SystemPageCache::Instance().Dump(false);
        break;

    case CON_REQUEST(CON_REQ_SET_CHECK_INTERVAL):
        ShowSetCheckInterval (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.SetCheckInterval);
        break;

    case CON_REQUEST(CON_REQ_DEBUG_TASK):
        DebugTask (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.DebugTask);
        break;

    case CON_REQUEST(CON_REQ_RES_IO):
        ResetIOInfo ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_REGION):
        ResetRegionInfo ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_TASK_COUNTS):
        ResetTaskCountInfo (ALL_TASK_COUNTS);
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_UKT_SLEEPTIME_STAT):
        ResetUKTSlStatInfo ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_QUEUE_STAT):
        ResetQueueInfo ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_SUSPENDS):
        ResetSuspends ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_RES_ALL):
        ResetAll ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_ENABLE_TIME_MEASURE):
        EnableTimeMeasure ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_DISABLE_TIME_MEASURE):
        DisableTimeMeasure ();
        reply.header.dataLen        =   0;
        break;

#if defined (_WIN32)
    case CON_REQUEST(CON_REQ_CANCEL):
/*
        //New request processing should be handled like this:
        if (!CancelTask (&m_Reply.data, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_..., ...);

            if (!SendError (messageList))
            {
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_ERR);
                return false;
            }

            return true;
        }
*/

        CancelTask (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.CancelTaskCtrl);
        break;

    case CON_REQUEST(CON_REQ_SET_UKT_AFFINITY):
        SetUKTAffinity (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.UKTAffinity);
        break;

    case CON_REQUEST(CON_REQ_SET_PROCESSOR_MASK):
        SetProcessorAffinityMask (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.SetProcessorMask);
        break;

    case CON_REQUEST(CON_REQ_DEBUG_BREAK):
        DebugBreak (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.DebugBreak);
        break;

    case CON_REQUEST(CON_REQ_KILL):
        KillTask (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.KillTaskCtrl);
        break;

    case CON_REQUEST(CON_REQ_CRASH_DUMP):
        CrashDump ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_MINI_DUMP):
        MiniDump ();
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_GETPARAM):
        ShowParam (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.GetParamInfo);
        break;

    case CON_REQUEST(CON_REQ_FORCE_IO_ERROR):
        ForceIOError (&reply.data);
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_SET_DEBUGLEVEL):
        SetDebugLevel (&reply.data);
        reply.header.dataLen        =   0;
        break;

    case CON_REQUEST(CON_REQ_TOTAL_COUNTS):
        GetTotalCountInfo (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.TotalCountsInfo);
        break;

    case CON_REQUEST(CON_REQ_SHOW_SUSPENDS):
        ShowSuspends (&reply.data);
        reply.header.dataLen        =   sizeof (reply.data.SuspendReasons);
        break;

    case CON_REQUEST(CON_REQ_STATE):
        reply.data.StateInfo.ulTerminationTimeOut = kgs.ulTerminationTimeOut;
        reply.data.StateInfo.DatabaseState        = *kgs.pDBState;
        reply.header.dataLen        =   sizeof (reply.data.StateInfo);
        break;
#endif

    default:
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_REQ_TYPE_UNKNOWN,
                                                    SAPDB_ToString (type));
        return RTE_CONS_ERROR;
    }

    reply.header.rc             =   RTE_CONS_NO_ERROR;
    reply.header.lastSend       =   true;

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData ((SAPDB_Byte *)&reply, sizeof (reply.header) + reply.header.dataLen,
                                            DATA_REP, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTEThread_ConsoleStandardWorker::TraceFlush
(
    SAPDBErr_MessageList &          messageList
)
{
    RTE_KGS::KernelGlobalSection *  pKGS = RTE_KGS::Instance().GetKGS();

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::TraceFlush",
        Console_Trace, 1);

#if defined (_WIN32)
    if (pKGS->pTW->TaskState != TSK_INITIALIZED_EO00)
#else
    if (pKGS->tw->state != TSK_INITIALIZED_EO00)
#endif
    {
        // --- send a resume tracewriter request
#if defined (_WIN32)
        RESUME_TRACEWRITER(pKGS->pTW);
#else
        RESUME_TRACEWRITER(pKGS->tw);
#endif
    }
    else
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_TRACE_FLUSH);
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetMoveInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                         ulUKT;
    SAPDB_UInt2                         CurrEntry;
    SAPDB_UInt2                         Entries;
    SAPDB_ULong                         ulUKTInfoOffset;
    PCONS_SINGLE_UKT_MOVE_INFO_REC      pUKTInfos;
    SAPDB_ULong                         ulLastMovesInfoOffset;
    PCONS_LAST_MOVES_ENTRY              pLastMoves;
    SAPDB_ULong                         ulTaskMoveInfoOffset;
    PCONS_MOVE_INFO_REC                 pTaskMoveInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_UInt4                         rc   = RTE_CONS_NO_ERROR;
    RTE_KGS::KernelGlobalSection*       pKGS = RTE_KGS::Instance().GetKGS();
    SAPDB_UInt4                         numEntries;
#if defined (_WIN32)
    PUKT_CTRL_REC                       pUKTCtrl;
    PUKT_CTRL_REC                       pMoveTaskToUKT;
    SAPDB_UInt2                         PrevTimeVar = PREV_TIME_VAR(kgs.CurrTimeVar);
#else
    struct ten50_UKT_Control*           pUKTCtrl;
    struct ten50_UKT_Control*           pMoveTaskToUKT;
    SAPDB_UInt2                         PrevTimeVar = PREV_TIME_VAR(pKGS->currTimeVar);
#endif

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetMoveInfo",
                             Console_Trace, 1);

    ulTaskMoveInfoOffset        = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulLastMovesInfoOffset       = ALIGN( sizeof(*pTaskMoveInfo), ALIGNMENT_VALUE );

#if defined (_WIN32)
    numEntries = sizeof(kgs.LastTaskMoves.Entries) / sizeof(kgs.LastTaskMoves.Entries[0]);
#else
    numEntries = sizeof(pKGS->lastTaskMoves.entries) / sizeof(pKGS->lastTaskMoves.entries[0]);
#endif
    memSize = ulTaskMoveInfoOffset +
              ulLastMovesInfoOffset +
              ALIGN( (sizeof(*pLastMoves) * numEntries),       ALIGNMENT_VALUE) +
              ALIGN( (sizeof(*pUKTInfos) * pKGS->ulNumOfUKTs), ALIGNMENT_VALUE);


    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                                                         SAPDB_ToString (memSize),
                                                         SAPDB_ToString (CON_REQ_MOVE_INFO));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader             = (RTE_ConsoleStandardDataHeader *) pData;
    pTaskMoveInfo       = (PCONS_MOVE_INFO_REC)(pData + ulTaskMoveInfoOffset);
    pLastMoves          = (PCONS_LAST_MOVES_ENTRY)(pData + ulTaskMoveInfoOffset + ulLastMovesInfoOffset);


    pTaskMoveInfo->CheckIntervalTime         = XPARAM(uLoadBalancingCheck);
    pTaskMoveInfo->ValueEquality             = XPARAM(uLoadBalancingEquality);
    pTaskMoveInfo->MinimumDiff               = XPARAM(uLoadBalancingDiff);
#if defined (_WIN32)
    pTaskMoveInfo->TaskMoveIntervalStartTime = pKGS->TaskMoveIntervalStartTimes[pKGS->CurrTimeVar];
    pTaskMoveInfo->TaskMoveMeasIntervals     = pKGS->TaskMoveMeasIntervals;
#else
    pTaskMoveInfo->TaskMoveIntervalStartTime = pKGS->taskMoveIntervalStartTimes[pKGS->currTimeVar];
    pTaskMoveInfo->TaskMoveMeasIntervals     = pKGS->taskMoveMeasIntervals;
#endif


#if defined (_WIN32)
    WAIT_UNTIL_ALONE(&kgs.LastTaskMoves.lExclusive);

    for ( CurrEntry = NEXT_ENTRY( kgs.LastTaskMoves.CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS), Entries = 0;
          CurrEntry != kgs.LastTaskMoves.CurrEntry;
          CurrEntry = NEXT_ENTRY( CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS) )
    {
        if (( kgs.LastTaskMoves.Entries[CurrEntry].MoveTime  != 0 )    &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pTaskCtrl != NULL ) &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pSrcUKT   != NULL ) &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pDstUKT   != NULL ))
        {
            pLastMoves[Entries].MoveTime                    = kgs.LastTaskMoves.Entries[CurrEntry].MoveTime;
            pLastMoves[Entries].ulTaskIndex                 = kgs.LastTaskMoves.Entries[CurrEntry].pTaskCtrl->ulTaskIndex;
            pLastMoves[Entries].TaskMaxRunnableMicroSeconds = kgs.LastTaskMoves.Entries[CurrEntry].TaskMaxRunnableMicroSeconds;
            pLastMoves[Entries].ulSrcUKTIndex               = kgs.LastTaskMoves.Entries[CurrEntry].pSrcUKT->ulUKTIndex;
            pLastMoves[Entries].SrcMaxRunnableMicroSeconds  = kgs.LastTaskMoves.Entries[CurrEntry].SrcMaxRunnableMicroSeconds;
            pLastMoves[Entries].ulDstUKTIndex               = kgs.LastTaskMoves.Entries[CurrEntry].pDstUKT->ulUKTIndex;
            pLastMoves[Entries].DstMaxRunnableMicroSeconds  = kgs.LastTaskMoves.Entries[CurrEntry].DstMaxRunnableMicroSeconds;
            Entries++;
        }
    }
    CLEARLOCK(&kgs.LastTaskMoves.lExclusive);
#else
    WAIT_UNTIL_ALONE(pKGS->lastTaskMoves.exclusive);

    for ( CurrEntry = NEXT_ENTRY( pKGS->lastTaskMoves.currEntry, NUM_OF_TASK_MOVE_STAT_RECS), Entries = 0;
          CurrEntry != pKGS->lastTaskMoves.currEntry;
          CurrEntry = NEXT_ENTRY( CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS) )
    {
        if (( pKGS->lastTaskMoves.entries[CurrEntry].moveTime  != 0 )    &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].task      != NULL ) &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].srcUKT    != NULL ) &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].dstUKT    != NULL ))
        {
            pLastMoves[Entries].MoveTime                    = pKGS->lastTaskMoves.entries[CurrEntry].moveTime;
            pLastMoves[Entries].ulTaskIndex                 = pKGS->lastTaskMoves.entries[CurrEntry].task->index;
            pLastMoves[Entries].TaskMaxRunnableMicroSeconds = pKGS->lastTaskMoves.entries[CurrEntry].taskMaxRunnableMicroSeconds;
            pLastMoves[Entries].ulSrcUKTIndex               = pKGS->lastTaskMoves.entries[CurrEntry].srcUKT->index;
            pLastMoves[Entries].SrcMaxRunnableMicroSeconds  = pKGS->lastTaskMoves.entries[CurrEntry].srcMaxRunnableMicroSeconds;
            pLastMoves[Entries].ulDstUKTIndex               = pKGS->lastTaskMoves.entries[CurrEntry].dstUKT->index;
            pLastMoves[Entries].DstMaxRunnableMicroSeconds  = pKGS->lastTaskMoves.entries[CurrEntry].dstMaxRunnableMicroSeconds;
            Entries++;
        }
    }
    CLEARLOCK(pKGS->lastTaskMoves.exclusive);
#endif


    pTaskMoveInfo->lNumOfLastRecs        = Entries;
    pTaskMoveInfo->ulLastMovesInfoOffset = ulLastMovesInfoOffset;

    ulUKTInfoOffset = ALIGN( sizeof(CONS_LAST_MOVES_ENTRY) * Entries, ALIGNMENT_VALUE) + ulLastMovesInfoOffset;
    pUKTInfos       = (PCONS_SINGLE_UKT_MOVE_INFO_REC)(pData + ulTaskMoveInfoOffset + ulUKTInfoOffset);
    //
    // --- UKT Info
    //
#if defined (_WIN32)
    for ( pUKTCtrl =  kgs.pFirstUKTCtrl, ulUKT = 0;
          pUKTCtrl <= kgs.pLastUKTCtrl;
          pUKTCtrl++  )
    {
        if ( pUKTCtrl->TasksAllowedToMoveTo )
        {
            pMoveTaskToUKT                             = pUKTCtrl->pMoveTaskToUKT;

            pUKTInfos[ulUKT].ulUKTIndex                = pUKTCtrl->ulUKTIndex;
            pUKTInfos[ulUKT].ulMoveTaskToUKTUKTIndex   = pMoveTaskToUKT ? pMoveTaskToUKT->ulUKTIndex : -1;
            pUKTInfos[ulUKT].NumOfMovableTasks         = pUKTCtrl->NumOfMovableTasks;
            pUKTInfos[ulUKT].TaskMoveIntervalStartTime = pUKTCtrl->TaskMoveIntervalStartTime;
            pUKTInfos[ulUKT].UKTSleepTimeMicroSeconds  = pUKTCtrl->UKTSleepTimeMicroSeconds[PrevTimeVar];
            pUKTInfos[ulUKT].RunnableMicroSeconds      = pUKTCtrl->RunnableMicroSeconds[PrevTimeVar];
            pUKTInfos[ulUKT].MaxRunnableMicroSeconds   = pUKTCtrl->MaxRunnableMicroSeconds[PrevTimeVar];
            ulUKT++;
        }
    }
#else
    for ( pUKTCtrl =  pKGS->first_kp, ulUKT = 0;
          pUKTCtrl <= pKGS->last_kp;
          pUKTCtrl++  )
    {
        if ( pUKTCtrl->tasksAllowedToMoveTo )
        {
            pMoveTaskToUKT                             = pUKTCtrl->moveTaskToUKT;

            pUKTInfos[ulUKT].ulUKTIndex                = pUKTCtrl->index;
            pUKTInfos[ulUKT].ulMoveTaskToUKTUKTIndex   = pMoveTaskToUKT ? pMoveTaskToUKT->index : -1;
            pUKTInfos[ulUKT].NumOfMovableTasks         = pUKTCtrl->numOfMovableTasks;
            pUKTInfos[ulUKT].TaskMoveIntervalStartTime = pUKTCtrl->taskMoveIntervalStartTime;
            pUKTInfos[ulUKT].UKTSleepTimeMicroSeconds  = pUKTCtrl->UKTSleepTimeMicroSeconds[PrevTimeVar];
            pUKTInfos[ulUKT].RunnableMicroSeconds      = pUKTCtrl->runnableMicroSeconds[PrevTimeVar];
            pUKTInfos[ulUKT].MaxRunnableMicroSeconds   = pUKTCtrl->maxRunnableMicroSeconds[PrevTimeVar];
            ulUKT++;
        }
    }
#endif

    pTaskMoveInfo->ulUKTInfoOffset = ulUKTInfoOffset;
    pTaskMoveInfo->lNumOfUKTs      = ulUKT;

    dataSize = ulTaskMoveInfoOffset +
               ulLastMovesInfoOffset +
               ALIGN(sizeof(*pLastMoves) * Entries, ALIGNMENT_VALUE) +
               ALIGN(sizeof(*pUKTInfos) * ulUKT, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulTaskMoveInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetTaskMoveInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulIdx;
    PCONS_TASK_MOVE_INFO_REC        pTaskMove;
    PCONS_SINGLE_TASK_MOVE_INFO_REC pTaskMoveInfos;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_ULong                     ulTaskMoveOffset;
    SAPDB_ULong                     ulTaskMoveInfoOffset;
    SAPDB_Byte *                    pData;
    SAPDB_UInt4                     bytesRead;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_ULong                     memSize;
    SAPDB_UInt4                     rc   = RTE_CONS_NO_ERROR;
    SAPDB_ULong                     dataSize;
    RTE_KGS::KernelGlobalSection*   pKGS = RTE_KGS::Instance().GetKGS();
#if defined (_WIN32)
    PTASK_CTRL_REC                  pCurrTaskCtrl;
    SAPDB_UInt2                     PrevTimeVar = PREV_TIME_VAR(kgs.CurrTimeVar);
#else
    struct TASK_TYPE*               pCurrTaskCtrl;
    SAPDB_UInt2                     PrevTimeVar = PREV_TIME_VAR(pKGS->currTimeVar);
#endif

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetTaskMoveInfo",
                             Console_Trace, 1);

    ulTaskMoveOffset     = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulTaskMoveInfoOffset = ALIGN( sizeof(*pTaskMove), ALIGNMENT_VALUE);

    memSize = ulTaskMoveOffset     +
              ulTaskMoveInfoOffset +
              ALIGN( sizeof(*pTaskMoveInfos) *  pKGS->ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);

    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                                                         SAPDB_ToString (memSize),
                                                         SAPDB_ToString (CON_REQ_TASK_MOVE));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader          = (RTE_ConsoleStandardDataHeader *) pData;
    pTaskMove        = (PCONS_TASK_MOVE_INFO_REC)        (pData + ulTaskMoveOffset);
    pTaskMoveInfos   = (PCONS_SINGLE_TASK_MOVE_INFO_REC) (pData + ulTaskMoveOffset + ulTaskMoveInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = GetRequestDataContinue ((SAPDB_Byte *)pTaskMove, DATA_REQ, sizeof (*pTaskMove),
                                                           bytesRead, bytesTotal, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }

    for (pCurrTaskCtrl =  pKGS->pFirstTaskCtrl, ulIdx = 0;
         pCurrTaskCtrl <= pKGS->pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
#       if defined (_WIN32)
         if ( IS_TASK_TO_SHOW_EO92(pCurrTaskCtrl->TaskType,
                                   pCurrTaskCtrl->TaskState,
                                   pCurrTaskCtrl->ulStateReason,
                                   pCurrTaskCtrl->TaskOMSStat.OMSCallActive,
                                   pCurrTaskCtrl->QueLocation,
                                   SHOW_TT_ALL_EO92,
                                   SHOW_ALL_EO92 ) )
         {
            pTaskMoveInfos[ulIdx].ulTaskIndex             = pCurrTaskCtrl->ulTaskIndex;
            pTaskMoveInfos[ulIdx].TaskState               = pCurrTaskCtrl->TaskState;
            pTaskMoveInfos[ulIdx].TaskType                = pCurrTaskCtrl->TaskType;
            strcpy (pTaskMoveInfos[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString (false));
            pTaskMoveInfos[ulIdx].ulStateReason           = pCurrTaskCtrl->ulStateReason ;
            pTaskMoveInfos[ulIdx].ulUktIndex              = pCurrTaskCtrl->pUKT->ulUKTIndex ;
            pTaskMoveInfos[ulIdx].TemporaryMoveLock       = pCurrTaskCtrl->MoveLock.Type.Temporary;
            pTaskMoveInfos[ulIdx].LongTermMoveLock        = pCurrTaskCtrl->MoveLock.Type.LongTerm;
            pTaskMoveInfos[ulIdx].RunnableMicroSeconds    = pCurrTaskCtrl->RunnableMicroSeconds[PrevTimeVar];
            pTaskMoveInfos[ulIdx].MaxRunnableMicroSeconds = pCurrTaskCtrl->MaxRunnableMicroSeconds[PrevTimeVar];
            ulIdx++;
         }
#else
         if ( IS_TASK_TO_SHOW_EO92(pCurrTaskCtrl->type,
                                   pCurrTaskCtrl->state,
                                   pCurrTaskCtrl->state_reason,
                                   pCurrTaskCtrl->TaskOMSStat.OMSCallActive,             /* PTS 1116933 */
                                   pCurrTaskCtrl->is_in_queue,
                                   SHOW_TT_ALL_EO92,
                                   SHOW_ALL_EO92 ) )
         {
            pTaskMoveInfos[ulIdx].ulTaskIndex             = pCurrTaskCtrl->index;
            pTaskMoveInfos[ulIdx].TaskState               = pCurrTaskCtrl->state;
            pTaskMoveInfos[ulIdx].TaskType                = pCurrTaskCtrl->type;
            strcpy (pTaskMoveInfos[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString (false));
            pTaskMoveInfos[ulIdx].ulStateReason           = pCurrTaskCtrl->state_reason;
            pTaskMoveInfos[ulIdx].ulUktIndex              = pCurrTaskCtrl->ukt->index;
            pTaskMoveInfos[ulIdx].TemporaryMoveLock       = pCurrTaskCtrl->moveLock.type.temporary;
            pTaskMoveInfos[ulIdx].LongTermMoveLock        = pCurrTaskCtrl->moveLock.type.longTerm;
            pTaskMoveInfos[ulIdx].RunnableMicroSeconds    = pCurrTaskCtrl->runnableMicroSeconds[PrevTimeVar];
            pTaskMoveInfos[ulIdx].MaxRunnableMicroSeconds = pCurrTaskCtrl->maxRunnableMicroSeconds[PrevTimeVar];
            ulIdx++;
         }
#endif
    }
    pTaskMove->ulTaskMoveInfoOffset  = (SAPDB_ULong)ulTaskMoveInfoOffset;
    pTaskMove->lNumOfActiveTasks     = (SAPDB_Long)ulIdx;

    dataSize = ulTaskMoveOffset +
               ulTaskMoveInfoOffset +
               ALIGN( sizeof(*pTaskMoveInfos) * ulIdx, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulTaskMoveOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }
    RTEMem_RteAllocator::Instance().Deallocate(pData);

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

static void BuildTaskIDListForRWRegion(void *pAddress,RTE_TaskId *taskIds)
{
    SAPDB_UInt                      index = 0;
    TASK_CTRL                       pCurrTaskCtrl;
    RTE_KGS::KernelGlobalSection*   pKGS = RTE_KGS::Instance().GetKGS();
    memset(taskIds,0,pKGS->ulNumOfTasks * sizeof(RTE_TaskId));
    for (pCurrTaskCtrl =  pKGS->pFirstTaskCtrl;
         pCurrTaskCtrl <= pKGS->pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        RTETask_Task *pTaskObject = pCurrTaskCtrl->pTaskObject;
        if(pAddress == pTaskObject->RWRegion() && RTETask_Task::TaskEnteringRWRegion != pTaskObject->State())
        {
            taskIds[index] = pTaskObject->ID();
            index++;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleStandardWorker::SendRWRegionPacket(SAPDB_Byte *pData,SAPDB_ULong dataSize,SAPDBErr_MessageList &messageList)
{
    static SAPDB_Bool firstPacketInSequence = true;
    SAPDB_UInt4 rc;
    RTE_ConsoleStandardDataHeader *pHeader = (RTE_ConsoleStandardDataHeader *) pData;
    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT));

    if(firstPacketInSequence)
    {
        rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList);
        firstPacketInSequence = false;
    }
    else
    {
        rc = SendReplyDataNext (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList);
    }
    if( pHeader->lastSend ) // if this packet was the last one in the sequence, the next one will start a new sequence
    {
        firstPacketInSequence = true;
    }
    if (RTE_CONS_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
    }
    return rc;
}

/*---------------------------------------------------------------------------*/

#define RWREGION_PACKET_SIZE_    256*1024

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetRWRegions
(

    SAPDBErr_MessageList &              messageList
)
{
    SAPDB_Byte *                        pData=NULL;
    SAPDB_ULong                         memSize;
    SAPDB_UInt4                         rc   = RTE_CONS_NO_ERROR;
    SAPDB_ULong                         dataSize=0;
    RTE_ConsoleStandardDataHeader       *pHeader;
    PCONS_SINGLE_RWREGION_INFO_REC      pCurrentRWRegionInfo;
    PCONS_SINGLE_SPINLOCKPOOL_INFO_REC  pCurrentSpinlockPoolInfo;
    PCONS_RWREGION_INFO_REC             pRWRegionHeader;

    TASK_CTRL                           pCurrTaskCtrl;
    RTE_KGS::KernelGlobalSection*       pKGS = RTE_KGS::Instance().GetKGS();
    SAPDB_ULong                         ulIdx;

    memSize = RWREGION_PACKET_SIZE_;
    // allocate the memory for the transfer buffer
    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);

    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                                                         SAPDB_ToString (memSize),
                                                         SAPDB_ToString (CON_REQ_TASK_MOVE));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader                 = (RTE_ConsoleStandardDataHeader *) pData;
    pHeader->lastSend       =   false;
    pRWRegionHeader         = (PCONS_RWREGION_INFO_REC)(pData + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT));


    // read the "in"-parameters
    SAPDB_UInt4                     bytesRead;
    SAPDB_UInt4                     bytesTotal;

    if (RTE_CONS_NO_ERROR != (rc = GetRequestDataContinue ((SAPDB_Byte *)pRWRegionHeader, DATA_REQ, sizeof (*pRWRegionHeader),
                                                            bytesRead, bytesTotal, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }
    RTESync_IRWRegionReader::Selection selection;
    if(pRWRegionHeader->info.showAll)
    {
        selection = RTESync_IRWRegionReader::all;
    }
    else
    {
        selection = RTESync_IRWRegionReader::usedOnesOnly;
    }

// step "zero": the spinlock pools
    pRWRegionHeader->info.outParams.numberOfSpinlockPools = 0;
    pRWRegionHeader->info.outParams.ulInfoNum = 0;
    pCurrentSpinlockPoolInfo = (PCONS_SINGLE_SPINLOCKPOOL_INFO_REC)(
                                                        pData
                                                      + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                      + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                   );

    SAPDB_ULong maxSpinlockPoolInfos =( RWREGION_PACKET_SIZE_
                                        - SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                        - SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                      )
                                      / sizeof(CONS_SINGLE_SPINLOCKPOOL_INFO_REC);
    RTESync_SpinlockPoolReader  SpinlockPoolReader;

    RTESync_RWRegionRegister **pRWRegionRegisters = NULL;
    RTESync_RWRegionRegister *pCurrentRWRegionRegister;
    SAPDB_ULong numberOfSpinlockPools = 0;

    RTESync_LockedScope lockedScope(RTESync_SpinlockPoolRegister::Instance().Spinlock());   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139124
    if (SpinlockPoolReader.First(RTESync_Spinlock::normal, RTESync_SpinlockPoolRegister::unsynchronized ))
    {
        numberOfSpinlockPools = RTESync_SpinlockPoolRegister::Instance().GetCount();
        SAPDB_ULong numberOfSpinlockPoolsToSend = numberOfSpinlockPools;
    
        pRWRegionRegisters = (RTESync_RWRegionRegister **)RTEMem_RteAllocator::Instance().Allocate( numberOfSpinlockPoolsToSend * sizeof(RTESync_RWRegionRegister *) );

        SAPDB_ULong numberOfPackets = (numberOfSpinlockPoolsToSend-1)/maxSpinlockPoolInfos + 1;
        for(SAPDB_UInt packetCounter = 0;packetCounter < numberOfPackets ;packetCounter++)
        {
            for(SAPDB_UInt spinlockPookCounter = 0;spinlockPookCounter < SAPDB_MIN(maxSpinlockPoolInfos,numberOfSpinlockPoolsToSend) ;spinlockPookCounter++)
            {
                SpinlockPoolReader.Next();
                SpinlockPoolReader.Name(pCurrentSpinlockPoolInfo->m_Name);
                SpinlockPoolReader.RWRegionRegister(pCurrentRWRegionRegister);
                pRWRegionRegisters[packetCounter * maxSpinlockPoolInfos + spinlockPookCounter] = (RTESync_RWRegionRegister *)pCurrentRWRegionRegister;
                pCurrentSpinlockPoolInfo++;
            }
            pRWRegionHeader->info.outParams.numberOfSpinlockPools = SAPDB_MIN(maxSpinlockPoolInfos,numberOfSpinlockPoolsToSend);
            if( numberOfSpinlockPoolsToSend > maxSpinlockPoolInfos )
            {
                dataSize =  SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pCurrentRWRegionInfo) *  pRWRegionHeader->info.outParams.ulInfoNum, SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pCurrentSpinlockPoolInfo) *  pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT);

                if (RTE_CONS_NO_ERROR != (rc = SendRWRegionPacket(pData,dataSize,messageList)))
                {
                    RTEMem_RteAllocator::Instance().Deallocate(pData);
                    if(pRWRegionRegisters)
                        RTEMem_RteAllocator::Instance().Deallocate(pRWRegionRegisters);
                    return rc;
                }

                numberOfSpinlockPoolsToSend -= maxSpinlockPoolInfos;
                pCurrentSpinlockPoolInfo = (PCONS_SINGLE_SPINLOCKPOOL_INFO_REC)(
                                                                                    pData
                                                                                    + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                                                    + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                                                );
            }
        }
    }


    // prepare the transfer of the rwregion infos
    pCurrentRWRegionInfo = (PCONS_SINGLE_RWREGION_INFO_REC)(
                                                        pData
                                                      + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                      + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                      + SAPDB_ALIGN( sizeof(*pCurrentSpinlockPoolInfo) * pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT)
                                                      );

    SAPDB_ULong maxInfos =( (SAPDB_Byte *)pData + RWREGION_PACKET_SIZE_ - (SAPDB_Byte *)pCurrentRWRegionInfo )
                           / sizeof(CONS_SINGLE_RWREGION_INFO_REC);
// step one: check the task control blocks for tasks that are waiting for rwregions
    for (pCurrTaskCtrl =  pKGS->pFirstTaskCtrl, ulIdx = 0;
         pCurrTaskCtrl <= pKGS->pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        RTETask_Task *pTaskObject = pCurrTaskCtrl->pTaskObject;
        if(RTETask_Task::TaskEnteringRWRegion == pTaskObject->State())
        {
            if( pRWRegionHeader->info.outParams.ulInfoNum >= maxInfos )
            {
                dataSize =  SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pCurrentRWRegionInfo) *  pRWRegionHeader->info.outParams.ulInfoNum, SAPDB_ALIGNMENT) +
                            ALIGN( sizeof(*pCurrentSpinlockPoolInfo) *  pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT);

                if (RTE_CONS_NO_ERROR != (rc = SendRWRegionPacket(pData,dataSize,messageList)))
                {
                    RTEMem_RteAllocator::Instance().Deallocate(pData);
                    if(pRWRegionRegisters)
                        RTEMem_RteAllocator::Instance().Deallocate(pRWRegionRegisters);
                    return rc;
                }
                pRWRegionHeader->info.outParams.ulInfoNum = 0;
                pRWRegionHeader->info.outParams.numberOfSpinlockPools = 0;
                pCurrentRWRegionInfo = (PCONS_SINGLE_RWREGION_INFO_REC)(
                                                                    pData
                                                                + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                                + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                            );
                maxInfos =( RWREGION_PACKET_SIZE_
                            - SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                            - SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                        )
                        / sizeof(CONS_SINGLE_RWREGION_INFO_REC);
            }
            RTESync_IRWRegion *pCurrentRWRegion = (RTESync_IRWRegion *)pTaskObject->RWRegion();
            pCurrentRWRegionInfo->pAddress = pCurrentRWRegion;
            if( pCurrentRWRegion )
            {
                pCurrentRWRegionInfo->collisionCounter = pCurrentRWRegion->GetCollisionCounter();
                pCurrentRWRegionInfo->exclusiveAccessCounter = pCurrentRWRegion->GetExclusiveAccessCounter();
                pCurrentRWRegionInfo->nonExclusiveAccessCounter = pCurrentRWRegion->GetNonExclusiveAccessCounter();
                pCurrentRWRegionInfo->waitCounter = pCurrentRWRegion->GetWaitCounter();
                pCurrentRWRegionInfo->tasCounter = pCurrentRWRegion->GetTasCounter();
                // find the index of spinlock pool
                for(SAPDB_UInt i=0;i < pRWRegionHeader->info.outParams.numberOfSpinlockPools;i++)
                {
                    if ( pCurrentRWRegion->GetSpinlockPool()->GetRWRegionRegister() == pRWRegionRegisters[i] )
                    {
                        pCurrentRWRegionInfo->spinlockPoolIndex = i;
                    }
                }
            }
            pCurrentRWRegionInfo->taskID = pTaskObject->ID();
            if(pTaskObject->TaskIsWaitingForExclusiveAccess())
            {
                pCurrentRWRegionInfo->access = CONS_SINGLE_RWREGION_INFO_REC::waitingForExclusive;
            }
            else
            {
                pCurrentRWRegionInfo->access = CONS_SINGLE_RWREGION_INFO_REC::waitingForNonExclusive;
            }
            pCurrentRWRegionInfo++;
            pRWRegionHeader->info.outParams.ulInfoNum++;
        }
    }
// step two: check the rwregions for information about tasks that are using them
    RTE_TaskId *taskIDs = (RTE_TaskId *)alloca(pKGS->ulNumOfTasks * sizeof(RTE_TaskId));

    SAPDB_UInt8 address;
    SAPDB_Int4 lastEnteredTask;
    SAPDB_UInt8 exclusiveAccessCount;
    SAPDB_UInt8 nonExclusiveAccessCount;
    SAPDB_UInt8 collisionCount;
    SAPDB_UInt8 waitCount;
    SAPDB_UInt8 spinLockCollisionCount;
    SAPDB_Int4 currentAccesses;
    CONS_SINGLE_RWREGION_INFO_REC::RWRegionAccess accessState;

    RTESync_IRWRegionReader *pReader;
    SAPDB_UInt currentSpinlockPoolIndex;
    if (SpinlockPoolReader.First(RTESync_Spinlock::normal, RTESync_SpinlockPoolRegister::unsynchronized ))
    {
        while(SpinlockPoolReader.Next())
        {
            RTESync_RWRegionRegister *pRWRegionRegister;

            if(SpinlockPoolReader.RWRegionRegister(pRWRegionRegister))  // if the caller supplied us with a spinlock name (to be developed...), it should be compared with the name of the current spinlock here
            {
                for(SAPDB_UInt spinlockPookCounter=0;spinlockPookCounter < numberOfSpinlockPools;spinlockPookCounter++)
                {
                    if ( pRWRegionRegister == pRWRegionRegisters[spinlockPookCounter] )
                    {
                        currentSpinlockPoolIndex = spinlockPookCounter;
                    }
                }
                pReader = RTESync_GetRWRegionReader(*pRWRegionRegister,RTEMem_RteAllocator::Instance());

                if (pReader->First(RTESync_IRWRegionReader::normal,selection))
                {
                    while(pReader->Next())
                    {
                        pReader->CurrentAccesses(currentAccesses);
                        switch(currentAccesses)
                        {
                        case -1:
                            accessState = CONS_SINGLE_RWREGION_INFO_REC::holdingExclusive;
                            break;
                        case 0:
                            accessState = CONS_SINGLE_RWREGION_INFO_REC::none;
                            break;
                        default:
                            accessState = CONS_SINGLE_RWREGION_INFO_REC::holdingNonExclusive;
                            break;
                        }
                        pReader->Address(address);
                        pReader->LastEnteredTask(lastEnteredTask);
                        pReader->CollisionCount(collisionCount);
                        pReader->ExclusiveAccessCount(exclusiveAccessCount);
                        pReader->NonExclusiveAccessCount(nonExclusiveAccessCount);
                        pReader->WaitCount(waitCount);
                        pReader->SpinLockCollisionCount(spinLockCollisionCount);

                        if(accessState == CONS_SINGLE_RWREGION_INFO_REC::none)
                        {
                            if( pRWRegionHeader->info.outParams.ulInfoNum >= maxInfos )
                            {
                                dataSize =  SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT) +
                                            ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT) +
                                            ALIGN( sizeof(*pCurrentRWRegionInfo) *  pRWRegionHeader->info.outParams.ulInfoNum, SAPDB_ALIGNMENT) +
                                            ALIGN( sizeof(*pCurrentSpinlockPoolInfo) *  pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT);

                                if (RTE_CONS_NO_ERROR != (rc = SendRWRegionPacket(pData,dataSize,messageList)))
                                {
                                    RTEMem_RteAllocator::Instance().Deallocate(pData);
                                    if(pRWRegionRegisters)
                                        RTEMem_RteAllocator::Instance().Deallocate(pRWRegionRegisters);
                                    return rc;
                                }

                                pRWRegionHeader->info.outParams.ulInfoNum = 0;
                                pRWRegionHeader->info.outParams.numberOfSpinlockPools = 0;
                                pCurrentRWRegionInfo = (PCONS_SINGLE_RWREGION_INFO_REC)(
                                                                                    pData
                                                                                + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                                                + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                                            );

                                maxInfos =( RWREGION_PACKET_SIZE_
                                            - SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                            - SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                        )
                                        / sizeof(CONS_SINGLE_RWREGION_INFO_REC);
                            }
                            pCurrentRWRegionInfo->access = accessState;
                            pCurrentRWRegionInfo->taskID = 0;
                            pCurrentRWRegionInfo->pAddress = (void *)address;
                            pCurrentRWRegionInfo->exclusiveAccessCounter = (SAPDB_UInt4)exclusiveAccessCount;
                            pCurrentRWRegionInfo->nonExclusiveAccessCounter = (SAPDB_UInt4)nonExclusiveAccessCount;
                            pCurrentRWRegionInfo->collisionCounter = (SAPDB_UInt4)collisionCount;
                            pCurrentRWRegionInfo->waitCounter = (SAPDB_UInt4)waitCount;
                            pCurrentRWRegionInfo->tasCounter = (SAPDB_UInt4)spinLockCollisionCount;
                            pCurrentRWRegionInfo->spinlockPoolIndex = currentSpinlockPoolIndex;
                            pCurrentRWRegionInfo++;
                            pRWRegionHeader->info.outParams.ulInfoNum++;
                        }
                        else
                        {
                            /* if some tasks are using the rwregion, the rwregion can only tell us which one    */
                            /* entered it last, if that one has not left the rwregion yet.                      */
                            /* In order to produce as much information as possible, the task list is checked    */
                            /* for tasks that inidicate that they use this rwregion                             */
                            BuildTaskIDListForRWRegion((void *)address,taskIDs);

                            // abs() is used because: if one task is using the rwregion exclusively, currentAccesses is -1.
                            // If tasks are using the rwregion non-exclusively, currentAccesses is the number of those tasks.
                            for(SAPDB_UInt i=0;i< (SAPDB_UInt)abs(currentAccesses);i++)
                            {
                                if( pRWRegionHeader->info.outParams.ulInfoNum >= maxInfos )
                                {
                                    dataSize =  SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT) +
                                                ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT) +
                                                ALIGN( sizeof(*pCurrentRWRegionInfo) *  pRWRegionHeader->info.outParams.ulInfoNum, SAPDB_ALIGNMENT) +
                                                ALIGN( sizeof(*pCurrentSpinlockPoolInfo) *  pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT);

                                    if (RTE_CONS_NO_ERROR != (rc = SendRWRegionPacket(pData,dataSize,messageList)))
                                    {
                                        RTEMem_RteAllocator::Instance().Deallocate(pData);
                                        if(pRWRegionRegisters)
                                            RTEMem_RteAllocator::Instance().Deallocate(pRWRegionRegisters);
                                        return rc;
                                    }

                                    pRWRegionHeader->info.outParams.ulInfoNum = 0;
                                    pRWRegionHeader->info.outParams.numberOfSpinlockPools = 0;
                                    pCurrentRWRegionInfo = (PCONS_SINGLE_RWREGION_INFO_REC)(
                                                                                        pData
                                                                                    + SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                                                    + SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                                                                );

                                    maxInfos =( RWREGION_PACKET_SIZE_
                                                - SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT)
                                                - SAPDB_ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT)
                                            )
                                            / sizeof(CONS_SINGLE_RWREGION_INFO_REC);
                                }
                                pCurrentRWRegionInfo->access = accessState;
                                if(0 != taskIDs[i])
                                {
                                    pCurrentRWRegionInfo->taskID = taskIDs[i];
                                }
                                else    // use lastEnteredTask if not enough entries are found in the task id list
                                {
                                    if(lastEnteredTask != -1)
                                    {
                                        pCurrentRWRegionInfo->taskID = lastEnteredTask;
                                        lastEnteredTask = -1;  // use lastEnteredTask only once
                                    }
                                    else
                                    {
                                        pCurrentRWRegionInfo->taskID = -1;
                                    }
                                }
                                // if lastEnteredTask is also contained in the task id list, do not use
                                if(lastEnteredTask  == taskIDs[i])
                                {
                                    lastEnteredTask = -1;
                                }
                                pCurrentRWRegionInfo->pAddress = (void *)address;
                                pCurrentRWRegionInfo->exclusiveAccessCounter = (SAPDB_UInt4)exclusiveAccessCount;
                                pCurrentRWRegionInfo->nonExclusiveAccessCounter = (SAPDB_UInt4)nonExclusiveAccessCount;
                                pCurrentRWRegionInfo->collisionCounter = (SAPDB_UInt4)collisionCount;
                                pCurrentRWRegionInfo->waitCounter = (SAPDB_UInt4)waitCount;
                                pCurrentRWRegionInfo->tasCounter = (SAPDB_UInt4)spinLockCollisionCount;
                                pCurrentRWRegionInfo->spinlockPoolIndex = currentSpinlockPoolIndex;

                                pCurrentRWRegionInfo++;
                                pRWRegionHeader->info.outParams.ulInfoNum++;
                            }
                        }
                    }
                }
                (RTEMem_RteAllocator::Instance()).Deallocate(pReader);
            }
        }

    }

    dataSize =  SAPDB_ALIGN( sizeof(*pHeader), SAPDB_ALIGNMENT) +
                ALIGN( sizeof(*pRWRegionHeader), SAPDB_ALIGNMENT) +
                ALIGN( sizeof(*pCurrentRWRegionInfo) *  pRWRegionHeader->info.outParams.ulInfoNum, SAPDB_ALIGNMENT) +
                ALIGN( sizeof(*pCurrentSpinlockPoolInfo) *  pRWRegionHeader->info.outParams.numberOfSpinlockPools, SAPDB_ALIGNMENT);

    pHeader->lastSend       =   true;
    SendRWRegionPacket(pData,dataSize,messageList);

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    if(pRWRegionRegisters)
        RTEMem_RteAllocator::Instance().Deallocate(pRWRegionRegisters);

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ShowSetCheckInterval
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ShowSetCheckInterval",
                             Console_Trace, 1);

    if ( XPARAM(fForbidLoadBalancing)
      || (pConsoleDataRec->SetCheckInterval.CheckIntervalTime != 0 && pConsoleDataRec->SetCheckInterval.CheckIntervalTime < 2) )
    {
        pConsoleDataRec->SetCheckInterval.usReturnCode = CON_WRONG_VALUE;
    }
    else
    {
        XPARAM(uLoadBalancingCheck) = pConsoleDataRec->SetCheckInterval.CheckIntervalTime;

#       if defined(_WIN32)
         sql72k_wake_kt ( &kgs.Timer.ThrdCtrlHeader );
#       else
         en81TimerThreadWakeup();
#       endif

        pConsoleDataRec->SetCheckInterval.usReturnCode = CON_NO_ERROR;
    }

    return;
}

/*---------------------------------------------------------------------------*/
#ifdef _WIN32
#define DISPATCHER_CALLS(_pUKT) (_pUKT)->DispatcherCalls
#else
#define DISPATCHER_CALLS(_pUKT) (_pUKT)->disp_counter
#endif

void RTEThread_ConsoleStandardWorker::DebugTask
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDB_ULong      ulTask = pConsoleDataRec->DebugTask.ulTask;
    TASK_CTRL        pTaskCtrl;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::DebugTask",
                             Console_Trace, 1);

    if (( ulTask < 1 ) || ( ulTask > KGS->ulNumOfTasks ))
    {
        pConsoleDataRec->DebugTask.usReturnCode = CON_WRONG_TASK;
    }
    else
    {
        pTaskCtrl = KGS->pFirstTaskCtrl + ulTask - 1;

        if ((( TASK_TYPE(pTaskCtrl) == TT_UT_EO00 ) ||
            (  TASK_TYPE(pTaskCtrl) == TT_EV_EO00 ) ||
            (  TASK_TYPE(pTaskCtrl) == TT_SV_EO00 ) ||
            (  TASK_TYPE(pTaskCtrl) == TT_US_EO00 )) &&
            (  TASK_TYPE(pTaskCtrl) == TSK_INACTIVE_EO00 ))
        {
            pConsoleDataRec->DebugTask.usReturnCode = CON_TASK_INACTIVE;
        }
        else
        {
#ifdef _WIN32
            RTETask_LegacyUKTCtrl &currentUKT = RTETask_Task::ByTaskID(ulTask)->TaskScheduler().LegacyUKTCtrl();
            SAPDB_UInt8 DispatcherCalls = currentUKT.dispatcherCalls();
            SAPDB_Int   i;

            // --- maximum wait time 10 seconds
            for ( i = 0;
                  i < (10 * 4) &&
                  DispatcherCalls == currentUKT.dispatcherCalls() &&
                  pTaskCtrl->pUKT->ThrdCtrlHeader.ThrdState != KT_SLEEPING;
                  i++ )
            {
                SLEEP (250); // sleep 250 msec
            }

            if (( currentUKT.dispatcherCalls()              == DispatcherCalls ) &&
                ( pTaskCtrl->pUKT->ThrdCtrlHeader.ThrdState != KT_SLEEPING ))
            { // the UKT might essentially stuck in an endless loop
                MSGD(( ERR_DUMP_CALLSTACK_ENDLESS_LOOP_TASK, pTaskCtrl->pUKT->pCTask->ulTaskIndex ));

                eo670TraceContext traceContext;

                traceContext.threadHandle   = pTaskCtrl->pUKT->ThrdCtrlHeader.hThrd;
                traceContext.pThreadContext = 0;

                eo670_CTraceContextStack( &traceContext );
            }
            else
#endif
            {
                //
                // --- build up debug task request DLQ element
                //
                DLQ_ELEMENT         *pRequest;
                pRequest            = RESERVE_FREELIST_ELEM();
#ifdef _WIN32
                pRequest->ulReqType = REQ_DEBUG_TASK_EO00;
                sql74k_UKT_enqu_and_wake ( pTaskCtrl->pUKT, pTaskCtrl, pRequest );
#else
                pRequest->req_type = REQ_DEBUG_TASK_EO00;
                en71_iocEnquAndWake( pTaskCtrl->ukt, pTaskCtrl, pRequest );
#endif
                pConsoleDataRec->DebugTask.usReturnCode = CON_NO_ERROR;
            }
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetRegionInfo ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("RTEThread_ConsoleStandardWorker::ResetRegionInfo",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "region accesses"));

#if defined(_WIN32)
    ULONG                          ulSem;
    PSEM_CTRL_REC                  pSemCtrl;

    for ( ulSem = 0; ulSem < XPARAM(ulNoOfRegions); ulSem++ )
    {
        if ( kgs.pSemaphores[ ulSem ].szSemaName[ 0 ] == '\0' )
            continue;
        pSemCtrl = kgs.pSemaphores + ulSem;
        pSemCtrl->lGetCount        = 0;
        pSemCtrl->lWaitCount       = 0;
        pSemCtrl->lCollisionCount  = 0;
        pSemCtrl->lBegExclTasCount = 0;
        pSemCtrl->lEndExclTasCount = 0;
    }

#else

    SAPDB_UInt4        ulSem;
    struct DDB4_SEMA * pSemCtrl;

    for ( ulSem = 0; ulSem < XPARAM(ulNoOfRegions); ulSem++ )
    {
        if ( kgs->semaphore [ ulSem ].semaname[ 0 ] == '\0' )
            continue;
        pSemCtrl = &kgs->semaphore [ ulSem ];
        pSemCtrl->get_count         = 0;
        pSemCtrl->wait_count        = 0;
        pSemCtrl->wait_count        = 0;
        pSemCtrl->begexcl_tas_count = 0;
        pSemCtrl->endexcl_tas_count = 0;
    }

    TASK_CTRL pCurrTaskCtrl;

    for (pCurrTaskCtrl =  kgs->pFirstTaskCtrl;
         pCurrTaskCtrl <= kgs->pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        // This is a special UNIX extension, added by Frank Hoffmann during a benchmark test...
        // It cannot survive if 'ulNoOfRegion' gets dynamic...
        for ( int i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
        {
            pCurrTaskCtrl->reg_excl_cnt[i] = pCurrTaskCtrl->reg_coll_cnt[i] = 0 ;
        }
    }
#endif
    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetIOInfo ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("RTEThread_ConsoleStandardWorker::ResetIOInfo",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "io operations"));

    PDEVI_CTRL_REC pDeviCtrl;
    UKT_CTRL pUktCtrl;

    for ( SAPDB_ULong DevNo = 0; DevNo < (SAPDB_ULong)XPARAM(lMaxDevspaces); ++DevNo )
    {
        if ( IS_DEVICE_ATTACHED(DevNo) )
        {
            for ( SAPDB_ULong ulDevi = 0; ulDevi < (SAPDB_ULong)DEVI_PER_VOLUME; ulDevi++ )
            {
                pDeviCtrl = FIRST_DEV +
                            (DevNo * DEVI_PER_VOLUME) + ulDevi;

                WAIT_UNTIL_ALONE( LOCK_OF_DEVI(pDeviCtrl) );
                    pDeviCtrl->DevIOStat.ResetPending = true;
                CLEARLOCK( LOCK_OF_DEVI(pDeviCtrl) );
            }

            for ( pUktCtrl = FIRST_UKT; pUktCtrl <= LAST_UKT; ++pUktCtrl )
            {
                WAIT_UNTIL_ALONE( LOCK_OF_UKT(pUktCtrl) );
                    pUktCtrl->pSingleIO[DevNo].selfIOStatisticResetPending = true;
                CLEARLOCK( LOCK_OF_UKT(pUktCtrl) );
            }
        }
    }
    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetTaskCountInfo
(
    SAPDB_Bool       fAllTaskCounts
)
{
    TASK_CTRL pCurrTaskCtrl;

    SAPDBTRACE_ROUTINE_DEBUG ("RTEThread_ConsoleStandardWorker::ResetTaskCountInfo",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, fAllTaskCounts ? "all task counts" : "all task non dispatch counts"));

#if defined(_WIN32)
    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl;
         pCurrTaskCtrl <= kgs.pLastTaskCtrl;
         pCurrTaskCtrl++)
#else
    for (pCurrTaskCtrl =  kgs->pFirstTaskCtrl;
         pCurrTaskCtrl <= kgs->pLastTaskCtrl;
         pCurrTaskCtrl++)
#endif
    {
        pCurrTaskCtrl->TaskStatRecResetPending = ( fAllTaskCounts ? EO52_RESET_TASK_FULL_PENDING : EO52_RESET_TASK_PARTIAL_PENDING );
#if !defined(_WIN32)
        // This is a special UNIX extension, added by Frank Hoffmann during a benchmark test...
        // It cannot survive if 'ulNoOfRegion' gets dynamic...
        for ( int i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
        {
            pCurrTaskCtrl->reg_excl_cnt[i] = pCurrTaskCtrl->reg_coll_cnt[i] = 0 ;
        }
#endif
    }

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetQueueInfo ()
{
    TASK_CTRL pCurrTaskCtrl;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ResetQueueInfo",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "queue infos"));

#if defined(_WIN32)
    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl ;
         pCurrTaskCtrl <= kgs.pLastTaskCtrl;
         pCurrTaskCtrl++)
#else
    for (pCurrTaskCtrl =  kgs->pFirstTaskCtrl ;
         pCurrTaskCtrl <= kgs->pLastTaskCtrl;
         pCurrTaskCtrl++)
#endif
    {
        pCurrTaskCtrl->QueueStatRecResetPending = true;
    }

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetUKTSlStatInfo ()
{
    UKT_CTRL pTmpUKTCtrl;

    SAPDBTRACE_ROUTINE_DEBUG ("RTEThread_ConsoleStandardWorker::ResetUKTSlStatInfo",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "task group idle times"));

#if defined(_WIN32)
    for (pTmpUKTCtrl  = kgs.pFirstUKTCtrl;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++)
#else
    for (pTmpUKTCtrl  = kgs->first_kp;
        pTmpUKTCtrl <= kgs->last_kp;
        pTmpUKTCtrl++)
#endif
    {
        pTmpUKTCtrl->SleepStatRecResetPending = true;
    }

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetSuspends ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ResetSuspends",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "suspend calls"));

#if defined(_WIN32)
    memset ( kgs.ulSuspendReasons, 0 , sizeof(ULONG) * MX_SUSPEND_REASONS_EO00 ) ;
#else
    for ( int i = 0; i < MX_SUSPEND_REASONS_EO00; ++i )
    {
        kgs->SuspendReason[i] = 0;
    }
#endif
    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ResetAll ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ResetAll",
        Console_Trace, 1);

    RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_RESET, "all resetable counts"));

    ResetTaskCountInfo (ALL_TASK_COUNTS);
    ResetUKTSlStatInfo ();
    ResetIOInfo ();
    ResetRegionInfo ();
    ResetSuspends ();

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::EnableTimeMeasure ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::EnableTimeMeasure",
        Console_Trace, 1);

    if ( 0 == XPARAM(fTimeMeasure) )
    {
        Msg_List messageList;
        if ( !RTEConf_ParameterAccess::Instance()->SetString( (RTEConf_Parameter::Name)"TIME_MEASUREMENT"
                                                           , (RTEConf_Parameter::String)"YES"
                                                           , false
                                                           , messageList) )
        {
            RTE_Message(messageList);
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::DisableTimeMeasure ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::DisableTimeMeasure",
        Console_Trace, 1);

    if ( XPARAM(fTimeMeasure) )
    {
        Msg_List messageList;
        if ( !RTEConf_ParameterAccess::Instance()->SetString( (RTEConf_Parameter::Name)"TIME_MEASUREMENT"
                                                           , (RTEConf_Parameter::String)"NO"
                                                           , false
                                                           , messageList) )
        {
            RTE_Message(messageList);
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/
/* This routine is called in callback when changing value of (RTEConf_Parameter::Name)"TIME_MEASUREMENT" */
/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ChangeTimeEnableValue(bool nowEnabled)
{
    SAPDBTRACE_ROUTINE_DEBUG ("RTEThread_ConsoleStandardWorker::ChangeTimeEnableValue",
        Console_Trace, 1);

    if ( XPARAM(fTimeMeasure) )
    {
        if ( !nowEnabled )
        {
            XPARAM(fTimeMeasure) = 0;
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_TIME_DISABLED));
        }
    }
    else
    {
        if ( nowEnabled )
        {
            // switching on time measurement is implicitly resetting some statistics
            ResetTaskCountInfo  (TASK_STATES_ONLY);
            ResetUKTSlStatInfo ();
            ResetIOInfo () ;
            XPARAM(fTimeMeasure) = TRUE;
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_TIME_ENABLED));
        }
    }
}

#if defined (_WIN32)

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::CancelTask
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDB_ULong      ulTask = pConsoleDataRec->CancelTaskCtrl.ulTask;
    PTASK_CTRL_REC   pTaskCtrl;


    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::CancelTask",
                             Console_Trace, 1);

    if (( ulTask < 1 ) || ( ulTask > kgs.ulNumOfTasks ) )
        pConsoleDataRec->CancelTaskCtrl.usReturnCode = CON_WRONG_TASK;
    else
    {
        pTaskCtrl = kgs.pFirstTaskCtrl + ulTask - 1;

        if (( pTaskCtrl->TaskType != TT_UT_EO00 ) &&
            ( pTaskCtrl->TaskType != TT_EV_EO00 ) &&
            ( pTaskCtrl->TaskType != TT_SV_EO00 ) &&
            ( pTaskCtrl->TaskType != TT_US_EO00 ))
        {
            pConsoleDataRec->CancelTaskCtrl.usReturnCode = CON_WRONG_TASK_TYPE;
        }
        else if ( pTaskCtrl->TaskType == TT_SV_EO00 )
        {
            pTaskCtrl->KernelCtrlRecord.to_cancel = TRUE;
        }
        else if (sql53k_comm_cancel( pTaskCtrl, commErrOk_esp01 ) == TRUE )
        {
            pConsoleDataRec->CancelTaskCtrl.usReturnCode = CON_NO_ERROR;
        }
    }

  return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::SetUKTAffinity
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    PUKT_CTRL_REC	pUKT;
    SAPDB_ULong		ulUKT;
    SAPDB_ULong     Proccesor;
    RTE_SystemRc    rc;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::SetUKTAffinity",
        Console_Trace, 1);


    ulUKT = pConsoleDataRec->UKTAffinity.ulUKTIndex;

    if (( ulUKT < 1 ) || ( ulUKT > kgs.ulNumOfUKTs ))
    {
        pConsoleDataRec->UKTAffinity.usReturnCode = CON_WRONG_UKT;
    }
    else
    {
        pUKT = &kgs.pFirstUKTCtrl[ulUKT - 1];

        pConsoleDataRec->UKTAffinity.usReturnCode = CON_NO_ERROR;

        if ( pUKT->ThrdCtrlHeader.Processor & kgs.ProcessorUsageMask ) // reset ?
        {
            // --- set the thread priority
            rc = SET_THREAD_PRIO ( pUKT->ThrdCtrlHeader.lThrdPrio,
                                   pUKT->ThrdCtrlHeader.Tid,
                                   pUKT->ThrdCtrlHeader.hThrd );

            if ( rc != NO_ERROR )
            {
                MSGD (( ERR_SETTING_THRD_PRIO, pUKT->ThrdCtrlHeader.Tid));
                pConsoleDataRec->UKTAffinity.usReturnCode = CON_ERROR;
            }

            // reset to the default value
            SAPDB_Bool ok =  sql88k_reset_UKT_CPU_relationship( pUKT );

            if ( false == ok )
                pConsoleDataRec->UKTAffinity.usReturnCode = CON_ERROR;
        }
        else
        {
            DWORD_PTR 	    SystemAffinityMask;
            DWORD_PTR 	    ProcessAffinityMask;

            if ( !GetProcessAffinityMask( GetCurrentProcess(), &ProcessAffinityMask,
                                          &SystemAffinityMask ))
            {
                ProcessAffinityMask = 1;
            }

            // reset old settings
            sql88k_reset_UKT_CPU_relationship ( pUKT );

            // - free processor available
            if ( 0 != (~kgs.ProcessorUsageMask & ProcessAffinityMask ) )
            {
                // - find free processor
                for ( Proccesor = 1; Proccesor != 0; Proccesor <<= 1 )
                { ; }

                // force set affinity
                SAPDB_Bool ok =  sql88k_set_UKT_CPU_relationship( pUKT, true );

                if ( false == ok )
                    pConsoleDataRec->UKTAffinity.usReturnCode = CON_ERROR;
            }
            else
                pConsoleDataRec->UKTAffinity.usReturnCode = CON_NO_PROCESSOR;
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

/* PTS 1107070 */
VOID RTEThread_ConsoleStandardWorker::SetProcessorAffinityMask
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    DWORD_PTR       ProcessAffinityMask = (DWORD )pConsoleDataRec->SetProcessorMask.ProcessAffinityMask;
    PUKT_CTRL_REC   pUKT;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::SetProcessorAffinityMask",
                             Console_Trace, 1);

    if ( ProcessAffinityMask < 0 ||
         ProcessAffinityMask >= (DWORD_PTR)(1<<kgs.ulNumOfProcessors) )
    {
        pConsoleDataRec->SetProcessorMask.usReturnCode = CON_ERROR;
    }
    else
    {
        pConsoleDataRec->SetProcessorMask.usReturnCode = CON_NO_ERROR;

        if ( pConsoleDataRec->SetProcessorMask.TaskType == 0 )
        {
            kgs.ProcessorUsageMask = ProcessAffinityMask;

            for (pUKT  = kgs.pFirstUKTCtrl; pUKT <= kgs.pLastUKTCtrl; pUKT++)
            {
                if ( !SetThreadAffinityMask( pUKT->ThrdCtrlHeader.hThrd ,
                                             ProcessAffinityMask))
                {
                    MSGALL (( ERR_SETTING_PROCESSOR_MASK, ProcessAffinityMask, GetLastError() ));
                    pConsoleDataRec->SetProcessorMask.usReturnCode = CON_ERROR;
                    break;
                }
                else
                {
                    pUKT->ThrdCtrlHeader.Processor       = ProcessAffinityMask;
                    pUKT->ThrdCtrlHeader.CPURelationship = UKT_CPU_RELATIONSHIP_AFFINITY;
                }
            }
        }
        else
        {
            PTASK_CTRL_REC                 pCurrTaskCtrl;

            for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl; pCurrTaskCtrl <= kgs.pLastTaskCtrl; pCurrTaskCtrl++)
            {
                if ( pCurrTaskCtrl->TaskType == pConsoleDataRec->SetProcessorMask.TaskType )
                {
                    if ( !SetThreadAffinityMask( pCurrTaskCtrl->pUKT->ThrdCtrlHeader.hThrd,
                                                 ProcessAffinityMask ))
                    {
                        MSGALL ((ERR_SETTING_PROCESSOR_MASK, ProcessAffinityMask, GetLastError()));
                        pConsoleDataRec->SetProcessorMask.usReturnCode = CON_ERROR;
                        break;
                    }
                    else
                        pCurrTaskCtrl->pUKT->ThrdCtrlHeader.Processor       = ProcessAffinityMask;
                        pCurrTaskCtrl->pUKT->ThrdCtrlHeader.CPURelationship = UKT_CPU_RELATIONSHIP_AFFINITY;
                        kgs.ProcessorUsageMask                             |= ProcessAffinityMask;
                }
            }
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::DebugBreak
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::DebugBreak",
        Console_Trace, 1);

    if ( pConsoleDataRec->DebugBreak.fClearPosition == TRUE )
        kgs.ulSingleDebugBreakPos = 0;
    else
        kgs.ulSingleDebugBreakPos = (ULONG)pConsoleDataRec->DebugBreak.ulBreakPosition;

    if ( !(kgs.ulServiceType & SERVICE_INTERACTIVE_PROCESS) )
        pConsoleDataRec->DebugBreak.usReturnCode = CON_NO_INTERACT_WITH_DECTOP;
    else
        pConsoleDataRec->DebugBreak.usReturnCode = CON_NO_ERROR;

    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::KillTask
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDB_ULong     ulTask = pConsoleDataRec->KillTaskCtrl.ulTask;
    PTASK_CTRL_REC  pTaskCtrl;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::KillTask",
        Console_Trace, 1);


    if (( ulTask < 1 ) || ( ulTask > kgs.ulNumOfTasks ))
        pConsoleDataRec->KillTaskCtrl.usReturnCode = CON_WRONG_TASK;
    else
    {
        pTaskCtrl = kgs.pFirstTaskCtrl + ulTask - 1;

        if (( pTaskCtrl->TaskType != TT_UT_EO00 ) &&
            ( pTaskCtrl->TaskType != TT_EV_EO00 ) &&
            ( pTaskCtrl->TaskType != TT_US_EO00 ))
        {
            pConsoleDataRec->KillTaskCtrl.usReturnCode = CON_WRONG_TASK_TYPE;
        }
        else if ( sql53k_comm_cancel ( pTaskCtrl, commErrCrash_esp01 ) == TRUE )
        {
            pConsoleDataRec->KillTaskCtrl.usReturnCode = CON_NO_ERROR;
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

static  void DumpThread  ( ULONG   dumpType )
{
  if ( DT_CRASH_DUMP & dumpType )
  {
      //
      // --- this thread is used to do 'async-abort'
      __try  
      { 
          RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_THREAD_CONS_CRASH_DUMP));
      } 
      __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}
  }
  else
  {
      PUKT_CTRL_REC                       pUKTCtrl;

      // suspend all UKTs
      for ( pUKTCtrl =  kgs.pFirstUKTCtrl; pUKTCtrl <= kgs.pLastUKTCtrl; pUKTCtrl++  )
      {
          SuspendThread( pUKTCtrl->ThrdCtrlHeader.hThrd );
      }

      __try
      {
          DWORD                   threadId = GetCurrentThreadId();
          LPEXCEPTION_POINTERS    except;


          __try
          {
              RaiseException( EXCEPT_MINI_DUMP, 0, 0, 0);
          }
          __except ( except = GetExceptionInformation(),
              eo670_WriteMiniDump( threadId, except, SAPDB_KNL_MINI_DUMP, true, false, &kgs.AdminSA ), EXCEPTION_CONTINUE_EXECUTION  )
          {
          }
      }
      __except( EXCEPTION_EXECUTE_HANDLER )
      {
          // ignore any errors!
      }

      // suspend all UKTs
      for ( pUKTCtrl =  kgs.pFirstUKTCtrl; pUKTCtrl <= kgs.pLastUKTCtrl; pUKTCtrl++  )
      {
          ResumeThread( pUKTCtrl->ThrdCtrlHeader.hThrd );
      }


  }
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::CrashDump ()
{
    TID                 Tid;
    HANDLE              hThrd;
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::CrashDump",
        Console_Trace, 1);

    if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
    {
        if ( kgs.pTW->TaskState != TSK_INITIALIZED_EO00 )
        {
            // --- don't block the console thread during crash phase, do an 'async-abort'
            rc = CREATE_THREAD( &Tid, &hThrd, DumpThread, DT_CRASH_DUMP, 0, 4096 );

            if ( rc != RTE_SYSTEMRC_NO_ERROR )
                RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_THREAD_CONS_CRASH_THREAD));
            else
                CloseHandle ( hThrd );
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/


void RTEThread_ConsoleStandardWorker::MiniDump ()
{
#if defined (_WIN32)
    TID                 Tid;
    HANDLE              hThrd;
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

    // --- don't block the console thread during crash phase, do an 'async-abort'
    rc = CREATE_THREAD( &Tid, &hThrd, DumpThread, DT_MINI_DUMP, 0, 4096 );

    if ( rc != RTE_SYSTEMRC_NO_ERROR )
        RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_THREAD_CONS_CRASH_THREAD));
    else
        CloseHandle ( hThrd );
#endif
    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::ShowParam 
( 
    PCONSOLE_DATA_REC  pConsoleDataRec 
)
{

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ShowParam",
        Console_Trace, 1);

    SAPDB_MemCopyNoCheck (&pConsoleDataRec->GetParamInfo.XParam , kgs.XParam ,
            sizeof ( RTE_XPARAM_REC ) );


    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::ForceIOError
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDB_Long     devspaceNo = pConsoleDataRec->ForceIOError.ulDevno ;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ForceIOError",
        Console_Trace, 1);


    if (devspaceNo < XPARAM(lMaxDevspaces))
    {
        PDEVI_CTRL_REC             pDeviCtrl ;
        pDeviCtrl = kgs.pFirstDeviCtrl + devspaceNo * DEVI_PER_VOLUME;
        if ( pConsoleDataRec->ForceIOError.fErrOnRead )
            pDeviCtrl->lForceIOError = 0x100 | DEV_READ  ;
        else
            pDeviCtrl->lForceIOError = 0x100 | DEV_WRITE ;
    }

    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::SetDebugLevel
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::SetDebugLevel",
        Console_Trace, 1);

    kgs.ulDebugLevel = (ULONG)pConsoleDataRec->SetDebugLevelCtrl.ulDebugLevel ;

    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::GetTotalCountInfo
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    PTASK_CTRL_REC                 pCurrTaskCtrl;
    ULONG                          ulSem;
    PSEM_CTRL_REC                  pSemCtrl;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetTotalCountInfo",
        Console_Trace, 1);

    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl ;
         pCurrTaskCtrl <= kgs.pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        if ( EO52_RESET_TASK_FULL_PENDING != pCurrTaskCtrl->TaskStatRecResetPending &&
            pCurrTaskCtrl->ulDispatcherCalls > 0 )
        {
            pConsoleDataRec->TotalCountsInfo.ulTotalDispCalls +=
                pCurrTaskCtrl->ulDispatcherCalls;

            if ( !pCurrTaskCtrl->QueueStatRecResetPending )
            {
                pConsoleDataRec->TotalCountsInfo.ulTotalCommands +=
                    pCurrTaskCtrl->pLegacyTaskCtrlObject->CommQueueCount();
            }

            if ( EO52_NO_RESET_PENDING == pCurrTaskCtrl->TaskStatRecResetPending )
            {
                pConsoleDataRec->TotalCountsInfo.ulTotalVwait    +=
                    pCurrTaskCtrl->TaskStateStat.VwaitStat.TimeStat.ulCount;
                pConsoleDataRec->TotalCountsInfo.ulTotalVsleep   +=
                    pCurrTaskCtrl->TaskStateStat.VsleepStat.TimeStat.ulCount;
                pConsoleDataRec->TotalCountsInfo.ulTotalVsusp    +=
                    pCurrTaskCtrl->TaskStateStat.VsuspStat.TimeStat.ulCount;
                pConsoleDataRec->TotalCountsInfo.ulTotalDevIO    +=
                    pCurrTaskCtrl->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount +
                    pCurrTaskCtrl->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount;
                pConsoleDataRec->TotalCountsInfo.ulTotalSelfIO   +=
                    pCurrTaskCtrl->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
                    pCurrTaskCtrl->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount;
            }

            pConsoleDataRec->TotalCountsInfo.TotalPrioStat.Resetable.ulTotalCount +=
                pCurrTaskCtrl->PrioStat.Resetable.ulTotalCount;
            pConsoleDataRec->TotalCountsInfo.TotalPrioStat.Resetable.ulOtherUkt   +=
                pCurrTaskCtrl->PrioStat.Resetable.ulOtherUkt;
        }
    }

    for ( ulSem = 0 ; ulSem < XPARAM(ulNoOfRegions) ; ulSem++ )
    {
        if ( kgs.pSemaphores[ ulSem ].szSemaName[ 0 ] != '\0' )
        {
            pSemCtrl = kgs.pSemaphores + ulSem;

            pConsoleDataRec->TotalCountsInfo.ulTotalRegAccess   +=
                pSemCtrl->lGetCount ;
            pConsoleDataRec->TotalCountsInfo.ulTotalRegColl     +=
                pSemCtrl->lCollisionCount ;
            pConsoleDataRec->TotalCountsInfo.ulTotalRegWait     +=
                pSemCtrl->lWaitCount  ;
            pConsoleDataRec->TotalCountsInfo.ulTotalBegExclTasCount  +=
                pSemCtrl->lBegExclTasCount  ;
            pConsoleDataRec->TotalCountsInfo.ulTotalEndExclTasCount  +=
                pSemCtrl->lEndExclTasCount  ;
        }
    }

    pConsoleDataRec->TotalCountsInfo.ulTotalSymbolResolutionLocks = RTEDiag_SymbolResolutionLockCalls();

    return;
}

/*---------------------------------------------------------------------------*/

VOID RTEThread_ConsoleStandardWorker::ShowSuspends
(
    PCONSOLE_DATA_REC  pConsoleDataRec
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ShowSuspends",
                             Console_Trace, 1);

    SAPDB_UInt4     idx;

    for (idx = 0; idx < MAX_SUSPEND_REASONS; ++idx)
    {
        pConsoleDataRec->SuspendReasons.ulSuspends[idx] = kgs.ulSuspendReasons[idx];
    }
    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetTaskInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulIdx;
    PTASK_CTRL_REC                  pCurrTaskCtrl;
    PCONS_TASK_INFO_REC             pTask;
    PCONS_SINGLE_TASK_INFO_REC      pTaskInfos;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_ULong                     ulTaskOffset;
    SAPDB_ULong                     ulTaskInfoOffset;
    SAPDB_Char *                    Hptr;
    SAPDB_UInt2                     PrevTimeVar = PREV_TIME_VAR(kgs.CurrTimeVar);
    SAPDB_Byte *                    pData;
    SAPDB_UInt4                     bytesRead;
    SAPDB_UInt4                     bytesTotal;
    SAPDB_ULong                     memSize;
    SAPDB_ULong                     dataSize;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetTaskInfo",
        Console_Trace, 1);

    ulTaskOffset     = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulTaskInfoOffset = ALIGN( sizeof(*pTask), ALIGNMENT_VALUE);

    memSize = ulTaskOffset +
              ulTaskInfoOffset +
              ALIGN( sizeof(*pTaskInfos) *  kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                SAPDB_ToString (memSize),
                SAPDB_ToString (CON_REQ_TASK));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader          = (RTE_ConsoleStandardDataHeader *) pData;
    pTask            = (PCONS_TASK_INFO_REC)(pData + ulTaskOffset);
    pTaskInfos       = (PCONS_SINGLE_TASK_INFO_REC) (pData + ulTaskOffset + ulTaskInfoOffset);


    if (RTE_CONS_NO_ERROR != (rc = GetRequestDataContinue ((SAPDB_Byte *)pTask, DATA_REQ, sizeof (*pTask),
                                                    bytesRead, bytesTotal, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }

    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl, ulIdx = 0;
         pCurrTaskCtrl <= kgs.pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        if ( IS_TASK_TO_SHOW_EO92(pCurrTaskCtrl->TaskType,
            pCurrTaskCtrl->TaskState,
            pCurrTaskCtrl->ulStateReason,
            pCurrTaskCtrl->TaskOMSStat.OMSCallActive,             /* PTS 1116933 */
            pCurrTaskCtrl->QueLocation,
            pTask->TypeToShow,
            pTask->StateToShow) )
        {
            pTaskInfos[ulIdx].ulTaskIndex  = pCurrTaskCtrl->ulTaskIndex;

            pTaskInfos[ulIdx].Tid          =
                pCurrTaskCtrl->pUKT->ThrdCtrlHeader.Tid;

            pTaskInfos[ulIdx].TaskState     = pCurrTaskCtrl->TaskState;
            pTaskInfos[ulIdx].TaskType      = pCurrTaskCtrl->TaskType;
            strcpy (pTaskInfos[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString (false));
            pTaskInfos[ulIdx].ulStateReason = pCurrTaskCtrl->ulStateReason ;
            pTaskInfos[ulIdx].lWaitForTask  = pCurrTaskCtrl->lWaitForTask ;
            pTaskInfos[ulIdx].lWaitForRoot  = pCurrTaskCtrl->lWaitForRoot ;
            pTaskInfos[ulIdx].lWaitForDevno = pCurrTaskCtrl->lWaitForDevno ;
            if ( (Hptr = pCurrTaskCtrl->DevNameWaitingFor) )
                strcpy ( pTaskInfos[ulIdx].DevNameWaitingFor, Hptr) ;

            pTaskInfos[ulIdx].ulTaskTimeOut           = pCurrTaskCtrl->pTaskObject->ClientConnections().GetSessionTimeOut();
            pTaskInfos[ulIdx].Sid                     = pCurrTaskCtrl->Sid;
            pTaskInfos[ulIdx].ulBegExclCnt            = pCurrTaskCtrl->ulBegExclCnt;
            pTaskInfos[ulIdx].PrioFlag                = pCurrTaskCtrl->PrioFlag;
            pTaskInfos[ulIdx].SavePrioFlag            = pCurrTaskCtrl->SavePrioFlag;
            pTaskInfos[ulIdx].QueLocation             = pCurrTaskCtrl->QueLocation;
            pTaskInfos[ulIdx].CancelFlag              = pCurrTaskCtrl->KernelCtrlRecord.to_cancel;     /* CR 1103476 */
            pTaskInfos[ulIdx].OmsCallbackActive       = pCurrTaskCtrl->TaskOMSStat.OMSCallActive ; /* CR 1106969 */

            if (( pCurrTaskCtrl->TaskType  == TT_US_EO00 ) ||
                ( pCurrTaskCtrl->TaskType  == TT_EV_EO00 ) ||
                ( pCurrTaskCtrl->TaskType  == TT_UT_EO00 ))
            { pTaskInfos[ulIdx].ApplPid = pCurrTaskCtrl->pUserCommCtrl->pidClientPID;
            strcpy ( pTaskInfos[ulIdx].szClientNode,
                     pCurrTaskCtrl->pUserCommCtrl->szClientNode) ;
            }
            else
            { pTaskInfos[ulIdx].ApplPid = RTE_UNDEF_OSPID;
            pTaskInfos[ulIdx].szClientNode[0] = '\0' ;
            }

            pTaskInfos[ulIdx].LockedPageNo = pCurrTaskCtrl->lockedPageNo ; /* CR 1115172 */
            pTaskInfos[ulIdx].ulUktIndex = pCurrTaskCtrl->pUKT->ulUKTIndex ; /* CR 1115172 */
            pTaskInfos[ulIdx].UktSleepCount = pCurrTaskCtrl->pUKT->UKTRunningStat.TimeStat.ulCount; /* CR 1115172 */
            pTaskInfos[ulIdx].UktSleepingFlag = (pCurrTaskCtrl->pUKT->ThrdCtrlHeader.ThrdState == KT_SLEEPING) ; /* CR 1115172 */
            pTaskInfos[ulIdx].WaitTime = (LONG)(pCurrTaskCtrl->ulWaitTime ? (kgs.ulCurrTime - pCurrTaskCtrl->ulWaitTime) : 0); /* CR 1115172 */
            pTaskInfos[ulIdx].ulDynPrio = pCurrTaskCtrl->ulDynPrio;
            strncpy( &pTaskInfos[ulIdx].NameOfSpinlockYieldingFor[0],
                     &pCurrTaskCtrl->NameOfSpinlockYieldingFor[0],
                     sizeof(pTaskInfos[ulIdx].NameOfSpinlockYieldingFor) );
            pTaskInfos[ulIdx].WaitingForExclusiveAccess = pCurrTaskCtrl->pTaskObject->TaskIsWaitingForExclusiveAccess();
            ulIdx++;
        }
    }

    pTask->ulTaskInfoOffset  = (ULONG)ulTaskInfoOffset;
    pTask->lNumOfActiveTasks = (LONG)ulIdx;

    dataSize = ulTaskOffset +
               ulTaskInfoOffset +
               ALIGN( sizeof(*pTaskInfos) * ulIdx, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulTaskOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetConnectionsInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                         ulIdx;
    PTASK_CTRL_REC                      pCurrTaskCtrl;
    PCONS_SINGLE_CONNECTION_INFO_REC    pConnectionInfos;
    SAPDB_ULong                         ulConnectionInfoOffset;
    SAPDB_ULong                         ulConnectOffset;
    PCONS_CONNECTION_INFO_REC           pConnect;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetConnectionsInfo",
        Console_Trace, 1);

    ulConnectOffset         = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulConnectionInfoOffset  = ALIGN( sizeof(*pConnect), ALIGNMENT_VALUE);

    memSize = ulConnectOffset +
              ulConnectionInfoOffset +
              ALIGN( sizeof(*pConnectionInfos) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_CONNECTIONS));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader          = (RTE_ConsoleStandardDataHeader *) pData;
    pConnect            = (PCONS_CONNECTION_INFO_REC)(pData + ulConnectOffset);
    pConnectionInfos = (PCONS_SINGLE_CONNECTION_INFO_REC) (pData + ulConnectOffset + ulConnectionInfoOffset);

    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl, ulIdx = 0;
        pCurrTaskCtrl <= kgs.pLastTaskCtrl;
        pCurrTaskCtrl++)
    {
        if ( pCurrTaskCtrl->pUserCommCtrl
            && pCurrTaskCtrl->TaskState != TSK_INACTIVE_EO00
            && pCurrTaskCtrl->TaskState != TSK_CONNECTWAIT_EO00 )
        {
            pConnectionInfos[ulIdx].taskIndex     = pCurrTaskCtrl->ulTaskIndex;
            pConnectionInfos[ulIdx].uktIndex      = pCurrTaskCtrl->pUKT->ulUKTIndex;
            pConnectionInfos[ulIdx].taskType      = pCurrTaskCtrl->TaskType;
            strcpy (pConnectionInfos[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString ());
            pConnectionInfos[ulIdx].connState     = sql53k_get_connection_state( pCurrTaskCtrl );
            pConnectionInfos[ulIdx].connectTime   = pCurrTaskCtrl->pUserCommCtrl->ulConnected;
            pConnectionInfos[ulIdx].peerPid       = pCurrTaskCtrl->pUserCommCtrl->pidLocalClientPID;
            pConnectionInfos[ulIdx].remotePid     = pCurrTaskCtrl->pUserCommCtrl->pidClientPID;
            strcpy( pConnectionInfos[ulIdx].clientNode, pCurrTaskCtrl->pUserCommCtrl->szClientNode );
            ulIdx++;
        }
    }

    pConnect->ulConnectionInfoOffset = ulConnectionInfoOffset;
    pConnect->lNumOfConnections      = ulIdx;


    dataSize = ulConnectOffset +
        ulConnectionInfoOffset +
        ALIGN( sizeof(*pConnectionInfos) * ulIdx, ALIGNMENT_VALUE);


    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulConnectOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetIOInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulIdx = 0;
    SAPDB_ULong                     ulDevspace;
    SAPDB_ULong                     ulDevi;
    SAPDB_UInt8                     ulReadCnt;
    SAPDB_UInt8                     ulReadPageCnt;
    SAPDB_UInt8                     ulWriteCnt;
    SAPDB_UInt8                     ulWritePageCnt;
    PDEVI_CTRL_REC                  pDeviCtrl;
    PUKT_CTRL_REC                   pTmpUKTCtrl;
    SAPDB_ULong                     ulDevspaceInfoOffset;
    SAPDB_ULong                     ulIOInfoOffset;
    PCONS_DEV_INFO_REC              pDevspaceInfos;
    PCONS_IO_INFO_REC               pIOInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_Byte *                    pData;
    SAPDB_ULong                     memSize;
    SAPDB_ULong                     dataSize;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetIOInfo",
        Console_Trace, 1);

    ulIOInfoOffset          = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulDevspaceInfoOffset    = ALIGN( sizeof(*pIOInfo), ALIGNMENT_VALUE);

    memSize = ulIOInfoOffset +
              ulDevspaceInfoOffset +
              ALIGN(sizeof(*pDevspaceInfos) *  XPARAM(lMaxDevspaces), ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                SAPDB_ToString (memSize),
                SAPDB_ToString (CON_REQ_IO));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader          = (RTE_ConsoleStandardDataHeader *) pData;
    pIOInfo          = (PCONS_IO_INFO_REC)(pData + ulIOInfoOffset);
    pDevspaceInfos   = (PCONS_DEV_INFO_REC) (pData + ulIOInfoOffset + ulDevspaceInfoOffset);

    for (ulDevspace = 0;
         ulDevspace < (ULONG)XPARAM(lMaxDevspaces);
         ulDevspace++ )
    {
        if ( kgs.Dev0.pIOCtrl[ulDevspace].szDevspace[0] == '\0' )
            continue;

        ulReadCnt  = 0;
        ulReadPageCnt  = 0;
        ulWriteCnt = 0;
        ulWritePageCnt = 0;

        for ( ulDevi = 0; ulDevi < (ULONG)DEVI_PER_VOLUME; ulDevi++ )
        {
            pDeviCtrl = kgs.pFirstDeviCtrl +
                ulDevspace * DEVI_PER_VOLUME + ulDevi;

            if ( pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED
                && !pDeviCtrl->DevIOStat.ResetPending )
            {
                ulReadCnt  += pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulCount;
                ulReadPageCnt  += pDeviCtrl->DevIOStat.DevRead.ulPageCount;
                ulWriteCnt += pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulCount;
                ulWritePageCnt += pDeviCtrl->DevIOStat.DevWrite.ulPageCount;
            }
        }

        for (pTmpUKTCtrl  = kgs.pFirstUKTCtrl;
            pTmpUKTCtrl <= kgs.pLastUKTCtrl;
            pTmpUKTCtrl++)
        {
            if ( !pTmpUKTCtrl->pSingleIO[ulDevspace].selfIOStatisticResetPending )
            {
                ulReadCnt  += pTmpUKTCtrl->pSingleIO[ulDevspace].ReadSelfIo.IOStat.TimeStat.ulCount;
                ulReadPageCnt  += pTmpUKTCtrl->pSingleIO[ulDevspace].ReadSelfIo.ulPageCount;
                ulWriteCnt += pTmpUKTCtrl->pSingleIO[ulDevspace].WriteSelfIo.IOStat.TimeStat.ulCount;
                ulWritePageCnt += pTmpUKTCtrl->pSingleIO[ulDevspace].WriteSelfIo.ulPageCount;
            }
        }

        if ( ulReadCnt || ulWriteCnt )
        {
            pDevspaceInfos[ ulIdx ].ulDevspaceNo    = ulDevspace;
            pDevspaceInfos[ ulIdx ].ulReadCnt       = ulReadCnt;
            pDevspaceInfos[ ulIdx ].ulReadPageCnt   = ulReadPageCnt;
            pDevspaceInfos[ ulIdx ].ulWriteCnt      = ulWriteCnt;
            pDevspaceInfos[ ulIdx ].ulWritePageCnt  = ulWritePageCnt;
            strcpy( pDevspaceInfos[ ulIdx ].DevspaceName,
                kgs.Dev0.pIOCtrl[ulDevspace].szDevspace );
            ulIdx++;
        }
    }

    pIOInfo->ulDevspaceInfoOffset = ulDevspaceInfoOffset;
    pIOInfo->lNumOfDevspaces      = ulIdx;

    dataSize = ulIOInfoOffset +
               ulDevspaceInfoOffset +
               ALIGN(sizeof(*pDevspaceInfos) * ulDevspace, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulIOInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetAIOInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    PASYNCI_CTRL_REC                pAsynciCtrl;
    PCONS_ASYNCI_INFO_REC           pASYNCiInfos;
    SAPDB_ULong                     ulASYNCiInfoOffset;
    SAPDB_ULong                     ulAsynci;
    SAPDB_ULong                     lNumOfASYNCis;
    SAPDB_ULong                     ulAIOInfoOffset;
    PCONS_AIO_INFO_REC              pAIOInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_Byte *                    pData;
    SAPDB_ULong                     memSize;
    SAPDB_ULong                     dataSize;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetAIOInfo",
        Console_Trace, 1);

    ulAIOInfoOffset       = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulASYNCiInfoOffset    = ALIGN( sizeof(*pAIOInfo), ALIGNMENT_VALUE);

    memSize = ulAIOInfoOffset +
              ulASYNCiInfoOffset +
              ALIGN(sizeof(*pASYNCiInfos) *  kgs.ulNumOfAsyncis, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                                                         SAPDB_ToString (memSize),
                                                         SAPDB_ToString (CON_REQ_AIO));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader        = (RTE_ConsoleStandardDataHeader *) pData;
    pAIOInfo       = (PCONS_AIO_INFO_REC)(pData + ulAIOInfoOffset);
    pASYNCiInfos   = (PCONS_ASYNCI_INFO_REC) (pData + ulAIOInfoOffset + ulASYNCiInfoOffset);

    //
    // --- ASYNCi info
    //
    for ( ulAsynci = 0, lNumOfASYNCis = 0; ulAsynci < kgs.ulNumOfAsyncis ; ulAsynci++ )
    {
        pAsynciCtrl = kgs.pFirstAsynciCtrl + ulAsynci;

        if (( pAsynciCtrl->fAsyncClosePending       == TRUE ) ||
            ( pAsynciCtrl->ThrdCtrlHeader.ThrdState == KT_INITIALIZED ))
            continue;

        strcpy ( pASYNCiInfos[lNumOfASYNCis].szLogDeviceName,
            pAsynciCtrl->szLogDeviceName );

        pASYNCiInfos[lNumOfASYNCis].Tid           =
            pAsynciCtrl->ThrdCtrlHeader.Tid;

        pASYNCiInfos[lNumOfASYNCis].fForWriting   =
            pAsynciCtrl->fForWriting != 0;

        pASYNCiInfos[lNumOfASYNCis].ulBlockSize   =
            pAsynciCtrl->ulBlockSize;

        pASYNCiInfos[lNumOfASYNCis].ulMaxBlockCnt =
            pAsynciCtrl->ulMaxBlockCnt;

        pASYNCiInfos[lNumOfASYNCis].DevIOStat = pAsynciCtrl->DevIOStat;
        /* will currently never be true, since Asynci are temporary resources only and the */
        /* statistic record will be resetted during initialization */
        if ( pAsynciCtrl->DevIOStat.ResetPending )
        {
            memset(&pASYNCiInfos[lNumOfASYNCis].DevIOStat.DevRead, 0, sizeof(teo52_IOStatisticRec));
            memset(&pASYNCiInfos[lNumOfASYNCis].DevIOStat.DevWrite, 0, sizeof(teo52_IOStatisticRec));
        }

        lNumOfASYNCis ++;
    }

    pAIOInfo->lNumOfASYNCis      = lNumOfASYNCis;
    pAIOInfo->ulASYNCiInfoOffset = ulASYNCiInfoOffset;

    dataSize = ulAIOInfoOffset +
               ulASYNCiInfoOffset +
               ALIGN(sizeof(*pASYNCiInfos) * lNumOfASYNCis, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulAIOInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetRegionInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     lNumWaitTasks;
    SAPDB_ULong                     ulIdx;
    SAPDB_ULong                     ulSem;
    PSEM_CTRL_REC                   pSemCtrl;
    PSEM_QUEUE_REC                  pSemQueElem;
    PCONS_SINGLE_SEM_INFO_REC       pSingleSemInfos;
    SAPDB_ULong                     ulSingleSemInfoOffset;
    SAPDB_ULong *                   pulWaitTasks;
    SAPDB_ULong                     ulWaitTaskOffset;
    PTASK_CTRL_REC                  pOwnerTaskCtrl, pWaitTaskCtrl ;
    SAPDB_ULong                     ulSemInfoOffset;
    PCONS_SEM_INFO_REC              pSemInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_Byte *                    pData;
    SAPDB_ULong                     memSize;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetRegionInfo",
        Console_Trace, 1);

    ulSemInfoOffset       = ALIGN(sizeof(*pHeader), ALIGNMENT_VALUE);
    ulSingleSemInfoOffset = ALIGN(sizeof(*pSemInfo), ALIGNMENT_VALUE);
    ulWaitTaskOffset      = ulSingleSemInfoOffset +
        ALIGN(sizeof(CONS_SINGLE_SEM_INFO_REC) * XPARAM(ulNoOfRegions),
              ALIGNMENT_VALUE);

    memSize = ulSemInfoOffset +
              ulWaitTaskOffset +
              ALIGN( sizeof(*pulWaitTasks) * kgs.ulNumOfTasks, ALIGNMENT_VALUE)* XPARAM(ulNoOfRegions);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_REGION));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader         = (RTE_ConsoleStandardDataHeader *) pData;
    pSemInfo        = (PCONS_SEM_INFO_REC)(pData + ulSemInfoOffset);
    pSingleSemInfos = (PCONS_SINGLE_SEM_INFO_REC) (pData + ulSemInfoOffset + ulSingleSemInfoOffset);
    pulWaitTasks    = (SAPDB_ULong *)(pData + ulSemInfoOffset + ulWaitTaskOffset);

#ifdef FHTEST
    // !!!!!!!!!!! FH_TEST !!!!!!!!!!!!!
    vTestGetRteInfo();
#endif

    for ( ulSem = 0, ulIdx = 0; ulSem < XPARAM(ulNoOfRegions); ulSem++ )
    {
        if ( kgs.pSemaphores[ ulSem ].szSemaName[ 0 ] == '\0' )
            continue;

        pSemCtrl = kgs.pSemaphores + ulSem;

        strncpy ( pSingleSemInfos[ulSem].szSemaName,
            pSemCtrl->szSemaName,
            sizeof( pSingleSemInfos[ulSem].szSemaName ) - 1 );

        if ( (pOwnerTaskCtrl = pSemCtrl->pOwnerTaskCtrl) )
            pSingleSemInfos[ulSem].ulOwnerTaskIndex = pOwnerTaskCtrl->ulTaskIndex;

        pSingleSemInfos[ulSem].ulSid            = ulSem + 1;
        pSingleSemInfos[ulSem].lGetCount        = pSemCtrl->lGetCount;
        pSingleSemInfos[ulSem].lBegExclTasCount = pSemCtrl->lBegExclTasCount;
        pSingleSemInfos[ulSem].lEndExclTasCount = pSemCtrl->lEndExclTasCount;
        pSingleSemInfos[ulSem].lCollisionCount  = pSemCtrl->lCollisionCount;
        pSingleSemInfos[ulSem].lWaitCount       = pSemCtrl->lWaitCount;
        pSingleSemInfos[ulSem].lExclusive       = pSemCtrl->SemaList.lExclusive;

        for ( pSemQueElem =  pSemCtrl->SemaList.pRemovePos, lNumWaitTasks = 0;
            (pWaitTaskCtrl = pSemQueElem->pTaskCtrl) != NULL ;
            pSemQueElem =  pSemQueElem->pNextElem, lNumWaitTasks++ )
        {
            pulWaitTasks[lNumWaitTasks] = pWaitTaskCtrl->ulTaskIndex;
        }

        pSingleSemInfos[ulSem].ulWaitTaskOffset = ulWaitTaskOffset;
        pSingleSemInfos[ulSem].lNumWaitTasks    = lNumWaitTasks;

        ulWaitTaskOffset += ALIGN( sizeof(*pulWaitTasks) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);
        pulWaitTasks    = (SAPDB_ULong *)(pData + ulSemInfoOffset + ulWaitTaskOffset);
        ulIdx++;
    }

    pSemInfo->ulSingleSemInfoOffset = ulSingleSemInfoOffset;
    pSemInfo->lNumOfSems            = ulIdx;

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(memSize - ulSemInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)memSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetRteInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    PUKT_CTRL_REC                   pUKTCtrl;
    PDEV_WORKER_CTRL_REC            pDevWorkerCtrl;
    SAPDB_ULong                     ulDevWorker;
    SAPDB_ULong                     ulUKT;
    SAPDB_ULong                     ulTotalTasks;
    CHAR                            szCluster[80];
    SAPDB_ULong                     ulDevspace;
    SAPDB_ULong                     lNumOfDev;
    SAPDB_ULong                     lNumOfDEVis;
    SAPDB_ULong                     ulDevi;
    PDEVI_CTRL_REC                  pDeviCtrl;
    SAPDB_ULong                     ulDevWorkerInfoOffset;
    PCONS_DEV_WORKER_INFO_REC       pDevWorkerInfos;
    SAPDB_ULong                     ulUKTInfoOffset;
    PCONS_UKT_INFO_REC              pUKTInfos;
    SAPDB_ULong                     ulDevspaceInfoOffset;
    PCONS_DEV_INFO_REC              pDevspaceInfos;
    SAPDB_ULong                     ulDEViInfoOffset;
    PCONS_DEVI_INFO_REC             pDEViInfos;
    SAPDB_ULong                     ulRTEInfoOffset;
    PCONS_RTE_INFO_REC              pRTEInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_Byte *                    pData;
    SAPDB_ULong                     memSize;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetRteInfo",
        Console_Trace, 1);

    ulRTEInfoOffset       = ALIGN(sizeof(*pHeader), ALIGNMENT_VALUE);
    ulDevWorkerInfoOffset = ALIGN( sizeof(*pRTEInfo), ALIGNMENT_VALUE);
    ulUKTInfoOffset       = ulDevWorkerInfoOffset + ALIGN( sizeof(*pDevWorkerInfos) * kgs.ulNumOfDevWorker, ALIGNMENT_VALUE);
    ulDevspaceInfoOffset  = ulUKTInfoOffset + ALIGN( sizeof(*pUKTInfos) * kgs.ulNumOfUKTs, ALIGNMENT_VALUE);
    ulDEViInfoOffset      = ulDevspaceInfoOffset + ALIGN( sizeof(*pDevspaceInfos) * XPARAM(lMaxDevspaces)  * kgs.ulNumOfUKTs, ALIGNMENT_VALUE);

    memSize = ulRTEInfoOffset +
              ulDEViInfoOffset +
              ALIGN( sizeof(*pDEViInfos) *  kgs.ulNumOfDevis,  ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_RTE));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader         = (RTE_ConsoleStandardDataHeader *) pData;
    pRTEInfo        = (PCONS_RTE_INFO_REC)(pData + ulRTEInfoOffset);
    pDevWorkerInfos = (PCONS_DEV_WORKER_INFO_REC)(pData + ulRTEInfoOffset + ulDevWorkerInfoOffset);
    pUKTInfos       = (PCONS_UKT_INFO_REC)(pData + ulRTEInfoOffset + ulUKTInfoOffset);
    pDevspaceInfos  = (PCONS_DEV_INFO_REC)(pData + ulRTEInfoOffset + ulDevspaceInfoOffset);
    pDEViInfos      = (PCONS_DEVI_INFO_REC)(pData + ulRTEInfoOffset + ulDEViInfoOffset);

    //
    // --- Kernel thread info
    //
    pRTEInfo->CoordTid       =
        kgs.Coord.ThrdCtrlHeader.Tid;
    pRTEInfo->CoordState     =
        (UINT1)kgs.Coord.ThrdCtrlHeader.ThrdState;

    pRTEInfo->ConsoleTid     =
        kgs.Console.ThrdCtrlHeader.Tid;
    pRTEInfo->ConsoleState   =
        (UINT1)kgs.Console.ThrdCtrlHeader.ThrdState;

    pRTEInfo->TimerTid       =
        kgs.Timer.ThrdCtrlHeader.Tid;
    pRTEInfo->TimerState     =
        (UINT1)kgs.Timer.ThrdCtrlHeader.ThrdState;

    pRTEInfo->ClockTid       =
        kgs.Clock.ThrdCtrlHeader.Tid;
    pRTEInfo->ClockState     =
        (UINT1)kgs.Clock.ThrdCtrlHeader.ThrdState;

    pRTEInfo->Dev0Tid        =
        kgs.Dev0.ThrdCtrlHeader.Tid;
    pRTEInfo->Dev0State      =
        (UINT1)kgs.Dev0.ThrdCtrlHeader.ThrdState;

    pRTEInfo->Async0Tid      =
        kgs.Async0.ThrdCtrlHeader.Tid;
    pRTEInfo->Async0State    =
        (UINT1)kgs.Async0.ThrdCtrlHeader.ThrdState;

    pRTEInfo->RequestorTid   =
        kgs.Requestor.ThrdCtrlHeader.Tid;
    pRTEInfo->RequestorState =
        (UINT1)kgs.Requestor.ThrdCtrlHeader.ThrdState;

    pRTEInfo->numLogProcessors   =
        kgs.ulNumOfProcessors;

    pRTEInfo->numPhysProcessors  =
        kgs.ulNumOfPhysicalProcessors;

    for ( pDevWorkerCtrl =  kgs.pFirstDevWorkerCtrl, ulDevWorker = 0;
        kgs.ulNumOfDevWorker && (pDevWorkerCtrl <= kgs.pLastDevWorkerCtrl);
        pDevWorkerCtrl++, ulDevWorker++ )
    {
        pDevWorkerInfos[ulDevWorker].Tid       =
            pDevWorkerCtrl->ThrdCtrlHeader.Tid;
        pDevWorkerInfos[ulDevWorker].ThrdState =
            pDevWorkerCtrl->ThrdCtrlHeader.ThrdState;
        pDevWorkerInfos[ulDevWorker].ulIOCount =
            pDevWorkerCtrl->ulIOCount;
    }

    pRTEInfo->ulNumOfConcurrentDevWorkers =
        kgs.ulNumOfConcurrentDevWorkers;

    pRTEInfo->lNumOfDevWorkers      = ulDevWorker;
    pRTEInfo->ulDevWorkerInfoOffset = ulDevWorkerInfoOffset;

    //
    // --- UKT Info
    //
    for ( pUKTCtrl =  kgs.pFirstUKTCtrl, ulUKT = 0;
        pUKTCtrl <= kgs.pLastUKTCtrl;
        pUKTCtrl++ )
    {

        ulTotalTasks = eo92CreateTaskcluster ( pUKTCtrl,
            szCluster, sizeof ( szCluster ) ) ;

        strcpy ( pUKTInfos[ulUKT].szCluster, szCluster );

        pUKTInfos[ulUKT].ulTotalTasks      = ulTotalTasks;
        pUKTInfos[ulUKT].ulProcessor       = pUKTCtrl->ThrdCtrlHeader.Processor;
        pUKTInfos[ulUKT].CPURelationship   = pUKTCtrl->ThrdCtrlHeader.CPURelationship;
        pUKTInfos[ulUKT].ulUKTIndex        = pUKTCtrl->ulUKTIndex;
        pUKTInfos[ulUKT].Tid               = pUKTCtrl->ThrdCtrlHeader.Tid;
        pUKTInfos[ulUKT].ThrdState         = pUKTCtrl->ThrdCtrlHeader.ThrdState;
        pUKTInfos[ulUKT].ulActiveTasks     = pUKTCtrl->ulActiveTasks;
        pUKTInfos[ulUKT].dDispatcherCalls  = (double)((SAPDB_Int8)pUKTCtrl->DispatcherCalls);
        pUKTInfos[ulUKT].dTaskSwitchCalls  = (double)((SAPDB_Int8)pUKTCtrl->taskSwitches);

        //
        // --- get io info
        //
        for ( ulDevspace = 0, lNumOfDev = 0;
            ulDevspace < (ULONG) XPARAM(lMaxDevspaces);
            ulDevspace++ )
        {
            if (  !pUKTCtrl->pSingleIO[ulDevspace].selfIOStatisticResetPending
              && ( pUKTCtrl->pSingleIO[ulDevspace].ReadSelfIo.IOStat.TimeStat.ulCount ||
                   pUKTCtrl->pSingleIO[ulDevspace].WriteSelfIo.IOStat.TimeStat.ulCount ) )
            {
                strcpy( pDevspaceInfos[lNumOfDev].DevspaceName,
                    kgs.Dev0.pIOCtrl[ulDevspace].szDevspace );
                pDevspaceInfos[lNumOfDev].ulDevspaceNo = ulDevspace;
                pDevspaceInfos[lNumOfDev].ulReadCnt    =
                    pUKTCtrl->pSingleIO[ulDevspace].ReadSelfIo.IOStat.TimeStat.ulCount;
                pDevspaceInfos[lNumOfDev].ulWriteCnt   =
                    pUKTCtrl->pSingleIO[ulDevspace].WriteSelfIo.IOStat.TimeStat.ulCount;

                lNumOfDev++;
            }
        }
        pUKTInfos[ulUKT].lNumOfDevspaces      = lNumOfDev;
        pUKTInfos[ulUKT].ulDevspaceInfoOffset = ulDevspaceInfoOffset;

        ulDevspaceInfoOffset += sizeof(*pDevspaceInfos) *
            XPARAM(lMaxDevspaces);
        pDevspaceInfos  = (PCONS_DEV_INFO_REC)(pData + ulRTEInfoOffset + ulDevspaceInfoOffset);
        ulUKT++;
    }

    pRTEInfo->lNumOfUKTs      = ulUKT;
    pRTEInfo->ulUKTInfoOffset = ulUKTInfoOffset;

    //
    // --- Devspace info
    //
    pRTEInfo->lTraceDevspaceNo = kgs.Dev0.lTraceDevspaceNo;

    for ( ulDevspace = 0, lNumOfDEVis = 0;
        ulDevspace < (ULONG) XPARAM(lMaxDevspaces);
        ulDevspace++ )
    {
        if ( kgs.Dev0.pIOCtrl[ulDevspace].szDevspace[0] == '\0' )
            continue;

        strcpy( pDEViInfos[lNumOfDEVis].DevspaceName,
            kgs.Dev0.pIOCtrl[ulDevspace].szDevspace );

        for ( ulDevi = 0; ulDevi < (ULONG)DEVI_PER_VOLUME; ulDevi++ )
        {
            pDeviCtrl = kgs.pFirstDeviCtrl +
                ulDevspace * DEVI_PER_VOLUME + ulDevi;

            if ( pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED )
            {
                pDEViInfos[lNumOfDEVis].Tid          = pDeviCtrl->ThrdCtrlHeader.Tid;
                pDEViInfos[lNumOfDEVis].ulDEViIndex  = ulDevi;
                pDEViInfos[lNumOfDEVis].ulDevspaceNo = ulDevspace;
                pDEViInfos[lNumOfDEVis].DevIOStat    = pDeviCtrl->DevIOStat;
                if ( pDEViInfos[lNumOfDEVis].DevIOStat.ResetPending )
                {
                    memset( &pDEViInfos[lNumOfDEVis].DevIOStat.DevRead, 0, sizeof(teo52_IOStatisticRec));
                    memset( &pDEViInfos[lNumOfDEVis].DevIOStat.DevWrite, 0, sizeof(teo52_IOStatisticRec));
                }
                lNumOfDEVis ++;
            }
        }
    }

    pRTEInfo->lNumOfDEVis          = lNumOfDEVis;
    pRTEInfo->ulDEViInfoOffset     = ulDEViInfoOffset;

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(memSize - ulRTEInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)memSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetStorageInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulIdx;
    RTE_SystemRc                    rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_ULong                     ulRegionSize  = 0;
    PTASK_CTRL_REC                  pCurrTaskCtrl;
    SAPDB_Byte *                    pCurrStackPage;
    SAPDB_Byte *                    pStackLimit;
    SAPDB_Char *                    pCurrStackPointer;
    SAPDB_Byte *                    pStackBase;
    SAPDB_ULong                     ulTaskStorInfoOffset;
    PCONS_TASK_STORAGE_INFO_REC     pTaskStorInfos;
    MEMORY_BASIC_INFORMATION        MemInfo;
    SAPDB_ULong                     ulStorageInfoOffset;
    PCONS_STORAGE_INFO_REC          pStorageInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_Byte *                    pData;
    SAPDB_ULong                     memSize;
    SAPDB_ULong                     dataSize;
    SAPDB_UInt4                     retCode = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetStorageInfo",
        Console_Trace, 1);

    ulStorageInfoOffset  = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulTaskStorInfoOffset = ALIGN( sizeof(*pStorageInfo), ALIGNMENT_VALUE);

    memSize = ulStorageInfoOffset +
              ulTaskStorInfoOffset +
              ALIGN( sizeof(*pTaskStorInfos) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                                                         SAPDB_ToString (memSize),
                                                         SAPDB_ToString (CON_REQ_STORAGE));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader         = (RTE_ConsoleStandardDataHeader *) pData;
    pStorageInfo    = (PCONS_STORAGE_INFO_REC)(pData + ulStorageInfoOffset);
    pTaskStorInfos  = (PCONS_TASK_STORAGE_INFO_REC)(pData + ulStorageInfoOffset + ulTaskStorInfoOffset);

    pStorageInfo->ulTasksTotal = kgs.ulNumOfTasks;
    pStorageInfo->lTaskStacks  = 0;

    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl, ulIdx = 0;
         pCurrTaskCtrl <= kgs.pLastTaskCtrl;
         pCurrTaskCtrl++)
    {
        // --- Do a dirty read first!
        if ((  pCurrTaskCtrl->TaskState == TSK_INITIALIZED_EO00        ) ||
            (  pCurrTaskCtrl->TaskState == TSK_INACTIVE_EO00           ) ||
            (  pCurrTaskCtrl->TaskState == TSK_MARKED_FOR_STARTUP_EO00 ))
        {
            continue;
        }

        WAIT_UNTIL_ALONE( &pCurrTaskCtrl->pUKT->lExclusive );

        if (( pCurrTaskCtrl->TaskState != TSK_INITIALIZED_EO00 ) &&
            ( pCurrTaskCtrl->TaskState != TSK_INACTIVE_EO00 ) &&
            ( pCurrTaskCtrl->TaskState != TSK_MARKED_FOR_STARTUP_EO00 ))
        {
            pTaskStorInfos[ulIdx].ulTaskIndex  =
                pCurrTaskCtrl->ulTaskIndex;
            pTaskStorInfos[ulIdx].TaskType     =
                pCurrTaskCtrl->TaskType;
            strcpy (pTaskStorInfos[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString (false));
            pStackLimit       = pCurrTaskCtrl->Context.pStackLimit;
            pCurrStackPointer = pCurrTaskCtrl->pCurrStackPointer;
            pStackBase        = pCurrTaskCtrl->Context.pStackBase;

            pTaskStorInfos[ulIdx].ulStackUsed  =
                (SAPDB_ULong)pStackLimit - (SAPDB_ULong)pCurrStackPointer;
            pTaskStorInfos[ulIdx].ulStackFree  =
                (SAPDB_ULong)pCurrStackPointer - (SAPDB_ULong)pStackBase;
            ulIdx++;
        }

        CLEARLOCK( &pCurrTaskCtrl->pUKT->lExclusive );
    }

    pStorageInfo->lDynDatAvail           =
        (SAPDB_ULong)kgs.pSharedDynDataTop - (SAPDB_ULong)kgs.pSharedDynDataPtr;
    pStorageInfo->lDynDatUsed            =
        (SAPDB_ULong)kgs.pSharedDynDataPtr - (SAPDB_ULong)kgs.pSharedDynDataBot;

    pStorageInfo->lDynPoolAvail          =
        (SAPDB_ULong)kgs.pSharedDynPoolTop - (SAPDB_ULong)kgs.pSharedDynPoolPtr;
    pStorageInfo->lDynPoolUsed           =
        (SAPDB_ULong)kgs.pSharedDynPoolPtr - (SAPDB_ULong)kgs.pSharedDynPoolBot;

    pStorageInfo->ulKGSSize            = sizeof( kgs );
    pStorageInfo->ulKSSSize            = ALIGN(kgs.ulKSSSize,
        kgs.ulSystemPageSize);
    pStorageInfo->lNumOfTasks          = ulIdx;
    pStorageInfo->ulTaskStorInfoOffset = ulTaskStorInfoOffset;

    //
    //  --- query the allocated stack size of all tasks
    //
    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl;
        pCurrTaskCtrl <= kgs.pLastTaskCtrl;
        pCurrTaskCtrl++)
    {
        // --- query allocated task stack
        for (pCurrStackPage  = pCurrTaskCtrl->Context.pStackBase;
            pCurrStackPage  < pCurrTaskCtrl->Context.pStackLimit;
            pCurrStackPage += ulRegionSize)
        {
            if (!VirtualQuery(pCurrStackPage, &MemInfo, sizeof(MemInfo)))
                rc = GetLastError ();

            if (rc != NO_ERROR)
            {
                rc = GetLastError ();
                MSGCD (( WRN_QUERY_TASK_STACK, rc ));
                break;
            }

            ulRegionSize = MemInfo.RegionSize;

            if (pCurrStackPage + ulRegionSize > pCurrTaskCtrl->Context.pStackLimit)
            {
                ulRegionSize = (SAPDB_ULong)(pCurrTaskCtrl->Context.pStackLimit -
                    pCurrStackPage);
            }

            if (MemInfo.State & MEM_COMMIT)
                pStorageInfo->lTaskStacks += ulRegionSize;
        }
    }

    dataSize = ulStorageInfoOffset +
              ulTaskStorInfoOffset +
              ALIGN( sizeof(*pTaskStorInfos) * ulIdx, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulStorageInfoOffset);

    if (RTE_CONS_NO_ERROR != (retCode = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return retCode;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;

}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetUKTSlStatInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    ULONG                               ulUKT = 0;
    PUKT_CTRL_REC                       pTmpUKTCtrl;
    ULONG                               ulSleepStatInfoOffset;
    PCONS_SINGLE_UKT_SL_STAT_INFO_REC   pSleepStatInfo;
    SAPDB_ULong                         ulSleepTimeOffset;
    PCONS_UKT_SLEEPTIME_REC             pSleepTimeInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetUKTSlStatInfo",
        Console_Trace, 1);

    ulSleepTimeOffset       = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulSleepStatInfoOffset   = ALIGN( sizeof(*pSleepTimeInfo), ALIGNMENT_VALUE );

    memSize = ulSleepTimeOffset +
            ulSleepStatInfoOffset +
            ALIGN( sizeof(*pSleepStatInfo) * kgs.ulNumOfUKTs, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                SAPDB_ToString (memSize),
                SAPDB_ToString (CON_REQ_UKT_SLEEPTIME_STAT));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader         = (RTE_ConsoleStandardDataHeader *) pData;
    pSleepTimeInfo  = (PCONS_UKT_SLEEPTIME_REC)(pData + ulSleepTimeOffset);
    pSleepStatInfo  = (PCONS_SINGLE_UKT_SL_STAT_INFO_REC)(pData + ulSleepTimeOffset + ulSleepStatInfoOffset);

    if ( XPARAM(fTimeMeasure) )
    {
        SAPDB_UInt8 actMicroSeconds = RTESys_MicroSecTimer();

        for (pTmpUKTCtrl  = kgs.pFirstUKTCtrl;
            pTmpUKTCtrl <= kgs.pLastUKTCtrl;
            pTmpUKTCtrl++ )
        {
            if ( !pTmpUKTCtrl->SleepStatRecResetPending
                && (pTmpUKTCtrl->UKTRunningStat.TimeStat.ulCount > 0)
                && (actMicroSeconds > pTmpUKTCtrl->IdleTimeMeasureBeginMicroseconds) )
            {
                pSleepStatInfo[ulUKT].Tid           = pTmpUKTCtrl->ThrdCtrlHeader.Tid;
                pSleepStatInfo[ulUKT].ulUKTIndex    = pTmpUKTCtrl->ulUKTIndex;
                pSleepStatInfo[ulUKT].ulRUNQueueLen = pTmpUKTCtrl->ulRUNQueueLen;
                pSleepStatInfo[ulUKT].measuredMicroSeconds = actMicroSeconds - pTmpUKTCtrl->IdleTimeMeasureBeginMicroseconds;
                pSleepStatInfo[ulUKT].UKTIdleStat.ulCount = pTmpUKTCtrl->UKTRunningStat.TimeStat.ulCount;
                pSleepStatInfo[ulUKT].UKTIdleStat.ulAbsMicroSeconds =  pSleepStatInfo[ulUKT].measuredMicroSeconds
                    - pTmpUKTCtrl->UKTRunningStat.TimeStat.ulAbsMicroSeconds
                    - pTmpUKTCtrl->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds;
                if ( (KT_RUNNING == pTmpUKTCtrl->ThrdCtrlHeader.ThrdState)
                    && (pTmpUKTCtrl->RunningStateEnterMicroSeconds > actMicroSeconds) )
                {
                    pSleepStatInfo[ulUKT].UKTIdleStat.ulAbsMicroSeconds -= (actMicroSeconds - pTmpUKTCtrl->RunningStateEnterMicroSeconds);
                }
                pSleepStatInfo[ulUKT].UKTIOWaitStat = pTmpUKTCtrl->UKTIOWaitStat.TimeStat;
                ulUKT++ ;
            }
        }
    }

    pSleepTimeInfo->ulSleepStatInfoOffset = ulSleepStatInfoOffset;
    pSleepTimeInfo->lNumOfActiveUKTs      = ulUKT;

    dataSize = ulSleepTimeOffset +
               ulSleepStatInfoOffset +
               ALIGN( sizeof(*pSleepStatInfo) * ulUKT, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulSleepTimeOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetThreadTimes
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                         ulThreadTimesOffset;
    PCONS_SINGLE_THREAD_TIMES_INFO_REC  pThreadTimesInfo;
    PUKT_CTRL_REC                       pTmpUKTCtrl;
    FILETIME                            CreationTime;
    FILETIME                            ExitTime;
    SAPDB_Char                          szUktIndex[10];
    SAPDB_ULong                         ulThreadOffset;
    PCONS_THREAD_TIMES_INFO_REC         pThreadInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_ULong                         ulUKT;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetThreadTimes",
                             Console_Trace, 1);

    ulThreadOffset          = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulThreadTimesOffset     = ALIGN( sizeof(*pThreadInfo), ALIGNMENT_VALUE );

    memSize = ulThreadOffset +
              ulThreadTimesOffset +
              ALIGN( sizeof(*pThreadTimesInfo) * kgs.ulNumOfUKTs, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_THREAD_TIMES));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader             = (RTE_ConsoleStandardDataHeader *) pData;
    pThreadInfo         = (PCONS_THREAD_TIMES_INFO_REC)(pData + ulThreadOffset);
    pThreadTimesInfo    = (PCONS_SINGLE_THREAD_TIMES_INFO_REC)(pData + ulThreadOffset + ulThreadTimesOffset);

    for (pTmpUKTCtrl  = kgs.pFirstUKTCtrl, ulUKT = 0;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++, pThreadTimesInfo++, ulUKT++ )
    {
        ltoa   ( pTmpUKTCtrl->ulUKTIndex, szUktIndex, 10 );
        strcpy ( pThreadTimesInfo->szThreadName, "UKT") ;
        strcat ( pThreadTimesInfo->szThreadName, szUktIndex );

        pThreadTimesInfo->Tid           = pTmpUKTCtrl->ThrdCtrlHeader.Tid;

        ::GetThreadTimes(pTmpUKTCtrl->ThrdCtrlHeader.hThrd,
                         &CreationTime,&ExitTime,
                         &(pThreadTimesInfo->SystemTime),
                         &(pThreadTimesInfo->UserTime) );

        eo92CreateTaskcluster ( pTmpUKTCtrl, pThreadTimesInfo->szCluster,
                                sizeof ( pThreadTimesInfo->szCluster ) ) ;
    }

    pThreadInfo->ulThreadTimesOffset   = ulThreadTimesOffset;
    pThreadInfo->lNumOfUKTs            = ulUKT;

    dataSize = ulThreadOffset +
               ulThreadTimesOffset +
               ALIGN( sizeof(*pThreadTimesInfo) * ulUKT, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulThreadOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void    RTEThread_ConsoleStandardWorker::GetUKTCOMQueue
(
    struct RTETask_TaskScheduler*       pTaskSchedulerObject,
    PCONS_SINGLE_UKT_QUEUES_INFO_REC    pUKTQueues,
    SAPDB_ULong *                       pulUKTSingleQueueOffset,
    PCONS_SINGLE_QUEUE_INFO_REC *       ppUKTSingleQueue
)
{
    RTETask_CommonQueueElement*    pElement;
    RTETask_Task *                 pTmpTask;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetUKTCOMQueue",
        Console_Trace, 1);

    for (pUKTQueues->ulCOMQueueLen = 0,
         pUKTQueues->ulCOMQueueOffset = *pulUKTSingleQueueOffset,
         pElement = pTaskSchedulerObject->CommQueue().FirstInQueue();
         pElement;
         pElement = pTaskSchedulerObject->CommQueue().NextInQueue(*pElement))
    {
        if (NULL != (pTmpTask = pElement->Task()))
        {
            (*ppUKTSingleQueue)->ulTaskIndex = pTmpTask->ID();
            (*ppUKTSingleQueue)->PrioFlag    = pTmpTask->LegacyTaskCtrl().PrioFlag();
            pUKTQueues->ulCOMQueueLen++ ;
            (*ppUKTSingleQueue)++ ;
            *pulUKTSingleQueueOffset += sizeof( CONS_SINGLE_QUEUE_INFO_REC );
        }
        else
        {
            break;
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

void    RTEThread_ConsoleStandardWorker::GetUKTUKTQueue
(
    struct ukt_queue_record *           pLastUsedElem,
    PCONS_SINGLE_UKT_QUEUES_INFO_REC    pUKTQueues,
    SAPDB_ULong *                       pulUKTSingleQueueOffset,
    PCONS_SINGLE_QUEUE_INFO_REC *       ppUKTSingleQueue
)
{
    PDLQ_REC                      pCurrReq;
    PTASK_CTRL_REC                pTmpTaskCtrl;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetUKTUKTQueue",
        Console_Trace, 1);

    pUKTQueues->ulUKTQueueLen    = 0;
    pUKTQueues->ulUKTQueueOffset = *pulUKTSingleQueueOffset;

    if ( pLastUsedElem != NULL )
    {
        while ( (pCurrReq = pLastUsedElem->pRequest) != NULL )
        {
            if (NULL != (pTmpTaskCtrl = pCurrReq->pTaskCtrl))
            {
                (*ppUKTSingleQueue)->ulTaskIndex = pTmpTaskCtrl->ulTaskIndex;
                (*ppUKTSingleQueue)->PrioFlag    = pTmpTaskCtrl->PrioFlag;
                pUKTQueues->ulUKTQueueLen++ ;
                (*ppUKTSingleQueue)++ ;
                *pulUKTSingleQueueOffset += sizeof( CONS_SINGLE_QUEUE_INFO_REC );
            }

            pLastUsedElem           = pLastUsedElem->pNextElem;
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

void    RTEThread_ConsoleStandardWorker::GetUKTRUNQueue
(
    PUKT_CTRL_REC                       pUKT,
    PCONS_SINGLE_UKT_QUEUES_INFO_REC    pUKTQueues,
    SAPDB_ULong *                       pulUKTSingleQueueOffset,
    PCONS_SINGLE_QUEUE_INFO_REC *       ppUKTSingleQueue
)
{
    PTASK_CTRL_REC                pTmpTaskCtrl;
    SAPDB_ULong                   uldx;
    PUKT_RUN_QUEUE_REC            pRUNQue;
    PDLQ_REC                      pCurrReq;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetUKTRUNQueue",
        Console_Trace, 1);

    pRUNQue = pUKT->RUNQue;
    pUKTQueues->ulRUNQueueLen    = 0;
    pUKTQueues->ulRUNQueueOffset = *pulUKTSingleQueueOffset;

    for ( uldx = 0; uldx < pUKT->ulRUNQueueLen; uldx++ )
    {
        if (NULL != (pTmpTaskCtrl = pRUNQue[uldx].pTaskCtrl))
        {
            (*ppUKTSingleQueue)->ulTaskIndex = pTmpTaskCtrl->ulTaskIndex;
            (*ppUKTSingleQueue)->PrioFlag    = pTmpTaskCtrl->PrioFlag;
            (*ppUKTSingleQueue)->ulDynPrio   = pTmpTaskCtrl->ulDynPrio;
            (*ppUKTSingleQueue)->TaskState   = pTmpTaskCtrl->TaskState;
            pCurrReq = pRUNQue[uldx].pRequest ;
            (*ppUKTSingleQueue)->ulReqType   = pCurrReq ?
                pCurrReq->ulReqType : UNDEF ;
            pTmpTaskCtrl                     = pRUNQue[uldx].pFromTaskCtrl ;
            (*ppUKTSingleQueue)->ulFromTask  = pTmpTaskCtrl ?
                pTmpTaskCtrl->ulTaskIndex : UNDEF ;
            (*ppUKTSingleQueue)++ ;
            pUKTQueues->ulRUNQueueLen++;
            *pulUKTSingleQueueOffset += sizeof( CONS_SINGLE_QUEUE_INFO_REC );
        }
    }

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetUKTQueues
(
    SAPDBErr_MessageList &          messageList
)
{
    PUKT_CTRL_REC                       pTmpUKTCtrl;
    SAPDB_ULong                         ulUKTQueuesOffset;
    PCONS_SINGLE_UKT_QUEUES_INFO_REC    pUKTQueues;
    SAPDB_ULong                         ulUKTSingleQueueOffset;
    PCONS_SINGLE_QUEUE_INFO_REC         pUKTSingleQueue;
    SAPDB_ULong                         ulUKTOffset;
    PCONS_UKT_QUEUES_REC                pUKTInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_ULong                         ulUKT;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetUKTQueues",
        Console_Trace, 1);

    ulUKTOffset             = ALIGN(sizeof(*pHeader), ALIGNMENT_VALUE);
    ulUKTQueuesOffset       = ALIGN(sizeof(*pUKTInfo), ALIGNMENT_VALUE );
    ulUKTSingleQueueOffset  = ulUKTQueuesOffset +
        ALIGN(sizeof (*pUKTQueues) * kgs.ulNumOfUKTs, ALIGNMENT_VALUE);

    memSize = ulUKTOffset +
        ulUKTSingleQueueOffset +
        ALIGN( sizeof(*pUKTSingleQueue) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_UKT_QUEUES));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader             = (RTE_ConsoleStandardDataHeader *) pData;
    pUKTInfo            = (PCONS_UKT_QUEUES_REC)(pData + ulUKTOffset);
    pUKTQueues          = (PCONS_SINGLE_UKT_QUEUES_INFO_REC)(pData + ulUKTOffset + ulUKTQueuesOffset);
    pUKTSingleQueue     = (PCONS_SINGLE_QUEUE_INFO_REC)(pData + ulUKTOffset + ulUKTSingleQueueOffset);


    for (pTmpUKTCtrl  = kgs.pFirstUKTCtrl, ulUKT = 0;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++, pUKTQueues++, ulUKT++ )
    {
        pUKTQueues->Tid           = pTmpUKTCtrl->ThrdCtrlHeader.Tid;
        pUKTQueues->ulUKTIndex    = pTmpUKTCtrl->ulUKTIndex;


        GetUKTCOMQueue ( pTmpUKTCtrl->pTaskSchedulerObject, pUKTQueues,
            &ulUKTSingleQueueOffset, &pUKTSingleQueue ) ;


        GetUKTUKTQueue ( pTmpUKTCtrl->UKTQue.pLastUsedElem, pUKTQueues,
            &ulUKTSingleQueueOffset, &pUKTSingleQueue ) ;

        GetUKTRUNQueue ( pTmpUKTCtrl, pUKTQueues,
            &ulUKTSingleQueueOffset, &pUKTSingleQueue ) ;

    }

    pUKTInfo->ulUKTQueuesOffset = ulUKTQueuesOffset;
    pUKTInfo->lNumOfActiveUKTs  = ulUKT;

    dataSize = ulUKTOffset +
               ulUKTSingleQueueOffset +
               ALIGN( sizeof(*pUKTSingleQueue) * ulUKT, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulUKTOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetQueueInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                         ulIdx;
    PTASK_CTRL_REC                      pCurrTaskCtrl;
    SAPDB_ULong                         ulQueStatInfoOffset;
    PCONS_SINGLE_QUEUE_STAT_INFO_REC    pQueStatInfo;
    SAPDB_ULong                         ulQueInfoOffset;
    PCONS_QUEUE_STAT_INFO_REC           pQueInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_ULong                         dataSize;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetQueueInfo",
        Console_Trace, 1);

    ulQueInfoOffset         = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulQueStatInfoOffset     = ALIGN( sizeof(*pQueInfo), ALIGNMENT_VALUE );

    memSize = ulQueInfoOffset +
        ulQueStatInfoOffset +
        ALIGN( sizeof(*pQueStatInfo) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_QUEUE_STAT));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader          = (RTE_ConsoleStandardDataHeader *) pData;
    pQueInfo         = (PCONS_QUEUE_STAT_INFO_REC)(pData + ulQueInfoOffset);
    pQueStatInfo     = (PCONS_SINGLE_QUEUE_STAT_INFO_REC)(pData + ulQueInfoOffset + ulQueStatInfoOffset);


    for (pCurrTaskCtrl =  kgs.pFirstTaskCtrl, ulIdx = 0;
        pCurrTaskCtrl <= kgs.pLastTaskCtrl;
        pCurrTaskCtrl++)
    {
        if ( !pCurrTaskCtrl->QueueStatRecResetPending &&
            ( pCurrTaskCtrl->pLegacyTaskCtrlObject->CommQueueCount() ||
            pCurrTaskCtrl->QueStat.Resetable.ulUKTQueCount  ||
            pCurrTaskCtrl->QueStat.Resetable.ulUToUQueCount ) )
        {
            pQueStatInfo[ulIdx].ulTaskIndex  = pCurrTaskCtrl->ulTaskIndex;
            pQueStatInfo[ulIdx].QueStat      = pCurrTaskCtrl->QueStat;
            ulIdx++;
        }
    }

    pQueInfo->lNumOfActiveTasks   = ulIdx;
    pQueInfo->ulQueStatInfoOffset = ulQueStatInfoOffset;

    dataSize = ulQueInfoOffset +
               ulQueStatInfoOffset +
               ALIGN( sizeof(*pQueStatInfo) * ulIdx, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulQueInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetTaskCountInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_UInt4                         j2;
    SAPDB_ULong                         ulIdx = 0;
    PTASK_CTRL_REC                      pCurrTaskCtrl, pFirstTaskCtrl, pLastTaskCtrl;
    SAPDB_ULong                         ulTaskCountsInfoOffset;
    SAPDB_Long                          lTaskTypeToShow = 0 ;
    PCONS_SINGLE_TASK_COUNTS_INFO_REC   pTaskCountsInfo;
    SAPDB_ULong                         ulTaskInfoOffset;
    PCONS_TASK_COUNTS_INFO_REC          pTaskInfo;
    RTE_ConsoleStandardDataHeader *     pHeader;
    SAPDB_Byte *                        pData;
    SAPDB_ULong                         memSize;
    SAPDB_UInt4                         bytesRead;
    SAPDB_UInt4                         bytesTotal;
    SAPDB_ULong                         dataSize;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetTaskCountInfo",
        Console_Trace, 1);

    ulTaskInfoOffset         = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    ulTaskCountsInfoOffset   = ALIGN( sizeof(*pTaskInfo), ALIGNMENT_VALUE );

    memSize = ulTaskInfoOffset +
        ulTaskCountsInfoOffset +
        ALIGN( sizeof(*pTaskCountsInfo) * kgs.ulNumOfTasks, ALIGNMENT_VALUE);

    pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
    if (NULL == pData)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSize),
            SAPDB_ToString (CON_REQ_TASK_COUNTS));
        return RTE_CONS_ERROR;
    }

    memset (pData, 0, memSize);

    pHeader             = (RTE_ConsoleStandardDataHeader *) pData;
    pTaskInfo           = (PCONS_TASK_COUNTS_INFO_REC)(pData + ulTaskInfoOffset);
    pTaskCountsInfo     = (PCONS_SINGLE_TASK_COUNTS_INFO_REC)(pData + ulTaskInfoOffset + ulTaskCountsInfoOffset);


    if (RTE_CONS_NO_ERROR != (rc = GetRequestDataContinue ((SAPDB_Byte *)pTaskInfo, DATA_REQ, sizeof (*pTaskInfo),
                                                            bytesRead, bytesTotal, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_GET_DATA);
        return rc;
    }

    pTaskInfo->usReturnCode = CON_NO_ERROR;

    if ( pTaskInfo->lNumOfActiveTasks < 0 )
    {                                   /* <0 ==> taskindex to show */
        SAPDB_Long  TaskIdToShow = (-pTaskInfo->lNumOfActiveTasks) ;

        /* PTS 1104727 */
        if ( TaskIdToShow < (SAPDB_Long)kgs.pFirstTaskCtrl->ulTaskIndex ||
            TaskIdToShow > (SAPDB_Long)kgs.pLastTaskCtrl->ulTaskIndex   )
            pTaskInfo->usReturnCode = CON_WRONG_TASK;
        else
        {
            pFirstTaskCtrl = kgs.pFirstTaskCtrl + TaskIdToShow - 1 ;
            pLastTaskCtrl  = pFirstTaskCtrl;
        }
    }
    else
    {                                   /* >0 ==> taskindex to show */
        pFirstTaskCtrl  = kgs.pFirstTaskCtrl;
        pLastTaskCtrl   = kgs.pLastTaskCtrl;
        lTaskTypeToShow = pTaskInfo->lNumOfActiveTasks ;
    }


    if ( pTaskInfo->usReturnCode == CON_NO_ERROR )
    {
        for (pCurrTaskCtrl =  pFirstTaskCtrl, ulIdx;
            pCurrTaskCtrl <= pLastTaskCtrl;
            pCurrTaskCtrl++)
        {
            if ( EO52_RESET_TASK_FULL_PENDING != pCurrTaskCtrl->TaskStatRecResetPending &&
                pCurrTaskCtrl->ulDispatcherCalls > 1 &&
                (lTaskTypeToShow == 0 || lTaskTypeToShow == pCurrTaskCtrl->TaskType))
            {
                pTaskCountsInfo[ulIdx].ulTaskIndex     = pCurrTaskCtrl->ulTaskIndex;
                pTaskCountsInfo[ulIdx].TaskType        = pCurrTaskCtrl->TaskType;
                strcpy (pTaskCountsInfo[ulIdx].displayName, pCurrTaskCtrl->pTaskObject->TypeAsString ());
                pTaskCountsInfo[ulIdx].ulBegExclCalls  = pCurrTaskCtrl->ulBegExclCalls;
                pTaskCountsInfo[ulIdx].ulSelfSuspCount = pCurrTaskCtrl->ulSelfSuspCount;
                if ( EO52_NO_RESET_PENDING != pCurrTaskCtrl->TaskStatRecResetPending )
                {
                    memset( &(pTaskCountsInfo[ulIdx].StateStat), 0, sizeof(teo52_TaskStateStatisticRec) );
                }
                else
                {
                    pTaskCountsInfo[ulIdx].StateStat       = pCurrTaskCtrl->TaskStateStat;
                }
                pTaskCountsInfo[ulIdx].PrioStat        = pCurrTaskCtrl->PrioStat;
                pTaskCountsInfo[ulIdx].TaskOMSStat     = pCurrTaskCtrl->TaskOMSStat; // 1106969
                pTaskCountsInfo[ulIdx].ulDispatcherCalls = pCurrTaskCtrl->ulDispatcherCalls;
                if ( !pCurrTaskCtrl->QueueStatRecResetPending )
                {
                    pTaskCountsInfo[ulIdx].ulCommands      = pCurrTaskCtrl->pLegacyTaskCtrlObject->CommQueueCount();
                }

                if (( pCurrTaskCtrl->TaskType  == TT_US_EO00 ) ||
                    ( pCurrTaskCtrl->TaskType  == TT_EV_EO00 ) ||
                    ( pCurrTaskCtrl->TaskType  == TT_UT_EO00 ))
                { pTaskCountsInfo[ulIdx].ApplPid =
                pCurrTaskCtrl->pUserCommCtrl->pidClientPID;
                strcpy ( pTaskCountsInfo[ulIdx].szClientNode,
                    pCurrTaskCtrl->pUserCommCtrl->szClientNode) ;
                }
                else
                { pTaskCountsInfo[ulIdx].ApplPid = RTE_UNDEF_OSPID;
                pTaskCountsInfo[ulIdx].szClientNode[0] = '\0' ;
                }


                pTaskCountsInfo[ulIdx].lResumeCount = pCurrTaskCtrl->lResumeCount;
                pTaskCountsInfo[ulIdx].totalResumerCount = pCurrTaskCtrl->totalResumerCount;
                for ( j2 = 0; j2 < MAX_RESUMER_PIDS; j2++ )
                {
                    pTaskCountsInfo[ulIdx].resumerPid[j2] = pCurrTaskCtrl->resumerPid[j2];
                }


                ulIdx++;
            }
        }
    }

    pTaskInfo->lNumOfActiveTasks      = ulIdx;
    pTaskInfo->ulTaskCountsInfoOffset = ulTaskCountsInfoOffset;
    pTaskInfo->fTimeMeasureActive     = XPARAM(fTimeMeasure);

    dataSize = ulTaskInfoOffset +
        ulTaskCountsInfoOffset +
        ALIGN( sizeof(*pTaskCountsInfo) * ulIdx, ALIGNMENT_VALUE);

    pHeader->rc             =   RTE_CONS_NO_ERROR;
    pHeader->lastSend       =   true;
    pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - ulTaskInfoOffset);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
    {
        RTEMem_RteAllocator::Instance().Deallocate(pData);
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    RTEMem_RteAllocator::Instance().Deallocate(pData);
    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  RTEThread_ConsoleStandardWorker::GetAWEInfo
(
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_ULong                     ulIdx = 0;
    PCONS_AWE_PHYS_PAGE_INFO_REC    pAWEPhysPageInfos;
    PCONS_AWE_MAP_PAGE_INFO_REC     pAWEMapPageInfos;
    SAPDB_UInt4                     MapPageIdx;
    SAPDB_UInt4                     PhysPageNo;
    SAPDB_Bool                      Reserved;
    SAPDB_UInt8                     MapCount;
    void  *                         pMappingPageAddr;
    SAPDB_Bool                      Ok;
    PCONS_AWE_INFO_REC              pAWEInfo;
    RTE_ConsoleStandardDataHeader * pHeader;
    SAPDB_ULong                     memSizeAWEInfo;
    SAPDB_Byte *                    pDataAWEInfo;
    SAPDB_ULong                     maxPages;
    SAPDB_ULong                     memSize;
    SAPDB_ULong                     tmpNumPages;
    SAPDB_ULong                     dataSize;
    SAPDB_ULong                     tmpPageNo;
    SAPDB_UInt4                     headerSize;
    SAPDB_Byte *                    pData;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::GetAWEInfo",
        Console_Trace, 1);

    headerSize = ALIGN( sizeof(*pHeader), ALIGNMENT_VALUE);
    memSizeAWEInfo = headerSize + ALIGN( sizeof(*pAWEInfo), ALIGNMENT_VALUE );

    pDataAWEInfo = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSizeAWEInfo);
    if (NULL == pDataAWEInfo)
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
            SAPDB_ToString (memSizeAWEInfo),
            SAPDB_ToString (CON_REQ_AWE));
        return RTE_CONS_ERROR;
    }

    memset (pDataAWEInfo, 0, memSizeAWEInfo);

    pHeader    = (RTE_ConsoleStandardDataHeader *) pDataAWEInfo;
    pAWEInfo   = (PCONS_AWE_INFO_REC)(pDataAWEInfo + headerSize);

    if ( RTEMem_AWEAllocator::Instance().AWEAvailable() )
    {
        pAWEInfo->AWEAvailable           = true;
        pAWEInfo->NumOfPhysPages         = RTEMem_AWEAllocator::Instance().NumOfPhysicalPages();
        pAWEInfo->NumOfFreePhysPages     = RTEMem_AWEAllocator::Instance().NumOfFreePhysicalPages();
        pAWEInfo->NumOfMapAreaPages      = RTEMem_AWEAllocator::Instance().NumOfMapAreaPages();
        pAWEInfo->NumOfFreeMapAreaPages  = RTEMem_AWEAllocator::Instance().NumOfFreeMapAreaPages();
        pAWEInfo->NumOfMaps              = RTEMem_AWEAllocator::Instance().NumOfMaps();
        pAWEInfo->NumOfUnMaps            = RTEMem_AWEAllocator::Instance().NumOfUnMaps();

        /* Sending AWE info */
        pHeader->rc             =   RTE_CONS_NO_ERROR;
        pHeader->lastSend       =   true;
        pHeader->dataLen        =   (SAPDB_UInt4)(memSizeAWEInfo - headerSize);

        if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pDataAWEInfo, (SAPDB_UInt4)memSizeAWEInfo,
                                                      DATA_REP, messageList)))
        {
            RTEMem_RteAllocator::Instance().Deallocate(pDataAWEInfo);
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
            return rc;
        }

        RTEMem_RteAllocator::Instance().Deallocate(pDataAWEInfo);

        memSize = ALIGN (AWE_SHM_SIZE, ALIGNMENT_VALUE);

        pData = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (memSize);
        if (NULL == pData)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_ALLOC_DATA,
                SAPDB_ToString (memSize),
                SAPDB_ToString (CON_REQ_AWE));
            return RTE_CONS_ERROR;
        }

        pHeader             = (RTE_ConsoleStandardDataHeader *) pData;

        pAWEMapPageInfos    = (PCONS_AWE_MAP_PAGE_INFO_REC) (pData + headerSize);
        maxPages = (memSize - headerSize) / ALIGN (sizeof(*pAWEMapPageInfos), ALIGNMENT_VALUE);
        Ok = RTEMem_AWEAllocator::Instance().GetFirstMapPageInfo( MapPageIdx, pMappingPageAddr, PhysPageNo, Reserved, MapCount );
        do
        {
            tmpNumPages = 0;
            memset (pData, 0, memSize);
            while (Ok && tmpNumPages < maxPages)
            {
                pAWEMapPageInfos[tmpNumPages].pMappingPageAddr = pMappingPageAddr;
                pAWEMapPageInfos[tmpNumPages].PhysPageNo       = PhysPageNo;
                pAWEMapPageInfos[tmpNumPages].Reserved         = Reserved ? 1 : 0;
                pAWEMapPageInfos[tmpNumPages].MapCount         = MapCount;

                tmpNumPages++;

                Ok = RTEMem_AWEAllocator::Instance().GetNextMapPageInfo ( MapPageIdx, pMappingPageAddr, PhysPageNo, Reserved, MapCount );
            }

            //Sending Data
            dataSize                = ALIGN (sizeof(*pAWEMapPageInfos) * tmpNumPages, ALIGNMENT_VALUE) + headerSize;
            pHeader->rc             = RTE_CONS_NO_ERROR;
            if (Ok)
            {
                pHeader->lastSend       =   false;
            }
            else
            {
                pHeader->lastSend       =   true;
            }

            pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - headerSize);
            if (RTE_CONS_NO_ERROR != (rc = SendReplyDataNext (pData, (SAPDB_UInt4)dataSize,
                                                              DATA_REP, messageList)))
            {
                RTEMem_RteAllocator::Instance().Deallocate(pData);
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
                return rc;
            }
        } while (Ok);

        maxPages = (memSize - headerSize) / ALIGN (sizeof(*pAWEPhysPageInfos), ALIGNMENT_VALUE);
        pAWEPhysPageInfos    = (PCONS_AWE_PHYS_PAGE_INFO_REC) (pData + headerSize);

        PhysPageNo = 0;
        Ok = RTEMem_AWEAllocator::Instance().GetPhysPageInfo(PhysPageNo, Reserved, MapCount );
        do
        {
            memset (pData, 0, memSize);
            tmpPageNo = 0;
            while (Ok && tmpPageNo < maxPages)
            {
                pAWEPhysPageInfos[ tmpPageNo ].Reserved = Reserved ? 1 : 0;
                pAWEPhysPageInfos[ tmpPageNo ].MapCount = MapCount;

                PhysPageNo++;
                tmpPageNo++;

                Ok = RTEMem_AWEAllocator::Instance().GetPhysPageInfo(PhysPageNo, Reserved, MapCount );
            }

            dataSize                = ALIGN (sizeof(*pAWEPhysPageInfos) * tmpPageNo, ALIGNMENT_VALUE) + headerSize;

            pHeader->rc             =   RTE_CONS_NO_ERROR;
            if (Ok)
            {
                pHeader->lastSend       =   false;
            }
            else
            {
                pHeader->lastSend       =   true;
            }

            pHeader->dataLen        =   (SAPDB_UInt4)(dataSize - headerSize);

            if (RTE_CONS_NO_ERROR != (rc = SendReplyDataNext (pData, (SAPDB_UInt4)dataSize, DATA_REP, messageList)))
            {
                RTEMem_RteAllocator::Instance().Deallocate(pData);
                messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
                return rc;
            }
        } while (Ok);

        RTEMem_RteAllocator::Instance().Deallocate(pData);
    }
    else
    {
        pAWEInfo->AWEAvailable = false;

        pHeader->rc             =   RTE_CONS_NO_ERROR;
        pHeader->lastSend       =   true;
        pHeader->dataLen        =   (SAPDB_UInt4)(memSizeAWEInfo - headerSize);

        if (RTE_CONS_NO_ERROR != (rc = SendReplyData (pDataAWEInfo, (SAPDB_UInt4)memSizeAWEInfo,
                                                      DATA_REP, messageList)))
        {
            RTEMem_RteAllocator::Instance().Deallocate(pDataAWEInfo);
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
            return rc;
        }

        RTEMem_RteAllocator::Instance().Deallocate(pDataAWEInfo);
    }

    return RTE_CONS_NO_ERROR;
}

#endif


//****************************************************************************
//*** TESTS ******************************************************************
//****************************************************************************

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_ConsoleStandardWorker::ConsoleLongTestRequest
(
    SAPDB_UInt4 const               type,
    SAPDBErr_MessageList &          messageList
)
{

    RTE_ConsoleTestDataPacket           longTestReply;
    SAPDB_UInt4                         rc = RTE_CONS_NO_ERROR;


    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::ConsoleLongTestRequest",
                             Console_Trace, 1);

    memset (&longTestReply, 0, sizeof (longTestReply));
    ConsoleTest02 (longTestReply.data);

    longTestReply.header.rc             =   RTE_CONS_NO_ERROR;
    longTestReply.header.lastSend       =   true;
    longTestReply.header.dataLen        =   sizeof (longTestReply.data);

    if (RTE_CONS_NO_ERROR != (rc = SendReplyData ((SAPDB_Byte *)&longTestReply, sizeof (longTestReply),
                                            DATA_REP, messageList)))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_SEND_DATA);
        return rc;
    }

    return RTE_CONS_NO_ERROR;
}

/*---------------------------------------------------------------------------*/

void RTEThread_ConsoleStandardWorker::ConsoleTest01
(
    PCONSOLE_DATA_REC           pConsoleDataRec
)
{

    strcpy ((SAPDB_Char *)pConsoleDataRec->test01String, "TEST");
}

void RTEThread_ConsoleStandardWorker::ConsoleTest02
(
    TestLongString           longTestString
)
{
    SAPDB_UInt4                             size = sizeof (TestLongString);

    for (SAPDB_UInt4    idx=0; idx < size - 10; idx += 80)
    {
        strcpy ((char *)longTestString + idx, SAPDB_ToString (idx));
    }
}
//****************************************************************************
//*** Ende TESTS *************************************************************
//****************************************************************************

