/* @(#)pgram/yyparse.c

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
#pragma warning (disable : 4033) /*RASE err. 1a: warnings.level 2*/
#endif

#include "0.h"
#include "yy.h"
#include "../common/globals.ci"
#include "../pascl/globals1.ci"

/*
 * Parser for 'yacc' output.
 * Specifially Modified for Berkeley Pascal
 */

int	yystate;	/* Current parser state */

unsigned yytshifts = 1;	/* Number of "true" shifts */

/*
 * Parse Tables
 */
extern int	yygo[];
extern int	yypgo[];
extern int	yyr1[];
extern int	yyr2[];
extern int	yyact[];
extern int	yypact[];

					/* EF4: 11-08-86 WB */
#define push_value\
	++sp,\
	val[sp] = yylval,\
	ColNo[sp] = yyecol,\
	CardCount = CardNum[sp] = yyeline

				/* EF4: 11-08-86 WB; parser stacks index */
int	sp = -1;

/*
 * Parse and parallel semantic stack
 */
int	yys[MAXDEPTH];

/*
 * This routine parses the input stream, and
 * returns if it accepts, or if an unrecoverable syntax
 * error is encountered.
 */
yyparse()
{
	register int *ps, n, *p;
	int paniced, *panicps, idfail;

	yystate = 0;
	yychar = yylex();
	OY.Yychar = -1;
	yyshifts = 3;
	paniced = 0;
	ps = &yys[0]-1;

stack:
	/*
	 * Push new state and value.
	 */
	if (ps >= &yys[MAXDEPTH-1]) {			/* EF4: 11-08-86 WB */
		yerror("Parse stack overflow");
		pexit(DIED);
	}
	*++ps = yystate;
newstate:
	/*
	 * Locate parsing actions for the
	 * new parser state.
	 */
	p = &yyact[ yypact[yystate+1] ];
	/*
	 * Search the parse actions table
	 * for something useful to do.
	 * While n is non-positive, it is the negation
	 * of the token we are testing for.
	 */
	while ((n = *p++) <= 0)
		if ((n += yychar) != 0)
			p++;

	switch (n >> 12) {

		/*
		 * Shift.
		 */
		case 2:
			OYcopy();
			yystate = n & 07777;
			push_value;			/* EF4: 11-08-86 WB */
			yychar = yylex();
			yyshifts++;
			yytshifts++;
			goto stack;

		/*
		 * Reduce.
		 */
		case 3:
			n &= 07777;
			N_redu = yyr2[n];
							/* EF4: 11-08-86 WB */
			if (N_redu == 1 && OY.Yychar == id && !yyEactr(n,
							val[sp])) {
				idfail = 1;
				goto errin;
			}
			OY.Yychar = -1;
			ps -= N_redu;
			sp -= N_redu;                /* EF4: 11-08-86 WB */
			mp = sp + 1;
			yyactr(n);
			sp++;
			/*
			 * Use goto table to find next state.
			 */
			p = &yygo[yypgo[yyr1[n]]];
			while (*p != *ps && *p >= 0)
				p += 2;
			yystate = p[1];
			goto stack;

		/*
		 * Accept.
		 */
		case 4:
			if (opt('l')) {
			    yysync();
			}
			return;

		/*
		 * Error.
		 */
		case 1:
			idfail = 0;
errin:
			if ((paniced || yyshifts != 0) && yyrecover(ps, idfail)) {
				paniced = 0;
				ps = Ps;
				yystate = *ps;
				goto newstate;
			}
			/*
			 * Find a state where 'error' is a
			 * legal shift action.
			 */
			if (paniced && yyshifts <= 0 && ps >= panicps) {
				sp -= (ps - panicps) + 1;	/* EF4: 11-08-86 WB */
				ps = panicps - 1;
			}
			while (ps >= yys) {
				for (p = &yyact[ yypact[*ps+1] ] ; *p <= 0; p += 2)
					if (*p == -256) {
						panicps = ps;
						yystate= p[1] & 07777;
						yyOshifts = yyshifts;
						yyshifts = 0;
						paniced = 1;
						push_value;	/* EF4: 11-08-86 WB */
						goto stack;
					}
				--ps;
				--sp;			/* EF4: 11-08-86 WB */
				OY.Yychar = -1;
			}
			if (yychar == YEOF)
				yyunexeof();
			if (yystate == 1)
				yyexeof();
			yerror("Unrecoverable syntax error - QUIT");
			pexit(ERRS);
			return;
	}
	panic("yyparse");
}
