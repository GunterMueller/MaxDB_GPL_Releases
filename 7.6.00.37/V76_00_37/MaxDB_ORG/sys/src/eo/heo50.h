/*!
  @file           heo50.h
  @author         JoergM
  @special area   Mathematical function
  @brief          Mathematical function wrapper, used to prevent from exceptions
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



#ifndef HEO50_H
#define HEO50_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

    tsp00_Longreal vasin ( tsp00_Longreal  value, 
					   tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vsin ( tsp00_Longreal  value, 
							 tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vsinh ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vacos ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vcos ( tsp00_Longreal  value, 
							 tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vcosh ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vatan ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vatan2 ( tsp00_Longreal  y, 
        tsp00_Longreal  x, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vtan ( tsp00_Longreal  value, 
							 tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vtanh ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vexp ( tsp00_Longreal  value, 
							 tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vlog10 ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
    tsp00_Longreal vln ( tsp00_Longreal  value, 
        tsp00_NumError VAR_VALUE_REF error );
    
#if defined(__cplusplus)
}
#endif

#endif  /* HEO50_H */
