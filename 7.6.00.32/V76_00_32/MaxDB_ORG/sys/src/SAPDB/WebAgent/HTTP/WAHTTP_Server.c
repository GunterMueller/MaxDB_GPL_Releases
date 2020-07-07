/*!**********************************************************************

  module: WAHTTP_Server.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: SAP DB WWW

  description:  Sockets, Request delivering

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

#define MODULE_NAME "WAHTTP_Server.c"

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <signal.h>
#endif

#ifdef WIN32
#define ThreadSleep	Sleep
#define closesocket(a)	closesocket(a)
#else
/*#define ThreadSleep(msecs)	sleep(msecs/1000)*/
#define ThreadSleep(msecs)	{ signal(SIGALRM, OurSleeper); alarm(msecs/1000); }
#define closesocket(a)		close(a)
#endif


static void OurSleeper
(
 int	sig
)
{
}

#include <sapdbwa_define.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Connect.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_HTTP.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Agent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Server.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Workers.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#include "heo07.h"

#define PERSISTENT_CONNECTIONS  1

#define NO_OF_WORKERS	10


#define MAXHOSTNAMESZ	40
#define MAXREQS			1
#define MAXHEADERSIZE	2000
#define HTTP_PROT		"HTTP/1.0"
#define	DEF_HTTP_PORT	80

/*
#define chunk 1000
#define BADPROXYREQ "Content-Type: text/html\r\n<body><h1>Bad Proxy Request</h1></body>"
#define PROXYREQERR "Content-Type: text/html\r\n<body><h1>Error during Proxy Request</h1></body>"
#define S501 "HTTP/1.0 501 Not Implemented\r\n\r\n"
*/

#define DEFAULT_CLIENT_KEEPALIVE FALSE
#define DEFAULT_SERVER_KEEPALIVE FALSE

#define INI_MULTI_THREADED	"MultiThreading"
#define INI_TRCLEVEL		"TraceLevel"
#define INI_LISTEN_IP		"ListenIp"
#define INI_LISTEN_PORT		"ListenPort"
#define INI_ALIAS		"Alias"

#define DEF_TRCLEVEL		0
#define DEF_MULTI_THREADED	FALSE
#define DEF_LISTEN_PORT		80

#define TRC_HTTP_HEAD	8
#define TRC_HTTP_BODY	9

#define INI_FNAME		"SapDbPort80.ini"

static teo07_Thread thisThread;
static sapdbwa_Bool	threading = sapdbwa_True;
static int JUSTLOG = FALSE;

static int Delegate2Proxy = FALSE;
static char Delegate2ProxyHost[100] = "";
static unsigned short Delegate2ProxyPort = 0;
static DynStr	ReqCookieFileStr = NULL,
		ResCookieFileStr = NULL;


static SOCKET		ServerSocket;
static unsigned short	LocalPort = DEF_LISTEN_PORT;
typedef char	*charp;
static charp		*LocalIps = NULL;
static size_t		NoOfLocalIps = 0;
static char		LocalIp[100];

typedef char t_header[MAXHEADERSIZE];

static sapdbwa_Bool		stopWebServer = sapdbwa_False;
static sapdbwa_UInt4	workerThreadCount = 0;
static sapdbwa_Bool     iniLogRequests = sapdbwa_True;

static ServiceLoop = TRUE;

/*========================================================================*/

/* Receive a HTTP header line including the "\r\n".
 * Return it terminated by '\0'
 * Allocate the buffer dynamically.
 * Add all data to the buffer.
 */
int recv_line( SOCKET    cs,
               DynStr   *RecLine,
               long     *readLg )
{
    char		c1 = '\0', c2 = '\0';
    int		lg;
    int		selectRtc;
    fd_set		readfds;
    struct timeval timeout;

    *readLg = 0;

    timeout.tv_sec = CONNECTIOM_TIMEOUT;
    timeout.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(cs, &readfds);
    /* recv one byte at a time */
    do
    {
        if (!DSextent(RecLine, 2)) {
            FD_CLR(cs, &readfds);
            return(FALSE);
        }

        (*RecLine)->str[(*RecLine)->lg] = '\0';

        /* Wait for date or time-out */
        /*
        selectRtc = select(1, &readfds, 0, 0, &timeout);

        switch (selectRtc) {
            case 0:	* Time-out *
                FD_CLR(cs, &readfds);
                Log_SyscallError("Receive data from remote client timed out", -1);
                return(FALSE);
            case SOCKET_ERROR:
                FD_CLR(cs, &readfds);
                Log_SyscallError("Waiting for data from remote client failed", -1);
                return(FALSE);
        }
        */
        c1 = c2;
        lg = recv( cs, &c2, 1, 0 );
        if (lg == SOCKET_ERROR) {
            FD_CLR(cs, &readfds);
            Log_SyscallError("Receive data from remote client failed", -1);
            return(FALSE);
        } else if (lg == 0) {
            FD_CLR(cs, &readfds);

            if (*readLg == 0) {
                /*
                return(TRUE);
                */
                return(FALSE);
            } else {
                return(FALSE);
            }
        }

        (*RecLine)->str[(*RecLine)->lg] = c2;
        (*RecLine)->lg++;

        (*readLg)++;

        if ((c1 == '\r' && c2 == '\n')) {
            FD_CLR(cs, &readfds);
            (*RecLine)->str[(*RecLine)->lg] = '\0';
            return(TRUE);
        }
    } while (1);

}

/*========================================================================*/

char *IllHeader[] =
{ "Proxy-Connection:",
  NULL
};

int ForbidHeader
(
 char	*Header,
 size_t	HeaderLg
)
{
 int	i = 0;

 while (IllHeader[i])
	if (HeaderLg == strlen(IllHeader[i]) &&
	    !strncmp(Header, IllHeader[i], HeaderLg))
		return(TRUE);
	else
		i++;
 return(FALSE);
}

static SOCKET AcceptingSocket = 0;

int AcceptConnection
(
 SOCKET			ps,
 int			KeepAlive,
 struct st_tcp_connection	*C
)
{
#if  WIN32 || OSF1 || HPUX
 int            clen;
#else
 socklen_t      clen;
#endif
 int            rtc = TRUE;
 FILE          *f = stdout;
 struct linger  Linger;
 int            just_say_no = 1;
 int            keepalive_value = 1;  
 int            one = 1;

 clen=sizeof(C->sa);
 AcceptingSocket = ps;
#ifdef WIN32
 if ((C->s = accept(ps, (struct sockaddr FAR  *)&C->sa, &clen)) == INVALID_SOCKET) 
#else
 if ((C->s = accept(ps, (struct sockaddr *)&C->sa, &clen)) == INVALID_SOCKET) 
#endif
 {
	AcceptingSocket = 0;

    if (ServiceLoop) {  /* In other case we are stoping the server */
	    Log_Error("Init: accept failed", -1);
    }

	return(FALSE);
 }
 AcceptingSocket = 0;

 clen = sizeof(C->sa);
#ifdef WIN32
 if(getsockname(C->s, (struct sockaddr FAR  *)&C->sa, &clen) < 0)
#else
 if(getsockname(C->s, (struct sockaddr *)&C->sa, &clen) < 0)
#endif
 {
	Log_SyscallError("could not get  client socket name", -1);
	Log_Error("Init: getsockname failed", -1);
	shutdown(C->s, SD_BOTH);
	closesocket(C->s);
	return(FALSE);
 }

 if((setsockopt(C->s, SOL_SOCKET, SO_REUSEADDR,(char *)&one, sizeof(one)))
       == -1) {
	Log_SyscallError("could not set client socket option (SO_REUSEADDR)", -1);
	shutdown(C->s, SD_BOTH);
	closesocket(C->s);
	return(FALSE);
 }

 if((setsockopt(C->s,SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive_value,
        sizeof(keepalive_value))) == -1) {
	Log_SyscallError("could not set client socket option (SO_KEEPALIVE)", -1); 
	shutdown(C->s, SD_BOTH);
	closesocket(C->s);
	return(FALSE);
 }

 if((setsockopt(C->s,IPPROTO_TCP, TCP_NODELAY,(char *)&just_say_no,
        sizeof(just_say_no))) == -1) {
	Log_SyscallError("could not set client socket option (TCP_NODELAY)", -1); 
	shutdown(C->s, SD_BOTH);
	closesocket(C->s);
	return(FALSE);
 }

 Linger.l_onoff = !0; 
 Linger.l_linger = 5;
#ifdef WIN32
 if (setsockopt (C->s, SOL_SOCKET, SO_LINGER, (const char FAR *)&Linger, sizeof(Linger)))
#else
 if (setsockopt (C->s, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger)))
#endif
 {
	Log_SyscallError("could not set client socket option (SO_LINGER)", -1); 
	shutdown(C->s, SD_BOTH);
	closesocket(C->s);
	return(FALSE);
 }

 C->Connected = TRUE;
 C->KeepAlive = KeepAlive;
 return(TRUE);
}

void CloseConnection( struct st_tcp_connection *C,
                      int                       persistentConnection )
{

    fd_set          readfds;
    struct timeval  timeout = {0,0};
    char            b[100];
    int             err;

    if (!C->Connected) return;

    FD_ZERO(&readfds);
    FD_SET(C->s, &readfds);

    if (!persistentConnection) {
    /* discard rest in socket, then close HTTP/1.0 no persistant connections */
    /* Is there a need for this select                                       */
    while (0 != select (1, &readfds, NULL, NULL, &timeout)) {
        err = recv( C->s, b, sizeof(b), 0 );
        if (err == 0 || err == SOCKET_ERROR) {
            break;
        } else {
            err = 0;
        }
    }

        shutdown(C->s, SD_BOTH);
        closesocket(C->s);
    }

    C->Connected = FALSE;
    C->KeepAlive = FALSE;

}

/*========================================================================*/

static size_t hdllen( char	*line)
{
    char	*tmp = NULL;
    if (!line) {
        return 0;
    }

    tmp = strstr(line, "\r\n");

    if (tmp)
        return(tmp - line);
    else
        return(0);
}

/*========================================================================*/

static int ReceiveHttpHeaders( SOCKET    cs,
                               DynStr   *HeadersLine,
                               size_t   *NoOfHeaderLines )
{

    size_t  LastHeaderLineAt = 0;
    size_t  LastHeaderLineLg = 0;
    long    readLg = 0;

    while (recv_line(cs, HeadersLine, &readLg)) {
        /*
        if (readLg == 0) {
            continue;
        }
        */

	    LastHeaderLineLg = hdllen((*HeadersLine)->str+LastHeaderLineAt);
	    if (0 == LastHeaderLineLg)
		    return(TRUE);

	    (*HeadersLine)->str[LastHeaderLineAt + LastHeaderLineLg] = '\0';
	    (*NoOfHeaderLines)++;
	    LastHeaderLineAt += LastHeaderLineLg + 2;
    }

    return(FALSE);

}

/*========================================================================*/

static int PrepareHeaders( struct st_http_req	*HttpReq )
{
    char			*p = NULL;
    char			*hl = NULL;
    size_t			hll = 0;
    size_t			HeaderNo = 0, pl;
    char			*tmp;
    tsp00_Bool		bOk;

    /* because there is an additional entry for HTTP_PORT the HeaderNames- and -ValuesArray 
    has to have one more line */
    char			*Host;
    char			*Port;

    if (!HttpReq || !HttpReq->HeadersLine) {
        return FALSE;
    }
    hl = HttpReq->HeadersLine->str;
    hll = HttpReq->HeadersLine->lg;

    HttpReq->NoOfHeaderLines++;

    wahttp_SQLALLOCAT( HttpReq->NoOfHeaderLines * sizeof(char*), &HttpReq->HeaderNamesArray, &bOk);
    if (!bOk) {
        HttpReq->HeaderNamesArray = NULL;
        return FALSE;
    }

    wahttp_SQLALLOCAT( HttpReq->NoOfHeaderLines * sizeof(char*), &HttpReq->HeaderValuesArray, &bOk);
    if (!bOk) {
        wahttp_SQLFREE(HttpReq->HeaderNamesArray);
        HttpReq->HeaderNamesArray = NULL;
	    HttpReq->HeaderValuesArray = NULL;

        return FALSE;
    }

    for (p=hl; p<hl+hll && HeaderNo < HttpReq->NoOfHeaderLines-1; p+=pl + HTTP_HEADER_SEP_LG, HeaderNo++) {
	    pl = strlen(p);
	    HttpReq->HeaderNamesArray[HeaderNo] = p;
    	if (HeaderNo == 0)
	    	HttpReq->HeaderValuesArray[HeaderNo] = NULL;
	    else {
		    tmp = strchr(p, ' ');
		    if (!tmp) {
			    if (HttpReq->HeaderNamesArray) wahttp_SQLFREE(HttpReq->HeaderNamesArray);
			    HttpReq->HeaderNamesArray = NULL;
			    if (HttpReq->HeaderValuesArray) wahttp_SQLFREE(HttpReq->HeaderValuesArray);
			    HttpReq->HeaderValuesArray = NULL;
			    return(FALSE);
		    }
		    *tmp = '\0';
		    tmp++;
		    while (*tmp == ' ') tmp++;
		    HttpReq->HeaderValuesArray[HeaderNo] = tmp;

		    /* remove the trailing colon */
		    tmp = strchr(p, ':');
		    if (tmp && *tmp == ':')
			    *tmp = '\0';
	    }
    }
    /* extract port from host and store it within the last line of the HeaderNames- /
        -ValuesArray */
    HttpReq->HeaderNamesArray[HttpReq->NoOfHeaderLines-1] = "Port";
    HttpReq->HeaderValuesArray[HttpReq->NoOfHeaderLines-1] = "";
    Host = FindHeader(HttpReq, HTTP_HOST);
    /* PTS1117671: Host may be not given -> null pointer */
    if (Host) {
	    Port = strchr(Host, ':');
	    if (Port) {
		    /* PTS1117691 clearly separate Host and Port */
		    *Port = '\0';
		    /* PTS1117691 */
		    (Port)++;
		    HttpReq->HeaderValuesArray[HttpReq->NoOfHeaderLines-1] = Port;
	    } else {
		    HttpReq->HeaderValuesArray[HttpReq->NoOfHeaderLines-1] = "";
	    }
    }

    return(TRUE);

}

/*========================================================================*/

struct st_our_socket
{
 struct st_tcp_connection	*tcp_con;
 struct st_http_req		*req;
};

/*========================================================================*/

static size_t our_recv( struct st_data  *this,
                        void            *buf,
                        size_t           lg )
{

    struct st_our_socket    *os = this->H;
    size_t                   rlg;

    if (os->req->contentChunked) {
        /* Additional way (HTTP 1.1) to read body data with chunks */
        char    ch[3];
        char    chunkSizeBuf[64] = "";
        int     i;

        /* No chunk available? */
        if (!os->req->chunkSize) {
            /* Get chunk size without CRLF */
            i = 0;
            do {
                rlg = recv(os->tcp_con->s, ch, 1, 0 );
                if (rlg == SOCKET_ERROR) {
                    Log_SyscallError("recv from server failed", rlg);
                    return(0);
                }

                /* Is character CR? */
                if (*ch == 0x0D) {
                    /* Chunk size end reached, read LF */
                    rlg = recv(os->tcp_con->s, ch, 1, 0 );
                    if (rlg == SOCKET_ERROR) {
                        Log_SyscallError("recv from server failed", rlg);
                        return(0);
                    }

                    chunkSizeBuf[i] = 0x00;

                    break;
                } else {
                    chunkSizeBuf[i] = *ch;
                    i++;
                }
            } while(i < 64);    /* Max chunk size buffer length */
            os->req->chunkSize = strtol(chunkSizeBuf, NULL, 16);
        }

        /* No more chunks? */
        if (os->req->chunkSize) {
            /* Read chunk */
            rlg = WAHTTP_MIN(os->req->chunkSize, lg);
            rlg = recv(os->tcp_con->s, buf, rlg, 0 );
            if (rlg == SOCKET_ERROR) {
                Log_SyscallError("recv from server failed", rlg);
                return(0);
            }
        } else {
            rlg = 0;
        }

        /* If read length == chunk size read end of chunk */
        if (os->req->chunkSize == rlg) {
            /* Read end of chunk CRLF*/
            if (recv(os->tcp_con->s, chunkSizeBuf, 2, 0) == SOCKET_ERROR) {
                Log_SyscallError("recv from server failed", rlg);
                return(0);
            }
        }

        /* Set size of remaining chunk */
        os->req->chunkSize -=rlg;
    } else {
        /* Traditional way to read body data with content length */
        if (!os->req->ContentLength)
            return(0);

        rlg = WAHTTP_MIN(os->req->ContentLength, lg);
        rlg = recv(os->tcp_con->s, buf, rlg, 0 );
        if (rlg == SOCKET_ERROR) {
            Log_SyscallError("recv from server failed", rlg);
            return(0);
        }
    }

    return(rlg);

}

/*========================================================================*/

static int BadRequest
(
 struct st_http_client	*client
)
{
 time_t			now;

 AddHeader_char(client, HTTP_SERVER, THIS_SERVER);
 time( &now );
#ifdef WIN32
 AddHeader_date(client, HTTP_DATE, now + _timezone);
#else
 AddHeader_date(client, HTTP_DATE, now + timezone);
#endif
/* AddHeader_char(&rep, CONTENT_TYPE, DEFAULTMIMETYPE);*/
 SendHttpHeader(HTTP_VERSION " " HTTP_STATUS_BAD_REQUEST, client);

 return(TRUE);
}

/*========================================================================*/

static int ProcessRequest( struct st_http_client    *client,
                           struct st_http_req       *HttpReq )
{

    struct st_our_socket	os;
    char			*Status;
    int			Persistent = FALSE;

    if (iniLogRequests == sapdbwa_True) {
        Log(HttpReq->HeadersLine->str, -1);
    }

    HttpReq->method = HttpReq->HeadersLine->str;
    if (!HttpReq->method)
        return(FALSE);

    HttpReq->uri = strchr(HttpReq->method, ' ');
    if (!HttpReq->uri)
        return(FALSE);
    else {
        *HttpReq->uri = '\0';		/* this is the end of method */
        HttpReq->uri++;

        while (*HttpReq->uri && *HttpReq->uri == ' ')	/* skip any more SP */
            HttpReq->uri++;

        if (!HttpReq->uri)
            return(FALSE);

        HttpReq->Type = hmt_simple_request;
    }

    HttpReq->hversion = strchr(HttpReq->uri, ' ');
    if (HttpReq->hversion) {
        *HttpReq->hversion = '\0';
        HttpReq->hversion++;

        while (*HttpReq->hversion && *HttpReq->hversion == ' ')	/* skip any more SP */
            HttpReq->hversion++;

        if (!HttpReq->hversion)
            return(FALSE);

        HttpReq->Type = hmt_full_request;
    }

    if (!strcmp(HttpReq->method, "GET"))
        HttpReq->Method = rq_get;
    else if (!strcmp(HttpReq->method, "HEAD"))
        HttpReq->Method = rq_head;
    else if (!strcmp(HttpReq->method, "POST"))
        HttpReq->Method = rq_post;
    /* insert DanielK */
    /* This part must only be processed iff the server runs in WebDAV Server mode!!!!!!!!!!! */
    else if (GetEnableWebDav()) {
        if (!strcmp(HttpReq->method, "PUT"))
            HttpReq->Method = rq_put;
        else if (!strcmp(HttpReq->method, "OPTIONS"))
            HttpReq->Method = rq_options;
        else if (!strcmp(HttpReq->method, "CONNECT"))
            HttpReq->Method = rq_connect;
        else if (!strcmp(HttpReq->method, "TRACE"))
            HttpReq->Method = rq_trace;
        else if (!strcmp(HttpReq->method, "MKCOL"))
            HttpReq->Method = rq_mkcol;
        else if (!strcmp(HttpReq->method, "PROPFIND"))
            HttpReq->Method = rq_propfind;
        else if (!strcmp(HttpReq->method, "PROPPATCH"))
            HttpReq->Method = rq_proppatch;
        else if (!strcmp(HttpReq->method, "DELETE"))
            HttpReq->Method = rq_delete;
        else if (!strcmp(HttpReq->method, "COPY"))
            HttpReq->Method = rq_copy;
        else if (!strcmp(HttpReq->method, "MOVE"))
            HttpReq->Method = rq_move;
        else if (!strcmp(HttpReq->method, "LOCK"))
            HttpReq->Method = rq_lock;
        else if (!strcmp(HttpReq->method, "UNLOCK"))
            HttpReq->Method = rq_unlock;
/*        else if (!strcmp(HttpReq->method, "VERSION-CONTROL"))
            HttpReq->Method = rq_versioncontrol;
        else if (!strcmp(HttpReq->method, "CHECKIN"))
            HttpReq->Method = rq_checkin;
        else if (!strcmp(HttpReq->method, "CHECKOUT"))
            HttpReq->Method = rq_checkout;
        else if (!strcmp(HttpReq->method, "UNCHECKOUT"))
            HttpReq->Method = rq_uncheckout;
        else if (!strcmp(HttpReq->method, "REPORT"))
            HttpReq->Method = rq_report;
*/        else if (!strcmp(HttpReq->method, "XQUERY"))
            HttpReq->Method = rq_xquery;
        else
	        return(FALSE);
    }
    /* end insert DanielK */
    else
	    return(FALSE);
	
    /* delete DanielK
    if (HttpReq->Method == rq_post)
        HttpReq->ContentLengthValid = GetContentLength(HttpReq, &HttpReq->ContentLength);
     * end delete DanielK */

    /* insert DanielK */
    if ((HttpReq->Method == rq_post) ||
        (HttpReq->Method == rq_put) ||
        (HttpReq->Method == rq_propfind) ||
        (HttpReq->Method == rq_proppatch) ||
        (HttpReq->Method == rq_copy) ||
        (HttpReq->Method == rq_lock) ||
        (HttpReq->Method == rq_versioncontrol) ||
        (HttpReq->Method == rq_checkin) ||
        (HttpReq->Method == rq_checkout) ||
        (HttpReq->Method == rq_uncheckout) ||
        (HttpReq->Method == rq_report) || 
		(HttpReq->Method == rq_xquery))
    HttpReq->ContentLengthValid = GetContentLength(HttpReq, &HttpReq->ContentLength, &HttpReq->contentChunked);
    /* end insert DanielK */

    HttpReq->chunkSize = 0;
/*    if (HttpReq->ContentLengthValid && HttpReq->ContentLength == 0) {
        HttpReq->contentChunked = TRUE;
    } else {
        HttpReq->contentChunked = FALSE;
    }
*/
    if (HttpReq->Type == hmt_simple_request) {
        HttpReq->HMajorVersion = 0;
        HttpReq->HMinorVersion = 0;
    } else if (!HttpGetVersion(HttpReq->hversion, &HttpReq->HMajorVersion, &HttpReq->HMinorVersion))
		return(FALSE);

    HttpReq->params = strchr(HttpReq->uri, ';');
    if (HttpReq->params) {
        *HttpReq->params = '\0';
        HttpReq->params++;
    }

    if (HttpReq->params)
        HttpReq->querystring = strchr(HttpReq->params, '?');
    else
        HttpReq->querystring = strchr(HttpReq->uri, '?');
    if (HttpReq->querystring) {
        *HttpReq->querystring = '\0';
        HttpReq->querystring++;
    }

    GetHostPort(HttpReq, &HttpReq->Host, &HttpReq->Port);
#ifdef PERSISTENT_CONNECTIONS
    HttpReq->PersistentRequested = TRUE;
    HttpReq->PersistentRequested = HttpIsPersistent(HttpReq);
#else
    HttpReq->PersistentRequested = FALSE;
#endif

    /* check consistency */
    if (!HttpIsReqConsistent(HttpReq, &Status)) {
        BadRequest(client);
        return(TRUE);
    }

    if (HttpReq->PersistentRequested) {
        client->tcp_con->KeepAlive = TRUE;

        /*
        if (HttpReq->Method == rq_mkcol) {
            AddHeader_char(client, HTTP_CONTENT_LENGTH, "0");
        }
        */

        if (HttpIsKeepAlive(HttpReq)) {
            HttpReq->PersistentRequested = FALSE;
            client->tcp_con->KeepAlive = FALSE;
        }
        /*
        AddHeader_char(client, HTTP_CONNECTION, HTTP_KEEP_ALIVE);
        */
    } else {
        client->tcp_con->KeepAlive = FALSE;
    }

    os.tcp_con = client->tcp_con;
    os.req = HttpReq;
    HttpReq->Body.H = &os;
    HttpReq->Body.read = our_recv;
    HttpReq->Body.destroy = NULL;
    HttpReq->Body.rewind = NULL;

    HttpReq->logDesc = Log_GetDesc();

    if (!TryAgents(client, HttpReq, &Persistent)) {
        Log_Error("TryAgents failed", -1);
        return(FALSE);
    }

    return(TRUE);

}

/*========================================================================*/

void DoHttp
(
 struct st_tcp_connection	*c,
 int				*KeepAlive
)
{
 char			*reqbuf = NULL;
 size_t			reqbufsize = 0, reqbuflg = 0;
 DynStr			ReqLine = NULL;
 int			recv_ok = TRUE;
 int			hno = 0;
 int			SimpleRequest = TRUE;
 size_t			cnt = 0;
 long			ContentLength = -1;
 int			ContentLengthDone = FALSE;
 char			Uri[MAXURISIZE] = "";
 DynStr			Body = NULL;
 size_t			rBodyLg = 0;
 size_t			ContentLg = 0;
 char			*cl = NULL;
 struct st_http_req	HttpReq;
 struct st_http_client	client;
 int			stop = FALSE;
 char           errMsg[1024] = "";

	memset(&HttpReq, 0, sizeof(HttpReq));
	HttpReq.HeadersLine = DSinit();
	HttpReq.NoOfHeaderLines = 0;
	memset(&client, 0, sizeof(client));
	client.HeadersLine = DSinit();
	client.tcp_con = c;

	if (!ReceiveHttpHeaders(c->s, &HttpReq.HeadersLine, &HttpReq.NoOfHeaderLines)) {
        /*Log_Error("ReceiveHttpHeader (Request) failed", -1);*/
        Log_SyscallError("ReceiveHttpHeader (Request) failed", -1);
		stop = TRUE;
    }
    
    if (!stop && !PrepareHeaders(&HttpReq)) {
		Log_Error("PrepareHeaders (Request) failed", -1);
		stop = TRUE;
    }
    
#ifdef DEBUG
    if (strncmp(HttpReq.HeadersLine->str, "GET /waadmin.wa?StopServer", 26) == 0) {
        /* Terminating Web Server */
        if (HttpReq.HeaderNamesArray) {
            wahttp_SQLFREE(HttpReq.HeaderNamesArray);
        }
	    HttpReq.HeaderNamesArray = NULL;

        if (HttpReq.HeaderValuesArray) {
            wahttp_SQLFREE(HttpReq.HeaderValuesArray);
        }
	    HttpReq.HeaderValuesArray = NULL;

	    DSdelete(client.HeadersLine);
	    DSdelete(HttpReq.HeadersLine);

        StopWebServer();

        return;
    }
#endif
    
    if (!stop && !ProcessRequest(&client, &HttpReq)) {
        sp77sprintf(errMsg, 1024, "ProcessRequest (Request) failed. [URI: %s]", HttpReq.uri);
		Log_Error(errMsg, -1);
		BadRequest(&client);
		stop = TRUE;
	}

    if (HttpReq.HeaderNamesArray) {
        wahttp_SQLFREE(HttpReq.HeaderNamesArray);
    }
	HttpReq.HeaderNamesArray = NULL;

    if (HttpReq.HeaderValuesArray) {
        wahttp_SQLFREE(HttpReq.HeaderValuesArray);
    }
	HttpReq.HeaderValuesArray = NULL;

	DSdelete(client.HeadersLine);
	DSdelete(HttpReq.HeadersLine);

    if (stop) {
        *KeepAlive = FALSE;
    } else {
        *KeepAlive = c->KeepAlive && c->Connected;
    }

    return;

}

static  struct st_workq	*HttpReqQ;

void *HttpWorker( void   *something )
{

    int			     stop;
    struct st_work	*work;
    int			     WorkNo = 0;
    int			     KeepAlive = FALSE;

    WA_TRY {
        do {
            work = GetWork(HttpReqQ);
            if (!work) {
                Log_Error("Got NULL work", -1);
                break;
            } else if (work->stop) {
                Log_Error("This HttpWorker got stop request", -1);
                DestroyWork(work, FALSE);
                break;
            }

            WorkNo++;
            DoHttp(work->ActualWork, &KeepAlive);
            stop = work->stop;

            if (KeepAlive)
                PutWork(work, HttpReqQ);
            else
                DestroyWork(work, FALSE);

        } while (!stop && ServiceLoop);
    } WA_CATCH;

    Log_Error("This HttpWorker is terminating", -1);
    sqlendthread(0);

    return (void*)0;

}

void *MakeTcpConnection
(
 void
)
{
 struct st_tcp_connection	*c;
 tsp00_Bool			bOk;

 wahttp_SQLALLOCAT( sizeof(struct st_tcp_connection),
		    &c,
		    &bOk);
 if (!bOk)
 {
	Log_Error("MakeTcpConnection: could not wahttp_SQLALLOCAT", -1);
	return(NULL);
 }
 memset(c, 0, sizeof(struct st_tcp_connection));
 return(c);
}

/*========================================================================*/

void DestroyTcpConnection( void *c,
                           int   persistentConnection )
{

    CloseConnection(c, persistentConnection);

    wahttp_SQLFREE(c);

}

/*========================================================================*/

static int Service_http
(
 SOCKET		ss
)
{
 int				rtc = TRUE;
 FILE				*f = stdout;
 struct	st_work			*work;
 struct st_tcp_connection	*c;

 work = MakeWork(MakeTcpConnection, DestroyTcpConnection);
 if (!work)
 {
	Log_Error("Service_http: could not MkWork", -1);
	return(FALSE);
 }

 c = work->ActualWork;
 c->KeepAlive = DEFAULT_CLIENT_KEEPALIVE;
 c->Connected = FALSE;

 if (!AcceptConnection(ss, DEFAULT_CLIENT_KEEPALIVE, c))
 {
	DestroyWork(work, FALSE);
	return(FALSE);
 }

 PutWork(work, HttpReqQ);
 if (threading)
 {
 }
 else
 {
	HttpWorker(0);
 }

 return(TRUE);
}

static void *Service_https
(
 void *ss
)
{
 if (!StartWorkers())
	return (void *)0;

 Log_Error("starting service loop", -1);
 ServiceLoop = TRUE;
 while (ServiceLoop)
	 Service_http(*(SOCKET*)ss);
 Log_Error("terminating service loop", -1);
 return (void *)0;
}

/* Initialize for request processing:
 * - FileSystemRetrieval
 * - WebAgent
 */
static int InitRequestProcessing( char *iniFile )
{

    if(!InitAgents(iniFile)) {
        Log_Error("Initializing Processing Agents failed", -1);
        return(FALSE);
    }

    return(TRUE);

}

static int ExitRequestProcessing
(
)
{
 if (!ExitAgents())
 {
	Log_Error("Exitalizing Processing Agents failed", -1);
	return(FALSE);
 }

 return(TRUE);
}

enum en_cmd
{
 cmd_terminate,
 cmd_continue,
 cmd_err,
 cmd_huh
};

enum en_watchwhat
{
 ww_nothing,
 ww_console,
 ww_flag
} WatchWhat;

static int FlagStop = TRUE;

static enum en_cmd GetConsoleCommand
(
 void
)
{
 char	line[1000];

 switch (WatchWhat)
 {
  case ww_console:
	do
	{
		printf("Enter command (q for quit, no other commands available) :");
		if (!fgets(line, sizeof(line), stdin))
			return(cmd_err);
		if (!strcasecmp(line, "q\n"))
			return(cmd_terminate);
		else
		if (!strcasecmp(line, "c\n"))
			return(cmd_continue);
		else
			printf("huh ?\n");
	}
	while (1);
	break;
  case ww_flag:
	if (FlagStop)
		return(cmd_terminate);
	else
		return(cmd_continue);
	break;
  case ww_nothing:
  default:
		return(cmd_terminate);
 }
}

/*========================================================================*/

int InitWebServer( char                              IniFilePath[],
                   struct wahttp_alternate_settings *alternateSettings,
                   char                             *errorOutText )
{

#ifdef WIN32
    WSADATA          WsaData;
#endif

    int              err; 
    char             errtxt[MAX_ERRORTEXT_LG];
    char             errtxt2[MAX_ERRORTEXT_LG];
    char             Ipar[100];
    int              trcl = DEF_TRCLEVEL;
    char            *sepat = 0;
    unsigned long    tid = 0;
    int              InitIniParFailed = FALSE;
    char            *tmp1;
    charp           *tmp2;
    char             errorText[MAX_ERRORTEXT_LG+1];
    tsp00_Bool       bOk;
    tsp00_ErrTextc   threadErrText;
    teo07_ThreadErr  threadErr;
    char             usedIniFile[MAX_FILENAME_LEN+1] = "";

    /* Init threading environment           */
    /* Also important for usage of mutex    */
    /* PTS: 1110225 (MaO)                   */
    sqlinit_multithreading(&thisThread, threadErrText, &threadErr);
    if (threadErr != THR_OK_EO07) {
        strcpy(errorOutText, "Cannot initialize web server: init threading");

        return(FALSE);
    }

    wahttp_SQLMALLOCINIT(&bOk);

    SetDefLogFilePath(IniFilePath);

    /*
    if (strlen(IniFilePath) > 0)
        sprintf(inifile, "%s%s%s", IniFilePath, DIRPATH_SEP, INI_FNAME);
    else
        sprintf(inifile, "%s", INI_FNAME);
    */
    if ( !InitIniPar( IniFilePath, alternateSettings, usedIniFile, errorText, MAX_ERRORTEXT_LG ) )
        InitIniParFailed = TRUE;

    /* Is a log requested? */
    iniLogRequests = GetLogRequests();

    InitLog();

    if (InitIniParFailed) {
        sp77sprintf( errtxt2, MAX_ERRORTEXT_LG, "Init web server failed:%s", errorText );
        Log_Error( errtxt2, -1);

        sp77sprintf( errtxt2, MAX_ERRORTEXT_LG, "Using default settings to init web server." );
        Log_Error( errtxt2, -1);
    }

	trcl = DEF_TRCLEVEL;
    /*
    if (!IniReadPar(INI_TRCLEVEL, sizeof(Ipar), Ipar, 0, NULL))
        trcl = DEF_TRCLEVEL;
    else {
        trcl = atoi(Ipar);
        if (trcl == 0)
        trcl = DEF_TRCLEVEL;
    }
    */

    SetTraceLevel(trcl);

	/* Get mime types	*/
	if ( !ReadMimeTypes( errorText, MAX_ERRORTEXT_LG ) ) {
		sp77sprintf( errtxt2, MAX_ERRORTEXT_LG, "Read mime types failed:%s", errorText );
		Log_Error( errtxt2, -1);
	}

    threading = GetThreading();
    /*
    if (!IniReadPar(INI_MULTI_THREADED, sizeof(Ipar), Ipar, 0, NULL))
        MULTI_THREADED = DEF_MULTI_THREADED;
    else
        MULTI_THREADED = !strcasecmp(Ipar, "yes");
    */

    LocalPort = GetPort();
    /*
    if (!IniReadPar(INI_LISTEN_PORT, sizeof(Ipar), Ipar, 0, NULL))
        LocalPort = DEF_LISTEN_PORT;
    else
        LocalPort = atoi(Ipar);
    */

    *LocalIp = '\0';
    /*
    if (!IniReadPar(INI_LISTEN_IP, sizeof(LocalIp), LocalIp, 0, NULL))
        *LocalIp = '\0';
    */

    NoOfLocalIps = 0;
    LocalIps = NULL;

    tmp1 = LocalIp;
    while (tmp1 && *tmp1) {
        tmp2 = LocalIps;
        wahttp_SQLREALLOCAT( ( NoOfLocalIps + 1 ) * sizeof(LocalIps), &tmp2, &bOk);
        if (!bOk) {
            if (LocalIps) wahttp_SQLFREE(LocalIps);
            Log_Error("InitWebServer: could not wahttp_SQLREALLOCAT LocalIps", -1);
            return(FALSE);
        }

        LocalIps = tmp2;
        LocalIps[NoOfLocalIps] = tmp1;
        if (tmp1 = strchr(tmp1, ';')) {
            *tmp1 = '\0';
            tmp1++;
        }

        NoOfLocalIps++;
    }
 
    sprintf(errtxt, "Listening on IP %s on port %d", LocalIp, LocalPort);
    Log_Error(errtxt, -1);

#ifdef WIN32
    err = WSAStartup (0x0101, &WsaData);
    if (err == SOCKET_ERROR) {
        Log_SyscallError("Init: WSAStartup Failed", -1);
        ExitLog();
        return FALSE;
    }
#endif

    if (!InitWorkers(threading)) {
        Log_Error("Init: Failed", -1);
        ExitLog();
        return FALSE;
    }

    /* Init agents */
    InitRequestProcessing(usedIniFile);

    Log_Error("Init: Initialized", -1);

    return(TRUE);

}

/*========================================================================*/

static teo07_Thread listenerThread = NULL;

/*========================================================================*/

int StartWebServer()
{

    SOCKADDR_IN		 sa_server;
    IN_ADDR			 bind_address;
    int				 b1, b2, b3, b4;
    size_t			 i;
    tsp00_ErrTextc	 errtext;
    teo07_ThreadErr	 err;
#ifndef WIN32
    struct protoent	*pe;
#endif
    int				 just_say_no = 1;
    int				 keepalive_value = 1;  
    int				 one = 1;
	sapdbwa_UInt4	 maxThreads = 0;
    char             sysCallErrorString[MAX_ERROR_TEXT_LEN+1];
	
	maxThreads = GetMaxThreads();
	if ( maxThreads < 1 ) {
		maxThreads = NO_OF_WORKERS;
	}

    if (listenerThread) {
        Log_Error("Already running, startup request ignored", -1);
        return(EXIT_SUCCESS);
    }

    if (!MakeWorkers( maxThreads, HttpWorker ))
	    return(EXIT_FAILURE);


#ifdef WIN32
    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
#else
    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
#endif
        Log_SyscallError("Init: could not get listen socket", -1);
        ExitLog();
        return EXIT_FAILURE;
    }

    /*
    if((setsockopt(ServerSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&one,sizeof(one))) == -1) {
        Log_SyscallError("Init: could not set listen socket option (SO_REUSEADDR)", -1);
        ExitLog();
        return EXIT_FAILURE;
    }
    */

    if((setsockopt(ServerSocket,SOL_SOCKET,SO_KEEPALIVE,(void *)&keepalive_value, sizeof(keepalive_value))) == -1) {
        Log_SyscallError("Init: could not set socket option (SO_KEEPALIVE)", -1); 
        ExitLog();
        return EXIT_FAILURE;
    }

    if((setsockopt(ServerSocket,IPPROTO_TCP, TCP_NODELAY,(char *)&just_say_no, sizeof(just_say_no))) == -1) {
        Log_SyscallError("Init: could not set socket option (TCP_NODELAY)", -1); 
        ExitLog();
        return EXIT_FAILURE;
    }

    memset((char *) &sa_server, 0, sizeof(sa_server));
    sa_server.sin_family=AF_INET;

    for (i=0; i<=NoOfLocalIps; i++) {
#ifdef WIN32
        if (i > 0 && LocalIps[i] && 4 == sscanf(LocalIps[i], "%3d.%3d.%3d.%3d", &b1, &b2, &b3, &b4)) {
            bind_address.S_un.S_un_b.s_b1 = b1;
            bind_address.S_un.S_un_b.s_b2 = b2;
            bind_address.S_un.S_un_b.s_b3 = b3;
            bind_address.S_un.S_un_b.s_b4 = b4;
        } else {
            bind_address.s_addr = htonl(INADDR_ANY);
        }
#else
        /*
        if (i > 0 && LocalIps[i] && !inet_aton(LocalIps[i], &bind_address.s_addr)) {
            Log_Error("Invalid IP address format", -1);
            ExitLog();
            return EXIT_FAILURE;
        }
        */

        if (i > 0 && LocalIps[i] ) {
            if ( ( bind_address.s_addr = inet_addr( LocalIps[i] )) == -1 ) {
                Log_SyscallError("Invalid IP address format", -1);
                ExitLog();
                return EXIT_FAILURE;
            }
        } else {
            bind_address.s_addr = htonl(INADDR_ANY);
        }
#endif

        sa_server.sin_addr=bind_address;
        sa_server.sin_port=htons(LocalPort);

        if(bind(ServerSocket,(struct sockaddr *) &sa_server, sizeof(sa_server)) == -1) {
            if (bind_address.s_addr != htonl(INADDR_ANY)) {
                Log_SyscallError("Init: could not bind to address port, listen socket", -1);
            } else {
                sp77sprintf(sysCallErrorString, MAX_ERROR_TEXT_LEN,
                "Init: could not bind to port %d, listen socket", LocalPort);
                Log_SyscallError(sysCallErrorString, -1);
            }

            ExitLog();
            return EXIT_FAILURE;
        }
    }

#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif

    if (listen (ServerSocket, 512)) {
        Log_Error("Init: listen failed", -1);
        ExitLog();
        return EXIT_FAILURE;
    }

    /*
    listenerThread = _beginthread( &Service_https, 0, &ServerSocket);
    */

    sqlbeginthread(WORKER_STACK_WAHTTP,
                   Service_https, 
                   &ServerSocket, 
                   THR_CREATE_SUSPENDED_EO07, 
                   &listenerThread,
                   errtext, 
                   &err);

    if (err == THR_OK_EO07) {
        sqlresumethread(listenerThread, errtext, &err);
        if (err == THR_OK_EO07) {
            Log_Error("Init: starting Web Server", -1);
            return(EXIT_SUCCESS);
        } else {
            Log_Error(errtext, -1);
            return(EXIT_FAILURE);
        }
    } else {
        Log_Error(errtext, -1);
        return(EXIT_FAILURE);
    }

}

/*========================================================================*/

int WaitWebServer( sapdbwa_Bool debug )
{

	tsp00_Int4		 status;
	tsp00_ErrTextc	 errtext;
	teo07_ThreadErr	 err;

	if ( !listenerThread ) {
		Log_Error( "Not running, termination request ignored", -1 );
		return EXIT_FAILURE;
	}

#ifdef WIN32
	if ( debug == sapdbwa_False ) {
		/* Report the status SERVICE_RUNNING	*/
		ServiceIsRunning();
	}
#endif

/*
	ServiceLoop = FALSE;
	do {
		ThreadSleep(1000);

		if (!sqlthreadalive( listenerThread, errtext, &err)) {
			break;
		}

	} while ( err == THR_OK_EO07 && !(stopWebServer == sapdbwa_True) );
*/
/*
#pragma "Tony is anoyed by the cpu eating jointhread ... "
Sleep(3600000);
*/
	sqljointhread( listenerThread, &status, errtext, &err );

	if (err != THR_OK_EO07) {
		Log_Error(errtext, -1);
	}

	listenerThread = NULL;
	Log_Error("terminated", -1);

	return EXIT_SUCCESS;

}

/****************************************************************************
 ***
 *** StopWebServer
 ***
 ****************************************************************************/

int StopWebServer()
{
	tsp00_Int4		status;
	tsp00_ErrTextc	errtext;
	teo07_ThreadErr	err;

	if ( !listenerThread ) {
		Log_Error( "Not running, termination request ignored", -1 );
		return EXIT_FAILURE;
	}

	ServiceLoop = FALSE;

    shutdown(AcceptingSocket, SD_BOTH);
	closesocket( AcceptingSocket );

    ExitWebServer();

	stopWebServer = sapdbwa_True;

/*
	do {
		ThreadSleep( 1000 );
		if (!sqlthreadalive( listenerThread, errtext, &err)) {
			break;
		}
	} while ( err == THR_OK_EO07 );
*/
	sqljointhread( listenerThread, &status, errtext, &err );

	if (err != THR_OK_EO07) {
		Log_Error(errtext, -1);
	}

	listenerThread = NULL;
	Log_Error( "terminated", -1 );

	return EXIT_SUCCESS;
}

/****************************************************************************
 ***
 *** ExitWebServer 
 ***
 ****************************************************************************/

int ExitWebServer()
{

	ExitRequestProcessing();

	if (LocalIps) free(LocalIps);

	ExitIniPar();
	ExitLog();

	return(EXIT_SUCCESS);

}

#ifdef kannweg
/****************************************************************************
 ***
 *** TcpGetIp
 ***
 ****************************************************************************/

unsigned long TcpGetIp( struct st_tcp_connection	*tcp_con )
{
	return( ntohl(tcp_con->sa.sin_addr.S_un.S_addr) );
}
#endif
