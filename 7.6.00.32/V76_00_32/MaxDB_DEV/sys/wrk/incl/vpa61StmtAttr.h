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
#ifndef VPA60STMTATTR_H
#define VPA60STMTATTR_H
/*!**********************************************************************

  module: vpa60StmtAttr.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC statement attributes

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: datastructures

  description: tpa61StmtAttr
  
************************************************************************/


typedef struct {
    /* tpa61SQLStmtAttr */
    UDWORD query_timeout;      /* Timeout for this statement.
                                * No SQL_CLOSE after a timeout           */
    UDWORD max_length;         /* Length of data for one column          */
    UDWORD max_select;         /* Maximum number of rows to return to
                                * the app. for a SELECT-Stmt.            */
    UDWORD noscan;             /* TRUE or FALSE (default) for scaning
                                * esc sequences                          */
    UDWORD async_enable;       /* 0 = Off (default); 1 = On;             */
    UDWORD reconnect_warning;  /* 0 = Off (default); 1 = On;             */
    UDWORD passthrough;        /* TRUE or FALSE (default) for scanning
                                * SQL Statements) for SQLExtendedFetch   */
  /*UDWORD bind_type; */       /* SQL_BIND_BY_COLUMN or length of Buffer */
    UDWORD cursor_type;        /* SQL_SCROLL_XXX types for the cursor
                                * (FORWARD | KEYSET | DYNAMIC | STATIC)  */
    UDWORD optimize_cursor;    /* SQL_TRUE/SQL_fALSE (default SQL_FALSE)
                                * When this flag is set to SQL_TRUE
                                * "FOR REUSE" is not appended to a select.
                                * So a keyset driven cursor hehaves like
                                * a dynamic cursor if the result table
                                * is not copied. Whether the result table
                                * is copied or not depends on the strategy
                                * of the DB kernel.                      */
    UDWORD concurrency;        /* SQL_CONCUR_READ_ONLY                   */
    UDWORD keyset_size;        /* any value not needed                   */
    /*UDWORD rowset_size; */       /* Number of Rows fetch by SQLExtendedFetch */
    UDWORD retrieve_data;      /* copy data to user buffer (ON | OFF)    */
    UDWORD use_bookmarks;      /* SQL_UB_OFF                             */
  /*    PACANCELPROC *lpCancelProc;    not longer in use */
    UWORD message_code;
    UWORD  sqlmode;            /* SQLMODE ( SQLDB, ORACLE, DB2, ANSI,
                                * SAPR3)                                 */
    UDWORD data_caching;       /* driver specific attr
                                * (see vpa00sqlod32.h)                   */

    UDWORD read_long_pos;      /* special attribut for positional reading
                                * of long values with SQLGetData.
                                * If this is <> 0 then SQLGetData reads
                                * from the position read_long_pos and
                                * read_long_pos is reset to 0. */
                               
    /* new in 3.0/3.5
     * ===============*/
    UDWORD cursorScrollable;   /* SQL_SCROLLABLE, SQL_NONSCROLLABLE      */
    UDWORD cursorSensitivity;  /* do changes affect another cursor?      */
    UDWORD enableAutoIPD;      /* automatic population of the IPD        */
    PTR    fetchBookmarkPtr;   /* binary bookmark value for fetch
                                * orientation SQL_FETCH_BOOKMARK         */
    UDWORD metadataId;         /* determines how the string arguments
                                * of catalog functions are treated.      */
} tpa61StmtAttr;

#endif

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa61InitStmtAttr

  description: Set default values

  arguments:
    stmtAttrPtr [in]  Pointer to statement attribute block
    
  return value: VOID
  */

VOID pa61InitStmtAttr( tpa61StmtAttr *stmtAttrPtr );

/*!
  Function: pa61GetStmtAttr

  description: Return value of statement attribute

  arguments:
    stmtAttrPtr [in]  Pointer to statement attribute block
    attr        [in]  Attribute type (SQL_ATTR_...)
    valuePtr    [out] Buffer to return value.
    bufferLen   [in]  Length of *valuePtr
    strLenPtr   [out] Buffer to return the length of the returned data
    sqlStatePtr [out] In the case of an error the SQLSTATE value is
                      written in this buffer.
  return value: SQL_ERROR, SQL_SUCCESS
  */

RETCODE pa61GetStmtAttr ( tpa61StmtAttr *stmtAttrPtr,
                          SDWORD         attr,
                          PTR            valuePtr,
                          SDWORD         bufferLen,
                          SDWORD        *strLenPtr,
                          UWORD         *sqlStatePtr );


/*!**********************************************************************
Function: pa61SetStmtAttr 

  description: Set value for statement attribute 

  arguments:
    hstmt       [in]  Statement handle
    stmtState   [in]  The current state of the statement handle
                      (allocated, ...)
    attr        [in]  Attribute type (SQL_ATTR_...)
    valuePtr    [in]  The new value.
    bufferLen   [in]  Length of *valuePtr
    stmtAttr    [in]  Pointer to statement attribute block
    sqlStatePtr [out] In the case of an error the SQLSTATE value is
                      written in this buffer.
                      
  return value: SQL_ERROR, SQL_SUCCESS
  */

RETCODE pa61SetStmtAttr( SQLHSTMT          hstmt,
                         UWORD          stmtState,
                         SDWORD         attr,
                         PTR            valuePtr,
                         SDWORD         strLen,
                         tpa61StmtAttr *stmtAttr,
                         UWORD         *sqlStatePtr );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

