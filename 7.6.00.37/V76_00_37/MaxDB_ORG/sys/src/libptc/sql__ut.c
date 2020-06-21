/* @(#)sql__ut.c		2.1.1  2001-01-15  15:05 SAP DB */
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

char sql__EINACTF1 [] = "Reference to an inactive file\n";
char sql__EINACTF2 [] = "%s: Reference to an inactive file\n";

struct iorec *
sql__ut(struct iorec   *curfile)
{
        if (curfile->fblk >= MAXFILES || sql__actfile[curfile->fblk] != curfile) {
		sql__perrorp ( sql__EINACTF1 , 0L , 0L );
		/*NOTREACHED*/
        }
        if (curfile->funit & FDEF) {
		sql__perrorp ( sql__EINACTF2 , (_ptoc_long) curfile->pfname , 0L );
		/*NOTREACHED*/
        }
        return curfile;
}
