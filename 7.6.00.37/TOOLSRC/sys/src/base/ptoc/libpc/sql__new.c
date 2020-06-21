/* @(#)sql__new.c		2.1.1  2001-01-15  14:06 SAP DB */
/*

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

 */

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

# include <stdlib.h>


void
sql__new( char   **var,  /* pointer to item being created */
          long   size )  /* sizeof struct pointed to by var */
{
        char    *memblk;

        memblk = malloc((size_t)size);
        if (memblk == 0) {
                sql__perrorp ( "Ran out of memory\n" , 0L , 0L );
                /*NOTREACHED*/
        }
        *var = memblk;
}
