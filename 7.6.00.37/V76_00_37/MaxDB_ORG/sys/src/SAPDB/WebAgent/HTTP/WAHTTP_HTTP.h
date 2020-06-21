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
#ifndef WAHTTP_HTTP_INC
#define WAHTTP_HTTP_INC

#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"


#define MAXURISIZE		2000
#define MAX_ERRORTEXT_LG	100

#define HTTP_VERSION		"HTTP/1.0"

#define HTTP_HEADER_SEP		"\r\n"
#define HTTP_HEADER_SEP_LG	sizeof(HTTP_HEADER_SEP)-1

#define HTTP_PORT               "Port"
#define HTTP_HOST               "Host"
#define HTTP_CONTENT_LENGTH     "Content-Length"
#define HTTP_CONTENT_TYPE       "Content-Type"
#define HTTP_DATE               "Date"
#define HTTP_LAST_MODIFIED      "Last-Modified"
#define HTTP_SERVER             "Server"
#define HTTP_IF_MODIFIED_SINCE  "If-Modified-Since"
#define HTTP_SET_COOKIE         "Set-Cookie"
#define HTTP_EXPIRES            "Expires"
#define HTTP_LOCATION           "Location"
#define HTTP_CONNECTION         "Connection"
#define HTTP_KEEP_ALIVE         "Keep-Alive"
#define HTTP_CLOSE              "close"
#define HTTP_TRANSFER_ENCODING  "Transfer-Encoding"
#define HTTP_CHUNKED            "chunked"

#define HTTP_STATUS_OK			"200 OK"
#define HTTP_STATUS_NOT_MODIFIED	"304 Not Modified"
#define HTTP_STATUS_BAD_REQUEST		"400 Bad Request"
#define HTTP_STATUS_NOT_FOUND		"404 Not Found"
#define HTTP_STATUS_LENGTH_REQUIRED	"411 Length Required"

#define HTTP_STATUS_CREATED		"201 Created"
#define HTTP_STATUS_NO_CONTENT		"204 No Content"
#define HTTP_STATUS_MOVE_PERM		"301 Moved Permanently"
#define HTTP_STATUS_REDIRECT		"307 Temporary Redirect"
#define HTTP_STATUS_UNAUTHORIZED	"401 Unauthorized"
#define HTTP_STATUS_FORBIDDEN		"403 Forbidden"
#define HTTP_STATUS_METHOD_NOT_ALLOWED	"405 Method Not Allowed"
#define HTTP_STATUS_CONFLICT		"409 Conflict"
#define HTTP_STATUS_SERVER_ERROR	"500 Internal Server Error"
#define HTTP_STATUS_NOT_IMPLEMENTED	"501 Not Implemented"
/* WebDAV specific HTTP status codes */
#define HTTP_STATUS_PARTIAL_CONTENT			"206 Partial Content"					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
#define HTTP_STATUS_MULTISTATUS				"207 Multistatus"
#define HTTP_STATUS_PRECONDITION_FAILED		"412 Precondition Failed"
#define HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE	"415 Unsupported Media Type"
#define HTTP_STATUS_RANGE_NOT_SATISFIABLE	"416 Requested Range Not Satisfiable"	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
#define HTTP_STATUS_LOCKED					"423 Locked"
#define HTTP_STATUS_BAD_GATEWAY				"502 Bad Gateway"
#define HTTP_STATUS_INSUFFICENT_STORAGE		"507 Insufficent Storage"

#ifndef THIS_SERVER
#define THIS_SERVER		"SAP-DB-WWW/1.0"
#endif

/* delete webdav
enum en_reqtype
{
 rq_get = 0,
 rq_head,
 rq_post
};
*/
/* insert webdav */
enum en_reqtype
{
 rq_get = 0,
 rq_head,
 rq_post,
 rq_put,
 rq_options,
 rq_connect,
 rq_trace,
 rq_mkcol,
 rq_propfind,
 rq_proppatch,
 rq_delete,
 rq_copy,
 rq_move,
 rq_lock,
 rq_unlock,
 rq_versioncontrol,
 rq_checkin,
 rq_checkout,
 rq_uncheckout,
 rq_report,
 rq_xquery
};
/* end insert webdav */

enum en_http_message_type
{
 hmt_simple_request = 0,
 hmt_full_request
};

struct st_http_client
{
 DynStr				HeadersLine;
 int				HeadersSent;
 struct st_tcp_connection	*tcp_con;
/* SOCKET		cs;
 SOCKADDR_IN	sa;*/
};

struct st_http_req {
    DynStr                        HeadersLine;
    size_t                        NoOfHeaderLines;
    char                        **HeaderNamesArray;
    char                        **HeaderValuesArray;
    char                         *method;
    char                         *uri;
    char                         *hversion;
    char                         *params;
    char                         *querystring;
    int                           HMajorVersion;
    int			                  HMinorVersion;
    char                         *Host;
    char                         *Port;
    enum en_http_message_type     Type;
    enum en_reqtype               Method;
    int                           PersistentRequested;
    int                           ContentLengthValid;
    size_t                        ContentLength;
    int                           contentChunked;
    unsigned long                 chunkSize;
    struct st_data                Body;
    char                         *alreadyReadBody;
    WAHTTP_LogDesc                logDesc;
};

int AddHeader_char
(
 struct st_http_client	*client,
 char			*Header,
 char			*Value
);

int AddHeader_size_t
(
 struct st_http_client	*client,
 char			*Header,
 size_t			Value
);

int AddHeader_date
(
 struct st_http_client	*client,
 char			*Header,
 time_t			Value
);

int SendHttpReply
(
 char			*Status,
 struct st_http_client	*client
);

int SendHttpHeader
(
 char			*Status,
 struct st_http_client	*client
);

int SendHttpBody( struct st_http_client *client,
                  void                  *b,
                  size_t                 n,
                  size_t                *rn );

char *FindHeader
(
 struct st_http_req	*HttpReq,
 char			*Header
);

int GetHeaderNo
(
 struct st_http_req	*HttpReq,
 size_t			HeaderNo,
 char			**HeaderName,
 char			**HeaderValue
);

int GetContentLength(struct st_http_req	*HttpReq, size_t *Cl, int *chunked);

int HttpIsReqConsistent
(
 struct st_http_req	*HttpReq,
 char			**Status
);

int HttpGetVersion
(
 char	*Hversion,
 int	*Major,
 int	*Minor
);

int HttpGetHost
(
 struct st_http_req	*HttpReq,
 char			**Host
);

int HttpIsPersistent( struct st_http_req *HttpReq );
int HttpIsKeepAlive( struct st_http_req *HttpReq );

int GetHostPort
(
 struct st_http_req	*HttpReq,
 char			**Host,
 char			**Port
);

#endif