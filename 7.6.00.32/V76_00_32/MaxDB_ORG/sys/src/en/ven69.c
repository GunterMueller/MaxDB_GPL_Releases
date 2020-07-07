/*!
  @file           ven69.c
  @author         JoergM
  @brief          Client/Kernel RunTime: Accounting
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

#define MOD__ "ven69.c:"

#include "gen00.h"
#include "heo00.h"
&ifndef  APPLIC
#include "geo50_0.h" /* nocheck */
&endif
#include "gen500.h"
#include "heo69.h"

#if defined(I386) || defined(HP9)
#include        <limits.h>
#endif  /* I386 || HP9 */

/* CLK_TCK is the obsolete POSIX.1-1988 name for the CLOCKS_PER_SEC. */
#if defined(CLOCKS_PER_SEC) && !defined(CLK_TCK)
#define CLK_TCK CLOCKS_PER_SEC
#endif

/* imported functions */

extern  void                            vabort ();

#if ! ( _IBMR2 || OSF1 || FREEBSD )	/* already declared in include file */
#   ifdef HP9
    extern  clock_t                         times ();
#   else
    extern  long                            times ();
#   endif
#endif	/* _IBMR2 */

/* local data */

&ifdef  APPLIC
static  tsp00_Int4                        cputim_begin ;
static  tsp00_Int4                        *stack_begin ;
&endif

/*
 * =====================================================================
 */
/*ARGSUSED*/ /* taskid unused at application side */
void    vcinit ( PROCESS_ID                      taskid )
{
    struct tms                      timbuf ;

&ifdef  APPLIC
    (void) times ( & timbuf );
    cputim_begin = ( timbuf.tms_utime + timbuf.tms_stime ) *
						( 1000 / CLK_TCK );
&else   APPLIC
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    (void) times ( & timbuf );
    DBG1 (( MF__,"vcinit: called,    T%d \n", this_ukt->curr_task->index ));

    if ( taskid != this_ukt->curr_task->index )
    {
        MSGALL (( IERR_ACCOUNT_WRONG_TASK,this_ukt->curr_task->index ,"vcinit", (long) taskid ))
        vabort ( WRITE_CORE );
    }

    /*Reset statistic counter*/
    eo52ResetTaskStatistic(taskid);
    this_ukt->curr_task->cputim = ( timbuf.tms_utime + timbuf.tms_stime ) *
    						( 1000 / CLK_TCK );
    DBG1 (( MF__,"vcinit: returning \n" ));
&endif  APPLIC
}
/*
 * =====================================================================
 */

/*ARGSUSED*/ /* taskid unused at application side */
void    vccheck (
PROCESS_ID                      taskid ,
tsp00_Int4                      * pTime )
{
struct tms                      timbuf ;

&ifdef  APPLIC
    (void) times ( & timbuf );
    * pTime = ( timbuf.tms_utime + timbuf.tms_stime ) *
			 ( 1000 / CLK_TCK ) - cputim_begin ;
&else   APPLIC
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    (void) times ( & timbuf );
    DBG1 (( MF__,"vccheck: called,    T%d \n", this_ukt->curr_task->index ));

    if ( taskid != this_ukt->curr_task->index )
    {
        MSGALL (( IERR_ACCOUNT_WRONG_TASK,this_ukt->curr_task->index,"vccheck", (long) taskid ))
        vabort ( WRITE_CORE );
    }

    * pTime = ( timbuf.tms_utime + timbuf.tms_stime ) *
			 ( 1000 / CLK_TCK ) - this_ukt->curr_task->cputim ;

    DBG1 (( MF__,"vccheck: returning \n" ));
&endif  APPLIC
}

/*
 * ===========================================================================
 */

/*ARGSUSED*/ /* taskid unused at application side */
void    vioinit ( PROCESS_ID                      taskid )

{
&ifdef  APPLIC
&else   APPLIC
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    DBG1 (( MF__,"vioinit: called,    T%d \n", this_ukt->curr_task->index ));

    if ( taskid != this_ukt->curr_task->index )
    {
        MSGALL (( IERR_ACCOUNT_WRONG_TASK,this_ukt->curr_task->index,"vioinit", (long) taskid ))
        vabort ( WRITE_CORE );
    }

    this_ukt->curr_task->dirio = 0 ;

    DBG1 (( MF__,"vioinit: returning \n" ));
&endif  APPLIC
}

&ifndef  APPLIC
/*
 * ===========================================================================
 */

void    vmonitor (
PROCESS_ID                      taskid ,
tsp00_Int4                        * phys_ios ,
tsp00_Int4                        * suspends ,
tsp00_Int4                        * waits )
{
    if ( taskid > 0 && taskid <= KGS->pLastTaskCtrl->index )
    {
        struct TASK_TYPE              * tcb ;
        teo52_TaskStateStatistic      *pTaskStatistic ;
        tcb = KGS->pFirstTaskCtrl + ( taskid - 1 ) ;
        pTaskStatistic = &(tcb -> TaskStateStat) ;
        *suspends = (tsp00_Int4) ( pTaskStatistic->VsuspStat.TimeStat.ulCount );
        *waits    = (tsp00_Int4) ( pTaskStatistic->VwaitStat.TimeStat.ulCount );

        *phys_ios = (tsp00_Int4) ( pTaskStatistic->SelfIOReadStat.IOStat.TimeStat.ulCount  +
                                   pTaskStatistic->SelfIOWriteStat.IOStat.TimeStat.ulCount +
                                   pTaskStatistic->DevIOReadStat.IOStat.TimeStat.ulCount   +
                                   pTaskStatistic->DevIOWriteStat.IOStat.TimeStat.ulCount );
    }
}
&endif
/*
 * ===========================================================================
 */

&ifdef  APPLIC
void    vsinit ( )
{
    tsp00_Int4 cur_stack ;

    stack_begin = & cur_stack ;
}
&endif  APPLIC

/*
 * ===========================================================================
 */

&ifdef  APPLIC
void    vscheck ( tsp00_Int4 * maxstacksize )
{
    register long s = (long) ( (char*) stack_begin - (char*) & maxstacksize );
    *maxstacksize = (tsp00_Int4) ( (s < 0) ? -s : s );
}
&endif  APPLIC

/*
 * ===========================================================================
 */

/*
 *  Fill the remaning stack with a pattern.
 *  Leave the two-word chgstack()-test-pattern intact.
 *  Current function's additional stack requirements
 *  estimated as eight words.
 */

&ifdef  APPLIC
void    vsfill ( )
{
}
&endif  APPLIC

/*
 * ===========================================================================
 */

/*
 *  Return the amount of stack used since the last call to vsfill().
 */

&ifdef  APPLIC
void    vsscan ( tsp00_Int4 * maxstacksize )
{
}
&endif  APPLIC
