/*!**********************************************************************

  module: WDVCAPI_Delete.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Delete

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

#ifndef WDVCAPI_DELETE_H
#define WDVCAPI_DELETE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "WebDAV/CAPI/WDVCAPI_Resource.h"

/***********************************************************************

	Types

 ***********************************************************************/

typedef struct st_delete_handle  *WDVCAPI_DeleteHandle;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool DeleteCreateHandle( WDVCAPI_WDV  wdv);

WDV_Bool DeleteDestroyHandle( WDVCAPI_WDV  wdv);


WDV_Bool Delete_Delete( WDVCAPI_WDV            wdv,
                        WDVCAPI_URI            uri,
                        WDVCAPI_LockIdString   lockIdString,
                        WDV_Bool               withNewTransaction,
                        WDV_Bool               withAutoCommit );

WDV_Bool Delete_Inode( WDVCAPI_WDV      wdv,
                       WDVCAPI_Resource resource,
                       WDV_Bool         withAutoCommit );

/***********************************************************************

	End

 ***********************************************************************/

#endif
