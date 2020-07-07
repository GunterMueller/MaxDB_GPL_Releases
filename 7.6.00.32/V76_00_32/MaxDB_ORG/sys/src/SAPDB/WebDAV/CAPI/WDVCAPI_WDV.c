/*!**********************************************************************

  module: WDVCAPI_WDV.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV handling

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Admin.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_END_TRANSACTION         "COMMIT"
#define SQL_CANCEL_TRANSACTION      "ROLLBACK"

#define SQL_START_SUB_TRANSACTION   "SUBTRANS BEGIN"
#define SQL_END_SUB_TRANSACTION     "SUBTRANS END"
#define SQL_CANCEL_SUB_TRANSACTION  "SUBTRANS ROLLBACK"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool InitWDV( WDVCAPI_WDV  wdv,
                  SQLHENV            hEnv,
                  SQLHDBC            hDBC,
                  XMLIMAPI_Handle   hXmlModelling );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_CreateWDV ( WDVCAPI_WDV       *wdv,
                             SQLHENV            hEnv,
                             SQLHDBC            hDBC,
                             XMLIMAPI_Handle   hXmlModelling)
{

	WDV_Bool ok = WDV_False;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat(sizeof(struct st_wdvcapi_wdv), (WDV_UInt1**) wdv, &ok);
	if (ok == WDV_True) {
        if (!InitWDV(*wdv, hEnv, hDBC, hXmlModelling)) {
            WDVCAPI_DestroyWDV(*wdv);
    		*wdv = NULL;

            return WDV_False;
        }

		return WDV_True;
	} else {
		*wdv = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool WDVCAPI_DestroyWDV( WDVCAPI_WDV  wdv )
{

	if ( !wdv ) {
		return WDV_False;
	}

    /* Destruct compression members */
    if (wdv->hStmtSetCompressedLength) {
        SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
        wdv->hStmtSetCompressedLength = 0;
    }

    if (wdv->hStmtGetCompressedLength) {
        SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
        wdv->hStmtGetCompressedLength = 0;
    }

    /* Destroy MkCol handle */
    MkCol_DestroyHandle(wdv, wdv->hMkCol);

    /* Destroy resource handle create inode */
    Resource_DestroyHandleCreateInode(wdv, wdv->hResourceCreateInode);

    /* Destroy property handle update short value */
    Property_DestroyHandleUpdateShortValue(wdv, wdv->hPropertyUpdateShortValue);

    /* Destroy property handle set last modified */
    Property_DestroyHandleSetLastModified(wdv, wdv->hPropertySetLastModified);

    /* Destroy property handle set creation date */
    Property_DestroyHandleSetCreationDate(wdv, wdv->hPropertySetCreationDate);

    /* Destroy property handle get short value */
    Property_DestroyHandleGetShortValue(wdv, wdv->hPropertyGetShortValue);

    /* Destroy property handle set short value */
    Property_DestroyHandleSetShortValue(wdv, wdv->hPropertySetShortValue);

    /* Destroy property handle set XML index */
    Property_DestroyHandleSetXmlIndex(wdv, wdv->hPropertySetXmlIndex);

    /* Destroy transaction management */
    if (wdv->hEndTransaction) {
        SQLFreeStmt(wdv->hEndTransaction, SQL_DROP);
        wdv->hEndTransaction = 0;
    }

    if (wdv->hCancelTransaction) {
        SQLFreeStmt(wdv->hCancelTransaction, SQL_DROP);
        wdv->hCancelTransaction = 0;
    }

    if (wdv->hEndSubTransaction) {
        SQLFreeStmt(wdv->hEndSubTransaction, SQL_DROP);
        wdv->hEndSubTransaction = 0;
    }

    if (wdv->hStartSubTransaction) {
        SQLFreeStmt(wdv->hStartSubTransaction, SQL_DROP);
        wdv->hStartSubTransaction = 0;
    }

    if (wdv->hCancelSubTransaction) {
        SQLFreeStmt(wdv->hCancelSubTransaction, SQL_DROP);
        wdv->hCancelSubTransaction = 0;
    }

    if (wdv->hStmtPropertyDeleteAllIndexValues) {
        SQLFreeStmt(wdv->hStmtPropertyDeleteAllIndexValues, SQL_DROP);
        wdv->hStmtPropertyDeleteAllIndexValues = 0;
    }

	/* Destroy head handle */
	Head_DestroyHandle(wdv);

	/* Destroy delete handle */
	Delete_DestroyHandle(wdv);

	/* Destroy put handle */
	PutDestroyHandle(wdv);

	/* Free get object */
	DestroyGetHandle(wdv);

    /* Destroy admin handle */
    Admin_DestroyHandle(wdv, &wdv->hAdmin);

    /* Destroy XML doc class list */
    if (wdv->hXmlDocClassList) {
        XMLIndexing_DestroyXmlDocClass(wdv->hXmlDocClassList);
    }

    /* Destroy modelling handle */
    if (wdv->hXmlModelling) {
	    XMLIMAPI_destroyHandle(wdv->hXmlModelling);
    }

    /* Destroy container free list */
    Container_DestroyHandleFreeList(wdv, &wdv->hContainerFreeList);

    /* Destroy lock free list */
    Lock_DestroyFreeList(wdv->lockFreeList);

    /* Destroy resource free list */
    Resource_DestroyFreeList(wdv, wdv->resourceFreeList);

    /* Doc class management */
    if (wdv->dcMgtStmtGetDocClassProperties) {
        SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
    }
    if (wdv->dcMgtStmtUpdateDocClass) {
        SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
    }
    if (wdv->dcMgtStmtInsertDocClass) {
        SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
    }
    if (wdv->dcMgtStmtUpdateIndexingState) {
        SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_DROP);
    }
    if (wdv->dcMgtStmtInsertIndexingState) {
        SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_DROP);
    }
    if (wdv->dcMgtStmtDeleteIndexValues) {
        SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
    }

    /* Deleted items */
    if (wdv->hStmtSetParent) {
        SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
    }

    if (wdv->propertyDeleteStmt) {
        SQLFreeStmt(wdv->propertyDeleteStmt, SQL_DROP);
    }

	/* Free error list */
	DestroyErrorList(wdv);

	/* Free WebDAV handle itself */
    WDVCAPI_Free((WDV_UInt1*) wdv );

	return WDV_True;

}

/*===========================================================================*/

void WDV_GetHandleMkCol( WDVCAPI_WDV          wdv,
                         WDVCAPI_MkColHandle *hMkCol )
{

    if ( !wdv ) {
        *hMkCol = NULL;
		return;
	}

    *hMkCol = wdv->hMkCol;

	return;

}

/*===========================================================================*/

void WDV_GetCacheHandle( WDVCAPI_WDV            wdv, 
                         WDVCAPI_ResourceCache *resourceCache )
{

    return;

}

/*===========================================================================*/

void WDV_GetLockFreeList( WDVCAPI_WDV          wdv,
                          WDVCAPI_LockHandle **lockFreeList )
{

    if ( !wdv ) {
        *lockFreeList = NULL;
		return;
	}

    *lockFreeList = &wdv->lockFreeList;

	return;

}

/*===========================================================================*/

void WDV_GetResourceFreeList( WDVCAPI_WDV        wdv,
                              WDVCAPI_Resource **resourceFreeList )
{

    TRACE_OUT(">> WDV_GetResourceFreeList()\n");

    if ( !wdv ) {
        *resourceFreeList = NULL;
		return;
	}

    *resourceFreeList = &wdv->resourceFreeList;

    TRACE_OUT("<< WDV_GetResourceFreeList()\n");

	return;

}

/*===========================================================================*/

void WDV_GetHandlePropertyUpdateShortValue( WDVCAPI_WDV                             wdv,
                                            WDVCAPI_PropertyHandleUpdateShortValue *hPropertyUpdateShortValue )
{

    if ( !wdv ) {
        *hPropertyUpdateShortValue = NULL;
		return;
	}

    *hPropertyUpdateShortValue = wdv->hPropertyUpdateShortValue;

	return;

}

/*===========================================================================*/

void WDV_GetHandlePropertySetShortValue( WDVCAPI_WDV                          wdv,
                                         WDVCAPI_PropertyHandleSetShortValue *hPropertySetShortValue )
{

    if ( !wdv ) {
        *hPropertySetShortValue = NULL;
		return;
	}

    *hPropertySetShortValue = wdv->hPropertySetShortValue;

	return;

}

/*===========================================================================*/

void WDV_GetHandlePropertyGetShortValue( WDVCAPI_WDV                          wdv,
                                         WDVCAPI_PropertyHandleGetShortValue *hPropertyGetShortValue )
{

    if ( !wdv ) {
        *hPropertyGetShortValue = NULL;
		return;
	}

    *hPropertyGetShortValue = wdv->hPropertyGetShortValue;

	return;

}

/*===========================================================================*/

void WDV_GetHandlePropertySetLastModified( WDVCAPI_WDV                            wdv,
                                           WDVCAPI_PropertyHandleSetLastModified *hPropertySetLastModified )
{

    if ( !wdv ) {
        *hPropertySetLastModified = NULL;
		return;
	}

    *hPropertySetLastModified = wdv->hPropertySetLastModified;

	return;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool WDV_EndTransaction( WDVCAPI_WDV  wdv )
{

	SQLRETURN   sqlRC = 0;

    if ( !wdv ) {
		return WDV_False;
	}

    if (!wdv->hEndTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hEndTransaction);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hEndTransaction, sqlRC);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hEndTransaction, (SQLCHAR*) SQL_END_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hEndTransaction, sqlRC);
		    SQLFreeStmt(wdv->hEndTransaction, SQL_DROP);
            wdv->hEndTransaction = 0;

            return WDV_False;
	    }
    }

    /* Commit transaction */
    sqlRC = SQLExecute(wdv->hEndTransaction);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hEndTransaction, sqlRC);
        SQLFreeStmt(wdv->hEndTransaction, SQL_DROP);
        wdv->hEndTransaction = 0;

		return WDV_False;
	}

	return WDV_True;

}

/*===========================================================================*/

/*                                                                           */
/* For WDV_CancelTransaction see define in WDV.H                             */
/*                                                                           */

/*===========================================================================*/

WDV_Bool WDV_StartTransaction( WDVCAPI_WDV  wdv )
{

	SQLRETURN   sqlRC = 0;

	if ( !wdv ) {
		return WDV_False;
	}

    if (!wdv->hCancelTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hCancelTransaction);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hCancelTransaction, sqlRC);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hCancelTransaction, (SQLCHAR*) SQL_CANCEL_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hCancelTransaction, sqlRC);
		    SQLFreeStmt(wdv->hCancelTransaction, SQL_DROP);
            wdv->hCancelTransaction = 0;

            return WDV_False;
	    }
    }

    /* Cancel transaction */
    sqlRC = SQLExecute(wdv->hCancelTransaction);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hCancelTransaction, sqlRC);
        SQLFreeStmt(wdv->hCancelTransaction, SQL_DROP);
        wdv->hCancelTransaction = 0;

		return WDV_False;
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_EndSubTransaction( WDVCAPI_WDV  wdv )
{

	SQLRETURN   sqlRC = 0;

    if ( !wdv ) {
		return WDV_False;
	}

    if (!wdv->hEndSubTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hEndSubTransaction);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hEndSubTransaction, sqlRC);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hEndSubTransaction, (SQLCHAR*) SQL_END_SUB_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hEndSubTransaction, sqlRC);
		    SQLFreeStmt(wdv->hEndSubTransaction, SQL_DROP);
            wdv->hEndSubTransaction = 0;

            return WDV_False;
	    }
    }

    /* Commit transaction */
    sqlRC = SQLExecute(wdv->hEndSubTransaction);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hEndSubTransaction, sqlRC);
        SQLFreeStmt(wdv->hEndSubTransaction, SQL_DROP);
        wdv->hEndSubTransaction = 0;

		return WDV_False;
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_StartSubTransaction( WDVCAPI_WDV  wdv )
{

	SQLRETURN   sqlRC = 0;

	if ( !wdv ) {
		return WDV_False;
	}

    if (!wdv->hStartSubTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hStartSubTransaction);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStartSubTransaction, sqlRC);

		    return WDV_False;
	    }

	    /* Prepare statement */
        sqlRC = SQLPrepare(wdv->hStartSubTransaction, (SQLCHAR*) SQL_START_SUB_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStartSubTransaction, sqlRC);
		    SQLFreeStmt(wdv->hStartSubTransaction, SQL_DROP);
            wdv->hStartSubTransaction = 0;

            return WDV_False;
	    }
    }

    /* Rollback sub transaction */
    sqlRC = SQLExecute(wdv->hStartSubTransaction);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStartSubTransaction, sqlRC);
        SQLFreeStmt(wdv->hStartSubTransaction, SQL_DROP);
        wdv->hStartSubTransaction = 0;

		return WDV_False;
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_CancelSubTransaction( WDVCAPI_WDV  wdv )
{

	SQLRETURN   sqlRC = 0;

	if ( !wdv ) {
		return WDV_False;
	}

    if (!wdv->hCancelSubTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hCancelSubTransaction);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hCancelSubTransaction, sqlRC);

		    return WDV_False;
	    }

	    /* Prepare statement */
        sqlRC = SQLPrepare(wdv->hCancelSubTransaction, (SQLCHAR*) SQL_CANCEL_SUB_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hCancelSubTransaction, sqlRC);
		    SQLFreeStmt(wdv->hCancelSubTransaction, SQL_DROP);
            wdv->hCancelSubTransaction = 0;

            return WDV_False;
	    }
    }

    /* Rollback sub transaction */
    sqlRC = SQLExecute(wdv->hCancelSubTransaction);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hCancelSubTransaction, sqlRC);
        SQLFreeStmt(wdv->hCancelSubTransaction, SQL_DROP);
        wdv->hCancelSubTransaction = 0;

		return WDV_False;
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_SetDBEnv( WDVCAPI_WDV  wdv,
                       SQLHENV      hEnv )
{

	if ( !wdv ) {
		return WDV_False;
	}

    wdv->hEnv = hEnv;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_SetDBC( WDVCAPI_WDV  wdv,
                     SQLHDBC      hDBC )
{

	if ( !wdv ) {
		return WDV_False;
	}

    wdv->hDBC = hDBC;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool GetDBEnv( WDVCAPI_WDV  wdv,
				   SQLHENV     *hEnv )
{

	if ( !wdv ) {
		return WDV_False;
	}

    *hEnv = wdv->hEnv;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool GetDBC( WDVCAPI_WDV  wdv,
				 SQLHDBC     *hDBC )
{

	if ( !wdv ) {
		return WDV_False;
	}

    *hDBC = wdv->hDBC;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool SetPutHandle( WDVCAPI_WDV       wdv,
                       WDVCAPI_PutHandle hPut )
{

	if ( !wdv ) {
		return WDV_False;
	}

    wdv->hPut = hPut;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool GetPutHandle( WDVCAPI_WDV        wdv,
                       WDVCAPI_PutHandle *hPut )
{

	if ( !wdv ) {
		return WDV_False;
	}

    *hPut = wdv->hPut;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool SetDeleteHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_DeleteHandle  hDelete )
{

	if ( !wdv ) {
		return WDV_False;
	}

    wdv->hDelete = hDelete;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool GetDeleteHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_DeleteHandle *hDelete )
{

	if ( !wdv ) {
		return WDV_False;
	}

    *hDelete = wdv->hDelete;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_SetHeadHandle( WDVCAPI_WDV         wdv,
                            WDVCAPI_HeadHandle  hHead )
{

	if ( !wdv ) {
		return WDV_False;
	}

    wdv->hHead = hHead;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDV_GetHeadHandle( WDVCAPI_WDV         wdv,
                            WDVCAPI_HeadHandle *hHead )
{

	if ( !wdv ) {
		return WDV_False;
	}

    *hHead = wdv->hHead;

	return WDV_True;

}

/*===========================================================================*/

void SetErrorList( WDVCAPI_WDV        wdv,
                   WDVCAPI_ErrorList  errorList )
{

    if ( !wdv ) {
		return;
	}

    wdv->errorList = errorList;

}

/*===========================================================================*/

void GetErrorList( WDVCAPI_WDV        wdv,
                   WDVCAPI_ErrorList *errorList )
{

    if ( !wdv ) {
        *errorList = NULL;
		return;
	}

    *errorList = wdv->errorList;

}

/*===========================================================================*/

void SetGetHandle( WDVCAPI_WDV        wdv,
                   WDVCAPI_GetHandle  hGet )
{

    if ( !wdv ) {
		return;
	}

    wdv->hGet = hGet;

}

/*===========================================================================*/

void GetGetHandle( WDVCAPI_WDV        wdv,
                   WDVCAPI_GetHandle *hGet )
{

    if ( !wdv ) {
        *hGet = NULL;
		return;
	}

    *hGet = wdv->hGet;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool InitWDV( WDVCAPI_WDV  wdv,
                  SQLHENV            hEnv,
                  SQLHDBC            hDBC,
                  XMLIMAPI_Handle   hXmlModelling )
{

    if ( !wdv ) {
		return WDV_False;
	}

	/* Inits */
	wdv->hEnv = hEnv;
	wdv->hDBC = hDBC;
    wdv->hGet = NULL;
    wdv->errorList = NULL;

	/* Construct and init error list */
	if (!CreateErrorList(wdv)) {
		return WDV_False;
	}

    /* Create resource free list */
    wdv->resourceFreeList = NULL;

    /* Create lock free list */
    wdv->lockFreeList = NULL;

    /* Create container free list */
    wdv->hContainerFreeList = NULL;

	/* Construct and init get object */
	if (!CreateGetHandle(wdv)) {
		return WDV_False;
	}

	/* Init id handle */
	if (!Id_Init(wdv)) {
		return WDV_False;
	}

    /* Init modelling API */
    wdv->hXmlModelling = hXmlModelling;

    /* Init XML indexing */
    wdv->hXmlDocClassList = NULL;


	/* Construct and init put handle */
	if (!PutCreateHandle(wdv)) {
		return WDV_False;
	}

	/* Construct and init delete handle */
	if (!Delete_CreateHandle(wdv)) {
		return WDV_False;
	}

	/* Construct and init head handle */
	if (!Head_CreateHandle(wdv)) {
		return WDV_False;
	}

    /* Init transaction management */
    wdv->hEndTransaction = 0;
    wdv->hCancelTransaction = 0;

    wdv->hEndSubTransaction = 0;
    wdv->hStartSubTransaction = 0;
    wdv->hCancelSubTransaction = 0;

    wdv->hStmtPropertyDeleteAllIndexValues = 0;
    memset(wdv->propertyDeleteAllIndexValuesResourceId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->propertyDeleteAllIndexValuesResourceIdIndicator = WDVCAPI_MAX_ID_LEN;

    /* Construct property handle set XML index */
    if (!Property_CreateHandleSetXmlIndex(wdv, &wdv->hPropertySetXmlIndex)) {
		return WDV_False;
	}

    /* Construct property handle set short value */
    if (!Property_CreateHandleSetShortValue(wdv, &wdv->hPropertySetShortValue)) {
		return WDV_False;
	}

    /* Construct property handle get short value */
    if (!Property_CreateHandleGetShortValue(wdv, &wdv->hPropertyGetShortValue)) {
		return WDV_False;
	}

    /* Construct property handle set last modified */
    if (!Property_CreateHandleSetLastModified(wdv, &wdv->hPropertySetLastModified)) {
		return WDV_False;
	}

    /* Construct property handle set creation date */
    if (!Property_CreateHandleSetCreationDate(wdv, &wdv->hPropertySetCreationDate)) {
		return WDV_False;
	}

    /* Construct property handle update short value */
    if (!Property_CreateHandleUpdateShortValue(wdv, &wdv->hPropertyUpdateShortValue)) {
		return WDV_False;
	}

    /* Construct resource handle create inode */
    if (!Resource_CreateHandleCreateInode(wdv, &wdv->hResourceCreateInode)) {
		return WDV_False;
	}

    /* Construct MkCol handle */
    if (!MkCol_CreateHandle(wdv, &wdv->hMkCol)) {
		return WDV_False;
	}

    /* Construct compression members */
    wdv->hStmtSetCompressedLength = 0;
    wdv->hStmtGetCompressedLength = 0;

    wdv->compressedLength = 0;
    wdv->compressedLengthIndicator = 0;

    WDVCAPI_IdInitValue(wdv->compressedParentId);
    wdv->compressedParentIdIndicator = WDVCAPI_MAX_ID_LEN;

    wdv->compressedName[0] = '\0';
    wdv->compressedNameIndicator = SQL_NTS;

    /* Create admin handle */
    if (!Admin_CreateHandle(wdv, &wdv->hAdmin)) {
        return WDV_False;
    }

    /* Doc class management */
    wdv->dcMgtStmtGetDocClassProperties = 0;
    wdv->dcMgtStmtUpdateDocClass = 0;
    wdv->dcMgtStmtInsertDocClass = 0;
    wdv->dcMgtStmtUpdateIndexingState = 0;
    wdv->dcMgtStmtInsertIndexingState = 0;
    wdv->dcMgtStmtDeleteIndexValues = 0;
    memset(wdv->dcMgtResourceId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->dcMgtResourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    wdv->dcMgtDocClassId[0] = '\0';
    wdv->dcMgtDocClassIdIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;
    wdv->dcMgtIndexingState[0] = '\0';
    wdv->dcMgtIndexingStateIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;

    /* Deleted items */
    wdv->hStmtSetParent = 0;
    memset(wdv->setParentParentId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->setParentParentIdIndicator = WDVCAPI_MAX_ID_LEN;
    memset(wdv->setParentResourceId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->setParentResourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    memset(wdv->setParentResourceName, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    wdv->setParentResourceNameIndicator = SQL_NTS;

    wdv->propertyDeleteStmt = 0;
    memset(wdv->propertyDeleteResourceId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->propertyDeleteResourceIdIndicator = WDVCAPI_MAX_ID_LEN;
    memset(wdv->propertyDeletePropertyId, 0, WDVCAPI_MAX_ID_LEN);
    wdv->propertyDeletePropertyIdIndicator = WDVCAPI_MAX_ID_LEN;

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
