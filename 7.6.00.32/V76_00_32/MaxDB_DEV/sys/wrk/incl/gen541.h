/*!
  @file           gen541.h
  @author         JoergM
  @special area   sqldevsize
  @brief          Inode information
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



#ifndef GEN541_H
#define GEN541_H

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "gsp00.h"

#include "gsp01.h"

/*===========================================================================*
*  FUNCTION PROTOTYPES                                                      *
*===========================================================================*/

externC int e541_get_nodinfo ( char     *fname,
							   tsp00_Int4 *fsize );

externC int e541_get_devsize0 ( int       devfile,
								tsp00_Int4  block_size );


externC int e541_filecopy ( char             *SrcName,
                            char             *DestName,
                            tsp01_RteError   *RteError );

#endif  /* GEN541_H */
