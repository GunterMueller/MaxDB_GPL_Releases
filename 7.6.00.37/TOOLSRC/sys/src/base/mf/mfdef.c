/*	mfdef.c		

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
 *	module frame definition part
 *
 *	split definition part into global var, and global procedural sections
 */

#define USAGE "usage: %s [-Dl] [-t] def(in) glv(out) glp(out) \n"
#define ARGS	== 3


#include	"mf.h"

static	int  def_section ();

static	FILE		*glvfp;
static	FILE		*glpfp;


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
		DEBUG ( 0, "definitions file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "definitions file '%s' opened \n", argv[argn] )
	argn++;

	glvfp = fopen ( argv[argn], "w" );
	if ( glvfp == NULL )
	{
		DEBUG ( 0, "global var file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "global var file '%s' opened \n", argv[argn] )
	argn++;

	glpfp = fopen ( argv[argn], "w" );
	if ( glpfp == NULL )
	{
		DEBUG ( 0, "global procedural file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "global procedural file '%s' opened \n", argv[argn] )
	argn++;

	rc = def_section ( );
	DEBUG ( 1, "definitions complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "definitions file closed \n", 0 )
	fclose ( glvfp );
	DEBUG ( 1, "global var file closed \n", 0 )
	fclose ( glpfp );
	DEBUG ( 1, "global procedural file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	def_section ( )

{
	int		rc;


	DEBUG ( 2, "def_section started \n", 0 )

	outfp = NULL;
	c = fgetc ( infp );

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "def_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				DEBUG ( 0, "CONST in definitions! \n", 0 )
				return ( ERRCON );
		case TYPTYP:
				DEBUG ( 0, "TYPE in definitions! \n", 0 )
				return ( ERRTYP );
		case TYPVAR:
				outfp = glvfp;
				break;
		case TYPFUN:
		case TYPPRC:
		case TYPPRG:
				outfp = glpfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
/*
				rc = copyall ( );
				if ( rc ) return ( rc );
*/
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "def_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "def_section complete \n", 0 )
		return ( 0 );
	}
}


