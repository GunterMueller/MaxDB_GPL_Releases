/*!
  @file           heo57.h
  @author         JoergM
  @special area   Kernel Memory
  @brief          Kernel Memory Allocation routines
  @see            example.html ...
  @first created  000-11-27  20:38

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


#ifndef HEO57_H
#define HEO57_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void                   vallocat         ( tsp00_Int4      length,
												  void          **objptr,
												  tsp00_Bool     *ok);

externC void                   valloc_convcache ( tsp00_Int4      count,
												  tsp00_Int4     *available,
												  tsp00_PageAddr *bufptr,
												  tsp00_Bool     *ok );

externC void                   valloc_datacache ( tsp00_Int4      count,
												  tsp00_Int4     *available,
												  tsp00_PageAddr *bufptr,
												  tsp00_Bool     *ok );


externC void                   vmalloc          ( tsp00_Int4      length,
												  void          **objptr,
												  tsp00_Bool     *ok );

externC void                   vmfree           ( void           *objptr );

externC void                   vmnewbuf         ( tsp00_Int4      count,
												  tsp00_BufAddr  *bufptr,
												  tsp00_Bool     *ok );

externC void                   vmfreebuf        ( tsp00_PageAddr  bufptr );

externC void                   vnewbuf          ( tsp00_Int4      count,
												  tsp00_Int4     *available,
												  tsp00_PageAddr *bufptr,
												  tsp00_Bool     *ok );

#endif
