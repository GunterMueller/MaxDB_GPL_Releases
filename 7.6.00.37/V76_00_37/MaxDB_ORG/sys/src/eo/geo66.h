/*!
  @file           geo66.h
  @author         JoergM
  @special area   virtual file LZU Kernel
  @brief          LZU Internal Virtual File Interface
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



#ifndef GEO66_H
#define GEO66_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void sql66kc_open( char                          *filename ,
                           tsp00_Bool                    no_magic_record,
						   tsp00_Int4                      *hostfileno ,
						   tsp00_VfReturn VAR_VALUE_REF   error ,
						   tsp00_ErrText   VAR_ARRAY_REF   errtext );

externC void sql66kc_append_open(  char            *filename ,
								   tsp00_Int4        *hostfileno ,
								   tsp00_VfReturn VAR_VALUE_REF  error ,
								   tsp00_ErrText   VAR_ARRAY_REF  errtext );

externC void sql66kc_close ( tsp00_Int4        hostfileno ,
							 tsp00_VfReturn  VAR_VALUE_REF error ,
							 tsp00_ErrText    VAR_ARRAY_REF errtext );

externC void sql66kc_write ( tsp00_Int4        hostfileno ,
							 char           *block ,
							 tsp00_Int4        block_len ,
							 tsp00_VfReturn  VAR_VALUE_REF error ,
							 tsp00_ErrText    VAR_ARRAY_REF errtext );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GEO66_H */
