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
#ifndef VPA41ESQBLOCK_H
#define VPA41ESQBLOCK_H
/*!**********************************************************************

  module: vpa41ESQBlock.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  struct tpa41ESQBlock

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: datastructures

  description: tpa41ESQBlock
  
************************************************************************/

/* a p i _ e s q _ b l o c k */
typedef struct {
  SDWORD esq_sql_code;            /* SQL code from ESQ          */
  UCHAR  sql_err_text [ PA_ERROR_TEXT_LEN + 1];
  SDWORD number_rows_modified;  /* number of rows touched   */
  UWORD  odbc_function_code;      /* ODBC ordinal               */
  sqlcatype sqlca;
  api_trace_block *tracep;
  void *sqlareas;
} tpa41ESQBlock;

/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/
/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa41apmercv

  description: Convert error code ESQ code to ODBC code.

  arguments:
    esqblk_ptr  [in]  Pointer to ESQBlock.
    
  return value: ODBC error code.
  */

UWORD pa41apmercv ( tpa41ESQBlock *esqblk_ptr );


/*!
  Function: pa41apmercv

  description: Set special SQL-ODBC error code

  For some ODBC errors we set error information in the ESQBlock
  by this function.
  
  arguments:
    code        [in]  ODBC error code
    esqblk_ptr  [in]  Pointer to ESQBlock.
    
  return value: API_OK, PA_ROW_NOT_FOUND.
  */

API_RETCODE pa41apmmker( SWORD          code,
                         tpa41ESQBlock *api_cb_ptr);


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
