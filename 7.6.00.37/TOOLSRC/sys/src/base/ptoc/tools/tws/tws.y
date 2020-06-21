/* @(#)tools/tws/tws.y		2.0.5  1996-09-03  SQL DBS


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
 * grammar for the input to tws
 */

%token <cval> IDENTIFIER	/* includes identifiers and literals */
%token <cval> C_IDENTIFIER	/* identifier followed by a colon */
%token <cval> NUMBER		/* [0-9]+ */
%token <cval> CODE		/* The text within an action */
%token LIST RECUR LEFTR RIGHTR
%token <cval> LEFT RIGHT NONASSOC TOKEN PREC TYPE TERM START UNION

%token <cval> MARK	/* the %% mark */
%token <cval> LCURL	/* the %{ mark */
%token <cval> RCURL    /* the %} mark */

%left '|'

%start spec
%type <cval> alt rword tag nlist nmno rbody E T F P act act1 act2 act3 prec

%union
{
	char *cval;
}

%{
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
%}

%%

spec	:	defs MARK
			{ if(start) printf("\n%%%%\n"); }
		rules tail
			{
			  fprintf(tempfile,"\n");
			  /*IST*/fclose(tempfile); }
	;

tail	:	MARK
			{ /* in this action, eat up the rest of the file */
			  printf("\n%s", $1);
			  while ((c = getchar()) != EOF) printf("%c", c); }
	|	/* empty: the second MARK is optional */
	;

defs	:	/* empty */
			{ tempfile = fopen("yytempfile","w"); }
	|	defs def
	;

def	:	START IDENTIFIER
			{ printf("\n%s %s", $1, $2); start = 1; }
	|	RECUR LEFTR
			{ default_recur = recur_flag = LEFTREC; }
	|	RECUR RIGHTR
			{ default_recur = recur_flag = RIGHTREC; }
	|	UNION act
			{ fprintf(tempfile,"\n%s\n%s", $1, $2); }
	|	LCURL CODE RCURL
			{ fprintf(tempfile,"\n%s %s \n%s", $1, $2, $3); }
	|	rword tag nlist
			{ fprintf(tempfile,"\n%s %s %s", $1, $2, $3); }
	;

rword	:	TOKEN
	|	LEFT
	|	RIGHT
	|	NONASSOC
	|	TYPE
	|	TERM
	;

tag	:	/* empty: union tag is optional */
			{ $$ = ""; tag_type = NULL; }
	|	'<' IDENTIFIER '>'
			{ $$ = tag_type = string = malloc(strlen($2)+3);
			  sprintf(string,"<%s>",$2); }
	;

nlist	:	nmno
	|	nlist nmno
			{ $$ = string = malloc(strlen($1)+strlen($2)+3);
			  sprintf(string,"%s %s",$1,$2); }
	|	nlist ',' nmno
			{ $$ = string = malloc(strlen($1)+strlen($3)+3);
			  sprintf(string,"%s %s",$1,$3); }
	;

nmno	:	IDENTIFIER	/* NOTE: literal illegal with %type */
			{ $$ = $1; enter_type(tag_type,$1); }
	|	IDENTIFIER NUMBER  /* NOTE: illegal with %type */
			{ string = malloc(strlen($1)+strlen($2)+3);
			  enter_type(tag_type,$1);
			  sprintf(string,"%s %s",$1,$2); $$=string; }
	;

rules	:	rule
	|	rules rule
	;

rule	:	C_IDENTIFIER
			{ Lhs = $1;
			  Lhs[strlen($1)-1] = '\0';
			  first = 1;
			  if(!start) {
			  	printf("\n%%start %s", $1);
				printf("\n%%%%\n"); start = 1; }
			}
		alt ';'
			{ if(strcmp($3,"") != 0) printf("%s:\t%s;\n", $1, $3); }
	;

alt	:	alt '|' { first = 1; } rbody
			{ $$ = string = malloc(strlen($1)+strlen($4)+6);
			  if(strcmp($1,"") == 0)
			  	sprintf(string,"%s",$4);
			  else if (strcmp($4,"") == 0)
			  	sprintf(string,"%s",$1);
			  else
			  	sprintf(string,"%s\n\t|\t%s",$1,$4); }
	|	rbody
	;

rbody:		prec
	|	T
	;

E	:	E '|' T
			{ $$ = string = malloc(strlen($1)+strlen($3)+5);
			  sprintf(string,"%s | %s",$1,$3); }
	|	T
	;

T	:	T F prec
			{ $$ = string = malloc(strlen($1)+strlen($2)+strlen($3) + 4);
			  sprintf(string,"%s %s%s",$1,$2,$3); }
	|	F prec
			{ $$ = string = malloc(strlen($1) + strlen($2) + 2);
			  sprintf(string,"%s%s", $1, $2); }
	;

F	:	P star act1 act2
			{ $$ = emit_rule(recur_flag,Lhs,$1,"",$4,"",$3); }
	|	P plus act1 act2
			{ $$ = emit_rule(recur_flag,Lhs,$1,"",$4,$1,$3); }
	|	P '?' act1 act2
			{ $$ = emit_rule(NONREC,Lhs,$1,"",$4,"",$3); }
	|	P
	;

P	:	P list IDENTIFIER act1 act2
			{ $$ = emit_rule(recur_flag,Lhs,$1,$3,$5,$1,$4); }
	|	'(' E ')'
			{ Lhs = "........";
			  enter_type(tag_type, $2);
			  $$ = emit_rule(NONALT,Lhs,$2,"","","","",""); }
	|	IDENTIFIER
			{ tag_type = lookup_type($1); }
	;

star	:	'*'
			{ recur_flag = default_recur; }
	|	'<' '*'
			{ recur_flag = RIGHTREC; }
	|	'>' '*'
			{ recur_flag = LEFTREC; }
	;

plus	:	'+'
			{ recur_flag = default_recur; }
	|	'<' '+'
			{ recur_flag = RIGHTREC; }
	|	'>' '+'
			{ recur_flag = LEFTREC; }
	;

list	:	LIST
			{ recur_flag = default_recur; }
	|	'<' LIST
			{ recur_flag = RIGHTREC; }
	|	'>' LIST
			{ recur_flag = LEFTREC; }
	;

act1	:	/* empty */
			{ $$=""; }
	|	'[' CODE ']'
			{ $$ = string = malloc(strlen($2)+9);
			  sprintf(string,"\n\t\t\t{ %s }",$2); }
	;

act2	:	/* empty */
			{ $$=""; }
	|	'[' CODE ']'
			{ $$ = string = malloc(strlen($2)+9);
			  sprintf(string,"\n\t\t\t{ %s }",$2); }
	;

act3	:	/* empty */
			{ $$ = ""; }
	|	'{' CODE '}'
			{ $$ = string = malloc(strlen($2)+11);
			  sprintf(string,"\n\t\t\t{ %s }\n\t",$2); }
	|	'=' '{' CODE '}'
			{ $$ = string = malloc(strlen($3)+13);
			  sprintf(string,"\n\t\t\t= { %s }\n\t",$3); }
	;

act	:	'{' CODE '}'
			{ $$ = string = malloc(strlen($2)+5);
			  sprintf(string,"{ %s }",$2); }
	;

prec	:	PREC IDENTIFIER act3
			{ $$ = string = malloc(strlen($1)+strlen($2)+strlen($3)+6);
			  sprintf(string," %s %s %s",$1,$2,$3); }
	|	act3
	;

%%

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
