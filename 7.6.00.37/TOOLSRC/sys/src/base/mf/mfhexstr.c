/*	mfhexstr.c		

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
 *	module frame hexadecimal string literal conversion
 *
 *	convert '\xx' in string literals to their binary equivalents.
 */

#define USAGE "usage: %s [-Dl] infile outfile \n"
#define	ARGS	== 2


#include	"mf.h"


/*============================================================================*/


int	main ( argc, argv )

int		argc;
char		**argv;

{
	int		argn, rc;


	DEBUGINI 

	for ( argn=1; argn < argc; argn++ )
	{
		if ( argv[argn][0] != '-' ) break;
		rc = mfoption ( argv[argn] );
		if ( rc ) return ( rc );
	}

	if ( ! (argc-argn ARGS) )
	{
		DEBUG ( 0, USAGE, argv[0] )
		return ( 1 );
	}

	infp = fopen ( argv[argn], "r" );
	if ( infp == NULL )
	{
		DEBUG ( 0, "input file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "input file '%s' opened \n", argv[argn] )
	argn++;

	outfp = fopen ( argv[argn], "w" );
	if ( outfp == NULL )
	{
		DEBUG ( 0, "output file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "output file '%s' opened \n", argv[argn] )
	argn++;

	rc = hex_string ( );
	DEBUG ( 1, "hex_string complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "input file closed \n", 0 )
	fclose ( outfp );
	DEBUG ( 1, "output file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	hex_string ( )

{
    int			strend ;
    int			c1 ;
    int			c2 ;


    DEBUG ( 2, "hex_string started \n", 0 )

    c = fgetc ( infp );

    while ( c != EOF )
    {
	do
	{
	    fputc ( c, outfp );
	    c = fgetc ( infp );
	}
	while ( c != '(' && c != '\'' && c != EOF );

	if ( c == EOF )
	{
	    break ;
	}
	fputc ( c, outfp );

	if ( c == '(' )
	{
	    c = fgetc ( infp );
	    if ( c == EOF )
	    {
		break ;
	    }
	    if ( c == '*' )
	    {
		skipcomment ();
		continue ;
	    }
	    if ( c != '\'' )
	    {
		continue ;
	    }
	    fputc ( c, outfp );
	}

	strend = 0;
	do
	{
	    c = fgetc ( infp );
	    if ( c == EOF )
	    {
		DEBUG ( 0, "mfhexstr: end of file in string \n", 0 )
		return ( ERREOF );
	    }

	    switch ( c )
	    {

	    case '\'':

		    strend = 1;
		    break;

	    case '\\':

		    c1 = fgetc ( infp );
		    c2 = fgetc ( infp );
		    if ( c1 == EOF || c2 == EOF )
		    {
			DEBUG ( 0, "mfhexstr: end of file in string \n", 0 )
			return ( ERREOF );
		    }
		    if ( isxdigit(c1) && isxdigit(c2) )
		    {
			if ( isalpha(c1) )
			{
			    if ( islower(c1) )
				c1 -= 'a' - 10 ;
			    else
				c1 -= 'A' - 10 ;
			}
			else
			{
			    c1 -= '0' ;
			}

			if ( isalpha(c2) )
			{
			    if ( islower(c2) )
				c2 -= 'a' - 10 ;
			    else
				c2 -= 'A' - 10 ;
			}
			else
			{
			    c2 -= '0' ;
			}
			c = c1 << 4 | c2 ;
#if ! OLD
			fprintf ( outfp , "'#%d'" , c );
#else
			if ( c == '\'' )
			{
			    fputc ( c, outfp );
			}
			fputc ( c, outfp );
#endif
		    }
		    else
		    {
			fputc ( c, outfp );
			fputc ( c1, outfp );
			fputc ( c2, outfp );
		    }
		    break;

	    default:

		    fputc ( c, outfp );

	    }
	    /*end switch */
	}
	while ( ! strend );
    }

    DEBUG ( 2, "hex_string complete \n", 0 )
    return ( 0 );
}


