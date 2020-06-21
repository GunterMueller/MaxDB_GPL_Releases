/*


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
#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 8 "concom.y"
 /* start of declaration section */

static char what[]="@(#) concom.y 1.2 1999/09/02";

#define		MAXNESTING		128

#if _IBMR2 || OSF1
#include	<stdlib.h>
#endif
#include	<stdio.h>
#include	<string.h>
#include	<memory.h>
#include	<ctype.h>
#include	"mf.h"

typedef	char			* char_ptr ;
#define	YYSTYPE			char_ptr

struct	token_info
{
	struct	token_info	* ti_next ;
	char			ti_string [ 2 ] ;
				/* allocated with required size */
};

struct var_info
{
	struct var_info		* vi_next ;
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
extern	void			err ();
extern	char			** palloc ();
extern	void			free ();
extern	long			strtol ();
#ifndef	IGNOREENV
extern	char			* getenv ();
#endif

/* local functions */

static	int			yylex ();
static	void			yyerror ();
static	void			nest_err ();
static	void			ifless_err ();
static	void			define_var ();
static	char			* get_var ();
static	int			concomlex ();
static	int			allocate_token ();
static	int			token_type ();
static	int			quoted_string ();
#ifdef	IGNORECOMMENT
static	void			ignore_comment ();
#endif
#ifdef	IGNORECASE
static	int			compare ();
#else
#define				compare		strcmp
#endif

/* exported data */

global	int			yylineno ;

/* local data */

static	char			suppress ;
static	int			lex_ignore ;
static	int			is_in ;
static	char			* in_expr ;
static	struct var_info		* var_list ;
static	int			nesting_level ;
static	char			suppress_value [ MAXNESTING + 2 ];

static	struct	token_info	* token_list ;
static	char			* ccp ;	/* current character (pointer) */
static	int			first_token ;
static	unsigned		tokenlgt ;
static	unsigned		tokensiz ;
static	char			* tokenval ;
static	char			* filename ;

#line 117 "y_tab.c"
#define DEFINE 257
#define UNDEF 258
#define IFDEF 259
#define IFNDEF 260
#define IF 261
#define ELIF 262
#define ELSE 263
#define ENDIF 264
#define IDENTIFIER 265
#define NUMBER 266
#define TOKEN 267
#define DEFINED 268
#define OR 269
#define AND 270
#define EQ 271
#define NEQ 272
#define NOT 273
#define IN 274
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    4,    0,    5,
    0,    2,    2,    6,    2,    8,    2,    2,    2,    2,
    2,    2,    2,    2,    7,    7,    3,    3,    1,    9,
    9,    9,
};
short yylen[] = {                                         2,
    3,    2,    2,    2,    2,    2,    2,    0,    3,    0,
    3,    3,    2,    0,    6,    0,    7,    3,    3,    3,
    3,    2,    2,    1,    1,    3,    0,    2,    1,    1,
    1,    1,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    8,   10,    0,   29,
    0,    3,    4,    5,   30,   31,   32,    0,    0,    0,
    0,    0,   24,    0,   27,   27,    0,   22,    0,    0,
   23,    0,    0,    0,    0,    0,   14,    0,    0,   12,
    0,    0,    0,    0,   16,    0,   28,    0,    0,    0,
    0,    0,    0,   15,    0,   17,    0,
};
short yydgoto[] = {                                       9,
   11,   51,   38,   25,   26,   46,   52,   48,   23,
};
short yysindex[] = {                                   -236,
 -258, -258, -258, -258,  -36,  -36,    0,    0,    0,    0,
  -36,    0,    0,    0,    0,    0,    0, -258,  -36,  -36,
 -258, -205,    0, -205,    0,    0, -205,    0, -238,  -31,
    0,  -36,  -36,  -36,  -36, -241,    0, -190, -190,    0,
 -200, -233, -238, -238,    0,  -48,    0,  -47,  -36,  -36,
 -205,  -38,  -33,    0,  -36,    0, -205,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   47,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   48,    0,   51,    0,    0,   58,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,   62,   80,    0,
   15,   13,    5,    9,    0,    0,    0,    0,    0,    0,
  -32,    0,    0,    0,    0,    0,  -30,
};
short yygindex[] = {                                      0,
   16,   -3,   55,    0,    0,    0,   32,    0,   40,
};
#define YYTABLESIZE 284
short yytable[] = {                                      21,
   13,   22,   24,   20,   18,   55,   10,   27,   19,   40,
   55,   25,   20,   26,   21,   29,   30,   12,   13,   14,
    1,    2,    3,    4,    5,    6,    7,    8,   41,   42,
   43,   44,   45,   28,   36,   37,   31,   34,   35,   36,
   37,   13,   49,   50,   13,   18,    2,    6,   18,   19,
    7,   57,   19,   20,   54,   21,   20,    1,   21,   56,
   25,    9,   26,   32,   33,   34,   35,   36,   37,   33,
   34,   35,   36,   37,   15,   16,   17,   47,   47,   11,
   39,   53,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   13,    0,    0,    0,   18,    0,    0,
    0,   19,    0,    0,    0,   20,    0,   21,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   15,   16,
   17,   18,    0,    0,    0,    0,   19,   32,   33,   34,
   35,   36,   37,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   13,
   13,   13,   13,   18,   18,   18,   18,   19,   19,   19,
   19,   20,   20,   21,
};
short yycheck[] = {                                      36,
    0,    5,    6,   40,    0,   44,  265,   11,    0,   41,
   44,   44,    0,   44,    0,   19,   20,    2,    3,    4,
  257,  258,  259,  260,  261,  262,  263,  264,   32,   33,
   34,   35,  274,   18,  273,  274,   21,  271,  272,  273,
  274,   41,   91,   91,   44,   41,    0,    0,   44,   41,
    0,   55,   44,   41,   93,   41,   44,    0,   44,   93,
   93,    0,   93,  269,  270,  271,  272,  273,  274,  270,
  271,  272,  273,  274,  265,  266,  267,   38,   39,    0,
   26,   50,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   93,   -1,   -1,   -1,   93,   -1,   -1,
   -1,   93,   -1,   -1,   -1,   93,   -1,   93,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  265,  266,
  267,  268,   -1,   -1,   -1,   -1,  273,  269,  270,  271,
  272,  273,  274,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,
  270,  271,  272,  269,  270,  271,  272,  269,  270,  271,
  272,  269,  270,  269,
};
#define YYFINAL 9
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 274
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'$'",0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DEFINE",
"UNDEF","IFDEF","IFNDEF","IF","ELIF","ELSE","ENDIF","IDENTIFIER","NUMBER",
"TOKEN","DEFINED","OR","AND","EQ","NEQ","NOT","IN",
};
char *yyrule[] = {
"$accept : line",
"line : DEFINE ident expr",
"line : DEFINE ident",
"line : UNDEF ident",
"line : IFDEF ident",
"line : IFNDEF ident",
"line : IF expr",
"line : ELIF expr",
"$$1 :",
"line : ELSE $$1 comment",
"$$2 :",
"line : ENDIF $$2 comment",
"expr : '(' expr ')'",
"expr : NOT expr",
"$$3 :",
"expr : expr IN $$3 '[' exprlst ']'",
"$$4 :",
"expr : expr NOT IN $$4 '[' exprlst ']'",
"expr : expr EQ expr",
"expr : expr NEQ expr",
"expr : expr AND expr",
"expr : expr OR expr",
"expr : DEFINED ident",
"expr : '$' ident",
"expr : token",
"exprlst : expr",
"exprlst : exprlst ',' expr",
"comment :",
"comment : comment token",
"ident : IDENTIFIER",
"token : IDENTIFIER",
"token : NUMBER",
"token : TOKEN",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 393 "concom.y"

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
	DBG2 (( "get '%s' from env\n", name ))
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
/* cba 1999/09/02 - open bracket results in endless loop */
			ccp ++;
/* !cba 1999/09/02*/
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
		if ( ! compare ( tokenval , "define" ) ) return ( DEFINE );
		if ( ! compare ( tokenval , "undef"  ) ) return ( UNDEF  );
		if ( ! compare ( tokenval , "ifdef"  ) ) return ( IFDEF  );
		if ( ! compare ( tokenval , "ifndef" ) ) return ( IFNDEF );
		if ( ! compare ( tokenval , "if"     ) ) return ( IF     );
		if ( ! compare ( tokenval , "elif"   ) ) return ( ELIF   );
		if ( ! compare ( tokenval , "else"   ) ) return ( ELSE   );
		if ( ! compare ( tokenval , "endif"  ) ) return ( ENDIF  );
	}
	if ( ! compare ( tokenval , "not"    ) ) return ( NOT    );
	if ( ! compare ( tokenval , "in"     ) ) return ( IN     );
	if ( ! compare ( tokenval , "and"    ) ) return ( AND    );
	if ( ! compare ( tokenval , "or"     ) ) return ( OR     );
/* cba */
	if ( ! compare ( tokenval , "defined"     ) ) return ( DEFINED );
/* !cba */
	return ( token_type () );
}

/*==========================================================================*/

static	int	token_type ( )

{
	register char		* p ;


	p = tokenval ;

	if ( !isalpha(*p) && (*p != '_') ) return ( TOKEN );

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


	filename = file ? file : "" ;

	if ( ! line )
	{
	    /* end of file */
	    if ( suppress || nesting_level )
		ERR (( "concom: incomplete construct at end of file '%s' \n",
								filename ))

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
	    return ( 0 );
	}

	yylineno ++ ;

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

#line 912 "y_tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 141 "concom.y"
{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '%s' \n", yyvsp[-1] , yyvsp[0] ))
		define_var ( yyvsp[-1] , yyvsp[0] );
	    }
	}
break;
case 2:
#line 149 "concom.y"
{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '1' \n", yyvsp[0] ))
		define_var ( yyvsp[0] , "1" );
	    }
	}
break;
case 3:
#line 157 "concom.y"
{
	    if ( ! suppress )
	    {
		DBG3 (( "undef '%s' \n", yyvsp[0] ))
		define_var ( yyvsp[0] , (char*)0 );
	    }
	}
break;
case 4:
#line 165 "concom.y"
{
		DBG3 (( "ifdef '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || ! get_var ( yyvsp[0] );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 5:
#line 177 "concom.y"
{
		DBG3 (( "ifndef '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || get_var ( yyvsp[0] );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 6:
#line 189 "concom.y"
{
		DBG3 (( "if '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress || ISFALSE ( yyvsp[0] );
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
break;
case 7:
#line 209 "concom.y"
{
		DBG3 (( "elif '%s' \n", yyvsp[0] ))
		if ( nesting_level > 1 )
		{
			suppress = ! suppress ||
				suppress_value [ nesting_level - 1 ] ||
				suppress_value [ nesting_level - 2 ];
			suppress = suppress || ISFALSE ( yyvsp[0] );
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
break;
case 8:
#line 231 "concom.y"
{
			lex_ignore = TRUE ;
		}
break;
case 9:
#line 235 "concom.y"
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
break;
case 10:
#line 253 "concom.y"
{
			lex_ignore = TRUE ;
		}
break;
case 11:
#line 257 "concom.y"
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
break;
case 12:
#line 272 "concom.y"
{
		DBG5 (( "parenthesis '%s' \n", yyvsp[-1] ))
		yyval = yyvsp[-1] ;
	}
break;
case 13:
#line 277 "concom.y"
{
		DBG5 (( "not '%s' \n", yyvsp[0] ))
		yyval = ISFALSE ( yyvsp[0] ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 14:
#line 283 "concom.y"
{
			DBG5 (( "in: testing args for '%s' \n", yyvsp[-1] ))
			is_in = 0 ;
			in_expr = yyvsp[-1] ;
		}
break;
case 15:
#line 289 "concom.y"
{
		DBG5 (( "in \n" ))
		yyval = is_in ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 16:
#line 295 "concom.y"
{
			DBG5 (( "not in: testing args for '%s' \n", yyvsp[-2] ))
			is_in = 0 ;
			in_expr = yyvsp[-2] ;
		}
break;
case 17:
#line 301 "concom.y"
{
		DBG5 (( "not in \n" ))
		yyval = is_in ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 18:
#line 307 "concom.y"
{
		DBG5 (( "eq '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( compare ( yyvsp[-2] , yyvsp[0] ) ) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 19:
#line 313 "concom.y"
{
		DBG5 (( "neq '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( compare ( yyvsp[-2] , yyvsp[0] ) ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 20:
#line 319 "concom.y"
{
		DBG5 (( "and '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = (ISFALSE ( yyvsp[-2] ) || ISFALSE ( yyvsp[0] )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 21:
#line 325 "concom.y"
{
		DBG5 (( "or '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = (ISFALSE ( yyvsp[-2] ) && ISFALSE ( yyvsp[0] )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 22:
#line 332 "concom.y"
{
		DBG5 (( "defined '%s' \n", yyvsp[0] ))
		yyval = get_var (yyvsp[0]) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 23:
#line 339 "concom.y"
{
		DBG5 (( "substitute '%s' \n", yyvsp[0] ))
		yyval = get_var ( yyvsp[0] );
		if ( ! yyval ) yyval = "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 24:
#line 346 "concom.y"
{
		yyval = yyvsp[0] ;
	}
break;
case 25:
#line 352 "concom.y"
{
		is_in = is_in || ! compare ( yyvsp[0] , in_expr );
	}
break;
case 26:
#line 356 "concom.y"
{
		is_in = is_in || ! compare ( yyvsp[0] , in_expr );
	}
break;
case 28:
#line 363 "concom.y"
{
		DBG5 (( "comment '%s' \n", yyvsp[0] ))
	}
break;
case 29:
#line 369 "concom.y"
{
		DBG5 (( "ident '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 30:
#line 376 "concom.y"
{
		DBG5 (( "token like identifier '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 31:
#line 381 "concom.y"
{
		DBG5 (( "token like number '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 32:
#line 386 "concom.y"
{
		DBG5 (( "token '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
#line 1352 "y_tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
