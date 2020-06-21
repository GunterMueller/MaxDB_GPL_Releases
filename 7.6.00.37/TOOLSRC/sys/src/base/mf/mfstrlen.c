/*	mfstrlen.c		

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
 *	module frame string length correcture
 *
 *	correct length of stringconstants assigned to a given variable
 */

#define USAGE "usage: %s [-Dl] infile outfile variable stringlength \n"
#define	ARGS	== 4


#include	"mf.h"


static	char		variable[SYMSIZ];
static	int		stringlgt;


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

	strncpy ( variable, argv[argn], sizeof(variable) );
	DEBUG ( 1, "variable is '%s' \n", variable )
	argn++;

	sscanf ( argv[argn], "%d", &stringlgt );
	DEBUG ( 1, "string length is %d \n", stringlgt );
	argn++;

	rc = correcture ( );
	DEBUG ( 1, "correcture complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "input file closed \n", 0 )
	fclose ( outfp );
	DEBUG ( 1, "output file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	correcture ( )

{
	int		rc;


	DEBUG ( 2, "correcture started \n", 0 )

	c = fgetc(infp);

	while (  ( rc = nextsymbol ( ) )  ==  0  )
	{
		DEBUG ( 2, "correcture: symbol found: %s \n", symbol )
		fputs ( symbol, outfp );

		if ( strcmp(symbol,variable) == 0 )
		{
			DEBUG ( 1, "correcture: variable '%s' found \n", symbol)
			while ( c == ' ' || c == '\t' || c == '\n' )
			{
				fputc ( c, outfp );
				c = fgetc ( infp );
				if ( c == EOF )
				{
					DEBUG ( 0,"correcture: end of file\n",0)
					return ( ERREOF );
				}
			}
			if ( c == ':' )
			{
				fputc ( c, outfp );
				if ( (c=fgetc(infp)) == '=' )
				{
					DEBUG ( 1, "correcture: assignment\n",0)
					rc = assignment ( );
					if ( rc ) return ( rc );
				}
				else	DEBUG(1,"correcture: no assignment\n",0)
			}
			else	DEBUG ( 1, "correcture: no assignment \n", 0 )
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "correcture: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "correcture complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	assignment ( )

{
	int		strend, rc, i;


	DEBUG ( 2, "assignment called \n", 0 )

	do
	{
		fputc ( c, outfp );
		c = fgetc ( infp );
	}
	while ( c == ' ' || c == '\t' || c == '\n' );
	if ( c == EOF )
	{
		DEBUG ( 2, "assignment: end of file \n", 0 )
		return ( ERREOF );
	}
	if ( c != '\'' )
	{
		DEBUG ( 2, "assignment: no string \n", 0 )
		return ( 0 );
	}
	fputc ( c, outfp );

	strend = 0;
	for ( i=0; ; i++ )
	{
		c = fgetc ( infp );
		if ( c == EOF )
		{
			DEBUG ( 2, "assignment: end of file \n", 0 )
			return ( ERREOF );
		}

		switch ( c )
		{

		case '\'':

			if ( (c=fgetc(infp)) == '\'' )
			{
				if ( i < stringlgt ) fputc ( c, outfp );
				if ( i < stringlgt ) fputc ( c, outfp );
			}
			else	strend = 1;
			break;

		case '\\':

			if ( i < stringlgt ) fputc ( c, outfp );
			if ( (c=fgetc(infp)) == '\\' )
			{
				if ( i < stringlgt ) fputc ( c, outfp );
			}
			else	/* hex nibble (two hex char following \) */
			{
				if ( i < stringlgt ) fputc ( c, outfp );
				c = fgetc( infp );
				if ( i < stringlgt ) fputc ( c, outfp );
			}
			break;

		default:

			if ( i < stringlgt ) fputc ( c, outfp );

		}
		if ( strend != 0 ) break;
	}
	for ( ; i < stringlgt; i++ ) fputc ( ' ', outfp );
	fputc ( '\'', outfp );

	DEBUG ( 2, "assignment returning %d \n", 0 )
	return ( 0 );
}


