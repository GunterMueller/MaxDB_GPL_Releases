/*!
  @file           ven32.c
  @author         JoergM
  @brief          RunTime: Communication Functions for Local Access
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

#define MOD__ "ven32.c:"

#include "gen00.h"
#include "vsp001c.h"
#include "gen32.h"
#include "hen40.h"
#include "gen41.h"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "gen42.h"
/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sql32_open_kernel_fifo"
int sql32_open_kernel_fifo ( 
char           *dbname ,
int			   *fdp ,
uid_t          *uidp ,
tsp00_ErrTextc  pErrText )
{
    int         rc ;
    struct stat statbuf ;
    tsp00_Pathc file ;

    DBG1 (( MF__,"sql32_kernel: dbname '%s' \n", dbname ));

    sql41_get_request_fifo_name ( (char *)file , (char *)dbname );
    DBG1 (( MF__,"sql32_kernel: fifo '%s' \n", file ));

    /*
     *  Check, if file is a fifo.
     */
    rc = RTE_save_stat ( (const char *)file , &statbuf );
    if ( rc == -1 )
    {
	    if ( errno == ENOENT )
	    {
	        DBG1 (( MF__,"sql32_kernel: cannot stat '%s': %s \n", file , sqlerrs() ));
	        en42FillErrText(pErrText, "database not running: no request pipe");
	        return ( SP1CE_START_REQUIRED );
	    }
	    else
	    {
	        DBG1 (( MF__, "sql32_kernel: cannot stat '%s': %s \n",
						    file , sqlerrs() ));
	        en42FillErrText(pErrText, "database not accesssible: stat on pipe (%d:%s)", errno, sqlerrs());
	        return ( SP1CE_NOTOK );
	    }
    }
    if ( ! S_ISFIFO(statbuf.st_mode) )
    {
	    DBG1 (( MF__, "sql32_kernel: not a fifo '%s' \n", sqlerrs() ));
	    en42FillErrText(pErrText, "database not accesssible: pipe is no pipe (%d)", (int)statbuf.st_mode);
	    return ( SP1CE_NOTOK );
    }
    DBG1 (( MF__,"sql32_kernel: fifo uid %ld \n", (long) statbuf.st_uid ));
    *uidp = statbuf.st_uid ;

    /*
     *  Test first, if a REQUESTOR is reading on the fifo.
     */
    rc = RTE_save_open ( (const char *)file , O_WRONLY | O_NONBLOCK );
    if ( rc == -1 )
    {
	    if ( errno == ENOENT )
	    {
	        DBG1 (( MF__,"sql32_kernel: cannot open '%s': %s \n", file , sqlerrs() ));
            en42FillErrText(pErrText, "database not accesssible: pipe does not exist" );
	        return ( SP1CE_START_REQUIRED );
        }
        else
        {
	        DBG1 (( MF__, "sql32_kernel: cannot open '%s': %s \n",
						    file , sqlerrs() ));
            en42FillErrText(pErrText, "database not accesssible: pipe open (%d:%s)", errno, sqlerrs() );
	        return ( SP1CE_NOTOK );
	    }
    }
    else
    {
        RTE_save_close ( rc );
    }

    /*
     *  This open would block, if the REQUESTOR did just close the fifo.
     */
    *fdp = RTE_save_open ( (const char *)file , O_WRONLY );
    if ( *fdp == -1 )
    {
	    if ( errno == ENOENT )
	    {
	        DBG1 (( MF__,"sql32_kernel: cannot open '%s': %s \n", file , sqlerrs() ));
            en42FillErrText( pErrText,
                             "database not accesssible: write pipe does not exist" );
	        return ( SP1CE_START_REQUIRED );
	    }
	    else
	    {
	        DBG1 (( MF__, "sql32_kernel: cannot open '%s': %s \n",
						    file , sqlerrs() ));
            en42FillErrText( pErrText, 
                             "database not accesssible: write pipe open(%d:%s)",
                             errno,
                             sqlerrs() );
	        return ( SP1CE_NOTOK );
	    }
    }
    DBG1 (( MF__,"sql32_kernel: fd %d \n", *fdp ));

    DBG1 (( MF__,"sql32_kernel: returning %d \n", SP1CE_OK ));
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sql32_open_reply_fifo"
int sql32_open_reply_fifo ( 
char			*dbname ,
pid_t			pid ,
tsp00_Int4		ref ,
char			*file ,
int			*fdp ,
tsp00_ErrTextc  pErrText )
{
    int				rc ;

    DBG1 (( MF__,"sql32_reply: dbname '%s' pid %ld ref %d \n",
			    dbname , (long) pid , ref ));

    sql41_get_reply_fifo_name ( file , dbname , pid , ref );
    DBG1 (( MF__,"sql32_reply: fifo '%s' \n", file ));

    rc = sql41_create_fifo ( file , 0666 );
    if ( rc == -1 )
    {
		en42FillErrText( pErrText, 
                         "Cannot create reply fifo (%d:%s)",
                         errno,
                         sqlerrs() );
		DBG1 (( MF__, "sql32_reply: %s '%s' \n", *errm , file ));
		return ( SP1CE_NOTOK );
    }

    /*
     *  This open does not block.
     */
    *fdp = RTE_save_open ( file , O_RDWR );
    if ( *fdp == -1 )
    {
		en42FillErrText( pErrText, 
                         "Cannot open reply fifo (%d:%s)",
                         errno,
                         sqlerrs() );
		DBG1 (( MF__, "sql32_reply: %s '%s': %s \n",
					*errm , file , sqlerrs() ));
		RTE_save_unlink ( file );
		return ( SP1CE_NOTOK );
    }
    DBG1 (( MF__,"sql32_reply: fd %d \n", *fdp ));

    DBG1 (( MF__,"sql32_reply: returning %d \n", SP1CE_OK ));
    return ( SP1CE_OK );
}

/*
 * ======================================================================
 */

#undef MF__
#define MF__ MOD__"sql32_attach_comseg"
int	sql32_attach_comseg ( 
connection_info		*cip )
{
    int				rc ;
    int				idx ;
    long			siz ;
    long			off ;
    ten_com_long   *offp ;
    char			*ptr ;
    comseg_header		*comseg ;
    struct shmid_ds		mbuf ;

    /*
     *  Check the received packet_cnt.
     */
    if ( (cip->ci_packet_cnt < 1) || (cip->ci_packet_cnt > MAX_SQL_PACKETS) )
    {
		DBG1 (( MF__, "ill packet count %d \n", cip->ci_packet_cnt ));
		return ( SP1CE_NOTOK );
    }

    if ( cip->ci_big_size > 0 )
    {
		siz = cip->ci_big_size ;
    }
    else
    {
		/*
		 *  Stat the shared memory segment to determine its size.
		 */
		rc = shmctl ( cip->ci_shmid , IPC_STAT , &mbuf );
		if ( rc == -1 )
		{
			DBG1 (( MF__, "sql32_att_comseg: shmctl (stat %d) error: %s\n",
								cip->ci_shmid , sqlerrs() ));
			return ( SP1CE_NOTOK );
		}
		siz = (long) mbuf.shm_segsz ;
		DBG1 (( MF__,"sql32_att_comseg: shmid %d size %ld \n", 
					cip->ci_shmid , siz ));
    }

    /*
     *  Check the received big_offset for range and alignment.
     */
    if (    (cip->ci_big_offset < 0)
         || (cip->ci_big_offset > siz - sizeof(comseg_header))
	 || (cip->ci_big_offset & (sizeof(comseg_header*)-1)) )
    {
		DBG1 (( MF__,"sql32_att_comseg: ill comseg offset %ld siz %ld \n",
						    cip->ci_big_offset , siz ));
		return ( SP1CE_NOTOK );
    }

    if ( cip->ci_big_comseg )
    {
		ptr = (char*) cip->ci_big_comseg ;
    }
    else
    {
		/*
		 *  Here, cip->ci_big_offset is in range.
		 *  Now, we have to attach the shared memory
		 *  before we can do further checks.
		 *  Except of FIXED_COMSEG, ci_loccomseg is NULL,
		 *  thus allowing to attach at a system-selected address.
		 */
		ptr = sql41_attach_shm ( cip->ci_shmid , (char*) cip->ci_loccomseg );
		if ( ! ptr )
		{
			return ( SP1CE_NOTOK );
		}
		DBG1 (( MF__,"sql32_att_comseg: attached shmid %5d at 0x%08lx \n",
					cip->ci_shmid , (long) ptr ));
    }

    if (    cip->ci_big_offset
	 && (((big_comseg_header*)ptr)->bc_size != siz) )
	{
		DBG1 (( MF__, "sql32_att_comseg: %s: shmstat %ld bc_size %ld \n",
				"inconsistent big comseg size" ,
				siz , ((big_comseg_header*)ptr)->bc_size ));
		if ( ! cip->ci_big_comseg )
		{
			(void) sql41_detach_shm ( &ptr );
		}
		return ( rc );
	}

    /*
     *  Check the offsets.
     */
    rc = SP1CE_OK ;
    comseg = (comseg_header*) (ptr + cip->ci_big_offset);
    off = comseg->cs_list_offset ;
    if ( comseg->cs_packet_cnt != cip->ci_packet_cnt )
    {
		DBG1 (( MF__,"sql32_att_comseg: ill packet cnt %d wanted %d \n",
					comseg->cs_packet_cnt , cip->ci_packet_cnt ));
		rc = SP1CE_NOTOK ;
    }
    else if (    (off < 0)
	      || (off >= siz - sizeof(long*) * cip->ci_packet_cnt)
	      || (off & (sizeof(long*)-1)) )
	{
		DBG1 (( MF__,"sql32_att_comseg: ill list offset %ld siz %ld \n",
									off , siz ));
		rc = SP1CE_NOTOK ;
	}
    else
	{
		offp = (ten_com_long *) (ptr + off);
		for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
		{
			/* packets must be 8-byte aligned. */
			if (    (*offp < 0)
			 || (*offp >= siz - cip->ci_packet_size)
			 || ((unsigned long) (ptr + *offp) & 7) )
			{
				DBG1 (( MF__, "sql32_att_comseg: ill pkt off %ld siz %ld \n",
										*offp , siz ));
				rc = SP1CE_NOTOK ;
				break ;
			}
			cip->ci_packet[idx] = (rte_header*) (ptr + *(offp ++));
		}
	}
    if ( rc != SP1CE_OK )
	{
		if ( ! cip->ci_big_comseg )
		{
			(void) sql41_detach_shm ( &ptr );
		}
		return ( rc );
	}

    if ( cip->ci_big_offset )
    {
		cip->ci_big_comseg = (big_comseg_header*) ptr ;
		cip->ci_big_size   = siz ;
    }
    else
    {
		/* these values might have been temporarily assigned. */
		cip->ci_big_comseg = 0 ;
		cip->ci_big_size   = 0 ;
    }
    cip->ci_comseg = comseg ;
    cip->ci_request = cip->ci_packet[0];
    cip->ci_reply   = cip->ci_request ; /* should always point inside packet */

    DBG1 (( MF__,"sql32_att_comseg: big_off  %7ld comseg 0x%08lx \n",
	    cip->ci_big_offset , (long) cip->ci_comseg ));
    DBG1 (( MF__,"sql32_att_comseg: list_off %7ld list   0x%08lx \n",
	    comseg->cs_list_offset , (long) (ptr + comseg->cs_list_offset) ));
    DBG1 (( MF__,"sql32_att_comseg: pack_off %7ld packet 0x%08lx \n",
	    (long) cip->ci_request - (long) ptr , (long) cip->ci_request ));

    return ( SP1CE_OK );
}

/*
 *  ======================================================================
 */
#undef MF__
#define MF__ MOD__"sql32_lock_comseg"
void sql32_lock_comseg (  connection_info		*cip ,
                          char	const * msg )
{
    /*
     *  This is coded as a function to avoid
     *  instruction reordering at the point where it is called.
     */
    if ( cip->ci_protocol == PROT_BIGSHM_EO003 )
	{
	    for ( ; 
#if defined(_IBMR2) || defined(PA11) || defined(PA20W)
			  e49_lock_int4_4fold ( &cip->ci_comseg->cs_exclusive , 1 ) ;
#else
		      RTESys_TestAndLock( (RTE_Lock*)&cip->ci_comseg->cs_exclusive ) ;
#endif  /* _IBMR2 || PA11 || PA20W */
 			)
	    {
            /* Prevent alreadsy crashed client from locking kernel.... */
            if ( cip->ci_state == CON_ABORTED ) /* PTS 1124429 */
            {
#if defined(_IBMR2) || defined(PA11) || defined(PA20W)
	            e49_unlock_int4_4fold ( &cip->ci_comseg->cs_exclusive , 1 ) ;
#else
		        RTESys_InitLock( (RTE_Lock*)&cip->ci_comseg->cs_exclusive );
#endif
            }
	        DBG1 (( MF__, msg , (long) &cip->ci_comseg->cs_exclusive ));
			(void) sleep ( 0 );
		}
	}
}

/*
 *  ======================================================================
 */
void sql32_unlock_comseg ( 
connection_info		*cip )
{
    /*
     *  This is coded as a function to avoid
     *  instruction reordering at the point where it is called.
     */
    if ( cip->ci_protocol == PROT_BIGSHM_EO003 )
    {
#if defined(_IBMR2) || defined(PA11) || defined(PA20W)
	    e49_unlock_int4_4fold ( &cip->ci_comseg->cs_exclusive , 1 ) ;
#else
		RTESys_Unlock( (RTE_Lock*)&cip->ci_comseg->cs_exclusive );
#endif
    }
}
