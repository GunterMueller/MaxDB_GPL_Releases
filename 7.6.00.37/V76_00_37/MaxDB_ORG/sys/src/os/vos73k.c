/*!
  @file           vos73k.c
  @author         RaymondR
  @brief          QUEUE Handling
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

#include "RunTime/System/RTESys_MemoryBarrier.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS73KC : "
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

VOID sql73k_IO_queue_init ( PIO_QUEUE_REC pIO_QueElem, LONG lNum )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_IO_queue_init"
  LONG            i;
  PIO_QUEUE_REC   pIO_TmpQueElem;

  DBGIN;
  DBG3 (( MF__, "pIO_QueElem: 0x%08x - lNum: %ld ", pIO_QueElem, lNum ));

  if ( lNum > 0 )
    {
    memset ( pIO_QueElem, 0, sizeof(IO_QUEUE_REC) * lNum );

    // --- create a circular list !!
    for ( pIO_TmpQueElem = pIO_QueElem, i = 0 ; i < lNum ; i ++ )
      {
      pIO_TmpQueElem->pNextElem      = pIO_TmpQueElem + 1;
      pIO_TmpQueElem->pPreviousElem  = pIO_TmpQueElem - 1;
      pIO_TmpQueElem ++;
      }

    pIO_TmpQueElem --;
    pIO_QueElem->pPreviousElem   = pIO_TmpQueElem;
    pIO_TmpQueElem->pNextElem    = pIO_QueElem;
    }
  else
    {
    pIO_QueElem->pNextElem       = NULL;
    pIO_QueElem->pPreviousElem   = NULL;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql73k_UKT_queue_init ( PUKT_QUEUE_HEAD_REC  pUKT_QueHead,
                             PUKT_QUEUE_REC       pUKT_QueElem,
                             LONG                 lNum )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_UKT_queue_init"
  LONG            i;
  PUKT_QUEUE_REC  pUKT_TmpQueElem;

  DBGIN;
  DBG3 (( MF__, "pUKT_QueElem: 0x%08x - lNum: %ld ", pUKT_QueElem, lNum ));

  if ( lNum > 0 )
    {
    // --- create a circular list !!
    for ( pUKT_TmpQueElem = pUKT_QueElem, i = 0; i < lNum; i ++ )
      {
      pUKT_TmpQueElem->pRequest      = 0;
      pUKT_TmpQueElem->pNextElem     = pUKT_TmpQueElem + 1;
      pUKT_TmpQueElem->pPreviousElem = pUKT_TmpQueElem - 1;
      pUKT_TmpQueElem ++;
      }

    pUKT_TmpQueElem --;
    pUKT_QueElem->pPreviousElem   = pUKT_TmpQueElem ;
    pUKT_TmpQueElem->pNextElem    = pUKT_QueElem;

    pUKT_QueHead->pFirstFreeElem  = pUKT_QueElem;
    pUKT_QueHead->pLastUsedElem   = pUKT_QueElem;
    pUKT_QueHead->lExclusive      = 0;
    }
  else
    {
    pUKT_QueHead->pFirstFreeElem  = NULL;
    pUKT_QueHead->pLastUsedElem   = NULL;
    pUKT_QueHead->lExclusive      = 0;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

/*
 *  Enter queue element in UKT queue
 *  this is the a enque-procedure that may be used by other
 *  threads.
 */
VOID sql73k_UKT_enqu ( PUKT_CTRL_REC pUKT, PTASK_CTRL_REC pReqTaskCtrl, PDLQ_REC pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_UKT_enqu"
  PUKT_QUEUE_HEAD_REC  pUKT_QueHead;

  DBGPAS;

  DBG4 (( MF__, "Send pRequest 0x%x, (type %u) to TID:%u, [T:0x%03u]",
          pRequest,
          pRequest->ulReqType,
          pUKT->ThrdCtrlHeader.Tid,
          pReqTaskCtrl->ulTaskIndex ));

  pRequest->pTaskCtrl = pReqTaskCtrl;
  pUKT_QueHead        = &pUKT->UKTQue;

  WAIT_UNTIL_ALONE( &pUKT_QueHead->lExclusive );

  // --- we are alone now
  pUKT_QueHead->pFirstFreeElem->pRequest = pRequest;
  pUKT_QueHead->pFirstFreeElem           = pUKT_QueHead->pFirstFreeElem->pNextElem;

  CLEARLOCK( &pUKT_QueHead->lExclusive );


  pReqTaskCtrl->QueStat.Total.ulUKTQueCount++;
  pReqTaskCtrl->QueStat.Resetable.ulUKTQueCount++;
  pReqTaskCtrl->QueLocation = TASK_IS_IN_UKT_QUEUE;

  return;
  }

/*------------------------------*/

/*
 *  Enter queue element in UKT queue
 *  with no task as receiver
 */

VOID sql73k_UKT_enqu_no_task ( PUKT_CTRL_REC   pUKT, PDLQ_REC pRequest )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_UKT_enqu_no_task"
  PUKT_QUEUE_HEAD_REC  pUKT_QueHead;

  DBGPAS;

  pRequest->pTaskCtrl = NULL;
  pUKT_QueHead        = &pUKT->UKTQue;

  WAIT_UNTIL_ALONE( &pUKT_QueHead->lExclusive );

  // --- we are alone now
  pUKT_QueHead->pFirstFreeElem->pRequest = pRequest;
  pUKT_QueHead->pFirstFreeElem           = pUKT_QueHead->pFirstFreeElem->pNextElem;

  CLEARLOCK( &pUKT_QueHead->lExclusive );

  return;
  }

/*------------------------------*/

VOID  sql73k_DLQ_init ( PDLQ_HEAD_REC pDLQHead )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_DLQ_init"

  DBGPAS;
  pDLQHead->pFirstFreeElem  = (PDLQ_REC) NULL;
  pDLQHead->pLastUsedElem   = (PDLQ_REC) NULL;

  pDLQHead->lExclusive = 0 ;
  }

/*------------------------------*/

VOID sql73k_UToU_queue_init ( PUKT_TO_UKT_QUEUE_HEAD_REC  pUToUQueHead,
                              PUKT_TO_UKT_QUEUE_REC       pUToUs,
                              LONG                        lNum )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_UToU_queue_init"
  INT                           i;
  PUKT_TO_UKT_QUEUE_REC         pUToUQueElem;

  DBGPAS;

  memset ( pUToUQueHead, 0, sizeof(UKT_TO_UKT_QUEUE_HEAD_REC) );

  /*
   * create a circular list !!
   */

  if (( pUToUs != NULL ) && ( lNum > 0 ))
    {
    for ( pUToUQueElem = pUToUs, i = 0; i < lNum; i++ )
      {
      pUToUQueElem->pFuncToExecute = sql74k_remote_func_error ;
      pUToUQueElem->pNextElem      = pUToUQueElem + 1;
      pUToUQueElem++;
      }

    pUToUQueElem--;
    pUToUQueElem->pNextElem = pUToUs;
    }
  else
    pUToUs = NULL;

  pUToUQueHead->pUToUs    = pUToUs;
  pUToUQueHead->pWork     = pUToUs;
  pUToUQueHead->pRequest  = pUToUs;

  return;
  }

/*------------------------------*/

VOID sql73k_UToU_enqu ( PUKT_CTRL_REC   pUKT,
                        PTASK_CTRL_REC  pFromTaskCtrl,
                        PUKT_CTRL_REC   pRemoteUKTCtrl,
                        PTASK_CTRL_REC  pRemoteTaskCtrl,
                        PFNREMOTEFUNC   pFuncToExecute,
                        void*           pFuncParam )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_UToU_enqu"
  PUKT_TO_UKT_QUEUE_REC         pUToUQueElem;
  PUKT_TO_UKT_QUEUE_HEAD_REC    pUToUQueHead;

  DBGPAS_T (pFromTaskCtrl->ulTaskIndex);

  DBG4 (( MF__, "[T:0x%03u] Send execute request to TID:%u, [T:0x%03u]",
          pFromTaskCtrl->ulTaskIndex,
          pRemoteUKTCtrl->ThrdCtrlHeader.Tid,
          pRemoteTaskCtrl->ulTaskIndex ));

  pUToUQueHead  = &pRemoteUKTCtrl->pUToUQueHeads[ pUKT->ulUKTIndex - 1 ];

  while ( pUToUQueHead->pRequest->pFuncToExecute != sql74k_remote_func_error ||
          pUToUQueHead->pRequest->pTaskCtrl )
    MSGD (( 19999, INFO_TYPE, "TASKING ","UToUQueue is full (%d)",
             pRemoteTaskCtrl->ulTaskIndex ));

  pUToUQueElem                   = pUToUQueHead->pRequest;
#ifdef SRCH_HAENGER
  pRemoteUKTCtrl->ulReqInUkt     = pUToUQueElem;
#endif
  pUToUQueElem->pFuncToExecute   = pFuncToExecute;
  pUToUQueElem->pFuncParam       = pFuncParam;
  pUToUQueElem->pFromTaskCtrl    = pFromTaskCtrl;
  RTESys_WriteMemoryBarrier();
  pUToUQueElem->pTaskCtrl        = pRemoteTaskCtrl;
  pUToUQueHead->pRequest         = pUToUQueElem->pNextElem;


  pRemoteTaskCtrl->QueStat.Total.ulUToUQueCount++;
  pRemoteTaskCtrl->QueStat.Resetable.ulUToUQueCount++;
  if ( pRemoteTaskCtrl->PrioFlag && XPARAM(fDynDispQueSrch) )
    pRemoteUKTCtrl->ulImmediateDispatch++;

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID  sql73k_tlist_init ( PTASK_LIST_HEAD_REC  pTaskList )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_init"

  DBGPAS;

  pTaskList->pFirstInTaskList  = NULL;
  pTaskList->lExclusive        = 0;
  }

/*------------------------------*/

VOID sql73k_tlist_dequ_task_nolock ( PTASK_LIST_HEAD_REC  pTaskList,
                                     PTASK_CTRL_REC       pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_dequ_task_nolock"

  DBGPAS;

  if ( pTaskCtrl->pNextInTaskList )
    pTaskCtrl->pNextInTaskList->pPrevInTaskList = pTaskCtrl->pPrevInTaskList;

  if ( pTaskCtrl->pPrevInTaskList )
    pTaskCtrl->pPrevInTaskList->pNextInTaskList = pTaskCtrl->pNextInTaskList;
  else
    pTaskList->pFirstInTaskList                 = pTaskCtrl->pNextInTaskList; 

  pTaskCtrl->pNextInTaskList = NULL;
  pTaskCtrl->pPrevInTaskList = NULL;

  return;
  }

/*------------------------------*/

VOID sql73k_tlist_enque_task_nolock ( PTASK_LIST_HEAD_REC  pTaskList,
                                      PTASK_CTRL_REC       pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_enque_task_nolock"

  DBGPAS;

  if ( pTaskList->pFirstInTaskList )
    {
    pTaskCtrl->pNextInTaskList                   = pTaskList->pFirstInTaskList;
    pTaskList->pFirstInTaskList->pPrevInTaskList = pTaskCtrl;
    pTaskList->pFirstInTaskList                  = pTaskCtrl;
    }
  else
    {
    pTaskList->pFirstInTaskList  = pTaskCtrl;
    pTaskCtrl->pNextInTaskList   = NULL;
    }
  
  return;
  }

/*------------------------------*/

VOID sql73k_tlist_dequ_task ( PTASK_LIST_HEAD_REC  pTaskList,
                              PTASK_CTRL_REC       pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_dequ_task"

  DBGPAS;

  WAIT_UNTIL_ALONE(&pTaskList->lExclusive);
  sql73k_tlist_dequ_task_nolock ( pTaskList, pTaskCtrl );
  CLEARLOCK(&pTaskList->lExclusive);

  return;
  }

/*------------------------------*/

VOID sql73k_tlist_enque_task( PTASK_LIST_HEAD_REC  pTaskList,
                              PTASK_CTRL_REC       pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_enque_task"

  DBGPAS;

  WAIT_UNTIL_ALONE(&pTaskList->lExclusive);
  sql73k_tlist_enque_task_nolock ( pTaskList, pTaskCtrl );
  CLEARLOCK(&pTaskList->lExclusive);

  return;
  }

/*------------------------------*/

PTASK_CTRL_REC sql73k_tlist_first_task( PTASK_LIST_HEAD_REC  pTaskList )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_first_task"

  PTASK_CTRL_REC       pTaskCtrl;

  DBGPAS;

  WAIT_UNTIL_ALONE(&pTaskList->lExclusive);
  pTaskCtrl = pTaskList->pFirstInTaskList;
  CLEARLOCK(&pTaskList->lExclusive);

  return pTaskCtrl;
  }

/*------------------------------*/

PTASK_CTRL_REC  sql73k_tlist_next_task( PTASK_LIST_HEAD_REC  pTaskList,
                                        PTASK_CTRL_REC       pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql73k_tlist_next_task"

  DBGPAS;

  WAIT_UNTIL_ALONE(&pTaskList->lExclusive);
  pTaskCtrl = pTaskCtrl->pNextInTaskList;
  CLEARLOCK(&pTaskList->lExclusive);

  return pTaskCtrl;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

/*
 * =============================== END ========================================
 */
