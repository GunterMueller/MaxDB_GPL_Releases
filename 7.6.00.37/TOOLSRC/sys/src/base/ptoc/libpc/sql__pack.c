/* @(#)sql__pack.c		2.1.1  2001-01-15  14:06 SAP DB */
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
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

/*
 * pack(a,i,z)
 *
 * with:	a: array[m..n] of t
 *	z: packed array[u..v] of t
 *
 * semantics:	for j := u to v do
 *			z[j] := a[j-u+i];
 *
 * need to check:
 *	1. i >= m
 *	2. i+(v-u) <= n		(i.e. i-m <= (n-m)-(v-u))
 *
 * on stack:	lv(z), lv(a), rv(i) (len 4)
 *
 * move w(t)*(v-u+1) bytes from lv(a)+w(t)*(i-m) to lv(z)
 */

void
sql__pack( long i,        /* subscript into a to begin packing */
           char	*a,     	/* pointer to structure a */
           char	*z,	      /* pointer to structure z */
           long	size_a,	  /* sizeof(a_type) */
           long	lb_a,	    /* lower bound of structure a */
           long	ub_a,	    /* (upper bound of a) - (lb_a + sizeof(z_type)) */
           long	size_z )  /* sizeof(z_type) */
{
	long		subscr;
	register char	*cp;
	register char	*zp = z;
	register char	*limit;

	subscr = i - lb_a;
	if (subscr < 0 || subscr > ub_a) {
		sql__perrorp("i = %ld: Bad i to pack(a,i,z)\n", i , 0L );
		/*NOTREACHED*/
	}
	cp = &a[subscr * size_a];
	limit = cp + size_z;
	do	{
		*zp++ = *cp++;
	} while (cp < limit);
}
