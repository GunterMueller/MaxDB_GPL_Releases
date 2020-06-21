/* @(#)pgram/yyoptions.c

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

/*
 * Options processes the option
 * strings which can appear in
 * comments and returns the next character.
 */
options()
{
	register c;
	register char *optp;

	c = readch();
	if (c != '$')
		return (c);
	do {
		c = readch();
		switch (c) {
			case 'b':
#ifdef RAS /*RASE err.1b:(char *) wg. MSVC's C4133*/
				optp = (char *) &opt( 'b' );
#else
				optp = &opt( c );
#endif
				c = readch();
				if (!digit(c))
					return (c);
				*optp = c - '0';
				c = readch();
				break;
			case 'k':
			case 'l':
			case 'n':
			case 'p':
			case 's':
			case 't':
			case 'u':
			case 'w':
			case 'z':
#ifdef RAS /*RASE err.1b:(char *) wg. MSVC's C4133*/
				optp = (char *) &opt( 'b' );
#else
				optp = &opt( c );
#endif
				c = readch();
				if (c == '+') {
					*optp = 1;
					c = readch();
				} else if (c == '-') {
					*optp = 0;
					c = readch();
				} else {
					return (c);
				}
				break;
			default:
				    return (c);
			}
	} while (c == ',');
	if ( opt( 'u' ) )
		setuflg();
	return (c);
}
