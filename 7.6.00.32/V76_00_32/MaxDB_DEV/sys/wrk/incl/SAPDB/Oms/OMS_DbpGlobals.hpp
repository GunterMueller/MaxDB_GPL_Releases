/*!
 * \file    OMS_DbpGlobals.hpp
 * \author  IvanS, MarkusS, PeterG
 * \brief   DBProcedure: common routines (OMS+SQL class)
 */
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

#ifdef LC_DBPROC_PROTO
#ifndef __OMS_DBPGLOBALS_HPP_PROTO
#define __OMS_DBPGLOBALS_HPP_PROTO

#define PROGIDPFX  "LiveCache" 
#include "livecache.h"			/* no check *    persistent object definitions */ 
#include "livecachesql.h"		/* no check *    C++ SQL Interface             */
#include "livecachesqldbc.h"		/* no check *    SQLDBC Interface             */
#include "livecachetypes.h" /* no check */
#define CONNECT_E_CANNOTCONNECT 0x80040202

#if defined(WIN32) && defined(SQL_STD_STRING_SUPPORTED)
#define INIT_STRING_HANDLING  static_cast<SqlHandle*>(this)->m_str_assign = &str_assign; \
        static_cast<SqlHandle*>(this)->m_str_append = &str_append;
#else
#define INIT_STRING_HANDLING  ;
#endif


//DEFINE_GUID(IID_IliveCacheSpec, 
//            0x0D4BFA71,0x5A99,0x11d1,0xA9,0x43,0x00,0xA0,0xC9,0x43,0x11,0xA5);

DECLSPEC_IMPORT void co90CheckVersion (IUnknown**     pIKernelSink, 
                                       const char*    pLCAppsVersion, 
                                       const char*    pDummy1,
                                       const char*    pDummy2, 
                                       const char*    pDummy3 );
DECLSPEC_IMPORT void co90SysRequest (void*, void*, int*, unsigned char* ); 
/*EXTERN_C DECLSPEC_IMPORT void STDAPICALLTYPE Init_PsODll();*/

extern DECLSPEC_IMPORT HRESULT co90AttachSqlSession(void* sql_handle);

extern DECLSPEC_IMPORT HRESULT co90AttachDefaultContext(void* oms_handle);

extern DECLSPEC_IMPORT HRESULT co90RegisterComObject (char *progIDPfx,
                                                      char *project, 
                                                      char *version,
                                                      const GUID &ClsID,
                                                      char *moduleName);

extern DECLSPEC_IMPORT HRESULT co90ReleaseSqlSession(void* sql_handle);

extern DECLSPEC_IMPORT HRESULT co90ReleaseDefaultContext(void* oms_handle);

extern DECLSPEC_IMPORT HRESULT co90UnregisterComObject (char *progIDPfx,
                                                        char *project, 
                                                        char *version,
                                                        const GUID &ClsID,
                                                        char *moduleName);
/* PTS 1111008
extern DECLSPEC_IMPORT void co90MonitorInit   (void*  oms_handle,
                                               void*  sql_handle,
                                               void** lpMonitor,
                                               long   numEntries);

extern DECLSPEC_IMPORT void co90MonitorInit   (void*  oms_handle,
                                               void*  sql_handle,
                                               CLSID* lpClsId,
                                               void** lpMonitor,
                                               long   numEntries);
*/
/* PTS 1111008 */
extern DECLSPEC_IMPORT void co90MonitorInit   (void*  oms_handle,
                                               void*  sql_handle,
                                               const  IID*   IId,
                                               void** lpMonitor,
                                               long   numEntries);

extern DECLSPEC_IMPORT void co90MonitorProlog (void* lpMonitor,
                                               void* oms_handle,
                                               int dispId,
                                               unsigned char* lpmethodName);

/* PTS 1120794 */
extern DECLSPEC_EXPORT void co90SqlMonitorProlog(void* sql_handle,
                                                 unsigned char* lpmethodName);

extern DECLSPEC_IMPORT void co90MonitorEpilog (void* lpMonitor,
                                               void* oms_handle, 
                                               void* sql_handle,     /* PTS 1120794 */
                                               int   dispId,
                                               int   runTime);

extern DECLSPEC_IMPORT void co90MonitorDelete (void* lpMonitor); 

extern DECLSPEC_IMPORT void co90Vtrace (IUnknown* psession, 
                                        long buflen, 
                                        unsigned char *lpv);

DECLSPEC_IMPORT LONG STDAPICALLTYPE co90InterlockedIncrement( LPLONG lpAddend );
DECLSPEC_IMPORT LONG STDAPICALLTYPE co90InterlockedDecrement( LPLONG lpAddend );

extern "C" DECLSPEC_IMPORT unsigned int dbpStrLen( const DbpTypeUnicode *s );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrCat( DbpTypeUnicode *dest, const DbpTypeUnicode *src );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrnCat( DbpTypeUnicode *dest, const DbpTypeUnicode *src, unsigned int maxlen );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrChr( const DbpTypeUnicode *s, const DbpTypeUnicode c );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrnChr( const DbpTypeUnicode *s, const DbpTypeUnicode c, unsigned int maxlen );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrCmp( DbpTypeUnicode *dest, const DbpTypeUnicode *src );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrnCmp( DbpTypeUnicode *dest, const DbpTypeUnicode *src, unsigned int maxlen );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrCpy( DbpTypeUnicode *dest, const DbpTypeUnicode *src );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrnCpy( DbpTypeUnicode *dest, const DbpTypeUnicode *src, unsigned int maxlen );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrrChr( const DbpTypeUnicode *s, const DbpTypeUnicode c );
extern "C" DECLSPEC_IMPORT DbpTypeUnicode * dbpStrnrChr( const DbpTypeUnicode *s, const DbpTypeUnicode c, unsigned int maxlen );

#if defined(SQL_INCLUDED)
#if defined (WIN32) && defined(SQL_STD_STRING_SUPPORTED)
#if defined(INITGUID)
VOID STDMETHODCALLTYPE str_assign ( char* va, const char* buf, long cl ) 
  { ((std::string&)*va).assign(buf, cl); };
VOID STDMETHODCALLTYPE str_append ( char* va, const char* buf, long cl ) 
  { ((std::string&)*va).append(buf, cl); };
#else
VOID STDMETHODCALLTYPE str_assign ( char* va, const char* buf, long cl );
VOID STDMETHODCALLTYPE str_append ( char* va, const char* buf, long cl );
#endif
#endif
#endif

/*PTS 1105154 FF*/
extern DECLSPEC_IMPORT HRESULT co90CleanUp (void*, void* ); 

/*PTS 1103516 FF*/
extern "C" VOID STDMETHODCALLTYPE Co_IntrospectObject (LPCLSID      *lpClsId,
                                                    LPIID          *lpIId,
                                                    unsigned short *lpNum,
                                                    long           *lpVersion );

#define GLOBAL_DLL_DATA

#endif  /* __OMS_DBPGLOBALS_HPP_PROTO */
#undef LC_DBPROC_PROTO

#else  /* ! LC_DBPROC_PROTO */

#ifndef __OMS_DBPGLOBALS_HPP
#define __OMS_DBPGLOBALS_HPP

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/


/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/


/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
*===========================================================================*/

class CMonitor {
public :
  void* m_pMon;
  CMonitor () {
    m_pMon = NULL; 
  }
  ~CMonitor() {
#if defined (OMS_INCLUDED)
   co90MonitorDelete (m_pMon);
#endif
  }
};

CMonitor dbpMonitor;

/*===========================================================================*
*  FUNCTION PROTOTYPES                                                      *
*===========================================================================*/

/*!
**\name DbpGlobals_ApplicationDefinedTracing Methods for Application-Defined Trace Files
** \see \ref omsDataTypes_Trace
*/
//@{
/*--------------------------------------------------------------------------*/
/// Closes the file
/*!
** Closes the file identified by fileHandle for the invoking task. Parts of 
** the file still stored in the main memory are written to the hard disk. 
** When the file is closed, it does not necessarily mean that it is closed in 
** the operating system, since it may still be open for concurrent tasks.
** 
** \param fileHandle [in] The file index provided when the file is opened.
*/ 
void  dbpCloseMessageFile(OmsTypeInt4 fileHandle) 
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpCloseMessageFile(fileHandle);
}

/*--------------------------------------------------------------------------*/
/// Deletes the file identified by fn. 
/*! 
** Deletes the file identified by fn. If the file does not exist, or is open 
** in at least one concurrent task, the method fails with an error event.
**
** \param fn [in] Null-terminated string that specifies the file name. The name 
**        can also contain path information.
*/
void  dbpDropMessageFile(const char* fn)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpDropMessageFile(fn);
}

/*--------------------------------------------------------------------------*/
/// Provides information on the existence of a file.
/*!
** Provides information on the existence of a file.
**
** \param fn [in] Null-terminated string that specifies the file name. The name 
**        can also contain path information.
** \param doesExist [out] True, if the file identified by fn exists; otherwise false
** \param isDirectory [out] True, if fn identifies a directory; otherwise false.
*/
void  dbpExistsFile(const char* fn, bool& doesExist, bool& isDirectory)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpExistsFile(fn, doesExist, isDirectory);
}

/*--------------------------------------------------------------------------*/
/// Opens the file
/*!
** Opens the file identified by fn for the current task. If the file does not 
** exist when the method is called, it is created implicitly with the size 
** specified in fileSize. If the file already exists, and has not been opened 
** by a concurrent task, the file is overwritten. This only applies, however, 
** if the doAppend parameter is false.
** The result of the method is a file index that is used to identify the file 
** once it has been opened.
**
** \param fn [in] Null-terminated string that specifies the file name. The name 
**        can also contain path information.
** \param fileSize [in] Maximum size of the file in bytes. As described above, 
**        this parameter is only significant when the file is created.
** \param doAppend [in] If true, the file is not overwritten if it already exists.
** \return File index identifying the file.
*/ 
OmsTypeInt4  dbpOpenMessageFile (const char* fn, unsigned long fileSize, bool doAppend)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  return dbpBase.dbpOpenMessageFile(fn, fileSize, doAppend);
}

/*--------------------------------------------------------------------------*/
/// Writes a message into the file
/*!
** Writes a message into the file identified by fileHandle. Formatting and 
** parameter transfer correspond to the method fprintf.
**
** \param fileHandle [in] The file index provided when the file is opened.
** \param fmt [in] The rules for fprintf apply to the format string.
** \param ... [in] The variable number of parameters must correspond to the 
** formatting descriptions in fmt.
**
** \attention The maximal length of the formated final string is 255 bytes.
*/
void  dbpWriteMessageFile(OmsTypeInt4 fileHandle,  const char*fmt, ... )
{
  va_list args;
  va_start (args, fmt);
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpWriteMessageFileVarg(fileHandle, fmt, args );
  va_end (args);
}

/*--------------------------------------------------------------------------*/
/// Writes a message into the file
/*!
** Writes a message into the file identified by fileHandle. In contrast to 
** dbpWriteMessageFile there is no formatting. 
**
** \note The maximal length is not restricted by the function, but if run in 
** ude-server mode migth by restricted by the maximal packet size.
**
** \param fileHandle [in] The file index provided when the file is opened.
** \param msg        [in] Message to be written.
*/
void  dbpWriteMessageFileConst(OmsTypeInt4 fileHandle,  const char*msg)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpWriteMessageFileConst(fileHandle, msg);
}


/*--------------------------------------------------------------------------*/

void  dbpSyncMessageFile (OmsTypeInt4 fileHandle)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpSyncMessageFile(fileHandle);
}

/*--------------------------------------------------------------------------*/
/// Sync the file
/*!
** Writes parts of the file still stored in the main memory to the hard disk.
**
** \param fn [in] Null-terminated string that specifies the file name. 
**        The name can also contain path information.
*/ 
void  dbpSyncMessageFile (const char* fn)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpSyncMessageFile(fn);
}
//@}


/*!
** \name DbpGlobals_SharedLibrarySupport Shared Library Support
** \see \ref omsDataTypes_SharedLibSupport
*/
//@{
/*--------------------------------------------------------------------------*/
/// Unloads the shared library
/*!
** Unloads the shared library identified by hd from the liveCache address space. 
** More precisely, unloading means that the reference counter, which is incremented 
** when the library is loaded, is decremented by 1. The shared library is only 
** unloaded if the value 0 is reached.
**
** \note Due to problems on some platforms, the function for unloading shared 
** libraries is currently deactivated.
**
** \param hd [in] The handle received when the shared library is loaded
*/ 
void  dbpFreeLibrary (DbpHandle hd)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpFreeLibrary(hd);
}

/*--------------------------------------------------------------------------*/
/// Provides the address of a function in a shared library.
/*!
** Provides the address of a function in a shared library.
**
** \param hd [in] The handle received when the shared library is loaded.
** \param pProcName [in] Pointer to a null-terminated string that contains the function name.
** \return Pointer to the function.
*/ 
DbpPROC dbpGetProcAddress(DbpHandle hd, const char *pProcName)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  return dbpBase.dbpGetProcAddress(hd, pProcName);
}

/*--------------------------------------------------------------------------*/
/// Loads the shared library
/*!
** Loads the shared library identified by name in the liveCache address space. 
** If it has already been loaded, a reference counter keeps track of how many 
** times the shared library has been loaded.
**
** \param path [in] Null-terminated string that identifies the shared library. 
**        The name can contain path information. It must not, however, have any 
**        file extension such as 'dll' or 'lib', since this is appended automatically 
**        by the liveCache, in accordance with the operating system.
** \param loadGlobal [in] If true, the entries in the shared library are resolved 
**        globally; otherwise locally.
** \return Handle identifying the shared library.
*/ 
DbpHandle   dbpLoadLibrary(const char* path, bool loadGlobal)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  return dbpBase.dbpLoadLibrary(path, loadGlobal);
}
//@}


/*!
** \name DbpGlobals_EnvironmentInformation Environment Information
** \see \ref omsDataTypes_EnvironmentInformation
*/
//@{
/*--------------------------------------------------------------------------*/
/// Provides the DBROOT path.
/*!
** Provides the DBROOT path. All the directories and files, such as libraries, 
** executable programs, scripts, and so on, which are relevant to the liveCache 
** instance, are stored under this path.
**
** \param path [out] A pointer to a memory area in which the DBROOT path is stored.
** \param  bufSize [in] Size of the memory area (in bytes) provided by the caller. 
*/
void  dbpGetDbrootSapPath(char* path, int bufSize)
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpGetDbrootSapPath(path, bufSize);
}

/*--------------------------------------------------------------------------*/
/// Provides the current directory path for the liveCache instance.
/*!
** Provides the current directory path for the liveCache instance. Trace and 
** diagnostic files are contained in this directory.
**
** \param path [out] A pointer to a memory area in which the current directory 
**        path is stored.
** \param bufSize [in] Size of the memory area (in bytes) provided by the caller. 
*/ 
void  dbpGetWorkingDirectory(char* path, int bufSize) 
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpGetWorkingDirectory(path, bufSize);
}
//@}

/*--------------------------------------------------------------------------*/
/// Sets a breakpoint.
/*!
** Sets a breakpoint. This is only active if the component instance was created 
** in the liveCacheProxy outside the liveCache; in other words, if the triggering 
** DB procedure is executed ‘locally’ or ‘remotely’.
**
** \note dbpDebugBreak is currently only available on Windows NT.
**
** \see \ref omsDataTypes_DbpDebugBreak
*/
void  dbpDebugBreak()
{
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpDebugBreak();
}

/*--------------------------------------------------------------------------*/

void  dbpOpError  ( const char*fmt, ... )
{
  va_list args;
  va_start (args, fmt);
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpOpErrorVarg(fmt, args);
  va_end (args);
}

/*--------------------------------------------------------------------------*/

void  dbpOpMsg  ( const char*fmt, ... )
{
  va_list args;
  va_start (args, fmt);
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpOpMsgVarg(fmt, args);
  va_end (args);
}

/*--------------------------------------------------------------------------*/
/// Writes to the liveCache trace file. 
/*!
** Writes to the liveCache trace file. dbpTrace can be used in the same way as 
** printf. The maximum length of a trace output is restricted to 256 characters. 
** For longer outputs, the excess length is truncated.
**
** \see \ref omsDataTypes_DbpTrace
** \see \ref example_dbpTrace
*/
void  dbpTrace  ( const char*fmt, ... ) 
{
  va_list args;
  va_start (args, fmt);
  DbpBase   dbpBase(this->m_pIUnknown);
  dbpBase.dbpTraceVarg(fmt, args);
  va_end (args);
}

/*--------------------------------------------------------------------------*/
#ifndef OMS_DBPGLOBALS_NOSYSFNC
/*--------------------------------------------------------------------------*/

STDMETHOD ( sysRequest) ( int __RPC_FAR *lpbuflen, unsigned char __RPC_FAR *lpv )
#if defined (SQL_INCLUDED)
#if defined (OMS_INCLUDED)
{ co90SysRequest( STATIC_CAST(OmsHandle*,this), STATIC_CAST(SqlHandle*, this), lpbuflen, lpv ); return S_OK; };
#else
{ co90SysRequest( NULL, STATIC_CAST(SqlHandle*, this), lpbuflen, lpv ); return S_OK; };
#endif
#else
#if defined (OMS_INCLUDED)
{ co90SysRequest( STATIC_CAST(OmsHandle*, this), NULL, lpbuflen, lpv ); return S_OK; };
#else
{ co90SysRequest( NULL, NULL, lpbuflen, lpv ); return S_OK; };
#endif
#endif

/*--------------------------------------------------------------------------*/

STDMETHOD ( sysSqlSupport ) ( BOOLEAN __RPC_FAR *sql_support)     
#if defined (SQL_INCLUDED)
{ *sql_support = true; return S_OK; };
#else
{ *sql_support = false; return S_OK; };
#endif

/*--------------------------------------------------------------------------*/

STDMETHOD ( sysMonitorInit ) (void)
{
#if defined (OMS_INCLUDED)
  LPCLSID         lpClsId;
  LPIID           lpIId;
  unsigned short  num;
  long            version;

  Co_IntrospectObject (&lpClsId, &lpIId, &num, &version );
  co90MonitorInit( STATIC_CAST(OmsHandle*, this), NULL, 
    /*lpIId 1111008*/&COM_INTERFACE_IID, 
    &dbpMonitor.m_pMon, num+3 );
#endif
  return S_OK; 
};

/*--------------------------------------------------------------------------*/

STDMETHOD ( sysMonitorProlog ) ( int dispId, unsigned char __RPC_FAR *lpMethodName )
{
  if (dbpMonitor.m_pMon) {
#   if defined (OMS_INCLUDED)    
      co90MonitorProlog (dbpMonitor.m_pMon, STATIC_CAST(OmsHandle*,this), dispId, lpMethodName);
#     if defined (SQL_INCLUDED)                                                 /* PTS 1120794 */
        co90SqlMonitorProlog( STATIC_CAST(SqlHandle*,this), lpMethodName );     /* PTS 1120794 */
#     endif                                                                     /* PTS 1120794 */
#   endif
  }
  return S_OK;
}

/*--------------------------------------------------------------------------*/

STDMETHOD ( sysMonitorEpilog ) ( int dispId, int runTime )
{
  if (dbpMonitor.m_pMon) {
#   if defined (OMS_INCLUDED)
      void* pSH = NULL;                         /* PTS 1120794 */
#     if defined (SQL_INCLUDED)                 /* PTS 1120794 */
        pSH = STATIC_CAST(SqlHandle*,this);     /* PTS 1120794 */
#     endif                                     /* PTS 1120794 */
      co90MonitorEpilog( dbpMonitor.m_pMon, STATIC_CAST(OmsHandle*,this),
                         pSH,                   /* PTS 1120794 */
                         dispId, runTime );
#   endif
  }
  return S_OK;
}

/*--------------------------------------------------------------------------*/
#else   // OMS_DBPGLOBALS_NOSYSFNC

#undef OMS_DBPGLOBALS_NOSYSFNC

#endif  // OMS_DBPGLOBALS_NOSYSFNC
/*--------------------------------------------------------------------------*/

STDMETHODIMP QueryInterface(const struct _GUID & rclsid,void **ppv )
{
  *ppv=NULL;
  if (IID_IUnknown==rclsid || COM_INTERFACE_IID == rclsid)
    *ppv=STATIC_CAST(COM_INTERFACE*,(this));
  if (IID_IliveCacheSpec == rclsid )
    *ppv=STATIC_CAST(IliveCacheSpec*,(this));
  if ( *ppv == NULL ) return ResultFromScode(E_NOINTERFACE);   
  ((LPUNKNOWN)*ppv)->AddRef();
  return NOERROR;
}

/*--------------------------------------------------------------------------*/

STDMETHODIMP_(unsigned long) AddRef(void)
{
  return co90InterlockedIncrement(&m_cRef);
}

/*--------------------------------------------------------------------------*/

STDMETHODIMP_(unsigned long) Release(void)
{
	unsigned long refCnt = co90InterlockedDecrement(&m_cRef);
	if (0 == refCnt) {
      delete this;
      return 0;
    }
    return refCnt;
}

/*--------------------------------------------------------------------------*/

STDMETHODIMP Advise(LPUNKNOWN pUnkSink)
{
  IUnknown       *pSink;
  HRESULT        hr = NOERROR;
  
  /* Verify that the sink has the interface it's supposed to.*/
  if (FAILED(pUnkSink->QueryInterface(IID_IliveCacheSink, (void**)&pSink)))
    return ResultFromScode(CONNECT_E_CANNOTCONNECT);
  m_pIUnknown=pSink;
#if defined (SQL_INCLUDED)
  hr = co90AttachSqlSession(STATIC_CAST(SqlHandle*, this));
  if ( FAILED(hr) ) return hr;
#endif
#if defined (OMS_INCLUDED)
  hr = co90AttachDefaultContext(STATIC_CAST(OmsHandle*, this));
#endif
  return hr;
}

/*--------------------------------------------------------------------------*/

STDMETHODIMP Unadvise()
{
  HRESULT hr = NOERROR;
  if ( m_pIUnknown != NULL ) {
#if defined (SQL_INCLUDED)
    hr = co90ReleaseSqlSession(STATIC_CAST(SqlHandle*, this));
#endif
#if defined (OMS_INCLUDED)
    hr = co90ReleaseDefaultContext(STATIC_CAST(OmsHandle*, this));
#endif
    m_pIUnknown->Release();
  }
  m_pIUnknown = NULL;
  return hr;
}

/*------------------------- PTS 1105154 ------------------------------------*/

STDMETHOD ( sysCleanUp )( void )
{
  HRESULT hr = S_OK;
#if defined (SQL_INCLUDED)
#if defined (OMS_INCLUDED)
   hr = co90CleanUp( STATIC_CAST(OmsHandle*,this), STATIC_CAST(SqlHandle*, this) );
#else
   hr = co90CleanUp( NULL, STATIC_CAST(SqlHandle*, this) );
#endif
#else
#if defined (OMS_INCLUDED)
   hr = co90CleanUp( STATIC_CAST(OmsHandle*,this), NULL );
#else
   hr = co90CleanUp( NULL, NULL );
#endif
#endif
  return hr;
}
#endif  /* __OMS_DBPGLOBALS_HPP */
#endif  /* LC_DBPROC_PROTO */
