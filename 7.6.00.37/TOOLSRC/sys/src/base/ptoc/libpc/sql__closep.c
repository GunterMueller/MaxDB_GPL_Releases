/* @(#)sql__closep.c		2.1.1  2001-01-15  14:09 SAP DB */
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

/*
 * Close a Pascal file deallocating resources as appropriate.
 */

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

struct iorec *
sql__closep(struct iorec *filep, bool lastuse)
{
        if ((filep->funit & FDEF) == 0 && filep->fbuf != NULL) {
                /*
                 * Have a previous buffer, close associated file.
                 */
                if (filep->fblk > PREDEF) {
                        fflush(filep->fbuf);
                        setbuf(filep->fbuf, NULL);
                }
                if (0 != fclose(filep->fbuf)) {
                        sql__perrorp("%s: Close failed\n",
                                        (_ptoc_long) filep->pfname , 0L );
                        /*NOTREACHED*/
                        return ( (struct iorec *)-1 );
                }
                /*
                 * Temporary files are discarded.
                 */
                if ((filep->funit & TEMP) != 0 && lastuse &&
                    unlink(filep->pfname)) {
                        sql__peer("Could not remove ", filep->pfname);
                        /*NOTREACHED*/
                        return ( (struct iorec *)-1 );
                }
        }
        sql__actfile[filep->fblk] = FILNIL;
        return (filep->fchain);
}
