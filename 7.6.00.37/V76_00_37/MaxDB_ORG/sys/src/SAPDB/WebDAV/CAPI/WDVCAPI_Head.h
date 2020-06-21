/*!**********************************************************************

  module: WDVCAPI_Head.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Head

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

#ifndef WDVCAPI_HEAD_H
#define WDVCAPI_HEAD_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Types

 ***********************************************************************/

typedef struct st_head_handle  *WDVCAPI_HeadHandle;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Head_CreateHandle( WDVCAPI_WDV  wdv);

WDV_Bool Head_DestroyHandle( WDVCAPI_WDV  wdv);

/***********************************************************************

	End

 ***********************************************************************/

#endif
