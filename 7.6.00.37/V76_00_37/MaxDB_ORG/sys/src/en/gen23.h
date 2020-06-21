/*!
  @file           gen23.h
  @author         JoergM
  @special area   dump packets
  @brief          Get Socket Addresses
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



#ifndef GEN23_H
#define GEN23_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "gen00.h"
#include "gen003.h" 

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int sql23_connect        ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_release        ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_request        ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_replyavailable ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_receive        ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_cancel         ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_clear          ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );
externC int sql23_dump           ( connection_info *cip , tsp00_ErrTextc VAR_ARRAY_REF pErrText );

#endif  /* GEN23_H */
