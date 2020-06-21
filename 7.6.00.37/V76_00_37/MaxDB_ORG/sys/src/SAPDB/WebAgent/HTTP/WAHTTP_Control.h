/*!**********************************************************************

  module: WAHTTP_Control.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: SAP DB WWW

  description:  Windows Service Control Manager support

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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





************************************************************************/

#ifndef WAHTTP_CONTROL_H
#define WAHTTP_CONTROL_H

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "hwd01wadef.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"

#define SERVICE_INTERNAL_NAME	"SAPDBWWW"
#define SERVICE_DISPLAY_NAME	"SAP DB WWW"

#if defined(__cplusplus)
extern "C" {
#endif

    int Start( char                             *execDir,
               sapdbwa_Bool                      debug,
               struct wahttp_alternate_settings *alternateSettings,
               char                             *errorText );

    sapdbwa_Bool SetDispatchTable();
    sapdbwa_Bool InstallAsService( char *execDir );
    sapdbwa_Bool RemoveAsService();

    sapdbwa_Bool GetExecDirectory( char          *cmdArg,
	                               char          *directory,
                                   sapdbwa_UInt2  size      );
#if defined(__cplusplus)
}
#endif

#endif
