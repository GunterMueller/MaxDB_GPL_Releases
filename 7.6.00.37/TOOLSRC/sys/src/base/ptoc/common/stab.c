/* @(#)common/stab.c

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
#include <stdlib.h>
#include "../common/stab.h"
#define HashSize 256
#define StrSize(len) (((len + 4) / 4) + StringCharactersOffset)

int	StrTable [StabLength];
int	StrHash [HashSize];
int	StrTableLen;
int	StrNo;


StabI ()
{
	int	i;
	char	*p;

	/* add null string to string table */
	StrTableLen = StrSize (0);
	StrTable [StringNextOffset] = -1;
	StrHash [0] = 0;
	StrTable [StringLenOffset] = 0;
	StrTable [StringNoOffset] = 0;
	StrNo = 0;
	StrTable [StringSemanticsOffset] = -1;
	p = (char *) (&StrTable [StringCharactersOffset]);
	*p = 0;

	/* initialize rest of hash table */
	for (i = 1; i < HashSize; i++) {
		StrHash [i] = -1;
	}
}


int
HashCode (strn, len)
	unsigned char	*strn;
	int	len;
{
	if (len == 0) {
		return 0;
	}
	else {
		return (strn [0] + (strn [len - 1] * 2) + len) % HashSize;
	}
}


int
EnterString (strn, len)
	char	*strn;
	int	len;
{
	int	idx;
	int	s;
	int	i;
	char	*p;

	idx = HashCode (strn, len);
	s = StrHash [idx];
	while (s != -1) {
		if (len == GetStringLength (s)) {
			for (i = 0, p = GetStringPtr (s); i < len; i++, p++) {
				if (strn [i] != *p) goto next;
			}
			return s;
		}
		next:
		s = StrTable [s + StringNextOffset];
	}
	if (StringSpace (StrTable, &StrTableLen, StrSize (len))) {
		s = StrTableLen;
		StrTableLen += StrSize (len);
		StrTable [s + StringNextOffset] = StrHash [idx];
		StrHash [idx] = s;
		StrTable [s + StringLenOffset] = len;
		StrTable [s + StringNoOffset] = ++StrNo;
		StrTable [s + StringSemanticsOffset] = -1;
		p = (char *) (&StrTable [s + StringCharactersOffset]);
		while (len--) {
			*p++ = *strn++;
		}
		*p = 0;
		return s;
	}
	else {
		cerror ("string space exceeded\n");
	}
}



int
FindString (strn, len)
	char	*strn;
	int	len;
{
	int	idx;
	int	s;
	int	i;
	char	*p;

	idx = HashCode (strn, len);
	s = StrHash [idx];
	while (s != -1) {
		if (len == GetStringLength (s)) {
			for (i = 0, p = GetStringPtr (s); i < len; i++, p++) {
				if (strn [i] != *p) goto next;
			}
			return s;
		}
		next:
		s = StrTable [s + StringNextOffset];
	}
	return (-1);
}



GetString (StrNdx, rv)
	int	StrNdx;
	char	*rv;
{
	int	len;
	char	*p;
	char	*q;

	len = GetStringLength (StrNdx);
	p = rv;
	q = GetStringPtr (StrNdx);
	while (len--) {
		*p++ = *q++;
	}
	*p = 0;
}


char *
_GetStringPtr (StrNdx)
	int StrNdx;
{
	return GetStringPtr (StrNdx);
}


int
_GetStringLength (StrNdx)
	int	StrNdx;
{
	return GetStringLength (StrNdx);
}


int
_GetStringNo (StrNdx)
	int	StrNdx;
{
	return GetStringNo (StrNdx);
}


char
_GetStringChar (StrNdx, i)
	int	StrNdx;
	int	i;
{
	return GetStringChar (StrNdx, i);
}


char
_GetFirstChar (StrNdx)
	int	StrNdx;
{
	return GetFirstChar (StrNdx);
}


int
StringSpace (string_table, string_inx, string_size)
	int	*string_table;
	int	*string_inx;
	int	string_size;
{
	static	int	StabLimit = StabLength;
	int	*new_space;

	if (*string_inx >= StabLimit - string_size) {
#if OLD
		new_space = (int *) sbrk (StabLength * sizeof (int));
		if ((int) new_space != -1) {
#else
		new_space = (int *) calloc (StabLength , sizeof (int));
		if ( new_space ) {
#endif
			/*
			 *  IST 1995-01-19
			 *  The string space will have big gaps!
			 *  This code increases the intended index.
			 */
			*string_inx = (new_space - string_table) + 1;
			StabLimit = *string_inx + StabLength - 1;
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 1;
	}
}
