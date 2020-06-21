/*!
  @file           ven58.c
  @author         JoergM
  @brief          Kernel RunTime: vdiaginit
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

#define MOD__ "ven58.c:"
#include "gen00.h"
#include "heo00.h"
#include "geo002.h"
#include "gen50.h"
#include "gen500.h"
#include "heo58.h"
#include "hen40.h"

/* imported functions */

#include "heo46.h"
#include "geo001.h"
#include "geo57.h"
#include "geo50_0.h"
#include "heo54k.h"
#include "hen50.h" /* vabort () */

#if !defined(BIT64) && defined(O_LARGEFILE)
#define DEFAULT_OPEN_MODE_EN54 ( O_LARGEFILE )
#else
#define DEFAULT_OPEN_MODE_EN54 ( 0 )
#endif
/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"vdiaginit"
void    vdiaginit ( 
tsp00_Bool     *traceflag,
tsp00_Bool     *dumpflag,
tsp00_VFilename vtracename,
tsp00_Int4     *vtracesize,
tsp00_VFilename vdumpname
)
{
	ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
	int                             i , rc ;
	int                              fp ;
	tsp00_Bool                       ok ;
	char                            *buf;
	tsp00_ErrText                    vds_errtext;

	DBG1 (( MF__,"called,    T%ld \n", this_ukt->curr_task->index ));
	DBG1 (( MF__,"traceflag 0x%08lx \n", (long) traceflag ));
	DBG1 (( MF__,"dumpflag  0x%08lx \n", (long) dumpflag  ));
	DBG1 (( MF__,"tracename 0x%08lx \n", (long) vtracename  ));
	DBG1 (( MF__,"tracesize 0x%08lx \n", (long) vtracesize  ));
	DBG1 (( MF__,"dumpname  0x%08lx \n", (long) vdumpname  ));

	this_ukt->curr_task->state = TSK_VDIAGINIT_EO00 ;
	KGS->traceflag    = traceflag ;
	KGS->dumpflag     = dumpflag ;

	eo46CtoP ( vtracename , XPARAM(szTraceDevspace), sizeof(tsp00_DevName) );
	eo46CtoP ( vdumpname ,  XPARAM(szDumpFilename) , sizeof(tsp00_VFilename)  );

	/*
	 * vdiaginit also initializes the tracedevspace
	 */
	if ( ALLOC_MEM_EO57( (void **)&buf, XPARAM(ulDBPageSize)) != NO_ERROR_EO001 )
    {
		vabort ( WRITE_CORE );
    }
	SAPDB_memset ( buf , 0 , XPARAM(ulDBPageSize) );

    fp = open ( (const char *)XPARAM(szTraceDevspace), DEFAULT_OPEN_MODE_EN54|O_WRONLY|O_TRUNC|O_CREAT, 0660 );
	if ( fp < 0 )
	{
		MSGALL (( IERR_TRACEFILE_OPEN_ERR, XPARAM(szTraceDevspace), sqlerrs() ))
		vabort ( !WRITE_CORE );
	}
	DBG1 (( MF__,"opened trace device \n" ));
	for ( i = 0 ; i < (XPARAM(lTraceDevspaceSize) + 1); i ++ )
	{
		rc = write ( fp, buf , XPARAM(ulDBPageSize) );
		if ( rc != XPARAM(ulDBPageSize) )
		{
			MSGALL (( IERR_TRACEFILE_WRITE_ERR , rc, sqlerrs() ))
			vabort ( !WRITE_CORE );
		}
	}
	DBG1 (( MF__,"wrote empty blocks, count %d \n", i  ));
	(void) close ( fp );
	DBG1 (( MF__,"closed trace device \n" ));

    FREE_MEM_EO57(buf);

	/*
	 * vdiaginit also does vdevsize for the tracedevspace
	 */
	DBG1 (( MF__,"tracedevspace '%s' \n", XPARAM(szTraceDevspace) ));
	* vtracesize = XPARAM(lTraceDevspaceSize) ;

    DBG1 (( MF__,"now calling 'vdevsize (3.0)' \n" ));
	vdevsize ( 1, sp2dt_trace , vtracesize , vds_errtext, & ok );
	DBG1 (( MF__,"returned from 'vdevsize' \n" ));

	/*
	 * Use EBCDIC name
	 */
	if ( * vtracesize != XPARAM(lTraceDevspaceSize) )
	{
		MSGALL (( ERR_INCORRECT_TR_DEVSIZE, XPARAM(szTraceDevspace) )) ;
		vabort ( !WRITE_CORE );
	}

	if ( ! ok )
	{
		MSGALL (( ERR_VDEVSIZE_UNSUCCSESSFUL , XPARAM(szTraceDevspace) )) ;
		vabort ( !WRITE_CORE );
	}

	this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
	return ;
}
