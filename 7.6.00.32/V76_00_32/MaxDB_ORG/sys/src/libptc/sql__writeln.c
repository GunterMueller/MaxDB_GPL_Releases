/* @(#)sql__writeln.c		2.1.1  2001-01-15  15:05 SAP DB */
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

void
sql__writeln(struct iorec   *curfile)
{
/*
 *      if (curfile->funit & FREAD) {
 *              sql__perrorp ( "%s: Attempt to write, but open for reading\n" ,
 *                      (long) curfile->pfname , 0L );
 *              / *NOTREACHED* /
 *      }
 */
        if (++curfile->lcount >= curfile->llimit) {
                sql__perrorp ( "%s: Line limit exceeded\n" ,
                        (_ptoc_long) curfile->pfname , 0L );
                /*NOTREACHED*/
        }
        fputc('\n', curfile->fbuf);
/*IST*/ fflush ( curfile->fbuf );
/*
        if (ferror(curfile->fbuf)) {
                sql__peer("Could not write to ", curfile->pfname);
                return;
        }
*/
}
