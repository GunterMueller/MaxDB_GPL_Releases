/*
 * vcn44.cpp
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

#include "SAPDBCommon/SAPDB_string.h"

#include "extern/zlib/zlib.h"
#include "hcn44.h"

#ifndef WIN32
#define UNIX
#endif

#ifdef UNIX
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined __osf__
#define NEED_SNPRINTF NEED_SNPRINTF
#endif

#ifdef NEED_SNPRINTF
#include <stdarg.h>
static int
snprintf (char *sz_out, int cb_out, char *fmt, ...) {
	int rc;

	va_list args;
	va_start (args, fmt);
	rc = vsprintf (sz_out, fmt, args);
	va_end (args);

	return (rc);
}
#endif
#endif

#ifdef WIN32
#include <WINDOWS.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define snprintf _snprintf
#endif

/* 
 * tar typeflag types
 */
#define TAR_AREGTYPE '\0'
#define TAR_REGTYPE  '0'
#define TAR_LNKTYPE  '1'
#define TAR_SYMTYPE  '2'
#define TAR_CHRTYPE  '3'
#define TAR_BLKTYPE  '4'
#define TAR_DIRTYPE  '5'
#define TAR_FIFOTYPE '6'

/*
 * tar magic
 */
static char TAR_MAGIC[5] = {'u', 's', 't', 'a', 'r'};

/*
 * this is known as the tar header
 */
typedef struct {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag[1];	
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
} tar_hdr_t;

/*
 * content of packagedata
 */
typedef struct {
	char *interface_version;
	char *package_name;
	char *software_version;
	char *mode;
} packagedata_t;

/*
 * list element for sdbinst archive names
 */
typedef struct sdbinst_archive_name {
	char *name;
	struct sdbinst_archive_name *next;
} sdbinst_archive_name_t;

static sdbinst_archive_name_t *GetArchiveNames (const char *, char *, int);
static void FreeArchiveNames (sdbinst_archive_name_t *);

static char *
PrintPACKAGEDATA (packagedata_t *pd, char *buff, int n, char *fmt) {
	buff[0] = '\0';

	if (pd == 0)
		return 0;

	if (pd->interface_version == 0)
		return 0;

	if (strcmp (pd->interface_version, "0.1") != 0)
		return 0;

	if (pd->software_version == 0)
		return 0;

	if (pd->package_name == 0)
		return 0;

	if (pd->mode != 0) {
		snprintf (buff, n, fmt, pd->package_name, pd->software_version, pd->mode);
	} else if (pd->mode == 0) {
		snprintf (buff, n, fmt, pd->package_name, pd->software_version, "");
	}
	
	return (buff);
}

static void
FreePACKAGEDATA (packagedata_t *pd) {
	if (pd == 0)
		return;

	if (pd->interface_version != 0)
		free (pd->interface_version);

	if (pd->package_name != 0)
		free (pd->package_name);

	if (pd->software_version != 0)
		free (pd->software_version);

	if (pd->mode != 0)
		free (pd->mode);

	free (pd);
}

static packagedata_t *
ParsePACKAGEDATA (char *data) {
	packagedata_t *pd;
	char *ptr;
	char *line;
	char *next;
	char *key;
	char *val;

	pd = (packagedata_t *) malloc (sizeof (packagedata_t));
	if (pd == 0) {
		return 0;
	}
	memset (pd, 0, sizeof (packagedata_t));

	next = data;
	for (;;) {
		/*
		 * find enf of text
		 */
		if (next == 0 || strlen (next) == 0)
			break;

		/*
		 * split text into lines
		 */
		ptr = strchr (next, '\n');
		if (ptr != 0) {
			line = (char *) malloc (ptr - next + 1);
			if (line == 0) {
				FreePACKAGEDATA (pd);
				return 0;
			}

			SAPDB_memcpy (line, next, ptr - next);
			line[ptr - next] = '\0';
			next = ptr + 1;
		} else {
			line = (char *) malloc (strlen (next) + 1);
			if (line == 0) {
				FreePACKAGEDATA (pd);
				return 0;
			}

			SAPDB_strcpy (line, next);
			next = 0;
		}

		/*
		 * remove trailing space
		 */
		for (;;) {
			char c;

			c = line[strlen (line) - 1]; 
			if (c == ' ' || c == '\n' || c == '\r' ||c == '\t') {
				line[strlen (line) - 1] = '\0';
				continue;
			}

			break;
		}

		/*
		 * find key-value separator
		 */
		ptr = strchr (line, '=');
		if (ptr == 0) {
			free (line);
			continue;
		}

		*ptr = '\0';
		key = line;
		val = ptr + 1;

		/*
		 * remove spaces around key
		 */
		for (;;) {
			char c;
			
			c = key[strlen (key) - 1]; 
			if (c == ' ' || c == '\n' || c == '\r' ||c == '\t') {
				key[strlen (key) - 1] = '\0';
				continue;
			}

			break;
		}

		for (;;) {
			char c;

			c = key[0]; 
			if (c == ' ' || c == '\n' || c == '\r' ||c == '\t') {
				key++;
				continue;
			}

			break;
		}

		/*
		 * remove spaces around val
		 */
		for (;;) {
			char c;

			c = val[strlen (val) - 1];
			if (c == ' ' || c == '\n' || c == '\r' ||c == '\t') {
				val[strlen (val) - 1] = '\0';
				continue;
			}

			break;
		}

		for (;;) {
			char c;

			c = val[0]; 
			if (c == ' ' || c == '\n' || c == '\r' ||c == '\t') {
				val++;
				continue;
			}

			break;
		}

		/*
		 * get rid of double quotes
		 */
		if (val[0] == '\"' && val[strlen(val) - 1] == '\"') {
			val[strlen (val) - 1] = '\0';
			val++;
		}
		
		/*
		 * store values
		 */
		if (strcmp (key, "INTERFACE_VERSION") == 0) {
			pd->interface_version = (char *) malloc (strlen (val) + 1);
			if (pd->interface_version == 0) {
				FreePACKAGEDATA (pd);
				free (line);
				return 0;
			}

			SAPDB_strcpy (pd->interface_version, val);
		} else 	if (strcmp (key, "SOFTWARE_VERSION") == 0) {
			pd->software_version = (char *) malloc (strlen (val) + 1);
			if (pd->software_version == 0) {
				FreePACKAGEDATA (pd);
				free (line);
				return 0;
			}

			SAPDB_strcpy (pd->software_version, val);
		} else if (strcmp (key, "PACKAGE_NAME") == 0) {
			pd->package_name = (char *) malloc (strlen (val) + 1);
			if (pd->package_name == 0) {
				FreePACKAGEDATA (pd);
				free (line);
				return 0;
			}

			SAPDB_strcpy (pd->package_name, val);
		} else if (strcmp (key, "MODE") == 0) {
			pd->mode = (char *) malloc (strlen (val) + 1);
			if (pd->mode == 0) {
				FreePACKAGEDATA (pd);
				free (line);
				return 0;
			}

			SAPDB_strcpy (pd->mode, val);
		}

		/*
		 * drop line
		 */
		free (line);
	}
	
	if (pd == 0)
		return 0;

	if (pd->interface_version == 0) {
		FreePACKAGEDATA (pd);
		return 0;
	}

	return (pd);
}

/*
 * helper funtion for converting octal numbers
 * from tar header to integer numbers
 */
static int
strnocttoi (char *ptr, int count) {
	int val;
	char c;
  
	for (val = 0; count > 0; count--) {
		c = *ptr++;
		if (c == ' ')
			continue;
		if (c == '\0')
			break;
		val = (val << 3)  + (c - '0');
	}
  	return val;
}

/*
 * read PACKAGEDATA file form sdbinst archive
 * and return pointer to buffer
 */
static char *
ReadPACKAGEDATA (char *archive) {
	char *data;
	gzFile gzfp;
	char hdrbuff[512];
	tar_hdr_t *hdr;
	int want;
	int got;
	int size;
	
	data = 0;
	hdr = (tar_hdr_t *) hdrbuff;

	gzfp =  gzopen (archive, "rb");
	if (gzfp == 0)
		return 0;

	for (;;) {
		int i;
		int blocks;

		got = gzread (gzfp, hdr, 512);
		if (got != 512)
			break;

		if (memcmp (hdr->magic, TAR_MAGIC, sizeof (TAR_MAGIC)) != 0)
			break;
			
		size = strnocttoi (hdr->size, sizeof (hdr->size));
		blocks = (size % 512 == 0) ? size / 512 : (size / 512) + 1;

		/*
		 * skip any file not named 'PACKAGEDATA'
		 */
		if (strcmp (hdr->name, "PACKAGEDATA") != 0) {
			for (i = 0; i < blocks; i++) {
				char tmp[512];

				got = gzread (gzfp, tmp, 512);
				if (got != 512) {
					gzclose (gzfp);
					return 0;
				}
			}
		}

		if (size == 0)
			break;

		want = blocks * 512;
		if (want == 0)
			break;

		data = (char *) malloc (want + 1);
		if (data == 0)
			break;

		got = gzread (gzfp, data, want);
		if (got != want) {
			free (data);
			break;
		}

		data[size] = '\0';
		gzclose (gzfp);
		return (data);
	}

	gzclose (gzfp);
	return 0;
}

bool
cn44_PackageInfo (const char *dir, char *buff, int buffsize) {
	sdbinst_archive_name_t *an;
	sdbinst_archive_name_t *ptr;
	char errmsg[1024 + 1];
	char fmt[] = "%-20s  %-10s  %-4s\n";

	SAPDB_strcpy (errmsg, "");
	if (buff == 0)
		return 0;

	if (buffsize == 0)
		return 0;

	buff[0] = '\0';

	if (dir == 0 || strlen (dir) == 0) {
		char errmsg[] = "directory name missing\n";

		snprintf (buff, buffsize, errmsg);
		return 0;
	}
	
	an =  GetArchiveNames (dir, errmsg, 1024);
	if (an == 0) {
		if (errmsg != 0 && strlen (errmsg) != 0) {
			snprintf (buff, buffsize, errmsg);
			return 0;
		} else {
			char errmsg[] = "cannot find packages\n";

			snprintf (buff, buffsize, errmsg);
			return 0;
		}
	}

	snprintf (buff, buffsize, fmt, "PACKAGE_NAME", "VERSION", "MODE");

	ptr = an;
	for (;;) {
		packagedata_t *pd;
		char *data;
		int len;
		
		if (ptr == 0)
			break;

		data = ReadPACKAGEDATA (ptr->name); 

		if (data == 0) {
			ptr = ptr->next;
			continue;
		}

		pd = ParsePACKAGEDATA (data);
		free (data);

		len = (int) strlen (buff);

		PrintPACKAGEDATA (pd, buff + len, buffsize - len, fmt);
		FreePACKAGEDATA (pd);

		ptr = ptr->next;
	}

	FreeArchiveNames (an);
	return 1;
}

/*
 * free list of sdbinst archives
 */
static void
FreeArchiveNames (sdbinst_archive_name_t *curr) {
	sdbinst_archive_name_t *ptr;

	for (;;) {
		if (curr == 0)
			break;

		if (curr->name != 0)
			free (curr->name);

		ptr = curr;
		curr = curr->next;
		free (ptr);
	}
}

/*
 * get a list of all sdbinst archives located in a given directory
 */
static sdbinst_archive_name_t *
GetArchiveNames (const char *dirname, char *errmsg, int errlen) {
#ifdef UNIX
	char pathsep[] = "/";

	int i;
	char *normalized;
	DIR *dirp;
	struct dirent *de;

	sdbinst_archive_name_t *archives;
	sdbinst_archive_name_t *curr;

	archives = 0;	
	SAPDB_strcpy (errmsg, "");

	normalized = (char *) malloc
	(strlen (dirname) + strlen (pathsep) + 1);
	if (normalized == 0)
		return 0;

	SAPDB_strcpy (normalized, dirname);

	/* normalize path separators */
	for (i = 0; i < (int) strlen (normalized); i++) {
		if (normalized[i] == '\\') normalized[i] = '/';
	}

	/* remove trailing whitespaces */
	for (;;) {
		int len;
		char c;

		len = strlen (normalized);
		c = normalized[len - 1];
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
			break;

		normalized[len - 1] = '\0';
	}

	/* remove trailing separators */
	for (;;) {
		int len;

		len = strlen (normalized);
		if (normalized[len - 1] != '\\')
			break;

		normalized[len - 1] = '\0';
	}

	/* make sure that this is an absolute path */
	if (normalized[0] != '/')
		return 0;

	dirp = opendir (normalized);
	if (dirp == 0) {
		char msg[] = "cannot open directory %s, %s";
		snprintf (errmsg, errlen, msg, normalized, strerror(errno));
		free (normalized);
		return 0;
	}

	/* build normalized dirname */
	strcat (normalized, pathsep);

	for (;;) {
		struct stat statbuf[1];
		char *name;
		char *ptr;

		de = readdir (dirp);
		if (de == 0)
			break;

		if (strcmp (de->d_name, "SDBINST.TGZ") == 0)
			continue;

		if (strcmp (de->d_name, "sdbinst.tgz") == 0)
			continue;

		/* make shure that extention is 'tgz' or 'TGZ'*/
		ptr = strrchr (de->d_name, '.');
		if (ptr == 0)
			continue;

		ptr++;
		if ((strcmp (ptr, "TGZ") != 0) && (strcmp (ptr, "tgz") != 0))
			continue;

		/* build full name */
		name = (char *) malloc
		(strlen (normalized) + strlen (de->d_name) + 1);
		if (name == 0) {
			FreeArchiveNames (archives);
			free (normalized);
			closedir (dirp);
			return 0;
		}

		SAPDB_strcpy (name, normalized);
		strcat (name, de->d_name);

		/* make shure that this is a regular file */
		if (stat (name, statbuf) != 0) {
			char msg[] = "cannot stat %s, %s";
			snprintf (errmsg, errlen, msg, name, strerror(errno));
			FreeArchiveNames (archives);
			free (name);
			free (normalized);
			closedir (dirp);
			return 0;
		}

		if (!(statbuf->st_mode & S_IFREG)) {
			free (name);
			continue;
		}

		/* convert it in own format */
		curr = (sdbinst_archive_name_t *) malloc
		(sizeof (sdbinst_archive_name_t));
		if (curr == 0) {
			FreeArchiveNames (archives);
			free (normalized);
			closedir (dirp);
			return 0;
		}

		curr->next = 0;
		curr->name = name;

		if (archives == 0) {
			archives = curr;
		} else {
			sdbinst_archive_name_t *ptr;

			for (ptr = archives; ptr->next != 0; ptr = ptr->next);
			ptr->next = curr;
		}
	}

	free (normalized);
	closedir (dirp);
	return (archives);
#endif
#ifdef WIN32
	char mask[] = "*.TGZ";
	char pathsep[] = "\\";
	
	int i;
	char *want;
	char *normalized;
	WIN32_FIND_DATA de[1];
	HANDLE hDir;

	sdbinst_archive_name_t *archives;
	sdbinst_archive_name_t *curr;

	archives = 0;	
	SAPDB_strcpy (errmsg, "");

	want = (char *)	malloc
	(strlen (dirname) + strlen (pathsep) + strlen (mask) + 1);
	if (want == 0)
		return 0;

	/* start with dirname */
	SAPDB_strcpy (want, dirname);

	/* normalize path separators */
	for (i = 0; i < (int) strlen (want); i++) {
		if (want[i] == '/') want[i] = '\\';
	}

	/* remove trailing whitespaces */
	for (;;) {
		int len;
		char c;

		len = (int) strlen (want);
		c = want[len - 1];
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
			break;

		want[len - 1] = '\0';
	}

	/* remove trailing separators */
	for (;;) {
		int len;

		len = (int) strlen (want);
		if (want[len - 1] != '\\')
			break;

		want[len - 1] = '\0';
	}

	/* make sure that this is an absolute path */
	if (
	!(want[0] == '\\' && want[1] == '\\') &&
	!(want[0] >= 'A' && want[0] <= 'Z' && want[1] == ':' && want[2] == '\\') &&
	!(want[0] >= 'a' && want[0] <= 'z' && want[1] == ':' && want[2] == '\\'))
		return 0;

	/* build normalized dirname */
	strcat (want, pathsep);
	normalized = (char *) malloc (strlen (want) + 1);
	if (normalized == 0) {
		free (want);
		return 0;
	}
	SAPDB_strcpy (normalized, want);

	/* add search pattern */
	strcat (want, mask);

	/* read first entry and convert it to own format */
	hDir = FindFirstFile (want, de);
	if (hDir == INVALID_HANDLE_VALUE) {
		free (want);
		free (normalized);
		return 0;
	}
	free (want);

	if (stricmp (de->cFileName, "SDBINST.TGZ") != 0) {
		archives = (sdbinst_archive_name_t*)
		malloc (sizeof (sdbinst_archive_name_t));

		if (archives == 0) {
			free (normalized);
			FindClose (hDir);
			return 0;
		}

		archives->next = 0;
		archives->name = (char *)
		malloc (strlen (normalized) + strlen (de->cFileName) + 1);
		if (archives->name == 0) {
			free (archives);
			free (normalized);
			FindClose (hDir);
			return 0;
		}

		SAPDB_strcpy (archives->name, normalized);
		strcat (archives->name, de->cFileName);
	}

	/* read following entries and convert them to own format */
	while (FindNextFile (hDir, de) != 0) {
		if (stricmp (de->cFileName, "SDBINST.TGZ") == 0) 
			continue;

		curr = (sdbinst_archive_name_t*)
		malloc (sizeof (sdbinst_archive_name_t));

		if (curr == 0) {
			FreeArchiveNames (archives);
			free (normalized);
			FindClose (hDir);
			return 0;
		}

		curr->next = 0;
		curr->name = (char *) malloc
		(strlen (normalized) + strlen (de->cFileName) + 1);

		if (curr->name == 0) {
			FreeArchiveNames (archives);
			free (curr);
			free (normalized);
			FindClose (hDir);
			return 0;
		}

		SAPDB_strcpy (curr->name, normalized);
		strcat (curr->name, de->cFileName);

		if (archives == 0) {
			archives = curr;
		} else {
			sdbinst_archive_name_t *ptr;

			for (ptr = archives; ptr->next != 0; ptr = ptr->next);
			ptr->next = curr;
		}
	}

	free (normalized);
	FindClose (hDir);
	return (archives);
#endif
}
