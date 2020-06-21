/*!**********************************************************************

  module: WDVCAPI_Common.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Common functions

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

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool SplitURI( WDVCAPI_WDV          wdv,
                   WDVCAPI_URI          uri,
				   WDVCAPI_URI          parent,
				   WDVCAPI_ResourceName child )
{

	WDVCAPI_URI uriEnd = NULL;
	WDVCAPI_URI lastSeparator = NULL;
	WDV_UInt4   parentLength = 0;
	WDV_UInt4   childLength = 0;

    TRACE_OUT(">> SplitURI()\n");

	*parent = '\0';
	*child = '\0';

	/* Is there an uri? */
	if ( !uri || *uri == '\0' ) {
		/* Nothing to split */
		return WDV_True;
	}

	/* Keep only one leading path seperator */
	while ( *(uri+1) == WDV_PATH_SEPARATOR ) uri++;

	/* Is uri only root? */
	if ( uri[0] == WDV_PATH_SEPARATOR && uri[1] == '\0' ) {
		/* Nothing to split */
		parent[0] = WDV_PATH_SEPARATOR;
		parent[1] = '\0';

		return WDV_True;
	}

	/* Skip trailing separator in uri */
	uriEnd = uri + strlen(uri) - 1;
	while ( *uriEnd == WDV_PATH_SEPARATOR ) uriEnd--;
	*(uriEnd + 1) = '\0';

	/* Is there a parent */
    lastSeparator = strrchr(uri, WDV_PATH_SEPARATOR);
    if (lastSeparator) {
		/* Check parent's length */
		parentLength = lastSeparator - uri;
        if (parentLength > WDV_MAX_RESOURCE_NAME_LEN) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
							   WDVCAPI_ERR_CODE_RESOURCE_NAME_TOO_LONG,
							   WDVCAPI_ERR_TEXT_RESOURCE_NAME_TOO_LONG);
            return WDV_False;
        };

		/* Get parent */
        if (parentLength > 0) {
            strncpy(parent, uri, parentLength);
            parent[parentLength] = '\0';
        } else {
		    /* Parent is root */
            strcpy(parent, WDV_PATH_SEPARATOR_STR);
        }

		/* Set child begin */
        uri += parentLength + 1;
	} else {
		/* Parent is root */
        strcpy(parent, WDV_PATH_SEPARATOR_STR);
	}

	/* Check child's length */
	childLength = strlen(uri);
    if (childLength > WDV_MAX_RESOURCE_NAME_LEN) {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
						    WDVCAPI_ERR_CODE_RESOURCE_NAME_TOO_LONG,
						    WDVCAPI_ERR_TEXT_RESOURCE_NAME_TOO_LONG);
        return WDV_False;
    };

	/* Get child */
    strncpy(child, uri, childLength);
    child[childLength] = '\0';

    TRACE_OUT("<< SplitURI()\n");

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Common_IsUriChild( WDVCAPI_WDV wdv,
                            WDVCAPI_URI srcUri,
                            WDVCAPI_URI destUri )
{

    WDVCAPI_URI                 uriToCompare;
    WDVCAPI_URIString           lastUri;
    WDVCAPI_URIString           parentUri;
    WDVCAPI_ResourceNameString  childName = "";

    /* Checks */
    if (!srcUri || !destUri || !srcUri[0]) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Inits */
    uriToCompare = destUri;

    /* Scan destination URI */
    while (strlen(uriToCompare) > strlen(srcUri)) {
        /* Get parent of last uri to compare */
        strcpy(lastUri, uriToCompare);
        SplitURI(wdv, lastUri, parentUri, childName);
        uriToCompare = parentUri;
    }

    if (strcmp(srcUri, uriToCompare) == 0) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/*===========================================================================*/

void Common_StrMaxCopy( char    *destination,
                        char    *source,
                        size_t   sizeToCopy )
{

    if (!destination || !source || sizeToCopy < 1) {
        return;
    }

    strncpy(destination, source, sizeToCopy);
    destination[sizeToCopy] = '\0';

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
