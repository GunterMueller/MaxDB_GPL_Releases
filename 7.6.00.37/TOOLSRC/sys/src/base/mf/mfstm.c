/*	mfstm.c		

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
 *	module frame statement part
 *
 *	split statement part into forward, and procedures declaration sections
 */

#define USAGE "usage: %s [-Dl] [-t] stm(in) fwd(out) pro(out) \n"
#define	ARGS	== 3


#include	"mf.h"

static	int	stm_section ( );

static	FILE		*fwdfp;
static	FILE		*profp;


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
		DEBUG ( 0, "statement file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "statement file '%s' opened \n", argv[argn] )
	argn++;

	fwdfp = fopen ( argv[argn], "w" );
	if ( fwdfp == NULL )
	{
		DEBUG ( 0, "forward file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "forward file '%s' opened \n", argv[argn] )
	argn++;

	profp = fopen ( argv[argn], "w" );
	if ( profp == NULL )
	{
		DEBUG ( 0, "procedures file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "procedures file '%s' opened \n", argv[argn] )
	argn++;

	rc = stm_section ( );
	DEBUG ( 1, "stm_section finished, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "statement file closed \n", 0 )
	fclose ( fwdfp );
	DEBUG ( 1, "forward file closed \n", 0 )
	fclose ( profp );
	DEBUG ( 1, "procedures file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	stm_section ( )

{
	int		rc;


	DEBUG ( 2, "stm_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "stm_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				if ( outfp != NULL )
				{
					fputs ( keyword, outfp );
					break;
				}
				DEBUG ( 0, "CONST in statement section! \n", 0 )
				return ( ERRCON );
		case TYPTYP:
				if ( outfp != NULL )
				{
					fputs ( keyword, outfp );
					break;
				}
				DEBUG ( 0, "TYPE in statement section! \n", 0 )
				return ( ERRTYP );
		case TYPVAR:
				if ( outfp != NULL )
				{
					fputs ( keyword, outfp );
					break;
				}
				DEBUG ( 0, "VAR in statement section! \n", 0 )
				return ( ERRVAR );
		case TYPFUN:
		case TYPPRC:
				DEBUG ( 2, "func- or proc head found \n", 0 )
				outfp = fwdfp;
				fputs ( keyword, fwdfp );
				fputs ( keyword, profp );
				fputs ( "\n        ", profp );
				rc = nextsymbol ( );
				if ( rc ) return ( rc );
				fputs ( symbol, fwdfp );
				fputs ( symbol, profp );
				rc = nextsemic ( );
				if ( rc ) return ( rc );
				if ( outfp != NULL ) fputc ( c, outfp );
				c = fgetc ( infp );
				fputs ( "\n        FORWARD;\n\n", fwdfp );
				fputs ( " ;\n\n", profp );
				outfp = profp;
				DEBUG ( 2, "head of %s finished \n", symbol )
				break;
		case TYPPRG:
				DEBUG ( 0, "PROGRAM in statement section!\n", 0)
				return ( ERRPRG );
		case TYPBEG:
				outfp = profp;
				fputs ( keyword, outfp );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "stm_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "stm_section complete \n", 0 )
		return ( 0 );
	}
}


