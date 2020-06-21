/*
 * smbglue.c 
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

#if defined LINUX && defined I386
#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#undef _FILE_OFFSET_BITS
#endif
#endif

#include "smbincl.h"
#include "smbglue.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG

static int smbcli_gmt_offset = 0;

static int smbcli_tm_diff (struct tm *tm_0, struct tm *tm_1) {
	int tm_0_y;
	int tm_1_y;
	int leap_days;
	int years;
	int days;
	int hours;
	int mins;
	int secs;

	tm_0_y = tm_0->tm_year + (1900 - 1);
	tm_1_y = tm_1->tm_year + (1900 - 1);
	leap_days = (tm_0_y/4 - tm_1_y/4) 
	          - (tm_0_y/100 - tm_1_y/100)
	          + (tm_0_y/400 - tm_1_y/400);
	years = tm_0_y - tm_1_y;
	days = 365 * years + leap_days + (tm_0->tm_yday - tm_1->tm_yday);
	hours = 24 * days + (tm_0->tm_hour - tm_1->tm_hour);
	mins = 60 * hours + (tm_0->tm_min - tm_1->tm_min);
	secs = 60 * mins + (tm_0->tm_sec - tm_1->tm_sec);
	
	return secs;
}

static int smbcli_timezone  (time_t t) {
	struct tm *tm;
	struct tm tm_utc;
	struct tm tm_local;

	tm = gmtime (&t);
	if (tm == 0)
		return 0;
	
	tm_utc = *tm;

	tm = localtime (&t);
	if (tm == 0)
		return 0;

	tm_local = *tm;
	
	return (smbcli_tm_diff (&tm_utc, &tm_local));
}

int smbcli_init () {
	DEBUG ("smbglue.c sizeof (off_t)=%d\n", sizeof(off_t));
	
	smbcli_gmt_offset = smbcli_timezone (time (0));

	TimeInit();
	charset_initialise ();
	load_interfaces ();
	return 0;
}

void *smbcli_connect (char *service, char *user, char *pass, char *domain) {

	char *_service;
	char *share;
	char *server;
	struct cli_state *cli;
	struct nmb_name called;
	struct nmb_name calling;
	struct in_addr ip;
	char myhostname[256];
	char name_type = 0x20;
	int port = SMB_PORT;

	get_myname (myhostname, 0);

	/* split server and share from service name */
	_service = (char *) malloc (strlen (service) + 1); 
	strcpy (_service, service);

	share = _service;
	if (*share != '\\') {
		free (_service);
		return 0;
	}

	server = share + 2;
	share = strchr (server, '\\');
	if (!share) {
		free (_service);
		return 0;
	}

	*share = 0;
	share++;

	DEBUG ("smbcli_connect() got server %s share %s\n", server, share);

	make_nmb_name (&calling, myhostname, 0, "");
	make_nmb_name (&called , server, name_type, "");

        /* try to open a session */
        for (;;) {
                memset (&ip, 0, sizeof (ip));

                cli = cli_initialise (0);
                if (cli == 0) {
			free (_service);
			DEBUG ("smbcli_connect() cli_initialize failed\n");
                        return 0;
		}

                if (cli_set_port (cli, port) == 0) {
			free (_service);
			DEBUG ("smbcli_connect() cli_set_port failed\n");
                        return 0;
		}
	
                if (cli_connect (cli, server, &ip) == 0) {
			free (_service);
			DEBUG ("smbcli_connect() cli_connect failed\n");
                        return 0;
		}

                if (cli_session_request (cli, &calling, &called) != 0)
                        /* got requested session */
                        break;

                cli_shutdown (cli);
                if (!strcmp (called.name, "*SMBSERVER")) {
			free (_service);
                        return 0;
		}

                make_nmb_name (&called , "*SMBSERVER", 0x20, "");
        }
	DEBUG ("smbclie_connect() session connected\n");
        
	/* negoate best protocol */
        if (cli_negprot (cli) == 0) {
                cli_shutdown (cli);
		free (_service);
                return 0;
        }
	DEBUG ("smbclie_connect() session negoated\n");

        /* logon */
        if (cli_session_setup (cli, user, pass, strlen (pass),
                               pass, strlen (pass), domain) == 0) {
                cli_shutdown (cli);
		free (_service);
                return 0;
        }
	DEBUG ("smbclie_connect() session loged on\n");

        /* tree-connect to a service */
        if (cli_send_tconX (cli, share, "?????",
                            pass, strlen (pass) + 1) == 0) {
                cli_shutdown (cli);
		free (_service);
                return 0;
        }
	DEBUG ("smbclie_connect() tree connected\n");
        
	free (_service);
	return cli;
}

int smbcli_shutdown (void *cli) {
	cli_shutdown (cli);
	DEBUG ("smbcli_shutdown()\n");
	return 0;
} 

int smbcli_open (void *cli, char *name, int mode) {
	int rc;
	
	rc = cli_open (cli, name, mode, DENY_NONE);
	DEBUG ("smbcli_open() returned %d\n", rc);
	return (rc);
}

int smbcli_close (void *cli, int handle) {
	cli_close (cli, handle);
        DEBUG ("smbcli_close()\n");
	return 0;
}

int smbcli_read (void *cli, int fh, unsigned char *text, int len, int offset) {
	int got;
	
	got = cli_read (cli, fh, (char *) text, offset, len);
	return (got);
}

int smbcli_list (void *cli, const char *mask, void *fn) {
	typedef void (* fn_t)(file_info *, const char*);

	return (cli_list (cli, mask, aDIR | aSYSTEM | aHIDDEN, (fn_t) fn));
}

char *smbcli_get_name_form_fileinfo (void *info) {
	file_info *fi;
	
	fi = (file_info *) info;
	return (fi->name);
}

static smbcli_uint64 d2u_nlink (uint16 dos_mode) {
	if (IS_DOS_DIR (dos_mode)) {
		return 2;
	} else {
		return 1;
	}
}

static smbcli_uint64 d2u_mode (uint16 dos_mode) {
	smbcli_uint64 mode;
	
	if (IS_DOS_DIR (dos_mode)) {
		mode = SMBCLI_DIR_MODE;
	} else {
		mode = SMBCLI_FILE_MODE;
	}

	if (IS_DOS_ARCHIVE (dos_mode))
		mode |= S_IXUSR;

	if (IS_DOS_SYSTEM (dos_mode))
		mode |= S_IXGRP;

	if (IS_DOS_HIDDEN (dos_mode))
		mode |= S_IXOTH;

	if (!IS_DOS_READONLY (dos_mode))
		mode |= S_IWUSR;

	return (mode);
}

int smbcli_stat (
void *cli, const char *filename, 
struct smbcli_stat *buf, int *p_has_pathinfo2) {
	uint16 mode;
	size_t size;
	time_t c_time, a_time, m_time;
	SMB_INO_T ino; 
	
	if (*p_has_pathinfo2 == 1) {
		if (cli_qpathinfo2 (
		cli, filename, &c_time, &a_time, &m_time, 
		NULL, &size, &mode, &ino)) {
			c_time += smbcli_gmt_offset;
			a_time += smbcli_gmt_offset;
			m_time += smbcli_gmt_offset;
			buf->c_time = c_time;
			buf->a_time = a_time;
			buf->m_time = m_time;
			buf->size = size;
			buf->dev = 0;
			buf->rdev = 0;
			buf->ino = 0;
			buf->blksize = 512;
			buf->blocks = (size+511) / 512;
			buf->uid = getuid ();
			buf->gid = getgid ();
			buf->nlink = d2u_nlink (mode);
			buf->mode = d2u_mode (mode);
			return 0;
		}
	}	

	if (cli_getatr(cli, filename, &mode, &size, &m_time)) {
		*p_has_pathinfo2 = 0;
		m_time += smbcli_gmt_offset;
		buf->c_time = buf->a_time = buf->m_time = m_time;
		buf->size = size;
		buf->dev = 0;
		buf->rdev = 0;
		buf->ino = 0;
		buf->blksize = 512;
		buf->blocks = (size+511) / 512;
		buf->uid = getuid ();
		buf->gid = getgid ();
		buf->nlink = d2u_nlink (mode);
		buf->mode = d2u_mode (mode);
		return 0;
	}
	
	return -1;
}

char *smbcli_getpass (char *prompt) {
	char *pass;
	pass = getsmbpass (prompt);

	return (pass);
}

