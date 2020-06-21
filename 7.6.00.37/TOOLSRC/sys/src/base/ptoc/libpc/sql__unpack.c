/* @(#)sql__unpack.c		2.1.1  2001-01-15  15:04 SAP DB */
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
 * unpack(z,a,i)
 *
 * with:        z and a as in pack
 *
 * semantics:   for j := u to v do
 *                      a[j-u+i] := z[j]
 */

void
sql__unpack( long    i,      /* subscript into a to begin packing */
             char    *a,     /* pointer to structure a */
             char    *z,     /* pointer to structure z */
             long    size_a, /* sizeof(a_type) */
             long    lb_a,   /* lower bound of structure a */
             long    ub_a,   /* (upper bound of a) - (lb_a + sizeof(z_type)) */
             long    size_z )/* sizeof(z_type) */
{
        long            subscr;
        register char   *cp;
        register char   *zp = z;
        register char   *limit;

        subscr = i - lb_a;
        if (subscr < 0 || subscr > ub_a) {
                sql__perrorp ( "i = %ld: Bad i to unpack(z,a,i)\n" , i , 0L );
                /*NOTREACHED*/
        }
        cp = &a[subscr * size_a];
        limit = cp + size_z;
        do      {
                *cp++ = *zp++;
        } while (cp < limit);
}
