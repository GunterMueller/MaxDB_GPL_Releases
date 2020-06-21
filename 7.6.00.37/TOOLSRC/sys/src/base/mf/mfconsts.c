/*      mfconsts.c      

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
 *      module frame constants list
 *
 *      list constant definitions
 */

#define USAGE "usage: %s [-Dl] con(in) dst(out) \n"
#define ARGS    == 2


#include	"mf.h"


static  FILE            *dstfp;


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
		DEBUG ( 0, "source file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "source file '%s' opened \n", argv[argn] )
	argn++;

	dstfp = fopen ( argv[argn], "w" );
	if ( dstfp == NULL )
	{
		DEBUG ( 0, "define file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "define file '%s' opened \n", argv[argn] )
	argn++;

	rc = consts ( );
	DEBUG ( 1, "consts complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "source file closed \n", 0 )
	fclose ( dstfp );
	DEBUG ( 1, "destination file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static  int     consts ( )

{
	int		rc;


	DEBUG ( 2, "consts started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nextsymbol ( ) )  ==  0  )
	{
		DEBUG ( 2, "consts: symbol found: '%s' \n", symbol )

		fprintf ( dstfp, "%-32.32s", symbol );

		if ( c == ' ' || c == '\t' || c == '\n' )
		{
			rc = nextchar ( );
			if ( rc ) return ( rc );
		}
		if ( c != '=' )
		{
			DEBUG ( 0, "consts: syntax error \n", 0 )
			return ( ERRCON );
		}

		rc = nextchar ( );
		if ( rc ) return ( rc );
		if ( c == ' ' || c == '\t' || c == '\n' )
		{
			rc = nextchar ( );
			if ( rc ) return ( rc );
		}

		outfp = dstfp;
		rc = nextsemic ( );
		if ( rc ) return ( rc );
		fputc ( '\n', dstfp );
		outfp = NULL;
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "consts: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "consts complete \n", 0 )
		return ( 0 );
	}
}


