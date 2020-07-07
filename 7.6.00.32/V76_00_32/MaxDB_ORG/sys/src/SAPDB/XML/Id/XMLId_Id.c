/*!**********************************************************************

  module: XMLId_Id.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Id Handling

  description:  Id

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Id/XMLId_Id.h"
#include "SAPDB/XML/Id/XMLId_ErrorCodes.h"

#include "SAPDB/XML/Error/XMLError_Error.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/RunTime/RTE_UniqueId.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define ID_INITIAL_STRING                          \
    "000000000000000000000000000000000000000000000000"
#define ID_INITIAL                                 \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

static SAPDB_Bool    idManagementInitialized = SAPDB_FALSE;
static RTE_UniqueId  hIdManagement;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool Id_GetNext( XMLId_Id       id,
                       XMLError_Error hError )
{

    if (!RTE_FillUniqueId((RTE_UniqueId*)id)) {
        Error_Set(XMLERROR_POS, hError, XMLID_ERR_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
                                        XMLID_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;
}

/*===========================================================================*/

void Id_AsString( XMLId_Id         id,
                  XMLId_IdString   idString )
{

	SAPDB_UInt2 i = 0;

	for (i = 0; i < XMLID_MAX_ID_LEN; i++ ) {
		sprintf( idString + (i*2), "%02X", (unsigned char)id[i] );
	}
	idString[XMLID_MAX_ID_STRING_LEN] = '\0';

}

/*===========================================================================*/

void Id_AsDebugString( XMLId_Id             id,
                       XMLId_IdDebugString  idDebugString )
{

    RTE_DumpUniqueId((const RTE_UniqueId *) id, (SAPDB_UTF8*) idDebugString);

}

/*===========================================================================*/

void Id_InitValue( XMLId_Id    id )
{

    memset(id, 0, XMLID_MAX_ID_LEN);

}

/*===========================================================================*/

void Id_InitStringValue( XMLId_IdString    idString )
{

    Com_StrMaxCopy(idString, ID_INITIAL_STRING, XMLID_MAX_ID_STRING_LEN);

}

/*===========================================================================*/

void Id_StringAsId( XMLId_IdString idString,
                    XMLId_Id       id )
{

    SAPDB_UInt2   i = 0;
    char        tmp[3] = "\0\0\0";

    for (i = 0; i < XMLID_MAX_ID_LEN; i++) {
        tmp[0] = idString[i*2];
        tmp[1] = idString[i*2+1];

        id[i] = strtoul(tmp, NULL, 16);
    }

}

/*===========================================================================*/

SAPDB_Bool Id_IsInitialValue( XMLId_Id id )
{

    if (memcmp(id, ID_INITIAL, XMLID_MAX_ID_LEN) == 0) {
        return SAPDB_TRUE;
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool Id_IsInitialStringValue( XMLId_IdString idString )
{

    if (memcmp(idString, ID_INITIAL_STRING, XMLID_MAX_ID_STRING_LEN) == 0) {
        return SAPDB_TRUE;
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool Id_IsIdentical( XMLId_Id id1,
                           XMLId_Id id2 )
{

    if (memcmp(id1, id2, XMLID_MAX_ID_LEN) == 0) {
        return SAPDB_TRUE;
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool Id_IsIdenticalString( XMLId_IdString idString1,
                                 XMLId_IdString idString2 )
{

    if (memcmp(idString1, idString2, XMLID_MAX_ID_STRING_LEN) == 0) {
        return SAPDB_TRUE;
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool Id_Init()
{

    if (idManagementInitialized == SAPDB_FALSE) {
        idManagementInitialized = SAPDB_TRUE;
        RTE_InitIdGenerator();
    }

	return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
