/* @(#)sql__catchae.c		2.1.1  2001-01-15  14:01 SAP DB */
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

#include        <signal.h>

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

/*
 * catch runtime arithmetic errors
 */
#if defined ( _IBMR2 ) || defined ( SVR4 ) || defined(_WIN32)  || defined(LINUX) || defined ( SOLARIS )
void _CDECL  sql__catchae(int signum)
{
        signal ( SIGFPE , SIG_IGN );
#if defined(_WIN32)
        sql__perrorp ( "libsqlptc: Floating Point Exception Signal\n", 0L, 0L);
#else
        sql__perrorp ( "libsqlptc: Floating Point Exception Signal, pid %d \n", (long) getpid(), 0L );
#endif
        /*NOTREACHED*/
}
#else
void sql__catchae(int signum, int code)
{
        signal ( SIGFPE , SIG_IGN );

        switch (code) {
#ifdef PYRAMID
        case FPE_INTOVF_EXC:
                sql__perrorp("Integer overflow\n" , 0L , 0L );
        case FPE_INTDIV_EXC:
                sql__perrorp("Integer division by zero\n" , 0L , 0L );
        case FPE_FLTOVF_EXC:
                sql__perrorp("Real overflow\n" , 0L , 0L );
        case FPE_FLTDIV_EXC:
                sql__perrorp("Real division by zero\n" , 0L , 0L );
        case FPE_FLTUND_EXC:
                sql__perrorp("Real underflow\n" , 0L , 0L );
#endif

#ifdef M68020
        case KFPDZ:
                sql__perrorp("FPU - Real division by zero\n" , 0L , 0L );
        case KFPUNFL:
                sql__perrorp("FPU - Real underflow\n" , 0L , 0L );
        case KFPOPER:
                sql__perrorp("FPU - Operand error\n" , 0L , 0L );
        case KFPOVFL:
                sql__perrorp("FPU - Real overflow\n" , 0L , 0L );
#endif
        default:
                sql__perrorp ( "libsqlptc: Floating Point Exception Signal, pid %d \n",
                        (long) getpid() , 0L );
        }
        /*NOTREACHED*/
        return;
}
#endif
/* _IBMR2 || SVR4 || _WIN32 */
