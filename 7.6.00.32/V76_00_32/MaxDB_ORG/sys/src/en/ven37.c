/*!
  @file           ven37.c
  @author         JoergM
  @brief          Client RunTime: script_functions
  @see            


  Miscellaneous Functions used by Script

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

#define MOD__ "ven37.c:"

#include <stdlib.h>

/* exported functions */

#include "heo46.h"
#include "geo60.h"
#include "hen37.h"

/* imported functions */

#undef MF__
#define MF__ MOD__"sqlconcat"
void sqlconcat ( 
tsp00_C64    first_part  ,
tsp00_C64    second_part ,
tsp00_C64    resconcat ,
tsp00_Bool  *error )
{
	tsp00_C64c            Cfirst ;
	tsp00_C64c            Csecond ;
	tsp00_C64c            Cresult ;

	eo46PtoC( Cfirst , first_part , sizeof ( tsp00_C64 ) ) ;
	eo46PtoC( Csecond , second_part , sizeof ( tsp00_C64 ) ) ;

	sprintf ( Cresult , "%s/%s" , Cfirst , Csecond ) ;
	eo46CtoP( resconcat , (char *)Cresult , sizeof ( tsp00_C64 ) ) ;

	DBG1 (( MF__,"result = %s\n" , Cresult )) ;

	*error = 0 ;
}
