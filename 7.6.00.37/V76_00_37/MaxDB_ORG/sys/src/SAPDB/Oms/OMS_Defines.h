/*!
 * \file    OMS_Defines.h
 * \author  MarkusSi, Roterring
 * \brief   Defines used in the liboms.
 */

/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_DEFINES_HPP
#define __OMS_DEFINES_HPP

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

// Necessary to activate interface for windows2000.
// In SAPDBMem_NewDestroy.hpp/oaidl.h the file windows.h is included and therefore the
// define must occur before including SAPDBMem_NewDestroy.hpp/oaidl.h.
#define NEEDED_WIN32_WINNT 0x0500
#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#ifdef __cplusplus
#include "Oms/OMS_GUID.hpp"
#endif

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#define OMS_DEFAULT_SCHEMA_HANDLE        1
#define OMS_MAX_SCHEMA_NAME_LENGTH      31

/*! Maximal number of objects which are processed by a single mass-operation-call 
**  to the kernel. 
*/
#define OMS_MASS_OPERATION_CNT          20
/*! Maximal number of objects which are processed as a 'cluster' (same logical
**  page in the kernel) during commit.
*/
#define OMS_MASS_COMMIT_CNT            100
/*! Number of chunk buckets in context hash array*/
#define OMS_HASH_ARRAY_SIZE             8192
/*! Number of buckets in hash array chunks */
#define OMS_HASH_ARRAY_CHUNK_BUCKET_CNT 8192
/*! Default UserDataSize in allocator RawChunks*/
#define OMS_DEFAULT_CHUNK_SIZE       (OMS_HASH_ARRAY_CHUNK_BUCKET_CNT*sizeof(void*))

/*! Maximum size of error strings in oms layer used in character conversions */
#define OMS_MAX_ERROR_TEXT_LENGTH 256

struct IliveCacheSink;

#if !defined(HPUX) && !defined(AIX) && !defined(LINUX)
#define OMS_PLACEMENT_DELETE
#endif

/* The following defines are used to ensure, that a com-object is running with
** the same version as it was build against.
** The version in the string does NOT correspond to the current version, but 
** to the version in which the last change of the corresponding interface has
** taken place.
** Therefore every time a change of one of the interface take place the version
** and the date should be adopted for the corresonding entry
*/
//#define LIVECACHEOMS_VERSION     "LiveCache.h       7.6.00.01   DATE: 2004-09-01"
#define LIVECACHESQL_VERSION     "<MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>"
#define ILIVECACHESINK_VERSION   "<MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>"
#define LIVECACHETYPES_VERSION   "<MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>"
#define LIVECACHEOMS_VERSION     "<MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>"

&if $OS = WIN32
/* if std::string type is to be used as parameter for SQL, the following symbol
* has to be defined 

#define SQL_STD_STRING_SUPPORTED
*/
&endif

#ifndef GEO00_H
&if $MACH in [ _IBMR2 ] 
#if (__IBMCPP__ < 500)
typedef unsigned char bool;
#define     true    1
#define     false   0
#endif
&endif

&if $MACH in [ SUN ] || $OSSPEC == SOLARIS 
#if !defined(_BOOL)
typedef unsigned char bool;
#define     true    ((const bool)1)
#define     false   ((const bool)0)
#endif
&endif

&if $MACH in [ _IBMR2 ] 
/*
* These keywords are not supported by the AIX compiler (C++ Standard of 1992 ...)
* If you are carefull, you are able to ignore them sometimes...
*/

// PTS 1106619
#if (__IBMCPP__ < 500)
#define explicit
#define mutable
#define typename
#define CONST_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#define REINTERPRET_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#define STATIC_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#else
#define CONST_CAST(TYPE_,EXPR_) const_cast< TYPE_ >(EXPR_)
#define REINTERPRET_CAST(TYPE_,EXPR_) reinterpret_cast< TYPE_ >(EXPR_)
#define STATIC_CAST(TYPE_,EXPR_) static_cast< TYPE_ >(EXPR_)
#endif

&else

#define CONST_CAST(TYPE_,EXPR_) const_cast< TYPE_ >(EXPR_)
#define REINTERPRET_CAST(TYPE_,EXPR_) reinterpret_cast< TYPE_ >(EXPR_)
#define STATIC_CAST(TYPE_,EXPR_) static_cast< TYPE_ >(EXPR_)

&endif

#endif  /* ifndef GEO00_H */

#include "Oms/OMS_AFX.h"

&if $OS = WIN32
#pragma warning(disable:4103)
#ifdef _AFXDLL
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif

#include <oaidl.h>
&else

#define MAX_PATH          260
#define MIDL_INTERFACE(x) struct

#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif

#define _MAX_PATH   256 /* max. length of full pathname */
#define __stdcall
#define DECLSPEC_UUID(x)   
typedef long long hyper;
#define __RPC_USER 
#define __RPC_STUB 
#define __cdecl

&if $BIT64
typedef int                 HRESULT;
&else
typedef long                HRESULT;
&endif

#define STDMETHODCALLTYPE       __stdcall
#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method
#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE
#define STDAPICALLTYPE          __stdcall
#define STDAPI                  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE
#define WINAPI                  __stdcall

#define __RPC_FAR
#define FAR
#define BEGIN_INTERFACE
#define END_INTERFACE

#define DECLARE_HANDLE(name) typedef HANDLE name
#define _ASSERT(expr) ((void)0)
#define _ASSERTE(expr) ((void)0)

#define _CrtCheckMemory() ;
#define _CrtIsValidPointer(p, n, r)         ((int)1)

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
*===========================================================================*/

#define CONST               const
typedef int                 INT;
typedef unsigned int        UINT;
typedef int                 *PINT;
typedef int                 *LPINT;
typedef unsigned int        *PUINT;
#ifndef _CHAR
#define _CHAR
typedef char                CHAR;
#endif
typedef CHAR                *PCHAR;
typedef CHAR                *LPCH, *PCH;
typedef char                *PSZ;
#ifndef _UCHAR
#define _UCHAR
typedef unsigned char       UCHAR;
#endif
typedef UCHAR               *PUCHAR;
typedef unsigned char       BYTE;
typedef BYTE                *PBYTE;
typedef BYTE                *LPBYTE;
typedef BYTE                BOOLEAN;           
typedef int                 BOOL;
typedef BOOL                *PBOOL;
typedef BOOL                *LPBOOL;
typedef unsigned char       boolean;
typedef unsigned short      WORD;
typedef WORD                *PWORD;
typedef WORD                *LPWORD;
typedef unsigned long       DWORD;
typedef DWORD               *LPDWORD;
typedef short               SHORT;
typedef long                LONG;
typedef long                *LPLONG;
typedef void                VOID;
typedef void                *LPVOID;
typedef void                *PVOID;    
typedef unsigned long       ULONG;
typedef ULONG               *PULONG;
typedef unsigned short      USHORT;
typedef USHORT              *PUSHORT;
typedef float               FLOAT;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
#ifndef __HANDLE_T__
#define __HANDLE_T__ __HANDLE_T__
typedef PVOID               HANDLE;
#endif
typedef void                *RPC_IF_HANDLE;
typedef void                IRpcStubBuffer;
typedef void                IRpcChannelBuffer;
typedef void                RPC_MESSAGE;
typedef void*               PRPC_MESSAGE;
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HKEY);
typedef HINSTANCE           HMODULE;      
typedef CHAR                *LPSTR, *PSTR;
typedef wchar_t             WCHAR;        // wc,   16-bit UNICODE character
typedef WCHAR               *LPWSTR, *PWSTR;
typedef LPWSTR              PTSTR, LPTSTR;
typedef WCHAR				        OLECHAR;
typedef OLECHAR				      *BSTR;
typedef BSTR				        *LPBSTR;
typedef CONST CHAR          *LPCSTR, *PCSTR;
typedef OLECHAR             *LPOLESTR;
#if !defined (_WIN64)
typedef int               HFILE;  // Old style, no longer supported!!!
#endif

#define __IID_DEFINED__     /* needed for MIDL generated Code */
#define IID_NULL            GUID_NULL
#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
#define CLSID_NULL          GUID_NULL
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)
#define FMTID_NULL          GUID_NULL
#define IsEqualFMTID(rfmtid1, rfmtid2) IsEqualGUID(rfmtid1, rfmtid2)


typedef struct _GUID {          // size is 16
&if $BIT64
  unsigned int  Data1;
&else
  DWORD         Data1;
&endif
  WORD          Data2;
  WORD          Data3;
  BYTE          Data4[8];
} GUID;

typedef GUID                IID;
typedef IID                 *LPIID;
typedef GUID                CLSID;
typedef CLSID               *LPCLSID;
typedef GUID                *LPGUID;

#ifdef __cplusplus
#define REFGUID             const GUID &
#define REFCLSID            const CLSID &
#define REFIID              const IID &

inline BOOL IsEqualGUID(REFGUID rguid1, REFGUID rguid2) {
  return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}
inline BOOL operator==(const GUID& guidOne, const GUID& guidOther)
{
  return IsEqualGUID(guidOne,guidOther);
}

inline BOOL operator!=(const GUID& guidOne, const GUID& guidOther)
{
  return !(guidOne == guidOther);
}
#else   //  ! __cplusplus
#define IsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(GUID)))
#endif  //  __cplusplus


#ifdef __cplusplus
extern "C"{
#endif 

  /* Forward Declarations */ 
  typedef struct IUnknown IUnknown;
  typedef struct IClassFactory IClassFactory;

  void *MIDL_user_allocate(size_t);
  void MIDL_user_free( void * ); 

  extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_c_ifspec;
  extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_s_ifspec;

  typedef /* [unique] */ IUnknown *LPUNKNOWN;

  //////////////////////////////////////////////////////////////////
  // IID_IUnknown and all other system IIDs are provided in UUID.LIB
  // Link that library in with your proxies, clients and servers
  //////////////////////////////////////////////////////////////////

#if defined(INITGUID)
  /* IID's, normally defined within UUID.LIB */
  EXTERN_C const IID IID_IUnknown = {0x00000000,0x0000,0x0000,
  {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
  EXTERN_C const IID GUID_NULL = {0x00000000,0x0000,0x0000,
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
#else
  EXTERN_C const IID IID_IUnknown;
  EXTERN_C const IID GUID_NULL;
#endif

#if defined(__cplusplus) && !defined(CINTERFACE)

  struct IUnknown {
public:
  BEGIN_INTERFACE
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void  **ppvObject) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef( void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release( void) = 0;
  END_INTERFACE
  };

#else 	/* C style interface */

  typedef struct IUnknownVtbl {
    BEGIN_INTERFACE
      HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
      IUnknown  *This,
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ void  **ppvObject);
      ULONG ( STDMETHODCALLTYPE *AddRef )( 
      IUnknown *This);
      ULONG ( STDMETHODCALLTYPE *Release )( 
        IUnknown *This);
    END_INTERFACE
  } IUnknownVtbl;

&if $OS = WIN32
  interface IUnknown {
&else
  struct IUnknown {
&endif
    CONST_VTBL struct IUnknownVtbl *lpVtbl;
  };
#endif

  typedef /* [unique] */ IClassFactory *LPCLASSFACTORY;

#if defined(INITGUID)
  /* IID's, normally defined within UUID.LIB */
  EXTERN_C const IID IID_IClassFactory = {0x00000001,0x0000,0x0000,
  {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
#else
  EXTERN_C const IID IID_IClassFactory;
#endif

#if defined(__cplusplus) && !defined(CINTERFACE)

  struct IClassFactory : public IUnknown
  {
  public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
      /* [unique][in] */ IUnknown *pUnkOuter,
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ void **ppvObject) = 0;

      virtual /* [local] */ HRESULT STDMETHODCALLTYPE LockServer( 
      /* [in] */ BOOL fLock) = 0;
  };

#else 	/* C style interface */

  typedef struct IClassFactoryVtbl
  {
    BEGIN_INTERFACE

      HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
      IClassFactory *This,
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ void **ppvObject);

      ULONG ( STDMETHODCALLTYPE *AddRef )( 
      IClassFactory *This);

      ULONG ( STDMETHODCALLTYPE  *Release )( 
        IClassFactory *This);

      /* [local] */ HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
      IClassFactory *This,
        /* [unique][in] */ IUnknown *pUnkOuter,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void  **ppvObject);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE *LockServer )( 
        IClassFactory *This,
        /* [in] */ BOOL fLock);

    END_INTERFACE
  } IClassFactoryVtbl;

  struct IClassFactory
  {
    CONST_VTBL struct IClassFactoryVtbl *lpVtbl;
  };

#endif 	/* C style interface */

#ifdef __cplusplus
}
#endif

#ifndef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  EXTERN_C const GUID name
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  EXTERN_C const GUID name \
  = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif // INITGUID

#define HKEY_CLASSES_ROOT					   (( HKEY ) 0x80000000 )

&if $BIT64
#define MAKE_HRESULT(sev,fac,code) \
  ((HRESULT) (((unsigned int)(sev)<<31) | ((unsigned int)(fac)<<16) | ((unsigned int)(code))) )
&else
#define MAKE_HRESULT(sev,fac,code) \
  ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
&endif
#define FAILED(Status)                         ((HRESULT)(Status)<0)
#define SUCCEEDED(Status)                      ((HRESULT)(Status)>=0)
#define ResultFromScode(sc)                    ((HRESULT) (sc))
#define _HRESULT_TYPEDEF_(_sc)                 ((HRESULT)_sc)
#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)
#if !defined (NO_ERROR)
#define NO_ERROR						             0L                                                 
#endif   
#define NOERROR                          0L                       
#define ERROR_SUCCESS                    0L
#define ERROR_FILE_NOT_FOUND             2L
#define ERROR_NOT_ENOUGH_MEMORY          8L    
#define ERROR_INVALID_PARAMETER          87L    
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
#define CLASS_E_NOAGGREGATION            _HRESULT_TYPEDEF_(0x80040110L)
#define CLASS_E_CLASSNOTAVAILABLE        _HRESULT_TYPEDEF_(0x80040111L)
#define REGDB_E_CLASSNOTREG              _HRESULT_TYPEDEF_(0x80040154L)
#define CO_E_CLASSSTRING                 _HRESULT_TYPEDEF_(0x800401F3L)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)

#define DLL_PROCESS_ATTACH 1    
#define DLL_THREAD_ATTACH  2    
#define DLL_THREAD_DETACH  3    
#define DLL_PROCESS_DETACH              0    

#if !defined(FALSE)
#define FALSE               0
#define TRUE                1
#endif

typedef enum tagCLSCTX {	
  CLSCTX_INPROC_SERVER	= 0x1,
  CLSCTX_INPROC_HANDLER	= 0x2,
  CLSCTX_LOCAL_SERVER	= 0x4,
  CLSCTX_INPROC_SERVER16	= 0x8,
  CLSCTX_REMOTE_SERVER	= 0x10,
  CLSCTX_INPROC_HANDLER16	= 0x20,
  CLSCTX_INPROC_SERVERX86	= 0x40,
  CLSCTX_INPROC_HANDLERX86	= 0x80
}	CLSCTX;

//DECLSPEC_IMPORT 
EXTERN_C VOID WINAPI DebugBreak( void );

EXTERN_C LONG STDAPICALLTYPE InterlockedIncrement( LPLONG lpAddend );
EXTERN_C LONG STDAPICALLTYPE InterlockedDecrement( LPLONG lpAddend );

EXTERN_C DECLSPEC_IMPORT DWORD WINAPI GetModuleFileNameA(
  HMODULE hModule,
  LPSTR lpFilename,
  DWORD nSize
  );
EXTERN_C DECLSPEC_IMPORT DWORD WINAPI GetModuleFileNameW(
  HMODULE hModule,
  LPWSTR lpFilename,
  DWORD nSize
  );
EXTERN_C DECLSPEC_IMPORT DWORD WINAPI GetLastError(
  void
  );
EXTERN_C DECLSPEC_IMPORT HMODULE WINAPI LoadLibraryA(
  LPCSTR lpLibFileName
  );
typedef int (WINAPI *PROC)();
EXTERN_C DECLSPEC_IMPORT PROC WINAPI GetProcAddress(
  HMODULE hModule,
  LPCSTR lpProcName
  );
EXTERN_C DECLSPEC_IMPORT BOOL WINAPI FreeLibrary(
  HMODULE hLibModule
  );

#ifdef UNICODE
#define GetModuleFileName  GetModuleFileNameW
#else
#define GetModuleFileName  GetModuleFileNameA
#endif 

&endif 

// Assertions : compile with option /-D_ASSERT_OMS

#ifdef _ASSERT_OMS
#define _OMSASSERT(session,expr) if (!(expr)) { session->ThrowDBError(e_requested_dbperror, #expr, __FILE__, __LINE__);}
#else
#define _OMSASSERT(h,expr) 
#endif

#if defined(OMSTST)
#define OMS_CHECK_EXCEPTION(HANDLE) { (HANDLE).m_pSession->CheckCancelCountDown();\
  (HANDLE).m_pSession->CheckExceptionCountDown(); }
#else
#define OMS_CHECK_EXCEPTION(HANDLE)
#endif

#ifndef _DEBUG

/****************************************************************************
*
* Debug OFF
* Debug OFF
* Debug OFF
*
***************************************************************************/

#define _TRACE_METHOD_ENTRY(h,methodname)
#define _TRACE_OID(h,oid)
#else

/****************************************************************************
*
* Debug ON
* Debug ON
* Debug ON
*
***************************************************************************/

#define _TRACE_METHOD_ENTRY(h,methodname) (h)->omsTracef(methodname);

#define _TRACE_OID(h,oid) (h)->omsTraceOid(oid);

#endif

#define MEMBER_OFFSET(T,M) (((char*) &((T*) 0)->M) - (char*) 0)


/*===========================================================================*
*  Defines for Synchronization                                               *
*============================================================================*/

enum {
  RGN_EMERGENCY_CHUNK = 6,
  RGN_STACK_HEAP      = 5,
  RGN_SHARED_HEAP     = 4,
  RGN_VERSION_DIR     = 3,   /* PTS 1124533: used to synchonize the access to the global iterator (Glob_co10.VersionIter) */
  RGN_CLASS_DIR       = 2,
  RGN_UNLOAD_VERSION  = 1,
  RGN_MONITOR         = 0
};

/* If the following define is changed, the define OMS_SYS_REGION in geo38.h,
** the define OMS_SYS_RGNS in vgg08, and the computation of _OMS_RGNS in
** cserv.pcf must be adapted
*/
#define OMS_INTERNAL_CRITICAL_SECTIONS 7 /* PTS 1124533 */

enum OMS_LockMode{ OMS_NO_LOCK, OMS_LOCK_SHARED, OMS_LOCK_EXCLUSIVE }; /* PTS 1124533 */

#define OMS_RWLOCK_AREA 0

#define VERSION_DIR_SIZE 17
#define ALLOCATOR_DIR_SIZE 5
#define ANCHOR_DIR_SIZE 5

enum {
  VERSION_DIR_START_CNT = 1,
  ALLOCATOR_DIR_START_CNT = VERSION_DIR_START_CNT + VERSION_DIR_SIZE,
  ANCHOR_DIR_START_CNT = ALLOCATOR_DIR_START_CNT + ALLOCATOR_DIR_SIZE
};


/*===========================================================================*
*  Defines of Error-Codes                                                    *
*============================================================================*/
/* These constants define the value of the error codes which are used after
** the mapping in the kernel (see SQLManErr_Instance.cpp).
** For example in the kernel for a lock-collision the constant 
** e_lock_collision = 5000 (see ggg00) is used and before returning the error-
** code to the liboms, this error is transformed in SQLManErr_Instance.cpp 
** to the value 400 and therefore a new corresponding constant e_OMS_lock_collision
** is defined here.
*/
#define e_OMS_system_error          -9404
#define e_OMS_sysinfo_not_found     -9205
#define e_OMS_file_not_accessible   -9041
#define e_OMS_not_implemented       -9000
#define e_OMS_not_translatable      -8044
#define e_OMS_no_open_subtrans      -7065
#define e_OMS_duplicate_name        -6006
#define e_OMS_missing_privilege     -5001
#define e_OMS_invalid_parameter     -3999
#define e_OMS_invalid               -3999
#define e_OMS_invalid_timestamp     -3050
#define e_OMS_invalid_command       -3005
#define e_OMS_identifier_too_long   -2014
#define e_OMS_too_small_packet_size -1114
#define e_OMS_too_many_subtrans     -1023
#define e_OMS_no_more_memory         -915
#define e_OMS_work_rolled_back       -911
#define e_OMS_hostfile_error         -903
#define e_OMS_file_not_found         -108
#define e_OMS_cancelled              -102
#define e_OMS_lock_request_timeout    -51
#define e_OMS_duplicate_key           -20
#define e_OMS_no_next_record          100
#define e_OMS_lock_collision          400
#define e_OMS_request_timeout         500

DEFINE_GUID(IID_IliveCacheSpec, 
            0x0D4BFA71,0x5A99,0x11d1,0xA9,0x43,0x00,0xA0,0xC9,0x43,0x11,0xA5);

const GUID    OMS_VAR_OBJ_GUID_DEF   = { 0x8fb0963d, 0xe5aa, 0x11d1, 
                                       { 0xa9, 0x5c, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31 } };
#define OMS_VAR_OBJ_GUID    -1

enum OmsObjectType {
  OMS_NORMAL_OBJ=1, 
  OMS_KEYED_OBJ, 
  OMS_VAR_OBJ, 
  OMS_ARRAY_OBJ
};

enum OMS_DiagnoseAction {
  OMS_DIAGNOSE_DUMP=1,
  OMS_DIAGNOSE_STOP,
  OMS_DIAGNOSE_FLUSH
};

enum OMS_ExceptionClass { 
  OMS_EXCEPTION_DEFAULT=0,
  OMS_EXCEPTION_BAD_ALLOC,
  OMS_EXCEPTION_OUT_OF_DATE,
  OMS_EXCEPTION_DUPLICATE_KEY,
  OMS_EXCEPTION_KEY_IN_USE,
  OMS_EXCEPTION_LOCK_REQUEST_TIMEOUT,
  OMS_EXCEPTION_OBJECT_NOT_FOUND,
  OMS_EXCEPTION_OVERFLOW,
  OMS_EXCEPTION_CONTAINER_ERROR,
  OMS_EXCEPTION_CANCELLED,
  OMS_EXCEPTION_PROVOKED_ERROR,
  OMS_EXCEPTION_INVALID_OBJECT
};

#define OMS_VAROBJ_CHECK_PATTERN 0xdddddddd

#endif  /* __OMS_DEFINES_HPP */
