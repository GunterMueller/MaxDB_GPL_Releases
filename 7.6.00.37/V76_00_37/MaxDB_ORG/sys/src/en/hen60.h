/*!
  @file           hen60.h
  @author         JoergM
  @special area   vopmsg
  @brief          Message Output
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



#ifndef HEN60_H
#define HEN60_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void vopmsg ( tsp00_Int2 prio ,
					  tsp00_Int2 msgno ,
					  tsp00_Int4 npar1 ,
					  tsp00_Int4 npar2 ,
					  tsp00_C64  spar ,
					  tsp00_C64  msgline );

externC void vksprintf ( tsp00_Addr pszMsg, 
						 const tsp00_Addr pszFormatStr, 
						 ... );

#endif  /* HEN60_H */
