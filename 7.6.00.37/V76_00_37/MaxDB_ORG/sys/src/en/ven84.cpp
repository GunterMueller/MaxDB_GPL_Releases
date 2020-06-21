/*!
  @file           ven84.cpp
  @author         JoergM
  @brief          Kernel Runtime: Requestor Thread
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

#define AFL_BASIC                             0x1
#define AFL_SCRAMMD5                          0x2


#undef MOD__
#define MOD__ "ven84.c:"

/* exported functions */

#include "gen00.h"
#include "heo00.h"
#include "geo002.h"
#include "gsp01.h"
#include "geo50_0.h"
#include "gen500.h"
#include "vsp001c.h"
#include "gen003.h"
#include "gen81.h"
#include "gen84.h"
#include "hen50.h"
#include "heo56.h"

/* imported functions */

#include "hen40.h" /* sqlerrs */
#include "gen32.h"
#include "gen38.h"
#include "gen41.h"
#include "gen42.h"
#include "gen71.h"
#include "gen72.h"
#include "gen73.h"
#include "hsp77.h"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"


/* local functions */

static  void            e84_request             ( rte_header*                 header,
                                                  char*                       authenticationAllow );
static  int             e84_find_free_task      ( connection_info*            cip, 
                                                  struct TASK_TYPE**          pTcb,
                                                 RTETask_CommonQueueElement* &pQueueElement);

static  int             e84_open_reply_channel  ( connection_info*            cip,
                                                  VF_FILENAMEC                file );
static  void            e84_write_reply_channel ( connection_info*            cip,
                                                  rte_header*                 header );
static  void            e84_close_reply_channel ( connection_info*            cip );
static  void            e84_reject              ( connection_info*            cip,
                                                  SQL_DBNAMEC                 mydbname,
                                                  rte_header*                 header,
                                                  int                         reason );
static  void            e84_accept              ( connection_info*            cip,
                                                  SQL_DBNAMEC                 mydbname,
                                                  SAPDB_Bool                  omitReplyPart,
                                                  char*                       authenticationAllow,
                                                  rte_header*                 header,
                                                  struct TASK_TYPE*           tcb );
static  void            e84_conn_send           ( connection_info*            cip,
                                                  struct TASK_TYPE*           tcb,
                                                  RTETask_CommonQueueElement* &pQueueElement );
static  int             e84_reply_class         ( connection_info*            cip );
static  void            en84_CatchAlarm         ( int                         sig,
                                                  siginfo_t*                  pInfo,
                                                  void *                      arg );
static  int             en84_RecvConpkt         ( int                         sd,
                                                  rte_header*                 header,
                                                  int*                        statep,
                                                  char**                      errm,
                                                  int                         msecTimeoutTime );
static  void            en84_PreventClientStarvation(int                      peer_semid);
static  SAPDB_Bool      e84_dump_allowed          ();


/* exported data */

/* local data */

volatile int         en84_NeverStop = 1;
static  int          en84_Timeout = 0;
static  tsp00_Bool   en84_DumpRequest = false;


/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"en84_requestor"
/*ARGSUSED*/
void *en84_requestor ( void *arg )
{
#undef  MF__
#define MF__    "e84_requestor"
ten50_KT_Control         *this_kt;
    int                 rc ;
    int                 fifo_fd ;
    char                *errmsg ;
    rte_header          *header ;
    char                packet [ RTE_CONPKT_SIZE ];
    VF_FILENAMEC        file ;
    SAPDB_UInt4         authenticationFlags;
    char                authenticationAllow[256];

    DBGIN;

    MSGD (( INFO_PROCESS_STARTED, "REQUESTOR" ));

    this_kt = & KGS-> requestor ;
    this_kt->start_time = KGS->current_time;

    SAPDBErr_MessageList        errList;
    RTEConf_Parameter::String   valueString;

    if((RTEConf_ParameterAccess::Instance())->GetString( KERNELPARAM_AUTHENTICATION_ALLOW, valueString, errList))
    {
        if ( 0 == strcmp("BASIC", (char*)valueString ))
            authenticationFlags = AFL_BASIC;
        else if ( 0 == strcmp("SCRAMMD5", (char*)valueString ))
            authenticationFlags = AFL_SCRAMMD5;
        else
            authenticationFlags = AFL_SCRAMMD5 | AFL_BASIC;
    }
    else
        authenticationFlags = AFL_SCRAMMD5 | AFL_BASIC;

    if((RTEConf_ParameterAccess::Instance())->GetString( KERNELPARAM_AUTHENTICATION_DENY, valueString, errList))
    {
        if ( 0 == strcmp("BASIC", (char*)valueString ))
            authenticationFlags &= ~AFL_BASIC;
        else if ( 0 == strcmp("SCRAMMD5", (char*)valueString ))
            authenticationFlags != ~AFL_SCRAMMD5;
    }

    authenticationAllow[0] = '\0';

    if ( (authenticationFlags & ( AFL_SCRAMMD5 | AFL_BASIC )) == ( AFL_SCRAMMD5 | AFL_BASIC ) )
        strcat( authenticationAllow, "BASIC,SCRAMMD5" );
    else if ( authenticationFlags & AFL_BASIC )
        strcat( authenticationAllow, "BASIC" );
    else if ( authenticationFlags & AFL_SCRAMMD5 )
        strcat( authenticationAllow, "SCRAMMD5" );


    DBG1 (( MF__,"REQUESTOR: thread %ld \n", (long)this_kt->tid ));

    sql41_get_request_fifo_name ( file , KGS-> serverdb );
    /*
     *  The fifo might exist from a previous run.
     */
    (void) unlink ( (const char *)file );
    rc = sql41_create_fifo ( file , 0622 );
    if ( rc )
    {
        MSGD (( ERR_REQUESTOR_CR_FIFO )) ;
        return NULL;
    }
    /*
     *  O_RDWR avoids blocking and does not deliver EOF when
     *  last client closes.
     */
    fifo_fd = open ( (const char *)file , O_RDWR , 0 );
    if ( fifo_fd == -1 )
    {
        MSGD (( ERR_REQUESTOR_OPEN_REQ_FIFO, sqlerrs() ));
        return NULL;
    }

    header = (rte_header*) packet ;
    while ( en84_NeverStop )
    {
        /*
         *  Read a connect packet from the fifo.
         *  Read error are checked after copy of debuglevel
         *  and check of server state.
         */
        DBG1 (( MF__,"recv_conpkt fd %d \n", fifo_fd ));

        en84_Timeout = 0;
        errmsg = (char*)"";

        rc = en84_RecvConpkt ( fifo_fd ,
                               header , 
                              &en84_Timeout , 
                              &errmsg, 
                               LOCL_CON_TMO * 1000 );

        if ( (KGS-> state == SERVER_STOP) || (KGS-> state == SERVER_KILL) )
        {
            MSGCD (( INFO_PROCESS_STOPPED, "REQUESTOR" ));
            break;
        }

        /*
         *  Now, check return code of the receive packet operation.
         *  Try to recover from read errors by re-opening the fifo.
         */
        if ( rc != SP1CE_OK )
        {
            if ( en84_Timeout != CON_TIMEDOUT )
            {
                const char *errMsg;
                MSGD (( WRN_REQUESTOR_REOPEN_FIFO, rc , errmsg, file.asCharp() ));
                (void) close ( fifo_fd );

                /* PTS 1109407 */
                errMsg = en41CheckFifo(file, 0622);
                if ( errMsg )
                {
                    MSGCD (( ERR_REQUESTOR_OPEN_REQ_FIFO, errMsg ));
                    vabort(0);
                }

                fifo_fd = open ( (const char *)file , O_RDWR , 0 );
                if ( fifo_fd == -1 )
                {
                    MSGCD (( ERR_REQUESTOR_OPEN_REQ_FIFO, sqlerrs() ));
                    vabort(0);
                }
            }
            continue ;
        }

#ifdef DEBUG_DEBUG
        { 
            extern void e74_print_all_conns ( char * );
            e74_print_all_conns ( "REQUESTOR" );
        }
#endif /*DEBUG_DEBUG*/

        /*
         *  Check and process the request.
         */
        e84_request ( header, authenticationAllow );
    }

    en81_NotifyCoordinator(0);
    while ( en84_NeverStop )
    {
        sqlyieldthread();
    }
    /*NOTREACHED*/
    return NULL;
}
/* e84_requestor */

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_request"
static  void    e84_request ( rte_header      *header,
                              char*            authenticationAllow )
{
    RTETask_CommonQueueElement*    pQueueElement = 0;
    int                            rc;
    int                            retcod;
    tsp00_Int4                     pktcnt4;
    tsp00_Int4                     semid4;
    tsp00_Int4                     shmid4;
    long                           lpid;
    connection_info                *cip;
    connection_info                connection_buffer;
    SQL_DBNAMEC                    mydbname;
    VF_FILENAMEC                   file;
    char                           logbuf [ 256 ];
    struct TASK_TYPE               *tcb = 0;
    SAPDB_Bool                     omitReplyPart = false;


    /*
     *  Extract the various information from the connect packet
     *  and store them in the connection info buffer.
     */
    cip = &connection_buffer ;
    SAPDB_memset ( cip , 0 , sizeof(*cip) );
    sql42_extract_conpkt ( header , &cip->ci_mess_class ,
                &cip->ci_my_ref , &cip->ci_peer_ref ,
                &retcod , &cip->ci_service ,
                &cip->ci_max_segment_size ,
                &cip->ci_max_data_size , &cip->ci_packet_size ,
                &cip->ci_min_reply_size , &cip->ci_peer_swap_type ,
                mydbname , cip->ci_peer_dbname );
    sql42_get_string ( header , RSQL_OPT_NODE_EO003 , sizeof(cip->ci_peer_node) ,
                            cip->ci_peer_node );
    sql42_get_string ( header , RSQL_OPT_REM_PID_EO003 , sizeof(cip->ci_remote_pid) ,
                            cip->ci_remote_pid );
    sql42_get_long ( header , RSQL_OPT_OFFSET_EO003 , &cip->ci_big_offset );
    sql42_get_int1 ( header , RSQL_OPT_ALTER_SERV_SEM_EO003 , &cip->ci_alter_server_sem );
    sql42_get_int1 (header, RSQL_OPT_OMIT_REPLY_PART_EO003, (tsp00_Uint1 *)&omitReplyPart);
    sql42_get_int1 (header, RSQL_OPT_ENCRYPTION_EO003, &cip->encryption);

    /*
     *  Option values are unchanged, if not present in packet.
     */
    pktcnt4 = 1 ;
    lpid = 0 ;
    semid4 = 0 ;
    shmid4 = 0 ;
    file[0] = 0 ;
    sql42_get_int4 ( header , RSQL_OPT_PKTCNT_EO003 , &pktcnt4 );
    sql42_get_long ( header , RSQL_OPT_PID_EO003    , &lpid );
    sql42_get_int4 ( header , RSQL_OPT_SEMID_EO003  , &semid4 );
    sql42_get_int4 ( header , RSQL_OPT_SHMID_EO003  , &shmid4 );
    sql42_get_string ( header , RSQL_OPT_FIFO_EO003 , sizeof(file) , file );
    cip->ci_packet_cnt = pktcnt4 ;
    cip->ci_peer_pid = (pid_t) lpid ;
    cip->ci_peer_semid = semid4 ;
    cip->ci_shmid = shmid4 ;
    DBG1 (( MF__,"REQUESTOR: mess_class %d service %d pktcnt %d \n",
        cip->ci_mess_class , cip->ci_service , cip->ci_packet_cnt ));
    DBG1 (( MF__,"REQUESTOR: peerpid %ld peerref %d semid %d shmid %d \n",
        (long) cip->ci_peer_pid , cip->ci_peer_ref ,
        cip->ci_peer_semid , cip->ci_shmid ));
    DBG1 (( MF__,"REQUESTOR: node '%s' db '%s' rempid '%s' \n",
        cip->ci_peer_node , cip->ci_peer_dbname , cip->ci_remote_pid ));
    DBG1 (( MF__,"REQUESTOR: fifo '%s' \n", file ));

    rc = e84_open_reply_channel ( cip , file );

    if ( rc != SP1CE_OK )
    {
        return ;
    }

    if ( (KGS-> state != SERVER_COLD)         &&
         (KGS-> state != SERVER_STANDBY_EO00) &&
         (KGS-> state != SERVER_WARM)    )
    {
        MSGD (( INFO_REQUESTOR_REJECT, KGS-> state ));
        switch ( KGS-> state )
        {
        case SERVER_STARTING :
            e84_reject ( cip, mydbname, header , SP1CE_START_REQUIRED );
            break ;
        default :
            e84_reject ( cip , mydbname, header , SP1CE_SHUTDOWN );
            break ;
        }
        e84_write_reply_channel ( cip , header );
        e84_close_reply_channel ( cip );
        return ;
    }

    /*
     *  When handling a CONNECT REQUEST or an INFO REQUEST
     *  we insert the current time into 'tcb->connection->ci_connect_time'
     *  to give 'e84_test_application' a chance to clear tasks
     *  which have (tcb->connection->ci_connect_time != 0) and
     *  (tcb->state == TSK_CONNECTWAIT) for too long a time.
     */
    switch ( cip->ci_mess_class )
    {
    case RSQL_INFO_REQUEST_EO003 :
    case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003:
        /*
         *  Reply to the client.
         */
        e84_accept ( cip, mydbname, omitReplyPart, authenticationAllow, header, tcb );
        break ;

    case RSQL_USER_CONN_REQUEST_EO003 :
    case RSQL_KERN_CONN_REQUEST_EO003 :

        rc = e84_find_free_task ( cip, &tcb, pQueueElement );

        if ( rc != SP1CE_OK )
        {
            MSGD (( WRN_REQUESTOR_REJ_REQ )) 
            e84_reject ( cip , mydbname, header , rc );
            break ;
        }

        tcb->pTaskObject->ResetPendingCancel(); /* reset cancel flag */

        /*
         *  Log the connect request.
         */
        (void) sp77sprintf ( logbuf , sizeof(logbuf), "Connecting T%d" , tcb->index );
        rc = strlen ( logbuf );
        if ( cip->ci_peer_node[0] )
            (void) sp77sprintf ( logbuf + rc , sizeof(logbuf)-rc, " %s" , cip->ci_peer_node.asCharp() );
        else
            (void) sp77sprintf ( logbuf + rc , sizeof(logbuf)-rc, " local" );
        rc = strlen ( logbuf );
        if ( cip->ci_peer_dbname[0] )
        {
            (void) sp77sprintf ( logbuf + rc , sizeof(logbuf)-rc, ":%s" , cip->ci_peer_dbname.asCharp() );
            rc = strlen ( logbuf );
        }
        if ( cip->ci_remote_pid[0] )
            (void) sp77sprintf ( logbuf + rc , sizeof(logbuf)-rc, " %s" , cip->ci_remote_pid.asCharp() );
        else
            (void) sp77sprintf ( logbuf + rc ,  sizeof(logbuf)-rc," %ld" , (long) cip->ci_peer_pid );


        /*  start task if inactive! */
        if (( tcb->type == TT_US_EO00 ||
              tcb->type == TT_EV_EO00 ||
              tcb->type == TT_UT_EO00 ))
        {
            SAPDBErr_MessageList messageList;

            if ( false == tcb->pTaskObject->Startup( messageList ))
            {
                RTE_Message (messageList, MSG_DIAG_CONSOLE);
                MSGD (( WRN_REQUESTOR_REJ_REQ ));
                e84_reject ( cip , mydbname, header , rc );
                break ;
            }
        }

        MSGD (( INFO_CONNECTING_TASK, &logbuf[0] ));
        /*
         *  Reply to the client.
         */
        e84_accept ( cip, mydbname, omitReplyPart, authenticationAllow, header, tcb );

       /*
        *  Inform the UKP of the connect request.
        */
        e84_conn_send ( cip, tcb, pQueueElement );


        break ;

    case RSQL_USER_CANCEL_REQUEST_EO003 :
        MSGD (( INFO_REQUESTOR_CANCEL_REQ, cip->ci_my_ref ));
        if (    (cip->ci_my_ref < 1)
             || (cip->ci_my_ref > KGS-> ulNumOfTasks) )
        {
            MSGD (( IERR_REQUESTOR_CANCEL_ILL_T, cip->ci_my_ref ));
            break ;
        }
        tcb = &KGS-> pFirstTaskCtrl[cip->ci_my_ref-1] ;
        tcb->pTaskObject->Cancel();
        /* PTS 1105149 */
        if ( tcb->type == TT_EV_EO00 )
        {
          if ( tcb->connection )
          {
            tcb->connection->ci_state = CON_ABORTED ;
            e72_wake ( tcb->ukt ); /* wakeup if blocked in dispatcher */
          }
          tcb->pTaskObject->Cancel();
          vcontinue(tcb->index); /* wakeup if block in vstop() */
        }
        else
        {
          /* If client was not able to attach to shared memory, it must abort connection */
          /* Its only way to do that, is by sending a cancel request. This leads to ABORT of connection */
          /* If the client was already connected, the behaviour of a cancel is as before. Only commands */
          /* are canceled, but the connection is kept alive */
          if ( tcb->connection 
            && (CON_CONNECTING == tcb->connection->ci_state
             || TSK_CONNECTWAIT_EO00 == tcb->state
             || TSK_INACTIVE_EO00     == tcb->state
             || (TSK_VRECEIVE_EO00     == tcb->state
              && 0 == tcb->connection->BytesReceived) ) )
          {
             MSGD (( WRN_REQUESTOR_CNCL_ABORT_0, cip->ci_my_ref ));
             MSGD (( WRN_REQUESTOR_CNCL_ABORT_1 ));
             tcb->connection->ci_state = CON_ABORTED ;
             e72_wake ( tcb->ukt ); /* wakeup if blocked in dispatcher */
          }
        }
        break ;

    case RSQL_DUMP_REQUEST_EO003 :

        if ( e84_dump_allowed() )
        {
            MSGCD (( INFO_REQUESTOR_DUMP_REQ ));

            if ( en84_DumpRequest )
            {
                MSGCD (( ERR_REQUESTOR_NO_MORE_DUMP )) 
            }
            else
            {
                en84_DumpRequest = true ;
                KGS-> mode = MODE_DUMP ;
                (void) en81_kill_database ();
            }
        }
        break ;

    default :
        MSGD (( WRN_REQUESTOR_ILL_MSG_TYPE , cip->ci_mess_class ));
        e84_reject ( cip , mydbname, header , SP1CE_NOTOK );
        break;
    }

    e84_write_reply_channel ( cip , header );
    e84_close_reply_channel ( cip );
}
/* e84_request */

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_find_free_task"
static  int  e84_find_free_task ( connection_info *            cip,
                                  struct TASK_TYPE **          pTcb,
                                  RTETask_CommonQueueElement* &pQueueElement)
{
    int                                  idx ;
    ten50_UKT_Control*                   Ukt ;
    ten50_UKT_Control*                   OptimalUkt ;
    struct TASK_TYPE*                    tcb ;
    connection_info*                     tsk_cip ;
    tsp00_Int4                           TaskType;
    static RTECKC_KernelCommunication *  pKernelCommunication = 0;

    DBGIN;

    if ( !pKernelCommunication )
        pKernelCommunication = RTE_KGS::Instance().KernelCommunication();

    if ( cip->ci_packet_cnt != 1 )
    {
        MSGD (( WRN_REQUESTOR_PACK_CNT_NE_1  ));
        return ( SP1CE_NOTOK );
    }

    /*
     *  Is the CONNECT REQUEST for the Utility Task ?
     */
    switch ( cip->ci_service )
    {
    case srvDistribution_esp01: /* no longer supported */
        return ( SP1CE_NOTOK );
    case srvUtility_esp01:
        DBG1 (( MF__,"e84_find: trying to connect utility \n" ));
        /*
         *  Test whether a possibly existing utility connection is still alive.
         */
        e84_test_application ( KGS-> ut );

        if  (( KGS-> ut->state == TSK_CONNECTWAIT_EO00 ) ||
             ( KGS-> ut->state == TSK_INACTIVE_EO00 ))
        {
            DBG1 (( MF__,"utility is waiting for connect \n" ));

            if ( ! KGS-> ut->connection->ci_connect_time )
            {
                DBG1 (( MF__,"connecting UTILITY \n" ));
                tcb = KGS->ut;

                WAIT_UNTIL_ALONE ( tcb->ukt->exclusive );
                tcb->connection->ci_connect_time = KGS->current_time;
                CLEARLOCK( tcb->ukt->exclusive );

                *pTcb         = tcb;
                pQueueElement = RTE_KGS::Instance().GlobalFreeList()->Dequeue();

                DBG1 (( MF__,"returning SP1CE_OK \n" ));
                return ( SP1CE_OK );
            }
        }
        MSGD (( WRN_REQUESTOR_UT_IS_BUSY,  KGS->ut->state, (long)KGS->ut->connection->ci_connect_time ));

        *pTcb = NULL;
        return ( SP1CE_TASKLIMIT );

    case srvEvent_esp01:
        if ( XPARAM(ulMaxEventTasks) == 0 )
        {
            *pTcb = NULL;
            return ( SP1CE_TASKLIMIT );
        }
        break;
    default:
        break;
    }

    pQueueElement = pKernelCommunication->UnconnectedTaskPool().GetTask( 
            cip->ci_service == srvEvent_esp01 ? RTETask_ITask::Event : RTETask_ITask::User );

    /*
     * Dump State of all connection in all UKTS for debugging purpose
     */
    if ( ! pQueueElement )
    {
        MSGD (( WRN_REQUESTOR_ALL_UKPS_BUSY ));

        for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
        { 
            if ( tcb->connection )
            {
                MSGD (( INFO_REQUESTOR_FIND_ALL_TSK, 
                    THREAD_INDEX(tcb->ukt), tcb->index ,
                    (long) tcb->connection->ci_connect_time ,
                    e38_tskstate_name(tcb->state) ));
            }
        }

        *pTcb = NULL;
        return ( SP1CE_TASKLIMIT );
    }

    *pTcb = tcb = pQueueElement->Task()->LegacyTaskCtrl();

    WAIT_UNTIL_ALONE ( tcb->ukt->exclusive );
    tcb->connection->ci_connect_time = KGS->current_time;
    tsk_cip = tcb->connection ;
    tsk_cip->ci_service = cip->ci_service ;
    CLEARLOCK ( tcb->ukt->exclusive );

    DBG1 (( MF__,"returning SP1CE_OK \n" ));
    return ( SP1CE_OK );
}

/*
 * ======================================================================
 */

#undef  MF__
#define MF__    MOD__"e84_open_reply_channel"
static  int e84_open_reply_channel ( connection_info     *cip,
                                     VF_FILENAMEC         file )
{
    int             rc ;


    /*
     *  Open the reply channel.
     */
    cip->ci_pipe_fd = -1 ;
    if ( file[0] )
    {
        /*
         *  O_RDWR avoids blocking.
         *  The data are written into the fifo, wether the client has
         *  the fifo open or not. If it opens later it gets the data then.
         */
        cip->ci_pipe_fd = open ( (const char *)file , O_RDWR , 0 );
        if ( cip->ci_pipe_fd != -1 )
        {
            DBG1 (( MF__,"opened '%s' fd %d \n", file , cip->ci_pipe_fd ));
            return ( SP1CE_OK );
        }
        MSGD (( ERR_REQUESTOR_OPEN_REPL_FIFO, file.asCharp() , sqlerrs() ));
    }
    else if ( (cip->ci_shmid == 0) || (cip->ci_peer_semid == 0) )
    {
        /*
         *  Cancel and Dump don't need a reply.
         */
        return ( SP1CE_OK );
    }

    /*
     *  If there is a semid available,
     *  try to save the client from hanging on its semaphore.
     */
    if ( cip->ci_peer_semid )
    {
        rc = semop ( cip->ci_peer_semid , &semsnd , 1 );
        if ( rc == -1 )
            MSGD (( ERR_REQUESTOR_REL_SEMID, 
                        cip->ci_peer_semid , sqlerrs() ));
        DBG1 (( MF__,"semop semid %d rc %d \n", cip->ci_peer_semid , rc ));

        DBG1 (( MF__,"removing semid %d \n", cip->ci_peer_semid ));
        (void) sql41_remove_sem ( &cip->ci_peer_semid ,
                        (char*)"us" , KGS-> serverdb );
    }

    return ( SP1CE_NOTOK );
}

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_write_reply_channel"
static  void    e84_write_reply_channel ( connection_info*   cip,
                                          rte_header*        header )
{
#if defined(SAPDB_DEBUG_BAD_REPLY_PACKET) /* normally not defined !!! */
    static int neverDumped = 1;
    if ( neverDumped )
    {
        char first_packet_dump[1024];
        neverDumped = 0;
        sp77sprintf(first_packet_dump, sizeof(first_packet_dump), "Reply packet dump (%d bytes): %*.*r",
        header->rh_max_send_len, header->rh_max_send_len, header->rh_max_send_len, header);
#define INFO_REQUESTOR_FIRST_REPLY_DUMP          11273,INFO_TYPE,"COMMUNIC","First reply packet dump: %s"
        MSGD (( INFO_REQUESTOR_FIRST_REPLY_DUMP, &first_packet_dump[0] ));

        char outputLine[1024];
        int fd = open("/SAP_DB/data/wrk/remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
        if ( fd < 0 )
        {
            fd = open("remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
        }
        if ( fd >= 0 )
        {
            time_t now = KGS->current_time;
            ctime_r(&now, outputLine);
            sp77sprintf(outputLine + strlen(outputLine), sizeof(outputLine) - strlen(outputLine),
                "KERN: PID %8ld Connect Reply Packet dump (%d bytes): %*.*r\n",
                getpid(), header->rh_act_send_len, header->rh_act_send_len, header->rh_act_send_len, header );
            write(fd, outputLine, strlen(outputLine));
            close(fd);
        }
    }
#endif
    /*
     *  Cancel and Dump don't need a reply.
     */
    if ( -1 != cip->ci_pipe_fd )
    {
        tsp00_ErrTextc pErrText;

        pErrText[0] = 0;
        if ( SP1CE_OK != sql42_send_conpkt ( cip->ci_pipe_fd , header , pErrText ) )
        {
            char packet_dump[1024];
#define ERR_REQUESTOR_FAILED_TO_REPLY     11273,ERR_TYPE,"COMMUNIC","Cant send connect reply to client pipe: %s"
            MSGD (( ERR_REQUESTOR_FAILED_TO_REPLY, &pErrText[0] ));
            sp77sprintf(packet_dump, sizeof(packet_dump), "Reply packet dump (%d bytes): %*.*r",
                    header->rh_max_send_len, header->rh_max_send_len, header->rh_max_send_len, header);
#define ERR_REQUESTOR_REPLY_DUMP          11273,ERR_TYPE,"COMMUNIC","Reply packet dump: %s"
            MSGD (( ERR_REQUESTOR_REPLY_DUMP, &packet_dump[0] ));
        }
    }
}

/*
 * ======================================================================
 */

#undef  MF__
#define MF__    MOD__"e84_close_reply_channel"
static  void    e84_close_reply_channel ( connection_info     *cip )
{
    /*
     *  Cancel and Dump don't need a reply.
     */
    if ( -1 != cip->ci_pipe_fd )
    {
        /*
         *  Close the reply channel.
         */
        /*
         *  O_RDWR avoids blocking.
         *  The data are written into the fifo, wether the client has
         *  the fifo open or not. If it opens later it gets the data then.
         */
        (void) close ( cip->ci_pipe_fd );
    }
}

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_reject"
static  void    e84_reject (connection_info     *cip,
                            SQL_DBNAMEC          mydbname,
                            rte_header          *header,
                            int                  reason)
{
    int             messclass ;


    DBG1 (( MF__,"entered, reason %d \n", reason ));

    messclass = e84_reply_class ( cip );
    sql42_create_conpkt ( header , messclass ,
                (tsp00_Int4)0 , cip->ci_peer_ref , reason ,
                cip->ci_service , cip->ci_max_segment_size ,
                cip->ci_max_data_size , cip->ci_packet_size ,
                cip->ci_min_reply_size ,
                mydbname, cip->ci_peer_dbname );

    DBGOUT;
}

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_accept"
static  void    e84_accept (connection_info     *cip ,
                            SQL_DBNAMEC          mydbname,
                            SAPDB_Bool           omitReplyPart,
                            char*                authenticationAllow,
                            rte_header          *header,
                            struct TASK_TYPE    *tcb )
{
    int             messclass ;
    long            maxdatasize ;
    connection_info     *tsk_cip ;

    DBGIN;

    if ( cip->ci_comseg && ! cip->ci_big_offset )
        cip->ci_protocol = PROT_SHM_EO003 ;
    else
        cip->ci_protocol = PROT_BIGSHM_EO003 ;

    if (    (cip->ci_packet_size <= 0)
     || (cip->ci_packet_size > XPARAM(ulPacketSize)) )
    cip->ci_packet_size = XPARAM(ulPacketSize) ;

    maxdatasize = XPARAM(ulPacketSize) - sizeof(comseg_header)
            - 8 /*alignment*/ - 2 * sizeof(rte_header);
    if (    (cip->ci_max_data_size <= 0)
     || (cip->ci_max_data_size > maxdatasize) )
    cip->ci_max_data_size = maxdatasize ;

    //If a MinReplySize is necessary (> 0 for older versions) the database kernel always expects a 
    //MinReplySize == XPARAM(ulMinReplySize)
    if (omitReplyPart)
    {
        cip->ci_min_reply_size = 0;
    }
    else
    {
        cip->ci_min_reply_size = XPARAM(ulMinReplySize) ;
    }

    cip->ci_max_request_size = cip->ci_max_data_size - cip->ci_min_reply_size ;

    if (   cip->ci_mess_class != RSQL_INFO_REQUEST_EO003
        && cip->ci_mess_class != RSQL_INFO_REQUEST_KEEP_ALIVE_EO003)
     
    {
        /*
         *  Copy the connect information to the task's structure.
         */
        tsk_cip = tcb->connection;
        tsk_cip->ci_state = CON_CONNECTING ;
        tsk_cip->BytesReceived = tsk_cip->BytesSend = 0;
        tsk_cip->ci_use_count ++ ;
        tsk_cip->ci_service = cip->ci_service ;
        tsk_cip->ci_protocol = cip->ci_protocol ;
        tsk_cip->ci_packet_cnt = cip->ci_packet_cnt ;
        tsk_cip->ci_packet_size = cip->ci_packet_size ;
        tsk_cip->ci_min_reply_size = cip->ci_min_reply_size ;
        tsk_cip->ci_max_data_size = cip->ci_max_data_size ;
        tsk_cip->ci_max_request_size = cip->ci_max_request_size ;
        tsk_cip->ci_peer_pid = cip->ci_peer_pid ;
        tsk_cip->ci_peer_ref = cip->ci_peer_ref ;
        tsk_cip->ci_peer_semid = cip->ci_peer_semid ;
        tsk_cip->ci_alter_server_sem = cip->ci_alter_server_sem;

        SAPDB_memcpy ( tsk_cip->ci_peer_node , cip->ci_peer_node ,
                        sizeof(cip->ci_peer_node) );
        SAPDB_memcpy ( tsk_cip->ci_peer_dbname , cip->ci_peer_dbname ,
                        sizeof(cip->ci_peer_dbname) );
        SAPDB_memcpy ( tsk_cip->ci_remote_pid , cip->ci_remote_pid ,
                        sizeof(cip->ci_remote_pid) );

        /*
         *  If the client does not supply a comseg, use own big comseg.
         *  This is true for both: a local big comseg request
         *  and a remote request.
         */
        if ( cip->ci_shmid <= 0)
        {
            tsk_cip->ci_shmid = 0 ;
            tsk_cip->ci_big_comseg = tcb->bigComSegBelongsToUKT->connection->ci_big_comseg ;
            tsk_cip->ci_big_offset = tsk_cip->ci_locbigoffset ;
            tsk_cip->ci_big_size   = tcb->bigComSegBelongsToUKT->connection->ci_big_size ;
        }
        else
        {
            tsk_cip->ci_shmid = cip->ci_shmid ;
        }

        tsk_cip->ci_my_ref = tcb->index ;
        DBG1 (( MF__,"T%d my_ref %d peer_ref %d \n",
            tcb->index,
            tsk_cip->ci_my_ref , tsk_cip->ci_peer_ref ));

        tsk_cip->encryption = cip->encryption;
    }
    /* preliminary */
    messclass = e84_reply_class ( cip );

    if ((    (cip->ci_mess_class != RSQL_INFO_REQUEST_EO003
              && cip->ci_mess_class != RSQL_INFO_REQUEST_KEEP_ALIVE_EO003)
          && (cip->ci_shmid <= 0) ) )
    {

        sql42_create_conpkt ( header , messclass ,
                tcb->index , cip->ci_peer_ref , SP1CE_OK ,
                cip->ci_service , cip->ci_max_segment_size ,
                cip->ci_max_data_size , cip->ci_packet_size ,
                cip->ci_min_reply_size ,
                mydbname, cip->ci_peer_dbname);

        /* local connect */
        sql42_put_int4 ( header , RSQL_OPT_PKTCNT_EO003 ,(tsp00_Int4) cip->ci_packet_cnt );
        sql42_put_int4 ( header , RSQL_OPT_SEMID_EO003 , (tsp00_Int4) tcb->ukt->semid );
        sql42_put_long ( header , RSQL_OPT_PID_EO003   , (long)tcb->ukt->tid );
        sql42_put_int4 ( header , RSQL_OPT_SHMID_EO003 , (tsp00_Int4) tcb->bigComSegBelongsToUKT->connection->ci_shmid );
        sql42_put_long ( header , RSQL_OPT_OFFSET_EO003 , tsk_cip->ci_locbigoffset );
    }
    else
    {
        sql42_create_conpkt ( header , messclass ,
                cip->ci_my_ref , cip->ci_peer_ref , SP1CE_OK ,
                cip->ci_service , cip->ci_max_segment_size ,
                cip->ci_max_data_size , cip->ci_packet_size ,
                cip->ci_min_reply_size ,
                mydbname, cip->ci_peer_dbname);
    }

    sql42_put_string ( header, RSQL_OPT_AUTH_ALLOW_EO003, authenticationAllow );

    DBGOUT;
}

/*
 * =======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_conn_send"
static  void    e84_conn_send ( connection_info*             cip,
                                struct TASK_TYPE *           tcb,
                                RTETask_CommonQueueElement* &pQueueElement )

{
    struct DOUBLY_LINKED    *lquu ;

    DBGIN;

    /* Set move lock, because the task should not move to another UKT 
    until we have received a command from client */ 
    en71LongTermMoveLock( tcb, WAITING_FOR_NEW_COMMAND, true );

    /*
     *  After an accepted connect: send request to proper UKP
     *  This procedure is called from the REQUESTOR process, and
     *  enters the queue element in the IOC queue, the only
     *  queue for external processes
     */
    lquu = *pQueueElement;
    lquu->req_type = REQ_CONNECT ;
    lquu->args.conn_auftrag.task_index = tcb->index;
    DBG1 (( MF__,"put IOC request %d T%d \n", lquu->req_type , tcb->index ));
    en71_iocEnquAndWake ( tcb->ukt, tcb, lquu );

    DBGOUT;
}

/*
 * ======================================================================
 */
#undef  MF__
#define MF__    MOD__"e84_test_application"
void    e84_test_application ( 
struct TASK_TYPE                *tcb )
{
    pid_t             pid ;
    int               task_state ;
    time_t            connect_time ;

    /*
     *  Test, whether the application process is still alive.
     *  If died, cancel its task session.
     */
    if ( tcb->type != TT_UT_EO00 
      && tcb->type != TT_EV_EO00 
      && tcb->type != TT_US_EO00 )
    {
        MSGD (( IERR_REQUESTOR_TEST_ILL_TASK ));
        return ;
    }

    /*
     *  Copy the states to avoid inconsistent values
     *  due to process scheduling during task release.
     */
    task_state   = tcb->state ;
    connect_time = tcb->connection->ci_connect_time ;

    if ( ! connect_time ) return ;

    DBG1 (( MF__,"check T%d \n", tcb->index ));

    /*
     *  If a task had been selected for a connect,
     *  'tcb->connection->ci_connect_time' got the current time value.
     *  If the parcitipating processes failed to complete the
     *  connection establishment, the task hangs in an intermediate state.
     *  If this lasts too long, free the task.
     */
    if ( ((task_state == TSK_CONNECTWAIT_EO00) || (task_state == TSK_INACTIVE_EO00) )
	     && (    tcb->connection->ci_state == CON_CONNECTING ))
    {
        time_t time_now = KGS->current_time;

        if ( (time_now - connect_time) > (2 * SOCK_CON_TMO) )
        {
            MSGD (( WRN_REQUESTOR_CNCL_FROM_CONN , tcb->index , task_state ));
            MSGD (( WRN_REQUESTOR_CNCL_CONN_TIME,  tcb->index , (long) connect_time , (long) time_now ));
            /* Previous implementation destroyed connect_time information 
               This was more than deadly, since almost all tests used connect_time
               to find active communication segments... Changing communication state
               from CON_CONNECTING to CON_ABORTED is possibly colliding with 
               e75_connect_request changing from CON_CONNECTING to CON_ESTABLISHED.
               But if interleaved either 'CON_ABORTED' will overwrite CON_CON_ESTABLISHED
               or 'CON_CON_ESTABLISHED' will overwrite 'CON_ABORTED'. The latter case is
               unproblematik, since it simply erases the effect of a timeout condition.
               The first one leads to abort of communication soon afterwards, which is ok
               since this is what was intended by the timeout anyhow... But in most cases
               the CON_ABORT will be noticed during connect request handling and will
               simply release the connection as intended. */
            tcb->connection->ci_state = CON_ABORTED ; /* PTS 1113931 */
            e72_wake ( tcb->ukt );
        }
    }

    /*
     *  Test applications of locally connected tasks only.
     */
    if ( (  (tcb->connection->ci_state != CON_TIMEDOUT)
         && (tcb->connection->ci_state != CON_ABORTED)
         && (tcb->connection->ci_state != CON_CONNECTING)
         && (tcb->connection->ci_state != CON_UNUSED)
         && (tcb->connection->ci_peer_semid > 0) )
      || (  (tcb->connection->ci_state == CON_UNUSED)
         && (tcb->state == TSK_VRELEASE_EO00)
         && tcb->connection->ci_connect_time ) )
    {
        pid = tcb->connection->ci_peer_pid ;
        DBG1 (( MF__,"test pid %ld \n", (long) pid ));
        if ( pid > 1 )
        {
            if ( kill ( pid , 0 ) != 0 )
            {
                if ( errno == ESRCH )
                {
                    MSGD (( WRN_KILL_TSK_DIED_APPL, tcb->index , (long) pid ));
                    tcb->connection->ci_state = CON_ABORTED ;
                    tcb->pTaskObject->Cancel();
                    e72_wake ( tcb->ukt );
                }
            }
        }
    }
}

/*
 * ======================================================================
 */

#undef  MF__
#define MF__    MOD__"en84_remove_unrel_connnections"
extern "C" void en84_remove_unrel_connnections()
{
    struct TASK_TYPE                *tcb;

    /*
    *  The CONNECT REQUEST is for a user or event task.
    *
    *  Test, whether the application processes for all user tasks
    *  are still alive.
    */
    DBG1 (( MF__,"testing applications \n" ));
    for ( tcb = KGS-> first_event_task ; tcb < KGS-> first_backup_task ; tcb ++ )
    {
        e84_test_application ( tcb );
        sqlyieldthread();
    }
    for ( tcb = KGS-> first_user ; tcb <= KGS-> pLastTaskCtrl ; tcb ++ )
    {
        e84_test_application ( tcb );
        sqlyieldthread();
    }
}

/*
 * ======================================================================
 */

#undef  MF__
#define MF__    MOD__"e84_reply_class"
static  int    e84_reply_class ( connection_info     *cip )
{
    int             messclass ;


    switch ( cip->ci_mess_class )
    {
    case RSQL_INFO_REQUEST_EO003 :
    case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003 :
    messclass = RSQL_INFO_REPLY_EO003 ;
    break ;
    case RSQL_USER_CONN_REQUEST_EO003 :
    messclass = RSQL_USER_CONN_REPLY_EO003 ;
    break ;
    case RSQL_KERN_CONN_REQUEST_EO003 :
    messclass = RSQL_KERN_CONN_REPLY_EO003 ;
    break ;
    default :
    messclass = cip->ci_mess_class ;
    break ;
    }

    return ( messclass );
}

/*==========================================================================*/
 
#include <poll.h>

static int en84_RecvConpkt ( int              sd ,
                             rte_header      *header ,
                             int             *statep ,
                             char           **errm,
                             int              msecTimeoutTime )
{
#undef  MF__
#define MF__    "en84_RecvConpkt"
    int                 rc ;
    int                 lgt ;
    int                 len ;
    int                 swap_type ;
    tsp00_Int4          len4 ;
    rte_connect_packet *cpack ;
    char               *ptr ;
    struct pollfd       fifoFd[1];    

    DBGIN;

    cpack = reinterpret_cast<rte_connect_packet*>(header + 1);
    ptr   = (char*) header ;
    /*
     *  In the fist place, try to receive the minimal packet only.
     *  This is to avoid reading parts of packets that follow
     *  immediately after the connect packet.
     *  After the minimal part of the connect packet is received,
     *  we can determine the exact packet length.
     */
    lgt = RTE_CONPKT_MINSIZ ;
    len = 0 ;
    len4 = 0 ;

    fifoFd[0].fd = sd;
    fifoFd[0].events = POLLIN;
    /*
     *  Receive connect packet.
     */
    if ( msecTimeoutTime <= 0 ) msecTimeoutTime = -1;

    for ( ; lgt > 0 ; )
    {
        fifoFd[0].revents = 0;
        rc = poll(fifoFd, 1, msecTimeoutTime);
        if ( rc < 0 )
        {
            if ( errno == EINTR )
            {
                DBG1 (( MF__,"poll interrupted, continuing \n" ))
                continue;
            }
            else
            {
                DBG1 (( MF__,"poll error: %s\n" , sqlerrs() ))
                *errm = (char*)"requestor fifo poll error" ;
                return ( SP1CE_NOTOK );
            }
        }
        else if ( rc == 0 )
        {
            if ( statep )
            {
                *statep = CON_TIMEDOUT;
            }
            *errm = (char*)"connect timed out" ;
            DBG1 (( MF__,"%s \n", *errm ));
            return ( SP1CE_NOTOK );
        }


        DBG1 (( MF__,"read sd %d size %d \n", sd , lgt ));

        rc = read ( sd , ptr , lgt );

        DBG1 (( MF__,"read sd %d rc %d \n", sd , rc ));
        if ( rc == -1 )
        {
            if ( errno == EINTR )
            {
                DBG1 (( MF__,"fifo read interrupted, continuing \n" ));
                rc = 0 ;
                continue ;
            }

            if ( (errno == ECONNRESET) || (errno == EPIPE) )
            {
                *errm = (char*)"connection closed by counterpart" ;
                DBG1 (( MF__,"%s \n", *errm ));
                rc = SP1CE_RELEASED ;
            }
            else
            {
                *errm = (char*)"fifo read error" ;
                DBG1 (( MF__,": %s: %s\n", *errm , sqlerrs() ));
                rc = SP1CE_NOTOK ;
            }
            return ( rc );
        }

        if ( rc == 0 )
        {
            *errm = (char*)"connection closed by counterpart" ;
            DBG1 (( MF__,"%s \n", *errm ));
            return ( SP1CE_RELEASED );
        }

        DBG1 (( MF__,"received %d bytes \n", rc ));
        ptr += rc ;
        lgt -= rc ;
        len += rc ;
        if ( ! len4 && (len >= RTE_CONPKT_MINSIZ) )
        {
            swap_type = cpack->cp_mess_code[1];
            sql42_unpack_int4 ( swap_type , header->rh_act_send_len , &len4 );
            if ( (len4 < RTE_CONPKT_MINSIZ) || (len4 > RTE_CONPKT_SIZE) )
            {
                *errm = (char*)"received a garbled packet" ;
                DBG1 (( MF__,": %s, length %d \n", *errm , len4 ));
                return ( SP1CE_NOTOK );
            }
            lgt = len4 - len ;
        }
    }

    DBG1 (( MF__,"returning %d \n", SP1CE_OK ));
    return ( SP1CE_OK );
}

/*
 * ======================================================================
 */

#undef  MF__
#define MF__    MOD__"e84_dump_allowed"
static SAPDB_Bool e84_dump_allowed()
{
  SAPDB_Bool                     dumpEnabled = false;

  SAPDBErr_MessageList          errList;
  RTEConf_Parameter::String     valueString;

  if((RTEConf_ParameterAccess::Instance())->GetString( KERNELPARAM_EXTERNAL_DUMP_REQUEST, valueString, errList))
  {
      if ( 0 == strcmp("YES", (char*)valueString ))
          dumpEnabled = true;
      else
          dumpEnabled = false;
  }
  else
      dumpEnabled = false;

  return dumpEnabled;
}


