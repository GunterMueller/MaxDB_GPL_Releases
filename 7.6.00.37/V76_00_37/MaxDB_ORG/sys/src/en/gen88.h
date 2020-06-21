/*!
  @file           gen88.h
  @author         JoergM
  @ingroup        Runtime
  @brief          USER KERNEL Task Group Thread, Task Initialization and Task Switch

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



#ifndef GEN88_H
#define GEN88_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "gen50.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @brief Return pointer to current user kernel thread control block
  @return valid pointer if called from inside UKT, otherwise 0
 */
externC ten50_UKT_Control *en88GetThisUkt();

/*!---------------------------------------------------------------------------
  @brief UKT thread main routine

  Special UKT thread main routine used if coroutines are enabled

  @param arg [in] Pointer to user kernel thread control block
  @return always 0
 */
externC void *en88RunCoroutineUKT(void *arg);

/*!---------------------------------------------------------------------------
  @brief Initialize task control block to use coroutines for task

  @param tcb [in] Pointer to current task control block
  @return true if call succeeded, false if error
 */
externC tsp00_Int4 en88InitCoroutineTask ( struct  TASK_TYPE * tcb );

/*!---------------------------------------------------------------------------
  @brief Initialize task control block to use threads for task

  @param tcb [in] Pointer to current task control block
  @return true if call succeeded, false if error
 */
externC tsp00_Int4 en88_InitDynPthreadTasks ( struct  TASK_TYPE * tcb );

/*!---------------------------------------------------------------------------
  @brief Switch context from old task to new task
  
  Called after dispatcher returned with a new current task.
  In case of problems, this call will abort the kernel.

  @param pOld [in] Pointer to mementary task control block
  @param pNew [in] Pointer to new current task control block
  @return none
 */
externC void en88TaskContextSwitch ( struct TASK_TYPE * pOld, struct TASK_TYPE * pNew );


/*!---------------------------------------------------------------------------
  @brief Return task

  Using the address of a local variable the stack area is searched and the 
  task control pointer returned.

  @return  Task control pointer.
           If called from non task thread, a zero is returned.
 */
externC struct TASK_TYPE * RTE_GetOwnTask();

#endif  /* GEN88_H */
