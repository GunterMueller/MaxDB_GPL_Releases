/* @(#)sql__lor.c		2.1.1  2001-01-15  14:06 SAP DB */
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

#include <errno.h>

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

extern char sql__readbf [];     /* sql__readbf is defined in sql__sync.c */

double
sql__lor(struct iorec	*curfile)
{
	double			data;
	int	    		retval;

/**
 *      if (curfile->funit & FWRITE) {
 *              sql__perrorp("%s: Attempt to read, but open for writing\n",
 *                      (long) curfile->pfname, 0L );
 *              / *NOTREACHED* /
 *      }
 **/
	sql__uncs(curfile);
	errno = 0;
	retval = fscanf(curfile->fbuf, "%lf", &data);
	if (retval == EOF) {
		sql__perrorp (sql__readbf, (_ptoc_long) curfile->pfname, 0L );
				/* %s: Tried to read past end of file */
		/*NOTREACHED*/
	}
	if (retval == 0) {
		sql__perrorp("%s: Bad data found on real read\n", (_ptoc_long) curfile->pfname, 0L );
		/*NOTREACHED*/
	}
	if (errno == ERANGE) {
		if (data == 0.0)
			sql__perrorp("%s: Underflow on real read\n", (_ptoc_long) curfile->pfname, 0L );
		else
			sql__perrorp("%s: Overflow on real read\n", (_ptoc_long) curfile->pfname, 0L );
		/*NOTREACHED*/
	}
	if (errno != 0) {
		sql__peer("sql__lor",curfile->pfname);
		/*NOTREACHED*/
	}
	curfile->funit &= ~EOLN;
	curfile->funit |= SYNC;
	return data;
}
