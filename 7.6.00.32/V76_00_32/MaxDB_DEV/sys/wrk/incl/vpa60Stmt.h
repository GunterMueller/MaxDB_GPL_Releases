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
