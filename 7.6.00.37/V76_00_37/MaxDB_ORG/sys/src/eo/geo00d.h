/* @lastChanged: "1998-10-22  9:25"
 * @filename:   geo00d.h
 * @purpose:    "DCOM Layer global definition file"
 * @release:    7.2.0.0
 * @see:        "-.-"
 * @author:     Ferdinand Flohe (D024849 - Ferdinand.Flohe@SAP-AG.de)
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

*/

#ifndef GEO00D
#define GEO00D

#include <stdio.h>
#include "ilivecachesink.h"  /* no check */ 
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/* PTS 1112670 define literals as integer instead as long */

#define  GEO00D_DCOM_WRONG_FIRST_PARAM              0x80040200
#define  GEO00D_DCOM_TOO_MANY_OUT_PARAM             0x80040201
#define  GEO00D_DCOM_UNSUPPORTED_PARAM_TYPE         0x80040202
#define  GEO00D_DCOM_MISSING_USERTYPE_GUID          0x80040203
#define  GEO00D_DCOM_TOO_MANY_INTERFACES            0x80040204
#define  GEO00D_DCOM_PROGID_NOT_FOUND               0x80040205
#define  GEO00D_DCOM_INPROCSERVER_NOT_FOUND         0x80040206
#define  GEO00D_DCOM_LOCALSERVER_NOT_FOUND          0x80040207
#define  GEO00D_DCOM_METHOD_NOT_FOUND               0x80040208
#define  GEO00D_DCOM_COGUID_NOT_FOUND_IN_TYPELIB    0x80040209
#define  GEO00D_DCOM_INSPEC_ROUTINE_NOT_FOUND       0x8004020A
#define  GEO00D_DCOM_COCLASS_IN_REGISTRY_NOT_FOUND  0x8004020B
#define  GEO00D_DCOM_ONLY_INPROC_SUPPORTED          0x8004020C
#define  GEO00D_DCOM_LIBRARY_NOT_LOADABLE           0x8004020D
#define  GEO00D_DCOM_DLL_MAIN_MISSING               0x8004020E
#define  GEO00D_DCOM_DLL_MAIN_ERROR                 0x8004020F
#define  GEO00D_DCOM_NO_COCLASS_OBJECT_FOUND        0x80040210
#define  GEO00D_DCOM_DBPROC_CRASHED                 0x80040211
#define  GEO00D_DCOM_BUFFER_LIMIT                   0x80040212
#define  GEO00D_DCOM_BAD_ALLOC                      0x80040213     // PTS XXXEXCP
#define  GEO00D_DCOM_UNEXPECTED_TYPELIB_STRUCTURE   0x80040313
#define  GEO00D_DCOM_TRANSEND_FAILED                0x80040314
#define  GEO00D_DCOM_INSTALLATION_PATH_NOT_FOUND    0x80040315
#define  GEO00D_DCOM_INSTALLATION_NOT_REGISTERED    0x80040316
#define  GEO00D_DCOM_COULD_NOT_RETRIEVE_PATH_VAL    0x80040317
#define  GEO00D_DCOM_COULD_NOT_RETRIEVE_MD5_MARK    0x80040318  /* PTS 1121178 FF 19-Mar-2003 */
#define  GEO00D_DCOM_MD5_MARK_DOES_NOT_MATCH        0x80040319  /* PTS 1121178 FF 19-Mar-2003 */ 
#define  GEO00D_DCOM_EXCEPTION_RAISED               0xFFFFFFFF
#define  GEO00D_DCOM_TERMINATED                     0xFFFFFFFE


#define  GEO00D_DBO_METHOD  0
#define  GEO00D_DBO_DBPROC  1
#define  GEO00D_DBO_DBFUNC  2

#if defined(WIN32)
#define   STATUS_SETERROR_EXCEPTION          (tsp00_Uint4   )0xC0004711
#define   STATUS_TERMINATE_EXCEPTION         (tsp00_Uint4   )0xC0004712
#endif

class CHRESULT_EO00D
{
private:
  IliveCacheSink* m_psession;
  HRESULT         m_hr;
  char            m_ModuleName[64];
  
public:
  CHRESULT_EO00D(tsp00_C4 session,const char* pcModuleName)
  {
    SAPDB_MemCopyNoCheck(&m_psession,&session[0],sizeof(m_psession));
    SAPDB_strncpy_zero_terminated(&m_ModuleName[0],pcModuleName,sizeof(m_ModuleName));
  }

  bool    succeeded()
  {
    char cVtraceMsg[256];

    if (SUCCEEDED(m_hr)) return true;
    sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s", m_hr, errText(m_hr), m_ModuleName);
    m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
    return false;
  }

  bool    succeeded( const char* s)
  {
    char cVtraceMsg[256];

    if (SUCCEEDED(m_hr)) return true;
    sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s:%.64s", m_hr, errText(m_hr), m_ModuleName, s );
    m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
    return false;
  }

  bool    failed()
  {
    char cVtraceMsg[256];

    if (FAILED(m_hr)){
      sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s", m_hr, errText(m_hr), m_ModuleName);
      m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
      return true;
    }
    return false;
  }

  bool    failed( const char* s)
  {
    char cVtraceMsg[256];

    if (FAILED(m_hr)) 
    {
      sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s:%.64s", m_hr, errText(m_hr), m_ModuleName, s );
      m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
      return true;
    }
    else
      return false;
  }

  bool    noError()
  {
    char cVtraceMsg[256];

    if (m_hr == NO_ERROR) return true;
    sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s", m_hr, errText(m_hr), m_ModuleName);
    m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
    return false;
  }

  bool    noError( const char* s)
  {
    char cVtraceMsg[256];

    if (m_hr == NO_ERROR) return true;
    sp77sprintf ( &cVtraceMsg[0], sizeof(cVtraceMsg), "HRESULT: 0x%X (%s), %s:%.64s", m_hr, errText(m_hr), m_ModuleName, s );
    m_psession->ConsoleMsg ((tsp00_Int4)strlen(&cVtraceMsg[0]), (unsigned char*)&cVtraceMsg[0] );
    return false;
  }

  HRESULT hresult()
  {
    return m_hr;
  }

  void    operator= (HRESULT hr)
  {
    m_hr = hr;
  };

  const char* errText(HRESULT hr)   // PTS 1126283
  {
    switch (hr) {
      case 0x80040200:  return "WRONG_FIRST_PARAM"; break;             
      case 0x80040201:  return "TOO_MANY_OUT_PARAM"; break;         
      case 0x80040202:  return "UNSUPPORTED_PARAM_TYPE"; break;        
      case 0x80040203:  return "MISSING_USERTYPE_GUID"; break;         
      case 0x80040204:  return "TOO_MANY_INTERFACES"; break;           
      case 0x80040205:  return "PROGID_NOT_FOUND"; break;              
      case 0x80040206:  return "INPROCSERVER_NOT_FOUND"; break;        
      case 0x80040207:  return "LOCALSERVER_NOT_FOUND"; break;         
      case 0x80040208:  return "METHOD_NOT_FOUND"; break;              
      case 0x80040209:  return "COGUID_NOT_FOUND_IN_TYPELIB"; break;   
      case 0x8004020A:  return "INSPEC_ROUTINE_NOT_FOUND"; break;      
      case 0x8004020B:  return "COCLASS_IN_REGISTRY_NOT_FOUND"; break;
      case 0x8004020C:  return "ONLY_INPROC_SUPPORTED"; break;         
      case 0x8004020D:  return "LIBRARY_NOT_LOADABLE"; break;          
      case 0x8004020E:  return "DLL_MAIN_MISSING"; break;              
      case 0x8004020F:  return "DLL_MAIN_ERROR"; break;                
      case 0x80040210:  return "NO_COCLASS_OBJECT_FOUND"; break;       
      case 0x80040211:  return "DBPROC_CRASHED"; break;                
      case 0x80040212:  return "BUFFER_LIMIT"; break;                  
      case 0x80040213:  return "BAD_ALLOC"; break;                          
      case 0x80040313:  return "UNEXPECTED_TYPELIB_STRUCTURE"; break;  
      case 0x80040314:  return "TRANSEND_FAILED"; break;               
      case 0x80040315:  return "INSTALLATION_PATH_NOT_FOUND"; break;   
      case 0x80040316:  return "INSTALLATION_NOT_REGISTERED"; break;   
      case 0x80040317:  return "COULD_NOT_RETRIEVE_PATH_VAL"; break;   
      case 0x80040318:  return "COULD_NOT_RETRIEVE_MD5_MARK"; break;     
      case 0x80040319:  return "MD5_MARK_DOES_NOT_MATCH"; break;          
      case 0xFFFFFFFF:  return "EXCEPTION_RAISED"; break;              
      case 0xFFFFFFFE:  return "TERMINATED"; break;
      default: return "Unknown HRESULT";
    }
  }

};

#if !defined (_WIN64)
 typedef int (WINAPI *PROC)();
#endif

#if defined (_WIN64) && defined (_M_IA64)
 EXTERN_C LONG WINAPI sql38aVdcomCall(
    IUnknown *lpIUnknown,
    PROC  pProc,
    void* pBufAddr,
    long  BufLen,
	void* pfBufAddr,
    long  fBufLen,
    long  ParmStackSize
    );
#elif defined (_WIN64) && defined (_M_AMD64)
#ifdef __cplusplus
//
// msc 14.00 on windows amd64
// does not handle c++ exceptions from functions declared 'extern "C"'
//
 LONG WINAPI sql38aVdcomCall(
    void *lpIUnknown,
    void *pProc,
    void *pBufAddr,
    __int64 BufLen,
    __int64 ParmStackSize
    );
#endif
#else
 EXTERN_C LONG WINAPI sql38aVdcomCall(
    IUnknown *lpIUnknown,
    PROC  pProc,
    void* pBufAddr,
    long  BufLen,
    long  ParmStackSize
    );
#endif  // _WIN64

EXTERN_C PROC WINAPI sql36GetProcAddress(
    HMODULE hModule,
    LPCSTR lpProcName
    );

EXTERN_C BOOL WINAPI sql36FreeLibrary(
    HMODULE hLibModule
    );



#endif  /*GEO00D*/
