/****************************************************************************

  module      : XMLIMAPI_Id.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-08  10:39
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Id.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

static XMLIMAPI_Bool	idManagementInitialized = XMLIMAPI_False;
static RTE_UniqueId		hIdManagement;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	API Functions

 ***********************************************************************/

XMLIMAPI_Bool XMLIMAPI_IdGetNext(XMLIMAPI_Handle	handle,
								   XMLIMAPI_Id		id )
{

    if (!RTE_FillUniqueId(&hIdManagement)) {
        addErrorItem(handle,XMLIMAPI_ERR_TYPE_MAPI,
							XMLIMAPI_ERR_MAPI_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
							XMLIMAPI_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID);

        return XMLIMAPI_False;
    }

    SAPDB_memcpy(id, &hIdManagement, XMLIMAPI_MAX_ID_LEN);

    return XMLIMAPI_True;
}

/*===========================================================================*/

void XMLIMAPI_IdAsString( XMLIMAPI_Id         id,
                         XMLIMAPI_IdString   idString )
{

	XMLIMAPI_UInt2 i = 0;

	for (i = 0; i < XMLIMAPI_MAX_ID_LEN; i++ ) {
		sprintf( idString + (i*2), "%02X", (unsigned char)id[i] );
	}
	idString[XMLIMAPI_MAX_ID_STRING_LEN] = '\0';

}

/*===========================================================================*/

void XMLIMAPI_IdAsDebugString( XMLIMAPI_Id             id,
                              XMLIMAPI_IdDebugString  idDebugString )
{

    RTE_DumpUniqueId((const RTE_UniqueId *) id, (unsigned char*)idDebugString);

}

/*===========================================================================*/

void XMLIMAPI_IdInitValue( XMLIMAPI_Id    id )
{

    memset(id, 0, XMLIMAPI_MAX_ID_LEN);

}

/*===========================================================================*/

void XMLIMAPI_IdStringInitValue( XMLIMAPI_IdString    idString )
{

    strMaxCopy(idString, ID_INITIAL_STRING, XMLIMAPI_MAX_ID_STRING_LEN);

}

/*===========================================================================*/

void XMLIMAPI_IdStringAsId( XMLIMAPI_IdString idString,
                           XMLIMAPI_Id       id )
{

    XMLIMAPI_UInt2   i = 0;
    char        tmp[3] = "\0\0\0";

    for (i = 0; i < XMLIMAPI_MAX_ID_LEN; i++) {
        tmp[0] = idString[i*2];
        tmp[1] = idString[i*2+1];

        id[i] = strtoul(tmp, NULL, 16);
    }

}

/*===========================================================================*/

XMLIMAPI_Bool XMLIMAPI_IdIsInitialValue( XMLIMAPI_Id id )
{

    if (memcmp(id, ID_INITIAL, XMLIMAPI_MAX_ID_LEN) == 0) {
        return XMLIMAPI_True;
    } else {
        return XMLIMAPI_False;
    }

}

/*===========================================================================*/

XMLIMAPI_Bool XMLIMAPI_IdStringIsInitialValue( XMLIMAPI_IdString idString )
{

    if (memcmp(idString, ID_INITIAL_STRING, XMLIMAPI_MAX_ID_STRING_LEN) == 0) {
        return XMLIMAPI_True;
    } else {
        return XMLIMAPI_False;
    }

}

/*===========================================================================*/

XMLIMAPI_Bool XMLIMAPI_IdsAreIdentical( XMLIMAPI_Id id1,
                                  XMLIMAPI_Id id2 )
{

    if (memcmp(id1, id2, XMLIMAPI_MAX_ID_LEN) == 0) {
        return XMLIMAPI_True;
    } else {
        return XMLIMAPI_False;
    }

}

/*===========================================================================*/

XMLIMAPI_Bool XMLIMAPI_IdStringssAreIdentical( XMLIMAPI_IdString idString1,
                                         XMLIMAPI_IdString idString2 )
{

    if (memcmp(idString1, idString2, XMLIMAPI_MAX_ID_STRING_LEN) == 0) {
        return XMLIMAPI_True;
    } else {
        return XMLIMAPI_False;
    }

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

XMLIMAPI_Bool IdInit()
{

    if (idManagementInitialized == XMLIMAPI_False) {
        RTE_InitIdGenerator();
    }

	return XMLIMAPI_True;

}

