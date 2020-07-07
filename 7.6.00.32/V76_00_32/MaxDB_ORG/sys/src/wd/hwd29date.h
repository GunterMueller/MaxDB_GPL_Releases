/*!**********************************************************************

  module: hwd29date.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Date conversion functions

  see also:     RFC 822

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG



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




************************************************************************/

#ifndef HWD29DATE_H
#define HWD29DATE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: Date conversion functions

************************************************************************/

#define MAX_DATE_LEN_WD29 50

#define ODBC_DATE_LEN_WD29  11
#define ODBC_TIME_LEN_WD29  9

/* calculate differnce in moth between two dates */
SAPDB_Int4 wd29DiffMonth( char *date1, char *date2 );


#define DATE_FORMAT_RFC822_WD29   1
#define DATE_FORMAT_ISO8601_WD29  2

/* format odbc date. buf must be at least MAX_DATE_LEN_WD29 bytes big. */
void wd29FormatDate( SAPDB_Int4  format,
                     const char *odbcDate,
                     const char *odbcTime,
                     char       *buf );

/* get actual date in ODBC date format */
void wd29GetActualDate( char *dateBuf );


/* get actual time in ODBC time format */
void wd29GetActualTime( char *timeBuf );


/* get actual time in HTTP format */
SAPDB_UInt4 wd29HttpDate( char *Buf );


SAPDB_UInt4 wd29HttpDateToWADate( const char *http_date,
                                  char       *wa_date );


SAPDB_UInt4 wd29WADateToHttpDate( const char *my_date,
                                  char       *http_date );

/*!**********************************************************************

  EndChapter: Date conversion functions

************************************************************************/
#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
