/* @(#)sql__new.c		2.1.1  2001-01-15  14:06 SAP DB */
/*


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




 */

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

#if defined ( _WIN32 )
# include <stdlib.h>
#else
 extern  char *malloc();
#endif

char  *sql__minptr = (char *)MIN_POINTER;
char  *sql__maxptr = (char *)MAX_POINTER;

void
sql__new( char   **var,  /* pointer to item being created */
          long   size )  /* sizeof struct pointed to by var */
{
        char    *memblk;

#ifdef ADDR64
        memblk = malloc((size_t)size);
#else
        memblk = malloc((int)size);
#endif
        if (memblk == 0) {
                sql__perrorp ( "Ran out of memory\n" , 0L , 0L );
                /*NOTREACHED*/
        }
        *var = memblk;
        if (memblk < sql__minptr)
                sql__minptr = memblk;
        if (memblk + size > sql__maxptr)
                sql__maxptr = memblk + size;
}
