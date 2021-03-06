/* @(#)sql__cos.c		2.1.1  2001-01-15  14:09 SAP DB */
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

/*
 * Cosine ('COS')
 */

#include <math.h>
#include <errno.h>

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif


double
sql__cos(double value)
{
        double result;

        errno = 0;
        result = cos(value);
        if (errno != 0) {
                sql__pmatherrorp ( "Cannot compute cos(%e)\n" , value );
                /*NOTREACHED*/
                return ( (double)-1 );
        }
        return result;
}
