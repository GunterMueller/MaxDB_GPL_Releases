/*	debug.c		

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

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
 *	debug tool:  functions
 */


#include	"debug.h"
#include <string.h>


/*============================================================================*/


void	debug ( level, string, value )

int		level;
char		*string;
int		value;

DEBUG ( level, string, value )


/*============================================================================*/


void	debugini ( )

DEBUGINI 


/*============================================================================*/


void	debuglvl ( level )

int		level;

DEBUGLVL ( level )


/*============================================================================*/


void	debugfp ( fp )

FILE		*fp;

DEBUGFP ( fp )


/*============================================================================*/


void	debuglevel ( levelname )

char		*levelname;

{
	int		lvl;


	lvl = levelname[0] - 48;

	if ( strlen(levelname) == 1 && lvl >= 0 && lvl <= 9 )
	{
		DEBUGLVL ( lvl )
	}
	else
	{
		DEBUG ( 0, "DEBUGLEVEL illegal: ", 0 )
		DEBUG ( 0, "%s \n", levelname )
	}
}


/*============================================================================*/


void	debugfile ( filename )

char		*filename;

{
	FILE		*fp;
	int		error;


	if (  ( fp=fopen(filename,"a") ) != NULL  )
	{
		DEBUGFP ( fp )
	}
	else
	{
		error = errno;
		DEBUG ( 0, "DEBUGFILE open error %d, ", error );
		DEBUG ( 0, "opening %s \n", filename );
	}
}


/*============================================================================*/


void	debugenv ( )

{
	char		*dbglvl, *dbgfil;
	extern	char	*getenv();


	if ( dbglvl=getenv("DEBUGLEVEL") )
	{
		DEBUGLEVEL ( dbglvl )
	}

	if ( dbgfil=getenv("DEBUGFILE") )
	{
		DEBUGFILE ( dbgfil )
	}
}


