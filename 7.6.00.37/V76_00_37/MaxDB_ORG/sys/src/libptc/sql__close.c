/* @(#)sql__close.c		2.1.1  2001-01-15  14:02 SAP DB */
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

/*
 * Close associated file ('CLOSE')
 */

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

extern char sql__EINACTF1 [];        /* Defined in sql__ut.c */
extern char sql__EINACTF2 [];

void
sql__close(struct iorec   *curfile)
{
	struct iorec   *next;

	if (curfile->fblk >= MAXFILES || sql__actfile[curfile->fblk] != curfile) {
		sql__perrorp ( sql__EINACTF1 , 0L , 0L );
					/* Reference to an inactive file */
		return;
	}
	if (curfile->funit & FDEF) {
		sql__perrorp ( sql__EINACTF2, (_ptoc_long) curfile->pfname, 0L );
					/* Reference to an inactive file */
		return;
	}
	next = sql__fchain.fchain;
	while (next->fchain != curfile) {
		next = next->fchain;
	}
	next->fchain = sql__closep (next->fchain, TRUE);
}
