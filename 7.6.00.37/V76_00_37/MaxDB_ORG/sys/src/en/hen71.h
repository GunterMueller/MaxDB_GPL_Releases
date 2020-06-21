/*!
  @file           hen71.h
  @author         JoergM
  @special area   dispatcher
  @brief          Dispatcher and Queue Management
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



#ifndef HEN71_H
#define HEN71_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void v2prio ( tsp00_Int4 task_id ,
					  tsp00_Uint1 prio ,
					  tsp00_Bool  set_prio ,
					  int root ,
					  int leaf,
					  int locktype );

externC void vprio ( tsp00_Int4  task_id,
					 tsp00_Uint1 prio,
					 tsp00_Bool  set_prio );

externC void vdelay_commit ( tsp00_TaskId pid );

externC void vdelay_lw ( tsp00_TaskId pid );

#endif  /* HEN71_H */

