/* @lastChanged: "1998-07-17  11:57" 
 * @filename:   geo102.h
 * @purpose:    "User LZU Function Includes and Typedefs"
 * @release:    7.2.0.0
 * @see:        ""
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

#ifndef GEO102_H
#define GEO102_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "gsp01.h"

/* if updated eliminate some standard typedefs below... */
#include "vsp004.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef PASCALSTRING_TYPE_MSP00(tsp00_C100, 100);

typedef struct teo04_KeyStroke {
    tsp00_Uint1 ucVTKey;
    tsp00_Uint1 ucCh;
    tsp00_Int4  MouseX;
    tsp00_Int4  MouseY;
}  teo04_KeyStroke;

typedef tsp00_Int2 teo04_InsertLabelPos[2];

/* standard types for prototypes... subject to elimination... */

typedef tsp4_xuser_record tsp04_XuserRecord;

#endif
