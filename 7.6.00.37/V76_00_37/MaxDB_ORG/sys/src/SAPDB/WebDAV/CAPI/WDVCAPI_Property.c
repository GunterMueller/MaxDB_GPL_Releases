/*!**********************************************************************

  module: WDVCAPI_Property.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Property Management

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

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_NameSpace.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_XMLIndexing.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_COPY_PROPERTY                                           \
    "INSERT INTO WEBDAV_PROPERTY (CId, Property_Id, Property_Short_Value)  \
     Select X'%s', Property_Id, Property_Short_Value FROM WEBDAV_PROPERTY  \
     WHERE CId = X'%s' AND Property_Id IN \
     (X'"ID_PROPERTY_RESOURCE_TYPE_STRING"', X'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', \
      X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', , X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"', \
      X'"ID_PROPERTY_DISPLAYNAME_STRING"')"

#define SQL_COPY_PROPERTY_EXCEPT_DISPLAYNAME    \
    "INSERT INTO WEBDAV_PROPERTY (CId, Property_Id, Property_Short_Value)      \
     Select X'%s', Property_Id, Property_Short_Value FROM WEBDAV_PROPERTY      \
     WHERE CId = X'%s' AND Property_Id IN \
     (X'"ID_PROPERTY_RESOURCE_TYPE_STRING"', X'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', \
      X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"')"


#define SQL_UPDATE_CONTENT_LENGTH           \
    "INSERT INTO WEBDAV_PROPERTY SET CID = X'%s', Property_Id = X'%s', Property_Short_Value = %lu UPDATE DUPLICATES"

#define SQL_INSERT_PROPERTY \
     "INSERT INTO WEBDAV_Property_Management SET Id = X'%s', Name_Space_Id = X'%s', Name_Prefix = '%s'"

#define SQL_SELECT_SHORT_VALUE  \
    "SELECT Property_Short_Value FROM WEBDAV_Property WHERE CId = ? And Property_Id = ?"

#define SQL_SELECT_DESC_BY_SHORT_VALUE  \
    "SELECT CId, Property_Id FROM WEBDAV_Property WHERE Property_Short_Value = '%s'"

#define SQL_SELECT_CID_BY_SHORT_VALUE_AND_PROPERTY_ID   \
    "SELECT CId FROM WEBDAV_Property WHERE Property_Id = X'%s' AND Property_Short_Value = '%s'"

#define SQL_SET_PROPERTY    \
    "INSERT INTO WEBDAV_PROPERTY SET CId = ?, Property_Id = ?, Property_Short_Value = ?"

#define SQL_UPDATE_PROPERTY    \
    "UPDATE WEBDAV_Property SET Property_Short_Value = ? WHERE CId = ? AND Property_Id = ?"

#define SQL_SET_CREATION_DATE   \
    "INSERT INTO WEBDAV_PROPERTY SET CId = ?, \
     Property_Id = X'"ID_PROPERTY_CREATION_DATE_STRING"', Property_Short_Value = TIMESTAMP"

#define SQL_SET_LAST_MODIFIED   \
    "INSERT INTO WEBDAV_PROPERTY SET CId = ?, \
     Property_Id = X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"', Property_Short_Value = TIMESTAMP"

#define SQL_UPDATE_LAST_MODIFIED   \
    "UPDATE WEBDAV_Property SET Property_Short_Value = TIMESTAMP WHERE \
     CId = X'%s' AND Property_Id = X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"'"

#define SQL_SET_INDEX                                                       \
    "INSERT WEBDAV_Property (CId, Property_Id, Property_Short_Value)               \
     SELECT ?, PM.Id, ? FROM WEBDAV_Property_Management PM, WEBDAV_Name_Space NS   \
     WHERE NS.Name_Space = ? AND PM.Name_Prefix = ? AND PM.Name_Space_Id = NS.Id"

#define SQL_DELETE_ALL_INDEX_VALUES \
     "DELETE WEBDAV_PROPERTY WHERE cId = ? AND Property_Id IN  \
     (SELECT PM.Id FROM WEBDAV_Property_Management PM, WEBDAV_Name_Space NS \
      WHERE NS.Name_Space = 'http://www.sapdb.org/xml/indexing' AND NS.Id = PM.Name_Space_Id)"

#define SQL_PROPERTY_DELETE \
    "DELETE WEBDAV_Property WHERE CId = ? AND Property_Id = ?"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_property_handle {
    SQLHSTMT                    hStmt;

    WDVCAPI_Id                  cid;
};

/*---------------------------------------------------------------------------*/

struct st_property_handle_set_xml_index {
    SQLHSTMT                            hStmt;

    WDVCAPI_NameSpaceString             indexNameSpace;
    SQLLEN                              indexNameSpaceIndicator;

    WDVCAPI_PropertyNameString          indexName;
    SQLLEN                              indexNameIndicator;

    WDVCAPI_PropertyShortValueBuffer    indexValue;
    SQLLEN                              indexValueIndicator;
};

/*---------------------------------------------------------------------------*/

struct st_property_handle_set_short_value {
    SQLHSTMT                            hStmt;

    WDVCAPI_Id                          resourceId;
    SQLLEN                              resourceIdIndicator;

    WDVCAPI_Id                          propertyId;
    SQLLEN                              propertyIdIndicator;

    WDVCAPI_PropertyShortValueBuffer    propertyShortValue;
    SQLLEN                              propertyShortValueIndicator;
};

/*---------------------------------------------------------------------------*/

struct st_property_handle_get_short_value {
    SQLHSTMT                            hStmt;

    WDVCAPI_Id                          resourceId;
    SQLLEN                              resourceIdIndicator;

    WDVCAPI_Id                          propertyId;
    SQLLEN                              propertyIdIndicator;

    WDVCAPI_PropertyShortValueBuffer    propertyShortValue;
    SQLLEN                              propertyShortValueIndicator;
};

/*---------------------------------------------------------------------------*/

struct st_property_handle_update_short_value {
    SQLHSTMT                            hStmt;

    WDVCAPI_Id                          resourceId;
    SQLLEN                              resourceIdIndicator;

    WDVCAPI_Id                          propertyId;
    SQLLEN                              propertyIdIndicator;

    WDVCAPI_PropertyShortValueBuffer    propertyShortValue;
    SQLLEN                              propertyShortValueIndicator;
};

/*---------------------------------------------------------------------------*/

struct st_property_handle_set_last_modified {
    SQLHSTMT    hStmt;

    WDVCAPI_Id  resourceId;
    SQLLEN      resourceIdIndicator;
};

struct st_property_handle_set_creation_date {
    SQLHSTMT    hStmt;

    WDVCAPI_Id  resourceId;
    SQLLEN      resourceIdIndicator;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Property_InitHandle( WDVCAPI_WDV             wdv,
                              WDVCAPI_PropertyHandle  hProperty );

WDV_Bool Property_InitHandleSetXmlIndex( WDVCAPI_WDV                       wdv,
                                         WDVCAPI_PropertyHandleSetXmlIndex hPropertySetXmlIndex );

WDV_Bool Property_InitHandleSetShortValue( WDVCAPI_WDV                         wdv,
                                           WDVCAPI_PropertyHandleSetShortValue hPropertySetShortValue );

WDV_Bool Property_InitHandleGetShortValue( WDVCAPI_WDV                         wdv,
                                           WDVCAPI_PropertyHandleGetShortValue hPropertyGetShortValue );

WDV_Bool Property_InitHandleUpdateShortValue( WDVCAPI_WDV                            wdv,
                                              WDVCAPI_PropertyHandleUpdateShortValue hPropertyUpdateShortValue );

WDV_Bool Property_InitHandleSetLastModified( WDVCAPI_WDV                           wdv,
                                             WDVCAPI_PropertyHandleSetLastModified hPropertySetLastModified );

WDV_Bool Property_InitHandleSetCreationDate( WDVCAPI_WDV                           wdv,
                                            WDVCAPI_PropertyHandleSetCreationDate hPropertySetCreationDate );

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Property_CreateHandle( WDVCAPI_WDV             wdv,
                                WDVCAPI_PropertyHandle *hProperty )
{

	WDV_Bool            ok = WDV_False;

	if ( !wdv ) {
		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle), (WDV_UInt1**) hProperty, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property handle */
	Property_InitHandle(wdv, *hProperty);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandle( WDVCAPI_WDV            wdv,
                                 WDVCAPI_PropertyHandle hProperty )
{

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hProperty );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleSetXmlIndex( WDVCAPI_WDV                        wdv,
                                           WDVCAPI_PropertyHandleSetXmlIndex *hPropertySetXmlIndex )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertySetXmlIndex) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle_set_xml_index), (WDV_UInt1**) hPropertySetXmlIndex, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init handle */
	Property_InitHandleSetXmlIndex(wdv, *hPropertySetXmlIndex);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleSetShortValue( WDVCAPI_WDV                          wdv,
                                             WDVCAPI_PropertyHandleSetShortValue *hPropertySetShortValue )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertySetShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle_set_short_value), (WDV_UInt1**) hPropertySetShortValue, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property set short value handle */
	Property_InitHandleSetShortValue(wdv, *hPropertySetShortValue);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleGetShortValue( WDVCAPI_WDV                          wdv,
                                             WDVCAPI_PropertyHandleGetShortValue *hPropertyGetShortValue )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertyGetShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle_get_short_value), (WDV_UInt1**) hPropertyGetShortValue, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property get short value handle */
	Property_InitHandleGetShortValue(wdv, *hPropertyGetShortValue);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleSetXmlIndex( WDVCAPI_WDV                       wdv,
                                            WDVCAPI_PropertyHandleSetXmlIndex hPropertySetXmlIndex )
{

	if (!wdv || !hPropertySetXmlIndex) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertySetXmlIndex->hStmt) {
        SQLFreeStmt(hPropertySetXmlIndex->hStmt, SQL_DROP);
        hPropertySetXmlIndex->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertySetXmlIndex );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleSetShortValue( WDVCAPI_WDV                         wdv,
                                              WDVCAPI_PropertyHandleSetShortValue hPropertySetShortValue )
{

	if (!wdv || !hPropertySetShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertySetShortValue->hStmt) {
        SQLFreeStmt(hPropertySetShortValue->hStmt, SQL_DROP);
        hPropertySetShortValue->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertySetShortValue );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleGetShortValue( WDVCAPI_WDV                         wdv,
                                              WDVCAPI_PropertyHandleGetShortValue hPropertyGetShortValue )
{

	if (!wdv || !hPropertyGetShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertyGetShortValue->hStmt) {
        SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
        hPropertyGetShortValue->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertyGetShortValue );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleUpdateShortValue( WDVCAPI_WDV                             wdv,
                                                WDVCAPI_PropertyHandleUpdateShortValue *hPropertyUpdateShortValue )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertyUpdateShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat(sizeof(struct st_property_handle_update_short_value),
                    (WDV_UInt1**) hPropertyUpdateShortValue, &ok);
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property update short value handle */
	Property_InitHandleUpdateShortValue(wdv, *hPropertyUpdateShortValue);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleUpdateShortValue( WDVCAPI_WDV                            wdv,
                                                 WDVCAPI_PropertyHandleUpdateShortValue hPropertyUpdateShortValue )
{

	if (!wdv || !hPropertyUpdateShortValue) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertyUpdateShortValue->hStmt) {
        SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_DROP);
        hPropertyUpdateShortValue->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertyUpdateShortValue );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleSetLastModified( WDVCAPI_WDV                            wdv,
                                               WDVCAPI_PropertyHandleSetLastModified *hPropertySetLastModified )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertySetLastModified) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle_set_last_modified),
                           (WDV_UInt1**) hPropertySetLastModified, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property handle set last modified */
	Property_InitHandleSetLastModified(wdv, *hPropertySetLastModified);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleSetLastModified( WDVCAPI_WDV                           wdv,
                                                WDVCAPI_PropertyHandleSetLastModified hPropertySetLastModified )
{

	if (!wdv || !hPropertySetLastModified) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertySetLastModified->hStmt) {
        SQLFreeStmt(hPropertySetLastModified->hStmt, SQL_DROP);
        hPropertySetLastModified->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertySetLastModified );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateHandleSetCreationDate( WDVCAPI_WDV                            wdv,
                                               WDVCAPI_PropertyHandleSetCreationDate *hPropertySetCreationDate )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hPropertySetCreationDate) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_property_handle_set_creation_date),
                           (WDV_UInt1**) hPropertySetCreationDate, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property handle set creation date */
	Property_InitHandleSetCreationDate(wdv, *hPropertySetCreationDate);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DestroyHandleSetCreationDate( WDVCAPI_WDV                           wdv,
                                                WDVCAPI_PropertyHandleSetCreationDate hPropertySetCreationDate )
{

	if (!wdv || !hPropertySetCreationDate) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hPropertySetCreationDate->hStmt) {
        SQLFreeStmt(hPropertySetCreationDate->hStmt, SQL_DROP);
        hPropertySetCreationDate->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPropertySetCreationDate);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_UpdateContentLength( WDVCAPI_WDV              wdv,
                                       WDVCAPI_Id               resourceId,
                                       WDVCAPI_ContentLength    contentLength )
{

	SQLRETURN   sqlRC = 0;
    SQLHDBC     hDBC = 0;
	SQLHSTMT    hStmt = 0;
	SQLCHAR     sqlStmt [256] = "";
    SQLINTEGER  idIndicator;
    SQLINTEGER  contentTypeIndicator;
	WDVCAPI_IdString  resourceIdString = "";

    /* Get id strings */
	WDVCAPI_IdAsString(resourceId, resourceIdString);

	/* Build SQL statement */
	sp77sprintf(sqlStmt, 256, SQL_UPDATE_CONTENT_LENGTH,
                resourceIdString, ID_PROPERTY_GET_CONTENT_LENGTH_STRING, contentLength);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_SetLastModified( WDVCAPI_WDV   wdv,
                                   WDVCAPI_Id    resourceId )
{

	SQLRETURN   sqlRC = 0;
    SQLHDBC     hDBC = 0;
	SQLHSTMT    hStmt = 0;
	SQLCHAR     sqlStmt [256] = "";
	WDVCAPI_IdString  resourceIdString = "";

    WDVCAPI_PropertyHandleSetLastModified hPropertySetLastModified = NULL;

    /* Get statement handle */
    WDV_GetHandlePropertySetLastModified(wdv, &hPropertySetLastModified);

    /* Set members                                          */
    /* To copy these 24 bytes every time should be better   */
    /* than to bind the resource id every time (performance)*/
    SAPDB_memcpy(hPropertySetLastModified->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);

    /* Is statement already prepared? */
    if (!hPropertySetLastModified->hStmt) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPropertySetLastModified->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertySetLastModified->hStmt, sqlRC );
		    return WDV_False;
	    }

	    /* Parse SQL */
	    sqlRC = SQLPrepare(hPropertySetLastModified->hStmt, (SQLCHAR*) SQL_SET_LAST_MODIFIED, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetLastModified->hStmt, sqlRC);
		    SQLFreeStmt(hPropertySetLastModified->hStmt, SQL_DROP);
            hPropertySetLastModified->hStmt = 0;

            return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hPropertySetLastModified->hStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertySetLastModified->resourceId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertySetLastModified->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetLastModified->hStmt, sqlRC);

            SQLFreeStmt(hPropertySetLastModified->hStmt, SQL_DROP);
            hPropertySetLastModified->hStmt = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hPropertySetLastModified->hStmt, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hPropertySetLastModified->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropertySetLastModified->hStmt, sqlRC );
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_UpdateLastModified( WDVCAPI_WDV   wdv,
                                      WDVCAPI_Id    resourceId )
{

	SQLRETURN   sqlRC = 0;
    SQLHDBC     hDBC = 0;
	SQLHSTMT    hStmt = 0;
	SQLCHAR     sqlStmt [256] = "";
	WDVCAPI_IdString  resourceIdString = "";

    /* Get id strings */
	WDVCAPI_IdAsString(resourceId, resourceIdString);

	/* Build SQL statement */
	sp77sprintf(sqlStmt, 256, SQL_UPDATE_LAST_MODIFIED, resourceIdString);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_SetCreationDate( WDVCAPI_WDV   wdv,
                                   WDVCAPI_Id    resourceId )
{

	SQLRETURN   sqlRC = 0;
	SQLHSTMT    hStmt = 0;
	SQLCHAR     sqlStmt [256] = "";
	WDVCAPI_IdString  resourceIdString = "";

    WDVCAPI_PropertyHandleSetCreationDate hPropertySetCreationDate = wdv->hPropertySetCreationDate;

    /* Set members                                          */
    /* To copy these 24 bytes every time should be better   */
    /* than to bind the resource id every time (performance)*/
    SAPDB_memcpy(hPropertySetCreationDate->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);

    /* Is statement already prepared? */
    if (!hPropertySetCreationDate->hStmt) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hPropertySetCreationDate->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertySetCreationDate->hStmt, sqlRC );
		    return WDV_False;
	    }

	    /* Parse SQL */
	    sqlRC = SQLPrepare(hPropertySetCreationDate->hStmt, (SQLCHAR*) SQL_SET_CREATION_DATE, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetCreationDate->hStmt, sqlRC);
		    SQLFreeStmt(hPropertySetCreationDate->hStmt, SQL_DROP);
            hPropertySetCreationDate->hStmt = 0;

            return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hPropertySetCreationDate->hStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertySetCreationDate->resourceId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertySetCreationDate->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetCreationDate->hStmt, sqlRC);

            SQLFreeStmt(hPropertySetCreationDate->hStmt, SQL_DROP);
            hPropertySetCreationDate->hStmt = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hPropertySetCreationDate->hStmt, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hPropertySetCreationDate->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropertySetCreationDate->hStmt, sqlRC );
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DeleteAll( WDVCAPI_WDV    wdv,
                             WDVCAPI_Id     cId,
                             WDV_Bool       withDisplayName )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          hStmt = 0;
	SQLCHAR           sqlStmt [1024] = "";
	WDVCAPI_IdString  cIdString = "";

    /* Get id strings */
	WDVCAPI_IdAsString(cId, cIdString);

	/* Build SQL statement to delete old properties */
    if (withDisplayName == WDV_True) {
	    sp77sprintf(sqlStmt, 1024, "DELETE WEBDAV_PROPERTY WHERE CID = X'%s'", cIdString);
    } else {
	    sp77sprintf(sqlStmt, 1024, "DELETE WEBDAV_PROPERTY WHERE CID = X'%s' AND Property_Id != X'"ID_PROPERTY_DISPLAYNAME_STRING"'", cIdString);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

		return WDV_False;
	}

    /* Execute delete statement */
    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED) &&
            !WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
	}

    /* Free statement handle */
    SQLFreeStmt(hStmt, SQL_DROP);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DeleteAllIndexValues ( WDVCAPI_WDV    wdv,
                                         WDVCAPI_Id     resourceId )
{

	SQLRETURN   sqlRC = 0;

	/* Allocat statement handle */
    if (!wdv->hStmtPropertyDeleteAllIndexValues) {
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hStmtPropertyDeleteAllIndexValues);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtPropertyDeleteAllIndexValues, sqlRC);

            wdv->hStmtPropertyDeleteAllIndexValues = 0;

		    return WDV_False;
	    }

        sqlRC = SQLPrepare(wdv->hStmtPropertyDeleteAllIndexValues, (SQLCHAR*) SQL_DELETE_ALL_INDEX_VALUES, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtPropertyDeleteAllIndexValues, sqlRC);

            SQLFreeStmt(wdv->hStmtPropertyDeleteAllIndexValues, SQL_DROP);
            wdv->hStmtPropertyDeleteAllIndexValues = 0;

		    return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(wdv->hStmtPropertyDeleteAllIndexValues, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->propertyDeleteAllIndexValuesResourceId, WDVCAPI_MAX_ID_LEN,
                                 &wdv->propertyDeleteAllIndexValuesResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtPropertyDeleteAllIndexValues, sqlRC);

            SQLFreeStmt(wdv->hStmtPropertyDeleteAllIndexValues, SQL_DROP);
            wdv->hStmtPropertyDeleteAllIndexValues = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->hStmtPropertyDeleteAllIndexValues, SQL_CLOSE);
    }

    /* Set input parameter */
    SAPDB_memcpy(wdv->propertyDeleteAllIndexValuesResourceId, resourceId, WDVCAPI_MAX_ID_LEN);

    /* Execute delete statement */
    sqlRC = SQLExecute(wdv->hStmtPropertyDeleteAllIndexValues);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStmtPropertyDeleteAllIndexValues, sqlRC);

        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED) &&
            !WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            SQLFreeStmt(wdv->hStmtPropertyDeleteAllIndexValues, SQL_DROP);

		    return WDV_False;
        }
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CopyAll( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropertyHandle  hProperty,
                           WDVCAPI_Id              sourceId,
                           WDVCAPI_Id              destinationId,
                           WDV_Bool                withDisplayName )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          hStmt = 0;
	SQLCHAR           sqlStmt [1024] = "";
	WDVCAPI_IdString  sourceIdString = "";
	WDVCAPI_IdString  destinationIdString = "";

    /* Get id strings */
	WDVCAPI_IdAsString(sourceId, sourceIdString);
	WDVCAPI_IdAsString(destinationId, destinationIdString);

	/* Build SQL statement to delete old properties */
    if (withDisplayName == WDV_True) {
	    sp77sprintf(sqlStmt, 1024, "DELETE WEBDAV_PROPERTY WHERE CID = X'%s'", destinationIdString);
    } else {
	    sp77sprintf(sqlStmt, 1024, "DELETE WEBDAV_PROPERTY WHERE CID = X'%s' AND Property_Id != X'"ID_PROPERTY_DISPLAYNAME_STRING"'", destinationIdString);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

		return WDV_False;
	}

    /* Execute delete statement */
    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED) &&
            !WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
	}

    /* Free statement handle */
    SQLFreeStmt(hStmt, SQL_CLOSE);

	/* Build SQL statement to copy properties from source resource to destination resource */
    if (withDisplayName == WDV_True) {
    	sp77sprintf(sqlStmt, 1024, SQL_COPY_PROPERTY, destinationIdString, sourceIdString);
    } else {
    	sp77sprintf(sqlStmt, 1024, SQL_COPY_PROPERTY_EXCEPT_DISPLAYNAME, destinationIdString, sourceIdString);
    }

    /* Execute copy statement */
    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED) &&
            !WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
	}

    /* Free statement handle */
    SQLFreeStmt(hStmt, SQL_DROP);

    /* Set getlastmodified */
    /*
    if (!Property_SetLastModified(wdv, destinationId)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }
    */

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_Create( WDVCAPI_WDV           wdv,
                          WDVCAPI_NameSpace     nameSpace,
                          WDVCAPI_PropertyName  propertyName )
{

    SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          hStmt = 0;
	SQLCHAR           sqlStmt [1024] = "";
    WDVCAPI_Id        newId;
	WDVCAPI_IdString  nameSpaceIdString = "";
	WDVCAPI_IdString  propertyIdString = "";

    /* Is name space known? */
    if (!NameSpace_GetIdAsString(wdv, nameSpace, nameSpaceIdString)) {
        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_UNKNOWN_NAME_SPACE)) {
            /* Create name space */
            if (!NameSpace_Create(wdv, nameSpace, nameSpaceIdString)) {
                return WDV_False;
            }
        } else {
            return WDV_False;
        }
    }

    /* Get new property id */
    if (!WDVCAPI_IdGetNext(wdv, newId)) {
        return WDV_False;
    }
	WDVCAPI_IdAsString(newId, propertyIdString);

	/* Build SQL statement to create new property */
	sp77sprintf(sqlStmt, 1024, SQL_INSERT_PROPERTY, propertyIdString, nameSpaceIdString, propertyName);

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

/*===========================================================================*/

WDV_Bool Property_GetShortValue( WDVCAPI_WDV                wdv,
                                 WDVCAPI_Id                 resourceId,
                                 WDVCAPI_Id                 propertyId,
                                 WDVCAPI_PropertyShortValue propertyShortValue )
{

	SQLRETURN                           sqlRC = 0;
    SQLHDBC                             hDBC = 0;
    WDVCAPI_PropertyHandleGetShortValue hPropertyGetShortValue = NULL;

    /* Get statement handle */
    WDV_GetHandlePropertyGetShortValue(wdv, &hPropertyGetShortValue);

	/* Set input values                         */
	SAPDB_memcpy(hPropertyGetShortValue->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
	SAPDB_memcpy(hPropertyGetShortValue->propertyId, propertyId, WDVCAPI_MAX_ID_LEN);

    /* Statement already parsed? */
    if (!hPropertyGetShortValue->hStmt) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPropertyGetShortValue->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertyGetShortValue->hStmt, sqlRC );
    	    SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
            hPropertyGetShortValue->hStmt = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare( hPropertyGetShortValue->hStmt, (SQLCHAR*) SQL_SELECT_SHORT_VALUE, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertyGetShortValue->hStmt, sqlRC );
    	    SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
            hPropertyGetShortValue->hStmt = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hPropertyGetShortValue->hStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertyGetShortValue->resourceId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertyGetShortValue->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyGetShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
            hPropertyGetShortValue->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( hPropertyGetShortValue->hStmt, 2,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertyGetShortValue->propertyId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertyGetShortValue->propertyIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyGetShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
            hPropertyGetShortValue->hStmt = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hPropertyGetShortValue->hStmt, 1, SQL_C_CHAR,
                           hPropertyGetShortValue->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hPropertyGetShortValue->propertyShortValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertyGetShortValue->hStmt, sqlRC );
    	    SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_DROP);
            hPropertyGetShortValue->hStmt = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(hPropertyGetShortValue->hStmt, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hPropertyGetShortValue->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropertyGetShortValue->hStmt, sqlRC );

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(hPropertyGetShortValue->hStmt);
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_PROPERTY_DOESNT_EXIST,
                               WDVCAPI_ERR_TEXT_PROPERTY_DOESNT_EXIST);
        } else {
            Error_AddSQLErrorItem( wdv, hPropertyGetShortValue->hStmt, sqlRC );
        }

		return WDV_False;
	}

    /* Set return value */
    strcpy(propertyShortValue, hPropertyGetShortValue->propertyShortValue);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_DeleteProperty( WDVCAPI_WDV   wdv,
                                  WDVCAPI_Id    resourceId,
                                  WDVCAPI_Id    propertyId )
{

	SQLRETURN                           sqlRC = 0;

    /* Is statement already prepared? */
    if (!wdv->propertyDeleteStmt) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->propertyDeleteStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->propertyDeleteStmt, sqlRC );
		    return WDV_False;
	    }

	    /* Parse SQL */
	    sqlRC = SQLPrepare(wdv->propertyDeleteStmt, (SQLCHAR*) SQL_PROPERTY_DELETE, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->propertyDeleteStmt, sqlRC);
		    SQLFreeStmt(wdv->propertyDeleteStmt, SQL_DROP);
            wdv->propertyDeleteStmt = 0;

            return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( wdv->propertyDeleteStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  wdv->propertyDeleteResourceId, WDVCAPI_MAX_ID_LEN,
                                  &wdv->propertyDeleteResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->propertyDeleteStmt, sqlRC);

            SQLFreeStmt(wdv->propertyDeleteStmt, SQL_DROP);
            wdv->propertyDeleteStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( wdv->propertyDeleteStmt, 2,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  wdv->propertyDeletePropertyId, WDVCAPI_MAX_ID_LEN,
                                  &wdv->propertyDeletePropertyIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->propertyDeleteStmt, sqlRC);

            SQLFreeStmt(wdv->propertyDeleteStmt, SQL_DROP);
            wdv->propertyDeleteStmt = 0;

		    return WDV_False;
	    }
    }

    /* Set members                              */
    SAPDB_memcpy(wdv->propertyDeleteResourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(wdv->propertyDeletePropertyId, propertyId, WDVCAPI_MAX_ID_LEN);

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->propertyDeleteStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, wdv->propertyDeleteStmt, sqlRC );
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_GetResourceIdByShortValueAndPropertyId( WDVCAPI_WDV                wdv,
                                                          WDVCAPI_Id                 propertyId,
                                                          WDVCAPI_PropertyShortValue propertyShortValue,
                                                          WDVCAPI_Id                 cId )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
    SQLLEN              cIdIndicator;
    WDVCAPI_IdString    propertyIdString = "";

    /* Convert property id to an id string */
    WDVCAPI_IdAsString(propertyId, propertyIdString);

	/* Build SQL statement */
	sp77sprintf( sqlStmt, 1024, SQL_SELECT_CID_BY_SHORT_VALUE_AND_PROPERTY_ID,
                 propertyIdString, propertyShortValue );

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hStmt, 1, SQL_C_BINARY,
                       cId, WDVCAPI_MAX_ID_LEN,
                       &cIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch( hStmt );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_PROPERTY_DOESNT_EXIST,
                               WDVCAPI_ERR_TEXT_PROPERTY_DOESNT_EXIST);
        } else {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
        }

		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

	/* Free statement handle */
	SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_GetDescriptionByShortValue( WDVCAPI_WDV                wdv,
                                              WDVCAPI_PropertyShortValue propertyShortValue,
                                              WDVCAPI_Id                 cId,
                                              WDVCAPI_Id                 propertyId )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
    SQLLEN              cIdIndicator;
    SQLLEN              propertyIdIndicator;

	/* Build SQL statement */
	sp77sprintf( sqlStmt, 1024, SQL_SELECT_DESC_BY_SHORT_VALUE, propertyShortValue );

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hStmt, 1, SQL_C_BINARY,
                       cId, WDVCAPI_MAX_ID_LEN,
                       &cIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

    sqlRC = SQLBindCol(hStmt, 2, SQL_C_BINARY,
                       propertyId, WDVCAPI_MAX_ID_LEN,
                       &propertyIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	SQLFreeStmt(hStmt, SQL_DROP);
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch( hStmt );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_PROPERTY_DOESNT_EXIST,
                               WDVCAPI_ERR_TEXT_PROPERTY_DOESNT_EXIST);
        } else {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
        }

		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

	/* Free statement handle */
	SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateDefaultProperties( WDVCAPI_WDV  wdv,
                                           WDVCAPI_Id   cId,
                                           WDV_Bool     withDisplayName )
{

    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer = "";

    /* Delete all existing properties */
    if (!Property_DeleteAll(wdv, cId, withDisplayName)) {
        return WDV_False;
    }

    /* Insert displayname */
    if (withDisplayName == WDV_True) {
        if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_DISPLAYNAME, "")) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                               WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

            return WDV_False;
        }
    }

    /* Insert resourcetype */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_RESOURCE_TYPE, "null resource")) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }

    /* Insert getcontenttype */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_GET_CONTENT_TYPE, "sapdbwww/nullresource")) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }

    /* Insert getcontentlength */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_GET_CONTENT_LENGTH, "0")) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }

    /* Insert getlastmodified */
    if (!Property_SetLastModified(wdv, cId)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }

    /* Insert lockdiscovery */
/*
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_LOCK_DISCOVERY, ID_INITIAL_STRING)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }
*/

    /* Insert supported lock */
/*
    sp77sprintf(propertyShortValueBuffer,
                WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%d",
                WDVCAPI_SUPPORTED_LOCK_EXCLUSIVE_WRITE);
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_SUPPORTED_LOCK, propertyShortValueBuffer)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
                           WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY);

        return WDV_False;
    }
*/

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_CreateLiveProperties( WDVCAPI_WDV                 wdv,
                                        WDVCAPI_Id                  cId,
                                        WDVCAPI_PropertyShortValue  displayName,
                                        WDVCAPI_PropertyShortValue  resourceType,
                                        WDVCAPI_PropertyShortValue  contentType,
                                        WDVCAPI_PropertyShortValue  contentLength,
                                        WDV_Bool                    deleteExistingProperties )
{

    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer = "";

    /* Delete all existing properties */
    if (deleteExistingProperties == WDV_True) {
        if (!Property_DeleteAll(wdv, cId, WDV_True)) {
            return WDV_False;
        }
    }

    /* Insert displayname */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_DISPLAYNAME, displayName)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

    /* Insert resourcetype */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_RESOURCE_TYPE, resourceType)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

    /* Insert getcontenttype */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_GET_CONTENT_TYPE, contentType)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

    /* Insert getcontentlength */
    if (!Property_SetShortValue(wdv, cId, ID_PROPERTY_GET_CONTENT_LENGTH, contentLength)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

    /* Insert getlastmodified */
    if (!Property_SetLastModified(wdv, cId)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

    /* Insert creationdate */
    if (!Property_SetCreationDate(wdv, cId)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
                            WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY);

        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_SetShortValue( WDVCAPI_WDV                wdv,
                                 WDVCAPI_Id                 resourceId,
                                 WDVCAPI_Id                 propertyId,
                                 WDVCAPI_PropertyShortValue propertyShortValue)
{

	SQLRETURN                           sqlRC = 0;
    SQLHDBC                             hDBC = 0;
    WDVCAPI_PropertyHandleSetShortValue hPropertySetShortValue = NULL;

    /* Get statement handle */
    WDV_GetHandlePropertySetShortValue(wdv, &hPropertySetShortValue);

    /* Set members                              */
    SAPDB_memcpy(hPropertySetShortValue->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(hPropertySetShortValue->propertyId, propertyId, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(hPropertySetShortValue->propertyShortValue, propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    hPropertySetShortValue->propertyShortValueIndicator = SQL_NTS;

    /* Is statement already prepared? */
    if (!hPropertySetShortValue->hStmt) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPropertySetShortValue->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropertySetShortValue->hStmt, sqlRC );
		    return WDV_False;
	    }

	    /* Parse SQL */
	    sqlRC = SQLPrepare(hPropertySetShortValue->hStmt, (SQLCHAR*) SQL_SET_PROPERTY, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetShortValue->hStmt, sqlRC);
		    SQLFreeStmt(hPropertySetShortValue->hStmt, SQL_DROP);
            hPropertySetShortValue->hStmt = 0;

            return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hPropertySetShortValue->hStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertySetShortValue->resourceId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertySetShortValue->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertySetShortValue->hStmt, SQL_DROP);
            hPropertySetShortValue->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( hPropertySetShortValue->hStmt, 2,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertySetShortValue->propertyId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertySetShortValue->propertyIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertySetShortValue->hStmt, SQL_DROP);
            hPropertySetShortValue->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( hPropertySetShortValue->hStmt, 3,
                                  SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                  hPropertySetShortValue->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                  &hPropertySetShortValue->propertyShortValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertySetShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertySetShortValue->hStmt, SQL_DROP);
            hPropertySetShortValue->hStmt = 0;

		    return WDV_False;
	    }
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hPropertySetShortValue->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropertySetShortValue->hStmt, sqlRC );
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_UpdateShortValue( WDVCAPI_WDV                wdv,
                                    WDVCAPI_Id                 resourceId,
                                    WDVCAPI_Id                 propertyId,
                                    WDVCAPI_PropertyShortValue propertyShortValue)
{

	SQLRETURN                               sqlRC = 0;
    SQLHDBC                                 hDBC = 0;
    WDVCAPI_PropertyHandleUpdateShortValue  hPropertyUpdateShortValue = NULL;

    /* Get statement handle */
    WDV_GetHandlePropertyUpdateShortValue(wdv, &hPropertyUpdateShortValue);

    /* Set members */
    SAPDB_memcpy(hPropertyUpdateShortValue->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(hPropertyUpdateShortValue->propertyId, propertyId, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(hPropertyUpdateShortValue->propertyShortValue, propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);

    /* Is statement already prepared? */
    if (!hPropertyUpdateShortValue->hStmt) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPropertyUpdateShortValue->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyUpdateShortValue->hStmt, sqlRC);
		    return WDV_False;
	    }

	    /* Parse SQL */
	    sqlRC = SQLPrepare(hPropertyUpdateShortValue->hStmt, (SQLCHAR*) SQL_UPDATE_PROPERTY, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyUpdateShortValue->hStmt, sqlRC);
		    SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_DROP);
            hPropertyUpdateShortValue->hStmt = 0;

            return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hPropertyUpdateShortValue->hStmt, 1,
                                  SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                  hPropertyUpdateShortValue->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                  &hPropertyUpdateShortValue->propertyShortValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyUpdateShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_DROP);
            hPropertyUpdateShortValue->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( hPropertyUpdateShortValue->hStmt, 2,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertyUpdateShortValue->resourceId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertyUpdateShortValue->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyUpdateShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_DROP);
            hPropertyUpdateShortValue->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter( hPropertyUpdateShortValue->hStmt, 3,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hPropertyUpdateShortValue->propertyId, WDVCAPI_MAX_ID_LEN,
                                  &hPropertyUpdateShortValue->propertyIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPropertyUpdateShortValue->hStmt, sqlRC);

            SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_DROP);
            hPropertyUpdateShortValue->hStmt = 0;

		    return WDV_False;
	    }
    } else {
		SQLFreeStmt(hPropertyUpdateShortValue->hStmt, SQL_CLOSE);
    }

    hPropertyUpdateShortValue->propertyShortValueIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(hPropertyUpdateShortValue->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropertyUpdateShortValue->hStmt, sqlRC );
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

XMLXPath_Idx_RetCode Property_SetIndex( void             *parserUserData,
                                        void	         *indexUserData,
                                        const SAPDB_UTF8 *indexValue )
{

    WDVCAPI_XmlIndex    hXmlIndex = NULL;
    WDVCAPI_WDV         hWDV = NULL;
	SQLRETURN           sqlRC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
    WDV_Bool            rc = WDV_False;
    WDVCAPI_IdString    idString = "";

    /* Inits */
    hXmlIndex = (WDVCAPI_XmlIndex)indexUserData;
    hWDV = hXmlIndex->hWDV;

    /* Checks */
    if (!hWDV) {
        return WDV_False;
    }

	/* Set current input values */
    /* DaK */
    /* Common_StrMaxCopy(hWDV->hPropertySetXmlIndex->indexNameSpace, hXmlIndex->index.indexNameSpace, WDV_MAX_NAME_SPACE_LEN);*/
	/* Due to changes in XMLIMAPI a XMLIndex no longer holds a value for the namespace. Instead the
	   fixed Value for Namespace used to be set in XMLIMAPI now will be set right here: */
    Common_StrMaxCopy(hWDV->hPropertySetXmlIndex->indexNameSpace, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", WDV_MAX_NAME_SPACE_LEN);
    Common_StrMaxCopy(hWDV->hPropertySetXmlIndex->indexName, hXmlIndex->index.indexName, WDV_MAX_PROPERTY_NAME_PREFIX_LEN);
    Common_StrMaxCopy(hWDV->hPropertySetXmlIndex->indexValue, (char*)indexValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);

    /* Is statement already parsed? */
    if (!hWDV->hPropertySetXmlIndex->hStmt) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hWDV->hDBC, &hWDV->hPropertySetXmlIndex->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);
		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hWDV->hPropertySetXmlIndex->hStmt, (SQLCHAR*) SQL_SET_INDEX, strlen(SQL_SET_INDEX));
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);
		    return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hWDV->hPropertySetXmlIndex->hStmt, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hWDV->hPut->cid, WDVCAPI_MAX_ID_LEN,
                                 &hWDV->hPut->cIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);

            SQLFreeStmt(hWDV->hPropertySetXmlIndex->hStmt, SQL_DROP);
            hWDV->hPropertySetXmlIndex->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hWDV->hPropertySetXmlIndex->hStmt, 2,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hWDV->hPropertySetXmlIndex->indexValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                 &hWDV->hPropertySetXmlIndex->indexValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);

            SQLFreeStmt(hWDV->hPropertySetXmlIndex->hStmt, SQL_DROP);
            hWDV->hPropertySetXmlIndex->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hWDV->hPropertySetXmlIndex->hStmt, 3,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hWDV->hPropertySetXmlIndex->indexNameSpace, WDV_MAX_NAME_SPACE_LEN,
                                 &hWDV->hPropertySetXmlIndex->indexNameSpaceIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);

            SQLFreeStmt(hWDV->hPropertySetXmlIndex->hStmt, SQL_DROP);
            hWDV->hPropertySetXmlIndex->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hWDV->hPropertySetXmlIndex->hStmt, 4,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hWDV->hPropertySetXmlIndex->indexName, WDV_MAX_PROPERTY_NAME_PREFIX_LEN,
                                 &hWDV->hPropertySetXmlIndex->indexNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);

            SQLFreeStmt(hWDV->hPropertySetXmlIndex->hStmt, SQL_DROP);
            hWDV->hPropertySetXmlIndex->hStmt = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hWDV->hPropertySetXmlIndex->hStmt, SQL_CLOSE);
    }

    hWDV->hPropertySetXmlIndex->indexValueIndicator = SQL_NTS;
    hWDV->hPropertySetXmlIndex->indexNameSpaceIndicator = SQL_NTS;
    hWDV->hPropertySetXmlIndex->indexNameIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(hWDV->hPropertySetXmlIndex->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);

        if (WDVCAPI_IsState(hWDV, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            /* Maybe the property is unknown in system? */
            /* DaK */
            /* if (Property_Create(hWDV, hXmlIndex->index.indexNameSpace, hXmlIndex->index.indexName)) {*/
			/* Due to changes in XMLIMAPI a XMLIndex no longer holds a value for the namespace. Instead the
	   		   fixed Value for Namespace used to be set in XMLIMAPI now will be set right here: */
            if (Property_Create(hWDV, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", hXmlIndex->index.indexName)) {
                /* Try this proppatch again */
	            sqlRC = SQLExecute(hWDV->hPropertySetXmlIndex->hStmt);
	            if ( sqlRC != 0 ) {
                    Error_AddSQLErrorItem(hWDV, hWDV->hPropertySetXmlIndex->hStmt, sqlRC);
                } else {
                    rc = WDV_True;
                }
            }

            if (!rc) {
		        Error_AddErrorItem(hWDV, WDVCAPI_ERR_TYPE_CAPI,
                                   WDVCAPI_ERR_CODE_COULD_NOT_SET_PROPERTY,
                                   WDVCAPI_ERR_TEXT_COULD_NOT_SET_PROPERTY);
            }
        }

        if (!rc) {
		    return WDV_False;
        }
	}

	/* Return */
	return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Property_InitHandle( WDVCAPI_WDV             wdv,
                              WDVCAPI_PropertyHandle  hProperty )
{

    hProperty->hStmt = 0;

    memset(hProperty->cid, 0, WDVCAPI_MAX_ID_LEN);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleSetXmlIndex( WDVCAPI_WDV                       wdv,
                                         WDVCAPI_PropertyHandleSetXmlIndex hPropertySetXmlIndex )
{

    hPropertySetXmlIndex->hStmt = 0;

    hPropertySetXmlIndex->indexNameSpace[0] = '\0';
    hPropertySetXmlIndex->indexName[0] = '\0';
    hPropertySetXmlIndex->indexValue[0] = '\0';

    hPropertySetXmlIndex->indexNameSpaceIndicator = SQL_NTS;
    hPropertySetXmlIndex->indexNameIndicator = SQL_NTS;
    hPropertySetXmlIndex->indexValueIndicator = SQL_NTS;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleSetShortValue( WDVCAPI_WDV                         wdv,
                                           WDVCAPI_PropertyHandleSetShortValue hPropertySetShortValue )
{

    hPropertySetShortValue->hStmt = 0;

    memset(hPropertySetShortValue->resourceId, 0, WDVCAPI_MAX_ID_LEN);
    memset(hPropertySetShortValue->propertyId, 0, WDVCAPI_MAX_ID_LEN);
    hPropertySetShortValue->propertyShortValue[0] = '\0';

    hPropertySetShortValue->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertySetShortValue->propertyIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertySetShortValue->propertyShortValueIndicator = SQL_NTS;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleGetShortValue( WDVCAPI_WDV                         wdv,
                                           WDVCAPI_PropertyHandleGetShortValue hPropertyGetShortValue )
{

    hPropertyGetShortValue->hStmt = 0;

    memset(hPropertyGetShortValue->resourceId, 0, WDVCAPI_MAX_ID_LEN);
    memset(hPropertyGetShortValue->propertyId, 0, WDVCAPI_MAX_ID_LEN);
    hPropertyGetShortValue->propertyShortValue[0] = '\0';

    hPropertyGetShortValue->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertyGetShortValue->propertyIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertyGetShortValue->propertyShortValueIndicator = SQL_NTS;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleUpdateShortValue( WDVCAPI_WDV                            wdv,
                                              WDVCAPI_PropertyHandleUpdateShortValue hPropertyUpdateShortValue )
{

    hPropertyUpdateShortValue->hStmt = 0;

    memset(hPropertyUpdateShortValue->resourceId, 0, WDVCAPI_MAX_ID_LEN);
    memset(hPropertyUpdateShortValue->propertyId, 0, WDVCAPI_MAX_ID_LEN);
    hPropertyUpdateShortValue->propertyShortValue[0] = '\0';

    hPropertyUpdateShortValue->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertyUpdateShortValue->propertyIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPropertyUpdateShortValue->propertyShortValueIndicator = SQL_NTS;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleSetLastModified( WDVCAPI_WDV                           wdv,
                                             WDVCAPI_PropertyHandleSetLastModified hPropertySetLastModified )
{

    hPropertySetLastModified->hStmt = 0;

    memset(hPropertySetLastModified->resourceId, 0, WDVCAPI_MAX_ID_LEN);

    hPropertySetLastModified->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Property_InitHandleSetCreationDate( WDVCAPI_WDV                           wdv,
                                             WDVCAPI_PropertyHandleSetCreationDate hPropertySetCreationDate )
{

    hPropertySetCreationDate->hStmt = 0;

    memset(hPropertySetCreationDate->resourceId, 0, WDVCAPI_MAX_ID_LEN);

    hPropertySetCreationDate->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
