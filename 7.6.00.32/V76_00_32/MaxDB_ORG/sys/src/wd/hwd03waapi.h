/*!**********************************************************************

  module: hwd003waapi.h -> sapdbwa.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Oezgen

  special area: SAPDB WebAgent C Interface

  description:  Export description of WebAgent C interface

  see also:     

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 2000-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef SAPDBWA_C_H
#define SAPDBWA_C_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <sapdbwa_define.h>
#include <sapdbwa_session.h>
#include <sapdbwa_error.h>

/* ADT´s */

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct st_wa_handle			*sapdbwa_Handle;
typedef struct st_wa_stringseq		*sapdbwa_StringSeqP;
typedef struct st_wa_http_req		*sapdbwa_HttpRequestP;
typedef struct st_wa_http_rep		*sapdbwa_HttpReplyP;
typedef struct st_wa_web_session	*sapdbwa_WebSessionP;

/****************************************************************************
 ***
 *** Trace Macros
 ***
 ****************************************************************************/

/*
#ifdef SAPDB_FAST
*/
#define sapdbwa_TraceDefModule(module)
#define sapdbwa_TraceDefFunction(function)
#define sapdbwa_TraceAdd(value)
/*
#else
#define sapdbwa_TraceDefModule(module)		static const char sapdbwa_TraceModule[] = module;
#define sapdbwa_TraceDefFunction(function)	const char sapdbwa_TraceFunction[] = function;\
											sapdbwa_WriteTraceLine( sapdbwa_TraceModule, sapdbwa_TraceFunction, "Entering funtion" );
#define sapdbwa_TraceAdd(value)				sapdbwa_WriteTraceLine( sapdbwa_TraceModule, sapdbwa_TraceFunction, value );
#endif
*/


/*!**********************************************************************

  Chapter: Prototypes for UserDll functions

************************************************************************/

/* Initialize UserDll */
sapdbwa_Int2 WebAgentApiInit( sapdbwa_Handle wa );

/* Service function in UserDll */
sapdbwa_Int2 WebAgentApiService( sapdbwa_Handle        wa,
                               sapdbwa_HttpRequestP  request,
                               sapdbwa_HttpReplyP    reply );

typedef sapdbwa_Bool (*sapdbwa_CDestructor)( sapdbwa_Handle wa, void *userData );

/*!**********************************************************************
  Function: sapdbwa_WriteTraceLine

  description: Write trace plus end of line.

  arguments:
	module		[in]	The module whis is to be traced.
	function	[in]	The function whis is to be traced.
    trace		[in]	The message which is to be written.
    
  return value: void */

void sapdbwa_WriteTraceLine( const char *module,
                        const char *function,
                        const char *trace	);
    
/*!**********************************************************************

  EndChapter: Prototypes for UserDll

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_WebSessionP

  description:
  
************************************************************************/

void* WINAPI sapdbwa_GetSessionData( sapdbwa_WebSessionP wses );


sapdbwa_Bool WINAPI sapdbwa_SetSessionData( sapdbwa_WebSessionP   wses,
                                     void            *data,
                                     sapdbwa_CDestructor    destructor );

sapdbwa_Bool WINAPI sapdbwa_NewSessionId( sapdbwa_WebSessionP  wses );
    
/*!**********************************************************************

  EndChapter: sapdbwa_WebSessionP

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_Handle

  description:
  
************************************************************************/

sapdbwa_Bool sapdbwa_GetParam( sapdbwa_Handle    wa,
                        const char  *key,
                        char        *valueBuf,
                        sapdbwa_Int4   bufLen );

sapdbwa_Bool sapdbwa_GetDocumentRoot( sapdbwa_Handle  wa,
                                      char           *valueBuf,
                                      sapdbwa_Int4    bufLen );
    
sapdbwa_SessionPoolP WINAPI sapdbwa_GetSessionPool( sapdbwa_Handle wa );

sapdbwa_WebSessionP WINAPI sapdbwa_GetWebSession( sapdbwa_Handle wa );

void WINAPI sapdbwa_ReleaseWebSession( sapdbwa_Handle wa );

sapdbwa_WebSessionP WINAPI sapdbwa_StartWebSession( sapdbwa_Handle    wa,
                                          sapdbwa_UInt4  timeOut );

sapdbwa_WebSessionP WINAPI sapdbwa_StartNewWebSession( sapdbwa_Handle     wa,
                                                       sapdbwa_HttpReplyP rep,
                                                       sapdbwa_UInt4      timeOut );

sapdbwa_WebSessionP WINAPI sapdbwa_StartDefaultWebSession( sapdbwa_Handle  wa );

sapdbwa_ErrP WINAPI sapdbwa_HandleGetErr( sapdbwa_Handle wa );
    
sapdbwa_Bool WINAPI sapdbwa_WriteLogMsg( sapdbwa_Handle wa, const char *msg );

sapdbwa_Bool WINAPI sapdbwa_SetUseWebSessions( sapdbwa_Handle   wa,
                                        sapdbwa_Bool  useWebSessions );    

sapdbwa_Bool WINAPI sapdbwa_CloseWebSession( sapdbwa_Handle      wa,
                                             sapdbwa_WebSessionP wses,
											 sapdbwa_Bool		 freeUserData );

const char* WINAPI sapdbwa_GetServiceName( sapdbwa_Handle   wa );

/*!**********************************************************************

  EndChapter: sapdbwa_Handle

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_StringSeqP

  description:
  
************************************************************************/

sapdbwa_StringSeqP WINAPI sapdbwa_CreateStringSeq();
    
void WINAPI sapdbwa_DestroyStringSeq( sapdbwa_StringSeqP seq );

sapdbwa_Int4 WINAPI sapdbwa_GetNumElem( sapdbwa_StringSeqP seq );

const char* WINAPI sapdbwa_GetStringByIndex( sapdbwa_StringSeqP strSeq, sapdbwa_Int4 index );    
    
/*!**********************************************************************

  EndChapter: sapdbwa_StringSeqP
  
************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_HttpRequestP

  description:
  
************************************************************************/

sapdbwa_Bool WINAPI sapdbwa_GetAuthorization( sapdbwa_HttpRequestP   req,
                                       const char     **userName,
                                       const char     **password );
    
sapdbwa_ErrP WINAPI sapdbwa_ReqGetErr( sapdbwa_HttpRequestP req );
    
sapdbwa_Bool WINAPI sapdbwa_GetCookieNames( sapdbwa_HttpRequestP req, sapdbwa_StringSeqP names );

const char* WINAPI sapdbwa_GetCookieValue( sapdbwa_HttpRequestP req, const char *name );
    
const char* WINAPI sapdbwa_GetHeader( sapdbwa_HttpRequestP req, const char *fieldName );

const char* WINAPI sapdbwa_GetRequestURI( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetIfModifiedSince( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetQueryString( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetPathInfo( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetMethod( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetContentType( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetContentLength( sapdbwa_HttpRequestP req );

sapdbwa_Bool WINAPI sapdbwa_GetContentChunked( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetPathTranslated( sapdbwa_HttpRequestP req );

const char* WINAPI sapdbwa_GetServerName( sapdbwa_HttpRequestP req );

sapdbwa_Bool WINAPI sapdbwa_GetParameterNames( sapdbwa_HttpRequestP req, sapdbwa_StringSeqP names );

sapdbwa_Bool WINAPI sapdbwa_GetParameterValues( sapdbwa_HttpRequestP  req,
                                         const char      *parameterName,
                                         sapdbwa_StringSeqP    values );

sapdbwa_Bool WINAPI sapdbwa_IsSecure( sapdbwa_HttpRequestP  req );

sapdbwa_ULong WINAPI sapdbwa_GetNumericalContentLength( sapdbwa_HttpRequestP req  );

sapdbwa_ULong WINAPI sapdbwa_GetReadBodyBytes( sapdbwa_HttpRequestP req  );

sapdbwa_Bool WINAPI sapdbwa_ExtractValue(const char    * headerField,
                                                sapdbwa_ULong   headerFieldLen,
                                                const char    * upperCaseName,
                                                const char    * lowerCaseName,
                                                sapdbwa_ULong * valueStart, 
                                                sapdbwa_ULong * valueLen);
                                            
sapdbwa_ULong WINAPI sapdbwa_ReadBody( sapdbwa_HttpRequestP req, char *buff, sapdbwa_ULong len );

sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_NextPart(sapdbwa_HttpRequestP   req,
                                                       const char           * boundary,
                                                       sapdbwa_Bool         * moreParts);

sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_PartHeaderField(sapdbwa_HttpRequestP   req,
                                                              char                 * buff,
                                                              sapdbwa_ULong          len,
                                                              sapdbwa_ULong        * readBytes,
                                                              sapdbwa_Bool         * moreData,
                                                              sapdbwa_Bool         * moreHeaderFields);

sapdbwa_Bool WINAPI sapdbwa_MimeMultipartBody_PartBody(sapdbwa_HttpRequestP    req,
                                                       const char            * boundary,
                                                       char                  * buff,
                                                       sapdbwa_ULong           len,
                                                       sapdbwa_ULong         * readBytes,
                                                       sapdbwa_Bool          * moreData,
                                                       sapdbwa_Bool          * moreParts);


/*!**********************************************************************

  EndChapter: sapdbwa_HttpRequestP

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_HttpReply

  description:
  
************************************************************************/

sapdbwa_ErrP WINAPI sapdbwa_RepGetErr( sapdbwa_HttpReplyP rep );
    
sapdbwa_Bool WINAPI sapdbwa_InitHeader( sapdbwa_HttpReplyP  rep,
                                 sapdbwa_Int2     Status,
                                 const char    *ContentType,
                                 const char    *ContentLength,
                                 const char    *LastModified,
                                 const char    *Expires,
                                 const char    *Location );

sapdbwa_Bool WINAPI sapdbwa_SetHeader( sapdbwa_HttpReplyP  rep,
                                const char    *name,
                                const char    *value );

sapdbwa_Bool WINAPI sapdbwa_ReplaceHeader( sapdbwa_HttpReplyP  rep,
                                           const char         *name,
                                           const char         *oldvalue,
                                           const char         *value );

sapdbwa_Bool WINAPI sapdbwa_SetCookie( sapdbwa_HttpReplyP  rep,
                                const char    *name,
                                const char    *value,
                                const char    *expires,
                                const char    *path,
                                const char    *domain,
                                sapdbwa_Bool     secure );

sapdbwa_Bool WINAPI sapdbwa_ReplaceCookieValue( sapdbwa_HttpReplyP  rep,
                                                const char         *name,
                                                const char         *newvalue );
    
sapdbwa_Bool WINAPI sapdbwa_SendHeader( sapdbwa_HttpReplyP rep );


sapdbwa_UInt4 WINAPI sapdbwa_SendBody( sapdbwa_HttpReplyP    rep,
                                       const void           *buff,
                                       sapdbwa_UInt4         len );

sapdbwa_UInt4 WINAPI sapdbwa_SendBodyChunk( sapdbwa_HttpReplyP   rep,
                                            const void          *buff,
                                            sapdbwa_UInt4        len );

sapdbwa_UInt4 WINAPI sapdbwa_SendBodyChunkEnd( sapdbwa_HttpReplyP   rep );


sapdbwa_Bool WINAPI sapdbwa_SendFile( sapdbwa_HttpReplyP   rep,
                               const char	  *fileName );

sapdbwa_Bool WINAPI sapdbwa_SendTemplate( sapdbwa_HttpReplyP  rep,
                                   const char    *fileName );

/*!**********************************************************************

  EndChapter: sapdbwa_HttpReply

************************************************************************/
    
#if defined(__cplusplus)
}
#endif

#endif

