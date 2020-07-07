/* @(#)sql__lexe.c		2.1.1  2001-01-15  14:07 SAP DB */
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
sql__lexe( struct iorec   *curfile,
           long           limit )
{
    if (limit <= 0 || 0x7fffffff < limit)
      limit = 0x7fffffff;

    curfile->llimit = limit;
    if (curfile->lcount >= curfile->llimit) {
      sql__perrorp("%s: Line limit exceeded\n",
                   (_ptoc_long) curfile->pfname, 0L );
     /*NOTREACHED*/
    }
}
