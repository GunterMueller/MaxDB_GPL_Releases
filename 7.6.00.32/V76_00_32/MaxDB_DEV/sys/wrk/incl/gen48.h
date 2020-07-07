/*!
  @file           gen48.h
  @author         JoergM
  @special area   string
  @brief          Miscellaneous String Functions
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



#ifndef GEN48_H
#define GEN48_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC char * sql48_strloc ( char  * s1 ,
						 	  char  * s2 );

#ifdef  REL24
externC tsp00_Int2 sql48_strblcpy ( char      * s1 ,
								  char      * s2 ,
								  tsp00_Int2  n );
#endif /* REL24 */

externC void sql48_change_log ( char       * in_name ,
								char       * out_name ,
								tsp00_Bool * error );


#endif  /* GEN48_H */
