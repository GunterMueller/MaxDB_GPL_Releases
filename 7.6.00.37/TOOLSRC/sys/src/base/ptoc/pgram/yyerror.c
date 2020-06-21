/* @(#)pgram/yyerror.h

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

#include "0.h"
#include "yy.h"
#include "../common/globals.ci"

/*
 * Yerror prints an error
 * message and then returns
 * NIL for the tree if needed.
 * The error is flagged on the
 * current line which is printed
 * if the listing is turned off.
 */
/*VARARGS*/
yerror(s, a1, a2, a3, a4, a5)
	char *s;
	char *a1, *a2, *a3, *a4, *a5;
{
	register int i;
	static yySerrs;

	if (errpfx == 'w' && opt('w') != 0) {
		errpfx = 'E';
		return;
	}
	yyResume = 0;
	yysync();
	pchr(errpfx);
	pchr(' ');
	for (i = 3; i < yyecol; i++)
		pchr('-');
	printf("^--- ");
/*
 *      if (yyecol > 60)
 *              printf("\n\t");
 */
	printf(s, a1, a2, a3, a4, a5);
	pchr('\n');
	if (errpfx != 's' && errpfx != 'w') {
		ErrorCount++;
		if (errpfx != 'e') {
			yySerrs++;
		}
	}
	errpfx = 'E';
	if (yySerrs >= MAXSYNERR) {
		yySerrs = 0;
		yerror("Too many syntax errors - QUIT");
		pexit(ERRS);
	}
}

/*
 * A bracketing error message
 */
brerror(where, what)
	int where;
	char *what;
{

	if (where == 0) {
		line = yyeline;
		setpfx(' ');
		error("End matched %s on line %d", what, where);
		return;
	}
	if (where < 0)
		where = -where;
	yerror("Inserted keyword end matching %s on line %d", what, where);
}
