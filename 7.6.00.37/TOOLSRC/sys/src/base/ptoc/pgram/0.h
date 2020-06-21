/* @(#)pgram/0.h

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

#define DEBUG
#define	CHAR
#define	STATIC

/* 1992-Jul-14 J.Bruehe  machine specific !  #define ADDR32  EF4: 11-08-86 WB */
#include	<stdio.h>
#include	<sys/types.h>

#ifdef FALSE	/* at least on _IBMR2 , from <sys/types.h> */
#undef FALSE
#endif
#ifdef TRUE	/* at least on _IBMR2 , from <sys/types.h> */
#undef TRUE
#endif

typedef enum {FALSE, TRUE} bool;

/*
 * Option flags
 *
 * The following options are recognized in the text of the program
 * and also on the command line:
 *
 *	b	block buffer the file output
 *
 *	i	make a listing of the procedures and functions in
 *		the following include files
 *
 *	l	make a listing of the program
 *
 *	n	place each include file on a new page with a header
 *
 *	p	disable post mortem and statement limit counting
 *
 *	t	disable run-time tests
 *
 *	u	card image mode; only first 72 chars of input count
 *
 *	w	suppress special diagnostic warnings
 *
 *	z	generate counters for an execution profile
 */
#ifdef DEBUG
bool	fulltrace, errtrace, testtrace;
extern bool yyunique;
#endif /* DEBUG */

/*
 * Each option has a stack of 17 option values, with opts giving
 * the current, top value, and optstk the value beneath it.
 * One refers to option `l' as, e.g., opt('l') in the text for clarity.
 */
char	opts[ 'z' - 'A' + 1];
short	optstk[ 'z' - 'A' + 1];

extern int control[];			/* EF4: 11-08-86 WB */
#define opt(c) control[c]


/*
 * NOTES ON THE DYNAMIC NATURE OF THE DATA STRUCTURES
 *
 */


/*
 * The initial sizes of the structures.
 * These should be large enough to compile
 * an "average" sized program so as to minimize
 * storage requests.
 * On a small system or and 11/34 or 11/40
 * these numbers can be trimmed to make the
 * compiler smaller.
 */
#define	INL	200

/*
 * The following limits on hash and tree tables currently
 * allow approximately 1200 symbols and 20k words of tree
 * space.  The fundamental limit of 64k total data space
 * should be exceeded well before these are full.
 */
/*
 * TABLE_MULTIPLIER is for uniformly increasing the sizes of the tables
 */
#define TABLE_MULTIPLIER	8
#define	MAXHASH	(4 * TABLE_MULTIPLIER)
/*
 * MAXDEPTH is the depth of the parse stack.
 * STACK_MULTIPLIER is for increasing its size.
 */
#define	STACK_MULTIPLIER	8
#define	MAXDEPTH ( 150 * STACK_MULTIPLIER )

/*
 * ERROR RELATED DEFINITIONS
 */

/*
 * Exit statuses to pexit
 *
 * AOK
 * ERRS		Compilation errors inhibit obj productin
 * NOSTART	Errors before we ever got started
 * DIED		We ran out of memory or some such
 */
#define	AOK	0
#define	ERRS	1
#define	NOSTART	2
#define	DIED	3

bool	Recovery;


/*
 * The character in errpfx will precede the next error message.
 * Code generation is suppressed whenever we have an error
 * and when we are walking the tree to determine types only.
 */
extern char	errpfx;

#define	setpfx(x)	errpfx = x

#define	standard()	setpfx('s')
#define	warning()	setpfx('w')
#define	recovered()	setpfx('e')
#define	continuation()	setpfx(' ')


/*
 * PRE-DEFINED NAMELIST OFFSETS
 *
 * The following are the namelist offsets for the
 * primitive types. The ones which are negative
 * don't actually exist, but are generated and tested
 * internally. These definitions are sensitive to the
 * initializations in nl.c.
 */
#define	NIL	0

/*
 * SEMANTIC DEFINITIONS
 */

/*
 * The variable line is the current semantic
 * line and is set in stat.c from the numbers
 * embedded in statement type tree nodes.
 */
short	line;



/*
 * Routines which need types
 * other than "integer" to be
 * assumed by the compiler.
 */
double		atof();

char		*savestr();

char		*skipbl();

char		*myctime();


/*
 * MAIN PROGRAM VARIABLES, MISCELLANY
 */

char	*filename;		/* current source file name */
time_t	tvec;		/* see myctime() */

extern char	*classes[ ];	/* maps namelist classes to string names */

