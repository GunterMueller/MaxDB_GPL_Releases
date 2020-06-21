/* @(#)ptoc/ptoc.c


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
/*IST   10.09.91  remove -> ptoc_remove (remove was defined in stdio.h)*/
/*IST             how_pc included into list of relative pathnames      */

/*JH    14.06.91  new Options -XZ (for Tracing), -Y (change global     */
/*JH              Pascal variable to pointer) and -XQ for debug        */
/*JH                                                                   */
/*JH              Don't use Shell variables MCPP etc., otherwise       */
/*JH              problems on machines that use the CLE-Pascal Compiler*/

/*JH    10.05.91  changed absolute pathnames for pcomp...              */
/*JH              into relative ones ($TOOL)                           */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <signal.h>
#include "wait.h"
#if ! WIN32
#include <sys/param.h>
#endif

#ifdef UCB_SYS_INC
#include <strings.h>
#else
#include <string.h>
#endif

#include "../include/devconfig.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define WARN_UNKOWN_OPT(x) \
	fprintf(stderr,"ptoc: warning: Ignoring unknown option '%s' \n", (x))

#define MESG_NUM \
 "ptoc: warning: ignoring option '-W', because it's not followed by a number\n"

#define MESG_2_HEX \
 "ptoc: warning: ignoring option '%s', because it's not followed by 2 hex digits\n"

static sete (char **gdir, char *s);
static init_globals ();

/*
 * Pc - front end for Pascal compiler.
 */
char	*cpp	= "/lib/cpp";
char	*pcomp	= "/usr/lib/pgenc";
char	*how_pc = "/usr/lib/how_pc";

char	*tfile;

extern  char *PTOC_id[] ; /* 92-Sep-15 J.Bruehe  forces 'Version.c' even on AIX */

char	*setsuf(), *savestr();

int	debug;
int	peephole = 0;
int	globalopt = 0;

#define	NARGS	512

#if defined ( HP9 ) || defined ( _IBMR2 ) || defined (__ia64)
/* 1997-Aug-28 J.Bruehe  integrated in common code: Treat AIX like HP */
/* suppress line number info - disturbs the 'lex' generated code */
int	cppargx = 2;
char	*cppargs [NARGS]	= { "cpp", "-P", 0 };
#else
int	cppargx = 1;
char	*cppargs [NARGS]	= { "cpp", 0 };
#endif

int	pcompargx = 3;
char	*pcompargs [NARGS]	= { "pgenc", "-o", "XXX", 0 };
char	*optimizerargs;

char *mesg[] = {
	0,
	"Hangup",
	"Interrupt",	
	"Quit",
	"Illegal instruction",
	"Trace/BPT trap",
	"IOT trap",
	"EMT trap",
	"Floating exception",
	"Killed",
	"Bus error",
	"Segmentation fault",
	"Bad system call",
	"Broken pipe",
	"Alarm clock",
	"Terminated",
	"Signal 16",
	"Stopped (signal)",
	"Stopped",
	"Continued",
	"Child exited",
	"Stopped (tty input)",
	"Stopped (tty output)",
	"Tty input interrupt",
	"Cputime limit exceeded",
	"Filesize limit exceeded",
	"Signal 26",
	"Signal 27",
	"Signal 28",
	"Signal 29",
	"Signal 30",
	"Signal 31",
	"Signal 32"
};


static int	errs;
static char	erropt[4] = "-Xx";

static void
check_X_opts(optp)
	char *optp;
{
	erropt[1] = 'X';
	optp += 2;

	while (*optp)
		switch(*optp++) {
			case 'a': /* enable ANSI C function prototypes */
			case 'c': /* use symbolic constants */
			case 'p': /* enable pointer arithmetic */
			case 'x': /* produce static links for nested procs */
			/* debug options */
			case 'D': /* debug: output of NodeIndex in C code*/
			case 'H': /* debug: output of HDG tree */
			case 'i': /* debug: inhibit flush of debug output */
			case 'Q': /* debug: output from C code generator */
			case 'T': /* debug: output of Symbol table */
				break;
			default:
				erropt[2] = *(optp - 1);
				WARN_UNKOWN_OPT(erropt);
				strcpy(optp - 1, optp);
				--optp;
				break;
		}
}

static void
check_K_opts(optp)
	char *optp;
{
	erropt[1] = 'K';
	optp += 2;

	while (*optp)
		switch(*optp++) {
			case 'b': /* break pointer type */
			case 'd': /* disable! delayed parameter copy */
			case 't': /* disable! deleting unused types */
			case 'v': /* disable! deleting unused variables */
				break;
			default:
				erropt[2] = *(optp - 1);
				WARN_UNKOWN_OPT(erropt);
				strcpy(optp - 1, optp);
				--optp;
				break;
		}
}

#ifdef RAS

static void
check_R_opts(optp)
	char *optp;
{
	erropt[1] = 'R';
	optp += 2;

	while (*optp)
		switch(*optp++) {
			/* -R... options valid by 2.0.6 of 28.08.97 */
			case 'V': /* RAS01 inline moves for short value parm. arrays */
			case 'M': /* RAS01 replace more _ptoc_MOVE by _ptoc_ASSIGN */
			case 'A': /* RAS01 inline moves for short array assigns */
			case 'C': /* RAS01 inline expansion for short compares */
			case 'S': /* RAS02 use integers for small sets */
				break;
#ifdef RASN
			case 'c': /* RASN1 use symbolic & 'c' consts  */
			case 'v': /* RASN2 use anonym. variant recs (unions)  */
			case 'a': /* RASN3 gener. ANSI headers only  */
			case '0': /* RASN4 gener. 00-header files only */
				break;
#endif
#ifdef RASX
			/* options not yet released. */
			/* EmbedC(...) (RAS07) is used without option! */
			case 'E': /* RAS03 chars as ebcdic hex for cross-comp.  */
			case 's': /* RAS04 make LitSizeOp num.const for SQLMOVC */
			case 'W': /* RAS05 don't use temps for WITH expr. */
			case 'w': /* RAS05 use WITH temps in loops only */
			case 'H': /* RAS06 generate macros (->.h-files) instead of procs / funcs */
				break;
#endif
			default:
				erropt[2] = *(optp - 1);
				WARN_UNKOWN_OPT(erropt);
				strcpy(optp - 1, optp);
				--optp;
				break;
		}
}

#endif

static int
check_2hex_digits(optp)
	char *optp;
{
	if (isxdigit(*(optp + 2)) && isxdigit(*(optp + 3)) && *(optp + 4) == 0)
		return 1;
	else
	{
		*(optp + 2) = 0;
		fprintf(stderr, MESG_2_HEX, optp);
		return 0;
	}
}

check_int(optp)
	char *optp;
{
	optp += 2;
	if ( *optp == 0 )
	{
		fprintf(stderr, MESG_NUM);
		return 0;
	}

	while (isdigit(*optp))
		++optp;

	if ( *optp == 0 )
		return 1;
	else
	{
		fprintf(stderr, MESG_NUM);
		return 0;
	}
}

RETSIGTYPE onintr ();

main(argc, argv)
	int argc;
	char **argv;
{
	register char *argp;
	register int i;
	char *t, c;
	int j;
	int docpp;

/*JH*/  init_globals () ;


	argc--, argv++;
	if (argc == 0) {
#if WIN32
		fprintf(stderr,"ptoc: missing arguments \n" );
#else
		execl("/bin/cat", "cat", how_pc, 0);
#endif
		exit(1);
	}
	if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
		signal(SIGINT, onintr);
		signal(SIGTERM, onintr);
	}
	for (i = 0; i < argc; i++) {
		argp = argv [i];

		if (argp [0] != '-')
			continue;

		switch (argp [1]) {

		case 'd':	/* print version and steps performed by ptoc */
			if (argp [2] == '\0')
				debug++;
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'O': /* -O : call optimizer */
			if (argp [2] == '\0')
				pcompargs [pcompargx++] = "-XOABCPS";
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'X':
			check_X_opts(argp);
			pcompargs [pcompargx++] = argp;
			continue;
#ifdef RAS /*RAS00 pass -R... */
		case 'R':
			check_R_opts(argp);
			pcompargs [pcompargx++] = argp;
			continue;
#endif
		case 'K':
			check_K_opts(argp);
			pcompargs [pcompargx++] = argp;
			continue;

		case 'C':	/* produce range check code */
			if (argp [2] == '\0')
				pcompargs [pcompargx++] = "-r";
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'D':	/* defines for cpp */
		case 'I':	/* include paths for cpp */
			cppargs [cppargx++] = argp;
			continue;

		case 'l':	/* produce source listing */
		case 'e':	/* enable \ escape in strings */
		case 's':	/* allow only standard  pascal */
		case 'v':	/* print optimizer statistics */
		case 'Y':	/* debug output into C code */
			if (argp [2] == '\0')
				pcompargs[pcompargx++] = argp;
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'y':	/* allow pascal extensions from PYRAMID */
			if ((argp [2] == 'p') && (argp [3] == 'c'))
				pcompargs[pcompargx++] = argp;
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'q':	/* pack sets and enumerative types */
			if ((argp [2] == '\0') || (argp [2] == 'n'))
				pcompargs[pcompargx++] = argp;
			else
				WARN_UNKOWN_OPT(argp);
			continue;

		case 'f':	/* fill local vars with byte pattern */
		case 'g':	/* fill FOR control var with byte pattern */
			if (check_2hex_digits(argp))
				pcompargs [pcompargx++] = argp;
			continue;

		case 'W':	/* warnings for value params > size bytes */
			if (check_int(argp))
				pcompargs [pcompargx++] = argp;
			continue;

		default:
			WARN_UNKOWN_OPT(argp);
			continue;
		}
	}

	if (debug)
		fprintf(stderr,"ptoc: Version is: %s \n" , PTOC_id [0] );

	for (i = 0; i < argc; i++) {
		argp = argv[i];

		if (argp[0] == '-')
			continue;

		/* .p and .P files */

		if (suffix (argp) == 'p')
			docpp = 0;
		else if (suffix (argp) == 'P')
			docpp = 1;
		else {
			fprintf(stderr,"ptoc: warning: ignoring file '%s'!", argp);
			fprintf(stderr," (extension is not '.p' or '.P')\n");
			continue;
		}

		tfile = setsuf (argp, 'c');

		pcompargs [2] = tfile;
		if (docpp) {
			cppargs [cppargx++] = argp;
			cppargs [cppargx] = 0;
		}
		else
			pcompargs [pcompargx++] = argp;

		pcompargs [pcompargx] = 0;

#if WIN32
		dosys (pcomp, pcompargs);
#else /*WIN32*/
		if (docpp)
			dopipe (cpp, cppargs, pcomp, pcompargs);
		else
			dosys (pcomp, pcompargs);
#endif /*WIN32*/
	}

	exit(errs);
}

#if WIN32
dosys(cmd, argv)
	char *cmd, **argv;
{
	int idx;
	int lgt;
	int len;
	char comd [ 32768 ];


	lgt = strlen ( cmd );
	(void) memcpy ( comd , cmd , lgt );
	comd[lgt ++] = ' ' ;

	for ( idx = 1 ; argv[idx] ; idx ++ )
	{
	    len = strlen ( argv[idx] );
	    if ( lgt + len + 2 >= sizeof(comd) )
	    {
		fprintf(stderr, "ptoc: command too long\n");
		done();
		return ( -1 );
	    }
	    (void) memcpy ( comd + lgt , argv[idx] , len );
	    lgt += len ;
	    comd[lgt ++] = ' ' ;
	}
	comd[lgt] = 0 ;

	if (debug) {
		printf( comd );
		printf("\n");
	}

#if WIN32
	return ( ksystem ( comd ) );
#else
	return ( system ( comd ) );
#endif
}
#else /*WIN32*/
dosys(cmd, argv)
	char *cmd, **argv;
{
	union wait status;
	int pid;

	if (debug) {
		int i;
		printf("%s:", cmd);
		for (i = 0; argv[i]; i++)
			printf(" %s", argv[i]);
		printf("\n");
	}
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "ptoc: No more processes\n");
		done();
	}
	if (pid == 0) {
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			signal(SIGINT, SIG_DFL);
		execv(cmd, argv);
		perror(cmd);
		exit(1);
	}
	while (wait(&status) != pid)
		;
	if (WIFSIGNALED(status)) {
		if (status.w_termsig != SIGINT) {
			fprintf(stderr, "%s: %s", cmd, mesg[status.w_termsig]);
			if (status.w_coredump)
				fprintf(stderr, " (core dumped)");
			fprintf(stderr, "\n");
		}
		errs = 100;
		done();
		/*NOTREACHED*/
	}
	if (status.w_retcode) {
		errs = 1;
		ptoc_remove();
	}
	return (status.w_retcode);
}

int
dopipe (cmd_p, argv_p, cmd_c, argv_c)
	char *cmd_p, **argv_p;
	char *cmd_c, **argv_c;
{
	union wait status;
	int pid_p, pid_c, pid;
	char *cmd;
	int fildes [2];
	int retcode;

	if (debug) {
		int i;
		printf("%s:", cmd_p);
		for (i = 0; argv_p[i]; i++)
			printf(" %s", argv_p[i]);
		printf (" | ");
		printf("%s:", cmd_c);
		for (i = 0; argv_c[i]; i++)
			printf(" %s", argv_c[i]);
		printf("\n");
	}
	if (pipe (fildes) == -1) {
		perror ("ptoc: ");
		exit (1);
	}

	pid_p = fork ();

	if (pid_p < 0) {
		fprintf (stderr, "ptoc: No more processes\n");
		done ();
	}
	if (pid_p == 0) {
		close (fildes [0]);
		close (1);
		if (dup (fildes [1]) == -1) {
			perror ("ptoc: ");
			exit (1);
		}
		close (fildes [1]);
		if (signal (SIGINT, SIG_IGN) != SIG_IGN)
			signal (SIGINT, SIG_DFL);
		execv (cmd_p, argv_p);
		perror (cmd_p);
		exit (1);
	}

	pid_c = fork ();

	if (pid_c < 0) {
		fprintf (stderr, "ptoc: No more processes\n");
		done ();
	}
	if (pid_c == 0) {
		close (fildes [1]);
		close (0);
		if (dup (fildes [0]) == -1) {
			perror ("ptoc: ");
			exit (1);
		}
		close (fildes [0]);
		if (signal (SIGINT, SIG_IGN) != SIG_IGN)
			signal (SIGINT, SIG_DFL);
		execv (cmd_c, argv_c);
		perror (cmd_c);
		exit (1);
	}

	close (fildes [0]);
	close (fildes [1]);
	while ((pid = wait (&status)) != -1) {
		if (pid == pid_p) {
			cmd = cmd_p;
		}
		else {
			cmd = cmd_c;
		}
		if (WIFSIGNALED (status)) {
			if (status.w_termsig != SIGINT) {
				fprintf(stderr, "%s: %s", cmd, mesg[status.w_termsig]);
				if (status.w_coredump)
					fprintf(stderr, " (core dumped)");
				fprintf(stderr, "\n");
			}
			while (wait (&status) != -1)
				;
			errs = 100;
			done();
			/*NOTREACHED*/
		}
		if (retcode = status.w_retcode) {
			while (wait (&status) != -1)
				;
			errs = 1;
			ptoc_remove();
		}
	}
	return retcode;
}
#endif /*WIN32*/

done()
{

	ptoc_remove();
	exit(errs);
}

ptoc_remove()
{

	if (errs)
		unlink(tfile);
}

RETSIGTYPE onintr ()
{

	errs = 10;
	done();
}

getsuf(cp)
	char *cp;
{

	if (*cp == 0)
		return(0);
	while (cp[1])
		cp++;
	if (cp[-1] != '.')
		return (0);
	return (*cp);
}

char *
setsuf(as, ch)
	char *as;
{
	register char *s, *s1;

	s = s1 = savestr(as);
	while (*s)
		if (*s++ == '/')
			s1 = s;
	s[-1] = ch;
	return (s1);
}

#define	NSAVETAB	512
char	*savetab;
int	saveleft;

char *
savestr(cp)
	register char *cp;
{
	register int len;

	len = strlen(cp) + 1;
	if (len > saveleft) {
		saveleft = NSAVETAB;
		if (len > saveleft)
			saveleft = len;
		savetab = (char *)malloc(saveleft);
		if (savetab == 0) {
			fprintf(stderr, "ran out of memory (savestr)\n");
			exit(1);
		}
	}
	strncpy(savetab, cp, len);
	cp = savetab;
	savetab += len;
	return (cp);
}

suffix(cp)
	char *cp;
{

	if (cp[0] == 0 || cp[1] == 0)
		return (0);
	while (cp[1])
		cp++;
	if (cp[-1] == '.')
		return (*cp);
	return (0);
}

static  init_globals ()
{
    if ( getenv ("TOOL") )
    {
 	sete ( &pcomp   , "lib/pgenc") ;
 	sete ( &how_pc  , "lib/how_pc") ;

	/* already defined
	 * char    *cpp    = "/lib/cpp";
	 * char    *as     = "bin/as";
	 */
    }
}

static sete ( gdir , s )
char    **gdir ;
char    *s ;
{
    char        *tooldir ;
    char        *pos ;
    char        linebuf[80] ;

    /* MWa extern char *malloc () ; */

    tooldir = getenv ("TOOL") ;
    sprintf ( linebuf , "%s/%s" , tooldir , s ) ;

    if (( pos = malloc ( strlen ( linebuf ) + 1 )) == NULL )
    {
	perror ("malloc") ;
	exit (1) ;
    }

#ifdef _IBMR2
    strcpy ( (unsigned char *) pos , (unsigned char *) linebuf ) ;
#else
    strcpy ( pos , linebuf ) ;
#endif
    *gdir = pos ;
}
