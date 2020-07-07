/*!
  @file           gos962.h
  @author         RaymondR
  @brief          Xserver NI support > 6.1 applications
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



#ifndef GOS962_H
#define GOS962_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos96.h"

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

tsp00_Int
os962InitializeNISupport();

tsp00_Int
os962StartNICoord ( NI_COORD_CTRL_REC *pNICoord );

void
os962NIFinish    ( void );

externC bool RTEListener_BuildNiTraceFilename (tsp00_Pathc      VAR_ARRAY_REF     pathToFill, 
                                               tsp01_RteError   VAR_VALUE_REF     pRteError);

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS962_H */
