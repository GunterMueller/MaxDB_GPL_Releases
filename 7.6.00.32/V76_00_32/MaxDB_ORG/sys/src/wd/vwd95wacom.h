/* this ALWAYS GENERATED file contains the definitions for the interfaces 


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




*/


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Jan 27 09:56:42 2000
 */
/* Compiler settings for vwd95wacom.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifndef __vwd95wacom_h__
#define __vwd95wacom_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IWebAgentCOM_FWD_DEFINED__
#define __IWebAgentCOM_FWD_DEFINED__
typedef interface IWebAgentCOM IWebAgentCOM;
#endif 	/* __IWebAgentCOM_FWD_DEFINED__ */


#ifndef __IWACallBackObject_FWD_DEFINED__
#define __IWACallBackObject_FWD_DEFINED__
typedef interface IWACallBackObject IWACallBackObject;
#endif 	/* __IWACallBackObject_FWD_DEFINED__ */


#ifndef __IWebAgentCB_FWD_DEFINED__
#define __IWebAgentCB_FWD_DEFINED__
typedef interface IWebAgentCB IWebAgentCB;
#endif 	/* __IWebAgentCB_FWD_DEFINED__ */


#ifndef __IHttpRequestCB_FWD_DEFINED__
#define __IHttpRequestCB_FWD_DEFINED__
typedef interface IHttpRequestCB IHttpRequestCB;
#endif 	/* __IHttpRequestCB_FWD_DEFINED__ */


#ifndef __IHttpReplyCB_FWD_DEFINED__
#define __IHttpReplyCB_FWD_DEFINED__
typedef interface IHttpReplyCB IHttpReplyCB;
#endif 	/* __IHttpReplyCB_FWD_DEFINED__ */


#ifndef __CWebAgentCB_FWD_DEFINED__
#define __CWebAgentCB_FWD_DEFINED__

#ifdef __cplusplus
typedef class CWebAgentCB CWebAgentCB;
#else
typedef struct CWebAgentCB CWebAgentCB;
#endif /* __cplusplus */

#endif 	/* __CWebAgentCB_FWD_DEFINED__ */


#ifndef __CHttpRequestCB_FWD_DEFINED__
#define __CHttpRequestCB_FWD_DEFINED__

#ifdef __cplusplus
typedef class CHttpRequestCB CHttpRequestCB;
#else
typedef struct CHttpRequestCB CHttpRequestCB;
#endif /* __cplusplus */

#endif 	/* __CHttpRequestCB_FWD_DEFINED__ */


#ifndef __CHttpReplyCB_FWD_DEFINED__
#define __CHttpReplyCB_FWD_DEFINED__

#ifdef __cplusplus
typedef class CHttpReplyCB CHttpReplyCB;
#else
typedef struct CHttpReplyCB CHttpReplyCB;
#endif /* __cplusplus */

#endif 	/* __CHttpReplyCB_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __WebAgentCOM_LIBRARY_DEFINED__
#define __WebAgentCOM_LIBRARY_DEFINED__

/* library WebAgentCOM */
/* [version][uuid] */ 










EXTERN_C const IID LIBID_WebAgentCOM;

#ifndef __IWebAgentCOM_INTERFACE_DEFINED__
#define __IWebAgentCOM_INTERFACE_DEFINED__

/* interface IWebAgentCOM */
/* [object][oleautomation][nonextensible][helpstring][dual][hidden][version][uuid] */ 


EXTERN_C const IID IID_IWebAgentCOM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("888c87f0-bec7-11d3-a9c1-006094b91c8b")
    IWebAgentCOM : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ProcessRequest( 
            /* [in] */ IWebAgentCB __RPC_FAR *pIWebAgentCB,
            /* [in] */ IHttpRequestCB __RPC_FAR *pIReqCB,
            /* [in] */ IHttpReplyCB __RPC_FAR *pIRepCB) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebAgentCOMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWebAgentCOM __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWebAgentCOM __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWebAgentCOM __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProcessRequest )( 
            IWebAgentCOM __RPC_FAR * This,
            /* [in] */ IWebAgentCB __RPC_FAR *pIWebAgentCB,
            /* [in] */ IHttpRequestCB __RPC_FAR *pIReqCB,
            /* [in] */ IHttpReplyCB __RPC_FAR *pIRepCB);
        
        END_INTERFACE
    } IWebAgentCOMVtbl;

    interface IWebAgentCOM
    {
        CONST_VTBL struct IWebAgentCOMVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebAgentCOM_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebAgentCOM_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebAgentCOM_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebAgentCOM_ProcessRequest(This,pIWebAgentCB,pIReqCB,pIRepCB)	\
    (This)->lpVtbl -> ProcessRequest(This,pIWebAgentCB,pIReqCB,pIRepCB)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IWebAgentCOM_ProcessRequest_Proxy( 
    IWebAgentCOM __RPC_FAR * This,
    /* [in] */ IWebAgentCB __RPC_FAR *pIWebAgentCB,
    /* [in] */ IHttpRequestCB __RPC_FAR *pIReqCB,
    /* [in] */ IHttpReplyCB __RPC_FAR *pIRepCB);


void __RPC_STUB IWebAgentCOM_ProcessRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebAgentCOM_INTERFACE_DEFINED__ */


#ifndef __IWACallBackObject_INTERFACE_DEFINED__
#define __IWACallBackObject_INTERFACE_DEFINED__

/* interface IWACallBackObject */
/* [object][oleautomation][nonextensible][helpstring][dual][hidden][version][uuid] */ 


EXTERN_C const IID IID_IWACallBackObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c8b2a830-c841-11d3-a9c2-006094b91c8b")
    IWACallBackObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetErrMsg( 
            /* [out] */ BSTR __RPC_FAR *errMsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrId( 
            /* [out] */ long __RPC_FAR *errId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWACallBackObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWACallBackObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWACallBackObject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWACallBackObject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrMsg )( 
            IWACallBackObject __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *errMsg);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrId )( 
            IWACallBackObject __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *errId);
        
        END_INTERFACE
    } IWACallBackObjectVtbl;

    interface IWACallBackObject
    {
        CONST_VTBL struct IWACallBackObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWACallBackObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWACallBackObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWACallBackObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWACallBackObject_GetErrMsg(This,errMsg)	\
    (This)->lpVtbl -> GetErrMsg(This,errMsg)

#define IWACallBackObject_GetErrId(This,errId)	\
    (This)->lpVtbl -> GetErrId(This,errId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWACallBackObject_GetErrMsg_Proxy( 
    IWACallBackObject __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *errMsg);


void __RPC_STUB IWACallBackObject_GetErrMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWACallBackObject_GetErrId_Proxy( 
    IWACallBackObject __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *errId);


void __RPC_STUB IWACallBackObject_GetErrId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWACallBackObject_INTERFACE_DEFINED__ */


#ifndef __IWebAgentCB_INTERFACE_DEFINED__
#define __IWebAgentCB_INTERFACE_DEFINED__

/* interface IWebAgentCB */
/* [object][oleautomation][nonextensible][helpstring][dual][hidden][version][uuid] */ 


EXTERN_C const IID IID_IWebAgentCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8bea5db0-bec7-11d3-a9c1-006094b91c8b")
    IWebAgentCB : public IWACallBackObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParam( 
            /* [in] */ BSTR paramName,
            /* [out] */ BSTR __RPC_FAR *paramValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteLogMsg( 
            /* [in] */ BSTR msg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebAgentCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWebAgentCB __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWebAgentCB __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWebAgentCB __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrMsg )( 
            IWebAgentCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *errMsg);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrId )( 
            IWebAgentCB __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *errId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParam )( 
            IWebAgentCB __RPC_FAR * This,
            /* [in] */ BSTR paramName,
            /* [out] */ BSTR __RPC_FAR *paramValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteLogMsg )( 
            IWebAgentCB __RPC_FAR * This,
            /* [in] */ BSTR msg);
        
        END_INTERFACE
    } IWebAgentCBVtbl;

    interface IWebAgentCB
    {
        CONST_VTBL struct IWebAgentCBVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebAgentCB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebAgentCB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebAgentCB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebAgentCB_GetErrMsg(This,errMsg)	\
    (This)->lpVtbl -> GetErrMsg(This,errMsg)

#define IWebAgentCB_GetErrId(This,errId)	\
    (This)->lpVtbl -> GetErrId(This,errId)


#define IWebAgentCB_GetParam(This,paramName,paramValue)	\
    (This)->lpVtbl -> GetParam(This,paramName,paramValue)

#define IWebAgentCB_WriteLogMsg(This,msg)	\
    (This)->lpVtbl -> WriteLogMsg(This,msg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWebAgentCB_GetParam_Proxy( 
    IWebAgentCB __RPC_FAR * This,
    /* [in] */ BSTR paramName,
    /* [out] */ BSTR __RPC_FAR *paramValue);


void __RPC_STUB IWebAgentCB_GetParam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWebAgentCB_WriteLogMsg_Proxy( 
    IWebAgentCB __RPC_FAR * This,
    /* [in] */ BSTR msg);


void __RPC_STUB IWebAgentCB_WriteLogMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebAgentCB_INTERFACE_DEFINED__ */


#ifndef __IHttpRequestCB_INTERFACE_DEFINED__
#define __IHttpRequestCB_INTERFACE_DEFINED__

/* interface IHttpRequestCB */
/* [object][oleautomation][nonextensible][helpstring][dual][hidden][version][uuid] */ 


EXTERN_C const IID IID_IHttpRequestCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d7403720-bec7-11d3-a9c1-006094b91c8b")
    IHttpRequestCB : public IWACallBackObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHeader( 
            /* [in] */ BSTR name,
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRequestURI( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIfModifiedSince( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetQueryString( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPathInfo( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethod( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContentType( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContentLength( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPathTranslated( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetServerName( 
            /* [out] */ BSTR __RPC_FAR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCookieNames( 
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParameterNames( 
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParameterValues( 
            /* [in] */ BSTR paramName,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *values) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHttpRequestCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHttpRequestCB __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHttpRequestCB __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrMsg )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *errMsg);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrId )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *errId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHeader )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRequestURI )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIfModifiedSince )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetQueryString )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPathInfo )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMethod )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContentType )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContentLength )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPathTranslated )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetServerName )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCookieNames )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParameterNames )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParameterValues )( 
            IHttpRequestCB __RPC_FAR * This,
            /* [in] */ BSTR paramName,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *values);
        
        END_INTERFACE
    } IHttpRequestCBVtbl;

    interface IHttpRequestCB
    {
        CONST_VTBL struct IHttpRequestCBVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpRequestCB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHttpRequestCB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHttpRequestCB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHttpRequestCB_GetErrMsg(This,errMsg)	\
    (This)->lpVtbl -> GetErrMsg(This,errMsg)

#define IHttpRequestCB_GetErrId(This,errId)	\
    (This)->lpVtbl -> GetErrId(This,errId)


#define IHttpRequestCB_GetHeader(This,name,value)	\
    (This)->lpVtbl -> GetHeader(This,name,value)

#define IHttpRequestCB_GetRequestURI(This,value)	\
    (This)->lpVtbl -> GetRequestURI(This,value)

#define IHttpRequestCB_GetIfModifiedSince(This,value)	\
    (This)->lpVtbl -> GetIfModifiedSince(This,value)

#define IHttpRequestCB_GetQueryString(This,value)	\
    (This)->lpVtbl -> GetQueryString(This,value)

#define IHttpRequestCB_GetPathInfo(This,value)	\
    (This)->lpVtbl -> GetPathInfo(This,value)

#define IHttpRequestCB_GetMethod(This,value)	\
    (This)->lpVtbl -> GetMethod(This,value)

#define IHttpRequestCB_GetContentType(This,value)	\
    (This)->lpVtbl -> GetContentType(This,value)

#define IHttpRequestCB_GetContentLength(This,value)	\
    (This)->lpVtbl -> GetContentLength(This,value)

#define IHttpRequestCB_GetPathTranslated(This,value)	\
    (This)->lpVtbl -> GetPathTranslated(This,value)

#define IHttpRequestCB_GetServerName(This,value)	\
    (This)->lpVtbl -> GetServerName(This,value)

#define IHttpRequestCB_GetCookieNames(This,names)	\
    (This)->lpVtbl -> GetCookieNames(This,names)

#define IHttpRequestCB_GetParameterNames(This,names)	\
    (This)->lpVtbl -> GetParameterNames(This,names)

#define IHttpRequestCB_GetParameterValues(This,paramName,values)	\
    (This)->lpVtbl -> GetParameterValues(This,paramName,values)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetHeader_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetRequestURI_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetRequestURI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetIfModifiedSince_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetIfModifiedSince_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetQueryString_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetQueryString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetPathInfo_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetPathInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetMethod_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetContentType_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetContentType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetContentLength_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetContentLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetPathTranslated_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetPathTranslated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetServerName_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *value);


void __RPC_STUB IHttpRequestCB_GetServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetCookieNames_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names);


void __RPC_STUB IHttpRequestCB_GetCookieNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetParameterNames_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *names);


void __RPC_STUB IHttpRequestCB_GetParameterNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpRequestCB_GetParameterValues_Proxy( 
    IHttpRequestCB __RPC_FAR * This,
    /* [in] */ BSTR paramName,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *values);


void __RPC_STUB IHttpRequestCB_GetParameterValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHttpRequestCB_INTERFACE_DEFINED__ */


#ifndef __IHttpReplyCB_INTERFACE_DEFINED__
#define __IHttpReplyCB_INTERFACE_DEFINED__

/* interface IHttpReplyCB */
/* [object][oleautomation][nonextensible][helpstring][dual][hidden][version][uuid] */ 


EXTERN_C const IID IID_IHttpReplyCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4fb4cae0-bec8-11d3-a9c1-006094b91c8b")
    IHttpReplyCB : public IWACallBackObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitHeader( 
            /* [in] */ short status,
            /* [in] */ BSTR contentType,
            /* [in] */ BSTR contentLength,
            /* [in] */ BSTR lastModified,
            /* [in] */ BSTR expires,
            /* [in] */ BSTR location) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeader( 
            /* [in] */ BSTR headerName,
            /* [in] */ BSTR headerValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCookie( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR value,
            /* [in] */ BSTR expires,
            /* [in] */ BSTR path,
            /* [in] */ BSTR domain,
            /* [in] */ VARIANT_BOOL secure) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendHeader( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendBody( 
            /* [in] */ BSTR bodyData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHttpReplyCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHttpReplyCB __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHttpReplyCB __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrMsg )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *errMsg);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrId )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *errId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitHeader )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [in] */ short status,
            /* [in] */ BSTR contentType,
            /* [in] */ BSTR contentLength,
            /* [in] */ BSTR lastModified,
            /* [in] */ BSTR expires,
            /* [in] */ BSTR location);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHeader )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [in] */ BSTR headerName,
            /* [in] */ BSTR headerValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetCookie )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR value,
            /* [in] */ BSTR expires,
            /* [in] */ BSTR path,
            /* [in] */ BSTR domain,
            /* [in] */ VARIANT_BOOL secure);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendHeader )( 
            IHttpReplyCB __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendBody )( 
            IHttpReplyCB __RPC_FAR * This,
            /* [in] */ BSTR bodyData);
        
        END_INTERFACE
    } IHttpReplyCBVtbl;

    interface IHttpReplyCB
    {
        CONST_VTBL struct IHttpReplyCBVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpReplyCB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHttpReplyCB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHttpReplyCB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHttpReplyCB_GetErrMsg(This,errMsg)	\
    (This)->lpVtbl -> GetErrMsg(This,errMsg)

#define IHttpReplyCB_GetErrId(This,errId)	\
    (This)->lpVtbl -> GetErrId(This,errId)


#define IHttpReplyCB_InitHeader(This,status,contentType,contentLength,lastModified,expires,location)	\
    (This)->lpVtbl -> InitHeader(This,status,contentType,contentLength,lastModified,expires,location)

#define IHttpReplyCB_SetHeader(This,headerName,headerValue)	\
    (This)->lpVtbl -> SetHeader(This,headerName,headerValue)

#define IHttpReplyCB_SetCookie(This,name,value,expires,path,domain,secure)	\
    (This)->lpVtbl -> SetCookie(This,name,value,expires,path,domain,secure)

#define IHttpReplyCB_SendHeader(This)	\
    (This)->lpVtbl -> SendHeader(This)

#define IHttpReplyCB_SendBody(This,bodyData)	\
    (This)->lpVtbl -> SendBody(This,bodyData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHttpReplyCB_InitHeader_Proxy( 
    IHttpReplyCB __RPC_FAR * This,
    /* [in] */ short status,
    /* [in] */ BSTR contentType,
    /* [in] */ BSTR contentLength,
    /* [in] */ BSTR lastModified,
    /* [in] */ BSTR expires,
    /* [in] */ BSTR location);


void __RPC_STUB IHttpReplyCB_InitHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpReplyCB_SetHeader_Proxy( 
    IHttpReplyCB __RPC_FAR * This,
    /* [in] */ BSTR headerName,
    /* [in] */ BSTR headerValue);


void __RPC_STUB IHttpReplyCB_SetHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpReplyCB_SetCookie_Proxy( 
    IHttpReplyCB __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [in] */ BSTR value,
    /* [in] */ BSTR expires,
    /* [in] */ BSTR path,
    /* [in] */ BSTR domain,
    /* [in] */ VARIANT_BOOL secure);


void __RPC_STUB IHttpReplyCB_SetCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpReplyCB_SendHeader_Proxy( 
    IHttpReplyCB __RPC_FAR * This);


void __RPC_STUB IHttpReplyCB_SendHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpReplyCB_SendBody_Proxy( 
    IHttpReplyCB __RPC_FAR * This,
    /* [in] */ BSTR bodyData);


void __RPC_STUB IHttpReplyCB_SendBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHttpReplyCB_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CWebAgentCB;

#ifdef __cplusplus

class DECLSPEC_UUID("cf2824e0-bec7-11d3-a9c1-006094b91c8b")
CWebAgentCB;
#endif

EXTERN_C const CLSID CLSID_CHttpRequestCB;

#ifdef __cplusplus

class DECLSPEC_UUID("de6e7db0-bec7-11d3-a9c1-006094b91c8b")
CHttpRequestCB;
#endif

EXTERN_C const CLSID CLSID_CHttpReplyCB;

#ifdef __cplusplus

class DECLSPEC_UUID("e5c78d80-bec7-11d3-a9c1-006094b91c8b")
CHttpReplyCB;
#endif
#endif /* __WebAgentCOM_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
