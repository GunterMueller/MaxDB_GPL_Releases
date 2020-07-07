/*!
  @file           ven74.c
  @author         JoergM
  @brief          Kernel RunTime: Initialize Communication Shared Section
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

#define MOD__ "ven74.c:"

#if defined (OSF1) || defined (FREEBSD)
#include	<sys/param.h>
#else
#undef		NULL
#include	<sys/param.h>
#include	<sys/sysmacros.h>
#endif

#include "gen00.h"
#include "heo00.h"
#include "geo50_0.h"
#include "gen500.h"
#include "vsp001c.h"
#include "gen003.h"
#include "geo70k.h"
#include "hen40.h"
#include "hsp77.h"

/* exported functions */

unsigned long	e74_comdata_size (
                    RTE_XPARAM_REC		*XParam,
                    long			kgs_ukp_count );
void	e74_init_comdata ( RTE_XPARAM_REC		*XParam,
                           int                  max_sock_conns,
                           teo70k_ClusterInfo *    pCluInfo );
int     e74_create_big_comsegs ();
void	e74_init_big_comseg (ten50_UKT_Control *this_ukt );
void	e74_release_comseg ( connection_info *cip, struct TASK_TYPE  *tcb );
void	e74_print_all_conns ( char *hdr );
void	e74_print_connection ( char			*hdr ,
                            connection_info	*cip );

/* imported functions */

extern  int    sql41_create_shm ();
extern  char * sql41_attach_shm ();
extern  int	   sql41_detach_shm ();
extern  int    sql41_remove_shm ();
extern  int    sql41_remove_fifo ();
extern	int    e70_is_kgsshmid ();

/* local functions */

/* exported data */

/* imported data */

extern  int                             e84_connect_strategy ;

/* local data */

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"e74_comdata_size"
unsigned long	e74_comdata_size (
RTE_XPARAM_REC		*XParam,
long			kgs_ukp_count )
{
    unsigned long		tot_size ;

    tot_size = sizeof(connection_info) *
	(
	      1							/* utility */
	    + XParam->ulMaxEventTasks   /* event tasks */
	    + XParam->ulMaxUser		    /* users */
	    + kgs_ukp_count					/* UKPs */
	);

    return ( tot_size );
}

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"e74_init_comdata"
void	e74_init_comdata ( RTE_XPARAM_REC		*XParam,
                           int                  max_sock_conns,
                           teo70k_ClusterInfo *    pCluInfo )
{
    int				conn_count ;
    long			big_offset ;
    struct TASK_TYPE		*tcb ;
    ten50_UKT_Control           *kp ;
    connection_info		*cip ;

    DBGIN;
    DBG1(( MF__, "packet size                  %10ld",
			    XParam->ulPacketSize ));
    DBG1(( MF__, "max data size                %10ld",
			    XParam->ulPacketSize - sizeof(comseg_header)
			    - 8 /*alignment*/ - 2 * sizeof(rte_header) ));
    DBG1(( MF__, "min reply size               %10ld",
			    XParam->ulMinReplySize ));

    /*
     *  Initialize Tasks.
     */
    cip = KGS->connection ;
    for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl ; tcb ++ )
    {
	    tcb->connection     = 0 ;

	    switch ( tcb->type )
	    {
	    case TT_UT_EO00 :
	    case TT_US_EO00 :
	    case TT_EV_EO00 :
	        tcb->connection = cip ;
	    	cip->ci_tcb = tcb ;
		    cip ++ ;
	        break ;
	    default:
		    break;
	    }

    }

    /*
     *  Initialize UKPs.
     */
    for ( kp = KGS->first_kp ; kp <= KGS->last_kp ; kp ++ )
    {
	    kp->connection = cip ;
	    cip->ci_my_ref = THREAD_INDEX(kp) ;
	    cip ++ ;
    }
    /* Initialize UKPs */

    /*
     *  Fix global connection count.
     */
    KGS->connection_cnt = cip - KGS->connection ;

    /*
     *  Check the packet size from param.
     */
    if ( XParam->ulPacketSize == 0 )
    {
	    XParam->ulPacketSize = 64 * 1024 ;
	    XParam->ulMinReplySize = 4 * 1024 ;
	    MSGD (( WRN_INITCOM_DEF_PACKET_SZ, XParam->ulPacketSize ))
    }

    /*
     *  Get the size of each UKT's big communication segment and
     *  each task's offset in its comseg.
     */
    for ( kp = KGS->first_kp ; kp <= KGS->last_kp ; kp ++ )
    {
    	big_offset = sizeof(big_comseg_header);
	    conn_count = 0 ;
    	for ( tcb = kp->pFirstTaskCtrl ; tcb ; tcb = tcb->next_task )
	    {
            if ( tcb->connection )
            {
    		    DBG1 (( MF__,"T%d big_comseg_offset 0x%lx \n",
	    	        tcb->index , (long)big_offset ));
		        tcb->connection->ci_locbigoffset = big_offset ;
    		    big_offset += sizeof(comseg_header);
    	        ++conn_count;
            }
    	}
	    /* end for tcb */

    	/*
	     *  Offset lists to the packets.
    	 */
	    big_offset += conn_count * MAX_SQL_PACKETS * sizeof(ten_com_long);

	    /*
	     *  Space to allow 8 byte alignment
	     */
	    big_offset += 8 ;

	    /*
	     *  The packets.
	     */
	    big_offset += conn_count * XParam->ulPacketSize ;

    	if ( conn_count )
	        kp->connection->ci_big_size = big_offset ;
    	else
	        kp->connection->ci_big_size = 0 ;
    	DBG1 (( MF__,"UKT%d comseg size %ld",
	    	THREAD_INDEX(kp), kp->connection->ci_big_size ));
    }

    DBGOUT;
}

/*
 * ===========================================================================
 */
#if PA20W
#define EN74_COMSEG_MODE (0666 | IPC_SHARE32)
#else
#define EN74_COMSEG_MODE (0666)
#endif

#undef MF__
#define MF__ MOD__"e74_create_big_comsegs"
int     e74_create_big_comsegs ( )
{
    int				   shmid ;
    int				   ref ;
    ten50_UKT_Control *kp ;

	DBGIN;

    ref = 1 ;

    for ( kp = KGS->first_kp ; kp <= KGS->last_kp ; kp ++ )
	{
		if ( kp->connection->ci_big_size > 0 )
		{
			shmid = sql41_create_shm ( KGS->ipcBaseKey , ref ++ , 16 ,
						(int) kp->connection->ci_big_size ,
						EN74_COMSEG_MODE, getuid() , "db" , KGS->serverdb );
			DBG1 (( MF__," UKT%d big comseg shmid %d \n",
							THREAD_INDEX(kp), shmid ));
			if ( shmid == -1 )
			{
				MSGALL (( ERR_INITCOM_CREATE_BIGCOM , sqlerrs() )) ;
				return ( -1 );
			}
			kp->connection->ci_shmid = shmid ;
		}
		else
		{
			/* no communicating tasks in this UKT, hence no segment */
			kp->connection->ci_shmid = 0 ;
		}
	}

    DBGOUT;
    return ( 0 );
}

/*
 *============================================================================
 */
#undef MF__
#define MF__ MOD__"e74_init_big_comseg"
void	e74_init_big_comseg (ten50_UKT_Control *this_ukt )
{
    int				idx ;
    int				conn_count ;
    long			offset ;
    ten_com_long	*offsetp ;
    char			*addr ;
    big_comseg_header		*big_comseg ;
    comseg_header		*comseg ;
    struct TASK_TYPE		*tcb ;

    DBG1 (( MF__," entered shmid %d \n",
		this_ukt->connection->ci_shmid ));

    if ( this_ukt->connection->ci_shmid <= 0 )
    {
		DBG1 (( MF__,"e74_init_big: UKT%d has no big_comseg \n",
			THREAD_INDEX(this_ukt)));
		return ;
    }

    addr = sql41_attach_shm ( this_ukt->connection->ci_shmid , (char *)0 );
    if ( ! addr )
    {
		MSGD(( ERR_INITCOM_ATTACH_BIGCOM, sqlerrs() )) ;
        exit ( 2 );
    }
    big_comseg = (big_comseg_header*) addr ;

    if (    this_ukt->connection->ci_big_comseg
	 && (this_ukt->connection->ci_big_comseg != big_comseg) )
    {
		MSGD(( ERR_INITCOM_ATT_ADDR_BIGCOM,  (long) big_comseg ))
		MSGD(( ERR_INITCOM_WANT_ADDR_BIGCOM,
				(long) this_ukt->connection->ci_big_comseg ));
        exit ( 2 );
    }
    this_ukt->connection->ci_big_comseg = big_comseg ;

    MSGD  (( INFO_INITCOM_BIGCOM_ATT_AT,
				THREAD_INDEX(this_ukt), (long) addr )) ;
    MSGD  (( INFO_INITCOM_BIGCOM_ENDS_AT,
		    THREAD_INDEX(this_ukt), (long) addr +
		    (long) this_ukt->connection->ci_big_size ))
					
    /*
     *  Initialize Big Comseg Header.
     */
    big_comseg->bc_size = this_ukt->connection->ci_big_size ;
    big_comseg->bc_version = BIG_COMSEG_VERSION ;
    conn_count = 0 ;
    for ( tcb = this_ukt->pFirstTaskCtrl ; tcb ; tcb = tcb->next_task )
    {
        if ( tcb->connection )
        {
            ++conn_count;
        }
    }
    big_comseg->bc_comseg_cnt = conn_count ;
    big_comseg->bc_packet_cnt = conn_count ;

    big_comseg->bc_comseg_size = sizeof(comseg_header);
    big_comseg->bc_packet_size = XPARAM(ulPacketSize) ;

    big_comseg->bc_comseg_offset = sizeof(big_comseg_header);
    big_comseg->bc_list_offset   = big_comseg->bc_comseg_offset
				  + conn_count * sizeof(comseg_header);
    big_comseg->bc_packet_offset = big_comseg->bc_list_offset
				  + conn_count * MAX_SQL_PACKETS * sizeof(ten_com_long);
    /*
     *  Packets must be 8 byte aligned.
     */
    offset = big_comseg->bc_packet_offset & 7 ;
    if ( offset )
    {
		big_comseg->bc_packet_offset += 8 - offset ;
    }

    /*
     *  Initialize Comsegs and Offset Lists.
     */
    comseg  = (comseg_header*) (addr + big_comseg->bc_comseg_offset);
    offsetp = (ten_com_long *) (addr + big_comseg->bc_list_offset);
    offset  = big_comseg->bc_packet_offset ;
    for ( idx = 0 ; idx < conn_count ; idx ++ )
	{
		DBG1 (( MF__," comseg 0x%lx list 0x%lx pkt 0x%lx \n",
				(long) comseg , (long) offsetp , (long) addr + offset ));
		comseg->cs_size = 0 ; /* big comseg */
		comseg->cs_version = COMSEG_VERSION ;
		comseg->cs_packet_cnt = 1 ;
		comseg->cs_packet_size = XPARAM(ulPacketSize) ;
		comseg->cs_list_offset = (char*) offsetp - addr ;
		comseg->cs_current_packet = 0 ;
		comseg->cs_shmid = this_ukt->connection->ci_shmid ;

		*offsetp = offset ;

		comseg ++ ;
		offsetp += MAX_SQL_PACKETS ;
		offset  += XPARAM(ulPacketSize) ;
	}
}

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"e74_release_comseg"
void	e74_release_comseg ( connection_info		*cip,
                             struct TASK_TYPE       *tcb )
{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    DBG1 (( MF__,"e74_release_comseg: shmid %d big_comseg 0x%lx comseg 0x%lx \n",
			cip->ci_shmid , (long)cip->ci_big_comseg , (long)cip->ci_comseg ));

    /*
     *  Don't release the Kernel Global Section.
     */
    if ( e70_is_kgsshmid(cip->ci_shmid) )
    {
		DBG1(( MF__, "e74_release_comseg: don't release kgs!" ));
		return ;
    }

    /*
     *  Don't release a UKT's big comseg.
     */
    if (    (this_ukt >= KGS->first_kp)
	     && (this_ukt <= KGS->last_kp)
	     && (    (cip->ci_shmid      <= 0)
	      || (cip->ci_big_comseg == tcb->bigComSegBelongsToUKT->connection->ci_big_comseg) ) )
    {
		DBG1(( MF__, "don't release own big comseg!" ));
		return ;
    }

    if ( cip->ci_protocol == PROT_BIGSHM_EO003 )
	{
		DBG1 (( MF__,"e74_release_comseg: detaching big_comseg 0x%lx \n",
							(long)cip->ci_big_comseg ));
		(void) sql41_detach_shm ( (char**) &cip->ci_big_comseg );
		cip->ci_big_size = 0 ;
		cip->ci_big_offset = 0 ;
		cip->ci_comseg = 0 ;
		cip->ci_request = 0 ;
		cip->ci_reply = 0 ;
	}
    else
    {
		DBG1 (( MF__,"e74_release_comseg: detaching comseg 0x%lx \n",
						(long)cip->ci_comseg ));
		(void) sql41_detach_shm ( (char**) &cip->ci_comseg );
		cip->ci_request = 0 ;
		cip->ci_reply = 0 ;
    }

    /*
     *  Don't remove other one's or own big comseg.
     */
    if (    (cip->ci_protocol == PROT_BIGSHM_EO003)
         || (cip->ci_shmid    == tcb->bigComSegBelongsToUKT->connection->ci_shmid) )
    {
		DBG1(( MF__, "don't remove peer big comseg!"));
		return ;
    }

    (void) sql41_remove_shm ( &cip->ci_shmid , "us" , KGS->serverdb );
    DBG1 (( MF__,"e74_release_comseg: shmid %d big_comseg 0x%lx comseg 0x%lx \n",
			cip->ci_shmid , (long)cip->ci_big_comseg , (long)cip->ci_comseg ));
}

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"e74_print_all_conns"
void	e74_print_all_conns ( char *hdr )
{
    int				idx ;
    connection_info		*cip ;
    char			linbuf [ 256 ];


  for ( idx = 0 ; idx < KGS->connection_cnt ; idx ++ )
	{
		cip = KGS->connection + idx ;
		if ( cip->ci_tcb )
		{
			(void) sp77sprintf ( linbuf , sizeof(linbuf), "T%d" , cip->ci_tcb->index );
		}
		else if ( cip->ci_big_comseg )
		{
			(void) sp77sprintf ( linbuf , sizeof(linbuf), "UKT%d" , cip->ci_my_ref );
		}
		else
		{
			(void) sp77sprintf ( linbuf , sizeof(linbuf), "???" , cip->ci_my_ref );
		}
		DBG1 (( MF__,"%s: - - - - - - - - - - - - - - - - - - - \n", hdr ));
		DBG1 (( MF__,"%s: KGS connection   %d (%s) \n", hdr , idx , linbuf ));
		e74_print_connection ( hdr , cip );
	}
    DBG1 (( MF__,"%s: - - - - - - - - - - - - - - - - - - - \n", hdr ));
}

/*
 * ======================================================================
 */
#undef MF__
#define MF__ MOD__"e74_print_connection"
void	e74_print_connection (
char			*hdr ,
connection_info	*cip )
{
  DBG1 (( MF__,"%s: connect_time     %ld \n",     hdr , (long) cip->ci_connect_time ));
  DBG1 (( MF__,"%s: state            %d \n",      hdr , cip->ci_state            ));
  DBG1 (( MF__,"%s: use_count        %d \n",      hdr , cip->ci_use_count        ));
  DBG1 (( MF__,"%s: service          %d \n",      hdr , cip->ci_service          ));
  DBG1 (( MF__,"%s: protocol         %d \n",      hdr , cip->ci_protocol         ));
  DBG1 (( MF__,"%s: packet_cnt       %d \n",      hdr , cip->ci_packet_cnt       ));
  DBG1 (( MF__,"%s: packet_size      %ld \n",     hdr , cip->ci_packet_size      ));
  DBG1 (( MF__,"%s: min_reply_size   %ld \n",     hdr , cip->ci_min_reply_size   ));
  DBG1 (( MF__,"%s: max_data_size    %ld \n",     hdr , cip->ci_max_data_size    ));
  DBG1 (( MF__,"%s: max_request_size %ld \n",     hdr , cip->ci_max_request_size ));
  DBG1 (( MF__,"%s: my_pid           %ld \n",     hdr , (long) cip->ci_my_pid    ));
  DBG1 (( MF__,"%s: peer_pid         %ld \n",     hdr , (long) cip->ci_peer_pid  ));
  DBG1 (( MF__,"%s: my_ref           %d \n",      hdr , cip->ci_my_ref           ));
  DBG1 (( MF__,"%s: peer_ref         %d \n",      hdr , cip->ci_peer_ref         ));
  DBG1 (( MF__,"%s: my_semid         %d \n",      hdr , cip->ci_my_semid         ));
  DBG1 (( MF__,"%s: peer_semid       %d \n",      hdr , cip->ci_peer_semid       ));
  DBG1 (( MF__,"%s: shmid            %d \n",      hdr , cip->ci_shmid            ));
  DBG1 (( MF__,"%s: tcb              0x%lx \n",   hdr , (long)cip->ci_tcb        ));
#ifdef LATER
  DBG1 (( MF__,"%s: peer_swap_type   %d \n",      hdr , cip->ci_peer_swap_type   ));
  DBG1 (( MF__,"%s: max_segment_size %ld \n",     hdr , cip->ci_max_segment_size ));
  DBG1 (( MF__,"%s: sd               %d \n",      hdr , cip->ci_sd               ));
  DBG1 (( MF__,"%s: peer_sock_addr      \n",      hdr /*, cip->ci_peer_sock_addr*/ ));
  DBG1 (( MF__,"%s: peer_dbname      '%.18s' \n", hdr , cip->ci_peer_dbname      ));
  DBG1 (( MF__,"%s: peer_node        '%.18s' \n", hdr , cip->ci_peer_node        ));
  DBG1 (( MF__,"%s: remote_pid       '%.18s' \n", hdr , cip->ci_remote_pid       ));
  DBG1 (( MF__,"%s: big_comseg       0x%lx \n",   hdr , (long)cip->ci_big_comseg ));
  DBG1 (( MF__,"%s: big_offset       %ld \n",     hdr , cip->ci_big_offset       ));
  DBG1 (( MF__,"%s: big_size         %ld \n",     hdr , cip->ci_big_size         ));
  DBG1 (( MF__,"%s: comseg           0x%lx \n",   hdr , (long)cip->ci_comseg           ));
  DBG1 (( MF__,"%s: compack          0x%lx \n",   hdr , (long)cip->ci_compack          ));
  DBG1 (( MF__,"%s: packet[0]        0x%lx \n",   hdr , (long)cip->ci_packet[0]        ));
  DBG1 (( MF__,"%s: packet[1]        0x%lx \n",   hdr , (long)cip->ci_packet[1]        ));
  DBG1 (( MF__,"%s: request          0x%lx \n",   hdr , (long)cip->ci_request          ));
  DBG1 (( MF__,"%s: reply            0x%lx \n",   hdr , (long)cip->ci_reply            ));
  DBG1 (( MF__,"%s: request_lgt      %ld \n",     hdr , cip->ci_request_lgt      ));
  DBG1 (( MF__,"%s: reply_size       %ld \n",     hdr , cip->ci_reply_size       ));
  DBG1 (( MF__,"%s: current_packet   %d \n",      hdr , cip->ci_current_packet   ));
  DBG1 (( MF__,"%s: cache_buf        0x%lx \n",   hdr , (long)cip->ci_cache_buf        ));
  DBG1 (( MF__,"%s: cache_lgt        %ld \n",     hdr , cip->ci_cache_lgt        ));
  DBG1 (( MF__,"%s: pipe_fd          %d \n",      hdr , cip->ci_pipe_fd          ));
  DBG1 (( MF__,"%s: mess_class       %d \n",      hdr , cip->ci_mess_class       ));
  DBG1 (( MF__,"%s: locbigoffset     %ld \n",     hdr , cip->ci_locbigoffset     ));
  DBG1 (( MF__,"%s: loccomseg        0x%lx \n",   hdr , (long)cip->ci_loccomseg        ));
#endif /*LATER*/
}
