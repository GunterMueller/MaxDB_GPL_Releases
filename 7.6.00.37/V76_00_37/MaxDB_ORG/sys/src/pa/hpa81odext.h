/*



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
#ifndef VPA81ODEXT_H
#define VPA81ODEXT_H
/*!**********************************************************************

  module: hpa81odext.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver Extension

  description:  C wrapper functions for VB

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"


SQLRETURN WINAPI pa81SetStrStmtOption( SQLHSTMT     hstmt,
                                       SQLUSMALLINT option,
                                       SQLPOINTER   value);
                               
SQLRETURN WINAPI pa81SetStrConnectOption( SQLHSTMT     hstmt,
                                          SQLUSMALLINT option,
                                          SQLPOINTER   value);

SQLRETURN WINAPI pa81GetStrConnectOption( SQLHSTMT     hstmt,
                                          SQLUSMALLINT option,
                                          SQLPOINTER   value);

#endif
