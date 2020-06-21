/*! 
  -----------------------------------------------------------------------------
 
  module: vcn18.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  Christoph
 
  special area: dbmctrl
 
  description:  implementation of dbmsrv control program 

  version:      7.5

  -----------------------------------------------------------------------------
 
                          Copyright (c) 2000-2005 SAP AG
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

*/

#ifndef WIN32
#define UNIX UNIX
#endif

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hsp100.h"
#include "RunTime/RTE_FileUser.hpp" 
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_string.h"

#ifdef UNIX
#include "RunTime/RTE_UNIXAuthenticate.hpp"
#endif

#ifdef UNIX
#include <unistd.h>
#include <limits.h>
#endif

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

/* 
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

#ifdef UNIX
#define PATH_SPLITTER  ':'
#define PATH_SEPARATOR '/'
#endif

#ifdef WIN32
#define PATH_SPLITTER ';'
#define PATH_SEPARATOR '\\'
#endif

#ifdef WIN32
#define access(a,b) _access(a,b)
#ifndef R_OK
#define R_OK 4
#endif
#ifndef X_OK
#define X_OK 0
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif


/* 
  -----------------------------------------------------------------------------
  private declarations
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  function: getDBMSrvName
  -----------------------------------------------------------------------------
*/

static bool
inst_unreg (const char *instroot) {
	Msg_List errList;

	bool ok =
	(RTE_DBRegister::Instance()).UnregisterInstallation
	(instroot, errList);

	return (ok);
}

static bool
inst_reg (const char *instroot) {
	tsp100_VersionID1 their;
	RTE_Installation::InstVersionInfo our;
	Msg_List errList;

	sp100_GetVersionID (VersionIDType1_esp100, 0, &their);

	strncpy (our.key, instroot, sizeof(RTE_DBKey));
	our.release = their.MajorVersion_sp100;
	our.major   = their.MinorVersion_sp100;
	our.minor   = their.CorrLevel_sp100;
	our.build   = their.BuildNumberPrefix_sp100;

	bool ok =
	(RTE_DBRegister::Instance()).RegisterInstallation
	(instroot, our, errList);

	return (ok);
}

static char *
substDots (char *rel) {
#ifdef UNIX
	if (memcmp (rel, "./", 2) == 0) {
#endif
#ifdef WIN32
	if (memcmp (rel, ".\\", 2) == 0) {
#endif
		char *ptr = new char[PATH_MAX + 1];
		char *cwd = getcwd (ptr, PATH_MAX);

		char *filename = new char[strlen (cwd) + strlen (rel) + 1];
		SAPDB_strcpy (filename, cwd);
		strcat (filename, rel + 1);

		delete (ptr);
		delete (rel);

		return (filename);
	}

#ifdef UNIX
	if (memcmp (rel, "../", 3) == 0) {
#endif
#ifdef WIN32
	if (memcmp (rel, "..\\", 3) == 0) {
#endif
		char *ptr = new char [PATH_MAX + 1];
		char *cwd = getcwd (ptr, PATH_MAX);

		char *filename = new char [strlen (cwd) + strlen (rel)];
		SAPDB_strcpy (filename, cwd);
		strcat (filename, rel + 2);

		delete (ptr);
		delete (rel);

		return (filename);
	}

	return (rel);
}

static char *
getFullname (char *arg0) {
	char *ptr;

	if (arg0 == 0)
		return 0;

	// try absolute path
#ifdef UNIX
	if (arg0[0] == PATH_SEPARATOR) {
		char *filename = new char [strlen (arg0) + 1];
#endif
#ifdef WIN32
	if
	((arg0[0] == PATH_SEPARATOR && arg0[1] == PATH_SEPARATOR) ||
	(((arg0[0] >= 'a' && arg0[0] <= 'z') ||
	(arg0[0] >= 'A' && arg0[0] <= 'Z')) &&
	arg0[1] == ':' && arg0[2] == PATH_SEPARATOR)) {
		char *filename = new char [strlen (arg0) + 4 + 1];
#endif
		if (filename == 0)
			return 0;

		SAPDB_strcpy (filename, arg0);
#ifdef WIN32
		ptr = strrchr (filename, '.');
		if (ptr == 0)
			strcat (filename, ".exe");
#endif
		return (filename);
	}

	// normalize image name
#ifdef UNIX
	char *image = new char [strlen (arg0) + 1];
	SAPDB_strcpy (image, arg0);
#endif
#ifdef WIN32
	char *image = new char [strlen (arg0) + 4 + 1];
	SAPDB_strcpy (image, arg0);
	ptr = strrchr (image, '.');
	if (ptr == 0)
		strcat (image, ".exe");
#endif

#ifdef UNIX
	if (memcmp (image, "./", 2) == 0 || memcmp (image, "../", 3) == 0) {
#endif
#ifdef WIN32
	if (memcmp (image, ".\\", 2) == 0 || memcmp (image, "..\\", 3) == 0) {
#endif
		return (substDots (image));
	}

	// relative to working directory
	if (strchr (image, PATH_SEPARATOR) != 0) {
		ptr = new char [PATH_MAX + 1];
		char *cwd = getcwd (ptr, PATH_MAX);

		char *filename =
		new char [strlen (cwd) + 1 + strlen (image) + 1];

		sprintf (filename,
		"%s%c%s", cwd, PATH_SEPARATOR, image);

		delete (ptr);
		if (access (filename, R_OK | X_OK) == 0) {
			delete (image);
			return (substDots (filename));
		}
	
		delete (filename);	
	}

	// cwd under windows
#ifdef WIN32
	ptr = new char [PATH_MAX + 1];
	char *cwd = getcwd (ptr, PATH_MAX);
	if (cwd != 0) {
		char *filename =
		new char [strlen (cwd) + 1 + strlen (image) + 1];

		sprintf (filename,
		"%s%c%s", cwd, PATH_SEPARATOR, image);

		delete (ptr);
		if (access (filename, R_OK | X_OK) == 0) {
			delete (image);
			return (substDots (filename));
		}
	}

	delete (ptr);
#endif

	// try environment 
	ptr = getenv ("PATH");
	if (ptr == 0)
		return 0;

	char *envPath = new char[strlen (ptr) + 1];
	SAPDB_strcpy (envPath, ptr);
	char *path = envPath;

	for (;;) {
		if (path == 0) {
			delete (image);
			delete (envPath);
			return 0;
		}

		char *dir = path;

		// split first part, if there is one move to next part
		ptr = strchr (dir, PATH_SPLITTER);
		if (ptr != 0) {
			*ptr = '\0';
			path = ptr + 1;
		} else {
			path = 0;
		}

		// skip empty string
		if (dir[0] == '\0')
			continue;

		// setup probable program name
		char *filename = 0;

		filename =
		new char [strlen (dir) + 1 + strlen (image) + 1];
		sprintf (filename,
		"%s%c%s", dir, PATH_SEPARATOR, image);

		// check existence of file
		if (access (filename, R_OK | X_OK) == 0) {
			delete (image);
			delete (envPath);
			return (substDots (filename));
		}
	}

	delete (image);
	delete (envPath);
	return 0;
}

static char *
getDBROOTFromFilename (const char *filename) {
	if (filename == 0 || filename[0] == '\0')
		return 0;

	char *dbroot = new char [strlen (filename) + 1];
	SAPDB_strcpy (dbroot, filename);

	char *ptr;
	for (int i = 0; i < 2; i++) {
		ptr = strrchr (dbroot, PATH_SEPARATOR);
		if (ptr == 0) {
			delete (dbroot);
			return 0;
		}

		*ptr = '\0';
	}

	return (dbroot);
}

static char *
getDBMSrvName (const char *dbroot) {
	if (dbroot == 0 || dbroot[0] == '\0')
		return 0;

#ifdef UNIX
	const char relname[] = "pgm/dbmsrv";
#endif
#ifdef WIN32
	const char relname[] = "pgm\\dbmsrv.exe";
#endif

	char *dbmsrv = new char [strlen (dbroot) + 1 + strlen (relname) + 1];
	sprintf (dbmsrv, "%s%c%s", dbroot, PATH_SEPARATOR, relname);

	return (dbmsrv);
}


static int
stopDBMSrv (const char *dbroot) {
	if (dbroot == 0 || dbroot[0] == '\0')
		return (-1);

	char *dbmsrv = getDBMSrvName (dbroot);

	inst_unreg (dbroot);

	RTE_FileUser fu;
	RTE_ProcInfo pi = fu.WhoUsesModule (dbmsrv);

	procinfo *curr = pi.First ();
	if (curr == 0) {
		delete (dbmsrv);
		return 0;
	}

	while (curr != 0) {
		printf ("terminating process %d ... ", curr->pid);

		int rc = pi.Terminate (curr);
		if (rc == 0)
			printf (" done\n");
		else
			printf (" failed\n");

		curr = pi.Next (curr);
	}

	delete (dbmsrv);
	return 0;
}

static int
startDBMSrv (const char *dbroot) {
	if (dbroot == 0 || dbroot[0] == '\0')
		return (-1);

	inst_reg (dbroot);
	return 0;
}

static int
becomeSDBOwner () {
	Msg_List errList;

#ifdef UNIX
	if (!RTE_IsSapdbOwnerOrRoot(errList)) {
		fprintf (stderr, "only root or sdb owner can do this\n");
		return -1;
	}

	if (!RTE_SetSapdbOwnerAndGroupId(errList)) {
		fprintf (stderr, "cannot switch to sdb owner\n");
		return -1;
	}
#endif

	return 0;
}

/*
  -----------------------------------------------------------------------------
  function: main
  -----------------------------------------------------------------------------
*/

int
main (int argc, char *argv[])
{
	if (becomeSDBOwner () != 0)
		return 1;

	char *image = getFullname (argv[0]);
	char *dbroot = getDBROOTFromFilename (image);

	if (dbroot == 0 || dbroot == '\0') {
		fprintf (stderr, "path not found\n");
		delete (image);
		return 1;
	}

	if (argc == 2 && strcmp (argv[1], "start") == 0) {
		startDBMSrv (dbroot);
		delete (dbroot);
		delete (image);
		return 0;
	}

	if (argc == 2 && strcmp (argv[1], "stop") == 0) {
		stopDBMSrv (dbroot);
		delete (dbroot);
		delete (image);
		return 0;
	}
	
	fprintf (stderr, "usage: %s start | stop\n", image);
	delete (dbroot);
	delete (image);
	return 1;
}
