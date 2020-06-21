/*!
  @file           veo999sysrc.c
  @author         JoergM
  @special area   logging ...
  @brief          description ...
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "heo46.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

int main ( int argc, char **argv )
{
    if ( argc == 2 )
    {
        char ErrText [ 100 ] ;
        int ErrCode = atoi ( argv[1] ) ;

        eo46_system_error_text(ErrCode, ErrText, sizeof (ErrText) ) ;
        printf("\nErrortext for errorcode %d:\n'%s'\n\n", ErrCode, ErrText ) ;
    }
    else
        printf("Usage: <%s> <Error Number>\n", argv[0] ) ;
    return(0);
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/