/*!
  @file           RTETask_Yield.h
  @author         JoergM
  @ingroup        Runtime
  @brief          Scheduling

  System Independend Tasking Internel Yield Call. Used for C and C++ coding...
  
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
#ifndef RTETASK_YIELD_H
#define RTETASK_YIELD_H

#include "SAPDBCommon/SAPDB_Types.h"
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(__cplusplus)
/*!
  @brief Yield call inside kernel
  If kernel code runs outside of task group a thread rescheduling is executed.
  If kernel code runs inside a task group, it uses the dispatcher to find a new
  task, and a thread rescheduling is only executed, if a new tasks exists. Otherwise
  a switch to the new task is done.
  @param allowToMove [in] if true, dynamic load balancing is possible, otherwise task is locked to its UKT
 */
extern "C" void RTETask_Yield(SAPDB_Bool allowToMove=false);
#else
/*!
  @brief Yield call inside kernel
  If kernel code runs outside of task group a thread rescheduling is executed.
  If kernel code runs inside a task group, it uses the dispatcher to find a new
  task, and a thread rescheduling is only executed, if a new tasks exists. Otherwise
  a switch to the new task is done.
  @param allowToMove [in] if true, dynamic load balancing is possible, otherwise task is locked to its UKT
 */
extern void RTETask_Yield(SAPDB_Bool allowToMove);
#endif
#endif  /* RTETASK_YIELD_H */
