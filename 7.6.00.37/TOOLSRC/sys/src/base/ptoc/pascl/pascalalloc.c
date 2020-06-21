/* @(#)pascl/pascalalloc.c

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

/* pascalalloc.c */
#include <stdlib.h>

#include <sys/types.h>
#include "../common/stab.h"

#if OLD
extern
caddr_t	sbrk ();
#endif

typedef struct {
	int symbol;
	int field;
} n_s_t;

extern n_s_t NameSpace [];

alloc_name_space (StrNdx)
	int	StrNdx;
{
	static int	name_ptr = -1;
	static int	name_limit = StabLength - 1;
	n_s_t		*new_space;

	if (name_ptr == name_limit) {
#if OLD
		new_space = (n_s_t *) sbrk ( (StabLength + 1) * sizeof (n_s_t));
		if ((int) new_space != -1) {
#else
		new_space = (n_s_t *) calloc ((StabLength+1) , sizeof(n_s_t));
		if ( new_space ) {
#endif
			/*
			 *  IST 1995-01-19
			 *  The name table 'NameSpace' will have big gaps!
			 *  Therefore we MUST NOT scan the list by increasing
			 *  indexes.
			 */
			name_ptr = (new_space - NameSpace) + 1;
			name_limit = name_ptr + StabLength - 1;
		}
		else {
			cerror ("out of memory - AllocNameSpace\n");
		}
	}
	else {
		name_ptr++;
	}
	NameSpace [name_ptr].symbol = -1;
	NameSpace [name_ptr].field = -1;
	StringSemantics (StrNdx) = name_ptr;
}
