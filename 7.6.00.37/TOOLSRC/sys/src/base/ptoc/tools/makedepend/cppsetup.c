/* @(#)tools/makedepend/cppsetup.c

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
#include "def.h"

#ifdef	CPP
/*
 * This file is strictly for the sake of cpy.y and yylex.c (if
 * you indeed have the source for cpp).
 */
#define IB 1
#define SB 2
#define NB 4
#define CB 8
#define QB 16
#define WB 32
#define SALT '#'
#if pdp11 | vax | ns16000 | mc68000 | ibm032
#define COFF 128
#else
#define COFF 0
#endif
/*
 * These variables used by cpy.y and yylex.c
 */
extern char	*outp, *inp, *newp, *pend;
extern char	*ptrtab;
extern char	fastab[];
extern char	slotab[];

/*
 * cppsetup
 */
struct filepointer	*currentfile;
struct inclist		*currentinc;

cppsetup(linep, filep, inc)
	register char   *linep;
	register struct filepointer	*filep;
	register struct inclist		*inc;
{
	register char *p, savec;
	static boolean setupdone = FALSE;
	boolean	value;

	if (!setupdone) {
		cpp_varsetup();
		setupdone = TRUE;
	}

	currentfile = filep;
	currentinc = inc;
	inp = newp = linep;
	for (p=newp; *p; p++)
		;

	/*
	 * put a newline back on the end, and set up pend, etc.
	 */
	*p++ = '\n';
	savec = *p;
	*p = '\0';
	pend = p;

	ptrtab = slotab+COFF;
	*--inp = SALT;
	outp=inp;
	value = yyparse();
	*p = savec;
	return(value);
}

pperror(tag, x0,x1,x2,x3,x4)
	int	tag,x0,x1,x2,x3,x4;
{
	log_warn("\"%s\", line %d: ", currentinc->i_file, currentfile->f_line);
	log_warn(x0,x1,x2,x3,x4);
}


yyerror(s)
	register char	*s;
{
	log_fatal("Fatal error: %s\n", s);
}
#endif
/*	CPP */
