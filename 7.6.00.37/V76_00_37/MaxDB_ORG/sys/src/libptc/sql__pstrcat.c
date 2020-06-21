/* @(#)sql__pstrcat.c		2.1.1  2001-01-15  14:04 SAP DB */
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
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

unsigned char *
sql__pstrcat ( unsigned char   *result,
               int             max,
               unsigned char   *left,
               unsigned char   *right )
{
        int     lenl;
        int     lenr;
        int     size;
        unsigned char   *temp;

        lenl = *left;
        lenr = *right;
        size = lenl + lenr;
        if (size > max) {
                sql__perrorp ("string overflow\n" , 0L , 0L );
                /*NOTREACHED*/
        }
        temp = result;
        *temp++ = size;
        while (lenl--) {
                *temp++ = *++left;
        }
        while (lenr--) {
                *temp++ = *++right;
        }
        return result;
}
