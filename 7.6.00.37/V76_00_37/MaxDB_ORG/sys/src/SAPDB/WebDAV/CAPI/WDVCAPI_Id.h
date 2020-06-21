/*!**********************************************************************

  module: WDVCAPI_Id.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Id Handling

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

#ifndef WDVCAPI_ID_H
#define WDVCAPI_ID_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

#define ID_INITIAL_STRING                          \
    "000000000000000000000000000000000000000000000000"
#define ID_INITIAL                                 \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

#define ID_ROOT         ID_INITIAL
#define ID_ROOT_STRING  ID_INITIAL_STRING

#define ID_DELETED_ITEMS_STRING \
    "000000000000000000000000000000000000000000000001"
#define ID_DELETED_ITEMS    \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"

#define ID_PROPERTY_DISPLAYNAME_STRING          \
    "000000000000000000000000000000000000000000000001"
#define ID_PROPERTY_DISPLAYNAME                 \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"

#define ID_PROPERTY_RESOURCE_TYPE_STRING        \
    "000000000000000000000000000000000000000000000002"
#define ID_PROPERTY_RESOURCE_TYPE               \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"

#define ID_PROPERTY_GET_CONTENT_TYPE_STRING     \
    "000000000000000000000000000000000000000000000003"
#define ID_PROPERTY_GET_CONTENT_TYPE            \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03"

#define ID_PROPERTY_GET_CONTENT_LENGTH_STRING   \
    "000000000000000000000000000000000000000000000004"
#define ID_PROPERTY_GET_CONTENT_LENGTH          \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04"

#define ID_PROPERTY_GET_LAST_MODIFIED_STRING    \
    "000000000000000000000000000000000000000000000005"
#define ID_PROPERTY_GET_LAST_MODIFIED           \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05"

#define ID_PROPERTY_LOCK_ID_STRING              \
    "000000000000000000000000000000000000000000000006"
#define ID_PROPERTY_LOCK_ID                     \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06"

#define ID_PROPERTY_LOCK_DISCOVERY_STRING   ID_PROPERTY_LOCK_ID_STRING
#define ID_PROPERTY_LOCK_DISCOVERY          ID_PROPERTY_LOCK_ID

#define ID_PROPERTY_LOCK_OWNER_STRING           \
    "000000000000000000000000000000000000000000000007"
#define ID_PROPERTY_LOCK_OWNER                  \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07"

#define ID_PROPERTY_LOCK_TYPE_STRING            \
    "000000000000000000000000000000000000000000000008"
#define ID_PROPERTY_LOCK_TYPE                   \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"

#define ID_PROPERTY_LOCK_SCOPE_STRING           \
    "000000000000000000000000000000000000000000000009"
#define ID_PROPERTY_LOCK_SCOPE                  \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09"

#define ID_PROPERTY_LOCK_TIMEOUT_STRING         \
    "00000000000000000000000000000000000000000000000A"
#define ID_PROPERTY_LOCK_TIMEOUT                \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A"

#define ID_PROPERTY_LOCK_DEPTH_STRING           \
    "00000000000000000000000000000000000000000000000B"
#define ID_PROPERTY_LOCK_DEPTH                  \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0B"

#define ID_PROPERTY_SUPPORTED_LOCK_STRING       \
    "00000000000000000000000000000000000000000000000C"
#define ID_PROPERTY_SUPPORTED_LOCK              \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0C"

#define ID_PROPERTY_DOC_CLASS_ID_STRING         \
    "00000000000000000000000000000000000000000000000D"
#define ID_PROPERTY_DOC_CLASS_ID                \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0D"

#define ID_PROPERTY_PARSE_STATE_STRING         \
    "00000000000000000000000000000000000000000000000E"
#define ID_PROPERTY_PARSE_STATE                \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0E"

#define ID_PROPERTY_ORIGINAL_LOCATION_STRING   \
    "00000000000000000000000000000000000000000000000F"
#define ID_PROPERTY_ORIGINAL_LOCATION          \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0F"

#define ID_PROPERTY_CREATION_DATE_STRING   \
    "000000000000000000000000000000000000000000000010"
#define ID_PROPERTY_CREATION_DATE          \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10"

 /***********************************************************************

	Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Id_Init( WDVCAPI_WDV   wdv );

/***********************************************************************

	End

 ***********************************************************************/

#endif
