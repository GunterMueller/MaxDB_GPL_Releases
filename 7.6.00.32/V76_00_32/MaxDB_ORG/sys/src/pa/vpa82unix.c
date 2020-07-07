/*!**********************************************************************

  module: vpa82unix.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Mapping ODBC 2.5 functions to ODBC 3.5 functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"
#include "vpa40DBC.h"
#include "vpa50Env.h"
#include "vpa60Stmt.h"

#include "hpa83odbcinstext.h" /* definition of HODBCINSTPROPERTY */

#ifndef _UNICODE_ODBC
/*!**********************************************************************

  Chapter: Setup for unixODBC

************************************************************************/

int ODBCINSTGetProperties( HODBCINSTPROPERTY hLastProperty )
{
	hLastProperty->pNext 				= (HODBCINSTPROPERTY) apdallo( sizeof(ODBCINSTPROPERTY) );
	hLastProperty 						= hLastProperty->pNext;
	memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    
    hLastProperty->nPromptType			= ODBCINST_PROMPTTYPE_TEXTEDIT;
	strncpy( hLastProperty->szName, "Host", INI_MAX_PROPERTY_NAME );
	strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );

	hLastProperty->pNext 				= (HODBCINSTPROPERTY) apdallo( sizeof(ODBCINSTPROPERTY) );
	hLastProperty 						= hLastProperty->pNext;
	memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    
	hLastProperty->nPromptType			= ODBCINST_PROMPTTYPE_TEXTEDIT;
	strncpy( hLastProperty->szName, "Database", INI_MAX_PROPERTY_NAME );
	strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );

	hLastProperty->pNext 				= (HODBCINSTPROPERTY) apdallo( sizeof(ODBCINSTPROPERTY) );
	hLastProperty 						= hLastProperty->pNext;
	memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    
	hLastProperty->nPromptType			= ODBCINST_PROMPTTYPE_TEXTEDIT;
	strncpy( hLastProperty->szName, "Tracefilename", INI_MAX_PROPERTY_NAME );
	strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );

	return 1;
} /* ODBCINSTGetProperties */

/*!**********************************************************************

  EndChapter: Setup for unixODBC

************************************************************************/


/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

ODBC_PROC(SQLAllocConnect,( SQLHENV  henv,
                            SQLHDBC *phdbc ),
           ( henv, phdbc ))
{
    return CALL_ODBC( SQLAllocHandle,
                      ( SQL_HANDLE_DBC, henv, phdbc ) );
} /* SQLAllocConnect */


ODBC_PROC( SQLAllocEnv,( SQLHENV  *phenv ),
           ( phenv ))
{
    return CALL_ODBC( SQLAllocHandle,
                      ( SQL_HANDLE_ENV, SQL_NULL_HANDLE, phenv ));
} /* SQLAllocEnv */


ODBC_PROC( SQLAllocStmt,( SQLHDBC   hdbc,
                          SQLHSTMT *phstmt ),
           ( hdbc, phstmt ))
{
    return CALL_ODBC( SQLAllocHandle,
                      ( SQL_HANDLE_STMT, hdbc, phstmt ));
} /* SQLAllocEnv */


ODBC_PROC( SQLBindParam,( SQLHSTMT       hstmt,
                          SQLUSMALLINT   ipar,
                          SQLSMALLINT    fCType,
                          SQLSMALLINT    fSqlType,
                          SQLULEN        cbColDef,
                          SQLSMALLINT    ibScale,
                          SQLPOINTER     rgbValue,
                          SQLLEN        *pcbValue),
          (hstmt,
           ipar,
           fCType,
           fSqlType,
           cbColDef,
           ibScale,
           rgbValue,
           pcbValue))
{
    return CALL_ODBC( SQLBindParameter,( hstmt,
                                         ipar,
                                         SQL_PARAM_INPUT,
                                         fCType,
                                         fSqlType,
                                         cbColDef,
                                         ibScale,
                                         rgbValue,
                                         *pcbValue,
                                         pcbValue ));
} /* SQLBindParam */
#endif  /* of _UNICODE_ODBC */

#ifdef _UNICODE_ODBC
ODBC_PROC( SQLColAttributesW,
           (SQLHSTMT       hstmt,
            SQLUSMALLINT   icol,
            SQLUSMALLINT   fDescType,
            SQLPOINTER     rgbDesc,
            SQLSMALLINT    cbDescMax,
            SQLSMALLINT   *pcbDesc,
            SQLLEN        *pfDesc),
           (hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc))
#else
ODBC_PROC( SQLColAttributes,
           (SQLHSTMT       hstmt,
            SQLUSMALLINT   icol,
            SQLUSMALLINT   fDescType,
            SQLPOINTER     rgbDesc,
            SQLSMALLINT    cbDescMax,
            SQLSMALLINT   *pcbDesc,
            SQLLEN        *pfDesc),
           (hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc))
#endif
{
    /* date-time data-type conversion for SQL_COLUMN_TYPE ... */
#ifdef _UNICODE_ODBC
    return CALL_ODBC( SQLColAttributeW,
                      (hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc));
#else
    return CALL_ODBC( SQLColAttribute,
                      (hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc));
#endif
} /* SQLColAttribute */

#ifdef _UNICODE_ODBC
ODBC_PROC(SQLErrorW,
          (SQLHENV      henv,
           SQLHDBC      hdbc,
           SQLHSTMT     hstmt,
           SQLWCHAR    *szSqlState,
           SQLINTEGER  *pfNativeError,
           SQLWCHAR    *szErrorMsg,
           SQLSMALLINT  cbErrorMsgMax,
           SQLSMALLINT *pcbErrorMsg),
          (henv, hdbc, hstmt, szSqlState, pfNativeError, 
           szErrorMsg, cbErrorMsgMax, pcbErrorMsg))
#else
ODBC_PROC(SQLError,
          (SQLHENV      henv,
           SQLHDBC      hdbc,
           SQLHSTMT     hstmt,
           SQLCHAR     *szSqlState,
           SQLINTEGER  *pfNativeError,
           SQLCHAR     *szErrorMsg,
           SQLSMALLINT  cbErrorMsgMax,
           SQLSMALLINT *pcbErrorMsg),
          (henv, hdbc, hstmt, szSqlState, pfNativeError,
           szErrorMsg, cbErrorMsgMax, pcbErrorMsg))
#endif
{
    SQLRETURN   retcode = SQL_SUCCESS;
    tpa60Stmt  *stmtPtr = NULL;
    tpa40DBC   *dbcPtr  = NULL;
    tpa50Env   *envPtr  = NULL;
    tsp00_Uint4 destBytesWritten, srcBytesParsed;
#ifdef _UNICODE_ODBC
    const tsp77encoding *encodingType = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *encodingType = sp77encodingAscii;
#endif   
    const int charSize = encodingType->fixedCharacterSize;

    if (szSqlState != NULL)
      /* initialize state (00000) and error message ("")  PTS 1110652 */
      sp78convertString (encodingType, szSqlState, 6*charSize,
                         &destBytesWritten,
                         TRUE,
                         sp77encodingAscii, "00000", 5,
                         &srcBytesParsed);

    if (pcbErrorMsg != NULL)
      *pcbErrorMsg = 0;
    if (cbErrorMsgMax >= encodingType->terminatorSize && szErrorMsg != NULL)
      API_MEMCPY (szErrorMsg,
                  encodingType->charTable->terminator.bytes,
                  encodingType->charTable->terminator.byteCount);
    
    if (hstmt != SQL_NULL_HSTMT) {
        if (pa60VerifyStmt( hstmt )) {
            stmtPtr = (tpa60Stmt*) hstmt;
#ifdef _UNICODE_ODBC
            retcode = CALL_ODBC( SQLGetDiagRecW,(SQL_HANDLE_STMT,  hstmt,
                                                 (SQLSMALLINT) (stmtPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#else
            retcode = CALL_ODBC( SQLGetDiagRec,( SQL_HANDLE_STMT,  hstmt,
                                                 (SQLSMALLINT) (stmtPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#endif
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                stmtPtr->errorIndex++;
            }; /* if */
        } else {
            retcode = SQL_INVALID_HANDLE;
        };
    } else if (hdbc != SQL_NULL_HDBC) {
        if (pa40VerifyDBC( hdbc )) {
            dbcPtr = (tpa40DBC*) hdbc;
#ifdef _UNICODE_ODBC
            retcode = CALL_ODBC( SQLGetDiagRecW,(SQL_HANDLE_DBC,  hdbc,
                                                 (SQLSMALLINT)(dbcPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#else
            retcode = CALL_ODBC( SQLGetDiagRec,( SQL_HANDLE_DBC,  hdbc,
                                                 (SQLSMALLINT)(dbcPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#endif
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                dbcPtr->errorIndex++;
            }; /* if */
        } else {
            retcode = SQL_INVALID_HANDLE;
        };
    } else if (henv != SQL_NULL_HENV) {
        if (pa50VerifyEnv(henv)) {
            envPtr = (tpa50Env*) henv;
#ifdef _UNICODE_ODBC
            retcode = CALL_ODBC( SQLGetDiagRecW,(SQL_HANDLE_ENV,  henv,
                                                 (SQLSMALLINT)(envPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#else
            retcode = CALL_ODBC( SQLGetDiagRec,( SQL_HANDLE_ENV,  henv,
                                                 (SQLSMALLINT)(envPtr->errorIndex+1),
                                                 szSqlState,
                                                 pfNativeError,  szErrorMsg,
                                                 cbErrorMsgMax,  pcbErrorMsg));
#endif
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                envPtr->errorIndex++;
            }; /* if */
        } else {
            retcode = SQL_INVALID_HANDLE;
        };
    } else {
        retcode = SQL_INVALID_HANDLE;
    }; /* else */

    return retcode;
} /* SQLError */


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetConnectOptionW,
          (SQLHDBC      hdbc,
           SQLUSMALLINT fOption,
           SQLPOINTER   pvParam),
          (hdbc, fOption, pvParam))
#else
ODBC_PROC(SQLGetConnectOption,
          (SQLHDBC      hdbc,
           SQLUSMALLINT fOption,
           SQLPOINTER   pvParam),
          (hdbc, fOption, pvParam))
#endif
{
#ifdef _UNICODE_ODBC
    return CALL_ODBC( SQLGetConnectAttrW,( hdbc,  fOption,  pvParam,
                                           SQL_MAX_OPTION_STRING_LENGTH,
                                           NULL ));
#else
    return CALL_ODBC( SQLGetConnectAttr, ( hdbc,  fOption,  pvParam,
                                           SQL_MAX_OPTION_STRING_LENGTH,
                                           NULL ));
#endif
} /* SQLGetConnectOption */

#ifdef _UNICODE_ODBC
ODBC_PROC(SQLSetConnectOptionW,
	  (SQLHDBC      hdbc,
	   SQLUSMALLINT fOption,
	   SQLULEN      vParam),
	  (hdbc, fOption, vParam))
#else
ODBC_PROC(SQLSetConnectOption,
	  (SQLHDBC      hdbc,
	   SQLUSMALLINT fOption,
	   SQLULEN      vParam),
	  (hdbc, fOption, vParam))
#endif
{
#ifdef _UNICODE_ODBC
    return CALL_ODBC( SQLSetConnectAttrW,( hdbc, fOption, (SQLPOINTER) vParam, SQL_NTS ));
#else
    return CALL_ODBC( SQLSetConnectAttr, ( hdbc, fOption, (SQLPOINTER) vParam, SQL_NTS ));
#endif
} /* SQLSetConnectOption */


#ifndef _UNICODE_ODBC

ODBC_PROC(SQLFreeConnect, 
          (SQLHDBC    hdbc),
          (hdbc))
{
    return CALL_ODBC( SQLFreeHandle, ( SQL_HANDLE_DBC, hdbc ));
} /* SQLFreeConnect */


ODBC_PROC(SQLFreeEnv,
          (SQLHENV    henv),
          (henv))
{
    return CALL_ODBC( SQLFreeHandle, ( SQL_HANDLE_ENV, henv ));
} /* SQLFreeEnv */


ODBC_PROC(SQLGetStmtOption,
	  (SQLHSTMT   hstmt,
	   UWORD      fOption,
	   PTR        pvParam),
	  (hstmt, fOption, pvParam))
{
    return CALL_ODBC( SQLGetStmtAttr, ( hstmt,
                                        fOption,
                                        pvParam,
                                        SQL_MAX_OPTION_STRING_LENGTH,
                                        NULL ));    
} /* SQLGetStmtOption */


ODBC_PROC(SQLParamOptions,
          (SQLHSTMT        hstmt,
           SQLROWSETSIZE   crow,
           SQLROWSETSIZE  *pirow),
          (hstmt, crow, pirow))
{
    RETCODE retcode;

    retcode = CALL_ODBC( SQLSetStmtAttr,
                         ( hstmt, SQL_ATTR_PARAMSET_SIZE, (PTR) crow, 0 ));
    if (retcode == SQL_SUCCESS) {
        retcode = CALL_ODBC( SQLSetStmtAttr,
                             ( hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR,
                               pirow, 0 ));
    }; /* if */

    return retcode;
} /* SQLParamOptions */


/* SQLSetParam still exists in vpa10gc */

/* SQLSetScrollOptions -> vpa12pc */

ODBC_PROC( SQLSetStmtOption,
           (SQLHSTMT        hstmt,
            SQLUSMALLINT    fOption,
            SQLROWCOUNT     vParam),
           (hstmt, fOption, vParam))
{
    return CALL_ODBC( SQLSetStmtAttr,
                      ( hstmt, fOption, (PTR) vParam, SQL_NTS ));
} /* SQlSetStmtOption */


ODBC_PROC(SQLTransact,
          (SQLHENV    henv,
           SQLHDBC    hdbc,
           UWORD      fType),
          (henv, hdbc, fType))
{
    if (hdbc != SQL_NULL_HDBC) {
        return CALL_ODBC( SQLEndTran,( SQL_HANDLE_DBC, hdbc, fType ));
    } else {
        return CALL_ODBC( SQLEndTran,( SQL_HANDLE_ENV, henv, fType ));
    }; /* else */        
} /* SQLTransact */

#endif  /* of _UNICODE_ODBC */

/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/
