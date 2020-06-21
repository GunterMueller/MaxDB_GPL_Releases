/*!
  @file           ven75.c
  @author         JoergM
  @brief          Kernel Runtime: Communication Functions for Kernel
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

#define MOD__  "ven75.c:"
/* exported functions */

#include "gen00.h"
#include "heo00.h"
#include "gen500.h"
#include "vsp001c.h"
#include "gen003.h"
#include "hen40.h"

/* imported functions */

#include "geo50_0.h"
#include "gen32.h"
#include "gen41.h"
#include "gen46.h"
#if defined(AIX) || defined(HP9)
#include "gen49.h" /* nocheck */

#endif /* AIX || HP9 */
#include "gen53.h"
#include "gen71.h"
#include "gen73.h"
#include "gen74.h"
#include "gen75.h"
#include "gen88.h"
#include "geo67.h"
#include "hsp77.h"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "heo92.h"
#include "RunTime/RTE_ConsoleStandardOutput.h"
#include "SAPDBCommon/SAPDB_Names.h"

/*
 * =====================================================================
 */

int	e75_wake_peer ( connection_info *cip )
{
#undef  MF__
#define MF__ MOD__"e75_wake_peer"
    int				rc ;
    int				index ;
    int				fd ;
    int             ret ;

    DBGIN;

	ret = SP1CE_OK;

    while ( ret == SP1CE_OK )
	{
		DBG1 (( MF__,"semop semid %d \n", cip->ci_peer_semid ));
		rc = semop ( cip->ci_peer_semid , &semsnd , 1 );
		if ( rc == UNDEF )
		{
			if ( errno == EINTR )
			{
				MSGD (( WRN_DISTRIB_WAKE_SEMOP_ERR ))
				continue;
			}

			/*
			 *  Ignore a semaphore value overrun.
			 */
			if ( errno != ERANGE )
			{
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
                system("ipcs >last_client_semop_error_ipcs_output");
#endif
                MSGD (( 11253,ERR_TYPE,"COMMUNIC","client semop %d error: %s"
                    , cip->ci_peer_semid
                    , sqlerrs() ));
				sql41_stat_sem ( cip->ci_peer_semid );
				ret = SP1CE_NOTOK;
			}
		}
		else
		{
			break;
		}
	}

    DBG1 (( MF__,"Returning %d \n", ret ));

	DBGOUT;
    return ( ret );
}

/*
 *  =======================================================================
 */

void	e75_release_all_ukp_tasks ( )
{
#undef  MF__
#define MF__ MOD__"e75_release_all_ukp_tasks"

ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    struct TASK_TYPE		*tcb ;

	DBGIN;
    /*
     *  This function is intended to be called from dispatcher,
     *  where the value of this_ukt->curr_task is changed anyway.
     *  If someone later tries to call it from elsewhere,
     *  it is safer to save the original value of this_ukt->curr_task.
     *  We need to set this_ukt->curr_task for usage within e75_release_connection.
     */
    tcb = this_ukt->curr_task ;

    for ( this_ukt->curr_task = this_ukt->pFirstTaskCtrl ;
		  this_ukt->curr_task ;
		  this_ukt->curr_task = this_ukt->curr_task->next_task )
    {
		DBG1 (( MF__,"Shutdown test T%d \n", this_ukt->curr_task->index ));
		if ( (this_ukt->curr_task->type == TT_US_EO00 || this_ukt->curr_task->type == TT_EV_EO00) 
	      && e75_is_connected(this_ukt->curr_task) )
		{
		    e75_release_connection ( SP1CE_SHUTDOWN, FALSE ); /* no wait */
		}
    }

    this_ukt->curr_task = tcb ;

	DBGOUT;
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

void en75DumpCommunicationSegment(struct TASK_TYPE *tcb, SAPDB_Byte *big_commseg_addr, SAPDB_ULong segSize)
{
    tsp00_Pathc CommunicationSegmentDumpPath;
    SAPDB_Int dumpFd;

    sp77sprintf( 
          CommunicationSegmentDumpPath
        , sizeof(tsp00_Pathc)
        , "%s/" SAPDB_COMMUNICATION_DUMP_NAME
        , XPARAM(szRunDirectory) );
    
    dumpFd = RTE_save_open((char *)CommunicationSegmentDumpPath, O_APPEND|O_WRONLY);
    if ( dumpFd >= 0 )
    {
    	struct connection_info * pCi = tcb->connection;
        SAPDB_Char buffer[4096];
#define MAX_NAME_BUFFER_SIZE 128
        char stateBuffer[MAX_NAME_BUFFER_SIZE];
        SAPDB_Byte *packet;
#define SAPDB_INT4S_PER_LINE 4
        SAPDB_UInt iOffset;
        SAPDB_UInt lastOffset;
        SAPDB_UInt iInt4;
        SAPDB_UInt iChar;
        SAPDB_UInt lastShownOffset = 0;
        char dumpLine[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];
        char dumpLineCopy[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];

        sp77sprintf(buffer, sizeof(buffer), "====================================================\n" );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        sp77sprintf(buffer, sizeof(buffer), "Communication segment dump for" XCONS_FORMAT_SHOW_CONNECTION_TASK, 
                        tcb->index, tcb->ukt->index, eo92GetTaskTypeName ( tcb->type ) );
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        if ( pCi->ci_connect_time == 0 )
		{
            sp77sprintf(buffer, sizeof(buffer), "Connection " XCONS_FORMAT_SHOW_CONNECTION_STATE_NOT_CONNECTED,
                eo92GetConnectStateName( pCi->ci_state, stateBuffer, MAX_NAME_BUFFER_SIZE ) );
        }
		else
		{
            struct tm resultBuffer;
			struct tm *pTime = localtime_r( &pCi->ci_connect_time, &resultBuffer );
			if ( pTime )
			{
                /* manual page for asctime_r says at least 26 characters... */
                SAPDB_Char timeBuffer[32];
#if (defined(SUN) || defined(SOLARIS)) && !defined(_POSIX_PTHREAD_SEMANTICS)
                sp77sprintf(buffer, sizeof(buffer), "Connection " XCONS_FORMAT_SHOW_CONNECTION_STATE_TIME,
                    eo92GetConnectStateName( pCi->ci_state, stateBuffer, MAX_NAME_BUFFER_SIZE ), asctime_r( pTime, &timeBuffer[0], sizeof(timeBuffer) ) );
#else
                sp77sprintf(buffer, sizeof(buffer), "Connection " XCONS_FORMAT_SHOW_CONNECTION_STATE_TIME,
                    eo92GetConnectStateName( pCi->ci_state, stateBuffer, MAX_NAME_BUFFER_SIZE ), asctime_r( pTime, &timeBuffer[0] ) );
#endif
			}
			else
			{
                sp77sprintf(buffer, sizeof(buffer), "Connection " XCONS_FORMAT_SHOW_CONNECTION_STATE_DUMPTIME, 
                    eo92GetConnectStateName( pCi->ci_state, stateBuffer, MAX_NAME_BUFFER_SIZE ), (long)pCi->ci_connect_time );
			}
		}
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        if ( 0 == pCi->ci_remote_pid[0] || ' ' == pCi->ci_remote_pid[0] )
		{
			sp77sprintf(buffer, sizeof(buffer), XCONS_FORMAT_SHOW_CONNECTION_LOCAL, (int)pCi->ci_peer_pid);
		}
		else
		{
			sp77sprintf(buffer, sizeof(buffer), XCONS_FORMAT_SHOW_CONNECTION_REMOTE, (int)pCi->ci_peer_pid, pCi->ci_remote_pid, pCi->ci_peer_node);
		}
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        sp77sprintf(buffer, sizeof(buffer), "----- connection_info DUMP-----\n");
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        sp77sprintf(buffer, sizeof(buffer), " ci_connect service   %s\n", eo92GetConnectServiceName( pCi->ci_service, stateBuffer, MAX_NAME_BUFFER_SIZE ) );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        sp77sprintf(buffer, sizeof(buffer), " ci_use_count         %d\n", pCi->ci_use_count );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_protocol          %s\n", eo92GetConnectProtocolName( pCi->ci_protocol, stateBuffer, MAX_NAME_BUFFER_SIZE ) );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_packet_cnt        %d\n",  pCi->ci_packet_cnt );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_packet_size       %ld\n", pCi->ci_packet_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_min_reply_size    %ld\n", pCi->ci_min_reply_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_reply_size        %ld\n", pCi->ci_reply_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_max_request_size  %ld\n", pCi->ci_max_request_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_request_lgt       %ld\n", pCi->ci_request_lgt );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_max_data_size     %ld\n", pCi->ci_max_data_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_my_ref            %d\n",  pCi->ci_my_ref );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " tcb->bigComSegBelongsToUKT->semid      %d\n",  tcb->bigComSegBelongsToUKT->semid );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_peer_ref          %d\n",  pCi->ci_peer_ref );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_peer_semid        %d\n",  pCi->ci_peer_semid );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_shmid             %d\n",  pCi->ci_shmid );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_peer_swap_type    %d\n",  pCi->ci_peer_swap_type );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_max_segment_size  %ld\n",  pCi->ci_max_segment_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_big_offset        %ld\n", pCi->ci_big_offset );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " ci_big_size          %ld\n", pCi->ci_big_size );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " BytesReceived        %d\n", pCi->BytesReceived );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
		sp77sprintf(buffer, sizeof(buffer), " BytesSend            %d\n", pCi->BytesSend );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        sp77sprintf(buffer, sizeof(buffer), " Encryption           %d\n", pCi->encryption );
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        sp77sprintf(buffer, sizeof(buffer), "-----comseg_header DUMP-----\n");
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        if ( segSize < (tcb->connection->ci_big_offset + sizeof(comseg_header)) )
        {
            sp77sprintf(buffer, sizeof(buffer),
                "Interpretation aborted: segment size too small < offset + sizeof(comseg_header) = %d\n",
                (int)(tcb->connection->ci_big_offset + sizeof(comseg_header)) );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
        }
        else if ( (tcb->connection->ci_big_offset & (sizeof(comseg_header *)-1)) != 0 )
        {
            sp77sprintf(buffer, sizeof(buffer),
                "Interpretation aborted: bad alignment of connection segment header offset = %d\n",
                (int)tcb->connection->ci_big_offset );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
        }
        else
        {
            comseg_header *pCommseg = (comseg_header *)(big_commseg_addr + tcb->connection->ci_big_offset);

            sp77sprintf(buffer, sizeof(buffer), "    cs_size           %ld\n", (long)pCommseg->cs_size );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_version        %d\n", pCommseg->cs_version );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_packet_cnt     %d\n", pCommseg->cs_packet_cnt );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_packet_size    %ld\n", (long)pCommseg->cs_packet_size );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_list_offset    %ld\n", (long)pCommseg->cs_list_offset );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_current_packet %ld\n", (long)pCommseg->cs_current_packet );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_client_pid     %ld\n", (long)pCommseg->cs_client_pid );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_server_pid     %ld\n", (long)pCommseg->cs_server_pid );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_client_ref     %d\n", (long)pCommseg->cs_client_ref );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_server_ref     %d\n", (long)pCommseg->cs_server_ref );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_client_state   %d\n", (long)pCommseg->cs_client_state );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_server_state   %d\n", (long)pCommseg->cs_server_state );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_client_flag    %d\n", (long)pCommseg->cs_client_flag );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_server_flag    %d\n", (long)pCommseg->cs_server_flag );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_shmid          %d\n", (long)pCommseg->cs_shmid );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_client_semid   %d\n", (long)pCommseg->cs_client_semid );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
            sp77sprintf(buffer, sizeof(buffer), "    cs_server_semid   %d\n", (long)pCommseg->cs_server_semid );
            RTE_save_write(dumpFd, buffer, strlen(buffer));

            sp77sprintf(buffer, sizeof(buffer), "-----commseg packet content DUMP-----\n");
            RTE_save_write(dumpFd, buffer, strlen(buffer));

            if ( segSize < pCommseg->cs_list_offset
              || (pCommseg->cs_list_offset & (sizeof(long *)-1)) != 0 )
            {
                sp77sprintf(buffer, sizeof(buffer),
                    "Interpretation aborted: cs_list_offset not aligned or not in segment\n");
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                return;
            }

            sp77sprintf(  buffer
                        , sizeof(buffer)
                        , "packet offset 0x%lx total size 0x%lx request offset 0x%lx size 0x%lx\n"
                        , (long)*(ten_com_long *)(big_commseg_addr +  pCommseg->cs_list_offset)
                        , (pCi->ci_max_data_size+(2*sizeof (rte_header)))
                        , (long)sizeof(rte_header)
                        , (long)pCi->ci_request_lgt );
            RTE_save_write(dumpFd, buffer, strlen(buffer));

            packet = ((SAPDB_Byte *)big_commseg_addr) + (long)*(ten_com_long *)(big_commseg_addr +  pCommseg->cs_list_offset);

            memset(dumpLineCopy, 0xFF, sizeof(dumpLineCopy));

            lastShownOffset = 0;
            lastOffset = pCi->ci_max_data_size+(2*sizeof (rte_header));

            if ( (pCommseg->cs_list_offset + lastOffset) > segSize )
            {
                lastOffset = segSize -  pCommseg->cs_list_offset;

                sp77sprintf(buffer, sizeof(buffer),
                    "Dump truncated tp %ld: offset + + 2*RTE_Header + max_data_size %ld larger than segment size",
                        (long)lastOffset,
                        ((long)pCommseg->cs_list_offset) + pCi->ci_max_data_size+(2*sizeof (rte_header)) );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
            }

            for ( iOffset = 0; iOffset < lastOffset; iOffset += (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)) )
            {
                for ( iInt4 = 0; iInt4 < SAPDB_INT4S_PER_LINE; iInt4++ )
                {
                    for ( iChar = 0; iChar < sizeof(SAPDB_Int4); iChar++ )
                    {
                        SAPDB_UInt iCharInLine = (sizeof(SAPDB_Int4)*iInt4) + iChar;
                        if ( (iOffset + iCharInLine) < lastOffset )
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
                        RTE_save_write(dumpFd, buffer, strlen(buffer));
                    }
                    memcpy(dumpLineCopy, dumpLine, sizeof(dumpLineCopy));
                    sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X %s\n", iOffset, &dumpLine[0] );
                    RTE_save_write(dumpFd, buffer, strlen(buffer));
                    lastShownOffset = iOffset;
                }
            }

            if ( (lastShownOffset+(SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4))) < iOffset )
            {
                sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X - 0x%05X identical\n",
                    lastShownOffset + (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)), iOffset-1);
                RTE_save_write(dumpFd, buffer, strlen(buffer));
            }

        }


        RTE_save_close(dumpFd);
    }
}

void en75DumpBigCommunicationSegment(struct ten50_UKT_Control * pUkt, SAPDB_Int4 taskIndex, SAPDB_Byte *big_commseg_addr, SAPDB_ULong segSize)
{
    tsp00_Pathc CommunicationSegmentDumpPath;
    SAPDB_Int dumpFd;

    sp77sprintf( 
          CommunicationSegmentDumpPath
        , sizeof(tsp00_Pathc)
        , "%s/" SAPDB_COMMUNICATION_DUMP_NAME
        , XPARAM(szRunDirectory) );
    
    dumpFd = RTE_save_openCreate((char *)CommunicationSegmentDumpPath, O_APPEND|O_WRONLY|O_CREAT, 0660);
    if ( dumpFd >= 0 )
    {
        SAPDB_Char buffer[4096];
#define MAX_NAME_BUFFER_SIZE 128
        char stateBuffer[MAX_NAME_BUFFER_SIZE];
        SAPDB_Byte *packet;
#define SAPDB_INT4S_PER_LINE 4
        SAPDB_UInt iOffset;
        SAPDB_UInt iInt4;
        SAPDB_UInt iChar;
        SAPDB_UInt lastShownOffset = 0;
        char dumpLine[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];
        char dumpLineCopy[(SAPDB_INT4S_PER_LINE * (1 + (sizeof(SAPDB_Int4) * 3))) + 1];

        sp77sprintf(buffer, sizeof(buffer), "====================================================\n" );
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        sp77sprintf(buffer, sizeof(buffer), "Big Communication segment dump for UKT%03d (due to Task T%03d)\n", pUkt->index, taskIndex);
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        sp77sprintf(buffer, sizeof(buffer), "-----big_comseg_header DUMP (size %lu)-----\n", segSize);
        RTE_save_write(dumpFd, buffer, strlen(buffer));
        if ( segSize < sizeof(big_comseg_header) )
        {
            sp77sprintf(buffer, sizeof(buffer), "Interpretation aborted: segment size too small < sizeof(big_comseg_header) = %d... \n", (int)sizeof(big_comseg_header) );
            RTE_save_write(dumpFd, buffer, strlen(buffer));
        }
        else
        {
            big_comseg_header *pBigCommseg = (big_comseg_header *)big_commseg_addr;

            sp77sprintf(buffer, sizeof(buffer), "    bc_size           %ld\n", (long)pBigCommseg->bc_size );
            RTE_save_write(dumpFd, buffer, strlen(buffer));

            if ( segSize != (SAPDB_ULong)pBigCommseg->bc_size )
            {
                sp77sprintf(buffer, sizeof(buffer), "Interpretation aborted: segment size (%lu) mismatch\n", (SAPDB_ULong)pBigCommseg->bc_size );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
            }
            else
            {
                sp77sprintf(buffer, sizeof(buffer), "    bc_version        %d\n", pBigCommseg->bc_version );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_comseg_cnt     %d\n", pBigCommseg->bc_comseg_cnt );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_packet_cnt     %d\n", pBigCommseg->bc_packet_cnt );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_comseg_size    %ld\n", (long)pBigCommseg->bc_comseg_size );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_packet_size    %ld\n", (long)pBigCommseg->bc_packet_size );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_comseg_offset  %ld\n", (long)pBigCommseg->bc_comseg_offset );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_list_offset    %ld\n", (long)pBigCommseg->bc_list_offset );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_packet_offset  %ld\n", (long)pBigCommseg->bc_packet_offset );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_kp_running     %d\n", pBigCommseg->bc_kp_running );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                sp77sprintf(buffer, sizeof(buffer), "    bc_common_flag    %d\n", pBigCommseg->bc_common_flag );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
            }
        }

        sp77sprintf(buffer, sizeof(buffer), "-----big_comseg content DUMP-----\n");
        RTE_save_write(dumpFd, buffer, strlen(buffer));

        packet = ((SAPDB_Byte *)big_commseg_addr);
        memset(dumpLineCopy, 0xFF, sizeof(dumpLineCopy));

        lastShownOffset = 0;
        for ( iOffset = 0; iOffset < segSize; iOffset += (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)) )
        {
            for ( iInt4 = 0; iInt4 < SAPDB_INT4S_PER_LINE; iInt4++ )
            {
                for ( iChar = 0; iChar < sizeof(SAPDB_Int4); iChar++ )
                {
                    SAPDB_UInt iCharInLine = (sizeof(SAPDB_Int4)*iInt4) + iChar;
                    if ( (iOffset + iCharInLine) < segSize )
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
                    RTE_save_write(dumpFd, buffer, strlen(buffer));
                }
                memcpy(dumpLineCopy, dumpLine, sizeof(dumpLineCopy));
                sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X %s\n", iOffset, &dumpLine[0] );
                RTE_save_write(dumpFd, buffer, strlen(buffer));
                lastShownOffset = iOffset;
            }
        }

        if ( (lastShownOffset+(SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4))) < iOffset )
        {
            sp77sprintf(&buffer[0], sizeof(buffer), "0x%05X - 0x%05X identical\n",
                lastShownOffset + (SAPDB_INT4S_PER_LINE*sizeof(SAPDB_Int4)), iOffset-1);
            RTE_save_write(dumpFd, buffer, strlen(buffer));
        }

        RTE_save_close(dumpFd);
    }
}

/* ======================================================================= */

void en75DumpActiveCommunicationSegments(SAPDB_Bool needAttach)
{
    static int dumpTriggered = 0;
    struct ten50_UKT_Control *pUkt; 
 
    if ( dumpTriggered )
    {
        return;
    }

    dumpTriggered = 1;

    for ( pUkt = KGS->first_kp; pUkt <= KGS->last_kp; ++pUkt )
    {
    	struct TASK_TYPE *tcb;
        SAPDB_Int4 taskIndex;
        SAPDB_Bool bigCommsegDumped = false;
        for ( taskIndex = 0; taskIndex < KGS->ulNumOfTasks; taskIndex++ )
        {
            tcb = KGS->pFirstTaskCtrl + taskIndex;
            if ( tcb->bigComSegBelongsToUKT == pUkt
              && (tcb->type == TT_US_EO00
               || tcb->type == TT_EV_EO00
               || tcb->type == TT_UT_EO00) 
		      && e75_is_connected(tcb) )
            {
                comseg_header *comseg;
                connection_info *cip = tcb->connection;
                SAPDB_ULong shmSize;
                char *addr;

                if ( needAttach )
                {
                    /* Shared memory id of big commsect is found in ukt->connection info only */
                    addr = sql41_attach_shm ( tcb->bigComSegBelongsToUKT->connection->ci_shmid, (char *)0 );
                    if ( ((char *)0) == addr )
                    {
                        continue;
                    }
                }
                else
                {
                    addr = (char *)tcb->bigComSegBelongsToUKT->connection->ci_big_comseg;
                }

                shmSize = sql41_getShmSize(tcb->bigComSegBelongsToUKT->connection->ci_shmid);

                if ( tcb->type  != TT_EV_EO00
                  && tcb->state != TSK_CONNECTWAIT_EO00 )
                {
                    if ( !bigCommsegDumped )
                    {
                        en75DumpBigCommunicationSegment(pUkt, tcb->index, (SAPDB_Byte *)addr, shmSize);
                        bigCommsegDumped = true;
                    }
                    en75DumpCommunicationSegment(tcb, (SAPDB_Byte *)addr, shmSize);
                }

                if ( needAttach )
                {
                    sql41_detach_shm ( &addr );
                }
            }
        }
    }
}

/* ======================================================================= */

/* PTS 1113008 */
void en75FinalReleaseConnections( int kernelExitCode )
{
	struct TASK_TYPE *tcb;
	int taskIndex;
	int reason = ( kernelExitCode == 0 ? SP1CE_SHUTDOWN : SP1CE_CRASH );

    if ( reason == SP1CE_CRASH || SERVER_KILL == KGS->state )
    {
        en75DumpActiveCommunicationSegments(true);
    }

    for ( taskIndex = 0; taskIndex < KGS->ulNumOfTasks; taskIndex++ )
    {
        tcb = KGS->pFirstTaskCtrl + taskIndex;
        if ( (tcb->type == TT_US_EO00 || tcb->type == TT_EV_EO00 || tcb->type == TT_UT_EO00) 
		  && e75_is_connected(tcb)
/*          && tcb->index != KGS->taskResponsibleForOffline  */ /* PTS 1134089 */
          )
        {
            SAPDB_ULong shmSize;
            comseg_header *comseg;
            connection_info *cip = tcb->connection;
            /* Shared memory id of big commsect is found in ukt->connection info only */
            char *addr = sql41_attach_shm ( tcb->bigComSegBelongsToUKT->connection->ci_shmid, (char *)0 );
            if ( ((char *)0) == addr )
            {
                MSGD(( ERR_INITCOM_ATT_ADDR_BIGCOM,  (long) addr ))
                continue;
            }

            shmSize = sql41_getShmSize(tcb->bigComSegBelongsToUKT->connection->ci_shmid);
            if ( shmSize >= ( cip->ci_big_offset + sizeof(comseg_header) )
              && (cip->ci_big_offset & (sizeof(comseg_header *)-1)) == 0 )
            {
                /* Setting the cs_server_state is all that is needed, to inform the peer
                   about SHUTDOWN or CRASH state */
                comseg = (comseg_header *)(addr + cip->ci_big_offset);
                comseg->cs_server_state = reason;
                if ( reason != SP1CE_SHUTDOWN )
                {
    			    MSGD (( WRN_RELEASE_TSK_BY_REASON, tcb->index , "kernel abort" ));
                }
                else
                {
    		        MSGD (( INFO_RELEASING_TSK, tcb->index ));
                }

                /* PTS 1113359 */
#if defined(_IBMR2) || defined(PA11) || defined(PA20W)
                e49_unlock_int4_4fold ( &comseg->cs_exclusive , 1 ) ;
#else
                RTESys_Unlock( (RTE_Lock*)&comseg->cs_exclusive );
#endif
            }
            /* We do not inform the task about release state here !!!
               It will get informed by removing semaphore later on */
            sql41_detach_shm ( &addr );
#ifdef SDB_WITH_PROFILER
            /* If profiling is used, the semaphore is not removed, so we inform client now */
            (void)semop ( cip->ci_peer_semid , &semsnd , 1 );
#endif /* SDB_WITH_PROFILER */
        }
    }
}

/*
 *  =======================================================================
 */

void e75_release_connection ( int reason ,
                              int waitflag )
{
#undef  MF__
#define MF__ MOD__"e75_release_connection"

	ten50_UKT_Control *this_ukt = THIS_UKT_CTRL;
    struct TASK_TYPE  *tcb = this_ukt->curr_task;
    int				idx ;
    int				oldstate ;
    int				tskstate ;
    long			off ;
    rte_header      *header ;
    comseg_header   *comseg ;
    connection_info *cip ;
    char            *errmsg ;
    tsp00_ErrTextc   errtext ;
    int wait_for_client = waitflag;

/*    if ( tcb->index == KGS->taskResponsibleForOffline ) *//* PTS 1134089 */
/*    {
        return;
    }
 */

	DBGIN;
    if ( reason == SP1CE_CRASH || SERVER_KILL == KGS->state )
    {
        en75DumpActiveCommunicationSegments(false);
    }

    cip = tcb->connection;
#ifdef DEBUG_RTE
    DBG1 (( MF__,"Releasing  T%d  reason %d wait %d",
		tcb->index , reason , waitflag ));
#endif
    if ( cip->ci_connect_time )
    {
		if ( reason != SP1CE_OK )
		{
			switch ( reason )
			{
				case SP1CE_TIMEOUT :
					errmsg = "command timeout" ;
					break ;
				case SP1CE_CRASH :
					errmsg = "connection aborted" ;
					break ;
				case SP1CE_SHUTDOWN :
					errmsg = "database shutdown" ;
					break ;
				case SP1CE_RELEASED :
					errmsg = "without reply" ;
					break ;
				default :
					(void) sp77sprintf ( errtext , sizeof(errtext), "reason: %d" , (int) reason );
					errmsg = errtext ;
					break ;
			}
			MSGD (( WRN_RELEASE_TSK_BY_REASON, tcb->index , errmsg ));
		}
		else
		    MSGD (( INFO_RELEASING_TSK, tcb->index ));
    }

    /* FH
     *  Set stat_receive to SP1CE_CRASH to indicate to the REQUESTOR
     *  (e84_test_application), that this task will be released at
     *  this time. In this case the function e84_test_application 
     *  should NOT test the application.
     *  It is possible that we have a time-problem, and the requestor
     *  will kill the task for the application, which made this release,
     *  so the application thinks: kernel broke connection!
     *  (The UKP has not made the vrelease completely,
     *  a new connect has been received by the requestor.)
     */
    tskstate = tcb->state ;
    tcb->state = TSK_VRELEASE_EO00 ;
    oldstate = cip->ci_state ;

	if ( cip->ci_state != CON_UNUSED )
    {
        if ( tcb->type == TT_EV_EO00 )
        {
            eo67ReleaseEventTask(tcb->index);
        }
    }

    cip->ci_state = CON_UNUSED ;
    /*
     *  Only if still attached
     */
    comseg = cip->ci_comseg;
    DBG1 (( MF__,"comseg 0x%08lx", (long) comseg ));
    if ( comseg )
    {
		DBG1 (( MF__,"cli,srv %d,%d",
		    comseg->cs_client_flag , comseg->cs_server_flag ));
		if ( reason != SP1CE_OK )
		{
			DBG1 (( MF__,"clearing comseg values" ));
			sql32_lock_comseg ( cip , "e75_release: 0x%08lx \n" );
			idx = comseg->cs_current_packet ;
			if ( (idx < 0) || (idx > cip->ci_packet_cnt) ) idx = 0 ;
			comseg->cs_current_packet = idx ;
			header = cip->ci_packet[idx];
			off = (long) header->rh_max_send_len & 7L ;
			header = (rte_header*) ((char*) header + header->rh_max_send_len);
			if ( off )
			{
				header = (rte_header*) ((char*) header + 8L - off);
			}
			SAPDB_memset ( header , 0 , RTE_HEADER_SIZE );
			header->rh_act_send_len    = RTE_HEADER_SIZE_EO003 ;
			header->rh_max_send_len    = RTE_HEADER_SIZE_EO003 ;
			header->rh_mess_class      = RSQL_USER_RELEASE_REQUEST_EO003 ;
			header->rh_rte_return_code = reason ;
			comseg->cs_server_state = reason ;
			comseg->cs_server_pid   = 0 ;
			comseg->cs_server_ref   = 0 ;
			comseg->cs_server_semid = 0 ;
			comseg->cs_server_flag  = 1 ;
            if ( SP1CE_RELEASED == comseg->cs_client_state )
            {
                wait_for_client = false;
            }
            sql32_unlock_comseg ( cip );

			if ( oldstate != CON_REPLIED && wait_for_client )
			{
				(void) e75_wake_peer ( cip );
			}
		}

		if ( wait_for_client )
		{
            e53_wait_for_communication( tcb, cip,
                                        LOCL_PKT_TMO + 
                                        KGS->current_time + 
                                        MAXSLEEPTIME );
            comseg->cs_client_flag = 2;
            comseg->cs_server_flag = 0;
		}

		/*
		 *  Might be that the application did not yet see the server flag
		 *  due to UNIX scheduling. Nevertheless, we must not release the
		 *  comseg with the server flag set, it could be misunderstood at
		 *  the next connect.
		 */
		comseg->cs_server_state = (reason == SP1CE_OK) ?
						SP1CE_RELEASED : reason ;
		comseg->cs_server_pid   = 0 ;
		comseg->cs_server_ref   = 0 ;
		comseg->cs_server_semid = 0 ;
		comseg->cs_server_flag  = 0 ;

		e74_release_comseg ( cip, tcb );
		cip->ci_comseg = 0 ;
    }  

    if ( cip->ci_peer_semid > 0 )
    {
		DBG1 (( MF__,"removing client semaphore" ));
		(void) sql41_remove_sem ( &cip->ci_peer_semid , "us" , (char *)KGS->serverdb );
    }

    cip->ci_peer_pid   = 0 ;
    SAPDB_memset ( cip->ci_remote_pid , 0 , sizeof(cip->ci_remote_pid) );

    if ( cip->ci_connect_time )
    {
        WAIT_UNTIL_ALONE ( tcb->ukt->exclusive );
        if ( cip->ci_connect_time )
        {
            cip->ci_connect_time = 0 ;

            /* cip->ci_state = CON_UNUSED ; already done above */
            KGS->connectedUserTasks -= this_ukt->curr_task->type == TT_US_EO00 ? 1 : 0;
        }
        CLEARLOCK ( tcb->ukt->exclusive );
    }

    tcb->state = tskstate ;

    DBGOUT;
}

/*
 * ===========================================================================
 */


int     e75_is_connected ( struct TASK_TYPE	*tcb )
{
#undef  MF__
#define MF__ MOD__"e75_is_connected"
	DBGIN;
	if ( tcb->connection
	  && tcb->connection->ci_connect_time )
	{
		DBG1 (( MF__,"connected" ));
		DBGOUT;
		return ( TRUE );
	}
	DBG1 (( MF__,"not connected" ));
	DBGOUT;
    return ( FALSE ) ;
}

/*
 * ===========================================================================
 */

int e75_connect_request ( struct TASK_TYPE  * tcb,
                          connection_info   * cip )
{
#undef  MF__
#define MF__ MOD__"e75_connect_request"
    ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;
    int				    rc ;
    int				    stack_avail = TRUE ;
    tsp1_comm_error		result ;
    comseg_header		*comseg ;
    char			logbuf [ 256 ];

	DBGIN;

    DBG1 (( MF__,"connecting T%d", tcb->index ));

    result = SP1CE_OK ;

    DBG1 (( MF__,"T%d my_ref %d peer_ref %d",
	    tcb->index,
	    cip->ci_my_ref , cip->ci_peer_ref ));

    /*
     *  Cleanup
     */
    if ( cip->ci_comseg )
    {
		MSGD (( WRN_CONNECT_LAST_COMSEG, (long) cip->ci_comseg ));
		e74_release_comseg ( cip, tcb );
		cip->ci_comseg = 0 ;
    }
    /*
     *  If, some day, multiple packets are required,
     *  we have to set up the big comseg and comseg within it
     *  to support the requested number of packets: cip->ci_packet_cnt.
     */

    /*
     *  Map task to shared memory segment
     */
    rc = sql32_attach_comseg ( cip );
    if ( rc != SP1CE_OK )
    {
		MSGD (( WRN_CONNECT_CLIENT_LOST ));
		/*
		 *  the action is: re-init task and wait for next connect !
		 */
		cip->ci_connect_time = 0 ; /* mark as not connected anymore */
		rc = tcb->state ;
		if ( rc != TSK_INACTIVE_EO00 ) rc = TSK_CONNECTWAIT_EO00 ;
		e75_release_connection ( SP1CE_NOTOK , FALSE ); /* no wait */
		tcb->state = rc ;
		return ( -1 );
    }

    KGS->connectedUserTasks += this_ukt->curr_task->type == TT_US_EO00 ? 1 : 0;

    comseg = cip->ci_comseg ;
    sql32_unlock_comseg ( cip );

	if ( (tcb->state != TSK_CONNECTWAIT_EO00) &&
		 (tcb->state != TSK_INACTIVE_EO00)         )
	{
		/*
		 *  The task could be already connected:
		 *  - On a late remote request, after the connect reservation
		 *    has just been cancelled.
		 *  - On an expropriation of TT_UT for local re-use.
		 */
#define WRN_TASK_ALREADY_CONNECTED  11845,WRN_TYPE,"COMMUNIC","Connect reject for Task T%d already connected (%d)"
		MSGD (( WRN_TASK_ALREADY_CONNECTED, tcb->index, tcb->state ));
		result = SP1CE_NOTOK ;
	}
	else
	{
		/*
		 *  Create the user task's stack.
		 */
		if (    (tcb->state == TSK_INACTIVE_EO00)
			 && (tcb->type  == TT_US_EO00) )
		{
			stack_avail = en88_InitDynPthreadTasks(tcb);
			if ( ! stack_avail )
			{
				MSGD (( WRN_CONNECT_NO_STACK_MEM, tcb->index ));
				result = SP1CE_TASKLIMIT ;
			}
		}
    }


    if ( cip->ci_state == CON_ABORTED ) /* PTS 1113931 */
    {
		MSGD (( WRN_RELEASE_TSK_BY_REASON, tcb->index, "connection timeout" ));
        result = SP1CE_TIMEOUT;
    }
    /* if state is set to CON_ABORTED now before set to CON_ESTABLISHED below it is silently ignored */

    sql32_lock_comseg ( cip , "e75_connect: 0x%08lx \n" );
    DBG1 (( MF__,"OLDcli pid %ld ref %d flg %d",
		(long) comseg->cs_client_pid , comseg->cs_client_ref ,
		comseg->cs_client_flag ));
    DBG1 (( MF__,"OLDsrv pid %ld ref %d flg %d sem %d",
		(long) comseg->cs_server_pid , comseg->cs_server_ref ,
		comseg->cs_server_flag , comseg->cs_server_semid ));
    SAPDB_memset ( cip->ci_request , 0 , cip->ci_max_data_size );
    comseg->cs_client_pid   = cip->ci_peer_pid ;
    comseg->cs_client_ref   = cip->ci_peer_ref ;
    comseg->cs_server_pid   = tcb->ukt->tid ;
    comseg->cs_server_ref   = cip->ci_my_ref ;
    comseg->cs_server_state = result ;
    comseg->cs_server_semid = tcb->ukt->semid ;
    comseg->cs_client_flag  = 2 ;
    comseg->cs_server_flag  = 1 ;
    DBG1 (( MF__,"client pid %ld ref %d flg %d",
		(long) comseg->cs_client_pid , comseg->cs_client_ref ,
		comseg->cs_client_flag ));
    DBG1 (( MF__,"server pid %ld ref %d flg %d sem %d",
		(long) comseg->cs_server_pid , comseg->cs_server_ref ,
		comseg->cs_server_flag , comseg->cs_server_semid ));
    DBG1 (( MF__,"shm %d off %ld adr 0x%08lx",
		comseg->cs_shmid , cip->ci_big_offset , (long) comseg ));
    sql32_unlock_comseg ( cip );

    /*
     *  Set the state before waking NETWORK.
     *  Otherwise, the first packet may arrive, while the state
     *  is still CON_CONNECTING.
     */
    cip->ci_state    = CON_ESTABLISHED ;

    /*
     *  Wake the application
     */
    DBG1 (( MF__,"waking application" ));
    rc = e75_wake_peer ( cip );
    if ( (rc != SP1CE_OK) || (result != SP1CE_OK) )
    {
		e74_release_comseg ( cip, tcb );
		e75_release_connection ( SP1CE_NOTOK , FALSE ); /* no wait */
		tcb->state = stack_avail ? TSK_CONNECTWAIT_EO00 : TSK_INACTIVE_EO00 ; 
		return ( -1 );
    }

    /*
     *  Log the connection establishment.
     */
    (void) sp77sprintf ( logbuf , sizeof(logbuf), "Connected  T%d" , tcb->index );
    rc = strlen ( logbuf );
    if ( cip->ci_peer_node[0] )
		(void) sp77sprintf ( logbuf + rc , sizeof(logbuf) - rc, " %s" , cip->ci_peer_node );
    else
		(void) sp77sprintf ( logbuf + rc , sizeof(logbuf) - rc, " local" );
    rc = strlen ( logbuf );
    if ( cip->ci_peer_dbname[0] )
    {
		(void) sp77sprintf ( logbuf + rc , sizeof(logbuf) - rc, ":%s" , cip->ci_peer_dbname );
		rc = strlen ( logbuf );
    }
    if ( cip->ci_remote_pid[0] )
		(void) sp77sprintf ( logbuf + rc , sizeof(logbuf) - rc, " %s" , cip->ci_remote_pid );
    else
		(void) sp77sprintf ( logbuf + rc , sizeof(logbuf) - rc, " %ld" , (long) cip->ci_peer_pid );

    MSGD (( INFO_CONNECTING_TASK, logbuf ));

	switch ( tcb->state )
	{
		case TSK_INACTIVE_EO00 : 
		case TSK_CONNECTWAIT_EO00 :
			/*
			CLR_CURR_TASK_STAT_COUNTER ;
			 */
			rc = 0 ;	/* means ok and return to task */
			break ; 
		default :
			MSGD ((IERR_CONNECT_UNEXP_STATE, tcb->state ));
			e75_release_connection ( SP1CE_NOTOK , FALSE ); /*no wait*/
			tcb->state = TSK_CONNECTWAIT_EO00 ; 
			rc = -1 ;
	}

    DBG1 (( MF__,"leaving %d", rc ));
	DBGOUT;
    return ( rc );
}
/* e75_connect_request */

/*
 *  =======================================================================
 */
