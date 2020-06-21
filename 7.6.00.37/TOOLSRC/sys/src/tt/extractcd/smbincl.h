/*
 * smbincl.h
 *
 * cba 2000/07/26
 * typedefs and prototypes got from samba


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

#ifndef __SMBINCL_H__
#define __SMBINCL_H__ __SMBINCL_H__

#ifdef AIX
#define _LARGE_FILES
#endif

#if defined LINUX && defined I386
#define HAVE_OFF64_T 1
#define HAVE_EXPLICIT_LARGEFILE_SUPPORT 1
typedef long long int off64_t;
#endif

#include <sys/types.h>
#include <netinet/in.h>

#define SMB_PORT 139

/* deny modes */
#define DENY_DOS 0
#define DENY_ALL 1
#define DENY_WRITE 2
#define DENY_READ 3
#define DENY_NONE 4
#define DENY_FCB 7

/* DOS attributes */
#define aRONLY (1L<<0)
#define aHIDDEN (1L<<1)
#define aSYSTEM (1L<<2)
#define aVOLID (1L<<3)
#define aDIR (1L<<4)
#define aARCH (1L<<5)

/* test for DOS attributes */
#define IS_DOS_READONLY(test_mode) (((test_mode) & aRONLY) != 0)
#define IS_DOS_DIR(test_mode) (((test_mode) & aDIR) != 0)
#define IS_DOS_ARCHIVE(test_mode) (((test_mode) & aARCH) != 0)
#define IS_DOS_SYSTEM(test_mode) (((test_mode) & aSYSTEM) != 0)
#define IS_DOS_HIDDEN(test_mode) (((test_mode) & aHIDDEN) != 0)

#ifndef SMB_OFF_T
#if defined HAVE_OFF64_T && defined HAVE_EXPLICIT_LARGEFILE_SUPPORT
#define SMB_OFF_T off64_t
#else
#define SMB_OFF_T off_t
#endif
#endif

#ifndef SMB_DEV_T
#define SMB_DEV_T dev_t
#endif

#ifndef SMB_INO_T
#define SMB_INO_T ino_t
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned short int
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef int32
#define int32 signed int
#endif

typedef int BOOL;
typedef char pstring[1024];
typedef char fstring[128];

struct nmb_name {
  char         name[17];
  char         scope[64];
  unsigned int name_type;
};

/* DOM_CHAL - challenge info */
typedef struct chal_info {
  uchar data[8]; /* credentials */
} DOM_CHAL;

/* 32 bit time (sec) since 01jan1970 - cifs6.txt, section 3.5, page 30 */
typedef struct time_info {
  uint32 time;
} UTIME;

/* DOM_CREDs - timestamped client or server credentials */
typedef struct cred_info {
  DOM_CHAL challenge; /* credentials */
  UTIME timestamp;    /* credential time-stamp */
} DOM_CRED;

struct pwd_info {
    BOOL null_pwd;
    BOOL cleartext;
    BOOL crypted;

    fstring password;

    uchar smb_lm_pwd[16];
    uchar smb_nt_pwd[16];

    uchar smb_lm_owf[24];
    uchar smb_nt_owf[24];
};

struct cli_state {
        int port;
        int fd;
        uint16 cnum;
        uint16 pid;
        uint16 mid;
        uint16 vuid;
        int protocol;
        int sec_mode;
        int rap_error;
        int privileges;

        fstring eff_name;
        fstring desthost;
        fstring user_name;
        fstring domain;

        /*
         * The following strings are the
         * ones returned by the server if
         * the protocol > NT1.
         */
        fstring server_type;
        fstring server_os;
        fstring server_domain;

        fstring share;
        fstring dev;
        struct nmb_name called;
        struct nmb_name calling;
        fstring full_dest_host_name;
        struct in_addr dest_ip;

        struct pwd_info pwd;
        unsigned char cryptkey[8];
        uint32 sesskey;
        int serverzone;
        uint32 servertime;
        int readbraw_supported;
        int writebraw_supported;
        int timeout; /* in milliseconds. */
        int max_xmit;
        int max_mux;
        char *outbuf;
        char *inbuf;
        int bufsize;
        int initialised;
        int win95;
        uint32 capabilities;

        /*
         * Only used in NT domain calls.
         */

        uint32 nt_error;                   /* NT RPC error code. */
        uint16 nt_pipe_fnum;               /* Pipe handle. */
        unsigned char sess_key[16];        /* Current session key. */
        unsigned char ntlmssp_hash[258];   /* ntlmssp data. */
        uint32 ntlmssp_cli_flgs;           /* ntlmssp client flags */
        uint32 ntlmssp_srv_flgs;           /* ntlmssp server flags */
        uint32 ntlmssp_seq_num;            /* ntlmssp sequence number */
        DOM_CRED clnt_cred;                /* Client credential. */
        fstring mach_acct;                 /* MYNAME$. */
        fstring srv_name_slash;            /* \\remote server. */
        fstring clnt_name_slash;           /* \\local client. */
        uint16 max_xmit_frag;
        uint16 max_recv_frag;

        BOOL use_oplocks; /* should we use oplocks? */
};

typedef struct file_info {
        SMB_OFF_T size;
        uint16 mode;
        uid_t uid;
        gid_t gid;
        time_t mtime;
        time_t atime;
        time_t ctime;
        pstring name;
} file_info;

void TimeInit ();
void charset_initialise ();
void load_interfaces ();

struct cli_state *cli_initialise (struct cli_state *);
size_t cli_read (struct cli_state *cli, 
                 int fnum, char *buf, off_t offset, size_t size);
int cli_list (struct cli_state *cli, 
		const char *mask, uint16 attribute, void (*fn)(file_info *, const char *));

char *getsmbpass (char *);

#endif

