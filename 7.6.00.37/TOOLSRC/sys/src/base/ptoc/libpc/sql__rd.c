/* @(#)sql__rd.c		2.1.1  2001-01-15  14:27 SAP DB */
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

#include <stdlib.h>

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

double
sql__rd()
{
        double d;

        /*
         * calculate (1103515245 * seed_value) mod 2^31-1
         */
#ifdef PYRAMID
        asm ("  movw    $0,lr0");
        asm ("  movw    $1103515245,lr1");
        asm ("  emul    _sql__seed_v,lr0");
        asm ("  ediv    $0x7fffffff,lr0");
        asm ("  movw    lr1,_sql__seed_v");
        asm ("  cvtwd   lr1,lr0");
        d = d / 2147483647.0;
#else
        d = ( (double) (rand()) ) / (double) 32767;
#endif /* PYRAMID */
        return d;
}

/* int                  1992-Oct-13  J.Bruehe
 * seed (i)             remainder of renaming the builtin routines,
 *         int i;       this name is never generated but replaced by
 * {                    'sql__srand' (see there).
 *         int temp;
 *
 *         temp = sql__seed_v;
 *         sql__seed_v = i;
 *         return temp;
 * }
 */
