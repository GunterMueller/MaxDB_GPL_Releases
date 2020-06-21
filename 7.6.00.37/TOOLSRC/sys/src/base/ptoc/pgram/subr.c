/* @(#)pgram/subr.c

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

#include "devconfig.h"

#if WIN32
# include	<io.h>		/* write,... */
#endif
#include "0.h"
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <time.h>

/* EF4: 11-08-86 WB; Definition of classes is placed here */
char	*classes[ ] = {
	"undefined",
	"constant",
	"type",
	"variable",	/*	VARIABLE	*/
	"array",
	"pointer or file",
	"record",
	"field",
	"procedure",
	"function",
	"variable",	/*	VARIABLE	*/
	"variable",	/*	VARIABLE	*/
	"pointer",
	"file",
	"set",
	"subrange",
	"label",
	"withptr",
	"scalar",
	"string",
	"program",
	"improper",
	"variant",
	"formal procedure",
	"formal function"
};

/*
 * Does the string fp end in '.' and the character c ?
 */
dotted(fp, c)
	register char *fp;
	char c;
{
	register int i;

	i = strlen(fp);
	return (i > 1 && fp[i - 2] == '.' && fp[i - 1] == c);
}

#include <sys/stat.h>

/*
 * Set the time vector "tvec" to the
 * modification time stamp of a file.
 */
gettime( filename )
    char *filename;
{
	struct stat stb;

	stat(filename, &stb);
	tvec = stb.st_mtime;
}

/*
 * Convert a "ctime" into a Pascal styple time line
 */
char *
myctime(tv)
	time_t *tv;
{
	register char *cp, *dp;
	extern char *ctime();
	char *cpp;
	static char mycbuf[26];

	cpp = ctime(tv);
	dp = mycbuf;
	cp = cpp;
	cpp[16] = 0;
	while (*dp++ = *cp++);
	dp--;
	cp = cpp+19;
	cpp[24] = 0;
	while (*dp++ = *cp++);
	return (mycbuf);
}


/*
 * Boom!
 */
Perror(file, error)
	char *file, *error;
{

	fprintf( stderr , "%s: %s\n" , file , error );
}

int *
pcalloc(num, size)
	int num, size;
{
	register int *p1, *p2, nbyte;

	nbyte = (num*size+( ( sizeof ( int ) ) - 1 ) ) & ~( ( sizeof ( int ) ) - 1 );
	if ((p1 = (int *) malloc((unsigned) nbyte)) == 0)
		return (0);
	p2 =  p1;
	nbyte /= sizeof ( int );
	do {
		*p2++ = 0;
	} while (--nbyte);
	return (p1);
}


/*
 * Strlen is currently a freebie of perror
 * Take the length of a string.
 * Note that this does not include the trailing null!
 * strlen(cp)
 *         register char *cp;
 * {
 *         register int i;
 *
 *         for (i = 0; *cp != 0; cp++)
 *                 i++;
 *         return (i);
 * }
 */
copy(to, from, bytes)
	register char *to, *from;
	register int bytes;
{

	if (bytes != 0)
		do
			*to++ = *from++;
		while (--bytes);
}

/*
 * Is ch one of the characters in the string cp ?
 */
any(cp, ch)
	register char *cp;
	char ch;
{

	while (*cp)
		if (*cp++ == ch)
			return (1);
	return (0);
}

opush(c)
	register CHAR c;
{

	c -= 'A';
	optstk[c] <<= 1;
	optstk[c] |= opts[c];
	opts[c] = 1;
}

opop(c)
	register CHAR c;
{

	c -= 'A';
	opts[c] = optstk[c] & 1;
	optstk[c] >>= 1;
}
		/* EF4: 11-08-86 WB; pchr, pexit are taken from main.c */
pchr(c)
	char c;
{

	putc ( c , stdout );
}

char	ugh[]	= "Fatal error in pcomp\n";
/*
 * Exit from the Pascal system.
 * We throw in an ungraceful termination
 * message if c > 1 indicating a severe
 * error such as running out of memory
 * or an internal inconsistency.
 */
pexit(c)
	int c;
{

	if (opt('l') && c != DIED && c != NOSTART)
		while (getline() != -1)
			continue;
	yyflush();
	switch (c) {
		case DIED:
			write(2, ugh, sizeof ugh);
		case NOSTART:
		case ERRS:
			break;
		case AOK:
			break;
	}
	sql__exit(c);
}
