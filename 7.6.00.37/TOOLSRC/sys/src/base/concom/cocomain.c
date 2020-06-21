/*	main.c		

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

#include	"stdio.h"
#include	"istd.h"

/* imported functions */

extern	int			concom ();
extern	void			out ();

/* local functions */

static	void			con_com ();

/* exported data */

global	int			dbgdbglvl ;

/* imported data */

extern	int			errfd ;
extern	int			outfd ;
extern	int			dbgdbgfd ;
extern	int			yydebug ;
extern	int			optind ;
extern	char			* optarg ;

/*==========================================================================*/

global	int	main ( argc , argv )

int			argc ;
char			** argv ;

{
	int			c ;
	char			line [ 256 ];


	errfd = 1 ;
	outfd = 1 ;
	dbgdbgfd = 1 ;

	while ( (c = getopt( argc , argv , "D:Y" )) != -1 )
	{
		switch ( c )
		{
		case 'D' :
			dbgdbglvl = * optarg - '0' ;
			DBG0 (( "debuglevel %d \n", dbgdbglvl ))
			break ;

		case 'Y' :
			yydebug = 1 ;
			break ;

		}
	}

	while ( gets ( line ) )
	{
		con_com ( line );
	}
	con_com ( (char*) 0 );

	return ( 0 );
}

/*==========================================================================*/

static	void	con_com ( line )

char			* line ;

{
	int			rc ;


	if ( (dbgdbglvl > 0 || yydebug) && line )
	{
		out ( "====> '%s' \n", line );
	}

	rc = concom ( line , "" );
	if ( ! line ) return ;

#ifdef	nonumber
	if ( line [ 0 ] == '&' && ! rc )
		out ( "    '%s' \n", line );
	else
#endif
		out ( "(%d) '%s' \n", rc , line );
}

