/*      mfconcom.c      

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
 *      module frame conditional compiling
 */

#define USAGE "usage: mfconcom [-Dl] [-n] [-<symbol>=<value>] \
src dst 'machine' 'kind' 'trace' 'compiler' 'os' 'osspec' \n"
#define ARGS    == 8

#ifdef FREEBSD
#include <stdlib.h>
#else
#include	"malloc.h"
#endif

#include	"mf.h"

#define         TOKENSIZE               8
#ifdef	MSDOS
		/* this is a preliminary hack. Necessary because
		 * MSDOS Development Environment gives compiler options
		 * (e.g. -Dsql_oracle) also to mfconcom! */
#undef          TOKENSIZE
#define         TOKENSIZE               12
#endif
#define         NAMESIZE                20
#define         ERRTXTSIZ               60
#define         MAXOPTS                 64

static  int  mf_section ();
global	void			err ( char * txt , ... );

typedef char                    token_type [ TOKENSIZE + 2 ];
typedef char                    name_type  [ NAMESIZE + 2 ];


extern	int			yylineno ;
extern	int			yydebug ;

static  token_type              machine ;
static  token_type              kind ;
static  token_type              trace ;
static  token_type              compiler ;
static  token_type              os ;
static  token_type              osspec ;
static  int                     opt_count ;
static  token_type              opt_symbol [ MAXOPTS ] ;
static  token_type              opt_value  [ MAXOPTS ] ;
static	char			* source ;

static  int                     empty_lines = 0 ; /* 0= no empty lines KWe
                                                     1= empty lines    KWe */

/*============================================================================*/


int	main ( argc, argv )

int		argc;
char		**argv;

{
	int		argn, rc;
	char		*ptr ;


	DEBUGINI 

	ptr = getenv ( "CONCOM_FLAGS" );
	if ( ptr ) get_concom_flags ( ptr );

	for ( argn=1; argn < argc; argn++ )
	{
		if ( argv[argn][0] != '-' ) break;
		rc = concomoption ( argv[argn] );
		if ( rc )
		{
			DBG0 (( USAGE ))
			return ( rc );
		}
	}

	if ( ! (argc-argn ARGS) )
	{
		DBG0 (( "illegal number of arguments %d \n", argc - 1 ))
		DBG0 (( USAGE, argv[0] ))
		return ( 1 );
	}

	source = argv[argn];
	infp = fopen ( source , "r" );
	if ( infp == NULL )
	{
		DBG0 (( "source file open error %d \n", errno ))
		return ( errno );
	}
	DBG1 (( "source file '%s' opened \n", source ))
	argn++;

	outfp = fopen ( argv[argn], "w" );
	if ( outfp == NULL )
	{
		DBG0 (( "destination file open error %d \n", errno ))
		return ( errno );
	}
	DBG1 (( "destination file '%s' opened \n", argv[argn] ))
	argn++;

	strncpy ( machine, argv[argn], sizeof(machine) );
	DBG3 (( "machine is '%s' \n", machine ))
	argn++;

	strncpy ( kind, argv[argn], sizeof(kind) );
	DBG3 (( "kind is '%s' \n", kind ))
	argn++;

	strncpy ( trace, argv[argn], sizeof(trace) );
	DBG3 (( "trace is '%s' \n", trace ))
	argn++;

	strncpy ( compiler, argv[argn], sizeof(compiler) );
	DBG3 (( "compiler is '%s' \n", compiler ))
	argn++;

	strncpy ( os, argv[argn], sizeof(os) );
	DBG3 (( "os is '%s' \n", os ))
	argn++;

	strncpy ( osspec, argv[argn], sizeof(osspec) );
	DBG3 (( "osspec is '%s' \n", osspec ))
	argn++;

	rc = mf_section ( );
	DBG1 (( "mf_section complete, return code = %d \n", rc ))

	fclose ( infp );
	DBG1 (( "source file closed \n", 0 ))
	fclose ( outfp );
	DBG1 (( "destination file closed \n", 0 ))

	return ( rc );
}


/*============================================================================*/


get_concom_flags ( flags )

char			*flags ;

{
	int			rc ;
	char			term ;
	char			*ptr ;
	char			*opt ;


	ptr = flags ;
	for ( ; *ptr ; )
	{
	    /* skip spaces */
	    for ( ; isspace(*ptr) ; ptr ++ );
	    if ( ! *ptr ) break ;
	    opt = ptr ;
	    /* find end of option */
	    for ( ; *ptr && ! isspace(*ptr) ; ptr ++ );
	    /* terminate option */
	    term = *ptr ;
	    *ptr = 0 ;
	    rc = concomoption ( opt );
	    *ptr = term ;
	    if ( rc )
	    {
		DBG0 (( "mfconcom: illegal option in 'CONCOM_FLAGS' \n" ))
		break ;
	    }
	}
}


/*============================================================================*/


int     concomoption ( opt )

char		*opt;

{
	int                     lgt ;
	char                    * p ;


	if ( opt [ 1 ] == 'D' && isdigit ( opt [ 2 ] ) && opt [ 3 ] == '\0' )
	{
		DEBUGLEVEL ( & opt [ 2 ] )
		if ( dbgdbglvl > 5 ) yydebug = 1 ;
		return ( 0 ) ;
	}
/* KWe  */
	if ( opt [ 1 ] == 'n' && opt [ 2 ] == '\0' )
	{
		empty_lines = 1;
		return ( 0 ) ;
	}
/* KWe  */
	/*
	 *  Find the equation sign (=).
	 *  An alternative to this is 0xf7 which is
	 *  an approximation sign in IBM437 code (similar to = and ~) or
	 *  a devisioning sign in ISO8859 code (similar to - and :).
	 */
	for ( p = opt + 1 ;
	      (*p != '=') && (*p != '\0') && (*p != (char) 0xf7) ;
	      p ++ );
	lgt = p - ( opt + 1 );
	if ( lgt > TOKENSIZE )
	{
		DBG0 (( "symbol name too long '%s' \n", opt ))
		return ( 2 );
	}
	if ( (*p != '=') && (*p != (char) 0xf7) )
	{
		DBG0 (( "illegal option '%s' \n", opt ))
		return ( 2 );
	}

	(void) memcpy ( opt_symbol [ opt_count ] , opt + 1 , lgt ) ;
	opt_symbol [ opt_count ] [ lgt ] = '\0' ;
	(void) strncpy ( opt_value [ opt_count ] , opt + 2 + lgt , TOKENSIZE ) ;

	opt_count ++ ;

	return ( 0 );
}


/*============================================================================*/


static  int     mf_section ( )

{
	int                     rc ;
	int                     ec = 0 ;
	int                     i ;
	unsigned                lgt ;


	DBG3 (( "mf_section started \n", 0 ))

	/* let the pre-definitions have line number 0 */
	yylineno = -1 ;
	SPRI ( line , "&define MACH   %s" , machine  );
	if ( concom(line,source) < 0 ) return ( 2 );

	yylineno = -1 ;
	SPRI ( line , "&define KIND   %s" , kind     );
	if ( concom(line,source) < 0 ) return ( 2 );

	yylineno = -1 ;
	SPRI ( line , "&define COMP   %s" , compiler );
	if ( concom(line,source) < 0 ) return ( 2 );

	yylineno = -1 ;
	SPRI ( line , "&define OS     %s" , os       );
	if ( concom(line,source) < 0 ) return ( 2 );

	yylineno = -1 ;
	SPRI ( line , "&define OSSPEC %s" , osspec   );
	if ( concom(line,source) < 0 ) return ( 2 );

	if ( trace [ 0 ] == 'J' ||
	     trace [ 0 ] == 'j' ||
	     trace [ 0 ] == 'Y' ||
	     trace [ 0 ] == 'y'    )
	{
	    yylineno = -1 ;
	    SPRI ( line , "&define TRACE Y" );
	    if ( concom(line,source) < 0 ) return ( 2 );
	}

	/*
	 *  Defines.
	 */
	for ( i = 0 ; i < opt_count ; i ++ )
	{
	    if ( ! opt_value[i][0] ) continue ;

	    DBG3 (( "'%s' is '%s' \n", opt_symbol[i] , opt_value[i] ))

	    yylineno = -1 ;
	    SPRI ( line , "&define %s %s" , opt_symbol[i] , opt_value[i] );
	    if ( concom(line,source) < 0 ) return ( 2 );
	}

	/*
	 *  UNdefines.
	 */
	for ( i = 0 ; i < opt_count ; i ++ )
	{
	    if ( opt_value[i][0] ) continue ;

	    DBG3 (( "'%s' is undefined \n", opt_symbol[i] ))

	    yylineno = -1 ;
	    SPRI ( line , "&undef %s" , opt_symbol[i] );
	    if ( concom(line,source) < 0 ) return ( 2 );
	}

	/* let the first input line start with number 1 */
	yylineno = 0 ;

	while (  fgets(line , sizeof(line) , infp)  )
	{
	    /* eliminate trailing white spaces */
	    lgt = strlen ( line );
	    if ( lgt >= sizeof(line)-1 )
	    {
		DBG0 (( "input line too long (>%d) \n", sizeof(line)-1 ))
		return ( 2 );
	    }
	    if ( lgt )
	    {
		for ( lgt -- ; lgt && isspace(line[lgt]) ; lgt -- );
		if ( ! isspace(line[lgt]) ) lgt ++ ;
	    }
/*IST* /    if ( ! lgt )
/*IST*/	    if ( ! lgt && ! empty_lines )
	    {
/* KWe  */
/*IST: not in this way:		if ( empty_lines ) fputc ( '\n' , outfp );
       delivers empty lines even if in sections wich are to be excluded */
/* KWe  */
		yylineno ++ ;
		continue;
	    }
	    line [ lgt ] = '\0';

	    rc = concom ( line , source );
	    DBG2 (( "%cline %04d '%s' \n", ((rc < 0) ? '?' : (rc?'+':'-')) ,
							yylineno , line ))
	    if ( rc < 0 )
	    {
		ec ++ ;
		continue ;
	    }

	    if ( rc > 0 )
	    {
		line [ lgt ++ ] = '\n';
		line [ lgt ] = '\0';
		fputs ( line , outfp );
	    }
	}

	rc = concom ( (char*)0 , source );
	if ( rc < 0 )
	{
	    ec ++ ;
	}

	DBG3 (( "mf_section complete, rc %d \n", ec ))
	return ( ec );
}


/*============================================================================*/


static  blank_terminate ( line , length )

char                    * line ;
int                     length ;


{
	int                     i ;


	for ( i = 0 ; i < length ; i ++ ) if ( line [ i ] == '\0' ) break;
	for (       ; i < length ; i ++ ) line [ i ] = ' ' ;
}


/*==========================================================================*/

#define	FRAGMENT_SIZE	64

global	int	check_buffer ( bufp , sizp , minsiz )

char			** bufp ;
unsigned		* sizp ;
unsigned		minsiz ;

{
	unsigned		newsiz ;
	char			* newbuf ;


	if ( minsiz < * sizp ) return ( 0 );
	/*
	 *  buffer size must be at least one greater for the '\0'.
	 */
	newsiz = ( minsiz / FRAGMENT_SIZE + 1 ) * FRAGMENT_SIZE ;
	if ( * bufp && * sizp )
		newbuf = realloc ( * bufp , newsiz );
	else	newbuf = malloc ( newsiz );
	if ( newbuf == NULL )
	{
		DBG0 (( "\007\r\ninsufficient memory \r\n" ))
		return ( 3 );
	}
	* bufp = newbuf ;
	* sizp = newsiz ;
	return ( 0 );
}

/*==========================================================================*/

global	int	allocate_string ( dstp , src )

char			** dstp ;
char			* src ;

{
	unsigned		lgt ;
	char			* p ;


	if ( * dstp && ! strcmp ( * dstp , src ) ) return ( 0 );

	lgt = strlen ( src ) + 1 ; /* include '\0' */
	p = malloc ( lgt );
	if ( p == NULL )
	{
		DBG0 (( "insufficient memory for string \n" ))
		return ( 3 );
	}

	COPY ( p , src , lgt );

	if ( * dstp )
	{
		free ( * dstp );
	}
	* dstp = p ;
	return ( 0 );
}

/*==========================================================================*/

global	char	** palloc ( size )

unsigned		size ;

{
#ifdef	OLD
	char			* mem_ptr , ** retval ;


	mem_ptr = malloc ( size );
	
	if ( mem_ptr == NULL )
	{
	    DBG0 (( "FATAL: insufficient dynamic memory \n", 0 ))
	    exit ( 3 );
	}

	if ( (long) mem_ptr % (long) sizeof(char **) )
	{
	    DBG0 (( "FATAL: malloc returned unaligned pointer 0x%08x \n",
								mem_ptr ))
	    exit ( 3 );
	}

#ifdef	lint
	retval = (char **) 0 ; /* suppress lint message:
				  'possible pointer alignment problem' */
#else
	retval = (char **) mem_ptr ;
#endif

	return ( retval );

#else	/*OLD*/
	return ( (char**) malloc(size) );
#endif	/*OLD*/
}

/*==========================================================================*/

#include	"stdarg.h"

/*VARARGS1*/
global	void	err ( char * txt , ... )
{
	va_list			args ;


        va_start ( args, txt );
	vfprintf ( dbgdbgfp , txt , args );
	va_end ( args );
}

