/*!**********************************************************************

  module: vwd96wacom.cpp

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Özgen

  special area: WebAgent

  description:  UserDll for WebAgent COM interface

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "hwd97stdafx.h"

#include "heo07.h" /* threading API */

#include "hwd09util.h"
#include "hwd20control.h"
#include "hwd25log.h"
#include "hwd27excl.h"
#include "hwd96wacom.h"
#include "hwd98ustring.h" // uxdup, ...

//#include "vwd95wacom_i.c"
////////////////////////////////////////////
// contents of vwd95wacom_i.c

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_WebAgentCOM = {0xee05d370,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const IID IID_IWebAgentCOM = {0x888c87f0,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const IID IID_IWACallBackObject = {0xc8b2a830,0xc841,0x11d3,{0xa9,0xc2,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const IID IID_IWebAgentCB = {0x8bea5db0,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const IID IID_IHttpRequestCB = {0xd7403720,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const IID IID_IHttpReplyCB = {0x4fb4cae0,0xbec8,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const CLSID CLSID_CWebAgentCB = {0xcf2824e0,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const CLSID CLSID_CHttpRequestCB = {0xde6e7db0,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};


const CLSID CLSID_CHttpReplyCB = {0xe5c78d80,0xbec7,0x11d3,{0xa9,0xc1,0x00,0x60,0x94,0xb9,0x1c,0x8b}};

// end contents of vwd95wacom_i.c
////////////////////////////////////////////

# define THREADING_MODEL COINIT_MULTITHREADED
//# define THREADING_MODEL COINIT_APARTMENTTHREADED

# define WE_DO_IT_INPROCLY
//# define WE_DO_IT_LOCALLY
//# define WE_DO_IT_REMOTELY


# ifdef WE_DO_IT_INPROCLY
# define SERVERTYPE CLSCTX_INPROC_SERVER 
# define SERVERINFO NULL
# define REMOTE_SERVERNAME L""
# define SERVER_PRINCIPAL_NAME L""
# define REMOTE_USER ""
# define REMOTE_PASSWD ""
# define AUTHIDENT_DOMAIN ""
# define AUTHIDENT_USER ""
# define AUTHIDENT_PASSWORD ""
# endif

# ifdef WE_DO_IT_LOCALLY
# define SERVERTYPE CLSCTX_LOCAL_SERVER
# define SERVERINFO NULL
# define REMOTE_SERVERNAME L""
# define SERVER_PRINCIPAL_NAME L""
# define REMOTE_USER ""
# define REMOTE_PASSWD ""
# define AUTHIDENT_DOMAIN ""
# define AUTHIDENT_USER ""
# define AUTHIDENT_PASSWORD ""
# endif

# ifdef WE_DO_IT_REMOTELY
# define SERVERTYPE CLSCTX_REMOTE_SERVER
# define SERVERINFO &ServerInfo
# define REMOTE_SERVERNAME L"P26240"
# define SERVER_PRINCIPAL_NAME L"foekje.de.infolytics.com"
# define REMOTE_USER "internet"
# define REMOTE_PASSWD "internet"
# define AUTHIDENT_DOMAIN "Infolytics"
# define AUTHIDENT_USER "internet"
# define AUTHIDENT_PASSWORD "internet"
# endif

//////////////////
// globals
//////////////////

static sapdbwa_Int4  wd96TlsIndexHandle;   // thread local storage index handle

/* *********************************************************** */
/* *********************************************************** */

sapdbwa_Bool wd96ComInit()
{
    teo07_Thread     thisThread;
    tsp00_ErrTextc   errtext;
    teo07_ThreadErr  err;
    
    errtext[0] = '\0';
            
    sqlinit_multithreading( &thisThread, errtext, &err );    
    if (err == THR_OK_EO07) {
        sqlcreatetls( &wd96TlsIndexHandle, errtext, &err );
        if(err != THR_OK_EO07) {
            // error handling ...
        };
    };

    return (err == THR_OK_EO07);
} /* wd86COMInit */


void wd96ComExit()
{
    // free tls ...
}; // wd86WAComExit

////////////////////////////////////
// prototypes for helper functions
////////////////////////////////////


bool wd96_ThisThreadIsCoInitialized(void);

void wd96_MarkThisThreadAsCoInitialized(void);

HRESULT wd96_MakeBSTR( const char *strIn, BSTR *bstrOut );

HRESULT wd96_MakeBSTRArray( sapdbwa_StringSeqP strSeq, SAFEARRAY **array );

/*!**********************************************************************

  Chapter: CWebAgentCB

************************************************************************/

class CWebAgentCB : public IWebAgentCB
{
public:
    CWebAgentCB( sapdbwa_Handle  wa, twd96ComClassP  comClass );

	// IUnknown :
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface( REFIID riid, void ** ppv );

    // IWACallBackObject
    HRESULT __stdcall GetErrMsg( BSTR  *errMsg );
    HRESULT __stdcall GetErrId( long *errId );

    // IWebAgentCB
    HRESULT __stdcall WriteLogMsg( BSTR msg );
    HRESULT __stdcall GetParam( BSTR   paramName,
                                BSTR  *paramValue );
private:
	ULONG           m_cRef;   // ref counter
    sapdbwa_Handle       webAgent;
    twd96ComClassP  comClass;
}; // class CWebAgentCB

/*!**********************************************************************

  EndChapter: CWebAgentCB

************************************************************************/
/*!**********************************************************************

  Chapter: CHttpRequestCB

************************************************************************/

// this implements the WebAgentCOM read request-body callback interface,
// "IHttpRequestCB" :

class CHttpRequestCB : public IHttpRequestCB
{
public:
	CHttpRequestCB( sapdbwa_HttpRequestP  req);

	// IUnknown :
    ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface( REFIID riid, void ** ppv);

    // IWACallBackObject
    HRESULT __stdcall GetErrMsg( BSTR  *errMsg );
    HRESULT __stdcall GetErrId( long *errId );

	// IHttpRequestCB :
    HRESULT __stdcall GetHeader( BSTR   name, BSTR *value );
    HRESULT __stdcall GetRequestURI( BSTR *value );
    HRESULT __stdcall GetIfModifiedSince( BSTR *value );
    HRESULT __stdcall GetQueryString( BSTR *value );
    HRESULT __stdcall GetPathInfo( BSTR *value );
    HRESULT __stdcall GetMethod( BSTR *value );
    HRESULT __stdcall GetContentType( BSTR *value );
    HRESULT __stdcall GetContentLength( BSTR *value );
    HRESULT __stdcall GetPathTranslated( BSTR *value );
    HRESULT __stdcall GetServerName( BSTR *value );

    HRESULT __stdcall GetCookieNames( SAFEARRAY **ppNames );
    HRESULT __stdcall GetParameterNames( SAFEARRAY **ppNames );
    HRESULT __stdcall GetParameterValues( BSTR         paramName,
                                          SAFEARRAY  **ppValues );
    
	HRESULT __stdcall GetReqBody( SAFEARRAY **Buf );

private:
    ULONG            m_cRef; // ref counter
    sapdbwa_HttpRequestP  m_req; 
}; // class CHttpRequestCB


/*!**********************************************************************

  EndChapter: CHttpRequestCB

************************************************************************/
/*!**********************************************************************

  Chapter: CHttpReplyCB

************************************************************************/

// this implements the WebAgentCOM write reply-body callback interface,
// "IHttpReplyCB" :
class CHttpReplyCB : public IHttpReplyCB
{
public:
	CHttpReplyCB( sapdbwa_HttpReplyP reply );

	~CHttpReplyCB();

	// IUnknown :
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void ** ppv);

    // IWACallBackObject
    HRESULT __stdcall GetErrMsg( BSTR  *errMsg );
    HRESULT __stdcall GetErrId( long *errId );

	// IHttpReplyCB :
    HRESULT __stdcall InitHeader( short status,
                                  BSTR  contentType,
                                  BSTR  contentLength,
                                  BSTR  lastModified,
                                  BSTR  expires,
                                  BSTR  location );
    HRESULT __stdcall SetHeader( BSTR  headerName,
                                 BSTR  headerValue );
    HRESULT __stdcall SetCookie( BSTR         name,
                                 BSTR         value,
                                 BSTR         expires,
                                 BSTR         path,
                                 BSTR         domain,
                                 short        secure );
    HRESULT __stdcall SendHeader();        
    HRESULT __stdcall SendBody( BSTR  ReplyBody);
    
private:
	ULONG         m_cRef;    // ref counter :
    sapdbwa_HttpReplyP  httpReply;
	char         *repBuf;
	long          repBuflen;
}; // class CHttpReplyCB


/*!**********************************************************************

  EndChapter: CHttpReplyCB

************************************************************************/
/*!**********************************************************************

  Chapter: CObjPool

************************************************************************/

#define MAX_FREELIST_COUNT 1024

class CObjPool
{
public :
	CObjPool( UUID uuid );
	~CObjPool();
	void GetObj(IWebAgentCOM **ppWebAgentCOM );
	void ReleaseObj(IWebAgentCOM *pWebAgentCOM);

private :
    UUID           uuid;
    twd27ExclP     objPoolExcl;
	unsigned long  freelistCount;
	IWebAgentCOM  *freelist[MAX_FREELIST_COUNT];

	bool CheckObj( IWebAgentCOM *pWebAgentCOM );
	bool MakeObj( IWebAgentCOM **ppWebAgentCOM );
}; // class CObjPool

/*!**********************************************************************

  EndChapter: CObjPool

************************************************************************/
/*!**********************************************************************

  Chapter: twd96ComClassP

************************************************************************/

struct st_com_class
{
    sapdbwa_UInt4        id;
    char               classId[MAX_CLASSID_LEN_WD00];
    sapdbwa_Bool         initialized; /* com class init function called ? */
    char               name[MAX_NAME_LEN_WD00];
    char               pathPrefix[MAX_NAME_LEN_WD00];
    twd25LogP          log;

    CObjPool          *objPool;            // the COM object pool
}; /* struct st_com_object */


twd96ComClassP wd96CreateComClass( char        *name,
                                   sapdbwa_UInt4  id,
                                   twd26ErrP    err )
{
    sapdbwa_Bool	allocatOK;
    twd96ComClassP	newComClass;
    size_t          nameLen;
    
    nameLen = strlen( name );
    if (nameLen >= sizeof( newComClass->name )) {
        /* name too long */
        wd26SetErr( err, ERR_NAME_TOO_LONG_WD26, name, NULL );
        return false;
    }; /* if */
    
    sapdbwa_SQLALLOCAT( sizeof(struct st_com_class),
                   (sapdbwa_UInt1**) &newComClass,
                   &allocatOK );
    if (allocatOK) {
        strcpy( newComClass->name, name );
        newComClass->id             = id;
        newComClass->initialized    = false;
        newComClass->pathPrefix[0]  = '\0';
        newComClass->classId[0]     = '\0';
        newComClass->log            = NULL;
        newComClass->objPool        = NULL;
    };

    return newComClass;
} /* wd96CreateComClass */


void wd96DestroyComClass( twd96ComClassP comClass )
{
    if (comClass->log != NULL) {
        wd25DestroyLog( comClass->log );
    }; /* if */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) comClass );
} /* wd96DestroyComClass */


sapdbwa_Bool wd96CallExitFunc( twd96ComClassP  comClass,
                             sapdbwa_Handle       wa,
                             sapdbwa_Int2     *exitRet,
                             twd25LogP       log,
                             twd26ErrP       err      )
{
    return true;
} /* wd96CallExitFunc */


sapdbwa_Bool wd96CallInitFunc( twd96ComClassP  comClass,
                             sapdbwa_Handle        wa,
                             sapdbwa_Int2     *initRet,
                             twd25LogP       log,
                             twd26ErrP       err )
{
    return true;
} /* wd96CallInitFunc */


sapdbwa_Bool wd96CallServiceFunc( twd96ComClassP   comClass,
                                sapdbwa_Handle         wa,
                                sapdbwa_HttpRequestP   request,
                                sapdbwa_HttpReplyP     reply,
                                sapdbwa_Int2      *svcRet,
                                twd25LogP        log,
                                twd26ErrP        err )
{
    IWebAgentCOM	*pWebAgentCOM = NULL;
    CWebAgentCB      wacb( wa, comClass );
    CHttpRequestCB   reqcb( request );
    CHttpReplyCB     repcb( reply );
    HRESULT		     result = S_OK;

    if(!wd96_ThisThreadIsCoInitialized()) {
        result = CoInitializeEx(NULL, THREADING_MODEL);
        wd96_MarkThisThreadAsCoInitialized();
    };

    if (SUCCEEDED(result)) {
        comClass->objPool->GetObj( &pWebAgentCOM );
        if (pWebAgentCOM != NULL) {
            pWebAgentCOM->ProcessRequest( &wacb, &reqcb, &repcb );
            comClass->objPool->ReleaseObj( pWebAgentCOM );
            *svcRet = sapdbwa_RTC_Accepted;
        } else {
            // error ...
            return false;
        };
    } else {
        // error ...
        return false;
    };

    return true;
} /* wd96CallServiceFunc */


sapdbwa_Bool wd96GetConf( twd96ComClassP  comClass,
                        char           *textBuffer,
                        sapdbwa_UInt4    *bufferLen )
{
    sapdbwa_Bool ok = true;
    
    ok = wd09AddString( textBuffer, "[", bufferLen )
        && wd09AddString( textBuffer, comClass->name, bufferLen )
        && wd09AddString( textBuffer, "]\n", bufferLen )
        && wd09AddString( textBuffer, "PathPrefix=", bufferLen )
        && wd09AddString( textBuffer, comClass->pathPrefix, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "ClassId=", bufferLen )
        && wd09AddString( textBuffer, comClass->classId, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "LogFile=", bufferLen )
        && (comClass->log == NULL 
            || (wd09AddString( textBuffer,
                               wd25GetFileName( comClass->log ),
                               bufferLen )))
        && wd09AddString( textBuffer, "\n", bufferLen );

    return ok;
} /* wd96GetConf */


char* wd96GetPathPrefix( twd96ComClassP comClass )
{
    return comClass->pathPrefix;
} /* wd96GetPathPrefix */


sapdbwa_UInt4 wd96GetComClassId( twd96ComClassP comClass )
{
    return comClass->id;
} /* wd96GetComClassId */


char* wd96GetComClassName( twd96ComClassP comClass )
{
    return comClass->name;
} /* wd96GetComClassName */


sapdbwa_Bool wd96InitComClass( twd96ComClassP     comClass,
                             char              *pathPrefix,
                             char              *classId,
                             char              *logFile,
                             twd26ErrP          err )
{
    UUID uuid;

    if ( strlen(pathPrefix) >= sizeof(comClass->pathPrefix)
         || strlen(classId) >= sizeof(comClass->classId)) {
        /* parameter too long */
        wd26SetErr( err, ERR_PARAM_TOO_LONG_WD26, comClass->name, NULL );

        return false;
    } else {
        if (logFile != NULL && strlen( logFile ) != 0) {
            comClass->log = wd25CreateLog( logFile );
            if (comClass->log == NULL) {
                wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );

                return false;
            }; /* if */
            if (UuidFromString( (unsigned char*) classId,
                                &uuid ) != RPC_S_OK) {
                // error ...
            };
            comClass->objPool = new CObjPool( uuid );
        }; /* if */
        strcpy( comClass->pathPrefix, pathPrefix );
        strcpy( comClass->classId, classId );
        comClass->initialized = true;
    }; /* else */        

    return true;
} /* wd96InitcomClass */


sapdbwa_Bool wd96WriteLogMsg( twd96ComClassP comClass, const char *msg )
{
    if (comClass->log != NULL) {
        wd25WriteLnLogMsg( comClass->log, (char*) msg );
        return true;
    } else {
        return false;
    }; /* else */
} /* wd96WriteLogMsg */


/*!**********************************************************************

  EndChapter: twd96ComClassP

************************************************************************/
/*!**********************************************************************

  Chapter: CWebAgentCB

************************************************************************/

CWebAgentCB::CWebAgentCB( sapdbwa_Handle  wa, twd96ComClassP comClass )
    : webAgent(wa), comClass(comClass), m_cRef(1)
{ };


ULONG __stdcall CWebAgentCB::AddRef()
{
    return ++m_cRef;
}; /* AddRef */


ULONG __stdcall CWebAgentCB::Release()
{
    if(--m_cRef != 0)
        return m_cRef;
    delete this;

    return 0;
}; /* Release */


HRESULT __stdcall CWebAgentCB::QueryInterface( REFIID riid, void ** ppv )
{
    if(riid == IID_IUnknown)
        *ppv = (IUnknown*)this;
    else if (riid == IID_IWACallBackObject)
        *ppv = (IWACallBackObject*) this;
    else if (riid == IID_IWebAgentCB)
        *ppv = (IWebAgentCB*) this;
    else {
        *ppv = NULL;

        return E_NOINTERFACE;
    };
    AddRef();

    return S_OK;
}; // QueryInterface


HRESULT __stdcall CWebAgentCB::GetErrMsg( BSTR  *errMsg )
{
    return wd96_MakeBSTR( sapdbwa_GetErrMsg( sapdbwa_HandleGetErr( this->webAgent )),
                          errMsg );
}; // GetErrMsg


HRESULT __stdcall CWebAgentCB::GetErrId( long *errId )
{
    *errId = sapdbwa_GetErrId( sapdbwa_HandleGetErr( this->webAgent ));

    return S_OK;
}; // GetErrId


HRESULT __stdcall CWebAgentCB::WriteLogMsg( BSTR msg )
{
    HRESULT  ret = S_OK;
    char    *charMsg = uxdup( msg );

    if ( charMsg == NULL || !wd96WriteLogMsg( this->comClass, charMsg )) {
        ret = E_FAIL;
    };
    if (charMsg != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) charMsg );
    };

    return ret;
}; // WriteLogMsg


HRESULT __stdcall CWebAgentCB::GetParam( BSTR   paramName,
                                         BSTR  *paramValue )
{
    HRESULT  ret = S_OK;

    char *charName = uxdup( paramName );
    char  charVal[MAX_INI_PAR_LEN_WD00];

    if (charName != NULL
        && wd20GetParam( wd96GetComClassName( this->comClass ),
                         charName,
                         charVal,
                         sizeof(charVal),
						 true )) {
        ret = wd96_MakeBSTR( charVal, paramValue );
    } else {
        ret = E_FAIL;
    };
    if (charName != NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) charName );
    };

    return ret;
}; // GetParam

/*!**********************************************************************

  EndChapter: CWebAgentCB

************************************************************************/
/*!**********************************************************************

  Chapter: CHttpRequestCB

************************************************************************/

CHttpRequestCB::CHttpRequestCB( sapdbwa_HttpRequestP  req )
    : m_cRef(1), m_req( req )
{ };


ULONG __stdcall CHttpRequestCB::AddRef()
{
    return ++m_cRef;
}; // AddRef


ULONG __stdcall CHttpRequestCB::Release()
{
    if(--m_cRef != 0)
        return m_cRef;
    delete this;

    return 0;
}; // Release


HRESULT __stdcall CHttpRequestCB::QueryInterface( REFIID riid, void ** ppv)
{
    if(riid == IID_IUnknown)
        *ppv = (IUnknown*)this;
    else if (riid == IID_IWACallBackObject)
        *ppv = (IWACallBackObject*) this;
    else if(riid == IID_IHttpRequestCB)
        *ppv = (IHttpRequestCB*)this;
    else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
    AddRef();
    return S_OK;
}; // QueryInterface


HRESULT __stdcall CHttpRequestCB::GetErrMsg( BSTR  *errMsg )
{
    return wd96_MakeBSTR( sapdbwa_GetErrMsg( sapdbwa_ReqGetErr( this->m_req )),
                          errMsg );
}; // GetErrMsg


HRESULT __stdcall CHttpRequestCB::GetErrId( long *errId )
{
    *errId = sapdbwa_GetErrId( sapdbwa_ReqGetErr( this->m_req ));

    return S_OK;
}; // GetErrId


HRESULT __stdcall CHttpRequestCB::GetHeader( BSTR   name, BSTR *value )
{
    HRESULT  ret = S_OK;
    char    *charName = uxdup( name );

    if (charName != NULL) {
        ret = wd96_MakeBSTR( sapdbwa_GetHeader( this->m_req, charName ), value );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) charName );
    } else {
        ret = E_FAIL;
    };

    return ret;
}; // GetHeader


HRESULT __stdcall CHttpRequestCB::GetRequestURI( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetRequestURI( this->m_req ), value );
}; // GetRequestURI


HRESULT __stdcall CHttpRequestCB::GetIfModifiedSince( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetIfModifiedSince( this->m_req ), value );
}; // GetIfModifiedSince


HRESULT __stdcall CHttpRequestCB::GetQueryString( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetQueryString( this->m_req ), value );
}; // GetQueryString


HRESULT __stdcall CHttpRequestCB::GetPathInfo( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetPathInfo( this->m_req ), value );
}; // GetPathInfo


HRESULT __stdcall CHttpRequestCB::GetMethod( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetMethod( this->m_req ), value );
}; // GetMethod


HRESULT __stdcall CHttpRequestCB::GetContentType( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetContentType( this->m_req ), value );
}; // GetContentType


HRESULT __stdcall CHttpRequestCB::GetContentLength( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetContentLength( this->m_req ), value );
}; // GetContentLength


HRESULT __stdcall CHttpRequestCB::GetPathTranslated( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetPathTranslated( this->m_req ), value );
}; // GetPathTranslated


HRESULT __stdcall CHttpRequestCB::GetServerName( BSTR *value )
{
    return wd96_MakeBSTR( sapdbwa_GetServerName( this->m_req ), value );
}; // GetServerName


HRESULT __stdcall CHttpRequestCB::GetCookieNames( SAFEARRAY  **ppNames )
{
    HRESULT       ret = S_OK;
    sapdbwa_StringSeqP strSeq = sapdbwa_CreateStringSeq();
    
    if (strSeq != NULL) {
        if (sapdbwa_GetCookieNames( this->m_req, strSeq )) {
            ret = wd96_MakeBSTRArray( strSeq, ppNames );
        } else {
            ret = E_FAIL;
        };
    } else {
        ret = E_FAIL;
    };

    return ret;
}; // GetCookieNames


HRESULT __stdcall
CHttpRequestCB::GetParameterNames( SAFEARRAY  **ppNames )
{
    HRESULT       ret = S_OK;
    sapdbwa_StringSeqP strSeq = sapdbwa_CreateStringSeq();
    
    if (strSeq != NULL) {
        if (sapdbwa_GetParameterNames( this->m_req, strSeq )) {
            ret = wd96_MakeBSTRArray( strSeq, ppNames );
        } else {
            ret = E_FAIL;
        };
        sapdbwa_DestroyStringSeq( strSeq );
    } else {
        ret = E_FAIL;
    };

    return ret;
}; // GetParameterNames


HRESULT __stdcall
CHttpRequestCB::GetParameterValues( BSTR          paramName,
                                    SAFEARRAY   **ppValues )
{
    HRESULT       ret = S_OK;
    sapdbwa_StringSeqP strSeq = sapdbwa_CreateStringSeq();
    
    if (strSeq == NULL) {
        return E_FAIL;
    };

    char *charName = uxdup( paramName );
    if (charName == NULL) {
        sapdbwa_DestroyStringSeq( strSeq );

        return E_FAIL;
    };

    if (sapdbwa_GetParameterValues( this->m_req, charName, strSeq )) {
        ret = wd96_MakeBSTRArray( strSeq, ppValues );
    } else {
        ret = E_FAIL;
    };
    sapdbwa_DestroyStringSeq( strSeq );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) charName );

    return ret;
}; // GetParameterValues

HRESULT __stdcall CHttpRequestCB::GetReqBody(SAFEARRAY **Buf)
{
#if 0
    // write the request body into the request callback object buffer :
    while (0 != (l = m_req->ReadBody(m_req, reqb, sizeof(reqb))))
        {
            append_buf2sfarry(reqb, *Buf, l);
        };
#endif
    return S_OK;
};

/*!**********************************************************************

  EndChapter: CHttpRequestCB

************************************************************************/
/*!**********************************************************************

  Chapter: CHttpReplyCB

************************************************************************/

CHttpReplyCB::CHttpReplyCB( sapdbwa_HttpReplyP reply )
    : httpReply( reply ), repBuf( NULL ), repBuflen( 0 ), m_cRef( 1 )
{ };


CHttpReplyCB::~CHttpReplyCB()
{
    if(this->repBuf != NULL)
        free(this->repBuf);
}; // ~CHttpReplyCB


ULONG __stdcall CHttpReplyCB::AddRef()
{
    return ++m_cRef;
}; // AddRef


ULONG __stdcall CHttpReplyCB::Release()
{
    if(--m_cRef != 0)
        return m_cRef;
    delete this;

    return 0;
}; // Release


HRESULT __stdcall CHttpReplyCB::QueryInterface(REFIID riid, void ** ppv)
{
    if(riid == IID_IUnknown)
        *ppv = (IUnknown*)this;
    else if (riid == IID_IWACallBackObject)
        *ppv = (IWACallBackObject*) this;
    else if(riid == IID_IHttpReplyCB)
        *ppv = (IHttpReplyCB*)this;
    else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    AddRef();

    return S_OK;
}; // QueryInterface



HRESULT __stdcall CHttpReplyCB::GetErrMsg( BSTR  *errMsg )
{
    return wd96_MakeBSTR( sapdbwa_GetErrMsg( sapdbwa_RepGetErr( this->httpReply )),
                          errMsg );
}; // GetErrMsg


HRESULT __stdcall CHttpReplyCB::GetErrId( long *errId )
{
    *errId = sapdbwa_GetErrId( sapdbwa_RepGetErr( this->httpReply ));

    return S_OK;
}; // GetErrId


HRESULT __stdcall CHttpReplyCB::InitHeader( short status,
                                            BSTR  contentType,
                                            BSTR  contentLength,
                                            BSTR  lastModified,
                                            BSTR  expires,
                                            BSTR  location )
{
    HRESULT ret = S_OK;
    char *cContentType   = uxdup( contentType );
    char *cContentLength = uxdup( contentLength );
    char *cLastModified  = uxdup( lastModified );
    char *cExpires       = uxdup( expires );
    char *cLocation      = uxdup( location );

    if (cContentType && cContentLength  && cLastModified
        && cExpires && cLocation) {
        if (!sapdbwa_InitHeader( this->httpReply,
                           status,
                           cContentType,
                           cContentLength,
                           cLastModified,
                           cExpires,
                           cLocation )) {
            ret = E_FAIL;
        };
    } else {
        ret = E_FAIL;
    };
    if (cContentType) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cContentType );
    };
    if (cContentLength) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cContentLength);
    };
    if (cLastModified) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cLastModified);
    };
    if (cExpires) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cExpires);
    };
    if (cLocation) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cLocation);
    };

    return ret;
}; // InitHeader


HRESULT __stdcall CHttpReplyCB::SetHeader( BSTR  headerName,
                                           BSTR  headerValue )
{
    HRESULT ret = S_OK;
    char *cHeaderName  = uxdup( headerName );
    char *cHeaderValue = uxdup( headerValue );

    if (cHeaderName && cHeaderValue) {
        if (!sapdbwa_SetHeader( this->httpReply, cHeaderName, cHeaderValue )) {
            ret = E_FAIL;
        };
    };
    if (cHeaderName) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cHeaderName );
    };
    if (cHeaderValue) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cHeaderValue );
    };

    return ret;
}; // SetHeader


HRESULT __stdcall CHttpReplyCB::SetCookie( BSTR         name,
                                           BSTR         value,
                                           BSTR         expires,
                                           BSTR         path,
                                           BSTR         domain,
                                           short        secure )
{
    HRESULT ret = S_OK;
    char *cName    = uxdup( name );
    char *cValue   = uxdup( value );
    char *cExpires = uxdup( expires );
    char *cPath    = uxdup( path );
    char *cDomain  = uxdup( domain );

    if (cName && cValue && cExpires && cPath && cDomain) {
        if (!sapdbwa_SetCookie( this->httpReply,
                           cName, cValue, cExpires, cPath, cDomain,
                           (secure == 0))) {
            ret = E_FAIL;
        };
    };
    if (cName) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cName);
    };
    if (cValue) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cValue);
    };
    if (cExpires) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cExpires);
    };
    if (cPath) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cPath);
    };
    if (cDomain) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cDomain);
    };

    return ret;
}; // SetCookie


HRESULT __stdcall CHttpReplyCB::SendHeader()
{
    if (sapdbwa_SendHeader( this->httpReply )) {
        return S_OK;
    } else {
        return E_FAIL;
    };
}; // SendHeader


HRESULT __stdcall CHttpReplyCB::SendBody( BSTR  bodyData )
{
    HRESULT ret = S_OK;
    char *cBodyData = uxdup( bodyData );

    if (cBodyData) {
        if (!sapdbwa_SendBody( this->httpReply, cBodyData, 0 )) {
            ret = E_FAIL;
        };
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) cBodyData );
    } else {
        ret = E_FAIL;
    };

    return ret;
}; // SendBody


/*!**********************************************************************

  EndChapter: CHttpReplyCB

************************************************************************/

// all dynamic arrays are ZERO BASED,
// however VB does not allow to create an array with no entries.
// (as in prehistoric times, before the notion of zero as a number
// has been adopted)
// so we have to establish our own conventions :
//
//for an array A :
// Lbound(A) should always equal ZERO .
// we regard A as empty iff Ubound(A) == 0 .
//as a consequence we never access A(0), and Ubound(A) is the number
//of entries of A .
//
//we consider our arrays to hold binary data.
//care has to be taken if we interpret this binary data as
//null terminated strings (i.e. the HTTP request and reply header fields) .
//the binary data of such a string _includes_ the null character .
//(for example if the array A holds an empty string, the array is _not_
// empty, in this case we have Ubound(A) == 1 , A(1) == 0 , and
// A(0) is undefined.)

/*!**********************************************************************

  Chapter: CObjPool

************************************************************************/

// public :
CObjPool::CObjPool( UUID classId )
    : freelistCount(0), uuid( classId )
{
    wd27InitExcl( &this->objPoolExcl );
};


CObjPool::~CObjPool()
{
    sapdbwa_UInt4 i;

    if (this->freelistCount > 0) {
        CoInitializeEx( NULL, COINIT_MULTITHREADED);
    }
    for ( i = 0; i < this->freelistCount; i++) {
        if (CheckObj(freelist[i])) {
            (freelist[i])->Release();
        }
    }
    if (this->freelistCount > 0) {
        CoUninitialize();
    }
    wd27ExitExcl( this->objPoolExcl );
}; // ~CObjPool


void CObjPool::GetObj(IWebAgentCOM **ppWebAgentCOM )
{
    wd27BegExcl( this->objPoolExcl );
    while(this->freelistCount > 0) {
        if(CheckObj(freelist[this->freelistCount - 1])) {
            *ppWebAgentCOM = freelist[this->freelistCount - 1];
            break;
        } else {
            this->freelistCount--;
        }
    }
    if(this->freelistCount == 0) {
        MakeObj(ppWebAgentCOM);
    } else {
        this->freelistCount--;
    }
    wd27EndExcl( this->objPoolExcl );
}; // GetObj


void CObjPool::ReleaseObj(IWebAgentCOM *pWebAgentCOM)
{
    wd27BegExcl( this->objPoolExcl );
    if(CheckObj(pWebAgentCOM)) {
        if(freelistCount < MAX_FREELIST_COUNT) {
            freelist[freelistCount++] = pWebAgentCOM;
        } else {
            pWebAgentCOM->Release();
        }
    }
    wd27EndExcl( this->objPoolExcl );
}; // ReleaseObj


// private :

bool CObjPool::CheckObj( IWebAgentCOM *pWebAgentCOM )
{
    void     *pDummy = NULL;
    bool      myresult;
    HRESULT   result;

    if(pWebAgentCOM == NULL) {
        myresult = false;
    }
    else {
        try {
            result = pWebAgentCOM->QueryInterface(IID_IWebAgentCOM, 
                                                  &pDummy);
        }
        catch(...) {
            result = S_OK + 1;
        }
        if(result != S_OK)
            myresult = false;
        else
            myresult = true;
    }

    return myresult;
}; // CheckObj


bool CObjPool::MakeObj(IWebAgentCOM **ppWebAgentCOM )
{
    IUnknown       *pUnk = NULL;
    HRESULT			result;
    COSERVERINFO	ServerInfo;
    COAUTHINFO		AuthInfo;
    COAUTHIDENTITY	AuthIdent;
    MULTI_QI		pResults[1];
		 
    // this template/macro voodoo instantiated the COM server object
    // but only inproc :
    // (for this to work the VB server dll "WebAgentCOMsrvr.dll"
    //  must be '#import'ed.)
    //_COM_SMARTPTR_TYPEDEF(IWebAgentCOM, __uuidof(IWebAgentCOM));
    //IWebAgentCOMPtr pWebAgentCOM(__uuidof(CWebAgentCOM));
 
    // we do it traditionally because we even want to create
    // out-of-proc servers :
    AuthIdent.Domain = (unsigned short*)AUTHIDENT_DOMAIN;
    AuthIdent.DomainLength = (ULONG)strlen(AUTHIDENT_DOMAIN);
    AuthIdent.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    AuthIdent.Password = (unsigned short *)AUTHIDENT_PASSWORD;
    AuthIdent.PasswordLength = (ULONG)strlen(AUTHIDENT_PASSWORD);
    AuthIdent.User = (unsigned short *)AUTHIDENT_USER;
    AuthIdent.UserLength = (ULONG)strlen(AUTHIDENT_USER);

    AuthInfo.dwAuthnLevel = RPC_C_AUTHN_LEVEL_DEFAULT;
    AuthInfo.dwAuthnSvc = RPC_C_AUTHN_WINNT;
    AuthInfo.dwAuthzSvc = RPC_C_AUTHZ_NONE;
    AuthInfo.dwCapabilities = EOAC_NONE;
    AuthInfo.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
    AuthInfo.pAuthIdentityData = &AuthIdent;
    AuthInfo.pwszServerPrincName = NULL;


			
    ServerInfo.dwReserved1 = 0;
    ServerInfo.dwReserved2 = 0;
    //			ServerInfo.pAuthInfo = &AuthInfo;
    ServerInfo.pAuthInfo = NULL;
    ServerInfo.pwszName = REMOTE_SERVERNAME;

    pResults[0].hr = 0;
    pResults[0].pIID = &IID_IUnknown;
    pResults[0].pItf = NULL;

    result = CoCreateInstanceEx( this->uuid,
                                 NULL, 
                                 SERVERTYPE,
                                 SERVERINFO,
                                 1,
                                 pResults);
    pUnk = pResults[0].pItf;

    if(result != S_OK) {
        // error ...
    }

    if(SUCCEEDED(result)) {            
        result = pUnk->QueryInterface(IID_IWebAgentCOM, 
                                      (void **)ppWebAgentCOM);
        pUnk->Release();
            
        if(!SUCCEEDED(result)) {
            // error ...
        }
    }; /* if */

    return (SUCCEEDED(result));
}; // MakeObj

/*!**********************************************************************

  EndChapter: CObjPool

************************************************************************/
/*!**********************************************************************

  Chapter: internal helper functions

************************************************************************/

bool wd96_ThisThreadIsCoInitialized(void)
{
    bool *isitreally;

    isitreally = (bool*) sqlgettls( wd96TlsIndexHandle );
	if(isitreally != NULL)
		return *isitreally;
	else
		return false;
} // wd96_ThisThreadIsCoInitialized
 

void wd96_MarkThisThreadAsCoInitialized(void)
{
	sapdbwa_Bool      *pIsCoInitialized;
	sapdbwa_Bool		 allocatOk;
	tsp00_ErrTextc   errtext;
	teo07_ThreadErr  err = THR_OK_EO07;

	errtext[0] = 0;

    sapdbwa_SQLALLOCAT( sizeof(bool),
                   (sapdbwa_UInt1**) &pIsCoInitialized,
                   &allocatOk );
	*pIsCoInitialized = true;
	sqlsettls( wd96TlsIndexHandle, pIsCoInitialized, errtext, &err);
} // wd96_MarkThisThreadAsCoInitialized


HRESULT wd96_MakeBSTR( const char *strIn, BSTR *bstrOut )
{
    HRESULT  ret = S_OK;
    wchar_t *wcharStr = NULL;

    if (strIn) {
        wcharStr = uxdup( strIn );
    } else {
        wcharStr = uxdup( "" );
    };        
    if ( wcharStr != NULL ) {
        *bstrOut = SysAllocString( &wcharStr[0] );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) wcharStr );
    } else {
        ret = E_FAIL;
    };

    return ret;
} // wd96_MakeBSTR


HRESULT wd96_MakeBSTRArray( sapdbwa_StringSeqP strSeq, SAFEARRAY **array )
{
    HRESULT      ret = S_OK;
    BSTR        *value = NULL;
    sapdbwa_UInt4  numElem = sapdbwa_GetNumElem( strSeq );
            
    *array = SafeArrayCreateVector( VT_BSTR,
                                    0,
                                    numElem);
    if (*array != NULL) {
        ret = SafeArrayAccessData( *array, (void**) &value );
        if (SUCCEEDED(ret)) {
            for ( sapdbwa_UInt4 i = 0;
                  SUCCEEDED(ret) && i < numElem;
                  i++ ) {
                ret = wd96_MakeBSTR( sapdbwa_GetStringByIndex( strSeq, i ),
                                     &value[i] );
            }; // for
            SafeArrayUnaccessData( *array );
        };
    } else {
        ret = E_FAIL;
    };

    return ret;
} // wd96_MakeBSTRArray

/*!**********************************************************************

  EndChapter: internal helper functions

************************************************************************/

