/* @(#)sql__pstrcpy.c		2.1.1  2001-01-15  14:19 SAP DB */
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

unsigned char *
sql__pstrcpy ( unsigned char   *result,
               int             max,
               unsigned char   *source,
               int             first,
               int             size )
{
        unsigned char   *temp;

        if ((first < 1) || (first > (int) *source) || (size < 1)) {
                *result = 0;
                return result;
        }
        if (first + size - 1 > (int) *source) {
                size = *source - first + 1;
        }
        if (size > max) {
                sql__perrorp ("string overflow\n" , 0L , 0L );
                /*NOTREACHED*/
        }
        temp = result;
        *temp++ = size;
        source += first;
        while (size--) {
                *temp++ = *source++;
        }
        return result;
}
