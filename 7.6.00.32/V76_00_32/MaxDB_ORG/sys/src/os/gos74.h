/*!
  @file           gos74.h
  @author         RaymondR
  @brief          Dispatcher
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef GOS74_H
#define GOS74_H


#define LOAD_BALANCING_CHECK_UKT 

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"
#include "geo007_1.h"
#include "geo002.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

// --- Don't call this macro directly!!
//
#if defined ( NO_GOTO_DISP_INLINE )

# define __GOTO_DISPATCHER(_ppUKT)                                            \
      {                                                                       \
      PTASK_CTRL_REC   _pTaskCtrl       = (*_ppUKT)->pCTask;                  \
      _pTaskCtrl->ulBegExclCnt  = 0;                                          \
                                                                              \
      sql74k_dispatcher ( *_ppUKT );                                          \
                                                                              \
      if ( _pTaskCtrl != (*_ppUKT)->pCTask )                                  \
        {                                                                     \
        (*_ppUKT)->taskSwitches++;                                            \
                                                                              \
        if ( XPARAM(fUseFibers) )                                             \
          SwitchToFiber( (*_ppUKT)->pCTask->pFiberAddress );                  \
        else                                                                  \
          {                                                                   \
          if (sql71k_chg_task_stack(&(*_ppUKT)->pCTask->Context,              \
                                    &_pTaskCtrl->Context))                    \
            {                                                                 \
            MSGD  (( ERR_STACK_OVERFLOW, _pTaskCtrl->ulTaskIndex ));          \
            ABORT();                                                          \
            }                                                                 \
          }                                                                   \
        (*_ppUKT)->fRunningOnTempStack = FALSE;                               \
        *_ppUKT                        = _pTaskCtrl->pUKT;                    \
                                                                              \
#if defined (LOAD_BALANCING_CHECK_UKT)
        if ( *_ppUKT != THIS_UKT_CTRL )                                       \
          {                                                                   \
          MSGD  (( ERR_LOAD_BAL_TASK_WRONG_UKT,                               \
                   *_ppUKT->ulUKTIndex, THIS_UKT_CTRL->ulUKTIndex ));         \
          ABORT();                                                            \
          }                                                                   \
#endif
        }                                                                     \
      }

#define GOTO_DISP(_ppUKT)                                                     \
      __GOTO_DISPATCHER(_ppUKT);                                              \
      if ( (*_ppUKT)->pCTask->fDebugTask )                                    \
        RTETask_ProcessDebugTaskRequest( _ppUKT );
#else

#define GOTO_DISP           os74GotoDisp 
#define __GOTO_DISPATCHER   os74GotoDispatcher

#endif


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  INLINES                                                                  *
 *===========================================================================*/

#if !defined ( NO_GOTO_DISP_INLINE )

_INLINE void os74GotoDispatcher( PUKT_CTRL_REC *ppUKT )
{
  PUKT_CTRL_REC    pUKT          = *ppUKT;
  PTASK_CTRL_REC   pTaskCtrl     = pUKT->pCTask;

  pTaskCtrl->ulBegExclCnt  = 0;

  sql74k_dispatcher ( pUKT );

  if ( pTaskCtrl != pUKT->pCTask )
  {
    pUKT->taskSwitches++;

    if ( XPARAM(fUseFibers)  )
      SwitchToFiber( pUKT->pCTask->pFiberAddress );
    else
    {
      if (sql71k_chg_task_stack(&pUKT->pCTask->Context,
                                &pTaskCtrl->Context))
      {
        MSGD  (( ERR_STACK_OVERFLOW, pTaskCtrl->ulTaskIndex ));
        ABORT();
      }
    }
  *ppUKT                        = pTaskCtrl->pUKT;
  (*ppUKT)->fRunningOnTempStack = FALSE;
  }
}

/*------------------------------*/

_INLINE void os74GotoDisp( PUKT_CTRL_REC *ppUKT )
{
  os74GotoDispatcher(ppUKT);

  if ( (*ppUKT)->pCTask->fDebugTask )
        RTETask_ProcessDebugTaskRequest( ppUKT );
}

#endif

#endif  /* GOS74_H */
