/****************************************************************************

  module      : XMLIMAPI_Error.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-03  12:47
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

	#include <XMLIMAPI_Definition.h>
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_CommonInclude.h"
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Error.h"

	/***********************************************************************

		Structs & Types

	 ***********************************************************************/
/*	struct st_xmlimapi_error_list {
		XMLIMAPI_ErrorItem  lastErrorItem;
	};

	struct st_xmlimapi_error_item {
		XMLIMAPI_ErrorType        type;
		XMLIMAPI_ErrorCode        code;
		XMLIMAPI_ErrorTextString  text;
		XMLIMAPI_ErrorStateString state;
		XMLIMAPI_ErrorItem        nextErrorItem;
	};
*/
	/***********************************************************************

		Prototypes

	 ***********************************************************************/

	XMLIMAPI_Bool createErrorItem		( XMLIMAPI_ErrorItem  *errorItem      );
	XMLIMAPI_Bool destroyErrorItem		( XMLIMAPI_ErrorItem  errorItem       );
	void initErrorList					( XMLIMAPI_ErrorList errorList        );
	void initErrorItem					( XMLIMAPI_ErrorItem errorItem        );

	XMLIMAPI_Bool setOdbcErrorMessage	( SQLHENV						hEnv,
										  SQLHDBC						hDBC,
										  SQLHSTMT						hStmt,
										  XMLIMAPI_ErrorStateString	state,
										  XMLIMAPI_ErrorTextString		text,
										  XMLIMAPI_ErrorCode			*nativeODBCCode );

	/***********************************************************************

		API Functions

	 ***********************************************************************/

	XMLIMAPI_Bool XMLIMAPI_ClearErrorList( XMLIMAPI_Handle handle )
	{

		XMLIMAPI_ErrorList  errorList = NULL;

		/* Get error list */
		getErrorList( handle, &errorList );
		if ( !errorList ) {
			return XMLIMAPI_False;
		}

		/* Destroy error items */
		if (!destroyErrorItem(errorList->lastErrorItem)) {
			return XMLIMAPI_False;
		}

		/* Init error list */
		errorList->lastErrorItem = NULL;

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool XMLIMAPI_GetLastError( XMLIMAPI_Handle        handle,
								   XMLIMAPI_ErrorItem *errorItem )
	{

		XMLIMAPI_ErrorList  errorList = NULL;

		if ( !handle || !errorItem ) {
			return XMLIMAPI_False;
		}

		/* Get error list */
		getErrorList( handle, &errorList );
		if ( !errorList ) {
			createErrorItem(errorItem);

			return XMLIMAPI_True;
		}

		/* Get first error item in list */
		*errorItem = errorList->lastErrorItem;
		if ( !*errorItem ) {
			createErrorItem(errorItem);
		}

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool XMLIMAPI_GetNextError( XMLIMAPI_ErrorItem  currentItem,
								   XMLIMAPI_ErrorItem *errorItem )
	{

		if ( !errorItem ) {
			return XMLIMAPI_False;
		}

		/* Init return value */
		*errorItem = NULL;

		/* Get next error item in list */
		if ( !currentItem ) {
			return XMLIMAPI_True;
		} else {
			*errorItem = currentItem->nextErrorItem;
		}

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool XMLIMAPI_FindError( XMLIMAPI_Handle        handle,
								XMLIMAPI_ErrorType  errorType,
								XMLIMAPI_ErrorCode  errorCode,
								XMLIMAPI_ErrorItem *errorItem )
	{

		XMLIMAPI_ErrorList  errorList = NULL;
		XMLIMAPI_ErrorItem currentItem = NULL;

		if ( !handle || !errorItem ) {
			createErrorItem(errorItem);
			return XMLIMAPI_False;
		}

		/* Get error list */
		getErrorList( handle, &errorList );
		if ( !errorList ) {
			createErrorItem(errorItem);

			return XMLIMAPI_True;
		}

		/* Search error list */
		currentItem = errorList->lastErrorItem;
		while(currentItem) {
			if (currentItem->type == errorType && currentItem->code == errorCode) {
				*errorItem = currentItem;

				return XMLIMAPI_True;
			}

			currentItem = currentItem->nextErrorItem;
		}

		createErrorItem(errorItem);

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool XMLIMAPI_IsError( XMLIMAPI_Handle        handle,
							  XMLIMAPI_ErrorType  errorType,
							  XMLIMAPI_ErrorCode  errorCode )
	{

		XMLIMAPI_ErrorList  errorList = NULL;

		/* Get error list */
		getErrorList( handle, &errorList );

		/* Check last error */
		if (errorList) {
			if (errorList->lastErrorItem) {
				if (errorType == XMLIMAPI_ERR_TYPE_UNDEFINED || errorList->lastErrorItem->type == errorType) {
					if (errorCode == XMLIMAPI_ERR_MAPI_CODE_UNDEFINED || errorList->lastErrorItem->code == errorCode) {
						return XMLIMAPI_True;
					}
				}
			}
		}

		return XMLIMAPI_False;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool XMLIMAPI_IsState         ( XMLIMAPI_Handle        handle,
									   XMLIMAPI_ErrorType  errorType,
									   XMLIMAPI_ErrorState errorState )
	{

		XMLIMAPI_ErrorList  errorList = NULL;

		/* Get error list */
		getErrorList( handle, &errorList );

		/* Check last error */
		if (errorList) {
			if (errorList->lastErrorItem) {
				if (errorType == XMLIMAPI_ERR_TYPE_UNDEFINED || errorList->lastErrorItem->type == errorType) {
					if (stricmp(errorState, errorList->lastErrorItem->state) == 0) {
						return XMLIMAPI_True;
					}
				}
			}
		}

		return XMLIMAPI_False;

	}

	/*===========================================================================*/

	void XMLIMAPI_GetErrorType( XMLIMAPI_ErrorItem   error,
							   XMLIMAPI_ErrorType  *errorType )
	{

		*errorType = error->type;

	}

	/*===========================================================================*/

	void  XMLIMAPI_GetErrorCode( XMLIMAPI_ErrorItem   error,
								XMLIMAPI_ErrorCode  *errorCode )
	{

		*errorCode = error->code;

	}

	/*===========================================================================*/

	void  XMLIMAPI_GetErrorText( XMLIMAPI_ErrorItem   error,
								XMLIMAPI_ErrorText  *errorText )
	{

		*errorText = error->text;

	}

	/*===========================================================================*/

	void  XMLIMAPI_GetErrorState( XMLIMAPI_ErrorItem    error,
								 XMLIMAPI_ErrorState  *errorState )
	{

		*errorState = error->state;

	}

	/***********************************************************************

		API Internal Functions

	 ***********************************************************************/

	XMLIMAPI_Bool createErrorList( XMLIMAPI_Handle handle )
	{

		XMLIMAPI_Bool           ok = XMLIMAPI_False;
		XMLIMAPI_ErrorList  newErrorList = NULL;
    
		sqlallocat( sizeof(struct st_xmlimapi_error_list), (XMLIMAPI_UInt1**) &newErrorList, &ok );
		if (ok == XMLIMAPI_True) {
			/* Init error */
			initErrorList(newErrorList);

			/* Set return value */
			setErrorList(handle, newErrorList);

			return XMLIMAPI_True;
		} else {
			setErrorList(handle, NULL);

			return XMLIMAPI_False;
		}

	}

	/*===========================================================================*/

	XMLIMAPI_Bool destroyErrorList( XMLIMAPI_Handle  handle )
	{

		XMLIMAPI_ErrorList  errorList = NULL;

		/* Get error list */
		getErrorList( handle, &errorList );
		if ( errorList == NULL ) {
			return XMLIMAPI_True;
		}

		/* Destroy error items */
		if (!destroyErrorItem(errorList->lastErrorItem)) {
			return XMLIMAPI_False;
		}

		/* Destroy error list itself */
		if (errorList != NULL)
			sqlfree((XMLIMAPI_UInt1*) errorList );

		setErrorList(handle, NULL);

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool addErrorItem( XMLIMAPI_Handle              handle,
						   XMLIMAPI_ErrorType        type,
						   XMLIMAPI_ErrorCode        code,
						   XMLIMAPI_ErrorTextString  text )
	{

		XMLIMAPI_ErrorList  errorList = NULL;
		XMLIMAPI_ErrorItem  newErrorItem = NULL;

		if (!handle) {
			return XMLIMAPI_False;
		}

		/* Get error list */
		getErrorList(handle, &errorList);
		if (!errorList) {
			return XMLIMAPI_False;
		}

		/* Create new error item */
		if (!createErrorItem(&newErrorItem)) {
			return XMLIMAPI_False;
		}

		/* Describe new error item */
		newErrorItem->type          = type;
		newErrorItem->code          = code;
		strncpy(newErrorItem->text, text, XMLIMAPI_MAX_ERROR_TEXT_LEN);
		newErrorItem->text[XMLIMAPI_MAX_ERROR_TEXT_LEN] = '\0';
		memset(newErrorItem->state, 0, XMLIMAPI_MAX_STATE_TEXT_LEN);
		newErrorItem->nextErrorItem = errorList->lastErrorItem;

		/* Add new error item to error list */
		errorList->lastErrorItem = newErrorItem;

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool addSQLErrorItem( XMLIMAPI_Handle  handle,
							  SQLHSTMT     hStmt,
							  SQLRETURN    sqlRC )
	{

		SQLHENV                   hEnv = 0;
		SQLHDBC                   hDBC = 0;
		XMLIMAPI_ErrorStateString  state = "";
		XMLIMAPI_ErrorTextString   text = "";
		XMLIMAPI_ErrorCode         nativeODBCCode = 0;
		XMLIMAPI_ErrorItem         errorItem = NULL;

		/* Checks */
		if(!handle) {
			return XMLIMAPI_False;
		}

		/* Get ODBC handles */
		hEnv = handle->sqlHenv;
		hDBC = handle->sqlHdbc;

		switch (sqlRC) {
			case SQL_ERROR:
				setOdbcErrorMessage( hEnv, hDBC, hStmt, state, text, &nativeODBCCode);
				break;
			case SQL_SUCCESS_WITH_INFO:
				setOdbcErrorMessage( hEnv, hDBC, hStmt, state, text, &nativeODBCCode);
				break;
			case SQL_INVALID_HANDLE:
				nativeODBCCode = XMLIMAPI_ERR_SQL_CODE_INVALID_HANDLE;
				strcpy(text, XMLIMAPI_ERR_SQL_TEXT_INVALID_HANDLE);
				break;
			case SQL_NO_DATA_FOUND:
				nativeODBCCode = XMLIMAPI_ERR_SQL_CODE_NO_DATA;
				strcpy(text, XMLIMAPI_ERR_SQL_TEXT_NO_DATA_FOUND);
				break;
			case SQL_SUCCESS:
				strcpy(text, XMLIMAPI_ERR_SQL_TEXT_SUCCESS);
				break;
			default:
				nativeODBCCode = XMLIMAPI_ERR_MAPI_CODE_UNKNOWN_ODBC_RETURN_CODE;
				strcpy(text, XMLIMAPI_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE);
				break;
		};
    
		addErrorItem(handle, XMLIMAPI_ERR_TYPE_SQL, nativeODBCCode, text);

		if( XMLIMAPI_GetLastError(handle, &errorItem)) {
			sp77sprintf(errorItem->state, XMLIMAPI_MAX_STATE_TEXT_LEN, state);
		}

		return XMLIMAPI_True;

	}

	/***********************************************************************

		Module Functions

	 ***********************************************************************/

	XMLIMAPI_Bool createErrorItem( XMLIMAPI_ErrorItem  *errorItem )
	{

		XMLIMAPI_Bool           ok = XMLIMAPI_False;
		XMLIMAPI_ErrorItem  newErrorItem = NULL;

		if ( !errorItem ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_xmlimapi_error_item), (XMLIMAPI_UInt1**) &newErrorItem, &ok );
		if (ok == XMLIMAPI_True) {
			/* Init error */
			initErrorItem(newErrorItem);

			/* Set return value */
			*errorItem = newErrorItem;

			return XMLIMAPI_True;
		} else {
			*errorItem = NULL;

			return XMLIMAPI_False;
		}

	}

	/*===========================================================================*/

	XMLIMAPI_Bool destroyErrorItem( XMLIMAPI_ErrorItem  errorItem )
	{

		XMLIMAPI_ErrorItem  errorItemToDelete = NULL;

		if ( errorItem == NULL) {
			return XMLIMAPI_True;
		}

		/* Destroy error item list */
		while(errorItem->nextErrorItem != NULL) {
			/* Set next error item to delete */
			errorItemToDelete = errorItem->nextErrorItem;
			errorItem->nextErrorItem = errorItemToDelete->nextErrorItem;

			/* Free error item */
			if (errorItemToDelete != NULL)
				sqlfree((XMLIMAPI_UInt1*) errorItemToDelete);
		}

		/* Destroy error item itself */
		if (errorItem != NULL)
			sqlfree((XMLIMAPI_UInt1*) errorItem );

		return XMLIMAPI_True;

	}

	/*===========================================================================*/

	void initErrorList( XMLIMAPI_ErrorList errorList )
	{
		errorList->lastErrorItem = NULL;

	}

	/*===========================================================================*/

	void initErrorItem( XMLIMAPI_ErrorItem errorItem )
	{

		errorItem->type          = XMLIMAPI_ERR_TYPE_UNDEFINED;
		errorItem->code          = XMLIMAPI_ERR_MAPI_CODE_UNDEFINED;
		sp77sprintf(errorItem->text, XMLIMAPI_MAX_ERROR_TEXT_LEN, XMLIMAPI_ERR_TEXT_UNDEFINED);
		sp77sprintf(errorItem->text, XMLIMAPI_MAX_STATE_TEXT_LEN, XMLIMAPI_ERR_STATE_UNDEFINED);
		errorItem->nextErrorItem = NULL;

	}

	/*===========================================================================*/

	XMLIMAPI_Bool setOdbcErrorMessage( SQLHENV                    hEnv,
								  SQLHDBC                    hDBC,
								  SQLHSTMT                   hStmt,
								  XMLIMAPI_ErrorStateString   state,
								  XMLIMAPI_ErrorTextString    text,
								  XMLIMAPI_ErrorCode         *nativeODBCCode )
	{

		SQLRETURN    sqlRC;
		SQLSMALLINT  textLen;

		sqlRC = SQLError( hEnv, hDBC, hStmt,
						  (SQLCHAR*)state, nativeODBCCode, (SQLCHAR*)text, XMLIMAPI_MAX_ERROR_TEXT_LEN, &textLen );

		if (sqlRC == SQL_SUCCESS || sqlRC == SQL_SUCCESS_WITH_INFO) {
			return XMLIMAPI_True;
		} else {
			return XMLIMAPI_False;
		};

	}


