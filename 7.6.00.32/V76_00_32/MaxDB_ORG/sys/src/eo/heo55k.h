/*!
  @file           heo55k.h
  @author         JoergM
  @ingroup        RunTime
  @brief          vbegexcl/vendexcl semaphore in tasking context

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

#ifndef HEO55K_H
#define HEO55K_H

#include "gsp00.h"

/* --- VOS55KC / VEN55C ------------------------------------------------------ */

/*!
  @brief reserve a semaphore and get its semaphore identifier

  The semaphores are a critical resource for the kernel. Each semaphore 'slot' inside the runtime is
  reserved only once and can never be freed again. The semaphore identifier is copied into the slot
  and the slot index is returned as semaphore identifier. This identifier must be used in subsequent calls
  to vbegexcl and vendexcl.

  @param semaname [in] the 'non blank' semaphore identifier visible in show region
  @param sid [out] the semaphore identifier
  @return none, but kernel will abort with vabort() if no more semaphores available
 */
externC void       vsemaphid( tsp00_Region   VAR_ARRAY_REF semaname,
                              tsp00_RegionId VAR_VALUE_REF sid );

/*!
  @brief check tasks semaphores
  
  If a semaphore is specifed its owning task id is checked against the current task id, which MUST be passed as first argument.

  If an invalid semaphore identifier is given (-1) the task semphore count is checked, allowing the task to check if it
  currently owns any semaphore.

  @param pid [in] must match current task id
  @param sid [in] either a semaphore to check, or -1 to check for any owned semaphore
  @return true if task is owner, false if not
 */
externC SAPDB_Bool visexcl( tsp00_TaskId   pid,
                            tsp00_RegionId sid );

/*
 * The following two lines replace the prototype definition
 * void       vbegexcl          ( tsp00_TaskId    pid,
 *												  tsp00_RegionId  sid );
 */
/************/
#ifdef WIN32
/************/

/*!
  @brief lock a tasking semaphore to begin an exclusive section

  This call may force a task dispatching, if semaphore is already locked. This cannot be prevented!!!
  There is by design!!! no chance to use any kind of timeout etc...

  @param pid [in] must match calling task
  @param sid [in] semaphore to lock
 */
externC void vbegexcl( tsp00_TaskId pid,
                       tsp00_RegionId  sid );

/************/
#else
/************/

/*! defined to allow calling code to use 'vbegexcl' as is used in NT coding */
#define vbegexcl (*vbegexcl_func_ptr)
/*!
  @brief lock a tasking semaphore to begin an exclusive section

  This call may force a task dispatching, if semaphore is already locked. This cannot be prevented!!!
  There is by design!!! no chance to use any kind of timeout etc...

  @param pid [in] must match calling task
  @param sid [in] semaphore to lock
 */
externC void (*vbegexcl_func_ptr) ( tsp00_TaskId pid,
                                    tsp00_RegionId sid );

/************/
#endif
/************/

/*!
  @brief unlock a tasking semaphore to end an exclusive section

  @param pid [in] must match calling task
  @param sid [in] semaphore to lock
 */
externC void vendexcl( tsp00_TaskId    pid,
				       tsp00_RegionId  sid );

#endif /* HEO55K_H */
