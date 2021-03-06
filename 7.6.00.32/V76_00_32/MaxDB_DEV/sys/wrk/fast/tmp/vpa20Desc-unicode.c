/*!**********************************************************************

  module: vpa20Desc.c

  -----------------------------------------------------------------------

  responsible:

  special area: ODBC-Driver

  description:  ODBC-Descriptors - Datastructures and functions

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

#include "vpa09.h"
#include "vpa10Handle.h"
#include "vpa20Desc.h"
#include "vpa50Env.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"
#include "vpa70SQLDA.h"
#include "vpa80String.h"
#include "livecachetypes.h" /* no check */

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

/* hostindicator must be aligned. PTS 1102928 */
#define ALIGNMENT 8
#define ALIGN(n) ((n % ALIGNMENT) == 0) ? n : (n + ALIGNMENT - (n % ALIGNMENT))

/* Access rights for header and record fields */
#define API_ACCESS_READ_ONLY    (0)
#define API_ACCESS_READ_WRITE   (1)
#define API_ACCESS_UNUSED       (2)

/* display size for numeric data types (pa20_GetDisplaySize) */
#define API_BIT_DISP      (1)
#define API_TINYINT_DISP  (4)
#define API_SMALLINT_DISP (6)
#define API_INTEGER_DISP  (11)
#define API_BIGINT_DISP   (20)
#define API_REAL_DISP     (13)
#define API_FLOAT_DISP    (22)
#define API_DOUBLE_DISP   (22)
#define API_DATE_DISP     (10)
#define API_TIME_DISP     (8)
#define API_TIMESTMP_DISP (19)
#define API_TIMESTMP_EXT_DISP (26)

/* description size for numeric data types (pa20_GetDescLength) */
#define API_BIT_DESC      (1)
#define API_TINYINT_DESC  (3)
#define API_SMALLINT_DESC (5)
#define API_INTEGER_DESC  (10)
#define API_BIGINT_DESC   (20)
#define API_REAL_DESC     (13)
#define API_FLOAT_DESC    (22)
#define API_DOUBLE_DESC   (22)
#define API_DATE_DESC     (10)
#define API_TIME_DESC     (8)
#define API_TIMESTMP_DESC (19)
#define API_TIMESTMP_EXT_DESC (26)


/* ESQ data type names  (pa20_GetTypeNames) */
#define API_CHAR_NAME "CHAR"
#define API_VARCHAR_NAME "VARCHAR"
#define API_LONGVARCHAR_NAME "LONG"
#define API_CHAR_UNICODE_NAME_OLD "CHAR() UNICODE"
#define API_CHAR_UNICODE_NAME "CHAR UNICODE"
#define API_VARCHAR_UNICODE_NAME_OLD "VARCHAR() UNICODE"
#define API_VARCHAR_UNICODE_NAME "VARCHAR UNICODE"
#define API_LONGVARCHAR_UNICODE_NAME "LONG UNICODE"
#define API_DECIMAL_NAME "DECIMAL"
#define API_NUMERIC_NAME "NUMERIC"
#define API_BIT_NAME "BOOLEAN"
#define API_TINYINT_NAME "TINYINT"
#define API_SMALLINT_NAME "SMALLINT"
#define API_INTEGER_NAME "INTEGER"
#define API_BIGINT_NAME "BIGINT"
#define API_REAL_NAME "REAL"
#define API_FLOAT_NAME "FLOAT"
#define API_DOUBLE_NAME "DOUBLE PRECISION"
#define API_BINARY_NAME_OLD "CHAR() BYTE"
#define API_BINARY_NAME "CHAR BYTE"
#define API_VARBINARY_NAME_OLD "VARCHAR() BYTE"
#define API_VARBINARY_NAME "VARCHAR BYTE"
#define API_LONGVARBINARY_NAME "LONG BYTE"
#define API_DATE_NAME "DATE"
#define API_TIME_NAME "TIME"
#define API_TIMESTAMP_NAME "TIMESTAMP"

#undef PA07PASSTHROUGH

/* !!! The following enums are duplicated from vpa70SQLDA.c !!! */

/* for ODBC 3.5 the CODETYPE column is different */
#define COCODETYPE_35     19
#define PCOLCODETYPE_35   20

typedef struct {
   UCHAR type_name[API_MAX_KEYWORD_LEN+1];
   UCHAR code_type[8];
   UCHAR sql_type_name[API_MAX_KEYWORD_LEN+1];
   SWORD data_type;
   SDWORD precision;
   SDWORD lenght;
} pa20ODBCTypesStruct;

#define LONG_PREC INT_MAX

/* now kernel supports CHAR UNICODE()  length info at the end
   http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138840 */
static pa20ODBCTypesStruct pa20ODBCTypes_old[] = {
{ "CHAR", "ASCII", "", SQL_CHAR, 254, 0},
{ "CHAR", "BYTE", "CHAR() BYTE", SQL_BINARY, 254, 0},
{  "CHAR",  "EBCDIC",  "CHAR() BYTE", SQL_BINARY, 254, 0},
{  "CHAR",  "UNICODE",  "CHAR() UNICODE", SQL_UNICODE, 254, 0},
{  "VARCHAR",  "ASCII",  "", SQL_VARCHAR, 4000, 0},
{  "VARCHAR",  "BYTE",  "VARCHAR() BYTE", SQL_VARBINARY, 4000, 0},
{  "VARCHAR",  "EBCDIC",  "VARCHAR() BYTE", SQL_VARBINARY, 4000, 0},
{  "VARCHAR",  "UNICODE",  "VARCHAR() UNICODE", SQL_UNICODE_VARCHAR, 2000, 0},
{  "FIXED",  "",  "DECIMAL", SQL_DECIMAL, API_SQL_DECIMAL_PRECISION, 0},
{  "FIXED",  "",  "NUMERIC", SQL_NUMERIC, API_SQL_NUMERIC_PRECISION, 0},
{  "INTEGER",  "",  "", SQL_INTEGER, 10, 0},
{  "SMALLINT",  "",  "", SQL_SMALLINT, 5, 0},
{  "FLOAT",  "",  "", SQL_FLOAT, API_SQL_FLOAT_PRECISION, 0},
{  "REAL",  "",  "", SQL_REAL, 15, 0},
{  "DOUBLE PRECISION", "",  "", SQL_DOUBLE, 16, 0},
{  "DATE",  "",  "", SQL_DATE, 10, sizeof(DATE_STRUCT)},
{  "TIME",  "",  "", SQL_TIME, 8, sizeof(TIME_STRUCT)},
{  "TIMESTAMP",  "",  "", SQL_TIMESTAMP, 19, sizeof(TIMESTAMP_STRUCT)},
{  "LONG",  "ASCII",  "", SQL_LONGVARCHAR, LONG_PREC, 0},
{  "LONG",  "BYTE",  "LONG BYTE", SQL_LONGVARBINARY, LONG_PREC, 0},
{  "LONG",  "EBCDIC",  "LONG BYTE", SQL_LONGVARBINARY, LONG_PREC, 0},
{  "LONG",  "UNICODE",  "LONG UNICODE", SQL_UNICODE_LONGVARCHAR, LONG_PREC, 0},
{  "BOOLEAN",  "",  "", SQL_BIT, 1, 0},
{  "ABAPTABLE",  "",  "ABAPTABLE", SQL_ABAPHANDLE, sizeof(SQL_LC_StreamParm), 0},
{  "",  "",  "", SQL_TYPE_NOTFOUND, 0, 0}
};

static pa20ODBCTypesStruct pa20ODBCTypes[] = {
{ "CHAR", "ASCII", "", SQL_CHAR, 254, 0},
{ "CHAR", "BYTE", "CHAR BYTE", SQL_BINARY, 254, 0},
{  "CHAR",  "EBCDIC",  "CHAR BYTE", SQL_BINARY, 254, 0},
{  "CHAR",  "UNICODE",  "CHAR UNICODE", SQL_UNICODE, 254, 0},
{  "VARCHAR",  "ASCII",  "", SQL_VARCHAR, 4000, 0},
{  "VARCHAR",  "BYTE",  "VARCHAR BYTE", SQL_VARBINARY, 4000, 0},
{  "VARCHAR",  "EBCDIC",  "VARCHAR BYTE", SQL_VARBINARY, 4000, 0},
{  "VARCHAR",  "UNICODE",  "VARCHAR UNICODE", SQL_UNICODE_VARCHAR, 2000, 0},
{  "FIXED",  "",  "DECIMAL", SQL_DECIMAL, API_SQL_DECIMAL_PRECISION, 0},
{  "FIXED",  "",  "NUMERIC", SQL_NUMERIC, API_SQL_NUMERIC_PRECISION, 0},
{  "INTEGER",  "",  "", SQL_INTEGER, 10, 0},
{  "SMALLINT",  "",  "", SQL_SMALLINT, 5, 0},
{  "FLOAT",  "",  "", SQL_FLOAT, API_SQL_FLOAT_PRECISION, 0},
{  "REAL",  "",  "", SQL_REAL, 15, 0},
{  "DOUBLE PRECISION", "",  "", SQL_DOUBLE, 16, 0},
{  "DATE",  "",  "", SQL_DATE, 10, sizeof(DATE_STRUCT)},
{  "TIME",  "",  "", SQL_TIME, 8, sizeof(TIME_STRUCT)},
{  "TIMESTAMP",  "",  "", SQL_TIMESTAMP, 19, sizeof(TIMESTAMP_STRUCT)},
{  "LONG",  "ASCII",  "", SQL_LONGVARCHAR, LONG_PREC, 0},
{  "LONG",  "BYTE",  "LONG BYTE", SQL_LONGVARBINARY, LONG_PREC, 0},
{  "LONG",  "EBCDIC",  "LONG BYTE", SQL_LONGVARBINARY, LONG_PREC, 0},
{  "LONG",  "UNICODE",  "LONG UNICODE", SQL_UNICODE_LONGVARCHAR, LONG_PREC, 0},
{  "BOOLEAN",  "",  "", SQL_BIT, 1, 0},
{  "ABAPTABLE",  "",  "ABAPTABLE", SQL_ABAPHANDLE, sizeof(SQL_LC_StreamParm), 0},
{  "",  "",  "", SQL_TYPE_NOTFOUND, 0, 0}
};


/*!*********************************************************************
  Caution: Internal Functions do not check the input parameters
           for NULL etc. */


/*!*********************************************************************
  Function: pa20_ConsistentCopyDesc

  description: Like pa20CopyDesc but preserves the allocType and
               descType in the target desc and long handles are
               not copied (they are initialized with NULL).

  arguments:
    tpa20Desc  *inDesc  [in]  The source desc.
    tpa20Desc  *outDesc [in]  The target desc.

  return value: API_RETCODE (API_OK or API_NOT_OK)

  The function alloctes a new record-area, diagArea and long handles
  for outDesc. The descriptor outDesc m u s t be freed with pa20FreeDesc!
  The pointers are not checked for NULL.
  */

API_RETCODE pa20_ConsistentCopyDesc( tpa20Desc *inDesc, tpa20Desc *outDesc );


/*!*********************************************************************
  Function: pa20_FreeRecords

  description: Frees all records

  arguments:
    tpa20Desc  *descPtr  [in]  The descriptor.

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20_FreeRecords( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20_GetAccessRights

  description: Returns the access rights (read, read/write, unused)
               for a given field and descriptor-type (AD,IPD,IRD,ARD,APD).

  arguments:
    SWORD   fieldId   [in]   The field id of the header or record field
                             as defined in sql.h (SQL_DESC_...)
    UWORD   descType  [out]  The type of descriptor (API_DESC_TYPE_AD,
                             ...IPD,...IRD.

  return value: RETCODE
                API_ACCESS_READ_WRITE -- the value may be changed
                API_ACCESS_READ_ONLY  -- the value can only be read
                API_ACCESS_UNUSED     -- the field is not used for the
                                         given descriptor type

  For invalid field id�s or descriptor types the function returns
  API_ACCESS_UNUSED.
  */

UWORD pa20_GetAccessRights( SWORD fieldId, UWORD descType );


SWORD pa20_GetConciseType( UDWORD odbcVersion, SWORD type );

SWORD pa20GetDateTimeIntervalCode( UWORD descType, tpa21DescRecord *rec );

SDWORD pa20_GetDisplaySize( tpa21DescRecord *irdRecPtr );

UDWORD pa20_GetCLength( tpa21DescRecord *applRecPtr );

SDWORD pa20_GetNumPrecRadix( SWORD sqlType );

UDWORD pa20_GetSQLLength( tpa21DescRecord *implRecPtr );

SDWORD pa20_GetDescLength (tpa21DescRecord *implRecPtr);

SWORD pa20_GetSQLPrec( tpa21DescRecord *implRecPtr );

SWORD pa20GetType( UWORD descType, tpa21DescRecord *rec );

UCHAR* pa20_GetTypeName( tpa21DescRecord *implRecPtr, int old );

SWORD pa20_IsCaseSensitive( tpa21DescRecord *implRecPtr );

UWORD pa20_IsConciseCType( SWORD type );

UWORD pa20_IsConciseSqlType( SWORD type );

UDWORD pa20_ComputeCharacterLength (UDWORD           length,
                                    tpa21DescRecord *impl_rec_ptr,
                                    SWORD            fCType);

/*!*********************************************************************
  Function: pa20_IsCType

  description: Check if C type id is valid.

  arguments:
    SWORD   type [in]  C type id (SQL_C_CHAR, ...)

  return value: API_RETODE
    API_OK      --  type is a C type id
    API_NOT_OK  --  type is not a C type id
  */

UWORD pa20_IsCType( SWORD type );

/* ... */
UWORD pa20_IsSearchable( tpa21DescRecord *irdPtr );


/*!*********************************************************************
  Function: pa20_IsSqlType

  description: Check if SQL type id is valid.

  arguments:
    SWORD   type [in]   SQL type id (SQL_INTEGER, ...)

  return value: API_RETODE
    API_OK      --  type is a SQL type id
    API_NOT_OK  --  type is not a SQL type id
  */

UWORD pa20_IsSqlType( SWORD type );


/*!*********************************************************************
  Function: pa20_IsValidFieldId

  description: Check if field id is an valid header or record field.

  arguments:
    SWORD   fieldId [in]  Field id (SQL_DESC_ALLOC_TYPE, ...)

  return value: API_RETODE
    API_OK      --  fieldId is a valid field id
    API_NOT_OK  --  fieldId is not a valid field id
  */

UWORD pa20_IsValidFieldId( SWORD fieldId );


/*!*********************************************************************
  Function: pa20_ResetError

  description: Frees all eisting error records

  arguments:
    SQLHDESC  hdesc     [out]  Descriptor handle

  return value: API_RETODE
    API_OK      --  the error record was successfully freed
    API_NOT_OK  --  the error record could not be freed
  */

API_RETCODE pa20_ResetError( SQLHDESC hdesc );


/*!**********************************************************************

  Chapter: ODBC functions

************************************************************************/

#ifndef SAPDB_FAST
#ifndef _UNICODE_ODBC
UCHAR *PA20DESC_FN_SQLCOPYDESC     = { (UCHAR*) "SQLCopyDesc" };
UCHAR *PA20DESC_FN_SQLGETDESCFIELD = { (UCHAR*) "SQLGetDescField" };
UCHAR *PA20DESC_FN_SQLGETDESCREC   = { (UCHAR*) "SQLGetDescRec" };
UCHAR *PA20DESC_FN_SQLSETDESCFIELD = { (UCHAR*) "SQLSetDescField" };
UCHAR *PA20DESC_FN_SQLSETDESCREC   = { (UCHAR*) "SQLSetDescRec" };
#else
extern UCHAR *PA20DESC_FN_SQLCOPYDESC;
extern UCHAR *PA20DESC_FN_SQLGETDESCFIELD;
extern UCHAR *PA20DESC_FN_SQLGETDESCREC;
extern UCHAR *PA20DESC_FN_SQLSETDESCFIELD;
extern UCHAR *PA20DESC_FN_SQLSETDESCREC;
#endif
#endif  /* of SAPDB_FAST */


#ifndef _UNICODE_ODBC
/* ODBC 3.0 */
ODBC_PROC(SQLCopyDesc, ( SQLHDESC sourceHdesc,
                         SQLHDESC targetHdesc ),
          ( sourceHdesc, targetHdesc ))
{
    SQLRETURN  retcode = SQL_SUCCESS;
    UWORD      sqlState = API_ODBC_00000; /* Success */
    tpa20Desc *targetPtr;
    tpa20Desc *sourcePtr;
    UWORD      stmtState;
    tpa60Stmt *parentStmt;

    API_TRACE( API_TR_ENTRY,PA20DESC_FN_SQLCOPYDESC, 0);
    API_TRACE( API_TR_HANDLE,"sourceHdesc", &sourceHdesc);
    API_TRACE( API_TR_HANDLE,"targetHdesc", &targetHdesc);

    if ( pa20VerifyDesc(sourceHdesc) != API_OK
        || pa20VerifyDesc(targetHdesc) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        pa20_ResetError( sourceHdesc );
        targetPtr = (tpa20Desc*) targetHdesc;
        sourcePtr = (tpa20Desc*) sourceHdesc;
        if (pa20GetDescType( targetPtr ) == API_DESC_TYPE_IRD) {
            retcode = SQL_ERROR;
            sqlState = API_ODBC_HY016; /* Cannot modify an implementation
                                        * row descriptor */
        } else {
            if ( pa20GetDescType( sourcePtr ) == API_DESC_TYPE_IRD) {
                API_ASSERT( pa60VerifyStmt(sourcePtr->parentStmt) == API_OK );
                parentStmt = (tpa60Stmt*) sourcePtr->parentStmt;
                stmtState = parentStmt->state;
                if ( stmtState != API_STMT_PREPARED
                     && stmtState != API_STMT_EXECUTED
                     && stmtState != API_STMT_POSITIONED ) {
                    retcode = SQL_ERROR;
                    sqlState = API_ODBC_HY007; /* Associated statement is
                                                * not prepared */
                }; /* if */
            } else {
                if (pa20_ConsistentCopyDesc( sourcePtr, targetPtr )!=API_OK) {
                    retcode = SQL_ERROR;
                    sqlState = API_ODBC_HY013; /* Memory menagement error */
                }; /* if */
                /* Konstistenz check ... !!! */
            }; /* else */
        }; /* else */
    }; /* else */

    if (sqlState!=API_ODBC_00000) { /* != SUCCESS */
        pa20PutError( sourceHdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA20DESC_FN_SQLCOPYDESC, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    return retcode;
} /* SQLCopyDesc */
#endif  /* of _UNICODE_ODBC */

/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetDescFieldW, (SQLHDESC     hdesc,
                             SQLSMALLINT  recNo,
                             SQLSMALLINT  fieldId,
                             SQLPOINTER   valuePtr,
                             SQLINTEGER   len,
                             SQLINTEGER  *lenPtr ),
          ( hdesc, recNo, fieldId, valuePtr, len, lenPtr ))
#else
ODBC_PROC(SQLGetDescField, (SQLHDESC     hdesc,
                            SQLSMALLINT  recNo,
                            SQLSMALLINT  fieldId,
                            SQLPOINTER   valuePtr,
                            SQLINTEGER   len,
                            SQLINTEGER  *lenPtr ),
          ( hdesc, recNo, fieldId, valuePtr, len, lenPtr ))
#endif
{
    SQLRETURN        retcode = SQL_ERROR;
    UWORD            sqlState = API_ODBC_00000; /* Success */
    tpa20Desc       *descPtr;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii;
#endif

    API_TRACE( API_TR_ENTRY,PA20DESC_FN_SQLGETDESCFIELD, 0);
    API_TRACE( API_TR_HANDLE,"hdesc", &hdesc);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_DESCTYPE, "fieldId", &fieldId);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);
    API_TRACE( API_TR_PTR, "lenPtr", &lenPtr );

    if (pa20VerifyDesc(hdesc)!=API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        pa20_ResetError( hdesc );
        descPtr = (tpa20Desc*) hdesc;
        retcode = pa20GetDescField( descPtr,
                                    recNo,
                                    fieldId,
                                    valuePtr,
                                    len,
                                    lenPtr,
                                    &sqlState,
                                    nativeEncoding);
    }; /* else */
    if (sqlState!=API_ODBC_00000) { /* != SUCCESS */
        pa20PutError( hdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA20DESC_FN_SQLGETDESCFIELD, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE( API_TR_SDWORD, "*lenPtr", lenPtr );
    return retcode;
} /* SQLGetDescField */


/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLGetDescRecW, (SQLHDESC      hdesc,
                           SQLSMALLINT   recNo,
                           SQLWCHAR     *name,
                           SQLSMALLINT   bufferLen,
                           SQLSMALLINT  *strLenPtr,
                           SQLSMALLINT  *typePtr,
                           SQLSMALLINT  *sub_typePtr,
                           SQLLEN       *lengthPtr,
                           SQLSMALLINT  *precisionPtr,
                           SQLSMALLINT  *scalePtr,
                           SQLSMALLINT  *nullablePtr ),
          ( hdesc, recNo, name, bufferLen, strLenPtr,
            typePtr, sub_typePtr, lengthPtr, precisionPtr,
            scalePtr, nullablePtr ))
#else
ODBC_PROC(SQLGetDescRec, (SQLHDESC      hdesc,
                          SQLSMALLINT   recNo,
                          SQLCHAR      *name,
                          SQLSMALLINT   bufferLen,
                          SQLSMALLINT  *strLenPtr,
                          SQLSMALLINT  *typePtr,
                          SQLSMALLINT  *sub_typePtr,
                          SQLLEN       *lengthPtr,
                          SQLSMALLINT  *precisionPtr,
                          SQLSMALLINT  *scalePtr,
                          SQLSMALLINT  *nullablePtr ),
          ( hdesc, recNo, name, bufferLen, strLenPtr,
            typePtr, sub_typePtr, lengthPtr, precisionPtr,
            scalePtr, nullablePtr ))
#endif
{
    SQLRETURN  retcode = SQL_ERROR;
    UWORD      sqlState = API_ODBC_00000; /* Success */
    SDWORD     nameLen = 0;

    API_TRACE( API_TR_ENTRY,PA20DESC_FN_SQLGETDESCREC, 0);
    API_TRACE( API_TR_HANDLE,"hdesc", &hdesc);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_PTR, "name", &name );
    API_TRACE( API_TR_SWORD, "bufferLen", &bufferLen);
    API_TRACE( API_TR_PTR, "strLenPtr", &strLenPtr);
    API_TRACE( API_TR_PTR, "typePtr", &typePtr);
    API_TRACE( API_TR_PTR, "sub_typePtr", &sub_typePtr);
    API_TRACE( API_TR_PTR, "lengthPtr", &lengthPtr);
    API_TRACE( API_TR_PTR, "precisionPtr", &precisionPtr);
    API_TRACE( API_TR_PTR, "scalePtr", &scalePtr);
    API_TRACE( API_TR_PTR, "nullablePtr", &nullablePtr);

#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_NAME, name, bufferLen, &nameLen ));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_NAME, name, bufferLen, &nameLen ));
#endif

    *strLenPtr = (SWORD) nameLen;
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_TYPE, typePtr, 0, NULL));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_TYPE, typePtr, 0, NULL));
#endif

    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
    if (*typePtr==SQL_DATETIME || *typePtr==SQL_INTERVAL) {
#ifdef _UNICODE_ODBC
      retcode = CALL_ODBC( SQLGetDescFieldW,
                           ( hdesc, recNo, SQL_DESC_DATETIME_INTERVAL_CODE, typePtr, 0, NULL));
#else
      retcode = CALL_ODBC( SQLGetDescField,
                           ( hdesc, recNo, SQL_DESC_DATETIME_INTERVAL_CODE, typePtr, 0, NULL));
#endif

        if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
            goto exit;
        }; /* if */
    }; /* if */
#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_LENGTH, lengthPtr, 0, NULL));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_LENGTH, lengthPtr, 0, NULL));
#endif

    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_PRECISION, precisionPtr, 0, NULL));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_PRECISION, precisionPtr, 0, NULL));
#endif

    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_SCALE, scalePtr, 0, NULL));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_SCALE, scalePtr, 0, NULL));
#endif

    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
#ifdef _UNICODE_ODBC
    retcode = CALL_ODBC( SQLGetDescFieldW,
                         ( hdesc, recNo, SQL_DESC_NULLABLE, nullablePtr, 0, NULL));
#else
    retcode = CALL_ODBC( SQLGetDescField,
                         ( hdesc, recNo, SQL_DESC_NULLABLE, nullablePtr, 0, NULL));
#endif

exit:
    if (sqlState!=API_ODBC_00000) { /* != SUCCESS */
        pa20PutError( hdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA20DESC_FN_SQLGETDESCREC, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE_LEN( API_TR_ODBC_STRING, "name", name, SQL_NTS );
    API_TRACE( API_TR_SWORD, "*strLenPtr", strLenPtr);
    API_TRACE( API_TR_SWORD, "*typePtr", typePtr);
    API_TRACE( API_TR_SWORD, "*sub_typePtr", sub_typePtr);
    API_TRACE( API_TR_SDWORD, "*lengthPtr", lengthPtr);
    API_TRACE( API_TR_SWORD, "*precisionPtr", precisionPtr);
    API_TRACE( API_TR_SWORD, "*scalePtr", scalePtr);
    API_TRACE( API_TR_SWORD, "*nullablePtr", nullablePtr);

    return retcode;
} /* SQLGetDescRec */


/* ODBC 3.0 */
#ifdef _UNICODE_ODBC
ODBC_PROC(SQLSetDescFieldW, (SQLHDESC    hdesc,
                             SQLSMALLINT recNo,
                             SQLSMALLINT fieldId,
                             SQLPOINTER  valuePtr,
                             SQLINTEGER  len ),
          ( hdesc, recNo, fieldId, valuePtr, len ))
#else
ODBC_PROC(SQLSetDescField, (SQLHDESC    hdesc,
                            SQLSMALLINT recNo,
                            SQLSMALLINT fieldId,
                            SQLPOINTER  valuePtr,
                            SQLINTEGER  len ),
          ( hdesc, recNo, fieldId, valuePtr, len ))
#endif
{
    SQLRETURN   retcode = SQL_ERROR;
    UWORD       sqlState = API_ODBC_00000; /* Success */
    tpa20Desc  *descPtr;
    UWORD       accessRights;
#ifdef _UNICODE_ODBC
    const tsp77encoding *nativeEncoding = sp77nativeUnicodeEncoding ();
#else
    const tsp77encoding *nativeEncoding = sp77encodingAscii;
#endif

    API_TRACE( API_TR_ENTRY,PA20DESC_FN_SQLSETDESCFIELD, 0);
    API_TRACE( API_TR_HANDLE,"hdesc", &hdesc);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_DESCTYPE, "fieldId", &fieldId);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);

    if (pa20VerifyDesc( hdesc ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        pa20_ResetError( hdesc );
        if (!pa20_IsValidFieldId( fieldId )) {
            retcode = SQL_ERROR;
            sqlState = API_ODBC_HY091; /* Invalid descriptor field id */
        } else {
            descPtr = (tpa20Desc*) hdesc;
            accessRights = pa20_GetAccessRights( fieldId,
                                                 pa20GetDescType( descPtr ) );
            if ( accessRights != API_ACCESS_READ_WRITE) {
                if ( pa20GetDescType( descPtr )==API_DESC_TYPE_IRD
                     && accessRights == API_ACCESS_READ_ONLY ) {
                    retcode = SQL_ERROR;
                    sqlState = API_ODBC_HY016; /* Cannot modify an
                                                * implementation
                                                * row descriptor */
                } else {
                    /* field is read-only or unused */
                    retcode = SQL_ERROR;
                    sqlState = API_ODBC_HY091; /* Invalid descriptor
                                                * field identifier */
                }; /* else */
            } else {
                /* try to set value */
                retcode = pa20SetDescField( descPtr,
                                            recNo,
                                            fieldId,
                                            valuePtr,
                                            len,
                                            &sqlState,
                                            nativeEncoding);
            }; /* else */
        }; /* else */
    }; /* else */

    if (sqlState!=API_ODBC_00000) { /* != SUCCESS */
        pa20PutError( hdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA20DESC_FN_SQLSETDESCFIELD, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);

    return retcode;
} /* SQLSetDescField */


#ifndef _UNICODE_ODBC
/* ODBC 3.0 */
ODBC_PROC(SQLSetDescRec, ( SQLHDESC     hdesc,
                           SQLSMALLINT  recNo,
                           SQLSMALLINT  type,
                           SQLSMALLINT  subType,
                           SQLLEN       length,
                           SQLSMALLINT  precision,
                           SQLSMALLINT  scale,
                           SQLPOINTER   dataPtr,
                           SQLLEN      *lenPtr,
                           SQLLEN      *indicatorPtr),
          ( hdesc, recNo, type, subType, length, precision, scale,
            dataPtr, lenPtr, indicatorPtr ))
{
    SQLRETURN  retcode = SQL_ERROR;
    UWORD      sqlState = API_ODBC_00000; /* Success */

    API_TRACE( API_TR_ENTRY,PA20DESC_FN_SQLSETDESCREC, 0);
    API_TRACE( API_TR_HANDLE,"hdesc", &hdesc);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_SWORD, "type", &type);
    API_TRACE( API_TR_SWORD, "sub_type", &subType);
    API_TRACE( API_TR_SDWORD, "length", &length);
    API_TRACE( API_TR_SWORD, "precision", &precision);
    API_TRACE( API_TR_SWORD, "scale", &scale);
    API_TRACE( API_TR_PTR, "dataPtr", &dataPtr );
    API_TRACE( API_TR_PTR, "lenPtr", &lenPtr);
    API_TRACE( API_TR_PTR, "indicatorPtr", &indicatorPtr);

    if (pa20VerifyDesc( hdesc ) != API_OK) {
        retcode = SQL_INVALID_HANDLE;
        goto exit;
    }; /* if */
    pa20_ResetError( hdesc );

    /* Reihenfolge Konsistenz ... !!! */
    retcode =  CALL_ODBC( SQLSetDescField,
                         ( hdesc, recNo, SQL_DESC_TYPE,
                           SQLPOINTER_CAST (type), 0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */

    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc,
                           recNo,
                           SQL_DESC_DATETIME_INTERVAL_CODE,
                           SQLPOINTER_CAST (subType),
                           0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */

    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc,
                           recNo,
                           SQL_DESC_OCTET_LENGTH,
                           SQLPOINTER_CAST (length),
                           0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */

    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc,
                           recNo,
                           SQL_DESC_PRECISION,
                           SQLPOINTER_CAST (precision),
                           0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */

    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc, recNo, SQL_DESC_SCALE,
                           SQLPOINTER_CAST (scale), 0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */

    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc, recNo, SQL_DESC_DATA_PTR, dataPtr, 0 ));
    if (retcode!=SQL_SUCCESS && retcode!=SQL_SUCCESS_WITH_INFO) {
        goto exit;
    }; /* if */
    retcode = CALL_ODBC( SQLSetDescField,
                         ( hdesc,
                           recNo,
                           SQL_DESC_INDICATOR_PTR,
                           &indicatorPtr,
                           0 ));

exit:
    if (sqlState!=API_ODBC_00000) { /* != SUCCESS */
        pa20PutError( hdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, PA20DESC_FN_SQLSETDESCREC, 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE( API_TR_SDWORD, "*lenPtr", lenPtr);
    API_TRACE( API_TR_SDWORD, "*indicatorPtr", indicatorPtr);

    return retcode;
} /* SQLSetDescRec */


/*!**********************************************************************

  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/


SQLRETURN pa20AllocExplicitDesc( SQLHDBC   hdbc,
                                 SQLHDESC *phdesc )
{
    SQLRETURN  retcode = SQL_SUCCESS;
    UWORD      sqlState    = API_ODBC_00000; /* Success */
    SQLHDESC   hdesc;
    tpa20Desc *descPtr;
    tpa40DBC  *dbcPtr;

    API_TRACE( API_TR_ENTRY, "pa20AllocExplicitDesc", 0);
    API_TRACE( API_TR_HANDLE,"hdbd", &hdbc);

    if (pa40VerifyDBC( hdbc ) != API_OK) { /* hdbc valid? */
        retcode = SQL_INVALID_HANDLE;
    } else if (phdesc == NULL) { /* NULL pointer? */
        retcode = SQL_ERROR;
        sqlState = API_ODBC_S1009; /* invaild argument value */
    } else {
        switch (((tpa40DBC*) hdbc)->state) {
        case (API_DBC_CONNECTED): /* already connected? */
            hdesc = (SQLHDESC) apdallo( (DWORD) sizeof(tpa20Desc));
            if (hdesc == 0) {
                retcode = SQL_ERROR;
                *phdesc = SQL_NULL_HDESC;
                sqlState = API_ODBC_S1001; /* memory allocation failure */
            } else {
                pa20InitApplDesc( hdesc, hdbc, SQL_NULL_HSTMT );
                API_ASSERT( pa20VerifyDesc(hdesc) == API_OK );
                descPtr = (tpa20Desc*) hdesc;
                descPtr->allocType = SQL_DESC_ALLOC_USER;
                *phdesc = hdesc;
                /* insert into desc list of hdbc */
                dbcPtr = (tpa40DBC*) hdbc;
                pa40AddDesc( dbcPtr, descPtr );
            }; /* else */
            break;
        default:
            retcode = SQL_ERROR;
            sqlState = API_ODBC_08003; /* Connection does not exist */
            break;
        }; /* switch */
    }; /* else */

    if ( sqlState != API_ODBC_00000 ) { /* != SUCCESS */
        pa40PutError( hdbc, sqlState, NULL );
    }; /* if */

    API_TRACE( API_TR_EXIT, "pa20AllocExplicitDesc", 0 );
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );
    API_TRACE( API_TR_HANDLE, "*phdesc", phdesc );

    return retcode;
} /* pa20AllocExplicitDesc */


API_RETCODE pa20AllocLong( tpa20Desc *desc_ptr,
                           UWORD      num_attr )
{
    API_RETCODE retcode;
    tpa21DescRecord *desc_rec_ptr;
    UWORD i;

    API_TRACE(API_TR_ENTRY, "pa20AllocLong",0);
    API_TRACE(API_TR_UWORD, "num_attr", &num_attr);

    API_ASSERT_PTR( desc_ptr );
    API_ASSERT( desc_ptr->descType == API_DESC_TYPE_IRD
                || desc_ptr->descType == API_DESC_TYPE_IPD );
    API_ASSERT( num_attr < desc_ptr->allocRec );

    retcode = API_OK;
    for( i=1; i <= num_attr; i++ ) {
        desc_rec_ptr = pa20GetRecord( desc_ptr, i );
        API_ASSERT( desc_rec_ptr != NULL );
        if ( apgislong( desc_rec_ptr->type ) ) {
            API_TRACE( API_TR_HANDLE,"longhnld", &desc_rec_ptr->longHndl);
            if (desc_rec_ptr->longHndl)
                continue;
            else {
              /* extra space for termintors (cmp. vpa04tc, aptchar) */
                desc_rec_ptr->longHndl =
                    apdallo (desc_rec_ptr->esqLength + API_VSTRING_OFFSET + sizeof(tsp81_UCS2Char));
                if (!desc_rec_ptr->longHndl) {
                    retcode = API_NOT_OK;
                    break;
                }; /* if */
            }; /* else */
        }
        else {
            if (desc_rec_ptr->longHndl) {
                apdfree(desc_rec_ptr->longHndl);
                desc_rec_ptr->longHndl = NULL;
            }; /* if */
        }; /* else */
    }; /* for */

    API_TRACE(API_TR_EXIT, "pa20AllocLong",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return(retcode);
} /* pa20AllocLong */


API_RETCODE pa20AllocRecords( SQLHDESC  hdesc,
                              UWORD     numRecords )
{
    API_RETCODE apiRetcode;
    tpa20Desc *descPtr;
    UDWORD totalSize;
    UDWORD recNo;
    tpa21DescRecord *descRecPtr;

    API_TRACE( API_TR_ENTRY, "pa20AllocRecords", 0 );
    API_TRACE( API_TR_HANDLE, "hdesc", &hdesc );
    API_TRACE( API_TR_UWORD, "numRecords", &numRecords );

    API_ASSERT( pa20VerifyDesc( hdesc )==API_OK );
    API_ASSERT( numRecords > 0 );

    descPtr = (tpa20Desc*) hdesc;
    /* +1 for bookmark record */
    totalSize = sizeof( tpa21DescRecord ) * (numRecords+1);
    descPtr->records = apdallo( (DWORD) totalSize );
    if ( descPtr->records == NULL ) {
        apiRetcode = API_NOT_OK;
    } else {
        apiRetcode = API_OK;
        descPtr->allocRec = numRecords+1;
        /* init records */
        for ( recNo=0; recNo <= numRecords; recNo++ ) {
            descRecPtr = &descPtr->records[ (UDWORD) (recNo)];
            pa21InitDescRecord( descRecPtr, descPtr );
        }; /* for */
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20AllocRecords", 0 );
    API_TRACE( API_TR_PTR, "descPtr->records", &descPtr->records );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20AllocRecords */


tpa20Desc* pa20CloneDesc( tpa20Desc *inDesc )
{
    tpa20Desc *newDesc;

    API_TRACE( API_TR_ENTRY, "pa20CloneDesc", 0 );
    API_TRACE( API_TR_PTR, "inDesc", &inDesc );

    newDesc = apdallo( sizeof( tpa20Desc ) );
    if (newDesc != NULL) {
        if (pa20CopyDesc( inDesc, newDesc ) != API_OK ) {
            apdfree( newDesc );
            newDesc = NULL;
        }; /* if */
    }; /* if */

    API_TRACE( API_TR_EXIT, "pa20CloneDesc", 0 );
    API_TRACE( API_TR_PTR, "newDesc", &newDesc );

    return newDesc;
} /* pa20CloneDesc */



API_RETCODE pa20ColAttr( sqldatype       *sqlda_ptr,
                         tpa20Desc       *ird_ptr,
                         UWORD            column,
                         UWORD            description_type,
                         UCHAR           *description_ptr,
                         SWORD            max_description_size,
                         SWORD           *description_size_ptr,
                         SQLPOINTER      *description_num_ptr,
                         const tsp77encoding *destEncoding)
{
    API_RETCODE      api_retcode;
    UCHAR           *local_description_ptr = NULL;
    SDWORD           description_num = 0;
    SWORD            description_length = 0;
    tpa21DescRecord *desc_rec_ptr = NULL;
    UDWORD           odbcVersion = 0;
    SWORD            information_int = TRUE;  /* true if desciptor info.
                                               * is an integer value     */

    API_TRACE(API_TR_ENTRY,"pa20ColAttr", 0);
    API_TRACE(API_TR_PTR,"sqlda_ptr", &sqlda_ptr);
    API_TRACE(API_TR_PTR,"ird_ptr", &ird_ptr);
    API_TRACE(API_TR_UWORD,"column", &column);
    API_TRACE(API_TR_UWORD,"description_type", &description_type);
    API_TRACE(API_TR_PTR,"description_ptr", &description_ptr);
    API_TRACE(API_TR_PTR,"description_num_ptr", &description_num_ptr);
    API_TRACE(API_TR_SWORD,"max_description_size", &max_description_size);
    API_TRACE(API_TR_SWORD,"*description_size_ptr", description_size_ptr);
    API_TRACE(API_TR_SDWORD,"*description_num_ptr", description_num_ptr);

    API_ASSERT_PTR( ird_ptr );
    /* must be implemetation row descriptor */
    API_ASSERT( pa20GetDescType( ird_ptr ) == API_DESC_TYPE_IRD );

    api_retcode = API_OK;

    if (description_type == SQL_DESC_NAME ||
        description_type == SQL_DESC_BASE_COLUMN_NAME ||
        description_type == SQL_DESC_BASE_TABLE_NAME ||
        description_type == SQL_COLUMN_NAME ||
        description_type == SQL_DESC_LABEL ||
        description_type == SQL_COLUMN_TYPE_NAME ||
        description_type == SQL_COLUMN_TABLE_NAME ||
        description_type == SQL_COLUMN_OWNER_NAME ||
        description_type == SQL_COLUMN_QUALIFIER_NAME ||
        description_type == SQL_COLUMN_LABEL) {
		information_int = FALSE;
        /* cbDescMax, pcbDesc will */
		/* be ignored              */
    }
    /* get the desired entry   */
    if (description_type != SQL_COLUMN_COUNT) {
        API_ASSERT( column >= 0 );
        desc_rec_ptr = pa20GetRecord( ird_ptr, column );
        API_ASSERT( desc_rec_ptr != NULL );
    }
    API_TRACE(API_TR_SWORD, "type", &desc_rec_ptr -> type);
    /* get information in local*/
    /* local variables         */
    /* depending on description */
    /* type                    */
    pa10GetODBCVersion( SQL_HANDLE_DESC, (SQLHDESC) ird_ptr, &odbcVersion );
    switch (description_type) {
    case (SQL_DESC_COUNT): /* ODBC 3.0 */
    case (SQL_COLUMN_COUNT): {
        description_num = pa70NumCol( sqlda_ptr );
        break;
    }
    case (SQL_DESC_NAME): /* ODBC 3.0 */
    case (SQL_DESC_BASE_COLUMN_NAME):
    case (SQL_COLUMN_NAME):
        /* SQL_DESC_LABEL (ODBC 3.0) == */
    case (SQL_COLUMN_LABEL): {
        if (column > 0) {
            /* "regular" column */
            local_description_ptr = sqlda_ptr -> sqlvar[ column -1].colname;
            description_length = sizeof(sqlda_ptr -> sqlvar[column -1].colname);
            description_length -= sp77encodingUTF8->countPadChars (local_description_ptr,
                                                                   description_length,
                                                                   ' ');
            /*            while ( (description_length > 0)
                    && (*(local_description_ptr + description_length-1) == ' '))
                    description_length--; */
            break;
        } else {
            /* bookmark column has no label */
            local_description_ptr = (UCHAR*) "";
        }; /* else */
    }
    /* SQL_DESC_CONCISE_TYPE (ODBC 3.0) == SQL_COLUMN_TYPE */
    case (SQL_DESC_TYPE): {}
    case (SQL_COLUMN_TYPE): {
        description_num = pa20_GetConciseType( odbcVersion, desc_rec_ptr -> type );
        break;
    }
    case (SQL_DESC_LENGTH): { /* ODBC 3.0 */
        description_num = (SDWORD) pa20_GetDescLength( desc_rec_ptr );
        break;
    }
    case (SQL_COLUMN_LENGTH): {
        description_num = (SDWORD) pa20_GetSQLLength( desc_rec_ptr );
        break;
    }

    case (SQL_DESC_OCTET_LENGTH): { /* ODBC 3.0 */  /* PTS ... */
        description_num = (SDWORD) pa20_GetSQLLength( desc_rec_ptr );
        break;
    }

    case (SQL_DESC_PRECISION): /* ODBC 3.0 */
        /* ... differnet in ODBC 3.0 */
    case (SQL_COLUMN_PRECISION): {
        description_num = pa20_GetSQLPrec( desc_rec_ptr );
        break;
    }
    case (SQL_DESC_SCALE): /* ODBC 3.0 */
        /* ... differnet in ODBC 3.0 */
    case (SQL_COLUMN_SCALE): {
        description_num = desc_rec_ptr -> esqScale;
        break;
    }
    /* SQL_DESC_DISPLAY_SIZE (ODBC 3.0) == */
    case (SQL_COLUMN_DISPLAY_SIZE): {
        description_num = pa20_GetDisplaySize( desc_rec_ptr );
        break;
    }
    case (SQL_DESC_NULLABLE): /* ODBC 3.0 */
    case (SQL_COLUMN_NULLABLE): {
        description_num = desc_rec_ptr -> nullable;
        break;
    }
    /* SQL_DESC_UNSIGNED (ODBC 3.0) == */
    case (SQL_COLUMN_UNSIGNED): {
        description_num = API_FALSE;
        break;
    }
    /* SQL_FIXED_PREC_SCALE (ODBC 3.0) == */
    case (SQL_COLUMN_MONEY): {
        description_num = API_FALSE;
        break;
    }
    /* SQL_DESC_TYPE_NAME (ODBC 3.0) == */
    case (SQL_COLUMN_TYPE_NAME): {
      int       old;
      tpa40DBC *dbc_block_ptr;
      dbc_block_ptr = (tpa40DBC*) ird_ptr->parentDBC;

      if (dbc_block_ptr->kernelVersionString[0] < '7'  ||
          (dbc_block_ptr->kernelVersionString[0] == '7'  &&
           dbc_block_ptr->kernelVersionString[1] == '0'  &&
           dbc_block_ptr->kernelVersionString[2] <= '5'))
        old = TRUE;
      else
        old = FALSE;

      local_description_ptr = pa20_GetTypeName( desc_rec_ptr, old );
        description_length = (SWORD) API_STRLEN( local_description_ptr );
        break;
    }
    /* SQL_DESC_UPDATEABLE (ODBC 3.0) == */
    case (SQL_COLUMN_UPDATABLE): {
        description_num = SQL_ATTR_READWRITE_UNKNOWN;
        API_TRACE(API_TR_UPDATEABLE, "updateable", &description_num );
        break;
    }
    /* SQL_DESC_AUTO_UNIQUE_VALUE (ODBC 3.0) == */
    case (SQL_COLUMN_AUTO_INCREMENT): {
        description_num = API_FALSE;         /* not supported by ESQ   */
        break;
    }
    /* SQL_DESC-CASE_SENSITIVE (ODBC 3.0) == */
    case (SQL_COLUMN_CASE_SENSITIVE): {
        description_num = pa20_IsCaseSensitive( desc_rec_ptr );
        break;
    }
    /* SQL_DESC_SEARCHABLE (ODBC 3.0) == */
    case (SQL_COLUMN_SEARCHABLE): {
        description_num = pa20_IsSearchable( desc_rec_ptr );
        break;
    }
    /* SQL_DESC_TABLE_NAME (ODBC 3.0) == */
    /* case (SQL_COLUMN_TABLE_NAME): { } handled in vpa10ac SQLColAttributes */
    case (SQL_DESC_BASE_TABLE_NAME):
    case (SQL_COLUMN_OWNER_NAME): {}
    case (SQL_COLUMN_QUALIFIER_NAME): {
        local_description_ptr = (UCHAR FAR *)API_EMPTY_STRING;
        description_length = (SWORD) API_STRLEN( API_EMPTY_STRING );
        break;
    }
    default: {
        api_retcode = API_NOT_OK;
        break;
    }
    }

    if (api_retcode == API_OK) {
        if (information_int == TRUE) {       /* description and size */
            if (description_num_ptr != NULL) { /* will be ignored     */
                *((SQLLEN*)description_num_ptr) = description_num;     /* cmp. sql.h, SQLColAttribute */
            }
        }
        else {
            if (description_ptr != NULL) {
              tsp00_Uint4 destBytesWritten;

              api_retcode =
                pa80convertString (destEncoding,
                                   description_ptr,
                                   max_description_size,
                                   &destBytesWritten,
                                   /*                                   sp77encodingAscii,*/
                                   sp77encodingUTF8,   /* colname etc */
                                   local_description_ptr,
                                   description_length);

              description_length = (SWORD) destBytesWritten;
            }

              /*
                if (description_length < max_description_size) {
                    API_MEMCPY( description_ptr,
                                local_description_ptr,
                                description_length );
                    description_ptr [ description_length ] = 0;
                    / make it a C string  /
                }
                else {                         / too small           /
                    api_retcode = API_TRUNCATE;
                    API_MEMCPY( description_ptr,
                                local_description_ptr,
                                max_description_size);
                }
*/
            if (description_size_ptr != NULL) {
                *description_size_ptr = description_length;
            }
        }
    } /* of if API_OK */

    API_TRACE(API_TR_EXIT,"pa20ColAttr",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode", &api_retcode);
    API_TRACE(API_TR_STRING,"*description_ptr", description_ptr);
    API_TRACE(API_TR_SWORD,"*description_size_ptr", description_size_ptr);
    API_TRACE(API_TR_SDWORD,"*description_num_ptr", description_num_ptr);

    return (api_retcode);
} /* pa20ColAttr */


API_RETCODE pa20CopyDesc( tpa20Desc *inDesc, tpa20Desc *outDesc )
{
    API_RETCODE      apiRetcode = API_OK;
    UDWORD           allocSize;
    UWORD            iRec = 0;
    tpa21DescRecord *outRec = NULL;
    tpa30DiagArea    diagArea = NULL;

    API_TRACE( API_TR_ENTRY, "pa20CopyDesc", 0 );
    API_TRACE( API_TR_PTR, "inDesc", &inDesc );
    API_TRACE( API_TR_PTR, "outDesc", &outDesc );

    API_ASSERT_PTR( inDesc );
    API_ASSERT_PTR( outDesc );

    /* free record area of target desc */
    if (outDesc->records != NULL ) {
        apdfree( outDesc->records );
        outDesc->records = NULL;
    }; /* if */
    /* keep diagArea of outDesc */
    diagArea = outDesc->diagArea;
    /* copy descriptor */
    API_MEMCPY( outDesc, inDesc, sizeof( tpa20Desc ) );
    if (inDesc->count >= 0) {
        /* copy records; copy only the used records + bookmark record */
        allocSize = sizeof(tpa21DescRecord) * (inDesc->count+1);
        outDesc->records = apdallo( allocSize );
        if (outDesc->records == NULL) {
            apiRetcode = API_NOT_OK;
        } else {
            API_MEMCPY( outDesc->records, inDesc->records, allocSize );
            outDesc->allocRec = inDesc->count+1;
            outDesc->count    = inDesc->count;
            /* set parentDesc in records of outDesc */
            for (iRec = 0; iRec < outDesc->allocRec; iRec++) {
                outRec = pa20GetRecord( outDesc, iRec );
                if (outRec != NULL) {
                    outRec->parentDesc = outDesc;
                }; /* if */
            }; /* for */
        }; /* else */
        outDesc->diagArea = diagArea;
        /* pa30InitDiagArea( &outDesc->diagArea ); */
    } else {
        outDesc->records = NULL;
        outDesc->allocRec = 0;
        outDesc->count    = 0;
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20CopyDesc", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20CopyDesc */


API_RETCODE pa20DeleteDesc( tpa20Desc *descPtr )
{
    API_RETCODE apiRetcode = API_OK;

    API_TRACE( API_TR_ENTRY, "pa20DeleteDesc", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    API_ASSERT_PTR( descPtr );
    API_ASSERT( pa20VerifyDesc( descPtr ) == API_OK );

    /* ARD/APD: associated stmts must revert to default handle for explicit
     * allocated desc */
    if ( descPtr->allocType == SQL_DESC_ALLOC_USER
         && descPtr->parentStmt != SQL_NULL_HSTMT ) {
        pa60SetDefaultDesc( descPtr->parentStmt, descPtr );
    } else { /* SQL_DESC_ALLOC_AUTO */
        /* free long handles */
        apiRetcode = pa20FreeLongHandles( descPtr );
    };
    /* free records */
    if ((apiRetcode == API_OK) && pa20_FreeRecords( descPtr ) != API_OK) {
        apiRetcode = API_NOT_OK;
    } else if (pa20_FreeRecords( descPtr ) != API_OK) { /* free records */
        apiRetcode = API_NOT_OK;
        /* free error-blocks:  */
    } else if (pa30FreeDiagArea( &descPtr->diagArea ) != API_OK) {
        apiRetcode = API_NOT_OK;
    } else if (pa30DeleteDiagArea( &descPtr->diagArea ) != API_OK) {
        apiRetcode = API_NOT_OK;
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20DeleteDesc", 0 );
    API_TRACE( API_TR_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20DeleteDesc */


API_RETCODE pa20DescribeCol( sqldatype      *sqlda_ptr,
                             tpa20Desc      *ird_ptr,
                             UWORD           column,
                             UCHAR          *name_ptr,
                             SWORD           max_name_size,
                             SWORD          *name_size_ptr,
                             SWORD          *datatype_ptr,
                             SQLULEN        *precision_ptr,
                             SWORD          *scale_ptr,
                             SWORD          *nullable_ptr,
                             const tsp77encoding *destEncoding)
{
    API_RETCODE      api_retcode;
    SWORD            sql_type;
    UDWORD           length;
    SWORD            scale;
    UDWORD           precision;
    SWORD            name_length;
    UCHAR           *local_name_ptr;
    tpa21DescRecord *desc_rec_ptr;

    API_TRACE(API_TR_ENTRY,"pa20DescribeCol",0);
    API_TRACE(API_TR_PTR,"sqlda_ptr",&sqlda_ptr);
    API_TRACE(API_TR_PTR,"ird_ptr",&ird_ptr);
    API_TRACE(API_TR_UWORD,"column",&column);
    API_TRACE(API_TR_PTR,"name_ptr",&name_ptr);
    API_TRACE(API_TR_SWORD,"max_name_size",&max_name_size);
    API_TRACE(API_TR_PTR,"name_size_ptr",&name_size_ptr);
    API_TRACE(API_TR_PTR,"datatype_ptr",&datatype_ptr);
    API_TRACE(API_TR_PTR,"precision_ptr",&precision_ptr);
    API_TRACE(API_TR_PTR,"scale_ptr",&scale_ptr);
    API_TRACE(API_TR_PTR,"nullable_ptr",&nullable_ptr);

    api_retcode = API_OK;

    API_ASSERT_PTR( ird_ptr );
    /* must be implemetation row descriptor */
    API_ASSERT( pa20GetDescType( ird_ptr ) == API_DESC_TYPE_IRD );
    API_ASSERT( column >= 0 );
    desc_rec_ptr = pa20GetRecord( ird_ptr, column );
    API_ASSERT_PTR( desc_rec_ptr );

    sql_type = desc_rec_ptr -> type;
    if (datatype_ptr != NULL) {
        *datatype_ptr = sql_type;
    }
    if (nullable_ptr != NULL) {
        *nullable_ptr = desc_rec_ptr -> nullable;
    }
    if (column == 0) {
        /* bookmark column */
        length = 4; /* only support 32 bit bookmarks yet */
        scale  = 0;
        precision = 0;
    } else {
        length   = desc_rec_ptr -> esqLength;
        scale    = desc_rec_ptr -> esqScale;
        precision = desc_rec_ptr -> esqPrecision;
        if (sql_type == SQL_WCHAR || sql_type == SQL_WVARCHAR || sql_type == SQL_WLONGVARCHAR)
          precision /= sizeof (tsp81_UCS2Char);
        local_name_ptr = sqlda_ptr -> sqlvar[column-1].colname;
        name_length = sizeof(sqlda_ptr -> sqlvar[column-1].colname);

        /* "cut off" trailing spaces */
        name_length -= sp77encodingUTF8->countPadChars (local_name_ptr, name_length, ' ');

        if (name_size_ptr != NULL) {
            *name_size_ptr = name_length;
        }
        if (name_length >= max_name_size) {
            name_length = max_name_size > 0 ? max_name_size-1 : 0;
            api_retcode = API_TRUNCATE;
        }
        if (name_ptr != NULL && max_name_size > 0) {
          if (destEncoding == sp77encodingAscii) {
            API_MEMCPY( name_ptr, local_name_ptr, name_length );
            name_ptr [ name_length ] = 0;
          }
          else  {
            tsp00_Uint4           parsedBytes, destBytesWritten;
            tsp78ConversionResult cnvResult;

            cnvResult = sp78convertString (destEncoding,
                                           name_ptr, name_length * sizeof (tsp81_UCS2Char),
                                           &destBytesWritten,
                                           FALSE,
                                           sp77encodingUTF8,
                                           local_name_ptr,
                                           name_length,
                                           &parsedBytes);
            if (cnvResult == sp78_TargetExhausted || cnvResult == sp78_Ok) {
              /* append always terminator */
              API_MEMCPY (name_ptr+destBytesWritten,
                          destEncoding->charTable->terminator.bytes,
                          destEncoding->charTable->terminator.byteCount);
            }

            if (cnvResult == sp78_TargetExhausted) {  /* should not happen: len = max - 1 */
              if (name_size_ptr != NULL)
                *name_size_ptr = (SQLSMALLINT) destBytesWritten / sizeof (tsp81_UCS2Char);
              api_retcode = API_TRUNCATE;
            }
            else if (cnvResult != sp78_Ok)
              api_retcode = API_NOT_OK;
          }
        }
    }; /* else */
    if (precision_ptr != NULL) {
        switch (sql_type) {
        case (SQL_UNICODE_LONGVARCHAR): {}
        case (SQL_LONGVARCHAR): {}
        case (SQL_LONGVARBINARY): {
            *precision_ptr = SAPDB_MAX_INT4;  /* PTS 1119281 */
            break;
        }
        default: {
            *precision_ptr = precision;
            break;
        }
        }; /* switch */
    }; /* if */
    if (scale_ptr != NULL) {
        *scale_ptr = scale;
    }

    API_TRACE(API_TR_EXIT,"pa20DescribeCol",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    if (name_ptr != NULL) {
        API_TRACE(API_TR_STRING,"*name_ptr",name_ptr);
    }
    if (name_size_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*name_size_ptr",name_size_ptr);
    }
    if (datatype_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*datatype_ptr",datatype_ptr);
    }
    if (precision_ptr != NULL) {
        API_TRACE(API_TR_UDWORD,"*precision_ptr",precision_ptr);
    }
    if (scale_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*scale_ptr",scale_ptr);
    }
    if (nullable_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*nullable_ptr",nullable_ptr);
    }

    return (api_retcode);
} /* pa20DescribeCol */


API_RETCODE pa20DescribeParam( tpa20Desc *ipd_ptr,
                               UWORD      column,
                               SWORD     *datatype_ptr,
                               SQLULEN   *precision_ptr,
                               SWORD     *scale_ptr,
                               SWORD     *nullable_ptr,
                               UDWORD     passthrough )
{
    API_RETCODE api_retcode;
    SWORD sql_type;
    UDWORD length;
    SWORD scale;
    UDWORD precision;
    tpa21DescRecord *impl_rec_ptr;

    API_TRACE(API_TR_ENTRY,"pa20DescribeParam",0);
    API_TRACE(API_TR_PTR, "ipd_ptr", &ipd_ptr);
    API_TRACE(API_TR_UWORD,"column",&column);
    API_TRACE(API_TR_PTR,"datatype_ptr",&datatype_ptr);
    API_TRACE(API_TR_PTR,"precision_ptr",&precision_ptr);
    API_TRACE(API_TR_PTR,"scale_ptr",&scale_ptr);
    API_TRACE(API_TR_PTR,"nullable_ptr",&nullable_ptr);

    API_ASSERT_PTR( ipd_ptr );
    API_ASSERT( pa20GetDescType( ipd_ptr ) == API_DESC_TYPE_IPD );

    api_retcode = API_OK;

    impl_rec_ptr = pa20GetRecord( ipd_ptr, column );

    sql_type = impl_rec_ptr -> type;
    length   = impl_rec_ptr -> esqLength;
    scale    = impl_rec_ptr -> esqScale;
    precision = impl_rec_ptr -> esqPrecision;

    if (datatype_ptr != NULL) {
        *datatype_ptr = sql_type;
        if ( passthrough == TRUE ) {
            switch( impl_rec_ptr -> esqIO ) {
            case CPR_INFO_OUTPUT:
                /* let the application know that this is an output param */
                *datatype_ptr *= SQL_OUTPRM_FACTOR;
                break;
            case CPR_INFO_INOUT:
                /* here we use an offset instead of a factor because we
                 * only have a SWORD. */
                if (*datatype_ptr > 0) {
                    *datatype_ptr += SQL_INOUTPRM_OFFSET;
                } else if (*datatype_ptr < 0) {
                    *datatype_ptr -= SQL_INOUTPRM_OFFSET;
                }; /* else */
                break;
            default: /* cpr_info_input */
                /* orinal value */
                break;
            }; /* switch */
        }; /* if */
    }
    if (precision_ptr != NULL) {
        *precision_ptr = precision;
    }
    if (scale_ptr != NULL) {
        *scale_ptr = scale;
    }
    if (nullable_ptr != NULL) {
        *nullable_ptr = impl_rec_ptr -> nullable;
    }

    API_TRACE(API_TR_EXIT,"pa20DescribeParam",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    if (datatype_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*datatype_ptr",datatype_ptr);
    }
    if (precision_ptr != NULL) {
        API_TRACE(API_TR_UDWORD,"*precision_ptr",precision_ptr);
    }
    if (scale_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*scale_ptr",scale_ptr);
    }
    if (nullable_ptr != NULL) {
        API_TRACE(API_TR_SWORD,"*nullable_ptr",nullable_ptr);
    }
    return (api_retcode);
} /* pa20DescribeParam */


SQLRETURN pa20FreeDesc( SQLHDESC hdesc )
{
    SQLRETURN  retcode = SQL_SUCCESS;
    UWORD      sqlState = API_ODBC_00000;
    tpa20Desc *descPtr;

    PA30PROTECTTHREAD();

    API_TRACE( API_TR_ENTRY, "pa20FreeDesc", 0 );
    API_TRACE( API_TR_HANDLE, "hdesc", &hdesc );

    if ( pa20VerifyDesc(hdesc) != API_OK ) {
        retcode = SQL_INVALID_HANDLE;
    } else {
        descPtr = (tpa20Desc*) hdesc;
        if (descPtr->allocType != SQL_DESC_ALLOC_USER) {
            /* user allocated descriptors are deleted when the connection
             * handle is deleted */
            if (pa20DeleteDesc( descPtr ) != API_OK ) {
                sqlState = API_ODBC_S1000;
            } else {
                /* set invalid type. So next operation on this freed
                 * handle will return SQL_INVALID_HANDLE (or crashes) */
                descPtr -> handleType = API_INVALID_HANDLE_TYPE;
                apdfree( (void*) hdesc );
            }; /* else */
        }; /* if */
    }; /* else */

    if ( sqlState != API_ODBC_00000 ) { /* != SUCCESS */
        retcode = SQL_ERROR;
        pa20PutError( hdesc, sqlState );
    }; /* if */

    API_TRACE( API_TR_EXIT, "pa20FreeDesc", 0 );
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );

    PA30UNPROTECTTHREAD();
    return (retcode);
} /* pa20FreeDesc */


API_RETCODE pa20FreeCopiedDesc( tpa20Desc *descPtr )
{
    API_RETCODE apiRetcode = API_OK;

    API_TRACE( API_TR_ENTRY, "pa20FreeCopiedDesc", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    API_ASSERT( pa20VerifyDesc( descPtr ) == API_OK );

    /* free records */
    if (pa20_FreeRecords( descPtr ) != API_OK) {
        apiRetcode = API_NOT_OK;
    } else if (pa30FreeDiagArea( &descPtr->diagArea ) != API_OK) {
        apiRetcode = API_NOT_OK;
    } else if (pa30DeleteDiagArea( &descPtr->diagArea ) != API_OK) {
        apiRetcode = API_NOT_OK;
    } else {
        /* free descriptor */
        apdfree( descPtr );
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20FreeCopiedDesc", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20FreeCopiedDesc */


API_RETCODE pa20FreeLongHandles( tpa20Desc *descPtr )
{
    API_RETCODE apiRetcode = API_OK;
    UWORD recNo;
    tpa21DescRecord *recPtr;

    API_TRACE( API_TR_ENTRY, "pa20FreeLongHandles", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    API_ASSERT_PTR( descPtr );
    recPtr = descPtr->records;
	for ( recNo = 0; recNo < descPtr->allocRec; recNo++ ) {
	   if( recPtr[ recNo ].longHndl != API_NULL_HANDLE ) {
	      apdfree( recPtr[ recNo ].longHndl );
	      recPtr[ recNo ].longHndl = API_NULL_HANDLE;
	   }; /* if */
	}; /* for */

    API_TRACE( API_TR_EXIT, "pa20FreeLongHandles", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20FreeLongHandles */


SQLRETURN pa20GetDescField( tpa20Desc *descPtr,
                            SWORD      recNo,
                            SWORD      fieldId,
                            PTR        valuePtr,
                            SDWORD     len,
                            SDWORD    *lenPtr,
                            UWORD     *sqlStatePtr,
                            const tsp77encoding *destEncoding )
{
    SQLRETURN        retcode = SQL_SUCCESS;
    tpa21DescRecord *recPtr;
    UWORD            descType;
    sqldatype       *sqldaPtr;
    tpa60Stmt       *parentStmt = NULL;
    UDWORD           odbcVersion = 0;
    UDWORD           byteLen;

    API_TRACE( API_TR_ENTRY, "pa20GetDescField", 0);
    API_TRACE( API_TR_PTR, "descPtr", &descPtr);
    API_TRACE( API_TR_SWORD, "recNo", &recNo);
    API_TRACE( API_TR_SWORD, "fieldId", &fieldId);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);
    API_TRACE( API_TR_PTR, "lenPtr", &lenPtr );

    API_ASSERT_PTR( descPtr );

    descType = pa20GetDescType( descPtr );
    pa10GetODBCVersion( SQL_HANDLE_DESC, (SQLHDESC) descPtr, &odbcVersion );
    switch (fieldId) {
        /* Header Fields */
    case SQL_DESC_ALLOC_TYPE:
        *((SWORD*) valuePtr) = descPtr->allocType;
        break;
    case SQL_DESC_ARRAY_SIZE:
        if (descType == API_DESC_TYPE_AD) {
            *((UDWORD*) valuePtr) = descPtr->header.arraySize;
        }; /* if */
        break;
    case SQL_DESC_ARRAY_STATUS_PTR:
        *((UWORD**) valuePtr) = descPtr->header.arrayStatusPtr;
        break;
    case SQL_DESC_BIND_OFFSET_PTR:
        if (descType == API_DESC_TYPE_AD) {
            *((SQLLEN**) valuePtr) = descPtr->header.bindOffsetPtr;
        }; /* if */
        break;
    case SQL_DESC_BIND_TYPE:
        if (descType == API_DESC_TYPE_AD) {
            *((SDWORD*) valuePtr) = descPtr->header.bindType;
        }; /* if */
        break;
    case SQL_DESC_COUNT:
        *((SWORD*) valuePtr) = descPtr->count;
        break;
    case SQL_DESC_ROWS_PROCESSED_PTR:
        if ( descType == API_DESC_TYPE_IRD
             || descType == API_DESC_TYPE_IPD) {
            *((UDWORD**) valuePtr) = descPtr->header.rowsProcessedPtr;
        }; /* if */
        break;
    default: /* record field */
        if (recNo > descPtr->count) {
            retcode = SQL_NO_DATA;
        } else {
            recPtr = pa20GetRecord( descPtr, recNo );
            if (recPtr==NULL) {
                retcode = SQL_NO_DATA;
            } else {
                sqldaPtr = NULL;
                if (descPtr->parentStmt != API_NULL_HANDLE) {
                    parentStmt = (tpa60Stmt*) descPtr->parentStmt;
                    if (descType == API_DESC_TYPE_IRD) {
                        sqldaPtr =
                            (sqldatype*) parentStmt->output_sqlda_handle;
                    } else if (descType == API_DESC_TYPE_IPD) {
                        sqldaPtr =
                            (sqldatype*) parentStmt->input_sqlda_handle;
                    }; /* if */
                }; /* if */
                switch (fieldId) {
                case SQL_DESC_AUTO_UNIQUE_VALUE:
                    if (descType == API_DESC_TYPE_IRD) {
                        *((SDWORD*) valuePtr) = SQL_FALSE;
                    }; /* if */
                    break;
                case SQL_DESC_BASE_COLUMN_NAME:
                    if (descType == API_DESC_TYPE_IRD && sqldaPtr != NULL) {
                        retcode =
                          pa80CopyFixedStringEncoding (destEncoding,
                                                       (char*) valuePtr,
                                                       len,
                                                       NULL,          /* char count */
                                                       &byteLen,      /* byte count */
                                                       sqldaPtr->sqlvar[recNo-1].colname,
                                                       sizeof( sqldaPtr->sqlvar[recNo-1].colname ),
                                                       sqlStatePtr );
                        if (lenPtr != NULL)
                          *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_BASE_TABLE_NAME:
                    if (descType == API_DESC_TYPE_IRD && sqldaPtr != NULL) {
                        retcode =
                          pa80CopyStringEncoding (destEncoding,
                                                  (char*) valuePtr,
                                                  len,
                                                  NULL,          /* char count */
                                                  &byteLen,      /* byte count */
                                                  "",
                                                  sqlStatePtr );
                        if (lenPtr != NULL)
                          *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_CASE_SENSITIVE:
                    if ( descType == API_DESC_TYPE_IPD
                         || descType == API_DESC_TYPE_IRD) {
                        *((SDWORD*) valuePtr) = pa20_IsCaseSensitive( recPtr );
                    }; /* if */
                    break;
                case SQL_DESC_CATALOG_NAME:
                    if ( descType == API_DESC_TYPE_IRD ) {
                      retcode =
                        pa80CopyStringEncoding (destEncoding,
                                                (char*) valuePtr,
                                                len,
                                                NULL,          /* char count */
                                                &byteLen,      /* byte count */
                                                "",
                                                sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_CONCISE_TYPE:
                    *((SWORD*) valuePtr) = pa20_GetConciseType( odbcVersion,
                                                                recPtr->type );
                    break;
                case SQL_DESC_DATA_PTR:
                    if ( descType == API_DESC_TYPE_AD ) {
                        *((PTR*) valuePtr) = recPtr->dataPtr;
                    }; /* if */
                    break;
                case SQL_DESC_DATETIME_INTERVAL_CODE:
                    *((SWORD*) valuePtr) =
                        pa20GetDateTimeIntervalCode( descType, recPtr );
                    break;
                case SQL_DESC_DATETIME_INTERVAL_PRECISION:
                    *((SDWORD*) valuePtr) = recPtr->datetimeIntervalPrecision;
                    break;
                case SQL_DESC_DISPLAY_SIZE:
                    if ( descType == API_DESC_TYPE_IRD ) {
                        *((SDWORD*) valuePtr) = pa20_GetDisplaySize( recPtr );
                    }; /* if */
                    break;
                case SQL_DESC_FIXED_PREC_SCALE:
                    if (descType == API_DESC_TYPE_IRD
                        || descType == API_DESC_TYPE_IPD) {
                        *((SWORD*) valuePtr) = apgisnum( recPtr->type );
                    }; /* if */
                    break;
                case SQL_DESC_INDICATOR_PTR:
                    if (descType == API_DESC_TYPE_AD) {
                        *((SQLLEN**) valuePtr) = recPtr->indicatorPtr;
                    }; /* if */
                    break;
                case SQL_DESC_LABEL:
                    if (descType == API_DESC_TYPE_IRD && sqldaPtr != NULL) {
                      retcode =
                        pa80CopyFixedStringEncoding (destEncoding,
                                                     (char*) valuePtr,
                                                     len,
                                                     NULL,          /* char count */
                                                     &byteLen,      /* byte count */
                                                     sqldaPtr->sqlvar[recNo-1].colname,
                                                     sizeof( sqldaPtr->sqlvar[recNo-1].colname ),
                                                     sqlStatePtr );
                        if (lenPtr != NULL)
                          *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_LENGTH:
                    if ( descType == API_DESC_TYPE_IRD
                         || descType == API_DESC_TYPE_IPD ) {
                        *((UDWORD*) valuePtr) = pa20_GetSQLLength( recPtr );
                    } else {
                        *((UDWORD*) valuePtr) =
                            pa20_GetCLength( recPtr );
                    }; /* else */
                    break;
                case SQL_DESC_LITERAL_PREFIX:
                case SQL_DESC_LITERAL_SUFFIX:
                    if (descType == API_DESC_TYPE_IRD) {
                      retcode =
                        pa80CopyStringEncoding (destEncoding,
                                                (char*) valuePtr,
                                                len,
                                                NULL,          /* char count */
                                                &byteLen,      /* byte count */
                                                "",
                                                sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_LOCAL_TYPE_NAME:
                    if ( descType == API_DESC_TYPE_IPD
                         || descType == API_DESC_TYPE_IRD) {
                        /* we have no local type name
                         * so return empty string */
                      retcode =
                        pa80CopyStringEncoding (destEncoding,
                                                (char*) valuePtr,
                                                len,
                                                NULL,          /* char count */
                                                &byteLen,      /* byte count */
                                                "",
                                                sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_NAME:
                    if ( descType == API_DESC_TYPE_IRD
                         && sqldaPtr != NULL ) {
                      retcode =
                        pa80CopyFixedStringEncoding (destEncoding,
                                                     (char*) valuePtr,
                                                     len,
                                                     NULL,          /* char count */
                                                     &byteLen,      /* byte count */
                                                     sqldaPtr->sqlvar[recNo-1].colname,
                                                     sizeof( sqldaPtr->sqlvar[recNo-1].colname ),
                                                     sqlStatePtr );
                        if (lenPtr != NULL)
                          *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_NULLABLE:
                    *((SWORD*) valuePtr) = recPtr->nullable;
                    break;
                case SQL_DESC_NUM_PREC_RADIX:
                    *((SDWORD*) valuePtr) = pa20_GetNumPrecRadix( recPtr->type );
                    break;
                case SQL_DESC_OCTET_LENGTH: /* ... */
                    *((SQLLEN*) valuePtr) = recPtr->octetLength;
                    break;
                case SQL_DESC_OCTET_LENGTH_PTR:
                    if (descType == API_DESC_TYPE_AD ) {
                        *((SQLLEN**) valuePtr) = recPtr->octetLengthPtr;
                    }; /* if */
                    break;
                case SQL_DESC_PARAMETER_TYPE:
                    if (descType == API_DESC_TYPE_IPD ) {
                        *((SWORD*) valuePtr) = recPtr->parameterType;
                    }; /* if */
                    break;
                case SQL_DESC_PRECISION:
                    if (descType == API_DESC_TYPE_AD ) {
                        *((SWORD*) valuePtr) = recPtr->precision;
                    } else {
                        *((SWORD*) valuePtr) = pa20_GetSQLPrec( recPtr );
                    }; /* else */
                    break;
                case SQL_DESC_ROWVER:
                    *((SWORD*) valuePtr) = SQL_FALSE;
                    break;
                case SQL_DESC_SCALE:
                    *((SWORD*) valuePtr) = recPtr->scale;
                    break;
                case SQL_DESC_SCHEMA_NAME:
                    if (descType == API_DESC_TYPE_IRD) {
                      retcode =
                        pa80CopyStringEncoding (destEncoding,
                                                (char*) valuePtr,
                                                len,
                                                NULL,          /* char count */
                                                &byteLen,      /* byte count */
                                                "",
                                                sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_SEARCHABLE:
                    if (descType == API_DESC_TYPE_IRD) {
                        *((SWORD*) valuePtr) = pa20_IsSearchable( recPtr );
                    }; /* if */
                    break;
                case SQL_DESC_TABLE_NAME:
                    if (descType == API_DESC_TYPE_IRD) {
                        retcode = pa80CopyTpr05String ((char*) valuePtr,
                                                       len,
                                                       &byteLen,
                                                       parentStmt->table_name,
                                                       sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                        /*
                         pa80CopyFixedString( parentStmt->table_name,
                                              sizeof( parentStmt->table_name ),
                                              len,
                                              (UCHAR*) valuePtr,
                                              lenPtr, sqlStatePtr );
                        */
                    }; /* if */
                    break;
                case SQL_DESC_TYPE:
                    *((SWORD*) valuePtr) = pa20GetType( descType, recPtr );
                    break;
                case SQL_DESC_TYPE_NAME:
                    /* only for implementation desc */
                    if (descType == API_DESC_TYPE_IRD
                        || descType == API_DESC_TYPE_IPD ) {
                      int       old;
                      tpa40DBC *dbc_block_ptr;
                      dbc_block_ptr = (tpa40DBC*) descPtr->parentDBC;

                      if (dbc_block_ptr->kernelVersionString[0] < '7'  ||
                          (dbc_block_ptr->kernelVersionString[0] == '7'  &&
                           dbc_block_ptr->kernelVersionString[1] == '0'  &&
                           dbc_block_ptr->kernelVersionString[2] <= '5'))
                        old = TRUE;
                      else
                        old = FALSE;

                      retcode =
                        pa80CopyStringEncoding (destEncoding,
                                                (char*) valuePtr,
                                                len,
                                                NULL,          /* char count */
                                                &byteLen,      /* byte count */
                                                (char*) pa20_GetTypeName( recPtr, old ),
                                                sqlStatePtr );
                      if (lenPtr != NULL)
                        *lenPtr = byteLen;
                    }; /* if */
                    break;
                case SQL_DESC_UNNAMED:
                    if (descType == API_DESC_TYPE_IRD
                        || descType == API_DESC_TYPE_IPD ) {
                        *((SWORD*) valuePtr) = recPtr->unnamed;
                    }; /* if */
                    break;
                case SQL_DESC_UNSIGNED:
                    if (descType == API_DESC_TYPE_IRD
                        || descType == API_DESC_TYPE_IPD ) {
                        *((SWORD*) valuePtr) = SQL_FALSE;
                    }; /* if */
                    break;
                case SQL_DESC_UPDATABLE:
                    *((SWORD*) valuePtr) = SQL_ATTR_READWRITE_UNKNOWN;
                    break;
                default:
                    retcode = SQL_ERROR;
                    *sqlStatePtr = API_ODBC_HY091; /* Invalid descriptor field
                                                    * identifier */
                    break;
                }; /* switch */
                break;
            }; /* else */
        }; /* else */
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa20GetDescField", 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode);
    API_TRACE( API_TR_SDWORD, "*lenPtr", lenPtr );

    return retcode;
} /* pa20GetDescField */


UWORD pa20GetDescType( tpa20Desc  *descPtr )
{
    UWORD descType;

    API_TRACE( API_TR_ENTRY, "pa20GetDescType", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    descType = descPtr->descType;

    API_TRACE( API_TR_EXIT, "pa20GetDescType", 0 );
    API_TRACE( API_TR_UWORD, "descType", &descType );

    return descType;
} /* pa20GetDescType */


SQLHDBC pa20GetParentDBC( tpa20Desc *descPtr )
{
    SQLHDBC parentDBC;

    API_TRACE( API_TR_ENTRY, "pa20GetParentDBC", 0);
    API_TRACE( API_TR_PTR, "descPtr", &descPtr);

    parentDBC = descPtr->parentDBC;

    API_TRACE( API_TR_EXIT, "pa20GetParentDBC", 0);
    API_TRACE( API_TR_PTR, "parentDBC", &parentDBC );

    return parentDBC;
} /* pa20GetParentDBC */


SQLHENV pa20GetParentEnv( tpa20Desc *descPtr )
{
    SQLHENV parentEnv;

    API_TRACE( API_TR_ENTRY, "pa20GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "descPtr", &descPtr);

    switch (descPtr->allocType) {
    case SQL_DESC_ALLOC_AUTO:
        /* parent is statement */
        parentEnv = pa60GetParentEnv( (tpa60Stmt*) descPtr->parentStmt );
        break;
    case SQL_DESC_ALLOC_USER:
        /* parent is connection */
        parentEnv = pa40GetParentEnv( (tpa40DBC*) descPtr->parentDBC );
        break;
    default:
        parentEnv = SQL_NULL_HENV;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa20GetParentEnv", 0);
    API_TRACE( API_TR_PTR, "parentEnv", &parentEnv);

    return parentEnv;
} /* pa20GetParentEnv */


tpa21DescRecord* pa20GetRecord( tpa20Desc *descPtr, SWORD recNo)
{
    tpa21DescRecord *recPtr;

    API_TRACE( API_TR_ENTRY, "pa20GetRecord", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );
    API_TRACE( API_TR_SWORD, "recNo", &recNo );

    if ( (recNo >= 0) && (recNo < descPtr->allocRec)) {
        API_ASSERT_PTR( descPtr->records );
        recPtr = &descPtr->records[ (UWORD) (recNo) ];
    } else {
        recPtr = NULL;
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20GetRecord", 0);
    API_TRACE( API_TR_PTR, "recPtr", &recPtr );

    return recPtr;
} /* pa20_GetRecord */


SWORD pa20GetType( UWORD descType, tpa21DescRecord *rec )
{
    SWORD type;

    /* we must distinguish because the user defined
     * sql-type is stored in sqlType for impl. desc. */
    if (descType == API_DESC_TYPE_AD) {
        type = rec->type;
    } else {
        type = rec->sqlType;
    }; /* else */

    switch (type) {
    case SQL_C_DATE:
    case SQL_C_TYPE_DATE:
    case SQL_C_TIME:
    case SQL_C_TYPE_TIME:
    case SQL_C_TIMESTAMP:
    case SQL_C_TYPE_TIMESTAMP:
        return SQL_DATETIME;
        break;
    default:
        return type;
        break;
    }; /* switch */
} /* pa20GetType */


SWORD pa20GetDateTimeIntervalCode( UWORD descType, tpa21DescRecord *rec )
{
    switch (rec->type) {
    case SQL_C_DATE:
    case SQL_C_TYPE_DATE:
        return SQL_CODE_DATE;
        break;
    case SQL_C_TIME:
    case SQL_C_TYPE_TIME:
        return SQL_CODE_TIME;
        break;
    case SQL_C_TIMESTAMP:
    case SQL_C_TYPE_TIMESTAMP:
        return SQL_CODE_TIMESTAMP;
        break;
    default:
        return 0; /* undefined */
        break;
    }; /* switch */
} /* pa20GetDateTimeIntervalCode */


API_RETCODE pa20InitApplDesc( SQLHDESC hdesc, SQLHDBC hdbc, SQLHSTMT hstmt )
{
    tpa20Desc *descPtr;

    API_TRACE( API_TR_ENTRY, "pa20InitApplDesc", 0 );
    API_TRACE( API_TR_HANDLE, "hdbc", &hdbc );
    API_TRACE( API_TR_HANDLE, "hdesc", &hdesc );

    API_ASSERT( pa40VerifyDBC( hdbc ) == API_OK
                || pa60VerifyStmt( hstmt ) == API_OK );
    API_ASSERT( hdbc == SQL_NULLHANDLE
                || hstmt == SQL_NULLHANDLE );

    descPtr = (tpa20Desc*) hdesc;
    API_ASSERT_PTR( descPtr );

    pa30InitDiagArea( &descPtr->diagArea );

    descPtr->handleType     = API_DESC_TYPE;
    descPtr->parentDBC      = hdbc;
    descPtr->parentStmt     = hstmt;
    descPtr->allocType      = SQL_DESC_ALLOC_AUTO;
    descPtr->descType       = API_DESC_TYPE_AD;
    descPtr->count          = 0;
    descPtr->records        = NULL;
    descPtr->allocRec       = 0;

    descPtr->header.arraySize         = 1;
    descPtr->header.previousArraySize = 1;
    descPtr->header.arrayStatusPtr    = NULL;
    descPtr->header.bindOffsetPtr     = NULL;
    descPtr->header.bindType          = SQL_BIND_TYPE_DEFAULT;

    descPtr->state = API_DESC_EALLOCATED;

    API_TRACE( API_TR_EXIT, "pa20InitApplDesc", 0);

    return API_OK;
} /* pa20InitApplDesc */


API_RETCODE pa20InitCount( tpa20Desc *descPtr )
{
    while (!descPtr->records[descPtr->count].bound && descPtr->count > 0) {
        descPtr->count--;
    };
    return API_OK;
} /* pa20InitCount */


API_RETCODE pa20InitImplDesc( SQLHDESC hdesc,
                              SQLHDBC  hdbc,
                              SQLHSTMT hstmt,
                              UWORD descType )
{
    tpa20Desc *descPtr;

    API_TRACE( API_TR_ENTRY, "pa20InitImplDesc", 0 );
    API_TRACE( API_TR_HANDLE, "hstmt", &hstmt );
    API_TRACE( API_TR_HANDLE, "hdesc", &hdesc );
    API_TRACE( API_TR_UWORD, "descType", &descType );

    API_ASSERT( pa20VerifyDesc( hdesc ) == API_OK );
    API_ASSERT( pa60VerifyStmt( hstmt ) == API_OK );

    descPtr = (tpa20Desc*) hdesc;
    API_ASSERT_PTR( descPtr );

    pa30InitDiagArea( &descPtr->diagArea );

    descPtr->handleType     = API_DESC_TYPE;
    descPtr->parentDBC      = hdbc;
    descPtr->parentStmt     = hstmt;
    descPtr->allocType      = SQL_DESC_ALLOC_AUTO;
    descPtr->descType       = descType;
    descPtr->count          = 0; /* not available yet */
    descPtr->records        = NULL;
    descPtr->allocRec       = 0;

    descPtr->header.arrayStatusPtr   = NULL;
    descPtr->header.rowsProcessedPtr = NULL;

    descPtr->state = API_DESC_IALLOCATED;

    API_TRACE( API_TR_EXIT, "pa20InitImplDesc", 0);

    return API_OK;
} /* pa20InitImplDesc */


API_RETCODE pa20InitLongHandles( tpa20Desc *descPtr )
{
    tpa21DescRecord *recPtr = descPtr->records;
    UDWORD           recNo;

	for ( recNo = 0; recNo < descPtr->allocRec; recNo++ ) {
        recPtr[ recNo ].longHndl = API_NULL_HANDLE;
	}; /* for */

    return API_OK;
} /* pa20InitLongHandles */


API_RETCODE pa20MapDataType( tpa20Desc *impl_desc_ptr,
                             UWORD      icol,
                             UWORD      pos_typename,
                             UWORD      pos_datatype,
                             UWORD      pos_sqldatatype,
                             UWORD      pos_precision,
                             UWORD      pos_length,
                             UWORD      pos_codetype )
{
    API_RETCODE          api_retcode;
    UCHAR               *type_name;
    UCHAR               *code_type;
    SWORD                data_type;
    pa20ODBCTypesStruct *p;
    UDWORD               odbcVersion;

    pa20ODBCTypesStruct  *ODBCTypes;
    tpa40DBC             *dbc_block_ptr;

    char lname[API_MAX_KEYWORD_LEN+1];
    char ltype[API_MAX_KEYWORD_LEN+1];
    unsigned int len;
    int  unicodeFlag = FALSE;

    tpa21DescRecord *prec_rec_ptr    = NULL;
    tpa21DescRecord *len_rec_ptr     = NULL;
    tpa21DescRecord *type_rec_ptr    = NULL;
    tpa21DescRecord *sqltype_rec_ptr = NULL;

    API_TRACE( API_TR_ENTRY,"pa20MapDatatype",0);
    API_TRACE( API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE( API_TR_UWORD, "icol", &icol);
    API_TRACE( API_TR_UWORD, "pos_typename", &pos_typename);
    API_TRACE( API_TR_UWORD, "pos_datatype", &pos_datatype);
    API_TRACE( API_TR_UWORD, "pos_precision", &pos_precision);
    API_TRACE( API_TR_UWORD, "pos_length", &pos_length);
    API_TRACE( API_TR_UWORD, "pos_codetype", &pos_codetype);

    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    api_retcode = API_OK;

    pa10GetODBCVersion( SQL_HANDLE_DESC, (SQLHDESC) impl_desc_ptr, &odbcVersion );


    dbc_block_ptr = (tpa40DBC*) impl_desc_ptr->parentDBC;
    if (dbc_block_ptr->kernelVersionString[0] < '7'  ||
        (dbc_block_ptr->kernelVersionString[0] == '7'  &&
         dbc_block_ptr->kernelVersionString[1] == '0'  &&
         dbc_block_ptr->kernelVersionString[2] <= '5'))
      ODBCTypes = pa20ODBCTypes_old;
    else
      ODBCTypes = pa20ODBCTypes;

    if ( icol == pos_typename || icol == pos_datatype
         || icol == pos_precision || icol == pos_length) {
        API_ASSERT( pos_datatype > 0 );
        type_rec_ptr = pa20GetRecord( impl_desc_ptr, pos_datatype );
        if (pos_sqldatatype > 0) {
            sqltype_rec_ptr = pa20GetRecord( impl_desc_ptr, pos_sqldatatype );
        };
        API_ASSERT_PTR( type_rec_ptr );
        API_MEMCPY( &data_type,
                    type_rec_ptr -> optimizePtr,
                    sizeof(data_type) );
        API_TRACE( API_TR_SWORD, "data_type", &data_type);
        if (data_type == SQL_TYPE_EMPTY) {
            data_type = SQL_TYPE_NOTFOUND;
            API_ASSERT( pos_typename > 0 );
            type_name = pa20GetRecord( impl_desc_ptr, pos_typename )
                -> optimizePtr;
            API_ASSERT( pos_codetype > 0 );
            code_type = pa20GetRecord( impl_desc_ptr, pos_codetype )
                -> optimizePtr;

            /* if we are connected against a Unicode kernel, catalog info will be in Unicode.
               Therefore translate type name and code type back to ASCII, if possible.
               http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122580 */
            /* strlen <= 1 in swapped and unswapped UCS2 (little/big endian)
               http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127406 */
            if (API_STRLEN (type_name) <= 1) {
              unicodeFlag = TRUE;
              sp81UCS2toASCII ((tsp00_Byte*) lname, sizeof (lname), &len,
                               (tsp81_UCS2Char*) type_name, sp81UCS2strlen((tsp81_UCS2Char*)type_name),
                               sp77nativeUnicodeEncoding () == sp77encodingUCS2Swapped);
              lname[len] = '\0';
            }
            else {
              API_STRNCPY (lname, type_name, sizeof(lname));
              lname[sizeof(lname)-1] = '\0';
            }

            if (API_STRLEN (code_type) <= 1) {
              sp81UCS2toASCII ((tsp00_Byte*) ltype, sizeof (ltype), &len,
                               (tsp81_UCS2Char*) code_type, sp81UCS2strlen((tsp81_UCS2Char*)code_type),
                               sp77nativeUnicodeEncoding () == sp77encodingUCS2Swapped);
              ltype[len] = '\0';
            }
            else {
              API_STRNCPY (ltype, code_type, sizeof(ltype));
              lname[sizeof(ltype)-1] = '\0';
            }

            for ( p=ODBCTypes; p->data_type != SQL_TYPE_NOTFOUND; p++) {
              /* PTS 1116570: cmp length because of TIME/TIMESTAMP (strcmp (TS, T, 4) = 0) */
              if (API_STRLEN (lname) != API_STRLEN (p->type_name) ||
                  API_STRNCMP (lname, p->type_name, API_STRLEN(p->type_name)) != 0)
                    continue;
                else {
                    if (API_STRNCMP( ltype,
                                     p->code_type,
                                     API_STRLEN(p->code_type)))
                        continue;
                    else {
                        /* Precision uebersteuern, da der Kern keine liefert */
                        API_ASSERT( pos_precision > 0 );
                        prec_rec_ptr =
                            pa20GetRecord( impl_desc_ptr, pos_precision);
                        API_ASSERT_PTR( prec_rec_ptr );
                        API_ASSERT( pos_length > 0 );
                        len_rec_ptr = pa20GetRecord( impl_desc_ptr,
                                                     pos_length );
                        API_ASSERT_PTR( len_rec_ptr );
                        if (apgisindi( prec_rec_ptr-> optimizeNullPtr)) {
                            prec_rec_ptr -> isNull = API_FALSE;
                            prec_rec_ptr -> optimizeNullPtr = API_FALSE;
                            API_MEMCPY( prec_rec_ptr-> optimizePtr,
                                        &p->precision,
                                        sizeof(p->precision) );
                            API_MEMCPY( len_rec_ptr -> optimizePtr,
                                        &p->precision,
                                        sizeof(p->precision) );
                        }
                        /* Laenge uebersteuern, da der Kern die Laenge nicht
                           kennen kann, da sie vom ODBCDatentype abhaengt */
                        if (p->lenght != 0)
                            API_MEMCPY( len_rec_ptr -> optimizePtr,
                                        &p->lenght,
                                        sizeof(p->lenght));
                        /* odbc datentype einsetzten */
                        p->data_type = pa20_GetConciseType( odbcVersion,
                                                            p->data_type );
                        API_MEMCPY( type_rec_ptr -> optimizePtr,
                                    &p->data_type,
                                    sizeof(p->data_type) );
                        if (pos_sqldatatype > 0) {
                            API_MEMCPY( sqltype_rec_ptr -> optimizePtr,
                                        &p->data_type,
                                        sizeof(p->data_type) );
                        };
                        if (p->sql_type_name[0] != '\0')
                          if (unicodeFlag == TRUE) {  /* if type_name was in unicode */
                            sp81ASCIItoUCS2 ((tsp81_UCS2Char*) type_name,
                                             (unsigned int) API_STRLEN (p->sql_type_name),
                                             sp77nativeUnicodeEncoding () == sp77encodingUCS2Swapped,
                                             &len, p->sql_type_name,
                                             (unsigned int) API_STRLEN (p->sql_type_name));
                            API_MEMSET (type_name + len*sizeof(tsp81_UCS2Char), 0, sizeof(tsp81_UCS2Char));
                          }
                          else
                            API_STRCPY( type_name, p->sql_type_name );

                        API_TRACE( API_TR_STRING, "type_name", type_name);
                        API_TRACE( API_TR_SWORD, "data_type",
                                   type_rec_ptr -> optimizePtr);
                        API_TRACE( API_TR_SDWORD, "precision",
                                   prec_rec_ptr -> optimizePtr);
                        API_TRACE( API_TR_SDWORD, "lenght",
                                   len_rec_ptr -> optimizePtr);
                        break;
                    }
                }
            }
        }
    } /* end if icol */

    API_TRACE(API_TR_EXIT,"pa20MapDataType",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return(api_retcode);
} /* pa20MapDataType */


     /* move fields from sqlda areas to user fields */
API_RETCODE pa20Move( tpa20Desc       *appl_desc_ptr,
                      tpa20Desc       *impl_desc_ptr,
                      UWORD            number_cols,
                      tpa62DBCSpecial *special_dbc_ptr)

{
    API_RETCODE  api_retcode;
    API_RETCODE  conv_retcode;
    SQLLEN       logical_length;
    SQLLEN       used_length;
    UWORD        icol;
    tpa21DescRecord *appl_rec_ptr;
    tpa21DescRecord *impl_rec_ptr;

    API_TRACE( API_TR_ENTRY, "pa20Move",0);
    API_TRACE( API_TR_PTR, "appl_desc_ptr", &appl_desc_ptr);
    API_TRACE( API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE( API_TR_UWORD, "number_cols", &number_cols);

    API_ASSERT_PTR( appl_desc_ptr );
    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );
    API_ASSERT( appl_desc_ptr->descType == API_DESC_TYPE_AD );

    api_retcode = API_OK;
    conv_retcode = API_OK;
    /* process bound columns. */
    for (icol=1; icol <= number_cols; icol++ ) {
        API_TRACE(API_TR_UWORD, "icol", &icol);

        appl_rec_ptr = pa20GetRecord( appl_desc_ptr, icol );
        API_ASSERT_PTR( appl_rec_ptr );
        impl_rec_ptr = pa20GetRecord( impl_desc_ptr, icol );
        API_ASSERT_PTR( impl_rec_ptr );

        /* always set the is_null     */
        /* flag so SQLGetData works!  */
        /* (even un-bound columns)    */
        if (  impl_rec_ptr -> esqIO == CPR_INFO_INPUT )
            continue;
        API_TRACE( API_TR_SWORD, "nullable", &impl_rec_ptr -> nullable );
        if (impl_rec_ptr -> nullable == SQL_NULLABLE) {
            if ( apgisindi( impl_rec_ptr -> optimizeNullPtr )) {
                impl_rec_ptr -> isNull = API_TRUE;
            }
            else
                impl_rec_ptr -> isNull = API_FALSE;
        }
        else {
            impl_rec_ptr -> isNull = API_FALSE;
        }
        if ( apgislong( impl_rec_ptr->type ))
            continue;
        if ( appl_rec_ptr -> bound == API_TRUE
             && (impl_rec_ptr -> parameterType == SQL_PARAM_INPUT_OUTPUT
                || impl_rec_ptr -> parameterType == SQL_PARAM_OUTPUT)) {
            /* move bound column */
            impl_rec_ptr -> movedLen = 0;
            conv_retcode = pa20SMap( impl_desc_ptr,
                                     icol,
                                     special_dbc_ptr,
                                     appl_rec_ptr -> type,
                                     appl_rec_ptr -> scale,
                                     (UCHAR*) PA20_DATA_PTR( appl_rec_ptr ),
                                     appl_rec_ptr ->  octetLength,
                                     &used_length );
            API_TRACE(API_TR_API_RETCODE,"conv_retcode",&conv_retcode);

            logical_length = used_length;

            /*
               conv_retcode might be :
               API_OK	-- rgbValue: data
               API_TRUNCATE	-- rgbValue: truncated data
               API_DATA_LOSS	-- rgbValue: Untouched
               API_NOT_OK	-- problem during processing
               API_NO_DATA_FOUND	-- all data read
               */

            if (conv_retcode == API_NOT_OK || conv_retcode > api_retcode) {
                api_retcode = conv_retcode;
            }

            if (api_retcode == API_NOT_OK) {
                break;                       /* out of loop */
            }

            /* set the null indicator     */
            /* and/or length              */
            /* ODBC reference:
             * SQLFetch determines whether the application has specified
             * separate length and indicator buffers. In this case, when
             * the data is not NULL, SQLFetch sets the indicator buffer
             * to 0 and returns the length in the length buffer. When the
             * data is NULL, SQLFetch sets the indicator buffer to
             * SQL_NULL_DATA and does not modify the length buffer. */
            if (api_retcode == API_OK || api_retcode == API_TRUNCATE)
                if ( impl_rec_ptr -> isNull == API_TRUE) {
                    if (appl_rec_ptr->indicatorPtr != NULL) {
                        *(PA20_INDICATOR_PTR(appl_rec_ptr)) = SQL_NULL_DATA;
                    } else if (appl_rec_ptr->octetLengthPtr != NULL) {
                        *(PA20_OCTETLENGTH_PTR(appl_rec_ptr)) = SQL_NULL_DATA;
                    } else {
                        /* Indicator variable required but not supplied */
                        api_retcode = API_NO_INDICATOR;
                    }; /* else */
                } else {
                    if (appl_rec_ptr->octetLengthPtr != NULL) {

                        /* for debugging
                           SQLLEN *p;
                           p = PA20_OCTETLENGTH_PTR(appl_rec_ptr);
                           *p = logical_length;
                        */

                        *(PA20_OCTETLENGTH_PTR(appl_rec_ptr)) = logical_length;
                    }; /* if */
                    if (appl_rec_ptr->indicatorPtr != NULL
                        && (appl_rec_ptr->octetLengthPtr !=
                            appl_rec_ptr->indicatorPtr)) {
                        *PA20_INDICATOR_PTR(appl_rec_ptr) = 0;
                    }; /* if */
                }; /* else */
        } /* of if bound */
    } /* of for loop */

    API_TRACE(API_TR_EXIT,"pa20Move",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return (api_retcode);
} /* pa20Move */


/* populate the attribute blocks with descriptive data */
API_RETCODE pa20PopulateDesc( sqldatype       *sqlda_ptr,
                              tpa20Desc       *impl_desc_ptr,
                              UWORD            number_cols,
                              UDWORD          *size_ptr,
                              tpa62DBCSpecial *special_dbc_ptr,
                              SQLUSMALLINT     unicode_catalog)
{
    UDWORD tuple_size;
    UDWORD length;
    UWORD scale;
    UDWORD precision;
    UWORD i;
    API_RETCODE api_retcode;
    sqlvartype * sqlvar_ptr;
    tpa21DescRecord *impl_rec_ptr;

    API_TRACE(API_TR_ENTRY,"pa20PopulateDesc",0);
    API_TRACE(API_TR_PTR,"sqlda_ptr",&sqlda_ptr);
    API_TRACE(API_TR_PTR,"impl_desc_ptr",& impl_desc_ptr);
    API_TRACE(API_TR_PTR,"size_ptr",&size_ptr);

    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    tuple_size = 0;
    api_retcode = API_OK;

    /* set info for bookmark column */
    impl_rec_ptr = pa20GetRecord( impl_desc_ptr, API_DESC_BOOKMARK_INDEX );
    impl_rec_ptr->type = SQL_INTEGER; /* don't support variable length
                                       * bookmarks yet */
    impl_rec_ptr -> movedLen     = 0;
    impl_rec_ptr -> nullable     = SQL_NO_NULLS;
    impl_rec_ptr -> esqType      = dinteger;
    impl_rec_ptr -> esqIO        = CPR_INFO_OUTPUT;
    impl_rec_ptr -> esqLength    = 4;
    impl_rec_ptr -> esqScale     = 0;
    impl_rec_ptr -> esqPrecision = 10;
    impl_rec_ptr -> isNull       = API_FALSE;
    impl_rec_ptr -> alreadyRead  = 0;
    /* set info for "regular" columns */
    for ( i=0; i < number_cols; i++ ) {
        API_TRACE(API_TR_UWORD,"sqlcol",&i);
        impl_rec_ptr = pa20GetRecord( impl_desc_ptr, (UWORD) (i+1) );
        API_ASSERT_PTR( impl_rec_ptr );

        impl_rec_ptr -> isNull = API_FALSE;
        sqlvar_ptr = &sqlda_ptr -> sqlvar [i];
        impl_rec_ptr -> esqType = sqlvar_ptr->coltype;
        impl_rec_ptr -> realEsqType = impl_rec_ptr->esqType;
        impl_rec_ptr -> esqIO = sqlvar_ptr->colio;
        impl_rec_ptr -> movedLen = 0;
        impl_rec_ptr -> alreadyRead  = 0;
        length = sqlvar_ptr->collength;
        precision = sqlvar_ptr->collength;
        scale = sqlvar_ptr->colfrac;
        API_TRACE(API_TR_SWORD,"coltype", &sqlvar_ptr->coltype);
        API_TRACE(API_TR_SWORD,"colmode", &sqlvar_ptr->colmode);
        API_TRACE(API_TR_SDWORD,"collength", &sqlvar_ptr->collength);
        /* ADIS 1001549
         * Bei einem outer join k�nnen trotz CPR_INFO_MANDATORY
         * Null-Werte auftreten. Deshalb wird bei Ausgabeparametern
         * grunds�tzlich ein Null-Wert vorgesehen. */
        if ((sqlvar_ptr->colmode & 1 << CPR_INFO_MANDATORY)         /* PTS 1118671 */
            && (impl_rec_ptr -> esqIO == CPR_INFO_INPUT)) {
            impl_rec_ptr -> nullable = SQL_NO_NULLS;
        }
        else {
            tuple_size += ALIGN(sizeof(sqlvar_ptr->hostindicator));
            if ((sqlvar_ptr->colmode & 1 << CPR_INFO_OPTIONAL)      /* PTS 1118671 */
                || (impl_rec_ptr -> esqIO != CPR_INFO_INPUT) ) {
                impl_rec_ptr -> nullable = SQL_NULLABLE;
            }
            else
                impl_rec_ptr -> nullable = SQL_NULLABLE_UNKNOWN;
        }
        /* is default value defined for column?  PTS 1118562 */
        impl_rec_ptr->defaultValue = (sqlvar_ptr->colmode & 1 << CPR_INFO_DEFAULT) ? 1 : 0;

        switch (impl_rec_ptr -> esqType) {
        case (dche): {}
        case (dcha): {
            impl_rec_ptr -> esqPrecision = length;
            length++;   /* one for 0 terminator */
            tuple_size += length;
            impl_rec_ptr -> type = SQL_CHAR;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
        case (dunicode): {
            impl_rec_ptr -> esqPrecision = length * sizeof (tsp81_UCS2Char);
            length += 1;   /* for zero terminator */
            tuple_size += length * sizeof (tsp81_UCS2Char);
            impl_rec_ptr -> type = SQL_WCHAR;
            impl_rec_ptr -> esqLength = length * sizeof (tsp81_UCS2Char);
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dvarchara): {}
        case (dvarchare): {
            impl_rec_ptr -> esqPrecision = length;
            length++;   /* one for 0 terminator */
            tuple_size += length;
            impl_rec_ptr -> type = SQL_VARCHAR;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dvarcharuni): {
            impl_rec_ptr -> esqPrecision = length * sizeof (tsp81_UCS2Char);
            length += 1;   /* for zero terminator */
            tuple_size += length * sizeof (tsp81_UCS2Char);
            impl_rec_ptr -> type = SQL_WVARCHAR;
            impl_rec_ptr -> esqLength = length * sizeof (tsp81_UCS2Char);
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dstra): {}
        case (dstre): {}
        case (dlonga): {}
        case (dlonge): {
            impl_rec_ptr -> esqPrecision = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqLength = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqScale = 0;
            length = 0;
            tuple_size += length;
            impl_rec_ptr -> type = (UWORD) SQL_LONGVARCHAR;

            /* quick&dirty solution (cmp vpa04gc). If packet encoding
               is unicode, long ascii is in unicode, confirmed by HB */
            if (unicode_catalog != 0)
              impl_rec_ptr -> esqType = dstruni;
            break;
        }
        case (dstruni): {}
        case (dlonguni): {
            impl_rec_ptr -> esqPrecision = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqLength = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqScale = 0;
            length = 0;
            tuple_size += length;
            impl_rec_ptr -> type = (UWORD) SQL_UNICODE_LONGVARCHAR;
            break;
        }
        case (dstrb): {}
        case (dstrdb): {}
        case (dlongb): {}
        case (dlongdb): {
            impl_rec_ptr -> esqPrecision = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqLength = special_dbc_ptr->blocksize;
            impl_rec_ptr -> esqScale = 0;
            length = 0;
            tuple_size += length;
            impl_rec_ptr -> type = (UWORD) SQL_LONGVARBINARY;
            break;
        }
        case (ddbyteebcdic): {}
        case (dchb): {
            impl_rec_ptr -> esqPrecision = length;
            length += API_VSTRING_OFFSET;
            length++;   /* one for 0 terminator */
            tuple_size += length;
            impl_rec_ptr -> type = (UWORD) SQL_BINARY;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dvarcharb): {
            impl_rec_ptr -> esqPrecision = length;
            length += API_VSTRING_OFFSET;
            length++;   /* one for 0 terminator */
            tuple_size += length;
            impl_rec_ptr -> type = (UWORD) SQL_VARBINARY;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dsmallint): {
            length = sizeof(SWORD);
            tuple_size += length;
            impl_rec_ptr -> type = SQL_SMALLINT;
            impl_rec_ptr -> esqPrecision = API_SMALLINT_PREC;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dinteger): {
            length = sizeof(SDWORD);
            tuple_size += length;
            impl_rec_ptr -> type = SQL_INTEGER;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = API_INTEGER_PREC;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dfloat): {
          if (length > API_REAL_PREC) {  /* Alexanders Mail */
                length = sizeof(SDOUBLE);
                tuple_size += length;
                impl_rec_ptr -> esqPrecision = API_FLOAT_PREC;
                impl_rec_ptr -> type = SQL_FLOAT;
            }
            else {
                length = sizeof(SFLOAT);
                tuple_size += length;
                impl_rec_ptr -> esqPrecision = API_REAL_PREC;
                impl_rec_ptr -> type = SQL_REAL;
            }
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dfixed): {
            length = (precision + 2); /* one for decimalpoint and one
                                       * for sign */
            length = (precision > scale) ? length : length + 1;
            /* and one for the posible zero */
            length++;   /* one for 0 terminator */
            tuple_size += length;
            impl_rec_ptr -> type = SQL_DECIMAL;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = precision;
            impl_rec_ptr -> esqScale = scale;
            break;
        }
        case (ddate): {
            tuple_size += length;
            impl_rec_ptr -> type = SQL_TYPE_DATE;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = API_DATE_PREC;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dtime): {
            tuple_size += length;
            impl_rec_ptr -> type = SQL_TYPE_TIME;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = API_TIME_PREC;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dtimestamp): {
            tuple_size += length;
            impl_rec_ptr -> type = SQL_TYPE_TIMESTAMP;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = API_TIMESTAMP_EXT_PREC;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        case (dboolean): {
            tuple_size += length;
            impl_rec_ptr -> type = SQL_BIT;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = API_BIT_PREC;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1128557 */
        case (dabaptabhandle): {
            tuple_size += length;
            impl_rec_ptr -> type = SQL_ABAPHANDLE;
            impl_rec_ptr -> esqLength = length;
            impl_rec_ptr -> esqPrecision = length;
            impl_rec_ptr -> esqScale = 0;
            break;
        }
        default:
            API_TRACE( API_TR_SWORD,"** unknown ** coltype",
                       &sqlda_ptr -> sqlvar [i] . coltype);
            api_retcode = API_NOT_OK;
            break;
        } /* of switch */
        tuple_size = ALIGN(tuple_size);

        API_TRACE(API_TR_SWORD,"esqIO", &impl_rec_ptr -> esqIO);
        API_TRACE(API_TR_SWORD,"sql_type", &impl_rec_ptr -> type);
        API_TRACE(API_TR_UWORD,"precision", &impl_rec_ptr -> esqPrecision);
        API_TRACE(API_TR_UWORD,"esqLength", &impl_rec_ptr -> esqLength);
        API_TRACE(API_TR_UWORD,"scale", &impl_rec_ptr -> esqScale);
        if (api_retcode != API_OK) {
            break; /* exit loop, have an error */
        }

    } /* of for */

    *size_ptr = tuple_size;

    API_TRACE(API_TR_EXIT,"pa20PopulateDesc",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    API_TRACE(API_TR_UDWORD,"*size_ptr",size_ptr);

    return(api_retcode);
} /* pa20PopulateDesc */


/* put fields from user fields to sqlda areas */
API_RETCODE pa20PutDesc( tpa20Desc *apd_ptr,
                         tpa20Desc *ipd_ptr,
                         UWORD      number_parms,
                         UWORD      sqlmode)     /* PTS 1122478 */
{
    API_RETCODE api_retcode;
    API_RETCODE conv_retcode;
    UWORD i;
    tpa21DescRecord *appl_desc_rec_ptr;
    tpa21DescRecord *impl_desc_rec_ptr;
    SQLLEN          *octetLengthPtr = NULL;

    API_TRACE( API_TR_ENTRY, "pa20PutDesc", 0);
    API_TRACE( API_TR_PTR, "apd_ptr", &apd_ptr);
    API_TRACE( API_TR_PTR, "ipd_ptr", &ipd_ptr);
    API_TRACE( API_TR_UWORD, "number_parms", &number_parms);

    api_retcode = API_OK;
    conv_retcode = API_OK;

    API_ASSERT_PTR( apd_ptr );
    API_ASSERT_PTR( ipd_ptr );
    API_ASSERT( pa20GetDescType( ipd_ptr ) == API_DESC_TYPE_IPD );
    API_ASSERT( number_parms < ipd_ptr->allocRec );

    for ( i=1; i <= number_parms; i++ ) {
        appl_desc_rec_ptr = pa20GetRecord( apd_ptr, i );
        impl_desc_rec_ptr = pa20GetRecord( ipd_ptr, i );
        API_ASSERT_PTR( appl_desc_rec_ptr );
        API_ASSERT_PTR( impl_desc_rec_ptr );
        if (impl_desc_rec_ptr->esqIO == CPR_INFO_OUTPUT) {
            continue;
        }; /* if */
        octetLengthPtr = PA20_OCTETLENGTH_PTR( appl_desc_rec_ptr );
        /* skip params with length == SQL_IGNORE. These are also skipped
         * in SQLSetPos. So parameter numbers are matching. PTS 1104910 */
        if (octetLengthPtr != NULL && *octetLengthPtr == SQL_IGNORE) {
            continue;
        }; /* if */
        API_TRACE( API_TR_SWORD, "esqIO", &impl_desc_rec_ptr -> esqIO );
        if ( appl_desc_rec_ptr -> bound == API_TRUE
             && ( impl_desc_rec_ptr -> parameterType == SQL_PARAM_INPUT ||
                  impl_desc_rec_ptr -> parameterType
                  == SQL_PARAM_INPUT_OUTPUT)) {
            conv_retcode = pa21PutRec( appl_desc_rec_ptr, impl_desc_rec_ptr, sqlmode );
            if ( conv_retcode == API_NOT_OK || conv_retcode > api_retcode ) {
                api_retcode = conv_retcode;
                conv_retcode = API_OK;
            }; /* if */
            if ( api_retcode != API_OK
                 && api_retcode != API_TRUNCATE
                 && api_retcode != API_NEED_DATA) {
                break; /* out of loop */
            }; /* if */
        } /* of if bound */
        else {
            api_retcode = API_FEW_BOUNDS;
        }; /* else */
    } /* of for loop */

    API_TRACE(API_TR_EXIT,"pa20PutDesc",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return (api_retcode);
} /* pa20PutDesc */


API_RETCODE pa20PutError( SQLHDESC hdesc, UWORD sqlState )
{
    API_RETCODE apiRetcode = API_OK;
    tpa20Desc *descPtr;
    tpa31DiagRec *newDiagRec;

    API_TRACE( API_TR_ENTRY, "pa20PutError", 0);

    descPtr    = (tpa20Desc*) hdesc;
    newDiagRec = pa30AddDiagRec( &descPtr->diagArea );
    if (newDiagRec!=NULL) {
        pa31InitDiagRec( newDiagRec, sqlState, 0, (UCHAR*) "", 0, 0 );
    } else {
        apiRetcode = API_NOT_OK;
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20PutError", 0 );
    API_TRACE( API_TR_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20PutError */


/* put fields from user fields to sqlda areas */
API_RETCODE pa20PutRec( tpa21DescRecord *appl_desc_rec_ptr,
                        tpa21DescRecord *impl_desc_rec_ptr,
                        UWORD            sqlmode)
{
    API_RETCODE api_retcode;

    API_TRACE( API_TR_ENTRY, "pa20PutRec", 0);
    API_TRACE( API_TR_PTR, "appl_desc_rec_ptr", &appl_desc_rec_ptr);
    API_TRACE( API_TR_PTR, "impl_desc_rec_ptr", &impl_desc_rec_ptr);

    api_retcode = API_OK;

    API_ASSERT_PTR( appl_desc_rec_ptr );
    API_ASSERT_PTR( impl_desc_rec_ptr );

    if (impl_desc_rec_ptr->esqIO != CPR_INFO_OUTPUT) {
        API_TRACE( API_TR_SWORD, "esqIO", &impl_desc_rec_ptr -> esqIO );
        if ( appl_desc_rec_ptr -> bound == API_TRUE
             && ( impl_desc_rec_ptr -> parameterType == SQL_PARAM_INPUT ||
                  impl_desc_rec_ptr -> parameterType
                  == SQL_PARAM_INPUT_OUTPUT)) {
            api_retcode = pa21PutRec( appl_desc_rec_ptr, impl_desc_rec_ptr, sqlmode );
        } /* of if bound */
        else {
            api_retcode = API_FEW_BOUNDS;
            if (appl_desc_rec_ptr -> bound == API_TRUE) {
            }
        }; /* else */

    }; /* if */

    API_TRACE(API_TR_EXIT,"pa20PutRec",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return (api_retcode);
} /* pa20PutRec */

API_RETCODE pa20ReallocRecords( tpa20Desc *descPtr,
                                UWORD      numRecords )
{
    API_RETCODE apiRetcode = API_OK;
    UDWORD oldSize;
    UDWORD newSize;
    UDWORD oldNumRec;
    UDWORD recNo;
    tpa21DescRecord *newRec;
    tpa21DescRecord *descRecPtr;

    API_TRACE( API_TR_ENTRY, "pa20ReallocRecords", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    API_ASSERT_PTR( descPtr );

    /* +1 for bookmark record */
    if ( numRecords+1 > descPtr->allocRec ) {
        oldNumRec = descPtr->allocRec;
        oldSize = sizeof(tpa21DescRecord) * descPtr->allocRec;
        /* +1 for the bookmark record */
        newSize = sizeof(tpa21DescRecord) * (numRecords+1);

        /* alloc new area */
        newRec = apdallo( (UDWORD) newSize);
        if (newRec == NULL) {
            apiRetcode = API_NOT_OK;
        } else {
            API_MEMCPY( newRec, descPtr->records, oldSize );
            /* free the old area */
            pa20_FreeRecords( descPtr );
            descPtr->records  = newRec;
            descPtr->allocRec = numRecords+1;
            descPtr->count    = numRecords;
            /* init new records */
            for ( recNo=oldNumRec; recNo<=numRecords; recNo++ ) {
                descRecPtr = &descPtr->records[ (UDWORD) (recNo)];
                pa21InitDescRecord( descRecPtr, descPtr );
            }; /* for */
        }; /* else */
    }; /* if */

    API_TRACE( API_TR_EXIT, "pa20ReallocRecords", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20ReallocRecords */


/* set the pointers in the attribute blocks */
API_RETCODE pa20SetAttr( sqldatype  *sqlda_ptr,
                         tpa20Desc  *impl_desc_ptr,
                         UWORD       attr_array_cnt,
                         UCHAR      *buffer_ptr,
                         UDWORD      buffer_size,
                         SWORD       iotype )
{
    API_RETCODE      api_retcode;
    tpa21DescRecord *desc_rec_ptr;
    UDWORD           used;
    UCHAR           *pointer;
    UWORD            i;
    UDWORD           length;
    SWORD            hostvartype;
    UCHAR           *hostvaraddr;
    UDWORD           collength;
    sqlvartype      *sqlvar_ptr;

    API_TRACE(API_TR_ENTRY, "pa20SetAttr", 0);
    API_TRACE(API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE(API_TR_UWORD, "attr_array_cnt", &attr_array_cnt);
    API_TRACE(API_TR_PTR, "buffer_ptr", &buffer_ptr);
    API_TRACE(API_TR_UDWORD, "buffer_size", &buffer_size);
    API_TRACE(API_TR_SWORD, "iotype", &iotype);

    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    used = 0;
    pointer = buffer_ptr;
    api_retcode = API_OK;

    API_ASSERT( attr_array_cnt <= impl_desc_ptr->allocRec );

    for ( i=1; i <= attr_array_cnt; i++ ) {
        desc_rec_ptr = pa20GetRecord( impl_desc_ptr, i );
        API_ASSERT_PTR( desc_rec_ptr );

        API_TRACE(API_TR_UWORD, "column", &i);
        API_TRACE(API_TR_SWORD, "type", &desc_rec_ptr->type );
        API_TRACE(API_TR_SWORD, "nullable", &desc_rec_ptr -> nullable );

        api_retcode = pa21SetHostVarType( desc_rec_ptr,
                                          &hostvartype,
                                          &collength);
        if (api_retcode != API_OK ) {
            break;
        }
        if (!apgislong(desc_rec_ptr->type)) {
            hostvaraddr = pointer;
            length = desc_rec_ptr -> esqLength;
        }
        else {
            api_retcode = pa21GetLongAddr( &hostvaraddr, desc_rec_ptr );
            length = 0;
        }; /* else */
        length = ALIGN(length);
        if ( apgisvstr( desc_rec_ptr -> type )) {
            /* ADIS 1001690 */
            apgsvstrl( hostvaraddr, 0 /* collength-API_VSTRING_OFFSET */);
        }

        desc_rec_ptr->optimizePtr = hostvaraddr;
        desc_rec_ptr->lastData = API_FALSE;
        desc_rec_ptr->movedLen = 0;
        desc_rec_ptr->alreadyRead  = 0;
        desc_rec_ptr->isNull = API_FALSE;

        sqlvar_ptr = &sqlda_ptr -> sqlvar [i-1];
        sqlvar_ptr->hostvartype = hostvartype;
        sqlvar_ptr->collength = (tsp00_Int4) collength;
        sqlvar_ptr->hostvaraddr = hostvaraddr;

        /* handle NULL indicator */
        if ( desc_rec_ptr->nullable == SQL_NULLABLE ) {
            sqlvar_ptr->hostindaddr = (sqlint4 *) (pointer + length);
            desc_rec_ptr->optimizeNullPtr = (tsp00_Int4*) (pointer + length);
            length += ALIGN(sizeof(sqlvar_ptr->hostindicator));
        }
        else {
            sqlvar_ptr->hostindaddr = &sqlvar_ptr -> hostindicator;
            desc_rec_ptr -> optimizeNullPtr = NULL;
            API_TRACE( API_TR_SDWORD,
                       "hostindicator",
                       &(sqlvar_ptr->hostindicator) );
        }; /* else */

        API_TRACE( API_TR_PTR, "optimizeNullPtr",
                   &desc_rec_ptr -> optimizeNullPtr);
        API_TRACE( API_TR_SWORD, "hostvartype", &(sqlvar_ptr->hostvartype) );
        API_TRACE( API_TR_PTR, "hostvaraddr", &(sqlvar_ptr->hostvaraddr) );
        API_TRACE( API_TR_PTR, "hostindaddr", &(sqlvar_ptr->hostindaddr) );

        if ((length + used) > buffer_size) {
            length += used;
            API_TRACE(API_TR_SDWORD, "length + used", &length);
            api_retcode = API_NOT_OK;
            break;
        }
        pointer += length;
        used += length;
    }; /* for */

    API_TRACE(API_TR_EXIT,"pa20SetAttr",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return (api_retcode);
} /* pa20SetAttr */


VOID pa20SetCount( tpa20Desc *descPtr, SWORD count )
{
    API_ASSERT( pa20VerifyDesc( descPtr ) == API_OK );

    descPtr->count = count;
} /* pa20SetCount */


VOID pa20SetAttrAddr( tpa20Desc *base_desc_ptr,
                      tpa20Desc *appl_desc_ptr,
                      tpa20Desc *impl_desc_ptr,
                      UWORD      number_cols,
                      SDWORD     bind_type,
                      SDWORD     pos,
                      UDWORD     irow )
{
    UWORD            col;
    SQLULEN          c_len;
    tpa20Desc       *base_desc;
    tpa21DescRecord *base_rec;
    tpa21DescRecord *target_appl_rec;
    tpa21DescRecord *target_impl_rec;

    API_TRACE( API_TR_ENTRY, "pa20SetAttrAddr", 0);
    API_TRACE( API_TR_PTR, "base_desc_ptr", &base_desc_ptr );
    API_TRACE( API_TR_PTR, "appl_desc_ptr", &appl_desc_ptr);
    API_TRACE( API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE( API_TR_UWORD, "number_cols", &number_cols);
    API_TRACE( API_TR_SDWORD, "bind_type", &bind_type);
    API_TRACE( API_TR_SDWORD, "pos", &pos);
    API_TRACE( API_TR_UDWORD, "irow", &irow);

    API_ASSERT_PTR( appl_desc_ptr );
    API_ASSERT_PTR( impl_desc_ptr );
    /* base_desc_ptr may be NULL */

    API_ASSERT( appl_desc_ptr->descType == API_DESC_TYPE_AD );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    if (base_desc_ptr)
        base_desc = base_desc_ptr;
    else
        base_desc = appl_desc_ptr;

    /* 0 is the bookmark column */
    for ( col=0; col <= number_cols; col++) {
        target_appl_rec = pa20GetRecord( appl_desc_ptr, col );
        API_ASSERT_PTR( target_appl_rec );
        target_impl_rec = pa20GetRecord( impl_desc_ptr, col );
        API_ASSERT_PTR( target_impl_rec );

        base_rec = pa20GetRecord( base_desc, col );
        API_ASSERT_PTR( base_rec );

        if (target_appl_rec->bound) {
            SWORD real_c_type = 0;

            /* get right c-length PTS 1104975 */
            apgstyc( base_rec->conciseType,
                     pa20_GetCLength( base_rec ),
                     target_impl_rec->type,
                     PA_NO_VALID_LEN,
                     PA_NO_VALID_LEN,
                     &real_c_type,
                     &c_len );
            if (irow) {
                if (irow > 1) {
                    target_appl_rec->dataPtr = (base_rec->dataPtr) ?
                        (base_rec->dataPtr + c_len) : NULL;
                    target_appl_rec->octetLengthPtr =
                        (base_rec->octetLengthPtr) ?
                        (base_rec->octetLengthPtr + (irow - 1)) : NULL;
                    target_appl_rec->indicatorPtr =
                        (base_rec->indicatorPtr) ?
                        (base_rec->indicatorPtr + (irow - 1)) : NULL;
                    target_impl_rec->movedLen = 0;
                    target_impl_rec->alreadyRead  = 0;
                }
            } else {
                target_impl_rec->movedLen = 0; /* PTS 1105391 */
                target_impl_rec->alreadyRead  = 0;
                target_appl_rec->dataPtr = (base_rec->dataPtr) ?
                    (base_rec->dataPtr +
                     (((bind_type) ?
                       (UDWORD) bind_type : c_len) * pos))
                    : NULL;
                target_appl_rec->octetLengthPtr =
                    (target_appl_rec->octetLengthPtr) ?
                    ((SQLLEN*)(((UCHAR*) (base_rec->octetLengthPtr)
                                + ((bind_type) ?
                                   (UDWORD) bind_type : sizeof(SQLLEN)) * pos)))
                    : NULL;
                target_appl_rec->indicatorPtr =
                    (target_appl_rec->indicatorPtr) ?
                    ((SQLLEN*)(((UCHAR*) (base_rec->indicatorPtr)
                                + ((bind_type) ?
                                   (UDWORD) bind_type : sizeof(SQLLEN)) * pos)))
                    : NULL;
            }
            API_TRACE(API_TR_SDWORD, "octetLength",
                      &target_appl_rec->octetLength);
            API_TRACE(API_TR_PTR, "dataPtr", &target_appl_rec->dataPtr);
            API_TRACE(API_TR_PTR,
                      "octetLengthPtr",
                      &target_appl_rec->octetLengthPtr);
        }; /* if */
    }; /* for */

    API_TRACE(API_TR_EXIT, "pa20SetAttrAddr", 0);

    return;
} /* pa20SetAttrAddr */


SQLRETURN pa20SetDescField( tpa20Desc *descPtr,
                            SWORD      recNo,
                            SWORD      fieldId,
                            PTR        valuePtr,
                            SDWORD     len,
                            UWORD     *sqlStatePtr,
                            const tsp77encoding *destEncoding)
{
    SQLRETURN  retcode = SQL_SUCCESS;
    UWORD      descType;

    tpa21DescRecord *recPtr;

    API_TRACE( API_TR_ENTRY, "pa20SetDescField", 0);
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );
    API_TRACE( API_TR_SWORD, "recNo", &recNo );
    API_TRACE( API_TR_SWORD, "fieldId", &fieldId);
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "len", &len);

    descType = pa20GetDescType( descPtr );
    switch (fieldId) {
        /* Header Fields */
    case SQL_DESC_ARRAY_SIZE:
        descPtr->header.arraySize = (UDWORD) VALUE_CAST (valuePtr);
        break;
    case SQL_DESC_ARRAY_STATUS_PTR:
        descPtr->header.arrayStatusPtr = (UWORD*) valuePtr;
        break;
    case SQL_DESC_BIND_OFFSET_PTR:
        descPtr->header.bindOffsetPtr = (SQLLEN*) valuePtr;
        break;
    case SQL_DESC_BIND_TYPE:
        descPtr->header.bindType = (SDWORD) VALUE_CAST (valuePtr);
        break;
    case SQL_DESC_COUNT:
        pa20ReallocRecords( descPtr, (UWORD) VALUE_CAST (valuePtr));
        descPtr->count = (SWORD) VALUE_CAST (valuePtr);
        break;
    case SQL_DESC_ROWS_PROCESSED_PTR:
        descPtr->header.rowsProcessedPtr = (UDWORD*) valuePtr;
        break;
    default: /* record field */
        if ((recNo == 0 && descType == API_DESC_TYPE_IPD)
            || (recNo < 0 && descType == API_DESC_TYPE_AD)
            || (recNo > csp_max_fields)) {
            retcode = SQL_ERROR;
            *sqlStatePtr = API_ODBC_07009; /* Invalid descriptor index */
        } else {
            if (recNo >= descPtr->allocRec) {
                /* realloc desc-records */
                if (pa20ReallocRecords( descPtr, (UWORD) recNo ) != API_OK) {
                    /* memory allocation failure */
                    *sqlStatePtr = API_ODBC_S1001;
                    break;
                }; /* if */
            }; /* if */
            recPtr = pa20GetRecord( descPtr, recNo );
            if (recPtr==NULL) {
                retcode = SQL_ERROR;
                *sqlStatePtr = API_ODBC_HY000; /* General error */
                API_TRACE( API_TR_MSG, "error -- recPtr==NULL", 0 );
            } else {
                switch (fieldId) {
                case SQL_DESC_CONCISE_TYPE:
                    if (descType == API_DESC_TYPE_AD) {
                        pa21SetCType( recPtr, (SWORD) VALUE_CAST (valuePtr) );
                    } else {
                        pa21SetSQLType( recPtr, (SWORD) VALUE_CAST (valuePtr) );
                    }; /* else */
                    break;
                case SQL_DESC_DATA_PTR:
                    switch (descType) {
                    case API_DESC_TYPE_IPD:
                        /* only make consistency check */
                        if (!pa20TypesAreConsistent( descPtr, recNo )) {
                            *sqlStatePtr = API_ODBC_HY021; /* Inconsistent
                                                            * descriptor
                                                            * information */
                        }; /* if */
                        break;
                    case API_DESC_TYPE_AD:
                        if (pa20TypesAreConsistent( descPtr, recNo )) {
                            recPtr->dataPtr = valuePtr;
                            recPtr->bound   = API_TRUE; /* is this ok ? */
                        } else {
                            *sqlStatePtr = API_ODBC_HY021; /* Inconsistent
                                                            * descriptor
                                                            * information */
                        }; /* else */
                        break;
                    default:
                        break;
                    }; /* switch */
                case SQL_DESC_DATETIME_INTERVAL_CODE:
                    recPtr->datetimeIntervalCode = (SWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_DATETIME_INTERVAL_PRECISION:
                    recPtr->datetimeIntervalPrecision = (SDWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_INDICATOR_PTR:
                    recPtr->indicatorPtr = (SQLLEN*) valuePtr;
                    break;
                case SQL_DESC_LENGTH:
                    recPtr->length = (UDWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_NAME: {
                  API_RETCODE api_retcode;
                  api_retcode = pa80AllocString (destEncoding,
                                                 &recPtr->name,
                                                 (UCHAR*) valuePtr,
                                                 len,
                                                 MX_IDENTIFIER);
                  if (api_retcode == API_TRUNCATE)
                    *sqlStatePtr = API_ODBC_22001; /* string data truncated */
                    break;
                }
                case SQL_DESC_NUM_PREC_RADIX:
                    recPtr->numPrecRadix = (SDWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_OCTET_LENGTH:
                    recPtr->octetLength = (SDWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_OCTET_LENGTH_PTR:
                    recPtr->octetLengthPtr = (SQLLEN*) valuePtr;
                    break;
                case SQL_DESC_PARAMETER_TYPE:
                    if ((SWORD) VALUE_CAST (valuePtr) == SQL_PARAM_INPUT
                        || (SWORD) VALUE_CAST (valuePtr) == SQL_PARAM_INPUT_OUTPUT
                        || (SWORD) VALUE_CAST (valuePtr) == SQL_PARAM_OUTPUT) {
                        recPtr->parameterType = (SWORD) VALUE_CAST (valuePtr);
                    } else {
                        retcode = SQL_ERROR;
                        *sqlStatePtr = API_ODBC_HY092; /* Invalid attribute/option
                                                        * identifier */
                    }; /* else */
                    break;
                case SQL_DESC_PRECISION:
                    recPtr->precision = (SWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_SCALE:
                    recPtr->scale = (SWORD) VALUE_CAST (valuePtr);
                    break;
                case SQL_DESC_TYPE:
                    if (descType == API_DESC_TYPE_AD) {
                        pa21SetCType( recPtr, (SWORD) VALUE_CAST (valuePtr) );
                    } else {
                        pa21SetSQLType( recPtr, (SWORD) VALUE_CAST (valuePtr) );
                    }; /* else */
                    break;
                case SQL_DESC_UNNAMED:
                    /* it is only allowed to set the field for an IPD
                 * to SQL_UNNAMED */
                    if ( (SWORD) VALUE_CAST (valuePtr) == SQL_UNNAMED) {
                        recPtr->unnamed = SQL_UNNAMED;
                    } else {
                        retcode = SQL_ERROR;
                        *sqlStatePtr = API_ODBC_HY091; /* Invalid descriptor
                                                        * field identifier */
                    }; /* else */
                    break;
                default:
                    /* invalid field id or read only field */
                    retcode = SQL_ERROR;
                    *sqlStatePtr = API_ODBC_HY091; /* Invalid descriptor
                                                * field identifier */
                    break;
                }; /* switch */
                break;
            }; /* else */
        }; /* else */
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa20SetDescField", 0);
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );

    return retcode;
} /* pa20SetDescField */


/* apmsmap() - API miscellaneous function, normally just copy */
/*             the data using apgetos; but, in certain        */
/*             special cases convert/split the data for the   */
/*             catalog accessing functions.                   */

API_RETCODE pa20SMap( tpa20Desc       *impl_desc_ptr,
                      UWORD            icol,
                      tpa62DBCSpecial *dbc_special_ptr,
                      SWORD            c_type,
                      UWORD            numericScale,
                      UCHAR           *c_data_ptr,
                      SQLLEN           c_data_max_len,
                      SQLLEN          *c_data_used_ptr )
{
    API_RETCODE  api_retcode = API_OK;
    UDWORD       column_definition;
    UWORD        type;
    UCHAR       *data_ptr;
    SWORD       *is_null_ptr;
    UDWORD       len;
    UWORD        scale;
    UDWORD       precision;
    UDWORD      *moved_len_ptr, prev_movedLen;
    UWORD        special;
    SWORD        sql_type;
    SWORD        real_c_type;
    SQLULEN      real_c_len;

    SQLINTEGER lc_data_used, *lc_data_used_ptr;
    SQLINTEGER lc_data_max_len = (SQLINTEGER) c_data_max_len;

    tpa21DescRecord *impl_rec_ptr;

    API_TRACE( API_TR_ENTRY,"pa20SMap",0);
    API_TRACE( API_TR_PTR,"impl_desc_ptr",&impl_desc_ptr);
    API_TRACE( API_TR_UWORD,"icol",&icol);
    API_TRACE( API_TR_UWORD,"special",&dbc_special_ptr->special);
    special = dbc_special_ptr->special;
    API_TRACE( API_TR_SWORD,"c_type",&c_type);
    API_TRACE( API_TR_PTR,"c_data_ptr",&c_data_ptr);
    API_TRACE( API_TR_SDWORD,"c_data_max_len",&lc_data_max_len);
    API_TRACE( API_TR_PTR,"c_data_used_ptr",&c_data_used_ptr);

    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    /* map SQLLEN and SQLUINTEGER (UDWORD) */
    if (c_data_used_ptr == NULL)
      lc_data_used_ptr = NULL;
    else
      lc_data_used_ptr = &lc_data_used;

    /* check for SQL_SETPARAM_VALUE_MAX */
    if (c_data_max_len == SQL_SETPARAM_VALUE_MAX)
      lc_data_max_len = MAX_INT4_SP00;        /* 2^31 - 1 */

    /* get the location of data and attributes */

    API_ASSERT( icol > 0 );
    impl_rec_ptr = pa20GetRecord( impl_desc_ptr, icol );
    API_ASSERT_PTR( impl_rec_ptr );

    type          = impl_rec_ptr -> esqType;
    sql_type      = impl_rec_ptr -> type;
    len           = impl_rec_ptr -> esqLength;
    precision     = impl_rec_ptr -> esqPrecision;
    scale         = impl_rec_ptr -> esqScale;
    data_ptr      = impl_rec_ptr -> optimizePtr;
    is_null_ptr   = &( impl_rec_ptr -> isNull );
    moved_len_ptr = &( impl_rec_ptr -> movedLen);
    prev_movedLen = impl_rec_ptr -> movedLen;

    API_TRACE(API_TR_UWORD, "type", &type);
    API_TRACE(API_TR_SWORD, "sql_type", &sql_type);
    API_TRACE(API_TR_UDWORD, "len", &len);
    API_TRACE(API_TR_UDWORD, "precision", &precision);
    API_TRACE(API_TR_UWORD, "scale", &scale);
    API_TRACE(API_TR_UDWORD, "moved_len", moved_len_ptr);

    /* determine column-definition: */
    /* depending on esqType        */
    column_definition = len;
    if (apgisindi( impl_rec_ptr -> optimizeNullPtr)) {
        if (*moved_len_ptr == SQL_NULL_DATA) {
            api_retcode = API_NO_DATA_FOUND;
        }
        else {
            api_retcode = API_OK;
            if (c_data_used_ptr != NULL) {
                *c_data_used_ptr = SQL_NULL_DATA;
                if (c_data_ptr) {
                    apgstyc( c_type,
                             0,
                             sql_type,
                             PA_NO_VALID_LEN,
                             PA_NO_VALID_LEN,
                             (SWORD FAR *) &real_c_type,
                             &real_c_len );
                    if (real_c_type == SQL_C_CHAR)
                        *c_data_ptr = 0;
                }; /* if */
            }; /* if */
        }; /* else */
    }
    else {
        UDWORD odbcVersion = 0;
        pa10GetODBCVersion( SQL_HANDLE_DESC,
                            (SQLHDESC) impl_desc_ptr,
                            &odbcVersion );
        switch (special) {
        case (API_SPEC_SQLCOLUMNS): {
            if (odbcVersion == SQL_OV_ODBC3) {
                api_retcode = pa20MapDataType( impl_desc_ptr,
                                               icol,
                                               COTYPE_NAME,
                                               CODATA_TYPE,
                                               COSQL_DATA_TYPE,
                                               COPRECISION,
                                               COLENGTH,
                                               COCODETYPE_35 );
            } else {
                api_retcode = pa20MapDataType( impl_desc_ptr,
                                               icol,
                                               COTYPE_NAME,
                                               CODATA_TYPE,
                                               0,
                                               COPRECISION,
                                               COLENGTH,
                                               COCODETYPE );
            };
            break;
        }
        case (API_SPEC_SQLPROCEDURESCOLUMNS): {
            if (odbcVersion == SQL_OV_ODBC3) {
                api_retcode = pa20MapDataType( impl_desc_ptr,
                                               icol,
                                               PCOLTYPE_NAME,
                                               PCOLDATA_TYPE,
                                               PCOLSQL_DATA_TYPE,
                                               PCOLPRECISION,
                                               PCOLLENGTH,
                                               PCOLCODETYPE_35 );
            } else {
                api_retcode = pa20MapDataType( impl_desc_ptr,
                                               icol,
                                               PCOLTYPE_NAME,
                                               PCOLDATA_TYPE,
                                               0,
                                               PCOLPRECISION,
                                               PCOLLENGTH,
                                               PCOLCODETYPE );
            };
            break;
        }
        case (API_SPEC_SQLSPECIALCOLUMNS): {
            api_retcode = pa20MapDataType( impl_desc_ptr,
                                           icol,
                                           SPTYPE_NAME,
                                           SPDATA_TYPE,
                                           0,
                                           SPPRECISION,
                                           SPLENGTH,
                                           SPCODETYPE );
            break;
        }
        default:{
            break;
        }
        }

        /* quick&dirty: cmp vpa04gc
           ascii long columns cannot distinguished from unicode long columns
           in the converting routines. Therefore the data is converted here. */
        if (type == dstruni && sql_type == SQL_LONGVARCHAR &&
            c_type == SQL_C_BINARY && pa01GetUnicodeCatalog () != 0)
          {
            unsigned int destOut;
            UCHAR *p = apgvstraddr (data_ptr);
            tsp00_Int4 len = apggvstrl (data_ptr);

            sp81UCS2toASCII (p, len, &destOut,
                             (tsp81_UCS2Char*) p, len / sizeof (SQLWCHAR),
                             sp77nativeUnicodeEncoding () == sp77encodingUCS2Swapped);

            len = destOut;
            apgsvstrl (data_ptr, len);
          }

        if (c_data_ptr != NULL) {          /* PTS 1113705 */
          api_retcode = apgetoc( type,
                                 data_ptr,
                                 column_definition,
                                 scale,
                                 moved_len_ptr,
                                 c_type,
                                 numericScale,
                                 c_data_ptr,
                                 lc_data_max_len,
                                 lc_data_used_ptr );
          /* map SQLLEN and SQLUINTEGER (UDWORD) */
          if (c_data_used_ptr != NULL) {
            *c_data_used_ptr = *lc_data_used_ptr;
          }
          /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
          impl_rec_ptr->alreadyRead += pa20_ComputeCharacterLength (*moved_len_ptr - prev_movedLen,
                                                                    impl_rec_ptr,
                                                                    c_type);
          /* the difference takes added zero termination into account
             in case of SQL_CHAR. PTS 1119132 */
        }
    }
    API_TRACE( API_TR_EXIT, "pa20SMap",0);
    API_TRACE( API_TR_API_RETCODE, "api_retcode", &api_retcode);

    return(api_retcode);

} /* pa20SMap */


API_RETCODE pa20TypesAreConsistent( tpa20Desc *descPtr, SWORD recNo )
{
    API_RETCODE apiRetcode = API_OK;
    tpa21DescRecord *recPtr;

    API_ASSERT( pa20VerifyDesc( descPtr ) == API_OK );

    recPtr = pa20GetRecord( descPtr, recNo );
    API_ASSERT_PTR( recPtr );

    switch (recPtr->type) {
    case API_DESC_TYPE_AD:
        if ( pa20_IsConciseCType( recPtr->conciseType )
             && pa20_IsCType( recPtr->type ) ) {
            /* for numeric datatypes check precision and scale */
            /* ... */
            apiRetcode = API_OK;
        } else {
            apiRetcode = API_NOT_OK;
        }; /* else */
        break;
    case API_DESC_TYPE_IRD:
    case API_DESC_TYPE_IPD:
        if ( pa20_IsConciseSqlType( recPtr->conciseType )
             && pa20_IsSqlType( recPtr->type ) ) {
            switch (recPtr->type) {
/* intervall datatypes are not supported */
#if 0
            case SQL_DATETIME:
                switch (recPtr->datetimeIntervalCode) {
                case SQL_CODE_DATE:
                    apiRetcode = API_OK;
                    break;
                case SQL_CODE_TIME:
                case SQL_CODE_TIMESTAMP:
                    /* check precision ... */
                    apiRetcode = API_OK;
                    break;
                default:
                    apiRetcode = API_NOT_OK;
                    break;
                }; /* switch */
#endif
            case SQL_NUMERIC:
                /* check precision and scale ... */
                apiRetcode = API_OK;
                break;
            default:
                apiRetcode = API_OK;
                break;
            }; /* switch */
        } else {
            apiRetcode = API_NOT_OK;
        }; /* else */
        break;
    }; /* switch */

    return apiRetcode;
} /* pa20TypesAreConsistent */


VOID pa20Unset( tpa20Desc *impl_desc_ptr,
                UWORD      attr_array_cnt )
{
    tpa21DescRecord *impl_rec_ptr;
    UWORD i;

    API_TRACE(API_TR_ENTRY, "pa20Unset", 0);
    API_TRACE(API_TR_PTR, "impl_desc_ptr", &impl_desc_ptr);
    API_TRACE(API_TR_UWORD, "attr_array_cnt", &attr_array_cnt);

    API_ASSERT_PTR( impl_desc_ptr );
    API_ASSERT( impl_desc_ptr->descType == API_DESC_TYPE_IRD
                || impl_desc_ptr->descType == API_DESC_TYPE_IPD );

    for ( i=1; i <= attr_array_cnt; i++ ) {
        impl_rec_ptr = pa20GetRecord( impl_desc_ptr, i );
        if (impl_rec_ptr->longHndl)
            apdunlk(impl_rec_ptr->longHndl);
    }; /* for */

    API_TRACE(API_TR_EXIT,"pa20Unset",0);
} /* pa20Unset */


API_RETCODE pa20VerifyDesc( SQLHDESC hdesc )
{
  tpa20Desc *descPtr;
  API_RETCODE apiRetcode = API_OK;

  API_TRACE( API_TR_ENTRY, "pa20VerifyDesc", 0);
  API_TRACE( API_TR_HANDLE, "hdesc", &hdesc);

  descPtr = (tpa20Desc*) hdesc;
  if ((descPtr == NULL) || ( descPtr->handleType != API_DESC_TYPE )) {
      apiRetcode = API_NOT_OK;
  }; /* if */

  API_TRACE( API_TR_EXIT, "pa20VerifyDesc", 0);
  API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode);

  return apiRetcode;
} /* pa20VerifyDesc */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/


API_RETCODE pa20_ConsistentCopyDesc( tpa20Desc *inDesc, tpa20Desc *outDesc )
{
    API_RETCODE      apiRetcode;
    SWORD            targetAllocType;
    UWORD            targetDescType;
    SQLHDBC          parentHdbc;

    /* save the descriptor type and parentDBC */
    targetAllocType = outDesc->allocType;
    targetDescType  = outDesc->descType;
    parentHdbc      = outDesc->parentDBC;

    apiRetcode = pa20CopyDesc( inDesc, outDesc );

    /* initialize long handles */
    pa20InitLongHandles( outDesc );

    /* restore the descriptor type */
    outDesc->allocType  = targetAllocType;
    outDesc->descType   = targetDescType;
    outDesc->parentDBC  = parentHdbc;

    return apiRetcode;
} /* pa20_ConsistentCopyDesc */


API_RETCODE pa20_FreeRecords( tpa20Desc *descPtr )
{
    API_RETCODE apiRetcode = API_OK;

    API_TRACE( API_TR_ENTRY, "pa20_FreeRecords", 0 );
    API_TRACE( API_TR_PTR, "descPtr", &descPtr );

    API_ASSERT_PTR( descPtr );
    if (descPtr->records != NULL)
      apdfree( descPtr->records );
    descPtr->records  = NULL;
    /* descPtr->count    = 0; */
    descPtr->allocRec = 0;

    API_TRACE( API_TR_EXIT, "pa20_FreeRecords", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20_FreeRecords */


UWORD pa20_GetAccessRights( SWORD fieldId, UWORD descType )
{
    UWORD accessRights = API_ACCESS_READ_WRITE;

    API_TRACE( API_TR_ENTRY, "pa20_GetAccessRights", 0 );
    API_TRACE( API_TR_SWORD, "fieldId", &fieldId );
    API_TRACE( API_TR_UWORD, "descType", &descType );

    if (fieldId==SQL_DESC_ALLOC_TYPE) { /* read only for all */
        accessRights = API_ACCESS_READ_ONLY;
    } else if (descType==API_DESC_TYPE_IRD) {
        switch (fieldId) {
            /* Header fields */
        case SQL_DESC_ARRAY_STATUS_PTR:
        case SQL_DESC_ROWS_PROCESSED_PTR:
            accessRights = API_ACCESS_READ_WRITE;
            break;
        case SQL_DESC_ARRAY_SIZE:
        case SQL_DESC_BIND_OFFSET_PTR:
        case SQL_DESC_BIND_TYPE:
            /* Record fields */
        case SQL_DESC_DATA_PTR:
        case SQL_DESC_INDICATOR_PTR:
        case SQL_DESC_OCTET_LENGTH_PTR:
        case SQL_DESC_PARAMETER_TYPE:
            accessRights = API_ACCESS_UNUSED;
            break;
        default: /* most fields are read only */
            accessRights = API_ACCESS_READ_ONLY;
            break;
        }; /* switch */
    } else if (descType==API_DESC_TYPE_IPD) {
        switch (fieldId) {
            /* Header fields */
        case SQL_DESC_ARRAY_STATUS_PTR:
        case SQL_DESC_COUNT:
        case SQL_DESC_ROWS_PROCESSED_PTR:
            /* Record fields */
        case SQL_DESC_CONCISE_TYPE:
        case SQL_DESC_DATETIME_INTERVAL_CODE:
        case SQL_DESC_DATETIME_INTERVAL_PRECISION:
        case SQL_DESC_LENGTH:
        case SQL_DESC_NAME:
        case SQL_DESC_NUM_PREC_RADIX:
        case SQL_DESC_OCTET_LENGTH:
        case SQL_DESC_PARAMETER_TYPE:
        case SQL_DESC_PRECISION:
        case SQL_DESC_SCALE:
        case SQL_DESC_TYPE:
        case SQL_DESC_UNNAMED:
            accessRights = API_ACCESS_READ_WRITE;
            break;
        case SQL_DESC_CASE_SENSITIVE:
        case SQL_DESC_FIXED_PREC_SCALE:
        case SQL_DESC_LOCAL_TYPE_NAME:
        case SQL_DESC_NULLABLE:
        case SQL_DESC_ROWVER:
        case SQL_DESC_TYPE_NAME:
        case SQL_DESC_UNSIGNED:
            accessRights = API_ACCESS_READ_ONLY;
            break;
        default: /* most fields are unused */
            accessRights = API_ACCESS_UNUSED;
            break;
        }; /* switch */
    } else { /* APD or ARD or AD */
        switch (fieldId) {
            /* Header fields */
        case SQL_DESC_ARRAY_SIZE:
        case SQL_DESC_ARRAY_STATUS_PTR:
        case SQL_DESC_BIND_OFFSET_PTR:
        case SQL_DESC_BIND_TYPE:
        case SQL_DESC_COUNT:
            /* Record fields */
        case SQL_DESC_CONCISE_TYPE:
        case SQL_DESC_DATA_PTR:
        case SQL_DESC_DATETIME_INTERVAL_CODE:
        case SQL_DESC_DATETIME_INTERVAL_PRECISION:
        case SQL_DESC_INDICATOR_PTR:
        case SQL_DESC_LENGTH:
        case SQL_DESC_NUM_PREC_RADIX:
        case SQL_DESC_OCTET_LENGTH:
        case SQL_DESC_OCTET_LENGTH_PTR:
        case SQL_DESC_PRECISION:
        case SQL_DESC_SCALE:
        case SQL_DESC_TYPE:
            accessRights = API_ACCESS_READ_WRITE;
            break;
        default: /* most fields are unused */
            accessRights = API_ACCESS_UNUSED;
            break;
        }; /* switch */
    }; /* else */

    API_TRACE( API_TR_EXIT, "pa20_GetAccesRights", 0);
    API_TRACE( API_TR_UWORD, "accessRights", &accessRights);

    return accessRights;
} /* pa20_GetAccessRights */


SWORD pa20_GetConciseType( UDWORD odbcVersion, SWORD type )
{
    SWORD conciseType = type;

    if (odbcVersion >= SQL_OV_ODBC3) {
        switch (type) {
        case SQL_DATE:
            conciseType = SQL_TYPE_DATE;
            break;
        case SQL_TIME:
            conciseType = SQL_TYPE_TIME;
            break;
        case SQL_TIMESTAMP:
            conciseType = SQL_TYPE_TIMESTAMP;
            break;
        }; /* switch */
    }; /* if */

    return conciseType;
} /* pa20_GetConciseType */


SDWORD pa20_GetDisplaySize( tpa21DescRecord *irdRecPtr )
{
    SDWORD displaySize = 0;

    API_ASSERT_PTR( irdRecPtr );
    API_ASSERT( implRecPtr->descType == API_DESC_TYPE_IRD );

    switch (irdRecPtr -> type) {
    case (SQL_LONGVARCHAR): {}
    case (SQL_WLONGVARCHAR): {}
    case (SQL_LONGVARBINARY): {
        displaySize = SAPDB_MAX_INT4;
        break;
    }
    case (SQL_SMALLINT): {
        displaySize = API_SMALLINT_DISP;
        break;
    }
    case (SQL_INTEGER): {
        displaySize = API_INTEGER_DISP;
        break;
    }
    case (SQL_FLOAT): {
        displaySize = API_FLOAT_DISP;
        break;
    }
    case (SQL_REAL): {
        displaySize = API_REAL_DISP;
        break;
    }
    case (SQL_DOUBLE): {
        displaySize = API_DOUBLE_DISP;
        break;
    }
    case (SQL_DECIMAL): {}
    case (SQL_NUMERIC): {
        displaySize = irdRecPtr -> esqPrecision + 2;
        break;
    }
    case (SQL_TYPE_DATE):
    case (SQL_DATE): {
        displaySize = API_DATE_DISP;
        break;
    }
    case (SQL_TYPE_TIME): {
        displaySize = API_TIME_DISP;
        break;
    }
    case (SQL_TYPE_TIMESTAMP): {
        displaySize = API_TIMESTMP_EXT_DISP;    /* PTS 1118224 */
        /* timestamp has always a fraction of 6 digits */
        break;
    }
    case (SQL_BIT): {
        displaySize = API_BIT_DISP;
        break;
    }
    case (SQL_TINYINT): {
        displaySize = API_TINYINT_DISP;
        break;
    }
    case (SQL_BIGINT): {
        displaySize = API_BIGINT_DISP;
        break;
    }
    default: {
        displaySize = irdRecPtr -> esqPrecision;
        if (irdRecPtr->type == SQL_WCHAR ||
            irdRecPtr->type == SQL_WVARCHAR ||
            irdRecPtr->type == SQL_WLONGVARCHAR)
          displaySize /= sizeof (SQLWCHAR);

        break;
    }
    }; /* switch */

    return displaySize;
} /* pa20_GetDisplaySize */

SDWORD pa20_GetDescLength( tpa21DescRecord *irdRecPtr )
{
    SDWORD descSize = 0;

    API_ASSERT_PTR( irdRecPtr );
    API_ASSERT( implRecPtr->descType == API_DESC_TYPE_IRD );

    switch (irdRecPtr -> type) {
    case (SQL_LONGVARCHAR): {}
    case (SQL_LONGVARBINARY): {
        descSize = SAPDB_MAX_INT4;
        break;
    }
    case (SQL_SMALLINT): {
        descSize = API_SMALLINT_DESC;
        break;
    }
    case (SQL_INTEGER): {
        descSize = API_INTEGER_DESC;
        break;
    }
    case (SQL_FLOAT): {
        descSize = API_FLOAT_DESC;
        break;
    }
    case (SQL_REAL): {
        descSize = API_REAL_DESC;
        break;
    }
    case (SQL_DOUBLE): {
        descSize = API_DOUBLE_DESC;
        break;
    }
    case (SQL_DECIMAL): {}
    case (SQL_NUMERIC): {
        descSize = irdRecPtr -> esqPrecision + 2;
        break;
    }
    case (SQL_TYPE_DATE):
    case (SQL_DATE): {
        descSize = API_DATE_DESC;
        break;
    }
    case (SQL_TYPE_TIME): {
        descSize = API_TIME_DESC;
        break;
    }
    case (SQL_TYPE_TIMESTAMP): {
        if (irdRecPtr -> esqScale > 0)
            descSize = API_TIMESTMP_DESC;
        else
            descSize = API_TIMESTMP_DESC + irdRecPtr -> esqScale;
        break;
    }
    case (SQL_BIT): {
        descSize = API_BIT_DESC;
        break;
    }
    case (SQL_TINYINT): {
        descSize = API_TINYINT_DESC;
        break;
    }
    case (SQL_BIGINT): {
        descSize = API_BIGINT_DESC;
        break;
    }
    default: {
        descSize = irdRecPtr -> esqPrecision;
        if (irdRecPtr->type == SQL_WCHAR ||
            irdRecPtr->type == SQL_WVARCHAR ||
            irdRecPtr->type == SQL_WLONGVARCHAR)
          descSize /= sizeof (SQLWCHAR);

        break;
    }
    }; /* switch */

    return descSize;
} /* pa20_GetDescLength */


UDWORD pa20_GetCLength( tpa21DescRecord *applRecPtr )
{
    SQLLEN cLength = 0;

    API_ASSERT( implRecPtr->descType == API_DESC_TYPE_AD );

    switch (applRecPtr->conciseType) {
    case (SQL_C_BINARY):
    case (SQL_C_CHAR):
        cLength = applRecPtr->octetLength;
        break;
    case (SQL_C_SLONG):
        cLength = sizeof(SDWORD);
        break;
    case (SQL_C_ULONG):
        cLength = sizeof(SDWORD);
        break;
    case (SQL_C_LONG):
        cLength = sizeof(SDWORD);
        break;
    case (SQL_C_SSHORT):
        cLength = sizeof(SWORD);
        break;
    case (SQL_C_USHORT):
        cLength = sizeof(UWORD);
        break;
    case (SQL_C_SHORT):
        cLength = sizeof(SWORD);
        break;
    case (SQL_C_FLOAT):
        cLength = sizeof(SQLREAL);
        break;
    case (SQL_C_DOUBLE):
        cLength = sizeof(SQLDOUBLE);
        break;
    case (SQL_C_TYPE_DATE):
    case (SQL_C_DATE):
        cLength = sizeof(DATE_STRUCT);
        break;
    case (SQL_C_TIME):
    case (SQL_C_TYPE_TIME):
        cLength = sizeof(TIME_STRUCT);
        break;
    case (SQL_C_TIMESTAMP):
    case (SQL_C_TYPE_TIMESTAMP):
        cLength = sizeof(TIMESTAMP_STRUCT);
        break;
    case (SQL_C_BIT):
        cLength = sizeof(SQLCHAR);
        break;
    case (SQL_C_STINYINT):
        cLength = sizeof(SQLSCHAR);
        break;
    case (SQL_C_UTINYINT):
        cLength = sizeof(SQLCHAR);
        break;
    case (SQL_C_TINYINT):
        cLength = sizeof(SQLCHAR);
        break;
#ifdef BIGINT_SUPPORTED
    case (SQL_C_SBIGINT):
        cLength = sizeof(SQLBIGINT);
        break;
    case (SQL_C_UBIGINT):
        cLength = sizeof(SQLUBIGINT);
        break;
#endif /* BIGINT_SUPPORTED */
    case (SQL_C_NUMERIC):
        cLength = sizeof(SQL_NUMERIC_STRUCT);
        break;
    case (SQL_C_ABAPHANDLE):
        cLength = sizeof(SQLStreamDesc);
        break;
    case (SQL_C_DEFAULT):
        cLength = applRecPtr->octetLength;
        break;
        /*
         * SQL_C_BOOKMARK == SQL_C_ULONG
         * SQL_C_VARBOOKMARK == SQL_C_BINARY */

        /* ODBC 3.0 interval types */
        /*
    case SQL_C_INTERVAL_MONTH:
    case SQL_C_INTERVAL_YEAR:
    case SQL_C_INTERVAL_YEAR_TO_MONTH:
    case SQL_C_INTERVAL_DAY:
    case SQL_C_INTERVAL_HOUR:
    case SQL_C_INTERVAL_MINUTE:
    case SQL_C_INTERVAL_SECOND:
    case SQL_C_INTERVAL_DAY_TO_HOUR:
    case SQL_C_INTERVAL_DAY_TO_MINUTE:
    case SQL_C_INTERVAL_DAY_TO_SECOND:
    case SQL_C_INTERVAL_HOUR_TO_MINUTE:
    case SQL_C_INTERVAL_HOUR_TO_SECOND:
    case SQL_C_INTERVAL_MINUTE_TO_SECOND:
    */
    default:
        cLength = applRecPtr->octetLength;
    }; /* switch */

    return (UDWORD) cLength;       /* ia 64 cast */
} /* GetCLength */


SDWORD pa20_GetNumPrecRadix( SWORD sqlType )
{
    SDWORD numPrecRadix = 0;

    switch (sqlType) {
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_INTEGER:
    case SQL_SMALLINT:
        numPrecRadix = 10;
        break;
    case SQL_FLOAT:
    case SQL_REAL:
    case SQL_DOUBLE:
        numPrecRadix = 2;
        break;
    default:
        numPrecRadix = 0;
        break;
    }; /* switch */

    return numPrecRadix;
} /* pa20_GetNumPrecRadix */



UDWORD pa20_GetSQLLength( tpa21DescRecord *implRecPtr )
{
    UDWORD sqlLength = 0;

    API_ASSERT_PTR( implRecPtr );
    API_ASSERT( implRecPtr->descType == API_DESC_TYPE_IPD
                || implRecPtr->descType == API_DESC_TYPE_IRD );

    switch (implRecPtr -> type) {
    case (SQL_LONGVARCHAR): {}
    case (SQL_WLONGVARCHAR): {}
    case (SQL_LONGVARBINARY): {
        sqlLength = (UDWORD) SAPDB_MAX_INT4;
        break;
    }
    case (SQL_SMALLINT): {
        sqlLength = API_SMALLINT_LEN;
        break;
    }
    case (SQL_INTEGER): {
        sqlLength = API_INTEGER_LEN;
        break;
    }
    case (SQL_DECIMAL): {}
    case (SQL_NUMERIC): {
        sqlLength = implRecPtr -> esqPrecision + 2;
        break;
    }
    case (SQL_FLOAT): {
        sqlLength = API_FLOAT_LEN;
        break;
    }
    case (SQL_REAL): {
        sqlLength = API_REAL_LEN;
        break;
    }
    case (SQL_DOUBLE): {
        sqlLength = API_DOUBLE_LEN;
        break;
    }
    case (SQL_TYPE_DATE):
    case (SQL_DATE): {
        sqlLength = API_DATE_LEN;
        break;
    }
    case (SQL_TIME):
    case (SQL_TYPE_TIME): {
        sqlLength = API_TIME_LEN;
        break;
    }
    case (SQL_TIMESTAMP):
    case (SQL_TYPE_TIMESTAMP): {
        sqlLength = API_TIMESTMP_LEN;
        break;
    }
    case (SQL_BIGINT): {
        sqlLength = API_BIGINT_LEN;
        break;
    }
    case (SQL_BIT): {
        sqlLength = API_BIT_LEN;
        break;
    }
    case (SQL_TINYINT): {
        sqlLength = API_TINYINT_LEN;
        break;
    }
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
    case (SQL_WCHAR): {}
    case (SQL_WVARCHAR): {
        sqlLength = implRecPtr -> esqPrecision / sizeof (SQLWCHAR);
        break;
    }
    default: {
        sqlLength = implRecPtr -> esqPrecision;
        break;
    }
    }; /* switch */

    return sqlLength;
} /* pa20_GetSQLLength */


SWORD pa20_GetSQLPrec( tpa21DescRecord *implRecPtr )
{
    SWORD prec = 0;
    switch (implRecPtr -> type) {
    case (SQL_LONGVARCHAR):
    case (SQL_WLONGVARCHAR):
    case (SQL_LONGVARBINARY):
        prec = (SWORD) -1;     /* PTS 1119281 LONG_MAX; */
        break;
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
    case (SQL_WCHAR):          /* only to avoid differences in tests */
    case (SQL_WVARCHAR):
        prec = (SWORD) implRecPtr -> esqPrecision / sizeof (SQLWCHAR);
        break;
    default:
        prec = (SWORD) implRecPtr -> esqPrecision;
        break;
    }; /* switch */

    return prec;
} /* pa20_GetSQLPrec */


UCHAR* pa20_GetTypeName( tpa21DescRecord *implRecPtr, int old )
{
    UCHAR *typeName;

    API_ASSERT_PTR( implRecPtr );

    switch (implRecPtr -> esqType) {
    case (dcha): {}
    case (dche): {
        typeName = (UCHAR FAR *)API_CHAR_NAME;
        break;
    }
    case (dunicode): {
      if (old)
        typeName = (UCHAR FAR *)API_CHAR_UNICODE_NAME_OLD;
      else
        typeName = (UCHAR FAR *)API_CHAR_UNICODE_NAME;
        break;
    }
    case (dvarchara): {}
    case (dvarchare): {
        typeName = (UCHAR FAR *)API_VARCHAR_NAME;
        break;
    }
    case (dvarcharuni): {
      if (old)
        typeName = (UCHAR FAR *)API_VARCHAR_UNICODE_NAME_OLD;
      else
        typeName = (UCHAR FAR *)API_VARCHAR_UNICODE_NAME;
        break;
    }
    case (dstra): {}
    case (dstre): {}
    case (dlonga): {}
    case (dlonge): {
        typeName = (UCHAR FAR *)API_LONGVARCHAR_NAME;
        break;
    }
    case (dstruni): {}
    case (dlonguni): {
        typeName = (UCHAR FAR *)API_LONGVARCHAR_UNICODE_NAME;
        break;
    }
    case (dchb): {}
    case (ddbyteebcdic): {
      if (old)
        typeName = (UCHAR FAR *)API_BINARY_NAME_OLD;
      else
        typeName = (UCHAR FAR *)API_BINARY_NAME;
        break;
    }
    case (dvarcharb): {
      if (old)
        typeName = (UCHAR FAR *)API_VARBINARY_NAME_OLD;
      else
        typeName = (UCHAR FAR *)API_VARBINARY_NAME;
        break;
    }
    case (dstrb): {}
    case (dstrdb): {}
    case (dlongb): {}
    case (dlongdb): {
        typeName = (UCHAR FAR *)API_LONGVARBINARY_NAME;
        break;
    }
    case (dinteger): {
        typeName = (UCHAR FAR *)API_INTEGER_NAME;
        break;
    }
    case (dsmallint): {
        typeName = (UCHAR FAR *)API_SMALLINT_NAME;
        break;
    }
    case (dfloat): {
        if (implRecPtr -> esqLength > sizeof(SFLOAT)) {
            typeName = (UCHAR FAR *)API_FLOAT_NAME;
        }
        else {
            typeName = (UCHAR FAR *)API_REAL_NAME;
        }
        break;
    }
    case (dfixed): {
        typeName = (UCHAR FAR *)API_DECIMAL_NAME;
        break;
    }
    case (ddate): {
        typeName = (UCHAR FAR *)API_DATE_NAME;
        break;
    }
    case (dtime): {
        typeName = (UCHAR FAR *)API_TIME_NAME;
        break;
    }
    case (dtimestamp): {
        typeName = (UCHAR FAR *)API_TIMESTAMP_NAME;
        break;
    }
    case (dboolean): {
        typeName = (UCHAR FAR *)API_BIT_NAME;
        break;
    }
    default: {
        typeName = (UCHAR*) "";
        break;
    }
    }; /* switch */

    return typeName;
} /* pa20_GetTypeName */


SWORD pa20_IsCaseSensitive( tpa21DescRecord *implRecPtr )
{
    SWORD isCaseSensitive = SQL_FALSE;

    API_ASSERT_PTR( implRecPtr );

    switch (implRecPtr -> esqType) {
    case (dchb): {}
    case (dcha): {}
    case (dche): {}
    case (dstrb): {}
    case (dstrdb): {}
    case (dlongb): {}
    case (dlongdb): {}
    case (dstra): {}
    case (dstre): {}
    case (dlonga): {}
    case (dlonge): {}
    case (ddbyteebcdic): {}
    case (dvarchara): {}
    case (dvarchare): {}
    case (dvarcharb): {}
    case (dunicode): {}
    case (dstruni): {}
    case (dvarcharuni): {
      /* added for UCS2 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
        isCaseSensitive = API_TRUE;
        break;
    }
    default: {
        isCaseSensitive = API_FALSE;
            break;
    }
    }; /* switch */

    return isCaseSensitive;
} /* pa20_IsCaseSensitive */


UWORD pa20_IsConciseCType( SWORD type )
{
    return pa20_IsCType( type );
} /* pa20_IsConciseCType */


UWORD pa20_IsConciseSqlType( SWORD type )
{
    UWORD isSqlType;

    switch (type) {
    case SQL_DATETIME:
    case SQL_INTERVAL:
        isSqlType = API_TRUE;
        break;
    default:
        isSqlType = pa20_IsSqlType( type );
        break;
    }; /* switch */

    return isSqlType;
} /* pa20_IsConciseSqlType */


UWORD pa20_IsCType( SWORD type )
{
    UWORD isCType;

    API_TRACE( API_TR_ENTRY, "pa20_IsCType", 0 );
    API_TRACE( API_TR_SWORD, "type", &type );

    switch (type) {
    case (SQL_C_CHAR):
    case (SQL_C_SLONG):
    case (SQL_C_ULONG):
    case (SQL_C_LONG):
    case (SQL_C_SSHORT):
    case (SQL_C_USHORT):
    case (SQL_C_SHORT):
    case (SQL_C_FLOAT):
    case (SQL_C_DOUBLE):
    case (SQL_C_TYPE_DATE):
    case (SQL_C_TYPE_TIME):
    case (SQL_C_TYPE_TIMESTAMP):
    case (SQL_C_DATE):
    case (SQL_C_TIME):
    case (SQL_C_TIMESTAMP):
    case (SQL_C_BINARY):
    case (SQL_C_BIT):
    case (SQL_C_STINYINT):
    case (SQL_C_UTINYINT):
    case (SQL_C_TINYINT):
#ifdef BIGINT_SUPPORTED
    case (SQL_C_SBIGINT):
    case (SQL_C_UBIGINT):
#endif /* BIGINT_SUPPORTED */
    case (SQL_C_DEFAULT):
    case (SQL_C_NUMERIC):
        /*
         * SQL_C_BOOKMARK == SQL_C_ULONG
         * SQL_C_VARBOOKMARK == SQL_C_BINARY */

        /* ODBC 3.0 interval types */
    case SQL_C_INTERVAL_MONTH:
    case SQL_C_INTERVAL_YEAR:
    case SQL_C_INTERVAL_YEAR_TO_MONTH:
    case SQL_C_INTERVAL_DAY:
    case SQL_C_INTERVAL_HOUR:
    case SQL_C_INTERVAL_MINUTE:
    case SQL_C_INTERVAL_SECOND:
    case SQL_C_INTERVAL_DAY_TO_HOUR:
    case SQL_C_INTERVAL_DAY_TO_MINUTE:
    case SQL_C_INTERVAL_DAY_TO_SECOND:
    case SQL_C_INTERVAL_HOUR_TO_MINUTE:
    case SQL_C_INTERVAL_HOUR_TO_SECOND:
    case SQL_C_INTERVAL_MINUTE_TO_SECOND:
        isCType = API_TRUE;
        break;
    default:
        isCType = API_FALSE;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa20_IsCType", 0);
    API_TRACE( API_TR_UWORD, "isCType", &isCType );

    return isCType;
} /* pa20_IsCType */


UWORD pa20_IsSearchable( tpa21DescRecord *irdPtr )
{
    SWORD searchable = SQL_PRED_NONE;

    API_ASSERT_PTR( irdPtr );

    switch (irdPtr -> esqType) {
    case (dchb):
    case (dcha):
    case (dche):
    case (dvarchara):
    case (dvarchare):
    case (dvarcharb):
    case (dunicode):
    case (dvarcharuni):
      /* added for UCS2 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
        searchable = SQL_SEARCHABLE;
        break;
    case (dstra):
    case (dstre):
    case (dstrb):
    case (dstrdb):
    case (dlonga):
    case (dlonge):
    case (dlongb):
    case (dlongdb):
    case (dstruni):
    case (dlonguni):
        /* LONG values may only be used in a <null-predicate>. Because there
        * is no value for this we say SQL_PRED_NONE */
        searchable = SQL_PRED_NONE;
        break;
    default:
        searchable = SQL_PRED_BASIC;
        break;
    }; /* switch */

    return searchable;
} /* pa20_IsSearchable */



UWORD pa20_IsSqlType( SWORD type )
{
    UWORD isSqlType;

    API_TRACE( API_TR_ENTRY, "pa20_IsSqlType", 0 );
    API_TRACE( API_TR_SWORD, "type", &type );

    switch (type) {
    case (SQL_CHAR):
    case (SQL_UNICODE):
    case (SQL_NUMERIC):
    case (SQL_DECIMAL):
    case (SQL_INTEGER):
    case (SQL_SMALLINT):
    case (SQL_FLOAT):
    case (SQL_REAL):
    case (SQL_DOUBLE):
    case (SQL_VARCHAR):
    case (SQL_UNICODE_VARCHAR):
    case (SQL_TYPE_DATE):
    case (SQL_TYPE_TIME):
    case (SQL_TYPE_TIMESTAMP):
    case (SQL_DATE):
    case (SQL_TIME):
    case (SQL_TIMESTAMP):
    case (SQL_LONGVARCHAR):
    case (SQL_UNICODE_LONGVARCHAR):
    case (SQL_BINARY):
    case (SQL_VARBINARY):
    case (SQL_LONGVARBINARY):
    case (SQL_BIGINT):
    case (SQL_TINYINT):
    case (SQL_BIT):
        /* ODBC 3.0 interval types */
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        isSqlType = API_TRUE;
        break;
    default:
        isSqlType = API_FALSE;
        break;
    }; /* switch */

    API_TRACE( API_TR_EXIT, "pa20_IsSqlType", 0);
    API_TRACE( API_TR_UWORD, "isSqlType", &isSqlType );

    return isSqlType;
} /* pa20_IsSqlType */


UWORD pa20_IsValidFieldId( SWORD fieldId )
{
    UWORD isValid;

    API_TRACE( API_TR_ENTRY, "pa20_IsValidFieldId", 0 );
    API_TRACE( API_TR_SWORD, "fieldId", &fieldId );

    isValid = (fieldId >= SQL_DESC_COUNT && fieldId <= SQL_DESC_ALLOC_TYPE)
        || (fieldId >= SQL_DESC_ARRAY_SIZE && fieldId <= SQL_DESC_ROWVER)
        || (fieldId == SQL_COLUMN_DISPLAY_SIZE)
        || (fieldId == SQL_COLUMN_TYPE)
        || (fieldId >= SQL_COLUMN_UNSIGNED && fieldId <= SQL_COLUMN_LABEL);

    API_TRACE( API_TR_EXIT, "pa20_IsValidFieldId", 0);
    API_TRACE( API_TR_UWORD, "isValid", &isValid );

    return isValid;
} /* pa20_IsValidFieldId */


API_RETCODE pa20_ResetError( SQLHDESC hdesc )
{
    API_RETCODE apiRetcode = API_OK;
    tpa20Desc *descPtr;

    API_TRACE( API_TR_ENTRY, "pa20_ResetError", 0);

    descPtr    = (tpa20Desc*) hdesc;
    pa30FreeDiagArea( &descPtr->diagArea );

    API_TRACE( API_TR_EXIT, "pa20_ResetError", 0);
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );

    return apiRetcode;
} /* pa20_ResetError */

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
UDWORD pa20_ComputeCharacterLength (UDWORD           length,
                                    tpa21DescRecord *impl_rec_ptr,
                                    SWORD            fCType)
{
  int clientIsUni, dbIsUni;
  SWORD  sql_type;       /* type of implementation buffer */
  UDWORD sql_len;
  UWORD  sql_scale;

  apgstys (impl_rec_ptr->esqType, impl_rec_ptr->esqLength, impl_rec_ptr->esqScale,
           &sql_type, &sql_len, &sql_scale);

  if (sql_type == SQL_WCHAR  ||  sql_type == SQL_WVARCHAR  ||
      sql_type == SQL_WLONGVARCHAR)
    dbIsUni = TRUE;
  else
    dbIsUni = FALSE;

  if (fCType == SQL_WCHAR  ||  fCType == SQL_WVARCHAR  ||
      fCType == SQL_WLONGVARCHAR)
    clientIsUni = TRUE;
  else
    clientIsUni = FALSE;

  /* following table shows, what is calculated:

     client    db      length
      UCS2     UCS2       * 2
      UCS2     ASC        =
      ASC      UCS2       / 2
      ASC      ASC        =
  */
  if (clientIsUni) {
    if (dbIsUni) {
    }
  } else {
    if (dbIsUni) {
      length /= sizeof (SQLWCHAR);
    }
  }

  return length;
}



/* for debugging (replaces macros defined in vpa20Desc.h */

SQLLEN pa20BindOffset (tpa21DescRecord *rec)
{
  SQLLEN ret;

	if (rec->bound && rec->parentDesc->header.bindOffsetPtr != NULL)
		ret = *(rec->parentDesc->header.bindOffsetPtr);
	else
		ret = 0;
        return ret;
}

SQLLEN* pa20OctetLengthPtr (tpa21DescRecord *rec)
{
  char *p = (char*) rec->octetLengthPtr;

  p += PA20_BINDOFFSET(rec);

  return ((SQLLEN*) (((char*) rec->octetLengthPtr) + PA20_BINDOFFSET(rec)));
}

#endif  /* of _UNICODE_ODBC */


/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/


static char ident_sr[] = "@(#)vpa20Desc-unicode.c dated 2006-07-17 05:56:28 used 2008-05-09 00:59:33";
extern char *vpa20Desc_unicode_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpa20Desc-unicode FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF";
static char ident_cc[] = "@(#)vpa20Desc-unicode cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDLL -DHAS_NPTL -DSQLODBC -D_UNICODE_ODBC -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpa20Desc_unicode_force_whatinfo () { return( ident_cc ); }
