/*!
  @file           ven52.c
  @author         JoergM
  @brief          Kernel RunTime: date_time
  @see            


                vclock, vdattime, vtimestamp

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

#define MOD__ "ven52.c:"

#include "gen00.h"
#include "heo00.h"
#include "geo50_0.h"

/* exported functions */

#include "gen52.h"
#include "geo007_1.h"
#include "RunTime/System/RTESys_Time.h"
#include "RunTime/System/RTESys_MicroTime.h"

#define EN52_CENTURY_ADJUST 1900
#define EN52_MICROSECONDS_PER_SECOND (1000000U)
/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"en52Localtime_r"
externC struct tm *en52Localtime_r(const time_t *clock, struct tm *res)
{
    return localtime_r(clock, res);
}

#undef MF__
#define MF__ MOD__"en52GMTime_r"
static struct tm *en52_GMTime_r(const time_t *clock, struct tm *res)
{
    return gmtime_r(clock, res);
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"e52_init_time"
externC	void	e52_init_time ( )
{
    time_t startTime;
    /*
     *  Get start_time in seconds
     */
    for ( KGS->current_time = KGS->start_time = startTime = time((time_t *)0); KGS->start_time == startTime; )
    {
        KGS->current_time = KGS->start_time = time((time_t *)0);
    }
    /* Initialize microsecond timer now. This allows a smaller jitter for calculating actual real time */
    RTESys_InitMicroSecTimer();
}


/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"vclock"
externC	void    vclock ( 
tsp00_Int4			*sec ,
tsp00_Int4			*usec )
{
#ifdef DEBUG_RTE
ten50_UKT_Control *this_ukt = THIS_UKT_CTRL;
#endif

    SAPDB_UInt8 microSeconds = RTESys_MicroSecTimer();

    /*
     *  Return the time since the start of the database.
     */
    *sec =  (tsp00_Int4)(microSeconds / EN52_MICROSECONDS_PER_SECOND);
    *usec = (tsp00_Int4)(microSeconds % EN52_MICROSECONDS_PER_SECOND);

#ifdef DEBUG_RTE
    DBG1 (( MF__,"T%d sec %ld usec %ld \n", (int) this_ukt->curr_task->index ,
		 (long) *sec , (long) *usec ));
#endif
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"vdattime"
externC	void    vdattime ( 
tsp00_Date pDate ,
tsp00_Time pTime )
{
#undef  MF__
#define MF__ MOD__"vdattime"
    RTESys_Timestamp timeBuffer;
    SAPDB_Char *timeStamp = RTESys_FillProtocolTimeStamp(timeBuffer);

    DBGPAS;

    pDate[0] = timeStamp[SAPDB_TIMESTAMP_YEAR_OFFSET];
    pDate[1] = timeStamp[SAPDB_TIMESTAMP_YEAR_OFFSET+1];
    pDate[2] = timeStamp[SAPDB_TIMESTAMP_YEAR_OFFSET+2];
    pDate[3] = timeStamp[SAPDB_TIMESTAMP_YEAR_OFFSET+3];
    pDate[4] = timeStamp[SAPDB_TIMESTAMP_MONTH_OFFSET];
    pDate[5] = timeStamp[SAPDB_TIMESTAMP_MONTH_OFFSET+1];
    pDate[6] = timeStamp[SAPDB_TIMESTAMP_DAY_OFFSET];
    pDate[7] = timeStamp[SAPDB_TIMESTAMP_DAY_OFFSET+1];

    pTime[0] = '0';
    pTime[1] = '0';
    pTime[2] = timeStamp[SAPDB_TIMESTAMP_HOUR_OFFSET];
    pTime[3] = timeStamp[SAPDB_TIMESTAMP_HOUR_OFFSET+1];
    pTime[4] = timeStamp[SAPDB_TIMESTAMP_MINUTE_OFFSET];
    pTime[5] = timeStamp[SAPDB_TIMESTAMP_MINUTE_OFFSET+1];
    pTime[6] = timeStamp[SAPDB_TIMESTAMP_SECOND_OFFSET];
    pTime[7] = timeStamp[SAPDB_TIMESTAMP_SECOND_OFFSET+1];
}

/*-------------------------------------------*/
/* vtimestamp now in veo52.c */
/*-------------------------------------------*/

externC tsp00_Longreal vTimeZoneDelta()
{
    return ((tsp00_Longreal)RTESys_TimeZoneDelta())/3600.; /* convert seconds in hours */
}
