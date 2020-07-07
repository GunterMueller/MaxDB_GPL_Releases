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
#ifndef HPA05NUM_H
#define HPA05NUM_H
/*!**********************************************************************

  module: hpa05num.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Conversion for SQL_NUMERIC struct

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

/*!**********************************************************************

  Chapter: conversion functions for SQL_NUMERIC_STRUCT

************************************************************************/

#define MAX_NUMERIC_DIGITS_PA05  38
#define MAX_NUMERIC_BYTES_PA05   SQL_MAX_NUMERIC_LEN

/* numeric conversion
 */
API_RETCODE pa05Numeric2String( SQL_NUMERIC_STRUCT *numeric,
                                UWORD               numericScale,
                                UCHAR              *stringBuf,
                                UDWORD              bufSize );


API_RETCODE pa05String2Numeric( UCHAR              *string,
                                UWORD               numericScale,
                                SQL_NUMERIC_STRUCT *numeric );


API_RETCODE pa05Double2Numeric( SDOUBLE             number,
                                SQL_NUMERIC_STRUCT *numeric );


/*!**********************************************************************

  EndChapter: conversion functions for SQL_NUMERIC_STRUCT

************************************************************************/
#endif
