/*	mfcod.c		

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
 *	module frame code part
 *
 *	split code part into const, type, var, and procedural sections
 */

#define USAGE "usage: %s [-Dl] [-t] cod(in) con(o) typ(o) var(o) stm(o) \n"
#define ARGS	== 5


#include	"mf.h"

static	int	cod_section ( );

static	FILE		*confp;
static	FILE		*typfp;
static	FILE		*varfp;
static	FILE		*stmfp;


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
		DEBUG ( 0, "code file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "code file '%s' opened \n", argv[argn] )
	argn++;

	confp = fopen ( argv[argn], "w" );
	if ( confp == NULL )
	{
		DEBUG ( 0, "const file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "const file '%s' opened \n", argv[argn] )
	argn++;

	typfp = fopen ( argv[argn], "w" );
	if ( typfp == NULL )
	{
		DEBUG ( 0, "type file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "type file '%s' opened \n", argv[argn] )
	argn++;

	varfp = fopen ( argv[argn], "w" );
	if ( varfp == NULL )
	{
		DEBUG ( 0, "var file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "var file '%s' opened \n", argv[argn] )
	argn++;

	stmfp = fopen ( argv[argn], "w" );
	if ( stmfp == NULL )
	{
		DEBUG ( 0, "statement file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "statement file '%s' opened \n", argv[argn] )
	argn++;

	rc = cod_section ( );
	DEBUG ( 1, "code complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "code file closed \n", 0 )
	fclose ( confp );
	DEBUG ( 1, "const file closed \n", 0 )
	fclose ( typfp );
	DEBUG ( 1, "type file closed \n", 0 )
	fclose ( varfp );
	DEBUG ( 1, "var file closed \n", 0 )
	fclose ( stmfp );
	DEBUG ( 1, "statement file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	cod_section ( )

{
	int		rc;


	DEBUG ( 2, "cod_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "cod_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				outfp = confp;
				break;
		case TYPTYP:
				outfp = typfp;
				break;
		case TYPVAR:
				outfp = varfp;
				break;
		case TYPFUN:
		case TYPPRC:
		case TYPBEG:
				outfp = stmfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
				rc = copyall ( );
				if ( rc ) return ( rc );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "cod_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "cod_section complete \n", 0 )
		return ( 0 );
	}
}


