/*!**********************************************************************

  module: WAHTTP_FileAgent.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: SAP DB WWW

  description:  File management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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





************************************************************************/

#define MODULE_NAME "WAHTTP_FileAgent.c"

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gwd00.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Agent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_FileAgent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#include "hwd29date.h"

typedef tsp00_Int4	My_FILE;

#define FILE_EXTENSION_SEPARATOR	'.'
#define DEFAULTMIMETYPE			"application/octet-string"

#define ERR_404	"\
<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\
<HTML><HEAD>\
<TITLE>404 Not Found</TITLE>\
</HEAD><BODY>\
<H1>Not Found</H1>\
The requested URL %s was not found on this server.<P>\
<HR>\
<ADDRESS>%s Server at %s Port %s</ADDRESS>\
</BODY></HTML>"

#define HTTP_STATUS_NOT_FOUND		"404 Not Found"

struct st_agent_client
{
 char	DocumentRoot[MAXURISIZE+1];
 char	DefaultDocument[MAXURISIZE+1];
};

static size_t ourFread
(
 struct st_data *This,
 void		*Buf,
 size_t		Lg
)
{
 return(fread(Buf, 1, Lg, This->H));
}

static size_t ourFrewind
(
 struct st_data *This
)
{
 return(fseek(This->H, 0, SEEK_SET));
}

static int ourFclose
(
 struct st_data *This
)
{
 return(This->H && fclose(This->H));
}

int InitFileAgent
(
 struct st_agent_server *as,
 struct st_agent_client	**fah
)
{
 char			errtxt[MAX_ERRORTEXT_LG];
 tsp00_Bool		bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_agent_client),
		  fah,
		  &bOk);
 if (!bOk)
	return(FALSE);

 strncpy( (*fah)->DocumentRoot, GetDocumentRoot(), MAXURISIZE );
 (*fah)->DocumentRoot[MAXURISIZE] = '\0';

 strncpy( (*fah)->DefaultDocument, GetDefaultDocument(), MAXURISIZE );
 (*fah)->DefaultDocument[MAXURISIZE] = '\0';

/*
 if (!IniReadPar(INI_DOC_ROOT, sizeof((*fah)->DocumentRoot), (*fah)->DocumentRoot, 0, NULL))
	(*fah)->DocumentRoot[0] = '\0';
*/

 sprintf(errtxt, "DocumentRoot is: %s", (*fah)->DocumentRoot);
 Log_Error(errtxt, -1);
 return(TRUE);
}

int ExitFileAgent
(
 struct st_agent_server *as,
 struct st_agent_client	*fah
)
{
 wahttp_SQLFREE(fah);
 return(TRUE);
}

static int FileNotFound
(
 char			*uri,
 char			*host,
 char			*port,
 struct st_agent_server *as,
 struct st_agent_rep	*rep
)
{
 time_t			now;
 char			*tmp;
 tsp00_Bool		bOk;
 unsigned long		tmplg;
 char			errtxt[100];

 if (uri!=NULL)
	sp77sprintf( errtxt, sizeof(errtxt), "%s %s", HTTP_STATUS_NOT_FOUND, uri);
 else
	sp77sprintf( errtxt, sizeof(errtxt), "%s", HTTP_STATUS_NOT_FOUND);
 Log_Error(errtxt, -1);

 as->AddHeader_char(rep, AGENT_SERVER, "WapiAgentServer");
 time( &now );
#ifdef WIN32
 as->AddHeader_date(rep, AGENT_DATE, now + _timezone);
#else
 as->AddHeader_date(rep, AGENT_DATE, now + timezone);
#endif
/* AddHeader_char(&rep, CONTENT_TYPE, DEFAULTMIMETYPE);*/
 as->SendHeader(rep, AGENT_STATUS_NOT_FOUND);

 if (host!=NULL && port!=NULL && uri!=NULL) {
	wahttp_SQLALLOCAT( sizeof(ERR_404)-1 + strlen(uri) + strlen(THIS_SERVER) + strlen(host) + strlen(port),
			&tmp,
			&bOk);
	if (bOk)
	{
		tmplg = sprintf(tmp, ERR_404, uri, THIS_SERVER, host, port);
		as->Send(rep, tmp, tmplg);
	}
	else
		as->Send(rep, ERR_404, sizeof(ERR_404)-1);
	if (bOk)
		wahttp_SQLFREE(tmp);
 } else {
	as->Send(rep, ERR_404, sizeof(ERR_404)-1);
 }

 return(TRUE);
}

static int GetIfModifiedSince
(
 struct st_agent_server *as,
 struct st_agent_req	*req,
 time_t			*Value
)
{
 char	*s = as->GetHeaderValue(req, AGENT_IF_MODIFIED_SINCE);
 struct tm when;
 char	dayname[4], monname[4], TZ[4];

 if (!s)
	return(FALSE);

 sscanf(s, "%3s, %d %3s %d %d:%d:%d %3s", dayname, &when.tm_mday, monname, &when.tm_year, &when.tm_hour, &when.tm_min, &when.tm_sec, &TZ);
 if (!strcasecmp(monname, "jan"))
	when.tm_mon = 0;
 else
 if (!strcasecmp(monname, "feb"))
	when.tm_mon = 1;
 else
 if (!strcasecmp(monname, "mar"))
	when.tm_mon = 2;
 else
 if (!strcasecmp(monname, "apr"))
	when.tm_mon = 3;
 else
 if (!strcasecmp(monname, "may"))
	when.tm_mon = 4;
 else
 if (!strcasecmp(monname, "jun"))
	when.tm_mon = 5;
 else
 if (!strcasecmp(monname, "jul"))
	when.tm_mon = 6;
 else
 if (!strcasecmp(monname, "aug"))
	when.tm_mon = 7;
 else
 if (!strcasecmp(monname, "sep"))
	when.tm_mon = 8;
 else
 if (!strcasecmp(monname, "oct"))
	when.tm_mon = 9;
 else
 if (!strcasecmp(monname, "nov"))
	when.tm_mon = 10;
 else
 if (!strcasecmp(monname, "dec"))
	when.tm_mon = 11;

 when.tm_year -= 1900;
 when.tm_isdst = 0;
 return(-1 != (*Value = mktime(&when)));
}

static int FileFound
(
 char			*path,
 struct st_agent_server *as,
 struct st_agent_req	*req,
 struct st_agent_rep	*rep,
 My_FILE			fin
)
{
 char			pathExtension[MAXURISIZE+1];
 char			MimeType[100];
 char			ErrorText[MAX_ERRORTEXT_LG];
 time_t			ims;
 char			b[1000];
 tsp00_Longint		rlg;
 size_t			slg;
 time_t			lm;
 tsp05_RteFileInfo	FileStat;
 tsp05_RteFileError	ferror;
 char			http_date[100];

#ifdef was
 char			date[9];
 char			time[9];
#else
 tsp00_Date		date;
 tsp00_Time		time;
#endif

 char			dateBuf[18 + 1];
 char			*MimeTypeP;
 char			logText[MAX_ERRORTEXT_LG+1];
    
 size_t			fs = sizeof(time);

/* Get info about the resource */

 sqldattime( date, time );
 strncpy(dateBuf, (char*)date, 8);
 strncpy(dateBuf+8, (char*)time+2, 6);
 dateBuf[14] = '\0';
 if (wd29WADateToHttpDate( dateBuf, http_date ))
	as->AddHeader_char(rep, AGENT_DATE, http_date);
 else
 {
	char	tmp[100];
	sp77sprintf( tmp, sizeof(tmp)-1, "No HttpDate, wd29WADateToHttpDate failed: dateBuf=%s", dateBuf );
	Log_Error(tmp, -1);
 }

 as->AddHeader_char(rep, AGENT_SERVER, THIS_SERVER);

 sqlfinfoc (path, &FileStat, &ferror);
 if (ferror.sp5fe_result == vf_ok)
 {
	struct tm TM;

	strncpy(dateBuf, (char*)FileStat.sp5fi_date_modified, 8);
	strncpy(dateBuf+8, (char*)FileStat.sp5fi_time_modified, 6);
	dateBuf[14] = '\0';
	wd29WADateToHttpDate( dateBuf, http_date );

	memset(&TM, 0, sizeof(TM));
	TM.tm_year = 1000 * (FileStat.sp5fi_date_modified[0] - '0') +
		      100 * (FileStat.sp5fi_date_modified[1] - '0') +
		       10 * (FileStat.sp5fi_date_modified[2] - '0') +
			     FileStat.sp5fi_date_modified[3] - '0' -
		     1900;
	TM.tm_mon =    10 * (FileStat.sp5fi_date_modified[4] - '0') +
		             FileStat.sp5fi_date_modified[3] - '0';
        TM.tm_mday =   10 * (FileStat.sp5fi_date_modified[6] - '0') +
			     FileStat.sp5fi_date_modified[7] - '0';
        TM.tm_hour =   10 * (FileStat.sp5fi_time_modified[0] - '0') +
			     FileStat.sp5fi_time_modified[1] - '0';
        TM.tm_min =    10 * (FileStat.sp5fi_time_modified[2] - '0') +
			     FileStat.sp5fi_time_modified[3] - '0';
        TM.tm_sec =    10 * (FileStat.sp5fi_time_modified[4] - '0') +
			     FileStat.sp5fi_time_modified[5] - '0';;

	lm = mktime(&TM);

#ifdef BIT64
/*	fs = (FileStat.fi_size_x * csp_maxlongint) + FileStat.fi_size;*/
	fs = FileStat.sp5fi_size;
#else
	fs = FileStat.sp5fi_size;
#endif
 }
 else
	lm = -1;

 if (!GetIfModifiedSince(as, req, &ims))
	ims = -1;

 if (ims > 0 && ims >= lm)
	as->SendHeader(rep, AGENT_STATUS_NOT_MODIFIED);
 else
 {
	if (lm > 0)
	{
		as->AddHeader_char(rep, AGENT_LAST_MODIFIED, http_date);
		as->AddHeader_size_t(rep, AGENT_CONTENT_LENGTH, fs);
	}

	Basename(pathExtension, path, FILE_EXTENSION_SEPARATOR);
	if (IniGetMimeType(sizeof(MimeType), MimeType,
		    pathExtension,
		    sizeof(ErrorText), ErrorText))
		MimeTypeP = MimeType;
	else {
		MimeTypeP = DEFAULTMIMETYPE;

		sp77sprintf( logText, MAX_ERRORTEXT_LG, "Mime type not found: %s", pathExtension );
		Log_Error( logText, -1);
	}

	as->AddHeader_char(rep, AGENT_CONTENT_TYPE, MimeTypeP);
	as->SendHeader(rep, AGENT_STATUS_OK);
	do
	{
		sqlfreadc (fin, b, sizeof(b), &rlg, &ferror);
		if (ferror.sp5fe_result == vf_ok && rlg > 0)
			if (0 == (slg = as->Send(rep, b, rlg)))
				break;
	} while (ferror.sp5fe_result == vf_ok && rlg > 0);
 }

 return(TRUE);
}

/* ==========================================================================*/

enum en_af_returncode TryFileAgent( struct st_agent_server  *as,
                                    struct st_agent_client	*fah,
                                    struct st_agent_req	*req,
                                    struct st_agent_rep	*rep )
{

    size_t               UriLg = strlen(req->uri);
    char                *s;
    char                *tmpstr;
    size_t               rlg, lg, tmplg;
    char                 block[1024];
    int                  rtc;
    char                *path = NULL;
    My_FILE              fin;
    tsp05_RteFileError   err;
    tsp00_Bool           bOk;
    char                *host = as->GetHeaderValue(req, AGENT_HOST);
    char                *port = as->GetHeaderValue(req, AGENT_PORT);
    char                 errMsg[1024] = "";

    /* First read the rest of the request */
    tmpstr = as->GetHeaderValue(req, AGENT_CONTENT_LENGTH);
    if (tmpstr)
        tmplg = atoi(tmpstr);
    else
        tmplg = 0;

    while (tmplg > 0) {
        lg = WAHTTP_MIN(tmplg, sizeof(block));
        rlg = as->Receive(req, block, lg);

        if (lg != rlg) {
            sp77sprintf(errMsg, 1024, "URI: %s\n                        Contentlength is inconsistent with received length", req->uri);
            Log_Error(errMsg, -1);
            return(FALSE);
        }

        tmplg -= lg;	
    }

    /* which file are we talking about */
    wahttp_SQLALLOCAT( strlen(fah->DocumentRoot) + UriLg + strlen(fah->DefaultDocument) + 1 , &path, &bOk);
    if (!bOk) {
        return(af_error);
    }

    strcpy(path, fah->DocumentRoot);
    strcat(path, req->uri);
    if (req->uri[strlen(req->uri)-1] == '/')
        strcat(path, fah->DefaultDocument);

#ifdef WIN32
    for (s=strchr(path, '/'); s; s=strchr(s+1, '/'))
        *s = '\\';
#endif

    if(strstr(path, "..")) {
        sp77sprintf(errMsg, 1024, "URI: %s not allowed", req->uri);
        Log_Error(errMsg, -1);
        return(af_error);
    }

    sqlfopenc(path, sp5vf_binary, sp5vf_read, sp5bk_buffered, &fin, &err);
    if (err.sp5fe_result != vf_ok) {
        rtc = FileNotFound(req->uri, host, port, as, rep);
    } else {
        rtc = FileFound(path, as, req, rep, fin);
        sqlfclosec (fin, sp5vf_close_normal, &err);
    }

    wahttp_SQLFREE(path);

    if (rtc)
        return(af_accepted);
    else
        return(af_error);

}
