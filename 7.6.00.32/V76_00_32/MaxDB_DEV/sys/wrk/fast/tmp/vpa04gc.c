
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpa00global.h"
/*



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





*/
#ifndef VPA00GLOBAL_H
#define VPA00GLOBAL_H
/*!**********************************************************************

  module: vpa00global.h

  -----------------------------------------------------------------------

  responsible:

  special area: ODBC-Driver

  description:  global include file with common includes

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpr100.h"
#include "heo07.h"
#include "vpa00vrc.h"
#include "vpa00enc.h"
#include "vpa00wic.h"
#include "vpa00sqlext.h"
#include "vpa00txc.h"
#include "vpa00dlc.h"
#include "geo00.h"
#include "gsp00.h"
#include "vsp001.h"
#include "gpr00.h"
#include "vpa00odc.h"
#include "vpa00sqlod32.h"
#include "vpa00odstream.h"

#include <assert.h>

/* ****** assert-macro ******** */

#ifdef DEBUG
#define API_ASSERT(expression) assert(expression)
#define API_ASSERT_PTR(ptr) assert(ptr!=NULL)
#else
#define API_ASSERT(expression)
#define API_ASSERT_PTR(ptr)
#endif

/* internal return codes      */
#define API_NOT_OK              (0)
#define API_OK                  (1)
#define API_TRUNCATE            (2)
#define API_NO_DATA_FOUND       (3)
#define API_NEED_DATA           (4)
#define API_DATA_LOSS           (5)
#define API_NOT_NUMBER          (6)
#define API_NOT_DATE            (7)
#define API_INVALID             (8)
#define API_FEW_BOUNDS          (9)
#define API_INVALID_LENGTH      (10)
#define API_CONSTRAINT_VIOLATION (11)
#define API_TIME_OUT            (12)
#define API_NO_INDICATOR        (13)
#define API_INVALID_CHAR        (14)
#define API_FRACTIONAL_TRUNC    (15)
#define API_TRUNCATE_22001      (16)      /* truncation of input parameter, 1126177 */
#define API_MARK_END            (-1)
#define API_ANY                 (9999)

/* constants */
#define MX_IDENTIFIER 64
#define API_COLUMN_NAME_MAX    MX_IDENTIFIER   /* "owner . name" max len */
#define API_TABLE_NAME_MAX     MX_IDENTIFIER   /* "owner . name" max len */
#define API_USERID_MAX_LEN     MX_IDENTIFIER
#define API_PASSWD_MAX_LEN     MX_IDENTIFIER
/* make this big because sap router strings are large */
#define API_MAX_SERVERNODE_LEN 64000

#define API_EMPTY_STRING 	""

/* statement states */
#define API_STMT_UNALLOCATED (0)  /* statement S0: unallocated  */
#define API_STMT_ALLOCATED   (1)  /* statement S1: allocated    */
#define API_STMT_PREPARED    (2)  /* statement S2: prepared     */
#define API_STMT_EXECUTED    (3)  /* statement S3: executed     */
#define API_STMT_POSITIONED  (4)  /* statement S4: positioned   */
#define API_STMT_RECONNECTED (5)

/*****************************************************************
 * BIGINT support
 */

#ifdef ODBCINT64
#define BIGINT_SUPPORTED

typedef unsigned ODBCINT64 API_UBIGINT;
typedef ODBCINT64          API_BIGINT;

#endif

/* BIGINTs are only supported on WIN32 ans 64 bit platforms */
/* #if  defined(WIN32) || defined(BIT64)
   #define BIGINT_SUPPORTED
   #endif
*/

#ifdef BIGINT_SUPPORTED
#ifdef WIN32
/*
typedef unsigned _int64 API_UBIGINT;
typedef _int64          API_BIGINT;
*/

/* sprintf placeholder for BIGINTS */
#define PRINT_SBIGINT "%I64d"
#define PRINT_UBIGINT "%I64u"

#define API_ATOI64 _atoi64

#else /* 64 bit platforms: BIGINT == long */

/*
typedef unsigned long API_UBIGINT;
typedef long          API_BIGINT;
*/

/* sprintf placeholder for BIGINTS */
#define PRINT_SBIGINT "%ld"
#define PRINT_UBIGINT "%lu"

#define API_ATOI64 atol

#endif /* WIN32 */

#define SQLBIGINT  API_BIGINT   /* datatype for SQL_C_SBIGINT, SQL_C_UBIGINT */
#define SQLUBIGINT API_UBIGINT
#endif /* BIGINT_SUPPORTED */

/*
 * BIGINT support END
 *****************************************************************/



/* sign indicator for c datatype SQL_C_NUMERIC */
#define NUMERIC_POSITIVE_SIGN 1
#define NUMERIC_NEGATIVE_SIGN 0

/* implementation defined precisions */
#define API_SQL_NUMERIC_PRECISION  18
#define API_SQL_DECIMAL_PRECISION  18
#define API_SQL_FLOAT_PRECISION    15

/* #define MEMCHECK */
/* sqlod32.dld : msvcrt -> msvcrtd !!! */

#endif


#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpa60Stmt.h"
/*



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





*/
#ifndef VPA60STMT_H
#define VPA60STMT_H
/*!**********************************************************************

  module: vpa60Stmt.h

  -----------------------------------------------------------------------

  responsible:

  special area: ODBC-Driver

  description:  ODBC statement handles (datstructures,functions)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

#include "vpa61StmtAttr.h"
#include "vpa62DBCSpecial.h"
#include "vpa30DiagArea.h"
#include "vpa20Desc.h"
#include "vpa40DBC.h"
#include "vpa41ESQBlock.h"

/*!**********************************************************************

  Chapter: datastructures

  description: tpa60Stmt

************************************************************************/

typedef struct
{
    UWORD          asyncstate;
    BOOL           canceled;
    UWORD          function_code; /* function code of odbc function */
    SQLRETURN      retcode;
#ifdef ASYNCENABLED
    teo07_ThreadId ThreadId;
    teo07_Thread   hThread;
#endif
    API_HANDLE     hLocalValues;
} tpa60Async;


typedef struct
{
    tsp00_Bool *ignoredArray; /* one entry for every column. true if column
                               * was ignored (SQL_IGNORE as length) at last
                               * call of SQLSetPos */
    tsp00_Int4  arraySize;    /* number of allocated elements */
} tpa60SetPosSpecial;


typedef struct
{
    tpa30DiagArea *diagArea;
    UWORD         type;             /* type of structure (STMT)        */
    SQLHDBC       parent_hdbc;      /* handle for parent DBC           */
    SQLHSTMT      next_hstmt;       /* link to next sibling            */
    SQLHSTMT      prev_hstmt;       /* link to prev sibling            */
    UWORD         state;            /* transition state:
                                     * API_STMT_ALLOCATED, etc.        */
    UWORD         reconnect_status; /* transition state:
                                     *  API_STMT_ALLOCATED, etc.       */
    tpa62DBCSpecial dbc_special;    /* flag for special process by
                                     * SQLTables/SQLColumes            */
    UWORD         stmt_type;        /* flag to mark stmt attributes
                                     * (API_ATTRIB_MFETCH_ALLOWED)     */

    /* SQLSetStmtOptions */
    tpa61StmtAttr stmtopt;

    /* SQLSetPos */
    UWORD         setpos_irow;      /* Position of row in the rowset    */

    UDWORD        statement_id;     /* unique statement id              */
    SDWORD        number_rows;      /* number of rows "touched"; -1 if
                                       there are rows but number not known */
    UDWORD        rowset_pos;       /* pos of the cursor in the row set
                                     * is set to one by SQLExtendedFetch */
    SQLROWOFFSET  fetch_pos;        /* actual position of the cursor from
                                     * the begining (posive value) or the
                                     * end (negative value) of the
                                     * result table. Negative values are
                                     * used by
                                     * pa12_DynamicCursorExtendedFetch   */
    UWORD         number_cols;      /* number of cols in select          */
    UWORD         number_parms;     /* number of input parms: "?"        */
    API_HANDLE    hrowstat;         /* handle for internal rowstatus
                                     * table points to api_row_status
                                     * one entry for each row of the
                                     * resulttable possible values are
                                     * SQL_ROW_SUCCESS, SQL_ROW_DELETE,
                                     * SQL_ROW_UPDATED                   */
    API_HANDLE    input_sqlda_handle;    /* handle for input sqlda       */
    API_HANDLE    output_sqlda_handle;   /* handle for output sqlda      */
    struct SQLLD *rasqlldp;
    sqldatype    *input_sqlda_ptr;     /* ptr to input sqlda             */
    sqldatype    *output_sqlda_ptr;    /* ptr to output sqlda            */
    UDWORD        col_size;         /* size of column record             */
    UDWORD        parm_size;        /* size of parameter record          */
    API_HANDLE    col_handle;       /* handle for column area            */
    API_HANDLE    parm_handle;      /* handle for parameter area         */
    SWORD         next_param;
    /*
    UCHAR         cursor_name[ API_ODBC_CURSOR_LEN+1 ]; * cursor name   *
    UCHAR         table_name [API_TABLE_NAME_MAX*3+sizeof("''.''.''")];
                                    * tablename from select for update  *
    */
    tpr05_String  *cursor_name;     /* cursor name   */
    tpr05_String  *table_name;      /* tablename from select for update  */
    API_HANDLE    stmt_str_handle;  /* handle for statement string*/
    SDWORD        stmt_str_len;
    SDWORD        allocated_stmt_len; /* len of alloc. statement string  */
    UWORD         stmt_function_code; /* function code of the statement  */
    UCHAR         stmt_parsid [ API_SQLDB_PARSID_LEN ]; /* parsid of the
                                                         * statement string*/
    UCHAR         close_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_first_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_last_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_next_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_prior_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_relative_parsid [ API_SQLDB_PARSID_LEN ];
    UCHAR         fetch_absolute_parsid [ API_SQLDB_PARSID_LEN ];

    SQLHSTMT      hstmt_parent;   /* parent statement if this is a handle
                                   * for SQLSetPos */
    SQLHSTMT      hstmt_setpos;   /* Handle for UPDATE statement in
                                   * SQLSetPos */
    SQLHSTMT      hstmt_extfetch; /* handle for select direct in
                                   * SQLExtendedFetch  */
    tpa60Async    async;
    SWORD         need_data; /* is TRUE if last call of SQLParamData
                              * returned SQL_NEED_DATA and SQLPutData
                              * hasn´t been called yet. A call to
                              * SQLPutData sets this to FALSE */

    SWORD         no_result_set; /* Is TRUE if the result is implicitely
                                  * closed by the kernel because the
                                  * autocommit flag was set and the result
                                  * table was empty.
                                  * In this case we don't have to do a
                                  * CLOSE CURSOR */
    SWORD         row_not_found; /* Is TRUE if the result set is empty.
                                  * For an empty result set fetch returns
                                  * SQL_NO_DATA_FOUND without asking the
                                  * kernel */
    tsp00_Int4    errorIndex;    /* Used by SQLError for the actual error
                                  * to retrieve. Index is starting at 0. */
    tsp00_Bool    setPosNeedData;/* This is true if SQLSetPos returned
                                  * SQL_NEED_DATA. In this case
                                  * SQLParamData and SQLPutData have to
                                  * use hstmt_setpos */
    tpa60SetPosSpecial setPosSpecial; /* store information for SQLSetPos */
    SQLRETURN     fetchRetcode; /* return-code of last SQLFetch, SQLExtendedFetch,
                                 * or SQLFetchScroll. Used to check if cursor
                                 * is positioned before or after result-set. */
    /* new in 3.0/3.5
     * ===============*/
    tpa20Desc     ird;        /* implementation row descriptor            */
    tpa20Desc     ipd;        /* implementation parameter descriptor      */
    tpa20Desc    *ardPtr;     /* appliction row descriptor, points to
                               * defaultArd or explicitely allocated ARD  */
    tpa20Desc    *apdPtr;     /* application parameter descriptor, points
                               * to defaultAPD or explicitely alloc. APD  */
    tpa20Desc     defaultArd; /* default application row descriptor       */
    tpa20Desc     defaultApd; /* default application parameter descriptor */
} tpa60Stmt;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: ODBC functions
  - SQLSetStmtAttr

  For sepecification see Microsoft ODBC 3.5 reference.*/
/*!
  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa60AllocStmt

  description: Allocates statement handle.

  arguments:
      hdbc   [IN]  Parent connection handle.
      phstmt [OUT] The allocated statement handle

  return value: SQL_SUCCESS, SQL_ERROR, SQL_INVALID_HANDLE
  */

SQLRETURN pa60AllocStmt( SQLHDBC hdbc, SQLHSTMT *phstmt );


/*!**********************************************************************
  Function: pa60DropParseIds

  description: Drops the fetch, stmt and close-parseids.

  arguments:
      stmt_block_ptr  [IN]  The statement block for which the parsids
                            should be dropped.

  return value: void */

VOID pa60DropParseIds( tpa60Stmt *stmt_block_ptr );


/*!**********************************************************************
  Function: pa60DropFetchParseIds

  description: Drops only the fetch parseids.

  arguments:
      stmt_block_ptr  [IN]  The statement block for which the parsids
                            should be dropped.

  return value: void */

VOID pa60DropFetchParseIds( tpa60Stmt *stmt_block_ptr );


/*!**********************************************************************
  Function: pa60DropChildStmts

  description: Drops statements that are allocated for SQLSetPos and
               SQLExtendedFetch.

  arguments:
      hstmt  [IN]  The statement handle whose child stmts should be dropped.

  return value: void */

SQLRETURN pa60DropChildStmts( SQLHSTMT hstmt );


/*!**********************************************************************
  Function: pa60DropStmt

  description: Free all resources for this statement handle.

  arguments:
      hstmt  [IN]  The statement handle to be dropped.

  return value: void */

SQLRETURN pa60DropStmt( SQLHSTMT hstmt );


/*!**********************************************************************
  Function: pa60GetAPD

  description: Get the actual application parameter decriptor.

  The returned APD may be the default or a user defined descriptor,
  that was set with SQLSetStmtAttr (SQL_ATTR_APD).

  arguments:
      stmt  [IN]  The statement.

  return value: The actual APD.
*/

tpa20Desc* pa60GetAPD( tpa60Stmt* stmt );


/*!**********************************************************************
  Function: pa60GetARD

  description: Get the actual application row decriptor.

  The returned ARD may be the default or a user defined descriptor,
  that was set with SQLSetStmtAttr (SQL_ATTR_ARD).

  arguments:
      stmt  [IN]  The statement.

  return value: The actual ARD.
*/

tpa20Desc* pa60GetARD( tpa60Stmt* stmt );


/*!**********************************************************************
  Function: pa60GetFetchParseId

  description: Retrieve a fetch parse-id for the given fetchtype.

  arguments:
      stmt      [IN]  The statement.
      parseid   [OUT] A valid fetch parse-id for the given fetch type.
      fetchtype [IN]  One of SQL_FETCH_NEXT, SQL_FETCH_ABSOLUTE,
                      SQL_FETCH_PRIOR, SQL_FETCH_FIRST, SQL_FETCH_LAST,
                      SQL_FETCH_RELATIVE, SQL_FETCH_BOOKMARK

  return value: API_OK if a valid parse id was returned. If a valid parse
                id for this fetch type does not exist API_NOT_OK is
                returned. */

API_RETCODE pa60GetFetchParseId( tpa60Stmt  *stmt,
                                 UCHAR     **parseid,
                                 UWORD       fetchtype);


/*!**********************************************************************
  Function: pa60GetParentDBC

  description: Return the parent connection handle.

  arguments:
    stmt  [IN]  The statement-block.

  return value: The HDBC or API_NULL_HANDLE.
  */

SQLHDBC pa60GetParentDBC( tpa60Stmt *stmt );


/*!**********************************************************************
  Function: pa60GetParentEnv

  description: Return the parent environment handle.

  arguments:
    stmt  [IN]  The statement-block.

  return value: The HENV or SQL_NULL_HANDLE.
  */

SQLHENV pa60GetParentEnv( tpa60Stmt *stmt );

API_RETCODE pa60MoveLong( tpa40DBC* dbc_block_ptr ,
                          tpa60Stmt* stmt_block_ptr,
                          tpa20Desc* appl_desc_ptr,
                          tpa20Desc* impl_desc_ptr,
                          UWORD       icol,
                          UWORD       number_cols );
/*
*/
/*!**********************************************************************
  Function: pa60Param

  description: Get next parameter for SQLParamData.

  arguments:
  appl_desc_ptr  [in]  Pointer to APD
  impl_desc_ptr  [in]  Pointer to IPD
  stmt_block_ptr [in]  Pointer to statement block.
  prgbValue      [out] Buffer to return the rgbValue of the parameter.
  nolong         [in]  Indicate if LONG columns should be skipped.

  return value: API_OK or API_NOT_OK */

API_RETCODE pa60Param( tpa20Desc *appl_desc_ptr,
                       tpa20Desc *impl_desc_ptr,
                       tpa60Stmt *stmt_block_ptr,
                       PTR       *prgbValue,
                       BOOLEAN    nolong );


/*!**********************************************************************
  Function: pa60PutError

  description: Set diagnose record in the diagnose area of the statement.

  arguments:
      hstmt     [IN]  statement handle.
      sqlState  [IN]  SQLSTATE constant. One of API_ODBC_...
      esqBlkPtr [IN]  If this is not NULL the native error information is
                      retrieved from this block.

  return value: API_OK or API_NOT_OK */

API_RETCODE pa60PutError( SQLHSTMT       hstmt,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr);


/*!**********************************************************************
  Function: pa60PutESQError

  description: Create diagnose record which depends on the return code
               of an internal function.

  arguments:
      hstmt        [IN]  statement handle.
      api_retcode  [IN]  The return code that describes the error.

  return value: SQL_SUCCESS if the return code did not indicate an error.
                Else SQL_ERROR. */

SQLRETURN pa60PutESQError( SQLHSTMT hstmt, API_RETCODE  api_retcode);


/*!**********************************************************************
  Function: pa60ResetError

  description: Free all diagnose record in the diagnose area.

  arguments:
      hstmt        [IN]  statement handle.

  return value: API_OK or API_NOT_OK. */

API_RETCODE pa60ResetError( SQLHSTMT hstmt );


/*!**********************************************************************
 Function: pa60SetDefaultDesc

  description: Revert to the default APD or ARD.

  This function should be called if a user defined descriptor is freed.
  In this case the stmt should automatically revert to the default desc.

  arguments:
      hstmt        [IN]  statement handle.
      appl_desc    [IN]  The descriptor that is used before the function
                         is called. This is needed to determine if the
                         default APD or ARD should be used.

  return value: void
*/

void pa60SetDefaultDesc( SQLHSTMT hstmt, tpa20Desc *appl_desc );


/*!**********************************************************************
  Function: pa60VerifyStmt

  description: Checks if hstmt is a valid statement handle.

  arguments:
      hstmt        [IN]  statement handle.

  return value: API_OK if hstmt is valid else API_NOT_OK. */

API_RETCODE pa60VerifyStmt( SQLHSTMT hstmt );


API_RETCODE pa60AllocSetPosSpecial( tpa60SetPosSpecial *setPosSpecial,
                                    tsp00_Int4          numCols );


API_RETCODE pa60RefreshSetPosSpecial( tpa60SetPosSpecial *setPosSpecial,
                                      tpa20Desc          *ardPtr,
                                      tsp00_Int4          rowsetPos );


tsp00_Bool pa60StmtChanged( tpa60SetPosSpecial *setPosSpecial,
                            tpa20Desc          *ard,
                            tsp00_Int4          rowsetPos );

VOID pa60ReinitSetPosSpecial( tpa60SetPosSpecial *setPosSpecial );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpa50Env.h"
/*



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





*/
#ifndef VPA50ENV_H
#define VPA50ENV_H
/*!**********************************************************************

  module: vpa50Env.h

  -----------------------------------------------------------------------

  responsible:

  special area: ODBC-Driver

  description:  ODBC-environment handles (datastructures,functions)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

#ifdef WIN32
#ifdef MEMCHECK
#define _DEBUG
#include <crtdbg.h> /* _CrtMemState */
#endif
#endif

#include "vpa30DiagArea.h"
#include "vpr01Env.h"

/*!**********************************************************************

  Chapter: datastructures

  description: tpa50Env

************************************************************************/

typedef struct {
    tpa30DiagArea diagArea;
    UWORD type;                /* type of structure (ENV)    */
    SQLHDBC  child_hdbc;       /* link to children           */
    SQLHENV  next_henv;        /* link to next sibling       */
    SQLHENV  prev_henv;        /* link to prev. sibling      */
    /* HERR herror; */
                               /* New in 3.x : */
                               /* ============ */
    UDWORD connection_pooling; /* enable/disable connection pooling */
    UDWORD cp_match;           /* strict or relaxed match for       */
                               /* connection pooling                */
    UDWORD odbc_version;       /* SQL_OV_ODBC2 or SQL_OV_ODBC3      */
    UDWORD output_nts;         /* null terminating returned string data */
                               /* always true for ODBC                  */
#ifdef ASYNCENABLED
    api_thread_block thread;
#endif
#ifdef WIN32
#ifdef MEMCHECK
    _CrtMemState  startMemState;
#endif
#endif
    tsp00_Int4    errorIndex;    /* Used by SQLError for the actual error
                                  * to retrieve. Index is starting at 0. */

    tpr01_EnvContainer *envContainer;   /* container for runtime evironment */

} tpa50Env;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: ODBC functions

  description:
  - SQLDataSources
  - SQLGetEnvAttr
  - SQLSetEnvAttr

  For sepecification see Microsoft ODBC 3.5 reference.*/
/*!
  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa50AllocEnv

  description: allocates environment handle

  arguments:
    phenv  [OUT]  Pointer to envoronment handle

  return value: SQL_SUCCESS, SQL_ERROR
  */

SQLRETURN pa50AllocEnv( SQLHENV *phenv );

/*!**********************************************************************
  Function:  pa50FreeEnv

  description: Free resources for environment handle

  arguments:
      henv  [IN]  Envoronment handle

  return value: SQL_SUCCESS, SQL_ERROR
  */

SQLRETURN pa50FreeEnv( SQLHENV henv );

/*!**********************************************************************
  Function:  pa50PutError

  description: Set error in diag-area of henv.

  arguments:
      henv      [IN]  Environment handle
      sqlState  [IN]  SQLState constant (API_ODBC_....) of error

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa50PutError( SQLHENV henv, UWORD sqlState );


/*!**********************************************************************
  Function:  pa50ResetError

  description: Free diag-area of environment.

  arguments:
      henv  [IN]  environment handle.

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa50ResetError( SQLHENV henv );


/*!**********************************************************************
  Function:  pa50VerifyEnv

  description: Check if henv is a valid environment handle.

  arguments:
    henv  [IN]  environment handle

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa50VerifyEnv( SQLHENV henv );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif


#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpa40DBC.h"
#ifndef VPA40DBC_H
#define VPA40DBC_H
/*!**********************************************************************

  module: vpa40DBC.h

  -----------------------------------------------------------------------

  responsible:

  special area: ODBC-Driver

  description:  ODBC connection handles (datastructures,functions)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------


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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

#include "vpr09DynaDesc.h"

#include "vpa20Desc.h"
#include "vpa30DiagArea.h"
#include "vpa41ESQBlock.h"
#include "vpa61StmtAttr.h"
#include "vpr01Env.h"
#include "vpr01Con.h"
/*!**********************************************************************

  Chapter: datastructures

  description: tpa40DBC

************************************************************************/

typedef struct {
  SWORD utility_session;
  UWORD  component;               /* Senderid-constants of the separate Components */
  UWORD  service;                 /* Service for sqlaconnect */
  UWORD  sp1c_producer;           /* Producer of the cmd see vsp001 */
} tpa40Utility;

typedef struct {
  tpa30DiagArea diagArea;
  UWORD type;                     /* type of structure (DBC)    */
  SQLHENV parent_henv;            /* handle for parent ENV      */
  SQLHDBC next_hdbc;              /* link to next sibling       */
  SQLHDBC prev_hdbc;              /* link to prev. sibling      */
  SQLHSTMT child_hstmt;           /* link to children           */
  tpa41ESQBlock esqblk;
  UWORD state;                    /* transition state:          */
				  /* API_DBC_ALLOCATED, etc.    */
				  /* Options for Set-/GetConnectOption */
  UWORD  connectionDead;          /* TRUE means the state is still
                                   * API_DBC_CONNECTED but the connection
                                   * is broken */
  UWORD  casesensitive;           /* Username and Password are Casesensitive */
  UWORD  catalogcomments;         /* Comments on Catalogfunctions TRUE /
                                   * FALSE (default) */
  UWORD  unicode_catalog;         /* Kernel runs in UNICODE */
  UDWORD autocommit;              /* 0 = Off (default); 1 = On */
  UDWORD set_autocommit_flag;     /* set the autocommit_flag in segment header?
                                   * this is not equal to autocommit if
                                   * long values are inserted with SQLParamData
                                   * SQLPutData or if a BEGIN SUBTRANS was
                                   * executed by the application.
                                   * For kernels older than 7.2. a COMMIT
                                   * command is send to the kernel instead
                                   * because inserting LONG-values fails else*/

  UDWORD subtranscount;
  UDWORD blocksize;               /* size of the block allocating for
                                   * LONGVARCHAR */
  SDWORD lvctrunc;                /* length who pcbValue returned if LONG`s
                                   * are truncated */
    /* +n | SQL_LONG_MAX=-1 | SQL_VALUEMAX=-2 | SQL_VALUEMAX_PLUSONE=-3 |
       SQL_NO_TOTAL=-4 (default) */
  SDWORD isolevel;
  SDWORD defisolevel;
  UINT   activestatements;
  UDWORD access_mode;        /* SQL_MODE_READ_ONLY | SQL_MODE_READ_WRITE (default) */
  UDWORD login_timeout;      /* time in sec. to wait for a login timeout */
  UDWORD opt_trace;          /* 0 = Trace off (default); 1 = Trace on */
  UCHAR  *opt_tracefile_ptr; /* Name of Tracefile */
  UDWORD translate_option;	 /* Flag value that is passed to the translation DLL.*/
  UDWORD odbc_cursors;		 /* How the DM uses the ODBC cursor library  */
  UDWORD quiet_mode;		 /* Window handle (hwnd)		      */
  UDWORD sessionTimeout;     /* driver defined connect attr SQL_ATTR_SESSION_TIMOUT */

  /* SQLSetStmtOptions */
  tpa61StmtAttr constmtopt;

  SWORD  dbno;                    /* Sessionnumber to execute   */
  tpr01_EnvDesc *EnvDesc;         /* container for runtime evironment */
  tpr01_ConDesc *ConDesc;         /* Connect descriptor from Interface runtime */

  tpr05_String *dsname;         /* Datasourcename */
  tpr05_String *userid;
  tpr05_String *passw;

  sqldbname serverdb;
  UCHAR *servernode;
  UDWORD allocServerNodeLen;      /* no of bytes allocated for servernode */
  UWORD status;                   /* dbc transaction status:    */
#ifdef ASYNCENABLED
  api_thread_block thread;
#endif
  tpa40Utility utility;
  tpr09DynamicDescriptor *applDescList; /* list of explicit allocated
                                         * descriptors */
    UCHAR  kernelVersionString[6]; /* Kernel version as detected at connect
                                    * (apeconn), e.g "70200\0" */
    void *kernelDirect; /* Pointer to struct that is used if ODBC
                         * runs in the DB-Kernel. For "normal" ODBC-
                         * applications this is always NULL */
    UDWORD stmtCount;   /* Used in pa40GetStmtId */
    tsp00_Int4    errorIndex;    /* Used by SQLError for the actual error
                                  * to retrieve. Index is starting at 0. */
    char         *cancelRouterStr; /* connect attr. used to make SQLCancel
                                      over NI-Connection */
    tpr05_String *driverConnectStr; /* store out connect string of last
                                     * SQLDriverConnect. This can be retrieved
                                     * by SQLGetDiagField with
                                     * SQL_DIAG_CONNECT_STRING */
    SQLCHAR  *cpName;            /* default NULL, otherwise null-terminated
                                    name of codepage, that should be loaded
                                    from the db after connect */
    char encrypt;                /* enumeration for encryption
                                    (0, no encryption) */
} tpa40DBC;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: ODBC functions

  description:
    - SQLBrowseConnect
    - SQLConnect
    - SQLDisconnect
    - SQLDriverConnect
    - SQLGetConnectAttr
    - SQLGetFunctions
    - SQLGetInfo
    - SQLNativeSql
    - SQLSetConnectAttr

    For sepecification see Microsoft ODBC 3.5 reference. */
/*!
  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa40AddDesc

  description: Stores an application descriptor in the dbc block.

  This function is used only for explicit allocated application
  descriptors. Because these have to be freed when the connection handle
  is freed. Implicitely allocated descriptors are associated to a
  statement block.

  arguments:
  dbcPtr       [in]  Connection block.
  applDescPtr  [in]  Pointer to the application decriptor.

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa40AddDesc( tpa40DBC *dbcPtr, tpa20Desc *applDescPtr );


/*!**********************************************************************
  Function: pa40AllocConnect

  description: Allocate memory and set default values for the connection
               block.

  arguments:
    henv   [in]  Parent environment handle.
    phdbc  [out] Buffer to store the allocated connection handle.

  return value: SQL_SUCCESS, SQL_INVALID_HANDLE, SQL_ERROR
  */

SQLRETURN pa40AllocConnect( SQLHENV henv, SQLHDBC *phdbc );


/*!**********************************************************************
  Function: pa40FreeConnect

  description: Free resources for a connection handle.

  First all remaining child statements and explicitely allocated
  descriptors are destroyed. Then the resources for the connection handle
  are freed.

  arguments:
    hdbc  [in]  Connection handle.

  return value: SQL_SUCCESS, SQL_INVALID_HANDLE, SQL_ERROR. A return code
  of SQL_ERROR indicates that some datastructures are not consistent.
  */

SQLRETURN pa40FreeConnect( SQLHDBC    hdbc);


/*!**********************************************************************
  Function: pa40GetParentEnv

  description: Return the parent environment handle.

  arguments:
      dbc  [in]  Pointer to connection block.

  return value: The parent environment handle.
  */

SQLHENV pa40GetParentEnv( tpa40DBC *dbc );


/*!**********************************************************************
  Function: pa40GetStmtId

  description: Return an id that is unique for this connection.

  arguments:
      dbc  [in]  Pointer to connection block.
      id   [out] Buffer to return the unique id.

  return value: void.
  */

VOID pa40GetStmtId( tpa40DBC *dbc, UDWORD *id );

/*!**********************************************************************
  Function:  pa40PutError

  description: Create error records in the diagnose area of the connection
               block.

  arguments:
      hdbc      [in]  Connection handle.
      sqlState  [in]  Constant from vpa00txc that indicates the SQLSTATE
                      value of the error.
      esqblkPtr [in]  If this pointer is not NULL further error
                      information is retrieved from the ESQBlock.

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa40PutError( SQLHDBC        hdbc,
                          UWORD          sqlState,
                          tpa41ESQBlock *esqblkPtr );


/*!**********************************************************************
  Function:  pa40ResetError

  description: Delete error records in the diagnose area of the connection
               block.

  arguments:
      hdbc  [in]  Connection handle.

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa40ResetError( SQLHDBC hdbc );


/*!**********************************************************************
  Function:  pa40SetReconnectFlag

  description: Sets the reconnect flag in all child statements.

  This flag is set if the ODBC driver made an implicit reconnect to the
  database after a timeout.

  arguments:
    api_dbc_ptr  [in]  Pointer to connection block.

  return value: VOID
  */

VOID pa40SetReconnectFlag( tpa40DBC  *api_dbc_ptr );


/*!**********************************************************************
  Function:  pa40SetServerNode

  description: Copies the servernode string into the dbc-block.

  The servernode string may have any length.

  arguments:
    dbcPtr      [in]  Pointer to connection block.
    servernode  [in]  Servernode string.

  return value: true, false
  */

tsp00_Bool pa40SetServerNode( tpa40DBC *dbcPtr, UCHAR *servernode );


/*!**********************************************************************
  Function: pa40UseOldAutocommit

  description: Determine how to commit in the autocommit mode.

  With database version less than 7.2 an explicit COMMIT command is sent
  to the db (old style). Since version 7.2 every command is commited by
  setting the commit_immediately flag in the segment header.

  arguments:
      dbcPtr  [in]  Pointer to connection block.

  return value: TRUE, FALSE
  */

BOOLEAN pa40UseOldAutocommit( tpa40DBC *dbcPtr );


/*!**********************************************************************
  Function:  pa40VerifyDBC

  description: Check if hdbc is valid.

  arguments:
    hdbc  [in]  Connection handle.

  return value: API_OK, API_NOT_OK
  */

API_RETCODE pa40VerifyDBC( SQLHDBC hdbc );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif


#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vsp46c.h"

void s46stodc (char*, int, int, const char*, int, int, char*);
void s46dctos (const char*, int, int, char*, int, int, int*,
                     char*);

#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//hsp77.h"
/*!***************************************************************************

  module:        hsp77.h

  -------------------------------------------------------------------------

  responsible:   DanielD, JoergM

  special area:  sprintf functions interface

  description:   Replacement for ANSI C sprintf and vsprintf functions

  Format description: %[FLAGS][WIDTH][.PREC][SIZE][TYPE]

  FLAGS,SIZE and TYPE are described below. WIDTH and PREC are the width
  and precision specifier. These are either positiv integer numbers or
  asterik '*'. Each asterik needs an additional integer argument that is
  used as width or precision respectivly. Note that precision is separated
  by a single dot.

  Recognized format FLAGS:

  - left padding

  + sign wanted. Influences %d,%i,%u,%e,%E,%f,%F,%g,%G

  Space(' ') Padding uses spaces

  0 Padding uses 0

  = (1 additional argument parsed) input encoding for string. Influences %s,%S,%r,%R

  ' interprets all width and precision specification as bytes instead of character

  # Use alternate form. %o force leading '0', %x force leading '0x', %X force leading
    '0X', %e,%E,%f,%g and %G force decimal point even if result would contain no
    decimal point.


  Recognized format SIZE:

  h force %d,%i,%o,%u,%x and %X to apply to a short or unsigned short

  l force %d,%i,%o,%u,%x and %X to apply to a long or unsigned long

  L force %d,%i,%o,%u,%x and %X to apply to a long or unsigned long

  q force %d,%i,%o,%u,%x and %X to apply to a long long or
    unsigned long long (quad word)

  Not implemented... L force %e,%E,%f,%g and %G to apply to a long double argument


  Recognized format TYPE:

  %c (char) Character

  %d (int) Integer

  %e (double) Scientific format number (small letter e)
  %E (double) Scientific format number (capital letter E)

  %f (double) Floating point number
  %F (double) Floating point number

  %g (double) Floating point or scientific format number (small letter e)
  %G (double) Floating point or scientific format number (capital letter E)

  %i (int) Integer

  %n (pointer to short (%hn) , int (%n) or long (%ln) ) Parameter is a pointer to
     a place where number of so far parsed bytes are stored (output not influenced)

  %o (unsigned) Octal number

  %p (void *) Pointer (unsigned long) as hex number

  %r (string) String hexdump (small letters)
  %R (string) String hexdump (capital letters)

  %s (string) Character string (Default ASCII)
  %S (string) Character string (Default UCS2Native)

  %u (unsigned) Unsigned integer

  %x (unsigned) Hexadecimal number (small letters)
  %X (unsigned) Hexadecimal number (capital letters)

  last changed: 2000-12-07  15:22
  Original Author: DanielD

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html
  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG-2000




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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//


*****************************************************************************/

#ifndef HSP77_H
#define HSP77_H

#include <stdarg.h>

#include "geo00.h"

#if defined(OSF1) && defined(__cplusplus)
#pragma message disable noctobutconrefm
#endif

#if defined(__cplusplus)
#define externCGlobal extern "C"
#else
#define externCGlobal extern
#endif

/*!
  Chapter: Character constants
 */

/*!
  Declaration: tsp77charConst
  Description: encoding independant representation of a character.

    Use as in SAPDB_memcpy (target, charConst->bytes, charConst->byteCount).
 */
typedef struct t_sp77charConst {
    int byteCount;
    char bytes [4];
} tsp77charConst;
/*! EndDeclaration */

/*!
    Function: sp77charIsEqual
    Description: compares raw bytes with a character constant
    returns:

        - TRUE when then bytes in <Param>buf</> are equivalent
            to <Param>charConst</>
        - FALSE otherwise
 */
externC teo00_Bool sp77charIsEqual (
    const tsp77charConst * charConst,
    const void           * buf,
    int                    bufLen);

/*!
    Function: sp77charMove
    Description: copies a charater into <Param>buf</>
    returns:
        - number of bytes copied if successful
        - -1 if <Param>buf</> is too small
 */
externC int sp77charMove (
    void * buf,
    int    bufLen,
    const tsp77charConst * charConst);

/*!
  Declaration: tsp77charTable
  Description: a collection of commonly used character constants

    - indicator: used at the start of files to detect the encoding
    - terminator: the null character used to signal the end of a string
    - nl: new line
    - cr: carriage return
    - blank: the character
 */
typedef struct t_sp77charTable {
    const tsp77charConst indicator;
    const tsp77charConst terminator;
    const tsp77charConst nl;
    const tsp77charConst cr;
    const tsp77charConst blank;
    const tsp77charConst dosEOF;
}tsp77charTable;
/*!
  EndChapter: Character constants
 */
/*!
  Chapter: tsp77encoding_Structure
  Description: Structure describing a character encoding

  An encoding contains all features and member functions needed
  for sprintf to work properly except conversion between other encodings.
 */
typedef struct t_sp77encoding {
  /*!
    Decleration: encodingClass
    Description: Identifier which is identical for all encoding classes

    This allows to automatically detect wrong or overwritten
    encoding arguments
   */
  const char **encodingClass;

  /*!
    Decleration: whichEncodingClass
    Description: Identifier which is unique for this encoding classes

    This is always a zero terminated 7bit ASCII string, that allows
    easy trace outputs too.
   */
  const char *whichEncodingClass;

  /*!
    Decleration: EncodingType
    Description: Identifier which is used as input encoding type argument
   */
  const int EncodingType;

  /*!
    Function: asciiCopy
    Description: Copy given ascii characters (Literal) in target buffer.

    Adjusts target buffer pointer and target buffer length

    Arguments: target [inout] the target buffer
               targetLength [inout] the target buffer length
               literal [in] the ascii literal
               literalLength [in]length in bytes
    Return value: true on error, false on success */
  int (*asciiCopy) (void **target,
                    unsigned int *targetLength,
                    const char *literal,
                    unsigned int literalLength);

  /*!
    Function: stringInfo
    Description: Retrieve information about given string

    String is handled as a C_String, that is a termination character
    sequence stops parsing.

    Arguments: buffer [in] the string buffer to parse
               bufferLength [in] the buffer length in bytes or characters
               bufferLengthIsInBytes [in] if true bufferLength is in bytes
               charLength [out] the number of characters found (excluding ZERO terminator)
               byteLength [out] the number of bytes found (excluding ZERO terminator)
               isTerminated [out] if true string is zero terminated
               isCorrupted [out] if true string contains corrupted chars
               isExhausted [out] if true string has partial last char
    Return value: true if corrupted or exhausted, false is ok */

  int (*stringInfo) (const void *buffer,
                     unsigned int bufferLength,
                     int bufferLengthIsInBytes,
                     unsigned int *charLength,
                     unsigned int *byteLength,
                     int  *isTerminated,
                     int *isCorrupted,
                     int *isExhausted);

  /*!
    Function: stringComplete
    Description: Test the last character in the buffer for completeness

    Since some encodings allow characters to consist of mulitbyte sequences,
    this routine allows to check for completeness of these sequences. Usefull
    if parsing an input stream.
    String is NOT handled as a C_String, that is termination character are ignored

    Arguments: buffer [in] the string buffer to parse
               bufferLengthInBytes [in] the buffer length in bytes
               completeLengthInBytes [out] the length in bytes of all complete characters
    Return value: true if last character only partial, false else
   */
  int (*stringComplete) (const void *buffer,
                         unsigned int bufferLengthInBytes,
                         unsigned int *completeLengthInBytes);

  /*!
    Function: fillString
    Description: Fill a target buffer with given number of padding characters

    Adjust target buffer pointer and target buffer length
    Arguments: target [inout] pointer to pointer to current target buffer position (updated)
               targetLength [inout] pointer to remaining target buffer size (updated)
               padLength [in] number of padding characters to fill
               padChar [in] the padding character
    Return value: none
   */
  void (*fillString) (void **target,
                      unsigned int *targetLength,
                      unsigned int padLength,
                      char padChar);

  /* PTS 1108620 */
  /*!
    Function: countPadChars
    Description: Count the padding character at end of given buffer

    The padding character is expanded to the given encoding and the number of these
    padding character found is returned.

    Arguments: target [in] pointer to current target buffer
               targetLength [in] target buffer size (in bytes)
               padChar [in] the padding character
    Return value: number of padding characters (not bytes!) found at end of buffer
   */
  unsigned int (*countPadChars) ( void *target,
                                  unsigned int targetLength,
                                  char padChar);

  /*!
    Function: isAscii7
    Description: Test on character being representing a 7bit ASCII character

    Test the given character on being a 7bit Ascii character
    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a 7bit ASCII character, false else
   */
  int (*isAscii7) (const void *testCharacter);

  /*!
    Function: isBlank
    Description: Test the given character on being a tab or space.

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a blank, false else
   */
  int (*isBlank) (const void *testCharacter);

  /*!
    Function: isSpace
    Description: Test the given character on being a whitespace.

    A whitespace is one out of (space,tab,newline,formfeed,carriage return)

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a whitespace, false else
   */
  int (*isSpace) (const void *testCharacter);

  /*!
    Function: isControl
    Description: Test the given character on being a control character.

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a control character, false else
   */
  int (*isControl) (const void *testCharacter);

  /*!
    Declaration: fixedCharacterSize
    Description: Fixed character size (0 if variable character size)
   */
  int      fixedCharacterSize;

  /*!
    Declaration: terminatorSize
    Description: Terminator character size (0 if no terminator character for this encoding)
   */
  int      terminatorSize;
  /*!
    Declaration: charTable
    Description: commonly used character constants
   */
  const tsp77charTable * const charTable;
} tsp77encoding;
/*!
  Endchapter: tsp77encoding_Structure
 */

/*!
  Chapter: Encodings
 */
/*!
  Declaration: sp77encodingAscii
  Description: 8Bit Ascii String encoding

  Character strings as 8bit Ascii
 */
externCGlobal const tsp77encoding * const sp77encodingAscii;
/*!
  Declaration: sp77encodingUCS2
  Description: SAPDB UCS2 String encoding

  SAPDB interpretation of UCS2: HighByte,LowByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS2;
/*!
  Declaration: sp77encodingUCS2Swapped
  Description: SAPDB swapped UCS2 encoding

  SAPDB interpretation of swapped UCS2: LowByte,HighByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS2Swapped;
/*!
  Declaration: sp77encodingUCS2Native
  Description: Machine dependend UCS2 encoding

  Native (machine dependend UCS2): Machine byte order
 */
/* PTS 1110303 */
#if defined(WIN32) || (defined(LINUX) && !(defined(S390X)||defined(SDBonPPC64))) || defined(OSF1) || defined (FREEBSD) || (defined (SOLARIS) && (defined(I386) || defined(X86_64)))
#define sp77encodingUCS2Native sp77encodingUCS2Swapped
#else
#define sp77encodingUCS2Native sp77encodingUCS2
#endif
/*!
  Declaration: sp77encodingUCS4
  Description: SAPDB UCS4 String encoding

  SAPDB interpretation of UCS4: HighByte,LowByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS4;
/*!
  Declaration: sp77encodingUCS4Swapped
  Description: SAPDB swapped UCS4 encoding

  SAPDB interpretation of swapped UCS4: LowByte,HighByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS4Swapped;
/*!
  Declaration: sp77encodingUCS4Native
  Description: Machine dependend UCS4 encoding

  Native (machine dependend UCS4): Machine byte order
 */
/* PTS 1110303 */
#if defined(WIN32) || (defined(LINUX) && !(defined(S390X)||defined(SDBonPPC64))) || defined(OSF1) || defined (FREEBSD)
#define sp77encodingUCS4Native sp77encodingUCS4Swapped
#else
#define sp77encodingUCS4Native sp77encodingUCS4
#endif
/*!
  Declaration: sp77encodingUTF8
  Description: UTF8 String encoding

  UTF8 Character strings
 */
externCGlobal const tsp77encoding * const sp77encodingUTF8;
/*!
  Endchapter: Encodings
 */

/*!
  Chapter: Prototypes
 */
/*!
  Function:     sp77sprintf

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for sprintf

  Since the POSIX API does not support an snprintf function, usage of sprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  Arguments:
    buf [out] pointer to the result buffer space
    buflen [in] size of buffer (including ZERO termination byte)
    format [in] print format string
    ... [in] variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77sprintf (
    char *buf,
    int buflen,
    const char *format,
    ...);

/*!
  Function:     sp77vsprintf

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for vsprintf

  Since the POSIX API does not support an vsnprintf function, usage of vsprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  Arguments:
    buf [out] pointer to the result buffer space
    buflen [in] size of buffer (including ZERO termination byte)
    format [in] print format string see
    see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

    args [in] "stdarg.h C/C++ compatible" variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77vsprintf (
    char *buf,
    int buflen,
    const char *format,
    va_list args);

/*!
  Function:     sp77sprintfUnicode

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for sprintf with addition choice of output encoding

  Identical to sp77sprintf but with additional encoding parameter, that allows
  to specify output encoding.

  Since the POSIX API does not support an snprintf function, usage of sprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  NOTE Since buflen cannot be estimated in characters, if UTF8 characters are
  wanted, it only makes sense to specify the result buffer length in bytes!

  Arguments:
    encoding [in] The wanted encoding for output buffer
    buf [out] pointer to the result buffer space
    buflen [in] size of result buffer in bytes (including ZERO termination bytes)
    format [in] print format string
    ... [in] variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77sprintfUnicode (
    const tsp77encoding * encoding,
    void *buf,
    int buflen,
    const char *format,
    ...);

/*!
  Function:     sp77vsprintfUnicode

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for vsprintf with additional choice of output encoding

  Identical to sp77vsprintf but with additional encoding parameter, that allows
  to specify output encoding.

  Since the POSIX API does not support an vsnprintf function, usage of vsprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.
  Additional format %S convert an unicode string.

  An additional format modifier '=' was added, to allow to specify input encoding
  for unicode strings. This modifier is used in %r,%R and %S and otherwise ignored.

  If the modifier is found, two arguments are needed. First the input encoding
  (see gsp00.h: csp_ascii,csp_unicode,csp_unicode_swap,csp_unicode_native and csp_utf8)
  follwed by the string in the given encoding.

  Example

  sp77vsprintfUnicode(sp77encondigUTF8, buffer, len, "%-20=S", csp_unicode, UCS2Str)

  'buffer' is filled with UTF8 characters. Input is a 'HighByte,LowByte' UCS2 string.
  Maximum buffer size is len bytes (additional characters are silently discarded.

  NOTE Since 'len' cannot be estimated in characters, if UTF8 characters are
  wanted, it only makes sense to specify the result buffer length in bytes!

  Arguments:
    encoding [in] The wanted encoding for output buffer
    buf [out] pointer to the result buffer space
    buflen [in] size of result buffer in bytes (including ZERO termination bytes)
    format [in] print format string see (ALWAYS ascii)
    see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

    args [in] "stdarg.h C/C++ compatible" variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77vsprintfUnicode (
    const tsp77encoding * encoding,
    void *buf,
    int buflen,
    const char *format,
    va_list args);

/*!
  Function: sp77nativeUnicodeEncoding
  Description: Return machine dependend native unicode encoding

  For example Intel architectures are normally used as LSB first, SPARC is MSB first.

  Return value: sp77encodingUCS2 on MSB first machines, sp77encodingUCS2Swapped on LSB first machines
 */
externC const tsp77encoding *sp77nativeUnicodeEncoding();

/*!
  Function: sp77isValidEncoding
  returns: true when <Param>encoding</> is a pointer to an encoding class.
 */
externC teo00_Bool sp77isValidEncoding (const tsp77encoding * encoding);

/*!
  Endchapter: Prototypes
 */
#endif

#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//hsp78_0.h"
/*!**************************************************************************

  module       : hsp78.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: String Convertion
  description : Function accessing string convertion between Ascii/UCS2 and UTF8

  last changed: 2000-09-04  22:14
  first created:2000-08-23  18:38

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef HSP78_H
#define HSP78_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "hsp77.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/
/*!
  Declaration: tsp78ConversionResult
  Description: Conversion result codes

  sp78_Ok - Conversion sucessfull

  sp78_SourceExhausted - last character only partial in source buffer

  sp78_SourceCorrupted - source contains invalid chars or char sequences

  sp78_TargetExhausted - not enaugh room in target for conversion

  sp78_TargetNotTerminated - failed to add zero termination bytes

  sp78_BadSourceEncoding - unsupported source encoding

  sp78_BadTargetEncoding - unsupported destination encoding

  sp78_NullArgument - NULL pointer parsed
 */
typedef enum t_sp78ConversionResult
{
  sp78_Ok,              /* Conversion sucessfull */
  sp78_SourceExhausted, /* last character only partial in source buffer */
  sp78_SourceCorrupted, /* source contains invalid chars or char sequences */
  sp78_TargetExhausted, /* not enaugh room in target for conversion */
  sp78_TargetNotTerminated, /* failed to add zero termination bytes */
  sp78_BadSourceEncoding, /* unsupported source encoding */
  sp78_BadTargetEncoding, /* unsupported destination encoding */
  sp78_NullArgument       /* NULL pointer parsed */
} tsp78ConversionResult;
/*! EndDeclaration */

/*!
  Declaration: tsp78ConverterFunc
  Description: Conversion function pointer

  Converts srcBuffer into destBuffer unsig srcEncoding and destEncoding.
  Length of srcBufferInBytes is used ignoring any terminator characters in buffer.
  Length of destBufferInBytes is used to limit conversion if needed.
  The actual number of bytes parsed is returned in srcBytesParsed.
  The actual number of bytes written is returned in destBytesWritten.

  Possible results are sp78_Ok, sp78_SourceExhausted, sp78_SourceCorrupted and
  sp78_TargetExhausted. See tsp78ConversionResult for meaning of result codes.
 */
#if defined(__cplusplus)
extern "C" {
#endif
typedef tsp78ConversionResult (*tsp78ConvertFunc) (
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten);
/*! EndDeclaration */

/*!
  Declaration: sp78Converter
  Description: Array of converter function pointer

  First index is source , second index is destination.
 */
extern tsp78ConvertFunc sp78Converter[6][6];

#if defined(__cplusplus)
};
#endif
/*! EndDeclaration */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/*!
  Function: sp78convertString

  Description: String conversion function

  This call should be used instead of 'sp77sprintfUnicode' whenever only a conversion is needed
  and the number of bytes to be converted is known exactly. It returns more explicit error return value.

  Termination characters in the input buffer are converted like all other characters.
  But if addZeroTerminator is true, zero termination character is added if needed (that means last
  character is not already a zero terminator) and destBytesWritten modified according to it.
  If zero termination was not possible sp78_TargetNotTerminated is returned.

  Arguments: destBuffer [out] Destination buffer
             destBufferLengthInBytes [in] Destination buffer length in bytes, not characters
             destBytesWritten [out] Number of destination bytes written (including optional zeros)
             addZeroTerminator [in] Flag to force zero terminated output
             srcEncoding [in] Encoding of source buffer
             srcBuffer [in] Source buffer
             srcBufferLengthInBytes [in] Source buffer length in bytes, not characters
             srcBytesParsed [out] Number of source bytes parsed

  Return value: Conversion result of type tsp78ConversionResult

  destBytesWritten and srcBytesParsed are valid even if sp78_Ok was not returned.
 */
externC tsp78ConversionResult sp78convertString(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 tsp00_Bool     addZeroTerminator,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed);

/*!
  Function: sp78convertBuffer

  Description: Buffer conversion function

  Identical to sp78convertString if addZeroTerminator is false.

  Arguments: destBuffer [out] Destination buffer
             destBufferLengthInBytes [in] Destination buffer length in bytes, not characters
             destBytesWritten [out] Number of destination bytes written (including optional zeros)
             srcEncoding [in] Encoding of source buffer
             srcBuffer [in] Source buffer
             srcBufferLengthInBytes [in] Source buffer length in bytes, not characters
             srcBytesParsed [out] Number of source bytes parsed

  Return value: Conversion result of type tsp78ConversionResult

  destBytesWritten and srcBytesParsed are valid even if sp78_Ok was not returned.
 */
externC tsp78ConversionResult sp78convertBuffer(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed);

/*!
  Function: sp78getConvertFuncIndex

  Description: Map encoding to convert function index

  Arguments: encoding [in] Encoding to use

  Return value: Index to be used for referencing convert function in sp78convert array
 */
externC int sp78getConvertFuncIndex( const tsp77encoding *encoding );

/*!
  Function: sp78errString

  returns: a string representation of a tsp78ConversionResult
 */
externC const char * sp78errString (tsp78ConversionResult convResult);

#define csp78_ucs4      23
#define csp78_ucs4_swap 24

#endif  /* HSP78_H */
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpa04gc.h"

#line 86 "vpa04gc.c"
#ifdef DEBUG

#endif
#include <stdlib.h>
#include <time.h>

#include "vpa00global.h"
#include "vpa50Env.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "livecachetypes.h"

tsp81_CodePage   pa04gCodePage = {"WINDOWS-1252",
                                  {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,
                                   0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
                                   0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,
                                   0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
                                   0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
                                   0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
                                   0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
                                   0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
                                   0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,
                                   0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
                                   0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,
                                   0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
                                   0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,
                                   0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
                                   0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
                                   0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x007F,
                                   0x20AC,0x0081,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
                                   0x02C6,0x2030,0x0160,0x2039,0x0152,0x008D,0x017D,0x008F,
                                   0x0090,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
                                   0x02DC,0x2122,0x0161,0x203A,0x0153,0x009D,0x017E,0x0178,
                                   0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
                                   0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
                                   0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
                                   0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
                                   0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
                                   0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
                                   0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
                                   0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
                                   0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
                                   0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
                                   0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
                                   0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF}
                                 };
#define PRECOMREL30

#define PA01_SYMB_UNKNOWN 0
#define PA01_SYMB_QUOTED  1

/* PTS 1102741 */
#define PA04CLEN(c_len_ptr, c_ptr, real_c_len) (SQLINTEGER)\
/*if*/((c_len_ptr) ? \
    /*if*/((*c_len_ptr == SQL_NTS) ? \
        /*if*/((c_ptr) ? \
            API_STRLEN(c_ptr) \
        /*else*/: \
           0 /*(SDWORD)SQL_NULL_DATA*/) \
    /*else*/: \
        /*if*/((c_ptr) ? \
            /*if*/((*c_len_ptr == SQL_DEFAULT_PARAM) ? \
                real_c_len \
            /*else*/: \
                *c_len_ptr) \
        /*else*/: \
            0 /*SQL_NULL_DATA*/)) \
/*else*/: \
    /*if*/((c_ptr) ? \
         API_STRLEN(c_ptr) \
    /*else*/: \
         0 /*(SDWORD)SQL_NULL_DATA*/))

/* analog to PA04CLEN for wide strings (length in bytes!) */
#define PA04CLENW(c_len_ptr, c_ptr, real_c_len)  \
(SQLINTEGER)( \
/*if*/((c_len_ptr) ? \
    /*if*/((*c_len_ptr == SQL_NTS) ? \
        /*if*/((c_ptr) ? \
           sizeof(tsp81_UCS2Char) * \
           sp81UCS2strlen ((tsp81_UCS2Char*) c_ptr) \
        /*else*/: \
           0 /*(SDWORD)SQL_NULL_DATA*/) \
    /*else*/: \
        /*if*/((c_ptr) ? \
            /*if*/((*c_len_ptr == SQL_DEFAULT_PARAM) ? \
                real_c_len \
            /*else*/: \
                *c_len_ptr) \
        /*else*/: \
            0 /*SQL_NULL_DATA*/)) \
/*else*/: \
    /*if*/((c_ptr) ? \
         sizeof(tsp81_UCS2Char) * \
         sp81UCS2strlen ((tsp81_UCS2Char*) c_ptr) \
    /*else*/: \
         0 /*(SDWORD)SQL_NULL_DATA*/)))

const tsp77encoding *pa04gGetEncodingType(SWORD format);
static boolean pa04gcIsVarChar(SWORD format) ;
static void *  pa04SearchTerminator (void *ptr, int len);
static int pa04DetermineLength (UDWORD *c_len_ptr, UCHAR *c_ptr,
                                SQLULEN real_c_len, SWORD real_c_type);


/* move/convert a field from C to ODBC-SQL */
API_RETCODE pa04g_apgctos( SWORD      sql_type,
                           UCHAR     *sql_ptr,
                           UDWORD     sql_len,
                           UWORD      sql_scale,
                           UDWORD    *moved_len_ptr,
                           SWORD      fCtype,
                           UCHAR     *c_ptr,
                           UWORD      c_scale,
                           UDWORD    *c_len_ptr);

/* move/convert a field from ESQ to C */
API_RETCODE apgetoc(  SWORD   esq_type,
                      UCHAR  *esq_ptr,
                      UDWORD  esq_len,
                      UWORD   esq_scale,
                      UDWORD *moved_len_ptr,
                      SWORD   fCtype,
                      UWORD   numericScale,
                      UCHAR  *c_ptr,
                      SDWORD  c_len,
                      SDWORD *c_used_ptr )
{
    API_RETCODE api_retcode;
    UWORD sql_type;
    UDWORD sql_len;                 /* length of ODBC field       */
    UWORD sql_scale;

    API_TRACE(API_TR_ENTRY,"apgetoc",0);
    API_TRACE(API_TR_UWORD,"esq_type",&esq_type);
    API_TRACE(API_TR_PTR,"esq_ptr",&esq_ptr);
    API_TRACE(API_TR_UDWORD,"esq_len",&esq_len);
    API_TRACE(API_TR_UWORD,"esq_scale",&esq_scale);
    API_TRACE(API_TR_UDWORD,"*moved_len_ptr",moved_len_ptr);
    API_TRACE(API_TR_SWORD,"fCtype",&fCtype);
    API_TRACE(API_TR_PTR,"c_ptr",&c_ptr);
    API_TRACE(API_TR_UDWORD,"c_len",&c_len);
    API_TRACE(API_TR_PTR,"c_used_ptr",&c_used_ptr);


    api_retcode = API_OK;
    /* convert ESQ type  code to  */
    /* ODBC SQL type code         */
    api_retcode = apgstys( esq_type,
                           esq_len,
                           esq_scale,
                           (SWORD FAR *) &sql_type,
                           (UDWORD FAR *) &sql_len,
                           &sql_scale);

    if (api_retcode == API_OK) {
        api_retcode = apgstoc( sql_type,
                               esq_ptr,
                               sql_len,
                               sql_scale,
                               moved_len_ptr,
                               fCtype,
                               numericScale,
                               c_ptr,
                               c_len,
                               c_used_ptr);
    } /* of api_retcode == API_OK */

    API_TRACE(API_TR_EXIT,"apgetoc",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&api_retcode);
    API_TRACE(API_TR_UDWORD,"*c_used_ptr",c_used_ptr);

    return(api_retcode);
} /* apgetoc */


/* move/convert a field from ODBC/SQL to C */
API_RETCODE apgstoc( SWORD   sql_type,
                     UCHAR  *sql_ptr,
                     UDWORD  sql_len,
                     UWORD   sql_scale,
                     UDWORD *moved_len_ptr,
                     SWORD   fCtype,
                     UWORD   numericScale,
                     UCHAR  *c_ptr,
                     SDWORD  c_len,
                     SDWORD *c_used_ptr )
{
    API_RETCODE api_retcode;
    /* local variable-string      */
    UCHAR  *char_ptr;
    SWORD   real_c_type;
    SQLULEN real_c_len;
    UDWORD  used = 0;

    API_TRACE(API_TR_ENTRY,"apgstoc",0);
    API_TRACE(API_TR_SWORD,"sql_type",&sql_type);
    API_TRACE(API_TR_PTR,"sql_ptr",&sql_ptr);
    API_TRACE(API_TR_UDWORD,"sql_len",&sql_len);
    API_TRACE(API_TR_UWORD,"sql_scale",&sql_scale);
    API_TRACE(API_TR_UDWORD,"*moved_len_ptr",moved_len_ptr);
    API_TRACE(API_TR_SWORD,"fCtype",&fCtype);
    API_TRACE(API_TR_PTR,"c_ptr",&c_ptr);
    API_TRACE(API_TR_UDWORD,"c_len",&c_len);
    API_TRACE(API_TR_PTR,"c_used_ptr",&c_used_ptr);

    api_retcode = API_OK;


    if (apgisvstr(sql_type)) {
        char_ptr = apgvstraddr(sql_ptr);
        sql_len = apggvstrl (sql_ptr);
        sql_ptr[sql_len+API_VSTRING_OFFSET] = '\0';
    }
    else {
        char_ptr = sql_ptr;
        if (apgischar(sql_type)) {
            if (sql_type == SQL_CHAR)
                sql_type = SQL_VARCHAR;
            else
                if (sql_type == SQL_UNICODE)
                    sql_type = SQL_UNICODE_VARCHAR;
            /* if target variable is UCS2 one need two result bytes for
               one input byte */
/*             if (sizeof (SQLWCHAR) * sql_len < c_len) */
/*                 c_len = sizeof (SQLWCHAR) * sql_len; */
            if ((SDWORD) sql_len < c_len) {
              if (sql_type == SQL_VARCHAR && fCtype == SQL_C_WCHAR)
                c_len = sizeof (SQLWCHAR) * sql_len;
              else
                c_len = sql_len;
            }
        }
    }
    /* set the C type, may have   */
    /* been defaulted by client   */
    apgstyc( fCtype,
             c_len,
             sql_type,
             sql_len,
             sql_scale,
             (SWORD*) &real_c_type,
             &real_c_len);

    /* both fields have a real    */
    /* type, so let the mayhem    */
    /* begin                      */

    /* OUTER SWITCH (from field) */
    if (sql_len > 0) {
        switch (sql_type) {
        case (SQL_CHAR): {}
        case (SQL_UNICODE): {}
        case (SQL_VARCHAR): {}
        case (SQL_UNICODE_VARCHAR): {}
        case (SQL_LONGVARCHAR): {}
        case (SQL_UNICODE_LONGVARCHAR): {
            api_retcode = apfchar( char_ptr,
                                   sql_len,
                                   moved_len_ptr,
                                   sql_type,
                                   c_ptr,
                                   c_len,
                                   real_c_type,
                                   numericScale,
                                   &used );
            break;
        }
        case (SQL_BINARY): {}
        case (SQL_VARBINARY): {}
        case (SQL_LONGVARBINARY): {
            api_retcode = apfbinary( char_ptr,
                                     sql_len,
                                     moved_len_ptr,
                                     sql_type,
                                     c_ptr,
                                     c_len,
                                     real_c_type,
                                     &used);
            break;
        }
        case (SQL_BIT): {}
        case (SQL_SMALLINT): {}
        case (SQL_INTEGER): {}
        case (SQL_REAL): {}
        case (SQL_FLOAT): {}
        case (SQL_DOUBLE): {}
        case (SQL_DECIMAL): {}
        case (SQL_NUMERIC): {
            switch (real_c_type) {               /* INNER SWITCH      */
            case (SQL_C_CHAR): {}
            case (SQL_C_WCHAR): {}
            case (SQL_C_SSHORT): {}
            case (SQL_C_USHORT): {}
            case (SQL_C_SHORT): {}
            case (SQL_C_SLONG): {}
            case (SQL_C_ULONG): {}
            case (SQL_C_LONG): {}
            case (SQL_C_FLOAT): {}
            case (SQL_C_DOUBLE): {}
            case (SQL_C_BINARY): {}
            case (SQL_C_BIT): {}
            case (SQL_C_STINYINT): {}
            case (SQL_C_UTINYINT): {}
            case (SQL_C_TINYINT): {}
                /* these three are new in 3.0 */
            case (SQL_C_NUMERIC): {}
            case (SQL_C_SBIGINT): {}
            case (SQL_C_UBIGINT): {
                 api_retcode = apfnumeric( char_ptr,
                                           sql_len,
                                           sql_scale,
                                           moved_len_ptr,
                                           sql_type,
                                           c_ptr,
                                           c_len,
                                           real_c_type,
                                           numericScale,
                                           &used);
                break;
            }
            default: {
                API_TRACE(API_TR_SWORD, "format not supported", &real_c_type);
                api_retcode = API_NOT_OK;
                break;
            }
            } /* INNER SWITCH */
            break;
        }
        case (SQL_TYPE_DATE): {}
        case (SQL_TYPE_TIME): {}
        case (SQL_TYPE_TIMESTAMP): {}
        case (SQL_DATE): {}
        case (SQL_TIME): {}
        case (SQL_TIMESTAMP): {
            switch (real_c_type) {               /* INNER SWITCH      */
            case (SQL_C_WCHAR): {}
            case (SQL_C_CHAR): {}
            case (SQL_C_BINARY): {}
            case (SQL_C_TYPE_DATE): {}
            case (SQL_C_TYPE_TIME): {}
            case (SQL_C_TYPE_TIMESTAMP): {}
            case (SQL_C_DATE): {}
            case (SQL_C_TIME): {}
            case (SQL_C_TIMESTAMP): {
                api_retcode = apfdate( char_ptr,
                                       sql_len,
                                       moved_len_ptr,
                                       sql_type,
                                       c_ptr,
                                       c_len,
                                       real_c_type,
                                       &used);
                break;
            }
            default: {
                API_TRACE(API_TR_SWORD,"can't convert to", &real_c_type);
                api_retcode = API_NOT_OK;
                break;
            }
            } /* inner switch */
            break;
        }
        default: {
            API_TRACE(API_TR_SWORD,"** unknown ** sql_type", &sql_type);
            api_retcode = API_NOT_OK;
            break;
        }
        } /* switch SQL_XXX */
    }
    else {
        used = 0;
        if (*moved_len_ptr == -1)
            api_retcode = API_NO_DATA_FOUND;
        else {
            if (real_c_type == SQL_C_CHAR && c_ptr)
                *c_ptr = '\0';
            /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1132203 */
            if (real_c_type == SQL_C_WCHAR && c_ptr)
                *(SQLWCHAR*) c_ptr = 0;
            *moved_len_ptr = (UDWORD)-1;
        }
    }
    if (c_used_ptr != NULL &&
        (api_retcode == API_OK || api_retcode == API_TRUNCATE)) {
        if (api_retcode == API_TRUNCATE && apgislong(sql_type)) {
            *c_used_ptr = SQL_NO_TOTAL;
        }
        else {
            *c_used_ptr = used;
        }
    }

    API_TRACE(API_TR_UDWORD, "c_used_ptr", c_used_ptr);
    API_TRACE(API_TR_EXIT,"apgstoc",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&api_retcode);
    return(api_retcode);
} /* apgstoc */


UCHAR* apgvstraddr( UCHAR *ptr )
{
    UCHAR FAR * p;
    p = ptr + API_VSTRING_OFFSET;
    API_TRACE(API_TR_PTR, "apgvstraddr", &p);
    return (p);
} /* apgvstraddr */


VOID apgsvstrl( UCHAR *ptr, tsp00_Int4 len )
{
    if (ptr != NULL ) {
        /* UWORD l = (UWORD)len; */
        API_MEMCPY(ptr, &len, API_VSTRING_OFFSET);
        API_TRACE(API_TR_UDWORD, "apgsvstrl", &len);
    }
} /* apgsvstrl */


UDWORD apggvstrl( UCHAR *ptr )
{
    UDWORD l = 0;
    tsp00_Int4 x;
    API_MEMCPY(&x, ptr, API_VSTRING_OFFSET);
    l = (UDWORD) x;
    API_TRACE(API_TR_UDWORD,"apggvstrl", &l);
    return (l);
} /* apggvstrl */

/* apgstst() - convert string to a string    */
API_RETCODE apgstst( UCHAR  *in_ptr,
                     SDWORD  in_length,
                     SWORD   in_format,
                     UCHAR  *out_ptr,
                     SDWORD  out_length,
                     SWORD   out_format,
                     UDWORD *used )
{
    UCHAR FAR * ptr;
    UCHAR FAR * in_string_ptr;
    UCHAR FAR * out_string_ptr;
    const tsp77encoding *in_encoding;
    const tsp77encoding *out_encoding;
    int in_isTerminated;
    unsigned int sizeOfBlank;
    SDWORD len;
    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY, "apgstst", 0);
    in_string_ptr = in_ptr;
    API_TRACE(API_TR_PTR, "in_string_ptr", &in_string_ptr);
    API_TRACE(API_TR_UDWORD, "in_length", &in_length);
    API_TRACE(API_TR_UWORD, "in_format", &in_format);
    out_string_ptr = out_ptr;
    API_TRACE(API_TR_PTR, "out_string_ptr", &out_string_ptr);
    API_TRACE(API_TR_UDWORD, "out_length", &out_length);
    API_TRACE(API_TR_SWORD, "out_format", &out_format);

    api_retcode = API_OK;

    in_encoding = pa04gGetEncodingType(in_format);
    out_encoding = pa04gGetEncodingType(out_format);

    if ( in_length == SQL_NTS
         || in_format == SQL_VARCHAR
         || in_format == SQL_UNICODE_VARCHAR) {
      tsp00_Uint4 cLen, inLength = (tsp00_Uint4) in_length;
      int isCorrupted, isExhausted;
      /* maybe crashed if the string is unterminated (User Error) */
      in_encoding->stringInfo(in_string_ptr, 1UL << 31, TRUE,
                              &cLen, &inLength, &in_isTerminated,
                              &isCorrupted, &isExhausted);
      in_length = inLength;
    }
    else {
      in_isTerminated = FALSE;
    }
    API_TRACE(API_TR_UDWORD, "in_length", &in_length);
    API_TRACE_LEN(API_TR_ODBC_STRING, "in_string", in_string_ptr, in_length);
    if (out_length < 1) {
        *used = in_length;
        if (out_format == SQL_VARCHAR || out_format == SQL_CHAR) {
          if (in_encoding != sp77encodingAscii) {
            *used /= 2;
          }
        }
        else
          if (out_format == SQL_UNICODE || out_format == SQL_UNICODE_VARCHAR) {
            if (in_encoding == sp77encodingAscii) {
              *used *= 2;
            }
          }
        api_retcode = API_TRUNCATE;
    }
    else {
      tsp00_Uint4   cbWrite;        /* number of bytes written */
      tsp00_Uint4   cbParsed;       /* number of bytes parsed */
      tsp78ConversionResult rc;

      /* out_length is in character count */
/*       out_length *= out_encoding->fixedCharacterSize; */

      if (pa04gcIsVarChar(out_format)) {
        /* decrement the output size to append an zeroterminator later in this function */
        out_length -= out_encoding->terminatorSize;
      }

      rc = sp78convertBuffer(out_encoding, out_string_ptr, out_length,
                             &cbWrite,
                             in_encoding, in_string_ptr, in_length,
                             &cbParsed);
      if (rc != sp78_Ok) {
        if (rc == sp78_TargetExhausted) {
          /* ignore truncation if there are only blanks left in the input buffer */
          tsp00_Int4 cbPadBlanks;
          ptr = in_string_ptr+cbParsed;
          len = in_length-cbParsed;

          /* gives the number of padding Blanks in Bytes */
          if (in_encoding == sp77encodingUTF8)     /* fixedCharSize is 0! */
            sizeOfBlank = 1;
          else
            sizeOfBlank = in_encoding->fixedCharacterSize;
          cbPadBlanks = in_encoding->countPadChars(ptr, len, ' ') * sizeOfBlank;
          if (len == cbPadBlanks) {
            *used = cbWrite;
            api_retcode = API_OK;
          }
          else {
            /* byte length for output depends on ratio of character sizes
               of input and output encoding (does not work for UTF8!) */
            *used = in_length / in_encoding->fixedCharacterSize
                              * out_encoding->fixedCharacterSize;
            api_retcode = API_TRUNCATE;
          }
          if (pa04gcIsVarChar(out_format)) {
            tsp00_Uint4 targetLength = out_encoding->terminatorSize;
            ptr = out_string_ptr+cbWrite;
            out_encoding->fillString((void**)&ptr, &targetLength, 1, '\0');
          }
        }
        else
          if (rc == sp78_SourceCorrupted)   /* conversion was not possible */
            api_retcode = API_INVALID;
          else
            api_retcode = API_NOT_OK;
      }
      else {
        if (pa04gcIsVarChar(out_format)) {
          tsp00_Uint4 targetLength = out_encoding->terminatorSize;
          ptr = out_string_ptr+cbWrite;
          out_encoding->fillString((void**)&ptr, &targetLength, 1, '\0');
        }
        *used = cbWrite;
      }
    }
    API_TRACE(API_TR_STRING,"*out_string_ptr",out_string_ptr);
    API_TRACE(API_TR_UDWORD,"*used",used);
    API_TRACE(API_TR_EXIT,"apgstst",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return(api_retcode);
} /* apgstst */

API_RETCODE apgstdt( UCHAR  *in_ptr,
                     SWORD   in_format,
                     VOID   *out_ptr,
                     SWORD   out_format,
                     UDWORD *used)
{
    TIMESTAMP_STRUCT local_stamp;
    TIME_STRUCT *local_time;
    DATE_STRUCT *local_date;
    void *local_ptr = NULL;
    UCHAR FAR *char_ptr;
    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY,"apgstdt",0);
    API_TRACE(API_TR_PTR, "&in_ptr", &in_ptr);
    API_TRACE(API_TR_STRING,"in_ptr",in_ptr);
    API_TRACE(API_TR_UWORD,"in_format",&in_format);
    API_TRACE(API_TR_PTR, "&out_ptr", &out_ptr);
    API_TRACE(API_TR_UWORD,"out_format",&out_format);

    api_retcode = API_OK;
    local_date = (DATE_STRUCT *) &local_stamp.year;
    local_time = (TIME_STRUCT *) &local_stamp.hour;
    char_ptr = in_ptr;
    if (out_format == SQL_TYPE_DATE
        || out_format == SQL_DATE) {
        if (in_format != SQL_TYPE_DATE && in_format != SQL_TYPE_TIMESTAMP
            && in_format != SQL_DATE && in_format != SQL_TIMESTAMP) {
            api_retcode = API_NOT_OK;
        }
        else {
            /* build the year */
            local_date->year = (SWORD) atoi( (char*) char_ptr);
            char_ptr += 5;
            /* build the month */
            local_date->month = (UWORD) atoi( (char*) char_ptr);
            char_ptr += 3;
            /* build the day */
            local_date->day = (UWORD) atoi( (char*) char_ptr);
            char_ptr += 3;
            API_TRACE(API_TR_UWORD, "year", &local_date->year);
            API_TRACE(API_TR_UWORD, "month", &local_date->month);
            API_TRACE(API_TR_UWORD, "day", &local_date->day);
            *used = sizeof(DATE_STRUCT);
            local_ptr = local_date;
            if ( in_format == SQL_TYPE_TIMESTAMP
                 || in_format == SQL_TIMESTAMP) {
                apgstdt( char_ptr,
                         SQL_TYPE_TIME,
                         (VOID*) local_time,
                         SQL_TYPE_TIME,
                         used );
                if (local_time->hour != 0 || local_time->minute != 0 ||
                    local_time->second != 0) {
                    api_retcode = API_TRUNCATE;
                }
            }
        }
    }
    if ( out_format == SQL_TYPE_TIME
         || out_format == SQL_TIME) {
        if (in_format != SQL_TYPE_TIME && in_format != SQL_TYPE_TIMESTAMP
            && in_format != SQL_TIME && in_format != SQL_TIMESTAMP ) {
            api_retcode = API_NOT_OK;
        }
        else {
            if (in_format == SQL_TYPE_TIMESTAMP
                || in_format == SQL_TIMESTAMP) {
                apgstdt( char_ptr,
                         SQL_TYPE_DATE,
                         (VOID*) local_date,
                         SQL_TYPE_DATE,
                         used );
                if (local_date->year != 0 || local_date->month != 0 ||
                    local_date->day != 0) {
                    api_retcode = API_TRUNCATE;
                    char_ptr += 11;
                }
            }
            /* build the year */
            local_time->hour = (SWORD) atoi( (char*) char_ptr);
            char_ptr += 3;
            /* build the month */
            local_time->minute = (UWORD) atoi( (char*) char_ptr);
            char_ptr += 3;
            /* build the day */
            local_time->second = (UWORD) atoi( (char*) char_ptr);
            char_ptr += 3;
            API_TRACE(API_TR_UWORD, "hour", &local_time->hour);
            API_TRACE(API_TR_UWORD, "minute", &local_time->minute);
            API_TRACE(API_TR_UWORD, "second", &local_time->second);
            *used = sizeof(TIME_STRUCT);
            local_ptr = local_time;
        }
    }
    if (out_format == SQL_TYPE_TIMESTAMP
        || out_format == SQL_TIMESTAMP ) {
        if (in_format == SQL_TYPE_DATE || in_format == SQL_TYPE_TIMESTAMP
            || in_format == SQL_DATE || in_format == SQL_TIMESTAMP ) {
            apgstdt( char_ptr,
                     SQL_TYPE_DATE,
                     (VOID*) local_date,
                     SQL_TYPE_DATE,
                     used);
            if (in_format == SQL_TYPE_DATE || in_format == SQL_DATE) {
                local_stamp.hour = (UWORD) 0;
                local_stamp.minute = (UWORD) 0;
                local_stamp.second = (UWORD) 0;
                local_stamp.fraction = 0;
                API_TRACE(API_TR_UWORD, "hour", &local_time->hour);
                API_TRACE(API_TR_UWORD, "minute", &local_time->minute);
                API_TRACE(API_TR_UWORD, "second", &local_time->second);
            }
            else
                char_ptr += 11;
        }
        if (in_format == SQL_TYPE_TIME || in_format == SQL_TYPE_TIMESTAMP
            || in_format == SQL_TIME || in_format == SQL_TIMESTAMP) {
            apgstdt( char_ptr,
                     SQL_TYPE_TIME,
                     (VOID*) local_time,
                     SQL_TYPE_TIME,
                     used );
            if (in_format == SQL_TYPE_TIME || in_format == SQL_TIME ) {
                time_t ltime;
                struct tm *ltm;
                time(&ltime);
                ltm = localtime(&ltime);
                API_TRACE(API_TR_MSG, "localdate", 0);
                local_stamp.year = (UWORD) (ltm->tm_year+1900);
                local_stamp.month = (UWORD) (ltm->tm_mon+1);
                local_stamp.day = (UWORD) ltm->tm_mday;
                local_stamp.fraction = 0;
            }
            else
                char_ptr += 9;
        }
        if (in_format == SQL_TYPE_TIMESTAMP || in_format == SQL_TIMESTAMP) {
          local_stamp.fraction = 1000 * atol( (char*) char_ptr);  /* PTS 1121604 */
        }
        API_TRACE(API_TR_UDWORD, "fraction", &local_stamp.fraction);
        *used = sizeof(TIMESTAMP_STRUCT);
        local_ptr = &local_stamp;
    }
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        if (out_ptr != NULL && (int)*used > 0) {
            API_TRACE(API_TR_UDWORD, "*used", used);
            if ( out_format == SQL_TYPE_TIMESTAMP
                 || out_format == SQL_TYPE_DATE
                 || out_format == SQL_TIMESTAMP || out_format == SQL_DATE) {
                API_TRACE(API_TR_UWORD, "year", &local_date->year);
                API_TRACE(API_TR_UWORD, "month", &local_date->month);
                API_TRACE(API_TR_UWORD, "day", &local_date->day);
                if (out_format == SQL_TYPE_TIMESTAMP
                    || out_format == SQL_TIMESTAMP) {
                    API_TRACE(API_TR_UWORD, "hour", &local_stamp.hour);
                    API_TRACE(API_TR_UWORD, "minute", &local_stamp.minute);
                    API_TRACE(API_TR_UWORD, "second", &local_stamp.second);
                    API_TRACE(API_TR_UDWORD, "fraction",
                              &local_stamp.fraction);
                }
            }
            if (out_format == SQL_TYPE_TIME || out_format == SQL_TIME) {
                API_TRACE(API_TR_UWORD, "hour", &local_time->hour);
                API_TRACE(API_TR_UWORD, "minute", &local_time->minute);
                API_TRACE(API_TR_UWORD, "second", &local_time->second);
            }
            API_MEMCPY((TIMESTAMP_STRUCT *)out_ptr, local_ptr, (int)*used);
        }
        else
            api_retcode = API_NOT_OK;
    }
    API_TRACE(API_TR_EXIT,"apgstdt",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);

    return(api_retcode);
} /* apgstdt */


/* apgpkst() - convert packed decimal to ODBC style     */
/*             DECIMAL/NUMERIC                          */
VOID apgpkst( UCHAR  *packed_ptr,
              UWORD   scale,
              UDWORD *length_ptr,
              UCHAR  *decimal_ptr)
{
    char res;

    API_TRACE(API_TR_ENTRY,"apgpkst",0);
    API_TRACE(API_TR_PTR,"packed_ptr",&packed_ptr);
    API_TRACE(API_TR_UWORD,"scale",&scale);
    API_TRACE(API_TR_PTR,"length_ptr",&length_ptr);
    API_TRACE(API_TR_PTR,"decimal_string_ptr",&decimal_ptr);

    s46dctos( (char*) packed_ptr,
              (int) *length_ptr,
              (int) scale,
              (char*) decimal_ptr,
              1,
              API_CHAR_LENGTH_DECIMAL,
              (int *) length_ptr,
              (char *)&res );
    API_TRACE(API_TR_SWORD, "s46dctos", &res);
    decimal_ptr [*length_ptr]  = '\0';
    API_TRACE(API_TR_EXIT,"apgpkst",0);
    API_TRACE(API_TR_STRING,"*decimal_ptr",decimal_ptr);
} /* apgpkst */


/* move/convert a field from C to ESQ */
API_RETCODE apgctoe( SWORD   esq_type,
                     UCHAR  *esq_ptr,
                     UDWORD  esq_len,
                     UWORD   esq_scale,
                     UDWORD *moved_len_ptr,
                     SWORD   fCtype,
                     UCHAR  *c_ptr,
                     UDWORD  cbColDef,
                     UWORD   ibScale,
                     SQLLEN *c_used_ptr_in,
                     SWORD   fSqlType)
{

    API_RETCODE api_retcode;
    SWORD sql_type;
    UDWORD sql_len;                 /* length of ODBC field       */
    UWORD sql_scale;
    SQLULEN real_c_len;
    UDWORD dummy, *c_used_ptr;

    API_TRACE(API_TR_ENTRY,"apgctoe",0);
    API_TRACE(API_TR_SWORD,"esq_type",&esq_type);
    API_TRACE(API_TR_PTR,"esq_ptr",&esq_ptr);
    API_TRACE(API_TR_UDWORD,"esq_len",&esq_len);
    API_TRACE(API_TR_UWORD,"esq_scale",&esq_scale);
    API_TRACE(API_TR_UDWORD,"*moved_len_ptr",moved_len_ptr);
    API_TRACE(API_TR_SWORD,"fCtype",&fCtype);
    API_TRACE(API_TR_PTR,"c_ptr",&c_ptr);
    API_TRACE(API_TR_UWORD,"cbColDef",&cbColDef);
    API_TRACE(API_TR_UWORD,"ibScale",&ibScale);
    API_TRACE(API_TR_PTR,"c_used_ptr",&c_used_ptr);
    API_TRACE(API_TR_SWORD,"fSqlType",&fSqlType);

    if (c_used_ptr_in == NULL)
      c_used_ptr = NULL;
    else {
      c_used_ptr = &dummy;
      dummy = (UDWORD) *c_used_ptr_in;
    }

    api_retcode = API_OK;
    /* convert ESQ type code to   */
    /* ODBC type code             */
    api_retcode = apgstys( esq_type,
                           esq_len,
                           esq_scale,
                           (SWORD*) &sql_type,
                           (UDWORD*) &sql_len,
                           &sql_scale);

    if (api_retcode == API_OK) {
        apgstyc( fCtype,
                 0,
                 fSqlType,
                 sql_len,
                 sql_scale,
                 (SWORD*) &fCtype,
                 &real_c_len);
        api_retcode = pa04g_apgctos( sql_type,
                                     esq_ptr,
                                     sql_len,
                                     sql_scale,
                                     moved_len_ptr,
                                     fCtype,
                                     c_ptr,
                                     ibScale,
                                     c_used_ptr );
    }

    if (c_used_ptr_in != NULL)
      /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130257 */
      if (*c_used_ptr_in != SQL_NTS)
        *c_used_ptr_in = (SQLLEN) *c_used_ptr;

    API_TRACE(API_TR_EXIT, "apgctoe", 0);
    API_TRACE(API_TR_UDWORD, "*c_used_ptr", c_used_ptr);
    API_TRACE(API_TR_API_RETCODE, "retcode", &api_retcode);

    return(api_retcode);
} /* apgctoe */


/* move/convert a field from C to ODBC-SQL */
API_RETCODE pa04g_apgctos( SWORD      sql_type,
                           UCHAR     *sql_ptr,
                           UDWORD     sql_len,
                           UWORD      sql_scale,
                           UDWORD    *moved_len_ptr,
                           SWORD      fCtype,
                           UCHAR     *c_ptr,
                           UWORD      c_scale,
                           UDWORD    *c_len_ptr)
{
    API_RETCODE api_retcode;
    /* must be set for max double */
    UCHAR char_number [2*API_CHAR_LENGTH_CHAR];
    UCHAR  *char_ptr;
    SWORD   real_c_type = 0;
    SQLULEN real_c_len;
    SDWORD  c_len = 0;
    UDWORD  used = 0;

    API_TRACE(API_TR_ENTRY,"pa04g_apgctos", 0);
    API_TRACE(API_TR_SWORD,"sql_type", &sql_type);
    API_TRACE(API_TR_PTR,"sql_ptr", &sql_ptr);
    API_TRACE(API_TR_UDWORD,"sql_len", &sql_len);
    API_TRACE(API_TR_UWORD,"sql_scale", &sql_scale);
    API_TRACE(API_TR_UDWORD,"*moved_len_ptr", moved_len_ptr);
    API_TRACE(API_TR_SWORD,"fCtype", &fCtype);
    API_TRACE(API_TR_PTR,"c_ptr", &c_ptr);
    API_TRACE(API_TR_PTR,"c_len_ptr", &c_len_ptr);

    api_retcode = API_OK;
    if (apgisvstr(sql_type)) {
        char_ptr = apgvstraddr(sql_ptr);
        /* sql_len nicht setzten bei longspalten (SQLPutData) */
        /*      sql_len =  apggvstrl(sql_ptr); */

    }
    else
        char_ptr = sql_ptr;

    if (*moved_len_ptr) {
        char_ptr += *moved_len_ptr;
        sql_len -= *moved_len_ptr;
    }
    if (sql_len > 0) {
        /* set the C type, may have   */
        /* been defaulted by client   */
        apgstyc(fCtype, ((c_len_ptr) ? *c_len_ptr : 0), sql_type, sql_len,
                sql_scale, (SWORD FAR *) &real_c_type, &real_c_len);

        /* OUTER SWITCH (from field)  */
        switch (real_c_type) {
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1128557 */
        case (SQL_C_ABAPHANDLE): {
          if (sql_type == SQL_ABAPHANDLE) {
            API_MEMCPY (sql_ptr, c_ptr, sizeof(SQLStreamDesc));
          }
          else {
            api_retcode = API_NOT_OK;
          }
          break;
        }
        case (SQL_C_WCHAR): {}
        case (SQL_C_CHAR): {
            switch (sql_type) {         /* INNER SWITCH               */
            case (SQL_CHAR): {}
            case (SQL_UNICODE): {}
            case (SQL_VARCHAR): {}
            case (SQL_UNICODE_VARCHAR): {}
            case (SQL_LONGVARCHAR): {}
            case (SQL_UNICODE_LONGVARCHAR): {}
            case (SQL_BINARY): {}
            case (SQL_VARBINARY): {}
            case (SQL_LONGVARBINARY): {
                c_len = pa04DetermineLength (c_len_ptr, c_ptr, real_c_len,
                                             real_c_type);
                if (c_len >= 0) {
                    /* in vpr01l, p01l_putdata, data is copied to the
                       packet without converting. However, a unicode
                       db expects even long ascii character in unicode
                       (confirmed by HB), if the packet encoding is
                       unicode. Avoiding any changes in vpr01l leads
                       us to following quick&dirty solution */
                  if (sql_type == SQL_LONGVARCHAR && pa01GetUnicodeCatalog() != 0)
                      sql_type = SQL_WLONGVARCHAR;

                  api_retcode = aptchar(c_ptr, c_len, real_c_type,
                                        char_ptr, sql_len, sql_scale,
                                        sql_type, &used);
                }
                else {
                    api_retcode = API_INVALID_LENGTH;
                }
                break;
            }
            case (SQL_DECIMAL): {}
            case (SQL_NUMERIC): {}
            case (SQL_BIT): {}
            case (SQL_SMALLINT): {}
            case (SQL_INTEGER): {}
            case (SQL_REAL): {}
            case (SQL_FLOAT): {}
            case (SQL_DOUBLE):{
                c_len = pa04DetermineLength (c_len_ptr, c_ptr, real_c_len,
                                             real_c_type);
                if (c_len > 0) {
                    c_len = (c_len < (sizeof(char_number)-sizeof(tsp81_UCS2Char))) ?
                        c_len : (sizeof(char_number) - sizeof(tsp81_UCS2Char));

                    if (real_c_type == SQL_CHAR)
                      API_MEMCPY(char_number, c_ptr, (int) c_len);
                    else {
                      tsp00_Uint4   cbWrite;        /* number of bytes written */
                      tsp00_Uint4   cbParsed;       /* number of bytes parsed */

                      sp78convertBuffer (sp77encodingAscii, char_number, c_len,
                                         &cbWrite,
                                         sp77encodingUCS2Native, c_ptr, c_len,
                                         &cbParsed);
                      c_len = cbWrite;
                    }
                    char_number[c_len] = '\0';

                    pa04gpatchnumber(char_number);
                    api_retcode = aptnumeric( char_number,
                                              c_len, c_scale, SQL_CHAR,
                                              char_ptr,
                                              sql_len, sql_scale, sql_type,
                                              &used);
                }
                else {
                    if (c_len == 0) {
                        api_retcode = API_NOT_NUMBER;
                    }
                    else {
                        api_retcode = API_INVALID_LENGTH;
                    }
                }
                break;
            }
            case (SQL_TYPE_DATE): {}
            case (SQL_TYPE_TIME): {}
            case (SQL_TYPE_TIMESTAMP): {}
            case (SQL_DATE): {}
            case (SQL_TIME): {}
            case (SQL_TIMESTAMP): {
                c_len = pa04DetermineLength (c_len_ptr, c_ptr, real_c_len,
                                             real_c_type);
                if (c_len > 0) {
                    c_len = (c_len < (sizeof(char_number)-sizeof(tsp81_UCS2Char))) ?
                        c_len : (sizeof(char_number) - sizeof(tsp81_UCS2Char));

                    if (real_c_type == SQL_CHAR)
                      API_MEMCPY(char_number, c_ptr, (int) c_len);
                    else {
                      tsp00_Uint4   cbWrite;        /* number of bytes written */
                      tsp00_Uint4   cbParsed;       /* number of bytes parsed */

                      sp78convertBuffer (sp77encodingAscii, char_number, c_len,
                                         &cbWrite,
                                         sp77encodingUCS2Native, c_ptr, c_len,
                                         &cbParsed);
                      c_len = cbWrite;
                    }
                    char_number[c_len] = '\0';

                    pa04gpatchdate(char_number);
                    api_retcode = aptdate (char_number, c_len, SQL_CHAR,
                                           char_ptr, sql_len, sql_scale,
                                           sql_type, &used);
                }
                else {
                    if (c_len == 0) {
                        api_retcode = API_NOT_DATE;
                    }
                    else {
                        api_retcode = API_INVALID_LENGTH;
                    }
                }
                break;
            }
            default: {
                API_TRACE(API_TR_SWORD, "** unknown ** sql_type", &sql_type);
                api_retcode = API_NOT_OK;
                break;
            }
            } /* inner switch */
            break;
        }
        case (SQL_C_BIT): {}
        case (SQL_C_BINARY): {
            switch (sql_type) {
            case (SQL_CHAR): {}
            case (SQL_UNICODE): {}
            case (SQL_VARCHAR): {}
            case (SQL_UNICODE_VARCHAR): {}
            case (SQL_LONGVARCHAR): {}
            case (SQL_UNICODE_LONGVARCHAR): {}
            case (SQL_BIT): {}
            case (SQL_BINARY): {}
            case (SQL_VARBINARY): {}
            case (SQL_TYPE_DATE): {}
            case (SQL_TYPE_TIME): {}
            case (SQL_TYPE_TIMESTAMP): {}
            case (SQL_DATE): {}
            case (SQL_TIME): {}
            case (SQL_TIMESTAMP): {}
            case (SQL_LONGVARBINARY): {}
            case (SQL_DECIMAL): {} /* HBI */
            case (SQL_NUMERIC): {} /* HBI */
            case (SQL_SMALLINT): {}
            case (SQL_INTEGER): {}
            case (SQL_REAL): {}
            case (SQL_FLOAT): {}
            case (SQL_DOUBLE): {

                if (real_c_type == SQL_C_WCHAR) {
                    c_len = PA04CLENW (c_len_ptr, c_ptr, real_c_len);
                }
                else if (real_c_type == SQL_C_BIT) {
                      c_len = 1;
                }
                else
                    c_len = PA04CLEN (c_len_ptr, c_ptr, real_c_len);

                if (c_len >= 0) {
                    if (real_c_type == SQL_C_BIT)
                        api_retcode = aptbit (c_ptr, c_len, real_c_type,
                                              char_ptr, sql_len, sql_scale,
                                              sql_type, &used);
                    else
                        api_retcode = aptbinary(c_ptr, c_len, real_c_type,
                                                char_ptr, sql_len, sql_scale,
                                                sql_type, &used);
                }
                else {
                    api_retcode = API_INVALID_LENGTH;
                }
                break;
            }
            default: {
                API_TRACE(API_TR_SWORD,"** unknown ** sql_type", &sql_type);
                api_retcode = API_NOT_OK;
                break;
            }
            }
            break;
        }
        case (SQL_C_SSHORT): {}
        case (SQL_C_USHORT): {}
        case (SQL_C_SHORT): {}
        case (SQL_C_SLONG): {}
        case (SQL_C_ULONG): {}
        case (SQL_C_LONG): {}
        case (SQL_C_FLOAT): {}
        case (SQL_C_DOUBLE): {}
        case (SQL_C_STINYINT): {}
        case (SQL_C_UTINYINT): {}
        case (SQL_C_TINYINT): {}
        /* these three are new in 3.0 */
        case (SQL_C_NUMERIC): {}
        case (SQL_C_SBIGINT): {}
        case (SQL_C_UBIGINT): {
             api_retcode = aptnumeric(c_ptr,
                                      c_len, c_scale, real_c_type,
                                      char_ptr,
                                      sql_len, sql_scale, sql_type,
                                      &used);
            break;
        }
        case (SQL_C_TYPE_DATE): {}
        case (SQL_C_TYPE_TIME): {}
        case (SQL_C_TYPE_TIMESTAMP): {}
        case (SQL_C_DATE): {}
        case (SQL_C_TIME): {}
        case (SQL_C_TIMESTAMP): {
            switch (sql_type) {              /* INNER SWITCH      */
            case (SQL_CHAR): {}
            case (SQL_UNICODE): {}
            case (SQL_VARCHAR): {}
            case (SQL_UNICODE_VARCHAR): {}
            case (SQL_LONGVARCHAR): {}
            case (SQL_UNICODE_LONGVARCHAR): {}
            case (SQL_TYPE_DATE): {}
            case (SQL_TYPE_TIME): {}
            case (SQL_TYPE_TIMESTAMP): {}
            case (SQL_DATE): {}
            case (SQL_TIME): {}
            case (SQL_TIMESTAMP): {}
            case (SQL_LONGVARBINARY): {}
            case (SQL_VARBINARY): {}
            case (SQL_BINARY): {
                api_retcode = aptdate(c_ptr, c_len, real_c_type,
                                      char_ptr, sql_len, sql_scale,
                                      sql_type, &used);
                break;
            }
            default: {
                API_TRACE(API_TR_SWORD,"** unknown ** sql_type", &sql_type);
                api_retcode = API_NOT_OK;
                break;
            }
            }   /* inner switch */
            break;
        }

        default: {
            API_TRACE(API_TR_SWORD,"** unknown ** real_c_type", &real_c_type);
            api_retcode = API_NOT_OK;
            break;
        }
        }
    }
    else {
        used = 0;
        api_retcode = API_TRUNCATE;
    }

    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        UDWORD len = *moved_len_ptr;

        if (real_c_type == SQL_C_CHAR && sql_type == SQL_WLONGVARCHAR) {
          *moved_len_ptr += used / sizeof (SQLWCHAR);
          len += used;
        }
        else if (real_c_type == SQL_C_WCHAR && sql_type == SQL_LONGVARCHAR) {
          *moved_len_ptr += used;
          len += used * sizeof (SQLWCHAR);
        }
        else {
          *moved_len_ptr += used;
          len += used;
        }

        if (apgisvstr(sql_type)) {
            apgsvstrl (sql_ptr, (tsp00_Int4) len);
        }
    }
    API_TRACE(API_TR_EXIT,"pa04g_apgctos",0);
    API_TRACE(API_TR_UDWORD, "*c_len_ptr", c_len_ptr);
    API_TRACE(API_TR_API_RETCODE,"retcode",&api_retcode);

    return(api_retcode);
} /* apgctos */


/* apgstpk() - convert an ODBC/SQL style DECIMAL/NUMERIC      */
/*             to an ESQ packed format                        */
API_RETCODE apgstpk( UCHAR  *decimal_ptr,
                     UWORD   scale,
                     UDWORD  length,
                     UCHAR  *packed_ptr)
{
    UDWORD decimal_len;
    SWORD res=0;
    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY, "apgstpk",0);
    API_TRACE(API_TR_STRING, "decimal_ptr", decimal_ptr);
    API_TRACE(API_TR_UWORD,"scale",&scale);
    API_TRACE(API_TR_UDWORD,"length",&length);
    API_TRACE(API_TR_PTR,"packed_ptr",&packed_ptr);
    api_retcode = API_OK;

    decimal_len = (SQLUINTEGER) API_STRLEN(decimal_ptr);

    s46stodc( (char*) packed_ptr, (int) length, (int) scale,
              (char*) decimal_ptr, 1, (int) decimal_len, (char*)&res );
    switch (res) {
    case (0) : {
        break;
    }
    case (1) : {
        api_retcode = API_TRUNCATE;
        break;
    }
    case (2) : {
        api_retcode = API_NOT_NUMBER;
        break;
    }
    case (3) : {}
    default : {
        api_retcode = API_DATA_LOSS;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "s46stodc", &res);
    API_TRACE(API_TR_EXIT,"apgstpk",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    API_TRACE_BUF(packed_ptr, 1, 10);

    return(api_retcode);
} /* apgstpk */


/* apgstys() - set ODBC sql type code using ESQ type code     */
API_RETCODE apgstys( SWORD   esq_type,
                     UDWORD  esq_len,
                     UWORD   esq_scale,
                     SWORD  *sql_type,
                     UDWORD *sql_len,
                     UWORD  *sql_scale)
{
    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY,"apgstys", 0);
    API_TRACE(API_TR_SWORD,"esq_type", &esq_type);
    API_TRACE(API_TR_UDWORD,"esq_len", &esq_len);
    API_TRACE(API_TR_UWORD, "esq_scale", &esq_scale);

    api_retcode = API_OK;

    /* convert ESQ type code to   */
    /* ODBC type code             */
    *sql_scale = esq_scale;
    *sql_len = esq_len;
    switch (esq_type) {
    case (dcha): {}
    case (dche): {
        *sql_type = SQL_CHAR;
        /*  if (*sql_len > 0) */
        /*     *sql_len--;    */
        break;
    }
    case (dunicode): {
        *sql_type = SQL_UNICODE;
        /*  if (*sql_len > 0) */
        /*     *sql_len--;    */
        break;
    }
    case (dvarcharuni): {
        *sql_type = SQL_UNICODE_VARCHAR;
        break;
    }
    case (dvarchara): {}
    case (dvarchare): {
        *sql_type = SQL_VARCHAR;
        break;
    }
    case (dstra): {}
    case (dstre): {}
    case (dlonga): {}
    case (dlonge): {
        *sql_type = (UWORD) SQL_LONGVARCHAR;
        break;
    }
    case (dstruni): {}
    case (dlonguni): {
        *sql_type = (UWORD) SQL_UNICODE_LONGVARCHAR;
        break;
    }
    case (dstrb): {}
    case (dstrdb): {}
    case (dlongb): {}
    case (dlongdb): {
        *sql_type = (UWORD) SQL_LONGVARBINARY;
        break;
    }
    case (ddbyteebcdic): {}
    case (dchb): {}
    case (dvarcharb): {
        *sql_type = (UWORD) SQL_VARBINARY;
        break;
    }
    case (dinteger): {     /* integer */
        *sql_type = SQL_INTEGER;
        break;
    }
    case (dsmallint): {        /* short integer */
        *sql_type = SQL_SMALLINT;
        break;
    }
    case (dfloat): {        /* float & double */
        if (esq_len > sizeof(SFLOAT)) {
            *sql_type = SQL_FLOAT;
        }
        else {
            *sql_type = SQL_REAL;
        }
        break;
    }
    case (dfixed): {      /* no C type, packed dec. */
        *sql_type = SQL_DECIMAL;
        break;
    }
    case (ddate): {
        *sql_type = SQL_TYPE_DATE;
        break;
    }
    case (dtime): {
        *sql_type = SQL_TYPE_TIME;
        break;
    }
    case (dtimestamp): {
        *sql_type = SQL_TYPE_TIMESTAMP;
        break;
    }
    case (dboolean): {
        *sql_type = SQL_BIT;
        break;
    }
    case (dabaptabhandle): {
        *sql_type = SQL_ABAPHANDLE;
        break;
    }
    default:
        api_retcode = API_NOT_OK;
    }
    API_TRACE(API_TR_EXIT,"apgstys",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    API_TRACE(API_TR_SWORD,"*sql_type", sql_type);
    API_TRACE(API_TR_UDWORD,"*sql_len", sql_len);
    API_TRACE(API_TR_UWORD,"*sql_scale", sql_scale);

    return(api_retcode);
} /* apgstys */


/* apgstyc() - set ODBC C type code and len if type is defaulted    */
VOID apgstyc( SWORD    fCtype,
              SQLULEN  c_len,
              SWORD    sql_type,
              UDWORD   sql_len,
              UWORD    sql_scale,
              SWORD   *real_c_type,
              SQLULEN *real_c_len)
{

    API_TRACE(API_TR_ENTRY,"apgstyc", 0);
    API_TRACE(API_TR_SWORD,"fCtype", &fCtype);
    API_TRACE(API_TR_UDWORD,"c_len", &c_len);
    API_TRACE(API_TR_SWORD,"sql_type", &sql_type);
    API_TRACE(API_TR_UWORD,"sql_scale", &sql_scale);
    API_TRACE(API_TR_UDWORD,"sql_len", &sql_len);
    API_TRACE(API_TR_PTR, "&real_c_type",&real_c_type);

  /* set the C type, may have   */
  /* been defaulted by client   */
    if (fCtype != SQL_C_DEFAULT && c_len != SQL_DEFAULT_PARAM) {
        *real_c_type = fCtype;
        *real_c_len = c_len;
    }
    else {
        switch (sql_type) { /* ODBC book, pg 472 */
        case (SQL_CHAR): {}
        case (SQL_VARCHAR): {}
        case (SQL_LONGVARCHAR): {}
        case (SQL_BIGINT): {}
        case (SQL_DECIMAL): {}
        case (SQL_NUMERIC): {
            *real_c_type = SQL_C_CHAR;
            if (c_len == SQL_DEFAULT_PARAM)
                *real_c_len = sql_len;
            else {
                *real_c_len = c_len;
            }
            break;
        }
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126177 */
        case (SQL_UNICODE): {}
        case (SQL_UNICODE_VARCHAR): {}
        case (SQL_UNICODE_LONGVARCHAR): {
            *real_c_type = SQL_C_WCHAR;
            if (c_len == SQL_DEFAULT_PARAM)
                *real_c_len = sql_len;
            else {
                *real_c_len = c_len;
            }
            break;
        }
        case (SQL_BINARY): {}
        case (SQL_VARBINARY): {}
        case (SQL_LONGVARBINARY): {
            *real_c_type = SQL_C_BINARY;
            if (c_len == SQL_DEFAULT_PARAM)
                *real_c_len = sql_len;
            else {
                *real_c_len = c_len;
            }
            break;
        }
        case (SQL_BIT): {
            *real_c_type = SQL_C_BIT;
            *real_c_len = sizeof(UCHAR);
            break;
        }
        case (SQL_TINYINT): {}
        case (SQL_C_STINYINT): {}
        case (SQL_C_UTINYINT): {
            *real_c_type = SQL_C_TINYINT;
            *real_c_len = sizeof(SCHAR);
            break;
        }
        case (SQL_SMALLINT): {}
        case (SQL_C_SSHORT): {}
        case (SQL_C_USHORT): {
            *real_c_type = SQL_C_SHORT;
            *real_c_len = sizeof(SWORD);
            break;
        }
        case (SQL_INTEGER): {}
        case (SQL_C_SLONG): {}
        case (SQL_C_ULONG): {
            *real_c_type = SQL_C_LONG;
            *real_c_len = sizeof(SDWORD);
            break;
        }
        case (SQL_REAL): {
            *real_c_type = SQL_C_FLOAT;
            *real_c_len = sizeof(SFLOAT);
            break;
        }
        case (SQL_FLOAT): {}
        case (SQL_DOUBLE): {
            *real_c_type = SQL_C_DOUBLE;
            *real_c_len = sizeof(SDOUBLE);
            break;
        }
        case (SQL_DATE): {}    /* PTS 1121248 */
        case (SQL_TYPE_DATE): {
            *real_c_type = SQL_C_TYPE_DATE;
            *real_c_len = sizeof(DATE_STRUCT);
            break;
        }
        case (SQL_TIME): {}
        case (SQL_TYPE_TIME): {
            *real_c_type = SQL_C_TYPE_TIME;
            *real_c_len = sizeof(TIME_STRUCT);
            break;
        }
        case (SQL_TIMESTAMP): {}
        case (SQL_TYPE_TIMESTAMP): {
            *real_c_type = SQL_C_TYPE_TIMESTAMP;
            *real_c_len = sizeof(TIMESTAMP_STRUCT);
            break;
        }
        case (SQL_ABAPHANDLE): {
            *real_c_type = SQL_C_ABAPHANDLE;
            *real_c_len = sizeof(SQLStreamDesc);
            break;
        }
        default:
            *real_c_type = fCtype;
            *real_c_len = c_len;
        } /* of switch */
    } /* of fCtype != SQL_C_DEFAULT */

    API_TRACE(API_TR_EXIT,"apgstyc",0);
    API_TRACE(API_TR_SWORD,"*real_c_type", real_c_type);

    return;
} /* apgstys */


/* function to check whether the input string is a date */
API_RETCODE apgchdt( UCHAR FAR *string_ptr )
{
    UWORD local_yyyy = 0;
    UWORD local_mm = 0, local_dd = 0;
    UCHAR * last_ptr;
    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY,"apgchdt", 0);
    API_TRACE(API_TR_PTR,"string_ptr", &string_ptr);
    API_TRACE(API_TR_STRING,"string_ptr", string_ptr);

    api_retcode = API_OK;

    /* check for string not eq."" */
    if (string_ptr [0] == '\0' || string_ptr == NULL) {
        api_retcode = API_NOT_DATE;
    }
    if (api_retcode == API_OK) {
        local_yyyy = (SWORD) strtod( (char*) string_ptr, (char**) &last_ptr);
        API_TRACE(API_TR_SWORD,"local_yyyy", &local_yyyy);
        if ((errno == ERANGE) || (*last_ptr != PA_DATE_SEP)) {
            errno = 0;
            api_retcode = API_NOT_DATE;
        }
        if (api_retcode == API_OK) {
            local_mm = (UWORD) strtod( (char*) (last_ptr+1),
                                       (char**) &last_ptr);
            API_TRACE(API_TR_UWORD,"local_mm", &local_mm);
            if ((errno == ERANGE) || (*last_ptr != PA_DATE_SEP)) {
                errno = 0;
                api_retcode = API_NOT_DATE;
            }
            if (api_retcode == API_OK) {
                local_dd = (UWORD) strtod( (char*)(last_ptr+1),
                                           (char**)&last_ptr);
                API_TRACE(API_TR_UWORD,"local_dd", &local_dd);
                if ( (errno == ERANGE)
                     || ((*last_ptr != '\0')
                         && (*last_ptr != PA_TIMESTAMP_SEP))) {
                    errno = 0;
                    api_retcode = API_NOT_DATE;
                }
            }
        }
    }
    if (api_retcode == API_OK) {
        /* check the date  */
        api_retcode = API_NOT_DATE;
        if ( local_mm >= 1 && local_mm <= 12 )
            if (local_dd >= 1 && local_dd <= 31 )
                api_retcode = API_OK;
    }
    API_TRACE(API_TR_EXIT,"apgchdt", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode", &api_retcode);
    return(api_retcode);
} /* apgchdt */


/* function to check whether the input string is a time */
API_RETCODE apgchtm(UCHAR FAR *string_ptr)
{
    UWORD local_hh = 0, local_mm = 0, local_ss = 0;
    UCHAR * last_ptr;

    API_RETCODE api_retcode;

    API_TRACE(API_TR_ENTRY,"apgchtm", 0);
    API_TRACE(API_TR_PTR,"string_ptr", &string_ptr);
    API_TRACE(API_TR_STRING,"string_ptr", string_ptr);

    api_retcode = API_OK;

    /* check for string not eq."" */
    if (string_ptr [0] == '\0' || string_ptr == NULL) {
        api_retcode = API_NOT_DATE;
    }
    if (api_retcode == API_OK) {
        local_hh = (UWORD) strtod( (char*)string_ptr, (char**) &last_ptr);
        if ((errno == ERANGE) || (*last_ptr != PA_TIME_SEP)) {
            errno = 0;
            api_retcode = API_NOT_DATE;
        }
        if (api_retcode == API_OK) {
            local_mm = (UWORD) strtod( (char*)(last_ptr+1), (char**)&last_ptr);
            if ((errno == ERANGE) || (*last_ptr != PA_TIME_SEP)) {
                errno = 0;
                api_retcode = API_NOT_DATE;
            }
            if (api_retcode == API_OK) {
                local_ss = (UWORD) strtod((char*)(last_ptr+1),
                                          (char**)&last_ptr);
                if ( (errno == ERANGE)
                     || ((*last_ptr != '\0')
                         && (*last_ptr != ' ') && (*last_ptr != PA_FRAC_SEP))){
                    errno = 0;
                    api_retcode = API_NOT_DATE;
                }
            }
        }
    }
    if (api_retcode == API_OK) {
        /* check the date  */
        api_retcode = API_NOT_DATE;
        if ( local_hh <= 23 && local_mm <= 59 && local_ss <= 59 )
            api_retcode = API_OK;
    }
    API_TRACE(API_TR_EXIT,"apgchtm", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode", &api_retcode);
    return(api_retcode);
} /* apgchtm */


/* function to check whether the input string is a timestamp */
API_RETCODE apgchts(UCHAR FAR *string_ptr)
{
    API_RETCODE api_retcode;
    UCHAR FAR *ptr;
    UDWORD local_sf;
    API_TRACE(API_TR_ENTRY,"apgchts", 0);
    API_TRACE(API_TR_STRING,"string_ptr", string_ptr);
    api_retcode = apgchdt(string_ptr);
    if (api_retcode == API_OK) {
        ptr = (string_ptr + API_DATE_PREC);
        if ( *ptr == PA_TIMESTAMP_SEP) {
            api_retcode = apgchtm(ptr+1);
            if (api_retcode == API_OK) {
                ptr += API_TIME_PREC + 1;
                if (*ptr != '\0') {
                    local_sf = (UDWORD) strtod( (char*)(ptr+1), (char**) &ptr);
                    if ( (errno == ERANGE)
                         || ((*ptr != '\0') && (*ptr != ' '))) {
                        errno = 0;
                        api_retcode = API_NOT_DATE;
                    }
                }
            }
        }
        else {
            if (*ptr == ' ' || *ptr == '\0')
                api_retcode = API_OK;
            else
                api_retcode = API_NOT_OK;
        }
    }
    API_TRACE(API_TR_EXIT,"apgchts", 0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode", &api_retcode);
    return(api_retcode);
} /* apgchts */

BOOL apgisvstr(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_BINARY): {}
    case (SQL_VARBINARY):
    case (SQL_LONGVARCHAR): {}
    case (SQL_UNICODE_LONGVARCHAR): {}
    case (SQL_LONGVARBINARY):
        {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgisvstr", &ret);
    return(ret);
} /* apgisvstr */

BOOL apgisnum(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_NUMERIC): {}
    case (SQL_DECIMAL): {}
    case (SQL_INTEGER): {}
    case (SQL_SMALLINT): {}
    case (SQL_FLOAT): {}
    case (SQL_DOUBLE): {}
    case (SQL_REAL): {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgisnum", &ret);
    return(ret);
} /* apgisnum */


BOOL apgislong(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_LONGVARCHAR): {}
    case (SQL_UNICODE_LONGVARCHAR): {}
    case (SQL_LONGVARBINARY): {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgislong", &ret);
    return(ret);
} /* apgislong */


#define SQL_SPECIAL_NULL_DATA (-2)

BOOL apgisindi( tsp00_Int4 *ind_ptr)
{
    tsp00_Int4 ind; /* not SDWORD because it has 8 bytes on 64 bit platforms */
    BOOL ret=FALSE;
    if (ind_ptr) {
        API_MEMCPY(&ind, ind_ptr, sizeof(ind));
        if (ind == SQL_NULL_DATA)
            ret = TRUE;
        else
            if (ind == SQL_SPECIAL_NULL_DATA)
                ret = TRUE;
            else
                ret = FALSE;
    }
    API_TRACE(API_TR_SWORD, "apgisindi", &ret);
    return(ret);
} /* apgisindi */


BOOL apgischar(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_CHAR): {}
    case (SQL_UNICODE): {}
    case (SQL_VARCHAR): {}
    case (SQL_UNICODE_VARCHAR): {}
    case (SQL_LONGVARCHAR): {}
    case (SQL_UNICODE_LONGVARCHAR): {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgischar", &ret);
    return(ret);
} /* apgischar */


BOOL apgisbin(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_BINARY): {}
    case (SQL_VARBINARY): {}
    case (SQL_LONGVARBINARY): {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgisbin", &ret);
    return(ret);
} /* apgisbin */


BOOL apgistime(SWORD sql_type)
{
    BOOL ret;
    switch(sql_type) {
    case (SQL_TYPE_TIME): {}
    case (SQL_TYPE_DATE): {}
    case (SQL_TYPE_TIMESTAMP): {}
    case (SQL_TIME): {}
    case (SQL_DATE): {}
    case (SQL_TIMESTAMP): {
        ret = TRUE;
        break;
    }
    default: {
        ret = FALSE;
        break;
    }
    }
    API_TRACE(API_TR_SWORD, "apgistime", &ret);
    return(ret);
} /* apgistime */


API_RETCODE apgsti1( UCHAR  *in_string,
                     UDWORD  in_length,
                     UCHAR  *out_ptr,
                     SWORD   out_format)
{
    API_RETCODE api_retcode;
    SCHAR local_tiny;
    UWORD len;
    UCHAR char_number [API_CHAR_LENGTH_CHAR];
    API_TRACE(API_TR_ENTRY, "apgsti1", 0);
    len = sizeof(char_number)-2;
    len = (in_length < len) ? (UWORD) in_length : len;
    API_STRNCPY(char_number, in_string, len);
    char_number[len] = '\0';
    api_retcode = aptchsl(char_number, out_format);
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        local_tiny = (SCHAR) atoi( (char*) char_number);
        API_MEMCPY(out_ptr, (UCHAR FAR *) &local_tiny, sizeof(SCHAR));
    }
    API_TRACE(API_TR_SCHAR,"out_ptr", out_ptr);
    API_TRACE(API_TR_EXIT,"apgsti1",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    return (api_retcode);
} /* apgsti1 */


API_RETCODE apgsti2( UCHAR  *in_string,
                     UDWORD  in_length,
                     UCHAR  *out_ptr,
                     SWORD   out_format)
{
    API_RETCODE api_retcode;
    SWORD local_short;
    UWORD len;
    UCHAR char_number [API_CHAR_LENGTH_CHAR];
    API_TRACE(API_TR_ENTRY, "apgsti2", 0);
    len = sizeof(char_number)-2;
    len = (in_length < len) ? (UWORD) in_length : len;
    API_STRNCPY(char_number, in_string, len);
    char_number[len] = '\0';
    api_retcode = aptchsl(char_number, out_format);
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        local_short = (SWORD) atoi( (char*) char_number);
        API_MEMCPY(out_ptr, (UCHAR FAR *) &local_short, sizeof(SWORD));
    }
    API_TRACE(API_TR_SWORD,"out_ptr", out_ptr);
    API_TRACE(API_TR_EXIT,"apgsti2",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    return (api_retcode);
} /* apgsti2 */


API_RETCODE apgsti4( UCHAR  *in_string,
                     UDWORD  in_length,
                     UCHAR  *out_ptr,
                     SWORD   out_format)
{
    API_RETCODE api_retcode;
    SDWORD local_long;
    UWORD len;
    UCHAR char_number [API_CHAR_LENGTH_CHAR];
    API_TRACE(API_TR_ENTRY, "apgsti4", 0);
    len = sizeof(char_number)-2;
    len = (in_length < len) ? (UWORD) in_length : len;
    API_STRNCPY(char_number, in_string, len);
    char_number[len] = '\0';
    api_retcode = aptchsl(char_number, out_format);
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        local_long = atol( (char*) char_number);
        API_MEMCPY(out_ptr, (UCHAR FAR *) &local_long, sizeof(SDWORD));
    }
    API_TRACE(API_TR_SDWORD,"out_ptr", out_ptr);
    API_TRACE(API_TR_EXIT,"apgsti4",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    return (api_retcode);
} /* apgsti4 */

#ifdef BIGINT_SUPPORTED
/* string to bigint; new in 3.0  */
API_RETCODE apgsti8( UCHAR  *in_string,
                     UDWORD  in_length,
                     UCHAR  *out_ptr,
                     SWORD   out_format)
{
    API_RETCODE  api_retcode;

    UWORD len;
    UCHAR char_number [API_CHAR_LENGTH_CHAR];
    API_TRACE(API_TR_ENTRY, "apgsti8", 0);
    len = sizeof(char_number)-2;
    len = (in_length < len) ? (UWORD) in_length : len;
    API_STRNCPY(char_number, in_string, len);
    char_number[len] = '\0';
    api_retcode = aptchsl(char_number, out_format);
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        switch (out_format) {
        case SQL_C_UBIGINT: {
            API_UBIGINT   local_ubigint;

            local_ubigint = API_ATOI64( (char*) char_number );
            API_MEMCPY(out_ptr, (UCHAR*) &local_ubigint, sizeof(local_ubigint));
            break;
        };
        case SQL_C_SBIGINT:{
            API_BIGINT   local_bigint;

            local_bigint = API_ATOI64( (char*) char_number );
            API_MEMCPY(out_ptr, (UCHAR*) &local_bigint, sizeof(local_bigint));
            break;
        };
        default: /* illegal type */
            api_retcode = API_NOT_OK;
            break;
        }; /* switch */
    }
    API_TRACE(API_TR_SDWORD,"out_ptr", out_ptr);
    API_TRACE(API_TR_EXIT,"apgsti8",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    return (api_retcode);
} /* apgsti8 */
#endif /* BIGINT_SUPPORTED */

API_RETCODE apgstfd( UCHAR  *in_string,
                     UDWORD  in_length,
                     UCHAR  *out_ptr,
                     SWORD   out_format)
{
    API_RETCODE api_retcode;
    SFLOAT local_float;
    SDOUBLE local_double;
    UWORD len;
    UCHAR char_number [API_CHAR_LENGTH_CHAR];
    API_TRACE(API_TR_ENTRY, "apgstfd", 0);
    len = sizeof(char_number)-2;
    len = (in_length < len) ? (UWORD)in_length : len;
    API_STRNCPY(char_number, in_string, len);
    char_number[len] = '\0';
    api_retcode = aptchfd(char_number, out_format);
    if (api_retcode == API_OK || api_retcode == API_TRUNCATE) {
        if (out_format == SQL_C_FLOAT) {
            local_float = (SFLOAT) atof( (char*) char_number);
            API_MEMCPY(out_ptr, (UCHAR FAR *) &local_float, sizeof(SFLOAT));
        }
        else {
            local_double = atof( (char*) char_number);
            API_MEMCPY(out_ptr, (UCHAR FAR *) &local_double, sizeof(SDOUBLE));
        }
    }
    if (out_format == SQL_C_FLOAT) {
        API_TRACE(API_TR_SFLOAT,"out_ptr", out_ptr);
    }
    else {
        API_TRACE(API_TR_SDOUBLE,"out_ptr", out_ptr);
    }
    API_TRACE(API_TR_EXIT,"apgstfd",0);
    API_TRACE(API_TR_API_RETCODE,"api_retcode",&api_retcode);
    return (api_retcode);
} /* apgstfd */


VOID pa04gpatchdate(UCHAR FAR *str_ptr)
{
    /* patch the ODBC extension of date, time, timestamp to SQL Syntax */
    /* needs a copy of the string incase of modification */
    UCHAR FAR *s, *p;
    UCHAR local_str[30];
    UWORD symb;
    unsigned int length = (unsigned int) API_STRLEN (str_ptr);

    API_TRACE(API_TR_ENTRY, "pa04gpatchdate",0);
    API_TRACE(API_TR_STRING, "str_ptr", str_ptr);
    p=str_ptr;
    API_TRACE(API_TR_STRING, "p", p);
    for ( s=str_ptr;
          (s=pa01NextSymbol(s, &length, &symb,
                            local_str, sizeof(local_str))) != NULL;
          p=s ) {
        API_TRACE(API_TR_STRING, "local_str", &local_str);
        if (symb == PA01_SYMB_QUOTED) {
            API_STRCPY(str_ptr, local_str);
            break;
        }
        if (local_str[0] == '{') {
            s = pa01NextSymbol(s, &length, &symb,
                               local_str, sizeof(local_str));
            API_TRACE(API_TR_STRING, "s", s);
            aputoup ((UCHAR FAR *)local_str);
            if (API_STRCMP(local_str, "D") || API_STRCMP(local_str, "T") ||
                API_STRCMP(local_str, "TS") ) {
                s = pa01NextSymbol(s, &length, &symb,
                                   local_str, sizeof(local_str));
                if (symb == PA01_SYMB_QUOTED) {
                    API_STRCPY(str_ptr, local_str);
                    break;
                }
            }
        }
    }
    API_TRACE(API_TR_STRING, "str_ptr", str_ptr);
    API_TRACE(API_TR_EXIT, "pa04gpatchdate",0);
} /* pa04patchdate */


VOID pa04gpatchnumber(UCHAR FAR *str_ptr)
{
    /* patch the language specific numeric format */
    /* ACCESS uses a comma (comma instead of a decimal point) */
    /* needs a copy of the string incase of modification */
    UCHAR FAR *p;
    API_TRACE(API_TR_ENTRY, "pa04gpatchnumber",0);
    p = str_ptr;
    for (;p = (UCHAR*) API_STRCHR(p, ',');p++) {
        *p = '.';
        API_TRACE(API_TR_STRING, "str_ptr", str_ptr);
    }
    API_TRACE(API_TR_EXIT, "pa04gpatchnumber",0);
} /* pa04gpatchnumber */


RETCODE pa04gCheckConversion( SWORD   fCType,
                              SWORD   fSqlType,
                              UWORD  *sqlStatePtr )
{
    return SQL_SUCCESS;
} /* pa04gCheckConversion */

const tsp77encoding *pa04gGetEncodingType(SWORD format)
{
  switch(format) {
  case SQL_UNICODE: {}
  case (SQL_UNICODE_VARCHAR): {}
  case (SQL_UNICODE_LONGVARCHAR): {
    return sp77nativeUnicodeEncoding ();
    break;
  };
  default: {
    return sp77encodingAscii;
    break;
  }
  }; /* switch */
}

static boolean pa04gcIsVarChar(SWORD format)
{
  switch(format) {
  case (SQL_VARCHAR): {}
  case (SQL_UNICODE_VARCHAR): {}
  case (SQL_LONGVARCHAR): {}
  case (SQL_UNICODE_LONGVARCHAR): {
    return TRUE;
  }
  default: {
    return FALSE;
  }
  }
}

static void *  pa04SearchTerminator (void *ptr, int len)
{
  unsigned int i, length;
  tsp81_UCS2Char *p = (tsp81_UCS2Char*) ptr;

  if (len <= 0)
    return (NULL);
  else
    length = (unsigned int) len / sizeof(tsp81_UCS2Char);

  for (i=0; i<length; i++, p++)
    if (p->s == 0)
      return (p);

  return (NULL);
}

static int pa04DetermineLength (UDWORD *c_len_ptr,  UCHAR *c_ptr,
                                SQLULEN real_c_len, SWORD real_c_type)
{
  UCHAR *p;
  int c_len;

  if (real_c_type == SQL_C_CHAR) {
    c_len = PA04CLEN (c_len_ptr, c_ptr, real_c_len);
    if (c_len >= 0) {
      if (p = (UCHAR*) API_MEMCHR(c_ptr, '\0', (int) c_len))
        c_len = (SQLINTEGER) (p - c_ptr);
    }
  }
  else {
    c_len = PA04CLENW (c_len_ptr, c_ptr, real_c_len);
    if (c_len >= 0) {
      if (p = (UCHAR*) pa04SearchTerminator (c_ptr, (int) c_len))
        c_len = (SQLINTEGER) (p - c_ptr);
    }
  }
  return (c_len);
}



#ifdef TESTMAIN
#define NUMELEM(a) (sizeof(a)/sizeof(a[0]))
typedef struct apgstst_t {
  UCHAR  *in_ptr;
  SDWORD  in_length;
  SWORD   in_format;
  UCHAR  *out_ptr;
  SDWORD  out_length;
  SWORD   out_format;
} apgstst_t;

static apgstst_t t[] = {
  {"ABC", SQL_NTS, SQL_VARCHAR, NULL, 100, SQL_CHAR},
  {"1234    ", SQL_NTS, SQL_VARCHAR, NULL, 100, SQL_VARCHAR},
  {"abcdefghij", SQL_NTS, SQL_VARCHAR, NULL, 10, SQL_CHAR},
  {"987654321    ", SQL_NTS, SQL_VARCHAR, NULL, 10, SQL_VARCHAR}
};
main()
{
  API_RETCODE api_retcode;
  UDWORD used;
  int i;
  for (i=0; i < NUMELEM(t);i++) {
    t[i].out_ptr = malloc(t[i].out_length);
    api_retcode = apgstst( t[i].in_ptr, t[i].in_length, t[i].in_format, t[i].out_ptr, t[i].out_length, t[i].out_format, &used );
    api_retcode = apgststOld( t[i].in_ptr, t[i].in_length, t[i].in_format, t[i].out_ptr, t[i].out_length, t[i].out_format, &used );
    free(t[i].out_ptr);
  }
}
#endif /* TESTMAIN */

static char ident_sr[] = "@(#)vpa04gc dated 2006-07-17 05:56:25 used 2008-05-09 00:56:49";
extern char *vpa04gc_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpa04gc FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF";
static char ident_cc[] = "@(#)vpa04gc cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDLL -DHAS_NPTL -DSQLODBC -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpa04gc_force_whatinfo () { return( ident_cc ); }
