/*!**********************************************************************

  module: vpa10Handle.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC-Handles (environment,connection,statement,descriptor)

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

#include "vpa10Handle.h"

#include "vpa20Desc.h"
#include "vpa31DiagRec.h"
#include "vpa30DiagArea.h"
#include "vpa30.h"
#include "vpa60Stmt.h"
#include "vpa40DBC.h"
#include "vpa50Env.h"
#include "vpa80String.h"
#include "vpa06.h"
#include "vpa09.h"

#ifdef _UNICODE_ODBC
#include "hsp78_0.h"
#endif

/* external prototype (vpa03wc) */
/*tsp00_Bool SqlIsSapRouterString (char *serverNode);*/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/*!
  Function: 

  description: 

  arguments:
      
  return value: 
  */

tpa30DiagArea* pa10GetDiagArea( SWORD      handleType,
                                SQLHANDLE  handle );

tpa31DiagRec* pa10GetDiagRec( SWORD      handleType,
                              SQLHANDLE  handle,
                              SWORD      recNo );


SWORD pa10GetDynamicFuncCode( tpa60Stmt *stmtPtr );

const char *pa10GetDynamicFunction( SWORD dynFuncCode );

/*!**********************************************************************
  Function: 

  description: 

  arguments:
      
  return value: 
  */

tpa40DBC* pa10GetParentDBC( SWORD handleType, SQLHANDLE handle );


/*!**********************************************************************
  Function: 

  description: 

  arguments:
      
  return value: 
  */

tpa50Env* pa10GetParentEnv( SWORD handleType, SQLHANDLE handle );


/*!**********************************************************************
  Function: 

  description: 

  arguments:
      
  return value: 
  */

SQLRETURN pa10_Transact( SQLHENV henv, SQLHDBC hdbc, UWORD fType);


/*!**********************************************************************
  Function: 

  description: 

  arguments:
      
  return value: 
  */
    
API_RETCODE pa10VerifyHandle( SWORD handleType, SQLHANDLE handle );

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/

/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

#ifndef SAPDB_FAST
#ifndef _UNICODE_ODBC
UCHAR *PA10HANDLE_FN_SQLALLOCHANDLE   = { (UCHAR*) "SQLAllocHandle" };
UCHAR *PA10HANDLE_FN_SQLENDTRAN       = { (UCHAR*) "SQLEndTran" };
UCHAR *PA10HANDLE_FN_SQLFREEHANDLE    = { (UCHAR*) "SQLFreeHandle" };
UCHAR *PA10HANDLE_FN_SQLGETDIAGFIELD  = { (UCHAR*) "SQLGetDiagField" };
UCHAR *PA10HANDLE_FN_SQLGETDIAGREC    = { (UCHAR*) "SQLGetDiagRec" };
#else
extern UCHAR *PA10HANDLE_FN_SQLALLOCHANDLE;
extern UCHAR *PA10HANDLE_FN_SQLENDTRAN;
extern UCHAR *PA10HANDLE_FN_SQLFREEHANDLE;
extern UCHAR *PA10HANDLE_FN_SQLGETDIAGFIELD;
extern UCHAR *PA10HANDLE_FN_SQLGETDIAGREC;
#endif
#endif  /* of SAPDB_FAST */



#ifndef _UNICODE_ODBC
/* ODBC 3.0 */
ODBC_PROC( SQLAllocHandle,( SWORD       handleType,
                            SQLHANDLE   inputHandle,
                            SQLHANDLE  *outputHandle),
           ( handleType, inputHandle, outputHandle ))
{
    SQLRETURN retcode   = SQL_SUCCESS;
    UWORD     sql_state = API_ODBC_00000; /* Success */

    teo07_ThreadErr ok;
    tsp00_ErrTextc  errTxt;

    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124459 */
    pa09InitMultiThreading (errTxt, &ok);

    PA30PROTECTTHREAD();
    
    API_TRACE( API_TR_ENTRY, PA10HANDLE_FN_SQLALLOCHANDLE, 0);
    API_TRACE( API_TR_SWORD, "handleType", &handleType);
    API_TRACE( API_TR_HANDLE, "inputHandle", &inputHandle);
    API_TRACE( API_TR_PTR, "outputHandle", &outputHandle);

    switch( handleType) {
        case SQL_HANDLE_ENV :
            retcode = pa50AllocEnv( outputHandle );
            break;
        case SQL_HANDLE_DBC :
            retcode = pa40AllocConnect( inputHandle, outputHandle );
            break;
        case SQL_HANDLE_STMT :
            retcode = pa60AllocStmt( inputHandle, outputHandle );
        break;
        case SQL_HANDLE_DESC :
            retcode = pa20AllocExplicitDesc( inputHandle, outputHandle );
            break;
        default :
            retcode = SQL_ERROR;
            sql_state = API_ODBC_HY092; /* Invalid attribute/option */
                                        /* identifier               */
            break;
    }; /* switch */
    
    if ( sql_state != API_ODBC_00000 ) {
        /* We cant set sql state HY092 because we don´t know
         * the type of the inputHandle !
         */
    }; /* if */

    API_TRACE( API_TR_EXIT, PA10HANDLE_FN_SQLALLOCHANDLE, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE( API_TR_HANDLE, "*outputHandle", outputHandle);

    PA30UNPROTECTTHREAD();
    return retcode;
} /* SQLAllocHandle */



/* ODBC 3.0 */
ODBC_PROC(SQLEndTran, ( SWORD      handle_type,
                        SQLHANDLE  handle,
                        SWORD      completion),
          (handle_type, handle, completion))
{
   SQLRETURN retcode = SQL_ERROR;
   
   API_TRACE( API_TR_ENTRY, PA10HANDLE_FN_SQLENDTRAN, 0);
   API_TRACE( API_TR_SWORD, "handle_type", &handle_type);
   API_TRACE( API_TR_HANDLE, "handle", &handle);
   API_TRACE( API_TR_SWORD, "handle", &completion);

   switch ( handle_type ) {
   case SQL_HANDLE_ENV :
       retcode = pa10_Transact( handle, SQL_NULL_HDBC, completion );
       break;
   case SQL_HANDLE_DBC :
       retcode = pa10_Transact( SQL_NULL_HENV, handle, completion );
       break;
   case SQL_HANDLE_STMT:
       if (pa60VerifyStmt( handle ) == API_OK) {
           retcode = SQL_ERROR;
           pa60PutError( handle, API_ODBC_HY092, NULL );
           /* invalid attribute/option identifier */
       } else {
           retcode = SQL_INVALID_HANDLE;
       }; /* else */
       break;
   case SQL_HANDLE_DESC:
       if (pa20VerifyDesc( handle ) == API_OK) {
           retcode = SQL_ERROR;
           pa20PutError( handle, API_ODBC_HY092 );
           /* invalid attribute/option identifier */
       } else {
           retcode = SQL_INVALID_HANDLE;
       }; /* else */
       break;
   default:
       retcode = SQL_INVALID_HANDLE;
   }; /* switch */
   
   API_TRACE( API_TR_EXIT, PA10HANDLE_FN_SQLENDTRAN, 0);
   API_TRACE( API_TR_RETCODE, "retcode", &retcode);
   
   return retcode;
} /* SQLEndTran */


/* ODBC 3.0 */
ODBC_PROC( SQLFreeHandle,( SWORD       handleType,
                           SQLHANDLE   handle ),
           ( handleType, handle ))
{
    SQLRETURN retcode = SQL_SUCCESS;
    
    API_TRACE( API_TR_ENTRY, PA10HANDLE_FN_SQLFREEHANDLE, 0);
    API_TRACE( API_TR_SWORD, "handleType", &handleType);
    API_TRACE( API_TR_HANDLE, "phenv", &handle);
  
    switch( handleType) {
    case SQL_HANDLE_ENV :
        retcode = pa50FreeEnv( handle );
        break;
    case SQL_HANDLE_DBC :
        retcode = pa40FreeConnect( handle );
        break;
    case SQL_HANDLE_STMT :
        retcode = pa60DropStmt( handle );
        break;
    case SQL_HANDLE_DESC :
        retcode = pa20FreeDesc( handle );
        break;
    default :
        retcode = SQL_INVALID_HANDLE;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, PA10HANDLE_FN_SQLFREEHANDLE, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);

    return retcode;
} /* SQLFreeHandle */
#endif  /* of _UNICODE_ODBC */

/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetDiagFieldW, (SQLSMALLINT      handleType,
                             SQLHANDLE        handle,
                             SQLSMALLINT      recNo,
                             SQLSMALLINT      diagId,
                             SQLPOINTER       diagInfoPtr,
                             SQLSMALLINT      bufferLen,
                             SQLSMALLINT     *strLenPtr),
          ( handleType, handle, recNo, diagId, diagInfoPtr,
            bufferLen, strLenPtr ))
#else
ODBC_PROC(SQLGetDiagField, ( SQLSMALLINT      handleType,
                             SQLHANDLE        handle,
                             SQLSMALLINT      recNo,
                             SQLSMALLINT      diagId,
                             SQLPOINTER       diagInfoPtr,
                             SQLSMALLINT      bufferLen,
                             SQLSMALLINT     *strLenPtr),
          ( handleType, handle, recNo, diagId, diagInfoPtr,
            bufferLen, strLenPtr ))
#endif
{
    SQLRETURN        retcode = SQL_SUCCESS;
    tpa40DBC        *parentDBC = NULL;
    tpa50Env        *parentEnv = NULL;
    UWORD            sqlState; /* dummy */
    tpa30DiagArea   *diagArea = NULL;
    tpa60Stmt       *stmtPtr = NULL;
    UDWORD          strLen;      
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    SQLSMALLINT charSize = nativeEncoding->fixedCharacterSize;
    
    API_TRACE( API_TR_ENTRY,PA10HANDLE_FN_SQLGETDIAGFIELD, 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_DIAGFIELD, "diagId", &diagId);
    API_TRACE( API_TR_PTR, "diagInfoPtr", &diagInfoPtr );
    API_TRACE( API_TR_SWORD, "bufferLen", &bufferLen);
    API_TRACE( API_TR_PTR, "strLenPtr", &strLenPtr );

    if (pa10VerifyHandle( handleType, handle ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        switch (diagId) {
        case SQL_DIAG_CONNECTION_NAME:
        case SQL_DIAG_SERVER_NAME:
            parentDBC = pa10GetParentDBC( handleType, handle );
            if (parentDBC != NULL  &&  parentDBC->dsname != NULL) {
                retcode = pa80CopyTpr05String( diagInfoPtr,
                                               bufferLen*charSize,
                                               &strLen,
                                               parentDBC->dsname,
                                               &sqlState );
            } else {
                retcode = pa80CopyStringEncoding (nativeEncoding, diagInfoPtr, bufferLen*charSize,
                                                  NULL, &strLen,
                                                  "",
                                                  &sqlState);
            } /* else */
            *strLenPtr = (SQLSMALLINT) strLen;
            break;
            /* the following fields are only defined for
             * executed statements */
        case SQL_DIAG_CURSOR_ROW_COUNT:
            if (handleType == SQL_HANDLE_STMT) {
                stmtPtr = (tpa60Stmt*) handle;
                *((SDWORD*) diagInfoPtr) = stmtPtr->number_rows;
                API_TRACE( API_TR_SDWORD, "rowCount", (SDWORD*) diagInfoPtr);
            } else {
                retcode = SQL_ERROR;
            } /* else */
            break;
        case SQL_DIAG_DYNAMIC_FUNCTION:
            if (handleType == SQL_HANDLE_STMT) {
                stmtPtr = (tpa60Stmt*) handle;
                retcode = pa80CopyStringEncoding (nativeEncoding, diagInfoPtr, bufferLen*charSize,
                                                  NULL, &strLen,
                                                  pa10GetDynamicFunction (pa10GetDynamicFuncCode (stmtPtr)),
                                                  &sqlState);

                *strLenPtr = (SQLSMALLINT) strLen;
            } else {
                retcode = SQL_ERROR;
            } /* else */
            break;
        case SQL_DIAG_DYNAMIC_FUNCTION_CODE:
            if (handleType == SQL_HANDLE_STMT) {
                stmtPtr = (tpa60Stmt*) handle;
                *((SDWORD*) diagInfoPtr) = pa10GetDynamicFuncCode( stmtPtr );
                API_TRACE( API_TR_SDWORD, "dynFuncCode",
                           (SDWORD*) diagInfoPtr);
            } else {
                retcode = SQL_ERROR;
            } /* else */
            break;
        case SQL_DIAG_ROW_COUNT:
            if (handleType == SQL_HANDLE_STMT) {
                stmtPtr = (tpa60Stmt*) handle;
                *((SDWORD*) diagInfoPtr) = stmtPtr->number_rows;
                API_TRACE( API_TR_SDWORD, "rowCount", (SDWORD*) diagInfoPtr);
            } else {
                retcode = SQL_ERROR;
            } /* else */
            break;
        case SQL_DIAG_CONNECT_STRING: /* driver defined */
            parentDBC = pa10GetParentDBC( handleType, handle );
            if (parentDBC!=NULL && parentDBC->driverConnectStr != NULL) {
                retcode = pa80CopyTpr05String( diagInfoPtr,
                                               bufferLen*charSize,
                                               &strLen,
                                               parentDBC->driverConnectStr,
                                               &sqlState );
            } else {
                retcode = pa80CopyStringEncoding (nativeEncoding, diagInfoPtr, bufferLen*charSize,
                                                  NULL, &strLen,
                                                  "",
                                                  &sqlState);
            } /* else */
            *strLenPtr = (SQLSMALLINT) strLen;
            break;
        case (SQL_DIAG_IS_NI_CONNECTION): /* driver defined */
            parentDBC = pa10GetParentDBC( handleType, handle );
            if (parentDBC->servernode != NULL
                && SqlIsSaprouterString( (char*) parentDBC->servernode )) {
                retcode = pa80CopyStringEncoding (nativeEncoding, diagInfoPtr, bufferLen*charSize,
                                                  NULL, &strLen,
                                                  "Y",
                                                  &sqlState);
            } else {
                retcode = pa80CopyStringEncoding (nativeEncoding, diagInfoPtr, bufferLen*charSize,
                                                  NULL, &strLen,
                                                  "N",
                                                  &sqlState);
            }
            *strLenPtr = (SQLSMALLINT) strLen;
            break;
        default:
            diagArea = pa10GetDiagArea( handleType, handle );
            if (diagArea==NULL) {
                retcode = SQL_NO_DATA;
            } else {
                parentEnv = pa10GetParentEnv( handleType, handle );
                if (parentEnv!=NULL) {
                    retcode = pa30GetDiagField( diagArea,
                                                parentEnv->odbc_version,
                                                recNo,
                                                diagId,
                                                diagInfoPtr,
                                                (SQLSMALLINT) (bufferLen*charSize),
                                                strLenPtr,
                                                nativeEncoding);
                } else {
                    retcode = SQL_ERROR;
                }; /* else */
            }; /* else */
            break;
        }; /* switch */
    }; /* else */
    
    API_TRACE( API_TR_EXIT, PA10HANDLE_FN_SQLGETDIAGFIELD, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE( API_TR_SDWORD, "*strLenPtr", strLenPtr );
    return retcode;
} /* SQLGetDiagField */

/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetDiagRecW, (SQLSMALLINT      handleType,
                           SQLHANDLE        handle,
                           SQLSMALLINT      recNo,
                           SQLWCHAR        *sqlState,
                           SQLINTEGER      *nativeErrPtr,
                           SQLWCHAR        *messageText,
                           SQLSMALLINT      bufferLen,
                           SQLSMALLINT     *txtLenPtr ),
          ( handleType, handle, recNo, sqlState, nativeErrPtr,
            messageText, bufferLen, txtLenPtr ))
#else
ODBC_PROC(SQLGetDiagRec, ( SQLSMALLINT      handleType,
                           SQLHANDLE        handle,
                           SQLSMALLINT      recNo,
                           SQLCHAR         *sqlState,
                           SQLINTEGER      *nativeErrPtr,
                           SQLCHAR         *messageText,
                           SQLSMALLINT      bufferLen,
                           SQLSMALLINT     *txtLenPtr ),
          ( handleType, handle, recNo, sqlState, nativeErrPtr,
            messageText, bufferLen, txtLenPtr ))
#endif
{
    SQLRETURN      retcode = SQL_SUCCESS;
    tpa31DiagRec  *diagRec;
    tpa50Env      *parentEnv;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    SQLSMALLINT charSize = nativeEncoding->fixedCharacterSize;
    
    API_TRACE( API_TR_ENTRY,PA10HANDLE_FN_SQLGETDIAGREC, 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_PTR, "sqlState", &sqlState);
    API_TRACE( API_TR_PTR, "nativeErrPtr", &nativeErrPtr );
    API_TRACE( API_TR_PTR, "messageTxt", &messageText);
    API_TRACE( API_TR_SWORD, "bufferLen", &bufferLen);
    API_TRACE( API_TR_PTR, "txtLenPtr", &txtLenPtr );

    switch (handleType) {
    case SQL_HANDLE_ENV :
    case SQL_HANDLE_DBC :
    case SQL_HANDLE_STMT :
    case SQL_HANDLE_DESC :
        if (pa10VerifyHandle( handleType, handle )!=API_OK) {
            retcode = SQL_INVALID_HANDLE;
        } else {
            diagRec = pa10GetDiagRec( handleType, handle, recNo );
            if (diagRec==NULL) {
                retcode = SQL_NO_DATA;
            } else {
                parentEnv = pa10GetParentEnv( handleType, handle );
                if (parentEnv!=NULL) {
                    retcode = pa31GetDiagRec( diagRec,
                                              parentEnv->odbc_version,
                                              (SQLCHAR*) sqlState,
                                              nativeErrPtr,
                                              (SQLCHAR*) messageText,
                                              (SQLSMALLINT) (bufferLen*charSize),
                                              txtLenPtr,
                                              nativeEncoding);
                } else {
                    retcode = SQL_ERROR;
                    /* default diag rec ... */
                }; /* else */
            }; /* else */
        }; /* else */
        break;
    default:
        retcode = SQL_ERROR;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, PA10HANDLE_FN_SQLGETDIAGREC, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    if ((retcode != SQL_ERROR) && (retcode!=SQL_NO_DATA)) {
        if (sqlState != NULL) {
            API_TRACE( API_TR_SQLSTATE, "sqlState", sqlState);
        }
        if (nativeErrPtr != NULL) {
            API_TRACE( API_TR_SDWORD, "*nativErrPtr", nativeErrPtr );
        }
    }
    
    return retcode;
} /* SQLGetDiagRec */

/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/

#ifndef _UNICODE_ODBC
/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

tsp00_Bool pa10GetODBCVersion( SWORD      handleType,
                               SQLHANDLE  handle,
                               UDWORD    *odbcVersion )
{
    tsp00_Bool ok = false;
    tpa50Env *parentEnv = NULL;
    tpa40DBC *parentDBC = pa10GetParentDBC( handleType, handle );
    
    if (parentDBC!=NULL) {
        parentEnv = (tpa50Env*) pa40GetParentEnv( parentDBC );
        if (parentEnv!=NULL) {
            *odbcVersion = parentEnv->odbc_version;
            ok = true;
        }; /* if */
    }; /* if */

    return ok;
} /* pa10GetODBCVersion */


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

UCHAR *PA10_FN_SQLTRANSACT = { (UCHAR*) "pa10_Transact" };


tpa30DiagArea* pa10GetDiagArea( SWORD      handleType,
                                SQLHANDLE  handle )
{
    tpa30DiagArea *diagArea;
    tpa50Env *env;
    tpa40DBC *dbc;
    tpa60Stmt *stmt;
    tpa20Desc *desc;
    
    API_TRACE( API_TR_ENTRY, "pa10GetDiagArea", 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);

    switch (handleType) {
    case SQL_HANDLE_ENV :
        env = (tpa50Env*) handle;
        diagArea = &env->diagArea;
        break;
    case SQL_HANDLE_DBC :
        dbc = (tpa40DBC*) handle;
        diagArea = &dbc->diagArea;
        break;
    case SQL_HANDLE_STMT :
        stmt = (tpa60Stmt*) handle;
        diagArea = stmt->diagArea;
        break;
    case SQL_HANDLE_DESC :
        desc = (tpa20Desc*) handle;
        diagArea = &desc->diagArea;
        break;
    default:
        diagArea = NULL;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa10GetDiagArea", 0);
    API_TRACE( API_TR_PTR, "diagArea", &diagArea );

    return diagArea;
} /* pa10GetDiagArea */


tpa31DiagRec* pa10GetDiagRec( SWORD      handleType,
                              SQLHANDLE  handle,
                              SWORD      recNo )
{
    tpa31DiagRec *diagRec;
    tpa50Env *env;
    tpa40DBC *dbc;
    tpa60Stmt *stmt;
    tpa20Desc *desc;
    
    API_TRACE( API_TR_ENTRY, "pa10GetDiagRec", 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);

    switch (handleType) {
    case SQL_HANDLE_ENV :
        env = (tpa50Env*) handle;
        diagRec = pa30GetDiagRecEx( &env->diagArea, recNo );
        break;
    case SQL_HANDLE_DBC :
        dbc = (tpa40DBC*) handle;
        diagRec = pa30GetDiagRecEx( &dbc->diagArea, recNo );
        break;
    case SQL_HANDLE_STMT :
        stmt = (tpa60Stmt*) handle;
        diagRec = pa30GetDiagRecEx( stmt->diagArea, recNo );
        break;
    case SQL_HANDLE_DESC :
        desc = (tpa20Desc*) handle;
        diagRec = pa30GetDiagRecEx( &desc->diagArea, recNo );
        break;
    default:
        diagRec = NULL;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa10GetDiagRec", 0);
    API_TRACE( API_TR_PTR, "diagRec", &diagRec);

    return diagRec;
} /* pa10GetDiagRec */


SWORD pa10GetDynamicFuncCode( tpa60Stmt *stmtPtr )
{
    SWORD dynamicFuncCode;
    
    switch (stmtPtr->stmt_function_code) {
    case csp1_alter_table_fc:
        dynamicFuncCode = SQL_DIAG_ALTER_TABLE;
        break;
    case csp1_create_index_fc:
        dynamicFuncCode = SQL_DIAG_CREATE_INDEX;
        break;
    case csp1_create_table_fc:
        dynamicFuncCode = SQL_DIAG_CREATE_TABLE;
        break;
    case csp1_create_view_fc:
        dynamicFuncCode = SQL_DIAG_CREATE_VIEW;
        break;
    case csp1_create_domain_fc:
        dynamicFuncCode = SQL_DIAG_CREATE_DOMAIN;
        break;
    case csp1_drop_domain_fc:
        dynamicFuncCode = SQL_DIAG_DROP_DOMAIN;
        break;
    case csp1_drop_index_fc:
        dynamicFuncCode = SQL_DIAG_DROP_INDEX;
        break;
    case csp1_drop_table_fc:
        dynamicFuncCode = SQL_DIAG_DROP_TABLE;
        break;
    case csp1_drop_view_fc:
        dynamicFuncCode = SQL_DIAG_DROP_VIEW;
        break;
    case csp1_grant_fc:
        dynamicFuncCode = SQL_DIAG_GRANT;
        break;
    case csp1_insert_fc:
        dynamicFuncCode = SQL_DIAG_INSERT;
        break;
    case csp1_revoke_fc:
        dynamicFuncCode = SQL_DIAG_REVOKE;
        break;
    case csp1_create_schema_fc:
        dynamicFuncCode = SQL_DIAG_CREATE_SCHEMA;
        break;
    case csp1_nil_fc:
        dynamicFuncCode = SQL_DIAG_UNKNOWN_STATEMENT;
        break;
    default:
        dynamicFuncCode = stmtPtr->stmt_function_code
            + SQL_FUNCTION_CODE_OFFSET;
        break;        
    }; /* switch */

    return dynamicFuncCode;
} /* pa10GetDynamicFuncCode */


const char *pa10GetDynamicFunction( SWORD dynFuncCode )
{
    const char *dynFunc;

    switch (dynFuncCode) {
    case SQL_DIAG_ALTER_TABLE:
        dynFunc = "ALTER TABLE";
        break;
    case SQL_DIAG_CREATE_INDEX:
        dynFunc = "CREATE_INDEX";
        break;
    case SQL_DIAG_CREATE_TABLE:
        dynFunc = "CREATE TABLE";
        break;
    case SQL_DIAG_CREATE_VIEW:
        dynFunc = "CREATE VIEW";
        break;
    case SQL_DIAG_CREATE_DOMAIN:
        dynFunc = "CREATE DOMAIN";
        break;
    case SQL_DIAG_DROP_DOMAIN:
        dynFunc = "DROP DOMAIN";
        break;
    case SQL_DIAG_DROP_INDEX:
        dynFunc = "DROP INDEX";
        break;
    case SQL_DIAG_DROP_TABLE:
        dynFunc = "DROP TABLE";
        break;
    case SQL_DIAG_DROP_VIEW:
        dynFunc = "DROP VIEW";
        break;
    case SQL_DIAG_GRANT:
        dynFunc = "GRANT";
        break;
    case SQL_DIAG_INSERT:
        dynFunc = "INSERT";
        break;
    case SQL_DIAG_REVOKE:
        dynFunc = "REVOKE";
        break;
    case SQL_DIAG_CREATE_SCHEMA:
        dynFunc = "CREATE SCHEMA";
        break;
    default:
        dynFunc = "";
    }; /* switch */
    
    return dynFunc;
} /* pa10GetDynamicFunction */


tpa40DBC* pa10GetParentDBC( SWORD handleType, SQLHANDLE handle )
{
    tpa40DBC  *parentDBC;
    tpa20Desc *desc;
    tpa60Stmt *stmt;
    
    API_TRACE( API_TR_ENTRY, "pa10GetParentDBC", 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);

    switch (handleType) {
    case SQL_HANDLE_ENV:
        parentDBC = NULL;
        break;
    case SQL_HANDLE_DBC:
        parentDBC = (tpa40DBC*) handle;
        break;
    case SQL_HANDLE_STMT:
        stmt = (tpa60Stmt*) handle;
        parentDBC = (tpa40DBC*) pa60GetParentDBC( stmt );
        break;
    case SQL_HANDLE_DESC:
        desc = (tpa20Desc*) handle;
        parentDBC = (tpa40DBC*) pa20GetParentDBC( desc );
        break;
    default:
        parentDBC = NULL;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa10GetParentDBC", 0);
    API_TRACE( API_TR_PTR, "parentDBC", &parentDBC );

    return parentDBC;
} /* pa10GetParentDBC */


tpa50Env* pa10GetParentEnv( SWORD handleType, SQLHANDLE handle )
{
    tpa50Env       *parentEnv;
    tpa40DBC  *dbc;
    tpa20Desc      *desc;
    tpa60Stmt *stmt;
    
    API_TRACE( API_TR_ENTRY, "pa10GetParentEnv", 0);
    API_TRACE( API_TR_SWORD,"handleType", &handleType);
    API_TRACE( API_TR_HANDLE,"handle", &handle);

    switch (handleType) {
    case SQL_HANDLE_ENV:
        parentEnv = (tpa50Env*) handle;
        break;
    case SQL_HANDLE_DBC:
        dbc = (tpa40DBC*) handle;
        parentEnv = (tpa50Env*) pa40GetParentEnv( dbc );
        break;
    case SQL_HANDLE_STMT:
        stmt = (tpa60Stmt*) handle;
        parentEnv = (tpa50Env*) pa60GetParentEnv( stmt );
        break;
    case SQL_HANDLE_DESC:
        desc = (tpa20Desc*) handle;
        parentEnv = (tpa50Env*) pa20GetParentEnv( desc );
        break;
    default:
        parentEnv = NULL;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa10GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "parentEnv", &parentEnv );

    return parentEnv;
} /* pa10GetParentEnv */


API_RETCODE pa10VerifyHandle( SWORD handleType, SQLHANDLE handle )
{
    API_RETCODE    apiRetcode;
    tpa20Desc      *desc;
    
    API_TRACE( API_TR_ENTRY, "pa10VerifyHandle", 0);
    API_TRACE( API_TR_SWORD, "handleType", &handleType);
    API_TRACE( API_TR_HANDLE, "handle", &handle);

    switch (handleType) {
    case SQL_HANDLE_ENV:
        apiRetcode = pa50VerifyEnv( handle );
        break;
    case SQL_HANDLE_DBC:
        apiRetcode = pa40VerifyDBC( handle );
        break;
    case SQL_HANDLE_STMT:
        apiRetcode = pa60VerifyStmt( handle );
        break;
    case SQL_HANDLE_DESC:
        desc = (tpa20Desc*) handle;
        apiRetcode = pa20VerifyDesc( (SQLHDESC) desc );
        break;
    default:
        apiRetcode = API_NOT_OK;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa10VerifyHandle", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa10VerifyHandle */


SQLRETURN pa10_Transact( SQLHENV henv, SQLHDBC hdbc, UWORD fType)
{
    SQLRETURN   retcode=SQL_SUCCESS;
    API_RETCODE api_retcode;
    SQLHSTMT    hstmt=SQL_NULL_HSTMT;
    tpa60Stmt  *stmt_block_ptr;
    SQLHDBC     lhdbc;
    SQLHDBC     next_hdbc;
    tpa40DBC   *dbc_block_ptr;
    SQLHENV     lhenv;
    tpa50Env   *env_block_ptr;

    API_TRACE(API_TR_ENTRY,PA10_FN_SQLTRANSACT,0);
    API_TRACE(API_TR_HANDLE,"henv",&henv);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE(API_TR_UWORD,"fType",&fType);

    if (hdbc == SQL_NULL_HDBC ) {
        if (henv == SQL_NULL_HENV ) {
            retcode = SQL_INVALID_HANDLE;
            goto exit;
        }
        if (pa50VerifyEnv( henv ) != API_OK) {
            retcode = SQL_INVALID_HANDLE;
            goto exit;
        }
        lhenv = henv;
        env_block_ptr = (tpa50Env*) apdlock(lhenv);
        API_ASSERT_PTR(env_block_ptr);
        lhdbc = env_block_ptr->child_hdbc;
        apdunlk(lhenv);
        retcode = SQL_SUCCESS;
        while (lhdbc != 0 && retcode == SQL_SUCCESS) {	 
            retcode = pa10_Transact( lhenv, lhdbc, fType );
            if (retcode == SQL_SUCCESS) {
                dbc_block_ptr = (tpa40DBC*) apdlock(lhdbc);
                API_ASSERT_PTR(dbc_block_ptr);
                next_hdbc = dbc_block_ptr->next_hdbc;
                API_TRACE(API_TR_HANDLE,"next_hdbc",&next_hdbc);	    
                apdunlk(lhdbc);
                lhdbc = next_hdbc;
            }; /* if */
        }; /* while */
    }
    else {      
        if (pa40VerifyDBC( hdbc ) != API_OK) {
            retcode = SQL_INVALID_HANDLE;
            goto exit;
        }

        apmlocp(&hstmt, &stmt_block_ptr, &hdbc, &dbc_block_ptr, 
                &lhenv, &env_block_ptr);          
        if (dbc_block_ptr->autocommit == 1) {       /* PTS 1110649 */
          retcode = SQL_SUCCESS;
          goto exit;
        }

        api_retcode = apmstfc(lhenv, hdbc, SQL_NULL_HSTMT, 
                              SQL_API_SQLTRANSACT);
        PA09ENTERASYNCFUNCTION(dbc_block_ptr, NULL);	
        if (api_retcode != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        }
        if (retcode == SQL_SUCCESS) {
            /* apmrter(lhenv, hdbc, SQL_NULL_HSTMT); */
            if (hdbc!=SQL_NULL_HANDLE) {
                pa40ResetError( hdbc );
            } else if (lhenv!=SQL_NULL_HANDLE) {
                pa50ResetError( lhenv );
            }; /* else */
            switch (fType) {
            case ( SQL_COMMIT ): {
                apecomm (&dbc_block_ptr->esqblk, dbc_block_ptr);
                break;
            }
            case ( SQL_ROLLBACK ): {
                aperoll (&dbc_block_ptr->esqblk, dbc_block_ptr);
                break;
            }
            default: {
                retcode = SQL_ERROR;
                pa40PutError( hdbc, API_ODBC_S1012, NULL );
                break;
            }
            }
            if (retcode == SQL_SUCCESS) {
                if (PA_ENV_SQLCODE != 0) { /* ESQ commit-rollback ok? */
                    retcode = SQL_ERROR;
                    pa40PutError( hdbc,
                                  pa41apmercv( &dbc_block_ptr->esqblk ),
                                  &dbc_block_ptr->esqblk );
                }
                else {
                    /* mark as finished/landed   */
                    if (dbc_block_ptr->subtranscount != 0) {
                        dbc_block_ptr->subtranscount = 0;
                        dbc_block_ptr->set_autocommit_flag =
                            dbc_block_ptr->autocommit;
                    }
                    retcode = SQL_SUCCESS;
                }
            }
            PA09LEAVEASYNCFUNCTION();	
        }
    }
exit:

    API_TRACE(API_TR_EXIT,PA10_FN_SQLTRANSACT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return(retcode);

} /* pa10_Transact */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
#endif  /* of _UNICODE_ODBC */
