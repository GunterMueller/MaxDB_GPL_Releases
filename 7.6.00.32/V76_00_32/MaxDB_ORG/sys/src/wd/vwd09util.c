/*!**********************************************************************

  module: vwd09util.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Utility functions (string functions etc.)

  see also:

  -----------------------------------------------------------------------

  copyright:                 Copyright (c) 1999-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <string.h>

#include "hwd09util.h"

#include "SAPDBCommon/SAPDB_string.h"

/***************************************************************************
 ***
 ***	wd09AddString
 ***
 ***************************************************************************/

sapdbwa_Bool wd09AddString( char        *targetStr,
                          const char  *sourceStr,
                          sapdbwa_UInt4 *freeLen )
{
    if (sourceStr != NULL && strlen(sourceStr) < *freeLen) {
        strcat( targetStr, sourceStr );
        freeLen -= strlen(sourceStr);
        return true;
    } else {
        return false;
    }; /* else */
} /* wd09AddString */

/***************************************************************************
 ***
 ***	wd09BackSlashToSlash
 ***
 ***************************************************************************/

void wd09BackSlashToSlash( char	*string ) {

	char *myString = string;

	if ( myString ) {
		while ( *myString != '\0' ) {
			if ( *myString++ == '\\' ) *(myString-1) = '/';
		}
	}
} /* wd09BackSlashToSlash */

/***************************************************************************
 ***
 ***	wd09UpperCase
 ***
 ***************************************************************************/

void wd09UpperCase( char *str )
{
    char *pos = str;
    
    while ( *pos != '\0' ) {
        *pos = toupper( *pos );
		pos++;
    } ; /* while */
} /* wd09UpperCase */

/***************************************************************************
 ***
 ***	wd09LowerCase
 ***
 ***************************************************************************/

void wd09LowerCase( char *str )
{
    char *pos = str;
    
    while ( *pos != '\0' ) {
        *pos = tolower( *pos );
		pos++;
    } ; /* while */
} /* wd09LowerCase */

/***************************************************************************
 ***
 ***	wd09strdup/wd09strndup
 ***
 ***************************************************************************/

char *wd09strdup( const char *s )
{
    char *res;
    size_t len;
    sapdbwa_Bool allocatOk = true;

    if (s == NULL)
	return NULL;
    len = strlen(s) + 1;
    sapdbwa_SQLALLOCAT( len, (sapdbwa_UInt1**) &res, &allocatOk);
    SAPDB_memcpy(res, s, len);
    
    return res;
} /* wd09strdup */

char *wd09strndup( const char *s, sapdbwa_Int4 n )
{
    char *res;
    sapdbwa_Bool allocatOk = true;
    
    if (s == NULL)
        return NULL;
    sapdbwa_SQLALLOCAT( n+1, (sapdbwa_UInt1**) &res, &allocatOk);
    SAPDB_memcpy( res, s, n);
    res[n] = '\0';
    
    return res;
} /* wd09strdup */
    

/***************************************************************************
 ***
 ***	End
 ***
 ***************************************************************************/
