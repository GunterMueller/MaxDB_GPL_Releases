/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
#include <WINDOWS.H>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_HTTP.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Agent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_FileAgent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_WebAgent.h"

static struct st_agent
{
 int			Initialized;
 struct st_agent_client *AgentClientHandle;
 int			(*AgentInit)(struct st_agent_server *, struct st_agent_client **);
 enum en_af_returncode	(*AgentFunc)(struct st_agent_server *, struct st_agent_client *, struct st_agent_req *, struct st_agent_rep *);
 int			(*AgentExit)(struct st_agent_server *, struct st_agent_client *);
} Agents[] = 
{
 { FALSE, NULL,	InitWebAgent,	TryWebAgent,	ExitWebAgent },
 { FALSE, NULL,	InitFileAgent,	TryFileAgent,	ExitFileAgent },
};

#define NOOF_AGENTS	sizeof(Agents)/sizeof(struct st_agent)

static struct st_agent_server AgentServer;

static struct st_charp_int
{
 char	*V;
} Hmap[] =
{
 HTTP_SET_COOKIE,
 HTTP_VERSION,
 HTTP_EXPIRES,
 HTTP_CONTENT_TYPE,
 HTTP_CONTENT_LENGTH,
 HTTP_LAST_MODIFIED,
 HTTP_LOCATION,
 HTTP_DATE,
 HTTP_IF_MODIFIED_SINCE,
 HTTP_SERVER,
 HTTP_CONNECTION,
 HTTP_HOST,
 HTTP_PORT,
};
#define MAX_Hmap_LG 100

static struct st_charp
{
 char	*V;
} Smap[] =
{
 HTTP_STATUS_OK,
 HTTP_STATUS_NOT_MODIFIED,
 HTTP_STATUS_BAD_REQUEST,
 HTTP_STATUS_NOT_FOUND,
 HTTP_STATUS_LENGTH_REQUIRED,
 HTTP_STATUS_CREATED,
 HTTP_STATUS_NO_CONTENT,
 HTTP_STATUS_MOVE_PERM,
 HTTP_STATUS_REDIRECT,
 HTTP_STATUS_UNAUTHORIZED,
 HTTP_STATUS_FORBIDDEN,
 HTTP_STATUS_METHOD_NOT_ALLOWED,
 HTTP_STATUS_CONFLICT,
 HTTP_STATUS_SERVER_ERROR,
 HTTP_STATUS_NOT_IMPLEMENTED,
 HTTP_STATUS_MULTISTATUS,
 HTTP_STATUS_PRECONDITION_FAILED,
 HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,
 HTTP_STATUS_LOCKED,
 HTTP_STATUS_BAD_GATEWAY,
 HTTP_STATUS_INSUFFICENT_STORAGE,
 HTTP_STATUS_PARTIAL_CONTENT,			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
 HTTP_STATUS_RANGE_NOT_SATISFIABLE,		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
};
#define MAX_Smap_LG 100


/*========================================================================*/

static int AgentAddHeader_char( struct st_agent_rep	*rep, int Header, char *Value )
{

    if (!rep) {
        return FALSE;
    }

    return(AddHeader_char(rep->ServerRep, Hmap[Header].V, Value));

}

/*========================================================================*/

static int AgentAddHeaderByName_char( struct st_agent_rep *rep, char *Header, char *Value )
{

    if (!rep) {
        return FALSE;
    }

    return(AddHeader_char(rep->ServerRep, Header, Value));

}

/*========================================================================*/

static int AgentAddHeader_date( struct st_agent_rep	*rep, int Header, time_t Value )
{

    if (!rep) {
        return FALSE;
    }

    return(AddHeader_date(rep->ServerRep, Hmap[Header].V, Value));

}

/*========================================================================*/

static int AgentAddHeader_size_t( struct st_agent_rep *rep, int Header, size_t Value )
{

    if (!rep) {
        return FALSE;
    }

    return(AddHeader_size_t(rep->ServerRep, Hmap[Header].V, Value));

}

/*========================================================================*/

static char *AgentGetHeaderValue( struct st_agent_req *req, int Header)
{

    if (!req) {
        return FALSE;
    }

    switch(Header) {
	    case AGENT_HOST	: return(req->ServerReq->Host);
	    case AGENT_PORT	: return(req->ServerReq->Port);
	    default		    : return(FindHeader(req->ServerReq, Hmap[Header].V));
    }

}

/*========================================================================*/

static char *AgentGetHeaderValueByName( struct st_agent_req	*req, char *Header )
{

    if (!req) {
        return FALSE;
    }

    return(FindHeader(req->ServerReq, Header));

}

/*========================================================================*/

static int AgentGetHeaderNo( struct st_agent_req *req, size_t HeaderNo, char **HeaderName, char **HeaderValue)
{

    if (!req) {
        return FALSE;
    }

    return(GetHeaderNo(req->ServerReq, HeaderNo, HeaderName, HeaderValue));

}

/*========================================================================*/

static size_t AgentReceive( struct st_agent_req	*req, char *b, size_t n )
{

    if (!req || !req->ServerReq || !req->ServerReq->Body.read ) {
        return 0;
    }

    return(req->ServerReq->Body.read(&req->ServerReq->Body, b, n));

}

/*========================================================================*/

static size_t AgentSend( struct st_agent_rep *rep, char *b, size_t n)
{

    size_t	slg;

    if (!rep) {
        return 0;
    }

    if (SendHttpBody(rep->ServerRep, b, n, &slg))
	    return(slg);
    else
	    return(0);

}

/*========================================================================*/

static int AgentSendHeader( struct st_agent_rep	*rep, int Status )
{
    
    char	tmph[sizeof(HTTP_VERSION " ") + MAX_Smap_LG + 1];

    if (!rep) {
        return 0;
    }

    strcpy(tmph, HTTP_VERSION " ");
    strcat(tmph, Smap[Status].V);

    return(SendHttpHeader(tmph, rep->ServerRep));

}

/*========================================================================*/

int InitAgents( char *iniFile )
{

    struct st_agent	*a;

    /* Initialize server description */
    memset(&AgentServer, 0, sizeof(struct st_agent_server));

    AgentServer.AddHeader_char       = AgentAddHeader_char;
    AgentServer.AddHeaderByName_char = AgentAddHeaderByName_char;
    AgentServer.AddHeader_date       = AgentAddHeader_date;
    AgentServer.AddHeader_size_t     = AgentAddHeader_size_t;
    AgentServer.GetHeaderValue       = AgentGetHeaderValue;
    AgentServer.GetHeaderValueByName = AgentGetHeaderValueByName;
    AgentServer.GetHeaderNo          = AgentGetHeaderNo;
    AgentServer.Receive              = AgentReceive;
    AgentServer.Send                 = AgentSend;
    AgentServer.SendHeader           = AgentSendHeader;

    strncpy(AgentServer.iniFile, iniFile, MAX_FILENAME_LEN);
    AgentServer.iniFile[MAX_FILENAME_LEN] = '\0';

    /* Initialize all agents */
    for(a = Agents; a < Agents + NOOF_AGENTS; a++) {
        if (a->AgentInit) {
	        a->Initialized = a->AgentInit(&AgentServer, &a->AgentClientHandle);
        }
    }

    AgentServer.InitOk = TRUE;

    return(TRUE);

}

/*========================================================================*/

int TryAgents( struct st_http_client *client, struct st_http_req *HttpReq, int *Persistent )
{

    struct st_agent	*a;
    struct	st_agent_req	AgentRequest;
    struct	st_agent_rep	AgentReply;

    *Persistent = HttpIsPersistent(HttpReq);
    AgentRequest.ServerReq = HttpReq;
    AgentRequest.method = HttpReq->method;
    AgentRequest.uri = HttpReq->uri;
    AgentRequest.version = HttpReq->hversion;
    AgentRequest.params = HttpReq->params;
    AgentRequest.querystring = HttpReq->querystring;
    AgentReply.ServerRep = client;

    for (a=Agents; a<Agents+NOOF_AGENTS; a++) {
        if (a->Initialized) {
		    switch (a->AgentFunc(&AgentServer, a->AgentClientHandle, &AgentRequest, &AgentReply)) {
		        case af_accepted:
			        return(TRUE);
		        case af_ignored:
			        continue;
		        case af_error:
			        return(FALSE);
		    }
        }
    }

    return(FALSE);

}

/*========================================================================*/

int ExitAgents( void )
{

    struct st_agent	*a;

    for (a=Agents; a<Agents+NOOF_AGENTS; a++) {
        if (a->Initialized) {
            if (a->AgentExit) {
			    a->AgentExit(&AgentServer, a->AgentClientHandle);
            }
        }
    }

    return(TRUE);

}

/***********************************************************************

	End

 ***********************************************************************/

