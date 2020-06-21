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
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Connect.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_HTTP.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#define TCP_SEND_BLOCK_SZ	1400

#define FMT_HTTP_HEADER_CHAR	"%s: %s%s"
#define FMT_HTTP_HEADER_SIZE_T	"%s: %ld%s"

/*========================================================================*/

int AddHeader_char( struct st_http_client *client, char *Header, char *Value )
{
    size_t	l;
    
    if (!client || !Header || !Value) {
        return FALSE;
    }

    l = strlen(Header) + 1 + (Value ? strlen(Value) : 0) + HTTP_HEADER_SEP_LG + 1;

    if (!DSextent(&client->HeadersLine, l))
	    return(FALSE);

    /* MaO: check if header includes a colon or not	*/
    client->HeadersLine->lg += sprintf(&client->HeadersLine->str[client->HeadersLine->lg], FMT_HTTP_HEADER_CHAR, Header, Value ? Value : "", HTTP_HEADER_SEP);

    return(TRUE);

}

/*========================================================================*/

int AddHeader_size_t( struct st_http_client	*client, char *Header, size_t Value)
{

    size_t	l;

    if (!client || !Header) {
        return FALSE;
    }

    l = strlen(Header) + 1 + 15 + HTTP_HEADER_SEP_LG + 1;

    if (!DSextent(&client->HeadersLine, l))
	    return(FALSE);

    client->HeadersLine->lg += sprintf(&client->HeadersLine->str[client->HeadersLine->lg], FMT_HTTP_HEADER_SIZE_T, Header, Value, HTTP_HEADER_SEP);

    return(TRUE);

}

/*========================================================================*/

int AddHeader_date( struct st_http_client  *client,
                    char			       *Header,
                    time_t                  Value )
{

    size_t	tmplg;
    char	tmp[50];

    /* Tony/MaO: ctime-is-not-thread-safe */
    tmplg = sprintf(tmp, ctime(&Value));

    /* remove the trailing eoln which is added by ctime on WINDOWS */
    if (tmp[tmplg - 1] == '\n') {
        tmp[tmplg - 1] = '\0';
        tmplg--;
    }

    strcat(tmp, " GMT");

    AddHeader_char(client, Header, tmp);

    return(TRUE);

}

/*========================================================================*/

static int SendHeader ( SOCKET  cs,
                        char   *HeaderFieldName,
                        size_t  HeaderFieldNameLg,
                        char   *HeaderFieldValue,
                        size_t  HeaderFieldValueLg )
{

    int    slg;

    if (HeaderFieldNameLg > 0) {
        slg = send (cs,  HeaderFieldName, HeaderFieldNameLg, 0);
        Log_Journal(cs, HeaderFieldName, HeaderFieldNameLg);
        if (SOCKET_ERROR == slg) {
            Log_SyscallError("send to client failed", slg);
            return(FALSE);
        } else if (slg != HeaderFieldNameLg) {
            Log_SyscallError("send to client failed (slg != rlg)", 0);
            return(FALSE);
        }
    }

    if (HeaderFieldValueLg > 0) {
        slg = send (cs,  HeaderFieldValue, HeaderFieldValueLg, 0);
        Log_Journal(cs, HeaderFieldValue, HeaderFieldValueLg);
        if (SOCKET_ERROR == slg) {
            Log_SyscallError("send to client failed", slg);
            return(FALSE);
        } else if (slg != HeaderFieldValueLg) {
            Log_SyscallError("send to client failed (slg != rlg)", 0);
            return(FALSE);
        }
    }

    slg = send (cs,  HTTP_HEADER_SEP, HTTP_HEADER_SEP_LG, 0);
    Log_Journal(cs, HTTP_HEADER_SEP, HTTP_HEADER_SEP_LG);
    if (SOCKET_ERROR == slg) {
        Log_SyscallError("send to client failed", slg);
        return(FALSE);
    } else if (slg != HTTP_HEADER_SEP_LG) {
        Log_SyscallError("send to client failed (slg != rlg)", 0);
        return(FALSE);
    }

    return(TRUE);

}

/*========================================================================*/

int SendHttpHeader( char *Status, struct st_http_client	*client )
{

    size_t		slg;

    if (!client || !Status) {
        return FALSE;
    }

    /* send the headers */
    /*Log(Status, strlen(Status));*/
    if (!SendHeader(client->tcp_con->s, Status, strlen(Status), NULL, 0))
        return(FALSE);

    if (client) {
        /*Log(client->HeadersLine->str, client->HeadersLine->lg);*/
	    slg = send (client->tcp_con->s,  client->HeadersLine->str, client->HeadersLine->lg, 0);
        Log_Journal(client->tcp_con->s, client->HeadersLine->str, client->HeadersLine->lg);
	    if (SOCKET_ERROR == slg) {
		    Log_SyscallError("send to client failed", slg);
		    return(FALSE);
	    } else if (slg != client->HeadersLine->lg) {
		    Log_SyscallError("send to client failed (slg != rlg)", 0);
		    return(FALSE);
	    }

	    if (!SendHeader(client->tcp_con->s, NULL, 0, NULL, 0))
		    return(FALSE);
    } else {
	    if (!SendHeader(client->tcp_con->s, NULL, 0, NULL, 0))
		    return(FALSE);
    }

    client->HeadersSent = TRUE;

    return(TRUE);

}

/*========================================================================*/

int SendHttpBody ( struct st_http_client	*client,
                   void			            *b,
                   size_t			         n,
                   size_t		            *rn )
{

    if (!client || !client->HeadersSent)
        return(FALSE);

    *rn = send (client->tcp_con->s,  b, n, 0);
    Log_Journal(client->tcp_con->s, b, n);
    if (SOCKET_ERROR == *rn) {
        Log_SyscallError("send to client failed", *rn);
        return(FALSE);
    } else if (*rn != n) {
        Log_SyscallError("send to client failed (*rn != n)", 0);
        return(FALSE);
    } else
        return(TRUE);

}

/*========================================================================*/

#define HTTP_PREFIX	"HTTP_"
#define HTTP_PREFIX_LG	sizeof(HTTP_PREFIX)-1

/*========================================================================*/

static int httpheadercmp( char *HeaderField, char *Header )
{
    int	HttpPrefix = FALSE;
    size_t	n =0;
    int	rtc;

    /* compare the forms:
    * HttpHeader:	If-Modified-Since
    * with	Header:	HTTP_IF_MODIFIED_SINCE
    * or   Header: If-Modified-Since
    */
    if (!HeaderField || !Header) {
        return 0;
    }

    if (!strncmp(HTTP_PREFIX, Header, HTTP_PREFIX_LG)) {
	    Header += HTTP_PREFIX_LG;
	    HttpPrefix = TRUE;
    }

    while (HeaderField[n] && Header[n]) {
	    if (HttpPrefix)	{
		    if (HeaderField[n] == '-' && Header[n] != '_')
			    return(HeaderField[n] - Header[n] != '_');
		    if (0 != (rtc = toupper(HeaderField[n]) - Header[n]))
			    return(rtc);
    	} else {
		    if (0 != (rtc = toupper(HeaderField[n]) - toupper(Header[n])))
			    return(rtc);
	    }
	    n++;
    }

    return(HeaderField[n] - Header[n]);

}

/*========================================================================*/

char *FindHeader( struct st_http_req *HttpReq, char *Header )
{

    char		*p;
    size_t		i;

    if (!HttpReq || !Header) {
        return NULL;
    }

    /* insert webdav */
    for (i=1; i < HttpReq->NoOfHeaderLines; i++) {
	    p=HttpReq->HeaderNamesArray[i];
	    if (!httpheadercmp(p, Header))
		    return(HttpReq->HeaderValuesArray[i]);
    }
    /* end insert webdav */
    /* delete webdav
    for (i=1, p=HttpReq->HeaderNamesArray[i]; i < HttpReq->NoOfHeaderLines; p=HttpReq->HeaderNamesArray[++i]) {
	    if (!httpheadercmp(p, Header))
		    return(HttpReq->HeaderValuesArray[i]);
    }
    end delete webdav */

    return(NULL);

}

/*========================================================================*/

void LogHeaders( struct st_http_req	*HttpReq )
{

    char tmp[100], *p;
    size_t		i;
 
    if (!HttpReq) {
        return;
    }

    for (i=1, p=HttpReq->HeaderNamesArray[i]; i < HttpReq->NoOfHeaderLines; p=HttpReq->HeaderNamesArray[++i]) {
	    sp77sprintf(tmp, 100, "Header %s", p);
	    Log(p, -1);
    }

}

/*========================================================================*/

int GetHeaderNo( struct st_http_req	*HttpReq, size_t HeaderNo, char **HeaderName, char **HeaderValue )
{


    if (!HttpReq || !HeaderName || !HeaderValue || 0 == HeaderNo || HeaderNo >= HttpReq->NoOfHeaderLines) {
	    return(FALSE);
    }

    *HeaderName = HttpReq->HeaderNamesArray[HeaderNo];
    *HeaderValue = HttpReq->HeaderValuesArray[HeaderNo];

    while (*HeaderValue && **HeaderValue == ' ') (*HeaderValue)++;

    return(TRUE);

}

/*========================================================================*/

int GetContentLength( struct st_http_req    *HttpReq,
                      size_t                *Cl,
					  int					*chunked)
{

	char    *cl = NULL;

    if (!HttpReq || !Cl || !chunked) {
        return FALSE;
    }

	*chunked = FALSE;
	cl = FindHeader(HttpReq, HTTP_TRANSFER_ENCODING);

	if (cl) {
        if (!strcmp(cl, HTTP_CHUNKED)) {
            *Cl = 0;
			*chunked = TRUE;
            return(TRUE);
        } else {
            return(FALSE);
        }
    } else {
        cl = FindHeader(HttpReq, HTTP_CONTENT_LENGTH);
        if (!cl) {
            return(FALSE);
        } else {
            *Cl = atol(cl);
            return(TRUE);
        }
    }

}

/*========================================================================*/

int HttpIsReqConsistent( struct st_http_req	*HttpReq, char **Status )
{

    if (!HttpReq || !Status) {
        return FALSE;
    }

    /* check consistency */
    if (HttpReq->HMajorVersion >= 1) {
        if (HttpReq->HMinorVersion < 1) {
		    HttpReq->PersistentRequested = FALSE;
        } else {
            /*
		    if (!HttpReq->ContentLengthValid)
			    HttpReq->PersistentRequested = FALSE;
            */
	    }

        if (HttpReq->HMinorVersion >= 0) {
		    if ((HttpReq->Method == rq_post || HttpReq->Method == rq_put) && !HttpReq->ContentLengthValid) {
			    Log_Error("HTTP/1.0 with POST method requires Content-Length", -1);
			    *Status = HTTP_STATUS_LENGTH_REQUIRED;

			    return(FALSE);
		    }
	    }

	    if (HttpReq->HMinorVersion >= 1) {
		    if (!HttpReq->Host) {
			    Log_Error("HTTP/1.0 requires Host header field", -1);
			    *Status = HTTP_STATUS_BAD_REQUEST;

			    return(FALSE);
		    }
	    }
    } else {
	    /* keep-alive is not supported for 1.0 or lower */
	    HttpReq->PersistentRequested = FALSE;

    }

    return(TRUE);

}

/*========================================================================*/

int HttpGetVersion( char *Hversion, int	*Major, int	*Minor )
{

    if (!Hversion || !Major || !Minor) {
        return FALSE;
    }

    if (!strncmp(Hversion, "HTTP/", 5)) {
	    return(2 == sscanf(Hversion+5, "%d.%d", Major, Minor));
    } else {
	    Log_Error("Invalid HTTP Version format", -1);

	    return(FALSE);
    }

}

/*========================================================================*/

int HttpGetHost( struct st_http_req	*HttpReq, char **Host )
{

    char	*h;

    if (!HttpReq || !Host) {
        return FALSE;
    }

    h = FindHeader(HttpReq, HTTP_HOST);

    if (!h)
	    return(FALSE);

    *Host = h;

    return(TRUE);

}

/*========================================================================*/

int HttpIsPersistent( struct st_http_req    *HttpReq )
{

    char	*h;

    if (!HttpReq) {
        return FALSE;
    }

    h = FindHeader(HttpReq, HTTP_CONNECTION);

    return !(h && !strcmp(h, HTTP_CLOSE));

}

/*========================================================================*/

int HttpIsKeepAlive( struct st_http_req *HttpReq )
{

    char	*h;

    if (!HttpReq) {
        return FALSE;
    }

    h = FindHeader(HttpReq, HTTP_CONNECTION);

    return (h && (!strcmp(h, HTTP_KEEP_ALIVE) || !strcmp(h, "keep-alive")));

}

/*========================================================================*/

int GetHostPort( struct st_http_req	*HttpReq, char **Host, char **Port )
{

    if (!HttpReq || !Host || !Port) {
        return FALSE;
    }

    *Host = FindHeader(HttpReq, HTTP_HOST);
     if (!*Host) {
	    *Host = NULL;
	    *Port = NULL;

	    return(FALSE);
    } else {
	    /* PTS1117691 Try to read the port from the Headers 
	       was set in prepare_headers to <port> or to empty string if host was given
	    -> should not be null in this case */
	    *Port = FindHeader(HttpReq, HTTP_PORT);
	    /* PTS1117691 
	    *Port = strchr(*Host, ':'); */
	    /* PTS1117759: This is now done in WAHTTP_Server.c::PrepareHeaders */
	    /*if (*Port && strlen(*Port)>0)	{
		    **Port = '\0';
		    (*Port)++;
	    }*/

	    return(TRUE);
    }

}

/***********************************************************************

	End

 ***********************************************************************/
