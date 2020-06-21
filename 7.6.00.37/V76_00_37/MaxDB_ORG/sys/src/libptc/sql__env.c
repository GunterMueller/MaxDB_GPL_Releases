/* @(#)sql__env.c		2.1.1  2001-01-15  14:08 SAP DB */
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
#include <stdlib.h>
#else
extern char     *getenv ();
#endif

void
sql__env( char            *name,          /* name of environment var */
          long            namsiz,         /* sizeof (name) */
          char            *var,           /* pointer to pascal char array */
          long            siz )           /* sizeof(var) */
{
#define ENV_BUFLEN 100

        register char   *cp;
        register long   size = namsiz;
        char            buf [ENV_BUFLEN];

        if (size > ENV_BUFLEN) {           /* name is too long */
                sql__perrorp ("envp - name is too long (%ld > %1d chars)",
                        (long) size , (long) ENV_BUFLEN );
                return;
        }
        cp = &buf [0];
                                                /* copy the name to buf */
        while (*name != '\0' && *name != ' '  && size) {
                *cp++ = *name++;
                size--;
        }
        *cp = '\0';
        cp = getenv (buf);    /* Get the env. variable */
        size = siz;
        if (cp) {                       /* Env. Variable was found */
                do      {
                        *var++ = *cp++;
                } while (--size && *cp);
        }
        while (size--)                  /* Fill with blanks */
                *var++ = ' ';
}
