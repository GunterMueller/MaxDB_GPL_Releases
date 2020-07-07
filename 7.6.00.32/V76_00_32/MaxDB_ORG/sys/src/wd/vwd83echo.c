/*!**********************************************************************

  module: vwd83echo.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  UserDll that echos the request

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>

#include "hwd01wadef.h"
#include "hwd03waapi.h"
#include "hwd20control.h"

sapdbwa_Int2 wd83Init( sapdbwa_Handle wa )
{
    return 1;
} /* wd82FSInit */

sapdbwa_Int2 wd83Exit( sapdbwa_Handle wa )
{
    return 1;
} /* wd82FSInit */

#define BUFF_SIZE 32000

void wd83ShowVal( sapdbwa_HttpReplyP   rep,
                  const char     *name,
                  const char     *val );

sapdbwa_Int2 wd83Service( sapdbwa_Handle         wa,
                        sapdbwa_HttpRequestP   req,
                        sapdbwa_HttpReplyP     rep )
{
    const char *requestURI;
    const char *ifModifiedSince;
    const char *queryString;
    const char *pathInfo;
    const char *method;
    const char *contentType;
    const char *contentLength;
    const char *pathTranslated;
    const char *serverName;

    /* general headers */
    const char *cacheControl;
    const char *connection;
    const char *date;
    const char *pragma;
    const char *transferEncoding;
    const char *upgrade;
    const char *via;

    /* reuqest headers */
    const char *acceptCharset;
    const char *acceptEconding;
    const char *acceptLanguage;
    const char *authorization;
    const char *proxyAuthorisation;
    const char *from;
    const char *host;
    const char *ifUnmodifiedSince;
    const char *ifMatch;
    const char *ifNoneMatch;
    const char *ifRange;
    const char *maxForwards;
    const char *range;
    const char *referer;
    const char *userAgent;
    
    const char  bodyData[BUFF_SIZE] = "";

    requestURI      = sapdbwa_GetRequestURI( req );
    ifModifiedSince = sapdbwa_GetIfModifiedSince( req );
    queryString     = sapdbwa_GetQueryString( req );
    pathInfo        = sapdbwa_GetPathInfo( req );
    method          = sapdbwa_GetMethod( req );
    contentType     = sapdbwa_GetContentType( req );
    contentLength   = sapdbwa_GetContentLength( req );
    pathTranslated  = sapdbwa_GetPathTranslated( req );
    serverName      = sapdbwa_GetServerName( req );

    /* general headers */
    cacheControl = sapdbwa_GetHeader( req, "HTTP_CACHE_CONTROL" );
    connection = sapdbwa_GetHeader( req, "HTTP_CONNECTION" );
    date = sapdbwa_GetHeader( req, "HTTP_DATE" );
    pragma = sapdbwa_GetHeader(  req,"HTTP_PRAGMA" );
    transferEncoding = sapdbwa_GetHeader(  req,"HTTP_TRANSFER_ENCODING" );
    upgrade = sapdbwa_GetHeader(  req, "HTTP_UPGRADE" );
    via = sapdbwa_GetHeader(  req,"HTTP_VIA" );

    /* reuqest headers */
    acceptCharset = sapdbwa_GetHeader(  req,"HTTP_ACCEPT" );
    acceptEconding = sapdbwa_GetHeader(  req,"HTTP_ACCEPT_ENCODING" );
    acceptLanguage = sapdbwa_GetHeader(  req,"HTTP_ACCEPT_LANGUAGE" );
    authorization = sapdbwa_GetHeader(  req,"HTTP_AUTHORIZATION" );
    proxyAuthorisation = sapdbwa_GetHeader(  req,"HTTP_PROXY_AUTHORISATION" );
    from = sapdbwa_GetHeader(  req,"HTTP_FROM" );
    host = sapdbwa_GetHeader(  req,"HTTP_HOST" ); /* == ServerName */
    ifUnmodifiedSince = sapdbwa_GetHeader(  req,"IF_MODIFIED_SINCE" );
    ifMatch = sapdbwa_GetHeader(  req,"IF_MATCH" );
    ifNoneMatch = sapdbwa_GetHeader(  req,"IF_NONE_MATCH" );
    ifRange = sapdbwa_GetHeader(  req,"IF_RANGE" );
    maxForwards = sapdbwa_GetHeader(  req,"MAX_FORWARDS" );
    range = sapdbwa_GetHeader(  req,"RANGE" );
    referer = sapdbwa_GetHeader(  req,"REFERER" );
    userAgent = sapdbwa_GetHeader( req, "HTTP_USER_AGENT" );
    
    if (!sapdbwa_InitHeader( rep,
                        sapdbwa_Status_Ok,
                        "text/html",
                        NULL, NULL, NULL, NULL )) {
        /* invalid date format !? */
        return sapdbwa_RTC_Error;
    }; /* if */
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
    sapdbwa_SendHeader( rep );

    sapdbwa_SendBody( rep, "<body>", 0 );

    wd83ShowVal( rep, "requestURI       = %s <P>", requestURI );
    wd83ShowVal( rep, "ifModifiedSince  = %s <P>", ifModifiedSince );
    wd83ShowVal( rep, "queryString      = %s <P>", queryString );
    wd83ShowVal( rep, "pathInfo         = %s <P>", pathInfo );
    wd83ShowVal( rep, "method           = %s <P>", method );
    wd83ShowVal( rep, "contentType      = %s <P>", contentType );
    wd83ShowVal( rep, "contentLength    = %s <P>", contentLength );
    wd83ShowVal( rep, "pathTranslated   = %s <P>", pathTranslated );
    wd83ShowVal( rep, "serverName       = %s <P>", serverName );    
    wd83ShowVal( rep, "cacheControl     = %s <P>",cacheControl );
    wd83ShowVal( rep, "connection       = %s <P>", connection);
    wd83ShowVal( rep, "date             = %s <P>", date);
    wd83ShowVal( rep, "pragma           = %s <P>", pragma);
    wd83ShowVal( rep, "transferEncoding = %s <P>", transferEncoding );
    wd83ShowVal( rep, "upgrade          = %s <P>",upgrade );
    wd83ShowVal( rep, "via              = %s <P>", via);
    wd83ShowVal( rep, "acceptCharset    = %s <P>",acceptCharset );
    wd83ShowVal( rep, "acceptEconding   = %s <P>", acceptEconding);
    wd83ShowVal( rep, "acceptLanguage   = %s <P>", acceptLanguage);
    wd83ShowVal( rep, "authorization    = %s <P>",authorization );
    wd83ShowVal( rep, "proxyAuthorisation  = %s <P>",proxyAuthorisation );
    wd83ShowVal( rep, "from             = %s <P>",from );
    wd83ShowVal( rep, "host             = %s <P>", host);
    wd83ShowVal( rep, "ifUnmodifiedSince   = %s <P>",ifUnmodifiedSince );
    wd83ShowVal( rep, "ifMatch          = %s <P>", ifMatch);
    wd83ShowVal( rep, "ifNoneMatch      = %s <P>", ifNoneMatch);
    wd83ShowVal( rep, "ifRange          = %s <P>", ifRange);
    wd83ShowVal( rep, "maxForwards      = %s <P>", maxForwards);
    wd83ShowVal( rep, "range            = %s <P>", range);
    wd83ShowVal( rep, "userAgent        = %s <P>", userAgent);
    wd83ShowVal( rep, "referer          = %s <P>", referer);

    sapdbwa_SendBody( rep, "</body>", 0 );
    
    return sapdbwa_RTC_Accepted;
} /* vwd83Service */


void wd83ShowVal( sapdbwa_HttpReplyP rep, const char *name, const char *val )
{
    char textBuffer[1024];
    
    if (val != NULL) {
        sp77sprintf(textBuffer, 1023, name, val);
    } else {
        sp77sprintf(textBuffer, 1023, name, "NULL");
    }; /* else */
    strcat( textBuffer, "\n" );
    
    sapdbwa_SendBody( rep, textBuffer, strlen( textBuffer ) );
} /* wd83ShowVal */
