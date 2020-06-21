/*!
  @file           gen904.h
  @author         JoergM
  @special area   Database and Installation  Registry
  @brief          description ...
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



#ifndef GEN904_H
#define GEN904_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp09.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

externC tsp00_Bool
sql904_findControlServer (
    char *executable,		    /* OUT		*/
    const int  MaxExecutableLen,/* IN       */
    const char *dbname,		    /* IN		*/
    const char *serverpgm,      /* IN       */ 
    char *dbroot,		        /* IN/OUT	*/
    const int  MaxDBRootLen,    /* IN       */
    tsp00_ErrTextc VAR_ARRAY_REF errText ) ;  /* OUT */

#endif  /* GEN904_H */
