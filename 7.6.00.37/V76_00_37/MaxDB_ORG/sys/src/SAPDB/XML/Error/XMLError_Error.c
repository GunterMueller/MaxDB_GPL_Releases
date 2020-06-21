/*!**********************************************************************

  module: XMLError_Error.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Error

  description:  Error Handling

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

#include <time.h>

/*---------------------------------------------------------------------------*/

#include "gsp05.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/Lock/XMLLock_Lock.h"

/*---------------------------------------------------------------------------*/

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

SAPDB_Bool SetOdbcErrorMessage( SQLHENV                    hEnv,
                                SQLHDBC                    hDBC,
                                SQLHSTMT                   hStmt,
                                XMLError_ErrorState        state,
                                XMLError_ErrorText         text,
                                XMLError_ErrorCode        *nativeODBCCode );

/***********************************************************************

	Variables

 ***********************************************************************/

static XMLLock_Lock                   hLockLogFile = NULL;
static XMLError_LogFilenameString     logFilenameString;
static XMLError_LogFilename           logFilename = NULL;

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool	Error_Create()
{

    /* Initialize lock for log file */
    if (!hLockLogFile) {
        if (!Lock_Create(&hLockLogFile)) {
            return SAPDB_FALSE;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool	Error_SetLogFilename( XMLError_LogFilename  serviceLogFileName )
{

    if (!serviceLogFileName) {
        logFilename = NULL;
        logFilenameString[0] = '\0';
    }

    /* Get log file name */
    Com_StrMaxCopy(logFilenameString, serviceLogFileName, XMLERROR_MAX_LOG_FILENAME_LEN);
    if (!logFilenameString && strlen(logFilenameString) < 1) {
        logFilename = NULL;
    } else {
        logFilename = logFilenameString;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool	Error_Destroy()
{

    /* Destroy lock for log file */
    if (!hLockLogFile) {
        if (!Lock_Destroy(hLockLogFile)) {
            return SAPDB_FALSE;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool	Error_Init( XMLError_Error     hError,
                        XMLError_ErrorName errorName )
{

    Com_StrMaxCopy(hError->name, errorName, XMLERROR_MAX_ERROR_NAME_LEN);

    hError->code = XMLERROR_ERR_CODE_UNDEFINED;
    hError->text[0] = '\0';
    hError->odbcCode = 0;
    hError->odbcState[0] = '\0';
    hError->odbcText[0] = '\0';
    hError->file[0] = '\0';
    hError->line = 0;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Error_Clear( XMLError_Error   hError )
{

    hError->code = XMLERROR_ERR_CODE_UNDEFINED;
    hError->text[0] = '\0';
    hError->odbcCode = 0;
    hError->odbcState[0] = '\0';
    hError->odbcText[0] = '\0';
    hError->file[0] = '\0';
    hError->line = 0;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool	Error_Log( XMLError_Error     hError )
{

    SAPDB_Int4          logFile = 0;
    tsp05_RteFileError  err;
    char                logText[XMLERROR_MAX_ERROR_TEXT_LEN+200+1];
    char			    timeStampBuffer[100];
    time_t			    t;

    /* Any log requested? */
    if (!logFilename) {
        return SAPDB_TRUE;
    }

    /* Lock log file */
    if (hLockLogFile) {
        Lock_Begin(hLockLogFile);
    }

    /* Does an error exist? */
    if (hError && hError->code != XMLERROR_ERR_CODE_UNDEFINED && hError->odbcCode != XMLERROR_ERR_CODE_UNDEFINED) {
        Lock_End(hLockLogFile);
        return SAPDB_TRUE;
    }

    /* Open log file */
    sqlfopenc (logFilename, sp5vf_binary, sp5vf_append, sp5bk_unbuffered, &logFile, &err);
    if (err.sp5fe_result != vf_ok) {
        logFile = 0;
        Lock_End(hLockLogFile);

        return SAPDB_FALSE;
    }

    /* Create log entry */
    time(&t);
    strftime(timeStampBuffer, sizeof(timeStampBuffer), "%d/%b/%Y:%H:%M:%S", localtime(&t));

    sp77sprintf(logText, XMLERROR_MAX_ERROR_TEXT_LEN+200,
               "[%s][%s][%s][%d]: %d, %s", timeStampBuffer, hError->name, hError->file, hError->line, hError->code, hError->text);

    /* Write log entry */
    sqlfwritec(logFile, logText, strlen(logText), &err);
#ifdef WIN32
    sqlfwritec(logFile, "\r\n", sizeof("\r\n")-1, &err);
#else
    sqlfwritec(logFile, "\n", sizeof("\n")-1, &err);
#endif

    /* ODBC error ? */
    if (hError->odbcCode) {
        sp77sprintf(logText, XMLERROR_MAX_ERROR_TEXT_LEN+200,
                   "[%s][%s][%s][%d]: %d, %s, %s", timeStampBuffer, hError->name, hError->file, hError->line, hError->odbcCode, hError->odbcText, hError->odbcState);

        /* Write log entry */
        sqlfwritec(logFile, logText, strlen(logText), &err);
#ifdef WIN32
        sqlfwritec(logFile, "\r\n", sizeof("\r\n")-1, &err);
#else
        sqlfwritec(logFile, "\n", sizeof("\n")-1, &err);
#endif
    }

    /* Close log file */
    sqlfclosec(logFile, sp5vf_close_normal, &err);

    /* Unlock log file */
    if (hLockLogFile) {
        Lock_End(hLockLogFile);
    }

    /* Reset error */
    hError->code = XMLERROR_ERR_CODE_UNDEFINED;
    hError->text[0] = '\0';
    hError->odbcCode = 0;
    hError->odbcState[0] = '\0';
    hError->odbcText[0] = '\0';
    hError->file[0] = '\0';
    hError->line = 0;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Error_IsODBCState ( XMLError_Error        hError,
                               XMLError_ErrorState   odbcState )
{

    if (strcmp(hError->odbcState, odbcState) == 0) {
        return SAPDB_TRUE;
    } else {
        return SAPDB_FALSE;
    }

}

/*===========================================================================*/

SAPDB_Bool Error_Set( char               *file,
                      const int           line,
                      XMLError_Error      hError,
                      XMLError_ErrorCode  errorCode,
                      XMLError_ErrorText  errorText )
{

    if (!hError) {
        return SAPDB_FALSE;
    }

    hError->code = errorCode;

    if (file) {
        Com_StrMaxCopy(hError->file, file, XMLERROR_MAX_ERROR_TEXT_LEN);
        hError->line = line;
    }

    Com_StrMaxCopy(hError->text, errorText, XMLERROR_MAX_ERROR_TEXT_LEN);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Error_SetFormatted ( char               *file,
                                const int           line,
                                XMLError_Error      hError,
                                XMLError_ErrorCode  errorCode,
                                const char         *format,
                                ...)
{

    int             result;
    va_list         args;

    if (!hError) {
        return SAPDB_FALSE;
    }

    if (file) {
        Com_StrMaxCopy(hError->file, file, XMLERROR_MAX_ERROR_TEXT_LEN);
        hError->line = line;
    }

    hError->code = errorCode;

    va_start (args, format);
    sp77vsprintf(hError->text, XMLERROR_MAX_ERROR_TEXT_LEN, format, args);
    va_end (args);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Error_SetODBC ( char           *file,
                           const int       line,
                           XMLError_Error  hError,
                           SQLHENV         hEnv,
                           SQLHDBC         hDBC,
                           SQLHSTMT        hStmt,
						   SQLRETURN       sqlRC )
{

    /* Checks */
    if(!hError) {
        return SAPDB_FALSE;
    }

    /* Init */
    hError->code = 0;
    hError->text[0] = '\0';
    hError->odbcCode = 0;
    hError->odbcState[0] = '\0';
    hError->odbcText[0] = '\0';
    hError->file[0] = '\0';
    hError->line = 0;

    if (file) {
        Com_StrMaxCopy(hError->file, file, XMLERROR_MAX_ERROR_TEXT_LEN);
        hError->line = line;
    }

    switch (sqlRC) {
		case SQL_ERROR:
            hError->code = XMLERROR_ERR_CODE_SQL_ERROR;
            strcpy(hError->text, XMLERROR_ERR_TEXT_SQL_ERROR);
			SetOdbcErrorMessage( hEnv, hDBC, hStmt, hError->odbcState, hError->odbcText, &hError->odbcCode);
			break;
		case SQL_SUCCESS_WITH_INFO:
            hError->code = XMLERROR_ERR_CODE_SQL_SUCCESS_WITH_INFO;
            strcpy(hError->text, XMLERROR_ERR_TEXT_SQL_SUCCESS_WITH_INFO);
			SetOdbcErrorMessage( hEnv, hDBC, hStmt, hError->odbcState, hError->odbcText, &hError->odbcCode);
			break;
		case SQL_INVALID_HANDLE:
	        hError->odbcCode = XMLERROR_ERR_CODE_INVALID_ODBC_HANDLE;
			strcpy(hError->odbcText, XMLERROR_ERR_TEXT_INVALID_ODBC_HANDLE);
			break;
		case SQL_NO_DATA_FOUND:
	        hError->odbcCode = XMLERROR_ERR_CODE_NO_DATA;
			strcpy(hError->odbcText, XMLERROR_ERR_TEXT_NO_DATA);
			break;
		case SQL_SUCCESS:
			strcpy(hError->odbcText, XMLERROR_ERR_TEXT_SUCCESS);
			break;
		default:
	        hError->odbcCode = XMLERROR_ERR_CODE_UNKNOWN_ODBC_RETURN_CODE;
			strcpy(hError->odbcText, XMLERROR_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE);
			break;
    };

	return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool SetOdbcErrorMessage( SQLHENV                    hEnv,
                                SQLHDBC                    hDBC,
                                SQLHSTMT                   hStmt,
                                XMLError_ErrorState        state,
                                XMLError_ErrorText         text,
                                XMLError_ErrorCode        *nativeODBCCode )
{

	SQLRETURN    sqlRC;
    SQLSMALLINT  textLen;

    sqlRC = SQLError(hEnv, hDBC, hStmt,
                     (SQLCHAR*) state, (SQLINTEGER*)nativeODBCCode, (SQLCHAR*)text, XMLERROR_MAX_ERROR_TEXT_LEN, &textLen);

	Com_StrMaxCopy(state, state, XMLERROR_MAX_STATE_TEXT_LEN);

    if (sqlRC == SQL_SUCCESS || sqlRC == SQL_SUCCESS_WITH_INFO) {
		return SAPDB_TRUE;
    } else {
		return SAPDB_FALSE;
    };

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
