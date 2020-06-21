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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define REPLACE_SOURCE 0

char buf[2048], *arg;
int err, line, done, exitval, line_removed;

int handle_line() {
    register char *s, *d;
    int conditional = 0;
    s = d = buf;
    while (*s==' ' || *s=='\t') ++s;
    if (*s=='#') {
	do; while (*++s==' ' || *s=='\t');
	if (!strncmp(s, "line", 4)) {
	    ++line_removed;
	    ++line;
	    fputs("\n", stdout);
	    return (1);
	}
	s = buf;
    }
    fputs(buf, stdout); ++line;
    if (*buf == '&') conditional = 1;
    while (*d = *s++) if (*d!=' ' && *d!='\t') ++d;
    if (d > buf && *--d == '\n') *d = '\0';
    if (!strcmp(buf, "Code:")) {
	if (done++) {
	    fprintf(stderr, "warning (%s): multiple Code line\n",
		arg);
	    exitval = 2;
	} else {
#	    if REPLACE_SOURCE
		char *p;
		if (!fgets(buf, sizeof(buf), stdin)) return (0);
		for (p=buf; *p==' '; ++p);
		if (!strcmp(p, "/*PRETTY*/\n")
			|| !strcmp(p, "/* PRETTY */\n")
			|| !strcmp(p, "/* NO PRETTY */\n")
				       /*Bernds Schreibweise*/) {
		    fputs(buf, stdout); ++line;
		    printf("#line %d \"%s\"\n", line, arg); ++line;
		} else {
		    printf("#line %d \"%s\"\n", line, arg); ++line;
		    fprintf(stderr,
			"warning (%s): PRETTY comment not found\n",
			arg);
		    exitval = 1;
		    return (handle_line());
		}
#	    else
		printf("#line %d \"%s\"\n", line, arg);
#	    endif
	}
    } else if (conditional && (!strncmp(buf, "&if", 3)
			    || !strncmp(buf, "&else", 5)
			    || !strncmp(buf, "&elif", 5)
			    || !strncmp(buf, "&endif", 6)
			    || !strncmp(buf, "&IF", 3)
			    || !strncmp(buf, "&ELSE", 5)
			    || !strncmp(buf, "&ELIF", 5)
			    || !strncmp(buf, "&ENDIF", 6))) {
	printf("#line %d \"%s\"\n", line, arg);
#	if REPLACE_SOURCE
	    ++line;
#	endif
    }
    return (1);
}

main (argc, argv) int argc; char **argv; {
    line = 1;
    if (argc == 3) {
	char *arg = 0;
	if (argv[1][0] == '-') {
	    arg = argv[1];
	    ++argv;
	} else if (argv[2][0] == '-') arg = argv[2];
	if (arg) {
	    --argc;
	    switch (arg[1]) {
		case 'c': /*current line number instead of next*/
			  --line;
			  break;
		case 'n': /*ignored*/
			  break;
	    }
	}
    }
    if (argc != 2) {
	fprintf(stderr,
"Use: mfcline [-c] name <source >destination\n\n\
name\t is the module name inserted into the #line directive,\n\
-c  \t is needed if the compiler expects a line number to correspond\n\
    \t to the current source line instead of the following one.\n");
	exit (-1);
    }
    arg = argv[1];
#ifdef WIN32
	{
		char *ptmp;
		while( ptmp = strchr(arg, '\\')) { *ptmp = '/'; }
	}
#endif
    done = exitval = line_removed = 0;
    do; while (
    		fgets(buf, sizeof(buf), stdin) &&
		handle_line()
	      );
    if (err = ferror(stdin)) {
	fprintf(stderr, "read error %d (%s)", err, arg);
	exit (-1);
    }
    if (!done) {
	fprintf(stderr, "error (%s): code line not found.\n", arg);
	exitval = 3;
    }
    if (line_removed)
	fprintf(stderr, "(%s): %d times #line removed\n",
	    arg, line_removed);
    exit (exitval);
}
