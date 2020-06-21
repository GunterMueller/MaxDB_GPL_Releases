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


//
//   FreeBSD portions added by Kai Mosebach, 
//   For more informations see : http://www.komadev.de/sapdb
//
//
//
    
*/


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Mar 17 16:24:13 2000
 */
/* Compiler settings for sphsapdb.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif
#ifndef FREEBSD
#include <rpc.h>
#include <rpcndr.h>
#else
#include <rpc/rpc.h>
//#include <rpc/rpcndr.h>
#endif

#ifndef __sphsapdb_h__
#define __sphsapdb_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __Isphsapdb_FWD_DEFINED__
#define __Isphsapdb_FWD_DEFINED__
typedef struct Isphsapdb Isphsapdb;
#endif 	/* __Isphsapdb_FWD_DEFINED__ */


#ifndef __sphsapdb_FWD_DEFINED__
#define __sphsapdb_FWD_DEFINED__

#ifdef __cplusplus
typedef class sphsapdb sphsapdb;
#else
typedef struct sphsapdb sphsapdb;
#endif /* __cplusplus */

#endif 	/* __sphsapdb_FWD_DEFINED__ */


/* header files for imported files */
#ifndef FREEBSD
#include <oaidl.h>
#endif

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __SPHSAPDBLIB_LIBRARY_DEFINED__
#define __SPHSAPDBLIB_LIBRARY_DEFINED__

/* library SPHSAPDBLIB */
/* [helpstring][version][uuid] */ 

/*#if defined(WIN32)     PTS 1115508

#pragma pack(push, UDT, 4)
#endif*/
#define	LiveCacheApplVersion	( "V72" )

typedef /* [unique][helpstring][uuid] */ unsigned char __RPC_FAR TABLENAME[ 31 ];

typedef /* [public][uuid] */ unsigned char __RPC_FAR SVER[ 2 ];


EXTERN_C const IID LIBID_SPHSAPDBLIB;

#ifndef __Isphsapdb_INTERFACE_DEFINED__
#define __Isphsapdb_INTERFACE_DEFINED__

/* interface Isphsapdb */
/* [object][public][uuid] */ 


EXTERN_C const IID IID_Isphsapdb;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39C0C052-91E8-11d3-A469-0008C71E4305")
    Isphsapdb : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE test( 
            /* [in] */ int a) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE sphn( 
            /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
            /* [in] */ TABLENAME __RPC_FAR *hiertab,
            /* [in] */ OmsTypeInt4 act_level,
            /* [in] */ OmsTypeInt4 end_level,
            /* [in] */ SVER __RPC_FAR *sver_in,
            /* [in] */ OmsTypeInt4 hiesid_in,
            /* [in] */ OmsTypeInt4 start_succ,
            /* [in] */ OmsTypeInt4 start_is_succ) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE sphl( 
            /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
            /* [in] */ TABLENAME __RPC_FAR *hiertab,
            /* [in] */ OmsTypeInt4 act_level,
            /* [in] */ OmsTypeInt4 end_level1,
            /* [in] */ OmsTypeInt4 end_level2,
            /* [in] */ SVER __RPC_FAR *sver_in,
            /* [in] */ OmsTypeInt4 hiesid_in,
            /* [in] */ OmsTypeInt4 start_succ) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IsphsapdbVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Isphsapdb __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Isphsapdb __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Isphsapdb __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *test )( 
            Isphsapdb __RPC_FAR * This,
            /* [in] */ int a);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *sphn )( 
            Isphsapdb __RPC_FAR * This,
            /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
            /* [in] */ TABLENAME __RPC_FAR *hiertab,
            /* [in] */ OmsTypeInt4 act_level,
            /* [in] */ OmsTypeInt4 end_level,
            /* [in] */ SVER __RPC_FAR *sver_in,
            /* [in] */ OmsTypeInt4 hiesid_in,
            /* [in] */ OmsTypeInt4 start_succ,
            /* [in] */ OmsTypeInt4 start_is_succ);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *sphl )( 
            Isphsapdb __RPC_FAR * This,
            /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
            /* [in] */ TABLENAME __RPC_FAR *hiertab,
            /* [in] */ OmsTypeInt4 act_level,
            /* [in] */ OmsTypeInt4 end_level1,
            /* [in] */ OmsTypeInt4 end_level2,
            /* [in] */ SVER __RPC_FAR *sver_in,
            /* [in] */ OmsTypeInt4 hiesid_in,
            /* [in] */ OmsTypeInt4 start_succ);
        
        END_INTERFACE
    } IsphsapdbVtbl;

    struct Isphsapdb
    {
        CONST_VTBL struct IsphsapdbVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Isphsapdb_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Isphsapdb_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Isphsapdb_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Isphsapdb_test(This,a)	\
    (This)->lpVtbl -> test(This,a)

#define Isphsapdb_sphn(This,hier_inctab,hiertab,act_level,end_level,sver_in,hiesid_in,start_succ,start_is_succ)	\
    (This)->lpVtbl -> sphn(This,hier_inctab,hiertab,act_level,end_level,sver_in,hiesid_in,start_succ,start_is_succ)

#define Isphsapdb_sphl(This,hier_inctab,hiertab,act_level,end_level1,end_level2,sver_in,hiesid_in,start_succ)	\
    (This)->lpVtbl -> sphl(This,hier_inctab,hiertab,act_level,end_level1,end_level2,sver_in,hiesid_in,start_succ)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE Isphsapdb_test_Proxy( 
    Isphsapdb __RPC_FAR * This,
    /* [in] */ int a);


void __RPC_STUB Isphsapdb_test_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE Isphsapdb_sphn_Proxy( 
    Isphsapdb __RPC_FAR * This,
    /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
    /* [in] */ TABLENAME __RPC_FAR *hiertab,
    /* [in] */ OmsTypeInt4 act_level,
    /* [in] */ OmsTypeInt4 end_level,
    /* [in] */ SVER __RPC_FAR *sver_in,
    /* [in] */ OmsTypeInt4 hiesid_in,
    /* [in] */ OmsTypeInt4 start_succ,
    /* [in] */ OmsTypeInt4 start_is_succ);


void __RPC_STUB Isphsapdb_sphn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE Isphsapdb_sphl_Proxy( 
    Isphsapdb __RPC_FAR * This,
    /* [in] */ TABLENAME __RPC_FAR *hier_inctab,
    /* [in] */ TABLENAME __RPC_FAR *hiertab,
    /* [in] */ OmsTypeInt4 act_level,
    /* [in] */ OmsTypeInt4 end_level1,
    /* [in] */ OmsTypeInt4 end_level2,
    /* [in] */ SVER __RPC_FAR *sver_in,
    /* [in] */ OmsTypeInt4 hiesid_in,
    /* [in] */ OmsTypeInt4 start_succ);


void __RPC_STUB Isphsapdb_sphl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Isphsapdb_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_sphsapdb;

#ifdef __cplusplus

class DECLSPEC_UUID("6E822A6E-91E6-11D3-A469-0008C71E4305")
sphsapdb;
#endif
#endif /* __SPHSAPDBLIB_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
