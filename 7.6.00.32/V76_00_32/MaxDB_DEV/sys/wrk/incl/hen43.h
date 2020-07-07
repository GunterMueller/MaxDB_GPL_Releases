/*!
  @file           hen43.h
  @author         JoergM
  @special area   socket_address_resolution
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



#ifndef HEN43_H
#define HEN43_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC  void sqlhostname ( tsp00_NodeId   host ,
							tsp00_ErrText  errtext ,
							tsp01_CommErr *returncode );

externC  void sqlnodename ( tsp00_NodeId   host ,
							tsp00_NodeId   node ,
							tsp00_ErrText  errtext ,
							tsp01_CommErr *returncode );

#endif  /* HEN43_H */
