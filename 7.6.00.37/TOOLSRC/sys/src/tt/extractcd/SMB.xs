#ifndef HAS_BOOL
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

typedef char bool;
#endif

#define DBG3
#define DBG9
#define DBG

#include <string.h>
#include <fcntl.h>
#include "smbglue.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#define SMBBUFSIZ (40960)

typedef struct con {
	int ch;
	void *cli;
	char *service;
	int has_pathinfo2;
	struct con *next;
} con_t;

typedef struct fh {
	int ch;
	int my_fh; 
	int their_fh;
	long offset;
	char *buff;
	int binmode;
	struct fh *next;
} fh_t;

typedef struct de {
	char *buffer;
	struct de *next;
} de_t;

typedef struct dh {
	int ch;
	int dh;
	struct de *dir;
	struct de *fetch;
	struct dh *next;
} dh_t;

static con_t* cl = 0;
static fh_t* fl = 0;
static dh_t* dl = 0;
static char *smbcli_user = 0;
static char *smbcli_pass = 0;
static char *smbcli_domain = 0;

static int dh_helper = 0;

static int cl_add (void *cli, char *service) {
	con_t *elem;
	con_t *ptr;
	con_t *last;
		
	DBG9 ("cl_add 0x%04x 0x%04x\n", cl, cli);

	elem = malloc (sizeof (con_t));	
	if (elem == 0) {
		return (-1);
	}
	DBG9 ("cl_add new=0x%04x\n", elem);

	elem->cli = cli;
	elem->service = malloc (strlen (service) + 1);
	if (elem->service == 0)	{
		return (-1);
	}
	strcpy (elem->service, service);		
	elem->has_pathinfo2 = 1;

	/* no elements at all */
	if (cl == 0) {
		elem->ch = 0;
		elem->next = 0;
		cl = elem;
		DBG9 ("cl_add first element, %d\n", elem->ch);
		return (elem->ch);
	}

	/* there are elements, but they do not start with ch == 0 */
	if (cl->ch != 0) {
		elem->ch = 0;
		elem->next = cl;
		cl = elem;
		DBG9 ("cl_add insert head, %d\n", elem->ch);
		return (elem->ch);
	}

	last = cl;
	ptr = cl->next;
	for (;;) {
		/* put it on the end */
		if (ptr == 0)
			break;
		
		/* fill empty space */
		if (1 + last->ch != ptr->ch)
			break;
	
		/* loop iteration */
		last = ptr;
		ptr = ptr->next;
	}
	
	elem->ch = last->ch + 1;
	elem->next = last->next;
	last->next = elem;
	DBG9 ("cl_add insert middle, %d\n", elem->ch);
	return (elem->ch);
}

int cl_del (int ch) {
	con_t *ptr;
	con_t *last;
	
	last = 0;
	ptr = cl;
	for (;;) {
		if (ptr == 0)
			return (-1);
		
		if (ptr->ch > ch)
			return (-1);
		
		if (ptr->ch == ch)
			break;
		
		last = ptr;
		ptr = ptr->next;	
	}
	
	if (last == 0) {
		cl = ptr->next;
	} else {
		last = ptr->next;
	}

	free (ptr);
	return 0;
}

static void *cl_get (int ch) {
	con_t *ptr;
	void *cli = 0;
	
	DBG9 ("cl_get 0x%04x, %d\n", cl, ch);
	ptr = cl;
	for (;;) {
		if (ptr == 0)
			break;
		
		if (ptr->ch > ch)
			break;
			
		if (ptr->ch == ch) {
			cli = ptr->cli;
			break;
		}
	
		ptr = ptr->next;
	}
	
	DBG9 ("cl_get 0x%04x\n", cli);
	return (cli);
}

con_t *cl_get_con (int ch) {
	con_t *ptr;
	
	DBG9 ("cl_get_con 0x%04x, %d\n", cl, ch);
	ptr = cl;
	for (;;) {
		if (ptr == 0)
			return 0;
		
		if (ptr->ch > ch)
			return 0;
	
		if (ptr->ch == ch)
			break;
	
		ptr = ptr->next;
	}
	return ptr;
}

con_t *cl_get_share (char *service) {
	con_t *ptr;
	void *cli = 0;
	
	ptr = cl;
	for (;;) {
		if (ptr == 0)
			break;
		
		if (strcmp (ptr->service, service) == 0) {
			cli = ptr->cli;
			break;
		}
	
		ptr = ptr->next;
	}
	
	DBG9 ("cl_get 0x%04x\n", cli);
	return (ptr);
}

static int dl_add (int ch) {
	dh_t *elem;
	dh_t *ptr;
	dh_t *last;
		
	DBG9 ("dl_add %d\n", ch);

	elem = malloc (sizeof (dh_t));	
	if (elem == 0) {
		DBG9 ("dl_add malloc failed\n");
		return (-1);
	}
	DBG9 ("dl_add new=0x%04x\n", elem);

	elem->ch = ch;
	elem->dir = 0;
	elem->fetch = 0;

	/* no elements at all */
	if (dl == 0) {
		elem->dh = 0;
		elem->next = 0;
		dl = elem;
		DBG9 ("dl_add first element, dh=%d\n", elem->dh);
		return (elem->dh);
	}

	/* there are elements, but they do not start with dh == 0 */
	if (dl->dh != 0) {
		elem->dh = 0;
		elem->next = dl;
		dl = elem;
		DBG9 ("dl_add insert head, dl=%d\n", elem->dh);
		return (elem->dh);
	}

	/* we have at least one element and we start with dh == 0 */	
	last = dl;
	ptr = dl->next;
	for (;;) {
		/* put it on the end */
		if (ptr == 0)
			break;
		
		/* fill empty space */
		if (1 + last->dh != ptr->dh)
			break;
	
		/* loop iteration */
		last = ptr;
		ptr = ptr->next;
	}
	
	elem->dh = last->dh + 1;
	elem->next = last->next;
	last->next = elem;
	DBG9 ("dl_add insert middle, dh=%d\n", elem->dh);
	return (elem->dh);
}

static int dl_del (int dh) {
	dh_t *p_dh;
	dh_t *last_dh;

	de_t *p_de;
	de_t *last_de;

	last_dh = 0;
	p_dh = dl;
	for (;;) {
		if (p_dh == 0)
			return (-1);
		
		if (p_dh->dh > dh)
			return (-1);
		
		if (p_dh->dh == dh)
			break;
		
		last_dh = p_dh;
		p_dh = p_dh->next;	
	}
	
	if (last_dh == 0) {
		dl = p_dh->next;
	} else {
		last_dh = p_dh->next;
	}

	p_de = p_dh->dir;
	while (p_de != 0) {
		last_de = p_de;
		p_de = p_de->next;

		free (last_de->buffer);
		free (last_de);
	}

	free (p_dh);
	return 0;
}

static dh_t *dl_get (int dh) {
	dh_t *ptr;
	
	DBG9 ("dl_get %d\n", dh);
	ptr = dl;
	for (;;) {
		if (ptr == 0)
			break;
		
		if (ptr->dh > dh)
			break;
			
		if (ptr->dh == dh) {
			break;
		}
	
		ptr = ptr->next;
	}
	
	DBG9 ("dl_get 0x%04x\n", ptr);
	return (ptr);
}

static dh_t *dl_get_ch (int ch) {
	dh_t *ptr = dl;

	for (;;) {
		if (ptr == 0)
			break;

		if (ptr->ch = ch)
			break;
		
		ptr = ptr->next;
	}
	return (ptr);
}

static int fl_add (int ch, int their_fh, char *buff) {
	fh_t *elem;
	fh_t *ptr;
	fh_t *last;
		
	DBG9 ("fl_add %d %d\n", ch, their_fh);

	elem = malloc (sizeof (fh_t));	
	if (elem == 0) {
		DBG9 ("fl_add malloc failed\n");
		return (-1);
	}
	DBG9 ("fl_add new=0x%04x\n", elem);

	elem->ch = ch;
	elem->their_fh = their_fh;
	elem->offset = 0;
	elem->binmode = 0;
	elem->buff = buff;

	/* no elements at all */
	if (fl == 0) {
		elem->my_fh = 0;
		elem->next = 0;
		fl = elem;
		DBG9 ("fl_add first element, %d\n", elem->my_fh);
		return (elem->my_fh);
	}

	/* there are elements, but they do not start with my_fh == 0 */
	if (fl->my_fh != 0) {
		elem->my_fh = 0;
		elem->next = fl;
		fl = elem;
		DBG9 ("fl_add insert head, %d\n", elem->my_fh);
		return (elem->my_fh);
	}

	/* we have at least one element and we start with my_fh == 0 */	
	last = fl;
	ptr = fl->next;
	for (;;) {
		/* put it on the end */
		if (ptr == 0)
			break;
		
		/* fill empty space */
		if (1 + last->my_fh != ptr->my_fh)
			break;
	
		/* loop iteration */
		last = ptr;
		ptr = ptr->next;
	}
	
	elem->my_fh = last->my_fh + 1;
	elem->next = last->next;
	last->next = elem;
	DBG9 ("fl_add insert middle, %d\n", elem->my_fh);
	return (elem->my_fh);
}

static int fl_del (int my_fh) {
	fh_t *ptr;
	fh_t *last;
	
	last = 0;
	ptr = fl;
	for (;;) {
		if (ptr == 0)
			return (-1);
		
		if (ptr->my_fh > my_fh)
			return (-1);
		
		if (ptr->my_fh == my_fh)
			break;
		
		last = ptr;
		ptr = ptr->next;	
	}
	
	if (last == 0) {
		fl = ptr->next;
	} else {
		last = ptr->next;
	}

	free (ptr->buff);
	free (ptr);
	return 0;
}

static fh_t *fl_get (int my_fh) {
	fh_t *ptr;
	
	DBG9 ("fl_get %d\n", my_fh);
	ptr = fl;
	for (;;) {
		if (ptr == 0)
			break;
		
		if (ptr->my_fh > my_fh)
			break;
			
		if (ptr->my_fh == my_fh) {
			break;
		}
	
		ptr = ptr->next;
	}
	
	DBG9 ("fl_get 0x%04x\n", ptr);
	return (ptr);
}

static fh_t *fl_get_ch (int ch) {
	fh_t *ptr = fl;

	for (;;) {
		if (ptr == 0)
			break;

		if (ptr->ch = ch)
			break;
		
		ptr = ptr->next;
	}
	return (ptr);
}

static char *fetch_dir (dh_t *p_dh) {
	char *ptr;

	if (p_dh->fetch == 0)
		return 0;

	ptr = p_dh->fetch->buffer;
	
	p_dh->fetch = p_dh->fetch->next;	

	return (ptr);
}

static int _shutdown_dh_by_con (int ch) {
	dh_t *p_dh;

	/* free any dirhandles used by this connection */
	for (;;) {	
		p_dh = dl_get_ch (ch);
		if (p_dh == 0)
			break;
		
		dl_del (p_dh->dh);
	}
	return 0;
}

int _shutdown_fh_by_con (int ch, void *cli) {
	fh_t *ptr;	

	/* free any filehandles used by this connection */
	for (;;) {
		ptr = fl_get_ch (ch);
		if (ptr == 0)
			break;

		smbcli_close (cli, ptr->their_fh);
		fl_del (ptr->my_fh);
	}
	return 0;
}

static int _shutdown_con (int ch) {
	void *cli;
	
	cli = cl_get (ch);
	if (cli == 0)
		return (-1);
	
	_shutdown_fh_by_con (ch, cli);
	_shutdown_dh_by_con (ch);
	
	smbcli_shutdown (cli);
	cl_del (ch);
	return 0;
}

static int _shutdown_all () {
	int ch;
	
	for (;;) {
		if (cl == 0)
			break;
	
		ch = cl->ch;
		_shutdown_con (ch); 
	}
	
	return 0;
}
	
static void _remove_cr (int binmode, char *buf) {
	char *ptr;
	int len;
	
	if (binmode != 0)
		return;

	for (;;) {
		ptr = strchr (buf, '\r');
		if (ptr == 0)
			break;	
		len = strlen (buf) + 1 + (buf - ptr);
		memcpy (ptr, ptr + 1, len);
	}
	return;
}

static SV* _deref_typeglob (SV * sv_ref) {
	SV *sv_any;
	
	if (SvROK(sv_ref) == 0) {
		printf ("filehandle has to be a reference\n");
		return 0;
	} 
	
	sv_any = (SvRV(sv_ref));
	if (SvTYPE(sv_any) != SVt_PVGV) {
		printf 
		("filehandle has to be a reference to a typeglob\n");
		return 0;
	}	
	
	return (GvSV ((GV *)sv_any));
}

static char *
split_filename (char *in, char *err) {
	char *filename;
	filename = in;
	
        if ((filename[0] != '\\') || (filename[1] != '\\')) {
		DBG3 ("split_filename mailformed filename\n");	
		sprintf (err, 
		"filename has to look like "
		"\\\\host\\share\\...\n");
                return 0;
	}

        filename += 2;
        filename = strchr (filename, '\\');
        if (!filename) {
		DBG3 ("split_filename no hostname\n");	
        	sprintf (err, "no hostname found\n");
                return 0;
        }

        filename++;
	filename = strchr (filename, '\\');
	if (!filename) {
		DBG3 ("split_filename no share\n");	
        	sprintf (err, "no share found\n");
		return 0;
	}

	*filename = 0;
	filename++;

	return (filename);
}

static int 
get_connection (char *share, void **p_cli, char *err) {
	con_t* con_ptr;
	void *cli;
	int ch;

	DBG3 ("get_connection called with shared=%s\n", share);	

	con_ptr = cl_get_share (share);
	if (con_ptr == 0) {
		/* do implicit connect  */
		DBG3 ("get_connection try connect and login\n");	
		if (smbcli_domain == 0) {
			cli = (void *) smbcli_connect (
				share, smbcli_user, smbcli_pass, "");
		} else {
			cli = (void *) smbcli_connect (
		   	share, smbcli_user, smbcli_pass, smbcli_domain);
		}
		if (cli == 0) {
			DBG3 ("get_connection logon and connet failed\n");	
			sprintf (err, "logon and connect failed\n");
			return (-1);
		}

		ch = cl_add (cli, share);
		if (ch < 0) {
			sprintf (err, "malloc connection failed\n");
			DBG3 ("get_connection malloc failed\n");	
			smbcli_shutdown (cli);
			return (-1);
		}
	} else {
		/* already connected */
		DBG3 ("get_connection already connected\n");	
		cli = con_ptr->cli;
		ch = con_ptr->ch;
	}

	DBG3 ("get_connection sucessful, ch=%d, cli=0x%08lx\n", ch, cli);	
	
	*p_cli = cli;
	return (ch);
}

static int close_smbfile (int fh) {
	fh_t *ptr;
	void *cli;
	
	ptr = fl_get (fh);
	if (ptr == 0) {
		return (-1);
	}

	cli = cl_get (ptr->ch);
	if (cli == 0) {
		return (-1);
	}
	
	smbcli_close (cli, ptr->their_fh);
	fl_del (fh);

	return 0;
}

static int open_smbfile 
(char *file, int buffersize, int access_mode, char *err) {
	char *share;
	void *cli;
	int ch;
	int their_fh;
	int my_fh;
	char *buff;

	share = file;	
	file = split_filename (share, err);
	if (file == 0) {
		return (-1);
	}
	
	ch = get_connection (share, &cli, err);
	if (ch < 0) {
		return (-1);
	}

	/* try to open */
	access_mode = O_RDONLY;
	their_fh = smbcli_open (cli, file, access_mode);

	if (buffersize > 0) {
		buff = malloc (buffersize + 1);
		if (buff == 0) {
			sprintf (err, "malloc buffer failed\n");
			return (-1);
		}
		memset (buff, 0, buffersize + 1);
	} else {
		buff = 0;
	}

	my_fh = fl_add (ch, their_fh, buff);
	if (my_fh < 0) {
		sprintf (err, "malloc filehandle failed\n");
		smbcli_close (cli, their_fh);
		return (-1);
	}

	return (my_fh);
}
	
static void list_helper (void *info, const char *mask) {
	char *name;
	dh_t *p_dh;
	de_t *p_de;
	de_t *last_de;

	name = smbcli_get_name_form_fileinfo (info);
	p_dh = dl_get (dh_helper);
	p_de = p_dh->dir;
	while (p_de != 0) {
		last_de = p_de;
		p_de = p_de->next;
	}	
	p_de = malloc (sizeof (de_t));
	p_de->next = 0;
	p_de->buffer = malloc (strlen (name) + 1);
	strcpy (p_de->buffer, name);

	if (p_dh->dir == 0) {
		p_dh->dir = p_de;
		p_dh->fetch = p_de;
	} else {
		last_de->next = p_de;
	}
}

static void 
smbcli_free () {
	if (smbcli_user != 0) {
		free (smbcli_user);
		smbcli_user = 0;
	}

	if (smbcli_pass != 0) {
		free (smbcli_pass);
		smbcli_pass = 0;
	}

	if (smbcli_domain != 0) {
		free (smbcli_domain);
		smbcli_domain = 0;
	}
}

MODULE = SMB		PACKAGE = SMB

PROTOTYPES: DISABLE

BOOT:
	smbcli_user = smbcli_pass = smbcli_domain = 0;
	smbcli_init ();

void 
END (...)
CODE:
	_shutdown_all ();
	smbcli_free ();	

int
logon_to_smb (...)
PREINIT:
	char *user;
	char *pass;
	char *domain;
CODE:
	if ((items < 2) || (items > 3)) {
		printf ("use SMB::logon (user, pass , <domain>)\n");	
		XSRETURN_IV(-1);
	}

	if (smbcli_user != 0)
		free (smbcli_user);
	if (smbcli_pass != 0)
		free (smbcli_pass);
	if (smbcli_domain != 0)
		free (smbcli_domain);
	smbcli_user = smbcli_pass = smbcli_domain = 0;

	user = (char *) SvPV(ST(0), PL_na);
	smbcli_user = malloc (strlen (user) + 1);
	strcpy (smbcli_user, user);

	pass = (char *) SvPV(ST(1), PL_na);
	smbcli_pass = malloc (strlen (pass) + 1);
	strcpy (smbcli_pass, pass);

	if (items == 3 ) {
		domain = (char *) SvPV(ST(2), PL_na);
		smbcli_domain = malloc (strlen (domain) + 1);
		strcpy (smbcli_domain, domain);
	}

	XSRETURN_IV(0);

void
stat (...)
PREINIT:
	SV *perl_dh;
	char *their_filename;
	char *filename;
	char *share;
	int ch;
	void *cli;
	char err[256];
	con_t *con;
	struct smbcli_stat stat_buff[1];
PPCODE:
	share = 0;
	switch (GIMME) {
	case G_SCALAR:
		if (items != 1) {
			printf ("SMB::stat (filename)\n");
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		their_filename = (char *) SvPV(ST(0), PL_na);
		if (their_filename == 0) {
			printf ("SMB::stat (filename)\n");
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		share = malloc (strlen (their_filename) + 1);
		if (share == 0) {
			printf ("SMB::stat (), malloc failed\n");
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		strcpy (share, their_filename);
		filename = split_filename (share, err);
		if (filename == 0) {
			printf ("SMB::stat %s\n", err);
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		ch = get_connection (share, &cli, err);
		if (ch < 0) {
			printf ("SMB::stat %s\n", err);
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		con = cl_get_con (ch);
		
		if (smbcli_stat (
		cli, filename, stat_buff, &(con->has_pathinfo2)) < 0) {
			PUSHs(sv_2mortal(&PL_sv_no)); break;
		}

		PUSHs(sv_2mortal(&PL_sv_yes));
		break;

	case G_ARRAY:
		if (items != 1) { 
			printf ("SMB::stat (filename)\n");
			break;
		}

		their_filename = (char *) SvPV(ST(0), PL_na);
		if (their_filename == 0) {
			printf ("SMB::stat (filename)\n");
			break;
		}

		share = malloc (strlen (their_filename) + 1);
		if (share == 0) {
			printf ("SMB::stat (), malloc failed\n");
			break;
		}

		strcpy (share, their_filename);
		filename = split_filename (share, err);
		if (filename == 0) {
			printf ("SMB::stat %s\n", err);
			break;
		}

		ch = get_connection (share, &cli, err);
		if (ch < 0) {
			printf ("SMB::stat %s\n", err);
			break;
		}
		
		con = cl_get_con (ch);
	
		if (smbcli_stat (
		cli, filename, stat_buff, &(con->has_pathinfo2)) < 0) {
			break;
		}

		PUSHs (sv_2mortal (newSViv (stat_buff->dev)));
		PUSHs (sv_2mortal (newSViv (stat_buff->ino)));
		PUSHs (sv_2mortal (newSViv (stat_buff->mode)));
		PUSHs (sv_2mortal (newSViv (stat_buff->nlink)));
		PUSHs (sv_2mortal (newSViv (stat_buff->uid)));
		PUSHs (sv_2mortal (newSViv (stat_buff->gid)));
		PUSHs (sv_2mortal (newSViv (stat_buff->rdev)));
		PUSHs (sv_2mortal (newSViv (stat_buff->size)));
		PUSHs (sv_2mortal (newSViv (stat_buff->a_time)));
		PUSHs (sv_2mortal (newSViv (stat_buff->m_time)));
		PUSHs (sv_2mortal (newSViv (stat_buff->c_time)));
		PUSHs (sv_2mortal (newSViv (stat_buff->blksize)));
		PUSHs (sv_2mortal (newSViv (stat_buff->blocks)));
		break;
	}
	
	if (share != 0) {
		free (share);
	}
	
	
int
diropen (...)
PREINIT:
	SV *perl_dh;
	char *dirname;
	char *share;
	char *mask;
	void *cli;
	int ch;
	int dh;
	char err[256];
CODE:
	if (items != 2) {
		printf 
		("SMB::diropen (dirhandle, dirname)\n");
		XSRETURN_NO;
	}

	perl_dh = _deref_typeglob (ST(0));
	if (perl_dh == 0)
		XSRETURN_NO;
 
	dirname = (char *) SvPV(ST(1), PL_na);

	share = malloc (strlen (dirname) + 3);
	strcpy (share, dirname);
	strcat (share, "\\*");
	
	mask = split_filename (share, err);
	if (mask == 0) {
		printf ("SMB::diropen %s\n", err);
		XSRETURN_NO;
	}
	
	ch = get_connection (share, &cli, err);
	if (ch < 0) {
		printf ("SMB::diropen %s\n", err);
		XSRETURN_NO;
	}
	DBG3 ("SMB::diropen ch=%d, cli=0x%08lx\n", ch, cli);
	
	dh = dl_add (ch);

	/* try to get directory list */
	dh_helper = dh;
	smbcli_list (cli, mask, (void *) list_helper);
	dh_helper = 0;
	free (share);

	sv_setiv (perl_dh, dh);
	XSRETURN_YES;

int
dirclose (...)
PREINIT:
	SV *perl_dh;
	int dh;
CODE:
	if (items != 1) {
		printf ("SMB::dirclose (dirhandle)\n");
		XSRETURN_NO;
	}	

	perl_dh = _deref_typeglob(ST(0));
	if (perl_dh == 0)
		XSRETURN_NO;

	dh = SvIV (perl_dh);
	dl_del (dh);

	XSRETURN_YES;

int
rewinddir (...)
PREINIT:
	SV *perl_dh;
	int dh;
	dh_t *p_dh;
CODE:
	if (items != 1) {
		printf ("SMB::rewinddir (dirhandle)\n");
		XSRETURN_NO;
	}	

	perl_dh = _deref_typeglob(ST(0));
	if (perl_dh == 0)
		XSRETURN_NO;

	p_dh = dl_get (SvIV (perl_dh));
	p_dh->fetch = p_dh->dir;

	XSRETURN_YES;

void
readdir (...)
PREINIT:
	SV *perl_dh;
PPCODE:
	if (GIMME == G_SCALAR) {
		DBG3 ("readdir(): scalar wanted\n");
		if ((items == 1) &&
		((perl_dh = _deref_typeglob (ST(0))) == 0)) {
			PUSHs (sv_newmortal());
		} else {
			char *ptr;
			ptr = fetch_dir (dl_get (SvIV (perl_dh)));
			if (ptr == 0) {
				PUSHs (sv_newmortal());
			} else {
				PUSHs(sv_2mortal(newSVpv(ptr, strlen (ptr))));
			}
		}
	} else if (GIMME == G_ARRAY) { 
		DBG3 ("readdir(): array wanted\n");
		if ((items == 1) && 
		((perl_dh = _deref_typeglob (ST(0))) != 0)) {
			dh_t *p_dh;
			char *ptr;
			p_dh = dl_get (SvIV (perl_dh));
			while ((ptr = fetch_dir (p_dh)) != 0) {
				PUSHs(sv_2mortal(newSVpv(ptr, strlen (ptr))));
			}
		}
	}
	

int
sysopen (...)
PREINIT:
	SV *perl_fh;
	char *filename;
	int my_fh;
	char err[256];
CODE:
	if ((items < 2) || (items > 4)) {
		printf 
		("SMB::sysopen (filehandle, filename, <flags>, <mode>)\n");
		XSRETURN_NO;
	}

	perl_fh = _deref_typeglob (ST(0));
	if (perl_fh == 0)
		XSRETURN_NO;
 
	filename = (char *) SvPV(ST(1), PL_na);
	
	my_fh = open_smbfile (filename, SMBBUFSIZ, O_RDONLY, err);
	if (my_fh < 0) {
		printf ("SMB::sysopen %s", err);
		close_smbfile (my_fh);
		XSRETURN_NO;
	}
	
	sv_setiv (perl_fh, my_fh);
	XSRETURN_YES;

int
close (...)
PREINIT:
	SV *perl_fh;
	int fh;
	fh_t *ptr;
	void *cli;
CODE:
	if (items != 1) {
		printf ("SMB::close (filehandle)\n");
		XSRETURN_NO;
	}	

	perl_fh = _deref_typeglob(ST(0));
	if (perl_fh == 0)
		XSRETURN_NO;

	fh = SvIV (perl_fh);

	if (close_smbfile (fh) < 0) {
		XSRETURN_NO;
	} 

	XSRETURN_YES;

int
sysread (...)
PREINIT:
	SV *perl_fh;
	int fh;
	int len;
	int offset;
	int got;
	void *cli;
	fh_t *ptr;	
	unsigned char *text;
CODE:
	if ((items < 3) && (items > 4)) {
		printf ("SMB::sysread (filehandle, scalar, len, <offset>)\n");
		XSRETURN_IV(-1);
	}

	perl_fh = _deref_typeglob(ST(0));
	if (perl_fh == 0) {
		DBG ("SMB::sysread could not dereference filehandle\n");
		XSRETURN_IV(-1);
	}
	
	fh = SvIV(perl_fh);

	len = SvIV(ST(2));
	if (len < 0) {
		DBG ("SMB::sysread len < 0\n");
		XSRETURN_IV(-1);	
	}

	ptr = fl_get (fh);
	if (ptr == 0) {
		DBG ("SMB::sysread unknowen filehandle\n");
		XSRETURN_IV(-1);
	}

	if (len == 0)
		XSRETURN_IV(0);	

	cli = cl_get (ptr->ch);	

	if (items == 4 ) {
		offset = SvIV(ST(3));
	} else {
		offset = ptr->offset;
	}

	text = malloc (len);
	if (text == 0) {
		DBG ("SMB::sysread malloc failed\n");
		XSRETURN_IV(-1);
	}

	got = smbcli_read (
	      cli, ptr->their_fh, (unsigned char *) text, len, offset);

	if (got <= 0) {
		free (text);
		DBG ("SMB::sysread could not read\n");
		XSRETURN_IV(-1);
	}

	ptr->offset += got;
	sv_setpvn (ST(1), (const char *) text, got);
	free (text);
	XSRETURN_IV(got);

int
binmode (...)
PREINIT:
	SV *perl_fh;
	int fh;
	fh_t *fptr;
CODE:
	if (items =! 1) {
		printf ("SMB::binmode (filehandle)\n");
		XSRETURN_NO;
	}
	
	perl_fh = _deref_typeglob(ST(0));
	if (perl_fh == 0) {
		XSRETURN_NO;
	}

	fh = SvIV(perl_fh);
	fptr = fl_get (fh);
	if (fptr == 0) {
		printf ("SMB::binmode file closed\n");
		XSRETURN_NO;
	}

	fptr->binmode = 1;
	XSRETURN_YES;

char *
getline (...)
PREINIT:
	SV *perl_fh;
	int fh;
	fh_t *fptr;
	char *buff;
	int len;
	char *ret;
	void *cli;
	char *ptr;
	int want;
	int got;
	int binmode;
CODE:
	if (items =! 1) {
		printf ("SMB::getline (filehandle)\n");
		XSRETURN_UNDEF;
	}

	perl_fh = _deref_typeglob(ST(0));
	if (perl_fh == 0) {
		XSRETURN_UNDEF;
	}

	fh = SvIV(perl_fh);
	fptr = fl_get (fh);
	if (fptr == 0)
		XSRETURN_UNDEF;

	binmode = fptr->binmode;	
	
	DBG3 ("SMB::getline fptr=0x%08x\n", fptr);	

	cli = cl_get (fptr->ch);
	if (cli == 0)
		XSRETURN_UNDEF;
	
	DBG3 ("SMB::getline cli=0x%08x\n", cli);	

	buff = fptr->buff;
	DBG3 ("SMB::getline buff=0x%08x\n", buff);	

	ptr = strchr (buff, '\n');
	DBG3 ("SMB::getline ptr=0x%08x\n", ptr);	
	DBG3 ("SMB::getline buff=0x%08x\n", buff);	

	if (ptr == 0) {
		/* try to fill buffer */
		ptr = buff + strlen(buff);
		want = SMBBUFSIZ - strlen(buff);

		DBG3 ("SMB::getline read ptr=0x%08x want=%d\n", ptr, want);	

		/* buffer already filled */
		if (want <= 0) {
			*buff = '\0';
			XSRETURN_UNDEF;
		}

		got = smbcli_read (cli, fptr->their_fh, 
		                   (unsigned char *) ptr, want, fptr->offset);

		DBG3 ("SMB::getline read got=%d\n", got);	
		DBG3 ("SMB::getline buff=0x%08x\n", buff);	
	
		/* is there some read error */
		if (got < 0) {
			*buff = '\0';
			XSRETURN_UNDEF;
		}
	
		/* some corrections */
		fptr->offset += got;
		ptr[got] = '\0';

		DBG3 ("SMB::getline set offset=%d\n", fptr->offset);	
		DBG3 ("SMB::getline strlen(buff)=%d\n", strlen(buff));	

		if (got == 0) {
			len = strlen (buff);
			/* no more text in buffer? */
			if (len == 0) 
				XSRETURN_UNDEF;

			/* last line, without \n */
			ret = malloc (len + 2);
			strcpy (ret, buff);
			ret[len] = '\n';
			ret[len + 1] = '\0';
			_remove_cr (binmode, ret);
			*buff = '\0';
			XST_mPV(0, ret);
			free (ret);	
			XSRETURN(1);
		}
	}
	
	ptr = strchr (buff, '\n');
	if (ptr == 0)
		XSRETURN_UNDEF;
	
	ptr++;
	len = ptr - buff;
	ret = malloc (len + 1);
	memcpy (ret, buff, len);
	ret[len] = '\0';
	memcpy (buff, ptr, SMBBUFSIZ - len + 1);
 	_remove_cr (binmode, ret);
	XST_mPV(0, ret);
	free (ret);	
	XSRETURN(1);

char *
read (...)
PREINIT:
	SV *perl_fh;
	int fh;
	fh_t *fptr;
	char *buff;
	int len;
	char *ret;
	void *cli;
	int want;
	int got;
	int pos;
	int binmode;
CODE:
	if ((items < 3) && (items > 4)) {
		printf ("SMB::read (filehandle, scalar, len, <offset>)\n");
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}

	perl_fh = _deref_typeglob(ST(0));
	if (perl_fh == 0) {
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}

	len = SvIV (ST(2));
	if (len < 0) {
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}	

	if (len > SMBBUFSIZ)
		len = SMBBUFSIZ;	
	
	len -= 1;

	fh = SvIV(perl_fh);
	fptr = fl_get (fh);
	if (fptr == 0) {
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}
	
	binmode = fptr->binmode;
	if (binmode != 0) {
		/* only allowed in ascii mode */
		printf ("SMB::read() not allowed in binmode\n");
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}
	
	cli = cl_get (fptr->ch);
	if (cli == 0) {
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	}
	
	buff = fptr->buff;
	while ((pos = strlen(buff)) < len) {
		/* try to fill buffer */
		want = SMBBUFSIZ - strlen(buff);

		/* buffer already filled */
		if (want <= 0) {
			*buff = '\0';
			sv_setpvn (ST(1), "", 0);
			XSRETURN_IV (0);
		}

		got = smbcli_read (cli, fptr->their_fh, 
			(unsigned char *)(buff + pos), want, fptr->offset);

		if (got < 0) {
			/* read error */
			*buff = '\0';
			sv_setpvn (ST(1), "", 0);
			XSRETURN_IV (0);

		} else if (got > 0) {
			/* some corrections  in buffer */
			fptr->offset += got;
			buff[pos + got] = '\0';
			_remove_cr (binmode, buff);

		} else {
			/* got nothing, end of file */
			break;
		}
	}

	pos = strlen (buff);
	if (len <= pos) {
		/* enouth bytes in buffer */	
		ret = (char *) malloc (len + 1);
		memcpy (ret, buff, len);
		ret[len] = '\0';
		memcpy (buff, buff + len, SMBBUFSIZ - len + 1);
		sv_setpvn (ST(1), ret, len);
		free (ret);
		XSRETURN_IV (len);
	} else if (pos == 0) {
		/* no bytes in buffer */
		sv_setpvn (ST(1), "", 0);
		XSRETURN_IV (0);
	} else {
		ret = (char *) malloc (pos + 1);
		strcpy (ret, buff);
		buff[0] = '\0';
		sv_setpvn (ST(1), ret, pos);
		free (ret);
		XSRETURN_IV (pos);
	}

int
copy_from_smb (...)
PREINIT:
	char *from;
	char *to;
	char *mode;
	int binmode;
	int smb_fh;
	int fs_fh;
	int their_fh;
	void *cli;
	int offset;
	int want;
	int got;
	char err[256];
	int mask;
CODE:
	if ((items < 2) && (items > 3)) {
		printf ("SMB::copy (from, to, <mode>)\n");
		XSRETURN_IV (0);
	}

	from = (char *) SvPV(ST(0), PL_na);
	to = (char *) SvPV(ST(1), PL_na);

	binmode = 0;
	if (items == 3) {
		mode = (char *) SvPV(ST(2), PL_na);
		if (strcmp (mode, "ascii") == 0) {
			binmode = 0;
		} else if (strcmp (mode, "binary") == 0) {
			binmode = 1;
		} else {
			printf (
			"SMB::copy(): mode must be \"ascii\" or \"binary\"\n");
			XSRETURN_IV (0);
		}
	}

	smb_fh = open_smbfile (from, 0, O_RDONLY, err);
	if (smb_fh < 0) {
		printf ("SMB::copy(): %s", err);
		XSRETURN_IV (0);
	}

	/* open output file */
	mask = umask (0);
	umask (mask);
	fs_fh = open (to, O_RDWR | O_CREAT, 0777 & ~mask) ;
	if (fs_fh < 0) {
		close_smbfile (smb_fh);
		printf ("SMB::copy(): could not open %s\n", to);
		XSRETURN_IV(0);
	}

	cli = cl_get ((fl_get (smb_fh))->ch);
	their_fh = (fl_get (smb_fh))->their_fh;
	offset = 0;

	for (;;) {
		char buff[32768 + 1];
		want = 32768;
		got = smbcli_read (
		cli, their_fh, (unsigned char *) buff, want, offset);
		if (got < 0) {
			close_smbfile (smb_fh);
			close (fs_fh);
			unlink (to);
			XSRETURN_IV(0);
		} else if (got == 0) {
			close_smbfile (smb_fh);
			close (fs_fh);
			XSRETURN_IV(1);
		}
	
		offset += got;	
		if (binmode == 0) {
			buff[got] = '\0';
			_remove_cr (0, buff);
			want = strlen (buff);
		} else {
			want = got;
		}

		got = write (fs_fh, buff, want);
		if (want != got) {
			close_smbfile (smb_fh);
			close (fs_fh);
			unlink (to);
			XSRETURN_IV(0);
		}
	}

	/* close smb connection */	
	close_smbfile (smb_fh);
	XSRETURN_IV(1);

void
getsmbpass (...)
PREINIT:
	char *prompt;
	char *pass;
PPCODE:
	if (items != 1) {
		printf ("use SMB::getsmbpass (prompt)\n");	
		XSRETURN_UNDEF;
	}

	prompt = (char *) SvPV(ST(0), PL_na);
	pass = smbcli_getpass (prompt);

	PUSHs (sv_2mortal (newSVpv (pass, strlen (pass))));
	XSRETURN (1);

