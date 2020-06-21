/*	mfcat.c		

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
 *	module frame concatenation
 *
 *	concatenate declaration sections
 */

#define USAGE "usage: %s [-Dl] outputfile sectionheader inputfiles \n"
#define	ARGS	>= 3


#include	"mf.h"

static	int	append ( );

static	int		empty;
static	char		header[KWDSIZ];

static int append();

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

	outfp = fopen ( argv[argn], "w" );
	if ( outfp == NULL )
	{
		DEBUG ( 0, "output file open error %d, ", errno )
		DEBUG ( 0, "opening %s \n", argv[argn] )
		return ( errno );
	}
	DEBUG ( 1, "output file '%s' opened \n", argv[argn] )
	argn++;

	strncpy ( header, argv[argn], sizeof(header) );
	DEBUG ( 1, "section header is '%s' \n", header )
	argn++;

	empty = 1;

	for ( ; argn < argc; argn++ )
	{
		infp = fopen ( argv[argn], "r" );
		if ( infp == NULL )
		{
			DEBUG ( 0, "input file open error %d, ", errno )
			DEBUG ( 0, "opening %s \n", argv[argn] )
			return ( errno );
		}
		DEBUG ( 1, "input file '%s' opened \n", argv[argn] )

		rc = append ( );
		DEBUG ( 1, "append finished, return code = %d \n", rc )

		fclose ( infp );
		DEBUG ( 1, "input file '%s' closed \n", argv[argn] )

		if ( rc ) return ( rc );
	}

	fclose ( outfp );
	DEBUG ( 1, "output file closed \n", 0 )

	return ( 0 );
}


/*============================================================================*/


static	int	append ( )

{
	int		i;


	DEBUG ( 2, "append started \n", 0 )

	while (  empty  &&  fgets(line,sizeof(line),infp)  )
	{
		for ( i=0; line[i] != 0; i++ )
		{
			if (	line[i] != ' '   &&
				line[i] != '\t'  &&
				line[i] != '\n'     )
			{
				empty = 0;
				fputs ( header, outfp );
				fputs ( "\n\n", outfp );
				fputs ( line, outfp );
				break;
			}
		}
	}

	while (  fgets(line,sizeof(line),infp)  )  fputs ( line, outfp );

	DEBUG ( 2, "append complete \n", 0 )
	return ( 0 );
}


