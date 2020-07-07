/*!
  @file           ven53.cpp
  @author         JoergM
  @brief          Kernel RunTime: Communication Functions for Kernel
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

#define MOD__ "ven53,cpp:"

#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "gen00.h"
#include "heo00.h"
#include "heo52.h"
#include "hen53.h"
#include "geo50_0.h"
#include "gen500.h"
#include "vsp001c.h"
#include "gen003.h"
#include "geo67.h"
#include "hen50.h"
#include "gen32.h"
#include "gen45.h"
#include "gen75.h"
#include "gen42.h"
#include "gen53.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"

/* imported functions */
#include "gen41.h"
#include "gen42.h"
#include "gen71.h"
#include "heo420.h"

/* local functions */
static void               HandlePing( tsp00_TaskId          pid );

/* imported data */
extern  tsp00_DbNamec                   en81_dbname; 
static void               ReleaseConnection ( struct TASK_TYPE        *tcb,
                                              tsp01_CommErr_Enum       commState, 
                                              int                      waitflag );

/* local data */
static RTECKC_KernelCommunication * pKernelCommunication = 0;


/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"vcmd_wait_time"
externC tsp00_Int4  vcmd_wait_time ( tsp00_TaskId       pid )
{
  struct TASK_TYPE *tcb = KGS -> pFirstTaskCtrl + ( pid - 1 ) ;

  time_t WaitTime = tcb->state == TSK_VRECEIVE_EO00 ? 
                     KGS->current_time - tcb->CmdWaitTime : 0 ;
  return ( (tsp00_Int4 )WaitTime ) ;
}

/*
 * =====================================================================
 */

/*
 * =====================================================================
 */
#undef MF__
#define MF__ MOD__"vconnect"
externC void    vconnect (  tsp00_TaskId                  pid ,
                            tsp00_Int4   VAR_VALUE_REF    max_data_len ,
                            tsp01_CommErr  VAR_VALUE_REF  returncode )
{
    ten50_UKT_Control  *this_ukt = THIS_UKT_CTRL;
    struct TASK_TYPE   *tcb   = this_ukt->curr_task;
    RTETask_Task       &task  = *tcb->pTaskObject;
    connection_info    *cip   = tcb->connection;;
    int                 rc;

    DBGIN_T( tcb->index );

    tcb->state = TSK_CONNECTWAIT_EO00 ;

    if ( !pKernelCommunication )
        pKernelCommunication = RTE_KGS::Instance().KernelCommunication();

    DBG1(( MF__,"state %d\n", cip->ci_state ));
 
    do
    {
        MSGD (( INFO_VCON_WAITING, tcb->index ));

        /* - alter task activ counters */
        en71AlterTaskCnt( this_ukt->curr_task, SUB_ACTIVE_COUNTER );

        RTETask_CommonQueueElement* pQueueElement = task.ClientConnections().WaitForConnectRequest(RTETask_CommonQueueElement::Connect);
        pQueueElement->Release();

        /* - alter task activ counters */
        en71AlterTaskCnt( this_ukt->curr_task, ADD_ACTIVE_COUNTER );

        rc = e75_connect_request ( tcb, cip );
        } 
    while ( rc != 0 );

    RTESys_ReadMemoryBarrier();

    if ( !cip->ci_alter_server_sem )
    {   /* set move lock... */
        en71LongTermMoveLock( tcb, CLIENT_NOT_TASK_MOVE_ENABLED, true );
    }

    /* Set move lock, because the task should not move to another UKT 
    until we have received a command from client */ 
    en71LongTermMoveLock( tcb, WAITING_FOR_NEW_COMMAND, true );

    // Update connection state
    pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

    max_data_len = cip->ci_max_data_size ;
    returncode.becomes( cip->ci_state == CON_ESTABLISHED ? commErrOk_esp01 : commErrNotOk_esp01 );

    if ( returncode == commErrOk_esp01 )
    {
        if ( cip->ci_service == srvPing_esp01 )
        {
            HandlePing ( pid );

            /* --- release connection */
            returncode.becomes(commErrNotOk_esp01);
            ReleaseConnection ( tcb, commErrOk_esp01, TRUE );
        }
        else
        {
            if ( task.Type() == RTETask_ITask::Event )
            {
                if ( !eo67ConnectEventTask(pid) )
                {
                    vabort(1);
                }
            }
        }
    }
    else
    {
        /* remove long term move lock */
        en71LongTermMoveLock( tcb, CLIENT_NOT_TASK_MOVE_ENABLED, false );
        en71LongTermMoveLock( tcb, WAITING_FOR_NEW_COMMAND, false );
    }

    DBG1 (( MF__,"leaving state %d datasize %d\n",
             cip->ci_state , max_data_len ));

    DBGOUT_T( tcb->index );
}


/*
 * =====================================================================
 */

#undef MF__
#define MF__ MOD__"vrelease"
externC void    vrelease ( tsp00_TaskId pid )
{
    struct TASK_TYPE   *tcb = KGS -> pFirstTaskCtrl + ( pid - 1 ) ;
    connection_info		*cip ;

    DBGIN_T(tcb->index);

    // Update connection state
    pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

    cip = tcb->connection;

    ReleaseConnection( tcb, (cip->ci_state == CON_REPLIED ? commErrOk_esp01 : commErrReleased_esp01), TRUE ); /* wait */

    DBGOUT_T( tcb->index );
}

/*
 * =====================================================================
 */

#undef MF__
#define MF__ MOD__"vreceive"
externC void    vreceive ( tsp00_TaskId                  pid ,
                            tsp00_Int4                   timeout ,
                            void                         **rcv_packet_ptr ,
                            tsp00_Int4 VAR_VALUE_REF     rcv_length,
                            void                         **reply_packet_ptr ,
                            tsp00_Int4 VAR_VALUE_REF     reply_size ,
                            tsp01_CommErr VAR_VALUE_REF  returncode )
{
   ten50_UKT_Control *                  this_ukt = THIS_UKT_CTRL;
    struct TASK_TYPE *                  tcb      = this_ukt->curr_task;
    int                                 idx;
    int                                 state;
    int                                 length;
    comseg_header    *                  comseg;
    rte_header       *                  header;
    connection_info  *                  cip;
    time_t                              commandTimeOut;

    DBG1 (( MF__,"T%d timeout %d \n", tcb->index , timeout ));
    tcb->CmdWaitTime = KGS->current_time;
    tcb->state = TSK_VRECEIVE_EO00 ;
    cip = tcb->connection;

    returncode.becomes( commErrNotOk_esp01 );
    *rcv_packet_ptr   = 0 ;
    rcv_length        = 0 ;
    *reply_packet_ptr = 0 ;
    reply_size        = 0 ;

    state = cip->ci_state ;
    if ( (state != CON_ESTABLISHED) && (state != CON_REPLIED) )
    {
        MSGD (( WRN_VRECV_IN_WRONG_STATE, state , tcb->index ));
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    cip->ci_state = CON_ESTABLISHED ; /* avoid returning state CON_REPLIED */
    if ( state == CON_ABORTED )
    {
        MSGD (( WRN_VRECV_APPLICATION_DEAD, tcb->index ));
        ReleaseConnection( tcb, commErrCrash_esp01 , FALSE ); /* no wait */
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    state = KGS -> state ;
    if ( (state != SERVER_WARM)         &&
         (state != SERVER_COLD)         &&
         (state != SERVER_STANDBY_EO00) &&
         (state != SERVER_STARTING)     &&
         (state != SERVER_SHUTDOWN || tcb->type != TT_EV_EO00) )
    {
        MSGD (( WRN_VRECV_BAD_SERVERSTATE, KGS -> state ));

        if ( state == SERVER_SHUTDOWN )
            returncode.becomes( commErrShutdown_esp01 );
        else
            returncode.becomes( commErrCrash_esp01 );

        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    if ( timeout > 0 )
        commandTimeOut = timeout +  KGS->current_time;
    else
    {   /* Timeout == 0 means sleep indefinitely. */
        commandTimeOut = MAXTIMEVALUE ;
    }

    e53_wait_for_communication( tcb, cip, commandTimeOut );

    RTESys_ReadMemoryBarrier();
    comseg = cip->ci_comseg ;

    if ( comseg )
    {
		comseg->cs_client_flag = 2 ;
		comseg->cs_server_flag = 0 ;
    }

    if ( ( KGS -> state == SERVER_SHUTDOWN ) && ( tcb->type != TT_EV_EO00 ) )
    {
        DBG1 (( MF__,"database shutdown, T%d", tcb->index ));
        ReleaseConnection( tcb, commErrShutdown_esp01 , TRUE ); /* wait */
        returncode.becomes( commErrShutdown_esp01 );
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }


    if ( cip->ci_state == CON_TIMEDOUT )
    {
        DBG1 (( MF__,"connection timeout, T%d" , tcb->index ));
        ReleaseConnection( tcb, commErrTimeout_esp01 , FALSE ); /* no wait */
        returncode.becomes( commErrTimeout_esp01 );
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    if ( cip->ci_state == CON_ABORTED )
    {
        MSGD (( WRN_VRECV_APPLICATION_DEAD, tcb->index ));
        ReleaseConnection( tcb, commErrCrash_esp01 , FALSE ); /* no wait */
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    if ( ! comseg )
    {
        MSGD (( INFO_CONN_BROKEN_NO_COMSEG, tcb->index )) ;
        ReleaseConnection( tcb, commErrCrash_esp01 , FALSE ); /* no wait */
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    returncode.becomes( (tsp01_CommErr_Enum)comseg->cs_client_state );
    DBG1 (( MF__,"connection state %d \n", returncode ));
    if ( returncode != commErrOk_esp01 )
    {
        MSGD (( INFO_CONN_BROKEN_BY_APPL_ST, (tsp01_CommErr_Enum)returncode, tcb->index ));
        ReleaseConnection( tcb, commErrCrash_esp01 , FALSE ); /* no wait */
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    idx = comseg->cs_current_packet ;
    if ( (idx < 0) || (idx > cip->ci_packet_cnt) )
    {
        MSGD (( IERR_VREC_WRONG_PACKET_REQ, idx , 
            tcb->index ));
        ReleaseConnection( tcb, commErrNotOk_esp01 , TRUE ); /* wait */
        tcb->state = TSK_RUNNING_EO00 ;
        returncode.becomes( commErrNotOk_esp01 );
        return ;
    }
    header = cip->ci_packet[idx];
    cip->ci_request = header ;

    length = header->rh_max_send_len - RTE_HEADER_SIZE ;
    if ( (length < 0) || (length > cip->ci_max_request_size) )
    {
        MSGD (( IERR_VREC_ILL_LENGTH ,
             length , cip->ci_max_request_size, tcb->index ));
        ReleaseConnection( tcb, commErrNotOk_esp01 , TRUE ); /* wait */
        tcb->state = TSK_RUNNING_EO00 ;
        returncode.becomes( commErrNotOk_esp01 );
        return ;
    }

    cip->BytesReceived += header->rh_max_send_len;
    cip->ci_state       = CON_RECEIVED ;
    *rcv_packet_ptr     = (void*) (header + 1);
    rcv_length          = length ;
    idx = ( (length & 7) != 0 ?  (8 - (length & 7)) : 0 );

    cip->ci_request_lgt = length + idx ;
    if (0 != cip->ci_min_reply_size)
    {
        cip->ci_reply  = (rte_header*) ((char*) (header + 1) + length + idx);
        cip->ci_reply_size  = cip->ci_max_data_size - length - idx ;
    }
    else
    {
        cip->ci_reply  = header;
        //ci_max_data_size = "packet size" - 2 * "RTEHeader size" - therefore we give away 1 * "RTEHeader size".
        //But this should be ok because these are only 24 bytes
        cip->ci_reply_size  = cip->ci_max_data_size;
    }
    
    *reply_packet_ptr   = (void*) (cip->ci_reply + 1);
    reply_size          = cip->ci_reply_size ;
    DBG1 (( MF__,"req 0x%08lx rpl 0x%08lx lgt %d \n",
        (long) cip->ci_request , (long) cip->ci_reply , length ));

#ifdef  DEBUG_RTE
    sql46_dump_packet ( header );
#endif
#ifdef FROMUSER
    DBG1 (( MF__,"dumping packet to DUMPFILE \n" ));
    sql46_dump ( "\nFROMUSER------>", 16 );
    sql46_dump ( (char*) (header + 1) , length );
    sql46_dump ( "<---------------", 18 - (length + 13)%16 );
    sql46_dump ( "--FROMUSER\n", 11 );
#endif

    DBG1 (( MF__,"length   %d \n", length ));
    DBG1 (( MF__,"return   %d \n", returncode ));
    DBG1 (( MF__,"leaving, T%d \n", tcb->index ));

    eo52UpdateCountInStatisticRec(tcb->TaskStateStat.RplRcvStat);
    /*
      We have entered the dispatcher before, so 'tcb->TimeCollectionEnabled' is setup if time measurtement is enabled.
      We later use 'vreceiveORvreplyLeaveTime != 0' as condition to allow collection of time in vreply.

      Time measurement values collect here at end of vreceive (tcb->TaskStateStat):
          vreceiveORvreplyLeaveTime                Timestamp of last 'end' of vreply routine, updated with actualMicroSeconds
          runqueueEnqueueTime                     Time when task was moved from COM queue to RUN queue
          RplRcvStat.ulAbsMicroSeconds            Sum of all time intervals between vreply and following 'end' of vreceive
          RplRcvStat.ulRelMicroSeconds            Sum of all time intervals between vreply and following entering RUN queue
          RplRcvStat.ulCount                      Number of intervals added
    */
    if ( tcb->TimeCollectionEnabled )
    {
        SAPDB_UInt8 actualMicroSeconds = RTESys_MicroSecTimer();
        
        if ( tcb->TaskStateStat.vreceiveORvreplyLeaveTime )
        {
            eo52UpdateTimeInStatisticRec(tcb->TaskStateStat.RplRcvStat
                , actualMicroSeconds - tcb->TaskStateStat.vreceiveORvreplyLeaveTime
                , tcb->TaskStateStat.runqueueEnqueueTime - tcb->TaskStateStat.vreceiveORvreplyLeaveTime); 
        }
        
        tcb->TaskStateStat.vreceiveORvreplyLeaveTime  = actualMicroSeconds;
    }
    else
    {
        /* Reset start timestamp, so vreply wont produce illegal values */
        tcb->TaskStateStat.vreceiveORvreplyLeaveTime  = 0;
    }



    tcb->state = TSK_RUNNING_EO00 ;
}

/*
 * =====================================================================
 */

#undef MF__
#define MF__ MOD__"vreply"
externC void    vreply ( tsp00_TaskId                 pid ,
                         void                         *reply_packet_ptr,
                         tsp00_Int4                   reply_length ,
                         tsp01_CommErr VAR_VALUE_REF  returncode )
{
    struct TASK_TYPE   *tcb = KGS -> pFirstTaskCtrl + ( pid - 1 ) ;
    int             state ;
    rte_header          *header ;
    comseg_header       *comseg ;
    connection_info     *cip ;



    DBG1 (( MF__,"entered T%d pktptr 0x%08lx lgt %d \n",
        tcb->index , (long) reply_packet_ptr , reply_length ));

    tcb->state = TSK_VREPLY_EO00 ;
    returncode.becomes( commErrNotOk_esp01 );

    cip = tcb->connection;
    state = cip->ci_state ;
    cip->ci_state = CON_ESTABLISHED ; /* avoid returning state CON_REPLIED */

    if ( reply_packet_ptr != (void*) (cip->ci_reply + 1) )
    {
        MSGD (( IERR_VREPLY_WRONG_PACK_ADDR ,
         (long) reply_packet_ptr , cip->ci_reply, tcb->index ));
        vabort(1);
    }
    if (    (reply_length < 0) || (reply_length > cip->ci_reply_size) )
    {
        MSGD (( WRN_REPLY_PACK_SIZE,
         cip->ci_packet_size , cip->ci_max_data_size , cip->ci_request_lgt ));
        MSGD (( IERR_VREPLY_ILL_PACK_LEN,
         reply_length , cip->ci_reply_size , tcb->index ));
        vabort(1);
    }

    if ( state == CON_ABORTED )
    {
        DBG1 (( MF__,"application dead for T%d \n", tcb->index ));
        ReleaseConnection( tcb, commErrCrash_esp01 , FALSE ); /* no wait */
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }
    if ( state != CON_RECEIVED )
    {
        MSGD (( WRN_VREPLY_IN_WRONG_STATE, cip->ci_state , 
            tcb->index ));
        tcb->state = TSK_RUNNING_EO00 ;
        return ;
    }

    comseg = cip->ci_comseg ;
    if ( comseg )
        sql32_lock_comseg ( cip , "vreply: 0x%08lx \n" );

    if (    ! comseg
         || (comseg->cs_client_pid   != cip->ci_peer_pid)
         || (comseg->cs_client_ref   != cip->ci_peer_ref)
         || (comseg->cs_client_semid != cip->ci_peer_semid)
         || (comseg->cs_client_state != commErrOk_esp01)
       )
    {
        MSGD (( INFO_CONN_BROKEN_BAD_COMSEG, tcb->index,
            (long) comseg ));
        MSGD (( INFO_CONN_BROKEN_CI_INFO,
            (long) cip->ci_peer_pid , cip->ci_peer_ref , cip->ci_peer_semid ));
        if ( comseg )
          MSGD (( INFO_CONN_BROKEN_CS_INFO,
            (long) comseg->cs_client_pid , comseg->cs_client_ref ,
            comseg->cs_client_semid , comseg->cs_client_state ));
        sql32_unlock_comseg ( cip );
        tcb->state = TSK_RUNNING_EO00 ;
        DBGOUT_T( tcb->index );
        return ;
    }

    /*
     *  Setup the rte header.
     */
    header = cip->ci_reply ;
    header->rh_act_send_len     = reply_length + RTE_HEADER_SIZE ;
    header->rh_max_send_len     = reply_length + RTE_HEADER_SIZE ;
    header->rh_protocol_id      = RSQL_RTE_PROT_LOCAL_EO003 ;
    header->rh_mess_class       = RSQL_USER_DATA_REPLY_EO003 ;
    header->rh_rte_flags        = RSQL_NORMAL_EO003 ;
    header->rh_residual_packets = 0 ;
    header->rh_sender_ref       = comseg->cs_server_ref ;
    header->rh_receiver_ref     = comseg->cs_client_ref ;
    header->rh_rte_return_code  = 0 ;
    header->rh_new_swap_type    = sql42_new_swap_type();
    header->rh_filler1          = 0 ;
    DBG1 (( MF__,"req 0x%08lx rpl 0x%08lx lgt %d \n",
        (long) cip->ci_request , (long) cip->ci_reply ,
        reply_length + RTE_HEADER_SIZE ));

#ifdef  DEBUG_RTE
    sql46_dump_packet ( header );
#endif
#ifdef  TRACE_RETCOD
    DBG1 (( MF__,"sp1r_returncode %d \n",
       ((tsp1_packet*)(header+1))->integer.C_2.sp1_segm.integer.C_4.sp1r_returncode ));
#endif
#ifdef TOUSER
    DBG1 (( MF__,"dumping packet to DUMPFILE \n" ));
    sql46_dump ( "\nTOUSER-------->", 16 );
    sql46_dump ( (char*) (header + 1) , (int) reply_length );
    sql46_dump ( "<---------------", 18 - ((int)reply_length + 13)%16 );
    sql46_dump ( "----TOUSER\n", 11 );
#endif

    comseg->cs_server_semid = tcb->ukt->semid;  /* we are waiting on this UKT sem */
    en71LongTermMoveLock( tcb, WAITING_FOR_NEW_COMMAND, true );

    comseg->cs_server_flag = 1 ;

    DBG1 (( MF__,"comseg 0x%08lx cli,srv %d,%d \n", (long) comseg ,
        comseg->cs_client_flag , comseg->cs_server_flag ));

    sql32_unlock_comseg ( cip );

    cip->BytesSend += header->rh_max_send_len;

    returncode.becomes( (tsp01_CommErr_Enum)e75_wake_peer ( cip ));

    pKernelCommunication->UpdateLastSendTime();

    eo52UpdateCountInStatisticRec(tcb->TaskStateStat.RcvRplStat);

    if ( XPARAM(fTimeMeasure) )
    {
        SAPDB_UInt8 actualMicroSeconds = RTESys_MicroSecTimer();

        if ( tcb->TaskStateStat.vreceiveORvreplyLeaveTime )
        {
            SAPDB_UInt8 delta_abs_time = 
               ( actualMicroSeconds - tcb->TaskStateStat.vreceiveORvreplyLeaveTime );
            eo52UpdateTimeInStatisticRec(tcb->TaskStateStat.RcvRplStat, delta_abs_time);
            if ( delta_abs_time > EO52_TOO_LONG_LIMIT_IN_MICROSECONDS )
            {
                tcb->TaskStateStat.ulRcvRplToLong ++;
            }
        }
        tcb->TaskStateStat.vreceiveORvreplyLeaveTime = actualMicroSeconds;
    }
    else
    {
        /* Reset start timestamp, so vreply wont produce illegal values */
        tcb->TaskStateStat.vreceiveORvreplyLeaveTime  = 0;
    }

    cip->ci_state = CON_REPLIED ;

    tcb->state = TSK_RUNNING_EO00 ;
    DBG1 (( MF__,"leaving %d, T%d \n", returncode , tcb->index ));
}

/*
 * ==================================================================
 */
void    vgetapplpid( PROCESS_ID                    tid,
                     tsp00_NodeId  VAR_VALUE_REF   appl_node ,
                     tsp00_Int4 VAR_VALUE_REF      appl_pid ,
                     BOOLEAN   VAR_VALUE_REF       ok )
{
#undef  MF__
#define MF__ MOD__"vgetapplpid"

    if ( ( tid > 0 ) && ( ((SAPDB_Int4)tid) <= KGS->ulNumOfTasks ) )
    {
        struct TASK_TYPE *tcb = KGS->pFirstTaskCtrl + ( tid - 1 ) ;

        if ( 0 != tcb->connection )
        {
            char peer_pid_buffer[32];

            appl_pid = atoi ( sql45_peer_pid ( tcb, peer_pid_buffer ) ) ;
            appl_node.c2p(sql45_has_remote_pid ( tcb ) ? sql45_peer_node ( tcb ) : (char *)(KGS->servernode));
            ok = true;
        }
        else
        {
            appl_pid = -1;
            appl_node.c2p((char *)(KGS->servernode));
            ok = false;
        }
    }
    else
    {
#ifdef SDB_DEV_ONLY_CHECK
        MSGD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, tid ));
        vabort(1);
#else
        appl_pid = -1;
        appl_node.SetBlank();
        ok = false;
#endif
    }
}

/*
 * =====================================================================
 */

externC  tsp00_Int4  vconnectedusertasks ()
{
    return KGS->connectedUserTasks;
}


/*
 * =====================================================================
 */

externC void e53_wait_for_communication( struct TASK_TYPE*       tcb,
                                         connection_info*        cip,
                                         SAPDB_ULong             commandTimeOut )
{
    RTETask_Task*     pTask = tcb->pTaskObject;


    // Update connection state
    pKernelCommunication->ConnectionIsInactiveOrInCommandWaitState();

    if (( KGS->state != SERVER_WARM ) && ( tcb->type == TT_UT_EO00 ))
      pTask->ClientConnections().SetSessionTimeOut(MAXTIMEVALUE);
    else
      pTask->ClientConnections().SetSessionTimeOut(commandTimeOut);

    // Initialize communication queue element
    RTETask_CommonQueueElement& queueElement = pTask->CommonQueueElementFromFreeList();

    queueElement.Init( RTETask_CommonQueueElement::CommunicationReceiveWait );

    queueElement.CommunicationReceiveWaitPart().Init( pTask->ClientConnections().GetSessionTimeOut(),
                                                      cip->ci_state,
                                                      CON_ABORTED,
                                                      cip->ci_comseg->cs_client_flag );

    /*
     *  insert task into communication queue, where it waits for a packet
     */
    pTask->EnqueueIntoCommunicationQueue( queueElement );

    queueElement = pTask->WaitForReschedule();

    if ( true == queueElement.CommunicationReceiveWaitPart().SessionTimeoutOccurred() )
        cip->ci_state = CON_TIMEDOUT;

    queueElement.Release(); // we don't need the queue element release it

    // Update connection state
    pKernelCommunication->ConnectionIsActiveAndNotInCommandWaitState();

    return;
}

/*
* =====================================================================
*/

static void HandlePing( tsp00_TaskId          pid )
{
   tsp01_CommErr         commState;
   void*                 rcvPacket;
   tsp00_Int4            rcvPacketLen;
   void*			      replyPacket;
   tsp00_Int4		      replyPacketLen;

   commState.becomes( commErrOk_esp01 );
   do
   {
       vreceive ( pid, 240, &rcvPacket, rcvPacketLen, &replyPacket, replyPacketLen, commState );

       if  ( commState == commErrOk_esp01 )
       {
          if ( eo420ExtractPingPacket( (char*)rcvPacket, rcvPacketLen, 0, 0, 0 ) )
          {
               replyPacketLen =  eo420FillPingPacket ( (char*) replyPacket,
                                                        replyPacketLen,
                                                        1,   /* hops greater than 0 means that the final destination was reached */
                                                        en81_dbname,
                                                        XPARAM(szKernelVersion), 0 );
 
               if ( replyPacketLen )
                   vreply( pid, replyPacket, replyPacketLen, commState );
               else
                   commState.becomes( commErrNotOk_esp01 );
           }
           else
               commState.becomes( commErrNotOk_esp01 );
       }
       else
           commState.becomes( commErrNotOk_esp01 );
   } 
   while( commState == commErrOk_esp01  );
}
/*   
* =====================================================================
*/
static void ReleaseConnection ( struct TASK_TYPE        *tcb,
                                tsp01_CommErr_Enum       commState, 
                                int                      waitflag )
{
    e75_release_connection ( commState, waitflag );

    /* remove long term move lock */
    en71LongTermMoveLock( tcb, CLIENT_NOT_TASK_MOVE_ENABLED, false );
    en71LongTermMoveLock( tcb, WAITING_FOR_NEW_COMMAND, false );
}   
