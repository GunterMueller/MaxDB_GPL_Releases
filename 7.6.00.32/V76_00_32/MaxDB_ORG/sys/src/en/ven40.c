/*!
  @file           ven40.c
  @author         JoergM
  @brief          Client RunTime: sqlerrs/sqlabort
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/

#define MOD__ "ven40.c:"

#include    "gen00.h"
#include    "hen40.h"
/*
 * ==========================================================================
 */

externC char *sqlerrs ()
{
	/* imported data */
	if ( errno == 0 )
	{
		return ( "NO ERROR(0)" );
	}
#if defined(SUN) || defined(SOLARIS) || defined(HPUX) || defined(LINUX) || defined(FREEBSD)
	{
        /* On LINUX, FREEBSD and AIX strerror is not mt save... We have to eliminate sqlerrs() one day to get this right... */
		const char *ErrorMessage = strerror(errno);
		if ( ErrorMessage == NULL )
		{
			ErrorMessage = "ERRNO NOT IN sys_errlist[]";
		}
		return (char *)ErrorMessage;
	}
#else
    /* this at least is a thread save implementation */
    if ( (errno < 1) || (errno > sys_nerr) )
	{
		return ( "ERRNO NOT IN sys_errlist[]" );
	}
	return ( (char *)sys_errlist [ errno ] );
#endif
}

#if !defined (ENUMINST_ONLY) 

#  if defined(KERNEL_LZU)

extern void vabort(teo00_Bool writeCrashDump);

void sqlabort(void)
{
    vabort(1);
}

#  else

/* PTS 1105107 */
/* 
 * ==========================================================================
 */

void    sqlabort ( void )
{
    pid_t                           pid ;
    pid_t                           nul ;
    pid_t                           *ptr ;
    char                            *line ;


    nul = 0 ;
    ptr = 0 ;
    pid = getpid ();

    line = "                                          ABEND: sqlabort called\n";
    (void) write ( 1 , line , (unsigned) strlen(line) );

    /* try to write to address NULL */
    *ptr = pid ;

    /* if this missed, create a floating point exception trap */
    nul = pid / nul ;

    /* if this missed, try to kill myself by a SIGFPE */
    (void) kill ( pid , SIGFPE );
    (void) sleep ( 1 );

    /* if this missed, try to kill myself by a SIGBUS */
    (void) kill ( pid , SIGBUS );

    /* if this missed, try to kill myself by a SIGTRAP */
    (void) kill ( pid , SIGTRAP );

    /* if this missed, simply hold execution (for sure loop forever) */
    while ( pause () != 777777777 );
}
#  endif /* KERNEL_LZU */

#endif /* !ENUMINST_ONLY */

