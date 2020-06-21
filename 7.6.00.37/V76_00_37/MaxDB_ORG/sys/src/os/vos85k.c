/*!
  @file           vos85k.c
  @author         RaymondR
  @brief          CLOCK - Thread
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

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS85KC : "
#define MF__   MOD__"UNDEFINED"


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
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


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID _System sql85k_clock ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql85k_clock"
  APIRET                         rc = NO_ERROR;

  DBGIN;

  __try 
   {
#  if defined(_FASTCAP)
    CAPNameThread ("Clock");
#  endif

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( kgs.Clock.ThrdCtrlHeader.lThrdPrio,
                           kgs.Clock.ThrdCtrlHeader.Tid,
                           kgs.Clock.ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, kgs.Clock.ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u",
                    kgs.Clock.ThrdCtrlHeader.Tid));
      ABORT();
      }

    MSGD (( INFO_KERNEL_THREAD_STARTED, "CLOCK" ));

    // --- set thread control info
    kgs.Clock.ThrdCtrlHeader.ulStartTime = time (NULL);
    kgs.Clock.ThrdCtrlHeader.ThrdState   = KT_RUNNING;

    kgs.ulCurrTime  = time (NULL);

    //
    // --- loop forever
    //
    //     Set the time in units of approximatly 1/250 seconds
    //     (Used by 'vccheck' to compute CPU time) and
    //     gets the the current time.
    //
    for (;;)
      {
      kgs.ulCurrTime  = time (NULL);
      kgs.Clock.ThrdCtrlHeader.ThrdState   = KT_SLEEPING;
      SLEEP( 500 );
      kgs.Clock.ThrdCtrlHeader.ThrdState   = KT_RUNNING;
      }
    }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  MSGD (( INFO_KERNEL_THREAD_STOPPED, "CLOCK" ));

  DBGOUT;
  EXITPROCESS(0);
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

/*
 * =============================== END ========================================
 */
