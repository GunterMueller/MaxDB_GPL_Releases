/*!
  @file           hen56.h
  @author         JoergM
  @special area   scheduling
  @brief          Kernel Scheduling Functions
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



#ifndef HEN56_H
#define HEN56_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void  vsuspend ( tsp00_TaskId pid ,
                         tsp00_Int2 suspend_reason );

externC void  vresume ( tsp00_TaskId pid );

externC void  vwait ( tsp00_TaskId pid,
					  tsp00_Bool  VAR_VALUE_REF ok );

externC void  vsignal ( tsp00_TaskId pid ,
					    tsp00_Bool   ok );

externC void  vsleep ( tsp00_TaskId  pid,
					   tsp00_Int2    limit );

externC void  vreschedule_msec ( tsp00_TaskId      pid,
								 tsp00_Int4        VAR_VALUE_REF StartMsec,
								 tsp00_Int4        VAR_VALUE_REF RemainMsec );


#endif  /* HEN56_H */
