/*!
  @file           vos52k.c
  @author         RaymondR
  @brief          DATE/TIME QUERY
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "heo52.h"
#include "RunTime/System/RTESys_Time.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS52KC : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 *  EXPORTED VARIABLES
 */



/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static SAPDB_UInt4      globStartMilliSec;
static SAPDB_UInt8      globMilliSec;

typedef tsp00_C8					tsp00_Date;
typedef tsp00_C8					tsp00_Time;
 
/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

void vdattime (tsp00_Date pDate, tsp00_Time pTime )
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

/*------------------------------*/

/* vtimestamp now in veo52.c */

/*------------------------------*/


VOID  vclock (INT4  *Sec, INT4  *MicroSec )
  {
  #undef  MF__
  #define MF__ MOD__"vclock"

  static LONG         lExclusive;
  SAPDB_UInt4         CurrTickCount;
  SAPDB_UInt8         MilliSeconds;

  DBGPAS;

  WAIT_UNTIL_ALONE(&lExclusive);
  MilliSeconds = globMilliSec;   // --- get global millisecond value. Use 'interlock' because
                                 //     64-bit operation are not atomic on 32-bit platforms.
  CLEARLOCK( &lExclusive );

  CurrTickCount = GetTickCount();

  if ( (SAPDB_UInt4)MilliSeconds > CurrTickCount ) // overrun ?
    MilliSeconds += 0x100000000I64;

  MilliSeconds = ( MilliSeconds & ~0xFFFFFFFFI64 ) | CurrTickCount;


  WAIT_UNTIL_ALONE(&lExclusive);

  if ( MilliSeconds > globMilliSec ) // has another thread changed this value?
    globMilliSec = MilliSeconds;     // no - update global millisecond value
  else
    MilliSeconds = globMilliSec;

  CLEARLOCK( &lExclusive );

  *Sec      = (INT4) ((MilliSeconds - globStartMilliSec) / 1000);
  *MicroSec = (INT4)(((MilliSeconds - globStartMilliSec) % 1000) * 1000);
  }

/*------------------------------*/

VOID sql52k_vclock_init (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql52k_vclock_init"

  DBGPAS;

  globStartMilliSec  = GetTickCount();
  }

/*------------------------------*/

tsp00_Longreal vTimeZoneDelta()
{
    return ((tsp00_Longreal)RTESys_TimeZoneDelta())/3600.; /* convert seconds in hours */
}

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
