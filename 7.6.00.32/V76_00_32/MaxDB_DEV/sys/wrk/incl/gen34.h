/*!
  @file           gen34.h
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



#ifndef GEN34_H
#define GEN34_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifdef USE_KGS

#include "geo00.h"
#include "gen00.h"
#include "gen50.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int  sql34_map_kgs ( int shmid ,
							 int shmflag );
 
externC int  sql34_unmap_kgs ();

extern struct KERNEL_GLOBAL_SECTION *kgs;

externC struct KERNEL_GLOBAL_SECTION * sql34MapKgsNotRemapped ( int shmid,
                                                                int writeAccess );

externC void sql34UnmapKgsNotRemapped ( struct  KERNEL_GLOBAL_SECTION *KgsNotRemapped );

#endif /* USE_KGS */

#endif  /* GEN34_H */
