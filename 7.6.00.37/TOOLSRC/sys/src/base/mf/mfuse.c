/*	mfuse.c		

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
 *	module frame use part
 *
 *	split use part into external var, and external procedural sections
 */

#define USAGE "usage: %s [-Dl] [-t] use(in) exv(out) exp(out) \n"
#define	ARGS	== 3


#include	"mf.h"


#define		TYPLIN		88
#define		TYPFRM		89

static	int	use_section ( );
static	int	nxtkworln ( );
static	int	nxtsyorln ( );
static	int	nxtalorln ( );

static	FILE		*exvfp;
static	FILE		*expfp;


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
		DEBUG ( 0, "use file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "use file '%s' opened \n", argv[argn] )
	argn++;

	exvfp = fopen ( argv[argn], "w" );
	if ( exvfp == NULL )
	{
		DEBUG ( 0, "external var file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "external var file '%s' opened \n", argv[argn] )
	argn++;

	expfp = fopen ( argv[argn], "w" );
	if ( expfp == NULL )
	{
		DEBUG ( 0, "external procedural file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "external procedural file '%s' opened \n", argv[argn] )
	argn++;

	rc = use_section ( );
	DEBUG ( 1, "use section complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "use file closed \n", 0 )
	fclose ( exvfp );
	DEBUG ( 1, "external var file closed \n", 0 )
	fclose ( expfp );
	DEBUG ( 1, "external procedural file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	use_section ( )

{
	int		rc;


	DEBUG ( 2, "use_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = nxtkworln ( ) )  ==  0  )
	{
		DEBUG ( 2, "use_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		switch ( keywrdtyp )
		{
		case TYPCON:
				DEBUG ( 0, "CONST in use section! \n", 0 )
				return ( ERRCON );
		case TYPTYP:
				DEBUG ( 0, "TYPE in use section! \n", 0 )
				return ( ERRTYP );
		case TYPVAR:
				outfp = exvfp;
				break;
		case TYPFUN:
		case TYPPRC:
		case TYPPRG:
				outfp = expfp;
				if ( outfp != NULL ) fputs ( keyword, outfp );
				break;
		case TYPLIN:
		case TYPFRM:
				outfp = NULL;
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "use_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "use_section complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	nxtkworln ( )

{
	int		rc, i;


	DEBUG ( 3, "nxtkworln started \n" , 0 )

	keywrdtyp = TYPOTH; 

	while ( keywrdtyp == TYPOTH )
	{
		rc = nxtsyorln ( );
		if ( rc ) return ( rc );
		DEBUG ( 3, "nxtkworln: symbol '%s' found \n", symbol )

		for ( i=0; i < sizeof(keyword)-1; i++ )
		{
			if ( symbol[i] == 0 ) break;
			keyword[i] = toupper ( symbol[i] );
		}
		keyword[i] = 0;

		switch ( keyword[0] )
		{
		case 'C':
				if ( strcmp(keyword,"CONST") == 0 )
				{
					keywrdtyp = TYPCON;
				}
				break;
		case 'T':
				if ( strcmp(keyword,"TYPE") == 0 )
				{
					keywrdtyp = TYPTYP;
				}
				break;
		case 'V':
				if ( strcmp(keyword,"VAR") == 0 )
				{
					keywrdtyp = TYPVAR;
				}
				break;
		case 'F':
				if ( strcmp(keyword,"FUNCTION") == 0 )
				{
					keywrdtyp = TYPFUN;
					break;
				}
				if ( strcmp(keyword,"FROM") == 0 )
				{
					keywrdtyp = TYPFRM;
					break;
				}
				break;
		case '-':
				if ( strncmp(keyword,"---",3) == 0 )
				{
					keywrdtyp = TYPLIN;
				}
				break;
		case 'P':
				if ( strcmp(keyword,"PROCEDURE") == 0 )
				{
					keywrdtyp = TYPPRC;
					break;
				}
				if ( strcmp(keyword,"PROGRAM") == 0 )
				{
					keywrdtyp = TYPPRG;
					break;
				}
				break;
		case 'B':
				if ( strcmp(keyword,"BEGIN") == 0 )
				{
					keywrdtyp = TYPBEG;
				}
				break;
		}

		if ( keywrdtyp != TYPOTH )
		{
			DEBUG ( 3, "nxtkworln: keyword '%s' found, ", keyword)
			DEBUG ( 3, "type = %d \n", keywrdtyp )
		}
		else
		{
			DEBUG ( 3, "nxtkworln: no keyword '%s' \n", symbol )
			if ( outfp != NULL ) fputs ( symbol, outfp );
		}
	}

	DEBUG ( 3, "nxtkworln complete \n", 0 )
	return ( 0 );
}


/*============================================================================*/


static	int	nxtsyorln ( )

{
	int		rc, i;


	DEBUG ( 4, "nxtsyorln started \n", 0 )

	rc = nxtalorln ( );
	if ( rc ) return ( rc );
	DEBUG ( 4, "nxtsyorln: char '%c' found \n", c )

	for ( i=0; i < sizeof(symbol)-1; i++ )
	{
		if ( ! (isalnum(c)||c=='_' || c=='-') ) break; 
		symbol[i] = c;
		c = fgetc ( infp );
	}
	symbol[i] = 0;

	DEBUG ( 4, "nxtsyorln complete, symbol '%s' found \n", symbol )
	return ( 0 );
}


/*============================================================================*/


static	int	nxtalorln ( )

{
	int		rc;


	DEBUG ( 5, "nxtalorln started \n", 0 )

	while ( c != EOF  &&  !(isalpha(c)||c=='_' || c=='-')   )
	{
		rc = nextchar ( );
		if ( rc ) return ( rc );
	}

	if ( c == EOF )
	{
		DEBUG ( 5, "nxtalorln: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		DEBUG ( 5, "nxtalorln complete, alpha '%c' found \n", c )
		return ( 0 );
	}
}


