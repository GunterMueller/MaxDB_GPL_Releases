/*
 * vcn46try.cpp
//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//
\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
\endif
 */

#include <string.h>
#include <stdio.h>

#include "SAPDBCommon/SAPDB_string.h"

#ifndef WIN32
#define UNIX
#endif

#define BUFFERSIZE (16 * 1024)

#include <stdio.h>
#ifndef FREEBSD
#include <malloc.h>
#endif
#include <stdlib.h>
#include "heo01.h"
#include "cn/buildinfo/buildinfo.h"
#include "RunTime/RTE_GetDBRootOfDB.h"

static int
usage (char *prog) {
	printf ("%s [-d <...>][-R <...>] -h | -l | -f <...> | -s <...> | <...>\n", prog);
	printf ("  -d <database name>\n");
	printf ("  -R <database server directory>\n");
	printf ("  -h, --help                      show help\n");
	printf ("  -l, --list                      list files\n");
	printf ("  -s, --show <filename>           show buildnumber\n");
	printf ("  -f, --full <filename>           show detailed\n");
	printf ("  <filename>                      show buildnumber\n");

	return 0;
}

int main (int argc, char **argv) {
	int i;
	int rc;
	char buffer[BUFFERSIZE];

	int do_list = 0;
	int do_full = 0;
	int do_help = 0;

	char *dbname = 0;
	char *dbroot = 0;
	char *logicalname = 0;
	char *dbrootenv = 0;

	rc = 1;
	for (i = 1; i < argc; i++) {
		if (strcmp (argv[i], "-h") == 0 || strcmp (argv[i], "--help") == 0) {
			do_help = 1;
		} else if (strcmp (argv[i], "-d") == 0) {
			if (argc > i) dbname = argv[++i];
		} else if (strcmp (argv[i], "-R") == 0) {
			if (argc > i) dbroot = argv[++i];
		} else if (strcmp (argv[i], "-l") == 0 || strcmp (argv[i], "--list") == 0) {
			do_list = 1;
		} else if (*argv[i] != '-') {
			logicalname = argv[i];
		} else if (strcmp (argv[i], "-s") == 0 || strcmp (argv[i], "--show") == 0) {
			if (argc > i) logicalname = argv[++i];
		} else if (strcmp (argv[i], "-f") == 0 || strcmp (argv[i], "--full") == 0) {
			if (argc > i) {do_full = 1; logicalname = argv[++i];}
		} else {
			do_help = 1;
		}
	}

	if (dbroot != 0) {
		dbrootenv = (char *) malloc (strlen (dbroot) + 8);
		if (dbrootenv == 0)
			return (rc);

		SAPDB_strcpy (dbrootenv, "DBROOT=");
		strcat (dbrootenv, dbroot);
		putenv (dbrootenv);
	} else if (dbname != 0) {
		char dbroot_of_db[PATH_LEN_EO01 + 1];

		if (!RTE_GetDBRootOfDB (dbname, dbroot_of_db, PATH_LEN_EO01)) {
			printf ("unknown database name %s\n", dbname);
			return (rc);
		}

		dbrootenv = (char *) malloc (strlen (dbroot_of_db) + 8);
		if (dbrootenv == 0)
			return (rc);

		SAPDB_strcpy (dbrootenv, "DBROOT=");
		strcat (dbrootenv, dbroot_of_db);
		putenv (dbrootenv);
	}

	if (getenv ("DBROOT") == 0) {
		printf ("unknown server software directory\n");
		return (rc);
	}

	if (do_list != 0) {
		rc = cn46BuildInfoList (buffer, BUFFERSIZE - 1);
	} else if (do_full != 0 && logicalname != 0) {
		rc = cn46BuildInfoFull (logicalname, buffer, BUFFERSIZE - 1);
	} else if (do_full == 0 && logicalname != 0) {
		rc = cn46BuildInfo (logicalname, buffer, BUFFERSIZE - 1);
	} else {
		rc = usage (argv[0]);
	}

	if (dbrootenv != 0)
		free (dbrootenv);

	printf ("%s\n", buffer);
	return (rc);
}
