/*!
  @file           vos64k.c
  @author         RaymondR
  @brief          vprio
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
#include "geo002.h"
#include "gos003.h"
#include "gos74.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS64KC : "
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

VOID prio_task               (  PUKT_CTRL_REC       pUKT ,
                                PROCESS_ID          pid,
                                BYTE                prio,
                                BOOLEAN             set_prio ) ;

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID vdelay_lw ( PROCESS_ID pid )
  {
  #undef  MF__
  #define MF__ MOD__"vdelay_lw"
  #ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT         = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl    = pUKT->pCTask;
  #else
   PTASK_CTRL_REC                pTaskCtrl    = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT         = pTaskCtrl->pUKT;
  #endif
  ULONG                          ulCurrDelay;
  PDLQ_REC                       pRequest;
  static BOOL                    fDelayActive = FALSE;
  static ULONG                   ulLastDelay;
  SAPDB_UInt4                    actInComQueue = NumberOfCommunicationQueueElements(pUKT);

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  if ( fDelayActive ||
       (XPARAM(lDelayLW) &&
       ( actInComQueue + pUKT->ulActInRexQueue > 4 )) )
    {
    pRequest             = GetTaskDLQElement1( pTaskCtrl );
    pRequest->ulReqType  = REQ_RESCHEDULE;
    pRequest->pTaskCtrl  = pTaskCtrl;
    pTaskCtrl->TaskState = TSK_SLEEP_EO00;

    DBG5 (( MF__, "vorher : act_com_elem = %d, act_rex_elem = %d\n",
            actInComQueue, pUKT->ulActInRexQueue )) ;

    if ( !fDelayActive )
      {
      if ( actInComQueue > 10 )
        ulLastDelay = ulCurrDelay = 1 ;
      else
        ulLastDelay = ulCurrDelay = pUKT->ulActInRexQueue > 10 ? 10 : 0;
      }
    else
      {
      ulCurrDelay = ulLastDelay > 1 && pUKT->ulActInRexQueue < ulLastDelay ?
                    0 : ulLastDelay ;
      }

    sql74k_RUN_enqu ( pUKT, pTaskCtrl, pTaskCtrl, pRequest, BASE_PRIO_REX );

    fDelayActive = TRUE;

    GOTO_DISP(&pUKT);

    fDelayActive = FALSE;

    DBG5 (( MF__, "nachher : act_com_elem = %d, act_rex_elem = %d\n",
            actInComQueue, pUKT->ulActInRexQueue )) ;

    if ( !fDelayActive )
      {
      }
    }

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return ;
  }


/*------------------------------*/

VOID vprio ( PROCESS_ID    pid,
             BYTE          prio,
             BOOLEAN       set_prio )
  {
  #undef  MF__
  #define MF__ MOD__"vprio"
  PUKT_CTRL_REC                 pUKT         = THIS_UKT_CTRL;
  prio_task ( pUKT, pid, prio, set_prio ) ;
  }


/*------------------------------*/


VOID v2prio ( PROCESS_ID    pid,
              BYTE          prio,
              BOOLEAN       set_prio ,
              int           root,
              int           leaf,
              int           locktype )
  {
  #undef  MF__
  #define MF__ MOD__"v2prio"

  PUKT_CTRL_REC                 pUKT         = THIS_UKT_CTRL;
  pUKT->pCTask->lWaitForRoot = root ;
  prio_task ( pUKT, pid, prio, set_prio ) ;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static VOID prio_task (  PUKT_CTRL_REC pUKT ,
                         PROCESS_ID    pid,
                         BYTE          prio,
                         BOOLEAN       set_prio )

  {
  #undef  MF__
  #define MF__ MOD__"prio_task"
  PTASK_CTRL_REC                pTaskCtrl ;

  if ( ! XPARAM(lPrioFactor) )
    return ;

  if ( set_prio )
    pUKT->pCTask->lWaitForTask = pid ;

  pTaskCtrl    = &kgs.pFirstTaskCtrl[pid - 1];
  if ( ( !pTaskCtrl->PrioFlag         && !set_prio ) ||
       ( pTaskCtrl->PrioFlag == prio  &&  set_prio ) )
    return ;

    if ( set_prio )
      { if ( prio == 2 )
          { pTaskCtrl->SavePrioFlag = pTaskCtrl->PrioFlag ;
            DBG5 (( MF__, "Saving PRIO %d, NEW_PRIO is %d\n",
                    pTaskCtrl->SavePrioFlag, prio ));
          }
        else
          if ( pTaskCtrl->PrioFlag == 2 )
            { pTaskCtrl->SavePrioFlag = prio ;
              DBG5 (( MF__, "Saving PRIO %d, ACT_PRIO is %d\n",
                      pTaskCtrl->SavePrioFlag, pTaskCtrl->PrioFlag ));
              return ;
            }
        }
      else               /* "set_prio" is FALSE, looking for "SavePrioFlag" */
        if ( pTaskCtrl->SavePrioFlag )
          { pTaskCtrl->PrioFlag = pTaskCtrl->SavePrioFlag;
            pTaskCtrl->SavePrioFlag= 0 ;
            return ;
          }

    pTaskCtrl->PrioFlag = set_prio ? prio : 0 ;
    if ( set_prio )
      { 
        pTaskCtrl->PrioStat.Total.ulTotalCount++ ;
        pTaskCtrl->PrioStat.Resetable.ulTotalCount++ ;
        if ( pTaskCtrl != pUKT->pCTask )
            { 
/*
              MSGD (( 0, INFO_TYPE, "QueLocation %d", pTaskCtrl->QueLocation));
*/
              switch ( pTaskCtrl->QueLocation )
                { case TASK_IS_IN_NON_QUEUE_EO00  : break ;
                  case TASK_IS_IN_UKT_QUEUE_EO00 :
                       pUKT->ulImmediateDispatch++;
                       break ;
                  case TASK_IS_IN_RUN_QUEUE_EO00 :
                       pTaskCtrl->ulDynPrio += prio * XPARAM(lPrioFactor) ;
                       pUKT->ulImmediateDispatch++;
                       break ;
                }
              if ( pTaskCtrl->pUKT != pUKT )
              {
                pTaskCtrl->PrioStat.Total.ulOtherUkt++ ;
                pTaskCtrl->PrioStat.Resetable.ulOtherUkt++ ;
              }
            }
      }

  }

/*
 * =============================== END ========================================
 */
