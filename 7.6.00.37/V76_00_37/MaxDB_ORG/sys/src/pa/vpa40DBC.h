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


