/* @(#)sql__peer.c		2.1.1  2001-01-15  14:05 SAP DB */
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

#include        <stdio.h>
#include        <stdlib.h>
#include        <signal.h>

#if defined(_WIN32)
 #include <windows.h>
#endif

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

/*
 * Routine sql__peer is called from the runtime library when a runtime
 * I/O error occurs. Its arguments are a pointer to an error message and
 * the name of the offending file.
 */
void
sql__peer(char *msg, char *fname)
{
#if defined(_WIN32)
        if (!fname)
        {
            sql__perrorp("%s\nError %d", (_ptoc_long)msg, GetLastError());
        }
        else
        {
            char buf[2048];
            strcpy(buf, msg);
            strcat(buf, "\n%s: Error %d");
            sql__perrorp(msg, (_ptoc_long)fname, GetLastError());
        }
#else
        sql__flp();
        fputc('\n',stderr);
        fputs(msg, stderr);
        perror(fname);
#endif

#if defined(_WIN32)
        exit ((int)-1);
#else
        kill(getpid(), SIGTRAP);
#endif
        /*NOTREACHED*/
}
