/* @(#)sql__perrorp.c		2.1.1  2001-01-15  14:05 SAP DB */
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

#include        <stdio.h>
#include        <stdlib.h>
#include        <signal.h>

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

#if defined(_WIN32)
#include        <windows.h>

static char     Buf[240];
#endif

#if defined(_WIN32) && defined(USE_MSG_PRIO7)
 void          sql60_int_err_msg_prio7 ( char *pszFormatStr, ... );
#endif

/*
 * Routine sql__perrorp is called from the runtime library when a runtime
 * error occurs. Its arguments are a pointer to an error message and
 * an error specific piece of data.
 */
void _CDECL
sql__perrorp( char    *msg,  _ptoc_long d1, _ptoc_long d2)
{

#if defined(_WIN32) &&  defined(USE_MSG_PRIO7)
        sql60_int_err_msg_prio7( msg, d1, d2);
        { int *i = 0 ; *i = 11 ;} /*force an access violation trap*/
        exit ((int)-1);

#else

#if defined(_WIN32)
        wsprintf(Buf, msg, d1, d2);
        MessageBox(GetFocus(), Buf, NULL, MB_OK | MB_ICONSTOP);
        { int *i = 0 ; *i = 11 ;} /*force an access violation trap*/
        exit ((int)-1);
#else
        sql__flp();

        fputc('\n',stderr);
        fprintf(stderr, msg, d1, d2);
        { int *i = 0 ; *i = 11 ;}
        kill(getpid(), SIGTRAP);
#endif

#endif

        /*NOTREACHED*/
}

void _CDECL
sql__pmatherrorp(char    *msg, double  d1)
{

#if defined(_WIN32) &&  defined(USE_MSG_PRIO7)
        sql60_int_err_msg_prio7( msg, d1);
        { int *i = 0 ; *i = 11 ;} /*force an access violation trap*/
        exit ((int)-1);

#else

#if defined(_WIN32)
        wsprintf(Buf, msg, d1);
        MessageBox(GetFocus(), Buf, NULL, MB_OK | MB_ICONSTOP);
        { int *i = 0 ; *i = 11 ;} /*force an access violation trap*/
        exit ((int)-1);
#else
        sql__flp();

        fputc('\n',stderr);
        fprintf(stderr, msg, d1);
        { int *i = 0 ; *i = 11 ;}
        kill(getpid(), SIGTRAP);
#endif

#endif

        /*NOTREACHED*/
}
