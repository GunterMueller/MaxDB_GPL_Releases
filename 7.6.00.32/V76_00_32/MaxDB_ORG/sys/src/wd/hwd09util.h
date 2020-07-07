/*!**********************************************************************

  module: hwd09util.h

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

#ifndef HWD09UTIL_H
#define HWD09UTIL_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#if defined(__cplusplus)
extern "C" {
#endif

sapdbwa_Bool wd09AddString( char			*targetStr,
							const char		*sourceStr,
							sapdbwa_UInt4	*freeLen	);

void wd09BackSlashToSlash( char	*string );

void wd09UpperCase( char *str );
void wd09LowerCase( char *str );

char *wd09strdup( const char *str );

char *wd09strndup( const char *str, sapdbwa_Int4 n );
    
#if defined(__cplusplus)
}
#endif

#endif
