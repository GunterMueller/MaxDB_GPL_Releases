/*      mfclr.c         

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
 *      module frame clear procedure insertion
 *
 *      add call to 'clearproc'
 */

#define USAGE "usage: %s [-Dl] [-t] infile outfile clearproc \n"
#define ARGS    == 3


#include	"mf.h"


static  char            clearproc[SYMSIZ];


/*============================================================================*/


int	main ( argc, argv )

int		argc;
char		**argv;

{
	int		argn, rc;


	DEBUGINI 
	tdskipflag = 1;

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

	strncpy ( clearproc, argv[argn], sizeof(clearproc) );
	DEBUG ( 1, "clear procedure is '%s' \n", clearproc )
	argn++;

	rc = clr_section ( );
	DEBUG ( 1, "clr_section complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "input file closed \n", 0 )
	fclose ( outfp );
	DEBUG ( 1, "output file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static  int     clr_section ( )

{
	int             rc;


	DEBUG ( 2, "clr_section called \n", 0 )

	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "clr_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPFUN:
		case TYPPRC:
				fputs ( keyword, outfp );
				rc = nextbody ( );
				if ( rc ) return ( rc );
				fputs ( keyword, outfp );
				fprintf ( outfp, "\n%s;", clearproc);
				break;
		default:
				fputs ( keyword, outfp );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "clr_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "clr_section complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static  int     nextbody ( )

{
	int             rc, i;
	char            prcnam[SYMSIZ];


	DEBUG ( 2, "nextbody started \n", 0 )

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "nextbody: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		if ( keywrdtyp == TYPBEG ) break;

		fputs ( keyword, outfp );
	}

	if ( rc != 0  &&  rc != ERREOF )
	{
		DEBUG ( 2, "nextbody: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "nextbody complete \n", 0 )
		return ( 0 );
	}
}


