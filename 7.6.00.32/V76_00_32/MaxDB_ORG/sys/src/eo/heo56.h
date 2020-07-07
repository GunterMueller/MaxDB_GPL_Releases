/*!
  @file           heo56.h
  @author         JoergM
  @special area   scheduling
  @brief          Kernel Task Scheduling
  @see            example.html ...

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



#ifndef HEO56_H
#define HEO56_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/* --- VOS56KC / VEN56C ------------------------------------------------------- */

externC void                   vstop            ( tsp00_TaskId      pid );

externC void                   vcontinue        ( tsp00_TaskId      pid );

externC void                   vsuspend         ( tsp00_TaskId      pid,
												  tsp00_Int2        suspend_reason );

externC tsp00_Bool             visresume        ( tsp00_TaskId      pid);

externC void                   vresume          ( tsp00_TaskId      pid );

externC void                   vwait            ( tsp00_TaskId             pid,
												  tsp00_Bool VAR_VALUE_REF ok );

externC void                   vsignal          ( tsp00_TaskId      pid,
												  tsp00_Bool        ok );

externC void                   vsleep           ( tsp00_TaskId      pid,
												  tsp00_Int2        limit );


externC void                   vreschedule_msec ( tsp00_TaskId      pid,
                                                  tsp00_Int4 VAR_VALUE_REF StartMsec,
                                                  tsp00_Int4 VAR_VALUE_REF RemainMsec );

externC void                   vSuspendForPageLock ( tsp00_TaskId      pid,
												     tsp00_Int2        suspend_reason,
                                                     tsp00_Int4        page_no );

#endif  /* HEO56_H */
