/*


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
char id_mkvers[] = "@(#)tools/mkvers/mkvers.c		2.0.5  1996-09-03  SQL DBS" ;
/*
 * extract sccs id strings from source files
 * first arg is lib name.
 * Put them in Version.c
 */

#include	<stdio.h>
#include 	<stdlib.h>
#define SCCS_ID		"@(#)"
#define SCCS_ID_LEN	4
#define VERSION		"Version.c"

int
main(argc, argv)
int argc; char **argv;
{
	char buf[BUFSIZ];
	char *s, *e;
	char *index(), *ctime();
	long t;
	FILE *V, *fdopen();

	V = stdout; /* fdopen(creat(VERSION, 0644), "w"); */
	if (!V) {
		perror("mkvers");
		exit(1);
	}
	if (argc > 1 && argv[1][0] != '.') {
		fprintf(V, "char *"); /* was 'static', deleted by AIX-'ld' */
		for (s = argv[1]; *s && *s != '.'; s++)
			fputc(*s, V);
		fprintf(V, "_id[] = {\n");
	}
	else
		fprintf(V, "static char *sccs_id[] = {\n");
	if (argc-- > 1) {
		time(&t);
		s = ctime(&t) + 4;
		s[20] = '\0';
		fprintf(V, "\t\"%s%s\t%s\",\n", SCCS_ID, *++argv, s);
	}
	while (--argc) {
		if (freopen(*++argv, "r", stdin) == NULL) {
			perror(*argv);
			continue;
		}
		while(gets(buf)) {
			s = buf;
			/* In this line, look for a 'what' string, skip it */
			while (s = index (s, '@')) {
				if (strncmp (s, SCCS_ID, SCCS_ID_LEN) == 0) {
					s += SCCS_ID_LEN ;
					while ( *s == ' ' )
						s ++ ;
					break;
					}
				s++;
			}
			/* If it was found, copy rest of line */
			if (s) {
				e = index(s, '\n');
				if (e)
					*e = '\0';
				fprintf(V, "\t\"%s\",\n", s);
				break;
			}
		}
		if (feof(stdin))
			fprintf(stderr, "%s: no sccs id string\n", *argv);
	}
	fprintf(V, "};\n");
	fclose(V);
	fflush(stdout);
	fflush(stderr);
	return 0;
}

char *index(s,c)
char *s;
char c;
{ while (*s) {
	if (*s==c) return(s);
	s++;
  }
  return((char *)0);
}
