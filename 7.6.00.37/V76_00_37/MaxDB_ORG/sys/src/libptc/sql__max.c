/* @(#)sql__max.c		2.1.1  2001-01-15  14:06 SAP DB */
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

long
sql__max( long	width,	/* requested width */
	        long	reduce,	/* amount of extra space required */
	        long	min )		/* minimum amount of space needed */
{
	if (width <= 0) {
		sql__perrorp("Non-positive format width: %ld\n", width, 0L );
		/*NOTREACHED*/
	}
	if ((width -= reduce) >= min)
		return width;
	return min;
}
