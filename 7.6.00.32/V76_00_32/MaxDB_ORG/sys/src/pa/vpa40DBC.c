/*!**********************************************************************

  module: vpa40DBC.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff (D027451 - Holger.Bischoff@SAP-AG.de)

  special area: ODBC-Driver

  description:  ODBC connection handles (datastructures,functions)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

\if EMIT_LICENCE


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


\endif
************************************************************************/

/*=======================================================================
 *  special handling for ia64
 *=====================================================================*/
#ifdef _WIN64
#define API_STRLEN_UINT4(s)  (tsp00_Uint4) strlen((const char*) s)
#else
#define API_STRLEN_UINT4(s)  API_STRLEN(s)
#endif

/* set for SAPBO migration */
#define OS_USER_CONNECT (0)

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa50Env.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"
#include "vpa80String.h"
#include "vpa30.h"
#include "vpa06.h"
#include "vpa09.h"
#include "vpr05IfCom_String.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "vpr10c.h"

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

/* constants for SQLGetInfo */
#define API_MAX_CONNECTIONS (sqlgamax)
#define API_MAX_ENVIRONMENTS ((UWORD) 0)
#define API_QUALIFIER_TERM "Qualifier"
#define API_OWNER_TERM "Owner"
#define API_SCHEMA_TERM "Schema"
#define API_PROCEDURE_TERM "stored procedure"
#define API_TABLE_TERM "Table"
#define API_MAX_COLUMNS_IN_GROUPBY      (128)
#define API_MAX_COLUMNS_IN_INDEX	(16)
#define API_MAX_COLUMNS_IN_ORDERBY	(128)
#define API_MAX_COLUMNS_IN_SELECT 	(1023)
#define API_MAX_COLUMNS_IN_TABLE	(1023)
#define API_MAX_INDEX_SIZE      	(1024)
#define API_MAX_ROW_SIZE_INCLUDES_LONG	"N"
#define API_MAX_TABLES_IN_SELECT	(64)
#define API_SPEC_CHARACTERS		"#@$"
#define API_XOPEN_CLI_YEAR     "1995"

API_RETCODE pa40_apmadbc ( SQLHENV henv, SQLHDBC *phdbc );

API_RETCODE pa40_apmfdbc (SQLHDBC hdbc);

UWORD pa40_apmgfdb( tpa41ESQBlock *esqblk_ptr );

VOID pa40_FreeServerNode( tpa40DBC *dbc_block_ptr );

VOID pa40_GuessPacketSize( tpa40DBC *dbc_block_ptr );

SQLRETURN pa40_GetConnectOption( tpa40DBC  *dbc_block_ptr,
                                 SDWORD     fOption,
                                 PTR        pvParam,
                                 SQLINTEGER buffer_len,
                                 SDWORD    *strLenPtr,
                                 const tsp77encoding *destEncoding);


SQLRETURN pa40GetFunctions2( tpa40DBC  *dbc_block_ptr,
                             UWORD      fFunction,
                             UWORD     *pfExists);

SQLRETURN pa40GetFunctions3( tpa40DBC  *dbc_block_ptr,
                             UWORD      fFunction,
                             UWORD     *pfExists);

SQLRETURN pa40_SetConnectOption( tpa40DBC  *dbc_block_ptr,
                                 SDWORD     fOption,
                                 UDWORD     vParam);

boolean pa40_MapOsUser(UCHAR *driverName, tpr05_String *useridStr, tpr05_String *passwdStr);

int pa40_LoadCodePageFromDB (SQLHDBC hdbc, SQLCHAR *cpName, tsp81_CodePage *cp);

void pa40_quoteStringToUCS2 (tpr05_String *string);

void p03sqlfree (sqlcatype *sqlca);

API_RETCODE pa08GetDSNOptions( tpa40DBC *dbc_ptr, tpr05_String *profileStr, const tsp77encoding *encodingType );
VOID pa08DefaultConnection(tpa40DBC FAR * dbc_ptr);

/*tsp00_Bool SqlIsSapRouterString (char *serverNode);*/ /* vpa03wc */

/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

#ifndef SAPDB_FAST
#ifndef _UNICODE_ODBC
UCHAR *PA40_FN_SQLBROWSECONNECT  =  { (UCHAR*) "SQLBrowseConnect" };
UCHAR *PA40_FN_SQLCONNECT        =  { (UCHAR*)"SQLConnect" };
UCHAR *PA40_FN_SQLDISCONNECT     =  { (UCHAR*) "SQLDisconnect" };
UCHAR *PA40_FN_SQLDRIVERCONNECT  =  { (UCHAR*) "SQLDriverConnect" };
UCHAR *PA40_FN_SQLGETCONNECTATTR =  { (UCHAR*) "SQLGetConnectAttr" };
UCHAR *PA40_FN_SQLGETFUNCTIONS   =  { (UCHAR*) "SQLGetFunctions" };
UCHAR *PA40_FN_SQLGETINFO        =  { (UCHAR*) "SQLGetInfo" };
UCHAR *PA40_FN_SQLNATIVESQL      =  { (UCHAR*) "SQLNativeSql" };
UCHAR *PA40_FN_SQLSETCONNECTATTR =  { (UCHAR*) "SQLSetConnectAttr" };
#else
extern UCHAR *PA40_FN_SQLBROWSECONNECT;
extern UCHAR *PA40_FN_SQLCONNECT;
extern UCHAR *PA40_FN_SQLDISCONNECT;
extern UCHAR *PA40_FN_SQLDRIVERCONNECT;
extern UCHAR *PA40_FN_SQLGETCONNECTATTR;
extern UCHAR *PA40_FN_SQLGETFUNCTIONS;
extern UCHAR *PA40_FN_SQLGETINFO;
extern UCHAR *PA40_FN_SQLNATIVESQL;
extern UCHAR *PA40_FN_SQLSETCONNECTATTR;
#endif
#endif  /* of SAPDB_FAST */

#define PA11MIN_GETFUNC		(1)
#define PA11MAX_GETFUNC		(100)

#ifdef WIN32
extern HANDLE s_hModule; /* from vpa30wc; used to get driver path */
#else
#ifndef _UNICODE_ODBC
#else
extern HANDLE s_hModule; /* from vpa30wc; used to get driver path */
#endif  /* _UNICODE_ODBC */
#endif


#ifndef _UNICODE_ODBC
/* ------------------------------------------------------------------------ */
/* Keywords */
/* INTERNAL RESERVED  */
UCHAR FAR *PA11KEYWORDSINTERNAL =  { (UCHAR*)\
"ABS,ACOS,ADDDATE,ADDTIME,ALPHA,ASCII,ASIN,ATAN,ATAN2,\
BINARY,BOOLEAN,BYTE,\
CEIL,CEILING,CHR,CONCAT,CONNECTED,COS,COSH,COT,CURDATE,CURTIME,\
DATABASE,DATEDIFF,DAYNAME,DAYOFMONTH,DAYOFWEEK,DAYOFYEAR,DBYTE,DECODE,\
DEFAULT,DEGREES,DIGITS,DIRECT,\
EBCDIC,ENTRY,ENTRYDEF,EXP,EXPAND,\
FIXED,FLOOR,\
GRAPHIC,GREATEST,\
HEX,\
IFNULL,INITCAP,INT,INTERNAL,\
LCASE,LEAST,LENGTH,LFILL,LINK,LIST,LN,LOCALSYSDBA,LOCATE,LOG,LOG10,LONG,\
LPAD,LTRIM,\
MAKEDATE,MAKETIME,MAPCHAR,MBCS,MICROSECOND,MOD,MONTHNAME,\
NOROUND,NOW,NUM,\
OBJECT,\
PACKED,PI,POWER,PREV,\
RADIANS,REAL,REFERENCED,REJECT,REPLACE,RFILL,ROUND,ROWID,ROWNO,RPAD,RTRIM,\
SELUPD,SHOW,SIGN,SIN,SINH,SOUNDEX,SPACE,SQRT,STAMP,STATISTICS,STDDEV,SUBDATE,\
SUBSTR,SUBTIME,SYSDBA,\
TAN,TANH,TIMEDIFF,TIMEZONE,TOIDENTIFIER,TRIM,TRUNC,TRUNCATE,\
UCASE,UID,UNICODE,USERGROUP,\
VARGRAPHIC,VARIANCE,\
WEEK,WEEKOFYEAR,\
ZONED"};
                  
/* ANSI   RESERVED */
UCHAR FAR *PA11KEYWORDSANSI =  { (UCHAR*)\
"ACTION,BOTH,CONSTRAINT,DEFAULT,INT,LEADING,NATURAL,NO,READ,REAL,REFERENCES,\
TRAILING,TRIM,WRITE"};
                     
/* DB2    RESERVED */
UCHAR FAR *PA11KEYWORDSDB2 =  { (UCHAR*)\
"AUDIT,BUFFERPOOL,CLUSTER,CONCAT,DATABASE,EDITPROC,OBID,OPTIMIZE,RELEASE,\
SYNONYM,TABLESPACE,VALIDPROC"};

/* ORACLE RESERVED */
UCHAR FAR *PA11KEYWORDSORACLE =  { (UCHAR*)\
"AUDIT,CLUSTER,COMMENT,DEFAULT,EXCLUSIVE,IDENTIFIED,LONG,MINUS,MODE,MODIFY,\
NOWAIT,NUMBER,PCTFREE,RAW,RENAME,RESOURCE,ROW,ROWID,ROWNUM,SHARE,SYNONYM,\
SYSDATE,TRIGGER,UID,VARCHAR2"};
/* ------------------------------------------------------------------------ */


DWORD ConvertFunctions = 
/* SQL_FN_CVT_CONVERT | */
0x00000000L;

DWORD NumericFunctions = 
SQL_FN_NUM_ABS     |
SQL_FN_NUM_ACOS    |                   
SQL_FN_NUM_ASIN    |                   
SQL_FN_NUM_ATAN    |                   
SQL_FN_NUM_ATAN2   |                   
SQL_FN_NUM_CEILING |                   
SQL_FN_NUM_COS     |                   
SQL_FN_NUM_COT     |                   
#if (ODBCVER >= 0x0200)
SQL_FN_NUM_DEGREES |
#endif
SQL_FN_NUM_EXP     |                   
SQL_FN_NUM_FLOOR   |                   
SQL_FN_NUM_LOG     |                   
#if (ODBCVER >= 0x0200)
SQL_FN_NUM_LOG     |                   
/* SQL_FN_NUM_LOG10   | */
#endif
SQL_FN_NUM_MOD     |                   
#if (ODBCVER >= 0x0200)
/* SQL_FN_NUM_POWER |  */
SQL_FN_NUM_RADIANS |
SQL_FN_NUM_ROUND   |
#endif
SQL_FN_NUM_PI      |                   
/*SQL_FN_NUM_RAND   |    */
SQL_FN_NUM_SIGN    |                   
SQL_FN_NUM_SIN     |                   
SQL_FN_NUM_SQRT    |                   
SQL_FN_NUM_TAN     |
#if (ODBCVER >= 0x0200)
SQL_FN_NUM_TRUNCATE |
#endif
0x00000000L;

DWORD StringFunctions =
/*SQL_FN_STR_ASCII      | */
/*SQL_FN_STR_CHAR       | */
SQL_FN_STR_CONCAT     |
#if (ODBCVER >= 0x0200)
/*SQL_FN_STR_DIFFERENCE | */
#endif
/*SQL_FN_STR_INSERT     | */
SQL_FN_STR_LCASE        |              
SQL_FN_STR_LEFT         |
SQL_FN_STR_LENGTH       |              
SQL_FN_STR_LOCATE     |
#if (ODBCVER >= 0x0200)
SQL_FN_STR_LOCATE_2   |
#endif
SQL_FN_STR_LTRIM        |                      
/*SQL_FN_STR_REPEAT     | */
SQL_FN_STR_REPLACE      |              
SQL_FN_STR_RIGHT        |              
SQL_FN_STR_RTRIM        |              
#if (ODBCVER >= 0x0200)
SQL_FN_STR_SOUNDEX      |              
/*SQL_FN_STR_SPACE      | */
#endif
SQL_FN_STR_SUBSTRING    |              
SQL_FN_STR_UCASE        |
0x00000000L;
 
DWORD TimeDateFunctions =
SQL_FN_TD_CURDATE       |              
SQL_FN_TD_CURTIME       |              
#if (ODBCVER >= 0x0200)
SQL_FN_TD_DAYNAME       |              
#endif 
SQL_FN_TD_DAYOFMONTH    |              
SQL_FN_TD_DAYOFWEEK     |              
SQL_FN_TD_DAYOFYEAR     |              
SQL_FN_TD_HOUR          |              
SQL_FN_TD_MINUTE        |              
SQL_FN_TD_MONTH         |              
#if (ODBCVER >= 0x0200)
SQL_FN_TD_MONTHNAME     |              
#endif 
SQL_FN_TD_NOW           |
/*SQL_FN_TD_QUARTER     |*/
SQL_FN_TD_SECOND        |
#if (ODBCVER >= 0x0200)
/*SQL_FN_TD_TIMESTAMPADD  | */
/*SQL_FN_TD_TIMESTAMPDIFF | */
#endif 
SQL_FN_TD_WEEK          |              
SQL_FN_TD_YEAR          |              
0x00000000L;

DWORD SystemFunctions =
SQL_FN_SYS_USERNAME     |          
SQL_FN_SYS_DBNAME       |   
SQL_FN_SYS_IFNULL       |
0x00000000L;
#else

extern UCHAR *PA11KEYWORDSINTERNAL;
extern UCHAR *PA11KEYWORDSANSI;
extern UCHAR *PA11KEYWORDSDB2;
extern UCHAR *PA11KEYWORDSORACLE;

extern DWORD ConvertFunctions;
extern DWORD NumericFunctions;
extern DWORD StringFunctions;
extern DWORD TimeDateFunctions;
extern DWORD SystemFunctions;
#endif  /* _UNICODE_ODBC */


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLConnectW, 
	  (SQLHDBC  hdbc, 
	   SQLWCHAR *szDSN, 
	   SWORD    cbDSN, 
	   SQLWCHAR *szUID, 
	   SWORD    cbUID, 
	   SQLWCHAR *szAuthStr, 
	   SWORD    cbAuthStr), 
	  (hdbc, szDSN, cbDSN, szUID, cbUID, szAuthStr, cbAuthStr))
#else
ODBC_PROC(SQLConnect, 
	  (SQLHDBC  hdbc, 
	   UCHAR   *szDSN, 
	   SWORD    cbDSN, 
	   UCHAR   *szUID, 
	   SWORD    cbUID, 
	   UCHAR   *szAuthStr, 
	   SWORD    cbAuthStr), 
	  (hdbc, szDSN, cbDSN, szUID, cbUID, szAuthStr, cbAuthStr))
#endif  /* _UNICODE_ODBC */
{
    SQLRETURN    retcode;
    API_RETCODE  api_retcode;
    UWORD        state;
    UWORD        dbno;
    tpa40DBC    *dbc_block_ptr = NULL;
    SQLHENV      henv;
    tpa50Env    *env_block_ptr;
  
    tpr05_String *UID, *Auth;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    int charSize = nativeEncoding->fixedCharacterSize;

    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA40_FN_SQLCONNECT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE_LEN(API_TR_ODBC_STRING,"szDSN",szDSN,cbDSN);
    API_TRACE(API_TR_SWORD,"cbDSN",&cbDSN);
    API_TRACE_LEN(API_TR_ODBC_STRING,"szUID",szUID,cbUID);
    API_TRACE(API_TR_SWORD,"cbUID",&cbUID);
    API_TRACE_LEN(API_TR_ODBC_STRING,"szAuthStr", "?", 1);
    API_TRACE(API_TR_MSG, "cbAuthStr=?", 0);


    if (cbDSN == SQL_NTS)
#ifdef _UNICODE_ODBC
        cbDSN = sp81UCS2strlen ((tsp81_UCS2Char*) szDSN);
    cbDSN *= sizeof (tsp81_UCS2Char);
#else
        cbDSN = (SQLSMALLINT) API_STRLEN (szDSN);
#endif        

    if (cbUID == SQL_NTS)
#ifdef _UNICODE_ODBC
        cbUID = sp81UCS2strlen ((tsp81_UCS2Char*) szUID);
    cbUID *= sizeof (tsp81_UCS2Char);
#else
        cbUID = (SQLSMALLINT) API_STRLEN (szUID);
#endif        
    UID = pr05IfCom_String_NewDynString (cbUID, nativeEncoding);
    API_MEMCPY (UID->rawString,  szUID, cbUID);
    UID->cbLen = cbUID;
    
    if (cbAuthStr == SQL_NTS)
#ifdef _UNICODE_ODBC
        cbAuthStr = sp81UCS2strlen ((tsp81_UCS2Char*) szAuthStr);
    cbAuthStr *= sizeof (tsp81_UCS2Char);
#else
        cbAuthStr = (SQLSMALLINT) API_STRLEN (szAuthStr);
#endif        
    Auth = pr05IfCom_String_NewDynString (cbAuthStr, nativeEncoding);
    API_MEMCPY (Auth->rawString, szAuthStr, cbAuthStr);
    Auth->cbLen = cbAuthStr;
  
    retcode = SQL_SUCCESS;
    if (apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT,
                 SQL_API_SQLCONNECT) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {     
        if (pa40VerifyDBC( hdbc ) != API_OK) { /* hdbc valid? */
            retcode = SQL_INVALID_HANDLE;
        }
        else {
            state = ((tpa40DBC*) hdbc)->state;
            api_retcode = API_OK;
            if ((cbDSN < 0 && cbDSN != SQL_NTS && cbDSN != SQL_NULL_DATA) ||
                (cbUID < 0 && cbUID != SQL_NTS && cbUID != SQL_NULL_DATA) ||
                (cbAuthStr < 0 && cbAuthStr != SQL_NTS &&
                 cbAuthStr != SQL_NULL_DATA) ) {
                retcode = SQL_ERROR;
                pa40PutError(hdbc, API_ODBC_S1090, NULL);/* invalid buffer
                                                          * or string length*/
            }
            else {
                switch (state) {
                case (API_DBC_CONNECTED): /* already connected? */
                    retcode = SQL_ERROR;
                    pa40PutError( hdbc, API_ODBC_08002, NULL);
                    break;
                case (API_DBC_ALLOCATED): { /* state =  allocate? */
                    dbc_block_ptr = (tpa40DBC FAR *) apdlock(hdbc);
                    dbc_block_ptr->userid = UID;
                    dbc_block_ptr->passw  = Auth;

                    API_ASSERT_PTR(dbc_block_ptr);
                    henv = dbc_block_ptr->parent_henv;
                    env_block_ptr = (tpa50Env FAR *) apdlock(henv);
                    API_ASSERT_PTR(env_block_ptr);           
                    PA09ENTERASYNCFUNCTION(dbc_block_ptr, NULL);

                    if (dbc_block_ptr->dsname == NULL)
                      dbc_block_ptr -> dsname  
                        = pr05IfCom_String_NewDynString (charSize * API_DATA_SOURCE_MAX_LEN+2, nativeEncoding);
                    if (api_retcode != API_OK) {
                      retcode = SQL_ERROR;
                      pa40PutError(hdbc, API_ODBC_S1090, NULL);
                      goto error_exit;
                    }
                    api_retcode = pa80ODBCtoTpr05 (dbc_block_ptr->dsname,
                                                   nativeEncoding,
                                                   szDSN,
                                                   cbDSN);
                    if (api_retcode != API_OK)	{   
                        retcode = SQL_ERROR;
                        pa40PutError(hdbc, API_ODBC_S1090, NULL);
                    }
                    else {
                        api_retcode = pa08GetDSNOptions (dbc_block_ptr, NULL, nativeEncoding);
                        if (api_retcode != API_OK) {
                            retcode = SQL_ERROR;
                            pa40PutError( hdbc, API_ODBC_IM002, NULL);
                        }		
                        else {
                            dbno = pa40_apmgfdb (&dbc_block_ptr->esqblk);
                            if ( dbno > 0 ) {
                                pa06SetTraceFileName(dbc_block_ptr);
                                pa08DefaultConnection(dbc_block_ptr);
                                dbc_block_ptr->dbno = dbno;

                                /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1134137 */
                                if (dbc_block_ptr->encrypt == SQL_ATTR_ENCRYPT_SSL  &&
                                    *dbc_block_ptr->servernode != 0) {
                                  tsp00_ErrTextc  errtxt;
                                  if (!SqlSSLAvailable (errtxt)) {
                                    retcode = SQL_ERROR;
                                    strncpy ((char*)dbc_block_ptr->esqblk.sql_err_text, errtxt,
                                             sizeof(dbc_block_ptr->esqblk.sql_err_text));
                                    dbc_block_ptr->esqblk.sql_err_text[sizeof(dbc_block_ptr->esqblk.sql_err_text)-1] = '\0';
                                    dbc_block_ptr->esqblk.esq_sql_code = 1;
                                    pa40PutError (dbc_block_ptr, API_ODBC_S1000, &dbc_block_ptr->esqblk);
                                    goto error_exit;
                                  }
                                }

                                api_retcode =
                                    apeconn(&dbc_block_ptr->esqblk,
                                            dbc_block_ptr->serverdb, SQL_NTS,
                                            UID, Auth,
                                            dbc_block_ptr );
                                if (api_retcode != API_OK) {
                                    retcode = SQL_ERROR;
                                    pa40PutError(hdbc, API_ODBC_S1090, NULL);
                                }		
                                else {
                                    /* check for bad argument */
                                    if (dbc_block_ptr->esqblk.esq_sql_code
                                        != PA_OK) { 
                                        retcode = SQL_ERROR;
                                        if (dbc_block_ptr->esqblk.esq_sql_code
                                            == -9808) {
                                            pa41apmmker( API_SQLODBC_22003,
                                                     &dbc_block_ptr->esqblk);
                                            pa40PutError(hdbc,
                                                         API_ODBC_08004,
                                                         &dbc_block_ptr
                                                         ->esqblk);
                                        }
                                        else
                                            pa40PutError( hdbc,
                                              pa41apmercv(&dbc_block_ptr
                                                          ->esqblk),
                                                          &dbc_block_ptr
                                                          ->esqblk );
                                    }
                                    else {
                                        pa40_GuessPacketSize( dbc_block_ptr );
                                        retcode = SQL_SUCCESS;
                                        dbc_block_ptr -> state =
                                            API_DBC_CONNECTED;
                                        dbc_block_ptr -> connectionDead =
                                            API_FALSE;
                                        dbc_block_ptr -> subtranscount = 0;
                                        dbc_block_ptr -> set_autocommit_flag
                                            = dbc_block_ptr -> autocommit;
                                    } /* else esqblk.esq_sql_code != 0 */
                                } /* connect ok */
                            } /* dbno > 0 */
                            else {
                                retcode = SQL_ERROR;
                                pa41apmmker( API_SQLODBC_22000,
                                             &dbc_block_ptr->esqblk);
                                pa40PutError(hdbc,
                                             API_ODBC_08001,
                                             &dbc_block_ptr->esqblk);
                            }		      
                        }/* else IM003 */
                    } /* bad argument*/
                    PA09LEAVEASYNCFUNCTION();
                    break;
                }
                default:  /* dbc state is not allocated and not connected! */
                    retcode = SQL_ERROR;
                    pa40PutError( hdbc, API_ODBC_S1000, NULL);
                    break;
                } /* of switch */
            } /* of if length of */
        } /* of handle valid */
    }

    /* if connected, try to load a codepage if requested */
    if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)  &&  
        dbc_block_ptr->cpName != NULL) {
      tsp81_CodePage *codepage, *oldCodepage;
      extern tsp81_CodePage pa04gCodePage;

      codepage = apdallo (sizeof (tsp81_CodePage));
      if (codepage != NULL) {
        if (pa40_LoadCodePageFromDB (hdbc, dbc_block_ptr->cpName, codepage) == TRUE) {
          oldCodepage = sp81SetCodePage (codepage);
          if (oldCodepage != NULL  &&  oldCodepage != &pa04gCodePage)
            apdfree (oldCodepage);
        }
        else {
          API_SPRINTF ((char*)dbc_block_ptr->esqblk.sql_err_text,
                       "Could not load codepage: %.40s",
                       dbc_block_ptr->cpName);
          pa40PutError (hdbc, API_ODBC_01S02, &dbc_block_ptr->esqblk);
          retcode = SQL_SUCCESS_WITH_INFO;
          apdfree (codepage);
        }
      }
    }

 error_exit:
    API_TRACE(API_TR_EXIT,PA40_FN_SQLCONNECT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* SQLConnect */

#ifdef _UNICODE_ODBC
ODBC_PROC(SQLBrowseConnectW,
          (SQLHDBC       hdbc,
           SQLWCHAR     *szConnStrIn,
           SQLSMALLINT   cbConnStrIn,
           SQLWCHAR     *szConnStrOut,
           SQLSMALLINT   cbConnStrOutMax,
           SQLSMALLINT  *pcbConnStrOut),
          ( hdbc, szConnStrIn, cbConnStrIn, szConnStrOut, cbConnStrOutMax, pcbConnStrOut))
#else
ODBC_PROC(SQLBrowseConnect,
          (SQLHDBC    hdbc,
           UCHAR     *szConnStrIn,
           SWORD      cbConnStrIn,
           UCHAR     *szConnStrOut,
           SWORD      cbConnStrOutMax,
           SWORD     *pcbConnStrOut),
          ( hdbc, szConnStrIn, cbConnStrIn, szConnStrOut, cbConnStrOutMax, pcbConnStrOut))
#endif  /* of _UNICODE_ODBC */
{
    RETCODE retcode = SQL_ERROR;
    API_RETCODE api_retcode;
    SWORD  connect_len;
    sqldbname serverdb, out_serverdb;
    UCHAR  *servernode     = NULL;
    UCHAR  *out_servernode = NULL;
    UCHAR  *driverName     = NULL;           /* PTS 1108230 */

    tpa40DBC *dbc_block_ptr;

    tpr05_String *DSN, *UID, *Auth, *dsname, *connectStr = NULL;
    tpr05_String *outUID, *outAuth;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    const int charSize = nativeEncoding->fixedCharacterSize;
    tsp00_Uint4 destBytesWritten;

    API_TRACE(API_TR_ENTRY, PA40_FN_SQLBROWSECONNECT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE_LEN(API_TR_ODBC_STRING,"szConnStrIn",
                  szConnStrIn, cbConnStrIn);
    API_TRACE(API_TR_SWORD,"cbConnStrIn",&cbConnStrIn);
    API_TRACE(API_TR_PTR,"szConnStrOut",&szConnStrOut);
    API_TRACE(API_TR_SWORD,"cbConnStrOutMax",&cbConnStrOutMax);
    API_TRACE(API_TR_PTR,"pcbConnStrOut",&pcbConnStrOut);

    DSN  = pr05IfCom_String_NewDynString (charSize * API_DATA_SOURCE_MAX_LEN, 
                                          nativeEncoding);
    UID  = pr05IfCom_String_NewDynString (charSize * API_USERID_MAX_LEN,
                                          nativeEncoding);
    Auth = pr05IfCom_String_NewDynString (charSize * API_PASSWD_MAX_LEN, 
                                          nativeEncoding);

    outUID  = pr05IfCom_String_NewDynString (charSize * API_USERID_MAX_LEN,
                                             nativeEncoding);
    outAuth = pr05IfCom_String_NewDynString (charSize * API_PASSWD_MAX_LEN, 
                                             nativeEncoding);
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127862 */
    if (cbConnStrIn == SQL_NTS)
#ifdef _UNICODE_ODBC
        cbConnStrIn = sp81UCS2strlen ((tsp81_UCS2Char*) szConnStrIn);
    cbConnStrIn *= charSize;
#else
        cbConnStrIn = (SQLSMALLINT) API_STRLEN (szConnStrIn);
#endif
  
    api_retcode = apmstfc(SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                          SQL_API_SQLBROWSECONNECT);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {
        /* ------------------------------------------------------------ */
        connect_len = 0;
        if (pcbConnStrOut != NULL) {
            *pcbConnStrOut = 0;
        }
        dbc_block_ptr = (tpa40DBC FAR *) apdlock(hdbc);
        API_ASSERT_PTR(dbc_block_ptr);   

        if (dbc_block_ptr->dsname == NULL)
          dbc_block_ptr -> dsname  
            = pr05IfCom_String_NewDynString (charSize * API_DATA_SOURCE_MAX_LEN+2, nativeEncoding);
        if (api_retcode != API_OK) {
            retcode = SQL_ERROR;
            pa40PutError(hdbc, API_ODBC_S1090, NULL);
            goto error_exit;
        }
        dsname = dbc_block_ptr->dsname;

        /* convert ODBC string to     */
        /* "normal" C string          */
        /*         connect_string = apdallo( API_CONNECT_STR_MAX_LEN ); */
        connectStr 
          = pr05IfCom_String_NewDynString (charSize * API_CONNECT_STR_MAX_LEN,
                                           nativeEncoding);

        servernode = apdallo( API_MAX_SERVERNODE_LEN );
        out_servernode = apdallo( API_MAX_SERVERNODE_LEN );
        if (connectStr == NULL
            || servernode == NULL
            || out_servernode == NULL) {
            /* memory allocation error */
            pa40PutError( hdbc, API_ODBC_S1001, NULL);            
            goto error_exit;
        }; /* if */
        /*         api_retcode = apmwtos (szConnStrIn, cbConnStrIn,
                   connect_string, API_CONNECT_STR_MAX_LEN ); */
        if (cbConnStrIn != SQL_NTS)
          cbConnStrIn *= charSize;

        /* later we need a null-terminated string since functions
           like UCS2strchr are called. Append charSize many '0'. */
        api_retcode = pa80ODBCtoTpr05 (connectStr,
                                       nativeEncoding,
                                       szConnStrIn,
                                       cbConnStrIn+charSize);
        API_MEMSET (connectStr->rawString+cbConnStrIn, 0, charSize);

        if (api_retcode != API_OK) {
            retcode = SQL_ERROR;
            pa40PutError( hdbc, API_ODBC_S1090, NULL);
            goto error_exit;
        }
        /* ------------------------------------------------------------ */
        /* parse the input string     */
        api_retcode = apucpar(connectStr,
                              /*                dsname, sizeof(dsname), */
                              dsname,
                              UID,
                              Auth,
                              serverdb, sizeof(serverdb),
                              servernode, min( API_MAX_SERVERNODE_LEN, SHRT_MAX ),
                              NULL, 0);           /* PTS 1108230 */
        if (api_retcode == API_OK && dsname->cbLen != 0)  
          /*             api_retcode = apdgdbn( (UCHAR*) dsname->rawString, (SWORD) dsname->cbLen,  */
            api_retcode = apdgdbn (dsname,
                                   serverdb, sizeof(serverdb),
                                   servernode,
                                   min( API_MAX_SERVERNODE_LEN, SHRT_MAX ));
        /* ------------------------------------------------------------ */
        retcode = SQL_SUCCESS;
        out_serverdb[0] = '\0'; out_servernode[0] = '\0';
  
        if (dbc_block_ptr->userid == NULL) {
          dbc_block_ptr->userid = pr05IfCom_String_NewDynString (charSize * API_USERID_MAX_LEN,
                                                                 nativeEncoding);
          pr05IfCom_String_ClearString (dbc_block_ptr->userid);
        }
        if (dbc_block_ptr->passw == NULL) {
          dbc_block_ptr->passw = pr05IfCom_String_NewDynString (charSize * API_PASSWD_MAX_LEN,
                                                                nativeEncoding);
          pr05IfCom_String_ClearString (dbc_block_ptr->passw);
        }

        if (api_retcode == API_OK) {
            /* -------------------------------------------------------- */
            /* Check DSN=? */
            if (dsname->cbLen != 0) {  
              /* API_STRCPY(dbc_block_ptr->dsname, dsname);            */
                pa40SetServerNode( dbc_block_ptr, servernode);            
                API_STRCPY(dbc_block_ptr->serverdb, serverdb);
                pr05IfCom_String_ClearString (dbc_block_ptr->userid);
                pr05IfCom_String_ClearString (dbc_block_ptr->passw);
            }
            /* else { API_STRCPY(dsname, dbc_block_ptr->dsname); } */
            /* -------------------------------------------------------- */
            /* Check UID=? */
            if (UID->cbLen == 0) {  
                if (dbc_block_ptr->userid->cbLen == 0) {
                    retcode = SQL_NEED_DATA;
                    pr05IfCom_String_ConvertP (outUID, 
                                               API_BROWSECONN_QUESTION,
                                               (tsp00_Uint4) API_STRLEN (API_BROWSECONN_QUESTION),
                                               sp77encodingAscii);
                } else {
                    pr05IfCom_String_ClearString (UID);
                    pr05IfCom_String_Convert (UID, dbc_block_ptr->userid);
                }
            } else {
                pr05IfCom_String_ClearString (dbc_block_ptr->userid);
                pr05IfCom_String_Convert (dbc_block_ptr->userid, UID);
            }
            /* -------------------------------------------------------- */
            /* Check PWD=? */
            if (Auth->cbLen == 0) {
                if (dbc_block_ptr->passw->cbLen == 0) {
                    /*            if (API_STRLEN((UCHAR FAR *) passwd) == 0)  {
                                  if (API_STRLEN((UCHAR FAR *) dbc_block_ptr->passw) == 0) { */
                    retcode = SQL_NEED_DATA;
                    pr05IfCom_String_ConvertP (outAuth, 
                                               API_BROWSECONN_QUESTION,
                                               (tsp00_Uint4) API_STRLEN (API_BROWSECONN_QUESTION),
                                               sp77encodingAscii);
                } else {
                    pr05IfCom_String_ClearString (Auth);
                    pr05IfCom_String_Convert (Auth, dbc_block_ptr->passw);
                }
            } else {
                pr05IfCom_String_ClearString (dbc_block_ptr->passw);
                pr05IfCom_String_Convert (dbc_block_ptr->passw, Auth);
            }
            /* -------------------------------------------------------- */
            /* Check ServerNode=? */
            if (API_STRLEN((UCHAR FAR *) servernode) == 0) 
                API_STRCPY(servernode, dbc_block_ptr->servernode); 

            if ((API_STRCHR(servernode, API_BROWSECONN_COMMA) != NULL) || 
                (API_STRLEN((UCHAR FAR *) servernode) == 0)) {  
                retcode = SQL_NEED_DATA;
                if (API_STRLEN((UCHAR FAR *) servernode) == 0)
                    API_SPRINTF( (char*) servernode,
                                 "%s", API_BROWSECONN_QUESTION);
                API_SPRINTF( (char*) out_servernode, "%s%s%s",
                             API_BROWSECONN_BRACE_OP,
                             servernode, API_BROWSECONN_BRACE_CL);
            } else { API_STRCPY(dbc_block_ptr->servernode, servernode); }	  
            /* -------------------------------------------------------- */
            /* Check ServerDB=? */
            if (retcode != SQL_NEED_DATA) {
                if (API_STRLEN((UCHAR FAR *) serverdb) == 0) 
                    API_STRCPY(serverdb, dbc_block_ptr->serverdb); 
	   
                if ((API_STRCHR(serverdb, API_BROWSECONN_COMMA) != NULL) ||
                    (API_STRLEN((UCHAR FAR *) serverdb) == 0)) {   
                    retcode = SQL_NEED_DATA;
                    if (API_STRLEN((UCHAR FAR *) serverdb) == 0)
                        API_SPRINTF((char*)serverdb, "%s",
                                    API_BROWSECONN_QUESTION);
                    API_SPRINTF( (char*) out_serverdb, "%s%s%s",
                                 API_BROWSECONN_BRACE_OP,
                                 serverdb, API_BROWSECONN_BRACE_CL);
                } else { API_STRCPY(dbc_block_ptr->serverdb, serverdb); }
            }
            /* -------------------------------------------------------- */
        } else {
            retcode = SQL_ERROR;   /* Data source not found and
                                    * no default */
            pa40PutError( hdbc, API_ODBC_IM002, NULL);
            goto error_exit;
        }
        /* ------------------------------------------------------------ */
        if (retcode == SQL_NEED_DATA) {
            api_retcode = 
              pa08MakeConnectString (connectStr,
                                     dbc_block_ptr->dsname,
                                     outUID,
                                     outAuth,
                                     out_serverdb,
                                     out_servernode,
                                     driverName,
                                     TRUE);        /* return BrowseConnect string */

            if ((api_retcode == API_OK) && (szConnStrOut != NULL)) {
                /* copy C string to ODBC      */
                /* string                     */
              api_retcode = 
                pa80convertString (nativeEncoding,
                                   szConnStrOut,
                                   cbConnStrOutMax,
                                   &destBytesWritten,
                                   connectStr->encodingType,
                                   connectStr->rawString,
                                   connectStr->cbLen);
              if (pcbConnStrOut != NULL)
                *pcbConnStrOut = (SQLSMALLINT) destBytesWritten;

                if (api_retcode != API_OK) {
                    retcode = SQL_SUCCESS_WITH_INFO;		/* truncation */
                    pa40PutError( hdbc, API_ODBC_01004, NULL);
                }
            } else {
                retcode = SQL_SUCCESS_WITH_INFO;
                pa40PutError( hdbc,
                              API_ODBC_S1000, NULL); /* general error
                                                      * really internal
                                                      * truncation */
            }
            goto error_exit;
        } else  {
            /* ------------------------------------------------------- */     
            /* if no error, connect       */
            pr05IfCom_String_ClearString (DSN);
            pr05IfCom_String_Convert (DSN, dbc_block_ptr->dsname);

            switch (api_retcode) {
            case (API_OK):
              /* both string are newly set inside SQLConnect
                 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128067 */
              pr05IfCom_String_DeleteString (dbc_block_ptr->userid);
              pr05IfCom_String_DeleteString (dbc_block_ptr->passw);
              dbc_block_ptr->userid = dbc_block_ptr->passw = NULL;

              if (dsname->encodingType == sp77encodingAscii)
                retcode = CALL_ODBC(SQLConnect, (hdbc,
                                                 (SQLCHAR*) dsname->rawString, 
                                                 (SQLSMALLINT) dsname->cbLen, 
                                                 (SQLCHAR*) UID->rawString, SQL_NTS,
                                                 (SQLCHAR*) Auth->rawString, SQL_NTS));
              else
                retcode = CALL_ODBC(SQLConnectW,(hdbc,
                                                 (SQLWCHAR*) dsname->rawString, 
                                                 (SQLSMALLINT) (dsname->cbLen / charSize), 
                                                 (SQLWCHAR*) UID->rawString, SQL_NTS,
                                                 (SQLWCHAR*) Auth->rawString, SQL_NTS));
                /* --------------------------------------------------- */     
                /* if all ok, make the new    */
                /* connect string as C string */
                if (retcode == SQL_SUCCESS) {
                    if (szConnStrOut != NULL) {
                      api_retcode = 
                        pa08MakeConnectString (connectStr,
                                               DSN,
                                               UID,
                                               Auth,
                                               serverdb,
                                               servernode,
                                               NULL,
                                               TRUE);        /* return BrowseConnect string */

                        if (api_retcode != API_OK) {
                            retcode = SQL_SUCCESS_WITH_INFO;
                            pa40PutError( hdbc, API_ODBC_S1000, NULL);
                            /* general error really internal truncation */
                        } else {
                            /* copy C string to ODBC      */
                            /* string                     */
                          api_retcode = 
                            pa80convertString (nativeEncoding,
                                               szConnStrOut,
                                               cbConnStrOutMax,
                                               &destBytesWritten,
                                               connectStr->encodingType,
                                               connectStr->rawString,
                                               connectStr->cbLen);
                          if (pcbConnStrOut != NULL)
                            *pcbConnStrOut = (SQLSMALLINT) destBytesWritten;

                          if (api_retcode != API_OK) {
                            retcode = SQL_SUCCESS_WITH_INFO;
                            pa40PutError( hdbc, API_ODBC_01004, NULL);
                                /* trucation */
                          }
                        }
                    } /* of if szConnStrOut != NULL */
                }/* of if retcode == SQL_SUCCESS */
                break;
                /* something failed in        */
                /* processing the options     */
            case (API_NOT_OK): {
                retcode = SQL_ERROR;
                pa40PutError( hdbc,
                              API_ODBC_IM008, NULL); /* dialog faild */
                break;
            }
            case (API_DATA_LOSS): {
                retcode = SQL_NO_DATA_FOUND;
                break;
            }
            default: {
                retcode = SQL_ERROR;
                pa40PutError( hdbc, API_ODBC_S1000, NULL); /* general error */
                break;
            }
            }
        }
    error_exit:
        if (servernode != NULL) {
            apdfree( servernode );
        }; /* if */
        if (out_servernode != NULL) {
            apdfree( out_servernode );
        }; /* if */
        /*        if (connect_string != NULL) {
                  apdfree( connect_string ); 
        };*/ /* if */

        pr05IfCom_String_DeleteString (DSN);
        pr05IfCom_String_DeleteString (UID);
        pr05IfCom_String_DeleteString (Auth);
        pr05IfCom_String_DeleteString (outUID);
        pr05IfCom_String_DeleteString (outAuth);
        pr05IfCom_String_DeleteString (connectStr);

        if (pcbConnStrOut != NULL) {
            API_TRACE(API_TR_SWORD,"*pcbConnStrOut",pcbConnStrOut);
        }  
        /* ----------------------------------------------------------- */
    }
    API_TRACE(API_TR_EXIT, PA40_FN_SQLBROWSECONNECT, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    API_TRACE_LEN( API_TR_ODBC_STRING,"szConnStrOut",
                   szConnStrOut, connect_len);
  
    return (retcode);
} /* SQLBrowseConnect */

#ifndef _UNICODE_ODBC

ODBC_PROC(SQLDisconnect,
          (SQLHDBC    hdbc),
          (hdbc))
{
    SQLRETURN retcode = SQL_SUCCESS;
    UWORD     state;
    SQLHSTMT  hstmt;
    tpa40DBC *dbc_block_ptr;
    SQLHENV   henv;
    tpa50Env *env_block_ptr;
  
    API_HANDLE hnull = SQL_NULL_HSTMT;
    VOID      *null_ptr = NULL;

    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA40_FN_SQLDISCONNECT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);

    if (pa40VerifyDBC( hdbc ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {
        if (apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT,
                     SQL_API_SQLDISCONNECT) != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        }
        else {
            state = ((tpa40DBC*) hdbc)->state;
            switch (state) {
            case (API_DBC_ALLOCATED): /* non-connected? */
                retcode = SQL_ERROR;
                /* connection not open */
                pa40PutError( hdbc, API_ODBC_08003, NULL);
                break;
            case (API_DBC_CONNECTED): { /* connected! */
                apmlocp( (SQLHSTMT*) &hnull, (tpa60Stmt**)&null_ptr,
                         &hdbc, &dbc_block_ptr, 
                         &henv, &env_block_ptr);
                /*	  PA09ENTERASYNCFUNCTION(dbc_block_ptr, NULL); */
                pa09SetAsyncLocals( dbc_block_ptr, SQL_NULL_HANDLE );
                if (!pa01UtilityConnect()) {
                  /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129839 */
                  if( dbc_block_ptr->esqblk.esq_sql_code != -807)
                    apedisc(&dbc_block_ptr->esqblk, dbc_block_ptr);
                }; /* if */
                /* ESQ disconnect ok? */
                if (dbc_block_ptr->esqblk.esq_sql_code != 0) {
                    /* if disconnect failes with one of the following codes
                     * we are not connected anymore.
                     * So return SQL_SUCCESS_WITH_INFO and not SQL_ERROR */
                    /* cpr_no_session_connected: */
                    if( dbc_block_ptr->esqblk.esq_sql_code == -807
                        /* cpr_reflex_timeout: */
                        || dbc_block_ptr->esqblk.esq_sql_code == 700
                        /* cpr_reflex_start_required: */
                        || dbc_block_ptr->esqblk.esq_sql_code == -813
                        /* cpr_reflex_crash */
                        || dbc_block_ptr->esqblk.esq_sql_code == 800
                        /* cpr_reflex_shutdown */
                        || dbc_block_ptr->esqblk.esq_sql_code == -708) {
                        retcode = SQL_SUCCESS_WITH_INFO;
                        /* disconnect error */
                        pa40PutError( hdbc,
                                      API_ODBC_01002,
                                      &dbc_block_ptr->esqblk );
                    } else {
                        retcode = SQL_ERROR;
                        pa40PutError( hdbc,
                                      pa41apmercv(&dbc_block_ptr->esqblk),
                                      &dbc_block_ptr->esqblk );
                    }; /* else */
                }; /* if */
                if (retcode != SQL_ERROR) {
                    dbc_block_ptr -> state = API_DBC_ALLOCATED;
                    dbc_block_ptr -> connectionDead = API_TRUE;
                    /* free any remaining stmts   */
                    hstmt = dbc_block_ptr -> child_hstmt;
                    dbc_block_ptr -> serverdb[0] = '\0';
                    dbc_block_ptr -> servernode[0] = '\0';
                    dbc_block_ptr -> kernelVersionString[0] = '\0';
                    while (hstmt != 0) {
                        tpa60Stmt *stmt_block_ptr;
                        RETCODE ret;
                        stmt_block_ptr = (tpa60Stmt FAR *)apdlock(hstmt);
                        API_ASSERT_PTR(stmt_block_ptr);
                        stmt_block_ptr -> state = API_STMT_ALLOCATED;
                        /* stmt_block_ptr -> hstmt_setpos = API_NULL_HANDLE;
                        */
                        ret = pa60DropStmt( hstmt );
                        if (ret != SQL_SUCCESS) {
                            retcode = SQL_SUCCESS_WITH_INFO;
                            pa40PutError( hdbc, API_ODBC_01002, NULL);
                            /* disconnect error */
                            break;
                        } /* of if ret is OK */
                        hstmt = dbc_block_ptr -> child_hstmt;
                    } /* of while */
                    /* free tpr05_strings,
                       http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127862
                       http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128067 */
                    pr05IfCom_String_DeleteString (dbc_block_ptr->userid);
                    pr05IfCom_String_DeleteString (dbc_block_ptr->passw);
                    dbc_block_ptr->userid = dbc_block_ptr->passw            = NULL;
                }; /* if */
                /*	     PA09LEAVEASYNCFUNCTION(); */
                break;
            }

            default:  /* dbc state is not allocated and not connected! */
                retcode = SQL_ERROR;
                pa40PutError(hdbc, API_ODBC_S1000, NULL);
                /* general ODBC error */
                break;

            } /* of switch */
        } /* if handle valid */
    } /* set odbc_function */
    API_TRACE(API_TR_EXIT,PA40_FN_SQLDISCONNECT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* SQLDisconnect */
#endif  /* of _UNICODE_ODBC */

#ifdef _UNICODE_ODBC
ODBC_PROC(SQLDriverConnectW,
	  (SQLHDBC     hdbc,
	   SQLHWND     hwnd,
	   SQLWCHAR    *szConnStrIn,
	   SWORD       cbConnStrIn,
	   SQLWCHAR    *szConnStrOut,
	   SWORD       cbConnStrOutMax,
	   SWORD      *pcbConnStrOut,
	   UWORD       fDriverCompletion),
          (hdbc, hwnd, szConnStrIn, cbConnStrIn, szConnStrOut, cbConnStrOutMax,
           pcbConnStrOut, fDriverCompletion))
#else
ODBC_PROC(SQLDriverConnect,
	  (SQLHDBC     hdbc,
	   SQLHWND     hwnd,
	   UCHAR      *szConnStrIn,
	   SWORD       cbConnStrIn,
	   UCHAR      *szConnStrOut,
	   SWORD       cbConnStrOutMax,
	   SWORD      *pcbConnStrOut,
	   UWORD       fDriverCompletion),
          (hdbc, hwnd, szConnStrIn, cbConnStrIn, szConnStrOut, cbConnStrOutMax,
           pcbConnStrOut, fDriverCompletion))
#endif  /* of _UNICODE_ODBC */
{
				  /* almost,. but not totally   */
				  /* done, needs defaults from  */
				  /* "ini" file                 */

				  /* this function does the SQL */
				  /* connect but has the added  */
				  /* ability to look at the     */
				  /* string passed to it and/or */
				  /* use a pop-window to get the*/
				  /* used id and password       */

    RETCODE    retcode = SQL_ERROR;
    RETCODE    api_retcode;

#ifdef _UNICODE_ODBC
    SQLWCHAR   *dsnameW = NULL, *userid, *passwd;
#else
    /*
    UCHAR      userid [API_USERID_MAX_LEN];
    UCHAR      passwd [API_PASSWD_MAX_LEN];
    */
    UCHAR     *userid,
              *passwd;
#endif
    
    sqldbname  dbFromDataSource;
    sqldbname  serverdb;

    tpr05_String *useridStr, *passwdStr, *dsnameStr, *swapStr;

    /* comes from the connect string and may be a long SAP router string */
    UCHAR     *servernode = NULL;
    UCHAR     *driverName = NULL;           /* PTS 1108230 */
    
    /* the node in the datasource in a real (short) servernode */
    UCHAR      nodeFromDataSource[1024];
    tpa40DBC *dbc_block_ptr;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    const int charSize = nativeEncoding->fixedCharacterSize;

    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA40_FN_SQLDRIVERCONNECT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE(API_TR_HANDLE,"hwnd",&hwnd);
    API_TRACE(API_TR_MSG, "szConnStrIn=?", 0);
    API_TRACE(API_TR_MSG, "cbConnStrIn=?", 0);
    API_TRACE(API_TR_PTR,"szConnStrOut",&szConnStrOut);
    API_TRACE(API_TR_SWORD,"cbConnStrOutMax",&cbConnStrOutMax);
    API_TRACE(API_TR_PTR,"pcbConnStrOut",&pcbConnStrOut);
    API_TRACE(API_TR_UWORD,"fDriverCompletion",&fDriverCompletion);

    useridStr = pr05IfCom_String_NewDynString (charSize * API_USERID_MAX_LEN, 
                                               nativeEncoding);
    passwdStr = pr05IfCom_String_NewDynString (charSize * API_PASSWD_MAX_LEN, 
                                               nativeEncoding);
    dsnameStr = pr05IfCom_String_NewDynString (charSize * API_DATA_SOURCE_MAX_LEN+2,
                                               nativeEncoding);

#ifdef _UNICODE_ODBC
    userid    = (SQLWCHAR*) useridStr->rawString;
    passwd    = (SQLWCHAR*) passwdStr->rawString;
#else
    userid    = (UCHAR*) useridStr->rawString;
    passwd    = (UCHAR*) passwdStr->rawString;
#endif

    {
    long strLen = cbConnStrIn;
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131075 */

    if (cbConnStrIn == SQL_NTS)
      
#ifdef _UNICODE_ODBC
        strLen = sp81UCS2strlen ((tsp81_UCS2Char*) szConnStrIn);
    strLen *= sizeof (tsp81_UCS2Char);
#else
        strLen = (SQLSMALLINT) API_STRLEN (szConnStrIn);
#endif        

    if (strLen > API_CONNECT_STR_MAX_LEN) {
      retcode = SQL_ERROR;
      pa40PutError(hdbc, API_ODBC_S1000, NULL); /* general error */
      goto error_exit;
    }
    cbConnStrIn = (SQLSMALLINT) strLen;
    }


    api_retcode = apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT,
                           SQL_API_SQLDRIVERCONNECT);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {
        dbc_block_ptr = (tpa40DBC*) hdbc;
        /* convert ODBC string to     */
        /* "normal" C string          */
        if (dbc_block_ptr->driverConnectStr == NULL) {
          dbc_block_ptr->driverConnectStr = pr05IfCom_String_NewDynString 
            (charSize * API_CONNECT_STR_MAX_LEN+2, nativeEncoding);
          /* initialize string with \0: => vpa08msc, pa08GetProfileString 
             API_STRCHR expexts zero terminated string */
          dbc_block_ptr->driverConnectStr->cbLen = dbc_block_ptr->driverConnectStr->cbMaxLen;
          pr05IfCom_String_ClearString (dbc_block_ptr->driverConnectStr);
        };
        if (dbc_block_ptr->driverConnectStr == NULL) {
            /* memory allocation error */
            pa40PutError(hdbc, API_ODBC_S1001, NULL);            
            goto error_exit;
        }; /* if */

        api_retcode = pa80ODBCtoTpr05 (dbc_block_ptr->driverConnectStr,
                                       nativeEncoding,
                                       szConnStrIn,
                                       cbConnStrIn);
        if (api_retcode != API_OK) {
            retcode = SQL_ERROR;
            pa40PutError(hdbc, API_ODBC_S1090, NULL);
            goto error_exit;
        }

        serverdb[0] = '\0';
        servernode = apdallo( API_MAX_SERVERNODE_LEN );
        driverName = apdallo( API_DATA_SOURCE_MAX_LEN );          /* PTS 1108230 */
        if (servernode == NULL
            || driverName == NULL) {
            /* memory allocation error */
            pa40PutError(hdbc, API_ODBC_S1001, NULL);            
            goto error_exit;
        }; /* if */

        servernode[0] = '\0';
         /* parse the input string     */
        api_retcode = apucpar(dbc_block_ptr->driverConnectStr,
                              dsnameStr,
                              useridStr,
                              passwdStr,
                              serverdb, sizeof(serverdb),
                              servernode, min( API_MAX_SERVERNODE_LEN, SHRT_MAX ),
                              driverName, API_DATA_SOURCE_MAX_LEN);         /* PTS 1108230 */
        
        API_TRACE(API_TR_STRING, "DSN", &dbc_block_ptr->dsname);
        API_TRACE(API_TR_STRING, "UID", &userid);
        API_TRACE(API_TR_STRING, "SERVERDB", &serverdb);
        API_TRACE(API_TR_STRING, "SERVERNODE", &servernode);

        dbFromDataSource[0] = '\0';
        nodeFromDataSource[0] = '\0';
        api_retcode = apdgdbn (dsnameStr, 
                              /*                    (SWORD) dsnameStr->cbLen, */
                              dbFromDataSource, sizeof(dbFromDataSource),
                              nodeFromDataSource, sizeof(nodeFromDataSource));

        /* We use the servernode & db from the datasource only if
         * there is no servernode & no db specified in the connect-string.
         * This makes only sense for the SAP router string. */
        if ((servernode[0] == '\0' || servernode[0] == '?')
            && (serverdb[0] == '\0' || serverdb[0] == '?')) {
            API_STRCPY( servernode, nodeFromDataSource );
            API_STRCPY( serverdb, dbFromDataSource );
        }; /* if */
        /* we don't want a '?' in the dialog box */
        if (servernode[0] == '?') {
            servernode[0] = '\0';
        };
        if (serverdb[0] == '?') {
            serverdb[0] = '\0';
        };
        API_TRACE(API_TR_STRING, "SERVERDB", &serverdb);
        API_TRACE(API_TR_STRING, "SERVERNODE", &servernode);
	
        if (OS_USER_CONNECT && driverName[0] != '\0') {
          boolean mapped = 
          pa40_MapOsUser(driverName, useridStr, passwdStr);
        }

        /* handle upper/lower case username/password */
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123115 */
        pa40_quoteStringToUCS2 (useridStr);
        pa40_quoteStringToUCS2 (passwdStr);

        /* process the option given   */
        switch (fDriverCompletion) {

        case (SQL_DRIVER_PROMPT):  /* must prompt user */
#ifdef WIN
          api_retcode = apduser(dsnameStr,
                                useridStr,
                                passwdStr,
                                (UCHAR*) serverdb, sizeof(serverdb),
                                (UCHAR*) servernode,
                                min( API_MAX_SERVERNODE_LEN, SHRT_MAX ),
                                hwnd, nativeEncoding,
                                &dbc_block_ptr->encrypt);
#else
            api_retcode = API_NOT_OK;
#endif
            break;

        case (SQL_DRIVER_COMPLETE):
            if ((API_STRLEN((UCHAR FAR *) userid) == 0)
                || (API_STRLEN((UCHAR FAR *) passwd) == 0)
                || (API_STRLEN((UCHAR FAR *) serverdb) == 0)
                )
                {
#ifdef WIN
                  api_retcode = apduser(dsnameStr,
                                        useridStr,
                                        passwdStr,
                                        (UCHAR*) serverdb, sizeof(serverdb), 
                                        (UCHAR*) servernode,
                                        min( API_MAX_SERVERNODE_LEN, SHRT_MAX ),
                                        hwnd, nativeEncoding,
                                        &dbc_block_ptr->encrypt);
#else
                    api_retcode = API_NOT_OK;
#endif
                }
            else {
                api_retcode = API_OK;
            }
            break;

        case (SQL_DRIVER_COMPLETE_REQUIRED):
#ifdef WIN
            if ((API_STRLEN((UCHAR FAR *) userid) == 0)
                || (API_STRLEN((UCHAR FAR *) passwd) == 0)
                || (API_STRLEN((UCHAR FAR *) serverdb) == 0)
#ifndef WIN32	    
                || (API_STRLEN((UCHAR FAR *) servernode) == 0)
#endif	    
                )
                {
                  api_retcode = apduser(dsnameStr,
                                        useridStr,
                                        passwdStr,
                                        (UCHAR*) serverdb, sizeof(serverdb),
                                        (UCHAR*) servernode,
                                        min( API_MAX_SERVERNODE_LEN, SHRT_MAX ),
                                        hwnd, nativeEncoding,
                                        &dbc_block_ptr->encrypt);
                }
            else {
                api_retcode = API_OK;
            }
#else
            api_retcode = API_NOT_OK;
#endif
            break;

        case (SQL_DRIVER_NOPROMPT):  /* no prompt permitted */
            api_retcode = API_OK;
            break;

        default:
            retcode = SQL_ERROR;
            pa40PutError(hdbc,
                         API_ODBC_S1110,       /* Invalid code  */
                         NULL);
            goto error_exit;
        }
        /* if no error, connect       */
        switch (api_retcode) {
        case (API_OK):
            API_ASSERT_PTR(dbc_block_ptr);   
            swapStr = dbc_block_ptr->dsname;    /* temporarily swap dsname */
            dbc_block_ptr->dsname = dsnameStr;
            api_retcode = pa08GetDSNOptions (dbc_block_ptr,
                                             dbc_block_ptr->driverConnectStr,
                                             dbc_block_ptr->driverConnectStr->encodingType);
            dbc_block_ptr->dsname = swapStr;
            API_STRCPY(dbc_block_ptr->serverdb, serverdb);
            pa40SetServerNode( dbc_block_ptr, servernode );

#ifdef _UNICODE_ODBC
            retcode = CALL_ODBC( SQLConnectW, (hdbc,
                                               (SQLWCHAR*) dsnameStr->rawString, 
                                               (SQLSMALLINT) (dsnameStr->cbLen / charSize),
                                               (SQLWCHAR*) useridStr->rawString, SQL_NTS,
                                               (SQLWCHAR*) passwdStr->rawString, SQL_NTS));
#else
            retcode = CALL_ODBC( SQLConnect, (hdbc,
                                              (SQLCHAR*) dsnameStr->rawString, 
                                              (SQLSMALLINT) dsnameStr->cbLen,
                                              userid, SQL_NTS,
                                              passwd, SQL_NTS));

#endif  /* of _UNICODE_ODBC */

            api_retcode = 
              pa08MakeConnectString (dbc_block_ptr->driverConnectStr,
                                     dbc_block_ptr->dsname,
                                     dbc_block_ptr->userid,
                                     dbc_block_ptr->passw,
                                     serverdb,
                                     servernode,
                                     driverName,
                                     FALSE);        /* return DriverConnect string */

            if (dbc_block_ptr->encrypt == SQL_ATTR_ENCRYPT_SSL)
              pr05IfCom_String_strcatP (dbc_block_ptr->driverConnectStr, "ENCRYPT=SSL;",
                                        sizeof ("ENCRYPT=SSL;"), sp77encodingAscii);

            if (api_retcode != API_OK) {
                retcode = SQL_SUCCESS_WITH_INFO;
                pa40PutError(hdbc, API_ODBC_S1000, NULL);
                /* general error */
                /* really internal truncation */
            }
            if ( retcode == SQL_SUCCESS
                 || retcode == SQL_SUCCESS_WITH_INFO ) {
                /* make the new connect string as C string */
                if (szConnStrOut != NULL) {
                    if (api_retcode == API_OK) { /* api_retcode from apucmak */
                      tsp00_Uint4 destBytesWritten;
                        /* copy C string to ODBC string */
                        api_retcode = 
                          pa80convertString (nativeEncoding,
                                             szConnStrOut,
                                             cbConnStrOutMax,
                                             &destBytesWritten,
                                             dbc_block_ptr->driverConnectStr->encodingType,
                                             dbc_block_ptr->driverConnectStr->rawString,
                                             dbc_block_ptr->driverConnectStr->cbLen);
                        if (pcbConnStrOut != NULL)
                          *pcbConnStrOut = (SQLSMALLINT) destBytesWritten;

                        if (api_retcode != API_OK) {
                            retcode = SQL_SUCCESS_WITH_INFO;
                            /* trucation */
                            pa40PutError( hdbc, API_ODBC_01004, NULL);
                        }
                    }
                } /* of if szConnStrOut != NULL */
            }; /* if */
            break;
            /* something failed in        */
            /* processing the options     */
        case (API_NOT_OK): {
            retcode = SQL_ERROR;
            pa40PutError( hdbc, API_ODBC_IM008, NULL); /* dialog faild */
            break;
        }
        case (API_DATA_LOSS): {
            retcode = SQL_NO_DATA_FOUND;
            break;
        }
        default: {
            retcode = SQL_ERROR;
            pa40PutError(hdbc, API_ODBC_S1000, NULL); /* general error */
            break;
        }
        }; /* switch */

error_exit:
        if (servernode != NULL) {
            apdfree( servernode );
        }
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128067 */
        if (driverName != NULL) {
            apdfree (driverName);
        }

        if (pcbConnStrOut != NULL) {
            API_TRACE(API_TR_SWORD,"*pcbConnStrOut",pcbConnStrOut);
        }; /* if */
    }; /* else */

    pr05IfCom_String_DeleteString (useridStr);
    pr05IfCom_String_DeleteString (passwdStr);
    pr05IfCom_String_DeleteString (dsnameStr);

    API_TRACE(API_TR_EXIT,PA40_FN_SQLDRIVERCONNECT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    API_TRACE_LEN(API_TR_ODBC_STRING, "szConnStrOut", "?", 1);

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* SQLDriverConnect */


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetConnectAttrW, (SQLHDBC            hdbc,
                               SQLINTEGER         attr,
                               SQLPOINTER         value_ptr,
                               SQLINTEGER         buffer_len,
                               SQLINTEGER        *str_len_ptr),
          (hdbc, attr, value_ptr, buffer_len, str_len_ptr))
#else
ODBC_PROC(SQLGetConnectAttr, (SQLHDBC    hdbc,
                              SQLINTEGER     attr,
                              SQLPOINTER     value_ptr,
                              SQLINTEGER     buffer_len,
                              SQLINTEGER    *str_len_ptr),
          (hdbc, attr, value_ptr, buffer_len, str_len_ptr))
#endif
{
    RETCODE retcode = SQL_SUCCESS;
    API_RETCODE api_retcode;
    tpa40DBC *dbc_ptr;
    UWORD sqlState = API_ODBC_00000; /* Success */
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii; 
#endif
    int charSize = nativeEncoding->fixedCharacterSize;
    
    API_TRACE( API_TR_ENTRY, PA40_FN_SQLGETCONNECTATTR, 0);
    API_TRACE( API_TR_HANDLE, "hdbc", &hdbc);
    API_TRACE( API_TR_CONNATTR, "attr", &attr);
    API_TRACE( API_TR_PTR, "value_ptr", &value_ptr);
    API_TRACE( API_TR_SDWORD, "buffer_len", &buffer_len);
    API_TRACE( API_TR_PTR, "str_len_ptr", &str_len_ptr);

    api_retcode = apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                           SQL_API_SQLGETCONNECTOPTION);
    
    if (api_retcode != API_OK || pa40VerifyDBC( hdbc ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        dbc_ptr = (tpa40DBC*) hdbc;

        switch (attr) {
        case SQL_ATTR_AUTO_IPD:
            *((UDWORD*) value_ptr) = (UDWORD) SQL_TRUE;
            break;
        case SQL_ATTR_ASYNC_ENABLE:
            /* async execution is only supported per statement */
            *((UDWORD*) value_ptr) = SQL_FALSE;
            break;
        case SQL_ATTR_CONNECTION_TIMEOUT:
            *((UDWORD*) value_ptr) = (UDWORD) 0;
            break;
        case SQL_ATTR_METADATA_ID:
            *((UDWORD*) value_ptr) = (UDWORD) SQL_TRUE;
            break;
        case SQL_ATTR_CONNECTION_DEAD: /* new in 3.5 */
            *((UDWORD*) value_ptr) =
                (UDWORD) (dbc_ptr->connectionDead);
            break;
        case (SQL_CURRENT_QUALIFIER): {			/* *** ODBC 2.0 */
            UDWORD    strLenInBytes;
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              value_ptr,
                                              buffer_len,
                                              NULL,           /* char count */
                                              &strLenInBytes, /* byte count */
                                              "",
                                              &sqlState );
            if (str_len_ptr != NULL)
              *str_len_ptr = (SQLINTEGER) strLenInBytes;

            if (sqlState != API_ODBC_00000) {
                retcode = SQL_ERROR;
                pa40PutError( hdbc, sqlState, NULL );
            }; /* if */
            break;
        }
        /* always an ASCII string (cmp SQLCancel, SqlACancel) */
        case (SQL_CANCEL_ROUTER_STRING): {
          retcode = pa80CopyStringD (dbc_ptr->cancelRouterStr, 
                                    buffer_len,
                                    value_ptr, 
                                    str_len_ptr, 
                                    &sqlState);
            break;
        }
        case SQL_ATTR_CODEPAGE: {
          tsp81_CodePage *codepage = sp81GetCodePage();

          if (codepage == NULL  ||  codepage->name == NULL) {
            /* write terminator in return string */
            API_MEMCPY (value_ptr, '\0', buffer_len < charSize ? buffer_len : charSize);
            if (str_len_ptr != NULL)
              *str_len_ptr = 0;
          }
          else {
            UDWORD    strLenInBytes;

            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              value_ptr, buffer_len,
                                              NULL, &strLenInBytes,
                                              codepage->name,
                                              &sqlState);

            if (str_len_ptr != NULL)
              *str_len_ptr = (SQLINTEGER) strLenInBytes;
          }
          break;
        }
        /* only the unicode driver will return this attribute
        http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123226 */
#ifdef _UNICODE_ODBC
        case SQL_ATTR_UNICODEDB: {
          if (dbc_ptr->unicode_catalog)
            *((SQLINTEGER*) value_ptr) = (SQLINTEGER) SQL_TRUE;
          else
            *((SQLINTEGER*) value_ptr) = (SQLINTEGER) SQL_FALSE;
          break;
        }
#endif
        case SQL_ATTR_ENCRYPT: {
            *((SQLINTEGER*) value_ptr) = (SQLINTEGER) dbc_ptr->encrypt;
          break;
        }
        default:
            /* get 1.0-2.5 attributes */
            retcode = pa40_GetConnectOption( dbc_ptr,
                                             attr,
                                             value_ptr,
                                             buffer_len,
                                             str_len_ptr,
                                             nativeEncoding);
        }; /* switch */
    }; /* else */
    API_TRACE(API_TR_EXIT, PA40_FN_SQLGETCONNECTATTR, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    return retcode;
} /* SQLGetConnectAttr */


#ifndef _UNICODE_ODBC 
ODBC_PROC(SQLGetFunctions,
          (SQLHDBC    hdbc,
           UWORD      fFunction,
           UWORD  FAR *pfExists),
          (hdbc, fFunction, pfExists))
{
    tpa50Env *parentEnv;
    tpa40DBC *dbc_block_ptr;
    API_RETCODE api_retcode;
    RETCODE retcode;
    
    api_retcode = apmstfc(SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                          SQL_API_SQLGETFUNCTIONS);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        dbc_block_ptr = (tpa40DBC*) apdlock(hdbc);
        API_ASSERT_PTR(dbc_block_ptr);   
        if (dbc_block_ptr -> state != API_DBC_CONNECTED) {
            retcode = SQL_ERROR;
            pa40PutError(hdbc,
                         API_ODBC_S1010, NULL); /* Function sequence error */
        } else {
          /* under windows the driver manager calls SQL_API_ODBC3_ALL_FUNCTIONS
             regardless of the ODBC version of the connection
             http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1141056 */
          switch (fFunction) {
          case SQL_API_ALL_FUNCTIONS:
                retcode = pa40GetFunctions2( dbc_block_ptr,
                                             fFunction,
                                             pfExists );
                break;
          case SQL_API_ODBC3_ALL_FUNCTIONS:
                retcode = pa40GetFunctions3( dbc_block_ptr,
                                             fFunction,
                                             pfExists );
                break;
          default:
            parentEnv = (tpa50Env*) pa40GetParentEnv( dbc_block_ptr );
            if (parentEnv->odbc_version == SQL_OV_ODBC3) {
                retcode = pa40GetFunctions3( dbc_block_ptr,
                                             fFunction,
                                             pfExists );
            } else {
                retcode = pa40GetFunctions2( dbc_block_ptr,
                                             fFunction,
                                             pfExists );
            }; /* else */
          }
        }; /* else */
    }; /* else */

    return retcode;
} /* SQLGetFunctions */        
#endif  /* _UNICODE_ODBC */
#define SQL_INFO_DRIVER_START				1000


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetInfoW,
	  (SQLHDBC       hdbc,
	   SQLUSMALLINT  fInfoType,
	   SQLPOINTER    rgbInfoValue,
	   SQLSMALLINT   cbInfoValueMax,
	   SQLSMALLINT  *pcbInfoValue),
	  (hdbc, fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue))
#else
ODBC_PROC(SQLGetInfo,
	  (SQLHDBC       hdbc,
	   SQLUSMALLINT  fInfoType,
	   SQLPOINTER    rgbInfoValue,
	   SQLSMALLINT   cbInfoValueMax,
	   SQLSMALLINT  *pcbInfoValue),
	  (hdbc, fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue))
#endif
{
    SQLRETURN   retcode;
    SQLRETURN   api_retcode;
    tpa40DBC   *dbc_block_ptr;
    SQLHENV     henv;
    tsp00_Uint4 length;

#ifdef _UNICODE_ODBC
    const tsp77encoding *encodingType = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *encodingType = sp77encodingAscii;
#endif   

    API_TRACE(API_TR_ENTRY,PA40_FN_SQLGETINFO,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE(API_TR_GETINFO,"fInfoType",&fInfoType);
    API_TRACE(API_TR_PTR,"rgbInfoValue",&rgbInfoValue);
    API_TRACE(API_TR_SWORD,"cbInfoValueMax",&cbInfoValueMax);
    API_TRACE(API_TR_PTR,"pcbInfoValue",&pcbInfoValue);

    api_retcode = apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT,
                           SQL_API_SQLGETINFO );
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {      
        if (cbInfoValueMax < 0) {
            pa40PutError(hdbc, API_ODBC_S1090, NULL); /* invalid string or
                                                       * buffer length */
            retcode = SQL_ERROR;
            goto error_exit;
        }; /* if */
        dbc_block_ptr = (tpa40DBC*) apdlock(hdbc);
        API_ASSERT_PTR(dbc_block_ptr);   
        PA09ENTERASYNCFUNCTION(dbc_block_ptr, NULL);
        if (dbc_block_ptr -> state == API_DBC_CONNECTED ||
            fInfoType == SQL_ODBC_VER || fInfoType == SQL_DRIVER_ODBC_VER) {

            /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1132620 */
            UCHAR *kernelVer = dbc_block_ptr->kernelVersionString;
            int kernel_73orlower = TRUE;
            int kernel_75orlower = TRUE;

            if (kernelVer[0] != '\0') {
              kernel_75orlower = kernelVer[0] < '7'  ||  (kernelVer[0] == '7' && kernelVer[2] < '6');
              kernel_73orlower = kernelVer[0] < '7'  ||  (kernelVer[0] == '7' && kernelVer[2] < '4');
            }

            api_retcode = API_OK;
            retcode = SQL_SUCCESS;
            switch (fInfoType) {
            case (SQL_ACTIVE_CONNECTIONS): {
                /* == SQL_MAX_DRIVER_CONNECTIONS ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* set to one */
                if (rgbInfoValue)
                    *((SWORD FAR *) rgbInfoValue) = (SWORD)API_MAX_CONNECTIONS;
                break;
            }
            case (SQL_ACTIVE_STATEMENTS): {
                /* == SQL_MAX_CONCURRENT_ACTIVITIES ODBC 3.0 */
                /* max number open statements */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* set to zero */
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) =
                        (SWORD) dbc_block_ptr -> activestatements;
                break;
            }
            case (SQL_DATA_SOURCE_NAME): {
                api_retcode = pa80ODBCtoEncoding (encodingType,
                                                  rgbInfoValue,
                                                  cbInfoValueMax,
                                                  &length,
                                                  dbc_block_ptr->dsname);
                if (pcbInfoValue != NULL)                 
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_DRIVER_PATH): {
#ifdef WIN32
                {
                    /* get the drivername with full path */
                    UCHAR szBuf[255];
                    GetModuleFileName( s_hModule, szBuf, sizeof(szBuf));

                    api_retcode = pa80convertString (encodingType,
                                                     rgbInfoValue,
                                                     cbInfoValueMax,
                                                     &length,
                                                     sp77encodingAscii,
                                                     szBuf,
                                                     (tsp00_Uint4) API_STRLEN (szBuf));
                    if (pcbInfoValue != NULL)
                      *pcbInfoValue = (SWORD) length;
                };
#else
                /* simply return a constant */
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_DRIVER_NAME,
                                                 API_STRLEN (API_DRIVER_NAME));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
#endif
                break;
            }
            case (SQL_DRIVER_NAME): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_DRIVER_NAME,
                                                 API_STRLEN_UINT4 (API_DRIVER_NAME));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_DRIVER_VER): {
                char buffer[64];

                sprintf (buffer, "%s.%s.%s b%s", MAJOR_VERSION_NO_STR_SP100,
                         MINOR_VERSION_NO_STR_SP100, CORRECTION_LEVEL_STR_SP100,
                         BUILD_PREFIX_STR_SP100);
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 buffer,
                                                 API_STRLEN_UINT4 (buffer));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_FETCH_DIRECTION): {
                /* deprecated in ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    API_MEMSET(rgbInfoValue,0,(int) sizeof(DWORD));
                    *((DWORD*) rgbInfoValue) = (SQL_FD_FETCH_NEXT
                                                | SQL_FD_FETCH_FIRST
                                                | SQL_FD_FETCH_LAST
                                                | SQL_FD_FETCH_PRIOR
                                                | SQL_FD_FETCH_ABSOLUTE
                                                | SQL_FD_FETCH_RELATIVE
                                                | SQL_FD_FETCH_BOOKMARK );
                }
                break;
            }
            case (SQL_ODBC_API_CONFORMANCE): {
                /* deprecated in ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* set to one */
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)SQL_OAC_LEVEL2;
                break;
            }
            case (SQL_SERVER_NAME): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 dbc_block_ptr->servernode,
                                                 API_STRLEN_UINT4 (dbc_block_ptr->servernode));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_SEARCH_PATTERN_ESCAPE): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "\\",
                                                 API_STRLEN_UINT4 ("\\"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_ODBC_SQL_CONFORMANCE): {
                /* deprecated in ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue) {
                    *((SWORD*) rgbInfoValue) = (SWORD) SQL_OSC_EXTENDED;
                }
                break;
            }
            case (SQL_DATABASE_NAME): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 dbc_block_ptr->serverdb,
                                                 API_STRLEN_UINT4 (dbc_block_ptr->serverdb));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_DBMS_NAME): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_DBMS_NAME,
                                                 API_STRLEN_UINT4 (API_DBMS_NAME));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_DBMS_VER): {
                UCHAR dbmsVer[40];
                if (API_STRLEN( dbc_block_ptr->kernelVersionString )>0) {
                    dbmsVer[0] = '0';
                    dbmsVer[1] = dbc_block_ptr->kernelVersionString[0];
                    dbmsVer[2] = '.';
                    API_MEMCPY( &dbmsVer[3],
                                &dbc_block_ptr->kernelVersionString[1],
                                2 );
                    dbmsVer[5] = '.';
                    dbmsVer[6] = '0';                        
                    API_MEMCPY( &dbmsVer[7],
                                &dbc_block_ptr->kernelVersionString[3],
                                2 );
                    dbmsVer[9] = '\0';
                } else { /* old kernel does not send kernelVersionString */
                    API_STRCPY( dbmsVer, API_DBMS_VER );
                }; /* else */
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 dbmsVer,
                                                 API_STRLEN_UINT4 (dbmsVer));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_PROCEDURE_TERM): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_PROCEDURE_TERM,
                                                 API_STRLEN_UINT4 (API_PROCEDURE_TERM));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_CONCAT_NULL_BEHAVIOR): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD) SQL_CB_NULL;
                break;
            }
            case (SQL_CURSOR_COMMIT_BEHAVIOR): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue) {
                    if (dbc_block_ptr->constmtopt.sqlmode == CPR_KIND_ANSI) {
                        *((SWORD*) rgbInfoValue) = (SWORD)SQL_CC_DELETE;
                    }
                    else {
                        *((SWORD*) rgbInfoValue) = (SWORD) SQL_CC_PRESERVE;
                    }
                }
                break;
            }
            case (SQL_CURSOR_ROLLBACK_BEHAVIOR): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue) {
                    if (dbc_block_ptr->constmtopt.sqlmode == CPR_KIND_ANSI) {
                        *((SWORD*) rgbInfoValue) = (SWORD)SQL_CC_DELETE;
                    }
                    else {
                        *((SWORD*) rgbInfoValue) = (SWORD)SQL_CC_CLOSE;
                    }
                }
                break;
            }
            case (SQL_IDENTIFIER_CASE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* names are not case sensitive */
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)SQL_IC_UPPER;
                break;
            }
            case (SQL_IDENTIFIER_QUOTE_CHAR): {
                /* set " " string */
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "\"",
                                                 API_STRLEN_UINT4 ("\""));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_USER_NAME): {
                api_retcode = pa80ODBCtoEncoding (encodingType,
                                                  rgbInfoValue,
                                                  cbInfoValueMax,
                                                  &length,
                                                  dbc_block_ptr->userid);
                if (pcbInfoValue != NULL) 
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_SCROLL_CONCURRENCY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                } 
                if (rgbInfoValue) {
                    API_MEMSET(rgbInfoValue,0,(int) sizeof(DWORD));
                    *((DWORD*) rgbInfoValue) = (DWORD) (SQL_SCCO_READ_ONLY
                                                        | SQL_SCCO_LOCK
                                                        | SQL_SCCO_OPT_ROWVER
                                                        | SQL_SCCO_OPT_VALUES);
                }
                break;
            }
            case (SQL_SCROLL_OPTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    API_MEMSET(rgbInfoValue,0,(int) sizeof(DWORD));
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) ( SQL_SO_FORWARD_ONLY
                                  | SQL_SO_STATIC
                                  | SQL_SO_KEYSET_DRIVEN
                                  | SQL_SO_DYNAMIC );
                }
                break;
            }
            case (SQL_MAX_COLUMN_NAME_LEN): {}
                /* == SQL_MAX_SCHEMA_NAME_LEN ODBC 3.0 */
            case (SQL_MAX_CURSOR_NAME_LEN): {}
            case (SQL_MAX_PROCEDURE_NAME_LEN): {}
            case (SQL_MAX_TABLE_NAME_LEN): {}
            case (SQL_MAX_OWNER_NAME_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)API_TABLE_NAME_MAX/2;
                break;
            }
            case (SQL_MAX_QUALIFIER_NAME_LEN): {
                /* == SQL_MAX_CATALOG_NAME_LEN ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD FAR *) rgbInfoValue) = (SWORD)0;
                break;
            }

            case (SQL_CONVERT_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD FAR *) rgbInfoValue) = ConvertFunctions;
                break;
            }
            case (SQL_NUMERIC_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD FAR *) rgbInfoValue) = NumericFunctions;
                break;
            }
            case (SQL_SYSTEM_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD FAR *) rgbInfoValue) = SystemFunctions;        
                break;
            }
            case (SQL_TIMEDATE_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD FAR *) rgbInfoValue) = TimeDateFunctions;
                break;
            }
            case (SQL_STRING_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }            
                if (rgbInfoValue)
                    *((DWORD FAR *) rgbInfoValue) = StringFunctions;
                break;
            }      
            case (SQL_CONVERT_BIGINT): {}
            case (SQL_CONVERT_BINARY): {}
            case (SQL_CONVERT_BIT): {}
            case (SQL_CONVERT_CHAR): {}
            case (SQL_CONVERT_DATE): {}
            case (SQL_CONVERT_DECIMAL): {}
            case (SQL_CONVERT_DOUBLE): {}
            case (SQL_CONVERT_FLOAT): {}
            case (SQL_CONVERT_INTEGER): {}
            case (SQL_CONVERT_LONGVARCHAR): {}
            case (SQL_CONVERT_NUMERIC): {}
            case (SQL_CONVERT_REAL): {}
            case (SQL_CONVERT_SMALLINT): {}
            case (SQL_CONVERT_TIME): {}
            case (SQL_CONVERT_TIMESTAMP): {}
            case (SQL_CONVERT_TINYINT): {}
            case (SQL_CONVERT_VARBINARY): {}
            case (SQL_CONVERT_VARCHAR): {}
            case (SQL_CONVERT_LONGVARBINARY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                /* set all bits to zero */
                if (rgbInfoValue)
                    API_MEMSET(rgbInfoValue,0,(int) sizeof(DWORD));
                break;
            }

            case (SQL_EXPRESSIONS_IN_ORDERBY): {}
            case (SQL_DATA_SOURCE_READ_ONLY): {}
            case (SQL_ACCESSIBLE_TABLES): {}
            case (SQL_ODBC_SQL_OPT_IEF):
                /* == SQL_INTEGRITY ODBC 3.0 */
            case (SQL_ACCESSIBLE_PROCEDURES): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "N",
                                                 API_STRLEN_UINT4 ("N"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_OUTER_JOINS): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "Y",
                                                 API_STRLEN_UINT4 ("Y"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_OWNER_TERM): {
                /* == SQL_SCHEMA_TERM ODBC 3.0 */
                char *term;

                if (kernel_75orlower)
                  term = API_OWNER_TERM;
                else
                  term = API_SCHEMA_TERM;

                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 term,
                                                 API_STRLEN_UINT4 (term));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_QUALIFIER_NAME_SEPARATOR): {
                /* == SQL_CATALOG_NAME_SEPARATOR ODBC 2.0 */
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 ".",
                                                 API_STRLEN_UINT4 ("."));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_QUALIFIER_TERM): {
                /* == SQL_CATALOG_TERM ODBC 2.0 */
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_QUALIFIER_TERM,
                                                 API_STRLEN_UINT4 (API_QUALIFIER_TERM));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_TABLE_TERM): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_TABLE_TERM,
                                                 API_STRLEN_UINT4 (API_TABLE_TERM));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_TXN_CAPABLE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* set to one */
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)SQL_TC_ALL;
                break;
            }
            case (SQL_ODBC_SAG_CLI_CONFORMANCE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)SQL_OSCC_COMPLIANT; 
                break;
            }
            case (SQL_MULT_RESULT_SETS): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "N",
                                                 API_STRLEN_UINT4 ("N"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_MULTIPLE_ACTIVE_TXN): {}
            case (SQL_PROCEDURES): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "Y",
                                                 API_STRLEN_UINT4 ("Y"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_ROW_UPDATES): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "Y",
                                                 API_STRLEN_UINT4 ("Y"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }	    	    
            case (SQL_DEFAULT_TXN_ISOLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) dbc_block_ptr->defisolevel;
                break;
            }
            case (SQL_TXN_ISOLATION_OPTION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue)
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) (SQL_TXN_READ_UNCOMMITTED
                                 | SQL_TXN_READ_COMMITTED
                                 | SQL_TXN_REPEATABLE_READ
                                 | SQL_TXN_SERIALIZABLE );
                break;
            }

            case (SQL_DRIVER_HENV): {
                henv = dbc_block_ptr->parent_henv;          
                if (rgbInfoValue)
                    API_MEMCPY(rgbInfoValue, &henv, sizeof(HENV));
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(HENV);
                }
                break;              
            }
            case (SQL_DRIVER_HDBC): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(HDBC);
                }
                break;              
            }	    
            case (SQL_DRIVER_HSTMT): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(HSTMT);
                }
                break;              
            }
	    
            case (SQL_CORRELATION_NAME): {	
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((UWORD*) rgbInfoValue) = (UWORD)SQL_CN_ANY; 
                break;
            }
            case (SQL_NON_NULLABLE_COLUMNS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)SQL_NNC_NON_NULL;
                break;
            }
	    
            /* ------- *** Options ODBC 2.0 *** ----------------------- */
	    
            case (SQL_ALTER_TABLE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) ( SQL_AT_ADD_COLUMN
                                  | SQL_AT_DROP_COLUMN
                                  | SQL_AT_ADD_COLUMN_DEFAULT
                                  /* ODBC 3.0 : */
                                  | SQL_AT_ADD_CONSTRAINT
                                  | SQL_AT_ADD_TABLE_CONSTRAINT
                                  | SQL_AT_DROP_COLUMN_CASCADE
                                  | SQL_AT_DROP_COLUMN_DEFAULT
                                  | SQL_AT_SET_COLUMN_DEFAULT );
                }
                break;
            }

            case (SQL_BOOKMARK_PERSISTENCE): {		
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = (DWORD)
                        (SQL_BP_SCROLL
                         |SQL_BP_UPDATE);
                }
                break;
            }
	    
            case (SQL_COLUMN_ALIAS): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 "Y",
                                                 API_STRLEN_UINT4 ("Y"));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }
	    
            case (SQL_ODBC_VER): {}
            case (SQL_DRIVER_ODBC_VER): {
                api_retcode = pa80convertString (encodingType,
                                                 rgbInfoValue,
                                                 cbInfoValueMax,
                                                 &length,
                                                 sp77encodingAscii,
                                                 API_DRIVER_ODBC_VER,
                                                 API_STRLEN_UINT4 (API_DRIVER_ODBC_VER));
                if (pcbInfoValue != NULL)
                  *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_FILE_USAGE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD FAR *) rgbInfoValue) = SQL_FILE_NOT_SUPPORTED;
                break;
            }

            case (SQL_GETDATA_EXTENSIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) = (DWORD)(SQL_GD_ANY_COLUMN
                                                       | SQL_GD_ANY_ORDER
                                                       | SQL_GD_BOUND
                                                       | SQL_GD_BLOCK );
                }
                break;
            }
	    
            case (SQL_GROUP_BY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                /* set to SQL_GB_GROUP_BY_CONTAINS_SELECT */
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) =
                        (SWORD) SQL_GB_GROUP_BY_CONTAINS_SELECT;
                break;
            }

            case (SQL_KEYWORDS): {
                api_retcode = API_OK;
                switch(dbc_block_ptr->constmtopt.sqlmode) {
                case CPR_KIND_EMPTY : {}
                case CPR_KIND_INTERNAL : {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   PA11KEYWORDSINTERNAL,
                                                   API_STRLEN_UINT4 (PA11KEYWORDSINTERNAL));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                    break;
                }
                case CPR_KIND_DB2 : {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   PA11KEYWORDSDB2,
                                                   API_STRLEN_UINT4 (PA11KEYWORDSDB2));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                    break;
                }
                case CPR_KIND_ANSI : {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   PA11KEYWORDSANSI,
                                                   API_STRLEN_UINT4 (PA11KEYWORDSANSI));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                    break;
                }
                case CPR_KIND_SAPR3 : {}
                case CPR_KIND_ORACLE : {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   PA11KEYWORDSORACLE,
                                                   API_STRLEN_UINT4 (PA11KEYWORDSORACLE));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                    break;
                }
                }
                if (api_retcode == API_OK) {
                    retcode = SQL_SUCCESS;
                }
                else if (api_retcode == API_TRUNCATE) {
                    retcode = SQL_SUCCESS_WITH_INFO;
                    pa40PutError( hdbc, API_ODBC_01004, NULL);
                }
                else {
                    retcode = SQL_ERROR;
                    pa40PutError( hdbc, API_ODBC_S1090, NULL);
                }
                break;
            }
	    
            case (SQL_LIKE_ESCAPE_CLAUSE): {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   "Y",
                                                   API_STRLEN_UINT4 ("Y"));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                break;
            }
	    
            case (SQL_LOCK_TYPES): {
                /* deprecated in ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    API_MEMSET( rgbInfoValue, 0, (int) sizeof(DWORD) );
                    *((DWORD*) rgbInfoValue) = (DWORD) (SQL_LCK_NO_CHANGE);
                }
                break;
            }

            case (SQL_MAX_BINARY_LITERAL_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    API_MEMSET(rgbInfoValue,0,(int) sizeof(DWORD));
                    *((DWORD*) rgbInfoValue) = (DWORD) 0;
                }
                break;
            }

            case (SQL_MAX_CHAR_LITERAL_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = (DWORD)0;
                }
                break;
            }
	    
            case (SQL_MAX_COLUMNS_IN_GROUP_BY): {	
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) =
                        (SWORD) API_MAX_COLUMNS_IN_GROUPBY;
                break;
            }
	    
            case (SQL_MAX_COLUMNS_IN_INDEX): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)API_MAX_COLUMNS_IN_INDEX;
                break;
            }
	    
            case (SQL_MAX_COLUMNS_IN_ORDER_BY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) =
                        (SWORD)API_MAX_COLUMNS_IN_ORDERBY;
                break;
            }
	    
            case (SQL_MAX_COLUMNS_IN_SELECT): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) =
                        (SWORD)API_MAX_COLUMNS_IN_SELECT;
                break;
            }
	    
            case (SQL_MAX_COLUMNS_IN_TABLE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)API_MAX_COLUMNS_IN_TABLE;
                break;
            }

            case (SQL_MAX_INDEX_SIZE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) = (DWORD)API_MAX_INDEX_SIZE;
                }
                break;
            }
	    
            case (SQL_MAX_ROW_SIZE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) = (DWORD)0;
                }
                break;
            }
	    
            case (SQL_MAX_ROW_SIZE_INCLUDES_LONG): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               API_MAX_ROW_SIZE_INCLUDES_LONG,
                                               API_STRLEN_UINT4 (API_MAX_ROW_SIZE_INCLUDES_LONG));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_MAX_STATEMENT_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = 0;
                }
                break;
            }
	    
            case (SQL_MAX_TABLES_IN_SELECT): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)API_MAX_TABLES_IN_SELECT;
                break;
            }
	    
            case (SQL_MAX_USER_NAME_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD)API_USERID_MAX_LEN/2;
                break;
            }

            case (SQL_NEED_LONG_DATA_LEN): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               "N",
                                               API_STRLEN_UINT4 ("N"));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_NULL_COLLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD FAR *) rgbInfoValue) = SQL_NC_LOW;
                break;
            }
	    
            case (SQL_ORDER_BY_COLUMNS_IN_SELECT): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               "N",
                                               API_STRLEN_UINT4 ("N"));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_OWNER_USAGE): {
                /* == SQL_SCHEMA_USAGE ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) ( SQL_OU_DML_STATEMENTS
                                  | SQL_OU_PROCEDURE_INVOCATION
                                  | SQL_OU_TABLE_DEFINITION
                                  | SQL_OU_INDEX_DEFINITION
                                  | SQL_OU_PRIVILEGE_DEFINITION );
                }
                break;
            }

            case (SQL_POS_OPERATIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) = (DWORD) (SQL_POS_POSITION
                                                        | SQL_POS_REFRESH
                                                        | SQL_POS_UPDATE
                                                        | SQL_POS_DELETE
                                                        | SQL_POS_ADD );
                }
                break;
            }

            case (SQL_POSITIONED_STATEMENTS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) (SQL_PS_POSITIONED_DELETE
                                 | SQL_PS_POSITIONED_UPDATE
                                 | SQL_PS_SELECT_FOR_UPDATE );
                }
                break;
            }

            case (SQL_QUALIFIER_LOCATION): {
                /* == SQL_CATALOG_LOCATION ODBC 2.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue) /* PTS 1118144 */
                    *((SWORD FAR *) rgbInfoValue) = SQL_QL_START;
                break;
            }

            case (SQL_QUALIFIER_USAGE): {
                /* == SQL_CATALOG_USAGE ODBC 3.0 */
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = (DWORD)0;
                }
                break;
            }
	    
            case (SQL_QUOTED_IDENTIFIER_CASE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD FAR *) rgbInfoValue) = (SWORD)SQL_IC_SENSITIVE;
                break;
            }

            case (SQL_SPECIAL_CHARACTERS): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               API_SPEC_CHARACTERS,
                                               API_STRLEN_UINT4 (API_SPEC_CHARACTERS));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }

            case (SQL_STATIC_SENSITIVITY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) = (DWORD) 0;
                }
                break;
            }
	    
            case (SQL_SUBQUERIES): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD*) rgbInfoValue) =
                        (DWORD) (SQL_SQ_CORRELATED_SUBQUERIES
                                 | SQL_SQ_COMPARISON
                                 | SQL_SQ_EXISTS
                                 | SQL_SQ_IN );
                }
                break;
            }
	    
            case (SQL_TIMEDATE_ADD_INTERVALS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = (DWORD)0;
                }
                break;
            }

            case (SQL_TIMEDATE_DIFF_INTERVALS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) = (DWORD)0;
                }
                break;
            }
	    
            case (SQL_UNION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(DWORD);
                }
                if (rgbInfoValue) {
                    *((DWORD FAR *) rgbInfoValue) =
                        (DWORD)(SQL_U_UNION | SQL_U_UNION_ALL);
                }
                break;
            }
#ifdef WIN
            case (SQL_DRIVER_HLIB): {
                pa40PutError( hdbc, API_ODBC_S1096, NULL);
                retcode = SQL_ERROR;
                break;
            }
#endif
            /* ------- *** Options ODBC 3.0 *** ----------------------- */
            case (SQL_ACTIVE_ENVIRONMENTS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UWORD);
                }
                if (rgbInfoValue)
                    *((UWORD*) rgbInfoValue) = (UWORD) API_MAX_ENVIRONMENTS;
                break;
            }
            case (SQL_AGGREGATE_FUNCTIONS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_AF_ALL;
                break;
            }
            case (SQL_ALTER_DOMAIN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_ASYNC_MODE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_AM_STATEMENT;
                break;
            }
            case (SQL_BATCH_ROW_COUNT): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0; /* ... */
                break;
            }
            case (SQL_BATCH_SUPPORT): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0; /* ... */
                break;
                break;
            }
            case (SQL_DROP_ASSERTION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_DROP_CHARACTER_SET): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_DROP_COLLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_DROP_DOMAIN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_DD_DROP_DOMAIN;
                break;
            }
            case (SQL_DROP_SCHEMA): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                  if (kernel_75orlower)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                  else
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_DS_DROP_SCHEMA |
                                                 SQL_DS_CASCADE |
                                                 SQL_DS_RESTRICT;
                break;
            }
            case (SQL_DROP_TABLE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_DT_DROP_TABLE
                                   | SQL_DT_CASCADE
                                   | SQL_DT_RESTRICT );
                break;
            }
            case (SQL_DROP_TRANSLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_DROP_VIEW): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) ( SQL_DV_DROP_VIEW
                                                           | SQL_DV_CASCADE
                                                           | SQL_DV_RESTRICT);
                break;
            }
            case (SQL_CATALOG_NAME): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               "N",
                                               API_STRLEN_UINT4 ("N"));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_COLLATION_SEQ): {
              api_retcode = pa80convertString (encodingType,
                                               rgbInfoValue,
                                               cbInfoValueMax,
                                               &length,
                                               sp77encodingAscii,
                                               "",
                                               API_STRLEN_UINT4 (""));
              if (pcbInfoValue != NULL)
                *pcbInfoValue = (SWORD) length;
                break;
            }
            case (SQL_CREATE_ASSERTION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_CREATE_CHARACTER_SET): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
                break;
            }
            case (SQL_CREATE_COLLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
                break;
            }
            case (SQL_CREATE_DOMAIN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CDO_CREATE_DOMAIN
                                   | SQL_CDO_CONSTRAINT_NAME_DEFINITION
                                   | SQL_CDO_CONSTRAINT
                                   | SQL_CDO_DEFAULT );
                break;
            }
            case (SQL_CREATE_SCHEMA): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }

                if (rgbInfoValue)
                  if (kernel_75orlower)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                  else
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_CS_CREATE_SCHEMA |
                                                 SQL_CS_AUTHORIZATION |
                                                 SQL_CS_DEFAULT_CHARACTER_SET;
                break;
            }
            case (SQL_CREATE_TABLE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CT_CREATE_TABLE
                                   | SQL_CT_TABLE_CONSTRAINT
                                   | SQL_CT_CONSTRAINT_NAME_DEFINITION
                                   | SQL_CT_LOCAL_TEMPORARY
                                   | SQL_CT_COLUMN_CONSTRAINT
                                   | SQL_CT_COLUMN_DEFAULT );
                break;
            }
            case (SQL_CREATE_TRANSLATION): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_CREATE_VIEW): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CV_CREATE_VIEW
                                   | SQL_CV_CHECK_OPTION );
                break;
            }
            case (SQL_CURSOR_SENSITIVITY): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_UNSPECIFIED );
                break;
            }
            case (SQL_DATETIME_LITERALS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) ( 0 );
                break;
            }
            case (SQL_DDL_INDEX): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_DI_CREATE_INDEX
                                   | SQL_DI_DROP_INDEX );
                break;
            }
            case (SQL_DESCRIBE_PARAMETER): {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   "Y",
                                                   API_STRLEN_UINT4 ("Y"));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                break;
            }
            /* SQL_DM_VER : returned by the Driver Manager */
            /* SQL_DRIVER_HDES : returned by the Driver Manager */
            case (SQL_DYNAMIC_CURSOR_ATTRIBUTES1): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA1_NEXT
                                   | SQL_CA1_ABSOLUTE
                                   | SQL_CA1_RELATIVE
                                   | SQL_CA1_BOOKMARK
                                   | SQL_CA1_LOCK_NO_CHANGE
                                   | SQL_CA1_POS_POSITION
                                   | SQL_CA1_POS_REFRESH
                                   | SQL_CA1_SELECT_FOR_UPDATE
                                   /* .. Bulk operations */ );
                break;
            }
            case (SQL_DYNAMIC_CURSOR_ATTRIBUTES2): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA2_LOCK_CONCURRENCY
                                   | SQL_CA2_OPT_ROWVER_CONCURRENCY
                                   | SQL_CA2_OPT_VALUES_CONCURRENCY
                                   /* ... sensitivity */
                                   /* ... MAX_ROWS */
                                   /* ... CRC */ );
                 break;
            }
            case (SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA1_NEXT
                                   /* .. Bulk operations */ );
                break;
            }
            case (SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA2_READ_ONLY_CONCURRENCY
                                   /* ... MAX_ROWS */ );
                break;
            }
            case (SQL_INDEX_KEYWORDS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_IK_ASC | SQL_IK_DESC );
                break;
            }
            case (SQL_INFO_SCHEMA_VIEWS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_INSERT_STATEMENT): {                
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_IS_INSERT_LITERALS
                                   | SQL_IS_INSERT_SEARCHED
                                   | SQL_IS_SELECT_INTO );
                break;
            }
            case (SQL_KEYSET_CURSOR_ATTRIBUTES1): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA1_NEXT
                                   | SQL_CA1_ABSOLUTE
                                   | SQL_CA1_RELATIVE
                                   | SQL_CA1_BOOKMARK
                                   | SQL_CA1_LOCK_NO_CHANGE
                                   | SQL_CA1_POS_POSITION
                                   | SQL_CA1_POS_UPDATE
                                   | SQL_CA1_POS_DELETE
                                   | SQL_CA1_POS_REFRESH
                                   | SQL_CA1_POSITIONED_UPDATE
                                   | SQL_CA1_POSITIONED_DELETE
                                   | SQL_CA1_SELECT_FOR_UPDATE
                                   | SQL_CA1_BULK_ADD
                                   /*
                                   | SQL_CA1_BULK_UPDATE_BY_BOOKMARK
                                   | SQL_CA1_BULK_DELETE_BY_BOOKMARK
                                   | SQL_CA1_BULK_FETCH_BY_BOOKMARK
                                   */
                                   /* .. Bulk operations */ );
                break;
            }
            case (SQL_KEYSET_CURSOR_ATTRIBUTES2): {
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                  if (kernel_73orlower)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA2_READ_ONLY_CONCURRENCY
                                   | SQL_CA2_LOCK_CONCURRENCY
                                   | SQL_CA2_OPT_ROWVER_CONCURRENCY
                                   | SQL_CA2_OPT_VALUES_CONCURRENCY
                                   | SQL_CA2_SENSITIVITY_ADDITIONS
                                   | SQL_CA2_SENSITIVITY_DELETIONS
                                   | SQL_CA2_SENSITIVITY_UPDATES
                                   /* ... sensitivity */
                                   /* ... MAX_ROWS */
                                   /* ... CRC */ );
                else
                    *((UDWORD*) rgbInfoValue) =
                      (UDWORD) ( SQL_CA2_READ_ONLY_CONCURRENCY
                                 | SQL_CA2_SENSITIVITY_ADDITIONS
                                 | SQL_CA2_SENSITIVITY_DELETIONS
                                 | SQL_CA2_SENSITIVITY_UPDATES );

                break;
            }            
            case (SQL_MAX_ASYNC_CONCURRENT_STATEMENTS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0;
                break;
            }
            case (SQL_MAX_IDENTIFIER_LEN): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((SWORD*) rgbInfoValue) = (SWORD) MX_IDENTIFIER;
                break;
            }
            case (SQL_ODBC_INTERFACE_CONFORMANCE): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(SWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) SQL_OIC_LEVEL2;
                break;
            };
            case (SQL_PARAM_ARRAY_ROW_COUNTS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = (UDWORD) 0; /* ... */
                break;
            }
            case (SQL_PARAM_ARRAY_SELECTS): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = SQL_PAS_NO_SELECT;
                break;
            }
            case (SQL_SQL_CONFORMANCE): {
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = SQL_SC_SQL92_ENTRY;
                break;
            }
            case (SQL_SQL92_DATETIME_FUNCTIONS):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0; /* ... */
                break;                
            case (SQL_SQL92_FOREIGN_KEY_DELETE_RULE):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_FOREIGN_KEY_UPDATE_RULE):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_GRANT):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_NUMERIC_VALUE_FUNCTIONS):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_PREDICATES):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;
            case (SQL_SQL92_RELATIONAL_JOIN_OPERATORS):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_REVOKE):
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;                
            case (SQL_SQL92_ROW_VALUE_CONSTRUCTOR):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;
            case (SQL_SQL92_STRING_FUNCTIONS):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;
            case (SQL_SQL92_VALUE_EXPRESSIONS):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;
            case (SQL_STANDARD_CLI_CONFORMANCE):
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) = 0;
                break;
            case (SQL_STATIC_CURSOR_ATTRIBUTES1): {
                if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA1_NEXT
                                   | SQL_CA1_ABSOLUTE
                                   | SQL_CA1_RELATIVE
                                   | SQL_CA1_BOOKMARK
                                   | SQL_CA1_LOCK_NO_CHANGE
                                   | SQL_CA1_POS_POSITION
                                   | SQL_CA1_POS_UPDATE
                                   | SQL_CA1_POS_DELETE
                                   | SQL_CA1_POS_REFRESH
                                   | SQL_CA1_POSITIONED_UPDATE
                                   | SQL_CA1_POSITIONED_DELETE
                                   | SQL_CA1_SELECT_FOR_UPDATE
                                   /* .. Bulk operations */ );
                break;
            }
            case (SQL_STATIC_CURSOR_ATTRIBUTES2): {
               if (pcbInfoValue != NULL) {
                    *pcbInfoValue = sizeof(UDWORD);
                }
                if (rgbInfoValue)
                    *((UDWORD*) rgbInfoValue) =
                        (UDWORD) ( SQL_CA2_LOCK_CONCURRENCY
                                   | SQL_CA2_OPT_ROWVER_CONCURRENCY
                                   | SQL_CA2_OPT_VALUES_CONCURRENCY
                                   /* ... sensitivity */
                                   /* ... MAX_ROWS */
                                   /* ... CRC */ );
                break;
            }
            case (SQL_XOPEN_CLI_YEAR): {
                  api_retcode = pa80convertString (encodingType,
                                                   rgbInfoValue,
                                                   cbInfoValueMax,
                                                   &length,
                                                   sp77encodingAscii,
                                                   API_XOPEN_CLI_YEAR,
                                                   API_STRLEN_UINT4 (API_XOPEN_CLI_YEAR));
                  if (pcbInfoValue != NULL)
                    *pcbInfoValue = (SWORD) length;
                break;
            }
            /* ---------------------------------------------------- */
            default: {
                if (fInfoType > SQL_INFO_DRIVER_START) {  
                    pa40PutError( hdbc, API_ODBC_S1C00, NULL);
                }
                else
                    {
                        pa40PutError( hdbc, API_ODBC_S1C00, NULL);
                    }
                retcode = SQL_ERROR;
                break;
            }
            } /* end switch */
            if (api_retcode == API_OK) {
                retcode = SQL_SUCCESS;
            }
            else if (api_retcode == API_TRUNCATE) {
                retcode = SQL_SUCCESS_WITH_INFO;
                pa40PutError( hdbc, API_ODBC_01004, NULL);
            }
            else {
                retcode = SQL_ERROR;
            }; /* if */
        }
        else {
            /* connection not open */
            pa40PutError( hdbc, API_ODBC_08003, NULL );
            retcode = SQL_ERROR;
        }  
        PA09LEAVEASYNCFUNCTION();
    }
error_exit:   
    API_TRACE(API_TR_EXIT,PA40_FN_SQLGETINFO,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return(retcode);
} /* SQLGetInfo */


#ifdef _UNICODE_ODBC
ODBC_PROC(SQLNativeSqlW,
	  (SQLHDBC      hdbc,
	   SQLWCHAR    *szSqlStrIn,
	   SQLINTEGER   cbSqlStrIn,
	   SQLWCHAR    *szSqlStr,
	   SQLINTEGER   cbSqlStrMax,
	   SQLINTEGER  *pcbSqlStr),
	  (hdbc, szSqlStrIn, cbSqlStrIn, szSqlStr, cbSqlStrMax, pcbSqlStr))
#else
ODBC_PROC(SQLNativeSql,
	  (SQLHDBC      hdbc,
	   SQLCHAR     *szSqlStrIn,
	   SQLINTEGER   cbSqlStrIn,
	   SQLCHAR     *szSqlStr,
	   SQLINTEGER   cbSqlStrMax,
	   SQLINTEGER  *pcbSqlStr),
	  (hdbc, szSqlStrIn, cbSqlStrIn, szSqlStr, cbSqlStrMax, pcbSqlStr))
#endif
{
    RETCODE      retcode;
    API_RETCODE  api_retcode;
#ifdef _UNICODE_ODBC
    const tsp77encoding *encodingType = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *encodingType = sp77encodingAscii;
#endif   
    int charSize = encodingType->fixedCharacterSize;

    API_TRACE(API_TR_ENTRY, PA40_FN_SQLNATIVESQL , 0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);
    API_TRACE_LEN(API_TR_ODBC_STRING,"szSqlStrIn",
                  szSqlStrIn, cbSqlStrIn);
    API_TRACE(API_TR_SWORD,"cbSqlStrIn",&cbSqlStrIn);
    API_TRACE(API_TR_PTR,"szSqlStr",&szSqlStr);
    API_TRACE(API_TR_SWORD,"cbSqlStrMax",&cbSqlStrMax);
    API_TRACE(API_TR_PTR,"pcbSqlStr",&pcbSqlStr);

    api_retcode = apmstfc( SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                           SQL_API_SQLNATIVESQL);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {
        retcode = SQL_SUCCESS;
        /* ***** Invalid argument (S1009) ? szSqlStrIn == NULL */
        if (szSqlStrIn == NULL) {
			retcode = SQL_ERROR;
			pa40PutError( hdbc, API_ODBC_S1009, NULL );
			goto error_exit;
        }

        /* ***** Invalid string or buffer length (S1090) ? */
        /* ***** cbSqlStrIn < 0 , != SQL_NTS */
        if ( (cbSqlStrIn < 0) && (cbSqlStrIn != SQL_NTS) ) {
			retcode = SQL_ERROR;
			pa40PutError( hdbc, API_ODBC_S1090, NULL );
			goto error_exit;
        }
        /* ***** cbSqlStrMax < 0 */
        if ((cbSqlStrMax <= 0) && (szSqlStr != NULL)){
			retcode = SQL_ERROR;
			pa40PutError( hdbc, API_ODBC_S1090, NULL);
			goto error_exit;
        }
        /* ***** Syntax error or access violation (37000) ? */
        if (szSqlStr != NULL) {
          tsp00_Uint4  destBytesWritten;

          cbSqlStrMax *= charSize;  /* PTS 1119048 */

          if (cbSqlStrIn == SQL_NTS) {
            tsp00_Uint4 charLen, byteLen;
            int isTerm, isCorr, isExh;

            encodingType->stringInfo (szSqlStrIn, 1UL << 31, TRUE,
                                      &charLen, &byteLen,
                                      &isTerm, &isCorr, &isExh);
            cbSqlStrIn = byteLen;
          }
          else
            cbSqlStrIn *= charSize;

          api_retcode = pa80convertString (encodingType,
                                           szSqlStr,
                                           cbSqlStrMax,
                                           &destBytesWritten,
                                           encodingType,
                                           szSqlStrIn,
                                           cbSqlStrIn);
          switch (api_retcode) {
          case API_TRUNCATE:
            pa40PutError( hdbc, API_ODBC_01004, NULL);
            retcode = SQL_SUCCESS_WITH_INFO;
            break;
          case API_OK:
            retcode = SQL_SUCCESS;
            break;
          default:
            retcode = SQL_ERROR;
            goto error_exit;
            break;
          }

          if (pcbSqlStr != NULL)
            *pcbSqlStr = destBytesWritten / charSize;   /* PTS 1119048 */

        }  /* if */
    }; /* else */
    
 error_exit:
    API_TRACE(API_TR_EXIT, PA40_FN_SQLNATIVESQL, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return (retcode);
} /* SQLNativeSql */

/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLSetConnectAttrW, (SQLHDBC    hdbc,
                               SQLINTEGER attr,
                               SQLPOINTER valuePtr,
                               SQLINTEGER strLen),
          (hdbc, attr, valuePtr, strLen))
#else
ODBC_PROC(SQLSetConnectAttr, (SQLHDBC    hdbc,
                              SQLINTEGER attr,
                              SQLPOINTER valuePtr,
                              SQLINTEGER strLen),
          (hdbc, attr, valuePtr, strLen))
#endif
{
    RETCODE retcode = SQL_SUCCESS;
    API_RETCODE api_retcode;
    tpa40DBC *dbc_block_ptr;
#ifdef _UNICODE_ODBC
    const tsp77encoding *encodingType = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *encodingType = sp77encodingAscii;
#endif
    int charSize = encodingType->fixedCharacterSize;
    
    API_TRACE( API_TR_ENTRY, PA40_FN_SQLSETCONNECTATTR, 0);
    API_TRACE( API_TR_HANDLE, "hdbc", &hdbc);
    API_TRACE( API_TR_CONNATTR, "attr", &attr);
    API_TRACE( API_TR_PTR, "value_ptr", &valuePtr);
    API_TRACE( API_TR_SDWORD, "str_len", &strLen);

    api_retcode = apmstfc(SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                          SQL_API_SQLSETCONNECTOPTION);
    if (api_retcode != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    }
    else {
        dbc_block_ptr = (tpa40DBC *) apdlock(hdbc);
        API_ASSERT_PTR(dbc_block_ptr);
        switch (attr) {
        case SQL_ATTR_ASYNC_ENABLE:
            /* async excution is only supported at statement level */
            /* optional feature not supported */
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_HYC00, NULL );
            retcode = SQL_ERROR;
            break;
            /*
              dbc_ptr->constmtopt.async_enable = *((UDWORD*) value_ptr);
            */
            /* no UCS2 handling for cancelRouterStr. This string is
               used in SQLCancel for NI-connections. API SqlACancel
               (heo03.h), which accepts ASCII-Strings only */
        case (SQL_CANCEL_ROUTER_STRING): {
            tsp00_Uint4 strLen = API_STRLEN_UINT4( (char*) SQLPOINTER_CAST (valuePtr) );
            
            if (dbc_block_ptr->cancelRouterStr != NULL) {
                /* free old value */
                apdfree( dbc_block_ptr->cancelRouterStr );
            };
            dbc_block_ptr->cancelRouterStr = apdallo( strLen+1 );
            if ( dbc_block_ptr->cancelRouterStr != NULL ) {
                strcpy( dbc_block_ptr->cancelRouterStr, (char*) SQLPOINTER_CAST (valuePtr) );
            } else {
                /* memory allocation failure */
                pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1001, NULL );
                retcode = SQL_ERROR;
            };
            break;
        }
        case (SQL_ATTR_SESSION_TIMEOUT): {
            if (dbc_block_ptr->state == API_DBC_CONNECTED) {
                /* already connected -> attribute cannot be set now */
                pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_HY011, NULL );
            } else {
                dbc_block_ptr->sessionTimeout = (UDWORD) VALUE_CAST (valuePtr);
            };
            break;
        }
        case SQL_ATTR_CODEPAGE: {
          SQLCHAR *codepageName = (SQLCHAR*) valuePtr;
          SQLCHAR buffer[256];
          SQLCHAR *oldCpName;
          tsp00_Uint4 length;

          if (codepageName == NULL || (strLen < 0 && strLen != SQL_NTS)) {
            pa40PutError (dbc_block_ptr, API_ODBC_S1092, NULL);   /* Invalid attribute value */
            retcode = SQL_ERROR;
          }

          /* if codepage is in SQLWCHAR, translate into SQLCHAR */
          if ((codepageName[0] == '\0' || codepageName[1] == '\0') && (strLen > 1 || strLen == SQL_NTS)) {
            tsp00_Uint4          destBytesWritten;

            if (strLen == SQL_NTS)
              length = charSize * sp81UCS2strlen ((tsp81_UCS2Char*) valuePtr);
            else
              length = strLen;
            /* buffer is \0-terminated */
            pa80convertString (sp77encodingAscii,
                               buffer, sizeof(buffer),
                               &destBytesWritten,
                               encodingType,
                               valuePtr, length);

            codepageName = buffer;
            strLen = SQL_NTS;
          }

          if (strLen == SQL_NTS)
            length = API_STRLEN_UINT4 (codepageName);
          else
            length = strLen;

          oldCpName = dbc_block_ptr->cpName;
          dbc_block_ptr->cpName = apdallo (length+1);
          if (dbc_block_ptr->cpName == NULL) {
            dbc_block_ptr->cpName = oldCpName;
            pa40PutError (dbc_block_ptr, API_ODBC_HY013, NULL);   /* Memory management error */
            retcode = SQL_ERROR;
          }
          else
            if (oldCpName != NULL)
              apdfree (oldCpName);

          API_STRNCPY (dbc_block_ptr->cpName, codepageName, length);
          break;
        }

        case SQL_ATTR_ENCRYPT: {
          dbc_block_ptr->encrypt = (char) valuePtr;
          break;
        }
        default:
            /* set 1.0-2.5 attributes */
            retcode = pa40_SetConnectOption( dbc_block_ptr,
                                             attr,
                                             (UWORD) VALUE_CAST (valuePtr) );
            break;
        }; /* switch */
    }; /* else */
    
    API_TRACE(API_TR_EXIT, PA40_FN_SQLSETCONNECTATTR, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    return retcode;
} /* SQLSetConnectAttr */

/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

#ifndef _UNICODE_ODBC 
UCHAR FAR *PA40_FN_ALLOCCONNECT     = { (UCHAR*) "pa40AllocConnect" };
UCHAR FAR *PA40_FN_FREECONNECT      = { (UCHAR*) "pa40FreeConnect" };


API_RETCODE pa40AddDesc( tpa40DBC *dbcPtr, tpa20Desc *applDescPtr )
{
    API_RETCODE apiRetcode = API_OK;
    tpa20Desc **descPtrPtr = NULL;
    
    API_TRACE( API_TR_ENTRY, "pa40AddDesc", 0);
    API_TRACE( API_TR_PTR, "dbcPtr", &dbcPtr);
    API_TRACE( API_TR_PTR, "applDescPtr", &applDescPtr);

    descPtrPtr = pr09AddItem( dbcPtr->applDescList );
    if (descPtrPtr != NULL) {
        *descPtrPtr = applDescPtr;
    } else {
        apiRetcode = API_NOT_OK;
    }; /* else */
    
    API_ASSERT_PTR( dbcPtr );
    API_ASSERT_PTR( applDescPtr );
    API_ASSERT( applDescPtr->allocType == SQL_DESC_ALLOC_USER );
    
    API_TRACE(API_TR_EXIT, "pa40AddDesc", 0);
    API_TRACE(API_TR_RETCODE,"apiRetcode",&apiRetcode);

    return apiRetcode;
} /* pa40AddDesc */

SQLRETURN pa40AllocConnect( SQLHENV henv, SQLHDBC *phdbc )
{
    SQLRETURN retcode;

    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA40_FN_ALLOCCONNECT,0);
    API_TRACE(API_TR_HANDLE,"henv",&henv);
    API_TRACE(API_TR_PTR,"phdbc",&phdbc);

    if (pa50VerifyEnv(henv) != API_OK) { /* henv valid? */
        retcode = SQL_INVALID_HANDLE;
        if (phdbc != NULL) { /* non-NULL pointer? */
            *phdbc = SQL_NULL_HDBC;
        }; /* if */
    } else {
        pa50ResetError( henv );
        if (phdbc == NULL) { /* NULL pointer? */
            pa50PutError( henv, API_ODBC_S1009 ); /* invaild argument value */
            retcode = SQL_ERROR;
        } else {
            if (pa40_apmadbc(henv, phdbc) != API_OK) { /* allocate ok? */
                *phdbc = SQL_NULL_HDBC;
                pa50PutError( henv, API_ODBC_S1001 ); /* Memory allocation
                                                       * failure */
                retcode = SQL_ERROR;
            } else {
                tpa40DBC *dbc_block_ptr =
                    (tpa40DBC*) apdlock (*phdbc);
                dbc_block_ptr->esqblk.odbc_function_code =
                    SQL_API_SQLALLOCCONNECT;
                retcode = SQL_SUCCESS;
            }; /* else */
        }; /* else */
    }; /* else */
    API_TRACE(API_TR_EXIT,PA40_FN_ALLOCCONNECT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);
    API_TRACE(API_TR_HANDLE,"*phdbc",phdbc);

    PA30UNPROTECTTHREAD();
    return(retcode);
} /* pa40AllocConnect */


SQLRETURN pa40FreeConnect( SQLHDBC    hdbc)
{
    RETCODE      retcode;
    API_RETCODE  api_retcode;
    UWORD        state;
    tpa40DBC    *dbc_block_ptr;
    
    PA30PROTECTTHREAD();
    API_TRACE(API_TR_ENTRY,PA40_FN_FREECONNECT,0);
    API_TRACE(API_TR_HANDLE,"hdbc",&hdbc);

    if (pa40VerifyDBC( hdbc ) != API_OK) { /* handle valid? */
        retcode = SQL_INVALID_HANDLE;
    } else {
        api_retcode = apmstfc(SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT, 
                              SQL_API_SQLFREECONNECT);
        if (api_retcode != API_OK) {
            retcode = SQL_INVALID_HANDLE;
        } else {
            dbc_block_ptr = (tpa40DBC*) hdbc;
            state = dbc_block_ptr->state;
            pa09SetAsyncLocals( dbc_block_ptr, SQL_NULL_HANDLE );
            if (state != API_DBC_ALLOCATED) { /* disconnected? */
                retcode = SQL_ERROR;
                pa40PutError( hdbc, API_ODBC_S1010, NULL);
                /* function sequence error */
            }
            else if (pa40_apmfdbc(hdbc) == API_OK) { /* dbc free ok? */
                retcode = SQL_SUCCESS;
            }
            else {
                retcode = SQL_ERROR;
                pa40PutError(hdbc, API_ODBC_S1000, NULL);
                /* general ODBC error */
            }
        } /* valid handle */
    } /* set function code */
    API_TRACE(API_TR_EXIT,PA40_FN_FREECONNECT,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* pa40FreeConnect */


SQLRETURN pa40GetFunctions3( tpa40DBC *dbc_block_ptr,
                             UWORD     function_id,
                             UWORD    *supported_ptr)
{
    SQLRETURN retcode = SQL_SUCCESS;
    UWORD sql_state = API_ODBC_00000; /* SUCCESS */
    API_RETCODE api_retcode = API_OK;
    UWORD id = 0;
    
    API_TRACE( API_TR_ENTRY, PA40_FN_SQLGETFUNCTIONS, 0);
    API_TRACE( API_TR_HANDLE, "dbc_block_ptr", &dbc_block_ptr);
    API_TRACE( API_TR_UWORD, "function_id", &function_id);
    API_TRACE( API_TR_PTR, "supported_ptr", &supported_ptr);

    switch (function_id) {
    case SQL_API_SQLALLOCCONNECT :
    case SQL_API_SQLALLOCENV :
    case SQL_API_SQLALLOCHANDLE :
    case SQL_API_SQLALLOCSTMT :
    case SQL_API_SQLBINDCOL :
    case SQL_API_SQLBINDPARAM :
    case SQL_API_SQLBULKOPERATIONS :        
    case SQL_API_SQLCANCEL :
    case SQL_API_SQLCLOSECURSOR :
    case SQL_API_SQLCOLATTRIBUTE :
    case SQL_API_SQLCOLUMNS :
    case SQL_API_SQLCONNECT :
    case SQL_API_SQLCOPYDESC :
    case SQL_API_SQLDATASOURCES :
    case SQL_API_SQLDESCRIBECOL :
    case SQL_API_SQLDISCONNECT :
    case SQL_API_SQLENDTRAN :
    case SQL_API_SQLERROR :
    case SQL_API_SQLEXECDIRECT :
    case SQL_API_SQLEXECUTE :
    case SQL_API_SQLFETCH :
    case SQL_API_SQLFETCHSCROLL :
    case SQL_API_SQLFREECONNECT :
    case SQL_API_SQLFREEENV :
    case SQL_API_SQLFREEHANDLE :
    case SQL_API_SQLFREESTMT :
    case SQL_API_SQLGETCONNECTATTR :
    case SQL_API_SQLGETCONNECTOPTION :
    case SQL_API_SQLGETCURSORNAME :
    case SQL_API_SQLGETDATA :
    case SQL_API_SQLGETDESCFIELD :
    case SQL_API_SQLGETDESCREC :
    case SQL_API_SQLGETDIAGFIELD :
    case SQL_API_SQLGETDIAGREC :
    case SQL_API_SQLGETENVATTR :
    case SQL_API_SQLGETFUNCTIONS :
    case SQL_API_SQLGETINFO :
    case SQL_API_SQLGETSTMTATTR :
    case SQL_API_SQLGETSTMTOPTION :
    case SQL_API_SQLGETTYPEINFO :
    case SQL_API_SQLNUMRESULTCOLS :
    case SQL_API_SQLPARAMDATA :
    case SQL_API_SQLPREPARE :
    case SQL_API_SQLPRIMARYKEYS :
    case SQL_API_SQLPUTDATA :
    case SQL_API_SQLROWCOUNT :
    case SQL_API_SQLSETCONNECTATTR :
    case SQL_API_SQLSETCONNECTOPTION :
    case SQL_API_SQLSETCURSORNAME :
    case SQL_API_SQLSETDESCFIELD :
    case SQL_API_SQLSETDESCREC :
    case SQL_API_SQLSETENVATTR :
    case SQL_API_SQLSETPARAM :
    case SQL_API_SQLSETSTMTATTR :
    case SQL_API_SQLSETSTMTOPTION :
    case SQL_API_SQLSPECIALCOLUMNS :
    case SQL_API_SQLSTATISTICS :
    case SQL_API_SQLTABLES :
    case SQL_API_SQLTRANSACT :
        if (supported_ptr!=NULL) {
            *supported_ptr = SQL_TRUE;
        } else {
            /* undocumented */
            sql_state = API_ODBC_HY009; /* Invalid use of null */
            /* pointer             */
            retcode = SQL_ERROR;
        }; /* else */
        break;
    case SQL_API_ODBC3_ALL_FUNCTIONS :
        if (supported_ptr!=NULL) {
            for ( id=0; id<SQL_API_ODBC3_ALL_FUNCTIONS_SIZE; id++ ) {
                supported_ptr[id] = 0xFFFF;
            }; /* for */
        } else {
            /* undocumented */
            sql_state = API_ODBC_HY009; /* Invalid use of null */
            /* pointer             */
            retcode = SQL_ERROR;
        }; /* else */
        break;
    default :
        retcode = SQL_ERROR;
        sql_state = API_ODBC_S1095; /* Function type out of range */
        break;
    }; /* switch */
            
    if (sql_state!=API_ODBC_00000) { /* != SUCCESS */
        pa40PutError( (SQLHDBC) dbc_block_ptr, sql_state, NULL);
    }; /* if */
    
    API_TRACE( API_TR_EXIT, PA40_FN_SQLGETFUNCTIONS, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);

    return(retcode);
} /* pa40GetFunctions3 */


SQLRETURN pa40GetFunctions2( tpa40DBC  *dbc_block_ptr,
                             UWORD      fFunction,
                             UWORD     *pfExists)
{
    SQLRETURN    retcode = SQL_SUCCESS;
    UWORD        function;
    UWORD        desc_pfexists[PA11MAX_GETFUNC];
    UWORD        desc_length;
    UWORD        count_func;
    UWORD        i;

    API_TRACE(API_TR_ENTRY,PA40_FN_SQLGETFUNCTIONS,0);
    API_TRACE(API_TR_HANDLE,"dbc_block_ptr",&dbc_block_ptr);
    API_TRACE(API_TR_UWORD,"fFunction",&fFunction);
    API_TRACE(API_TR_PTR,"pfExists",&pfExists);

    function = fFunction;

    if (fFunction == SQL_API_ALL_FUNCTIONS) 
        count_func = PA11MAX_GETFUNC;
    else 
        count_func = PA11MIN_GETFUNC;

    for  (i=0; i < count_func; i++) {		
        if (fFunction == SQL_API_ALL_FUNCTIONS) function=i;

        switch(function) {
            /* ********************** CORE *********************** */
        case (SQL_API_SQLALLOCCONNECT): {}
        case (SQL_API_SQLALLOCENV): {}
        case (SQL_API_SQLALLOCSTMT): {}
        case (SQL_API_SQLBINDCOL): {}
        case (SQL_API_SQLCANCEL): {}
        case (SQL_API_SQLCOLATTRIBUTES): {}
        case (SQL_API_SQLCONNECT): {}
        case (SQL_API_SQLDESCRIBECOL): {}
        case (SQL_API_SQLDISCONNECT): {}
        case (SQL_API_SQLERROR): {}
        case (SQL_API_SQLEXECDIRECT): {}
        case (SQL_API_SQLEXECUTE): {}
        case (SQL_API_SQLFETCH): {}
        case (SQL_API_SQLFREECONNECT): {}
        case (SQL_API_SQLFREEENV): {}
        case (SQL_API_SQLFREESTMT): {}
        case (SQL_API_SQLGETCURSORNAME): {}
        case (SQL_API_SQLNUMRESULTCOLS): {}
        case (SQL_API_SQLPREPARE): {}
        case (SQL_API_SQLROWCOUNT): {}
        case (SQL_API_SQLSETCURSORNAME): {}
        case (SQL_API_SQLSETPARAM): {}
        case (SQL_API_SQLTRANSACT): {
            desc_pfexists[function] = API_TRUE;
            break;
        }
        /* ********************** LEVEL1 *********************** */
        case (SQL_API_SQLCOLUMNS): {}
        case (SQL_API_SQLDRIVERCONNECT): {}
        case (SQL_API_SQLGETCONNECTOPTION): {}
        case (SQL_API_SQLGETDATA): {}
        case (SQL_API_SQLGETFUNCTIONS): {}
        case (SQL_API_SQLGETINFO): {}
        case (SQL_API_SQLGETSTMTOPTION): {}
        case (SQL_API_SQLGETTYPEINFO): {}
        case (SQL_API_SQLPARAMDATA): {}
        case (SQL_API_SQLPUTDATA): {}
        case (SQL_API_SQLSETCONNECTOPTION): {}
        case (SQL_API_SQLSETSTMTOPTION): {}
        case (SQL_API_SQLSPECIALCOLUMNS): {}
        case (SQL_API_SQLSTATISTICS): {}
        case (SQL_API_SQLTABLES): {}
        case (SQL_API_SQLBINDPARAMETER): {
            desc_pfexists[function] = API_TRUE;
            break;
        }
        /* ********************** LEVEL2 *********************** */
        case (SQL_API_SQLCOLUMNPRIVILEGES): {}
        case (SQL_API_SQLDATASOURCES): {}
        case (SQL_API_SQLDESCRIBEPARAM): {}
        case (SQL_API_SQLFOREIGNKEYS): {}
        case (SQL_API_SQLMORERESULTS): {}
        case (SQL_API_SQLNATIVESQL): {}
        case (SQL_API_SQLNUMPARAMS): {}
        case (SQL_API_SQLPRIMARYKEYS): {}
        case (SQL_API_SQLPROCEDURECOLUMNS): {}
        case (SQL_API_SQLPROCEDURES): {}
        case (SQL_API_SQLTABLEPRIVILEGES): {}
        case (SQL_API_SQLDRIVERS): {}
        case (SQL_API_SQLEXTENDEDFETCH): {}
        case (SQL_API_SQLSETSCROLLOPTIONS): {}
        case (SQL_API_SQLBROWSECONNECT): {}
        case (SQL_API_SQLSETPOS): {}
        case (SQL_API_SQLPARAMOPTIONS): {
            desc_pfexists[function] = API_TRUE;
            break;
        }
        /* *************************************************** */
        case (SQL_API_ALL_FUNCTIONS): {
            desc_pfexists[function] = API_TRUE;
            break;
        }
        default : {
            desc_pfexists[function] = API_FALSE;
            break;
        }
        }  /* switch */   

        if (fFunction != SQL_API_ALL_FUNCTIONS) break;

    } /* for */
      
    if (pfExists) {
        if (fFunction != SQL_API_ALL_FUNCTIONS) {
            *pfExists = desc_pfexists[fFunction];
        }
        else {
            desc_length = sizeof(desc_pfexists);
            API_MEMCPY(pfExists, desc_pfexists, desc_length);
        }
    }

    API_TRACE(API_TR_EXIT, PA40_FN_SQLGETFUNCTIONS, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return(retcode);
} /* pa40GetFunctions2 */

SQLHENV pa40GetParentEnv( tpa40DBC *dbc )
{
    SQLHENV parentEnv;
    
    API_TRACE( API_TR_ENTRY, "pa40GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "dbc", &dbc );

    parentEnv = dbc->parent_henv;

    API_TRACE( API_TR_EXIT, "pa40GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "parentEnv", &parentEnv);
    
    return parentEnv;
} /* pa40GetParentEnv */


VOID pa40GetStmtId( tpa40DBC *dbc, UDWORD *id )
{
    *id = ++dbc->stmtCount;
} /* pa40GetStmtId */


API_RETCODE pa40PutError( SQLHDBC        hdbc,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr)
{
    API_RETCODE apiRetcode = API_OK;
    tpa40DBC *dbcPtr;
    
    API_TRACE( API_TR_ENTRY, "pa40PutError", 0);

    dbcPtr    = (tpa40DBC*) hdbc;
    apiRetcode = pa30PutError( &dbcPtr->diagArea, sqlState, esqblkPtr );
    if (esqblkPtr != NULL) {
        SDWORD      nativeErr  = 0;
        
        nativeErr = esqblkPtr->esq_sql_code;
        if (nativeErr == -708 || nativeErr == -807 || nativeErr == -813 ) {
            /* connection is broken. Reset state to allocated. This is used
             * the connection attrbut SQL_ATTR_CONNECTION_DOWN */
            dbcPtr->state = API_DBC_ALLOCATED;
        }; /* if */
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa40PutError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa40PutError */


API_RETCODE pa40ResetError( SQLHDBC hdbc )
{
    API_RETCODE apiRetcode = API_OK;
    tpa40DBC *dbcPtr;
    
    API_TRACE( API_TR_ENTRY, "pa40ResetError", 0);

    dbcPtr = (tpa40DBC*) hdbc;
    apiRetcode = pa30FreeDiagArea( &dbcPtr->diagArea );
    dbcPtr->errorIndex = 0;
    
    API_TRACE( API_TR_EXIT, "pa40ResetError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

    return apiRetcode;
} /* pa40ResetError */


VOID pa40SetReconnectFlag( tpa40DBC FAR * api_dbc_ptr )
{
    tpa60Stmt *stmt_block_ptr;
    SQLHSTMT   hstmt;
    
    hstmt = api_dbc_ptr->child_hstmt;
    for(;hstmt != SQL_NULL_HSTMT;) {
        SQLHSTMT next_hstmt;
        
        stmt_block_ptr = (tpa60Stmt*) hstmt;
        stmt_block_ptr->reconnect_status = API_TRUE;
        next_hstmt = stmt_block_ptr->next_hstmt;
        hstmt=next_hstmt;
    }
    hstmt = api_dbc_ptr->child_hstmt;
    for(;hstmt != SQL_NULL_HSTMT;) {
        SQLHSTMT prev_hstmt;
        
        stmt_block_ptr = (tpa60Stmt*) hstmt;
        stmt_block_ptr->reconnect_status = API_TRUE;
        prev_hstmt = stmt_block_ptr->prev_hstmt;
        hstmt=prev_hstmt;
    }
} /* pa40SetReconnectFlag */

#define MIN_ALLOC_NODE_LEN 64

tsp00_Bool pa40SetServerNode( tpa40DBC *dbcPtr, UCHAR *servernode )
{
    UDWORD newLen = (SQLUINTEGER) API_STRLEN( servernode )+1;
    tsp00_Bool retcode;
    
    if (dbcPtr->allocServerNodeLen < newLen) {
        /* reallocat servernode */
        if (dbcPtr->servernode != NULL) {
            sqlfree( dbcPtr->servernode );
        }; /* if */
        newLen = max( newLen, MIN_ALLOC_NODE_LEN );
        dbcPtr->servernode = apdallo( newLen );
        if (dbcPtr->servernode != NULL) {
            dbcPtr->allocServerNodeLen = newLen;
            API_STRCPY( dbcPtr->servernode, servernode );
            retcode = true;
        } else {
            retcode = false;
        };
    } else {
        API_STRCPY( dbcPtr->servernode, servernode );
        retcode = true;
    };

    return retcode;
} /* pa40SetServerNode */


BOOLEAN pa40UseOldAutocommit( tpa40DBC *dbcPtr )
{
    /* the kernel version string is set only version 7.2 since 2.2.99
     * for earlier kernel we use the old autocommit behaviour (explicit
     * sending COMMIT command). For newer kernel we set the
     * commit_immediately in the segment header. This does not work for
     * old kernels because inserting LONG values does not work. */
    return (BOOLEAN)
           (API_STRLEN( dbcPtr->kernelVersionString ) == 0
            /* if not 0 then the length must be 5 */
            || dbcPtr->kernelVersionString[0] < '7'
            || (dbcPtr->kernelVersionString[2] < '2'
                && dbcPtr->kernelVersionString[0] == '7'));
} /* pa40UseOldAutocommit */


API_RETCODE pa40VerifyDBC( SQLHDBC hdbc )
{
  tpa40DBC *dbcPtr;
  API_RETCODE apiRetcode = API_OK;

  API_TRACE( API_TR_ENTRY, "pa40VerifyDBC", 0);
  API_TRACE( API_TR_HANDLE, "hdbc", &hdbc);

  dbcPtr = (tpa40DBC*) hdbc;
  if (dbcPtr == NULL) {
      apiRetcode = API_NOT_OK;
  } else {
      if (dbcPtr->type != API_DBC_TYPE) {
          apiRetcode = API_NOT_OK;
      }; /* if */
  }; /* else */

  API_TRACE( API_TR_EXIT, "pa40VerifyDBC", 0);
  API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

  return (apiRetcode);
} /* pa40VerifyDBC */


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

UCHAR *PA40_FN_GETCONNECTOPTION = { (UCHAR*) "pa40_GetConnectOption" };
UCHAR *PA40_FN_SETCONNECTOPTION = { (UCHAR*) "pa40_SetConnectOption" };

/* apmadbc() - API miscellaneous function, allocate connection */

API_RETCODE pa40_apmadbc ( SQLHENV henv, SQLHDBC *phdbc )
{
    API_RETCODE retcode;
    SQLHDBC   hdbc;
    SQLHDBC   next_hdbc;
    tpa50Env *env_block_ptr;
    tpa40DBC *dbc_block_ptr;

    API_TRACE(API_TR_ENTRY,"pa40apmadbc",0);
    API_TRACE(API_TR_PTR,"phdbc",&phdbc);

    hdbc = (SQLHDBC) apdallo( (DWORD) sizeof(tpa40DBC));
    if (hdbc == 0) {
        hdbc = SQL_NULL_HDBC;
        retcode = API_NOT_OK;
    }
    else {
        retcode = API_OK;
        dbc_block_ptr = (tpa40DBC FAR *) apdlock (hdbc);
        API_ASSERT_PTR(dbc_block_ptr);
        env_block_ptr = (tpa50Env FAR *) apdlock(henv);
        dbc_block_ptr -> type = API_DBC_TYPE;
        dbc_block_ptr -> state = API_DBC_ALLOCATED;
        dbc_block_ptr -> connectionDead = API_TRUE;
        dbc_block_ptr -> dbno = 0;
        dbc_block_ptr -> ConDesc = NULL;
        dbc_block_ptr -> serverdb[0] = 0;
        dbc_block_ptr -> servernode = NULL;
        dbc_block_ptr -> allocServerNodeLen = 0;
        dbc_block_ptr -> autocommit = SQL_AUTOCOMMIT_DEFAULT;
        dbc_block_ptr -> set_autocommit_flag = dbc_block_ptr -> autocommit;
        dbc_block_ptr -> utility.sp1c_producer = sp1pr_user_cmd;
        dbc_block_ptr -> utility.component = COMP_ODBC;
        dbc_block_ptr -> utility.service = sql_user;
        dbc_block_ptr -> utility.utility_session = FALSE;
        dbc_block_ptr -> isolevel = API_SQL_TXN_EMPTY;
        dbc_block_ptr -> sessionTimeout = SQL_DEFAULT_SESSION_TIMEOUT;
        dbc_block_ptr -> login_timeout = 15;
        dbc_block_ptr -> opt_trace = SQL_OPT_TRACE_OFF;
        dbc_block_ptr -> opt_tracefile_ptr = NULL;
        dbc_block_ptr -> access_mode = SQL_MODE_READ_WRITE;
        dbc_block_ptr -> odbc_cursors = SQL_CUR_USE_DRIVER;
        dbc_block_ptr -> quiet_mode   = 0;
        dbc_block_ptr -> translate_option = 0;    
        dbc_block_ptr -> casesensitive = 0;
        dbc_block_ptr -> blocksize = SHRT_MAX;
        dbc_block_ptr -> parent_henv = henv;
        dbc_block_ptr -> next_hdbc = env_block_ptr -> child_hdbc;
        next_hdbc = env_block_ptr -> child_hdbc;
        dbc_block_ptr -> prev_hdbc = 0;
        env_block_ptr -> child_hdbc = hdbc;
        dbc_block_ptr -> child_hstmt = 0;
        dbc_block_ptr -> kernelVersionString[0] = '\0';
        dbc_block_ptr -> kernelDirect = NULL;
        dbc_block_ptr -> stmtCount = 0;
        dbc_block_ptr -> errorIndex = 0;
        dbc_block_ptr -> cancelRouterStr  = NULL;
        dbc_block_ptr -> dsname           = NULL;
        dbc_block_ptr -> driverConnectStr = NULL;
        dbc_block_ptr -> cpName           = NULL;
        dbc_block_ptr -> encrypt          = SQL_ATTR_ENCRYPT_OFF;

        pa61InitStmtAttr( &dbc_block_ptr -> constmtopt );
        
        pa30InitDiagArea( &dbc_block_ptr->diagArea );
        dbc_block_ptr->applDescList =
            pr09NewDescriptor( 1, sizeof( tpa20Desc* ));
        
        retcode = pa06InitSQLCA(dbc_block_ptr);
        if (retcode == API_OK) {
            dbc_block_ptr->esqblk.tracep = apdallo(sizeof(api_trace_block));
            if ( dbc_block_ptr->esqblk.tracep!= NULL) {
                dbc_block_ptr->esqblk.tracep->usedcount = 0;
                API_MEMCPY(&dbc_block_ptr->esqblk.tracep->sqlta,
                           dbc_block_ptr->esqblk.sqlca.sqlrap->rasqltap,
                           sizeof(sqltatype));
                dbc_block_ptr->esqblk.sqlca.sqlrap->rasqltap
                    = &dbc_block_ptr->esqblk.tracep->sqlta;
            }
            else
                retcode = API_NOT_OK;

            {   /* build EnvDesc, ConContainer, and ConDesc wihtin EnvContainer */
                tpr01_EnvContainer *EnvContainer = env_block_ptr->envContainer;

                dbc_block_ptr->EnvDesc = 
                    EnvContainer->AddDesc (EnvContainer, &dbc_block_ptr->esqblk.sqlca);

            }            

        }
#ifdef ASYNCENABLED
        sqlcreatemutex(&dbc_block_ptr->thread.cs);
#endif
        if (next_hdbc !=0) {
            dbc_block_ptr = (tpa40DBC*) next_hdbc;
            API_ASSERT_PTR(dbc_block_ptr);
            dbc_block_ptr -> prev_hdbc = hdbc;
        }
    }

    *phdbc = hdbc;

    API_TRACE(API_TR_EXIT,"pa40apmadbc",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
    API_TRACE(API_TR_HANDLE,"*phdbc",phdbc);

    return(retcode);
} /* pa40apmadbc */


/* apmfdbc() - API miscellaneous function, free connection */

API_RETCODE pa40_apmfdbc (SQLHDBC hdbc)
{
    API_RETCODE retcode;
    tpa50Env *env_block_ptr;
    tpa40DBC *dbc_block_ptr, *next, *prev;
    SQLHDBC next_hdbc;
    SQLHDBC prev_hdbc;
    SQLHENV parent_henv;
    tpa20Desc **applDescPtrPtr;

    int found = FALSE;
    
    API_TRACE(API_TR_ENTRY,"pa40_apmfdbc",0);
    dbc_block_ptr = (tpa40DBC FAR *) apdlock(hdbc);
    API_ASSERT_PTR(dbc_block_ptr);
#ifdef ASYNCENABLED  
    sqldestroymutex(&dbc_block_ptr->thread.cs);
#endif

    pr05IfCom_String_DeleteString (dbc_block_ptr->userid);
    pr05IfCom_String_DeleteString (dbc_block_ptr->passw);
    pr05IfCom_String_DeleteString (dbc_block_ptr->dsname);
    pr05IfCom_String_DeleteString (dbc_block_ptr->driverConnectStr);
    dbc_block_ptr->userid = dbc_block_ptr->passw            = NULL;
    dbc_block_ptr->dsname = dbc_block_ptr->driverConnectStr = NULL;

    if (dbc_block_ptr->cpName != NULL)
      apdfree (dbc_block_ptr->cpName);

    /* free ConDesc contained in EnvDesc */
    env_block_ptr = (tpa50Env *) dbc_block_ptr->parent_henv;
    env_block_ptr->envContainer->DeleteDesc (dbc_block_ptr->EnvDesc);

    /* rasqlldp is freed in apmfstmt */
    dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlldp = NULL;

    /* search for other connection with same trace file */  /* PTS 1120833 */
    next = (tpa40DBC*) dbc_block_ptr -> next_hdbc;
    while (!found && next != NULL) {
        if (!API_STRCMP (dbc_block_ptr->esqblk.tracep->sqlta.tatracefn,
                         next->esqblk.tracep->sqlta.tatracefn))
            found = TRUE;
    	next = (tpa40DBC*) next -> next_hdbc;
    }


    prev = (tpa40DBC*) dbc_block_ptr -> prev_hdbc;
    while (!found && prev != NULL) {
        if (!API_STRCMP (dbc_block_ptr->esqblk.tracep->sqlta.tatracefn,
                         prev->esqblk.tracep->sqlta.tatracefn))
            found = TRUE;
    	prev = (tpa40DBC*) prev -> prev_hdbc;
    }

    /* if there is no other connection with this trace file, finish */
    if (!found) {
        API_TRACE(API_TR_ENTRY,"p10finish",0);
        p10finish (&dbc_block_ptr -> esqblk.sqlca);
        API_TRACE(API_TR_EXIT,"p10finish",0);
    }

    apdfree(dbc_block_ptr->esqblk.tracep); /* sqlfree */
    API_MEMSET( &dbc_block_ptr->esqblk.sqlca, 0,
                sizeof(dbc_block_ptr->esqblk.sqlca));

    sqlfree(dbc_block_ptr->esqblk.sqlareas);
    next_hdbc = dbc_block_ptr -> next_hdbc;
    prev_hdbc = dbc_block_ptr -> prev_hdbc;
    parent_henv = dbc_block_ptr -> parent_henv;
  
    pa30DeleteDiagArea( &dbc_block_ptr->diagArea );

    /* delete all explicit allocated descriptors */
    pr09SetIndex( dbc_block_ptr->applDescList, 0 );
    applDescPtrPtr = pr09GetItem( dbc_block_ptr->applDescList );
    while (applDescPtrPtr != NULL) {
        retcode = pa20DeleteDesc( *applDescPtrPtr );
        API_ASSERT( retcode == API_OK );
        applDescPtrPtr = pr09GetNextItem( dbc_block_ptr->applDescList );
    }; /* while */
    pr09DeleteDescriptor( dbc_block_ptr->applDescList );

    pa40_FreeServerNode( dbc_block_ptr );
    /* set invalid type. So next operation on this freed handle will return
     * SQL_INVALID_HANDLE (or crashes) */    
    dbc_block_ptr -> type = API_INVALID_HANDLE_TYPE;

    if (prev_hdbc == 0) {           /* change link in parent      */
        env_block_ptr = (tpa50Env FAR *) apdlock(parent_henv);
        API_ASSERT_PTR(env_block_ptr);
        env_block_ptr -> child_hdbc = next_hdbc;
    }
    else {                          /* change link in prev        */
        dbc_block_ptr = (tpa40DBC FAR *) apdlock(prev_hdbc);
        API_ASSERT_PTR(dbc_block_ptr);
        dbc_block_ptr -> next_hdbc = next_hdbc;
    }
    if (next_hdbc != 0) {           /* change link in next        */
        dbc_block_ptr = (tpa40DBC FAR *) apdlock(next_hdbc);
        API_ASSERT_PTR(dbc_block_ptr);
        dbc_block_ptr -> prev_hdbc = prev_hdbc;
    }
    if (dbc_block_ptr -> cancelRouterStr != NULL) {
        apdfree( dbc_block_ptr -> cancelRouterStr );
        dbc_block_ptr -> cancelRouterStr = NULL;
    };
    /*    if (dbc_block_ptr -> driverConnectStr != NULL) {
        apdfree( dbc_block_ptr -> driverConnectStr );
        dbc_block_ptr -> driverConnectStr = NULL;
    };
    */

    apdfree((void*)hdbc);

    retcode = API_OK;

    API_TRACE(API_TR_EXIT,"pa40_apmfdbc",0);
    return(API_OK);
} /* pa40_apmfdbc */


UWORD pa40_apmgfdb( tpa41ESQBlock *esqblk_ptr )
{

return (1);

/*    
    SWORD i;
    SWORD dbno;
 *    API_TRACE(API_TR_ENTRY,"pa40_apmgfbd",0);
 *   i = 0;
 *   dbno = 0;
 *   while (i < API_MAX_CONNECTIONS && dbno == 0)
 *       {
 *           API_TRACE(API_TR_SWORD,"try dbno",&i);
 *           if (esqblk_ptr->sqlca.sqlgap->gaentry[i]->gareference == 0) {
 *               dbno = (SWORD) (i + 1);
 *           }
 *           i++;
 *       }
 *   
 *   API_TRACE(API_TR_EXIT,"pa40_apmgfdb",0);
 *   API_TRACE(API_TR_SWORD,"dbno",&dbno);
 *   
 *   return(dbno);
*/
} /* pa40_apmgfdb */


VOID pa40_FreeServerNode( tpa40DBC *dbc_block_ptr )
{
    if (dbc_block_ptr->servernode != NULL) {
        sqlfree(dbc_block_ptr->servernode);
        dbc_block_ptr->servernode = NULL;
    };
    dbc_block_ptr->allocServerNodeLen = 0;
} /* pa40_FreeServerNode */


/* dummy function for UNIX */
#ifndef WIN32
tsp00_Bool SqlIsSapRouterString (char *node)
{
    return false;
} /* SqlIsSapRouterString */
#endif


RETCODE pa40_GetConnectOption( tpa40DBC  *dbc_block_ptr,
                               SDWORD     fOption,
                               PTR        pvParam,
                               SQLINTEGER buffer_len,
                               SDWORD    *strLenPtr,
                               const tsp77encoding *destEncoding )
{
    RETCODE retcode;

    API_TRACE( API_TR_ENTRY,PA40_FN_GETCONNECTOPTION,0);
    API_TRACE( API_TR_PTR, "dbc_block_ptr", &dbc_block_ptr);
    API_TRACE( API_TR_UWORD,"fOption", &fOption);
    API_TRACE( API_TR_PTR,"pvParam", &pvParam);

    API_ASSERT_PTR(dbc_block_ptr);

    retcode = SQL_SUCCESS;   
    switch (fOption) {
    case (SQL_AUTOCOMMIT): {
        if (pvParam)
            *((UDWORD FAR *) pvParam) = dbc_block_ptr->autocommit;
        break;
	}
	case (SQL_LOGIN_TIMEOUT): {  
        if (pvParam)
            *((UDWORD*) pvParam) = dbc_block_ptr->login_timeout;
        break;
	}
	case (SQL_ACCESS_MODE): {
        if (pvParam)
            *((UDWORD*) pvParam) = dbc_block_ptr->access_mode;
        break;
	}
	case (SQL_OPT_TRACE): {
        if (pvParam)
            *((UDWORD*) pvParam) = dbc_block_ptr->opt_trace;
        break;
	}
	case (SQL_OPT_TRACEFILE): {
          /*
        if (pvParam)
            *((UCHAR**) pvParam) =
                (UCHAR*) dbc_block_ptr->opt_tracefile_ptr;
          */
        if (strLenPtr != NULL) {
            *strLenPtr = 0;
        }; /* if */
        break;
	}
    /* ------ driver defined ------------------------------------------- */
    case (SQL_COMPONENT): {	  
        if (pvParam)
            *((UWORD FAR *) pvParam) = dbc_block_ptr->utility.component;
        break;
    }
    case (SQL_SERVICE): {	  
        if (pvParam)
            *((UWORD FAR *) pvParam) = dbc_block_ptr->utility.service;
        break;
    }
    case (SQL_PRODUCER): {	  
        if (pvParam)
            *((UWORD FAR *) pvParam) = dbc_block_ptr->utility.sp1c_producer;
        break;
    }
    case (SQL_UTILITY_SESSION): {	  
        if (pvParam)	    
            *((UWORD FAR *) pvParam) = dbc_block_ptr->utility.utility_session;
        break;
    }
    case (SQL_IS_NI_CONNECTION): {
        if (SqlIsSaprouterString( (char*) dbc_block_ptr->servernode )) {
            *((UDWORD*) pvParam) = SQL_TRUE;
        } else {
            *((UDWORD*) pvParam) = SQL_FALSE;
        };
        break;
    }
    /* ------ *** ODBC 1.0 * ------------------------------------------- */
	case (SQL_TRANSLATE_DLL): {
          if (pvParam != NULL && 
              buffer_len >= destEncoding->charTable->terminator.byteCount)
            API_MEMCPY (pvParam,
                        destEncoding->charTable->terminator.bytes,
                        destEncoding->charTable->terminator.byteCount);

        if (strLenPtr != NULL) {
            *strLenPtr = 0;
        }; /* if */
        break;
	}
	case (SQL_TRANSLATE_OPTION): {
        if (pvParam)
            *((UDWORD FAR *) pvParam) = dbc_block_ptr->translate_option;
        break;
	}
	case (SQL_TXN_ISOLATION): {
        if (pvParam)
            *((UDWORD FAR *) pvParam) = dbc_block_ptr->isolevel;
        break;
	}	
    /* ------ *** ODBC 2.0 * ------------------------------------------- */
	case (SQL_ODBC_CURSORS): {
        if (pvParam)
            *((UDWORD FAR *) pvParam) = dbc_block_ptr->odbc_cursors;
        break;
	}
	case (SQL_QUIET_MODE): {
        if (pvParam)
            *((UDWORD FAR *) pvParam) = dbc_block_ptr->quiet_mode;
        break;
	}
    case (SQL_SQLMODE): {
        if (pvParam)
            *((SWORD FAR *) pvParam) = dbc_block_ptr->constmtopt.sqlmode;
        break;
	}
    case (SQL_ATTR_DATA_CACHING): {
        if (pvParam) {
            *((UDWORD FAR *) pvParam) =
                dbc_block_ptr->constmtopt.data_caching;
        }; /* if */
        break;
    }
	case (SQL_CONNECT_OPT_DRVR_START): {}
	case (SQL_PACKET_SIZE): {			/* *** ODBC 2.0 */
        pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1C00, NULL );
        retcode = SQL_ERROR;
        break;
	}
    case (SQL_KERNEL_DIRECT): {
        if (pvParam) {
            memcpy( &pvParam, dbc_block_ptr->kernelDirect, sizeof(void*));
        }; /* if */
        break;
    }
    case (SQL_ATTR_SESSION_TIMEOUT): {
        if (pvParam)
            *((UDWORD*) pvParam) = dbc_block_ptr->sessionTimeout;
        break;
    }
    /* ----------------------------------------------------------------- */
	default: {
        pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1092, NULL);
        retcode = SQL_ERROR;
	}
    }

    API_TRACE(API_TR_EXIT,PA40_FN_GETCONNECTOPTION,0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return(retcode);
} /* pa40_GetConnectOption */



SQLRETURN pa40_SetConnectOption( tpa40DBC   *dbc_block_ptr,
                                 SDWORD      fOption,
                                 UDWORD      vParam)
{
    SQLRETURN retcode;
    UWORD sqlState = API_ODBC_00000;
  
    API_TRACE( API_TR_ENTRY, PA40_FN_SETCONNECTOPTION, 0);
    API_TRACE( API_TR_HANDLE, "dbc_block_ptr", &dbc_block_ptr);
    API_TRACE( API_TR_UWORD, "fOption",&fOption);
    API_TRACE( API_TR_UWORD, "vParam",&vParam);

    retcode = SQL_SUCCESS;
    switch (fOption) {
    case (SQL_AUTOCOMMIT):
        switch (vParam) {
        case (0):
        case (1): 
            dbc_block_ptr->autocommit = vParam;
            dbc_block_ptr->set_autocommit_flag = dbc_block_ptr->autocommit;
            break;
        default:
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1009, NULL );
            /* invalid argument */
            retcode = SQL_ERROR;
            break; 
        }
        break;
    case (SQL_LOGIN_TIMEOUT):    
        dbc_block_ptr->login_timeout = vParam;
        break;
    case (SQL_ACCESS_MODE): 
        switch (vParam) {
        case (SQL_MODE_READ_ONLY):
        case (SQL_MODE_READ_WRITE): 
            dbc_block_ptr->access_mode = vParam;
            break;
        default:
            pa40PutError( (SQLHDBC) dbc_block_ptr,
                          API_ODBC_S1009,       /* invalid argument */
                          NULL);
            retcode = SQL_ERROR;
            break; 
        }
        break;       
    case (SQL_OPT_TRACE): 
        switch (vParam) {
        case (SQL_OPT_TRACE_OFF):
            dbc_block_ptr->opt_trace = vParam;
            break;
        case (SQL_OPT_TRACE_ON): 
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_01S02, NULL);
            /* Option value change */         
            retcode = SQL_SUCCESS_WITH_INFO;
            break;
        default:
            pa40PutError( (SQLHDBC) dbc_block_ptr,
                          API_ODBC_S1009,       /* invalid argument */
                          NULL);
            retcode = SQL_ERROR;
            break; 
        }
        break;
        /* --- *** ODBC 1.0 * -------------------------------------- */
    case (SQL_TRANSLATE_DLL): 
        pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_IM009, NULL );
        /* Unable to load translation DLL*/
        retcode = SQL_ERROR;
        break;               
    case (SQL_TRANSLATE_OPTION): 
        dbc_block_ptr->translate_option = vParam;
        break;
    case (SQL_TXN_ISOLATION): 
        switch (vParam) {
        case (SQL_TXN_READ_COMMITTED): {}
        case (SQL_TXN_READ_UNCOMMITTED): {}
        case (SQL_TXN_REPEATABLE_READ): {}
        case (SQL_TXN_SERIALIZABLE): {
            dbc_block_ptr->isolevel = vParam;
            break;
        }	
        default:
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1C00, NULL);
            dbc_block_ptr->isolevel = dbc_block_ptr->defisolevel;
            retcode = SQL_ERROR;
            break; 
        }
        break;               
        /* --- *** ODBC 2.0 * -------------------------------------- */
    case (SQL_ODBC_CURSORS): 
        switch (vParam) {
        case (SQL_CUR_USE_IF_NEEDED):
        case (SQL_CUR_USE_ODBC):
        case (SQL_CUR_USE_DRIVER):
            dbc_block_ptr->odbc_cursors = vParam;
            break;
        default:
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1009, NULL);
            /* invalid argument */
            retcode = SQL_ERROR;
            break; 
        }
        break;           
    case (SQL_QUIET_MODE):    
        dbc_block_ptr->quiet_mode = vParam;
        break;
    case (SQL_OPT_TRACEFILE): {} 
    case (SQL_CONNECT_OPT_DRVR_START): {}
    case (SQL_PACKET_SIZE): {}				/* *** ODBC 2.0 */
    case (SQL_CURRENT_QUALIFIER): {			/* *** ODBC 2.0 */
        pa40PutError( (SQLHDBC) dbc_block_ptr,
                      API_ODBC_HYC00, NULL); /* PTS 1105365 */
        retcode = SQL_ERROR;
        break;
    }      
    /* ----------------------------------------------------------- */    
    case (SQL_COMPONENT): {	  
        if ((SDWORD)vParam >= COMP_CALL && vParam <= (UWORD)COMP_XCI) {
            dbc_block_ptr->utility.component = (UWORD)vParam;
        }
        else {
            pa40PutError( (SQLHDBC) dbc_block_ptr, API_ODBC_S1009, NULL);
            /* invalid argument */
            retcode = SQL_ERROR;
        }
        break;
    }	
    case (SQL_SERVICE): {	  
        dbc_block_ptr->utility.service = (UWORD)vParam;
        break;
    }	
    case (SQL_PRODUCER): {	  
        dbc_block_ptr->utility.sp1c_producer = (UWORD)vParam;
        break;
    }	
    case (SQL_UTILITY_SESSION): {	  
        if ((UWORD)vParam) {
            dbc_block_ptr->utility.utility_session = TRUE;
        }
        else {
            dbc_block_ptr->utility.utility_session = FALSE;
        }
        break;
    }
    case (SQL_KERNEL_DIRECT): {
        memcpy( dbc_block_ptr->kernelDirect, &vParam, sizeof(void*) );
        break;
    }
    /******* SQLSetStmtOtionen ********/    
    default: {
        
        retcode = pa61SetStmtAttr( SQL_NULL_HSTMT,
                                   API_STMT_ALLOCATED,
                                   fOption,
                                   SQLPOINTER_CAST (vParam),
                                   0,
                                   &dbc_block_ptr->constmtopt,
                                   &sqlState );
        if (sqlState != API_ODBC_00000) {
            pa40PutError( (SQLHDBC) dbc_block_ptr, sqlState, NULL);      
        }; /* if */
    }
    }        

    API_TRACE(API_TR_EXIT, PA40_FN_SETCONNECTOPTION, 0);
    API_TRACE(API_TR_RETCODE,"retcode",&retcode);

    return(retcode);

} /* pa40_SetConnectOption */


#define MAX_PACKET_SIZE (1024*128)

VOID pa40_GuessPacketSize( tpa40DBC *dbc_block_ptr )
{
    /* if we already connected this part should exist */
    if (dbc_block_ptr->esqblk.sqlca.sqlrap
        ->rasegmpartptr[sp1pk_user_info_returned]) {
        /* blocksize must be at least as big as packet-size for
         * long handling (PTS 1105821) */
        dbc_block_ptr->blocksize = dbc_block_ptr->esqblk.sqlca.sqlrap
            ->rasegmpartptr[sp1pk_user_info_returned]->variant
            .C_1.sp1p_part_header_F.sp1p_buf_size + 2048;
    } else {
        /* assume max packet size if we cannot get the actual packet size */
        dbc_block_ptr->blocksize = MAX_PACKET_SIZE;
    };
} /* pa40_GuessPacketSize */

boolean pa40_MapOsUser(UCHAR *driverName, tpr05_String *useridStr, tpr05_String *passwdStr)
{
  /* Datasourceless connect read options from ODBCINST.INI */
  char szUseOSUser[512];
  tsp00_ErrTextc    errTextc;
  RTE_IniFileResult  ok;
  boolean mapped = false;
  SAPDB_Int4 ret = RTE_GetConfigString( "SOFTWARE\\ODBC\\ODBCINST.INI",
                                        (char*) driverName,
                                        "UseOSUser",
                                        szUseOSUser,
                                        sizeof(szUseOSUser),
                                        errTextc,
                                        &ok);
  if (SAPDB_INIFILE_RESULT_OK == ok) {
    if (szUseOSUser[0] == 'y' || szUseOSUser[0] == 'Y') {
      tsp78ConversionResult ConvRes = sp78_Ok;
      char tmp [API_USERID_MAX_LEN];
      char userid [API_USERID_MAX_LEN];
      tsp00_Uint4 destBytesWritten, srcBytesParsed;
      /* convert username to ascii */
      ConvRes = sp78convertString (sp77encodingAscii,
                                   userid, sizeof(userid), &destBytesWritten, TRUE,
                                   useridStr->encodingType, useridStr->rawString, useridStr->cbLen,
                                   &srcBytesParsed);
      if (ConvRes == sp78_Ok) {
        ret = RTE_GetConfigString ("SOFTWARE\\SAP\\SAP DBTech\\ODBC\\UserMapping", userid, "UID",
                                   tmp, sizeof(tmp), errTextc, &ok);
        if (SAPDB_INIFILE_RESULT_OK == ok) {
          ConvRes = sp78convertString (useridStr->encodingType, useridStr->rawString, 
                                       useridStr->cbMaxLen, &useridStr->cbLen, TRUE,
                                       sp77encodingAscii, tmp, (tsp00_Uint4)strlen(tmp), &srcBytesParsed);
          if (ConvRes == sp78_Ok)
            mapped = true;
        }
        
        ret = RTE_GetConfigString ("SOFTWARE\\SAP\\SAP DBTech\\ODBC\\UserMapping", userid, "PWD", 
                                    tmp, sizeof(tmp), errTextc, &ok);
        if (SAPDB_INIFILE_RESULT_OK == ok) {
          ConvRes = sp78convertString (passwdStr->encodingType, passwdStr->rawString, 
                                       passwdStr->cbMaxLen, &passwdStr->cbLen, TRUE,
                                       sp77encodingAscii, tmp, (tsp00_Uint4)strlen(tmp), &srcBytesParsed);
          if (ConvRes == sp78_Ok)
            mapped = true;
        }
      }
    }
  }
  return mapped;
}

/* loads a codepage for UCS2 <-> ASCII from the DB. Codepages have exactly 512 bytes length. */
/* Once a codepage is loaded, it is reused, otherwise it is requested from the DB */
#define PA40_CODEPAGELENGTH 512
#define PA_40CODEPAGENAME    40
#define PA40_SELECTSTMT      "select CODEPAGETABLE from %s.codepage where CODEPAGEID = '%s'"
int pa40_LoadCodePageFromDB (SQLHDBC hdbc, SQLCHAR *cpName, tsp81_CodePage *cp)
{
  static char codepageName[PA_40CODEPAGENAME] = "\0";
  static char codepage[PA40_CODEPAGELENGTH];

  char loaded = FALSE;

  if (API_STRCMP (cpName, codepageName) == 0) 
    loaded = TRUE;
  else {
    SQLHSTMT  hstmt;
    SQLLEN    len;
    SQLRETURN retcode;
    char buffer[PA40_CODEPAGELENGTH];
    SQLCHAR stmt[512];
    tsp00_Uint2 swaptest = 1;
    char *p = (char*) &swaptest;
    int i;
    tpa40DBC *dbc_block_ptr = (tpa40DBC*) hdbc;
    UCHAR *kernelVer = dbc_block_ptr->kernelVersionString;
    int kernel_75orlower;
    kernel_75orlower = kernelVer[0] < '7'  ||  (kernelVer[0] == '7' && kernelVer[2] < '6');

    /* select codepage from DB. If an error occurs call SQLFreeHandle. */
    retcode = SQLAllocHandle (SQL_HANDLE_STMT, hdbc, &hstmt);
    if (retcode != SQL_SUCCESS)
      goto exit;
    retcode = SQLBindCol (hstmt, 1, SQL_C_BINARY,
                          buffer, sizeof(buffer), &len);
    if (retcode != SQL_SUCCESS)
      goto FreeHandle;

    if (kernel_75orlower)
      API_SPRINTF ((char*) stmt, PA40_SELECTSTMT, "DBA", cpName);
    else
      API_SPRINTF ((char*) stmt, PA40_SELECTSTMT, "SYSLOADER", cpName);
    retcode = SQLExecDirect (hstmt, stmt, SQL_NTS);
    if (retcode != SQL_SUCCESS)
      goto FreeHandle;
    retcode = SQLFetch (hstmt);
    if (retcode != SQL_SUCCESS)
      goto FreeHandle;

    loaded = TRUE;
    API_STRNCPY (codepageName, cpName, PA_40CODEPAGENAME);

    /* codepage comes from DB in big endian. Swap if necessary  */
    if (*p == 1)   /* little endian */
      for (i=0; i<PA40_CODEPAGELENGTH; i+=2)  {
        codepage[i]   = buffer[i+1];
        codepage[i+1] = buffer[i];
      }
    else
      API_MEMCPY (codepage, buffer, sizeof(codepage));

  FreeHandle:
    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
  }

  if (loaded && cp != NULL) {
    API_STRNCPY (cp->name, cpName, sizeof(cp->name));
    API_MEMCPY (cp->map, codepage, sizeof(codepage));
  }

 exit:
  return (loaded);
}

/* the given user name is qouted if necessary (i.e. if it contains any letter
   that is not capital). Furthermore the tpr05_String is converted to UCS2.
   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123115 */
void pa40_quoteStringToUCS2 (tpr05_String *string)
{
#define STRING_LEN ((API_USERID_MAX_LEN > API_PASSWD_MAX_LEN ? API_USERID_MAX_LEN : API_PASSWD_MAX_LEN) + 2)
    unsigned int   charOut;
    tsp81_UCS2Char buffer[STRING_LEN];
    tsp81_UCS2Char l_stringBuf[STRING_LEN];

    if (string == NULL)
      return;

    /* convert to UCS2 */
    if (string->encodingType == sp77encodingAscii) {
      sp81ASCIItoUCS2 (l_stringBuf, sizeof (l_stringBuf) / sizeof (SQLWCHAR) - 1,
                       sp77encodingUCS2Swapped == sp77nativeUnicodeEncoding (),
                       &charOut,
                       (tsp00_Byte *) string->rawString,
                       string->cbLen);
      string->cbLen = charOut * sizeof (SQLWCHAR);
      API_MEMSET (&l_stringBuf[charOut], 0, sizeof (SQLWCHAR));
    }
    else  {   /* UCS 2 */
      sp81UCS2strncpy (l_stringBuf,
                       (tsp81_UCS2Char*) string->rawString, string->cbLen / sizeof (SQLWCHAR));
      API_MEMSET (&l_stringBuf[string->cbLen / sizeof (SQLWCHAR)], 0, sizeof (SQLWCHAR));
    }

    /* does a conversion to capital letters change the user name? */
    API_MEMCPY (buffer, l_stringBuf, sizeof (SQLWCHAR) +  string->cbLen);
    aputoupW (l_stringBuf);
    /* if yes, quote the name (") */
    if (API_MEMCMP (buffer, l_stringBuf, string->cbLen) != 0) {
      sp81ASCIItoUCS2 (l_stringBuf, 1,
                       sp77encodingUCS2Swapped == sp77nativeUnicodeEncoding (),
                       &charOut, (tsp00_Byte *) "\"", sizeof ("\""));
      API_MEMCPY (l_stringBuf + 1, buffer, string->cbLen);
      sp81ASCIItoUCS2 (l_stringBuf + 1 + sp81UCS2strlen (buffer), 1,
                       sp77encodingUCS2Swapped == sp77nativeUnicodeEncoding (),
                       &charOut, (tsp00_Byte *) "\"", sizeof ("\""));
      string->cbLen += 2 * sizeof(SQLWCHAR);
      API_MEMSET (&l_stringBuf[string->cbLen / sizeof (SQLWCHAR)], 0, sizeof (SQLWCHAR));
    }

    /* fill in the converted name into the tpr05_String */
    pr05IfCom_String_ClearString (string);
    pr05IfCom_String_strcatP (string, (char *) l_stringBuf,
                              sp81UCS2strlen (l_stringBuf) * sizeof (SQLWCHAR),
                              sp77nativeUnicodeEncoding ());

}

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
/* #endif   of _UNICODE_ODBC */
#endif  /* _UNICODE_ODBC */
