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
#ifndef VPA10HANDLE_H
#define VPA10HANDLE_H
/*!**********************************************************************

  module: vpa10Handle.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  ODBC-Handles (environment,connection,statement,descriptor)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:           Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: ODBC functions
  description:
  
  - SQLAllocHandle
  - SQLFreeHandle
  - SQLGetDiagField
  - SQLGetDiagRec

  For sepecification see Microsoft ODBC 3.5 reference. */
/*!  
  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/
/*!
  Function: pa10GetODBCVersion

  description: Get the ODBC-Version from the environment attribute
               SQL_ATTR_ODBC_VER

  arguments:
    handleType   [in]  The type of handle. (SQL_HANDLE_ENV, SQL_HANDLE_DBC,
                       SQL_HANDLE_STMT or SQL_HANDLE_DESC)
    handle       [in]  The handle that is used to retrieve the enrironment
                       attribute.
    odbcVersion  [out] Pointer to a buffer to return the ODBC version.
      
    return value: TRUE for succes FALSE for failure */

tsp00_Bool pa10GetODBCVersion( SWORD      handleType,
                               SQLHANDLE  handle,
                               UDWORD    *odbcVersion );

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif
