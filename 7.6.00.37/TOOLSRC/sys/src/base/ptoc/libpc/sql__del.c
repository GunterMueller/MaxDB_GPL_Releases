/* @(#)sql__del.c		2.1.1  2001-01-15  14:09 SAP DB */
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

void
sql__del(char  *name, long  namlim)
{
        register long   cnt;
        register long   maxnamlen = namlim;
        char            namebuf[NAMSIZ];

        /*
         * trim trailing blanks, and insure that the name
         * will fit into the file structure
         */
        for (cnt = 0; cnt < maxnamlen; cnt++)
                if (name[cnt] == '\0' || name[cnt] == ' ')
                        break;
        if (cnt >= NAMSIZ) {
                sql__perrorp("%s: File name too long\n", (_ptoc_long)name, 0);
                return; /*NOTREACHED*/
        }
        maxnamlen = cnt;
        /*
         * put the name into the buffer with null termination
         */
        for (cnt = 0; cnt < maxnamlen; cnt++)
                namebuf[cnt] = name[cnt];
        namebuf[cnt] = '\0';
        /*
         * unlink the file
         */
        if (unlink(namebuf)) {
                sql__peer("Could not remove ", namebuf);
                return;
        }
}
