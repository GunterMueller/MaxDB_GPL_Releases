/*	mfexp.c		

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
 *	module frame external procedural part
 *
 *	add 'external keyword' to external procedure declarations
 */

#define USAGE "usage: %s [-Dl] [-t] exp(in) ext(out) 'externalkeyword' \n"
#define	ARGS	== 3


#include	"mf.h"

static	int	exp_section ( );

static	char		extkey[WRDSIZ];


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
		DEBUG ( 0, "external proc file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "external proc file '%s' opened \n", argv[argn] )
	argn++;

	outfp = fopen ( argv[argn], "w" );
	if ( outfp == NULL )
	{
		DEBUG ( 0, "externals file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "externals file '%s' opened \n", argv[argn] )
	argn++;

	strncpy ( extkey, argv[argn], sizeof(extkey) );
	DEBUG ( 1, "externalkeyword is '%s' \n", extkey )
	argn++;

	rc = exp_section ( );
	DEBUG ( 1, "exp_section complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "external proc file closed \n", 0 )
	fclose ( outfp );
	DEBUG ( 1, "externals file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	exp_section ( )

{
	int		rc;


	DEBUG ( 2, "exp_section started \n", 0 )

	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "exp_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				DEBUG ( 0, "CONST in glb proc section! \n", 0 )
				return ( ERRCON );
		case TYPTYP:
				DEBUG ( 0, "TYPE in glb proc section! \n", 0 )
				return ( ERRTYP );
		case TYPVAR:
				DEBUG ( 0, "VAR in glb proc section! \n", 0 )
				return ( ERRVAR );
		case TYPFUN:
		case TYPPRC:
				if ( outfp != NULL ) fputs ( keyword, outfp );
				rc = nextsemic ( );
				if ( rc ) return ( rc );
				if ( outfp != NULL ) fputc ( c, outfp );
				c = fgetc ( infp );
				fputs ( "\n        ", outfp );
				fputs ( extkey, outfp );
				fputs ( " ;\n\n", outfp );
				break;
		case TYPPRG:
				DEBUG ( 0, "PROGRAM in glb proc section!\n", 0 )
				return ( ERRPRG );
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "exp_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "exp_section complete \n", 0 )
		return ( 0 );
	}
}


