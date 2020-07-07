/*!**********************************************************************

  module: WAHTTP_Server.h

  -----------------------------------------------------------------------

  responsible:	Tony Guepin
				Markus Oezgen

  special area: SAP DB WWW

  description:  

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

#ifndef WAHTTP_SERVER_H
#define WAHTTP_SERVER_H

#include "hwd01wadef.h"

int InitWebServer( char                              IniFilePath[],
                   struct wahttp_alternate_settings *alternateSettings,
                   char                             *errorOutText );

int StartWebServer();
int WaitWebServer( sapdbwa_Bool debug );
int StopWebServer();
int ExitWebServer();

#endif
