/*!
  @file           vos69k.c
  @author         RaymondR
  @brief          accounting
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


/*
 *  DEFINES
 */
#define MOD__  "VOS69KC : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */

#define QW_TO_DBL(_ulHigh,_ulLow) (((double)_ulHigh * 4294967296.) +      \
                                    (double)_ulLow)



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

VOID vcinit ( PROCESS_ID pid )
  {
  #undef  MF__
  #define MF__ MOD__"vcinit"
  #if defined (DEVELOP_CHECKS) || defined ( DEBUG )
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  #endif


  DBGIN_T (pTaskCtrl->ulTaskIndex);

  DBG3 ((MF__, "Current task Index %u", pTaskCtrl->ulTaskIndex))

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vcinit", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  /*Reset statistic counter*/
  eo52ResetTaskStatistic(pid);

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

/*
 * return an approximation of the time used by a given task
 *
 * -- since OS/2 provides no mechanism to measure the time (CPU)
 *    used by a single thread within a process, we compute an approximation
 *    as a percentage of the total time used by the tasks, based
 *    on the ratio of the number of times this task was dispatched to the
 *    total number of dispatches for the UKT.
 */

VOID vccheck ( PROCESS_ID  pid,
               INT4       *cputime)
  {
  #undef  MF__
  #define MF__ MOD__"vccheck"
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vccheck", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  if ( pUKT->DispatcherCalls )
    {
    *cputime = (INT4)((pUKT->UKTRunTimeMicroSeconds * pTaskCtrl->ulDispatcherCalls) / 
                       ( pUKT->DispatcherCalls * 1000 ));
    }

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID vioinit ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"vioinit"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vioinit", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  // -- reset disk IO counter
  eo52ResetIOStatistic(pid);

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/
VOID vmonitor ( PROCESS_ID pid,
                INT4       *phys_ios,
                INT4       *suspends,
                INT4       *waits )
  {
  #undef  MF__
  #define MF__ MOD__"vmonitor"
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC               pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC              pTaskCtrl = pUKT->pCTask;
  #else
   PTASK_CTRL_REC              pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  #endif
  teo52_TaskStateStatistic    *pTaskStatistic ;
  DBGPAS;

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vmonitor", pid ));
     ABORT();
     }
  #endif

  pTaskStatistic = &(pTaskCtrl -> TaskStateStat) ;

  *suspends = (INT4)( pTaskStatistic->VsuspStat.TimeStat.ulCount );

  *waits    = (INT4)( pTaskStatistic->VwaitStat.TimeStat.ulCount );

  *phys_ios = (INT4)( pTaskStatistic->SelfIOReadStat.IOStat.TimeStat.ulCount  +
                      pTaskStatistic->SelfIOWriteStat.IOStat.TimeStat.ulCount +
                      pTaskStatistic->DevIOReadStat.IOStat.TimeStat.ulCount   +
                      pTaskStatistic->DevIOWriteStat.IOStat.TimeStat.ulCount ) ;

  return;
  }
/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
