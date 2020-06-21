/* @(#)sql__atan.c		2.1.1  2001-01-15  14:00 SAP DB */
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
 * Arcus tangens ('ATAN')
 */

#include <math.h>

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

#include <errno.h>

double
sql__atan(double value)
{
        double result;

        errno = 0;
        result = atan(value);
        if (errno != 0) {
                sql__pmatherrorp("Argument %e is out of the domain of atan\n", value);
                /*NOTREACHED*/
                return ( (double)-1 );
        }
        return result;
}
