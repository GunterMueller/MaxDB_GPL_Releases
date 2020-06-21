/*!**********************************************************************

  module: WDVCAPI_Head.c

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

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Head.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_head_handle {

    WDVCAPI_Resource    resource;

};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Head_InitHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_HeadHandle    hHead );

WDV_Bool Head_Prepare( WDVCAPI_WDV        wdv,
                       WDVCAPI_HeadHandle hHead );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_Head( WDVCAPI_WDV   wdv,
                       WDVCAPI_URI   uri )
{

    WDVCAPI_HeadHandle      hHead = NULL;

    /* Checks */
    if (!wdv || !uri) {
        if (wdv) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

		return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Get head handle */
    if (!WDV_GetHeadHandle(wdv, &hHead)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get resource by uri */
    if (!Resource_GetByUri(wdv, uri, hHead->resource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_HeadGetContentLength( WDVCAPI_WDV            wdv,
                                       WDVCAPI_ContentLength *contentLength )
{

    WDVCAPI_HeadHandle                  hHead = NULL;
    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer;

    /* Checks */
    if (!wdv || !contentLength) {
        if (wdv) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

    /* Get head handle */
    if (!WDV_GetHeadHandle(wdv, &hHead)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get resource content length */
    if (!Property_GetShortValue(wdv, hHead->resource->id, ID_PROPERTY_GET_CONTENT_LENGTH, propertyShortValueBuffer)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }
    
    *contentLength = atol(propertyShortValueBuffer);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_HeadGetContentType( WDVCAPI_WDV          wdv,
                                     WDVCAPI_ContentType  contentType )
{

    WDVCAPI_HeadHandle                  hHead = NULL;

    /* Checks */
    if (!wdv || !contentType) {
        if (wdv) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

    /* Get head handle */
    if (!WDV_GetHeadHandle(wdv, &hHead)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get resource content type */
    if (!Property_GetShortValue(wdv, hHead->resource->id, ID_PROPERTY_GET_CONTENT_TYPE, contentType)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_HeadGetLastModified( WDVCAPI_WDV                 wdv,
                                      WDVCAPI_PropertyShortValue  lastModified )
{

    WDVCAPI_HeadHandle                  hHead = NULL;

    /* Checks */
    if (!wdv || !lastModified) {
        if (wdv) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

    /* Get head handle */
    if (!WDV_GetHeadHandle(wdv, &hHead)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get resource last modified */
    if (!Property_GetShortValue(wdv, hHead->resource->id, ID_PROPERTY_GET_LAST_MODIFIED, lastModified)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Head_CreateHandle( WDVCAPI_WDV  wdv)
{

	WDV_Bool            ok = WDV_False;
    WDVCAPI_HeadHandle  hHead = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_head_handle), (WDV_UInt1**) &hHead, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init head handle */
	Head_InitHandle(wdv, hHead);

    /* Set head handle in wdv */
    WDV_SetHeadHandle(wdv, hHead);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Head_DestroyHandle( WDVCAPI_WDV  wdv)
{

    WDVCAPI_HeadHandle   hHead = NULL;

    /* Get head handle from wdv handle */
    if (!WDV_GetHeadHandle(wdv, &hHead)) {
        return WDV_False;
    }

    /* Destroy resource object */
    if (!Resource_DestroyHandle(wdv, hHead->resource)) {
        return WDV_False;
    }
    
    /* Free head handle itself */
    WDVCAPI_Free((WDV_UInt1*) hHead );

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Head_InitHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_HeadHandle    hHead )
{

    /* Create and init resource object */
    if (!Resource_CreateHandle(wdv, &hHead->resource)) {
        return WDV_False;
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
