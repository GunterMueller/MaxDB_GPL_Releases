/*
 * RemoveFile.c
 *
 * $Header: //sapdb/V76/c_00/b_37/sys/src/SAPDB/InstallUtils/RemoveFile/RemoveFile.c#1 $
 * $DateTime: 2007/04/16 17:59:45 $
 * $Change: 149747 $
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

static char szWhatHeader[] = 
	"@(#) $Header: //sapdb/V76/c_00/b_37/sys/src/SAPDB/InstallUtils/RemoveFile/RemoveFile.c#1 $";

/*
 * sys_errlist specific defines
 */
extern char *sys_errlist[];

#ifndef __hpux
#error not yet implemented
#endif

#include <alloca.h>
#include <dirent.h>
#include <errno.h>
#include <malloc.h>
#include <mntent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
typedef struct stat stat_t;

#ifdef __hpux
#define DONT_DECLARE_STD
#endif

#define CAN_DIRNAME ".sapdb_deleted_files"

static char *getfsname (char *);
static char *getdirectory (char *);
static int isfsreadonly (char *);
static char *getuniquename (char *);
static int help (char *);
static int emptycan (int);
static int addtocan (char *, int);

static int
help (char *arg0) {
	printf ("%s -h | file0 file1 ...\n", arg0);
	printf ("%s [-v] file0 file1 ...\n", arg0);
	printf ("  -h --help          print this help\n");
	printf ("  -v --verbose       show detailed output\n");
	printf ("  file0 file1 ...    add file to deleted files directory\n");
	return 0;
}

static int
isfsreadonly (char *filename) {
	struct statvfs fssb[1];

	if (statvfs (filename, fssb) != 0)
		return 0;

	if (fssb->f_flag == ST_RDONLY)
		return 1;

	return 0;
}

static char *
getfsname (char *filename) {
	stat_t last_sb[1];
	stat_t sb[1];
	char *fsname;
	char cwd[1024 + 1];
	char mountpoint[1024 + 1];

	/* save current directory */
	if (getcwd (cwd, 1024) == 0)
		return 0;

	filename = getdirectory (filename);

	/* find directory where 'filename' is located */
	if (stat (filename, sb) != 0)
		return 0;

	if (S_ISDIR (sb->st_mode)) {
		/* it is already a directory */
		memcpy (last_sb, sb, sizeof (struct stat));
		if (chdir (filename) != 0)
			return 0;
	} else {
		char *dirname;
		char *ptr;

		dirname = alloca (strlen (filename) + 1);
		strcpy (dirname, filename);
	        ptr = strrchr (dirname, '/');
		if (ptr != 0) {
			*ptr = '\0';
		} else {
			strcpy (dirname, ".");
		}

		if (stat (dirname, sb) != 0 && chdir (cwd) != 0)
			return 0;

		memcpy (last_sb, sb, sizeof (stat_t));
		if (chdir (dirname) != 0)
			return 0;
	}

	/* walk up directory tree, until we come to an other filesystem */
	for (;;) {
		if (stat ("..", sb) != 0 && chdir (cwd) != 0)
			return 0;

		if (sb->st_dev != last_sb->st_dev || 
		    sb->st_ino == last_sb->st_ino)
			break;

		if (chdir ("..") != 0 && chdir (cwd) != 0)
			return 0;

		memcpy (last_sb, sb, sizeof (stat_t));
	}

	if (getcwd (mountpoint, 1024) == 0)
		return 0;

	if (chdir (cwd) != 0)
		return 0;

	fsname = malloc (strlen (mountpoint) + 1);
	strcpy (fsname, mountpoint);
	return (fsname);
}

static char *
getdirectory (char *filename) {
	struct stat sb[1];
	char *ptr;

	for (;;) {
		if (strlen (filename) == 0)
			break;

		if (stat (filename, sb) == 0)
			break;
	
		ptr = strrchr (filename, '/');
		if (ptr == 0)
			ptr = strrchr (filename, '\\');

		if (ptr == 0)
			break;
		
		*ptr = '\0';
	}

	return (filename);
}

static char *
getuniquename (char *dirname) {
	char *filename;
	int idx;
	struct stat sb[1];

	filename = malloc (strlen (dirname) + 1 + 12 + 1);
	if (filename == 0)
		return 0;

	for (idx = 0;; idx++) {
		int rc;

		sprintf (filename, "%s/%d", dirname, idx);

		rc = stat (filename, sb);
		if (rc == 0)
			continue;

		if (errno == ENOENT)
			break;

		free (filename);
		filename = 0;
		break;
	}

	return (filename); 
}

static int
addtocan (char *filename, int verbose) {
	struct stat sb[1];
	char *fsname;
	char *canname;
	char *delname;

	if (verbose)
		printf ("%s\n", filename);

	/* makes no sense on a read only filesystem */
	if (isfsreadonly (filename)) {
		if (verbose)
			printf ("  filesystem is read only\n");

		return 0;
	}

	/* we can simply remove this file if it is a symbolc link */
	if ((lstat (filename, sb) == 0) && S_ISLNK (sb->st_mode)) {
		if (verbose)
			printf ("  remove symbolic link\n");

		if (unlink (filename) != 0)
			return -1;

		return 0;
	}

	/* find filesystem root */
	fsname = getfsname (filename);
	if (fsname == 0)
		return 0;

	canname = alloca (strlen (fsname) + 1 + strlen (CAN_DIRNAME) + 1);
	strcpy (canname, fsname);
	strcat (canname, "/");
	strcat (canname, CAN_DIRNAME);
	free (fsname);

	/* create trashcan if it does not exist or it is not a directory */
	if (stat (canname, sb) != 0) {
		if (mkdir (canname, 0) != 0)
			return -1;
	}

	if (S_ISDIR (sb->st_mode) == 0) {
		if (unlink (canname) != 0)
			return -1;

		if (mkdir (canname, 0) != 0)
			return -1;
	}
	
	/* make trashcan owned by root:root and remove any access right */
	if (chown (canname, 0, 0) != 0)
		return -1;

	if (chmod (canname, 0) != 0)
		return -1;

	/* move file into trashcan */
	delname = getuniquename (canname);
	if (verbose)
		printf ("  move to deleted files directory\n");

	if (rename (filename, delname) != 0) {
		free (delname);
		printf ("    cannot move to %s\n", canname);
		return -1;
	}

	/* make trash owned by root:root and remove any access right */
	chown (delname, 0, 0);
	chmod (delname, 0);
	free (delname);
	return 0;
}

static int
rmdir (char *dirname, int verbose) {
	DIR *dp;
	struct dirent *de;
	struct stat sb[1];
	
	dp = opendir (dirname);
	if (dp == 0)
		return -1;

	for (;;) {
		char *filename;

		de = readdir (dp);
		if (de == 0)
			break;

		if (
		strcmp (de->d_name, ".") == 0 ||
		strcmp (de->d_name, "..") == 0)
			continue;

		filename =
		alloca (strlen (dirname) + 1 + strlen (de->d_name) + 1);
		strcpy (filename, dirname);
		strcat (filename, "/");
		strcat (filename, de->d_name);

		if (stat (filename, sb) != 0)
			continue;

		if (S_ISDIR (sb->st_mode)) {
			if (lstat (filename, sb) != 0)
				continue;

			if (S_ISLNK (sb->st_mode) == 0) {
				rmdir (filename, verbose);
				continue;
			}
		}

		if (verbose)
			printf ("    unlink %s\n", filename);

		if (unlink (filename) == 0)
			continue;

		if (verbose && errno == ETXTBSY)
			printf ("      text file busy\n");

		if (verbose && errno != ETXTBSY)
			printf ("      failed\n");
	}

	closedir (dp);

	if (verbose)
		printf ("    unlink %s\n", dirname);

	unlink (dirname);
	return 0;
}

static int
emptycan (int verbose) {
	FILE *fp;

	/* 
	 * read mnttab to find any mounted file system
	 * mnttab is updated by syncer (1M) periodicly every 30 seconds or
	 * by any call of mount (1M) and therefore it should be up to date
	 */
	fp = setmntent (MNT_MNTTAB, "r");
	if (fp == 0)
		return -1;

	if (verbose)
		printf ("clean deleted files directores\n");

	for (;;) {
		struct mntent *me;
		struct stat sb[1];
		char *canname;

		/* get next entry of mnttab */
		me = getmntent (fp);
		if (me == 0)
			break;

		if (verbose)
			printf ("  %s\n", me->mnt_dir);

		/* makes no sense on a read only filesystem */
		if (isfsreadonly (me->mnt_dir)) {
			if (verbose)
				printf ("    read only\n");

			continue;
		}

		/* find trashcan of the filesystem if there is any */
		if (strcmp (me->mnt_dir, "/") == 0) {
			canname	=
			alloca (1 + strlen (CAN_DIRNAME) + 1);
			strcpy (canname, "/");
			strcat (canname, CAN_DIRNAME);
		} else {
			canname	=
			alloca (strlen (me->mnt_dir) + 1 +
			        strlen (CAN_DIRNAME) + 1);
			strcpy (canname, me->mnt_dir);
			strcat (canname, "/");
			strcat (canname, CAN_DIRNAME);
		}

		if (stat (canname, sb) != 0) {
			if (verbose)
				printf ("    already clean\n");

			continue;
		}

		/* remove trashcan */
		if (S_ISDIR (sb->st_mode) == 0) {
			if (verbose)
				printf ("    unlink %s", canname);

			unlink (canname);
			continue;
		}
		
		rmdir (canname, verbose);
	}

	endmntent (fp);
	return 0;
}

int
main (int argc, char **argv) {
	int i;
	int num_of_files;
	int opt_verbose;

	opt_verbose = 0;
	num_of_files = 0;

	for (i = 1; i < argc; i++) {
		char *arg;

		arg = argv[i];
		if (arg[0] == '-') {
			if (
			strcmp (arg, "-h") == 0 ||
			strcmp (arg, "--help") == 0) {
				help (argv[0]);
				return 0;	
			} else if (
			strcmp (arg, "-v") == 0 ||
			strcmp (arg, "--verbose") == 0) {
				opt_verbose = 1;
			}
			continue;
		}

		/*
		 * only root can add files to trashcan,
		 * because a directory will be created in the root of the filesystem
		 */
		if (num_of_files == 0 && getuid () != 0) {
			printf ("only root can do that\n");
			return 0;
		}

		num_of_files++;
		addtocan (arg, opt_verbose);
	} 

	/* 
	 * only root can clean trashcans,
	 * because a directory will be removed from the root of the filesystem
	 */
	if (getuid () != 0) {
		printf ("only root can do that\n");
		return 0;
	}

	emptycan (opt_verbose);
	return 0;
}

