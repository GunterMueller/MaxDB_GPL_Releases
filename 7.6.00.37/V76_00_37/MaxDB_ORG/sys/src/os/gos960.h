/*!
  @file           gos960.h
  @author         RaymondR
  @brief          Xserver support > 6.1 applications
  @see            

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



#ifndef GOS960_H
#define GOS960_H



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

VOID _System os960TCPIP_ServerThread ( ULONG ulConnInfoHandle ) ;

VOID os960Finish ( TCPIP_COORD_CTRL_REC *pTCPIPCoord ) ;

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


#endif  /* GOS960_H */