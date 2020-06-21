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
#ifndef VPA30_H
#define VPA30_H
/*!**********************************************************************

  module: vpa30.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Communication with Interface Runtime

  see also:     

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

#include "vpa40DBC.h"
#include "vpa41ESQBlock.h"
#include "vpa60Stmt.h"

#include "vpr05IfCom_String.h"   /* for unicode */
/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*

  functions:

  Transaction functions:

  (1)  apeconn  - Perform ESQ CONNECT
  (2)  apedisc  - Perform ESQ DISCONNECT
  (3)  apecomm  - Perform ESQ COMMIT WORK
  (4)  aperoll  - Perform ESQ ROLLBACK WORK

  Compile/Execute functions:

  (5)  apeprep  - Perform ESQ Dynamic PREPARE
  (6)  apeexec  - Perform ESQ Dynamic EXECUTE
  (7)  apedesc  - Perform ESQ Dynamic DESCRIBE
  (8)  apeimme  - Perform ESQ Dynamic EXECUTE IMMEDIATE

  Cursor functions:

  (10) apeopen  - Perform ESQ OPEN CURSOR
  (11) apefetc  - Perform ESQ FETCH CURSOR
  (12) apeclos  - Perform ESQ CLOSE CURSOR

*/

VOID aperetg  (tpa41ESQBlock FAR * api_cb_ptr);


/* servernode is taken directly from api_dbc_ptr->servernode */
API_RETCODE apeconn (  tpa41ESQBlock *api_cb_ptr,
                       UCHAR         *serverdb_ptr,   SWORD serverdb_len,
                       tpr05_String  *useridStr,
                       tpr05_String  *AuthStr,
/*                       UCHAR         *userid_ptr,     SWORD userid_len,*/
/*                       UCHAR         *authstr_ptr,    SWORD authstr_len,*/
                       tpa40DBC      *api_dbc_ptr);


VOID apedisc ( tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr);


VOID apecancel ( tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr);


VOID apecomm (tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr);


VOID aperoll ( tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr);


API_RETCODE  apeprep ( tpa41ESQBlock *api_cb_ptr,
                       tpr05_String  *SqlStmt,
                       UCHAR         *parsid_ptr,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr );


API_RETCODE apeexec ( tpa41ESQBlock *api_cb_ptr,
                      tpr05_String  *SqlStmt,
                      UCHAR         *parsid_ptr,
                      tpa40DBC      *api_dbc_ptr,
                      tpa60Stmt     *api_stmt_ptr );


API_RETCODE apeput1l ( tpa41ESQBlock *api_cb_ptr,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr );


API_RETCODE apegetl ( tpa41ESQBlock  *api_cb_ptr,
                      tpa40DBC FAR   *api_dbc_ptr,
                      tpa60Stmt FAR  *api_stmt_ptr,
                      tpa20Desc      *impl_desc_ptr, 
                      SWORD          *col );


API_RETCODE apedesc ( tpa41ESQBlock *api_cb_ptr,
                      UCHAR         *parsid_ptr,
                      tpa40DBC      *api_dbc_ptr, 
                      tpa60Stmt     *api_stmt_ptr);


VOID apedrprs ( tpa41ESQBlock *api_cb_ptr,
                UCHAR         *parsid_ptr,
                tpa40DBC      *api_dbc_ptr,
                tpa60Stmt     *api_stmt_ptr);


API_RETCODE  apeimme ( tpa41ESQBlock *api_cb_ptr,
                       tpr05_String  *SqlStmt,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr);


API_RETCODE apefetc ( tpa41ESQBlock *api_cb_ptr,
                      tpa40DBC FAR  *api_dbc_ptr,
                      tpa60Stmt     *api_stmt_ptr,
                      UWORD          fetchtype,
                      SDWORD         pos );

VOID apeclos ( tpa41ESQBlock *api_cb_ptr,
               tpr05_String  *curnam_ptr,
               tpa40DBC      *api_dbc_ptr,
               tpa60Stmt     *api_stmt_ptr );


SWORD apereconn( tpa41ESQBlock *api_cb_ptr, 
                 tpa40DBC      *api_dbc_ptr);


API_RETCODE pa30commit( tpa41ESQBlock *api_cb_ptr,
                        tpa40DBC      *api_dbc_ptr);


API_RETCODE pa30SubtransBegin( tpa41ESQBlock *api_cb_ptr,
                               tpa40DBC      *api_dbc_ptr );


API_RETCODE pa30SubtransEnd( tpa41ESQBlock *api_cb_ptr,
                             tpa40DBC      *api_dbc_ptr );


API_RETCODE pa30SubtransRollback( tpa41ESQBlock *api_cb_ptr,
                                  tpa40DBC      *api_dbc_ptr );


API_RETCODE  pa30SyntaxCheck ( tpa41ESQBlock *api_cb_ptr,
                               tpa40DBC      *api_dbc_ptr,
                               tpa60Stmt     *api_stmt_ptr,
                               tpr05_String  *SqlCmd);

#endif
