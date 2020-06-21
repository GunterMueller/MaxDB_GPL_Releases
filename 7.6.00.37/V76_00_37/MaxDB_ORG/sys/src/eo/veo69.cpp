/*!
  @file           veo69.cpp
  @author         JoergM
  @special area   Task Stack Checking
  @brief          This contains additional task stack code not contained in 
              ven69c or vos69kc
  @see            http://p26326/Tools/Autorisierung-Ergebnis981013.html

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"    /* Base types */
#include "heo69.h"    /* Own prototype (vsscan(),vsoverflow()) */
#include "geo00_0.h"  /* Os dependend header */
#include "geo50_0.h"  /* KGS */
#include "geo002.h"   /* XPARAM structure */
#include "geo007_1.h" /* Errormessage */
#if !defined(WIN32)
#include "hen50.h" /* nocheck */ /* vabort() */
#endif /* WIN32 */
#include "RunTime/RTE_CurrentStack.hpp" /* RTE_MaxStackUsage() */
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

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* PTS 1104151 */

/*
 * If the stack is grown already above this limit, the vabort() is called
 */

externC void vsoverflow()
{
  #undef  MF__
  #define MF__ MOD__"vsoverflow"

  tsp00_Int4 MaxStackSize;
  tsp00_Int4 TaskId;

  vscheck(MaxStackSize);

  /* PTS 1110917 */
#if defined(WIN32)
  {
      PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
      
      TaskId = pTaskCtrl->ulTaskIndex;

      if ( MaxStackSize >= 
            ( pTaskCtrl->Context.pStackLimit - pTaskCtrl->Context.pStackBase ) )
      {
          MSGD(( ERR_STACK_OVERFLOW, TaskId ));
          ABORT();
      }
  }
#else
  {
      ten50_UKT_Control * this_ukt = THIS_UKT_CTRL;

      TaskId = this_ukt->curr_task->index;

      if ( MaxStackSize >= this_ukt->curr_task->stack_size )
      {
          MSGD(( ERR_STACK_OVERFLOW, TaskId ));
          vabort(1);
      }
  }
#endif
}

externC void vsShowMaxStackUse(RTE_TaskId taskId, const char *taskTypeName)
{
    /*
     *  If wanted show task final maximum stack usage
     */
    SAPDB_UInt4 maxStackUse = RTE_MaximumStackUsage(taskId);

    MSGD(( INFO_MAX_STACK_USE, taskId, taskTypeName, (long)maxStackUse ));
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/