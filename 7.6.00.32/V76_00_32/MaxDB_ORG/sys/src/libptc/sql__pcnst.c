/* @(#)sql__pcnst.c		2.1.1  2001-01-15  14:06 SAP DB */
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

#include <stdarg.h>

/*
 * Constant set constructor
 */

unsigned char *
sql__pcnst (unsigned char *result,
            long lowerbnd,
            long upperbnd,
            long paircnt,
            long singcnt,
            ...)
{
        va_list         ap;
        long            data;
        register unsigned char  *cp;
        unsigned char           *limit;
        long            lower;
        long            upper;
        long            cnt;
        long            i;

        va_start (ap, singcnt);

        i = (upperbnd < 8) ? 1 : ((upperbnd+16) >> 3) & (~1);
        limit = &result [i];
        for (cp = result; cp < limit; )
                *cp++ = 0;

        for (cnt = 0; cnt < paircnt; cnt++) {
                data = va_arg (ap, long);
                upper = data - lowerbnd;
                if (upper < 0 || upper > upperbnd) {
                        sql__perrorp("Range upper bound of %ld out of (mapped) set bounds 0 .. %d\n",
                                data, (long) upperbnd);
                        /*NOTREACHED*/
                }
                data = va_arg (ap, long);
                lower = data - lowerbnd;
                if (lower < 0 || lower > upperbnd) {
                        sql__perrorp("Range lower bound %ld out of (mapped) set bounds 0 .. %d\n",
                                data, (long) upperbnd);
                        /*NOTREACHED*/
                }
                if (lower > upper) {
                        continue;
                }

                for (i = lower; i <= upper; i++) {
                        result [i >> LG2BITSBYTE] |= (1 << (i & MSKBITSBYTE));
                }
        }

        for (cnt = 0; cnt < singcnt; cnt++) {
                data = va_arg (ap, long);
                lower = data - lowerbnd;
                if (lower < 0 || lower > upperbnd) {
                        sql__perrorp("Value %ld out of (mapped) set bounds 0 .. %d\n",
                                data, (long) upperbnd);
                        /*NOTREACHED*/
                }
                result [lower >> LG2BITSBYTE] |= (1 << (lower & MSKBITSBYTE));
        }

        va_end (ap);
        return(result);
}
