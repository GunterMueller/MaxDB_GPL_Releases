/*	mfglp.c		

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
 *	module frame global procedural part
 *
 *	split global proc part into program, and global procedural sections
 */

#define USAGE "usage: %s [-Dl] [-t] glp(in) prg(out) prc(out) \n"
#define	ARGS	== 3


#include	"mf.h"

static	int	glp_section ( );

static	FILE		*prgfp;
static	FILE		*prcfp;


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
		DEBUG ( 0, "global proc file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "global proc file '%s' opened \n", argv[argn] )
	argn++;

	prgfp = fopen ( argv[argn], "w" );
	if ( prgfp == NULL )
	{
		DEBUG ( 0, "program file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "program file '%s' opened \n", argv[argn] )
	argn++;

	prcfp = fopen ( argv[argn], "w" );
	if ( prcfp == NULL )
	{
		DEBUG ( 0, "procedures file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "procedures file '%s' opened \n", argv[argn] )
	argn++;

	rc = glp_section ( );
	DEBUG ( 1, "glp_section complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "global procedures file closed \n", 0 )
	fclose ( prgfp );
	DEBUG ( 1, "program file closed \n", 0 )
	fclose ( prcfp );
	DEBUG ( 1, "procedures file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	glp_section ( )

{
	int		rc;


	DEBUG ( 2, "glp_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "glp_section: keyword found: %s, ", keyword )
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
				outfp = prcfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
				break;
		case TYPPRG:
				outfp = prgfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "glp_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "glp_section complete \n", 0 )
		return ( 0 );
	}
}


