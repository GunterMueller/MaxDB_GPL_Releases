#if defined(__STDC__) || defined(__cplusplus)
#define YYCONST const
#define YYPARAMS(x) x
#define YYDEFUN(name, arglist, args) name(args)
#define YYAND ,
#define YYPTR void *
#else
#define YYCONST
#define YYPARAMS(x) ()
#define YYDEFUN(name, arglist, args) name arglist args;
#define YYAND ;
#define YYPTR char *
#endif
#ifndef lint
YYCONST static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley +Cygnus.28) 01/20/91";
#endif
#define YYBYACC 1
#ifndef YYDONT_INCLUDE_STDIO
#include <stdio.h>
#endif
#ifdef __cplusplus
#include <stdlib.h> /* for malloc/realloc/free */
#endif
 /* start of declaration section */

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

#ifdef DEBUG
#undef DEBUG
#endif

#define DEFINE 257
#define UNDEF 258
#define IFDEF 259
#define IFNDEF 260
#define IF 261
#define ELIF 262
#define ELSE 263
#define ENDIF 264
#define IFVAR 265
#define IFNVAR 266
#define IFVARIANT 267
#define IFNVARIANT 268
#define IDENTIFIER 269
#define NUMBER 270
#define TOKEN 271
#define FAST 272
#define ENDFAST 273
#define QUICK 274
#define ENDQUICK 275
#define SLOW 276
#define ENDSLOW 277
#define IFDEBUG 278
#define IFNDEBUG 279
#define ENDDEBUG 280
#define DEFINED 281
#define VARIANT 282
#define NOVARIANT 283
#define DEBUG 284
#define EXISTS 285
#define OR 286
#define AND 287
#define EQ 288
#define NEQ 289
#define NOT 290
#define IN 291
#define LT 292
#define LE 293
#define GT 294
#define GE 295
#define YYERRCODE 256
static YYCONST short yylhs[] = {                                        -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    4,
    0,    5,    0,    0,    0,    6,    0,    7,    0,    9,
    0,   11,    0,    8,    8,    8,   10,   10,   10,    2,
    2,   12,    2,   14,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,   13,   13,
    3,    3,    1,   15,   15,   15,
};
static YYCONST short yylen[] = {                                         2,
    3,    2,    2,    2,    2,    2,    2,    2,    2,    0,
    3,    0,    3,    2,    2,    0,    3,    0,    3,    0,
    3,    0,    3,    1,    1,    1,    1,    1,    1,    3,
    2,    0,    6,    0,    7,    3,    3,    3,    3,    3,
    3,    3,    3,    2,    4,    4,    2,    1,    1,    3,
    0,    2,    1,    1,    1,    1,
};
static YYCONST short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,   16,   18,    0,    0,
    0,    0,   24,   27,   25,   28,   26,   29,   10,   12,
    0,   20,   22,   53,    0,    3,    4,    5,   54,   55,
   56,    0,    0,    0,    0,    0,    0,    0,   48,    0,
   51,   51,    6,    8,    7,    9,   51,   51,   51,   51,
    0,   44,    0,    0,    0,    0,   47,    0,    0,    0,
    0,    0,   32,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   30,    0,    0,    0,    0,
   34,    0,    0,    0,    0,   37,   52,   45,   46,    0,
    0,    0,    0,    0,    0,   33,    0,   35,    0,
};
static YYCONST short yydgoto[] = {                                      21,
   25,   93,   68,   47,   48,   41,   42,   22,   49,   23,
   50,   82,   94,   90,   39,
};
static YYCONST short yysindex[] = {                                   -110,
 -253, -253, -253, -253,  -14,  -14,    0,    0, -253, -253,
 -253, -253,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -14,    0,    0,    0,    0,    0,
    0, -253,  -23,  -17,  -14,  -14, -253, -109,    0, -109,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -109,    0, -253, -250, -185,  -41,    0,  -14,  -14,  -14,
  -14, -267,    0,  -14,  -14,  -14,  -14, -250, -250, -250,
 -250, -250, -250,  -13,   -7,    0, -100, -202, -185, -185,
    0,  -64, -263, -292, -259,    0,    0,    0,    0,  -53,
  -14,  -14, -109,  -40,  -36,    0,  -14,    0, -109,
};
static YYCONST short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   43,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   44,    0,   50,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   52,    0,    0,    0,   25,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   55,   61,   63,
   64,   65,   67,    0,    0,    0,   39,   37,   29,   33,
    0,    0,   18,   10,    1,    0,    0,    0,    0,    0,
    0,    0,  -35,    0,    0,    0,    0,    0,  -33,
};
static YYCONST short yygindex[] = {                                      0,
    3,   79,   -1,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -24,    0,   28,
};
#define YYTABLESIZE 325
static YYCONST short yytable[] = {                                      76,
   36,   66,   67,   97,   26,   27,   28,   97,   49,   39,
   50,   43,   44,   45,   46,   24,   53,   38,   29,   30,
   31,   37,   54,   81,   31,   36,   91,   88,   40,   65,
   66,   67,   41,   89,   52,   67,   42,   92,   43,   57,
   69,   36,    2,   14,   36,   70,   71,   72,   73,   15,
   39,    1,   96,   39,   17,   74,   98,   49,   38,   50,
   19,   38,   11,   13,   21,   31,   23,   95,   31,   40,
    0,    0,   40,   41,    0,    0,   41,   42,    0,   43,
   42,   75,   43,   38,   40,   60,   61,   62,   63,   64,
   65,   66,   67,   36,    0,   87,   87,   87,   87,   87,
   87,    0,   39,   51,   62,   63,   64,   65,   66,   67,
   38,    0,    0,   55,   56,    0,    0,   31,    0,    0,
    0,   40,    0,    0,    0,   41,    0,    0,    0,   42,
    0,   43,    0,    0,    0,    0,   77,   78,   79,   80,
    0,    0,   83,   84,   85,   86,    1,    2,    3,    4,
    5,    6,    7,    8,    9,   10,   11,   12,    0,    0,
    0,   13,   14,   15,   16,   17,   18,   19,   20,    0,
    0,    0,    0,    0,    0,   99,   58,   59,   60,   61,
   62,   63,   64,   65,   66,   67,   59,   60,   61,   62,
   63,   64,   65,   66,   67,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   29,   30,   31,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   32,   33,    0,    0,
   34,    0,    0,    0,    0,   35,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   36,   36,   36,   36,
   36,   36,   36,   36,   36,   39,   39,   39,   39,   39,
   39,   39,   39,   38,   38,   38,   38,   38,   38,   38,
   31,   31,   31,   31,   40,   40,   40,   40,   41,   41,
   41,   41,   42,   42,   43,
};
static YYCONST short yycheck[] = {                                      41,
    0,  294,  295,   44,    2,    3,    4,   44,   44,    0,
   44,    9,   10,   11,   12,  269,   40,    0,  269,  270,
  271,   36,   40,  291,    0,   40,   91,   41,    0,  293,
  294,  295,    0,   41,   32,  295,    0,   91,    0,   37,
   42,   41,    0,    0,   44,   47,   48,   49,   50,    0,
   41,    0,   93,   44,    0,   53,   93,   93,   41,   93,
    0,   44,    0,    0,    0,   41,    0,   92,   44,   41,
   -1,   -1,   44,   41,   -1,   -1,   44,   41,   -1,   41,
   44,   54,   44,    5,    6,  288,  289,  290,  291,  292,
  293,  294,  295,   93,   -1,   68,   69,   70,   71,   72,
   73,   -1,   93,   25,  290,  291,  292,  293,  294,  295,
   93,   -1,   -1,   35,   36,   -1,   -1,   93,   -1,   -1,
   -1,   93,   -1,   -1,   -1,   93,   -1,   -1,   -1,   93,
   -1,   93,   -1,   -1,   -1,   -1,   58,   59,   60,   61,
   -1,   -1,   64,   65,   66,   67,  257,  258,  259,  260,
  261,  262,  263,  264,  265,  266,  267,  268,   -1,   -1,
   -1,  272,  273,  274,  275,  276,  277,  278,  279,   -1,
   -1,   -1,   -1,   -1,   -1,   97,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  295,  287,  288,  289,  290,
  291,  292,  293,  294,  295,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  286,  287,  288,  289,  290,  291,
  292,  293,  294,  295,  269,  270,  271,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  281,  282,   -1,   -1,
  285,   -1,   -1,   -1,   -1,  290,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  286,  287,  288,  289,  290,
  291,  292,  293,  286,  287,  288,  289,  290,  291,  292,
  286,  287,  288,  289,  286,  287,  288,  289,  286,  287,
  288,  289,  286,  287,  286,
};
#define YYFINAL 21
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 295
#if YYDEBUG
static YYCONST char *YYCONST yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'$'",0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DEFINE",
"UNDEF","IFDEF","IFNDEF","IF","ELIF","ELSE","ENDIF","IFVAR","IFNVAR",
"IFVARIANT","IFNVARIANT","IDENTIFIER","NUMBER","TOKEN","FAST","ENDFAST","QUICK",
"ENDQUICK","SLOW","ENDSLOW","IFDEBUG","IFNDEBUG","ENDDEBUG","DEFINED","VARIANT",
"NOVARIANT","DEBUG","EXISTS","OR","AND","EQ","NEQ","NOT","IN","LT","LE","GT",
"GE",
};
static YYCONST char *YYCONST yyrule[] = {
"$accept : line",
"line : DEFINE ident expr",
"line : DEFINE ident",
"line : UNDEF ident",
"line : IFDEF ident",
"line : IFNDEF ident",
"line : IFVAR ident",
"line : IFVARIANT ident",
"line : IFNVAR ident",
"line : IFNVARIANT ident",
"$$1 :",
"line : IFDEBUG $$1 comment",
"$$2 :",
"line : IFNDEBUG $$2 comment",
"line : IF expr",
"line : ELIF expr",
"$$3 :",
"line : ELSE $$3 comment",
"$$4 :",
"line : ENDIF $$4 comment",
"$$5 :",
"line : startblock $$5 comment",
"$$6 :",
"line : endblock $$6 comment",
"startblock : FAST",
"startblock : QUICK",
"startblock : SLOW",
"endblock : ENDFAST",
"endblock : ENDQUICK",
"endblock : ENDSLOW",
"expr : '(' expr ')'",
"expr : NOT expr",
"$$7 :",
"expr : expr IN $$7 '[' exprlst ']'",
"$$8 :",
"expr : expr NOT IN $$8 '[' exprlst ']'",
"expr : expr GT expr",
"expr : expr GE expr",
"expr : expr LT expr",
"expr : expr LE expr",
"expr : expr EQ expr",
"expr : expr NEQ expr",
"expr : expr AND expr",
"expr : expr OR expr",
"expr : DEFINED ident",
"expr : VARIANT '(' ident ')'",
"expr : EXISTS '(' token ')'",
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
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
#ifndef YYMAXSTACKSIZE
#define YYMAXSTACKSIZE 10000
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
YYSTYPE yyval;
YYSTYPE yylval;
static short *yyss;
static YYSTYPE *yyvs;
static int yystacksize;
#define yyfree(x) free(x)
extern int yylex();

static YYPTR
YYDEFUN (yymalloc, (bytes), unsigned bytes)
{
    YYPTR ptr = (YYPTR) malloc (bytes);
    if (ptr != 0) return (ptr);
    yyerror ("yyparse: memory exhausted");
    return (0);
}

static YYPTR
YYDEFUN (yyrealloc, (old, bytes), YYPTR old YYAND unsigned bytes)
{
    YYPTR ptr = (YYPTR) realloc (old, bytes);
    if (ptr != 0) return (ptr);
    yyerror ("yyparse: memory exhausted");
    return (0);
}

static int
#ifdef __GNUC__
__inline__
#endif
yygrow ()
{
    int old_stacksize = yystacksize;
    short *new_yyss;
    YYSTYPE *new_yyvs;

    if (yystacksize == YYMAXSTACKSIZE)
        return (1);
    yystacksize += (yystacksize + 1 ) / 2;
    if (yystacksize > YYMAXSTACKSIZE)
        yystacksize = YYMAXSTACKSIZE;
#if YYDEBUG
    if (yydebug)
        printf("yydebug: growing stack size from %d to %d\n",
               old_stacksize, yystacksize);
#endif
    new_yyss = (short *) yyrealloc ((char *)yyss, yystacksize * sizeof (short));
    if (new_yyss == 0)
        return (1);
    new_yyvs = (YYSTYPE *) yyrealloc ((char *)yyvs, yystacksize * sizeof (YYSTYPE));
    if (new_yyvs == 0)
    {
        yyfree (new_yyss);
        return (1);
    }
    yyss = new_yyss;
    yyvs = new_yyvs;
    return (0);
}

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
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#if YYDEBUG
#ifdef __cplusplus
extern "C" char *getenv();
#else
extern char *getenv();
#endif
#endif

int
yyparse()
{
    register int yym, yyn, yystate;
    register YYSTYPE *yyvsp;
    register short *yyssp;
    short *yysse;
#if YYDEBUG
    register YYCONST char *yys;

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

    if (yyss == 0)
    {
        yyss = (short *) yymalloc (YYSTACKSIZE * sizeof (short));
        if (yyss == 0)
          goto yyabort;
        yyvs = (YYSTYPE *) yymalloc (YYSTACKSIZE * sizeof (YYSTYPE));
        if (yyvs == 0)
        {
            yyfree (yyss);
            goto yyabort;
        }
        yystacksize = YYSTACKSIZE;
    }
    yysse = yyss + yystacksize - 1;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;
    goto yyloop;

yypush_lex:
    yyval = yylval;
    yystate = yytable[yyn];
yypush:
    if (yyssp >= yysse)
    {
        int depth = yyssp - yyss;
        if (yygrow() != 0)
             goto yyoverflow;
        yysse = yyss + yystacksize -1;
        yyssp = depth + yyss;
        yyvsp = depth + yyvs;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    yyn = yysindex[yystate];
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if (yyn != 0
        && ((yyn += yychar), ((unsigned)yyn <= (unsigned)YYTABLESIZE))
        && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyerrflag > 0)  --yyerrflag;
        yychar = (-1);
        goto yypush_lex;
    }
    yyn = yyrindex[yystate];
    if (yyn != 0
        && ((yyn += yychar), ((unsigned)yyn <= (unsigned)YYTABLESIZE))
        && yycheck[yyn] == yychar)
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
            yyn = yysindex[*yyssp];
            if (yyn != 0
                && ((yyn += YYERRCODE), ((unsigned)yyn <= (unsigned)YYTABLESIZE))
                && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                goto yypush_lex;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
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
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '%s' \n", yyvsp[-1] , yyvsp[0] ))
		define_var ( yyvsp[-1] , yyvsp[0] );
	    }
	}
break;
case 2:
{
	    if ( ! suppress )
	    {
		DBG3 (( "define '%s' as '1' \n", yyvsp[0] ))
		define_var ( yyvsp[0] , "1" );
	    }
	}
break;
case 3:
{
	    if ( ! suppress )
	    {
		DBG3 (( "undef '%s' \n", yyvsp[0] ))
		define_var ( yyvsp[0] , (char*)0 );
	    }
	}
break;
case 4:
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
{
		DBG3 (( "ifvar '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( yyvsp[0] ) == 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 7:
{
		DBG3 (( "ifvariant '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( yyvsp[0] ) == 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 8:
{
		DBG3 (( "ifnvar '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  (find_variant ( yyvsp[0] ) != 0);
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 9:
{
		DBG3 (( "ifnvariant '%s' \n", yyvsp[0] ))
		if ( nesting_level < MAXNESTING )
		{
			suppress_value [ nesting_level ++ ] = suppress ;
			suppress = suppress ||  ( find_variant ( yyvsp[0] ) != 0 );
			/* give 'else' the chance to switch the mode */
			suppress_value [ nesting_level ++ ] = 0 ;
		}
		else	nest_err ();
	}
break;
case 10:
{
			lex_ignore = TRUE ;
		}
break;
case 11:
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
break;
case 12:
{
			lex_ignore = TRUE ;
		}
break;
case 13:
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
break;
case 14:
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
case 15:
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
case 16:
{
			lex_ignore = TRUE ;
		}
break;
case 17:
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
case 18:
{
			lex_ignore = TRUE ;
		}
break;
case 19:
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
case 20:
{
			lex_ignore = TRUE ;
		}
break;
case 21:
{
		DBG3 (( "startblock '%s' \n", yyvsp[-2] ))
        if ( nesting_level > 1 )
            block_err( yyvsp[-2] );
		suppress_value [ nesting_level ++ ] = suppress ;
		suppress = suppress || ( concomversion != yyvsp[-2][0] );
		DBG5 (( "startblock:   block '%s', concomversion '%c'\n", yyvsp[-2], concomversion ))
		DBG5 (( "startblock:   nesting_level %d \n", nesting_level-1 ))
		DBG5 (( "startblock:   suppress[%d]   %d \n", nesting_level-1 ,
				suppress_value[nesting_level-1] ))
		DBG5 (( "startblock:   suppress      %d \n", suppress ))
    }
break;
case 22:
{
			lex_ignore = TRUE ;
		}
break;
case 23:
{
		DBG3 (( "endblock '%s' \n", yyvsp[-2] ))
        if ( nesting_level > 1 )
            block_err( yyvsp[-2] );
		suppress = suppress_value [ -- nesting_level ];
		DBG5 (( "endblock: nesting_level %d \n", nesting_level ))
		DBG5 (( "endblock: suppress      %d \n", suppress ))
    }
break;
case 30:
{
		DBG5 (( "parenthesis '%s' \n", yyvsp[-1] ))
		yyval = yyvsp[-1] ;
	}
break;
case 31:
{
		DBG5 (( "not '%s' \n", yyvsp[0] ))
		yyval = ISFALSE ( yyvsp[0] ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 32:
{
			DBG5 (( "in: testing args for '%s' \n", yyvsp[-1] ))
			is_in = 0 ;
			in_expr = yyvsp[-1] ;
		}
break;
case 33:
{
		DBG5 (( "in \n" ))
		yyval = is_in ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 34:
{
			DBG5 (( "not in: testing args for '%s' \n", yyvsp[-2] ))
			is_in = 0 ;
			in_expr = yyvsp[-2] ;
		}
break;
case 35:
{
		DBG5 (( "not in \n" ))
		yyval = is_in ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 36:
{
		DBG5 (( "gt '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = (num_comp ( yyvsp[-2] , yyvsp[0] ) > 0) ? "1" : "0";
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 37:
{
		DBG5 (( "ge '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( num_comp ( yyvsp[-2] , yyvsp[0] ) >= 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 38:
{
		DBG5 (( "lt '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( num_comp ( yyvsp[-2] , yyvsp[0] ) < 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 39:
{
		DBG5 (( "le '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( num_comp ( yyvsp[-2] , yyvsp[0] ) <= 0) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 40:
{
		DBG5 (( "eq '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( compare ( yyvsp[-2] , yyvsp[0] ) ) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 41:
{
		DBG5 (( "neq '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = ( compare ( yyvsp[-2] , yyvsp[0] ) ) ? "1" : "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 42:
{
		DBG5 (( "and '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = (ISFALSE ( yyvsp[-2] ) || ISFALSE ( yyvsp[0] )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 43:
{
		DBG5 (( "or '%s' '%s' \n", yyvsp[-2] , yyvsp[0] ))
		yyval = (ISFALSE ( yyvsp[-2] ) && ISFALSE ( yyvsp[0] )) ? "0" : "1" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 44:
{
		DBG5 (( "defined '%s' \n", yyvsp[0] ))
		yyval = get_var (yyvsp[0]) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 45:
{
		DBG5 (( "variant '%s' \n", yyvsp[-1] ))
		yyval = find_variant (yyvsp[-1]) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 46:
{
		DBG5 (( "exists '%s' \n", yyvsp[-1] ))
		yyval = check_for_existence (yyvsp[-1]) == 0 ? "0" : "1";
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 47:
{
		DBG5 (( "substitute '%s' \n", yyvsp[0] ))
		yyval = get_var ( yyvsp[0] );
		if ( ! yyval ) yyval = "0" ;
		DBG5 (( "-> '%s' \n", yyval ))
	}
break;
case 48:
{
		yyval = yyvsp[0] ;
	}
break;
case 49:
{
		is_in = is_in || ! compare ( yyvsp[0] , in_expr );
	}
break;
case 50:
{
		is_in = is_in || ! compare ( yyvsp[0] , in_expr );
	}
break;
case 52:
{
		DBG5 (( "comment '%s' \n", yyvsp[0] ))
	}
break;
case 53:
{
		DBG5 (( "ident '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 54:
{
		DBG5 (( "token like identifier '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 55:
{
		DBG5 (( "token like number '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
case 56:
{
		DBG5 (( "token '%s' \n", yyvsp[0] ))
		yyval = yyvsp[0] ;
	}
break;
    }
    yyssp -= yym;
    yyvsp -= yym;
    yym = yylhs[yyn];
    yystate = *yyssp;
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
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
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    yyn = yygindex[yym];
	 if (yyn != 0
        && ((yyn += yystate), ((unsigned)yyn <= (unsigned)YYTABLESIZE))
        && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    goto yypush;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
