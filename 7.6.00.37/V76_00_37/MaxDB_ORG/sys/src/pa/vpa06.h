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
#ifndef VPA06_H
#define VPA06_H
/*!**********************************************************************

  module: vpa00global.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  misc functions

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa50Env.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/* apmstfc() - API miscellaneous function, set ODBC_FUNCTION code */
API_RETCODE apmstfc( SQLHENV  henv,
                     SQLHDBC  hdbc,
                     SQLHSTMT hstmt,
                     UWORD    function_code );


/* apmastr() - API miscellaneous function, allocate stmt area */

API_RETCODE apmastr ( API_HANDLE *phand,
                      SDWORD      num_elements);

    
/* apmrstr() - API miscellaneous function, resize stmt area */

API_RETCODE apmrstr( API_HANDLE *phand,
                     UWORD       old_stmt_len,
                     UWORD       new_stmt_len);


/* apmfstr() - API miscellaneous function, free stmt area */

API_RETCODE apmfstr(API_HANDLE hand);


/* apmlocp gets the parent and locks the child */
/* if there are not null */
VOID apmlocp ( SQLHSTMT    *hstmt,
               tpa60Stmt  **stmt_block_ptr,
               SQLHDBC     *hdbc,
               tpa40DBC   **dbc_block_ptr,
               SQLHENV     *henv,
               tpa50Env   **env_block_ptr );


API_RETCODE pa06AllocRowStatus ( API_HANDLE  *phand,
                                 DWORD        num_elements,
                                 DWORD        rowset_size );


UCHAR FAR *pa06UniqueName( UCHAR  *szPrefix, 
                           UCHAR  *szName, 
                           UWORD   cbMaxName, 
                           UDWORD  unique_number );


API_RETCODE pa06InitSQLCA(tpa40DBC *dbc_block_ptr);


VOID pa06SetTraceFileName(tpa40DBC *dbc_block_ptr);


RETCODE pa06stfcerror(API_RETCODE api_retcode, SQLHSTMT hstmt);

void pa06InitNullParseId( UCHAR *parsid );

SWORD pa06IsNullParseId( UCHAR *parsid );

void pa06ConvDateTimeTypes( tpa60Stmt *stmtBlockPtr, SWORD *pType );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
#endif
