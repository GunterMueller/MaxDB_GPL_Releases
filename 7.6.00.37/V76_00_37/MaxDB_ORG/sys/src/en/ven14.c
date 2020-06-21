/*!
  @file           ven14.c
  @author         JoergM
  @brief          Client RunTime: sqltermid
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

#define MOD__ "ven14.c:"
#include "gen00.h"
#include "geo007_1.h"
#if defined(SUN) && defined(SUNOS)
#else
#include        <sys/utsname.h>
#endif
 
#ifdef  REL30
#define		TERMID			tsp00_C18
#else
#define		TERMID			tsp00_C8
#endif

/* imported functions */

#include "heo46.h"
#include "hen40.h"

#include "hsp77.h"
/*
 * ===========================================================================
 */

void    sqltermid ( 
TERMID                          terminalid )
{
    int                             i ;
    char                            * pos1 ;
    char                            * pos2 ;
    char                            lt [ 64 ];
    char                            lt2 [9] ;
#if defined(SUN) && defined(SUNOS)
    char                            host [ 64 ] ;
#else
    struct  utsname                 unam ;
#endif

    DBG1 (( MF__,"sqltermid: called \n" ));

    SAPDB_memset ( lt , '\0' , sizeof(lt) );

    for ( i = 2 ; i >= 0 ; i -- )
    {
        if ( isatty ( i ) )
        {
            char *myttyname = ttyname(i);
            if(NULL != myttyname)   /* PTS 1120563: in a chroot environment under solaris, ttyname(i) fails even if isatty(i) returns true */
            {
                (void) strncpy ( lt , myttyname , sizeof(lt)-1 );
                break ;
            }
        }
    }

    pos1 = strrchr ( lt , '/' );
    if ( pos1 == NULL )
        pos1 = lt ;
    else    pos1 ++ ;
    i = strlen ( pos1 );
    if ( i > 4 ) pos1 += i - 4 ;

#if defined(SUN) && defined(SUNOS)

    SAPDB_memset ( host , 0 , sizeof ( host)  );

    if ( gethostname ( host , sizeof ( host ) ) < 0 )
    {
        MSGD (( -11987, ERR_TYPE, "TERMID  ", "sql43_gmhn: hostname error, %s\n", sqlerrs() ))
        host[0] = (char) 0 ;
    }

    pos2 = host ;

#   else

    if ( uname ( & unam ) < 0 )
    {
   	    MSGD (( -11987, ERR_TYPE, "TERMID  ", "sqltermid : uname error, %s\n", sqlerrs() ))
        unam.nodename[0] = (char) 0 ;
    }

    pos2 = unam.nodename ;

#   endif

    i = strlen ( pos2 );
    if ( i > 4 ) pos2 += i - 4 ;

    sp77sprintf ( lt2 , sizeof(lt2) , "%s%s" , pos2 , pos1 ) ;

    eo46CtoP( terminalid , lt2 , sizeof(TERMID) );

    DBG1 (( MF__,"returning, '%8.8s' \n", terminalid ));
}
