/*!**********************************************************************

  module: XMLError_Error.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

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

#ifndef XMLERROR_ERROR_H
#define XMLERROR_ERROR_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WINDOWS.H>

#include "vpa00sql.h"
#include "vpa00sqlext.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLERROR_MAX_ERROR_TEXT_LEN   1023
#define XMLERROR_MAX_STATE_TEXT_LEN   100
#define XMLERROR_MAX_ERROR_NAME_LEN   63

/*---------------------------------------------------------------------------*/

#define XMLERROR_MAX_LOG_FILENAME_LEN     1024

/*---------------------------------------------------------------------------*/

#define XMLERROR_ODBC_STATE_01S04   "01S04"         /* More than one row updated or deleted */

#define XMLERROR_ERR_TEXT_SUCCESS                   ""
#define XMLERROR_ERR_TEXT_INVALID_ODBC_HANDLE       "Invalid ODBC handle"
#define XMLERROR_ERR_TEXT_NO_DATA                   "No data"
#define XMLERROR_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE  "Unknown ODBC return code"
#define XMLERROR_ERR_TEXT_INTERNAL_ERROR            "Internal error"
#define XMLERROR_ERR_TEXT_SQL_ERROR                 "SQL Error:"
#define XMLERROR_ERR_TEXT_SQL_SUCCESS_WITH_INFO     "SQL Info:"
#define XMLERROR_ERR_TEXT_NO_MORE_MEMORY            "No more memory"

#define XMLERROR_ERR_CODE_UNDEFINED                 0
#define XMLERROR_ERR_CODE_INVALID_ODBC_HANDLE       1
#define XMLERROR_ERR_CODE_NO_DATA                   2
#define XMLERROR_ERR_CODE_UNKNOWN_ODBC_RETURN_CODE  3
#define XMLERROR_ERR_CODE_INTERNAL_ERROR            4
#define XMLERROR_ERR_CODE_SQL_ERROR                 5
#define XMLERROR_ERR_CODE_SQL_SUCCESS_WITH_INFO     6
#define XMLERROR_ERR_CODE_NO_MORE_MEMORY            7

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmlerror_error *XMLError_Error;
typedef char                      XMLError_ErrorNameString[XMLERROR_MAX_ERROR_NAME_LEN+1];
typedef char                     *XMLError_ErrorName;
typedef char                      XMLError_ErrorTextString[XMLERROR_MAX_ERROR_TEXT_LEN+1];
typedef char                     *XMLError_ErrorText;
typedef SAPDB_UInt4               XMLError_ErrorCode;
typedef char                      XMLError_ErrorStateString[XMLERROR_MAX_STATE_TEXT_LEN+1];
typedef char                     *XMLError_ErrorState;

/*---------------------------------------------------------------------------*/

typedef char        XMLError_LogFilenameString[XMLERROR_MAX_LOG_FILENAME_LEN+1];
typedef char       *XMLError_LogFilename;

/*---------------------------------------------------------------------------*/

struct st_xmlerror_error {
    XMLError_ErrorNameString   name;
	XMLError_ErrorCode         code;
	XMLError_ErrorTextString   text;
	XMLError_ErrorCode         odbcCode;
	XMLError_ErrorTextString   odbcText;
	XMLError_ErrorStateString  odbcState;
    char                       file[XMLERROR_MAX_LOG_FILENAME_LEN+1];
    int                        line;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Error_Create();

SAPDB_Bool Error_SetLogFilename( XMLError_LogFilename  serviceLogFileName );

SAPDB_Bool Error_Destroy();

SAPDB_Bool Error_IsODBCState ( XMLError_Error        hError,
                               XMLError_ErrorState   odbcState );

SAPDB_Bool Error_Init( XMLError_Error     hError,
                       XMLError_ErrorName errorName );

SAPDB_Bool Error_Clear( XMLError_Error   hError );

SAPDB_Bool Error_Log( XMLError_Error     hError );

SAPDB_Bool Error_Set( char               *file,
                      const int           line,
                      XMLError_Error      hError,
                      XMLError_ErrorCode  errorCode,
                      XMLError_ErrorText  errorText );

SAPDB_Bool Error_SetFormatted ( char               *file,
                                const int           line,
                                XMLError_Error      hError,
                                XMLError_ErrorCode  errorCode,
                                const char         *format,
                                ...);

SAPDB_Bool Error_SetODBC( char           *file,
                          const int       line,
                          XMLError_Error  hError,
                          SQLHENV         hEnv,
                          SQLHDBC         hDBC,
                          SQLHSTMT        hStmt,
						  SQLRETURN       sqlRC );

/* Extended error description */
#define XMLERROR_COMPREHENSIVE_ERROR_DESCRIPTION     1

#ifdef XMLERROR_COMPREHENSIVE_ERROR_DESCRIPTION
    #define XMLERROR_POS                            __FILE__,__LINE__
#else
    #define XMLERROR_POS                            NULL,NULL
#endif

/***********************************************************************

	End

 ***********************************************************************/

#endif
