/*!
  @file           ven33.c
  @author         JoergM
  @brief          RunTime: Communication Functions for remote Access
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

/*PRETTY*/

#include "RunTime/System/RTESys_MemoryBarrier.h"

/* exported functions */
#include "gen00.h"
#include "geo007_1.h"
#include "vsp001c.h"
#include "gen003.h"
#include "gen33.h"

/* imported functions */

#include "gen32.h"
#include "gen41.h"
#include "gen42.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"
#include "RunTime/RTE_saveUNIXcalls.h"

/* local functions */

/* exported data */

static void sql33_detach_shm ( connection_info     *cip );

static void sql33_remove_shm ( connection_info     *cip );

static void sql33_remove_sem ( connection_info     *cip );

static  int sql33_attach_big_comseg ( connection_info *cip ,
                                      connection_info  conn_info [ ],
                                      int              count);

static void sql33_detach_big_comseg ( connection_info *cip ,
                                      connection_info  conn_info [ ],
                                      int              count);

static SAPDB_Bool  sql33_GetKernelPidAndPPid (SAPDB_Char *     szServerDB, 
                                               RTE_OSPid *     pKernelWatchDogPid,
                                               RTE_OSPid *     pKernelPid,
                                               tsp00_ErrTextc  pErrText );

/* imported data */


/* local data */


/* Macros */

/*
 * ===========================================================================
 */

externC teo00_Longuint 
en33Request ( connection_info         *pConnInfo, 
              teo00_Longuint           ulDataLen,
              tsp00_ErrTextc           pErrText )
{
#undef	MF__
#define	MF__	"en33Request"
  teo00_Longuint ulCommState;

  DBGIN;

  pConnInfo->ci_request_lgt = ulDataLen ;

  if (0 == pConnInfo->ci_min_reply_size)
  {
    pConnInfo->ci_reply = pConnInfo->ci_request;
    pConnInfo->ci_reply_size  = pConnInfo->ci_max_data_size;
  }
  else
  {
    pConnInfo->ci_reply = (rte_header*) 
                          (((char*) pConnInfo->ci_request) + ulDataLen + RTE_HEADER_SIZE_EO003 );
    pConnInfo->ci_reply_size  = pConnInfo->ci_max_data_size - pConnInfo->ci_request_lgt ;
  }

  ulCommState = sql33_request ( pConnInfo, pErrText ) ;

  return( ulCommState ) ;
}

/*
 * ===========================================================================
 */

externC teo00_Longuint 
en33Receive ( connection_info          *pConnInfo, 
              teo003_CommPacketRecord **ppResultPacket,
              teo00_Longuint           *pulResultLen,
              tsp00_ErrTextc            pErrText )
{
#undef	MF__
#define	MF__	"en33Receive"
  teo00_Longuint                  ulCommState;

  DBGIN;

  ulCommState = sql33_receive( pConnInfo, pErrText );

  *ppResultPacket = (teo003_CommPacketRecord *) pConnInfo->ci_reply ;
  *pulResultLen   = pConnInfo->ci_reply->rh_max_send_len - RTE_HEADER_SIZE_EO003 ;

  return( ulCommState ) ;
}

/*
 * ===========================================================================
 */

int     sql33_connect ( 
connection_info		*cip ,
tsp00_ErrTextc      pErrText,
connection_info		conn_info [ ],
int                 count)
{
#undef	MF__
#define	MF__	"sql33_connect"
    int  semaphoreId;
    int				rc ;
    int				idx ;
    int				savpktcnt ;
    int				reply_fd ;
    int				kernel_fd ;
    int				messclass ;
    tsp00_Int4			myref ;
    int				retcod ;
    int				service ;
    SQL_DBNAMEC			mydb ;
    SQL_DBNAMEC			peerdb ;
    tsp00_Int4			shmid4 ;
    uid_t			uid ;
    long			siz ;
    long			off ;
    char			*ptr ;
    comseg_header		*comseg ;
    rte_header			*header ;
    char			packet [ RTE_CONPKT_SIZE ];
    VF_FILENAMEC		file ;
    struct sembuf		semrcv ;
    tsp00_DbNamec   peer_dbname;  


    DBG1 (( MF__,"local dbname '%s' service %d \n",
			cip->ci_peer_dbname , cip->ci_service ))

    strcpy (peer_dbname, cip->ci_peer_dbname);

	for (idx = 0; cip->ci_peer_dbname[idx] != 0; ++idx)
	{
		if (islower (cip->ci_peer_dbname[idx]))
		{
			peer_dbname[idx] = toupper (cip->ci_peer_dbname[idx]);
		}
	}

    if (!sql33_GetKernelPidAndPPid (peer_dbname, &cip->kernelWatchDogPID, &cip->kernelPID, pErrText))
    {
 		DBG1 (( MF__,"cannot get kernel PID's\n"))
        //Error is ignored in order to support older kernel versions. In those versions the access to the PID and PPID file may
        //still be restricted.
        //return commErrNotOk_esp01;
    }

	rc = sql32_open_kernel_fifo ( peer_dbname , &kernel_fd ,
		&uid , pErrText );
	if ( rc != commErrOk_esp01 )
	{
		DBG1 (( MF__,"cannot open knl fifo: %s\n", pErrText ))
			return ( rc );
	}

    DBG1 (( MF__,"opened kernel fifo, fd %d \n", kernel_fd ))

    /*
     *  The reply fifo must be open before REQUESTOR writes to it.
     *  Otherwise REQUESTOR might be fast enough to close the reply fifo
     *  before the application had it open.
     *  The last close on a fifo clears its contents.
     */
    rc = sql32_open_reply_fifo ( peer_dbname , cip->ci_my_pid ,
				cip->ci_my_ref , (char *)file , &reply_fd , pErrText );
    if ( rc != commErrOk_esp01 )
    {
	DBG1 (( MF__,"cannot open reply fifo: %s\n", pErrText ))
	(void) close ( kernel_fd );
	return ( rc );
    }

    /*
     *  Create own semaphore for communication with DB.
     *  Key for semaphore is own pid modified by reference.
     *  If a semaphore key of this value exists, try some other values.
     */
    semaphoreId = cip->ci_my_semid = sql41_create_sem ( (key_t)0 , (int) cip->ci_my_ref , 16 ,
			    0666 , uid , "us" , peer_dbname );
    DBG1 (( MF__,"semid %d \n", cip->ci_my_semid ))
    if ( cip->ci_my_semid == UNDEF )
    {
	    en42FillErrText(pErrText, "cannot create communication semaphore");
	    (void) close ( kernel_fd );
	    (void) close ( reply_fd );
	    (void) unlink ( (char *)file );
	    return ( commErrNotOk_esp01 );
    }

    /*
     *  This method works with a single packet only.
     */
    savpktcnt = cip->ci_packet_cnt ;
    cip->ci_packet_cnt           = 1;
    strcpy ( cip->ci_authentication_allow, "BASIC" );;

    cip->ci_protocol = PROT_BIGSHM_EO003 ;
    /*
     *  Don't initialize these. They are used by vserver
     *  and are otherwise initialized by ven03c.
     *
     *  cip->ci_max_segment_size = 0 ;
     *  cip->ci_max_data_size = 0 ;
     *  cip->ci_packet_size = 0 ;
     *  cip->ci_min_reply_size = 0 ;
     */

    header = (rte_header*) packet ;
    sql42_create_conpkt ( header , RSQL_USER_CONN_REQUEST_EO003 ,
			    cip->ci_my_ref , (tsp00_Int4)0 ,
			    commErrOk_esp01 , cip->ci_service ,
			    cip->ci_max_segment_size ,
			    cip->ci_max_data_size ,
			    cip->ci_packet_size ,
			    cip->ci_min_reply_size ,
			    "" , cip->ci_peer_dbname );
    sql42_put_int4 ( header , RSQL_OPT_PKTCNT_EO003 , (tsp00_Int4) cip->ci_packet_cnt );
    sql42_put_long ( header , RSQL_OPT_PID_EO003    , (long) cip->ci_my_pid );
    sql42_put_int4 ( header , RSQL_OPT_SEMID_EO003  , (tsp00_Int4) cip->ci_my_semid );
    sql42_put_string (header, RSQL_OPT_FIFO_EO003   , (char *)file );
	sql42_put_int1 ( header, RSQL_OPT_ALTER_SERV_SEM_EO003, (tsp00_Uint1) true );

    /* Works for both the Local Client/Kernel and the Local XServer/Kernel communication */
    if (cip->ci_min_reply_size == 0)  /* In the VServer this means that we already know that the Kernel supports */
                                      /* omitting of the reply part (by the info request) */
    {
        /* Client intends not to use a reply part */
        sql42_put_int1 (header, RSQL_OPT_OMIT_REPLY_PART_EO003, (tsp00_Uint1) true);
    }
    /*
     *  The following will be used by vserver.
     */
    if ( cip->ci_peer_node[0] )
    {
	sql42_put_string ( header , RSQL_OPT_NODE_EO003 , (char *)cip->ci_peer_node );
    }

    if ( cip->ci_remote_pid[0] )
    {
	sql42_put_string ( header , RSQL_OPT_REM_PID_EO003 , (char *)cip->ci_remote_pid );
    }

    sql42_put_int1 ( header, RSQL_OPT_ENCRYPTION_EO003, cip->encryption );

    rc = sql42_send_conpkt ( kernel_fd , header , pErrText );
    if ( rc != commErrOk_esp01 )
    {
	    DBG1 (( MF__,"cannot write to kernel fifo: %s \n", pErrText ))
	    (void) close ( kernel_fd );
	    (void) close ( reply_fd );
	    (void) unlink ( (char *)file );
	    sql33_remove_sem ( cip );
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( rc );
    }
    (void) close ( kernel_fd );
    DBG1 (( MF__,"sent connect request to '%s' \n", cip->ci_peer_dbname ))

    rc = sql42_recv_conpkt ( reply_fd , header , &cip->ci_state , pErrText );
    if ( rc != commErrOk_esp01 )
    {
        tsp00_ErrTextc pTmpErrText;
        en42FillErrText(pTmpErrText, "sem %d:%s", semaphoreId, &pErrText[0]);
        memcpy(&pErrText[0], &pTmpErrText[0], sizeof(tsp00_ErrTextc));
        DBG1 (( MF__": %s \n", pErrText ))
	    RTE_save_close ( reply_fd );
	    RTE_save_unlink ( (char *)file );
	    sql33_remove_sem ( cip );
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( rc );
    }
    DBG1 (( MF__,"received reply from fd %d lgt %d \n", reply_fd , header->rh_max_send_len ))

    (void) close ( reply_fd );
    (void) unlink ( (char *)file );
    DBG1 (( MF__,"closed/unlinked '%s', fd %d \n", file , reply_fd ))

    shmid4 = -1 ;
    cip->ci_big_offset = -1 ;
    sql42_extract_conpkt ( header , &messclass ,
			    &myref , &cip->ci_peer_ref ,
			    &retcod , &service ,
			    &cip->ci_max_segment_size ,
			    &cip->ci_max_data_size ,
			    &cip->ci_packet_size ,
			    &cip->ci_min_reply_size ,
			    &cip->ci_peer_swap_type ,
			    (char *)mydb , (char *)peerdb );
    sql42_get_int4 ( header , RSQL_OPT_SHMID_EO003 , &shmid4 );
    sql42_get_long ( header , RSQL_OPT_OFFSET_EO003 , &cip->ci_big_offset );
    cip->ci_shmid = shmid4 ;
    DBG1 (( MF__,"big shmid %d off %ld \n",
		cip->ci_shmid , cip->ci_big_offset ))

    sql42_get_string ( header , RSQL_OPT_AUTH_ALLOW_EO003, 
                       sizeof(cip->ci_authentication_allow), 
                       cip->ci_authentication_allow );

    /*
     *  Check the received information.
     */
    rc = commErrOk_esp01 ;
    if ( messclass != RSQL_USER_CONN_REPLY_EO003 )
    {
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server messclass %d expected %d",
					    messclass , RSQL_USER_CONN_REPLY_EO003 ))
	    en42FillErrText(pErrText, "protocol error: got %d not USER_CONN_REPLY", messclass);
#if ACCEPT_KERN_CONN_REPLY
	    if ( messclass != RSQL_KERN_CONN_REPLY_EO003 )
#endif
	    rc = commErrNotOk_esp01 ;
    }
    if ( myref != cip->ci_my_ref )
    {
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server reference %d expected %d",
						        myref , cip->ci_my_ref ))
	    en42FillErrText(pErrText,
                        "protocol error: ref %d/%d",
                        myref,
                        cip->ci_my_ref);
	    rc = commErrNotOk_esp01 ;
    }
    if ( service != cip->ci_service )
    {
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server service %d expected %d",
						    service , cip->ci_service ))
	    en42FillErrText(pErrText,
                        "protocol error: service %d/%d",
                        service,
                        cip->ci_service);
	    rc = commErrNotOk_esp01 ;
    }
    if ( strcmp ( (char *)peerdb , (char *)cip->ci_peer_dbname ) )
    {
	    en42FillErrText(pErrText,
                        "protocol error: dbname %d/%d",
                        peerdb,
                        cip->ci_peer_dbname);
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server dbname '%s' expected '%s'",
						    peerdb , cip->ci_peer_dbname ))
	    rc = commErrNotOk_esp01 ;
    }
    if ( retcod != commErrOk_esp01 )
    {
	    if ( rc == commErrOk_esp01 )
	    {
	        switch ( retcod )
	        {
	        case commErrNotOk_esp01 :
	            en42FillErrText(pErrText,
                                "protocol error",
                                retcod);
                break;
	        case SP1CE_TIMEOUT :
	            en42FillErrText(pErrText,
                                "protocol error: TIMEOUT" );
	    	    break ;
	        case SP1CE_TASKLIMIT :
	            en42FillErrText(pErrText,
                                "task limit" );
		        break ;
	        case SP1CE_RELEASED :
	            en42FillErrText(pErrText,
                                "server rejected connection" );
		        break ;
	        default :
	            en42FillErrText(pErrText,
                                "database not running:retcod %d", retcod );
		        break ;
	        }
	        rc = (int) retcod ;
	        DBG1 (( MF__,"sql33_connect: server returned %d \n", rc ))
	    }
    }
    else /* if retcod != commErrOk_esp01 */
    {
	    if ( shmid4 == -1 )
	    {
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server '%s' didn't return shmid",
							    cip->ci_peer_dbname ))
	        en42FillErrText(pErrText,
                            "protocol error: shmid == -1" );
	        rc = commErrNotOk_esp01 ;
	    }
	    if ( cip->ci_big_offset == -1 )
	    {
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "server '%s' didn't return offset",
							    cip->ci_peer_dbname ))
	        en42FillErrText(pErrText,
                            "protocol error: shmoffset == -1" );
	        rc = commErrNotOk_esp01 ;
	    }
    }
    /* end if retcod != commErrOk_esp01 */

    /*
     *  On any protocol error close connection.
     */
    if ( rc != commErrOk_esp01 )
    {
	    sql33_remove_sem ( cip );
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( rc );
    }

    /*
     *  Well, the REQUESTOR made the connection and everything is fine,
     *  except of the fact that the UKP did not yet initialize the comseg.
     *  Wait on own semaphore until notified by the UKP.
     */
    {
        struct sembuf ownrcv;
        ownrcv.sem_num = 0;
        ownrcv.sem_op = -1;
        ownrcv.sem_flg = 0;
        DBG1 (( MF__,"wait  semid  %d \n", cip->ci_my_semid ))
        rc = RTE_save_semop ( cip->ci_my_semid , &ownrcv , 1 );
    }
    DBG1 (( MF__,"awoke semid  %d \n", cip->ci_my_semid ))
    if ( (rc == UNDEF) && (errno != EIDRM) && (errno != EINVAL) )
    {
    	en42FillErrText( pErrText,
                         "connection broken: semop (%d:%s)",
                         errno,
                         sqlerrs() );
	    MSGD (( -11987, ERR_TYPE, "CONNECT ", "receive sem %d : %s",
						cip->ci_my_semid , &pErrText[0] ))
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( commErrNotOk_esp01 );
    }

    /*
     *  Fine, the UKP awoke us. Now, the comseg should be set up right.
     *  Attach the big communication segment,
     *  if not already attached by another session,
     *  and initialize the comseg pointers.
     */
    rc = sql33_attach_big_comseg ( cip , conn_info, count );
    if ( rc != commErrOk_esp01 )
    {
    /* PTS 1109601 */
        (void)sql33_cancel ( cip, pErrText );
        /*
           If attach of big communication shared memory segment fails due to client resource problem
           (number of shared memory segments exceeded) the kernel resources for this connection are
           blocked forever! Therefore a cancellation request is enforced here. sql33_cancel() is
           needed, since sql33_release() would need a valid shared memory...
         */
    	en42FillErrText(pErrText,
                        "database not accessible: attach failed" );
	    sql33_remove_sem ( cip );
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( rc );
    }
    comseg = cip->ci_comseg ;

    /*
     *  Check the big comseg values.
     */
    sql32_lock_comseg ( cip , "sql33_connect: 0x%08lx \n" );
    if ( comseg->cs_server_state != commErrOk_esp01 )
    {
    	en42FillErrText(pErrText,
                        "server rejected connection server state %d",
                        comseg->cs_server_state );
	    MSGD (( -11987, ERR_TYPE, "CONNECT ", "server state %d",
				        comseg->cs_server_state ))
	    rc = comseg->cs_server_state ;
    }
    else if (    (comseg->cs_client_pid  != cip->ci_my_pid)
              || (comseg->cs_client_ref  != cip->ci_my_ref)
              || (comseg->cs_packet_cnt  != cip->ci_packet_cnt)
	      || (comseg->cs_packet_size != cip->ci_packet_size)  )
    {
    	en42FillErrText(pErrText,
                        "server rejected connection pid/ref/cnt/size mismatch" );
	    MSGD (( -11987, ERR_TYPE, "CONNECT ", "wrong packet values:" ))
	    MSGD (( -11987, ERR_TYPE, "CONNECT ", "  pid %ld ref %d cnt %d siz %ld",
			    (long) comseg->cs_client_pid , comseg->cs_client_ref ,
			    comseg->cs_packet_cnt , comseg->cs_packet_size ))
	    MSGD (( -11987, ERR_TYPE, "CONNECT ", "  wanted: pid %ld ref %d cnt %d siz %ld",
			    (long) cip->ci_my_pid , cip->ci_my_ref ,
			    cip->ci_packet_cnt , cip->ci_packet_size ))
	    rc = commErrNotOk_esp01 ;
    }
    if ( rc != commErrOk_esp01 )
    {
	    sql32_unlock_comseg ( cip );
	    sql33_detach_big_comseg ( cip , conn_info, count );
	    sql33_remove_sem ( cip );
	    cip->ci_packet_cnt = savpktcnt ;
	    return ( rc );
    }

    /*
     *  update connection info
     */
    cip->ci_peer_pid   = comseg->cs_server_pid ;
    cip->ci_peer_ref   = comseg->cs_server_ref ;
    cip->ci_peer_semid = comseg->cs_server_semid ;
    DBG1 (( MF__,"peer ref   %ld \n", (long) comseg->cs_server_ref ))
    DBG1 (( MF__,"peer semid %d \n", comseg->cs_server_semid ))

    /*
     *  initialize shared memory
     */
    comseg->cs_client_state = commErrOk_esp01 ;
    comseg->cs_client_semid = cip->ci_my_semid ;
    comseg->cs_client_flag  = 0 ;
    comseg->cs_server_flag  = 2 ;
    sql32_unlock_comseg ( cip );

    DBG1 (( MF__,"my_pid    %ld \n", (long) comseg->cs_client_pid ))
    DBG1 (( MF__,"peer_pid  %ld \n", (long) comseg->cs_server_pid ))
    DBG1 (( MF__,"my_ref    %d \n", comseg->cs_client_ref ))
    DBG1 (( MF__,"peer_ref  %d \n", comseg->cs_server_ref ))
    DBG1 (( MF__,"cs_shmid  %d \n", comseg->cs_shmid      ))

    /*
     *  Allocate a packet buffer for the application.
     *  Restore the original packet count.
     *  Use ci_compack to point into the comseg,
     *  while ci_packet is used to point into the local buffer.
     *  Add 8 bytes for alignment.
     */
    cip->ci_packet_cnt = savpktcnt ;
    cip->ci_compack    = cip->ci_packet[0];
    siz = cip->ci_packet_cnt * cip->ci_packet_size ;
    if ( ALLOC_MEM_EO57( (void **)&ptr, siz + 8 ) != NO_ERROR_EO001 )
    {
	    sql32_lock_comseg ( cip , "sql33_connect: 0x%08lx \n" );
	    if ( (comseg->cs_client_pid == cip->ci_my_pid) &&
	         (comseg->cs_client_ref == cip->ci_my_ref) )
	    {
		    union semun     arg ;
	        DBG1 (( MF__,"free shm \n" ))
	        comseg->cs_client_state = commErrNotOk_esp01 ;
            RTESys_WriteMemoryBarrier();
	        comseg->cs_client_flag  = 1 ;
		    arg.val = 1 ;
		    RTE_save_semctl ( comseg->cs_server_semid, 0 , SETVAL , SEMCTL_SETVAL(arg) );
	    }
	    sql32_unlock_comseg ( cip );
	    rc= commErrNotOk_esp01 ;
    }
    else
    {
	    cip->ci_packet_buffer = ptr ;
	    off = (long) ptr & 7 ;
	    if ( off )
	    {
	        ptr += 8 - off ;
	    }
	    siz = cip->ci_packet_size ;
	    for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
	    {
	        cip->ci_packet[idx] = (rte_header*) ptr ;
	        ptr += siz ;
	    }
    }

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

/*ARGSUSED*/	/* pErrText unused */
int     sql33_release ( 
connection_info		*cip ,
tsp00_ErrTextc VAR_ARRAY_REF pErrText,
connection_info		conn_info [ ],
int                 count)
{
#undef	MF__
#define	MF__	"sql33_release"
    comseg_header		*comseg ;


    DBGIN;
    comseg = cip->ci_comseg ;

    /*
     *  In case the session is not completed, we simulate a request with
     *  SP1CE_RELEASED, which will terminate the session
     *  if the UKP is in VRECEIVE.
     *  This MUST NOT be done, if the comseg has been dedicated to another
     *  session meanwhile (e.g. because of a command timeout).
     */
    if ( comseg && (cip->ci_shmid > 0) )
    {
	    sql32_lock_comseg ( cip , "sql33_release: 0x%08lx \n" );
	    if ( (comseg->cs_client_pid == cip->ci_my_pid) &&
	         (comseg->cs_client_ref == cip->ci_my_ref) )
	    {
		    union semun     arg ;
	        DBG1 (( MF__,"free shm \n" ))
	        comseg->cs_client_state = SP1CE_RELEASED ;
            RTESys_WriteMemoryBarrier();
	        comseg->cs_client_flag  = 1 ;
		    arg.val = 1 ;
		    RTE_save_semctl ( comseg->cs_server_semid, 0 , SETVAL , SEMCTL_SETVAL(arg) );
	    }
	    sql32_unlock_comseg ( cip );
    }

    if ( cip->ci_protocol == PROT_BIGSHM_EO003 )
    {
	    sql33_detach_big_comseg ( cip , conn_info, count );
	    cip->ci_shmid = 0 ;
    }
    else
    {
	    sql33_detach_shm ( cip );
	    sql33_remove_shm ( cip );
    }
    sql33_remove_sem ( cip );

    FREE_MEM_EO57( cip->ci_packet_buffer );
    cip->ci_packet_buffer = 0 ;

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

int     sql33_request ( 
connection_info		*cip ,
tsp00_ErrTextc VAR_ARRAY_REF pErrText )
{
#undef	MF__
#define	MF__	"sql33_request"
    int				rc ;
    comseg_header		*comseg ;


    DBGIN;
    comseg = cip->ci_comseg ;

    /*
     *  Set a lock on the comseg.
     */
    sql32_lock_comseg ( cip , "sql33_request: 0x%08lx \n" );
    DBG1 (( MF__,"client-pid %ld ref %d state %d \n",
			(long) comseg->cs_client_pid , comseg->cs_client_ref ,
			comseg->cs_client_state ))
    DBG1 (( MF__,"client-flag %d semid %d \n",
			comseg->cs_client_flag , comseg->cs_client_semid ))
    DBG1 (( MF__,"server-pid %ld ref %d state %d \n",
			(long) comseg->cs_server_pid , comseg->cs_server_ref ,
			comseg->cs_server_state ))
    DBG1 (( MF__,"server-flag %d semid %d \n",
			comseg->cs_server_flag , comseg->cs_server_semid ))

    if ( (comseg->cs_client_pid != cip->ci_my_pid)
	  || (comseg->cs_client_ref != cip->ci_my_ref) )
    {
	    /*
	     *  Kernel timed out and dedicated the User-Task to
	     *  another application. We should try a reconnect.
	     */
	    sql32_unlock_comseg ( cip );
	    en42FillErrText(pErrText,
                        "command timeout: pid %d/%d ref %d/%d",
                        comseg->cs_client_pid,
                        cip->ci_my_pid,
                        comseg->cs_client_ref,
                        cip->ci_my_ref ) ;
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "session re-used, command timeout?" ));
	    return ( SP1CE_TIMEOUT );
    }

    /*
     *  If there were previous errors, refuse the request
     */
    if ( comseg->cs_server_state != commErrOk_esp01 )
    {
	    rc = comseg->cs_server_state;
	    sql32_unlock_comseg ( cip );
        if ( rc == SP1CE_SHUTDOWN )
        {
    	    en42FillErrText(pErrText,
                            "connection broken by SHUTDOWN" );
        } else if ( rc == SP1CE_TIMEOUT )
        {
    	    en42FillErrText(pErrText,
                            "connection broken by TIMEOUT" );
        }
	    else
	    {
	        switch ( rc )
	        {
	        case SP1CE_RELEASED :
    	        en42FillErrText(pErrText,
                                "connection broken by kernel release" );
		        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel released connection!" ))
		        break ;
	        case SP1CE_CRASH :
    	        en42FillErrText(pErrText,
                                "connection broken by kernel CRASH" );
    		    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel aborted connection!" ))
    		    break ;
	        default :
    	        en42FillErrText(pErrText,
                                "connection broken by server state %d", rc );
    		    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel broke connection!" ))
	    	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "   (server-state %d)", rc ))
                break;
	        }
	    }
	    return ( rc );
    }

    if (    (comseg->cs_server_pid   != cip->ci_peer_pid)
	 || (comseg->cs_server_ref   != cip->ci_peer_ref))
    {
    	en42FillErrText(pErrText,
                        "connection broken by pid,ref or semid mismatch");
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel broke connection!" ))
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "   (server-pid %5ld ref %3d sem %5d",
	        (long) comseg->cs_server_pid , comseg->cs_server_ref ,
	        comseg->cs_server_semid ))
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "   remembered %5ld     %3d     %5d)",
	        (long) cip->ci_peer_pid , cip->ci_peer_ref , cip->ci_peer_semid ))
	    sql32_unlock_comseg ( cip );
	    return ( commErrNotOk_esp01 );
    }

    /*
     *  We have to observe a strictly alternating communication
     */
    if ( (comseg->cs_client_flag != 0) || (comseg->cs_server_flag != 2) )
    {
	    sql32_unlock_comseg ( cip );
    	en42FillErrText(pErrText,
                        "unbalanced request/reply %d/%d",
                        comseg->cs_client_flag,
                        comseg->cs_server_flag );
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "unbalanced request/reply" ))
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "    cliflg,srvflg %d,%d", 
		         comseg->cs_client_flag , comseg->cs_server_flag ))
	    return ( commErrNotOk_esp01 );
    }

    /*
     *  Now, we can access the comseg.
     */
    DBG1 (( MF__,"comseg 0x%08lx cmpack 0x%08lx \n",
	    (long) comseg , (long) cip->ci_compack ))
    DBG1 (( MF__,"shmreq 0x%08lx shmrpl 0x%08lx \n",
	    (long) cip->ci_compack , (long) (cip->ci_compack + 1)
					    + cip->ci_request_lgt ))
    DBG1 (( MF__,"reques 0x%08lx reply  0x%08lx \n",
	    (long) cip->ci_request , (long) cip->ci_reply ))
    DBG1 (( MF__,"req_lgt %ld send_len %d \n",
	    cip->ci_request_lgt , cip->ci_request->rh_max_send_len ))
    SAPDB_memcpy ( cip->ci_compack , cip->ci_request ,
	   RTE_HEADER_SIZE_EO003 + cip->ci_request_lgt );
    /*
     *  Flag the comseg as 'filled in by application'.
     */
    RTESys_WriteMemoryBarrier();
    comseg->cs_client_flag = 1 ;
    /*
     *  If connected via big-comseg, set the common flag.
     */
    if ( cip->ci_big_comseg ) cip->ci_big_comseg->bc_common_flag = 1 ;

    /* get new semid */
    cip->ci_peer_semid = comseg->cs_server_semid;

    /*
     *  Release the lock on the comseg.
     */
    sql32_unlock_comseg ( cip );

    /*
     *  Wake the remote kernel process
     */
    if ( cip->ci_big_comseg )
    {
	    union semun     arg ;
	    arg.val = 1 ;
	    DBG1 (( MF__,"semctl semid  %d \n", cip->ci_peer_semid ))
	    rc = RTE_save_semctl ( cip->ci_peer_semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
    }
    if ( rc == UNDEF )
    {
	    DBG1 (( MF__,"semctl (setval %d) error: %s \n", cip->ci_peer_semid , sqlerrs() ))
	    /*
	     *  If the kernel is too busy to check it's mail, ignore it.
	     */
	    if ( errno != ERANGE )
	    {
	        en42FillErrText(pErrText, "connection broken:semctl (%d:%s)", errno, sqlerrs() ) ;
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "semctl (setval %d) error: %s",
					        cip->ci_peer_semid , sqlerrs() ))
	        return ( commErrNotOk_esp01 );
	    }
    }

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

int     sql33_replyavailable ( 
connection_info *cip ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql33_replyavailable"
    int				rc ;
    rte_header			*header ;
    comseg_header		*comseg ;
    struct sembuf		semrcv ;


    DBGIN;
    comseg = cip->ci_comseg ;

    /*
     *  We don't lock the comseg here.
     */
    DBG1 (( MF__,"client-pid %ld ref %d state %d \n",
			(long) comseg->cs_client_pid , comseg->cs_client_ref ,
			comseg->cs_client_state ))
    DBG1 (( MF__,"client-flag %d semid %d \n",
			comseg->cs_client_flag , comseg->cs_client_semid ))
    DBG1 (( MF__,"server-pid %ld ref %d state %d \n",
			(long) comseg->cs_server_pid , comseg->cs_server_ref ,
			comseg->cs_server_state ))
    DBG1 (( MF__,"server-flag %d semid %d \n",
			comseg->cs_server_flag , comseg->cs_server_semid ))

    if (    (comseg->cs_client_pid   != cip->ci_my_pid)
	 || (comseg->cs_client_ref   != cip->ci_my_ref)
         || (comseg->cs_server_pid   != cip->ci_peer_pid )
	 || (comseg->cs_server_ref   != cip->ci_peer_ref )
	 || (comseg->cs_server_state != commErrOk_esp01)
	 || (comseg->cs_server_flag  == 1)
       )
    {
        return ( commErrOk_esp01 );
    }

    if (1 < cip->kernelWatchDogPID && RTE_UNDEF_OSPID != cip->kernelWatchDogPID)
    {
         rc = RTE_save_kill (cip->kernelWatchDogPID, 0) ;
         if ( rc != 0 && errno == ESRCH )
         {   // Watch dog has died
            if (1 < cip->kernelPID && RTE_UNDEF_OSPID != cip->kernelPID)
            {
                 rc = RTE_save_kill (cip->kernelPID, 0) ;
                 if ( rc != 0 && errno == ESRCH )
                 {   //Kernel has also died. This means:
                     //kernel has terminated without releasing the connection correctly
                     cip->kernelDied = true;
                     return commErrCrash_esp01;
                  }
             }
         }
    }

    en42FillErrText(pErrText, "no reply available" );
    DBG1 (( MF__,"returning %d \n", SP1CE_START_REQUIRED ))
    return ( SP1CE_WOULDBLOCK );
}

/*
 * ===========================================================================
 */

/*
 * ===========================================================================
 */

int     en33CheckedReceive ( 
int (*checkCallback)(void *arg),
void *arg,
connection_info *cip ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"en33CheckedReceive"
    int				rc ;
    rte_header			*header ;
    comseg_header		*comseg ;
    struct sembuf		semrcv ;

    DBGIN;

    if (true == cip->kernelDied )
    {
        MSGD (( ERR_CONN_BROKEN ));
        en42FillErrText(pErrText, "connection broken" );
        return (commErrCrash_esp01);
    }

    /*
     *  wait on own semaphore until received
     */
restart_after_interrupt:
    {
        struct sembuf ownrcv;
        ownrcv.sem_num = 0;
        ownrcv.sem_op = -1;
        ownrcv.sem_flg = 0;
        DBG1 (( MF__,"wait  semid  %d \n", cip->ci_my_semid ))
        rc = semop ( cip->ci_my_semid , &ownrcv , 1 );
    }
    DBG1 (( MF__,"awoke semid  %d rc %d \n", cip->ci_my_semid , rc ))
    if ( (rc == UNDEF) && (errno != EIDRM) && (errno != EINVAL) )
    {
	    if ( errno == EINTR )
	    {
            if ( (0 != checkCallback) && (0 != (*checkCallback)(arg)) )
            {
     	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "semop (receive %d) interrupted and check reports error",
						    cip->ci_my_semid ))
	            en42FillErrText(pErrText, "connection broken check reports error" );
	            return ( commErrNotOk_esp01 );
            }
	        goto restart_after_interrupt ;
	    }

        en42FillErrText(pErrText, "connection broken semop (%d:%s)", errno, sqlerrs() );
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "checked receive sem %d : %s",
						cip->ci_my_semid , &pErrText[0] ))
	    return ( commErrNotOk_esp01 );
    }

    comseg = cip->ci_comseg ;

    sql32_lock_comseg ( cip , "sql33_receive: 0x%08lx \n" );

    DBG1 (( MF__,"client-pid %ld ref %d state %d \n",
			(long) comseg->cs_client_pid , comseg->cs_client_ref ,
			comseg->cs_client_state ))
    DBG1 (( MF__,"client-flag %d semid %d \n",
			comseg->cs_client_flag , comseg->cs_client_semid ))
    DBG1 (( MF__,"server-pid %ld ref %d state %d \n",
			(long) comseg->cs_server_pid , comseg->cs_server_ref ,
			comseg->cs_server_state ))
    DBG1 (( MF__,"server-flag %d semid %d \n",
			comseg->cs_server_flag , comseg->cs_server_semid ))

    /*
     *  BIG_COMSEG: the comseg might be given to a new session before
     *  the application was scheduled by UNIX.
     *  In this case, cs_client_pid and/or cs_client_ref were changed.
     */
    if (    (comseg->cs_client_pid != cip->ci_my_pid)
	 || (comseg->cs_client_ref != cip->ci_my_ref) )
    {
    	sql32_unlock_comseg ( cip );
    	en42FillErrText(pErrText,
                        "command timeout" );
    	MSGD (( -11987, ERR_TYPE, "COMMUNIC", "session re-used, command timeout?" ))
	    return ( SP1CE_TIMEOUT );
    }

    /*
     *  If the kernel releases (either asynchronously via Command Timeout
     *  or Console Kill or synchronously via Commit/Rollback Release command,
     *  it clears cs_server_pid and cs_server_ref,
     *  updates cs_server_flag and sets the semaphore.
     *  If sql33_receive awakes and cs_server_pid, cs_server_ref
     *  and cs_server_flag are ok, deliver the last reply.
     *  A later sqlrequest (if any) will then find the connection broken.
     */
    if ( (comseg->cs_server_pid   == 0)
	  && (comseg->cs_server_ref   == 0)
	  && (comseg->cs_server_state == commErrOk_esp01)
	  && (comseg->cs_server_flag  == 1)
       )
    {
	    /*
	     *  It must be a reply fo a release command. Deliver it.
	     */
	    DBG1 (( MF__,"assuming reply for release command \n" ))
    }
    else
    {
	    if ( comseg->cs_server_state != commErrOk_esp01 )
	    {
	        rc = comseg->cs_server_state ;
	        sql32_unlock_comseg ( cip );
	        en42FillErrText( pErrText,
                             "connection broken server state %d",
                             comseg->cs_server_state);
	        if ( (rc != SP1CE_SHUTDOWN)
		      && (rc != SP1CE_TIMEOUT) )
	        {
		        switch ( rc )
		        {
		        case SP1CE_RELEASED :
		            MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel released connection!" ))
		            break ;
		        case SP1CE_CRASH :
		            MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel aborted connection!" ))
		            break ;
		        default :
		            MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel broke connection!" ))
		            MSGD (( -11987, ERR_TYPE, "COMMUNIC", " (server-state %d)", rc ))
		        }
	        }
	        return ( rc );
	    }

        if (    (comseg->cs_server_pid   != cip->ci_peer_pid)
	         || (comseg->cs_server_ref   != cip->ci_peer_ref) )
	    {
	        en42FillErrText(pErrText, "connection broken pid or ref differ");
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "kernel broke connection!" ))
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "  (server-pid %5ld ref %3d",
		        (long) comseg->cs_server_pid , comseg->cs_server_ref ))
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "   remembered %5ld     %3d)",
		        (long) cip->ci_peer_pid , cip->ci_peer_ref ))
	        sql32_unlock_comseg ( cip );
	        return ( commErrNotOk_esp01 );
	    }

        if ( rc == UNDEF )
	    {
	        en42FillErrText(pErrText, "connection broken semid %d disappeared", cip->ci_my_semid );
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "semid %d disappeared!",
						        cip->ci_my_semid ))
	        sql32_unlock_comseg ( cip );
	        return ( commErrNotOk_esp01 );
	    }

        if ( comseg->cs_server_flag != 1 )
	    {
	        sql32_unlock_comseg ( cip );
	        MSGD (( -11987, ERR_TYPE, "COMMUNIC", "awoke, but no message found" ))
	        goto restart_after_interrupt ;
	    }
	    /*
	     *  Everything is ok with the connection.
	     */
    }

    if (cip->ci_request_lgt & 7)
    {
        cip->ci_request_lgt += 8 - (cip->ci_request_lgt & 7);
    }

   if (0 != cip->ci_min_reply_size )
   {
        header = (rte_header*) ((char*) (cip->ci_compack + 1) + cip->ci_request_lgt);
   }
   else
   {
        header = cip->ci_compack;
   }

    
    DBG1 (( MF__,"comseg 0x%08lx cmpack 0x%08lx \n",
	    (long) comseg , (long) cip->ci_compack ))
    DBG1 (( MF__,"shmreq 0x%08lx shmrpl 0x%08lx \n",
	    (long) cip->ci_compack , (long) header ))
    DBG1 (( MF__,"reques 0x%08lx reply  0x%08lx \n",
	    (long) cip->ci_request , (long) cip->ci_reply ))
    DBG1 (( MF__,"rpl_siz %ld recv_len %d \n",
	    cip->ci_reply_size , header->rh_max_send_len ))

    if (    (header->rh_max_send_len < RTE_HEADER_SIZE_EO003)
	 || (header->rh_max_send_len > cip->ci_reply_size + RTE_HEADER_SIZE_EO003) )
    {
	    sql32_unlock_comseg ( cip );
	    en42FillErrText(pErrText, "protocol error: header length %d", header->rh_max_send_len);
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "illegal packet size %d max %ld",
	        header->rh_max_send_len - RTE_HEADER_SIZE_EO003 , cip->ci_reply_size ))
	    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "  pktsiz %ld datsiz %ld reqlen %ld",
	        cip->ci_packet_size , cip->ci_max_data_size , cip->ci_request_lgt ))
	    return ( commErrNotOk_esp01 );
    }
    SAPDB_memcpy ( cip->ci_reply , header , header->rh_max_send_len );

    /*
     *  cs_client_flag and cs_server_flag represent four states
     *  of the communication. See ven00cc for a picture of these.
     *  Setting the flags here indicates the last state
     *  transition and shows that we got the packet.
     */
    comseg->cs_client_flag = 0 ;
    comseg->cs_server_flag = 2 ;
    sql32_unlock_comseg ( cip );

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

int     sql33_receive ( 
connection_info *cip ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql33_receive"
    int				rc ;
    struct sembuf		semrcv ;

    DBGPAS;
    return en33CheckedReceive(0, 0, cip, pErrText);
}

/*
 * ===========================================================================
 */

int     sql33_cancel ( 
connection_info *cip ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql33_cancel"
    int				rc ;
    int				kernel_fd ;
    uid_t			uid ;
    rte_header			*header ;
    char			packet [ RTE_CONPKT_SIZE ];
    tsp00_DbNamec   peer_dbname;  
    int             idx;


    DBGIN;

    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "cancel local session %d, knlref %d",
			cip->ci_my_ref , cip->ci_peer_ref ))


    strcpy (peer_dbname, cip->ci_peer_dbname);

    rc = sql32_open_kernel_fifo (peer_dbname , &kernel_fd ,
                                  &uid , pErrText);
    if ( rc != commErrOk_esp01 )
    {
        for (idx = 0; cip->ci_peer_dbname[idx] != 0; ++idx)
        {
            if (islower (cip->ci_peer_dbname[idx]))
            {
                peer_dbname[idx] = toupper (cip->ci_peer_dbname[idx]);
            }
        }

        rc = sql32_open_kernel_fifo ( peer_dbname , &kernel_fd ,
                                      &uid , pErrText );
        if ( rc != commErrOk_esp01 )
        {
	        DBG1 (( MF__,"cannot open knl fifo: %s\n", pErrText ))
	        return ( rc );
        }
    }

    header = (rte_header*) packet ;
    sql42_create_conpkt ( header , RSQL_USER_CANCEL_REQUEST_EO003 ,
			    cip->ci_my_ref , cip->ci_peer_ref ,
			    commErrOk_esp01 , cip->ci_service ,
			    cip->ci_max_segment_size ,
			    cip->ci_max_data_size ,
			    cip->ci_packet_size ,
			    cip->ci_min_reply_size ,
			    "" , cip->ci_peer_dbname );

    rc = sql42_send_conpkt ( kernel_fd , header , pErrText );
    if ( rc != commErrOk_esp01 )
    {
	DBG1 (( MF__,"cannot write to kernel fifo: %s \n", pErrText ))
	(void) close ( kernel_fd );
	return ( rc );
    }
    (void) close ( kernel_fd );

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

int     sql33_dump   ( 
connection_info *cip ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql33_dump"
    int				rc ;
    int				kernel_fd ;
    uid_t			uid ;
    rte_header			*header ;
    char			packet [ RTE_CONPKT_SIZE ];
    tsp00_DbNamec   peer_dbname;  
    int             idx;


    DBGIN;

    MSGD (( -11987, ERR_TYPE, "COMMUNIC", "dumping local server '%s'", cip->ci_peer_dbname ))

    strcpy (peer_dbname, cip->ci_peer_dbname);

    rc = sql32_open_kernel_fifo (peer_dbname , &kernel_fd ,
                                  &uid , pErrText);
    if ( rc != commErrOk_esp01 )
    {
        for (idx = 0; cip->ci_peer_dbname[idx] != 0; ++idx)
        {
            if (islower (cip->ci_peer_dbname[idx]))
            {
                peer_dbname[idx] = toupper (cip->ci_peer_dbname[idx]);
            }
        }

        rc = sql32_open_kernel_fifo ( peer_dbname , &kernel_fd ,
                                      &uid , pErrText );
        if ( rc != commErrOk_esp01 )
        {
	        DBG1 (( MF__,"cannot open knl fifo: %s\n", pErrText ))
	        return ( rc );
        }
    }

    header = (rte_header*) packet ;
    sql42_create_conpkt ( header , RSQL_DUMP_REQUEST_EO003 ,
			    cip->ci_my_ref , cip->ci_peer_ref ,
			    commErrOk_esp01 , cip->ci_service ,
			    cip->ci_max_segment_size ,
			    cip->ci_max_data_size ,
			    cip->ci_packet_size ,
			    cip->ci_min_reply_size ,
			    "" , cip->ci_peer_dbname );

    rc = sql42_send_conpkt ( kernel_fd , header , pErrText );
    if ( rc != commErrOk_esp01 )
    {
	DBG1 (( MF__,"cannot write to kernel fifo: %s \n", pErrText ))
	(void) close ( kernel_fd );
	return ( rc );
    }
    (void) close ( kernel_fd );

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

/*ARGSUSED*/	/* pErrText unused */

int     sql33_clear ( 
connection_info		*cip ,
tsp00_ErrTextc      pErrText ,
connection_info		conn_info [ ],
int                 count)
{
#undef	MF__
#define	MF__	"sql33_clear"

    DBGIN;

    if ( cip->ci_protocol == PROT_BIGSHM_EO003 )
    {
	sql33_detach_big_comseg ( cip , conn_info, count );
	cip->ci_shmid = 0 ;
    }
    else
    {
	sql33_detach_shm ( cip );
    }

    FREE_MEM_EO57( cip->ci_packet_buffer );
    cip->ci_packet_buffer = 0 ;

    DBG1 (( MF__,"returning %d \n", commErrOk_esp01 ))
    return ( commErrOk_esp01 );
}

/*
 * ===========================================================================
 */

void    sql33_cleanup ( 
connection_info		*cip )
{
#undef	MF__
#define	MF__	"sql33_cleanup"

    sql33_detach_shm ( cip );
    sql33_remove_shm ( cip );
    sql33_remove_sem ( cip );
}

/*
 * ===========================================================================
 */

static  void    sql33_detach_shm ( 
connection_info		*cip )
{
#undef	MF__
#define	MF__	"sql33_detach_shm"

    if ( cip->ci_protocol == PROT_SHM_EO003 )
    {
	DBG1 (( MF__,"detach shmaddr 0x%08lx \n",
				    (long) cip->ci_comseg ))
	(void) sql41_detach_shm ( (char**) &cip->ci_comseg );
    }
}

/*
 * ===========================================================================
 */

static  void    sql33_remove_shm ( 
connection_info		*cip )
{
#undef	MF__
#define	MF__	"sql33_remove_shm"

    if ( cip->ci_protocol == PROT_SHM_EO003 )
    {
		SAPDB_UInt4		idx;
		tsp00_DbNamec   peer_dbname;  

		DBG1 (( MF__,"remove shmid %d\n", cip->ci_shmid ))

		strcpy (peer_dbname, cip->ci_peer_dbname);

		for (idx = 0; cip->ci_peer_dbname[idx] != 0; ++idx)
		{
			if (islower (cip->ci_peer_dbname[idx]))
			{
				peer_dbname[idx] = toupper (cip->ci_peer_dbname[idx]);
			}
		}

		(void) sql41_remove_shm ( &cip->ci_shmid , "us" , peer_dbname );
    }
}

/*
 * ===========================================================================
 */

static  void    sql33_remove_sem ( 
connection_info		*cip )
{
#undef	MF__
#define	MF__	"sql33_remove_sem"
	tsp00_DbNamec   peer_dbname;  
	SAPDB_UInt4		idx;

	DBG1 (( MF__,"remove semid %d \n", cip->ci_my_semid ))
	strcpy (peer_dbname, cip->ci_peer_dbname);

	for (idx = 0; cip->ci_peer_dbname[idx] != 0; ++idx)
	{
		if (islower (cip->ci_peer_dbname[idx]))
		{
			peer_dbname[idx] = toupper (cip->ci_peer_dbname[idx]);
		}
	}

    (void) sql41_remove_sem ( &cip->ci_my_semid , "us" , peer_dbname );
}

/*==========================================================================*/

static	int	sql33_attach_big_comseg ( 
connection_info		*cip ,
connection_info		conn_info [ ],
int                 count)
{
#undef	MF__
#define	MF__	"sql33_attach_big"
    int				idx ;
    connection_info		*scip ;


    DBG1 (( MF__,"search shmid %d conn 0x%08lx \n",
			cip->ci_shmid , (long) conn_info ))

    cip->ci_big_comseg = 0 ;
    cip->ci_big_size   = 0 ;
    if ( conn_info )
    {
	for ( idx = 0 ; idx < count ; idx ++ )
	{
	    scip = conn_info + idx ;
	    if (    scip->ci_big_comseg
		 && (scip->ci_shmid == cip->ci_shmid) )
	    {
		DBG1 (( MF__,"found shmaddr 0x%08lx (ref %d) \n",
					(long) scip->ci_big_comseg , idx+1 ))
		cip->ci_big_comseg = scip->ci_big_comseg ;
		cip->ci_big_size   = scip->ci_big_size ;
		break ;
	    }
	}
    }

    idx = sql32_attach_comseg ( cip );
    return ( idx );
}

/*==========================================================================*/

static	void	sql33_detach_big_comseg ( 
connection_info		*cip ,
connection_info		conn_info [ ],
int                 count)
{
#undef	MF__
#define	MF__	"sql33_detach_big"
    int				to_detach ;
    int				idx ;
    connection_info		*scip ;


    DBG1 (( MF__,"called  shmaddr 0x%08lx \n",
		(long) cip->ci_big_comseg ))

    to_detach = TRUE ;
    if ( conn_info )
    {
	for ( idx = 0 ; idx < count ; idx ++ )
	{
	    scip = conn_info + idx ;
	    if ( (scip != cip) &&
		 (scip->ci_shmid == cip->ci_shmid) &&
	         (scip->ci_big_comseg == cip->ci_big_comseg) )
	    {
		DBG1 (( MF__,"reference %d using comseg \n", idx ))
		to_detach = FALSE ;
		break ;
	    }
	}
    }

    if ( to_detach )
    {
        (void) sql41_detach_shm ( (char**) &cip->ci_big_comseg );
	cip->ci_big_size = 0 ;
    }

    DBG1 (( MF__,"leaving shmaddr 0x%08lx \n",
			    (long) cip->ci_big_comseg ))
}

/*==========================================================================*/

static SAPDB_Bool  sql33_GetKernelPidAndPPid (SAPDB_Char *     szServerDB, 
                                               RTE_OSPid *     pKernelWatchDogPid,
                                               RTE_OSPid *     pKernelPid,
                                               tsp00_ErrTextc  pErrText )
{
    if ( ((en41GetPPIDFromFile(szServerDB, pKernelWatchDogPid) != 0) || (*pKernelWatchDogPid == 0)) 
          || ((en41GetPIDFromFile(szServerDB, pKernelPid) != 0) || (*pKernelPid == 0)))
    {   /* while kernel processes did not write its pid file, state is OFFLINE */
        en42FillErrText(pErrText, "cannot get kernel pid and parent pid");
        return false;
    }
   
    return true;
}
