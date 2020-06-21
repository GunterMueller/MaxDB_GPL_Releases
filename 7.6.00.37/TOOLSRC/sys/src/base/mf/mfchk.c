/*      mfchk.c         

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
 *      module frame check define section against code section and
 *      optional check defined procedure names against module name
 *
 *      if third parameter (modname) is given, all procedures of the
 *      define section will be checked against the module name.
 */

#define USAGE "usage: %s [-Dl] [-t] gpc(in) fwd(in) [modname] \n"
#define ARGS    >= 2


#include	"mf.h"


#define         MAXGPCDEFS              256
#define         PROCDEFSIZE             2048

struct def_info
	{
		int                     coded ;
		char                    dtype [ KWDSIZ ] ;
		char                    name [ WRDSIZ ] ;
		char                    def [ PROCDEFSIZE ] ;
	} ;

int                     errcod;
int                     gpcno;
struct def_info         gpcdef[MAXGPCDEFS];
struct def_info         fwddef;

static  int     gpc_section ( );
static  int     fwd_section ( );
static  int     get_definition (struct def_info *def);
static  int     check_procedure (struct def_info * def);
static  int     find_uncoded_procedures ( );
static  int     check_procnames (char *modname);

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
		DEBUG ( 0, "global procedure file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "global procedure file '%s' opened \n", argv[argn] )
	argn++;

	rc = gpc_section ( );
	DEBUG ( 1, "gpc_section finished, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "global procedure file closed \n", 0 )

	if ( rc ) return ( rc );


	infp = fopen ( argv[argn], "r" );
	if ( infp == NULL )
	{
		DEBUG ( 0, "forward file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "forward file '%s' opened \n", argv[argn] )
	argn++;

	rc = fwd_section ( );
	DEBUG ( 1, "fwd_section finished, return code = %d \n", rc )

	fclose ( infp );
	DEBUG ( 1, "forward file closed \n", 0 )

	if ( rc ) return ( rc );


	if ( argn < argc )
	{
		check_procnames ( argv[argn] );
	}

	return ( errcod );
}


/*============================================================================*/


static  int     gpc_section ( )

{
	int             rc;


	DEBUG ( 2, "gpc_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	for ( gpcno = 0 ; gpcno < MAXGPCDEFS ; gpcno ++ )
	{
		rc = get_definition ( & gpcdef [ gpcno ] ) ;
		if ( rc ) break ;

		DEBUG ( 2, "gpc_section: got: '%s ", gpcdef[gpcno].dtype )
		DEBUG ( 2, "%s ", gpcdef[gpcno].name )
		DEBUG ( 2, "%s' \n", gpcdef[gpcno].def )
	}

	if ( gpcno >= MAXGPCDEFS )
	{
		DEBUG ( 0, "gpc_section: table overflow \n", 0 )
		return ( ERREOF );
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "gpc_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "gpc_section complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static  int     fwd_section ( )

{
	int             rc;


	DEBUG ( 2, "fwd_section started \n", 0 )

	outfp = NULL;
	c = fgetc(infp);

	while (  ( rc = get_definition ( & fwddef ) )  ==  0  )
	{
		DEBUG ( 2, "fwd_section: got: '%s ", fwddef.dtype )
		DEBUG ( 2, "%s ", fwddef.name )
		DEBUG ( 2, "%s' \n", fwddef.def )

		check_procedure ( & fwddef );
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "fwd_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		find_uncoded_procedures ( );

		DEBUG ( 2, "fwd_section complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static  int     get_definition ( def )

struct def_info         * def ;

{
	int             l, lgt, parenthlevel, rc;
	char            *p;


	DEBUG ( 2, "getdef: started \n", 0 )

	memset ( def , 0 , sizeof(*def) ) ;

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "getdef: keyword found: '%s' \n", keyword )
		if ( keywrdtyp == TYPPRC || keywrdtyp == TYPFUN ) break ;
	}
	if ( rc ) return ( rc ) ;

	strncpy ( def->dtype , keyword , sizeof(def->dtype)-1 ) ;

	rc = nextword ( ) ;
	if ( rc ) return ( rc ) ;
	DEBUG ( 2, "getdef: word found: '%s' \n", word )

	if ( ! isalpha ( word [ 0 ] ) )
	{
		DEBUG ( 0, "getdef: illegal proc/func name '%s' \n" , word )
		return ( ERROTH ) ;
	}

	strncpy ( def->name , word , sizeof(def->name)-1 ) ;

	p = def->def ;
	lgt = 0 ;
	parenthlevel = 0 ;

	while (  ( rc = nextword ( ) )  ==  0  )
	{
		DEBUG ( 2, "getdef: word found: '%s' \n", word )

		l = strlen ( word ) ;
		if ( lgt + l + 1 > PROCDEFSIZE )
		{
			DEBUG ( 0,
			"getdef: procedure definition too long '%s' \n",
							 def->name )
			return ( ERREOF );
		}

		memcpy ( p, word, l );
		p += l;
		lgt += l;
		*(p++) = ' ';
		lgt++;

		if ( l == 1 )
		{
			if ( parenthlevel == 0 && word[0] == ';' ) break ;
			if ( word[0] == '(' ) parenthlevel++;
			if ( word[0] == ')' ) parenthlevel--;
		}
	}

	DEBUG ( 2, "getdef: returning %d \n", rc )
	return ( rc );
}


/*============================================================================*/


static  check_procedure ( def )

struct def_info         * def ;

{
	int             i;


	DEBUG ( 2, "check_procedure: name '%s' \n" , def->name )

	for ( i=0; i < gpcno; i++ )
	{
		if ( strcmp ( gpcdef[i].name, def->name ) == 0 ) break;
	}
	if ( i >= gpcno )
	{
		DEBUG ( 2, "check_procedure: local proc/func \n" , 0 )
		return;
	}
	gpcdef[i].coded = 1;
	DEBUG ( 2, "check_procedure: found '%s' \n", gpcdef[i].name )

	if ( strcmp ( gpcdef[i].dtype, fwddef.dtype ) != 0 ||
	     strcmp ( gpcdef[i].def,   fwddef.def   ) != 0    )
	{
		DEBUG ( 0, "check_procedure: differences in '%s' \n",
							def->name )
		errcod++;
	}
	else    DEBUG ( 2, "check_procedure: match \n", 0 )
}


/*============================================================================*/


static  find_uncoded_procedures ( )

{
	int             i;


	DEBUG ( 2, "find_uncoded_procedures: called \n", 0 )

	for ( i=0; i < gpcno; i++ )
	{
		DEBUG ( 2, "find_uncoded_procedures: found '%s' \n",
						     gpcdef[i].name )

		if ( gpcdef[i].coded == 1 ) continue ;

		DEBUG ( 0, "find_uncoded_procedures: not coded: %s \n",
						gpcdef[i].name )
		errcod++;

	}

	DEBUG ( 2, "find_uncoded_procedures: returning \n", 0 )
}


/*============================================================================*/


static  check_procnames ( modname )

char            *modname;

{
	int             checklgt, altlgt, i;
	char            checkname [ 4 ] , altname [ 6 ] ;


	DEBUG ( 2, "check_procnames: called \n", 0 )

	checkname[0] = modname[1] ;
	checkname[1] = modname[3] ;
	checkname[2] = modname[4] ;
	checkname[3] = '\0';
	checklgt = 3;

	altname[0] = modname[1] ;
	altname[1] = modname[2] ;
	altname[2] = modname[3] ;
	altname[3] = modname[4] ;
	altname[4] = '\0';
	altlgt = 4;

	for ( i=0; i < gpcno; i++ )
	{
		DEBUG ( 2, "check_procnames: found '%s' \n", gpcdef[i].name )

		if ( memcmp ( gpcdef[i].name, checkname, checklgt ) != 0 &&
		     memcmp ( gpcdef[i].name,   altname,   altlgt ) != 0    )
		{
			DEBUG ( 0, "check_procnames: incorrect prefix: %s \n",
							gpcdef[i].name )
			errcod++;
		}
	}

	DEBUG ( 2, "check_procnames: returning \n", 0 )
	return ( 0 );
}


