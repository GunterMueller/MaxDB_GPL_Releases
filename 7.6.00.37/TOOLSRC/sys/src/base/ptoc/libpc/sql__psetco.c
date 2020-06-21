/* @(#)sql__psetco.c		2.1.1  2001-01-15  14:05 SAP DB */
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

unsigned char *
sql__psetco ( unsigned char *result,
              int           r_lower, 
              int           r_upper,
              unsigned char *source,
              int           s_lower, 
              int           s_upper )
{
        long i, data;
        unsigned char *r, *s;

        i = (r_upper < 8) ? 1 : ((r_upper+16) >> 3) & (~1);
        s = &result [i];
        for (r = result; r < s; )
                *r++ = 0;

        r = result;
        s = source;

        for (i = 0; i <= s_upper; i++) {
                if (s [i >> LG2BITSBYTE] & (1 << (i & MSKBITSBYTE))) {
                        data = i + s_lower - r_lower;
                        if (data < 0 || data > r_upper) {
                                sql__perrorp("Value of %ld out of set bounds\n",
                                        data , 0L );
                                /*NOTREACHED*/
                        }
                        r [data >> LG2BITSBYTE] |= (1 << (data & MSKBITSBYTE));
                }
        }
        return result;
}
