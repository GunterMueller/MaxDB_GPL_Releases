/*!
  @file           gen38.h
  @author         JoergM
  @special area   Enum2String
  @brief          Kernel Enum to String Tables
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



#ifndef GEN38_H
#define GEN38_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gen00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC char    * e38_dbstate_name ( UINT1 state );

externC char    * e38_type_name ( UINT1 type );

externC char    * e38_adv_tskstate_name ( UINT1  state ,
										  UINT2  state_reason );


externC char    * e38_tskstate_name ( UINT1 state );

externC char    * e38_req_name ( UINT1 request );

externC char    *en38KtStateName( tsp00_Int4 KtState );

#endif  /* GEN38_H */
