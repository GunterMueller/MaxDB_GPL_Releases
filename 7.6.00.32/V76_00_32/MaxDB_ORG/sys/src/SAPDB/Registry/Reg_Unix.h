/*!**********************************************************************

  module:	Reg_Unix.h

  -----------------------------------------------------------------------

  responsible:	Markus Özgen

  special area: Registry
  description : Handling registry entries - Unix functions

    Wraps an interface for registry entries. On Windows plattforms
	the registry will be used. On Unix plattforms an INI file will
	be used.

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

************************************************************************/

/*
 * first check if any UNIX platform is defined
 */
#if !(defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined(LINUX) || defined FREEBSD)
#error SUN, SOLARIS, AIX, OSF1, NMP, HPUX, FREEBSD or LINUX not defined !
#endif

#ifndef REGUNIX_H
#define REGUNIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include "SAPDB/Registry/Reg_Common.h"

/*===========================================================================*
 *===
 *===	Defines
 *===
 *===========================================================================*/

/*===========================================================================*
 *===
 *===	Types & Structs
 *===
 *===========================================================================*/

/*===========================================================================*
 *===
 *===	Interface Definition
 *===
 *===========================================================================*/

SAPDB_Bool Reg_UnixOpenRegistry ( Reg_RegistryP		*registry,
								  const char		*path,
								  tsp05_RteFileMode_Param openFlag	);

SAPDB_Bool Reg_UnixCloseRegistry ( Reg_RegistryP	registry	);

SAPDB_Bool Reg_UnixReadRegistry ( Reg_RegistryP	registry	);

SAPDB_Bool Reg_UnixWriteRegistry ( Reg_RegistryP	registry	);

#ifdef __cplusplus
}
#endif

#endif
