/* @(#)sql__pred.c		2.1.1  2001-01-15  14:05 SAP DB */
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

static char sql__E_pred[] =
"File: '%s', Line: %d, cannot take pred of first element of a range!\n";

long
sql__pred( long    value,
           long    lower,
           long    upper,
           char    *fname,          /* pointer to src file name */
           int     lno )            /* line number in src file */
{
        sql__setrang(value, lower, upper);

        if (value == lower) {
                sql__perrorp( sql__E_pred, (_ptoc_long)fname, lno);
                /*NOTREACHED*/
        }

        return  --value;
}
