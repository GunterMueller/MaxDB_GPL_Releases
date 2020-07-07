/*!**********************************************************************

  module: WDVCAPI_Move.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Move

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

#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Copy.h"

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

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_Move( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        sourceUri,
					   WDVCAPI_URI			        destinationUri,
					   WDVCAPI_Overwrite	        overwrite,
                       WDVCAPI_LockIdList           destinationLockIdList,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData,
                       WDV_Bool                    *destinationCreated )
{

    WDVCAPI_URIString           sourceParentUri = "";
    WDVCAPI_URIString           destinationParentUri = "";
    WDVCAPI_ResourceNameString  sourceName = "";
    WDVCAPI_ResourceNameString  destinationName = "";
    WDVCAPI_Resource            sourceResource = NULL;
    WDVCAPI_LockId              sourceLockId;
    WDVCAPI_LockHandle          hLock = NULL;

    if (!wdv || !sourceUri || !destinationUri) {
        return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Is source and destination equal? */
    if (strcmp(sourceUri, destinationUri) == 0) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION,
                           WDVCAPI_ERR_TEXT_EQUAL_RESOURCE_AND_DESTINATION);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

    /* Split source path into parent and source name */
	SplitURI(wdv, sourceUri, sourceParentUri, destinationName );

    /* Split destination path into parent and destination name */
	SplitURI(wdv, destinationUri, destinationParentUri, destinationName );

    /* Is this only a renaming? */
    if (strcmp(sourceParentUri, destinationParentUri) == 0) {
        /* Create source resource object */
        if (!Resource_CreateHandle(wdv, &sourceResource)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Get resource by uri */
        if (!Resource_GetByUri(wdv, sourceUri, sourceResource)) {
            Resource_DestroyHandle(wdv, sourceResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Is source resource or something in the uri already locked? */
        if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
            Resource_DestroyHandle(wdv, sourceResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        if (Resource_IsLockInURI(wdv, sourceResource, hLock)) {
            if (!Lock_IsLockIdInList(wdv, destinationLockIdList, sourceUri, hLock->id)) {
		        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                        WDVCAPI_ERR_CODE_LOCK_EXISTS,
				                        WDVCAPI_ERR_TEXT_LOCK_EXISTS);

                WDVCAPI_LockDestroyHandle(wdv, hLock);

                Resource_DestroyHandle(wdv, sourceResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        }

        WDVCAPI_LockDestroyHandle(wdv, hLock);

        /* Rename */
        if (!Resource_Rename(wdv, sourceResource, destinationUri, destinationName, overwrite, destinationCreated)) {
            /* Something went wrong */
            Resource_DestroyHandle(wdv, sourceResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Destroy source resource object */
        Resource_DestroyHandle(wdv, sourceResource);

        /* End transaction */
        WDV_EndTransaction(wdv);

        return WDV_True;
    }

    /* Move resource */
    if (!Copy_Move(wdv, sourceUri, destinationUri,
                   WDVCAPI_DEPTH_INFINITE, overwrite,
                   destinationLockIdList, WDV_True, clientErrorHandling, clientData, destinationCreated)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* End transaction */
    WDV_EndTransaction(wdv);

    return WDV_True;

}

/*===========================================================================*/

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

/***********************************************************************

	Module Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
