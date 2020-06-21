/*
 * smbglue.h 
 * cba 2000/07/26


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

#ifndef __SMBGLUE_H__
#define __SMBGLUE_H__ __SMBGLUE_H__

#include <sys/stat.h>

#ifndef SMBCLI_UINT64
#define SMBCLI_UNIT64 SMBCLI_UNIT64
#ifdef __lp64
typedef unsigned long int smbcli_uint64;
#else
typedef unsigned long long int smbcli_uint64;
#endif
#endif

#define SMBCLI_FILE_MODE (S_IFREG | 0444)
#define SMBCLI_DIR_MODE (S_IFDIR | 0555)

struct smbcli_stat {
	smbcli_uint64 dev;
	smbcli_uint64 ino;
	smbcli_uint64 mode;
	smbcli_uint64 nlink;
	smbcli_uint64 uid;
	smbcli_uint64 gid;
	smbcli_uint64 rdev;
	smbcli_uint64 size;
	smbcli_uint64 a_time;
	smbcli_uint64 m_time;
	smbcli_uint64 c_time;
	smbcli_uint64 blksize;
	smbcli_uint64 blocks;
};

extern int smbcli_init ();
extern void *smbcli_connect (char*, char*, char*, char*);
extern int smbcli_shutdown (void *);
extern int smbcli_open (void *, char*, int);
extern int smbcli_close (void *, int);
extern int smbcli_read (void *, int, unsigned char *, int, int);
extern int smbcli_list (void *cli, const char *mask, void *);
extern char *smbcli_get_name_form_fileinfo (void *);
extern int smbcli_stat (void *, const char *, struct smbcli_stat *, int *);
extern char *smbcli_getpass (char *);

#endif

