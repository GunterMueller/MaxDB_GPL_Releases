/*!**********************************************************************

  module: WDVCAPI_Id.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Id Management

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

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/RunTime/RTE_CompilerFeatures.h"
#include "SAPDB/RunTime/RTE_UniqueId.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

static WDV_Bool      idManagementInitialized = WDV_False;
static RTE_UniqueId  hIdManagement;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_IdGetNext( WDVCAPI_WDV   wdv,
                            WDVCAPI_Id    id )
{

    if (!id) {
        return WDV_False;
    }

    if (!RTE_FillUniqueId(&hIdManagement)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
                           WDVCAPI_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID);

        return WDV_False;
    }

    SAPDB_memcpy(id, &hIdManagement, WDVCAPI_MAX_ID_LEN);

    return WDV_True;
}

/*===========================================================================*/

void WDVCAPI_IdAsString( WDVCAPI_Id         id,
                         WDVCAPI_IdString   idString )
{

	WDV_UInt2 i = 0;

    if (!id || !idString) {
        return;
    }

	for (i = 0; i < WDVCAPI_MAX_ID_LEN; i++ ) {
		sprintf( idString + (i*2), "%02X", (unsigned char)id[i] );
	}
	idString[WDVCAPI_MAX_ID_STRING_LEN] = '\0';

}

/*===========================================================================*/

void WDVCAPI_IdAsDebugString( WDVCAPI_Id             id,
                              WDVCAPI_IdDebugString  idDebugString )
{

    if (!id || !idDebugString) {
        return;
    }

    RTE_DumpUniqueId((const RTE_UniqueId *) id, (SAPDB_UTF8*) idDebugString);

}

/*===========================================================================*/

void WDVCAPI_IdInitValue( WDVCAPI_Id    id )
{

    if (!id) {
        return;
    }

    memset(id, 0, WDVCAPI_MAX_ID_LEN);

}

/*===========================================================================*/

void WDVCAPI_IdStringInitValue( WDVCAPI_IdString    idString )
{

    Common_StrMaxCopy(idString, ID_ROOT_STRING, WDVCAPI_MAX_ID_STRING_LEN);

}

/*===========================================================================*/

void WDVCAPI_IdStringAsId( WDVCAPI_IdString idString,
                           WDVCAPI_Id       id )
{

    WDV_UInt2   i = 0;
    char        tmp[3] = "\0\0\0";

    if (!idString || !id) {
        return;
    }

    for (i = 0; i < WDVCAPI_MAX_ID_LEN; i++) {
        tmp[0] = idString[i*2];
        tmp[1] = idString[i*2+1];

        id[i] = strtoul(tmp, NULL, 16);
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IdIsInitialValue( WDVCAPI_Id id )
{

    if (!id) {
        return WDV_False;
    }

    if (memcmp(id, ID_INITIAL, WDVCAPI_MAX_ID_LEN) == 0) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IdStringIsInitialValue( WDVCAPI_IdString idString )
{

    if (!idString) {
        return WDV_False;
    }

    if (memcmp(idString, ID_INITIAL_STRING, WDVCAPI_MAX_ID_STRING_LEN) == 0) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IdsAreIdentical( WDVCAPI_Id id1,
                                  WDVCAPI_Id id2 )
{

    if (!id1 || !id2) {
        return WDV_False;
    }

    if (memcmp(id1, id2, WDVCAPI_MAX_ID_LEN) == 0) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IdStringssAreIdentical( WDVCAPI_IdString idString1,
                                         WDVCAPI_IdString idString2 )
{

    if (!idString1 || idString2) {
        return WDV_False;
    }

    if (memcmp(idString1, idString2, WDVCAPI_MAX_ID_STRING_LEN) == 0) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Id_Init( WDVCAPI_WDV   wdv )
{

    if (idManagementInitialized == WDV_False) {
        RTE_InitIdGenerator();
    }

	return WDV_True;

}

/*===========================================================================*/

/***********************************************************************

	Module Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
