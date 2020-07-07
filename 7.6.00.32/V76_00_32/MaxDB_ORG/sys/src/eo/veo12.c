/*!
  @file           veo12.c
  @author         JoergM
  @special area   getopt Wrapper
  @brief          Needed wrapper to hide 'POSIXLY_INCORRECT' GNU getopt implementation
  @see            example.html ...

\if EMIT_LICENCE

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo12.h"

#if !defined(_WIN32)
#include <stdlib.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (LINUX) || defined(FREEBSD)
#define GNU_GETOPT
/* If set as first char of 'opt_string' forces POSIXLY CORRECT mode of option parsing */
#define GNU_GETOPT_POSIXLY_CORRECT '+'
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

static bool eo12_optionEncoding = false;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC int sqlgetopt(int argc, char * const argv[], const char *optstring)
{
#if defined(_WIN32)
    return sql80_GetOpt(argc, (char **)argv, (char *)optstring);
#else
# ifdef GNU_GETOPT
    int   result;
    char *usedOptString;

    usedOptString = malloc(strlen(optstring)+2);
    if ( !usedOptString )
    {
        return -1;
    }

    usedOptString[0] = GNU_GETOPT_POSIXLY_CORRECT;
    strcpy(usedOptString+1, optstring);

    result = getopt(argc, argv, usedOptString);

    free(usedOptString);

    return result;
# else
    return getopt(argc, argv, optstring);
# endif
#endif
}

/* ======================================================================== */

externC void	sqlgetpid ( tsp00_Int4 *pid )
{
/***************************************************************/
#if _WIN32 || OS2
/***************************************************************/
    GETPROCESSID ( pid ) ;
/***************************************************************/
#else
/***************************************************************/
    *pid = (tsp00_Int4)getpid() ;
/***************************************************************/
#endif
/***************************************************************/
}

/* ======================================================================== */

externC void sqlSetOptionEncodingUTF8(bool isUTF8encoded)
{
  eo12_optionEncoding = isUTF8encoded;
}

/* ======================================================================== */

externC bool sqlIsOptionEncodingUTF8()
{
  return ( eo12_optionEncoding );
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
