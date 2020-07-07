/*!**********************************************************************

  module: vwd22rep.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  HttpReply functions

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

#include <stdio.h>
#include "hwd12hpool.h"
#include "hwd20control.h"
#include "hwd22rep.h"
#include "hwd29date.h"
#include "hwd91mime.h"

/****************************************************************************
 ***
 *** Module
 ***
 ****************************************************************************/

sapdbwa_TraceDefModule ("vwd22rep.c")

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

void * wd22_ReplyConstructor();

void wd22_ReplyDestructor( void * replyHandle );

void wd22_ReplyInit( sapdbwa_HttpReplyP       rep,
                     twd00ServerHandle   handle,
                     twd00SendHeaderFunc SendHeader,
                     twd00SendBodyFunc   SendBody );

sapdbwa_Bool wd22_CreateString( char       **newString,
                              const char  *sourceString );

#define HEADER_COOKIE "Set-Cookie"  /* header name for cookies    */

/*!**********************************************************************

  Chapter: sapdbwa_HttpReplyP (functions from hwd03waapi.h)

  description:
  
************************************************************************/
 
struct st_wa_http_rep
{
    sapdbwa_Int2  Status;
    char       *Location;
    char       *LastModified;
    char       *ContentLength;
    char       *ContentType;
    char       *Expires;

    twd28DictionaryP headers; /* additional header fields */
    
    twd00SendHeaderFunc  SendHeader;
    twd00SendBodyFunc    SendBody;
    
    twd00ServerHandle    handle;
    twd26ErrP            err;

    sapdbwa_Bool         headersSent;
}; /* struct st_wa_http_rep */


static twd12HandlePoolP  wd22replyPool = NULL;

sapdbwa_Bool wd22CreateReplyHandlePool()
{
    wd22replyPool = wd12CreateHandlePool( wd22_ReplyConstructor,
                                          wd22_ReplyDestructor );

    return (wd22replyPool != NULL);
} /* wd22CreateReplyHandlePool */


void wd22DestroyReplyHandlePool()
{
    wd12DestroyHandlePool( wd22replyPool );
    wd22replyPool = NULL;
} /* wd22DestroyReplyHandlePool */


sapdbwa_HttpReplyP wd22CreateReply( twd00ServerHandle   handle,
                              twd00SendHeaderFunc SendHeader,
                              twd00SendBodyFunc   SendBody )
{
    sapdbwa_HttpReplyP  newReply = NULL;

    /* get reply handle from global pool */
    newReply = wd12GetHandle( wd22replyPool );
    if (newReply != NULL) {
        wd22_ReplyInit( newReply, handle, SendHeader, SendBody );
    };
    
    return newReply;
} /* wd22CreateReply */

    
void wd22DestroyReply( sapdbwa_HttpReplyP rep )
{
    /* put handle back to pool */
    wd12PutHandle( wd22replyPool, rep );
} /* wd22DestroyReply */


twd28DictionaryP wd22GetHeaders( sapdbwa_HttpReplyP rep )
{
    return rep->headers;
} /* wd22GetHeaders */


sapdbwa_Int2  wd22GetStatus( sapdbwa_HttpReplyP rep )
{
    return rep->Status;
} /* wd22GetStatus */


twd00ServerHandle wd22GetServerHandle( sapdbwa_HttpReplyP rep )
{
    return rep->handle;
} /* wd22GetServerHandle */


char* wd22GetLocation( sapdbwa_HttpReplyP rep )
{
    return rep->Location;
} /* wd22GetLocation */


char* wd22GetLastModified( sapdbwa_HttpReplyP rep )
{
    return rep->LastModified;
} /* wd22GetLastModified */


char* wd22GetContentLength( sapdbwa_HttpReplyP rep )
{
    return rep->ContentLength;
} /* wd22GetContentLength */


char* wd22GetContentType( sapdbwa_HttpReplyP rep )
{
    if (rep->ContentType != NULL)
        return rep->ContentType;
    else
        return "";
} /* wd22GetContentType */


char* wd22GetExpires( sapdbwa_HttpReplyP rep )
{
    return rep->Expires;
} /* wd22GetGetExpires */


sapdbwa_ErrP WINAPI sapdbwa_RepGetErr( sapdbwa_HttpReplyP rep )
{
    return rep->err;
} /* sapdbwa_RepGetErr */


sapdbwa_Bool WINAPI sapdbwa_InitHeader(  sapdbwa_HttpReplyP  rep,
                                         sapdbwa_Int2		 Status,
										 const char			*ContentType,
										 const char			*ContentLength, 
										 const char			*LastModified,      
										 const char			*Expires,      
										 const char			*Location )
{

	char expiresTmp[MAX_DATE_LEN_WD29];
    char lastModifiedTmp[MAX_DATE_LEN_WD29];

	sapdbwa_TraceDefFunction ("sapdbwa_InitHeader")

    expiresTmp[0] = '\0';
    lastModifiedTmp[0] = '\0';
    if (Expires != NULL && strlen( Expires ) > 0) {
        if (wd29WADateToHttpDate( Expires, expiresTmp ) <= 0) {
            wd26SetErr( rep->err, ERR_INVALID_DATE_FORMAT_WD26, Expires, NULL );

			sapdbwa_TraceAdd( "Exiting function unexpected!" );

            return false;
        }; /* if */
    }; /* if */
    if (LastModified != NULL && strlen( LastModified ) > 0) {
        if (wd29WADateToHttpDate( LastModified, lastModifiedTmp ) <= 0) {
            wd26SetErr( rep->err, ERR_INVALID_DATE_FORMAT_WD26,
                        LastModified, NULL );

			sapdbwa_TraceAdd( "Exiting function unexpected!" );

            return false;
        }; /* if */
    }; /* if */
        
    rep->Status        = Status;    
    if ( wd22_CreateString( &rep->ContentType, ContentType )
         && wd22_CreateString( &rep->ContentLength, ContentLength )
         && wd22_CreateString( &rep->LastModified, lastModifiedTmp )
         && wd22_CreateString( &rep->Expires, expiresTmp )
         && wd22_CreateString( &rep->Location, Location )) {

		sapdbwa_TraceAdd( "Exiting function." );

        return true;
    } else {
        /* memory allocation error */

		sapdbwa_TraceAdd( "Exiting function unexpected!" );

        return false;
    }; /* else */

} /* sapdbwa_InitHeader */

sapdbwa_Bool WINAPI sapdbwa_SetHeader( sapdbwa_HttpReplyP  rep,
                               const char   *name,
                               const char   *value )
{
    if (wd28AddCopyEntry( rep->headers, name, value )) {
        return true;
    } else {
        return false;
    }; /* else */
} /* sapdbwa_SetHeader */

sapdbwa_Bool WINAPI sapdbwa_ReplaceHeader( sapdbwa_HttpReplyP  rep,
                                           const char         *name,
                                           const char         *oldvalue,
                                           const char         *value )
{
    if (wd28ReplaceCopyEntry( rep->headers, name, oldvalue, value )) {
        return true;
    } else {
        return false;
    }; /* else */
} /* sapdbwa_SetHeader */

const char * wd22_FindCookie ( sapdbwa_HttpReplyP  rep,
                               const char         *name)
{
  sapdbwa_StringSeqP     strSeq  = sapdbwa_CreateStringSeq();      
  sapdbwa_Int4           nIndex  = 0;
  const char *           pValue  = NULL;
  const char *           pReturn = NULL;

  wd28GetValuesByKey(rep->headers, HEADER_COOKIE, strSeq);

  /* now all cookies in strSeq */
  for (nIndex = 0; nIndex < sapdbwa_GetNumElem(strSeq); ++nIndex) {
      pValue = sapdbwa_GetStringByIndex(strSeq, nIndex);
      if ((strncmp(pValue, name, strlen(name)) == 0) && (pValue[strlen(name)] == '=')) {
            pReturn = pValue;
      } /* if */
  } /* for */

  return pReturn;
} /* wd22_FindCookie */

void wd22_AddCookieField( char       *cookieHeader,
                          const char *name,
                          const char *value )
{
    if (value != NULL) {
        strcat( cookieHeader, name );
        strcat( cookieHeader, "=" );
        strcat( cookieHeader, value );
        strcat( cookieHeader, ";" );
    }; /* if */
} /* wd22_AddCookieField */

char *wd22_MakeCookieString( const char    *name,
                             const char    *value,
                             const char    *expires,
                             const char    *path,
                             const char    *domain,
                             sapdbwa_Bool   secure )
{
    /* Attention!
       This function allocates memory for the cookie string 
       The caller have to free this memory.
    */
    sapdbwa_Int4  cookieLen    = 0;
    char         *cookieHeader = NULL;
    sapdbwa_Bool  allocatOK    = true;
    char          httpDate[MAX_DATE_LEN_WD29];
    char         *expiresStr   = NULL;

    if (name == NULL || value == NULL) {
        /* name and value are required all other are optional */
        return NULL;
    }; /* if */
    cookieLen = strlen( name ) + strlen( value );
    if (expires != NULL) {
        if (!wd29WADateToHttpDate( expires, httpDate )) {
            /* wrong date format */
            return NULL;
        }; /* if */
        cookieLen += strlen( httpDate );
        if (strlen(httpDate) == 0) {
            expiresStr = NULL;
        } else {
            expiresStr = httpDate;
        }; /* else */
    }; /* if */
    if (path != NULL) {
        cookieLen += strlen( path );
    }; /* if */
    if (domain != NULL) {
        cookieLen += strlen( domain );
    }; /* if */
    cookieLen += 128*sizeof(char); /* extra space for '=', ';', 'secure' ... */
    sapdbwa_SQLALLOCAT( cookieLen, (sapdbwa_UInt1**) &cookieHeader, &allocatOK );
    if (allocatOK) {
        cookieHeader[0] = '\0';
        wd22_AddCookieField( cookieHeader, name, value );
        /*wd22_AddCookieField( cookieHeader, "Expires", expiresStr );*/
        wd22_AddCookieField( cookieHeader, "Path", path );
        wd22_AddCookieField( cookieHeader, "Domain", domain );
        if (secure) {
            strcat( cookieHeader, ";Secure" );
        }; /* if */
        return cookieHeader;
    }; /* if */

    return NULL;
} /* wd22_MakeCookieString */

sapdbwa_Bool WINAPI sapdbwa_SetCookie( sapdbwa_HttpReplyP  rep,
                                const char    *name,
                                const char    *value,
                                const char    *expires,
                                const char    *path,
                                const char    *domain,
                                sapdbwa_Bool     secure )
{
    char         *cookieHeader = NULL;
    sapdbwa_Bool  setCookieOK  = false;

    /* !!! wd22_MakeCookieString allocates memory for cookieHeader */
    cookieHeader = wd22_MakeCookieString(name, value,  expires,
                                         path, domain, secure );

    if (cookieHeader != NULL) {
        setCookieOK = sapdbwa_SetHeader( rep, HEADER_COOKIE, cookieHeader );
        /* free the memory allocated in wd22_MakeCookieString */ 
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cookieHeader );
    } /* if */

    return setCookieOK;

} /* sapdbwa_SetCookie */


sapdbwa_Bool WINAPI sapdbwa_ReplaceCookieValue( sapdbwa_HttpReplyP  rep,
                                                const char         *name,
                                                const char         *value)
{
  const char   * pCookie         = wd22_FindCookie(rep, name);
  char         * oldCookieHeader = NULL;
  char         * newCookieHeader = NULL;
  sapdbwa_Bool   setCookieOK     = false;
  sapdbwa_Bool   allocatOK       = true;
  char         * expires         = NULL;
  char         * path            = NULL;
  char         * domain          = NULL;
  sapdbwa_Bool   secure          = false;
  
  if (pCookie != NULL) {

    /* copy value */
    sapdbwa_SQLALLOCAT( strlen(pCookie) + 1, (sapdbwa_UInt1**) &oldCookieHeader, &allocatOK );

    if (allocatOK) {

      strcpy(oldCookieHeader, pCookie);

      /* extract expires                                   */
      expires = strstr(oldCookieHeader, ";Expires=");
      expires = (expires != NULL) ? expires + strlen(";Expires=") : expires;

      /* extract path */
      path = strstr(oldCookieHeader, ";Path=");
      path = (path != NULL) ? path + strlen(";Path=") : path;

      /* extract domain  */
      domain = strstr(oldCookieHeader, ";Domain=");
      domain = (domain != NULL) ? domain + strlen(";Domain=") : domain;

      /* extract secure */
      secure = strstr(oldCookieHeader, ";Secure") != NULL;

      /* terminate values */
      if (expires != NULL) {
        if (strstr(expires, ";") != NULL) {
          *strstr(expires, ";") = 0;
        } /* if */
      } /* if */
      if (path != NULL) {
        if (strstr(path, ";") != NULL) {
          *strstr(path, ";") = 0;
        } /* if */
      } /* if */
      if (domain != NULL) {
        if (strstr(domain, ";") != NULL) {
          *strstr(domain, ";") = 0;
        } /* if */
      } /* if */

      /* !!! wd22_MakeCookieString allocates memory for newCookieHeader */
      newCookieHeader = wd22_MakeCookieString(name, value,  expires,
      path, domain, secure );

      if (newCookieHeader != NULL) {
        setCookieOK = sapdbwa_ReplaceHeader( rep, HEADER_COOKIE, pCookie, newCookieHeader );
        /* free the memory allocated in wd22_MakeCookieString */ 
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) newCookieHeader );
      } /* if */

      sapdbwa_SQLFREE( (sapdbwa_UInt1*) oldCookieHeader );

    } /* if */

  } /* if */
  
  return setCookieOK;

} /* sapdbwa_SetSIDCookie */


sapdbwa_Bool WINAPI sapdbwa_SendHeader( sapdbwa_HttpReplyP rep )
{
    sapdbwa_Bool sendOk = true;
    
    if (!rep->headersSent) {
        sendOk = rep->SendHeader( rep->handle, rep );
        rep->headersSent = true;
    } else {
        wd26SetErr( rep->err, ERR_HEADERS_ALREADY_SENT_WD26, "", "" );
        sendOk = false;
    };

    return sendOk;
} /* sapdbwa_SendHeader */


sapdbwa_UInt4 WINAPI sapdbwa_SendBody( sapdbwa_HttpReplyP   rep,
                                const void     *buf,
                                sapdbwa_UInt4     len )
{
    sapdbwa_UInt4    realLen;
    
    if (len == 0) {
        realLen = strlen( (char*) buf );
    } else {
        realLen = len;
    }; /* else */
    
    return rep->SendBody( rep->handle, buf, realLen );
} /* sapdbwa_SendBody */

sapdbwa_UInt4 WINAPI sapdbwa_SendBodyChunk( sapdbwa_HttpReplyP   rep,
                                            const void          *buf,
                                            sapdbwa_UInt4        len )
{

    sapdbwa_UInt4   rc;
    sapdbwa_UInt4   realLen;
    char            chunkSize[20];
    
    if (len == 0) {
        realLen = strlen( (char*) buf );
    } else {
        realLen = len;
    };
    
    if (realLen) {
        sp77sprintf(chunkSize, 20, "%X", realLen);
        rc = rep->SendBody(rep->handle, chunkSize, strlen(chunkSize));

        if (rc) {
            chunkSize[0] = 0x0D;
            chunkSize[1] = 0x0A;
            rc = rep->SendBody(rep->handle, chunkSize, 2);
        }

        if (rc) {
            rc = rep->SendBody(rep->handle, buf, realLen);
        }

        if (rc) {
            chunkSize[0] = 0x0D;
            chunkSize[1] = 0x0A;
            rc = rep->SendBody(rep->handle, chunkSize, 2);
        }
    } else {
        chunkSize[0] = '0';
        chunkSize[1] = 0x0D;
        chunkSize[2] = 0x0A;
        chunkSize[3] = 0x0D;
        chunkSize[4] = 0x0A;
        rc = rep->SendBody(rep->handle, chunkSize, 5);
    }

    return rc;

}

sapdbwa_UInt4 WINAPI sapdbwa_SendBodyChunkEnd( sapdbwa_HttpReplyP   rep )
{

    char            chunkSize[20];

    chunkSize[0] = '0';
    chunkSize[1] = 0x0D;
    chunkSize[2] = 0x0A;
    chunkSize[3] = 0x0D;
    chunkSize[4] = 0x0A;
    return rep->SendBody(rep->handle, chunkSize, 5);

}


sapdbwa_Bool WINAPI sapdbwa_SendFile( sapdbwa_HttpReplyP  rep,
                               const char	 *fileName   )
{
	sapdbwa_Int4			file = -1;
	tsp05_RteFileError	err;
	char				buffer[MAX_FILE_BUF_SIZE_WD00];
	tsp00_Longint			readLen = 0;
	const char			*extension;
	sapdbwa_Int4			 i = 0;

	/* Open file for reading in binary mode */
	sqlfopenc( fileName, sp5vf_binary, sp5vf_read, sp5bk_buffered, &file, &err );
	if ( err.sp5fe_result != vf_ok ) {
		/* Error: Can not open file */

		/* Initialize reply header for the error case */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "", NULL, NULL, NULL, NULL );
		sapdbwa_SendHeader( rep );

		return sapdbwa_False;
	}

	/* Initialize reply header for the ok case*/
	for ( i = strlen( fileName ); i >= 0; i-- ) {
		if ( fileName[i] == '.' ) extension = fileName+i+1;
	}

	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, wd20GetMIMEType( extension ), NULL, NULL, NULL, NULL );
	sapdbwa_SendHeader( rep );

	/* Read and send file */
	do {
		sqlfreadc ( file, buffer, sizeof( buffer ), &readLen, &err );

		if ( readLen > 0 ) {
			if ( sapdbwa_SendBody ( rep, buffer, readLen ) < 1) {
				/* Error: Sending reply */
				sqlfclosec( file, sp5vf_close_normal, &err );

				return sapdbwa_False;
			}
		}
	} while ( readLen > 0 );
    
    /* Close file */
	sqlfclosec( file, sp5vf_close_normal, &err );

	return sapdbwa_True;

}


sapdbwa_Bool WINAPI sapdbwa_SendTemplate( sapdbwa_HttpReplyP   rep,
                                   const char     *fileName )
{
	sapdbwa_Int4			file = -1;
	tsp05_RteFileError	err;
	char				buffer[MAX_FILE_BUF_SIZE_WD00];
	tsp00_Longint			readLen = 0;

	/* Open file for reading in binary mode */
	sqlfopenc( fileName, sp5vf_binary, sp5vf_read, sp5bk_buffered, &file, &err );
	if ( err.sp5fe_result != vf_ok ) {
		/* Error: Can not open file */

		return sapdbwa_False;
	}

	/* Read and send file */
	do {
		sqlfreadc ( file, buffer, sizeof( buffer ), &readLen, &err );

		if ( readLen > 0 ) {
			if ( sapdbwa_SendBody ( rep, buffer, readLen ) < 1) {
				/* Error: Sending reply */
				sqlfclosec( file, sp5vf_close_normal, &err );
				return sapdbwa_False;
			}
		}
	} while ( readLen > 0 );
    
    /* Close file */
	sqlfclosec( file, sp5vf_close_normal, &err );

	return sapdbwa_True;

}

void wd22SetErr( sapdbwa_HttpReplyP rep, twd26ErrP err )
{
    rep->err = err;
} /* wd22SetErr */


void wd22FreeHeader( sapdbwa_HttpReplyP rep )
{
    wd28FreeAllEntries( rep->headers );
    if (rep->ContentType != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep->ContentType );
        rep->ContentType = NULL;
    }; /* if */
    if (rep->ContentLength != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep->ContentLength );
        rep->ContentLength = NULL;
    }; /* if */
    if (rep->LastModified != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep->LastModified );
        rep->LastModified = NULL;
    }; /* if */
    if (rep->Expires != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep->Expires );
        rep->Expires = NULL;
    }; /* if */
    if (rep->Location != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep->Location );
        rep->Location = NULL;
    }; /* if */    
} /* wd22FreeHeader */

/*!**********************************************************************

  EndChapter: sapdbwa_HttpReplyP (functions from hwd03waapi.h)

************************************************************************/

/*!**********************************************************************

 internal functions

************************************************************************/

void * wd22_ReplyConstructor()
{
    sapdbwa_Bool   allocatOK;
    sapdbwa_HttpReplyP newReply;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_http_rep ),
                   (sapdbwa_UInt1**) &newReply,
                   &allocatOK );
    if (allocatOK) {
        newReply->Location      = NULL;
        newReply->LastModified  = NULL;
        newReply->ContentType   = NULL;
        newReply->ContentLength = NULL;
        newReply->Expires       = NULL;
        newReply->headers = wd28CreateDictionary();
        if (newReply->headers == NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newReply );
            newReply = NULL;
        }; /* else */
    } else {
        /* memory allocation failure */
        newReply = NULL;
    }; /* else */

    return newReply;
} /* wd22_ReplyConstructor */


void wd22_ReplyDestructor( void * handle )
{
    sapdbwa_HttpReplyP  rep = (sapdbwa_HttpReplyP) handle;

    wd22FreeHeader( rep );
    wd28DestroyDictionary( rep->headers );
    /* rep->err is destroyed by caller */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) rep );
} /* wd22_ReplyDestructor */


void wd22_ReplyInit( sapdbwa_HttpReplyP       rep,
                     twd00ServerHandle   handle,
                     twd00SendHeaderFunc SendHeader,
                     twd00SendBodyFunc   SendBody )
{
    rep->handle        = handle;
    rep->SendHeader    = SendHeader;
    rep->SendBody      = SendBody;
    wd22FreeHeader( rep );
    rep->err           = NULL;
    rep->headersSent   = false;
} /* wd22_ReplyInit */


sapdbwa_Bool wd22_CreateString( char       **newString,
                              const char  *sourceString )
{
    sapdbwa_Bool allocatOK;
    sapdbwa_Int4 strLen = 0;
    
    if (sourceString == NULL) {
        *newString = NULL;
        return true;
    }; /* if */

    strLen = strlen(sourceString);
    if (strLen > 0) {
        sapdbwa_SQLALLOCAT( strLen+1,
                       (sapdbwa_UInt1**) newString,
                       &allocatOK );
        if (allocatOK) {
            strcpy( *newString, sourceString );
            return true;
        } else {
            return false;
        }; /* else */
    } else {
        *newString = NULL;
        return true;
    };
} /* wd22_CreateString */


