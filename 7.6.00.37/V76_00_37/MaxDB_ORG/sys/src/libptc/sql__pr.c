/* @(#)sql__pr.c		2.1.1  2001-01-15  14:05 SAP DB */
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

/**
 **	Reads a string and copys it into a packed array of char,
 **	whose address is passed as a parameter.
 **	If the string is shorter than the array, then the array is
 **	filled with blanks.
 **/

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

#define	LINELENGTH	1024

extern char sql__readbf [];     /* sql__readbf is defined in sql__sync.c */

void
sql__pr( struct iorec		*curfile,
	       char			      *string,
	       long		        length )
{
	register int	c;
	register char	*cp;
	register FILE	*iop;

	sql__uncs(curfile);
	iop = curfile->fbuf;
	cp = string;
	while (--length >= 0 && (c = getc (iop)) != EOF) {
		if (c == '\n') {
			*curfile->fileptr = ' ';
			ungetc (c, iop);
			break;
		}
		*cp++ = (char) c;
	}
			
	if (cp == string) {
		sql__perrorp ( sql__readbf , (_ptoc_long) curfile->pfname , 0L );
				/* %s: Tried to read past end of file */
		/*NOTREACHED*/
	}

	if (c == EOF) {
		curfile->funit |= (SPEOLN|EOLN);
	}
	else {
		curfile->funit |= (SYNC|EOLN);
	}

	for (; length >= 0; length--) {
		*cp++ = ' ';
	}

	return;
}
