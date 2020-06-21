/*	mftop.c		

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
 *	module frame pascal top and bottom generation
 *
 *	if program then top = program heading and bot = '.'
 *	           else top = module heading  and bot = empty
 */

#define USAGE "usage: %s [-Dl] prg(in) 'moduleheading' top(out) bot(out) \n"
#define	ARGS	== 4


#include	"mf.h"

static	int	prg_section ( );

static	FILE		*topfp;
static	FILE		*botfp;
static	int		program;
static	char		modhead[SYMSIZ];


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
		DEBUG ( 0, "prg file open error %d, ", errno )
		DEBUG ( 0, "opening %s \n", argv[argn] )
		return ( errno );
	}
	DEBUG ( 1, "prg file '%s' opened \n", argv[argn] )
	argn++;

	strncpy ( modhead, argv[argn], sizeof(modhead) );
	DEBUG ( 1, "moduleheading is '%s' \n", modhead )
	argn++;

	topfp = fopen ( argv[argn], "w" );
	if ( topfp == NULL )
	{
		DEBUG ( 0, "top file open error %d, ", errno )
		DEBUG ( 0, "opening %s \n", argv[argn] )
		return ( errno );
	}
	DEBUG ( 1, "top file '%s' opened \n", argv[argn] )
	argn++;

	botfp = fopen ( argv[argn], "w" );
	if ( botfp == NULL )
	{
		DEBUG ( 0, "bot file open error %d, ", errno )
		DEBUG ( 0, "opening %s \n", argv[argn] )
		return ( errno );
	}
	DEBUG ( 1, "bot file '%s' opened \n", argv[argn] )
	argn++;

	program = 0;

	rc = prg_section ( );
	DEBUG ( 1, "prg_section finished, return code = %d \n", rc )

	if ( program )
	{
		fputs ( ".\n", botfp );
	}
	else
	{
		fputs ( modhead, topfp );
		fputs ( "\n\n", topfp );
	}

	fclose ( infp );
	DEBUG ( 1, "prg file closed \n", argv[argn] )
	fclose ( topfp );
	DEBUG ( 1, "top file closed \n", argv[argn] )
	fclose ( botfp );
	DEBUG ( 1, "bot file closed \n", argv[argn] )

	return ( rc );
}


/*============================================================================*/


static	int	prg_section ( )

{
	int		rc;


	DEBUG ( 2, "prg_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "prg_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				DEBUG ( 0, "CONST in prg section! \n", 0 )
				return ( ERRCON );
		case TYPTYP:
				DEBUG ( 0, "TYPE in prg section! \n", 0 )
				return ( ERRTYP );
		case TYPVAR:
				DEBUG ( 0, "VAR in prg section! \n", 0 )
				return ( ERRVAR );
		case TYPFUN:
				DEBUG ( 0, "FUNCTION in prg section! \n", 0 )
				break;
		case TYPPRC:
				DEBUG ( 0, "PROCEDURE in prg section! \n", 0 )
				break;
		case TYPPRG:
				program = 1;
				outfp = topfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "prg_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "prg_section complete \n", 0 )
		return ( 0 );
	}
}


