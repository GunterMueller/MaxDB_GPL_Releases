/* @(#)sql__ckl.c		2.1.1  2001-01-15  14:01 SAP DB */
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

/*
 * CPU time usage in system mode ('SCLCK')
 */

#if defined ( _WIN32 )
#include <time.h>

long
sql__ckl()
{
        return ( (((long)clock()) * 1000) / CLOCKS_PER_SEC ) ;
}

#else

#include <sys/types.h>
#include <sys/param.h>
#include <sys/times.h>
#ifndef HZ
/* 1992-Sep-15 J.Bruehe  'HZ' was simply defined here - silly idea */
/* 1995-Mar-09 G.Rom     look for a reasonable 'HZ' value          */
#   include <limits.h>
#   ifndef HZ
#       ifdef CLK_TCK
#           define HZ              CLK_TCK
#       else
#           define HZ              60
#       endif
#   endif
#endif

long
sql__ckl()
{
        struct tms tim ;

        times ( & tim );
        return ( ( (long) tim.tms_stime * 1000) / HZ ) ;
}

#endif
