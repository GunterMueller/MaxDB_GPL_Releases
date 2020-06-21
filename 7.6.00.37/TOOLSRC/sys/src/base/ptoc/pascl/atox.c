/* @(#)pascl/atox.c

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

/*LINTLIBRARY*/
#include <ctype.h>

#if ! OSF1
#define DIGIT(x) (isdigit(x)? ((x)-'0'): (10+x-(islower(x)?'a':'A')))
#define MBASE 36

long
strtol(str, ptr, base)
char *str, **ptr;
int base;
{
	long val;
	int xx, sign;

	val = 0L;
	sign = 1;
	if(base < 0 || base > MBASE)
		goto OUT;
	while(isspace(*str))
		++str;
	if(*str == '-') {
		++str;
		sign = -1;
	} else if(*str == '+')
		++str;
	if(base == 0) {
		if(*str == '0') {
			++str;
			if(*str == 'x' || *str == 'X') {
				++str;
				base = 16;
			} else
				base = 8;
		} else
			base = 10;
	} else if(base == 16)
		if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
			str += 2;
	/*
	 * for any base > 10, the digits incrementally following
	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 */
	while(isalnum(*str) && (xx=DIGIT(*str)) < base) {
		/* accumulate neg avoids surprises near maxint */
		val = base*val - xx;
		++str;
	}
OUT:
	if(ptr != (char**)0)
		*ptr = str;
	return(sign*(-val));
}
#endif
