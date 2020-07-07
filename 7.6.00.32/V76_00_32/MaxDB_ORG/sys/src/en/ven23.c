/*!
  @file           ven23.c
  @author         JoergM
  @brief          RunTime: socket_communication_functions
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

#define MOD__ "ven23.c:"

#include <poll.h>

#include "gen00.h"
#include "vsp001c.h"
#include "gsp09.h"
/* exported functions */

#include "gen23.h"

/* imported functions */

#include "gen42.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"
#include "gsp01.h"
#include "hsp77.h"
#include "RunTime/RTE_saveUNIXcalls.h"

/* local functions */


/* imported data */


/* local data */

#ifdef  SOCKETS
/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sql23_connect"
int     sql23_connect ( 
connection_info		*cip ,
tsp00_ErrTextc       pErrText )
{
    int				rc ;
    int				idx ;
    int				messclass ;
    int				service ;
    long			maxsegsiz ;
    int				retcod ;
    tsp00_Int4			myref ;
    long			siz ;
    long			off ;
    char			*ptr ;
    rte_header			*header ;
    SQL_DBNAMEC                 mydbname ;
    SQL_DBNAMEC                 peerdb ;
    char			packet [ RTE_CONPKT_SIZE ];
    char			pidbuf [ 32 ];
    int				request_type;
    int				reply_type;


    DBG1 (( MF__,"nodeid '%s' dbname '%s' service %d \n",
		cip->ci_peer_node , cip->ci_peer_dbname , cip->ci_service ))

    /*
     *  Initialize the socket descriptor to an impossible value.
     *  Else 'sql23_release' would close a random file descriptor
     *  (e.g. 0,stdin).
     */
    cip->ci_sd = -1 ;

    /*
     *  Get address of server socket.
     */
    rc = sql42_get_server_address ( &cip->ci_peer_sock_addr ,
				    (char *)cip->ci_peer_node , pErrText );
    if ( rc )
    {
	DBG1 (( MF__,"leaving %d \n", rc ))
	return ( rc );
    }

    if (cip->ci_service == srvControl_esp01) {
        /* packet sizes are fixed, no info request necessary */
        cip->ci_packet_size = csp9_ctrl_packet_size;
        cip->ci_min_reply_size = 112;
        cip->ci_max_data_size = cip->ci_packet_size - 
            (2 * sizeof (rte_header));
    }
    else {
	/*
	 *  Do an info request for the requested session.
	 *  If the address server is not available, or cannot deliver
	 *  the wanted address, the address remains unchanged.
	 *  In this case it will be reused for the normal connect.
	 */
	DBG1 (( MF__,"doing info request \n" ))
	rc = sql42_info_request ( cip->ci_peer_dbname , cip->ci_service ,
					&cip->ci_max_data_size ,
					&cip->ci_packet_size ,
					&cip->ci_min_reply_size ,
					&cip->ci_peer_sock_addr , pErrText );
	DBG1 (( MF__,"info request returned %d \n", rc ))
	if ( rc != SP1CE_OK )
	{
	    if ( rc != SP1CE_START_REQUIRED )
		DBG1 (( MF__,"info request: %s \n", pErrText ))
	    return ( rc );
	}
    }
    DBG1 (( MF__,"now doing a normal connect \n" ))

    /*
     *  Now connect to the possibly changed address.
     */
    rc = sql42_connect_server ( &cip->ci_peer_sock_addr , &cip->ci_sd , pErrText );
    if ( rc )
    {
	DBG1 (( MF__, "con_srv: %s \n", pErrText ))
	return ( rc );
    }

    /*
     *  We got the packet size from the info request.
     *  Now, we can use this size for the new connection.
     */
    cip->ci_max_segment_size = cip->ci_max_data_size ;
    rc = sql42_socket_buffer ( cip->ci_sd , &cip->ci_max_segment_size );
    if ( rc != SP1CE_OK )
    {
    	en42FillErrText( pErrText, "socket problems" );
	    return ( rc );
    }

    /*
     *  send connect packet
     */
    header = (rte_header*) packet ;
    if (cip->ci_service == srvControl_esp01)
    {
        request_type = RSQL_CTRL_CONN_REQUEST_EO003;
	reply_type   = RSQL_CTRL_CONN_REPLY_EO003;
    }
    else
    {
        request_type = RSQL_USER_CONN_REQUEST_EO003;
        reply_type    = RSQL_USER_CONN_REPLY_EO003;
    }
    sql42_create_conpkt ( header , request_type ,
			    cip->ci_my_ref , (tsp00_Int4)0 ,
			    SP1CE_OK , cip->ci_service ,
			    cip->ci_max_segment_size ,
			    cip->ci_max_data_size ,
			    cip->ci_packet_size ,
			    cip->ci_min_reply_size ,
			    "" , cip->ci_peer_dbname );
    sp77sprintf ( pidbuf , sizeof(pidbuf), "%ld" , (long) getpid() );
    sql42_put_string ( header , RSQL_OPT_REM_PID_EO003 , pidbuf );
    if (cip->ci_service == srvControl_esp01) {
        if ((cip->ci_serverpgm != NULL) && (cip->ci_serverpgm [0] != '\0')) {
            sql42_put_string ( header , RSQL_OPT_SERVERPGM_EO003 , cip->ci_serverpgm );
        }
        if ((cip->ci_dbroot != NULL) && (cip->ci_dbroot [0] != '\0')) {
            sql42_put_string ( header , RSQL_OPT_DBROOT_EO003 , cip->ci_dbroot );
        }
    }
    rc = sql42_send_conpkt ( cip->ci_sd , header , pErrText );
    if ( rc )
    {
	    (void) close ( cip->ci_sd );
	    cip->ci_sd = -1 ;
	    DBG1 (( MF__, "%s \n", pErrText ))
	    return ( rc );
    }

    /*
     *  receive reply from server
     */
    rc = sql42_recv_conpkt ( cip->ci_sd , header , &cip->ci_state , pErrText );
    if ( rc )
    {
	    DBG1 (( MF__,"%s \n", pErrText ))
	    (void) close ( cip->ci_sd );
	    cip->ci_sd = -1 ;
	    return ( rc );
    }
    sql42_extract_conpkt ( header , &messclass ,
			    &myref , &cip->ci_peer_ref ,
			    &retcod , &service , &maxsegsiz ,
			    &cip->ci_max_data_size ,
			    &cip->ci_packet_size ,
			    &cip->ci_min_reply_size ,
			    &cip->ci_peer_swap_type ,
			    (char *)mydbname , (char *)peerdb );

    /*
     *  Check the received information.
     */
    rc = SP1CE_OK ;
    if ( messclass != reply_type )
    {
    	DBG1 (( MF__,"server messclass %d expected %d \n",
					messclass , RSQL_USER_CONN_REPLY_EO003 ))
	    en42FillErrText( pErrText, "protocol error:reply_type %d", messclass );
#if ACCEPT_KERN_CONN_REPLY
	    if ( messclass != RSQL_KERN_CONN_REPLY_EO003 )
#endif
    	    rc = SP1CE_NOTOK ;
    }
    if ( myref != cip->ci_my_ref )
    {
	    DBG1 (( MF__,"server reference %d expected %d \n",
						        myref , cip->ci_my_ref ))
	    en42FillErrText( pErrText, "protocol error:ref %d/%d", myref, cip->ci_my_ref) ;
	    rc = SP1CE_NOTOK ;
    }
    if ( service != cip->ci_service )
    {
	    DBG1 (( MF__,"server service %d expected %d \n",
						    service , cip->ci_service ))
	    en42FillErrText( pErrText, "protocol error:service %d/%d", service, cip->ci_service) ;
	    rc = SP1CE_NOTOK ;
    }
    if ( strcmp ( (char *)peerdb , (char *)cip->ci_peer_dbname ) )
    {
	    DBG1 (( MF__, "server dbname '%s' expected '%s' \n",
						peerdb , cip->ci_peer_dbname ))
	    en42FillErrText( pErrText, "protocol error:dbname %s/%s", (char *)peerdb, (char *)cip->ci_peer_dbname) ;
	    rc = SP1CE_NOTOK ;
    }

    if ( retcod != SP1CE_OK )
    {
	    if ( rc == SP1CE_OK )
	    {
	        switch ( retcod )
	        {
	        case SP1CE_NOTOK :
                en42FillErrText( pErrText, "protocol error");
		        break ;
	        case SP1CE_TIMEOUT :
                en42FillErrText( pErrText, "protocol error:timeout");
		        break ;
	        case commErrServerOrDBUnknown_esp01 :
		        en42FillErrText( pErrText, "database or server not found" );
		        break ;
	        case SP1CE_TASKLIMIT :
		        en42FillErrText( pErrText, "task limit" );
		        break ;
	        case SP1CE_RELEASED :
		        en42FillErrText( pErrText, "server rejected connection" );
		        break ;
	        default :
		        en42FillErrText( pErrText, "database not running" );
		        break ;
	        }
	        rc = (int) retcod ;
	        DBG1 (( -11987, "sql23_connect: server returned %d \n", rc ))
	    }
    }
    /* end if retcod != SP1CE_OK */

    if ( rc == SP1CE_OK )
    {
        sql42_get_string ( header , RSQL_OPT_AUTH_ALLOW_EO003, 
                           sizeof(cip->ci_authentication_allow), 
                           cip->ci_authentication_allow );

	    if (    (maxsegsiz <= RTE_HEADER_SIZE)
	         || (maxsegsiz > cip->ci_max_segment_size) )
	    {
	        DBG1 (( MF__,"got incorrect maxsegsiz %ld \n",
								    maxsegsiz ))
		    en42FillErrText( pErrText, "protocol error:maxsegsiz %d", maxsegsiz );
	        rc = SP1CE_NOTOK ;
	    }
	    if ( maxsegsiz < cip->ci_max_segment_size )
	    {
	        /*
	         *  Reduction of the proposed segment size
	         *  is a valid protocol element.
	         */
	        DBG1 (( -11987, "sql23_connect: max_segment_size %ld -> %ld \n",
					    cip->ci_max_segment_size , maxsegsiz ))
	        cip->ci_max_segment_size = maxsegsiz ;
	    }
    }
    /* end if rc == SP1CE_OK */

    /*
     *  On any protocol error close connection.
     */
    if ( rc != SP1CE_OK )
    {
	    (void) close ( cip->ci_sd );
	    cip->ci_sd = -1 ;
    }
    else
    {
	    /*
	     *  Allocate a packet buffer for the application,
	     *  and a cache, which is required, if the peer
	     *  sends packet segments (see sql42_rcvpkt for mor information).
	     *  Add 8 bytes for alignment.
	     */
	    siz = (cip->ci_packet_cnt + 1) * cip->ci_packet_size ;
	    if ( ALLOC_MEM_EO57 ( (void **)&ptr, siz + 8 ) != NO_ERROR_EO001 )
	    {
	        DBG1 (( MF__, "insufficient memory \n" ))
	        sql42_send_control ( cip->ci_sd , RSQL_USER_RELEASE_REQUEST_EO003 ,
				    cip->ci_my_ref , cip->ci_peer_ref , SP1CE_OK );
	        (void) close ( cip->ci_sd );
	        cip->ci_sd = -1 ;
	        rc= SP1CE_NOTOK ;
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
	        cip->ci_cache_buf = ptr ;
	        cip->ci_cache_lgt = 0 ;
	    }
    }

    DBG1 (( MF__,"leaving %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sql23_release"
/*ARGSUSED*/
int     sql23_release ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    sql42_send_control ( cip->ci_sd , RSQL_USER_RELEASE_REQUEST_EO003 ,
			cip->ci_my_ref , cip->ci_peer_ref , SP1CE_OK );
    DBG1 (( MF__,"close %d \n", cip->ci_sd ))
    (void) close ( cip->ci_sd );
    cip->ci_sd = -1 ;
    if ( cip->ci_packet_buffer ) FREE_MEM_EO57 ( cip->ci_packet_buffer );
    cip->ci_packet_buffer = 0 ;
    cip->ci_cache_buf = 0 ;
    cip->ci_cache_lgt = 0 ;
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sql23_request"
int     sql23_request ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    int                             rc ;

    DBGIN;

    rc = sql42_send_packet ( cip->ci_sd ,
                             cip->ci_request ,
                	         cip->ci_max_segment_size ,
                             pErrText );
    if ( rc == SP1CE_RELEASED )
    {
	    rc = sql42_recv_packet ( cip->ci_sd , cip->ci_reply ,
	 			                 cip->ci_peer_swap_type ,
				                 cip->ci_reply_size + RTE_HEADER_SIZE ,
				                 pErrText ,
				                 cip->ci_cache_buf ,
				                 &cip->ci_cache_lgt );
	    if ( rc == SP1CE_OK )
	    {
	        /*
	         *  The valid packet might contain a release indication
	         *  with a reason. Return this reason.
	         */
	        rc = cip->ci_reply->rh_rte_return_code ;
	    }
    }

    if ( rc != SP1CE_OK )
    {
    	DBG1 (( MF__,"%s \n", pErrText ))
    }

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ "sql23_replayavailable"
int     sql23_replyavailable ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    int				rc ;
    struct pollfd   replyFd[1];    


    DBGIN;

    SAPDB_memset( replyFd, 0, sizeof(struct pollfd) );
    replyFd[0].fd = cip->ci_sd;
    replyFd[0].events = POLLIN;

    rc = poll( replyFd, 1, 0);
    if ( rc > 0 )
    {
        rc = SP1CE_OK ;
    }
    else if ( rc == 0 )
	{
	    en42FillErrText(pErrText, "no reply available" );
	    rc = SP1CE_WOULDBLOCK ;
	}
    else if ( errno == EINTR )
    {
	    en42FillErrText(pErrText, "no reply available: EINTR" );
	    rc = SP1CE_WOULDBLOCK ;
	}
	else
	{
	    en42FillErrText(pErrText, "illegal arguments (poll:%s)", sqlerrs() );
	    rc = SP1CE_NOTOK ;
	}

    DBG1 (( MF__,"returning %d \n", rc ));
    return ( rc );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ "sql23_receive"
int     sql23_receive ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    register int                    rc ;


    DBGIN;

    rc = sql42_recv_packet ( cip->ci_sd , cip->ci_reply ,
           	                 cip->ci_peer_swap_type ,
 				             cip->ci_reply_size + RTE_HEADER_SIZE ,
				             pErrText,
				             cip->ci_cache_buf ,
				            &cip->ci_cache_lgt );
    if ( rc != SP1CE_OK )
    {
    	DBG1 (( MF__, "%s \n", pErrText ))
    }
    else
    {
	    /*
	     *  The valid packet might contain a release indication
	     *  with a reason. Return this reason.
	     */
	    rc = cip->ci_reply->rh_rte_return_code ;
    }

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ "sql23_cancel"
/*ARGSUSED*/
int     sql23_cancel ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    int				rc ;
    rte_header			*header ;
    connection_info		*tmp_cip ;
    connection_info		tmpconninfo ;
    char			packet [ RTE_CONPKT_SIZE ];


    DBG1 (( MF__, "cancel remote session %d, knlref %d \n",
			    cip->ci_my_ref , cip->ci_peer_ref ))

    tmp_cip = &tmpconninfo ;
    SAPDB_memcpy ( tmp_cip , cip , sizeof(*tmp_cip) );

    /*
     *  Initialize socket descriptor to an impossible value.
     *  Else 'sql23_cancel' would close a random file descriptor (e.g. 0,stdin).
     */
    tmp_cip->ci_sd = -1 ;

    /*
     *  connect to server socket
     */
    rc = sql42_connect_server ( &tmp_cip->ci_peer_sock_addr ,
				&tmp_cip->ci_sd , pErrText );
    if ( rc )
    {
	return ( rc );
    }

    /*
     *  send cancel request
     */
    header = (rte_header*) packet ;
    sql42_create_conpkt ( header , RSQL_USER_CANCEL_REQUEST_EO003 ,
			    tmp_cip->ci_my_ref , tmp_cip->ci_peer_ref ,
			    SP1CE_OK , tmp_cip->ci_service ,
			    tmp_cip->ci_max_segment_size ,
			    tmp_cip->ci_max_data_size ,
			    tmp_cip->ci_packet_size ,
			    tmp_cip->ci_min_reply_size ,
			    "" , tmp_cip->ci_peer_dbname );
    rc = sql42_send_conpkt ( tmp_cip->ci_sd , header , pErrText );
    if ( rc )
    {
	DBG1 (( MF__, "sql23_cancel: %s \n", pErrText ))
    }

    /*
     *  We don't expect an answer to a cancel request.
     */
    (void) close ( tmp_cip->ci_sd );

    return ( rc );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ "sql23_dump"
/*ARGSUSED*/
int     sql23_dump ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    int				rc ;
    rte_header			*header ;
    connection_info		*tmp_cip ;
    connection_info		tmpconninfo ;
    char			packet [ RTE_CONPKT_SIZE ];


    DBG1 (( MF__, "dumping remote server '%s:%s' \n",
	    cip->ci_peer_node , cip->ci_peer_dbname ))

    tmp_cip = &tmpconninfo ;
    SAPDB_memcpy ( tmp_cip , cip , sizeof(connection_info) );

    /*
     *  Initialize socket descriptors to an impossible value.
     *  Else 'sql23_dump' would close a random file descriptor (e.g. 0,stdin).
     */
    tmp_cip->ci_sd = -1 ;

    /*
     *  get address of server socket
     */
    rc = sql42_get_server_address ( &tmp_cip->ci_peer_sock_addr ,
				    (char *)tmp_cip->ci_peer_node , pErrText );
    if ( rc ) return ( rc );
    DBGADR (( MF__,"addr %s \n", &tmp_cip->ci_peer_sock_addr ))

    /*
     *  connect to server socket
     */
    rc = sql42_connect_server ( &tmp_cip->ci_peer_sock_addr ,
				&tmp_cip->ci_sd , pErrText );
    if ( rc )
    {
	return ( rc );
    }

    /*
     *  send dump request
     */
    header = (rte_header*) packet ;
    sql42_create_conpkt ( header , RSQL_DUMP_REQUEST_EO003 ,
			    tmp_cip->ci_my_ref , tmp_cip->ci_peer_ref ,
			    SP1CE_OK , tmp_cip->ci_service ,
			    tmp_cip->ci_max_segment_size ,
			    tmp_cip->ci_max_data_size ,
			    tmp_cip->ci_packet_size ,
			    tmp_cip->ci_min_reply_size ,
			    "" , tmp_cip->ci_peer_dbname );
    rc = sql42_send_conpkt ( tmp_cip->ci_sd , header , pErrText );
    if ( rc )
    {
	DBG1 (( MF__, "%s \n", pErrText ))
    }

    /*
     *  We don't expect an answer to a dump request.
     */
    (void) close ( tmp_cip->ci_sd );

    return ( rc );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ "sql23_clear"
/*ARGSUSED*/
int     sql23_clear ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__,"close %d \n", cip->ci_sd ))
    RTE_save_close ( cip->ci_sd );
    cip->ci_sd = -1 ;
    if ( cip->ci_packet_buffer ) FREE_MEM_EO57 ( cip->ci_packet_buffer );
    cip->ci_packet_buffer = 0 ;
    cip->ci_cache_buf = 0 ;
    cip->ci_cache_lgt = 0 ;
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

#else   /*SOCKETS*/

#undef MF__
#define MF__ "sql23_connect"
/*ARGSUSED*/
int     sql23_connect ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_release"
/*ARGSUSED*/
int     sql23_release ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_request"
/*ARGSUSED*/
int     sql23_request ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_receive"
/*ARGSUSED*/
int     sql23_receive ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_cancel"
/*ARGSUSED*/
int     sql23_cancel (
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_clear"
/*ARGSUSED*/
int     sql23_clear ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#undef MF__
#define MF__ "sql23_dump"
/*ARGSUSED*/
int     sql23_dump ( 
connection_info		*cip ,
tsp00_ErrTextc    pErrText )
{
    DBG1 (( MF__, "REMOTE SQL not implemented \n" ));
    strcpy(pErrText, "REMOTE SQL not implemented");
    return ( SP1CE_NOTOK );
}

#endif  /*SOCKETS*/
