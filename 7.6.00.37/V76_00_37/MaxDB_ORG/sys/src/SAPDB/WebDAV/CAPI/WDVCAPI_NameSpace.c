/*!**********************************************************************

  module: WDVCAPI_NameSpace.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Name space Management

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_NameSpace.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_SELECT_NAME_SPACE_ID    \
    "SELECT Id FROM WEBDAV_Name_Space WHERE Name_Space = '%s'"

#define SQL_INSERT_NAME_SPACE	 \
    "INSERT INTO WEBDAV_Name_Space SET Id = X'%s', Name_Space = '%s'"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool NameSpace_GetIdAsString( WDVCAPI_WDV          wdv,
								  WDVCAPI_NameSpace    nameSpace,
								  WDVCAPI_IdString	   nameSpaceIdString )
{

    SQLRETURN   sqlRC = 0;
    SQLHDBC     hDBC = 0;
	SQLHSTMT    hStmt = 0;
	SQLCHAR     sqlStmt [256] = "";
    WDVCAPI_Id  nameSpaceId;
    SQLLEN      nameSpaceIdIndicator;

	/* Build SQL statement */
	sp77sprintf( sqlStmt, 256, SQL_SELECT_NAME_SPACE_ID, nameSpace );

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hStmt, 1, SQL_C_BINARY,
                       nameSpaceId, WDVCAPI_MAX_ID_LEN,
                       &nameSpaceIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Fetch first rowset */
    sqlRC = SQLFetch(hStmt);

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_UNKNOWN_NAME_SPACE,
                               WDVCAPI_ERR_TEXT_UNKNOWN_NAME_SPACE);
        } else {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
        }

		SQLFreeStmt(hStmt, SQL_DROP);

	    return WDV_False;
    }

    /* Convert id to id string */
	WDVCAPI_IdAsString(nameSpaceId, nameSpaceIdString);

    /* Close resultset */
	SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool NameSpace_Create( WDVCAPI_WDV          wdv,
                           WDVCAPI_NameSpace    nameSpace,
                           WDVCAPI_IdString		nameSpaceIdString )
{

    SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          hStmt = 0;
	SQLCHAR           sqlStmt [1024] = "";
	WDVCAPI_Id		  newId;

    /* Get new name space id */
    if (!WDVCAPI_IdGetNext(wdv, newId)) {
        return WDV_False;
    }
	WDVCAPI_IdAsString(newId, nameSpaceIdString);

	/* Build SQL statement to create new property */
	sp77sprintf(sqlStmt, 1024, SQL_INSERT_NAME_SPACE, nameSpaceIdString, nameSpace);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

		return WDV_False;
	}

    /* Execute statement */
    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

        SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    /* Free statement handle */
    SQLFreeStmt(hStmt, SQL_DROP);

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
