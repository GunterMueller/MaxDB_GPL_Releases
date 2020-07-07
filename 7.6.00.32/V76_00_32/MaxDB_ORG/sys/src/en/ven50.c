/*!---------------------------------------------------------------------
  @file           ven50.c
  @author         JoergM
  @brief          Kernel Task vabort routine and exception handling
  @see            example.html ...

\if EMIT_LICENCE
\endif
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hen50.h"

#include "geo50_0.h"
#include "geo007_1.h"
#include "heo07.h"
#include "heo670.h"
/* PTS 1112575 */
#include "geo002.h"
#include "gen81.h"
#include "gen71.h"
#include <sys/wait.h>
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

volatile tsp00_Bool en50NoCoreDumpWanted = FALSE;
volatile tsp00_Bool en50_neverStop = true;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void en50_DumpAk ( ten50_UKT_Control * this_ukt );
extern void Msg_RegistryDump();

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*
 * ==========================================================================
 */
extern void en54DumpNewRteDumpOnly( void );

#undef  MF__
#define MF__ MOD__"vabort"
void    vabort ( tsp00_Bool writeCore )
{
  ten50_UKT_Control * this_ukt;
  if( NULL == KGS)
  {
    this_ukt = NULL;
  }
  else
  {
    this_ukt = THIS_UKT_CTRL;
  }


  DBGIN;

  /* PTS 1106401 */
  eo670_CTraceStack(); /* First action, Stack backtrace in knldiag ! */

  /* Try to call debugger whenever possible... */
  (void)en81CallDebugger();

  if ( KGS           != NULL
    && KGS->first_kp != NULL
    && KGS->last_kp  != NULL )
  {
    en54DumpNewRteDumpOnly();

    Msg_RegistryDump();

    if ( this_ukt      != NULL )
    {
    ten50_UKT_Control * ukt;

	/* PTS 1112575 */
    en50_DumpAk ( this_ukt );

    for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
    {
      if ( ukt == this_ukt )
      {
        if ( this_ukt->curr_task != NULL )
        {
	  MSGALL (( ERR_KERNEL_VABORT_EXEPT_CHK , "Task", this_ukt->curr_task->index ));
        }
        else
        {
	  MSGALL (( ERR_KERNEL_VABORT_EXEPT_CHK , "Task", 0 ));
        }

        if ( (KGS->state != SERVER_STOP)
          && (KGS->state != SERVER_KILL)  )
        {
          /* This should avoid overwriting the core which corresponds to the */
          /* real source of the problem. */
          en50NoCoreDumpWanted = TRUE;
          /*
           * Trigger killing of database and try to resume tracewriter
           */
          (void)en81_kill_database ();
        }

        /*
         * Dispatcher will terminate the UKT and shutdown database
         */
        GOTO_DISP (&this_ukt);

        break;
      }
    }
    }
  }

  MSGALL (( ERR_KERNEL_VABORT_EXEPT )) ;

  /* We are one of several kernel processes. If the kernel seems to be */
  /* running till now, abort immediately with core - use 'sqlabort'. */
  if (  KGS == NULL 
     || (   (KGS->state != SERVER_STOP)
         && (KGS->state != SERVER_KILL)  ) )
  {
    /* This should avoid overwriting the core which corresponds to the */
    /* real source of the problem. */
    en50NoCoreDumpWanted = TRUE;
    /*
     * Trigger killing of database and try to resume tracewriter
     */
    (void)en81_kill_database ();
  }

  if (      KGS != NULL
    && this_ukt != NULL
    && this_ukt != (ten50_UKT_Control *) & KGS->coord )
  {
    /* Wake up coordinator to shutdown database */
    en81_NotifyCoordinator(0);

    /* Do not terminate thread... */
    while(en50_neverStop)
    {
        sqlyieldthread();
    }
  }
  else
  {
    /* Coordinator is the only one, who is allowed to exit ! */
    exit(1);
  }

  /* NOTREACHED */
  DBGOUT;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

/* PTS 1112575 */
static void en50_DumpAk ( ten50_UKT_Control * this_ukt )
{
    struct TASK_TYPE * pCurrTask = this_ukt->curr_task;

    if ( pCurrTask             && 
         pCurrTask->AcvPointer &&
         pCurrTask->AkDumpProc &&
         XPARAM(fAKDumpAllowed) )
    {
        MSGD(( INFO_CALLING_AK_DUMP, pCurrTask->index ));
        pCurrTask->AkDumpProc(pCurrTask->AcvPointer) ;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/