/* @(#)sql__reset.c		2.1.1  2001-01-15  14:28 SAP DB */
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
sql__reset( struct iorec   *filep,
            char           *name,
            long           maxnamlen,
            long           datasize )
{
        if (name == NULL && filep == INPUT && filep->fname[0] == '\0') {
                if (fseek(filep->fbuf, (long)0, 0)) {
                        sql__peer("Could not reset ", filep->pfname);
                        return;
                }
                filep->funit &= ~EOFF;
                filep->funit |= (SYNC | EOLN);
                return;
        }
        filep = sql__gn(filep, name, maxnamlen, datasize);
        filep->fbuf = fopen(filep->fname, "r");
        if (filep->fbuf == NULL) {
                /*
                 * This allows unnamed temp files to be opened even if
                 * they have not been rewritten yet. We decided to remove
                 * this feature since the standard requires that files be
                 * defined before being reset.
                 *
                if (filep->funit & TEMP) {
                        filep->funit |= (EOFF | SYNC | FREAD);
                        return;
                }
                 */
                sql__peer("Could not open ", filep->pfname);
                return;
        }
        filep->funit |= (SYNC | FREAD);
        if (filep->funit & FTEXT)
                filep->funit |= EOLN;
        if (filep->fblk > PREDEF) {
                setbuf(filep->fbuf, &filep->buf[0]);
        }
}
