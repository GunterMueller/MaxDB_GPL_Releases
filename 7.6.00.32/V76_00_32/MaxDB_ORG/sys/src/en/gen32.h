/*!
  @file           gen32.h
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



#ifndef GEN32_H
#define GEN32_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "gen00.h"
#include "gen003.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int     sql32_open_kernel_fifo ( char                        *dbname ,
										 int                         *fdp ,
										 uid_t                       *uidp ,
										 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC  int     sql32_open_reply_fifo ( char                        *dbname ,
										 pid_t                        pid ,
										 tsp00_Int4                   ref ,
										 char                        *file ,
										 int                         *fdp ,
										 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int     sql32_attach_comseg ( connection_info *cip );

externC void    sql32_lock_comseg ( connection_info   *cip ,
									char const        *msg );

externC void    sql32_unlock_comseg ( connection_info *cip );

#endif  /* GEN32_H */
