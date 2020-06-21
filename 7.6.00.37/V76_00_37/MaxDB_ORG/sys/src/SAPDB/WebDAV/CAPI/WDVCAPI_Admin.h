/*!**********************************************************************

  module: WDVCAPI_Admin.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Administration

  see also:

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




************************************************************************/

#ifndef WDVCAPI_ADMIN_H
#define WDVCAPI_ADMIN_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Admin_CreateHandle( WDVCAPI_WDV             wdv,
                             WDVCAPI_AdminHandle    *hAdmin );

WDV_Bool Admin_DestroyHandle( WDVCAPI_WDV             wdv,
                              WDVCAPI_AdminHandle    *hAdmin );

/***********************************************************************

	End

 ***********************************************************************/

#endif
