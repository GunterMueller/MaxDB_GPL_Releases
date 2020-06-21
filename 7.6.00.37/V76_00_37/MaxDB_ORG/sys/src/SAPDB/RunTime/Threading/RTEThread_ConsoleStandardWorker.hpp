/*!
  @file           RTEThread_ConsoleStandardWorker.hpp
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



#ifndef RTETHREAD_CONSOLESTANDARDWORKER_HPP
#define RTETHREAD_CONSOLESTANDARDWORKER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/Threading/RTEThread_ConsoleWorkerBase.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined (_WIN32)
#define     AWE_SHM_SIZE        2 * 1024 * 1024
#endif
#define ALL_TASK_COUNTS   TRUE
#define TASK_STATES_ONLY  FALSE
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          RTEThread_ConsoleStandardWorker
   @description    Request processing (Singleton)


                This class is derived from the abstract class
                'RTEThread_ConsoleWorkerBase'.
                This class implements the request processing for the standard 
                requests (current XCons requests). This includes the protocol
                handling on the data level (cf. RTE_ConsoleDataCommunication)
                The communication is not subject of this class.

 */


class RTEThread_ConsoleStandardWorker : public RTEThread_ConsoleWorkerBase
{
public:
  /*!
     @description    Constructor
     @param          openMode [in]  - RTE_CONSOLE_CLIENT/SERVER_MODE
     @param          remotePID [in] - Process ID of the client
     @param          myRef [in]     - Own reference (= connection slot number)
     @param          remoteRef [in] - Remote (XCons) reference

   */

    RTEThread_ConsoleStandardWorker (SAPDB_UInt4 const           openMode )
    : RTEThread_ConsoleWorkerBase (openMode) 
        {SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::RTEThread_ConsoleStandardWorker", 
                                  Console_Trace, 9);}

  /*!
     @description    Destructor

   */


    ~RTEThread_ConsoleStandardWorker () 
        {SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleStandardWorker::~RTEThread_ConsoleStandardWorker", 
                                  Console_Trace, 9);}

  /*!
     @description   Main method for all requests

                    Determines the the specific request type from the data header and
                    calls the method for processing this specific request.

     @param         messageList [in/out] - Message list
     @return        RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT


   */


    SAPDB_UInt4 RequestProcessing   (SAPDBErr_MessageList &             messageList); 

  /*!
     @description    Send the message list to the client if a error has occured during 
                processing a request.


                  The responds type 'ALL_REP' is intended to the situation where the
                  request is not known anymore (e.g. in case of a timeout)
     @param          respType [in] - responds type (DATA_REP, OPEN_REP, CLOSE_REP, ALL_REP)
     @param          errCode [in] - Error code (RTE_CONS_ERROR, RTE_CONS_FATAL_ERROR, RTE_CONS_TIMEOUT)
     @param          messageListToSend [in/out] - Message list to be send. If the function
                                             fails (rc = false) the derived error messages are
                                             added to this message list.
     @return         true/false


   */


    SAPDB_Bool  SendError           (SAPDB_UInt1 const                  respType,
                                     SAPDB_UInt4 const                  errCode,
                                     SAPDBErr_MessageList &             messageListToSend);

  /*!
     @description    Process a connect request
     @param          hConnect [in] - Unique handle of the created connection.
                                       To be send to the client for later connects.
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT


   */


    SAPDB_UInt4 Connect             (RTE_ConsoleHandle const            hConnect,
                                     SAPDBErr_MessageList &             messageList);

  /*!
     @description    Reject a connect request
     @param          sendMessageList [in/out] - Message list to be send

   */
    virtual void        ConnectReject       ( SAPDBErr_MessageList&        sendMessageList );


  /*!
     @description    Process a disconnect request
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT


   */


    SAPDB_UInt4 Disconnect          (SAPDBErr_MessageList &             messageList);

private:
/*!
   @description    Methods for processing the XCons requests based on the standard communication

 */

/* Request handled without dynamic memory */

    SAPDB_UInt4 ConsoleSimpleRequests  (SAPDB_UInt4 const                  type,
                                     SAPDBErr_MessageList &             messageList);

    void        ShowSetCheckInterval(PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        DebugTask           (PCONSOLE_DATA_REC                  pConsoleDataRec); 

#if defined (_WIN32)
/* Requests only handled by NT */
    void        CancelTask          (PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        SetUKTAffinity      (PCONSOLE_DATA_REC                  pConsoleDataRec); 

    void        SetProcessorAffinityMask    (PCONSOLE_DATA_REC                  pConsoleDataRec); 

    void        DebugBreak          (PCONSOLE_DATA_REC                  pConsoleDataRec); 
    
    void        KillTask            (PCONSOLE_DATA_REC                  pConsoleDataRec); 

    void        CrashDump           (); 

    void        MiniDump            (); 

    void        ShowParam           (PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        ForceIOError        (PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        SetDebugLevel       (PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        GetTotalCountInfo   (PCONSOLE_DATA_REC                  pConsoleDataRec);

    void        ShowSuspends        (PCONSOLE_DATA_REC                  pConsoleDataRec);
#endif
/* Requests handled by NT and UNIX */

    void        ResetRegionInfo     ();

    static void ResetIOInfo         ();

    static void ResetTaskCountInfo  (SAPDB_Bool                         fAllTaskCounts);

    static void ResetUKTSlStatInfo  ();

public:
    /*! 
        @brief Callback for parameter class to modifiy value of time measurement flag
        @param enabled [in] true if now to be enabled, false if now to be diabled
     */
    static void ChangeTimeEnableValue(bool enabled);
private:
    void        ResetQueueInfo      ();

    void        ResetSuspends       ();

    void        ResetAll            ();

    void        EnableTimeMeasure   ();

    void        DisableTimeMeasure  ();

    SAPDB_Bool  TraceFlush          (SAPDBErr_MessageList &             messageList);



/* Request handled with dynamic memory */

#if defined (_WIN32)

/* Requests only handled by NT */

    SAPDB_UInt4  GetTaskInfo         (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetConnectionsInfo  (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetIOInfo           (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetAIOInfo          (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetRegionInfo       (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetRteInfo          (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetStorageInfo      (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetUKTSlStatInfo    (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetThreadTimes      (SAPDBErr_MessageList &             messageList);

    void         GetUKTCOMQueue      (struct RTETask_TaskScheduler*      pTaskSchedulerObject,
                                      PCONS_SINGLE_UKT_QUEUES_INFO_REC   pUKTQueues,
                                      SAPDB_ULong *                      pulUKTSingleQueueOffset,
                                      PCONS_SINGLE_QUEUE_INFO_REC *      ppUKTSingleQueue);

    void         GetUKTUKTQueue      (struct ukt_queue_record *          pLastUsedElem,
                                      PCONS_SINGLE_UKT_QUEUES_INFO_REC   pUKTQueues,
                                      SAPDB_ULong *                      pulUKTSingleQueueOffset,
                                      PCONS_SINGLE_QUEUE_INFO_REC *      ppUKTSingleQueue);

    void         GetUKTRUNQueue      (PUKT_CTRL_REC                      pUKT,
                                      PCONS_SINGLE_UKT_QUEUES_INFO_REC   pUKTQueues,
                                      SAPDB_ULong *                      pulUKTSingleQueueOffset,
                                      PCONS_SINGLE_QUEUE_INFO_REC *      ppUKTSingleQueue);

    SAPDB_UInt4  GetUKTQueues        (SAPDBErr_MessageList &             messageList);
  
    SAPDB_UInt4  GetQueueInfo        (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetTaskCountInfo    (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetAWEInfo          (SAPDBErr_MessageList &             messageList);
#endif

    SAPDB_UInt4  GetMoveInfo         (SAPDBErr_MessageList &             messageList);
    SAPDB_UInt4  GetTaskMoveInfo     (SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  SendRWRegionPacket  (SAPDB_Byte *                       pData,
                                      SAPDB_ULong                        dataSize,
                                      SAPDBErr_MessageList &             messageList);

    SAPDB_UInt4  GetRWRegions        (SAPDBErr_MessageList &             messageList);
/* Test requests */
    SAPDB_UInt4 ConsoleLongTestRequest   (SAPDB_UInt4 const              type,
                                         SAPDBErr_MessageList &         messageList);

    void        ConsoleTest01       (PCONSOLE_DATA_REC                  pConsoleDataRec);
    void        ConsoleTest02       (TestLongString                     longTestString);

};


#endif  /* RTETHREAD_CONSOLESTANDARDWORKER_HPP */
