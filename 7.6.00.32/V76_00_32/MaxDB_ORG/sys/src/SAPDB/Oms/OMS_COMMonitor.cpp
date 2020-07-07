/* @lastChanged: "1998-10-22  8:39"

  * @filename:   OMS_COMMonitor.cpp
  * @purpose:    "C++ System Interface for DB-Procedures"
  * @release:    7.2.0.0
  * @see:        "http://www.bea.sap-ag.de/doku/rel-7.1/vco90.html"
  *
  * @Copyright (c) 1998-2005 SAP AG"



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




*/

/* ================================================================================ */
/* ==========              REPLACEMENTS BY THE PREPROCESSOR               ========= */
/* ================================================================================ */

/* ================================================================================ */
/* ==========                 DECLARATION OF TYPES                        ========= */
/* ================================================================================ */


#include "Oms/OMS_COMMonitor.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"  // PTS 1116802
#include "CppSQL/SQL_Handle.hpp"
#include "hsp77.h"      // PTS 1125307
#include "geo573.h"

DECLSPEC_EXPORT HRESULT co90AttachSqlSession(void* sql_handle);

DECLSPEC_EXPORT HRESULT co90AttachDefaultContext(void* oms_handle);

DECLSPEC_EXPORT void co90CheckVersion (IUnknown**     pIKernelSink, 
                                       const char*    pLCAppsVersion,
                                       const char*    pDummy1,
                                       const char*    pDummy2,
                                       const char*    pDummy3);

DECLSPEC_EXPORT void co90SysRequest   (void*          oms_handle, 
                                       void*          sql_handle,
                                       int*           lpbuflen, 
                                       unsigned char* lpv );

DECLSPEC_EXPORT HRESULT co90RegisterComObject (PSZ  pszProgIDPfx,
                                               PSZ  pszProject, 
                                               PSZ  pszVersion,
                                               const GUID &ClsID,
                                               PSZ  pszModuleName);

DECLSPEC_EXPORT HRESULT co90ReleaseSqlSession(void* sql_handle);

DECLSPEC_EXPORT HRESULT co90ReleaseDefaultContext(void* oms_handle);

DECLSPEC_EXPORT HRESULT co90UnregisterComObject (PSZ  pszProgIDPfx,
                                                 PSZ  pszProject, 
                                                 PSZ  pszVersion,
                                                 const GUID &ClsID,
                                                 PSZ  pszModuleName);
/*PTS 1103516 param numEntries inserted FF 5-Aug-1999*/
/* PTS 1111008 
DECLSPEC_EXPORT void co90MonitorInit (void* oms_handle, void* sql_handle, void** lpMonitor, long numEntries);
*/

DECLSPEC_EXPORT void co90MonitorInit (void* oms_handle, void* sql_handle, CLSID* lpIID, void** lpMonitor, long numEntries);

/* PTS 1111008 */
DECLSPEC_EXPORT void co90MonitorInit (void* oms_handle, void* sql_handle, const IID* IId, void** lpMonitor, long numEntries);

DECLSPEC_EXPORT void co90MonitorProlog (void* lpMonitor, void* oms_handle, int dispId, unsigned char* lpmethodName);

DECLSPEC_EXPORT void co90MonitorEpilog (void* lpMonitor, void* oms_handle, void* sql_handle, int dispId, int runTime);     /* PTS 1120794 */

/* PTS 1120794 */
DECLSPEC_EXPORT void co90SqlMonitorProlog (void* sql_handle, unsigned char* lpmethodName);

DECLSPEC_EXPORT void co90MonitorDelete (void* lpMonitor);

DECLSPEC_EXPORT void co90Vtrace (IUnknown* psession, long buflen, unsigned char *lpv);

DECLSPEC_EXPORT LONG STDAPICALLTYPE co90InterlockedIncrement( LPLONG lpAddend );
DECLSPEC_EXPORT LONG STDAPICALLTYPE co90InterlockedDecrement( LPLONG lpAddend );

/*PTS 1105154 FF*/
DECLSPEC_EXPORT HRESULT co90CleanUp (void* oms_handle, void* sql_handle ); 

/*---------------- PTS 1116802 FF             --------------------------*/

static RTESync_Spinlock co90RefCounterLock;

/*----------------------------------------------------------------------*/

void co90Vtrace (IUnknown* psession, long buflen, unsigned char *lpv)
{
  ((IliveCacheSink*)psession)->Vtrace (buflen, lpv);
}

/*----------------------------------------------------------------------*/

HRESULT co90AttachSqlSession(void* sql_handle)
{
  return REINTERPRET_CAST(SqlHandle*,(sql_handle))->AdviseSqlSession();
}

/*----------------------------------------------------------------------*/

HRESULT co90AttachDefaultContext(void* oms_handle)
{
  return reinterpret_cast<OmsHandle*>(oms_handle)->omsAttachDefaultContext();
}

/*----------------------------------------------------------------------*/

HRESULT co90ReleaseSqlSession(void* sql_handle)
{
  return REINTERPRET_CAST(SqlHandle*,(sql_handle))->UnAdviseSqlSession();
}

/*----------------------------------------------------------------------*/

HRESULT co90ReleaseDefaultContext(void* oms_handle)
{
  return reinterpret_cast<OmsHandle*>(oms_handle)->omsReleaseDefaultContext ();
}

/*----------------------------------------------------------------------*/

void co90CheckVersion (IUnknown** ppIliveCacheSink,
                       const char* pLCAppsVersion,
                       const char* pDummy1,
                       const char* pDummy2,
                       const char* pDummy3)
{

  if (pLCAppsVersion) {
    char pLCVersion[] = "<MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>";

    if (strcmp(pLCVersion, pLCAppsVersion)) {
      char  errmsg[128];
      sp77sprintf(errmsg, sizeof(errmsg), "Incompatible Version: LiveCache: %s, LC-Apps: %s",
        pLCVersion, pLCAppsVersion);

      IliveCacheSink* pLCSink = reinterpret_cast<IliveCacheSink*>(*ppIliveCacheSink);
      DbpBase base(pLCSink);
      base.dbpOpError(errmsg);

      pasbool         *pToCancel;
      tsp00_TaskId     taskId;
      OMS_Session     *pSession;
      bool             optimizedStreamCommunication;
      pLCSink->GetDefaultContext(reinterpret_cast<void**>(&pSession), &pToCancel, optimizedStreamCommunication, taskId);
      if (pSession != 0){
        OmsHandle *pHandle = pSession->m_handleList.front();
        pHandle->omsTerminate(errmsg);
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void co90SysRequest (void* oms_handle, void* sql_handle,
                     int* lpbuflen, unsigned char* lpv)
{  
  tgg00_MessType *messType = REINTERPRET_CAST(tgg00_MessType*, (lpv));
 
  switch (*messType) {
  case m_return_error:  // Check whether session is marked as corrupt
    {
      bool corrupt = reinterpret_cast<OmsHandle*>(oms_handle)->CheckSession();
      if (!corrupt){
        // Reset error code, to signal, that session is ok
        lpv[0] = 0;
      }
      else {
        // Keep error code, to signal, that session is corrupt
      }
    }
    break;

  case m_commit :
    *REINTERPRET_CAST(short*, (lpv)) = 0;
    if ( oms_handle ) {
      *REINTERPRET_CAST(short*, (lpv)) = 
        reinterpret_cast<OmsHandle*>(oms_handle)->omsCommitHandling();
    }
    *lpbuflen = sizeof(short);
    if ( sql_handle ) {
       (REINTERPRET_CAST(SqlHandle*,(sql_handle)))->sqlResetSession();
    }
    break;
  /* change request 1000139, T.A. 14.10.1998 */
  case m_diagnose :
    if (oms_handle) {
      reinterpret_cast<OmsHandle*>(oms_handle)->omsGetMonitorInfo(
        REINTERPRET_CAST(tgg01_COMMonitorTransferBuf*,lpv));
      *lpbuflen = sizeof (tgg01_COMMonitorTransferBuf);
    }
    else {
      *lpbuflen = 0;
    }
    break;
  case m_rollback :
    (*REINTERPRET_CAST(short*, (lpv))) = 0;
    if ( oms_handle )
      reinterpret_cast<OmsHandle*>(oms_handle)->omsRollbackHandling(false, false);
    *lpbuflen = sizeof(short);
    if ( sql_handle ) {
       (REINTERPRET_CAST(SqlHandle*,(sql_handle)))->sqlResetSession();
    }
    break;
  case m_shutdown :
    (*REINTERPRET_CAST(short*, (lpv))) = 0;
    if ( oms_handle ) {
      short res;
      try {
        reinterpret_cast<OmsHandle*>(oms_handle)->omsValidate();
        res = 0;
      } 
      catch (DbpError &e) { // PTS 1127871
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
        if (pCBInterface){
          pCBInterface->dbpCaughtError(e);
        }
        res = -9999;
      } 
      catch (BAD_ALLOC_GEO573&) { // PTS 1127871
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
        if (pCBInterface){
          pCBInterface->dbpCaughtBadAlloc();
        }
        res = -9999;
      } 
      (*REINTERPRET_CAST(short*, (lpv))) = res;
    }
    *lpbuflen = sizeof(short);
    if ( sql_handle ) {
       (REINTERPRET_CAST(SqlHandle*,(sql_handle)))->sqlResetSession();
    }
    break;

  // Callback after kernel commit resp. rollback
  case m_afterCommit : 
  case m_afterRollback:
    tgg00_MessType2 *messType2 = reinterpret_cast<tgg00_MessType2*>(lpv+1);

    bool isCommit = (*messType  == m_afterCommit);
    bool release  = (*messType2 == mm_close); 

    *reinterpret_cast<short*>(lpv) = 0;

    if ( oms_handle ) {
      short res = 0;
      int phase = 0;
      try{
        reinterpret_cast<OmsHandle*>(oms_handle)->omsTransEndEpilog(isCommit, release, phase);
      }
      catch (DbpError &e) { // PTS 1127871
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
        if (pCBInterface){
          pCBInterface->dbpCaughtError(e);
        }

        if (isCommit){
          char buf[1024];
          sp77sprintf(buf, sizeof(buf), "omsTransEndEpilog(COMMIT): DbpError %d (%s) in phase %d", 
            e.dbpError(), e.m_errorText, phase);
          reinterpret_cast<OmsHandle*>(oms_handle)->dbpOpError (buf);

          // Set the corrupt-flag in the session, so that the next call in this session will propagate
          // an error to the application (see m_corrput in OMS_Session)
          reinterpret_cast<OmsHandle*>(oms_handle)->SetSessionCorrupt();

          // Reset error, so that session will not be terminated.
          res = 0;
        }
        else {
          char buf[1024];
          sp77sprintf(buf, sizeof(buf), "omsTransEndEpilog(ROLLBACK): DbpError %d (%s) in phase %d", 
            e.dbpError(), e.m_errorText, phase);
          reinterpret_cast<OmsHandle*>(oms_handle)->dbpOpError (buf);

          res = -9999;
        }
      } 
      catch (BAD_ALLOC_GEO573&) { // PTS 1127871
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
        if (pCBInterface){
          pCBInterface->dbpCaughtBadAlloc();
        }

        if (isCommit){
          char buf[1024];
          sp77sprintf(buf, sizeof(buf), "omsTransEndEpilog(COMMIT): Bad-Alloc in phase %d", phase);
          reinterpret_cast<OmsHandle*>(oms_handle)->dbpOpError (buf);

          // Set the corrupt-flag in the session, so that the next call in this session will propagate
          // an error to the application (see m_corrput in OMS_Session)
          reinterpret_cast<OmsHandle*>(oms_handle)->SetSessionCorrupt();

          // Reset error, so that session will not be terminated.
          res = 0;
        }
        else {
          char buf[1024];
          sp77sprintf(buf, sizeof(buf), "omsTransEndEpilog(ROLLBACK): Bad-Alloc in phase %d", phase);
          reinterpret_cast<OmsHandle*>(oms_handle)->dbpOpError (buf);

          res = -9999;
        }
      } 
      *reinterpret_cast<short*>(lpv) = res;
    }
    *lpbuflen = sizeof(short);
    break;
  }
};

/*----------------------------------------------------------------------*/

HRESULT co90RegisterComObject (PSZ  pszProgIDPfx,
                               PSZ  pszProject, 
                               PSZ  pszVersion,
                               const GUID &ClsID,
                               PSZ  pszModuleName)
                               
{
/* functionality obsolete PTS 1124546 FF                */
/* therefore only DUMMY implementation is provided here */
#if !defined (WIN32)
    fprintf (stdout, "co90RegisterComObject: Registration no longer needed/supported ! \n");
#endif
    return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);

/* PTS 1124546 FF */
#if defined(OBSOLETE)
  teo78_ClsID   szClsID;
  teo78_Result  rc;

  if (( strlen(pszProgIDPfx) > sizeof( teo78_ProgIDPfx )) || 
      ( strlen(pszProject) > sizeof( teo78_Project )) || 
      ( strlen(pszVersion) > sizeof( teo78_Version )) || 
      ( strlen(pszModuleName) > sizeof( teo78_ModuleName )))
  {
#if !defined (WIN32)
    fprintf (stdout, "co90RegisterComObject one of the following parameters is too long\n");
    fprintf (stdout, "co90RegisterComObject Prefix: %s max: %d\n",pszProgIDPfx, sizeof( teo78_ProgIDPfx ));
    fprintf (stdout, "co90RegisterComObject Project: %s max: %d\n",pszProject, sizeof( teo78_Project ));
    fprintf (stdout, "co90RegisterComObject Version: %s max: %d\n",pszVersion, sizeof( teo78_Version ));
    fprintf (stdout, "co90RegisterComObject Path: %s max: %d\n",pszModuleName, sizeof( teo78_ModuleName ));
#endif
    return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);
  }

  //sprintf(szClsID,"{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}",
  //          ClsID.Data1, ClsID.Data2, ClsID.Data3,
  //          ClsID.Data4[0], ClsID.Data4[1], ClsID.Data4[2],
  //          ClsID.Data4[3], ClsID.Data4[4], ClsID.Data4[5],
  //          ClsID.Data4[6], ClsID.Data4[7] );                     // PTS 1125307
  sp77sprintf(szClsID,"{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}",
            ClsID.Data1, ClsID.Data2, ClsID.Data3,
            ClsID.Data4[0], ClsID.Data4[1], ClsID.Data4[2],
            ClsID.Data4[3], ClsID.Data4[4], ClsID.Data4[5],
            ClsID.Data4[6], ClsID.Data4[7] );                       // PTS 1125307


  rc = sqlComObjectRegister( pszProgIDPfx,
    pszProject,
    pszVersion,
    szClsID,
    pszModuleName );
  if ( success_eo78 == rc ) 
  {
#if defined (WIN32)
    SetLastError(S_OK);
#endif
    return S_OK;
  }
  else
  {
#if defined (WIN32)
    if ( system_error_eo78 == rc ) 
    {
      if ( 0 > GetLastError() )
        return GetLastError();
      else
        return MAKE_HRESULT(1,0,GetLastError());
    }
    else
#endif
    {
#if !defined (WIN32)
      switch ( rc ) {
      case illegal_parameter_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'illegal_parameter_eo78'\n"); 
        fprintf (stdout, "co90RegisterComObject Prefix: %s \n",pszProgIDPfx);
        fprintf (stdout, "co90RegisterComObject Project: %s \n",pszProject);
        fprintf (stdout, "co90RegisterComObject Version: %s \n",pszVersion);
        fprintf (stdout, "co90RegisterComObject Path: %s \n",pszModuleName);
        fprintf (stdout, "co90RegisterComObject ClsID: %s \n",szClsID);
        break;
      case access_denied_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'access_denied_eo78'\n"); break;
      case doubled_clsid_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'doubled_clsid_eo78'\n"); 
        fprintf (stdout, "co90RegisterComObject ClsID: %s \n",szClsID);
        break;
      case entry_not_found_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'entry_not_found_eo78'\n"); break;
      case clsid_version_mismatch_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'clsid_version_mismatch_eo78'\n"); break;
      case system_error_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'system_error_eo78'\n"); break;
      }
#endif
      return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);
    }
  }
#endif
}

/*----------------------------------------------------------------------*/

HRESULT co90UnregisterComObject (PSZ  pszProgIDPfx,
                                 PSZ  pszProject, 
                                 PSZ  pszVersion,
                                 const GUID &ClsID,
                                 PSZ  pszModuleName)
{
/* functionality obsolete PTS 1124546 FF                */
/* therefore only DUMMY implementation is provided here */
#if !defined (WIN32)
    fprintf (stdout, "co90RegisterComObject: Registration no longer needed/supported ! \n");
#endif
    return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);

/* PTS 1124546 FF */   
#if defined(OBSOLETE)
  teo78_ClsID   szClsID;
  teo78_Result  rc;

  if (( strlen(pszProgIDPfx) > sizeof( teo78_ProgIDPfx )) || 
      ( strlen(pszProject) > sizeof( teo78_Project )) || 
      ( strlen(pszVersion) > sizeof( teo78_Version )))
  {
#if !defined (WIN32)
    fprintf (stdout, "co90UnregisterComObject one of the following parameters is too long\n");
    fprintf (stdout, "co90UnregisterComObject Prefix: %s max: %d\n",pszProgIDPfx, sizeof( teo78_ProgIDPfx ));
    fprintf (stdout, "co90UnregisterComObject Project: %s max: %d\n",pszProject, sizeof( teo78_Project ));
    fprintf (stdout, "co90UnregisterComObject Version: %s max: %d\n",pszVersion, sizeof( teo78_Version ));
#endif
    return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);
  }

  //sprintf(szClsID,"{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}",
  //          ClsID.Data1, ClsID.Data2, ClsID.Data3,
  //          ClsID.Data4[0], ClsID.Data4[1], ClsID.Data4[2],
  //          ClsID.Data4[3], ClsID.Data4[4], ClsID.Data4[5],
  //          ClsID.Data4[6], ClsID.Data4[7] );
  sp77sprintf(szClsID,sizeof(szClsID),"{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}",
            ClsID.Data1, ClsID.Data2, ClsID.Data3,
            ClsID.Data4[0], ClsID.Data4[1], ClsID.Data4[2],
            ClsID.Data4[3], ClsID.Data4[4], ClsID.Data4[5],
            ClsID.Data4[6], ClsID.Data4[7] );

  rc = sqlComObjectUnregister( pszProgIDPfx,
    pszProject,
    pszVersion,
    szClsID);

  if ( success_eo78 == rc ) 
  {
    return S_OK;
  }
  else
  {
#if defined (WIN32)
    if ( system_error_eo78 == rc ) 
    {
      if ( 0 > GetLastError() )
        return GetLastError();
      else
        return MAKE_HRESULT(1,0,GetLastError());
    }
    else
#endif
    {
#if !defined (WIN32)
      switch ( rc ) {
      case illegal_parameter_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'illegal_parameter_eo78'\n");
        fprintf (stdout, "co90UnregisterComObject Prefix: %s \n",pszProgIDPfx);
        fprintf (stdout, "co90UnregisterComObject Project: %s \n",pszProject);
        fprintf (stdout, "co90UnregisterComObject Version: %s \n",pszVersion);
        fprintf (stdout, "co90UnregisterComObject ClsID: %s \n",szClsID);
        break;
      case access_denied_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'access_denied_eo78'\n"); 
        break;
      case doubled_clsid_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'doubled_clsid_eo78'\n"); 
        break;
      case entry_not_found_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'entry_not_found_eo78'\n");
        fprintf (stdout, "co90UnregisterComObject Prefix: %s \n",pszProgIDPfx);
        fprintf (stdout, "co90UnregisterComObject Project: %s \n",pszProject);
        fprintf (stdout, "co90UnregisterComObject Version: %s \n",pszVersion);
        fprintf (stdout, "co90UnregisterComObject ClsID: %s \n",szClsID);
        break;
      case clsid_version_mismatch_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'clsid_version_mismatch_eo78'\n"); 
        break;
      case system_error_eo78:
        fprintf (stdout, "sqlComObjectRegister error: 'system_error_eo78'\n"); 
        break;
      }
#endif
      return MAKE_HRESULT(1,0,ERROR_INVALID_PARAMETER);
    }
  }
#endif
};

/*----------------------------------------------------------------------*/

LONG STDAPICALLTYPE co90InterlockedIncrement( LPLONG lpAddend )
{
    /* PTS 1116802 FF */
    /* reference counting has to be synchronized because of oms_monitor */
    /* calls using instances in parallel                                */
    LONG val;
    co90RefCounterLock.Lock();
    ++(*lpAddend);
    val = *lpAddend;
    co90RefCounterLock.Unlock();
    return val;
};

/*----------------------------------------------------------------------*/

LONG STDAPICALLTYPE co90InterlockedDecrement( LPLONG lpAddend )
{
    /* PTS 1116802 FF */
    /* reference counting has to be synchronized because of oms_monitor */
    /* calls using instances in parallel                                */
    LONG val;
    co90RefCounterLock.Lock();
    --(*lpAddend);
    val = *lpAddend;
    co90RefCounterLock.Unlock();
    return val;
};

/*----------------------------------------------------------------------*/


/* change request 1000139, T.A. 14.10.1998 */
/* PTS 1111008 
void co90MonitorInit (void* oms_handle, void* sql_handle, void** lpMonitor, long numEntries) {
  CLSID iid;
  memset (&iid, 0, sizeof(iid));
  co90MonitorInit(oms_handle,  sql_handle, &iid, lpMonitor, numEntries);
}
*/
/*----------------------------------------------------------------------*/

void co90MonitorInit (void* oms_handle, void* sql_handle, CLSID* lpIID, void** lpMonitor, long numEntries)
{ 
#if defined(OSF1)     /* PTS 1112275 FF */
  numEntries = 4;     /* PTS 1112275 FF because of dlopen doesn't support RTLD_LOCAL on OSF1 numEntries   */
#endif                /* PTS 1112275 FF may be false  ( 4 == IUnknown methods + 1 user method == minimum) */
  *lpMonitor = REINTERPRET_CAST(void*, new OMS_COMMonitor(*lpIID, numEntries)); /*PTS 1103516 FF*/
  if (oms_handle) {
    reinterpret_cast<OmsHandle*>(oms_handle)->omsRegMonitor(*lpMonitor);
  }
}

/*----------------------------------------------------------------------*/

void co90MonitorInit (void* oms_handle, void* sql_handle, const IID* IId, void** lpMonitor, long numEntries)
{ 
#if defined(OSF1)     /* PTS 1112275 FF */
  numEntries = 4;     /* PTS 1112275 FF because of dlopen doesn't support RTLD_LOCAL on OSF1 numEntries   */
#endif                /* PTS 1112275 FF may be false  ( 4 == IUnknown methods + 1 user method == minimum) */
  *lpMonitor = REINTERPRET_CAST(void*, new OMS_COMMonitor(*(CLSID*)IId, numEntries)); 
  if (oms_handle) {
    reinterpret_cast<OmsHandle*>(oms_handle)->omsRegMonitor(*lpMonitor);
  }
}

/*----------------------------------------------------------------------*/

void OMS_COMMonitor::omsSetCurrentMonitorMethod(OmsHandle& h)
{
  h.m_pSession->SetCurrentMonitorMethod(getCurrent());
}

/*----------------------------------------------------------------------*/

void co90MonitorProlog (void* lpMonitor, void* oms_handle, int dispId, unsigned char* lpMethodName)
{
  OMS_COMMonitor* pMon = REINTERPRET_CAST(OMS_COMMonitor*, lpMonitor);
  if ( dispId >= pMon->size() ) {                                 /* PTS 1112275 FF */
//    REINTERPRET_CAST(OmsHandle *, oms_handle)->omsMonitorLock();  /* PTS 1112275 FF */
    pMon->increaseInfoArray( dispId + 1 );                        /* PTS 1112275 FF */
//    REINTERPRET_CAST(OmsHandle *, oms_handle)->omsMonitorUnlock();/* PTS 1112275 FF */
  }                                                               /* PTS 1112275 FF */
  if ( dispId < pMon->size() ) {                                  /* PTS 1112275 FF */
    if (pMon) {
      pMon->setCurrent(dispId, lpMethodName);     /*PTS 1103516 FF*/
      if (oms_handle) {
        pMon->omsSetCurrentMonitorMethod(*reinterpret_cast<OmsHandle*>(oms_handle));
        REINTERPRET_CAST(OmsHandle*,oms_handle)->omsSetCurrentMethod(lpMethodName); /* PTS 1135058, PG */
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_COMMonitor::omsMethodCallEpilogWrapper(OmsHandle& h, void* sql_handle, int runTime)     /* PTS 1120794 */
{
  h.m_pSession->CurrentMethodCallEpilog(runTime);
  OMS_Session::MethodCallEplilogScope epilogScope(*h.m_pSession);
  /* only in this scope a call of omsGetMethodCallMonitorInfo returns any result */
  h.omsMethodCallEpilog(sql_handle);     /* PTS 1120794 */
}

/*----------------------------------------------------------------------*/

void co90MonitorEpilog (void* lpMonitor, void* oms_handle, void* sql_handle, int dispId, int runTime) {     /* PTS 1120794 */
  OMS_COMMonitor* pMon = REINTERPRET_CAST(OMS_COMMonitor*, lpMonitor);
  if (NULL != pMon) {
    if ( dispId < pMon->size() ) {                                  /* PTS 1112275 FF */
      pMon->callEpilog(dispId, runTime);
      if (NULL != oms_handle) {
        /* PTS 1120794 */
        void* pSH = NULL;
        if (NULL != sql_handle) {
          REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlSetMethodCallEpilog();
          REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlSetRuntime(runTime);
          pSH = REINTERPRET_CAST(SqlHandle*,sql_handle);
        }
        OMS_COMMonitor::omsMethodCallEpilogWrapper( *REINTERPRET_CAST(OmsHandle*,oms_handle), pSH, runTime );
        if (NULL != sql_handle) {
          REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlResetMethodCallEpilog();
          REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlResetCurrMethod();
        }
        REINTERPRET_CAST(OmsHandle*,oms_handle)->omsResetCurrentMethod(); /* PTS 1135058, PG */
      }
    }
  }
};

/*----- PTS 1120794 ----------------------------------------------------*/

void co90SqlMonitorProlog (void* sql_handle, unsigned char* lpMethodName) {
  if (sql_handle) {
    REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlSetCurrMethod(lpMethodName);
    REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlResetMonitor();
    REINTERPRET_CAST(SqlHandle*,sql_handle)->sqlSetDefaultHandler();
  }
}

/*----------------------------------------------------------------------*/

void co90MonitorDelete (void* lpMonitor)
{
  OMS_COMMonitor* pMon = REINTERPRET_CAST(OMS_COMMonitor*, lpMonitor);
  if (pMon) {
    delete pMon;
  }
}

/*---------------------- PTS 1105154 -----------------------------------*/

HRESULT co90CleanUp (void* oms_handle, void* sql_handle )
{
  HRESULT hr = S_OK;
  if ( NULL != oms_handle ) {
    reinterpret_cast<OmsHandle*>(oms_handle)->omsCleanUpAfterCrash();
  }
  return hr;
}; 
