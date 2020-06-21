/* @(#)pgram/yyget.c

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
#include <string.h>
#include "0.h"
#include "yy.h"
			/* EF4: 11-08-86 WB ... */
#include "../common/globals.ci"
#include "../common/opcode.i"
#include "../pascl/globals1.ci"
#include "../common/stab.h"

OpenInput (file)
	char	*file;
{
	if ((ibuf = fopen (file, "r")) == NULL) {
		perror(file); pexit(NOSTART);
	}
	ibp = ibuf;
	yyline = 0;
	CurrentFile = EnterString (file, strlen (file));
	filename = GetStringPtr(CurrentFile);
	HdgHead = LinkChain (
		HdgHead,
		special (FileOp, 0, CurrentFile, yyline));
}

/*
 * extern int preprocessed;
 *
 * ibp_init()
 * {
 *         static char filchar[6] = "stdin";
 *
 *         preprocessed = 0;
 *         if (ibp == NULL) {
 *                 ibp = stdin;
 *                 filename = filchar;
 *                 preprocessed = 1;
 *         }
 *         if(getline() == -1) {
 *                 Perror(filename, "No lines in file");
 *                 pexit(NOSTART);
 *         }
 * }
 */


/*
 * Readch returns the next
 * character from the current
 * input line or -1 on end-of-file.
 * It also maintains yycol for use in
 * printing error messages.
 */
readch()
{
	register c;
	static int eofline = 0;

	if (*bufp == '\n' && bufp >= charbuf) {
		if (eofline) return (-1);
		eofline = 0;
		if (getline() < 0)
			eofline = 1;
	}
	c = *++bufp;
	if (c == '\t')
		yycol = ((yycol + 8) & ~7);
	else
		yycol++;
	return (c);
}

/*
 * Definitions of the structures used for the
 * include facility.  The variable "ibp" points
 * to the getc buffer of the current input file.
 * There are "yyinclev + 1" current include files,
 * and information in saved in the incs stack
 * whenever a new level of include nesting occurs.
 *
 * Ibp in the incs structure saves the pointer
 * to the previous levels input buffer;
 * filename saves the previous file name;
 * Printed saves whether the previous file name
 * had been printed before this nesting occurred;
 * and yyline is the line we were on on the previous file.
 */

#define	MAXINC	10

struct inc {
	FILE	*ibp;
	char	*filename;
	int	Printed;
	int	yyline;
	int	yyLinpt;
} incs[MAXINC];

extern	char printed;

int	yyinclev	= -1;

/*
 * YyLinpt is the seek pointer to the beginning of the
 * next line in the file.
 */
int	yyLinpt;

/*
 * Getline places the next line
 * from the input stream in the
 * line buffer, returning -1 at YEOF.
 */
getline()
{
	register char *cp;
	register CHAR c;
	register FILE *ib;
	int i, savlinpt;

	if (opt('l') && yyprtd == 0)
		yyoutline();
	yyprtd = 0;
top:
	savlinpt = yylinpt;
	yylinpt = yyLinpt;
	yyline++;
	yyseqid++;
	cp = charbuf;
	ib = ibp;
	i = sizeof charbuf - 1;
	for (;;) {
		c = getc(ib);
		if (c == EOF) {
					/* EF4: 11-08-86 WB ... */
		    if (uninclud()) {	/* Create file node? */
			HdgHead = LinkChain (
					HdgHead,
					special (FileOp, 0, CurrentFile, yyline));
			goto top;
		    }
		    *cp = '\n';
		    bufp = charbuf - 1;
		    yyline--;
		    yyseqid--;
		    yylinpt = savlinpt;
		    yyprtd = 1;
		    return (-1);
		}
		*cp++ = c;
		if (c == '\n')
			break;
		if (--i == 0) {
			line = yyline;
			error("Input line too long - QUIT");
			pexit(DIED);
		}
	}
	*cp = 0;
	yyLinpt = yylinpt + cp - charbuf;
#ifdef RAS
#ifdef WIN32
	yyLinpt ++; /*RASE err.3: assume DOS-like <cr><lf> line end in file*/
#endif
#endif
						/* EF4: 11-08-86 WB */
	if (*charbuf == '#') {			/* #include-Statement ? */
	    if (includ() || cpp_include()) {
		HdgHead = LinkChain (
				HdgHead,
				special (FileOp, 0, CurrentFile, yyline));
		goto top;
	    }
	}
	if (opt('u'))
		setuflg();
	bufp = charbuf - 1;
	yycol = 8;
	return (1);
}

/*
 * Check an input line to see if it is a "#include" pseudo-statement.
 * We allow arbitrary blanks in the line and the file name
 * may be delimited by either 's or "s.  A single semicolon
 * may be placed after the name, but nothing else is allowed
 */
includ()
{
	register char *cp, *dp;
	char ch;
	register struct inc *ip;

	cp = charbuf;
	cp++;					/* EF4: 11-08-86 WB; Skip '#' */
/*
 *         if (*cp++ != '#')
 *                 return (0);
 */
	cp = skipbl(cp);
	for (dp = "include"; *dp; dp++)
		if (*dp != *cp++)
			return (0);
	line = yyline;
	cp = skipbl(cp);
	ch = *cp++;
	if (ch != '\'' && ch != '"') {
		/*
		 * This should be a yerror flagging the place
		 * but its not worth figuring out the column.
		 */
		line = yyline;
		error("Include syntax error - expected ' or \" not found - QUIT");
		pexit(DIED);
	}
	for (dp = cp; *dp != ch; dp++)
		if (*dp == 0) {
			line = yyline;
			error("Missing closing %c for include file name - QUIT",  ch);
			pexit(DIED);
		}
	*dp++ = 0;
/*
 *	if (*dp == ';')
 *		dp++;
 *	dp = skipbl(dp);
 *	if (*dp != '\n') {
 *		line = yyline;
 *		error("Garbage after filename in include");
 *		pexit(DIED);
 *	}
 *      if (!dotted(cp, 'i') && !dotted(cp, 'h')) {
 *              line = yyline;
 *              error("Include filename must end in .i or .h");
 *      }
 */
	yyinclev++;
	if (yyinclev > MAXINC) {
		line = yyline;
		error("Absurdly deep include nesting - QUIT");
		pexit(DIED);
	}
	ip = &incs[yyinclev];
	ip->filename = filename;
						/* EF4: 11-08-86 WB */
	CurrentFile = EnterString(cp, strlen(cp));
	filename = GetStringPtr(CurrentFile);

/*
 *	left over from before stdio
 *
 *	cp = malloc(518);
 *	if (cp == -1) {
 *		error("Ran out of memory (include)");
 *		pexit(DIED);
 *	}
 *
 */
	ip->ibp = ibp;
	if ( ( ibp = fopen(filename, "r" ) ) == NULL ) {
		perror(filename);
		pexit(DIED);
	}
	ip->Printed = printed;
	printed = 0;
	ip->yyline = yyline;
	yyline = 0;
	ip->yyLinpt = yyLinpt;
	yyLinpt = 0;
	return (1);
}

/* EF4: 11-08-86 WB
 * Check an input line to see if it is a
 * cpp style "#include" pseudo-statement.
 */


int
cpp_include()
{
    register char *cp, *lp, *dp;
	     char ch;

    cp = charbuf;
    cp++;				/* Skip '#' */
    cp = skipbl (cp);
    ch = *cp;

    if (!digit(ch))	return(0);

    lp = cp;
    while (digit(ch))	ch = *cp++;
    line = atoi(lp) - 1;
    cp = skipbl(cp);

    if (*cp++ != '"')	return(0);

    for (dp =cp; *dp != '"'; dp++) {
	if (*dp == 0)
	    return(0);
    }
    *dp = 0;
    CurrentFile = EnterString (cp, strlen(cp));
    filename = GetStringPtr(CurrentFile);
    yyline = atoi(lp) - 1;
    return(1);
}


char *
skipbl(ocp)
	char *ocp;
{
	register char *cp;

	cp = ocp;
	while (*cp == ' ' || *cp == '\t')
		cp++;
	return (cp);
}


/*
 * At the end of an include,
 * close the file, free the input buffer,
 * and restore the environment before
 * the "push", including the value of
 * the z option for pxp and the l option for pi.
 */
uninclud()
{
	register struct inc *ip;

	if (yyinclev < 0)
		return (0);
/*
 *	left over from before stdio: becomes fclose ( ibp )
 *
 *	(void) close(ibp[0]);
 *	free(ibp);
 *
 */
	(void) fclose ( ibp );
	ip = &incs[yyinclev];
	ibp = ip->ibp;
	yyline = ip->yyline;
	filename = ip->filename;

	yyLinpt = ip->yyLinpt;
	/*
	 * If we printed out the nested name,
	 * then we should print all covered names again.
	 * If we didn't print out the nested name
	 * we print the uncovered name only if it
	 * has not been printed before (unstack).
	 */
	if (printed) {
		printed = 0;
		while (ip >= incs) {
			ip->Printed = 0;
			ip--;
		}
	} else
		printed = ip->Printed;
	yyinclev--;
	return (1);
}
