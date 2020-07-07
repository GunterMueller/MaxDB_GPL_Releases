/*!
  @file           vos53k.cpp
  @author         RaymondR
  @brief          Communication
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#include "RunTime/System/RTESys_MemoryBarrier.h"

/*
 * INCLUDE FILES
 */
//#include "gos00.h"
#include "gos41.h"
#include "heo00.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "gos003.h"
#include "heo52.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "geo67.h"
#include "heo56.h"
#include "gos74.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "heo420.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"

/*
 *  DEFINES
 */
#define MOD__  "VOS53KC : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */
static RTECKC_KernelCommunication *  pKernelCommunication = 0;


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static tsp01_CommErr_Enum sql53k_check_comm_seg  ( PCOMM_SEG_HEADER_REC  pCSHeader,
                                                   PCOMM_SEG_OFFSET_REC  pCSOffsetList );

static tsp01_CommErr_Enum sql53k_receive         ( PTASK_CTRL_REC        pTaskCtrl,
                                                   PUSER_TASK_COMM_CTRL  pUserCommCtrl,
                                                   PCOMM_SEG_HEADER_REC  pCSHeader,
                                                   ULONG                 ulTimeOut,
                                                   PCHAR                 *ppcRcvPacket,
                                                   PULONG                pulRcvPacketLen );

static tsp01_CommErr_Enum sql53k_reply           ( PTASK_CTRL_REC        pTaskCtrl,
                                                   PUSER_TASK_COMM_CTRL  pUserCommCtrl,
                                                   PCHAR                 pcRplPacket,
                                                   ULONG                 ulRplPacketLen );

static VOID sql53k_release                       ( PTASK_CTRL_REC        pTaskCtrl );

static VOID sql53k_remove_unrel_conn             ( PTASK_CTRL_REC        pTaskCtrl );

static VOID sql53k_handle_ping                   ( PTASK_CTRL_REC        pTaskCtrl,
                                                   PUSER_TASK_COMM_CTRL  pUserCommCtrl );

static tsp00_Uint1 sql53k_new_swap_type ();

void os53_DumpCommunicationSegment(PTASK_CTRL_REC pTaskCtrl);


#if defined(CLIENT_TRANSFER_TEST) || defined(UDE_SERVER_TRANSFER_TEST)
 void ClientTransferTest();
 void UDEServerTransferTest();
#endif

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */


VOID         vconnect ( PROCESS_ID                   pid,
                        tsp00_Int4 VAR_VALUE_REF     max_data_len,
                        tsp01_CommErr VAR_VALUE_REF  returncode )
  {
  #undef  MF__
  #define MF__ MOD__"vconnect"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                pTaskCtrl       = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl       = &kgs.pFirstTaskCtrl[pid - 1];
  #endif
  RTETask_Task                   &task           = *pTaskCtrl->pTaskObject;
  PUSER_TASK_COMM_CTRL           pUserCommCtrl   = pTaskCtrl->pUserCommCtrl;
  PCOMM_SEG_HEADER_REC           pCSHeader;
  ULONG                          ulCnt;
  PCOMM_SEG_OFFSET_REC           pCSOffsetList;
  ULONG                          rc;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pTaskCtrl->TaskState = TSK_CONNECTWAIT_EO00;

  if ( !pKernelCommunication )
    pKernelCommunication = RTE_KGS::Instance().KernelCommunication();

#if defined (CLIENT_TRANSFER_TEST)
  ClientTransferTest();
  returncode.becomes(commErrNotOk_esp01);
#endif

  if ( pUserCommCtrl->pCSHeader != NULL )
    {
    MSGCD (( ERR_TASK_ALREADY_CONNECTED, pTaskCtrl->ulTaskIndex ));
    returncode.becomes(commErrNotOk_esp01);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT;
    return;
    }

  RTETask_CommonQueueElement* pQueueElement = task.ClientConnections().WaitForConnectRequest(RTETask_CommonQueueElement::Connect);

  // we need acces to legacy arguments of the queue element
  DLQ_CONNECT_REQ_ARGS_REC  &connReqArgs = pQueueElement->DataPart().ConnectRequest;

  //
  // --- get contents of the connect-queue-element
  //
  pUserCommCtrl->hCS                 = connReqArgs.hCS;
  pUserCommCtrl->pCSHeader           = connReqArgs.pCSHeader;
  pUserCommCtrl->hClientSem          = connReqArgs.hClientSem;
  pUserCommCtrl->hLocalClientProcess = connReqArgs.hLocalClientProcess;
  pUserCommCtrl->pidLocalClientPID   = connReqArgs.pidLocalClientPID;

  pQueueElement->Release();

  max_data_len = 0;
  returncode.becomes(commErrOk_esp01);

  // Update connection state
  pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

  //
  // --- check some of the communication segment values
  //
  pCSHeader      = pUserCommCtrl->pCSHeader;
  pCSOffsetList  = (PCOMM_SEG_OFFSET_REC)
                   ((PCHAR)pCSHeader + pCSHeader->ulOffsetRecord);

  returncode.becomes(sql53k_check_comm_seg( pCSHeader, pCSOffsetList ));

  if ( returncode == commErrOk_esp01 )
    {
    //
    // --- save same communication parameters
    //

    //7.6 does not support long applPID's. It is only provided in the CSHeader to support the encryption info!!!
    pUserCommCtrl->pidClientPID    = pCSHeader->pidClientOrPeerPID;

    if ( pCSHeader->iCSVersion >= COMM_SEG_VERSION_WITH_ENCRYPTION_INFO )
    {
        pUserCommCtrl->encryption    = pCSHeader->encryption;
    }
    else
    {
        pUserCommCtrl->encryption    = RTETask_ITaskClientConnection::UnknownEncryption;
    }

    pUserCommCtrl->ulClientRef     = pCSHeader->ulClientOrPeerRef;
    pUserCommCtrl->ulPacketCnt     = pCSHeader->ulPacketCnt;
    pUserCommCtrl->ulMaxDataLen    = pCSHeader->ulMaxDataLen;
    pUserCommCtrl->ulMaxCmdDataLen = pCSHeader->ulMaxDataLen -
                                     pCSHeader->ulMinReplySize;
    pUserCommCtrl->ulCommState     = commErrOk_esp01;
    pUserCommCtrl->ulCmdWaitTime   = (ULONG)UNDEF;
    strcpy ( pUserCommCtrl->szClientNode, pCSHeader->szClientOrPeerNode );

    for ( ulCnt = 0; ulCnt < pCSHeader->ulPacketCnt; ulCnt++ )
      {
      pUserCommCtrl->pCommPacketList[ulCnt] =
        (PCOMM_PACKET_REC)((PCHAR)pCSHeader + pCSOffsetList->ulOffset[ulCnt]);
      }

   if ( pCSHeader->iCSVersion == COMM_SEG_VERSION_WITH_SEMNAME_REPLY )
      {
      // set long term move lock...
      sql74k_long_term_move_lock( pTaskCtrl, CLIENT_NOT_TASK_MOVE_ENABLED, true );

      strcpy ( pCSHeader->szSemPath, SEM_UKT );
      strcpy ( pCSHeader->szSemName, kgs.pSemaNames[pTaskCtrl->pUKT->ulSemaNo] );
      }

    /* Set move lock, because the task should not move to another UKT
    until we have received a command from client */ 
    sql74k_long_term_move_lock( pTaskCtrl, WAITING_FOR_NEW_COMMAND, true );

    //
    // --- send communication parameter to client
    //
    pCSHeader->ulSemaNo          = pTaskCtrl->pUKT->ulSemaNo;
    pCSHeader->ulCommFlagNo      = pTaskCtrl->ulCommFlagNo;
    pCSHeader->ulServerCommState = commErrOk_esp01;
    pCSHeader->ulServerRef       = pTaskCtrl->ulTaskIndex;
    pCSHeader->ulServerFlag      = 1;
    pCSHeader->ulClientFlag      = 2;
    GETPROCESSID (&pCSHeader->pidServerPID);

    kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] = 0;
    kgs.ConnectedUserTasks += pTaskCtrl->TaskType == TT_US_EO00 ? 1 : 0;

    // ---  Wake up the client
    //
    rc = sql41c_post_event_sem ( pUserCommCtrl->hClientSem, "Client" );

    if (( rc != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED ))
      {
      // --- release client semaphore
      sql41c_close_event_sem ( pUserCommCtrl->hClientSem, "Client" );
      pUserCommCtrl->hClientSem = INVALID_HANDLE_VALUE;

      //
      // --- release connection
      //
      pUserCommCtrl->ulCommState = commErrCrash_esp01;
      returncode.becomes(commErrCrash_esp01);

      sql53k_release ( pTaskCtrl );
      }
    else if ( pUserCommCtrl->ulServiceType == srvPing_esp01 )
      {
      MSGD (( INFO_CONN_TASK_WITH_APP, pTaskCtrl->ulTaskIndex,
              pCSHeader->szClientOrPeerNode,
              pCSHeader->pidClientOrPeerPID ));

      sql53k_handle_ping ( pTaskCtrl, pUserCommCtrl );

      // --- release connection
      pUserCommCtrl->ulCommState = commErrNotOk_esp01;
      returncode.becomes(commErrNotOk_esp01);
      sql53k_release ( pTaskCtrl );
      }

    else
      {
        max_data_len = pCSHeader->ulMaxDataLen;

        MSGD (( INFO_CONN_TASK_WITH_APP, pTaskCtrl->ulTaskIndex,
                pCSHeader->szClientOrPeerNode,
                pCSHeader->pidClientOrPeerPID ));

        if ( pTaskCtrl->TaskType == TT_EV_EO00 )
        {
          if ( !eo67ConnectEventTask(pid) )
          {
            ABORT ();
          }
        }

#if defined (UDE_SERVER_TRANSFER_TEST)
    UDEServerTransferTest();
#endif
      }
    }
  else
    {
    //
    // --- release connection
    //
    pUserCommCtrl->ulCommState = commErrCrash_esp01;

    sql53k_release ( pTaskCtrl );
    }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;


  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }


/*------------------------------*/

VOID  vreceive ( PROCESS_ID                   pid,
                 tsp00_Int4                   timeout,
                 char                      ** rcv_packet_ptr,
                 tsp00_Int4 VAR_VALUE_REF     rcv_packet_length,
                 char                      ** reply_packet_ptr,
                 tsp00_Int4 VAR_VALUE_REF     reply_packet_size,
                 tsp01_CommErr VAR_VALUE_REF  returncode )
  {
  #undef  MF__
  #define MF__ MOD__"vreceive"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                pTaskCtrl     = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl     = &kgs.pFirstTaskCtrl[pid - 1];
  #endif
  PUSER_TASK_COMM_CTRL           pUserCommCtrl = pTaskCtrl->pUserCommCtrl;
  PCOMM_SEG_HEADER_REC           pCSHeader     = pUserCommCtrl->pCSHeader;
  PCHAR                          pcRcvPacket;
  ULONG                          ulRcvPacketLen;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pUserCommCtrl->ulCmdWaitTime = kgs.ulCurrTime; // - set wait time first !!!

  pTaskCtrl->TaskState = TSK_VRECEIVE_EO00;

  if ( pUserCommCtrl->ulConnected == 0 )
    {
    MSGCD (( ERR_COMM_WITHOUT_CONNECTION, pTaskCtrl->ulTaskIndex ));
    returncode.becomes(commErrNotOk_esp01);
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime = 0;
    DBGOUT;
    return;
    }

  returncode.becomes(commErrOk_esp01);
  *rcv_packet_ptr   = NULL;
  rcv_packet_length = 0;
  *reply_packet_ptr = NULL;
  reply_packet_size = 0;

  returncode.becomes(sql53k_receive (pTaskCtrl, pUserCommCtrl,
                                     pCSHeader, timeout,
                                     &pcRcvPacket, &ulRcvPacketLen));
  

  if  ( returncode != commErrOk_esp01 )
    {
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime = 0;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }


  *rcv_packet_ptr    = pcRcvPacket;
  rcv_packet_length  = (INT4) ulRcvPacketLen;
  pUserCommCtrl->ulLastRequestSize  = ulRcvPacketLen;
  *reply_packet_ptr  = (PCHAR)pUserCommCtrl->pReplyCommPacket->pDataPart;

  if ( pCSHeader->ulMinReplySize )
    reply_packet_size = (INT4) pUserCommCtrl->ulMaxDataLen - ALIGN ( ulRcvPacketLen, SQL_PACKET_ALIGNMENT);
  else
    reply_packet_size = (INT4) pUserCommCtrl->ulMaxDataLen;

  /*
    We have entered the dispatcher before, so 'pTaskCtrl->TimeCollectionEnabled' is setup if time measurtement is enabled.
    We later use 'vreceiveORvreplyLeaveTime != 0' as condition to allow collection of time in vreply.

    Time measurement values collect here at end of vreceive (pTaskCtrl->TaskStateStat):
        vreceiveORvreplyLeaveTime                Timestamp of last 'end' of vreply routine, updated with actualMicroSeconds
        runqueueEnqueueTime                     Time when task was moved from COM queue to RUN queue
        RplRcvStat.ulAbsMicroSeconds            Sum of all time intervals between vreply and following 'end' of vreceive
        RplRcvStat.ulRelMicroSeconds            Sum of all time intervals between vreply and following entering RUN queue
        RplRcvStat.ulCount                      Number of intervals added
  */
  eo52UpdateCountInStatisticRec(pTaskCtrl->TaskStateStat.RplRcvStat);
  if ( pTaskCtrl->TimeCollectionEnabled )
  {
      SAPDB_UInt8 actualMicroSeconds = RTESys_MicroSecTimer();

      if ( pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime )
      {
          eo52UpdateTimeInStatisticRec(pTaskCtrl->TaskStateStat.RplRcvStat
                , actualMicroSeconds - pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime
                , pTaskCtrl->TaskStateStat.runqueueEnqueueTime - pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime); 
      }

      pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime  = actualMicroSeconds;
  }
  else
  {
      /* Reset start timestamp, so vreply wont produce illegal values */
      pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime  = 0;
  }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

INT4  vcmd_wait_time ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"vcmd_wait_time"
  PTASK_CTRL_REC                 pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  PUSER_TASK_COMM_CTRL           pUserCommCtrl = pTaskCtrl->pUserCommCtrl;
  INT4                           i4CmdWaitTime;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  if ( pTaskCtrl->TaskState == TSK_VRECEIVE_EO00 )
    i4CmdWaitTime = (INT4)(kgs.ulCurrTime - pUserCommCtrl->ulCmdWaitTime);
  else
    i4CmdWaitTime = 0;


  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return ( i4CmdWaitTime ) ;
  }

/*------------------------------*/

VOID vreply ( PROCESS_ID                   pid,
              char                         *reply_packet_ptr,
              tsp00_Int4                   reply_packet_length,
              tsp01_CommErr VAR_VALUE_REF  returncode )
  {
  #undef  MF__
  #define MF__ MOD__"vreply"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                pTaskCtrl     = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl     = &kgs.pFirstTaskCtrl[pid - 1];
  #endif
  PUSER_TASK_COMM_CTRL           pUserCommCtrl = pTaskCtrl->pUserCommCtrl;

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pTaskCtrl->TaskState = TSK_VREPLY_EO00;

  if ( pUserCommCtrl->ulConnected == 0 )
    {
    MSGCD (( ERR_COMM_WITHOUT_CONNECTION, pTaskCtrl->ulTaskIndex ));
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    returncode.becomes(commErrNotOk_esp01);
    pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime = 0;
    DBGOUT;
    return;
    }

  returncode.becomes(sql53k_reply (pTaskCtrl, pUserCommCtrl,
                                   reply_packet_ptr,
                                   reply_packet_length ));

  if ( returncode != commErrOk_esp01 )
    {
    pTaskCtrl->TaskState = TSK_RUNNING_EO00;
    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
    }

  /*
    We have called vreceive before, so pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime would be set to beginning of command
    if time measurement was active then. So we use it as indication, that time measurement is possible

    Time measurement values collect here at end of vreply (pTaskCtrl->TaskStateStat):
        vreceiveORvreplyLeaveTime                Timestamp of last 'end' of vreceive routine, updated with actualMicroSeconds
        RcvRplStat.ulAbsMicroSeconds            Sum of all command times ( intervals between end of vreceive and end of vreply )
        RcvRplStat.ulCount                      Number of intervals summed
        ulRcvRplToLong                          Number of command, where time interval vreceive - vreply was longer than 1 second
   */
  eo52UpdateCountInStatisticRec(pTaskCtrl->TaskStateStat.RcvRplStat);
  if ( XPARAM(fTimeMeasure) )
  {
      SAPDB_UInt8 actualMicroSeconds = RTESys_MicroSecTimer();

      if ( pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime )
      {
          SAPDB_UInt8 delta_abs_time = 
              actualMicroSeconds - pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime;
          eo52UpdateTimeInStatisticRec(pTaskCtrl->TaskStateStat.RcvRplStat
              , delta_abs_time );
#define OS53_TOO_LONG_LIMIT_IN_MICROSECONDS (1000000U)
          if ( delta_abs_time > EO52_TOO_LONG_LIMIT_IN_MICROSECONDS )
          {
              pTaskCtrl->TaskStateStat.ulRcvRplToLong ++;
          }
      }
      pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime = actualMicroSeconds;
  }
  else
  {
      pTaskCtrl->TaskStateStat.vreceiveORvreplyLeaveTime = 0;
  }

  pTaskCtrl->TaskState = TSK_RUNNING_EO00;


  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID vrelease ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"vrelease"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  #endif


  DBGIN_T (pTaskCtrl->ulTaskIndex);

  pTaskCtrl->TaskState = TSK_VRELEASE_EO00;

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vrelease", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  if (( pTaskCtrl->TaskType != TT_US_EO00 ) 
   && ( pTaskCtrl->TaskType != TT_UT_EO00 )
   && ( pTaskCtrl->TaskType != TT_EV_EO00 )
     )
    {
    MSGD  (( ERR_TASK_TYPE_UNEXPECTED, "vrelease",
             pTaskCtrl->TaskType ));
    ABORT ();
    }

  if ( pTaskCtrl->TaskType == TT_EV_EO00 )
  {
      if ( !eo67ReleaseEventTask(pid) )
      {
          ABORT ();
      }
  }

  // --- release connection
  sql53k_release ( pTaskCtrl );

  MSGD (( INFO_CONN_RELEASED, pTaskCtrl->ulTaskIndex ));

  return;
  }

/*------------------------------*/

tsp00_Int4  vconnectedusertasks ()
{
    return kgs.ConnectedUserTasks;
}

/*------------------------------*/

VOID	vgetapplpid( PROCESS_ID                   tid ,
                     tsp00_NodeId  VAR_VALUE_REF  appl_node ,
                     tsp00_Int4    VAR_VALUE_REF  appl_pid ,
                     BOOLEAN       VAR_VALUE_REF  ok )
{
#undef  MF__
#define MF__ MOD__"vgetapplpid"

    if ( ( tid > 0 ) && ( ((ULONG)tid) <= kgs.ulNumOfTasks ) )
    {
        PTASK_CTRL_REC				pTaskCtrl = &kgs.pFirstTaskCtrl[tid - 1];

        if ( pTaskCtrl->pUserCommCtrl )
        {
	 	    appl_pid = pTaskCtrl->pUserCommCtrl->pidClientPID ;

            appl_node.c2p( pTaskCtrl->pUserCommCtrl->szClientNode[0] == '\0' 
                       ? (char *)kgs.szServerNode  
				       : (char *)pTaskCtrl->pUserCommCtrl->szClientNode );
            ok = true;
        }
	    else 
        {
		    appl_pid = -1;
            appl_node.c2p( (char *)kgs.szServerNode );
            ok = false;
        }
    }
    else
    {
#ifdef SDB_DEV_ONLY_CHECK
        MSGD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, tid ));
        ABORT();
#else
        appl_pid = -1;
        appl_node.SetBlank();
        ok = false;
#endif
    }
}
/*------------------------------*/

void sql53k_cancel_task ( PTASK_CTRL_REC  pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_cancel_task"
  PUSER_TASK_COMM_CTRL           pUserCommCtrl;

  DBGIN;

  //
  // --- this function should be used by foreign threads
  //     to cancel the actual command or kill the the connection
  //
  pUserCommCtrl = pTaskCtrl->pUserCommCtrl;

  if ( pUserCommCtrl->ulConnected != 0 ) // - Do a dirty read first!
  {
    WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

    if ( pUserCommCtrl->ulConnected != 0 )
    {
      pTaskCtrl->pTaskObject->Cancel();
      /* PTS 1105149 */
      if ( pTaskCtrl->TaskType == TT_EV_EO00 )
      {
        CLEARLOCK( &pTaskCtrl->lExclusive );
        vcontinue(pTaskCtrl->ulTaskIndex); /* Get it out of blocking call */
        WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
      }
    }

    CLEARLOCK( &pTaskCtrl->lExclusive );
  }

  DBGPAS;
  }

/*------------------------------*/

BOOL sql53k_comm_cancel ( PTASK_CTRL_REC  pTaskCtrl,
                          ULONG           ulCommState )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_comm_cancel"
  BOOL                           fCanceled = FALSE;
  PUSER_TASK_COMM_CTRL           pUserCommCtrl;

  DBGIN;

  //
  // --- this function should be used by foreign threads
  //     to cancel the actual command or kill the the connection
  //
  pUserCommCtrl = pTaskCtrl->pUserCommCtrl;

  if ( pUserCommCtrl->ulConnected != 0 ) // - Do a dirty read first!
  {
    if ( ulCommState == commErrOk_esp01 )
    {
      MSGD (( INFO_CANCEL_REQ_FOR_TSK, pTaskCtrl->ulTaskIndex ));
    }

    WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

    if ( pUserCommCtrl->ulConnected != 0 )
    {
      if ( ulCommState != commErrOk_esp01 )
      {
        // --- on error kill connection
        pUserCommCtrl->ulGlobCommState  = ulCommState;
        RTESys_WriteMemoryBarrier();
        kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] = 1;
      }

      pTaskCtrl->pTaskObject->Cancel();
      fCanceled  = TRUE;
      /* PTS 1105149 */
      if ( pTaskCtrl->TaskType == TT_EV_EO00 )
      {
        CLEARLOCK( &pTaskCtrl->lExclusive );
        vcontinue(pTaskCtrl->ulTaskIndex); /* Get it out of blocking call */
        WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
      }
    }

    CLEARLOCK( &pTaskCtrl->lExclusive );

    if ( fCanceled )
      sql72k_wake_ukt ( pTaskCtrl->pUKT );
  }

  DBGPAS;

  return ( fCanceled );
  }

static char as_printable[256] =
{
/* 0x00 - 0x1F non printable */
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
/* printable */
    ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', 

    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '.', 
/* 0x80 - 0xFF non printable */
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
    '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
};

/*------------------------------*/
void os53_DumpCommunicationSegment(PTASK_CTRL_REC pTaskCtrl)
{
    tsp00_Pathc CommunicationSegmentDumpPath;
    HANDLE dumpFd;

    sp77sprintf(
          (char *)CommunicationSegmentDumpPath
        , sizeof(tsp00_Pathc)
        , "%s\\" SAPDB_COMMUNICATION_DUMP_NAME
        , (char *)XPARAM(szRunDirectory) );

    dumpFd = CreateFile ( CommunicationSegmentDumpPath, 
                          GENERIC_READ|GENERIC_WRITE, 
                          FILE_SHARE_READ|FILE_SHARE_WRITE,
                          NULL,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL );

    if ( dumpFd != INVALID_HANDLE_VALUE 
      && SetFilePointer(dumpFd, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER )
    {
        PUSER_TASK_COMM_CTRL pUserCommCtrl = pTaskCtrl->pUserCommCtrl;
        struct comm_seg_header_record   *pCSHeader = pUserCommCtrl->pCSHeader;
        PCOMM_SEG_OFFSET_REC pCSOffsetList;
        DWORD bytesWritten = 0;
        SAPDB_Char buffer[4096];
        SAPDB_Byte *packet;
#define SAPDB_INT4S_PER_LINE 6
        SAPDB_UInt iOffset;
        SAPDB_UInt iInt4;
        SAPDB_UInt iChar;
        SAPDB_UInt lastShownOffset;
        char dumpLine[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];
        char dumpLineCopy[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];
        SAPDB_UInt iPacket;

        sp77sprintf(&buffer[0], sizeof(buffer), "-----struct user_task_comm_ctrl DUMP-----\n");
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulConnected         %ld\n", (long)pUserCommCtrl->ulConnected );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulCommState         %ld\n", (long)pUserCommCtrl->ulCommState );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulGlobCommState     %ld\n", (long)pUserCommCtrl->ulGlobCommState );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulCmdWaitTime       %ld\n", (long)pUserCommCtrl->ulCmdWaitTime );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   hLocalClientProcess %p\n",  pUserCommCtrl->hLocalClientProcess );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pidLocalClientPID   %ld\n", (long)pUserCommCtrl->pidLocalClientPID );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   hClientSem          %p\n",  pUserCommCtrl->hClientSem );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   hCS                 %p\n",  pUserCommCtrl->hCS );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pidClientPID        %ld\n", (long)pUserCommCtrl->pidClientPID );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szClientNode        %s\n", (char *)&pUserCommCtrl->szClientNode );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulClientRef         %ld\n", (long)pUserCommCtrl->ulClientRef );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulMaxDataLen        %ld\n", (long)pUserCommCtrl->ulMaxDataLen );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulMaxCmdDataLen     %ld\n", (long)pUserCommCtrl->ulMaxCmdDataLen );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulLastRequestSize   %ld\n", (long)pUserCommCtrl->ulLastRequestSize );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulPacketCnt         %ld\n", (long)pUserCommCtrl->ulPacketCnt );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);

        sp77sprintf(&buffer[0], sizeof(buffer), "   pCSHeader           %p\n",  pUserCommCtrl->pCSHeader );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pReplyCommPacket    %p\n",  pUserCommCtrl->pReplyCommPacket );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   hCS                 %p\n",  pUserCommCtrl->hCS );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);

        sp77sprintf(&buffer[0], sizeof(buffer), "-----struct comm_seg_header_record DUMP-----\n");

        sp77sprintf(&buffer[0], sizeof(buffer), "   ulCSSize            %ld\n", (long)pCSHeader->ulCSSize );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   iCSVersion          %d\n",  pCSHeader->iCSVersion );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulPacketCnt         %ld\n", (long)pCSHeader->ulPacketCnt );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulPacketSize        %ld\n", (long)pCSHeader->ulPacketSize );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulMaxDataLen        %ld\n", (long)pCSHeader->ulMaxDataLen );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulMinReplySize      %ld\n", (long)pCSHeader->ulMinReplySize );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulOffsetRecord      %ld\n", (long)pCSHeader->ulOffsetRecord );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulCurrentPacket     %ld\n", (long)pCSHeader->ulCurrentPacket );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pidClientOrPeerPID  %ld\n", (long)pCSHeader->pidClientOrPeerPID );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pidServerPID        %ld\n", (long)pCSHeader->pidServerPID );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulClientOrPeerRef   %ld\n", (long)pCSHeader->ulClientOrPeerRef );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulServerRef         %ld\n", (long)pCSHeader->ulServerRef );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulClientCommState   %ld\n", (long)pCSHeader->ulClientCommState );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulServerCommState   %ld\n", (long)pCSHeader->ulServerCommState );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulClientFlag        %ld\n", (long)pCSHeader->ulClientFlag );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulServerFlag        %ld\n", (long)pCSHeader->ulServerFlag );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulServiceType       %ld\n", (long)pCSHeader->ulServiceType );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulCommFlagNo        %ld\n", (long)pCSHeader->ulCommFlagNo );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulSemaNo            %ld\n", (long)pCSHeader->ulSemaNo );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szClientOrPeerNode  %s\n",  (char *)pCSHeader->szClientOrPeerNode );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   pidLocalPID         %ld\n", (long)pCSHeader->pidLocalPID );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szSemPath           %s\n",  (char *)&pCSHeader->szSemPath[0] );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szSemName           %s\n",  (char *)&pCSHeader->szSemName );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szClientOrPeerServerDB %s\n", (char *)&pCSHeader->szClientOrPeerServerDB[0] );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szServerNode        %s\n",  (char *)&pCSHeader->szServerNode[0] );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        sp77sprintf(&buffer[0], sizeof(buffer), "   szServerDB          %s\n",  (char *)&pCSHeader->szServerDB[0] );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        if ( pCSHeader->iCSVersion >= COMM_SEG_VERSION_WITH_LONG_APPL_PID )
        {
            sp77sprintf(&buffer[0], sizeof(buffer), "   applPID             %ld\n", (long)pCSHeader->applPID );
            WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        }
        if ( pCSHeader->iCSVersion >= COMM_SEG_VERSION_WITH_ENCRYPTION_INFO )
        {
            sp77sprintf(&buffer[0], sizeof(buffer), "   encryption          %d\n", (long)pCSHeader->encryption );
            WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        }

        sp77sprintf(&buffer[0], sizeof(buffer), "-----struct comm_seg_offset_record DUMP-----\n");
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        pCSOffsetList  = (PCOMM_SEG_OFFSET_REC)((char *)pCSHeader + pCSHeader->ulOffsetRecord);
        sp77sprintf(&buffer[0], sizeof(buffer), "   ulOffsetCnt         %ld\n", (long)pCSOffsetList->ulOffsetCnt );
        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
        for ( iPacket = 0; iPacket < pCSHeader->ulPacketCnt; ++iPacket )
        {
            sp77sprintf(&buffer[0], sizeof(buffer), "   ulOffset[%d]    %ld\n", iPacket, (long)pCSOffsetList->ulOffset[iPacket] );
            WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);

            sp77sprintf(&buffer[0], sizeof(buffer), "-----Packet[%d] content DUMP %s-----\n", iPacket, 
                pCSHeader->ulCurrentPacket == iPacket ? "(current packet)" : "");
            WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);

            sp77sprintf(&buffer[0], sizeof(buffer)
                        , "packet total size 0x%lx request offset 0x%lx size 0x%lx\n"
                        , (pCSHeader->ulMaxDataLen+(RTE_HEADER_SIZE * 2))
                        , (long)RTE_HEADER_SIZE
                        , pCSHeader->ulCurrentPacket == iPacket ? (long)pUserCommCtrl->ulLastRequestSize : (long)0 );
            WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);

            packet = (SAPDB_Byte *)pUserCommCtrl->pCommPacketList[iPacket];

            memset(dumpLineCopy, 0xFF, sizeof(dumpLineCopy));

            lastShownOffset = 0;
            for ( iOffset = 0; iOffset < (pCSHeader->ulMaxDataLen+(RTE_HEADER_SIZE * 2)) ; iOffset += (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)) )
            {
                for ( iInt4 = 0; iInt4 < SAPDB_INT4S_PER_LINE; iInt4++ )
                {
                    for ( iChar = 0; iChar < sizeof(SAPDB_Int4); iChar++ )
                    {
                        SAPDB_UInt iCharInLine = (sizeof(SAPDB_Int4)*iInt4) + iChar;
                        if ( (iOffset + iCharInLine) < (pCSHeader->ulMaxDataLen+(RTE_HEADER_SIZE * 2))  )
                        {
                            dumpLine[(iInt4 + (iCharInLine * 2)) + 0] = "01234567890ABCDF"[(packet[iOffset + iCharInLine]&0x0F0)>>4];
                            dumpLine[(iInt4 + (iCharInLine * 2)) + 1] = "01234567890ABCDF"[(packet[iOffset + iCharInLine]&0x0F)];
                            dumpLine[SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 2)) + iCharInLine] = as_printable[packet[iOffset + iCharInLine]];
                        }
                        else
                        {
                            dumpLine[(iInt4 + (iCharInLine * 2)) + 0] = ' ';
                            dumpLine[(iInt4 + (iCharInLine * 2)) + 1] = ' ';
                            dumpLine[SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 2)) + iCharInLine] = ' ';
                        }
                    }
                    dumpLine[iInt4 + ((iInt4 + 1) * (sizeof(SAPDB_Int4) * 2))] = ' ';
                }
                dumpLine[SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))] = 0;

                if ( memcmp(dumpLineCopy, dumpLine, sizeof(dumpLineCopy)) != 0 )
                {
                    if ( (lastShownOffset+(SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4))) < iOffset )
                    {
                        sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X - 0x%05X identical\n",
                            lastShownOffset + (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)), iOffset-1);
                        WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
                    }
                    memcpy(dumpLineCopy, dumpLine, sizeof(dumpLineCopy));
                    sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X %s\n", iOffset, &dumpLine[0] );
                    WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
                    lastShownOffset = iOffset;
                }
            }

            if ( (lastShownOffset+(SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4))) < iOffset )
            {
                sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X - 0x%05X identical\n",
                    lastShownOffset + (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)), iOffset-1);
                WriteFile(dumpFd, buffer, strlen(buffer), &bytesWritten, 0);
            }
        }

        CloseHandle(dumpFd);
    }
}

/*------------------------------*/

BOOL sql53k_comm_termination_cleanup ( PTASK_CTRL_REC  pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_comm_termination_cleanup"
  BOOL                           fRemoved      = FALSE;
  PUSER_TASK_COMM_CTRL           pUserCommCtrl = pTaskCtrl->pUserCommCtrl;
  PCOMM_SEG_HEADER_REC           pCSHeader;
  HANDLE                         hClientSem;
  ULONG                          ulCommState;

  DBGIN;

//  if ( pTaskCtrl->ulTaskIndex == kgs.taskResponsibleForOffline ) // PTS 1134089
//  {
//      return false;
//  }

  // --- this function should be used by foreign threads
  //     to terminate the the connection during DB-system crash or shutdown
  //
  if (( *kgs.pDBState  == SERVER_SHUTDOWN )       ||
      ( *kgs.pDBState  == SERVER_SHUTDOWNREINIT ) ||
      ( *kgs.pDBState  == SERVER_SHUTDOWNKILL ))
    ulCommState = commErrShutdown_esp01;
  else
    ulCommState = commErrCrash_esp01;

  if ( pUserCommCtrl )
  {
      pCSHeader  = pUserCommCtrl->pCSHeader;
      
      if ( pCSHeader && pUserCommCtrl->ulConnected != 0 ) // - Do a dirty read first!
      {
          WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );
          
          pCSHeader  = pUserCommCtrl->pCSHeader;

          if ( pCSHeader && pUserCommCtrl->ulConnected != 0 )
          {
              pCSHeader->ulServerCommState              = ulCommState;
              pCSHeader->ulServerRef                    = (ULONG)UNDEF;
              RTESys_WriteMemoryBarrier();
              pCSHeader->ulServerFlag                   = 1;
              
              pUserCommCtrl->ulGlobCommState            = ulCommState;
              pTaskCtrl->pTaskObject->Cancel();

              RTESys_WriteMemoryBarrier();
              kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] = 1;
              
              hClientSem                                = pUserCommCtrl->hClientSem;
              fRemoved                                  = TRUE;

              if ( ulCommState == commErrCrash_esp01
                && pTaskCtrl->TaskType != TT_EV_EO00
                && pTaskCtrl->TaskState != TSK_CONNECTWAIT_EO00 )
              {
                  os53_DumpCommunicationSegment(pTaskCtrl);
              }
          }
          
          CLEARLOCK( &pTaskCtrl->lExclusive );
          
          if ( fRemoved )
          {
              // --- wake up the client and UKT
              sql41c_post_event_sem ( hClientSem, "Client" );
              sql72k_wake_ukt ( pTaskCtrl->pUKT );
          }
      }
  }
  DBGPAS;

  return ( fRemoved );
  }

/*------------------------------*/

static VOID sql53k_handle_ping ( PTASK_CTRL_REC        pTaskCtrl,
                                 PUSER_TASK_COMM_CTRL  pUserCommCtrl )
{
#   undef  MF__
#   define MF__ MOD__"sql53k_handle_ping"
    ULONG     ulCommState = commErrOk_esp01;

    PCOMM_SEG_HEADER_REC  pCSHeader   = pUserCommCtrl->pCSHeader;
    PCHAR                 pcRcvPacket;
    ULONG                 ulRcvPacketLen;

    do
    {
        ulCommState = sql53k_receive (pTaskCtrl, pUserCommCtrl, pCSHeader, 240, &pcRcvPacket, &ulRcvPacketLen);

        if  ( ulCommState == commErrOk_esp01 )
        {
           if ( eo420ExtractPingPacket( (char*)pcRcvPacket, ulRcvPacketLen, 0, 0, 0 ) )
           {
               ULONG ulReplyLen;
               if ( pCSHeader->ulMinReplySize )
                   ulReplyLen = pUserCommCtrl->ulMaxDataLen - ALIGN ( ulRcvPacketLen, SQL_PACKET_ALIGNMENT);
               else
                   ulReplyLen = pUserCommCtrl->ulMaxDataLen;

                ulReplyLen =  eo420FillPingPacket ( (char*) pUserCommCtrl->pReplyCommPacket->pDataPart,
                                                    ulReplyLen,
                                                    1,   // hops greater than 0 means that the final destination was reached
                                                    kgs.szServerDB,
                                                    XPARAM(szKernelVersion), 0 );

                if ( ulReplyLen )
                    ulCommState = sql53k_reply ( pTaskCtrl, pUserCommCtrl, (PCHAR)pUserCommCtrl->pReplyCommPacket->pDataPart, ulReplyLen );
                else
                    ulCommState = commErrNotOk_esp01;
            }
            else
                ulCommState = commErrNotOk_esp01;
        }
        else
            ulCommState = commErrNotOk_esp01;
    } 
    while( ulCommState == commErrOk_esp01  );
}

/*------------------------------*/


VOID sql53k_remove_unrel_connnections ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_remove_unrel_connnections"
  PTASK_CTRL_REC                  pTaskCtrl;

  #if defined(_WIN32)
   for ( pTaskCtrl =  kgs.pFirstUserTaskCtrl;
         pTaskCtrl <= kgs.pLastUserTaskCtrl;
         pTaskCtrl++ )
     {
     sql53k_remove_unrel_conn( pTaskCtrl );
     }

         /* PTS 1103964 jrg */
   for ( pTaskCtrl =  kgs.pFirstEVTaskCtrl;
        /* It is possible to have no event tasks, so we have to check on 'NULL' */
         pTaskCtrl != NULL && pTaskCtrl <= kgs.pLastEVTaskCtrl;
         pTaskCtrl++ )
     {
     sql53k_remove_unrel_conn( pTaskCtrl );
     }

   sql53k_remove_unrel_conn( kgs.pUT );
  #endif

  DBGOUT;
  return;
  }


/*------------------------------*/

ULONG sql53k_get_connection_state ( PTASK_CTRL_REC  pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_get_connection_state"

  ULONG                          ulConnState = CON_UNUSED_EO003;
  PUSER_TASK_COMM_CTRL           pUserCommCtrl;
  PCOMM_SEG_HEADER_REC           pCSHeader;
  ULONG                          ulClientFlag;
  ULONG                          ulServerFlag;
  ULONG                          ulRetry = 3;

  pUserCommCtrl = pTaskCtrl->pUserCommCtrl;

  if ( pUserCommCtrl->ulConnected != 0 ) // - Do a dirty read first!
  {
    WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

    pCSHeader = pUserCommCtrl->pCSHeader;

    if ( pCSHeader && pUserCommCtrl->ulConnected != 0 )
    {
     /*
      *    The ulClientFlag and ulServerFlag:
      *              
      *                  c s
      *           init-+-    
      *                  0 0  
      *                   |        c s 
      *             sqlaconnect -+-
      *                       \    1 0
      *                        \      \
      *                         \    vconnect
      *                          \   /
      *                 c s       c s
      *                 -+-       -+-
      *                 0 2       2 1
      *                /   \      
      *      sqlareceive   sqlarequest,sqlarelease
      *              /       \   
      *           c s         c s
      *           -+-         -+-          0: no event
      *           2 1         1 2          1: event pending
      *              \       /             2: destination has received the event
      *   vreply,vrelease   vreceive
      *                \   /
      *                 c s
      *                 -+-
      *                 2 0
      *
      *
      *
      *    if the communication is crashing:
      *
      *      kernel:   c s         client:   c s
      *                -+-                   -+-
      *                ? 1                   1 ?
      */

      if ( pCSHeader->ulServerCommState == commErrOk_esp01 )
      {
        do {
          ulServerFlag = pCSHeader->ulServerFlag;
          ulClientFlag = pCSHeader->ulClientFlag;

          if (( ulServerFlag == 0 ) && ( ulClientFlag  == 0 ))
            ulConnState = CON_INITIALIZED_EO003;
          else if (( ulServerFlag == 2 ) && ( ulClientFlag  == 0 ))
            ulConnState = CON_CONNECTED_EO003;
          else if (( ulServerFlag == 2 ) && ( ulClientFlag  == 1 ))
            ulConnState = CON_REQUESTED_EO003;
          else if (( ulServerFlag == 0 ) && ( ulClientFlag  == 2 ))
            ulConnState = CON_RECEIVED_EO003;
          else if (( ulServerFlag == 1 ) && ( ulClientFlag  == 2 ))
            ulConnState = CON_REPLIED_EO003;
          else
            Sleep (0);

        } while ( ulConnState == CON_UNUSED_EO003 && --ulRetry );
      }
      else
        ulConnState = CON_CONNECTED_EO003;
    }
    CLEARLOCK( &pTaskCtrl->lExclusive );
  }

  DBGOUT;
	return ulConnState;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static tsp01_CommErr_Enum sql53k_check_comm_seg ( PCOMM_SEG_HEADER_REC pCSHeader,
                                                  PCOMM_SEG_OFFSET_REC pCSOffsetList )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_check_comm_seg"
  ULONG                          ulCnt;
  ULONG                          ulCommPacketEndOffset;

  //
  // --- check some of the communication segment header values
  //
  if ( pCSHeader->iCSVersion > COMM_SEG_VERSION )
    {
    MSGD  (( ERR_WRONG_COMMSEG_VERSION, pCSHeader->iCSVersion ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if (pCSHeader->ulCSSize < pCSHeader->ulOffsetRecord + COMM_OFFSET_LIST_SIZE)
    {
    MSGD  (( ERR_WRONG_OFFSET_REC_POS, pCSHeader->ulOffsetRecord,
             pCSHeader->ulOffsetRecord + COMM_OFFSET_LIST_SIZE));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if (pCSHeader->ulPacketCnt > MAX_SQL_PACKETS)
    {
    MSGD  (( ERR_WRONG_PACKET_COUNT,
             pCSHeader->ulPacketCnt, MAX_SQL_PACKETS ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if (pCSHeader->ulPacketCnt != pCSOffsetList->ulOffsetCnt)
    {
    MSGD  (( ERR_PACKET_COUNT_MISMATCH,
             pCSHeader->ulPacketCnt, pCSOffsetList->ulOffsetCnt));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if (( pCSHeader->ulPacketSize > XPARAM(ulPacketSize) ) ||
      ( pCSHeader->ulMaxDataLen > XPARAM(ulPacketSize) ))
    {
    MSGD  (( ERR_WRONG_PACKET_SIZE, pCSHeader->ulPacketSize));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( pCSHeader->ulMinReplySize > XPARAM(ulMinReplySize) )
    {
    MSGD  (( ERR_WRONG_MIN_REPLY_SIZE, pCSHeader->ulMinReplySize,
             XPARAM(ulMinReplySize) ));
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  for ( ulCnt = 0; ulCnt < pCSHeader->ulPacketCnt; ulCnt++ )
    {
    ulCommPacketEndOffset = pCSOffsetList->ulOffset[ulCnt] +
                            pCSHeader->ulMaxDataLen +
                            (RTE_HEADER_SIZE * 2) - 1;

    //
    // - check if the packets are not inside the communication segment
    //
    if ( ulCommPacketEndOffset + 1 > pCSHeader->ulCSSize )
      {
      MSGD  (( ERR_WRONG_COMM_PACKET_POS,
               pCSOffsetList->ulOffset[ulCnt],
               ulCommPacketEndOffset ));
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }

    //
    // - check if the packets are overlapping each other
    //
    if (( ulCnt + 1             <  pCSHeader->ulPacketCnt ) &&
        ( ulCommPacketEndOffset >= pCSOffsetList->ulOffset[ulCnt + 1] ))
      {
      MSGD  (( ERR_WRONG_COMM_PACKET_POS,
               pCSOffsetList->ulOffset[ulCnt],
               ulCommPacketEndOffset ));
      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql53k_receive ( PTASK_CTRL_REC        pTaskCtrl,
                                           PUSER_TASK_COMM_CTRL  pUserCommCtrl,
                                           PCOMM_SEG_HEADER_REC  pCSHeader,
                                           ULONG                 ulTimeOut,
                                           PCHAR                 *ppcRcvPacket,
                                           PULONG                pulRcvPacketLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_receive"
  PCOMM_PACKET_REC                     pCommPacket;
  PCOMM_PACKET_REC                     pReplyCommPacket;
  PRTE_HEADER_REC                      pCmdRTEHeader;
  ULONG                                ulCurrentPacket;
  PRTE_HEADER_REC                      pReplyRTEHeader;
  RTETask_Task                         &task = *pTaskCtrl->pTaskObject;

  DBGIN;

  pUserCommCtrl->ulCommState = commErrOk_esp01;

  // - Data replied?
  if ( pCSHeader->ulServerFlag == 0 )
    {
    MSGD (( ERR_VREC_WITHOUT_REPLY, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = commErrNotOk_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // - kill connection exception received?
  if ( pUserCommCtrl->ulGlobCommState != commErrOk_esp01 )
    {
    MSGD (( INFO_CONNECTION_KILLED, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = pUserCommCtrl->ulGlobCommState;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // -- calculate communication queue elements
  //
  if ( ulTimeOut > 0 )
    {
    if (( *kgs.pDBState != SERVER_WARM ) &&
        ( pTaskCtrl->TaskType == TT_UT_EO00 ))
    {
      task.ClientConnections().SetSessionTimeOut(MAXTIMEVALUE);
    }
    else if ( ulTimeOut <= MIN_COMMAND_TIMEOUT )
      task.ClientConnections().SetSessionTimeOut(kgs.ulCurrTime + MIN_COMMAND_TIMEOUT);
    else
      task.ClientConnections().SetSessionTimeOut(kgs.ulCurrTime + ulTimeOut);
    }
  else
    task.ClientConnections().SetSessionTimeOut(MAXTIMEVALUE);

  // Initialize communication queue element
  RTETask_CommonQueueElement& queueElement = task.CommonQueueElementFromFreeList();

  queueElement.Init( RTETask_CommonQueueElement::CommunicationReceiveWait );
      
  queueElement.CommunicationReceiveWaitPart().Init( task.ClientConnections().GetSessionTimeOut(),
                                                    (SAPDB_UInt4&)kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] );
  //
  // --- wait for a valid packet or connection release
  //

  // Update connection state
  pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

  task.EnqueueIntoCommunicationQueue( queueElement );
  queueElement = task.WaitForReschedule();

  // command queue has seen the comm flag!
  RTESys_ReadMemoryBarrier();

  while (( pCSHeader->ulClientFlag          != 1 )               &&
         ( pUserCommCtrl->ulGlobCommState   == commErrOk_esp01 ) &&
         ( pCSHeader->pidClientOrPeerPID    != (PID)UNDEF )      &&
         ( !queueElement.CommunicationReceiveWaitPart().SessionTimeoutOccurred() ))
    { 
    // Oops, the client flag has been set, but there is no packet or error outstanding.
    // Correct the semaphore number and try it again!
    pCSHeader->ulSemaNo = pTaskCtrl->pUKT->ulSemaNo;  
    RTESys_WriteMemoryBarrier();

    task.EnqueueIntoCommunicationQueue( queueElement );
    queueElement = task.WaitForReschedule();

    // command queue has seen the comm flag!
    RTESys_ReadMemoryBarrier();
    }

  // Update connection state
  pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

  // --- Connection killed?
  if ( pUserCommCtrl->ulGlobCommState != commErrOk_esp01 )
    {
    queueElement.Release(); // we don't need the queue element release it
    MSGD (( INFO_CONNECTION_KILLED, pTaskCtrl->ulTaskIndex));
    pUserCommCtrl->ulCommState = pUserCommCtrl->ulGlobCommState;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // - we must not look for command timeouts,
  //   before a shutdown event is handled.
  //
  if ( ( *kgs.pDBState == SERVER_SHUTDOWN ) &&
       ( pTaskCtrl->TaskType != TT_EV_EO00 ) )
    {
    queueElement.Release(); // we don't need the queue element release it
    pUserCommCtrl->ulCommState = commErrShutdown_esp01;
    DBGOUT;
    return ( commErrShutdown_esp01 );
    }

  if ( queueElement.CommunicationReceiveWaitPart().SessionTimeoutOccurred() )
    {
    queueElement.Release(); // we don't need the queue element release it
    MSGD (( INFO_COMM_TIMEOUT, pTaskCtrl->ulTaskIndex, ulTimeOut));
    pUserCommCtrl->ulCommState = commErrTimeout_esp01;
    DBGOUT;
    return ( commErrTimeout_esp01 );
    }

  queueElement.Release(); // we don't need the queue element release it

  if (( pCSHeader->pidClientOrPeerPID == (PID)UNDEF ) &&
      ( pCSHeader->ulClientFlag       != 1 ))
    {
    MSGD (( INFO_CLIENT_CONN_RELEASE, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = commErrNotOk_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // - after all local errors are handled the only posible way to get
  //   here is that the 'ulClientFlag' is equal 1!
  //

  // --- signal that we have received the data
  pCSHeader->ulClientFlag = 2;

  if ( pCSHeader->ulClientCommState != commErrOk_esp01 )
    {
    MSGD (( INFO_CONN_BROKEN, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = pCSHeader->ulClientCommState;
    DBGOUT;
    return ( (tsp01_CommErr_Enum)pCSHeader->ulClientCommState );
    }

  ulCurrentPacket = pCSHeader->ulCurrentPacket;

  // --- wrong packet requested?
  if ( ulCurrentPacket > pUserCommCtrl->ulPacketCnt )
    {
    MSGD (( ERR_VREC_WRONG_PACKET_REQ, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = commErrNotOk_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  pCommPacket       = pUserCommCtrl->pCommPacketList[ulCurrentPacket];
  pCmdRTEHeader     = &pCommPacket->RTEHeader;

  if ( pCSHeader->ulMinReplySize )
    pReplyCommPacket  = (PCOMM_PACKET_REC)((PCHAR)pCommPacket + ALIGN ( (ULONG)pCmdRTEHeader->ActSendLen, SQL_PACKET_ALIGNMENT));
  else
    pReplyCommPacket  = (PCOMM_PACKET_REC)(PCHAR)pCommPacket;

  // --- RTE-Header check
  if (( pCmdRTEHeader->ResidualPackets    != 0 )                          ||
      ( (ULONG)pCmdRTEHeader->SenderRef   != pUserCommCtrl->ulClientRef ) ||
      ( (ULONG)pCmdRTEHeader->ReceiverRef != pTaskCtrl->ulTaskIndex )     ||
      ( pCmdRTEHeader->ProtocolID         != RSQL_RTE_PROT_LOCAL_EO003 )  ||
      ( pCmdRTEHeader->MessClass          != RSQL_USER_DATA_REQUEST_EO003 )     ||
      ( pCmdRTEHeader->MaxSendLen         != pCmdRTEHeader->ActSendLen ))
    {
    MSGD (( ERR_PROTOCOL, "vreceive", 0 ));

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  *ppcRcvPacket    = pCommPacket->pDataPart;
  *pulRcvPacketLen = (ULONG)pCmdRTEHeader->ActSendLen - RTE_HEADER_SIZE;

  // --- wrong packet size?
  if (( pCmdRTEHeader->ActSendLen != pCmdRTEHeader->MaxSendLen ) ||
      ( *pulRcvPacketLen          >  pUserCommCtrl->ulMaxCmdDataLen ))
    {
    MSGD (( ERR_VREC_ILL_LENGTH, pCommPacket->RTEHeader.ActSendLen,
            pCommPacket->RTEHeader.MaxSendLen));
    pUserCommCtrl->ulCommState = commErrNotOk_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // --- set the reply packet position
  pUserCommCtrl->pReplyCommPacket = pReplyCommPacket;

  //
  // --- Init the RTE reply header. This is needed if the kernel
  //     releases the connection without a reply.
  //
  pReplyRTEHeader                  = &pReplyCommPacket->RTEHeader;
  pReplyRTEHeader->ActSendLen      = (INT4)RTE_HEADER_SIZE;
  pReplyRTEHeader->ProtocolID      = RSQL_RTE_PROT_LOCAL_EO003;
  pReplyRTEHeader->MessClass       = (INT1)UNDEF;
  pReplyRTEHeader->RTEFlags        = RSQL_NORMAL_EO003;
  pReplyRTEHeader->ResidualPackets = 0;
  pReplyRTEHeader->SenderRef       = UNDEF;
  pReplyRTEHeader->ReceiverRef     = pUserCommCtrl->ulClientRef;
  pReplyRTEHeader->RTEReturnCode   = commErrNotOk_esp01;
  pReplyRTEHeader->NewSwapType     = sql53k_new_swap_type();
  pReplyRTEHeader->Filler1         = 0;

  pReplyRTEHeader->MaxSendLen      = (INT4)RTE_HEADER_SIZE;

  // --- signal that we are ready for the 'reply'
  pCSHeader->ulServerFlag = 0;

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql53k_reply ( PTASK_CTRL_REC        pTaskCtrl,
                                         PUSER_TASK_COMM_CTRL  pUserCommCtrl,
                                         PCHAR                 pcRplPacket,
                                         ULONG                 ulRplPacketLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_reply"
  APIRET                               rc            = NO_ERROR;
  PRTE_HEADER_REC                      pReplyRTEHeader;
  PCOMM_SEG_HEADER_REC                 pCSHeader;

  pCSHeader       = pUserCommCtrl->pCSHeader;
  pReplyRTEHeader = &pUserCommCtrl->pReplyCommPacket->RTEHeader;

  // - Data replied?
  if ( pCSHeader->ulServerFlag != 0 )
    {
    MSGD (( ERR_VREP_WITHOUT_RECEIVE, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = commErrNotOk_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // - kill connection exception received?
  if ( pUserCommCtrl->ulGlobCommState != commErrOk_esp01 )
    {
    MSGD (( INFO_CONNECTION_KILLED, pTaskCtrl->ulTaskIndex ));
    pUserCommCtrl->ulCommState = pUserCommCtrl->ulGlobCommState;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // - is the packet address valid?
  if ( pcRplPacket != (PCHAR)pReplyRTEHeader + RTE_HEADER_SIZE )
    {
    MSGD (( ERR_WRONG_PACK_ADDR, pcRplPacket ));
    pUserCommCtrl->ulCommState = commErrCrash_esp01;
    ABORT();
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- Setup the RTE header.
  //
  pReplyRTEHeader->ActSendLen      = (INT4)ulRplPacketLen +
                                     RTE_HEADER_SIZE;
  pReplyRTEHeader->ProtocolID      = RSQL_RTE_PROT_LOCAL_EO003;
  pReplyRTEHeader->MessClass       = RSQL_USER_DATA_REPLY_EO003;
  pReplyRTEHeader->RTEFlags        = RSQL_NORMAL_EO003;
  pReplyRTEHeader->ResidualPackets = 0;
  pReplyRTEHeader->SenderRef       = pTaskCtrl->ulTaskIndex;
  pReplyRTEHeader->ReceiverRef     = pUserCommCtrl->ulClientRef;
  pReplyRTEHeader->RTEReturnCode   = commErrOk_esp01;
  pReplyRTEHeader->NewSwapType     = sql53k_new_swap_type();
  pReplyRTEHeader->Filler1         = 0;
  pReplyRTEHeader->MaxSendLen      = (INT4)ulRplPacketLen +
                                     RTE_HEADER_SIZE;

  pCSHeader->ulSemaNo              = pTaskCtrl->pUKT->ulSemaNo;            // we are waiting on this UKT semaphore
  sql74k_long_term_move_lock( pTaskCtrl, WAITING_FOR_NEW_COMMAND, true );  // so that, we should not move to other UKTs

  pCSHeader->ulServerCommState     = pUserCommCtrl->ulCommState;
  RTESys_WriteMemoryBarrier();
  pCSHeader->ulServerFlag          = 1;

  // --- wake up client
  rc = sql41c_post_event_sem ( pUserCommCtrl->hClientSem, "Client" );

  if (( rc != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED ))
    {
    pUserCommCtrl->ulCommState = commErrCrash_esp01;
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  pKernelCommunication->UpdateLastSendTime();

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

static VOID sql53k_release ( PTASK_CTRL_REC    pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_release"
  PUSER_TASK_COMM_CTRL           pUserCommCtrl;
  PCOMM_SEG_HEADER_REC           pCSHeader;
  APIRET                         rc = NO_ERROR;

//  if ( pTaskCtrl->ulTaskIndex == kgs.taskResponsibleForOffline )  // PTS 1134089
//  {
//      return;
//  }

  // Update connection state
  pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

  pUserCommCtrl = pTaskCtrl->pUserCommCtrl;
  pCSHeader     = pUserCommCtrl->pCSHeader;

  if ( pUserCommCtrl->pCSHeader != NULL ) // --- if still attached
    {
    pCSHeader->ulServerCommState = pUserCommCtrl->ulCommState;
    pCSHeader->ulServerRef       = (ULONG)UNDEF;
    RTESys_WriteMemoryBarrier();
    pCSHeader->ulServerFlag      = 1;
    }

  //
  // --- wake up the client and free it's semaphore
  //
  if ( pUserCommCtrl->hClientSem != INVALID_HANDLE_VALUE )
    {
    // --- wake up client
    sql41c_post_event_sem ( pUserCommCtrl->hClientSem, "Client" );

    // --- release client semaphore
    sql41c_close_event_sem ( pUserCommCtrl->hClientSem, "Client" );
    pUserCommCtrl->hClientSem = INVALID_HANDLE_VALUE;
    }


  WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

  //
  // --- free the client process handle
  //
  #if defined(_WIN32)
   if ( pUserCommCtrl->hLocalClientProcess != INVALID_HANDLE_VALUE )
     {
     if ( CloseHandle (pUserCommCtrl->hLocalClientProcess) != TRUE )
       {
       rc = GetLastError();
       MSGD (( ERR_CLOSING, "client process handle", rc ));
       DBG1 (( MF__, "INTERNAL ERROR: on closing %s, rc = %u",
               "client process handle", rc ));
       }

     pUserCommCtrl->hLocalClientProcess = INVALID_HANDLE_VALUE;
     }
  #endif


  if ( pUserCommCtrl->pCSHeader != NULL ) // --- if still attached
    {
    rc = sql41c_free_shrd_mem( pUserCommCtrl->pCSHeader,
                               pUserCommCtrl->hCS );

    pUserCommCtrl->pCSHeader = NULL;

    if ( rc != NO_ERROR )
      {
      CLEARLOCK( &pTaskCtrl->lExclusive );
      DBG1 ((MF__, "Could not shared free memory, rc = %u", rc ));
      ABORT();
      }
    }

  sql70k_init_user_comm_ctrl ( pUserCommCtrl );

  kgs.ConnectedUserTasks -= pTaskCtrl->TaskType == TT_US_EO00 ? 1 : 0;

  CLEARLOCK( &pTaskCtrl->lExclusive );

  // remove long term move lock
  sql74k_long_term_move_lock( pTaskCtrl, CLIENT_NOT_TASK_MOVE_ENABLED, false );
  sql74k_long_term_move_lock( pTaskCtrl, WAITING_FOR_NEW_COMMAND, false );


  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

static VOID sql53k_remove_unrel_conn ( PTASK_CTRL_REC  pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql53k_remove_unrel_conn"
  BOOL                            fWake = FALSE;
  PUSER_TASK_COMM_CTRL            pUserCommCtrl;
  PCOMM_SEG_HEADER_REC            pCSHeader;
  tsp00_Longuint                  ulConnected;
  #if defined(_WIN32)
   APIRET                         rc             = NO_ERROR;
  #endif

  //
  //  --- remove a unreleased connection
  //
  pUserCommCtrl  = pTaskCtrl->pUserCommCtrl;
  ulConnected    = pUserCommCtrl->ulConnected;   // - get logon time.

  if (( ulConnected          == 0 )               ||
      ( pTaskCtrl->TaskState == TSK_INACTIVE_EO00    ) ||
      ( pTaskCtrl->TaskState == TSK_TERMINATED_EO00  ) ||
      ( pTaskCtrl->TaskState == TSK_CONNECTWAIT_EO00 ) ||
      ( pTaskCtrl->TaskState == TSK_VRELEASE_EO00    ) // ||
//      ( pTaskCtrl->ulTaskIndex == kgs.taskResponsibleForOffline )  // PTS 1134089
     )
    {
    DBGOUT;
    return;
    }

  #if defined(_WIN32)
   if( WaitForSingleObject( pUserCommCtrl->hLocalClientProcess, 0) != WAIT_TIMEOUT )
     {
     WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

     if ( ulConnected == pUserCommCtrl->ulConnected )
       {
       pUserCommCtrl->ulGlobCommState            = commErrCrash_esp01;

       RTESys_WriteMemoryBarrier();
       kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] = 1;
       pTaskCtrl->pTaskObject->Cancel();
       fWake  = TRUE;
       }
     CLEARLOCK( &pTaskCtrl->lExclusive );
     }

   if ( fWake )
     {
     MSGD (( INFO_UNREL_CONN_FOUND, pTaskCtrl->ulTaskIndex ));
     sql72k_wake_ukt ( pTaskCtrl->pUKT );
     }
  #endif

  // --- wake task if the client has not done this correctly!
  WAIT_UNTIL_ALONE( &pTaskCtrl->lExclusive );

  pCSHeader = pUserCommCtrl->pCSHeader;

  if (( pUserCommCtrl->ulConnected     == ulConnected) &&
      ( pCSHeader                      != NULL  )      &&
      (( pCSHeader->pidClientOrPeerPID == (PID)UNDEF ) ||
       ( pCSHeader->ulClientCommState  != commErrOk_esp01 )))
    {
    kgs.pulCommFlags[pTaskCtrl->ulCommFlagNo] = 1;
    fWake                                     = TRUE;
    }

  CLEARLOCK( &pTaskCtrl->lExclusive );

  if ( fWake )
    sql72k_wake_ukt ( pTaskCtrl->pUKT );

  DBGOUT;
  return;
  }

/*------------------------------*/

static tsp00_Uint1 sql53k_new_swap_type ()
{ 
  static  tsp00_Uint1               SwapType = (tsp00_Uint1)UNDEF_SP00;

  if ( SwapType == (tsp00_Uint1)UNDEF_SP00 )
  {
    union { tsp00_Int8         int8;
            tsp00_C8           c8; }  SwapCheck;
    
    /* --- set int8 to 0x100000000 */
    SwapCheck.int8   = 0x01;
    SwapCheck.int8 <<= 32;
    
    for ( SwapType = 0;
          ( SwapType < 8 ) && ( SwapCheck.c8[ SwapType ] != 1 ); 
          SwapType++ ) { ; }
  }
  
  return SwapType;
}


/*
 * =============================== END ========================================
 */
