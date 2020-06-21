/* @(#)sql__setco.c		2.1.1  2001-01-15  14:32 SAP DB */
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
 *  Set conversion
 */

char *
sql__setco ( char  *result,
             int   r_lower, 
             int   r_upper,
             char  *source,
             int   s_lower,
             int   s_upper )
{
        long i, data, diff;
        char *r, *s;
        long *lp,*limit;

        limit = &((long *)result)[(r_upper + 1 + BITSPERLONG - 1) >> LG2BITSLONG];
        for (lp = (long*)result; lp < limit; )
                *lp++ = 0;

        r = result;
        s = source;
        diff = s_lower - r_lower;

        for (i = 0; i <= s_upper; i++) {
                if (s [i >> LG2BITSBYTE] & (1 << (i & MSKBITSBYTE))) {
                        data = i + diff;
                        if (data < 0 || data > r_upper) {
				sql__perrorp("Value %ld out of set bounds 0 .. %d\n",
					data, r_upper);
				/*NOTREACHED*/
                                return (char*)-1;
                        }
                        r [data >> LG2BITSBYTE] |= (1 << (data & MSKBITSBYTE));
                }
        }
        return result;
}
