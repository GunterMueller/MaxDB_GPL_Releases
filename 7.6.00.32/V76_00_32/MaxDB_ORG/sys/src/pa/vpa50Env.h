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


