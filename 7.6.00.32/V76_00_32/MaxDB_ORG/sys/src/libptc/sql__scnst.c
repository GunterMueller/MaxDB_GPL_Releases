/* @(#)sql__scnst.c		2.1.1  2001-01-15  14:31 SAP DB */
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

static long    sql__s_mask [] = {
#               ifdef DEC11
                    0xffffffff , 0xfffffffe , 0xfffffffc , 0xfffffff8 ,
                    0xfffffff0 , 0xffffffe0 , 0xffffffc0 , 0xffffff80 ,
                    0xffffff00 , 0xfffffe00 , 0xfffffc00 , 0xfffff800 ,
                    0xfffff000 , 0xffffe000 , 0xffffc000 , 0xffff8000 ,
                    0xffff0000 , 0xfffe0000 , 0xfffc0000 , 0xfff80000 ,
                    0xfff00000 , 0xffe00000 , 0xffc00000 , 0xff800000 ,
                    0xff000000 , 0xfe000000 , 0xfc000000 , 0xf8000000 ,
                    0xf0000000 , 0xe0000000 , 0xc0000000 , 0x80000000 ,
                    0x00000000
#               else
#             if __alpha
0xffffffffffffffff, 0xfeffffffffffffff, 0xfcffffffffffffff, 0xf8ffffffffffffff,
0xf0ffffffffffffff, 0xe0ffffffffffffff, 0xc0ffffffffffffff, 0x80ffffffffffffff,
0x00ffffffffffffff, 0x00feffffffffffff, 0x00fcffffffffffff, 0x00f8ffffffffffff,
0x00f0ffffffffffff, 0x00e0ffffffffffff, 0x00c0ffffffffffff, 0x0080ffffffffffff,
0x0000ffffffffffff, 0x0000feffffffffff, 0x0000fcffffffffff, 0x0000f8ffffffffff,
0x0000f0ffffffffff, 0x0000e0ffffffffff, 0x0000c0ffffffffff, 0x000080ffffffffff,
0x000000ffffffffff, 0x000000feffffffff, 0x000000fcffffffff, 0x000000f8ffffffff,
0x000000f0ffffffff, 0x000000e0ffffffff, 0x000000c0ffffffff, 0x00000080ffffffff,
0x00000000ffffffff, 0x00000000feffffff, 0x00000000fcffffff, 0x00000000f8ffffff,
0xf000000000ffffff, 0x00000000e0ffffff, 0x00000000c0ffffff, 0x0000000080ffffff,
0x0000000000ffffff, 0x0000000000feffff, 0x0000000000fcffff, 0x0000000000f8ffff,
0x0000000000f0ffff, 0x0000000000e0ffff, 0x0000000000c0ffff, 0x000000000080ffff,
0x000000000000ffff, 0x000000000000feff, 0x000000000000fcff, 0x000000000000f8ff,
0x000000000000f0ff, 0x000000000000e0ff, 0x000000000000c0ff, 0x00000000000080ff,
0x00000000000000ff, 0x00000000000000fe, 0x00000000000000fc, 0x00000000000000f8,
0x00000000000000f0, 0x00000000000000e0, 0x00000000000000c0, 0x0000000000000080,
0x0000000000000000
#               else
                    0xffffffff , 0xfeffffff , 0xfcffffff , 0xf8ffffff ,
                    0xf0ffffff , 0xe0ffffff , 0xc0ffffff , 0x80ffffff ,
                    0x00ffffff , 0x00feffff , 0x00fcffff , 0x00f8ffff ,
                    0x00f0ffff , 0x00e0ffff , 0x00c0ffff , 0x0080ffff ,
                    0x0000ffff , 0x0000feff , 0x0000fcff , 0x0000f8ff ,
                    0x0000f0ff , 0x0000e0ff , 0x0000c0ff , 0x000080ff ,
                    0x000000ff , 0x000000fe , 0x000000fc , 0x000000f8 ,
                    0x000000f0 , 0x000000e0 , 0x000000c0 , 0x00000080 ,
                    0x00000000
#               endif /* __alpha */
#               endif /* DEC11 */
            };
/*
 * Constant set constructor
 */

long * _CDECL
sql__scnst (long   *result,     /* pointer to final set */
            long    lowerbnd,   /* lower bound of set */
            long    upperbnd,   /* upper - lower of set */
            long    paircnt,    /* number of pairs to construct */
            long    singcnt,    /* number of singles to construct */
            ...)                /* paircnt plus singcnt sets of data */
{
        va_list         ap;
        long            data;
        register long   *lp;
        register char   *cp;
        register long   temp;
        long            *limit;
        long            lower;
        long            lowerdiv;
        long            lowermod;
        long            upper;
        long            upperdiv;
        long            uppermod;
        long            cnt;

        va_start (ap, singcnt);

        limit = &result[(upperbnd + 1 + BITSPERLONG - 1) >> LG2BITSLONG];
        for (lp = result; lp < limit; )
                *lp++ = 0;
        for (cnt = 0; cnt < paircnt; cnt++) {
                data = va_arg (ap, long);
                upper = data - lowerbnd;
                if (upper < 0 || upper > upperbnd) {
                        sql__perrorp("Range upper bound of %ld out of set bounds\n",
                                data , 0L );
                        /*NOTREACHED*/
                }
                data = va_arg (ap, long);
                lower = data - lowerbnd;
                if (lower < 0 || lower > upperbnd) {
                        sql__perrorp("Range lower bound of %ld out of set bounds\n",
                                data , 0L );
                        /*NOTREACHED*/
                }
                if (lower > upper) {
                        continue;
                }
                lowerdiv = lower >> LG2BITSLONG;
                lowermod = lower & MSKBITSLONG;
                upperdiv = upper >> LG2BITSLONG;
                uppermod = upper & MSKBITSLONG;
                temp = sql__s_mask [lowermod];
                if ( lowerdiv == upperdiv ) {
                        temp &= ~sql__s_mask[ uppermod + 1 ];
                }
                result[ lowerdiv ] |= temp;
                limit = &result[ upperdiv-1 ];
                for ( lp = &result[ lowerdiv+1 ] ; lp <= limit ; lp++ ) {
                        *lp |= ~0;
                }
                if ( lowerdiv != upperdiv ) {
                        result[ upperdiv ] |= ~sql__s_mask[ uppermod + 1 ];
                }
        }
        for (cnt = 0, cp = (char *)result; cnt < singcnt; cnt++) {
                data = va_arg (ap, long);
                lower = data - lowerbnd;
                if (lower < 0 || lower > upperbnd) {
                        sql__perrorp ( "Value of %ld out of set bounds\n" ,
                                data , 0L );
                        /*NOTREACHED*/
                }
                cp[ lower >> LG2BITSBYTE ] |= (1 << (lower & MSKBITSBYTE));
        }
        va_end (ap);
        return(result);
}
