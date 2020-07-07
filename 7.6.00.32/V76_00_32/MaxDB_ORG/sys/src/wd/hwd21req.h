/*!**********************************************************************

  module: hwd21req.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  HttpRequest functions

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

#ifndef HWD21REQ_H
#define HWD21REQ_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd26err.h"
#include "hwd27excl.h"
#include "hwd28dict.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: sapdbwa_HttpRequestP

  description:
  
************************************************************************/

typedef struct st_wa_log_desc   *WA_LogDesc;

struct st_wa_log_desc {
    tsp00_Int4             logFile;
    twd27ExclP           logExcl;
    tsp00_Int4             errFile;
    twd27ExclP           errExcl;
};


typedef sapdbwa_HttpRequestP twd21HttpRequestP;

struct st_wa_http_req
{
    const char	*Uri;		        /* Url without the nodename  */
    const char	*IfModifiedSince;   /* HTTP Date, Only if requested is newer */
    const char	*PathInfo;          /* More parameters */
    const char	*RequestMethod;	    /* Method of request(HEAD or POST) */
    const char	*ContentType;	    /* Encoding type */
    const char	*ContentLength;	    /* Length of message body */
    sapdbwa_Bool contentChunked;    /* content chunked? */
    const char	*PhysicalPath;
    const char	*ServerName;
    
    twd28DictionaryP   cookieValues;    
    twd28DictionaryP   paramValues;
    sapdbwa_Bool       cookiesInserted;
    sapdbwa_Bool       paramsInserted;
    char              *bodyData;         /* for POST requests the first chunk of body data is
                                          * copied to buffers bodyData and paramBodyData */
    char              *paramBodyData;    /* copy of bodyData used for method
                                          * POST. InsertParams replaces '='
                                          * and '&' with '\0' */
    sapdbwa_ULong      bodyDataLen;      /* length of first chunk copied to bodyData */
    sapdbwa_ULong      bodyDataPos;      /* actual position in bodyData for
                                          * WAReadBody */


    sapdbwa_ULong      numContentLength; /* numerical content length */
    sapdbwa_ULong      readBodyBytes;    /* number of bytes already read from the request body. */


    char              *mimeBodyData;         
    sapdbwa_ULong      mimeBodyDataBufSize;           /* allocated size of buffer *mimeBodyData */
    sapdbwa_ULong      mimeBodyDataLen;               /* length of chunk copied to mimeBodyData */
    sapdbwa_ULong      mimeBodyDataPos;               /* actual position in mimeBodyData. */
    sapdbwa_ULong      mimeDelimiterToFlush;          /* how much of the delimiter has to be flushed into the
                                                         output buffer. */
    sapdbwa_ULong      mimeDelimiterToFlushOffset;    /* position up to which the delimiter has to be flushed. */



    char              *queryString;
    char              *paramQueryString;
    char              *authHeader;
    
    twd00ReadBodyFunc    ReadBody;
    twd00GetHeaderFunc   GetHeader;
    twd00IsSecureFunc    IsSecure;

    WA_LogDesc           logDesc;       /* Web server's log files description */
    
    twd00ServerHandle    serverHandle;
    twd26ErrP            err;
}; /* struct st_wa_http_req */


SAPDB_Bool wd21CreateRequestHandlePool();

void wd21DestroyRequestHandlePool();
    
twd21HttpRequestP wd21CreateRequest( twd00ServerHandle  serverHandle,
                                     twd00ReadBodyFunc  readBody,
                                     twd00GetHeaderFunc getHeader,
                                     twd00IsSecureFunc  isSecure );

SAPDB_ULong wd21GetNumericalContentLength( twd21HttpRequestP req  );

SAPDB_ULong wd21GetReadBodyBytes( twd21HttpRequestP req  );

void wd21DestroyRequest( twd21HttpRequestP req );

twd00ServerHandle wd21GetServerHandle( twd21HttpRequestP  req );


void wd21SetErr( twd21HttpRequestP req, twd26ErrP err );


void wd21SetUri( twd21HttpRequestP  req,
                 const char        *uri );


void wd21SetIfModifiedSince( twd21HttpRequestP  req,
                             const char        *ifModifiedSince );

void wd21SetQueryString( twd21HttpRequestP  req,
                         const char        *queryString );

void wd21SetPathInfo( twd21HttpRequestP  req,
                      const char        *pathInfo );

void wd21SetRequestMethod( twd21HttpRequestP  req,
                           const char        *requestMethod );

void wd21SetContentChunked( twd21HttpRequestP   req,
                            sapdbwa_Bool        contentChunked );

void wd21SetContentType( twd21HttpRequestP  req,
                         const char        *contentType );

void wd21SetContentLength( twd21HttpRequestP  req,
                           const char        *contentLength );

void wd21SetPhysicalPath( twd21HttpRequestP  req,
                          const char        *physicalPath );

void wd21SetServerName( twd21HttpRequestP  req,
                        const char        *serverName );

void wd21_UnescapeUrl( char *url, sapdbwa_Int4 len );



/*!**********************************************************************

  EndChapter: sapdbwa_HttpRequestP

************************************************************************/
#if defined(__cplusplus)
}
#endif

#endif

