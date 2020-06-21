/* @(#)sql__sync.c		2.1.1  2001-01-15  14:57 SAP DB */
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

/*
 * insure that a usable image is in the buffer window
 */

char sql__readbf [] = "%s: Tried to read past end of file\n";
char sql__openwerr [] = "%s: Attempt to read, but open for writing\n";

void
sql__sync(struct iorec   *curfile)
{
        char                    *limit, *ptr;

/**
 *      if (curfile->funit & FWRITE) {
 *              sql__perrorp ( sql__openwerr , (long) curfile->pfname , 0L );
 *              / *NOTREACHED* /
 *      }
 **/
        if ((curfile->funit & SYNC) == 0) {
                return;
        }
        if (curfile->funit & EOFF) {
		        sql__perrorp ( sql__readbf , (_ptoc_long) curfile->pfname , 0L );
		        /*NOTREACHED*/
        }
        curfile->funit &= ~SYNC;
        if (curfile->funit & SPEOLN) {
                curfile->funit &= ~(SPEOLN|EOLN);
                curfile->funit |= EOFF;
                return;
        }
        fread(curfile->fileptr, (int)curfile->fsize, 1, curfile->fbuf);
        if (ferror(curfile->fbuf)) {
		        sql__perrorp ( sql__readbf , (_ptoc_long) curfile->pfname , 0L );
		        /*NOTREACHED*/
        }
        if (feof(curfile->fbuf)) {
                if (curfile->funit & FTEXT) {
                        *curfile->fileptr = ' ';
                        if (curfile->funit & EOLN) {
                                curfile->funit &= ~EOLN;
                                curfile->funit |= EOFF;
                                return;
                        }
                        curfile->funit |= (SPEOLN|EOLN);
                        return;
                }
                curfile->funit |= EOFF;
                limit = &curfile->fileptr[curfile->fsize];
                for (ptr = curfile->fileptr; ptr < limit; )
                        *ptr++ = 0;
                return;
        }
        if (curfile->funit & FTEXT) {
                if (*curfile->fileptr == '\n') {
                        curfile->funit |= EOLN;
                        *curfile->fileptr = ' ';
                        return;
                }
                curfile->funit &= ~EOLN;
        }
}
