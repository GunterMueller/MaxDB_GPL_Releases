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
#ifndef HPA90INFO_H
#define HPA90INFO_H
/*!**********************************************************************

  module: hpa90info.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Convert ODBC types and codes to string (e.g for tracing)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: Conversion functions for odbc types

************************************************************************/

char* pa90ColAttrAsString( UWORD type, char *literal );

char* pa90DescTypeAsString( SWORD type, char *literal );

char* pa90GetInfoAttrAsString( UWORD attr, char *literal );

char* pa90StmtAttrAsString( SDWORD attr, char *literal );

char* pa90SQLTypeAsString( SWORD attr, char *literal );

char* pa90CTypeAsString( SWORD attr, char *literal );

char* pa90EnvAttrAsString( SWORD attr, char *literal );

char* pa90ConnAttrAsString( SDWORD attr, char *literal );

char* pa90UpdateableAsString( SDWORD attr, char *literal );

char* pa90DiagFieldAsString( SDWORD attr, char *literal );

char* pa90CursorTypeAsString( UDWORD attr, char *literal );

char* pa90FetchTypeAsString( UWORD attr, char *literal );

/* operation type for SQLSetPos */
char* pa90OpTypeAsString( UWORD opType, char *literal );

/*!**********************************************************************

  EndChapter: Conversion functions for odbc types

************************************************************************/

#endif
