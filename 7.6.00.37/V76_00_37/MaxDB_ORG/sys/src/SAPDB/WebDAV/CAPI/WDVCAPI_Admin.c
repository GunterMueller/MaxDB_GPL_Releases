/*!**********************************************************************

  module: WDVCAPI_Admin.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Administration

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

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Admin.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_GET_SUM_CONTENT_LENGTH  \
    "SELECT SUM(NUM(Property_Short_Value)) FROM WEBDAV_Property WHERE \
     Property_Id = X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"'"

#define SQL_GET_SUM_COMPRESSED_CONTENT_LENGTH  \
    "SELECT SUM(CompressedLength) FROM WEBDAV_Inode"

#define SQL_GET_SUM_PHYSICAL_LENGTH \
    "SELECT (SUM(NUM(Property_Short_Value))+A.\"Compressed Content Summary\") FROM WEBDAV_Property P, WEBDAV_Inode I, \
    (SELECT SUM(CompressedLength) \"Compressed Content Summary\" FROM WEBDAV_Inode) A \
     WHERE I.CId = P.CId And P.Property_Id = X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"' AND \
     I.CompressedLength = 0 Group BY A.\"Compressed Content Summary\""

#define SQL_GET_SUM_COMPRESSED_RATIO \
    "SELECT SUM(NUM(Property_Short_Value)), \
            SUM(CompressedLength), \
            100 - (SUM(CompressedLength)*100/SUM(NUM(Property_Short_Value))) \
     FROM WEBDAV_Property P, WEBDAV_Inode I \
     WHERE I.CId = P.CId And P.Property_Id = X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"' AND \
           I.CompressedLength <> 0"

#define SQL_GET_SUM_OF_RESOURCES    \
    "SELECT COUNT(CId) FROM WEBDAV_Inode"

#define SQL_GET_SUM_OF_COLLECTIONS  \
    "SELECT COUNT(CId) FROM WEBDAV_Property WHERE Property_Short_Value = '"PROPERTY_VALUE_COLLECTION"'"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_wdvcapi_admin {
	SQLHSTMT			    hStmtSumContentLength;
	SQLHSTMT			    hStmtSumCompressedContentLength;
    SQLHSTMT                hStmtSumPhysicalLength;
	SQLHSTMT			    hStmtSumRatio;
	SQLHSTMT			    hStmtSumOfResources;
	SQLHSTMT			    hStmtSumOfCollections;

	WDVCAPI_BigLengthString sumContentLength;
	SQLLEN   			    sumContentLengthIndicator;

	WDVCAPI_BigLengthString	sumCompressedContentLength;
	SQLLEN  			    sumCompressedContentLengthIndicator;

	WDVCAPI_BigLengthString	sumPhysicalLength;
	SQLLEN  			    sumPhysicalLengthIndicator;

	WDVCAPI_BigLengthString	sumLogicalCompressedLength;
	SQLLEN  			    sumLogicalCompressedLengthIndicator;

	WDVCAPI_BigLengthString	sumPhysicalCompressedLength;
	SQLLEN  			    sumPhysicalCompressedLengthIndicator;

	WDVCAPI_BigLengthString	sumRatio;
	SQLLEN  			    sumRatioIndicator;

	WDVCAPI_BigLengthString	sumOfResources;
	SQLLEN  			    sumOfResourcesIndicator;

	WDVCAPI_BigLengthString	sumOfCollections;
	SQLLEN  			    sumOfCollectionsIndicator;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Admin_InitHandle( WDVCAPI_WDV          wdv,
                           WDVCAPI_AdminHandle  hAdmin );

/*---------------------------------------------------------------------------*/

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_AdminTotalSumContentLength( WDVCAPI_WDV        wdv,
                                             WDVCAPI_BigLength  sumContentLength )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumContentLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumContentLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumContentLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumContentLength = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumContentLength, (SQLCHAR*) SQL_GET_SUM_CONTENT_LENGTH, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumContentLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumContentLength = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumContentLength, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumContentLength, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumContentLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumContentLength, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumContentLength = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumContentLength, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumContentLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumContentLength, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumContentLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumContentLength, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    Common_StrMaxCopy(sumContentLength, wdv->hAdmin->sumContentLength, WDV_MAX_BIG_LENGTH_LENGTH);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_AdminTotalSumCompressedContentLength( WDVCAPI_WDV        wdv,
                                                       WDVCAPI_BigLength  sumCompressedContentLength )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumCompressedContentLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumCompressedContentLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumCompressedContentLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumCompressedContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumCompressedContentLength = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumCompressedContentLength, (SQLCHAR*) SQL_GET_SUM_COMPRESSED_CONTENT_LENGTH, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumCompressedContentLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumCompressedContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumCompressedContentLength = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumCompressedContentLength, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumCompressedContentLength, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumCompressedContentLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumCompressedContentLength, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumCompressedContentLength, SQL_DROP);
            wdv->hAdmin->hStmtSumCompressedContentLength = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumCompressedContentLength, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumCompressedContentLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumCompressedContentLength, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumCompressedContentLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumCompressedContentLength, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    Common_StrMaxCopy(sumCompressedContentLength, wdv->hAdmin->sumCompressedContentLength, WDV_MAX_BIG_LENGTH_LENGTH);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_AdminTotalSumPhysicalLength( WDVCAPI_WDV        wdv,
                                              WDVCAPI_BigLength  sumPhysicalLength )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumPhysicalLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumPhysicalLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumPhysicalLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumPhysicalLength, SQL_DROP);
            wdv->hAdmin->hStmtSumPhysicalLength = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumPhysicalLength, (SQLCHAR*) SQL_GET_SUM_PHYSICAL_LENGTH, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumPhysicalLength, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumPhysicalLength, SQL_DROP);
            wdv->hAdmin->hStmtSumPhysicalLength = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumPhysicalLength, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumPhysicalLength, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumPhysicalLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumPhysicalLength, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumPhysicalLength, SQL_DROP);
            wdv->hAdmin->hStmtSumPhysicalLength = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumPhysicalLength, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumPhysicalLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumPhysicalLength, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumPhysicalLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumPhysicalLength, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    Common_StrMaxCopy(sumPhysicalLength, wdv->hAdmin->sumPhysicalLength, WDV_MAX_BIG_LENGTH_LENGTH);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_AdminTotalCompressedRatio( WDVCAPI_WDV        wdv,
                                            WDVCAPI_BigLength  sumLogicalCompressedLength,
                                            WDVCAPI_BigLength  sumPhysicalCompressedLength,
                                            WDVCAPI_BigLength  sumRatio )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumRatio) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumRatio);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumRatio, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_DROP);
            wdv->hAdmin->hStmtSumRatio = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumRatio, (SQLCHAR*) SQL_GET_SUM_COMPRESSED_RATIO, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumRatio, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_DROP);
            wdv->hAdmin->hStmtSumRatio = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumRatio, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumLogicalCompressedLength, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumLogicalCompressedLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumRatio, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_DROP);
            wdv->hAdmin->hStmtSumRatio = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumRatio, 2, SQL_C_CHAR,
                           wdv->hAdmin->sumPhysicalCompressedLength, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumPhysicalCompressedLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumRatio, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_DROP);
            wdv->hAdmin->hStmtSumRatio = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumRatio, 3, SQL_C_CHAR,
                           wdv->hAdmin->sumRatio, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumRatioIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumRatio, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_DROP);
            wdv->hAdmin->hStmtSumRatio = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumRatio, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumRatio);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumRatio, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumRatio);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumRatio, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    if (wdv->hAdmin->sumRatioIndicator == SQL_NULL_DATA) {
        /* No compressed content! */
        Common_StrMaxCopy(sumLogicalCompressedLength, "0", WDV_MAX_BIG_LENGTH_LENGTH);
        Common_StrMaxCopy(sumPhysicalCompressedLength, "0", WDV_MAX_BIG_LENGTH_LENGTH);
        Common_StrMaxCopy(sumRatio, "0", WDV_MAX_BIG_LENGTH_LENGTH);
    } else {
        Common_StrMaxCopy(sumLogicalCompressedLength, wdv->hAdmin->sumLogicalCompressedLength, WDV_MAX_BIG_LENGTH_LENGTH);
        Common_StrMaxCopy(sumPhysicalCompressedLength, wdv->hAdmin->sumPhysicalCompressedLength, WDV_MAX_BIG_LENGTH_LENGTH);
        Common_StrMaxCopy(sumRatio, wdv->hAdmin->sumRatio, WDV_MAX_BIG_LENGTH_LENGTH);
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_AdminTotalSumOfResources( WDVCAPI_WDV        wdv,
                                           WDVCAPI_BigLength  sumOfResources )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumOfResources) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumOfResources);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumOfResources, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfResources, SQL_DROP);
            wdv->hAdmin->hStmtSumOfResources = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumOfResources, (SQLCHAR*) SQL_GET_SUM_OF_RESOURCES, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumOfResources, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfResources, SQL_DROP);
            wdv->hAdmin->hStmtSumOfResources = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumOfResources, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumOfResources, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumOfResourcesIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfResources, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfResources, SQL_DROP);
            wdv->hAdmin->hStmtSumOfResources = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumOfResources, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumOfResources);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfResources, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumOfResources);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfResources, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    Common_StrMaxCopy(sumOfResources, wdv->hAdmin->sumOfResources, WDV_MAX_BIG_LENGTH_LENGTH);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_AdminTotalSumOfCollections( WDVCAPI_WDV        wdv,
                                             WDVCAPI_BigLength  sumOfCollections )
{

	SQLRETURN   sqlRC = 0;

    if(!wdv || !wdv->hAdmin) {
        return WDV_False;
    }

    /* Statement already parsed? */
    if (!wdv->hAdmin->hStmtSumOfCollections) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hAdmin->hStmtSumOfCollections);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumOfCollections, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfCollections, SQL_DROP);
            wdv->hAdmin->hStmtSumOfCollections = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hAdmin->hStmtSumOfCollections, (SQLCHAR*) SQL_GET_SUM_OF_COLLECTIONS, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hAdmin->hStmtSumOfCollections, sqlRC );

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfCollections, SQL_DROP);
            wdv->hAdmin->hStmtSumOfCollections = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->hAdmin->hStmtSumOfCollections, 1, SQL_C_CHAR,
                           wdv->hAdmin->sumOfCollections, WDV_MAX_BIG_LENGTH_LENGTH,
                           &wdv->hAdmin->sumOfCollectionsIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfCollections, sqlRC);

    	    SQLFreeStmt(wdv->hAdmin->hStmtSumOfCollections, SQL_DROP);
            wdv->hAdmin->hStmtSumOfCollections = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(wdv->hAdmin->hStmtSumOfCollections, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hAdmin->hStmtSumOfCollections);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfCollections, sqlRC);

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hAdmin->hStmtSumOfCollections);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hAdmin->hStmtSumOfCollections, sqlRC);

		return WDV_False;
	}

    /* Set return value */
    Common_StrMaxCopy(sumOfCollections, wdv->hAdmin->sumOfCollections, WDV_MAX_BIG_LENGTH_LENGTH);

	/* Return */
	return WDV_True;

}

/***********************************************************************

	Public Functions

 ***********************************************************************/

WDV_Bool Admin_CreateHandle( WDVCAPI_WDV             wdv,
                             WDVCAPI_AdminHandle    *hAdmin )
{

	WDV_Bool    ok = WDV_False;

	if ( !wdv || !hAdmin) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* We have to allocat new handle */
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_admin), (WDV_UInt1**) hAdmin, &ok );
	if (ok == WDV_False) {
        *hAdmin = NULL;
		return WDV_False;
	}

	/* Init handle */
	Admin_InitHandle(wdv, *hAdmin);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Admin_DestroyHandle( WDVCAPI_WDV             wdv,
                              WDVCAPI_AdminHandle    *hAdmin )
{

    WDVCAPI_AdminHandle hCurrentAdmin = NULL;

	if ( !wdv || !hAdmin) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    hCurrentAdmin = *hAdmin;
    if (!hCurrentAdmin) {
        return WDV_True;
    }

    /* Free statement handles */
    if (hCurrentAdmin->hStmtSumContentLength) {
        SQLFreeStmt(hCurrentAdmin->hStmtSumContentLength, SQL_DROP);
    }

    if (hCurrentAdmin->hStmtSumCompressedContentLength) {
        SQLFreeStmt(hCurrentAdmin->hStmtSumCompressedContentLength, SQL_DROP);
    }

    if (hCurrentAdmin->hStmtSumOfResources) {
        SQLFreeStmt(hCurrentAdmin->hStmtSumOfResources, SQL_DROP);
    }

    if (hCurrentAdmin->hStmtSumOfCollections) {
        SQLFreeStmt(hCurrentAdmin->hStmtSumOfCollections, SQL_DROP);
    }

    if (hCurrentAdmin->hStmtSumRatio) {
        SQLFreeStmt(hCurrentAdmin->hStmtSumRatio, SQL_DROP);
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hCurrentAdmin);

    *hAdmin = NULL;

    return WDV_True;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

WDV_Bool Admin_InitHandle( WDVCAPI_WDV          wdv,
                           WDVCAPI_AdminHandle  hAdmin )
{

    hAdmin->hStmtSumContentLength = 0;
    hAdmin->hStmtSumCompressedContentLength = 0;
    hAdmin->hStmtSumPhysicalLength = 0;
    hAdmin->hStmtSumRatio = 0;
    hAdmin->hStmtSumOfResources = 0;
    hAdmin->hStmtSumOfCollections = 0;

    hAdmin->sumContentLength[0] = '\0';
    hAdmin->sumContentLengthIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumCompressedContentLength[0] = '\0';
    hAdmin->sumCompressedContentLengthIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumPhysicalLength[0] = '\0';
    hAdmin->sumPhysicalLengthIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumLogicalCompressedLength[0] = '\0';
    hAdmin->sumLogicalCompressedLengthIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumPhysicalCompressedLength[0] = '\0';
    hAdmin->sumPhysicalCompressedLengthIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumRatio[0] = '\0';
    hAdmin->sumRatioIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumOfResources[0] = '\0';
    hAdmin->sumOfResourcesIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    hAdmin->sumOfCollections[0] = '\0';
    hAdmin->sumOfCollectionsIndicator = WDV_MAX_BIG_LENGTH_LENGTH;

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
