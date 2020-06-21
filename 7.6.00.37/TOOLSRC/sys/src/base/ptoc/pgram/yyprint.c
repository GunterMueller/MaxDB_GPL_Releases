/* @(#)pgram/yyprint.c

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

#include "0.h"
#include "yy.h"
				/* EF4: 11-08-86 WB */
#include "../common/stab.h"

char	*tokname();

STATIC	short bounce;

/*
 * Printing representation of a
 * "character" - a lexical token
 * not in a yytok structure.
 * 'which' indicates which char * you want
 * should always be called as "charname(...,0),charname(...,1)"
 */
char *
charname(ch , which )
	int ch;
	int which;
{
	struct yytok Ych;

	Ych.Yychar = ch;
	Ych.Yylval = nullsem(ch);
	return (tokname(&Ych , which ));
}

/*
 * Printing representation of a token
 * 'which' as above.
 */
char *
tokname(tp , which )
	register struct yytok *tp;
	int	 	      which;
{
	register char *cp;
	register struct kwtab *kp;
	char	*cp2;

	cp2 = "";
	switch (tp->Yychar) {
/*  EF4: 11-08-86 WB
 *		case YCASELAB:
 *			cp = "case-label";
 *			break;
 */
		case YEOF:
			cp = "end-of-file";
			break;
		case Yillch:
			cp = "illegal character";
			break;
		case 256:
			/* error token */
			cp = "error";
			break;
		case id:			/* EF4: 11-08-86 WB */
			cp = "identifier";
			break;
/*						   EF4: 11-08-86 WB
 *		case YNUMB:
 *			cp = "real number";
 *			break;
 *		case YINT:
 *		case YBINT:
 */
		case number:
			cp = "number";
			break;
		case string:
			cp = "string";
			break;
		case character:
			cp = "character";
			break;
		case Ydotdot:
			cp = "'..'";
			break;
		default:
			if (tp->Yychar < 256) {
				static char ar[] = "'x'\0'x'\0'x'\0'x'";
				cp = ar;
				/*
				 * for four times reentrant code!
				 * used to be:
				 * if (bounce = ((bounce + 1) & 1))
				 *	cp += 4;
				 */
				bounce = ( bounce + 1 ) % 4;
				cp += (4 * bounce);	/* 'x'\0 is 4 chars */
				cp[1] = tp->Yychar;
				break;
			}
			cp = "keyword ";
							/* EF4: 11-08-86 WB */
			cp2 = tokenstrings[ tp->Yychar - 257 ];
	}
	return ( which ? cp2 : cp );
}
