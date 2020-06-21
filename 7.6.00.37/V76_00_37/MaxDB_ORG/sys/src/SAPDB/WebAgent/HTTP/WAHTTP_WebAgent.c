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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Agent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_WebAgent.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_HTTP.h"

#include "gwd00.h"
#include "hwd01wadef.h"
#include "hwd03waapi.h"
#include "hwd20control.h"
#include "hwd21req.h"
#include "hwd22rep.h"
#include "hwd28dict.h"
#include "hwd29date.h"

#define MODULE_NAME "WAHTTP_WebAgent.c"
#ifndef __FILE__
#define __FILE__ MODULE_NAME
#endif

sapdbwa_TraceDefModule( MODULE_NAME )

struct st_agent_client
{
 int something;
};

/* Init Web Agent */
int InitWebAgent( struct st_agent_server     *as,
                  struct st_agent_client    **wah )
{

    if (!wd20ApiInit(as->iniFile, API_SAPDBWWW_WD20 )) {
        return 0;
    }

    return 1;

}

int ExitWebAgent( struct st_agent_server *as,
                  struct st_agent_client *wah )
{

    wd20ApiExit();

    return(TRUE);
}

typedef struct st_wa_sapw3_server_handle
{
 struct st_agent_server *as;
 struct st_agent_req	*req;
 struct st_agent_rep	*rep;
 twd28DictionaryP	headerFields;	/* allocated header fields of the
					 * request allocated by
					 * wd52GetHeaderField are stored here */
} *twd52SapW3ServerHandle;

twd52SapW3ServerHandle wd52_CreateServerHandle
(
 struct st_agent_server *as,
 struct st_agent_req	*req,
 struct st_agent_rep	*rep
)
{
 twd52SapW3ServerHandle	handle;
 sapdbwa_Bool		allocatOK;

 wahttp_SQLALLOCAT( sizeof( struct st_wa_sapw3_server_handle), &handle, &allocatOK );
 if (!allocatOK)
 {
	/* memory allocation error */
	wd20WriteLogMsg( ERR_MSG_MEMORY_WD26 );
	return NULL;
 }; /* if */

 handle->headerFields = wd28CreateDictionary();
 if (handle->headerFields == NULL)
 {
	wahttp_SQLFREE( handle );
	wd20WriteLogMsg( ERR_MSG_MEMORY_WD26 );
	return NULL;
 }; /* if */
    
 handle->as = as;
 handle->req = req;
 handle->rep = rep;

 return handle;
} /* wd52_CreateServerHandle */

void wd52_DestroyServerHandle
(
 twd52SapW3ServerHandle handle
)
{
    wd28DestroyDictionary( handle->headerFields );        
    wahttp_SQLFREE( handle );
} /* wd52_DestroyServerHandle */

/*========================================================================*/

sapdbwa_ULong wd52SapW3ReadBody( twd00ServerHandle   handle,
                                 char               *b,
                                 sapdbwa_ULong       len )
{

 twd52SapW3ServerHandle	sapw3Handle = (twd52SapW3ServerHandle) handle;

 return(sapw3Handle->as->Receive(sapw3Handle->req, b, len));

} /* wd52SapW3ReadBody */

/*========================================================================*/

const char* wd52SapW3GetHeader
(
 twd00ServerHandle	handle,
 const char		*fieldName
)
{
 twd52SapW3ServerHandle	sapw3Handle = (twd52SapW3ServerHandle) handle;
 const char		*result;
 char			*value;
    
  /* did we already retrieve this field ? */
 result = wd28GetValueByKey( sapw3Handle->headerFields, fieldName );
 if (result == NULL)
 {
	/* try to get value from web server */

	/* get the value */
	value = sapw3Handle->as->GetHeaderValueByName( sapw3Handle->req, (char*) fieldName);
	if (value)
	{
		if (!wd28AddCopyEntry( sapw3Handle->headerFields, fieldName, value ))
		{
			result = NULL;
	        }
		else
		{
			result = wd28GetValueByKey( sapw3Handle->headerFields, fieldName );
		}; /* else */
	}
	else
	{
		result = NULL;
	}; /* else */
 }; /* if */
    
 return result;
} /* wd52SapW3GetHeader */

sapdbwa_Bool wd52SapW3IsSecure
(
 twd00ServerHandle	handle
)
{
 const char *isSecure = NULL;
    
 isSecure = wd52SapW3GetHeader( handle, "SERVER_PORT_SECURE" );

 return (isSecure != NULL && (strcmp( isSecure, "1" ) == 0));
} /* wd52SapW3IsSecure */

sapdbwa_Bool wd52SapW3SendHeader
(
 twd00ServerHandle	handle,
 sapdbwa_HttpReplyP	rep
)
{
 twd52SapW3ServerHandle	sapw3Handle;
 char			*tmp;
 struct st_agent_server *sapw3_as;
 struct st_agent_rep	*sapw3_rep;
 char			Date[MAX_DATE_LEN_WD29] = "";
 sapdbwa_UInt4		iHeader;
 twd28DictionaryP	headers = wd22GetHeaders( rep );
 char			*name, *value;
 int			status;

 sapw3Handle = (twd52SapW3ServerHandle) handle;
 sapw3_as = sapw3Handle->as;
 sapw3_rep = sapw3Handle->rep;

 wd29HttpDate(Date);
 sapw3_as->AddHeader_char(sapw3_rep, AGENT_DATE, Date);

 tmp = wd22GetLocation(rep);
 if( tmp != NULL && strlen(tmp))
 {
	sapw3_as->AddHeader_char(sapw3_rep, AGENT_LOCATION, tmp);
 };

 tmp = wd22GetLastModified(rep);
 if( tmp != NULL && strlen(tmp))
 {
	sapw3_as->AddHeader_char(sapw3_rep, AGENT_LAST_MODIFIED, tmp);
 };

 tmp = wd22GetContentLength(rep);
 if( tmp != NULL && strlen(tmp))
 {
	sapw3_as->AddHeader_char(sapw3_rep, AGENT_CONTENT_LENGTH, tmp);
 };

 tmp = wd22GetContentType(rep);
 if( tmp != NULL && strlen(tmp))
 {
	sapw3_as->AddHeader_char(sapw3_rep, AGENT_CONTENT_TYPE, tmp);
 };

 tmp = wd22GetExpires(rep);
 if( tmp != NULL && strlen(tmp) && wd29WADateToHttpDate( tmp, Date))
 {
	sapw3_as->AddHeader_char(sapw3_rep, AGENT_EXPIRES, Date);
 };

 for (iHeader = 0; iHeader < wd28NumEntries( headers ); iHeader++)
 {
	if (wd28GetEntryByIndex( headers, iHeader, (const char**)&name, &value ))
	{
		/* there must not be a space between the ':' and the value;
		 * else IE 5 is confused */	/* Check this with sapw3 to make sure (TG) */
		sapw3_as->AddHeaderByName_char(sapw3_rep, name, value);
	}; /* if */
 }; /* for */
    
 switch (wd22GetStatus(rep))
 {
	case sapdbwa_Status_Ok:
		status = AGENT_STATUS_OK;
		break;
	case sapdbwa_Status_Created:
		status = AGENT_STATUS_CREATED;
		break;
	case sapdbwa_Status_Response:
		status = AGENT_STATUS_NO_CONTENT;
		break;
	case sapdbwa_Status_Moveto:
		status = AGENT_STATUS_MOVE_PERM;
		break;
	case sapdbwa_Status_Redirect:
		status = AGENT_STATUS_REDIRECT;
		break;
	case sapdbwa_Status_Modified:
		status = AGENT_STATUS_NOT_MODIFIED;
		break;
	case sapdbwa_Status_BadRequest:
		status = AGENT_STATUS_BAD_REQUEST;
		break;
	case sapdbwa_Status_Unauthorized:
		status = AGENT_STATUS_UNAUTHORIZED;
		break;
	case sapdbwa_Status_Forbidden:
		status = AGENT_STATUS_FORBIDDEN;
		break;
	case sapdbwa_Status_NotFound:
		status = AGENT_STATUS_NOT_FOUND;
		break;
	case sapdbwa_Status_MethodNotAllowed:
		status = AGENT_STATUS_METHOD_NOT_ALLOWED;
		break;
	case sapdbwa_Status_Conflict:
		status = AGENT_STATUS_CONFLICT;
		break;
	case sapdbwa_Status_ServerError:
		status = AGENT_STATUS_SERVER_ERROR;
		break;
	case sapdbwa_Status_NotImplemented:
		status = AGENT_STATUS_NOT_IMPLEMENTED;
		break;
	default:
		if (GetEnableWebDav()) {
			/* WebDAV knows some more status codes */
			switch (wd22GetStatus(rep))
			{
				case sapdbwa_Status_Multistatus:
					status = AGENT_STATUS_MULTISTATUS;
					break;
				case sapdbwa_Status_PreconditionFailed:
					status = AGENT_STATUS_PRECONDITION_FAILED;
					break;
				/* Correction Begin: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
				case sapdbwa_Status_Partial_Content:
					status = AGENT_STATUS_PARTIAL_CONTENT;
					break;
				case sapdbwa_Status_Requested_Range_Not_Satisfiable:
					status = AGENT_STATUS_RANGE_NOT_SATISFIABLE;
					break;
				/* Correction End: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
				case sapdbwa_Status_UnsupportedMediaType:
					status = AGENT_STATUS_UNSUPPORTED_MEDIA_TYPE;
					break;
				case sapdbwa_Status_Locked:
					status = AGENT_STATUS_LOCKED;
					break;
				case sapdbwa_Status_BadGateway:
					status = AGENT_STATUS_BAD_GATEWAY;
					break;
				case sapdbwa_Status_InsufficentStorage:
					status = AGENT_STATUS_INSUFFICENT_STORAGE;
					break;
				default:
					status = AGENT_STATUS_SERVER_ERROR;
			}
		} else {
			status = AGENT_STATUS_SERVER_ERROR;
		}
 }

 if (!sapw3_as->SendHeader(sapw3_rep, status))
 {
	wd20WriteLogMsg( __FILE__ ": sapw3_as->SendHeader failed.\n" );
	return(false);
 };
 return(true);
} /* wd52SapW3SendHeader */

sapdbwa_UInt4 wd52SapW3SendBody
(
 twd00ServerHandle	handle,
 const void		*Buf,
 sapdbwa_UInt4		len
)
{
 twd52SapW3ServerHandle	sapw3Handle = (twd52SapW3ServerHandle) handle;

 return(sapw3Handle->as->Send(sapw3Handle->rep, (void *)Buf, len));
} /* wd52SapW3SendBody */


enum en_af_returncode TryWebAgent( struct st_agent_server   *sapw3_as,
                                   struct st_agent_client   *sapw3_wah,
                                   struct st_agent_req      *sapw3_req,
                                   struct st_agent_rep      *sapw3_rep )
{

    DWORD                   rtc = af_ignored;
    twd21HttpRequestP       req = NULL;
    sapdbwa_HttpReplyP      rep = NULL;
    twd52SapW3ServerHandle  handle = NULL;

    handle = wd52_CreateServerHandle( sapw3_as , sapw3_req , sapw3_rep );
    if (handle == NULL) {
        /* memory allocation error */
        wd20WriteLogMsg( ERR_MSG_MEMORY_WD26 );
        return rtc;
    }
        
    req = wd21CreateRequest( handle, wd52SapW3ReadBody, wd52SapW3GetHeader, wd52SapW3IsSecure );
    if (req == NULL) {
        /* memory allocation error */
        wd20WriteLogMsg( ERR_MSG_MEMORY_WD26 );
        wd52_DestroyServerHandle( handle );
        return rtc;
    };

    /* Set log description */
    req->logDesc = (WA_LogDesc)sapw3_req->ServerReq->logDesc;

    rep = wd22CreateReply( handle, wd52SapW3SendHeader, wd52SapW3SendBody );
    if (rep == NULL) {
        /* memory allocation error */
        wd20WriteLogMsg( ERR_MSG_MEMORY_WD26 );
        wd21DestroyRequest( req );        
        wd52_DestroyServerHandle( handle );
        return rtc;
    };
    
    wd21SetUri( req, sapw3_req->uri );

    /*wd52_BillsIfModifiedSince( req );*/
    wd21SetQueryString( req, sapw3_req->querystring );

#ifdef notyet
    wd21SetPathInfo( req, sapw3_req->path_info );
#else
    /* path info not implemented yet */
    /*
    wd20WriteLogMsg( "path_info not implemented yet...\n" );
    */
#endif

    wd21SetRequestMethod( req, sapw3_req->method );
    wd21SetContentType( req, sapw3_as->GetHeaderValue(sapw3_req, AGENT_CONTENT_TYPE ));
    wd21SetContentLength( req, sapw3_as->GetHeaderValue(sapw3_req, AGENT_CONTENT_LENGTH ));
    wd21SetContentChunked( req, sapw3_req->ServerReq->contentChunked );

#ifdef notyet
    wd21SetPhysicalPath( req, sapw3_req->physical_path );
#else
    /* physical path not implemented yet */
    /*
    wd20WriteLogMsg( "physical_path not implemented yet..." );
    */
#endif

#ifdef notyet
    wd21SetServerName( req, sapw3_as->server_name );
#else
    /* server name not implemented yet */
    /*
    wd20WriteLogMsg( "server_name not implemented yet..." );
    */
#endif

    switch(wd20CallServiceFunc( req, rep )) {
	case sapdbwa_RTC_Ignored:
        rtc = af_ignored;
        break;
	case sapdbwa_RTC_Accepted:
        rtc = af_accepted;
        break;
	case sapdbwa_RTC_Error:
        rtc = af_error;
        break;
    default:
        rtc = af_error;
        break;
    }; /* switch */

    wd21DestroyRequest( req );
    wd22DestroyReply( rep );
    wd52_DestroyServerHandle( handle );
    
    return(rtc);

} /* RequestMap */
