/* @(#)pgram/error.c

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
#include <stdlib.h>
#include <string.h>

#include "0.h"
#include "yy.h"
#include "../common/globals.ci"

extern	int yyline;

char errpfx = 'E';

extern int CardCount;			/* EF4: 11-08-86 WB */
/*
 * Panic is called when impossible
 * (supposedly, anyways) situations
 * are encountered.
 * Panic messages should be short
 * as they do not go to the message
 * file.
 */
panic(s)
	char *s;
{

#ifdef DEBUG
	printf("Snark (%s) line=%d, yyline=%d\n", s, line, yyline);
	abort () ;	/* die horribly */
#endif
	Perror( "Snark in pi", s);
	pexit(DIED);
}

/*
 * Error is called for
 * semantic errors and
 * prints the error and
 * a line number.
 */

extern int Column;			/* EF4: 11-08-86 WB */

/*VARARGS1*/

error(a1, a2, a3, a4, a5)
	register char *a1;
	long a2, a3, a4, a5;
{
	register int i;

	if (errpfx == 'w' && opt('w') != 0) {
		errpfx = 'E';
		return;
	}
	if (errpfx != 'w') ErrorCount++;
	line = CardCount;		/* EF4: 11-08-86 WB */
	if (line < 0)
		line = -line;

/*  	if (opt('l') && (line == yyeline))  */
	if (opt('l') && (line >= yyseqid))
		yyoutline();

	yysetfile(filename);
	yySsync();
	if (errpfx == ' ') {
		printf("  ");
		for (i = line; i >= 10; i /= 10)
			pchr( ' ' );
		printf("... ");
	}
	if (Column == -1)
		Column = strlen(charbuf);
	printf("%c ", errpfx);
	for (i = 1; i < Column - 2; i++)
		printf("-");
	printf("^--- ");
	printf(a1, a2, a3, a4, a5);
	errpfx = 'E';
	pchr( '\n' );
}
