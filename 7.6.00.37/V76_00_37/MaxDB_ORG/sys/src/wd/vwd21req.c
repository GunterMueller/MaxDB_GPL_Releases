/*!**********************************************************************

  module: vwd21req.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd12hpool.h"
#include "hwd21req.h"

#include "SAPDBCommon/SAPDB_string.h"

void * wd21_RequestConstructor();

void wd21_RequestDestructor( void *requestHandle );

void wd21_RequestInit( sapdbwa_HttpRequestP     req,
                       twd00ServerHandle   serverHandle,
                       twd00ReadBodyFunc   readBody,
                       twd00GetHeaderFunc  getHeader,
                       twd00IsSecureFunc   isSecure );

sapdbwa_Bool wd21_InsertCookies( sapdbwa_HttpRequestP req );

sapdbwa_Bool wd21_InsertParams( sapdbwa_HttpRequestP req );

sapdbwa_Bool wd21_InsertValues( twd28DictionaryP dict, char *queryString );

sapdbwa_Bool wd21_InsertCookieValues( twd28DictionaryP  dict,
                                    const char       *queryString );

void wd21_PlusToSpace( char *str, sapdbwa_Int4 len );

sapdbwa_Bool wd21_ReadBodyData( sapdbwa_HttpRequestP req );

/*void wd21_UnescapeUrl( char *url, sapdbwa_Int4 len );*/

sapdbwa_Int4 wd21_Base64Decode( char       *bufplain,
                              const char *bufcoded);

/*!**********************************************************************

  Chapter: sapdbwa_HttpRequestP (functions from hwd03waapi.h)

  description:
  
************************************************************************/


static twd12HandlePoolP wd21requestPool = NULL;

sapdbwa_Bool wd21CreateRequestHandlePool()
{
    wd21requestPool = wd12CreateHandlePool( wd21_RequestConstructor,
                                            wd21_RequestDestructor );

    return (wd21requestPool != NULL);
} /* wd21CreateRequestHandlePool */


void wd21DestroyRequestHandlePool()
{
    wd12DestroyHandlePool( wd21requestPool );
} /* wd21DestroyRequestHandlePool */


twd21HttpRequestP wd21CreateRequest( twd00ServerHandle  serverHandle,
                                     twd00ReadBodyFunc  readBody,
                                     twd00GetHeaderFunc getHeader,
                                     twd00IsSecureFunc  isSecure )
{
    twd21HttpRequestP newRequest = NULL;

    newRequest = wd12GetHandle( wd21requestPool );
    if (newRequest != NULL) {
        wd21_RequestInit( newRequest,
                          serverHandle,
                          readBody,
                          getHeader,
                          isSecure );
    };
    
    return newRequest;
} /* wd21CreateRequest */


void wd21DestroyRequest( twd21HttpRequestP req )
{
    wd12PutHandle( wd21requestPool, req );
} /* wd21DestroyRequest */


sapdbwa_ErrP WINAPI sapdbwa_ReqGetErr( sapdbwa_HttpRequestP req )
{
    return req->err;
} /* sapdbwa_ReqGetErr */


sapdbwa_Bool WINAPI sapdbwa_GetAuthorization( sapdbwa_HttpRequestP   req,
                                       const char      **userName,
                                       const char      **password )
{
    const char *authHeader = NULL;
    sapdbwa_Bool  ok = true;
    sapdbwa_Bool  allocatOk = true;
    char       *colonPos = NULL;

    *userName = NULL;
    *password = NULL;
    authHeader = sapdbwa_GetHeader( req, "HTTP_AUTHORIZATION" );
    if (authHeader == NULL) {
        authHeader = sapdbwa_GetHeader( req, "HTTP_PROXY_AUTHORIZATION" );
    };
    if (authHeader != NULL) {
        if (req->authHeader != NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->authHeader );
        };
        sapdbwa_SQLALLOCAT( strlen(authHeader)+1,
                       (sapdbwa_UInt1**) &req->authHeader,
                       &allocatOk );
        if (allocatOk) {
            const char *startPos = authHeader;
            const char *base64Pos = NULL;
            /* skip leading spaces */
            while (*startPos != '\0' && *startPos == ' ')
                startPos++;
            ok = (startPos[0] == 'B'
                  && startPos[1] == 'a'
                  && startPos[2] == 's'
                  && startPos[3] == 'i'
                  && startPos[4] == 'c'
                  && startPos[5] == ' ');
            if (ok) {
                /* find space between Basic and base64-encoded string */
                base64Pos = startPos + 5;
                /* skip leading spaces */
                while (base64Pos != NULL && *base64Pos != '\0' && *base64Pos == ' ')
                    base64Pos++;
                if (base64Pos != NULL) {
                    wd21_Base64Decode( req->authHeader, base64Pos );
                    colonPos = strchr( req->authHeader, ':' );
                    if (colonPos != NULL) {
                        *colonPos = '\0';
                        *userName = req->authHeader;
                        *password = colonPos+1;
                    } else {
                        /* wrong header format */
                        ok = false;
                    };
                } else {
                    /* wrong header format */
                    ok = false;
                };
            };
        } else {
            /* memory alloc failure */
            ok = false;
        };
    } else {
        /* Authorization Header does not exist */
        ok = false;
    };

    return ok;
} /* sapdbwa_GetAuthorization */


sapdbwa_Bool WINAPI sapdbwa_GetCookieNames( sapdbwa_HttpRequestP req,
                                     sapdbwa_StringSeqP   names )
{
    sapdbwa_Bool  ok = true;

    if (!req->cookiesInserted) {
        wd21_InsertCookies( req );
        req->cookiesInserted = true;
    }; /* if */

    ok = wd28GetKeys( req->cookieValues, names );
    
    return ok;
} /* sapdbwa_GetCookieNames */


const char* WINAPI sapdbwa_GetCookieValue( sapdbwa_HttpRequestP req, const char *name )
{
    const char *value = NULL;
    
    if (!req->cookiesInserted) {
        wd21_InsertCookies( req );
        req->cookiesInserted = true;
    }; /* if */

    value = wd28GetValueByKey( req->cookieValues, name );

    return value;
} /* sapdbwa_GetCookieValue */


const char* WINAPI sapdbwa_GetHeader( sapdbwa_HttpRequestP req, const char *fieldName )
{
    return req->GetHeader( req->serverHandle, fieldName );
} /* sapdbwa_GetHeader */


const char* WINAPI sapdbwa_GetRequestURI( sapdbwa_HttpRequestP req )
{
    return req->Uri;
} /* sapdbwa_GetRequestURI */


const char* WINAPI sapdbwa_GetIfModifiedSince( sapdbwa_HttpRequestP req )
{
    return req->IfModifiedSince;
} /* sapdbwa_GetIfModifiedSince */


const char* WINAPI sapdbwa_GetQueryString( sapdbwa_HttpRequestP req )
{
    return req->queryString;
} /* sapdbwa_GetQueryString */


const char* WINAPI sapdbwa_GetPathInfo( sapdbwa_HttpRequestP req )
{
    return req->PathInfo;
} /* sapdbwa_GetPathInfo */


const char* WINAPI sapdbwa_GetMethod( sapdbwa_HttpRequestP req )
{
    return req->RequestMethod;
} /* sapdbwa_GetRequestMethod */


const char* WINAPI sapdbwa_GetContentType( sapdbwa_HttpRequestP req )
{
    return req->ContentType;
} /* sapdbwa_GetContentType */


const char* WINAPI sapdbwa_GetContentLength( sapdbwa_HttpRequestP req )
{
    return req->ContentLength;
} /* sapdbwa_GetContentLength */


sapdbwa_Bool WINAPI sapdbwa_GetContentChunked( sapdbwa_HttpRequestP req )
{
    return req->contentChunked;
} /* sapdbwa_GetContentLength */


const char* WINAPI sapdbwa_GetPathTranslated( sapdbwa_HttpRequestP req )
{
    return req->PhysicalPath;
} /* sapdbwa_GetPhysicalPat */


twd00ServerHandle wd21GetServerHandle( twd21HttpRequestP req )
{
    return req->serverHandle;
} /* wd21GetServerHandle */


const char* WINAPI sapdbwa_GetServerName( sapdbwa_HttpRequestP req )
{
    return req->ServerName;
} /* sapdbwa_GetServerName */


sapdbwa_Bool WINAPI sapdbwa_IsSecure( sapdbwa_HttpRequestP  req )
{
    return req->IsSecure( req->serverHandle );
} /* sapdbwa_IsSecure */

sapdbwa_ULong WINAPI sapdbwa_GetNumericalContentLength( sapdbwa_HttpRequestP req  )
{
    return req->numContentLength;
}

sapdbwa_ULong WINAPI sapdbwa_GetReadBodyBytes( sapdbwa_HttpRequestP req  )
{
    return req->readBodyBytes;
}

/*
    Utility Function to extract the value out of a name value pair in a buffer.
    'headerField' is assumed to be character sequence with length 'headerFieldLen'.
    If it countains the substring 'UpperCaseName', 'LowerCaseName', 
    or a mixed case variant of the two (which must be null terminated and have the same length), 
    the index '*valueStart' is set to the position after the last character of '~CaseName'.
    '*valueLen' is set to the maximum length of the following characters 
    being a boundary in the sense of RFC 1521 (which is sufficient for most other kinds 
    of header values as well).
    (The optional enclosing double quotes are trimmed (i.e. not counted).
    The function returns true, if a value was found, false otherwise.
*/
sapdbwa_Bool WINAPI sapdbwa_ExtractValue(const char    * headerField,
                                                sapdbwa_ULong   headerFieldLen,
                                                const char    * upperCaseName,
                                                const char    * lowerCaseName,
                                                sapdbwa_ULong * valueStart, 
                                                sapdbwa_ULong * valueLen)
{
    sapdbwa_ULong   nameLen = strlen(upperCaseName);
    sapdbwa_ULong   headerIdx = 0;
    sapdbwa_ULong   nameIdx = 0;
    char            curr = headerField[0];
    sapdbwa_Bool    retval = false;

    *valueStart = 0;
    *valueLen = 0;
    while(headerIdx < headerFieldLen) {
        if(nameIdx == nameLen) {
            /* boundary found */
            if(retval == false) {
                *valueStart = headerIdx;
                retval = true;
            }
            else if(curr == '"' || curr == ';' || curr == '\n' || curr == '\r') {
                break;
            }
            (*valueLen)++;
        }
        else if(curr == upperCaseName[nameIdx] || curr == lowerCaseName[nameIdx]) {
            nameIdx++;
        }
        else {
            nameIdx = 0;
        }
        curr = headerField[++headerIdx];
    }
    /* trim a heading doublequote :*/
    if(headerField[*valueStart] == '"') {
        (*valueStart)++;
        (*valueLen)--;
    }
    /* trim trailing whitespace :*/
    for(; 
        headerField[(*valueStart)+(*valueLen)] == '\x20' || headerField[(*valueStart)+(*valueLen)] == '\t'; 
        (*valueLen)--) 
    {/* no body */}

    return retval;
}

sapdbwa_ULong WINAPI sapdbwa_ReadBody( sapdbwa_HttpRequestP    req,
                                       char                  * buff,
                                       sapdbwa_ULong           len )
{
    sapdbwa_ULong retval = 0;
    if (strcmp( sapdbwa_GetMethod( req ), "POST" ) == 0
        && (req->bodyData == NULL)) {
        if (!wd21_ReadBodyData( req )) {
            /* this may cause an infinite loop in the calling function,
               better : make this function return a boolean value and
               return the number of read bytes in a parameter. */ 
            return retval;
        }; /* if */
    }; /* if */

    if (req->mimeBodyData != NULL && req->mimeBodyDataLen > req->mimeBodyDataPos) {
        /* read previously read data from the request body data buffer for mime
           multipart processing 'mimeBodyData'. This data may have been read by
           a call to mime multipart processing functions. */
        retval = min( req->mimeBodyDataLen - req->mimeBodyDataPos,
                                    len );
        SAPDB_memmove( buff, req->mimeBodyData + req->mimeBodyDataPos, retval );
        req->mimeBodyDataPos += retval;
        return retval;
    } 
    else if (req->bodyData != NULL && req->bodyDataLen > req->bodyDataPos) {
        /* read the previously read first chunk of data from buffer 'bodyData'.
            (this may have been read during a call to 'GetParamaterNames/Values'
            during parameter processing) */
        retval = min( req->bodyDataLen - req->bodyDataPos,
                                    len );
        SAPDB_memmove( buff, req->bodyData + req->bodyDataPos, retval );
        req->bodyDataPos += retval;
        return retval;
    } 
    else {
        /* if there is no previously read data in any buffers,
           and there is still data to read,
           read from the socket directly */
        if(req->readBodyBytes < req->numContentLength || req->contentChunked) {
            retval = req->ReadBody( req->serverHandle, buff, len );
        }
        /* don't forget to update the number of bytes read from the socket : */
        req->readBodyBytes += retval;
        return retval;
    }; /* else */
} /* sapdbwa_ReadBody */

union wd21_un_iterand_env {
    struct wd21_st_NextPart_env {
        const char           * boundary;
        sapdbwa_Bool         * moreParts;
        sapdbwa_UInt2          boundary_idx;
        sapdbwa_UInt2          dashCount;
        sapdbwa_UInt2          lineDelimCount;
        sapdbwa_Bool           afterBoundary;
        sapdbwa_ULong          boundaryLen;
   } np;

    struct wd21_st_PartHeaderField_env {
        char                 * buff;
        sapdbwa_ULong          len;
        sapdbwa_ULong        * readBytes;
        sapdbwa_Bool         * moreData;
        sapdbwa_Bool         * moreHeaderFields;
        sapdbwa_UInt2          lineDelimCount;
    } phf;

    struct wd21_st_PartBody_env {
        char                 * buff;
        sapdbwa_ULong          len;
        sapdbwa_ULong        * readBytes;
        char                 * delimiter;
        sapdbwa_Bool         * moreParts;
        sapdbwa_Bool         * moreData;
        sapdbwa_UInt2          dashCount;
        sapdbwa_Bool           afterDelimiter;
        sapdbwa_ULong          delimiterLen;
        sapdbwa_UInt2          lineDelimCount;
    } pb;
};

typedef union wd21_un_iterand_env wd21MimeBodyIterandEnv;

typedef sapdbwa_Bool (*wd21MimeBodyIterand) (sapdbwa_HttpRequestP     req,
                                             wd21MimeBodyIterandEnv * env,
                                             sapdbwa_Bool           * proceed,
                                             sapdbwa_Bool           * needMore);

sapdbwa_Bool wd21MimeBodyIterator(sapdbwa_HttpRequestP     req,
                                  wd21MimeBodyIterandEnv * env,
                                  wd21MimeBodyIterand      iterand)
{
    sapdbwa_Bool    retval  = true;
    sapdbwa_Bool    allocOk = true;
    sapdbwa_ULong   READBUFSIZE = 8192;
    sapdbwa_ULong   toCopy = 0;

    sapdbwa_Bool    proceed = false;
    sapdbwa_Bool    needMore = true;
    sapdbwa_Bool    iterandBreak = false;


    if(req->mimeBodyData == NULL) {
        sapdbwa_SQLALLOCAT(READBUFSIZE,
                           (sapdbwa_UInt1**)&req->mimeBodyData,
                           &allocOk);
        req->mimeBodyDataLen = 0;
        req->mimeBodyDataPos = 0;
        req->mimeBodyDataBufSize = READBUFSIZE;
    } /* if(req->bodyData == NULL) ... */
    if(allocOk == true) {
        do {
            if(req->mimeBodyDataPos == req->mimeBodyDataLen) {
                req->mimeBodyDataPos = 0;
                if(req->bodyData != NULL && req->bodyDataLen > req->bodyDataPos) {
                    /* there is still data to read in req->bodyData : 
                       copy this into the req->mimeBodyData buffer */
                    toCopy = min(req->bodyDataLen - req->bodyDataPos, req->mimeBodyDataBufSize);
                    SAPDB_memcpy(req->mimeBodyData, req->bodyData + req->bodyDataPos, toCopy);/* memcpy SAFE, NO OVERLAP HERE. */
                    req->mimeBodyDataLen = toCopy;
                    req->bodyDataPos += toCopy;
                } /* if(req->bodyDataLen > req->bodyDataPos) ... */
                else {
                    if(req->readBodyBytes < req->numContentLength) {
                        req->mimeBodyDataLen = req->ReadBody(req->serverHandle, req->mimeBodyData, req->mimeBodyDataBufSize);
                    }
                    else {
                        req->mimeBodyDataLen = 0;
                    }
                    req->readBodyBytes += req->mimeBodyDataLen;
                } /* if(req->bodyDataLen > req->bodyDataPos) ... else ... */
            } /* if(req->mimeBodyDataPos == req->mimeBodyDataLen) ... */

            retval = iterand(req, env, &proceed, &needMore);

        } while(needMore == true &&
                retval == true && 
                req->mimeBodyDataLen > 0 && 
                req->mimeBodyDataPos == req->mimeBodyDataLen);
        /* if we had a 'break' in the iterand
           increment body buffer position dependent on proceed: */ 
        iterandBreak = (req->mimeBodyDataPos < req->mimeBodyDataLen) ? true : false;
        if(iterandBreak && proceed == true) req->mimeBodyDataPos++;
        /* 'needMore == true' means that we did not find the 
           delimiting sequence of the iterand, which is fatal : */
        if(needMore == true &&
           iterandBreak != true &&
           (req->mimeBodyDataLen == 0 || 
            req->mimeBodyDataPos != req->mimeBodyDataLen)) {
                retval = false;
            }
    } /* if(allocOk == true) ... */
    else {
        retval = false;
    } /* if(allocOk == true) ... else ... */
    return retval;
} /* wd21MimeBodyIterator */

sapdbwa_Bool wd21MimeMultipartBody_NextPart_iterand(sapdbwa_HttpRequestP     req,
                                                    wd21MimeBodyIterandEnv * env,
                                                    sapdbwa_Bool           * proceed,
                                                    sapdbwa_Bool           * needMore)
{
    sapdbwa_Bool    retval  = true;
    char            curr = 0;

    *proceed = false;
    *needMore = true;
    for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) {
        curr = req->mimeBodyData[req->mimeBodyDataPos];
        if(env->np.afterBoundary == true) {
            if(curr == '-'&& env->np.dashCount < 2 && env->np.lineDelimCount == 0)         
                env->np.dashCount++;
            else if(curr == '\r' && env->np.lineDelimCount == 0 && (env->np.dashCount == 0 || env->np.dashCount == 2)) 
                env->np.lineDelimCount++;
            else if(curr == '\n' && env->np.lineDelimCount == 1 && (env->np.dashCount == 0 || env->np.dashCount == 2)) 
                env->np.lineDelimCount++;
            else { /* we have a mime syntax error, *proceed is false. */
                retval = false;
                *needMore = false;
                break; 
            }
            if(env->np.lineDelimCount == 2) { 
                if(env->np.dashCount == 0) *env->np.moreParts = true; /* found [delimiter] . */
                else if(env->np.dashCount == 2) *env->np.moreParts = false; /* found [close-delimiter] . */
                *proceed = true;
                *needMore = false;
                break;
            } /* if(env->np.lineDelimCount == 2) ... */
        } /* if(env->np.afterBoundary == true) ... */
        else {
            if(env->np.dashCount < 2) {
                if(curr == '-') env->np.dashCount++;
                else            env->np.dashCount = 0;
            } /* if(env->np.dashcount < 2) ... */
            else {
                if(curr == env->np.boundary[env->np.boundary_idx]) {
                    env->np.boundary_idx++;
                    if(env->np.boundary_idx == env->np.boundaryLen) {
                        env->np.afterBoundary = true;
                        env->np.dashCount = 0;
                    } /* if(env->np.boundary_idx == env->np.boundaryLen) ... */
                } /* if(curr == env->np.boundary[env->np.boundary_idx]) ... */
                else {
                    env->np.boundary_idx = 0;
                    env->np.dashCount = 0;
                } /* if(curr == env->np.boundary[env->np.boundary_idx]) ... else ... */
            } /* if(env->np.dashcount < 2) ... else ... */
        } /* if(env->np.afterBoundary == true) ... else ... */
    } /* for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) ... */

    return retval;
} /* wd21MimeMultipartBody_NextPart_iterand */

/* 
   (Terms in square brackets denote productions from the BNF for mime-multipart messages
   in RFC 1521.)
   The input parameter 'boundary' is a null-terminated ascii string and contains the [boundary].
   This function reads up to (and including) next [delimiter] or [close-delimiter].
   In the latter case the output parameter '*moreParts' is set to false (is set to true otherwise).

   The return value of this function is true, or false if an error occurred.

   Note that this function is mainly useful to set the read position initially after 
   the first [delimiter]/[close-delimiter] (i.e. skipping the optional [preamble])
   or to skip the remainder of a partially read mime part.
   The function 'sapdbwa_MimeMultipartBody_PartBody' (see below) also sets the read position 
   after the [delimiter]/[close-delimiter].
 */
sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_NextPart(sapdbwa_HttpRequestP   req,
                                                       const char           * boundary,
                                                       sapdbwa_Bool         * moreParts)
{
   wd21MimeBodyIterandEnv env;

    env.np.boundary = boundary;
    env.np.moreParts = moreParts;

    env.np.boundary_idx = 0;
    env.np.dashCount = 0;
    env.np.lineDelimCount = 0;
    env.np.afterBoundary = false;
    env.np.boundaryLen = strlen(boundary);

    *moreParts = false;

    /* first, discard what we have already read but held back :*/
    req->mimeDelimiterToFlush = 0;

    return wd21MimeBodyIterator(req, 
                                &env,
                                wd21MimeMultipartBody_NextPart_iterand);
} /* sapdbwa_MimeMultipartBody_NextPart */

sapdbwa_Bool wd21MimeMultipartBody_PartHeaderField_iterand(sapdbwa_HttpRequestP     req,
                                                           wd21MimeBodyIterandEnv * env,
                                                           sapdbwa_Bool           * proceed,
                                                           sapdbwa_Bool           * needMore)
{
    sapdbwa_Bool    retval = true;
    char            curr = 0;

    *proceed = true;
    *needMore = true;
    for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) {
        curr = req->mimeBodyData[req->mimeBodyDataPos];
        if(curr == '\r' && (env->phf.lineDelimCount == 0 || env->phf.lineDelimCount == 2)) {
            env->phf.lineDelimCount++;
        }/* if(curr == '\r' &&  ... ) ... */
        else if(curr == '\n' && (env->phf.lineDelimCount == 1 || env->phf.lineDelimCount == 3)) {
            env->phf.lineDelimCount++;
        } /* else if(curr == '\n' &&  ... ) ... */
        else if(env->phf.lineDelimCount == 4) {
            *env->phf.moreHeaderFields = false;
            *proceed = false;
            *needMore = false;
            break;
        } /* else if(env->phf.lineDelimCount == 4) ... */
        else if(env->phf.lineDelimCount == 2) {
            if(curr != '\x20' && curr != '\t') {
                /* line is not folded. */
                *env->phf.moreHeaderFields = true;
                *proceed = false;
                *needMore = false;
                break;
            } /* if(curr != '\x20' ...) ... */
            else {
                env->phf.lineDelimCount = 0;
            }
        } /* else if(env->phf.lineDelimCount == 2 ... ) ... */
        else {
            if(env->phf.lineDelimCount == 1) {
                /* first : write the remaining '\r', process 'curr' 
                   in next call, if outputbuffer is full */
                env->phf.buff[(*env->phf.readBytes)++] = '\r';
                if(*env->phf.readBytes == env->phf.len) {
                    *env->phf.moreData = true;
                    *needMore = false;
                    *proceed = false;
                    break;
                } /* if(*env->phf.readBytes == env->phf.len) ... */
            }/* if(env->phf.lineDelimCount == 1) ... */
            env->phf.lineDelimCount = 0;
            env->phf.buff[(*env->phf.readBytes)++] = curr;
            if(*env->phf.readBytes == env->phf.len) {
                *env->phf.moreData = true; 
                break;
            }
        } /* else if(env->phf.lineDelimCount == 2 ... ) ... else ... */
    } /* for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) ... */
    return retval;
} /* wd21MimeMultipartBody_PartHeaderField_iterand */

/*
   This function treats the next bytes from the request body as a (logical) line 
   from the header of the current mime part.
   Continuation lines (i.e. beginning with whitespace) are included without unfolding 
   them (see RFC 822, "folding/unfolding").
   It reads up to (and including) the terminal CRLF resp. CRLFCRLF (see RFC 1521) 
   and returns the data in 'buff', which must be provided with (at least) size 'len'.
   The number of bytes actually returned in 'buff' is returned in '*readBytes'.
   If the output parameter '*moreData' is set to true, more data of this header line 
   is available in a subsequent call to this function.
   The output buffer 'buff' does not include these CRLF or CRLFCRLF , nor is it null terminated.
   If the line was terminated with CRLFCRLF, the output parameter '*moreHeaderFields' 
   is set to false, otherwise it is true.

   The return value of this function is true, or false if an error occurred.
 */
sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_PartHeaderField(sapdbwa_HttpRequestP   req,
                                                              char                 * buff,
                                                              sapdbwa_ULong          len,
                                                              sapdbwa_ULong        * readBytes,
                                                              sapdbwa_Bool         * moreData,
                                                              sapdbwa_Bool         * moreHeaderFields)
{
   wd21MimeBodyIterandEnv env;

    env.phf.buff = buff;
    env.phf.len = len;
    env.phf.readBytes = readBytes;
    env.phf.moreData = moreData;
    env.phf.moreHeaderFields = moreHeaderFields;

    env.phf.lineDelimCount = 0;

    *readBytes = 0;
    *moreData = false;
    *moreHeaderFields = false;


    return wd21MimeBodyIterator(req, 
                                &env, 
                                wd21MimeMultipartBody_PartHeaderField_iterand);
} /* sapdbwa_MimeMultipartBody_PartHeaderField */

sapdbwa_Bool wd21MimeMultipartBody_PartBody_iterand(sapdbwa_HttpRequestP     req,
                                                    wd21MimeBodyIterandEnv * env,
                                                    sapdbwa_Bool           * proceed,
                                                    sapdbwa_Bool           * needMore)
{
    sapdbwa_Bool    retval  = true;
    char            curr = 0;
    sapdbwa_Bool    forBreak = false;
    

    *proceed = false;
    *needMore = true;
    for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) {
        curr = req->mimeBodyData[req->mimeBodyDataPos];
        if(env->pb.afterDelimiter == true) {
            if(curr == '-') {         
                env->pb.dashCount++;
                if(env->pb.dashCount == 2) { 
                    *env->pb.moreParts = false; /* found [close-delimiter] . */
                    *proceed = true;
                    *needMore = false;
                    break; 
                }
            }
            else if(curr != '-'&& env->pb.dashCount == 0) {
                if(env->pb.lineDelimCount == 0) {
                    if(curr == '\r') {
                        env->pb.lineDelimCount++;
                    }
                    else {
                        /* anything except '-' and CR following a [delimiter]
                           is a mime syntax error. */
                        retval = false;
                        *needMore = false;
                        break; 
                    }
                }
                else if(env->pb.lineDelimCount == 1) {
                    if(curr == '\n') {
                        env->pb.lineDelimCount++;
                    }
                    else {
                        /* anything except LF following a [delimiter] CR
                           is a mime syntax error. */
                        retval = false;
                        *needMore = false;
                        break; 
                    }
                }

                if(env->pb.lineDelimCount == 2) {
                    *env->pb.moreParts = true; /* found [delimiter] CRLF . */
                    *proceed = true;
                    *needMore = false;
                    break; 
                }
            }
            else if(curr != '-'&& env->pb.dashCount == 1) {
                /* a [delimiter] followed by a single dash is a mime syntax error. */
                retval = false;
                *needMore = false;
                break; 
            }
        } /* if(env->pb.afterDelimiter == true) ... */
        else {
            if(curr == env->pb.delimiter[req->mimeDelimiterToFlushOffset]) {
                req->mimeDelimiterToFlushOffset++;
                if(req->mimeDelimiterToFlushOffset == env->pb.delimiterLen) {
                    env->pb.afterDelimiter = true;
                    env->pb.dashCount = 0;
                } /* if(req->mimeDelimiterToFlushOffset == env->pb.boundaryLen) ... */
            } /* if(curr == env->pb.delimiter[req->mimeDelimiterToFlushOffset]) ... */
            else {
                /* this can't be the delimiter we look for,
                    write the boundary we have read so far and held back : */
                req->mimeDelimiterToFlush = req->mimeDelimiterToFlushOffset;
                while(req->mimeDelimiterToFlush > 0) {
                    env->pb.buff[(*env->pb.readBytes)++] = env->pb.delimiter[req->mimeDelimiterToFlushOffset - req->mimeDelimiterToFlush];
                    req->mimeDelimiterToFlush--;
                    if(*env->pb.readBytes == env->pb.len) {
                        forBreak = true;
                        break;
                    } /* if(*env.pb.readBytes == env.pb.len) ... */
                }/* for(req->mimeDelimiterToFlush =   ... ) ...*/
                if(forBreak == true) { 
                    *(env->pb.moreData) = true;
                    break;
                }
                req->mimeDelimiterToFlushOffset = 0;
                env->pb.dashCount = 0;
                /* now we have either the start of the delimiter or a character to flush : */
                if(curr == env->pb.delimiter[req->mimeDelimiterToFlushOffset]) {
                    req->mimeDelimiterToFlushOffset++;
                    if(req->mimeDelimiterToFlushOffset == env->pb.delimiterLen) {
                        env->pb.afterDelimiter = true;
                        env->pb.dashCount = 0;
                    } /* if(req->mimeDelimiterToFlushOffset == env->pb.boundaryLen) ... */
                } /* if(curr == env->pb.delimiter[req->mimeDelimiterToFlushOffset]) ... */
                else {
                    env->pb.buff[(*env->pb.readBytes)++] = curr;
                    if(*env->pb.readBytes == env->pb.len) {
                        *(env->pb.moreData) = true;
                        *proceed = true;
                        break;
                    } /* if(*env->pb.readBytes == len) ... */
                }
            } /* if(curr == env->pb.delimiter[req->mimeDelimiterToFlushOffset]) ... else ... */
        } /* if(env->pb.afterDelimiter == true) ... else ... */
    } /* for(; req->mimeBodyDataPos < req->mimeBodyDataLen; req->mimeBodyDataPos++) ... */
    return retval;
} /* wd21MimeMultipartBody_PartBody_iterand */

/* 
   (Terms in square brackets denote productions from the BNF for mime-multipart messages
   in RFC 1521.)
   Input parameter 'boundary' is a null-terminated ascii string and contains the [boundary].
   This function reads up to (and including) the next [delimiter] or [close-delimiter]
   and returns the data in 'buff', which must be provided with (at least) size 'len'.
   The number of bytes actually returned in 'buff' is returned in '*readBytes'.
   If the output parameter '*moreData' is set to true, more data of this mime part 
   is available in a subsequent call to this function.
   The output buffer 'buff' does not include the [delimiter] or [close-delimiter] , 
   nor is it null terminated.
   If a [close-delimiter] was found, the output parameter '*moreParts' is set to false,
   in case of a [delimiter] it is set to true.
   The function 'sapdbwa_ReadBody' could subsequently be used to read the optional [epilogue].

   The return value of this function is true, or false if an error occurred.
 */
sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_PartBody(sapdbwa_HttpRequestP    req,
                                                       const char            * boundary,
                                                       char                  * buff,
                                                       sapdbwa_ULong           len,
                                                       sapdbwa_ULong         * readBytes,
                                                       sapdbwa_Bool          * moreData,
                                                       sapdbwa_Bool          * moreParts)
{
    wd21MimeBodyIterandEnv env;
    char delim[100];
    sapdbwa_Bool forBreak = false;



    env.pb.delimiterLen = strlen(boundary)+4;
    if(env.pb.delimiterLen > 100) return false;

    env.pb.buff = buff;
    env.pb.len = len;
    env.pb.readBytes = readBytes;
    delim[0] = '\r';
    delim[1] = '\n';
    delim[2] = '-';
    delim[3] = '-';
    SAPDB_memcpy(delim+4, boundary, env.pb.delimiterLen-4);/* memcpy SAFE, NO OVERLAP HERE. */
    env.pb.delimiter = delim;
    env.pb.moreParts = moreParts;
    env.pb.moreData = moreData;

    env.pb.dashCount = 0;
    env.pb.afterDelimiter = false;
    env.pb.lineDelimCount = 0;
    

    *readBytes = 0;
    *moreData = false;
    *moreParts = false;


    /* first, flush what we have already read but held back :*/

    while(req->mimeDelimiterToFlush > 0) {
        env.pb.buff[(*env.pb.readBytes)++] = env.pb.delimiter[req->mimeDelimiterToFlushOffset - req->mimeDelimiterToFlush];
        req->mimeDelimiterToFlush--;
        if(*env.pb.readBytes == env.pb.len) {
            forBreak = true;
            break;
        } /* if(*env.pb.readBytes == env.pb.len) ... */
    }
    if(forBreak == true) {
        *(env.pb.moreData) = true;
        return true;
    }
    req->mimeDelimiterToFlushOffset = 0;
    req->mimeDelimiterToFlush = 0;

    return wd21MimeBodyIterator(req, 
                                &env, 
                                wd21MimeMultipartBody_PartBody_iterand);
} /* sapdbwa_MimeMultipartBody_PartBody */

sapdbwa_Bool WINAPI sapdbwa_GetParameterNames( sapdbwa_HttpRequestP  req,
                                        sapdbwa_StringSeqP    names )
{
    sapdbwa_Bool ok = true;

    if (!req->paramsInserted) {
        wd21_InsertParams( req );
        req->paramsInserted = true;
    };
    wd11FreeElems( names );
    ok = wd28GetKeys( req->paramValues, names );

    return ok;
} /* sapdbwa_GetParameterNames */


sapdbwa_Bool WINAPI sapdbwa_GetParameterValues( sapdbwa_HttpRequestP  req,
                                         const char      *parameterName,
                                         sapdbwa_StringSeqP    values )
{
    sapdbwa_Bool ok = true;
    
    if (!req->paramsInserted) {
        wd21_InsertParams( req );
        req->paramsInserted = true;
    };
    wd11FreeElems( values );
    ok = wd28GetValuesByKey( req->paramValues, parameterName, values );

    return ok;
} /* sapdbwa_GetParameterValues */


void wd21SetErr( twd21HttpRequestP  req, twd26ErrP err )
{
    req->err = err;
} /* wd21SetErr */


void wd21SetUri( twd21HttpRequestP  req,
                 const char        *uri )
{
    req->Uri             = uri;
} /* wd21SetUri */


void wd21SetIfModifiedSince( twd21HttpRequestP  req,
                             const char        *ifModifiedSince )
{
    req->IfModifiedSince = ifModifiedSince;
} /* wd21SetIfModifiedSince */


void wd21SetQueryString( twd21HttpRequestP  req,
                         const char        *queryString )
{
    sapdbwa_Int4  queryStringLen = 0;
    sapdbwa_Bool  allocatOK = true;

    if (queryString != NULL && strlen(queryString) != 0) {
        if (req->queryString != NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->queryString );
            req->queryString = NULL;
        }; /* if */
        /* ouer parameters are not valid anymore */
        if (req->paramValues != NULL) {
            wd28FreeAllEntries( req->paramValues );
            req->paramsInserted = false;
        }; /* if */
    
        queryStringLen = strlen( queryString );
        sapdbwa_SQLALLOCAT( queryStringLen+1,
                       (sapdbwa_UInt1**) &req->queryString,
                       &allocatOK );
        if (allocatOK) {
            sapdbwa_SQLALLOCAT( queryStringLen+1,
                           (sapdbwa_UInt1**) &req->paramQueryString,
                           &allocatOK );
            if (allocatOK) {
                strcpy( req->queryString, queryString );
				/* PTS1117669: Unescaping the querystring right here leads to problems
				   during separation of the querystring into its parameters.
				   It is better to unescape the separated parameter key and value when
				   separation is done.
				   For instance if a '&' is part of the value and it was sent escaped as
				   %26 then unescaping it right now would introduce a new parameter separator
				   into the querystring. The parameter value containg this %26 will be cut.
				   Information is lost */
                /* wd21_PlusToSpace( req->queryString, queryStringLen );*/
                /* wd21_UnescapeUrl( req->queryString, queryStringLen );*/
                strcpy( req->paramQueryString, req->queryString );
            } else {
                req->queryString = NULL;
            }; /* else */
        } else {
            req->queryString = NULL;
        }; /* else */
    }; /* if */
} /* wd21SetQueryString */


void wd21SetPathInfo( twd21HttpRequestP  req,
                      const char        *pathInfo )
{
    req->PathInfo        = pathInfo;
} /* wd21SetPathInfo */

void wd21SetRequestMethod( twd21HttpRequestP  req,
                           const char        *requestMethod )
{
    req->RequestMethod   = requestMethod;
} /* wd21SetRequestMethod */

/*========================================================================*/

void wd21SetContentChunked( twd21HttpRequestP   req,
                            sapdbwa_Bool        contentChunked )
{

    req->contentChunked = contentChunked;

}

/*========================================================================*/

void wd21SetContentType( twd21HttpRequestP  req,
                         const char        *contentType )
{
    req->ContentType     = contentType;
} /* wd21SetContentType */

void wd21SetContentLength( twd21HttpRequestP  req,
                           const char        *contentLength )
{
    req->ContentLength   = contentLength;
    if(contentLength != NULL) 
        req->numContentLength = atoi( sapdbwa_GetContentLength( req ));
} /* wd21SetContentLength */

void wd21SetPhysicalPath( twd21HttpRequestP  req,
                          const char        *physicalPath )
{
    req->PhysicalPath    = physicalPath;
} /* wd21SetPhysicalPath */

void wd21SetServerName( twd21HttpRequestP  req,
                        const char        *serverName )
{
    req->ServerName      = serverName;
} /* wd21SetServerName */

/*!**********************************************************************

  EndChapter: sapdbwa_HttpRequestP (functions from hwd03waapi.h)

************************************************************************/

void * wd21_RequestConstructor()
{
    twd21HttpRequestP newRequest = NULL;
    sapdbwa_Bool        allocatOK;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_http_req ),
                   (sapdbwa_UInt1**) &newRequest,
                   &allocatOK );
    if (allocatOK) {
        /* these are allocated later if necessary */
        newRequest->authHeader        = NULL;
        newRequest->cookieValues      = NULL;
        newRequest->paramValues       = NULL;
        newRequest->bodyData          = NULL;


        newRequest->mimeBodyData      = NULL;


        newRequest->paramBodyData     = NULL;
        newRequest->queryString       = NULL;
        newRequest->paramQueryString  = NULL;
        newRequest->cookieValues = wd28CreateDictionary();
        newRequest->paramValues = wd28CreateDictionary();
    } else {
        newRequest = NULL;
    };
    
    return newRequest;
} /* wd21_RequestConstructor */


void wd21_RequestDestructor( void *handle )
{
    sapdbwa_HttpRequestP req = (sapdbwa_HttpRequestP) handle;
    
    if (req->paramValues != NULL) {
        wd28DestroyDictionary( req->paramValues );
    }; /* if */
    /* req->err is destroyed by caller! */
    if (req->authHeader != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->authHeader );
    };
    if (req->cookieValues != NULL) {
        wd28DestroyDictionary( req->cookieValues );
    }; /* if */
    if (req->bodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->bodyData );
    }; /* if */
    if (req->paramBodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->paramBodyData );
    }; /* if */
    if (req->queryString != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->queryString );
    }; /* if */
    if (req->paramQueryString != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->paramQueryString );
    }; /* if */



    if (req->mimeBodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->mimeBodyData );
    }


    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) req );
} /* wd21_RequestDestructor */


void wd21_RequestInit( sapdbwa_HttpRequestP    req,
                       twd00ServerHandle  serverHandle,
                       twd00ReadBodyFunc  readBody,
                       twd00GetHeaderFunc getHeader,
                       twd00IsSecureFunc  isSecure )
{
    if (req->authHeader != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->authHeader );
        req->authHeader = NULL;
    };
    req->serverHandle    = serverHandle;
    req->ReadBody        = readBody;   /* the webserver API function which reads from the socket */
    req->GetHeader       = getHeader;
    req->IsSecure        = isSecure;
    req->Uri             = NULL;
    req->IfModifiedSince = NULL;
    req->PathInfo        = NULL;
    req->RequestMethod   = NULL;
    req->ContentType     = NULL;
    req->ContentLength   = NULL;
    req->contentChunked  = sapdbwa_False;
    req->PhysicalPath    = NULL;
    req->ServerName      = NULL;
    if (req->bodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->bodyData );
        req->bodyData        = NULL;
    }; /* if */
    if (req->paramBodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->paramBodyData );
        req->paramBodyData   = NULL;
    }; /* if */
    if (req->queryString != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->queryString );
        req->queryString     = NULL;
    }; /* if */
    if (req->paramQueryString != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->paramQueryString );
        req->paramQueryString = NULL;
    }; /* if */
    req->bodyDataLen     = 0;
    req->bodyDataPos     = 0;



    req->readBodyBytes       = 0;
    req->numContentLength    = 0;


    if (req->mimeBodyData != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) req->mimeBodyData );
        req->mimeBodyData        = NULL;
    }
    req->mimeBodyDataBufSize = 0; 
    req->mimeBodyDataLen     = 0;
    req->mimeBodyDataPos     = 0;
    req->mimeDelimiterToFlush       = 0;
    req->mimeDelimiterToFlushOffset = 0;
    

    
    req->err             = NULL;
    wd28FreeAllEntries( req->cookieValues );
    req->cookiesInserted = false;
    wd28FreeAllEntries( req->paramValues );
    req->paramsInserted  = false;
} /* wd21_RequestInit */


sapdbwa_Bool wd21_InsertCookies( sapdbwa_HttpRequestP req )
{
    sapdbwa_Bool  insertOK = true;    
    const char *cookieStr = NULL;

    cookieStr = sapdbwa_GetHeader( req, "HTTP_COOKIE" );
    if (cookieStr != NULL) {
        wd21_InsertCookieValues( req->cookieValues, cookieStr );
    }; /* if */

    return insertOK;
} /* wd21_InsertCookies */    


sapdbwa_Bool wd21_InsertParams( sapdbwa_HttpRequestP req )
{
    sapdbwa_Bool  insertOK = true;
    
    insertOK = wd21_InsertValues( req->paramValues,
                                  req->paramQueryString );

    if (insertOK) {
        if (strcmp( sapdbwa_GetMethod( req ), "POST" ) == 0
            && (req->bodyData == NULL)) {
            insertOK = wd21_ReadBodyData( req );
            if (insertOK) {
                insertOK = wd21_InsertValues( req->paramValues, req->paramBodyData );
            }; /* if */
        }; /* if */
    }; /* if */
        

    return insertOK;
} /* wd21_InsertParams */

void wd21_SkipSpaces( char **pos )
{
	char *tpos = *pos;

    while (*tpos == ' ')
        tpos++;

	*pos = tpos;
} /* wd21_SkipSpaces */


sapdbwa_Bool wd21_InsertCookieValues( twd28DictionaryP  dict,
                                    const char       *cookieString )
{
    sapdbwa_Bool  insertOK = true;
    char       *pos      = NULL;
    const char *name     = NULL;
    const char *value    = NULL;
    char       *tmpCookie = NULL;
    sapdbwa_Bool  allocatOK = true;
    
    sapdbwa_SQLALLOCAT( strlen(cookieString)+1,
                   (sapdbwa_UInt1**) &tmpCookie,
                   &allocatOK );
    if (allocatOK) {
        strcpy( tmpCookie, cookieString );
        pos = tmpCookie;
        while (insertOK && (pos != NULL) && (*pos != '\0')) {
            name = pos;
            pos = strchr( pos, '=' );
            if (pos != NULL) {
                *pos = '\0'; /* '=' -> '\0' */
                pos++;
                value = pos;
                pos = strchr( pos, ';' );
                if (pos != NULL) {
                    *pos = '\0'; /* ';' -> '\0' */
                    pos++;
                    wd21_SkipSpaces(&pos);
                }; /* if */
                insertOK = wd28AddCopyEntry( dict, name, value );
            }; /* if */
        }; /* while */
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) tmpCookie );
    } else {
        /* memory allocation error */
        insertOK = false;
    }; /* else */
    
    return insertOK;
} /* wd21_InsertCookieValues */
    

sapdbwa_Bool wd21_InsertValues( twd28DictionaryP dict, char *queryString )
{
    sapdbwa_Bool	 insertOK = true;
    char			*pos      = queryString;
    char			*name     = NULL;
    char			*value    = NULL;
	sapdbwa_UInt4	 strStack = 0;
    
	/* Scan query string */
    while ((pos != NULL) && (*pos != '\0')) {
        name = pos;

		/* Search value */
        pos = strchr( pos, '=' );
        if (pos != NULL) {
			/* Terminate name */
            *pos = '\0'; /* '=' -> '\0' */
            pos++;

			/* Is value a string? */
			if ( *pos == '"' ) {
				value = pos;

				strStack=1;
				pos++;

				if ( *pos != '\0' ) {

					do {

						pos = strchr( pos, '"');
						if ( pos != NULL ) {
							if ( *(pos-1) == '=' )
								strStack++;
							else
								strStack--;
		
							if ( strStack > 0 )
								pos++;

						}

					} while ( strStack && pos!=NULL && *pos != '\0' );

					/* Terminate value */
					if (pos!=NULL)
						pos++;
				}
			} else {
				value = pos;
			}

			/* Search next parameter */
			if ( pos!=NULL && *pos != '\0' ) {
				pos = strchr( pos, '&' );
				if (pos != NULL) {
					/* Terminate value */
					*pos = '\0'; /* '&' -> '\0' */
					pos++;
				}; /* if */
			}

			/* PTS1117669: unescape value and key */
			wd21_PlusToSpace( name, strlen(name) );
			wd21_UnescapeUrl( name, strlen(name) );
			wd21_PlusToSpace( value, strlen(value) );
			wd21_UnescapeUrl( value, strlen(value) );
			/* Add parameter/value to list */
            wd28AddEntry( dict, name, value );
        }; /* if */
    }; /* while */
    
    return insertOK;
} /* wd21_InsertValues */


char wd21_x2c(char *what) {
    char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
} /* wd21_x2c */


void wd21_UnescapeUrl( char *url, sapdbwa_Int4 len ) {
    sapdbwa_Int4 x,y;

    for(x=0,y=0; y<len; ++x,++y) {
        if((url[x] = url[y]) == '%') {
			url[x] = wd21_x2c(&url[y+1]);
			y+=2;
        }
    }
    url[x] = '\0';
} /* wd21_UnescapeUrl */


sapdbwa_Bool wd21_ReadBodyData( sapdbwa_HttpRequestP req )
{
    sapdbwa_Bool  readOK = true;
    sapdbwa_ULong BufLen = /*req->numContentLength*/min(req->numContentLength, 8192)/**/;
        
    /* insert values from the body */
    sapdbwa_SQLALLOCAT( BufLen+1,
                   (sapdbwa_UInt1**) &req->bodyData,
                   &readOK );
    if (readOK) {
        sapdbwa_SQLALLOCAT( BufLen+1,
                       (sapdbwa_UInt1**) &req->paramBodyData,
                       &readOK );
        if (readOK) {
            sapdbwa_ULong bytesRead = 0;
            if(req->readBodyBytes < req->numContentLength) {
                bytesRead = req->ReadBody( req->serverHandle,
                                        req->bodyData,
                                        BufLen);
            }
            req->readBodyBytes += bytesRead;
            /*readOK = (bytesRead == req->numContentLength);*/
            readOK = (bytesRead > 0);
            if (readOK) {
				/* PTS1117669: Unescaping the querystring right here leads to problems
				   during separation of the querystring into its parameters.
				   It is better to unescape the separated parameter key and value when
				   separation is done.
				   For instance if a '&' is part of the value and it was sent escaped as
				   %26 then unescaping it right now would introduce a new parameter separator
				   into the querystring. The parameter value containg this %26 will be cut.
				   Information is lost */
                /* wd21_PlusToSpace( req->bodyData, bytesRead );*/
                /* wd21_UnescapeUrl( req->bodyData, bytesRead );*/
                req->bodyDataLen = bytesRead;
                req->bodyDataPos = 0;
                SAPDB_memcpy( req->paramBodyData, req->bodyData, bytesRead );/* memcpy SAFE, NO OVERLAP HERE. */
                /* we need a null termination, bacause paramBodyData is parsed */
                req->paramBodyData[bytesRead] = '\0';
            }; /* if */
        }; /* if */
    }; /* if */

    return readOK;
} /* wd21_ReadBodyData */

void wd21_PlusToSpace( char *str, sapdbwa_Int4 len ) {
    sapdbwa_Int4 x;

    for( x=0; x<len; x++) if(str[x] == '+') str[x] = ' ';
} /* wd21_PlusToSpace */

/* stolen from Apache */
sapdbwa_Int4 wd21_Base64DecodeBinary( char       *bufplain,
                                    const char *bufcoded);

sapdbwa_Int4 wd21_Base64Decode( char       *bufplain,
                              const char *bufcoded)
{
    sapdbwa_Int4 len;
    
    len = wd21_Base64DecodeBinary( (char*) bufplain, bufcoded);

    return len;
} /* wd21_Base64Decode */


/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

sapdbwa_Int4 wd21_Base64DecodeBinary( char       *bufplain,
                                    const char *bufcoded)
{
    sapdbwa_Int4           nbytesdecoded;
    register const char *bufin;
    register char       *bufout;
    register sapdbwa_Int4  nprbytes;

    bufin = (const char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
    
    bufout = (char *) bufplain;
    bufin = (const char *) bufcoded;
    
    while (nprbytes > 4) {
        *(bufout++) =
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
        *(bufout++) =
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
        *(bufout++) =
            (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        *(bufout++) =
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
        *(bufout++) =
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
        *(bufout++) =
	    (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }
    
    *(bufout++) = '\0';
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
} /* wd21_Base64DecodeBinary */
