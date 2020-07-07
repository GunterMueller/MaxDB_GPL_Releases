/*!
  @file           geo00_0.h
  @author         JoergM
  @special area   Common operation system dependend header include
  @brief          Exists only to prevent vmake from complaining too often
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



#ifndef GEO00_0_H
#define GEO00_0_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(WIN32) /* &if $OS = WIN32 */
#ifdef KERNEL_LZU
#ifndef KERNEL
#define KERNEL
#endif /* KERNEL */
#endif /* KERNEL_LZU */
#include "gos00.h" /* nocheck */
#else             /* &else */
#ifdef KERNEL_LZU
#ifndef USE_KGS
#define USE_KGS
#endif /* KGS */
#endif /* KERNEL_LZU */
#include "gen00.h" /* nocheck */
#endif /* UNIX */ /* &endif */

#endif  /* GEO00_0_H */
