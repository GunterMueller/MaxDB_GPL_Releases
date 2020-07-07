/*!**********************************************************************

  module: WDVCAPI_Error.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Error handling

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_wdvcapi_error_list {
	WDVCAPI_ErrorItem  lastErrorItem;
};

struct st_wdvcapi_error_item {
	WDVCAPI_ErrorType        type;
	WDVCAPI_ErrorCode        code;
	WDVCAPI_ErrorTextString  text;
	WDVCAPI_ErrorStateString state;
	WDVCAPI_ErrorItem        nextErrorItem;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool CreateErrorItem    ( WDVCAPI_ErrorItem  *errorItem      );
WDV_Bool DestroyErrorItem   ( WDVCAPI_ErrorItem  errorItem       );
void InitErrorList          ( WDVCAPI_ErrorList errorList        );
void InitErrorItem          ( WDVCAPI_ErrorItem errorItem        );

WDV_Bool SetOdbcErrorMessage( SQLHENV                    hEnv,
                              SQLHDBC                    hDBC,
                              SQLHSTMT                   hStmt,
                              WDVCAPI_ErrorStateString   state,
                              WDVCAPI_ErrorTextString    text,
                              WDVCAPI_ErrorCode         *nativeODBCCode );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_ClearErrorList( WDVCAPI_WDV wdv )
{

    WDVCAPI_ErrorList  errorList = NULL;

    /* Get error list */
    GetErrorList( wdv, &errorList );
	if ( !errorList ) {
		return WDV_False;
	}

	/* Destroy error items */
	if (!DestroyErrorItem(errorList->lastErrorItem)) {
		return WDV_False;
	}

	/* Init error list */
    errorList->lastErrorItem = NULL;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetLastError( WDVCAPI_WDV        wdv,
							   WDVCAPI_ErrorItem *errorItem )
{

    WDVCAPI_ErrorList  errorList = NULL;

	if ( !wdv || !errorItem ) {
		return WDV_False;
	}

    /* Get error list */
    GetErrorList( wdv, &errorList );
	if ( !errorList ) {
		CreateErrorItem(errorItem);

        return WDV_True;
	}

	/* Get first error item in list */
	*errorItem = errorList->lastErrorItem;
	if ( !*errorItem ) {
		CreateErrorItem(errorItem);
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetNextError( WDVCAPI_ErrorItem  currentItem,
							   WDVCAPI_ErrorItem *errorItem )
{

	if ( !errorItem ) {
		return WDV_False;
	}

    /* Init return value */
    *errorItem = NULL;

	/* Get next error item in list */
	if ( !currentItem ) {
		return WDV_True;
	} else {
		*errorItem = currentItem->nextErrorItem;
	}

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_FindError( WDVCAPI_WDV        wdv,
						    WDVCAPI_ErrorType  errorType,
						    WDVCAPI_ErrorCode  errorCode,
							WDVCAPI_ErrorItem *errorItem )
{

    WDVCAPI_ErrorList  errorList = NULL;
	WDVCAPI_ErrorItem currentItem = NULL;

	if ( !wdv || !errorItem ) {
		CreateErrorItem(errorItem);
		return WDV_False;
	}

    /* Get error list */
    GetErrorList( wdv, &errorList );
	if ( !errorList ) {
		CreateErrorItem(errorItem);

        return WDV_True;
	}

	/* Search error list */
	currentItem = errorList->lastErrorItem;
	while(currentItem) {
		if (currentItem->type == errorType && currentItem->code == errorCode) {
			*errorItem = currentItem;

			return WDV_True;
		}

		currentItem = currentItem->nextErrorItem;
	}

	CreateErrorItem(errorItem);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IsError( WDVCAPI_WDV        wdv,
                          WDVCAPI_ErrorType  errorType,
                          WDVCAPI_ErrorCode  errorCode )
{

    WDVCAPI_ErrorList  errorList = NULL;

    /* Get error list */
    GetErrorList( wdv, &errorList );

    /* Check last error */
	if (errorList) {
		if (errorList->lastErrorItem) {
            if (errorType == WDVCAPI_ERR_TYPE_UNDEFINED || errorList->lastErrorItem->type == errorType) {
			    if (errorCode == WDVCAPI_ERR_CODE_UNDEFINED || errorList->lastErrorItem->code == errorCode) {
				    return WDV_True;
			    }
            }
		}
	}

	return WDV_False;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_IsState         ( WDVCAPI_WDV        wdv,
                                   WDVCAPI_ErrorType  errorType,
                                   WDVCAPI_ErrorState errorState )
{

    WDVCAPI_ErrorList  errorList = NULL;

    /* Get error list */
    GetErrorList( wdv, &errorList );

    /* Check last error */
	if (errorList) {
		if (errorList->lastErrorItem) {
            if (errorType == WDVCAPI_ERR_TYPE_UNDEFINED || errorList->lastErrorItem->type == errorType) {
			    if (strcasecmp(errorState, errorList->lastErrorItem->state) == 0) {
				    return WDV_True;
			    }
            }
		}
	}

	return WDV_False;

}

/*===========================================================================*/

void WDVCAPI_GetErrorType( WDVCAPI_ErrorItem   error,
                           WDVCAPI_ErrorType  *errorType )
{

    *errorType = error->type;

}

/*===========================================================================*/

void  WDVCAPI_GetErrorCode( WDVCAPI_ErrorItem   error,
                            WDVCAPI_ErrorCode  *errorCode )
{

    *errorCode = error->code;

}

/*===========================================================================*/

void  WDVCAPI_GetErrorText( WDVCAPI_ErrorItem   error,
                            WDVCAPI_ErrorText  *errorText )
{

    *errorText = error->text;

}

/*===========================================================================*/

void  WDVCAPI_GetErrorState( WDVCAPI_ErrorItem    error,
                             WDVCAPI_ErrorState  *errorState )
{

    *errorState = error->state;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_SetErrorItemEx( WDVCAPI_ErrorItem   *errorItem,
                                 WDVCAPI_ErrorType    type,
                                 WDVCAPI_ErrorCode    code,
                                 WDVCAPI_ErrorText    text,
                                 char                *file,
                                 const int            line )
{

    WDVCAPI_ErrorItem  newErrorItem = NULL;

    if (!errorItem) {
        return WDV_False;
    }

    if (!CreateErrorItem(&newErrorItem)) {
        return WDV_False;
    }

    /* Describe new error item */
	newErrorItem->type          = type;
	newErrorItem->code          = code;

#ifdef WDV_COMPREHENSIVE_ERROR_DESCRIPTION
    if (text && !file) {
	    strncpy(newErrorItem->text, text, WDV_MAX_ERROR_TEXT_LEN);
	    newErrorItem->text[WDV_MAX_ERROR_TEXT_LEN] = '\0';
    } else if (text && file) {
	    sp77sprintf(newErrorItem->text, WDV_MAX_ERROR_TEXT_LEN, "%s[%s: %d]", text, file, line);
    } else {
        DestroyErrorItem(newErrorItem);

        return WDV_False;
    }
#else
    if (text) {
	    strncpy(newErrorItem->text, text, WDV_MAX_ERROR_TEXT_LEN);
	    newErrorItem->text[WDV_MAX_ERROR_TEXT_LEN] = '\0';
    } else {
        DestroyErrorItem(newErrorItem);

        return WDV_False;
    }
#endif

	memset(newErrorItem->state, 0, WDV_MAX_STATE_TEXT_LEN);

    /* Set return value */
	*errorItem = newErrorItem;

	return WDV_True;

}

/*===========================================================================*/

void WDVCAPI_UnsetErrorItem( WDVCAPI_ErrorItem    errorItem )
{
    DestroyErrorItem(errorItem);
}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool CreateErrorList( WDVCAPI_WDV  wdv )
{

	WDV_Bool           ok = WDV_False;
	WDVCAPI_ErrorList  newErrorList = NULL;
    
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_error_list), (WDV_UInt1**) &newErrorList, &ok );
	if (ok == WDV_True) {
		/* Init error */
		InitErrorList(newErrorList);

		/* Set return value */
		SetErrorList(wdv, newErrorList);

		return WDV_True;
	} else {
		SetErrorList(wdv, NULL);

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyErrorList( WDVCAPI_WDV  wdv )
{

    WDVCAPI_ErrorList  errorList = NULL;

    /* Get error list */
    GetErrorList( wdv, &errorList );
	if ( !errorList ) {
        return WDV_True;
	}

	/* Destroy error items */
	if (!DestroyErrorItem(errorList->lastErrorItem)) {
		return WDV_False;
	}

	/* Destroy error list itself */
    WDVCAPI_Free((WDV_UInt1*) errorList );

	SetErrorList(wdv, NULL);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool AddErrorItemToList( WDVCAPI_WDV       wdv,
				             WDVCAPI_ErrorItem errorItem )
{

    WDVCAPI_ErrorList  errorList = NULL;

    if (!wdv) {
        return WDV_False;
    }

    /* Get error list */
	GetErrorList(wdv, &errorList);
    if (!errorList) {
		return WDV_False;
	}

    errorItem->nextErrorItem = errorList->lastErrorItem;

    /* Add error item to error list */
	errorList->lastErrorItem = errorItem;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool AddErrorItem( WDVCAPI_WDV              wdv,
				       WDVCAPI_ErrorType        type,
				       WDVCAPI_ErrorCode        code,
				       WDVCAPI_ErrorText        text,
                       char                    *file,
                       const int                line )
{

    WDVCAPI_ErrorList  errorList = NULL;
	WDVCAPI_ErrorItem  newErrorItem = NULL;

#ifdef WDV_COMPREHENSIVE_ERROR_DESCRIPTION
    /* An internal error occured                */
    /* With DebugBreak we get a chance to debug */
    if (code == WDVCAPI_ERR_CODE_INVALID_HANDLE ||
        code == WDVCAPI_ERR_CODE_INTERNAL_ERROR ||
        code == WDVCAPI_ERR_SQL_CODE_INVALID_HANDLE) {
#ifdef WIN32
        DebugBreak();
#endif

    }
#endif

    if (!wdv) {
        return WDV_False;
    }

    /* Get error list */
	GetErrorList(wdv, &errorList);
    if (!errorList) {
		return WDV_False;
	}

    /* Create new error item */
	if (!CreateErrorItem(&newErrorItem)) {
		return WDV_False;
	}

    /* Describe new error item */
	newErrorItem->type          = type;
	newErrorItem->code          = code;

#ifdef WDV_COMPREHENSIVE_ERROR_DESCRIPTION
	sp77sprintf(newErrorItem->text, WDV_MAX_ERROR_TEXT_LEN, "%s[%s: %d]", text, file, line);
#else
	strncpy(newErrorItem->text, text, WDV_MAX_ERROR_TEXT_LEN);
	newErrorItem->text[WDV_MAX_ERROR_TEXT_LEN] = '\0';
#endif

	memset(newErrorItem->state, 0, WDV_MAX_STATE_TEXT_LEN);
	newErrorItem->nextErrorItem = errorList->lastErrorItem;

    /* Add new error item to error list */
	errorList->lastErrorItem = newErrorItem;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool AddSQLErrorItem( WDVCAPI_WDV       wdv,
				          SQLHSTMT          hStmt,
						  SQLRETURN         sqlRC,
                          char             *file,
                          const int         line )
{

    SQLHENV                   hEnv = 0;
    SQLHDBC                   hDBC = 0;
	WDVCAPI_ErrorStateString  state = "";
	WDVCAPI_ErrorTextString   text = "";
	WDVCAPI_ErrorCode         nativeODBCCode = 0;
	WDVCAPI_ErrorItem         errorItem = NULL;

    /* Checks */
    if(!wdv) {
        return WDV_False;
    }

    /* Get ODBC handles */
    GetDBEnv(wdv, &hEnv);
    GetDBC(wdv, &hDBC);

    switch (sqlRC) {
		case SQL_ERROR:
			SetOdbcErrorMessage( hEnv, hDBC, hStmt, state, text, &nativeODBCCode);
			break;
		case SQL_SUCCESS_WITH_INFO:
			SetOdbcErrorMessage( hEnv, hDBC, hStmt, state, text, &nativeODBCCode);
			break;
		case SQL_INVALID_HANDLE:
	        nativeODBCCode = WDVCAPI_ERR_SQL_CODE_INVALID_HANDLE;
			strcpy(text, WDVCAPI_ERR_SQL_TEXT_INVALID_HANDLE);
			break;
		case SQL_NO_DATA_FOUND:
	        nativeODBCCode = WDVCAPI_ERR_SQL_CODE_NO_DATA;
			strcpy(text, WDVCAPI_ERR_SQL_TEXT_NO_DATA_FOUND);
			break;
		case SQL_SUCCESS:
			strcpy(text, WDVCAPI_ERR_SQL_TEXT_SUCCESS);
			break;
		default:
	        nativeODBCCode = WDVCAPI_ERR_CODE_UNKNOWN_ODBC_RETURN_CODE;
			strcpy(text, WDVCAPI_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE);
			break;
    };
    
	AddErrorItem(wdv, WDVCAPI_ERR_TYPE_SQL, nativeODBCCode, text, file, line);

	if( WDVCAPI_GetLastError(wdv, &errorItem)) {
		Common_StrMaxCopy(errorItem->state, state, WDV_MAX_STATE_TEXT_LEN);
	}

	return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool CreateErrorItem( WDVCAPI_ErrorItem  *errorItem )
{

	WDV_Bool           ok = WDV_False;
	WDVCAPI_ErrorItem  newErrorItem = NULL;

	if ( !errorItem ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_error_item), (WDV_UInt1**) &newErrorItem, &ok );
	if (ok == WDV_True) {
		/* Init error */
		InitErrorItem(newErrorItem);

		/* Set return value */
		*errorItem = newErrorItem;

		return WDV_True;
	} else {
		*errorItem = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyErrorItem( WDVCAPI_ErrorItem  errorItem )
{

	WDVCAPI_ErrorItem  errorItemToDelete = NULL;

	if ( !errorItem ) {
		return WDV_True;
	}

	/* Destroy error item list */
	while(errorItem->nextErrorItem) {
		/* Set next error item to delete */
		errorItemToDelete = errorItem->nextErrorItem;
		errorItem->nextErrorItem = errorItemToDelete->nextErrorItem;

		/* Free error item */
	    WDVCAPI_Free((WDV_UInt1*) errorItemToDelete);
	}

	/* Destroy error item itself */
    WDVCAPI_Free((WDV_UInt1*) errorItem );

	return WDV_True;

}

/*===========================================================================*/

void InitErrorList( WDVCAPI_ErrorList errorList )
{

	errorList->lastErrorItem = NULL;

}

/*===========================================================================*/

void InitErrorItem( WDVCAPI_ErrorItem errorItem )
{

	errorItem->type          = WDVCAPI_ERR_TYPE_UNDEFINED;
	errorItem->code          = WDVCAPI_ERR_CODE_UNDEFINED;
	Common_StrMaxCopy(errorItem->text, WDVCAPI_ERR_TEXT_UNDEFINED, WDV_MAX_ERROR_TEXT_LEN);
	Common_StrMaxCopy(errorItem->text, WDVCAPI_ERR_STATE_UNDEFINED, WDV_MAX_STATE_TEXT_LEN);
	errorItem->nextErrorItem = NULL;

}

/*===========================================================================*/

WDV_Bool SetOdbcErrorMessage( SQLHENV                    hEnv,
                              SQLHDBC                    hDBC,
                              SQLHSTMT                   hStmt,
                              WDVCAPI_ErrorStateString   state,
                              WDVCAPI_ErrorTextString    text,
                              WDVCAPI_ErrorCode         *nativeODBCCode )
{

	SQLRETURN    sqlRC;
    SQLSMALLINT  textLen;

    sqlRC = SQLError( hEnv, hDBC, hStmt,
                      (SQLCHAR *) state, nativeODBCCode, (SQLCHAR*) text, WDV_MAX_ERROR_TEXT_LEN, &textLen );

    if (sqlRC == SQL_SUCCESS || sqlRC == SQL_SUCCESS_WITH_INFO) {
		return WDV_True;
    } else {
		return WDV_False;
    };

}

/***********************************************************************

	End

 ***********************************************************************/
