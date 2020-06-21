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

# line 26 "tws.y"
typedef union
{
	char *cval;
} YYSTYPE;
# define IDENTIFIER 257
# define C_IDENTIFIER 258
# define NUMBER 259
# define CODE 260
# define LIST 261
# define RECUR 262
# define LEFTR 263
# define RIGHTR 264
# define LEFT 265
# define RIGHT 266
# define NONASSOC 267
# define TOKEN 268
# define PREC 269
# define TYPE 270
# define TERM 271
# define START 272
# define UNION 273
# define MARK 274
# define LCURL 275
# define RCURL 276

# line 32 "tws.y"
#include <stdio.h>
#define LEFTREC 0
#define RIGHTREC 1
#define NONREC 2
#define NONALT 3
	/*IST extern char *malloc(), *realloc(); */
	/*IST extern int strlen(); */
	char *new_NT();
	char c, *n, *Lhs;
	char *string;
	int start = 0, first = 1, x;
	int default_recur = LEFTREC;
	int recur_flag = LEFTREC;
	char *tag_type;
	FILE *tempfile;
#ifdef __cplusplus
#  include <stdio.h>
#  include <yacc.h>
#endif	/* __cplusplus */
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif

/* __YYSCLASS defines the scoping/storage class for global objects
 * that are NOT renamed by the -p option.  By default these names
 * are going to be 'static' so that multi-definition errors
 * will not occur with multiple parsers.
 * If you want (unsupported) access to internal names you need
 * to define this to be null so it implies 'extern' scope.
 * This should not be used in conjunction with -p.
 */
#ifndef __YYSCLASS
# define __YYSCLASS static
#endif
YYSTYPE yylval;
__YYSCLASS YYSTYPE yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 240 "tws.y"


#include <stdio.h>

#define BLOCKSIZE 1024
char *buffer;
int i = 0, blocks = 1;
#define getbuf  ((i / blocks > 1000) ? \
((buffer = realloc(buffer, ++blocks * BLOCKSIZE)), \
((buffer == 0) ? \
(char) yyerror("Out of memory for buffer") : (buffer[i++] = getch()))) : \
(buffer[i++] = getch()))

main()
{
	buffer = malloc(BLOCKSIZE);
	return(yyparse());
}

yyerror(s)
char *s;
{
	fprintf(stderr, "%s: %s\n", s, buffer);
}

#define MAX_TYPE 500
char *type_table[MAX_TYPE];
char *id_table[MAX_TYPE];
int  last_entry = 0; /* J.Bruehe 91-Aug-01  "int" was missing */

find(name)
char *name;
{
	int i;
	
	for(i = 0; i<last_entry; i++)
		if(strcmp(name, id_table[i]) == 0) return(i);
	return(-1);
}

enter_type(typ,name)
char *typ, *name;
{
	int i;

	if(typ == NULL) return;
	else {
		if((i = find(name)) < 0) i = last_entry++;
		id_table[i] = name;
		type_table[i] = typ;
	}
}

char *lookup_type(name)
char *name;
{
	int i;

	if((i = find(name)) < 0)
		return(NULL);
	else return(type_table[i]);
}

match(s1, s2)
char *s1, *s2;
{
	while (*s1 != '\0' && *s2 != '\0')
		if ((s1++,*s2++ != '.') && *(s1 - 1) != *(s2 - 1)) return(0);
	return((*s1 == *s2) ? 1 : 0);
}

#define MAXRULES 200
int count = -1;

char *new_NT(typ)
char *typ;
{
	char *name;

	name = malloc(10);
	sprintf(name, "yynew%03d", ++count);
	if(typ != NULL)
		fprintf(tempfile,"\n%%type %s %s", typ, name);
	return (name);
}

char *emit_rule(Rec, Lp, X1, X2, A2, Y1, A1)
int Rec;
char  *Lp, *X1, *X2, *A2, *Y1, *A1;
{
	int i = -1;
	char *rp, *typ;
	char *n, *temp, *temp1, *temp2, *rules[MAXRULES], *names[MAXRULES];

	typ = lookup_type(X1);
	if(!first || !(buffer[0] == '|' || buffer[0] == ';'))
		Lp = "........";
	if(strcmp(A2,"") == 0) A2 = A1;
	rp = string = malloc(strlen(Lp)+strlen(Lp)+strlen(X1)+strlen(X2)+
		strlen(A2)+strlen(Y1)+strlen(A1)+28);
	switch(Rec) {
		case LEFTREC:
			if(strcmp(X2,"") != 0) {
				temp = X2;
				X2 = X1;
				X1 = temp;
			}
			sprintf(string,
				"%s:\t  %s %s %s %s\n\t\t|  %s %s\n\t\t;\n",
				Lp,Lp,X1,X2,A2,Y1,A1);
			break;
		case RIGHTREC:
			sprintf(string,
				"%s:\t  %s %s %s %s\n\t\t|  %s %s\n\t\t;\n",
				Lp,X1,X2,Lp,A2,Y1,A1);
			break;
		case NONREC:
			sprintf(string,"%s:\t%s %s\n\t\t|  %s\n\t\t;\n",
				Lp,X1,A2,A1);
			break;
		case NONALT:
			sprintf(string,"%s:\t  %s\n\t\t;\n", Lp, X1);
			break;
	}
	if(strcmp(Lp, "........") != 0) {
		printf("%s", rp);
		return("");
	}
	while(++i <= count)
		if (match(rules[i], rp)) return (names[i]);
	n = temp2 = new_NT(typ);
	temp1 = rp;
	while(*temp1++ != '\0')
		if(strncmp(temp1 - 1, Lp, 8) == 0) strncpy(temp1 - 1, temp2, 8);
		else temp2 = n;
	if(count < MAXRULES) {
		rules[count] = rp;
		names[count] = n;
	} else printf("\nError: Exceeded limit on new NTs!\n");
	printf("%s", rp);
	return(n);
}

#define GRAM 0
#define ACT  1
#define ALT  2

#define BUFSIZE 128
char buf[BUFSIZE];
int bufp = 0;

isletter(c)
char c;
{
	return((c>='A' && c<='Z' || c>='a' && c<='z' || c=='_') ? 1 : 0);
}

isdigit(c)
char c;
{
	return((c>='0' && c<='9') ? 1 : 0);
}

isspace(c)
char c;
{
	return((c==' ' || c=='\t' || c=='\n') ? 1 : 0);
}

getch()
{
	return((bufp > 0) ? buf[--bufp] : getchar());
}

ungetch(c)
char c;
{
	if(bufp > BUFSIZE)
		printf("ungetch: too many characters!\n");
	else
		buf[bufp++] = c;
}

#define ret(token) { buffer[i] = '\0'; \
			yylval.cval = malloc(strlen(buffer) + 1); \
			strcpy(yylval.cval, buffer); \
			return(token); }

next(s)
char *s;
{
	int i = 0;
	char c[128];
	
	while(isspace(c[0] = getch()));
	ungetch(c[0]);
	while(*s != '\0') if((c[i++] = getch()) != *s) break; else s++;
	while(i > 0) ungetch(c[--i]);
	return((*s == '\0') ? 1 : 0);
}

next_text(s)
char *s;
{
	while(isspace(getbuf));
	ungetch(buffer[--i]);
	return(next(s));
}

read_substitute()
{
	char c;

	getbuf;
	if(next("<")) {
		getbuf;
		while(getbuf != '>');
	}
	if(next("$")) {
		getbuf;
		return;
	}
	if(getbuf != '-') ungetch(buffer[--i]);
	if(!isdigit(getbuf)) yyerror("Digit expected in $d\n");
	else {
		while((buffer[i] = getch()) == '.') {
			if(isdigit(buffer[i] = getch())) {
				buffer[i - 1] = buffer[i];
				continue;
			} else {
				ungetch(buffer[i]);
				buffer[i] = '.';
				break;
			}
		}
		ungetch(buffer[i]);
	}
}

read_string()
{
	getbuf;
	while(getbuf != '\"')
		if(buffer[i - 1] == '\\') getbuf;
}

read_lit()
{
	getbuf;
	while(getbuf != '\'')
		if(buffer[i - 1] == '\\') getbuf;
}

read_comm()
{
	getbuf;
	getbuf;
	while(!next_text("*/")) getbuf;
	getbuf;
	getbuf;
}

yylex()
{
	static state = GRAM;
	static level = 0;
	static alt_level = 0;
	char ch;

	if((ch = getch()) == EOF) return(EOF); else ungetch(ch);

	switch (state) {
		case GRAM:
			while(1) {
			i = 0;
			if(next("%start")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(START)
			} else
			if(next("%token")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(TOKEN)
			} else
			if(next("%left")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(LEFT)
			} else
			if(next("%right")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(RIGHT)
			} else
			if(next("%nonassoc")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(NONASSOC)
			} else
			if(next("%prec")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(PREC)
			} else
			if(next("%type")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(TYPE)
			} else
			if(next("%term")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(TERM)
			} else
			if(next("%union")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(UNION)
			} else
			if(next("%recur")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(RECUR)
			} else
			if(next("left")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(LEFTR)
			} else
			if(next("right")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(RIGHTR)
			} else
			if(next("list")) {
				getbuf;
				getbuf;
				getbuf;
				getbuf;
				ret(LIST)
			} else
			if(next("%%")) {
				getbuf;
				getbuf;
				ret(MARK)
			} else
			if(next("%}")) {
				getbuf;
				getbuf;
				ret(RCURL)
			} else
			if(next("}")) {
				getbuf;
				ret('}')
			} else
			if(next("]")) {
				getbuf;
				ret(']')
			} else
			if(next("/*")) {
				read_comm();
				/* ret(COMM) */
				continue;
			} else
			if(next("\'")) {
				read_lit();
				ret(IDENTIFIER)
			} else
			if(next("[") || next("{") || next("%{")) {
				if(next("[")) state = ALT; else state = ACT;
				if(!next("%{")) {
					getbuf;
					ret(buffer[i - 1])
				} else {
					getbuf;
					getbuf;
					ret(LCURL)
				}
			} else
			if(isletter(getbuf)) {
				while(isletter(getbuf) ||
					isdigit(buffer[i - 1]));
				ungetch(buffer[--i]);
				if(next(":")) {
					getbuf;
					ret(C_IDENTIFIER)
				} else ret(IDENTIFIER)
			} else
			if(isdigit(buffer[i - 1])) {
				while(isdigit(getbuf));
				ungetch(buffer[--i]);
				ret(NUMBER)
			} else ret(buffer[i - 1])
			}

		case ALT: /* fall through to the ACT case */ ;
		case ACT:
			i = 0; alt_level = level = 0;
			while(1) {
				while(isspace(getbuf));
				ungetch(buffer[--i]);
				if(next("]") || next("}") || next("%}"))
					if(next("]") ?
					   --alt_level < 0 && state == ALT :
					   --level < 0 && state == ACT) {
						state = GRAM;
						ret(CODE)
					} else
					if(!next("%}")) getbuf;
					else printf("\nError: nested %%}\n");
				else
				if(next("$")) {
					read_substitute();
					continue;
				} else
				if(next("/*")) {
					read_comm();
					continue;
				} else
				if(next("\'")) {
					read_lit();
					continue;
				} else
				if(next("\"")) {
					read_string();
					continue;
				} else
				if(next("[")) {
					alt_level++;
					getbuf;
				} else
				if(next("{")) {
					level++;
					getbuf;
				} else getbuf;
				continue;
			}
						
	}
}
__YYSCLASS yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 65
# define YYLAST 250
__YYSCLASS yytabelem yyact[]={

     6,    29,    27,    11,    12,    13,    10,    53,    14,    15,
     5,     7,     3,     8,    51,    67,    70,   101,    36,    18,
    19,    87,    88,    84,    85,    96,    78,    61,    51,    28,
    22,    41,    53,    68,    40,    69,    65,    27,    83,    59,
    32,    33,    17,    93,    91,    77,    56,    38,    62,    21,
    51,    49,   104,    94,   102,    46,    95,    80,    79,    44,
    42,    24,    31,    45,    66,    26,    48,    64,    63,    74,
    37,     4,    34,    25,    16,     2,    50,    20,    52,    72,
    30,    23,     9,    43,     1,     0,     0,     0,     0,     0,
    50,    35,     0,    39,     0,     0,     0,     0,    58,     0,
     0,     0,     0,    55,     0,     0,     0,     0,     0,    73,
     0,    57,    50,    60,     0,     0,     0,     0,     0,     0,
     0,     0,    75,    81,    82,    58,    76,    90,     0,     0,
     0,     0,     0,     0,    92,    97,    98,     0,     0,     0,
     0,     0,    99,     0,     0,     0,   100,     0,     0,     0,
     0,     0,    58,   103,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    47,     0,    54,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    71,     0,    47,     0,     0,     0,
    89,     0,    86,     0,     0,     0,     0,    32,     0,    54 };
__YYSCLASS yytabelem yypact[]={

 -3000, -3000,  -262, -3000, -3000,  -215,  -244,   -74,  -230,     1,
 -3000, -3000, -3000, -3000, -3000, -3000,  -221, -3000, -3000, -3000,
 -3000,  -231,  -275,  -217,  -216,  -256, -3000, -3000,   -78, -3000,
   -10, -3000,  -228,    -2, -3000, -3000, -3000,   -33, -3000, -3000,
  -217, -3000, -3000,   -13, -3000, -3000,    -8,  -218, -3000,   -47,
  -233,   -75,   -27,    -8, -3000, -3000, -3000, -3000,   -47,   -11,
 -3000,   -80,  -234,   -34,   -34,   -34,  -219, -3000,   -19,   -21,
 -3000, -3000,     3,    -8,   -33, -3000, -3000, -3000,   -82,   -35,
  -235,   -35,   -35,   -34, -3000, -3000, -3000, -3000, -3000, -3000,
    -8, -3000, -3000, -3000, -3000,  -243,   -39, -3000, -3000,   -35,
    -8,   -41, -3000, -3000, -3000 };
__YYSCLASS yytabelem yypgo[]={

     0,    84,    83,    82,    81,    80,    62,    59,    79,    55,
    51,    78,    77,    58,    53,    66,    63,    75,    74,    73,
    72,    71,    65,    70,    69,    68,    67,    64 };
__YYSCLASS yytabelem yyr1[]={

     0,    18,     1,    20,    20,    17,    17,    21,    21,    21,
    21,    21,    21,     3,     3,     3,     3,     3,     3,     4,
     4,     5,     5,     5,     6,     6,    19,    19,    23,    22,
    24,     2,     2,     7,     7,     8,     8,     9,     9,    10,
    10,    10,    10,    11,    11,    11,    25,    25,    25,    26,
    26,    26,    27,    27,    27,    13,    13,    14,    14,    15,
    15,    15,    12,    16,    16 };
__YYSCLASS yytabelem yyr2[]={

     0,     1,    11,     3,     0,     1,     4,     5,     5,     5,
     5,     7,     7,     2,     2,     2,     2,     2,     2,     1,
     7,     2,     5,     7,     3,     5,     2,     4,     1,     9,
     1,     9,     2,     2,     2,     7,     2,     7,     5,     9,
     9,     9,     2,    11,     7,     3,     3,     5,     5,     3,
     5,     5,     3,     5,     5,     1,     7,     1,     7,     1,
     7,     9,     7,     7,     2 };
__YYSCLASS yytabelem yychk[]={

 -3000,    -1,   -17,   274,   -21,   272,   262,   273,   275,    -3,
   268,   265,   266,   267,   270,   271,   -18,   257,   263,   264,
   -12,   123,   260,    -4,    60,   -19,   -22,   258,   260,   276,
    -5,    -6,   257,   257,   -20,   -22,   274,   -23,   125,    -6,
    44,   259,    62,    -2,    -7,   -16,    -9,   269,   -15,   -10,
   123,    61,   -11,    40,   257,    -6,    59,   124,   -10,   257,
   -16,   260,   123,   -25,   -26,    63,   -27,    42,    60,    62,
    43,   261,    -8,    -9,   -24,   -16,   -15,   125,   260,   -13,
    91,   -13,   -13,   257,    42,    43,   261,    42,    43,   261,
   124,    41,    -7,   125,   -14,    91,   260,   -14,   -14,   -13,
    -9,   260,    93,   -14,    93 };
__YYSCLASS yytabelem yydef[]={

     5,    -2,     0,     1,     6,     0,     0,     0,     0,    19,
    13,    14,    15,    16,    17,    18,     0,     7,     8,     9,
    10,     0,     0,     0,     0,     4,    26,    28,     0,    11,
    12,    21,    24,     0,     2,    27,     3,    59,    62,    22,
     0,    25,    20,     0,    32,    33,    34,     0,    64,    59,
     0,     0,    42,     0,    45,    23,    29,    30,    59,    59,
    38,     0,     0,    55,    55,    55,     0,    46,     0,     0,
    49,    52,     0,    36,    59,    37,    63,    60,     0,    57,
     0,    57,    57,    55,    47,    50,    53,    48,    51,    54,
     0,    44,    31,    61,    39,     0,     0,    40,    41,    57,
    35,     0,    56,    43,    58 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

__YYSCLASS yytoktype yytoks[] =
{
	"IDENTIFIER",	257,
	"C_IDENTIFIER",	258,
	"NUMBER",	259,
	"CODE",	260,
	"LIST",	261,
	"RECUR",	262,
	"LEFTR",	263,
	"RIGHTR",	264,
	"LEFT",	265,
	"RIGHT",	266,
	"NONASSOC",	267,
	"TOKEN",	268,
	"PREC",	269,
	"TYPE",	270,
	"TERM",	271,
	"START",	272,
	"UNION",	273,
	"MARK",	274,
	"LCURL",	275,
	"RCURL",	276,
	"|",	124,
	"-unknown-",	-1	/* ends search */
};

__YYSCLASS char * yyreds[] =
{
	"-no such reduction-",
	"spec : defs MARK",
	"spec : defs MARK rules tail",
	"tail : MARK",
	"tail : /* empty */",
	"defs : /* empty */",
	"defs : defs def",
	"def : START IDENTIFIER",
	"def : RECUR LEFTR",
	"def : RECUR RIGHTR",
	"def : UNION act",
	"def : LCURL CODE RCURL",
	"def : rword tag nlist",
	"rword : TOKEN",
	"rword : LEFT",
	"rword : RIGHT",
	"rword : NONASSOC",
	"rword : TYPE",
	"rword : TERM",
	"tag : /* empty */",
	"tag : '<' IDENTIFIER '>'",
	"nlist : nmno",
	"nlist : nlist nmno",
	"nlist : nlist ',' nmno",
	"nmno : IDENTIFIER",
	"nmno : IDENTIFIER NUMBER",
	"rules : rule",
	"rules : rules rule",
	"rule : C_IDENTIFIER",
	"rule : C_IDENTIFIER alt ';'",
	"alt : alt '|'",
	"alt : alt '|' rbody",
	"alt : rbody",
	"rbody : prec",
	"rbody : T",
	"E : E '|' T",
	"E : T",
	"T : T F prec",
	"T : F prec",
	"F : P star act1 act2",
	"F : P plus act1 act2",
	"F : P '?' act1 act2",
	"F : P",
	"P : P list IDENTIFIER act1 act2",
	"P : '(' E ')'",
	"P : IDENTIFIER",
	"star : '*'",
	"star : '<' '*'",
	"star : '>' '*'",
	"plus : '+'",
	"plus : '<' '+'",
	"plus : '>' '+'",
	"list : LIST",
	"list : '<' LIST",
	"list : '>' LIST",
	"act1 : /* empty */",
	"act1 : '[' CODE ']'",
	"act2 : /* empty */",
	"act2 : '[' CODE ']'",
	"act3 : /* empty */",
	"act3 : '{' CODE '}'",
	"act3 : '=' '{' CODE '}'",
	"act : '{' CODE '}'",
	"prec : PREC IDENTIFIER act3",
	"prec : act3",
};
#endif /* YYDEBUG */
#define YYFLAG  (-3000)
/* @(#) $Revision: 70.7 $ */

/*
** Skeleton parser driver for yacc output
*/

#if defined(NLS) && !defined(NL_SETN)
#include <msgbuf.h>
#endif

#ifndef nl_msg
#define nl_msg(i,s) (s)
#endif

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab

#ifndef __RUNTIME_YYMAXDEPTH
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#else
#define YYACCEPT	{free_stacks(); return(0);}
#define YYABORT		{free_stacks(); return(1);}
#endif

#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( (nl_msg(30001,"syntax error - cannot backup")) );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/

#ifdef YYREENTRANT
__thread int yydebug;		/* set to 1 to get debugging */
#else
int yydebug;			/* set to 1 to get debugging */
#endif

/*
** driver internal defines
*/
/* define for YYFLAG now generated by yacc program. */
/*#define YYFLAG		(FLAGVAL)*/

/*
** global variables used by the parser
*/
# ifndef __RUNTIME_YYMAXDEPTH

#   ifdef YYREENTRANT
__thread __YYSCLASS YYSTYPE yyv[ YYMAXDEPTH ];  /* value stack */
__thread __YYSCLASS int yys[ YYMAXDEPTH ];      /* state stack */
#   else
__YYSCLASS YYSTYPE yyv[ YYMAXDEPTH ];           /* value stack */
__YYSCLASS int yys[ YYMAXDEPTH ];               /* state stack */
#   endif

# else

#  ifdef YYREENTRANT
__thread __YYSCLASS YYSTYPE *yyv;               /* pointer to malloc'ed stack st
value stack */
__thread __YYSCLASS int *yys;                   /* pointer to malloc'ed stack st
ack */
#  else
__YYSCLASS YYSTYPE *yyv;                        /* pointer to malloc'ed value st
ack */
__YYSCLASS int *yys;                            /* pointer to malloc'ed stack stack */
#  endif

#if defined(__STDC__) || defined (__cplusplus)
#include <stdlib.h>
#else
	extern char *malloc();
	extern char *realloc();
	extern void free();
#endif /* __STDC__ or __cplusplus */


static int allocate_stacks();
static void free_stacks();
# ifndef YYINCREMENT
# define YYINCREMENT (YYMAXDEPTH/2) + 10
# endif
# endif	/* __RUNTIME_YYMAXDEPTH */
long  yymaxdepth = YYMAXDEPTH;


#ifdef YYREENTRANT

__thread __YYSCLASS YYSTYPE *yypv;      /* top of value stack */
__thread __YYSCLASS int *yyps;          /* top of state stack */

__thread __YYSCLASS int yystate;        /* current state */
__thread __YYSCLASS int yytmp;          /* extra var (lasts between blocks) */

__thread int yynerrs;                   /* number of errors */
__thread __YYSCLASS int yyerrflag;      /* error recovery flag */
__thread int yychar;                    /* current input token number */

__thread extern int yyinit_key;           /* init TLS data once */
extern int TLS_INIT;

#else

__YYSCLASS YYSTYPE *yypv;               /* top of value stack */
__YYSCLASS int *yyps;                   /* top of state stack */

__YYSCLASS int yystate;                 /* current state */
__YYSCLASS int yytmp;                   /* extra var (lasts between blocks) */

int yynerrs;                            /* number of errors */
__YYSCLASS int yyerrflag;               /* error recovery flag */
int yychar;
#endif


/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
l*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */


        /*
        ** Initialize externals - yyparse may be called more than once
        */

#ifdef YYREENTRANT
        if (yyinit_key != TLS_INIT)
        {
            yyinit_key = TLS_INIT;
            yyinit_tls();
        }
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
# ifdef __RUNTIME_YYMAXDEPTH
	if (allocate_stacks()) YYABORT;
# endif
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
# ifndef __RUNTIME_YYMAXDEPTH
			yyerror( (nl_msg(30002,"yacc stack overflow")) );
			YYABORT;
# else
			/* save old stack bases to recalculate pointers */
			YYSTYPE * yyv_old = yyv;
			int * yys_old = yys;
			yymaxdepth += YYINCREMENT;
			yys = (int *) realloc(yys, yymaxdepth * sizeof(int));
			yyv = (YYSTYPE *) realloc(yyv, yymaxdepth * sizeof(YYSTYPE));
			if (yys==0 || yyv==0) {
			    yyerror( (nl_msg(30002,"yacc stack overflow")) );
			    YYABORT;
			    }
			/* Reset pointers into stack */
			yy_ps = (yy_ps - yys_old) + yys;
			yyps = (yyps - yys_old) + yys;
			yy_pv = (yy_pv - yyv_old) + yyv;
			yypv = (yypv - yyv_old) + yyv;
# endif

		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( (nl_msg(30003,"syntax error")) );
				yynerrs++;
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 52 "tws.y"
{ if(start) printf("\n%%%%\n"); } break;
case 2:
# line 54 "tws.y"
{
			  fprintf(tempfile,"\n");
			  /*IST*/fclose(tempfile); } break;
case 3:
# line 60 "tws.y"
{ /* in this action, eat up the rest of the file */
			  printf("\n%s", yypvt[-0].cval);
			  while ((c = getchar()) != EOF) printf("%c", c); } break;
case 5:
# line 67 "tws.y"
{ tempfile = fopen("yytempfile","w"); } break;
case 7:
# line 72 "tws.y"
{ printf("\n%s %s", yypvt[-1].cval, yypvt[-0].cval); start = 1; } break;
case 8:
# line 74 "tws.y"
{ default_recur = recur_flag = LEFTREC; } break;
case 9:
# line 76 "tws.y"
{ default_recur = recur_flag = RIGHTREC; } break;
case 10:
# line 78 "tws.y"
{ fprintf(tempfile,"\n%s\n%s", yypvt[-1].cval, yypvt[-0].cval); } break;
case 11:
# line 80 "tws.y"
{ fprintf(tempfile,"\n%s %s \n%s", yypvt[-2].cval, yypvt[-1].cval, yypvt[-0].cval); } break;
case 12:
# line 82 "tws.y"
{ fprintf(tempfile,"\n%s %s %s", yypvt[-2].cval, yypvt[-1].cval, yypvt[-0].cval); } break;
case 19:
# line 94 "tws.y"
{ yyval.cval = ""; tag_type = NULL; } break;
case 20:
# line 96 "tws.y"
{ yyval.cval = tag_type = string = malloc(strlen(yypvt[-1].cval)+3);
			  sprintf(string,"<%s>",yypvt[-1].cval); } break;
case 22:
# line 102 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+strlen(yypvt[-0].cval)+3);
			  sprintf(string,"%s %s",yypvt[-1].cval,yypvt[-0].cval); } break;
case 23:
# line 105 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-2].cval)+strlen(yypvt[-0].cval)+3);
			  sprintf(string,"%s %s",yypvt[-2].cval,yypvt[-0].cval); } break;
case 24:
# line 110 "tws.y"
{ yyval.cval = yypvt[-0].cval; enter_type(tag_type,yypvt[-0].cval); } break;
case 25:
# line 112 "tws.y"
{ string = malloc(strlen(yypvt[-1].cval)+strlen(yypvt[-0].cval)+3);
			  enter_type(tag_type,yypvt[-1].cval);
			  sprintf(string,"%s %s",yypvt[-1].cval,yypvt[-0].cval); yyval.cval=string; } break;
case 28:
# line 122 "tws.y"
{ Lhs = yypvt[-0].cval;
			  Lhs[strlen(yypvt[-0].cval)-1] = '\0';
			  first = 1;
			  if(!start) {
			  	printf("\n%%start %s", yypvt[-0].cval);
				printf("\n%%%%\n"); start = 1; }
			} break;
case 29:
# line 130 "tws.y"
{ if(strcmp(yypvt[-1].cval,"") != 0) printf("%s:\t%s;\n", yypvt[-3].cval, yypvt[-1].cval); } break;
case 30:
# line 133 "tws.y"
{ first = 1; } break;
case 31:
# line 134 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-3].cval)+strlen(yypvt[-0].cval)+6);
			  if(strcmp(yypvt[-3].cval,"") == 0)
			  	sprintf(string,"%s",yypvt[-0].cval);
			  else if (strcmp(yypvt[-0].cval,"") == 0)
			  	sprintf(string,"%s",yypvt[-3].cval);
			  else
			  	sprintf(string,"%s\n\t|\t%s",yypvt[-3].cval,yypvt[-0].cval); } break;
case 35:
# line 149 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-2].cval)+strlen(yypvt[-0].cval)+5);
			  sprintf(string,"%s | %s",yypvt[-2].cval,yypvt[-0].cval); } break;
case 37:
# line 155 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-2].cval)+strlen(yypvt[-1].cval)+strlen(yypvt[-0].cval) + 4);
			  sprintf(string,"%s %s%s",yypvt[-2].cval,yypvt[-1].cval,yypvt[-0].cval); } break;
case 38:
# line 158 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval) + strlen(yypvt[-0].cval) + 2);
			  sprintf(string,"%s%s", yypvt[-1].cval, yypvt[-0].cval); } break;
case 39:
# line 163 "tws.y"
{ yyval.cval = emit_rule(recur_flag,Lhs,yypvt[-3].cval,"",yypvt[-0].cval,"",yypvt[-1].cval); } break;
case 40:
# line 165 "tws.y"
{ yyval.cval = emit_rule(recur_flag,Lhs,yypvt[-3].cval,"",yypvt[-0].cval,yypvt[-3].cval,yypvt[-1].cval); } break;
case 41:
# line 167 "tws.y"
{ yyval.cval = emit_rule(NONREC,Lhs,yypvt[-3].cval,"",yypvt[-0].cval,"",yypvt[-1].cval); } break;
case 43:
# line 172 "tws.y"
{ yyval.cval = emit_rule(recur_flag,Lhs,yypvt[-4].cval,yypvt[-2].cval,yypvt[-0].cval,yypvt[-4].cval,yypvt[-1].cval); } break;
case 44:
# line 174 "tws.y"
{ Lhs = "........";
			  enter_type(tag_type, yypvt[-1].cval);
			  yyval.cval = emit_rule(NONALT,Lhs,yypvt[-1].cval,"","","","",""); } break;
case 45:
# line 178 "tws.y"
{ tag_type = lookup_type(yypvt[-0].cval); } break;
case 46:
# line 182 "tws.y"
{ recur_flag = default_recur; } break;
case 47:
# line 184 "tws.y"
{ recur_flag = RIGHTREC; } break;
case 48:
# line 186 "tws.y"
{ recur_flag = LEFTREC; } break;
case 49:
# line 190 "tws.y"
{ recur_flag = default_recur; } break;
case 50:
# line 192 "tws.y"
{ recur_flag = RIGHTREC; } break;
case 51:
# line 194 "tws.y"
{ recur_flag = LEFTREC; } break;
case 52:
# line 198 "tws.y"
{ recur_flag = default_recur; } break;
case 53:
# line 200 "tws.y"
{ recur_flag = RIGHTREC; } break;
case 54:
# line 202 "tws.y"
{ recur_flag = LEFTREC; } break;
case 55:
# line 206 "tws.y"
{ yyval.cval=""; } break;
case 56:
# line 208 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+9);
			  sprintf(string,"\n\t\t\t{ %s }",yypvt[-1].cval); } break;
case 57:
# line 213 "tws.y"
{ yyval.cval=""; } break;
case 58:
# line 215 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+9);
			  sprintf(string,"\n\t\t\t{ %s }",yypvt[-1].cval); } break;
case 59:
# line 220 "tws.y"
{ yyval.cval = ""; } break;
case 60:
# line 222 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+11);
			  sprintf(string,"\n\t\t\t{ %s }\n\t",yypvt[-1].cval); } break;
case 61:
# line 225 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+13);
			  sprintf(string,"\n\t\t\t= { %s }\n\t",yypvt[-1].cval); } break;
case 62:
# line 230 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-1].cval)+5);
			  sprintf(string,"{ %s }",yypvt[-1].cval); } break;
case 63:
# line 235 "tws.y"
{ yyval.cval = string = malloc(strlen(yypvt[-2].cval)+strlen(yypvt[-1].cval)+strlen(yypvt[-0].cval)+6);
			  sprintf(string," %s %s %s",yypvt[-2].cval,yypvt[-1].cval,yypvt[-0].cval); } break;
	}
	goto yystack;		/* reset registers in driver code */
}

# ifdef __RUNTIME_YYMAXDEPTH

static int allocate_stacks() {
	/* allocate the yys and yyv stacks */
	yys = (int *) malloc(yymaxdepth * sizeof(int));
	yyv = (YYSTYPE *) malloc(yymaxdepth * sizeof(YYSTYPE));

	if (yys==0 || yyv==0) {
	   yyerror( (nl_msg(30004,"unable to allocate space for yacc stacks")) );
	   return(1);
	   }
	else return(0);

}


static void free_stacks() {
	if (yys!=0) free((char *) yys);
	if (yyv!=0) free((char *) yyv);
}

# endif  /* defined(__RUNTIME_YYMAXDEPTH) */

