/* @(#)pgram/yylex.c

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
#ifdef WIN32
#pragma warning (disable : 4033  ) /*RASE err. 1a: warnings.level 2*/
#endif

#include <assert.h>
#include <string.h>
#include "0.h"
#include "yy.h"
				/* EF4: 11-08-86 WB */
#include "../pcomp/tokenstrings.c"
#include "../common/opcode.i"
#include "../common/globals.ci"
#include "../common/stab.h"
#include "../pascl/globals1.ci"
#include "../pascl/pascalalloc.h"

#ifdef RASX /*RAS05 kludge: def. PushLoopScope() in lexer */
int PushLoopScope();
#endif
#ifdef RASN /*RASN4 decl. first/last const/type */
SymbolTableRange RAS_first_const, RAS_last_const,
		  RAS_first_type, RAS_last_type;
char tok_first_const [80], tok_last_const [80],	
	tok_first_type [80], tok_last_type [80];
#endif

/*
 * Scanner
 */
int	yylacnt;

#define	YYLASIZ	10

struct	yytok Yla[YYLASIZ];

unyylex(y)
	struct yytok *y;
{

	if (yylacnt == YYLASIZ)
		panic("unyylex");
	copy((char *) &Yla[yylacnt], (char *) y, sizeof Yla[0]);
	yylacnt++;

}

int	TokenValue;	/* EF4: 11-08-86 WB; global for value of token */
int KeywdBottom, KeywdTop,
    TypNmBottom, TypNmTop;

extern int preprocessed;

#ifdef RASX /*RAS03 create ebcdic_tbl*/

static unsigned char ebcdic_tbl [256] =

/*         0123456789ABCDEF */
/* x0n:*/ "                "
/* x1n:*/ "                "
/* x2n:*/ "                "
/* x3n:*/ "                "
/* x4n:*/ "            <( |"
/* x5n:*/ "&         !$*); "
/* x6n:*/ " /         ,%_>?"
/* x7n:*/ "          :#@\'=\""
/* x8n:*/ " abcdefghi {   +"
/* x9n:*/ " jklmnopqr      "
/* xAn:*/ "  stuvwxyz   [  "
/* xBn:*/ "             ] -"
/* xCn:*/ " ABCDEFGHI      "
/* xDn:*/ " JKLMNOPQR      "
/* xEn:*/ "  STUVWXYZ      "
/* xFn:*/ "0123456789      ";

static unsigned char ebcdic_tbl_inv [256];

#define ascii_or_ebcdic(c) \
( RAS_control ['E'] ? ebcdic_tbl_inv [c] : c )

invert_ebcdic_tbl ()
{
  unsigned int ebc, asc;
  memset ( ebcdic_tbl_inv, '\0', sizeof ( ebcdic_tbl_inv ) );
  ebcdic_tbl_inv [ ' ' ] = 0x40;
  for ( ebc = 0; ebc < 256; ebc ++ ) {
    if ( ( asc = ebcdic_tbl [ ebc ] ) != ' ' ) {
      if ( ebcdic_tbl_inv [ asc ] )
	Perror( & ebcdic_tbl [ ebc ], "ebcdic: dupl. code");
      ebcdic_tbl_inv [ asc ] = ebc; } }
}

#endif

initialize()
{
	int i, j, last;
	static char filearray[6] = "stdin";
#ifdef RASN /*RASN4 bld first/last const/type tokens */
	char file_pref [ 40 ], * cp;
	sprintf ( file_pref, "%2.2s%s", filename+1, filename+5 );
	cp = strchr ( file_pref, '.' );
	if ( cp ) * cp = '\0';
	sprintf ( tok_first_const, "c%s_%s", file_pref, "first_const" );
	sprintf ( tok_last_const,  "c%s_%s", file_pref, "last_const" );
	sprintf ( tok_first_type,  "t%s_%s", file_pref, "first_type" );
	sprintf ( tok_last_type,   "t%s_%s", file_pref, "last_type" );
#endif

#ifdef RASX /*RAS03 invert ebcdic_tbl*/
	invert_ebcdic_tbl ();
#endif
	preprocessed = 0;
	if(ibp == 0) {
		ibp = stdin;
		filename = filearray;
		preprocessed = 1;
	}
	if(getline() == -1) {
		Perror(filename, "No lines in file");
		pexit(NOSTART);
	}
	line = 1;
	errpfx = 'E';
	last = (sizeof tokenstrings / sizeof(char *)) - 1;
	KeywdBottom =
	EnterString(tokenstrings[0], strlen(tokenstrings[0]));
	for(i = 1; i <= last; i++) {
/**
 *              if(!(*tokenstrings[i] == '.' ||
 *              (control['s'] && strlen(extensionstrings[i])))) {
 **/
			j=EnterString(tokenstrings[i], strlen(tokenstrings[i]));
			StringSemantics(j) = 257 + i;
/**
 *              }
 **/
	}
	KeywdTop =
	EnterString(tokenstrings[last], strlen(tokenstrings[last]));
}

CheckIfSpecName(name)
	char	*name;
{
	if ( ! strcmp(name, "break")	||
	     ! strcmp(name, "continue")	||
	     ! strcmp(name, "return")	||
	     ! strcmp(name, "switch")	)
		yerror("Special name '%s' not allowed!", name);
}

yylex()
{
	register c;
	register int **ip;
	register char *cp, *cpp;
	int f;
	int ret_value;				/* EF4: 11-08-86 WB */
	char delim;

	if (yylacnt != 0) {
		yylacnt--;
		copy((char *) &Y, (char *) &Yla[yylacnt], sizeof Y);
		return (yychar);
	}
	if (c = yysavc)
		yysavc = 0;
	else
		c = readch();

next:
	/*
	 * skip white space
	 */
	while (c == ' ' || c == '\t') {
		c = readch();
	}
	yyecol = yycol;
	yyeline = yyline;
	yyefile = filename;
	yyeseqid = yyseqid;
	yyseekp = yylinpt;
	cp = token;
	yylval = yyline;
	switch (c) {
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_':
			do {
				if (opt('s') && c == '_') {	/* EF4: 11-08-86 WB */
					standard();
					yerror("character '_' is nonstandard");
				}
				*cp++ = c;
				c = readch();
			} while (alph(c) || digit(c));
			*cp = 0;
			for (cp = token, cpp = lowertoken; *cp; cp++, cpp++)
				if (*cp >= 'A' && *cp <= 'Z') {
					*cpp = *cp | ' ';
				}
				else {
					*cpp = *cp;
				}
			*cpp = 0;
			yysavc = c;
			if (!opt('y')) {
			/* 10.08.87
			 * if (opt('s')) {
			 *    yylval = EnterString(lowertoken,strlen(lowertoken));
			 * } else
			 */
			    yylval = FindString(lowertoken,strlen(lowertoken));
			    if ((yylval > TypNmTop) || (yylval < KeywdBottom)
				|| ((yylval > KeywdTop) && (yylval < TypNmBottom)))
			    {
			    	yylval = EnterString(token, strlen(token));
			    }
			} else {
			    yylval = EnterString (token, strlen (token));
			}
#ifdef RASX /*RAS05 kludge: some PushLoopScope() in lexer */
			if (   ( yylval < KeywdTop ) && ( yylval > KeywdBottom )
			    && (    ( YWHILE == StringSemantics(yylval) )
			         || ( YREPEAT == StringSemantics(yylval) ) ) )
			  PushLoopScope();
#endif
			if ((yylval < KeywdTop) && (yylval > KeywdBottom))
			    return (StringSemantics(yylval));
			AllocNameSpace(yylval);
			if (!opt('y'))
				CheckIfSpecName(token);
#ifdef RASN  /* RASN4 decl. first/last const/type */
	     /* attn! SymPtr not precise, add +2 later! */
			if ( 0 == strcmp ( token, tok_first_const ) )
			  RAS_first_const = SymPtr;
			
			if ( 0 == strcmp ( token, tok_last_const ) )
			  RAS_last_const = SymPtr;
			
			if ( 0 == strcmp ( token, tok_first_type ) )
			  RAS_first_type = SymPtr;
			
			if ( 0 == strcmp ( token, tok_last_type ) )
			  RAS_last_type = SymPtr;
#endif
			return (id);
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '$':
			yyParseNumber(c, cp);		/* EF4: 11-08-86 WB */
			yylval = TokenValue;
			return (number);
		case '"':
		case '`':
		case '#':
			if (!any(bufp + 1, c))
				goto illch;
			if (!dquote) {
				recovered();
				dquote++;
				yerror("Character/string delimiter is '");
			}
		case '\'':
			ret_value = yyScanString(c);	/* EF4: 11-08-86 WB */
			yylval = TokenValue;
			return (ret_value);
		case '.':
			c = readch();
			if (c == '.')
				return (Ydotdot);
			if (digit(c)) {
				recovered();
				yerror("Digits required before decimal point");
				*cp++ = '0';
				yyParseNumber(c, cp);	/* EF4: 11-08-86 WB */
				yylval = TokenValue;
				return (number);
			}
			yysavc = c;
			return ('.');
		case '{':
			/*
			 * { ... } comment
			 */

						  /* 10.08.87   */
			if (!opt('y')) {          /* EF4: 11-08-86 WB */
				yyscan_standard_comment();
			} else {
				c = readch();
				while (c != '}') {
					if (c <= 0)
						goto nonterm;
					if (c == '{') {
						warning();
						yyset();
						yerror("{ in a { ... } comment");
					}
					c = readch();
				}
			}
			c = readch();
			goto next;
		case '(':
			if ((c = readch()) == '*') {
				/*
				 * (* ... *) comment
				 */
						      /* 10.08.87     */
				if (!opt('y')) {      /* EF4: 11-08-86 WB */
			    	    yyscan_standard_comment();
				    c = readch();
				    goto next;
				} else {
				    c = readch();
				    for (;;) {
					if (c < 0) {
nonterm:
						yerror("Comment does not terminate - QUIT");
						pexit(ERRS);
					}
					if (c == '(' && (c = readch()) == '*') {
						warning();
						yyset();
						yerror("(* in a (* ... *) comment");
					}
					if (c == '*') {
						if ((c = readch()) != ')')
							continue;
						c = readch();
						goto next;
					}
					c = readch();
				    }
				}
			}
			yysavc = c;
			c = '(';
		case ';':
		case ',':
		case ':':
		case '=':
		case '*':
		case '+':
		case '/':
		case '-':
		case ')':
		case '[':
		case ']':
		case '<':
		case '>':
		case '^':
		case '@':				/* EF4: 03-12-86 WB */
			return (c);
		case '~':
		case '|':
		case '&':
			if ( !opt('y') ) {		/* EF4: 01-11-86 WB */
			    yyset();
			    setpfx('E');
			    yerror("%c is non-standard", c);
			}
			return c;
		default:
			switch (c) {
				case Ydotdot:
					return (c);
				case '\n':
					c = readch();
					goto next;
				case '\f':
					c = readch();
					goto next;
			}
			if (c <= 0)
				return (YEOF);
illch:
			do
				yysavc = readch();
			while (yysavc == c);
			yylval = c;
			return (Yillch);
	}
}

yyset()
{

	yyecol = yycol;
	yyeline = yyline;
	yyefile = filename;
	yyseekp = yylinpt;
}

/*
 * Setuflg trims the current
 * input line to at most 72 chars
 * for the u option.
 * EF4: 03-12-86 WB; unused here
 */
setuflg()
{

	if (charbuf[71] != '\n') {
		charbuf[72] = '\n';
		charbuf[73] = 0;
	}
}

yyscan_standard_comment ()
{
	register int c, lastc = 0;

	c = readch();
	while ((c != '}') && !(lastc == '*' && c == ')')) {
	    if (c == -1 || c == 0) {
		yerror("comment does not terminate - QUIT");
		pexit (ERRS);
	    }
	    lastc = c;
	    c = readch();
	}
}


yyScanString (delim)
char delim;
{
    register char *cp;			/* pointer to token[] */
    register int c,			/* next character */
		 cnt = -1;		/* character count beginning from 0 */
    	     int escflag;		/* '\..' will be evaluated */
	     char hexstr[3],		/* for hex strings escaped by '\' */
		  *s;

#ifdef RASX /*RAS07*/
    int embC;
    embC = ( 0 == strcmp ( token, "EmbedC" ) );
#endif

    escflag = control['e'];
    cp = token;
    c = readch();

    for (;;) {
	while (c == '\n') {		/* Newline is allowed in strings */
#ifdef RASX /*RAS07*/
	    if ( embC ) {
	      if (++cnt > StringMax)
		  goto toolong;
     	      *cp++ = ascii_or_ebcdic(c); }
#endif
	    c = readch(); }
	if (escflag && c == '\\') {	/* Evaluate '\' in strings */
	    c = readch();
	    if (hexdigit(c)) {
		s = hexstr;
		*s++ = c;
		c = readch();
		if (hexdigit(c)) {
		    *s++ = c;
		    c = readch();
		}
		*s = (char) 0;
		if (++cnt > StringMax)
		    goto toolong;
					/* Make hex string to char */
		*cp++ = (char) strtol(hexstr, 0, 16);
	    }
	    else {			/* No hex string after '\' */
		if (++cnt > StringMax)
		    goto toolong;
#ifdef RASX /*RAS03 convert to ebcdic*/
		*cp++ = ascii_or_ebcdic(c);
#else
		*cp++ = c;
#endif
		c = readch();
	    }
	    continue;
	}				     /* End if: Evaluation of '\' */

	if ((c == '\'') || (c == delim)) {   /* End of string or '' in string */
	    c = readch();
	    if (c == '\'') {		     /* '' in a string */
		if (++cnt > StringMax)
		    goto toolong;
		*cp++ = c;
		c = readch();
		continue;
	    } else
		goto str_end;
	}

	if (c == 0  || c == -1)
	    goto end_of_file;
	cnt++;
#ifdef RASX /*RAS03 convert to ebcdic*/
	*cp++ = ascii_or_ebcdic(c);
#else
	*cp++ = c;
#endif
	c = readch();
    }					     /* End for-loop */
str_end:
    yysavc = c;
    if (cnt == 0) {			/* it's a character */
	TokenValue = (int) token[0];
	TokenValue &= 0xff;		/* EG5: 26-08-87 WB */
	return (character);
    }
    if (cnt < StringMax) {		/* string or null string */
	*cp = (char) 0;
	TokenValue = EnterString (token, cnt + 1);
	return (string);
    }

toolong:				/* string lenght > StringMax */
    yerror("String constant too long");
    TokenValue = 0;
    return (string);

end_of_file:				/* EOF found */
    yysavc = c;
    yerror("End of file found before end of string");
#ifdef RAS
    pexit(ERRS); /*RASE err.13: sonst danach acc.viol. aus C-RTE*/
#endif
    TokenValue = 0;
    return (string);
}			/* End of yyScanString */
	

yyParseNumber (pc, pcp)
	int	pc;
	char 	*pcp;
{
#define	ILLEGAL		0
#define REAL		1
#define OCTAL		2
#define INT		3

    register int c,
		 len = 0;		/* length of integer */
    register char *cp;
	     char *exp_col = 0;		/* Exponent begins at exp_col */
	     int OctalOk,		/* Test, if 8 or 9 in octal number */
		 NumberKind;		/* Real or not? */
	     union {
		 struct {
		     int    i1, i2;
		 } word;
		 long l;
		 double d;
	     } kludge;			/* used to create double node */

    c = pc;
    cp = pcp;
    NumberKind = INT;
    if (cp != token)			/* No digit before decimal point */
	goto infpnumb;			/* very rude */
    if (c == '$') {
	if (opt('s')) {
	    standard();
	    yerror("Hexadecimal constants are non-standard");
	}
	c = readch();
        while (hexdigit(c)) {
	    *cp++ = c;
	    len++;
	    c = readch();
	}
	if (len == 0 || len > 8) {
	    recovered();
	    if (len == 0)
		yerror("No hex digits after $ sign");
	    else
		yerror("More than 8 digits in hex constant");
	    goto int_err;
	}
        *cp = 0;
	yysavc = c;
        TokenValue = node3 (LitOp, IntegerPtr, strtol (token, 0, 16), 0);
	return;
    }

    do {
	*cp++ = c;
	len++;
	c = readch();
    } while (digit(c));

    if (c == 'b' || c == 'B') {
    				/*
     				* nonstandard - octal constants
     				*/
	if (opt('s')) {
	    standard();
	    yerror("Octal constants are non-standard");
	}
	*cp = 0;
	OctalOk = OCTAL;
	for (cp = token; *cp != 0; cp++)
	    if (*cp == '8' || *cp == '9')
		OctalOk = ILLEGAL;
	if (len > 12 || OctalOk == ILLEGAL) {
	    recovered();
	    if (len > 12)
		yerror("More than 12 digits in octal number");
	    if (OctalOk == ILLEGAL)
		yerror("8 or 9 in octal number");
	    goto int_err;
	}
	TokenValue = node3 (LitOp, IntegerPtr, strtol (token, 0, 8), 0);
	return;
    }
    if (c == '.') {
	c = readch();
	if (c == '.') {
	    *cp = 0;
	    yysavc = Ydotdot;
	    TokenValue = node3 (LitOp, IntegerPtr, atoi (token), 0);
	    return;
	}

infpnumb:

    	NumberKind = REAL;
    	*cp++ = '.';
    	if (!digit(c)) {
		yyset();
		recovered();
		yerror("Digits required after decimal point");
		*cp++ = '0';
    	} else
		while (digit(c)) {
	    	*cp++ = c;
	    	c = readch();
		}
    }

    if (c == 'e' || c == 'E') {
	NumberKind = REAL;
	*cp++ = c;
	if ((c = yysavc) == 0)
	    c = readch();
	if (c == '+' || c == '-') {
	    *cp++ = c;
	    c = readch();
	}
	exp_col = cp;				/* Exponent begins */
	if (!digit(c)) {
	    yyset();
	    yerror("Digits required in exponent");
	    *cp++ = '0';
	} else
	    while (digit(c)) {
		*cp++ = c;
		c = readch();
	    }
    }
    *cp = 0;
    yysavc = c;
    if (NumberKind == REAL) {
	if ((exp_col) &&
		((cp - exp_col) > 10 || (atoi (exp_col) > 308))) {
	    yerror("Exponent is greater than 308");
	    TokenValue = node3 (LitOp, DoublePtr, 0, 0);
	    return;
	}
	kludge.d = atof (token);
	TokenValue = node3 (LitOp, DoublePtr, kludge.word.i1, kludge.word.i2);
	return;
    }
    if (len <= 10) {
        TokenValue = node3(LitOp, IntegerPtr, atoi(token), 0);
        return;
    }
#if __alpha
    if (len <= 19) {
	kludge.l = atol (token);
	TokenValue = node3 (LitOp, LongPtr, kludge.word.i1, kludge.word.i2);
        return;
    }
#endif
    recovered();
#if __alpha
    yerror("More than 19 digits in integer constant");
#else
    yerror("More than 10 digits in integer constant");
#endif

int_err:
    TokenValue = node3 (LitOp, IntegerPtr, 0, 0);
    return;
}
