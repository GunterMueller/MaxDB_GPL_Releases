/* @lastChanged: "1998-06-02  18:10"
 * @filename:    heo66.h
 * @purpose:     "component kernel file I/O"
 * @release:     M.m.c.p
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


#ifndef HEO66_H
#define HEO66_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo66.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void vbackup_info       ( char                            *backup_info, 
                                  tsp00_Int4                       info_len,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
		                          tsp00_ErrText    VAR_ARRAY_REF   errtext ) ;

externC void vbackup_medium_def ( char                            *backup_medium_def, 
                                  tsp00_Int4                       info_len,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
		                          tsp00_ErrText    VAR_ARRAY_REF   errtext );


externC void vfrawopen          ( tsp00_VFilename  VAR_ARRAY_REF   hostfile ,
        		                  tsp00_Int4		              *hostfileno ,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
		                          tsp00_ErrText    VAR_ARRAY_REF   errtext );

externC void vfopen             ( tsp00_VFilename  VAR_ARRAY_REF   hostfile ,
        		                  tsp00_Int4		              *hostfileno ,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
		                          tsp00_ErrText    VAR_ARRAY_REF   errtext );
 
externC void vfwrite            ( tsp00_Int4	                   hostfileno ,
                                  char                            *block ,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
                                  tsp00_ErrText	   VAR_ARRAY_REF   errtext );


externC void vfclose            ( tsp00_Int4	                   hostfileno ,
		                          tsp00_VfReturn   VAR_VALUE_REF   error ,
		                          tsp00_ErrText	   VAR_ARRAY_REF   errtext ) ;


#endif  /* HEO66_H */
