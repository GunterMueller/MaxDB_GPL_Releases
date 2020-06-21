/*	concom.y		

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
 *  Conditional Compilation
 *  Parser and Lexical Analyzer
 */

%{ /* start of declaration section */

#define		MAXNESTING		128

#include	<stdio.h>
#include	<ctype.h>
#include	"vmake.h"

#ifdef WIN32
#pragma warning(disable: 4102)
#endif

typedef	char			* char_ptr ;
#define	YYSTYPE			char_ptr

struct	token_info
{
	struct	token_info	* ti_next ;
	char			    ti_string [ 2 ] ;
				        /* allocated with required size */
};

struct var_info
{
	struct var_info * vi_next ;
	char			* vi_value ;
	char			vi_name [ 2 ];
				    /* allocated with required size */
};



#ifdef	DEBUG
#define	YYDEBUG	1
#define	ERR	DBG0
#else
#define	YYDEBUG	0
#define	ERR(x)	{ err x; }
#endif
#define	printf	err

#define	ISFALSE(x) ((x==0) || (x[0]=='\0') || ((x[0]=='0') && (x[1]=='\0')))

/* exported functions */

global	int			concom ();

/* imported functions */

extern	int			yyparse ();
extern	int			allocate_string ();
extern	int			check_buffer ();
extern	void		err ();
extern	char		** palloc ();
extern	char		** repalloc ();

/* imported data */


/* local functions */

static	int			yylex ();
static	void		yyerror ();
static	void		nest_err ();
static	void		ifless_err ();
static	void		block_err ();
static	void		define_var ();
static	char		* get_var ();
static	int			concomlex ();
static	int			allocate_token ();
static	int			token_type ();
static	int			quoted_string ();
#ifdef	IGNORECOMMENT
static	void		ignore_comment ();
#endif
#ifdef	IGNORECASE
static	int			compare ();
#else
#define				compare		strcmp
#endif
static  int	num_comp ( );
static  int find_variant ();
static  int subst_path ();

/* exported data */

global	int			        yylineno ;
global  char                concomversion = ' ';
#define MAXVARIANTLEN 50
char *concom_variant_local;
struct variant_info *concom_variants_global = NULL;



/* &gar (CTS 1105643) */ 
global int                  concom_debug = 0;
static  char                * in_variant;
static int					is_variant;

/* local data */

static	char		        suppress ;
static	int			        lex_ignore ;
static	int			        is_in ;
static	char		        * in_expr ;
static	struct var_info	    * var_list ;
static	int			        nesting_level ;
static	char		        suppress_value [ MAXNESTING + 2 ];

static	struct	token_info	* token_list ;
static	char		        * ccp ;	/* current character (pointer) */
static	int			        first_token ;
static	unsigned	        tokenlgt ;
static	unsigned	        tokensiz ;
static	char		        * tokenval ;
static	char		        * filename ;

#undef IN
#undef FAST
#undef QUICK
#undef SLOW

%} /* end of declaration section */

%token	DEFINE
%token	UNDEF
%token	IFDEF
%token	IFNDEF
%token	IF
%token	ELIF
%token	ELSE
%token	ENDIF
%token	IFVAR
%token	IFNVAR
%token	IFVARIANT
%token	IFNVARIANT
%token	IDENTIFIER
%token	NUMBER
%token	TOKEN
%token  FAST
%token  ENDFAST
%token  QUICK
%token  ENDQUICK
%token  SLOW
%token  ENDSLOW

/* &gar (CTS 1105643) */
%token  IFDEBUG
%token  IFNDEBUG
%token  ENDDEBUG 
/* <- &gar (CTS 1105643) */

/* cba (CTS 1103594) */
%token  DEFINED
/* !cba (CTS 1103594) */

%token  VARIANT
%token  NOVARIANT
%token  DEBUG
%token  EXISTS


%left	OR
%left	AND
%left	EQ NEQ
%right	NOT
%right	IN

/* cba */
%left	LT
%left	LE
%left	GT
%left	GE
/* !cba */

%start	line

%%

line	: DEFINE ident expr
	{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '%s' \n", $2 , $3 ))
		define_var ( $2 , $3 );
	    }
	}
	| DEFINE ident 
	{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '1' \n", $2 ))
		define_var ( $2 , "1" );
	    }
	}
	| UNDEF  ident
	{
	    if ( ! suppress )
	    {
		DBG3 (( "undef '%s' \n", $2 ))
		define_var ( $2 , (char*)0 );
	    }
	}
	| IFDEF  ident
	{
		DBG3 (( "ifdef '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || ! get_var ( $2 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFNDEF ident
	{
		DBG3 (( "ifndef '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || get_var ( $2 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFVAR  ident
	{
		DBG3 (( "ifvar '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( $2 ) == 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFVARIANT  ident
	{
		DBG3 (( "ifvariant '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( $2 ) == 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFNVAR ident
	{
		DBG3 (( "ifnvar '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  (find_variant ( $2 ) != 0);
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFNVARIANT ident
	{
		DBG3 (( "ifnvariant '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( $2 ) != 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	/* &gar (CTS 1105643) -> */
	| IFDEBUG
		{
			lex_ignore = TRUE ;
		}
		comment
	{
		DBG3 (( "ifdebug \n" ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  (concom_debug == 0 ) ;
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	| IFNDEBUG
		{
			lex_ignore = TRUE ;
		}
		comment
	{
		DBG3 (( "ifndebug \n" ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  (concom_debug != 0 ) ;
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
	/* <- &gar (CTS 1105643) */
	| IF     expr
	{
		DBG3 (( "if '%s' \n", $2 ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || ISFALSE ( $2 );
			/* give 'else' the chance to switch the mode */
			/* suppress_value [ nesting_level ++ ] = 0 ; error! */
			suppress_value [ nesting_level ++ ] = ! suppress ;

			DBG5 (( "if:   nesting_level %d \n", nesting_level-2 ))
			DBG5 (( "if:   suppress[%d]   %d \n", nesting_level-2 ,
					suppress_value[nesting_level-2] ))
			DBG5 (( "if:   suppress[%d]   %d \n", nesting_level-1 ,
					suppress_value[nesting_level-1] ))
			DBG5 (( "if:   suppress      %d \n", suppress ))
		}
		else	nest_err ();
	}
	| ELIF   expr
	{
		DBG3 (( "elif '%s' \n", $2 ))
		if ( nesting_level > 1 )
		{
			suppress = ! suppress ||
				suppress_value [ nesting_level - 1 ] ||
				suppress_value [ nesting_level - 2 ];
			suppress = suppress || ISFALSE ( $2 );
			/* take 'else' the chance to switch the mode */
			suppress_value [ nesting_level - 1 ] = ! suppress ||
				suppress_value [ nesting_level - 1 ];

			DBG5 (( "elif: nesting_level %d \n", nesting_level-2 ))
			DBG5 (( "elif: suppress[%d]   %d \n", nesting_level-2 ,
					suppress_value[nesting_level-2] ))
			DBG5 (( "elif: suppress[%d]   %d \n", nesting_level-1 ,
					suppress_value[nesting_level-1] ))
			DBG5 (( "elif: suppress      %d \n", suppress ))
		}
		else	ifless_err ( "elif" );
	}
	| ELSE
		{
			lex_ignore = TRUE ;
		}
		comment
	{
		DBG3 (( "else \n" ))
		if ( nesting_level > 1 )
		{
			suppress = ! suppress ||
				suppress_value [ nesting_level - 1 ] ||
				suppress_value [ nesting_level - 2 ];

			DBG5 (( "else: nesting_level %d \n", nesting_level-2 ))
			DBG5 (( "else: suppress[%d]   %d \n", nesting_level-2 ,
					suppress_value[nesting_level-2] ))
			DBG5 (( "else: suppress[%d]   %d \n", nesting_level-1 ,
					suppress_value[nesting_level-1] ))
			DBG5 (( "else: suppress      %d \n", suppress ))
		}
		else	ifless_err ( "else" );
	}
	| ENDIF
		{
			lex_ignore = TRUE ;
		}
		comment
	{
		DBG3 (( "endif \n" ))
		if ( nesting_level > 1 )
		{
			nesting_level -- ;
			suppress = suppress_value [ -- nesting_level ];

			DBG5 (( "endi: nesting_level %d \n", nesting_level ))
			DBG5 (( "endi: suppress      %d \n", suppress ))
		}
		else	ifless_err ( "endif" );
	}
    | startblock 
		{
			lex_ignore = TRUE ;
		}
		comment
    {
		DBG3 (( "startblock '%s' \n", $1 ))
        if ( nesting_level > 1 )
            block_err( $1 );
		suppress_value [ nesting_level ++ ] = suppress ;
		suppress = suppress || ( concomversion != $1[0] );
		DBG5 (( "startblock:   block '%s', concomversion '%c'\n", $1, concomversion ))
		DBG5 (( "startblock:   nesting_level %d \n", nesting_level-1 ))
		DBG5 (( "startblock:   suppress[%d]   %d \n", nesting_level-1 ,
				suppress_value[nesting_level-1] ))
		DBG5 (( "startblock:   suppress      %d \n", suppress ))
    }
    | endblock 
		{
			lex_ignore = TRUE ;
		}
		comment
    {
		DBG3 (( "endblock '%s' \n", $1 ))
        if ( nesting_level > 1 )
            block_err( $1 );
		suppress = suppress_value [ -- nesting_level ];
		DBG5 (( "endblock: nesting_level %d \n", nesting_level ))
		DBG5 (( "endblock: suppress      %d \n", suppress ))
    }
	;


startblock : FAST | QUICK | SLOW ;

endblock : ENDFAST | ENDQUICK | ENDSLOW ;


expr	: '(' expr ')'
	{
		DBG5 (( "parenthesis '%s' \n", $2 ))
		$$ = $2 ;
	}
	| NOT expr
	{
		DBG5 (( "not '%s' \n", $2 ))
		$$ = ISFALSE ( $2 ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr IN
		{
			DBG5 (( "in: testing args for '%s' \n", $1 ))
			is_in = 0 ;
			in_expr = $1 ;
		}
		'[' exprlst ']'
	{
		DBG5 (( "in \n" ))
		$$ = is_in ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr NOT IN
		{
			DBG5 (( "not in: testing args for '%s' \n", $1 ))
			is_in = 0 ;
			in_expr = $1 ;
		}
		'[' exprlst ']'
	{
		DBG5 (( "not in \n" ))
		$$ = is_in ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
/* cba */
	| expr GT expr
	{
		DBG5 (( "gt '%s' '%s' \n", $1 , $3 ))
		$$ = (num_comp ( $1 , $3 ) > 0) ? "1" : "0";
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr GE expr
	{
		DBG5 (( "ge '%s' '%s' \n", $1 , $3 ))
		$$ = ( num_comp ( $1 , $3 ) >= 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr LT expr
	{
		DBG5 (( "lt '%s' '%s' \n", $1 , $3 ))
		$$ = ( num_comp ( $1 , $3 ) < 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr LE expr
	{
		DBG5 (( "le '%s' '%s' \n", $1 , $3 ))
		$$ = ( num_comp ( $1 , $3 ) <= 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
/* !cba */
	| expr EQ expr
	{
		DBG5 (( "eq '%s' '%s' \n", $1 , $3 ))
		$$ = ( compare ( $1 , $3 ) ) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr NEQ expr
	{
		DBG5 (( "neq '%s' '%s' \n", $1 , $3 ))
		$$ = ( compare ( $1 , $3 ) ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr AND expr
	{
		DBG5 (( "and '%s' '%s' \n", $1 , $3 ))
		$$ = (ISFALSE ( $1 ) || ISFALSE ( $3 )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| expr OR expr
	{
		DBG5 (( "or '%s' '%s' \n", $1 , $3 ))
		$$ = (ISFALSE ( $1 ) && ISFALSE ( $3 )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
/* cba (CTS 1103594) */
	| DEFINED ident
	{
		DBG5 (( "defined '%s' \n", $2 ))
		$$ = get_var ($2) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", $$ ))
	}
/* !cba (CTS 1103594) */
	| VARIANT'(' ident ')'
	{
		DBG5 (( "variant '%s' \n", $3 ))
		$$ = find_variant ($3) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| EXISTS'('  token ')'
	{
		DBG5 (( "exists '%s' \n", $3 ))
		$$ = check_for_existence ($3) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| '$' ident
	{
		DBG5 (( "substitute '%s' \n", $2 ))
		$$ = get_var ( $2 );
		if ( ! $$ ) $$ = "0" ;
		DBG5 (( "-> '%s' \n", $$ ))
	}
	| token
	{
		$$ = $1 ;
	}
	;

exprlst	: expr
	{
		is_in = is_in || ! compare ( $1 , in_expr );
	}
	| exprlst ',' expr
	{
		is_in = is_in || ! compare ( $3 , in_expr );
	}
	;

comment : /* empty */
	| comment token
	{
		DBG5 (( "comment '%s' \n", $2 ))
	}
	;

ident	: IDENTIFIER
	{
		DBG5 (( "ident '%s' \n", $1 ))
		$$ = $1 ;
	}
	;

token	: IDENTIFIER
	{
		DBG5 (( "token like identifier '%s' \n", $1 ))
		$$ = $1 ;
	}
	| NUMBER
	{
		DBG5 (( "token like number '%s' \n", $1 ))
		$$ = $1 ;
	}
	| TOKEN
	{
		DBG5 (( "token '%s' \n", $1 ))
		$$ = $1 ;
	}
	;

%%

/*==========================================================================*/

static	void	yyerror ( str )

char			* str ;

{
	ERR (( "concom: %s in '%s' line %d: '%s' unexpected \n",
				str , filename , yylineno , yylval ))
}

/*==========================================================================*/

static	void	nest_err ( )

{
	ERR (( "concom: nesting depth exceeded in '%s' line %d, max is %d \n",
					    filename , yylineno , MAXNESTING ))
}

/*==========================================================================*/

static	void	ifless_err ( cmd )

char			* cmd ;

{
	ERR (( "concom: error: if-less %s in '%s' line %d \n",
				cmd , filename , yylineno ))
}

/*==========================================================================*/

static	void	block_err ( cmd )

char			* cmd ;

{
	ERR (( "concom: error: block '%s' not at top level in '%s' line %d \n",
				cmd , filename , yylineno ))
    exit( 2 );
}

/*==========================================================================*/

static	void	define_var ( name , value )

char			* name ;
char			* value ;

{
	int			rc ;
	unsigned		l ;
	struct var_info		** svpp , * vp ;


	if ( value )
	    DBG2 (( "define '%s' = '%s'\n", name , value ))
	else
	    DBG2 (( "undef  '%s'\n", name ))

	rc = -1 ;
	for ( svpp = & var_list ; * svpp ; svpp = & (*svpp)->vi_next )
	{
	    rc = compare ( (*svpp)->vi_name , name );
	    if ( rc >= 0 ) break ;
	}

	if ( rc == 0 )
	{
	    vp = * svpp ;
	    if ( ! value )
	    {
		/* undef */
		free ( vp->vi_value );
		vp->vi_value = (char*)0 ;
		return ;
	    }
	    if ( vp->vi_value )
	    {
		ERR ((
		"concom: warning: redefinition of '%s' in '%s' line %d \n",
						name , filename , yylineno ))
	    }
	    (void) allocate_string ( & vp->vi_value , value );
	    return ;
	}

	l = strlen ( name );
	vp = (struct var_info*) palloc ( sizeof(struct var_info) + l );
	if ( ! vp )
	{
	    ERR (( "insufficient memory \n" ))
	    return ;
	}
	FILL ( vp , 0 , sizeof(struct var_info) + l );
	COPY ( vp->vi_name , name , l );
	if ( value ) (void) allocate_string ( & vp->vi_value , value );

	vp->vi_next = * svpp ;
	* svpp = vp ;
}

/*==========================================================================*/

static	char	* get_var ( name )

char			* name ;

{
	int			rc ;
	struct var_info		* vp ;
	char			* p ;


	rc = -1 ;
	for ( vp = var_list ; vp ; vp = vp->vi_next )
	{
	    rc = compare ( vp->vi_name , name );
	    if ( rc >= 0 ) break ;
	}

	if ( rc == 0 )
	{
	    DBG2 (( "found  '%s' = '%s' \n", name , vp->vi_value ))
	    return ( vp->vi_value );
	}

#ifdef	IGNOREENV
	p = (char*) 0 ;
#else
	p = getenv ( name );
#endif
	if ( p )
	    DBG2 (( "found  '%s' = '%s' \n", name , p ))
	else
	    DBG2 (( "found  '%s' undefined \n", name ))
	return ( p );
}

/*==========================================================================*/
/*=== LEXICAL ANALYZER =====================================================*/
/*==========================================================================*/

static	int	yylex ( )

{
	int			token ;


	DBG4 (( "lex: called \n" ))

	token = concomlex ();
	if ( token < 0 ) return ( token );

	if ( allocate_token () ) return ( -1 );

	DBG6 (( "lex: returning %3d '%s' \n", token , yylval ))
	DBG8 (( "lex:                0x%08lx \n", (long) yylval ))
	return ( token );
}

/*==========================================================================*/

static	int	concomlex ( )

{
	int			end_token = FALSE ;


	/*
	 *  Allocate space for special token strings.
	 */
	if ( check_buffer ( & tokenval , & tokensiz , 32 ) ) return ( -1 );

	/*
	 *  Initialize token.
	 */
	FILL ( tokenval , 0 , tokensiz );
	tokenlgt = 0 ;

	if ( ! ccp || lex_ignore )
	{
	    DBG4 (( "lex: returning, ccp 0x%08lx, lex_ignore %d \n",
					(long) ccp , lex_ignore ))
	    lex_ignore = FALSE ;
	    (void) strcpy ( tokenval , "end-of-line" );
	    return ( 0 );
	}

	for ( ; * ccp ; ccp ++ )
	{
	    DBG9 (( isprint(*ccp)?"lex: got '%c'\n":"lex: got 0x%02x\n",
						(*ccp)&0xff ))
	    switch ( * ccp )
	    {
	    case ' ' :
	    case '\t' :
	    case '\n' :
		    /* ignore blanks and tabs, except to terminate token */
		    if ( ! tokenlgt ) continue ;
		    end_token = TRUE ;
		    break ;

	    case '\\' :
		    /* next char is normal char */
		    ccp ++ ;
		    /* backslash at end of line stands for itself */
		    if ( ! * ccp ) ccp -- ;
		    break ;

	    case '"' :
	    case '\'' :
	    case '`' :
		    /* include a quoted string without change */
		    /* exclude the quotes */
		    if ( quoted_string ( * ccp ) ) return ( -1 );
		    continue ;

	    case '#' :
		    /* at begin of token: comment up to end of line */
		    if ( ! tokenlgt )
		    {
			    DBG4 (( "lex: returning, comment \n" ))
			    ccp = (char*)0 ;
			    (void) strcpy ( tokenval , "end-of-line" );
			    return ( 0 );
		    }
		    /* else it is a normal character */
		    break ;

	    case '0' :
	    case '1' :
	    case '2' :
	    case '3' :
	    case '4' :
	    case '5' :
	    case '6' :
	    case '7' :
	    case '8' :
	    case '9' :
		    /*
		     *  Digits at the begin of a token 
		     *  introduce a number.
		     */
		    if ( ! tokenlgt )
		    {
			    long			m ;

			    m = strtol ( ccp , & ccp , 0 );
			    (void) sprintf ( tokenval , "%ld" , m );
			    return ( NUMBER );
		    }
		    /* else it is a normal character */
		    break ;

#ifdef	IGNORECOMMENT
	    case '/' :
		    if ( ccp[1] == '*' )
		    {
			if ( ! tokenlgt )
			{
			    ccp ++ ;
			    if ( ccp[1] )
			    {
				ccp ++ ;
				ignore_comment ( '/' );
			    }
			    continue ;
			}
			end_token = TRUE ;
		    }
		    break ;
/*else	'/' is not a special character */
#endif	/*IGNORECOMMENT*/

	    case '(' :
#ifdef	IGNORECOMMENT
		    if ( ! tokenlgt )
		    {
			if ( ccp[1] == '*' )
			{
			    ccp ++ ;
			    if ( ccp[1] )
			    {
				ccp ++ ;
				ignore_comment ( ')' );
			    }
			    continue ;
			}
			tokenval[0] = '(' ;
			return ( '(' );
			break ;
		    }
		    end_token = TRUE ;
		    break ;
/*else	'(' is treated like the characters below */
#endif	/*IGNORECOMMENT*/

	    case '!' :
	    case '$' :
	    case '&' :
	    case ')' :
	    case ',' :
	    case ';' :
	    case '<' :
	    case '=' :
	    case '>' :
	    case '[' :
	    case ']' :
	    case '{' :
	    case '|' :
	    case '}' :
		    if ( ! tokenlgt )
		    {
			    register char	* p = tokenval ;

			    * ( p ++ ) = * ( ccp ++ );
			    switch ( tokenval [ 0 ] )
			    {
			    case '!' :
				    if ( * ccp == '=' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( NEQ );
				    }
				    return ( NOT );

			    case '&' :
				    if ( * ccp == '&' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( AND );
				    }
				    break ;

			    case '<' :
				    if ( * ccp == '>' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( NEQ );
				    }
				    if ( * ccp == '=' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( LE );
				    }
				    return ( LT );
				    break ;

			    case '>' :
				    if ( * ccp == '=' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( GE );
				    }
				    return ( GT );
				    break ;

			    case '=' :
				    if ( * ccp == '=' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( EQ );
				    }
				    return ( EQ );

			    case '|' :
				    if ( * ccp == '|' )
				    {
					    * ( p ++ ) = * ( ccp ++ );
					    return ( OR );
				    }
			    }
			    return ( tokenval [ 0 ] );
		    }
		    end_token = TRUE ;
		    break ;
	    }

	    if ( end_token ) break ;
	    if ( tokenlgt >= tokensiz - 1 )
	    {
		if ( check_buffer ( & tokenval , & tokensiz , tokenlgt + 31 ) )
		    return ( -1 );
		FILL ( tokenval + tokenlgt , 0 , tokensiz - tokenlgt );
	    }
	    tokenval [ tokenlgt ++ ] = * ccp ;
	}
	tokenval [ tokenlgt ] = '\0' ;
	DBG4 (( "lex: got token '%s' \n", tokenval ))

	if ( ! tokenlgt )
	{
		/*
		 *  If we are here, no special character was found and
		 *  no token was found.
		 *  This can be true only at end of line.
		 *  Force to get a new line at next invocation.
		 */
		DBG4 (( "lex: returning, no more tokens on line \n" ))
		ccp = (char*)0 ;
		(void) strcpy ( tokenval , "end-of-line" );
		return ( 0 );
	}

	if ( first_token )
	{
		first_token = 0 ;
		if ( ! compare ( tokenval , "define"    ) ) return ( DEFINE    );
		if ( ! compare ( tokenval , "undef"     ) ) return ( UNDEF     );
		if ( ! compare ( tokenval , "ifdef"     ) ) return ( IFDEF     );
		if ( ! compare ( tokenval , "ifndef"    ) ) return ( IFNDEF    );
		if ( ! compare ( tokenval , "if"        ) ) return ( IF        );
		if ( ! compare ( tokenval , "elif"      ) ) return ( ELIF      );
		if ( ! compare ( tokenval , "else"      ) ) return ( ELSE      );
		if ( ! compare ( tokenval , "endif"     ) ) return ( ENDIF     );
		if ( ! compare ( tokenval , "ifvar"     ) ) return ( IFVAR     );
		if ( ! compare ( tokenval , "ifnvar"    ) ) return ( IFNVAR    );
		if ( ! compare ( tokenval , "ifvariant" ) ) return ( IFVARIANT );
		if ( ! compare ( tokenval , "ifnvariant") ) return ( IFNVARIANT);
		if ( ! compare ( tokenval , "fast"      ) ) return ( FAST      );
		if ( ! compare ( tokenval , "endfast"   ) ) return ( ENDFAST   );
		if ( ! compare ( tokenval , "quick"     ) ) return ( QUICK     );
		if ( ! compare ( tokenval , "endquick"  ) ) return ( ENDQUICK  );
		if ( ! compare ( tokenval , "slow"      ) ) return ( SLOW      );
		if ( ! compare ( tokenval , "endslow"   ) ) return ( ENDSLOW   );
		/* &gar (CTS 1105643) -> */
		if ( ! compare ( tokenval , "ifdebug"   ) )  return ( IFDEBUG  );
		if ( ! compare ( tokenval , "ifndebug" ) )   return ( IFNDEBUG  );
		/* <- &gar (CTS 1105643) */
	}
	if ( ! compare ( tokenval , "not"    ) ) return ( NOT    );
	if ( ! compare ( tokenval , "in"     ) ) return ( IN     );
	if ( ! compare ( tokenval , "and"    ) ) return ( AND    );
	if ( ! compare ( tokenval , "or"     ) ) return ( OR     );
/* cba (CTS 1103594) */
	if ( ! compare ( tokenval , "defined"     ) ) return ( DEFINED );
/* !cba (CTS 1103594) */
	if ( ! compare ( tokenval , "variant"   ) ) return ( VARIANT );
	if ( ! compare ( tokenval , "novariant" ) ) return ( NOVARIANT );
	if ( ! compare ( tokenval , "exists" ) ) return ( EXISTS );
	
	return ( token_type () );
}

/*==========================================================================*/

static	int	token_type ( )

{
	register char		* p ;


	p = tokenval ;

	if ( !isalpha(*p) && (*p != '_') &&  !isdigit(*p) ) return ( TOKEN );

	for ( p ++ ; * p ; p ++ )
	{
	    if ( !isalpha(*p) && (*p != '_') && !isdigit(*p) )
							return ( TOKEN );
	}

	return ( IDENTIFIER );
}

/*==========================================================================*/

static	int	quoted_string ( delimiter )

int			delimiter ;

{
	for ( ccp ++ ; * ccp && (* ccp != delimiter) ; ccp ++ )
	{
		/*
		 *  Reserve space for two more characters.
		 *  (For the case of backslash escaped characters.)
		 */
		if ( check_buffer ( & tokenval , & tokensiz , tokenlgt + 2 ) )
		{
			return ( -1 );
		}

		/*
		 *  Backslashes may be used to escape the quotes.
		 *  The backslashes are dropped.
		 */
		if ( (* ccp == '\\') && ccp [ 1 ] ) ccp ++ ;

		tokenval [ tokenlgt ++ ] = * ccp ;
	}

	return ( 0 );
}

/*==========================================================================*/

#ifdef	IGNORECOMMENT
static	void	ignore_comment ( delimiter )

int			delimiter ;

{
	for ( ; * ccp ; )
	{
	    for ( ; * ccp && (* ccp != '*') ; ccp ++ );
	    if ( * ccp ) ccp ++ ;
	    if ( * ccp == delimiter ) break ;
	}

	if ( ! * ccp ) ccp -- ;
}
#endif	/*IGNORECOMMENT*/

/*==========================================================================*/

static	int	allocate_token ( )

{
	unsigned		l ;
	struct	token_info	* tip ;


	DBG4 (( "allocate_token: called, tokenval '%s' \n", tokenval ))

	l = strlen ( tokenval );
	tip = (struct token_info*) palloc ( sizeof(struct token_info) + l );
	if ( ! tip )
	{
		ERR (( "insufficient memory \n" ))
		return ( -1 );
	}

	COPY ( tip->ti_string , tokenval , l + 1 ); /* include '\0' */
	tip->ti_next = token_list ;
	token_list = tip ;
	yylval = tip->ti_string ;

	DBG4 (( "allocate_token: allocated token '%s' \n", tip->ti_string ))
	return ( 0 );
}

/*============================================================*/

static  int	num_comp ( str1 , str2 )
char			* str1;
char			* str2;
{
	long l1;
	long l2;

	l1 = atol (str1);
	l2 = atol (str2);
	
	if (l1 < l2) 
		return -1;

	if (l1 > l2) 
		return 1;

	return 0;
}

/* return */
/* 0 if not found */
/* 1 if only local variant */
/* 2 if only global variant */
/* 3 if both variants */
static int find_variant ( str )
char * str;
{
	int rc = 0;
	struct variant_info *vip;

	/* found local variant */
	if (concom_variant_local)
		if ( strcmp (str, concom_variant_local ) == 0 )
			rc++;

	for (vip = concom_variants_global; vip; 
		vip=vip->next)
	{	
		if (strcmp (str, vip->name) == 0 )
		{
			rc+=2;
			break;
		}
	}
	return rc;
}

/* return */
/* 0 if not found */
/* 1 found file */
/* -1 error */
static int check_for_existence ( str )
char * str;
{
	int rc = 0;
	struct stat		statbuf ;
	char *substituted_string;

	DBG4 (( "Check for existence: '%s'\n", str))	
	
	rc = subst_path (str, &substituted_string);
	
	if ( rc != 0 )
	{
		ERR (( "concom: can't cannot substitute variable '%s' \n",
							str ))
		rc = -1;
	}
	else
	{
		rc = stat ( substituted_string , &statbuf );
	    if ( rc < 0 )
		    rc = 0 ; /* not found */
	    else
		    rc = 1 ; /* found */		
	}
	
	return rc;
}

/*==========================================================================*/

#ifdef	IGNORECASE
static	int	compare ( str1 , str2 )

char			* str1 ;
char			* str2 ;

{
	register char		* p , * q ;


	DBG7 (( "compare '%s' , '%s' \n", str1 , str2 ))

	for ( p = str1 , q = str2 ; * p && * q ; p ++ , q ++ )
	{
		if ( * p == * q ) continue ;
		if ( (isupper(*p) ? tolower(*p) : * p) !=
		     (isupper(*q) ? tolower(*q) : * q)    ) break ;
	}

	DBG7 (( "compare '%c' , '%c' \n", *p , *q ))
	return ( (isupper(*p) ? tolower(*p) : *p) -
		 (isupper(*q) ? tolower(*q) : *q)   );
}
#endif	/*IGNORECASE*/

/*==========================================================================*/

global	int	concom ( line , file )

char			* line ;
char			* file ;

{
	struct var_info		* tvp ;
	struct token_info	* tip ;
	int rc = 0;



	filename = file ? file : "" ;

	if ( ! line )
	{
	    /* end of file */
	    if ( suppress || nesting_level )
		{
			ERR (( "concom: incomplete construct at end of file '%s' \n",
								filename ))
			rc = -1;
		}
		
	    /* prepare for next file */
	    yylineno = 0 ;
	    suppress = 0 ;
	    nesting_level = 0 ;

	    while ( var_list )
	    {
	        tvp = var_list ;
	        var_list = tvp->vi_next ;
	        DBG7 (( "lex: freeing var   '%s' \n", tvp->vi_name ))
	        DBG8 (( "lex:                0x%08lx \n", (long) tvp->vi_name ))
	        if ( tvp->vi_value ) free ( tvp->vi_value );
	        free ( (char*) tvp );
	    }

        concom_variant_local  = NULL;
        concom_variants_global = NULL;
	    return ( rc );
	}

	yylineno ++ ;

/*	if ( (line [ 0 ] == '&' ) && suppress_all_concom) return ( 0 ); */
	
	if ( line [ 0 ] != '&' ) return ( suppress ? 0 : 1 );

	ccp = line + 1 ;
	first_token = 1 ;
	DBG3 (( "concom: line %d: %s \n", yylineno , line ))

	if ( yyparse () ) return ( -1 );

	while ( token_list )
	{
	    tip = token_list ; 
	    token_list = token_list->ti_next ;
	    DBG7 (( "lex: freeing token '%s' \n", tip->ti_string ))
	    DBG8 (( "lex:                0x%08lx \n", (long) tip->ti_string ))
	    free ( (char*) tip );
	}

	return ( 0 ); /* always suppress concom lines */
}

static  int     subst_path ( name, path)

    char                    * name ;
    char                    ** path;
    
{
	unsigned        i = 0, j, actlen ;
	char			* p , * q ;
	char			var [ FILENAMESIZE ];

	actlen = strlen( name );
    *path = (char *) palloc( actlen + 1 );

	/* expand environment variables */
	for ( p = name ; *p ; p ++ , i ++ )
	{
	    /* environment substitution starts with '$' */
	    if ( * p == '$' )
	    {
            unsigned varlen;

		    p ++ ;
            /* treat "$$" as "$" */
            if ( * p == '$' )
            {
    		    (*path) [ i ] = * p ;
            }
            else
            {
                if ( * p == LBRACKET )
                {
                    /* variable name is enclosed by braces {} */
                    p ++ ;
                    for ( j = 0 ; * p != RBRACKET && j < sizeof(var); p ++ , j ++ )
                    {
                        var [ j ] = * p ;
                    }
                    if ( * p == RBRACKET ) p ++ ;
                }
				else
					if (* p == LBRACKET2 )
					{
						/* variable name is enclosed by braces () */
						p ++ ;
						for ( j = 0 ; * p != RBRACKET2 && j < sizeof(var); p ++ , j ++ )
						{
							var [ j ] = * p ;
						}
						if ( * p == RBRACKET2 ) p ++ ;
					}
					else
					{
						/* variable name is terminated by non-symbol-character */
						for ( j = 0 ; 
						( isalnum ( * p ) || * p == '_' ) && j < sizeof(var); 
						p ++ , j ++ 
							)
						{
							var [ j ] = * p ;
						}
					}
                if ( j >= sizeof(var) )
                {
                    DBG0 (( "Error: variable name too long '%.*s' \n", sizeof(var) , var ))
                    return ( - 2 );
                }
                var [ j ] = '\0' ;
                
                q = getenv ( var );
                if ( q == NULL )
                {
                    DBG0 (( "Error: cannot substitute variable '%s' \n", var ))
                    return ( - 2 );
                }
                varlen = strlen( q );
                
                *path = (char *) repalloc( *path, actlen - ( strlen( var ) + 1 ) 
                    + varlen + 1 );
                for ( ; * q != '\0' ; q ++ , i ++ )
                {
                    (*path) [ i ] = * q ;
                }
                actlen = actlen - ( strlen( var ) + 1 ) + varlen;
                
                p -- , i -- ; /* compensate the surrounding loop's increment */
            }
        }
	    else
	    {
		    (*path) [ i ] = * p ;
	    }
	}

	(*path) [ i ] = '\0' ;
	DBG4 (( "substitute: path '%s' \n", *path ))

	return ( 0 );
}