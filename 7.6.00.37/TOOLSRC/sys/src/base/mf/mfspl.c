/*      mfspl.c         

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
 *	module frame split
 *
 *	split module frame into define, use, and code sections
 */

#define USAGE "usage: %s [-Dl] src def use cod \n"
#define ARGS	== 4


#include	"mf.h"


static  char            base_name[WRDSIZ];
static  char            bas_name[WRDSIZ];
static  char            mod_name[WRDSIZ];

static	FILE		*deffp;
static	FILE		*usefp;
static	FILE		*codfp;

static	int split ();
static  int chkmodnam ();
static	int makdef ();
static	int makuse ();
static	int makcod ();

/*============================================================================*/


int	main ( argc, argv )

int		argc;
char		**argv;

{
	int             i, argn, rc;
	char            *p, *pe;


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
#ifdef WIN32
	{
		char *ptmp;
		while( ptmp = strchr(argv[argn], '\\')) { *ptmp = '/';}
	}
#endif

	infp = fopen ( argv[argn], "r" );
	if ( infp == NULL )
	{
		DEBUG ( 0, "source file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "source file '%s' opened \n", argv[argn] )
	p = strrchr ( argv[argn], '/' );
	if ( p == NULL ) p = argv[argn];
	if ( *p == '/' ) p++;
	pe = strrchr ( p, '.' );
	if ( pe == NULL ) pe = p + strlen ( p );
	for ( i=0; i < WRDSIZ && p != pe; i++, p++ )
	{
		base_name[i] = toupper ( *p );
		bas_name[i] = toupper ( *p );
	}
	DEBUG ( 1, "file name is '%s' \n", base_name )
	argn++;
	/* support variants of a module : <bas_name>-<variant> || <bas_name>+<variant> */
	for ( p = bas_name ; * p != '-' && * p != '+' && * p != '\0' ; p ++ ); 
	* p = '\0' ;
	DEBUG ( 1, "module name is '%s' \n", bas_name )

	deffp = fopen ( argv[argn], "w" );
	if ( deffp == NULL )
	{
		DEBUG ( 0, "define file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "define file '%s' opened \n", argv[argn] )
	argn++;

	usefp = fopen ( argv[argn], "w" );
	if ( usefp == NULL )
	{
		DEBUG ( 0, "use file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "use file '%s' opened \n", argv[argn] )
	argn++;

	codfp = fopen ( argv[argn], "w" );
	if ( codfp == NULL )
	{
		DEBUG ( 0, "code file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "code file '%s' opened \n", argv[argn] )
	argn++;

	rc = split ( );
	DEBUG ( 1, "split complete, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "source file closed \n", 0 )
	fclose ( deffp );
	DEBUG ( 1, "define file closed \n", 0 )
	fclose ( usefp );
	DEBUG ( 1, "use file closed \n", 0 )
	fclose ( codfp );
	DEBUG ( 1, "code file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static	int	split ( )

{
	int		rc;


	DEBUG ( 2, "split started \n", 0 )

	rc = chkmodnam ( );
	if ( rc )
	{
		if ( rc == ERREOF )
		{
			DEBUG ( 0, "unexpected eof in search of modname \n",0)
			return ( 2 );
		}
		if ( rc == ERROTH ) return ( 2 );
		DEBUG ( 0, "module name search error %d \n", rc )
		return ( 3 );
	}

	rc = makdef ( );
	if ( rc )
	{
		if ( rc == ERREOF )
		{
			DEBUG ( 0, "unexpected eof in define section \n", 0 )
			return ( 2 );
		}
		DEBUG ( 0, "define section error %d \n", rc )
		return ( 3 );
	}

	rc = makuse ( );
	if ( rc )
	{
		if ( rc == ERREOF )
		{
			DEBUG ( 0, "unexpected eof in use section \n", 0 )
			return ( 4 );
		}
		DEBUG ( 0, "use section error %d \n", rc )
		return ( 5 );
	}

	rc = makcod ( );
	if ( rc )
	{
		if ( rc == ERREOF )
		{
			DEBUG ( 0, "unexpected eof in code section \n", 0 )
			return ( 6 );
		}
		DEBUG ( 0, "code section error %d \n", rc )
		return ( 7 );
	}

	DEBUG ( 2, "split complete \n", 0 )
	return ( 0 );
}


/*============================================================================*/


static  int     chkmodnam ( )

{
	int         i;
	char		*p;
    int         newtag = 0;


	DEBUG ( 2, "chkmodnam started \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "chkmodnam: can't find title line 1 \n", 0)
			return ( ERREOF );
		}
		if ( line [ 0 ]       == '.' &&
		     toupper(line[1]) == 'T' &&
		     toupper(line[2]) == 'T' &&
		     !isalpha(line[3])          )
		{
			DEBUG ( 3, "chkmodnam: found: %s", line )
			for ( i=3; line[i] == ' ' || line[i] == '\t'; i++ );
			/*if ( i == 3 ) continue;*/
			if ( line[i] == '1' ) break;
		}

		if ( toupper(line[0]) == 'M' &&
		     toupper(line[1]) == 'O' &&
		     toupper(line[2]) == 'D' &&
		     toupper(line[3]) == 'N' &&
		     toupper(line[4]) == 'A' &&
		     toupper(line[5]) == 'M' &&
		     toupper(line[6]) == 'E' &&
		     !isalpha(line[7])          )
		{
			DEBUG ( 3, "chkmodnam: found: %s", line )
			while( *p != '\0' && *p != ':' ) p++;
			if ( *p == ':' ) { newtag = 1; break; }
		}
	}

    if ( newtag )
        /* jump to module name */
        while( *p != '\0' && !isalpha(*p) ) p++;
    else
        for ( i=0; i < 3 && *p != '\0'; i++ )
        {
            for ( ; *p != '$' && *p != '\0'; p++ );
            p++;
            DEBUG ( 3, "chkmodnam: remainder: %s", p )
        }

	/* &gar PTS 1104034 */
	/* for ( i=0; i < WRDSIZ && *p != '$' && *p != '\0' && ( isalnum(*p) || *p=='_' ) ; i++, p++ ) */
	for ( i=0; i < WRDSIZ && *p != '$' && *p != '\0' && ( isalnum(*p) || *p=='_' ) ; i++, p++ )
		mod_name[i] = toupper(*p);
	mod_name[i] = '\0';
	DEBUG ( 3, "chkmodnam: module name is '%s' \n", mod_name )

	if ( strcmp(mod_name,bas_name) )
	{
		DEBUG ( 0, "file name and module name mismatch \n", 0 )
		DEBUG ( 0, "base   name is '%s' \n", bas_name )
		DEBUG ( 0, "module name is '%s' \n", mod_name )
		return ( ERROTH );
	}

	DEBUG ( 2, "chkmodnam complete \n", 0 )
	return ( 0 );
}


/*============================================================================*/


static	int	makdef ( )

{
	int		i, j;
	char		*p;


	DEBUG ( 2, "makdef started \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makdef: can't find define start line\n",0)
			return ( ERREOF );
		}
		if ( strncmp ( line, "Define", 6 ) ) continue;
		DEBUG ( 2, "makdef: found: %s", line )
		for ( j=6; line[j] == ' ' || line[j] == '\t'; j++ );
		i = j;
		if ( line[i] == ':' ) break;
	}

	DEBUG ( 2, "makdef: copying \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makdef: can't find termination line \n",0)
			return ( ERREOF );
		}
		if ( line[0]=='.' && isalpha(line[1]) &&
		     isalpha(line[2]) && !isalpha(line[3]) )
		{
			DEBUG ( 3, "makdef: found: %s", line )
			if ( line[1]!='C' || line[2]!='M' ) continue;
			for ( j=3; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == 3 ) continue;
			i = j;
			if ( strncmp ( line+i, "*-END-*", 7 ) ) continue;
			for ( j=i+7; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == i+7 ) continue;
			i = j;
			if ( strncmp ( line+i, "define", 6 ) )
			{
			     DEBUG ( 0,
			     "makdef: WARNING: define section ended with: %s",
									line )
			}
			break;
		}
		fputs ( line, deffp );
	}

	DEBUG ( 2, "makdef complete \n", 0 )
	return ( 0 );
}


/*============================================================================*/


static	int	makuse ( )

{
	int		i, j;
	char		*p;


	DEBUG ( 2, "makuse started \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makuse: can't find use start line \n", 0 )
			return ( ERREOF );
		}
		if ( strncmp ( line, "Use", 3 ) ) continue;
		DEBUG ( 2, "makuse: found: %s", line )
		for ( j=3; line[j] == ' ' || line[j] == '\t'; j++ );
		i = j;
		if ( line[i] == ':' ) break;
	}

	DEBUG ( 2, "makuse: copying \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makuse: can't find termination line \n",0)
			return ( ERREOF );
		}
		if ( line[0]=='.' && isalpha(line[1]) &&
		     isalpha(line[2]) && !isalpha(line[3]) )
		{
			DEBUG ( 3, "makuse: found: %s", line )
			if ( line[1]!='C' || line[2]!='M' ) continue;
			for ( j=3; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == 3 ) continue;
			i = j;
			if ( strncmp ( line+i, "*-END-*", 7 ) ) continue;
			for ( j=i+7; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == i+7 ) continue;
			i = j;
			if ( strncmp ( line+i, "use", 3 ) )
			{
				DEBUG ( 0,
				"makuse: WARNING: use section ended with: %s",
									line )
			}
			break;
		}
		fputs ( line, usefp );
	}

	DEBUG ( 2, "makuse complete \n", 0 )
	return ( 0 );
}


/*============================================================================*/


static	int	makcod ( )

{
	int             i, j;
	char		*p;


	DEBUG ( 2, "makcod started \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makcod: can't find code start line \n",0)
			return ( ERREOF );
		}
		if ( strncmp ( line, "Code", 4 ) ) continue;
		DEBUG ( 2, "makcod: found: %s", line )
		for ( j=4; line[j] == ' ' || line[j] == '\t'; j++ );
		i = j;
		if ( line[i] == ':' ) break;
	}

	DEBUG ( 2, "makcod: copying \n", 0 )

	for ( ; ; )
	{
		p = fgets ( line, sizeof(line), infp );
		if ( p == NULL )
		{
			DEBUG ( 0, "makcod: can't find termination line \n",0)
			return ( ERREOF );
		}
		if ( line[0]=='.' && isalpha(line[1]) &&
		     isalpha(line[2]) && !isalpha(line[3]) )
		{
			DEBUG ( 3, "makcod: found: %s", line )
			if ( line[1]!='C' || line[2]!='M' ) continue;
			for ( j=3; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == 3 ) continue;
			i = j;
			if ( strncmp ( line+i, "*-END-*", 7 ) ) continue;
			for ( j=i+7; line[j] == ' ' || line[j] == '\t'; j++ );
			if ( j == i+7 ) continue;
			i = j;
			if ( strncmp ( line+i, "code", 4 ) )
			{
			       DEBUG ( 0,
			       "makcod: WARNING: code section ended with: %s",
									line )
			}
			break;
		}
		fputs ( line, codfp );
	}

	DEBUG ( 2, "makcod complete \n", 0 )
	return ( 0 );
}

