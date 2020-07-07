/*!
 * \file    LVC_InProcDispatcher.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher for kernel.
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

#define USE_KGS

#include "liveCache/LVC_InProcDispatcher.hpp"
#include "liveCache/LVC_ProcServerExtender.hpp"
#include "liveCache/LVC_LoadComponentLibrary.hpp"   /* PTS 1121178 FF */
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "liveCache/LVC_DispatcherParams.hpp"
#include "liveCache/LVC_LoadLibrary.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "Oms/OMS_Defines.h"

#include "heo38.h"
#include "hsp77.h"
#include "geo50_0.h"
#ifdef _WIN32
extern "C" void vabort(teo00_Bool);
#else
#include "hen50.h"    // nocheck
#endif
#include "geo00d.h"

#include "heo01.h"
#include "heo52.h"
#include "heo58.h" //vGetAcvPtrFromCurrentTask
#include "heo15.h"
#include "heo55k.h"  // vbegexcl, vendexcl
#include "geo60.h"  
#include "geo007_1.h"  
#include "hgg08.h"  
#include "gsp92.h"
#include "heo922.h"
#include "hgg01.h"
#include "hgg01_3.h"



/* +++ *** PTS 1112617 *** +++ */
static tsp00_Uint4 LVC_InProcDispatcherThreadIndex( TASK_CTRL p ) {
#if defined(WIN32)                                              
  return (tsp00_Uint4) p->ulTaskIndex;
#else 
  return (tsp00_Uint4) THREAD_INDEX(p);
#endif
}
/* --- *** PTS 1112617 *** --- */

/*===========================================================================*
 *  Implementation of functions referenced from Pascal code:                 *
 *===========================================================================*/

externC void vdcom_iid_ilivecachespec ( tsp00_C16* pIId )
{
  // copy livecache GUID
  memcpy(pIId, &IID_IliveCacheSpec, sizeof (IID_IliveCacheSpec));
}

/*---------------------------------------------------------------------------*/

externC void vdcom_co_get_classobject (void *pInstance )
{
  // called after dispaddr
  ROUTINE_DBG_MEO60 ("vdcom_co_get_classobject");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispGetClassObjectWrapper(reinterpret_cast<tsp_dcom_co_create_instance_parms*>(pInstance));
}

/*---------------------------------------------------------------------------*/

externC void vdcom_paraminfo (void *pParamInfo )
{
  // TODO
  ROUTINE_DBG_MEO60 ("vdcom_paraminfo");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispGetParamInfoWrapper(reinterpret_cast<tsp_dcom_paraminfo_parms*>(pParamInfo));
}

/*---------------------------------------------------------------------------*/

externC void vdcom_create_instance ( void *pInstance )
{
  // called after getclassobject
  ROUTINE_DBG_MEO60 ("vdcom_create_instance");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispCreateInstance((tsp_dcom_co_create_instance_parms*) pInstance);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_release_instance ( void *pParamInfo )
{
  ROUTINE_DBG_MEO60 ("vdcom_release_instance");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispReleaseInstanceWrapper((tsp_dcom_release_instance_parms*) pParamInfo);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_release_all ( )
{
  ROUTINE_DBG_MEO60 ("vdcom_release_all");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispReleaseAllWrapper();
}

/*---------------------------------------------------------------------------*/

externC void vdcom_dbproc ( void *pParamInfo )
{
  // called after create instance, in transaction end
  ROUTINE_DBG_MEO60 ("vdcom_dbproc");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispDispatchDBProcWrapper((tsp_dcom_dispatch_parms*) pParamInfo);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_trans_end ( void *pParamInfo )
{
  // called after dbproc
  ROUTINE_DBG_MEO60 ("vdcom_trans_end");
  // if ( !g01is_livecache() ) return;   /* PTS 1120585 2003-02-13 FF */ PTS 1128483
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispTransEndWrapper((tsp_dcom_dispatch_parms*) pParamInfo);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_get_dispaddr ( void *pInstance)
{
  // called after paraminfo
  ROUTINE_DBG_MEO60 ("vdcom_get_dispaddr");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispGetDispAddrWrapper((tsp_dcom_co_create_instance_parms*) pInstance);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_init_sink ( void *pInstance )
{
  ROUTINE_DBG_MEO60 ("vdcom_init_sink");

  tsp_dcom_init_sink_parms *parms = reinterpret_cast<tsp_dcom_init_sink_parms*>(pInstance);
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispCreateSink(parms->sink_addr, parms->sink_size, parms->acv_addr);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_invalidate ( void *pInstance )
{
  ROUTINE_DBG_MEO60 ("vdcom_invalidate");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispInvalidateWrapper((tsp_dcom_co_create_instance_parms*) pInstance);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_monitor_open ( tsp_dcom_monitor_parms * parms )
{
  ROUTINE_DBG_MEO60 ("vdcom_monitor_open");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispMonitorOpenWrapper(parms);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_monitor_next ( tsp_dcom_monitor_parms * parms )
{
  ROUTINE_DBG_MEO60 ("vdcom_monitor_next");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispMonitorNextWrapper(parms);
}

/*---------------------------------------------------------------------------*/

externC void vdcom_monitor_close ( tsp_dcom_monitor_parms * parms )
{
  ROUTINE_DBG_MEO60 ("vdcom_monitor_close");
  LVC_InProcDispatcher &disp = LVC_InProcDispatcher::Instance();
  disp.dispMonitorCloseWrapper(parms);
}


/*===========================================================================*
*  END OF Pascal support routines                                           *
*===========================================================================*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/// Space for in-proc dispatcher instance.
static double LVC_InProcDispatcherInstanceSpace[sizeof(LVC_InProcDispatcher) / sizeof(double) + 1];

/// Lock for in-proc dispatcher.
static RTESync_Spinlock LVC_InProcDispatcherInstanceLock;

// Default dispatcher instance.
LVC_InProcDispatcher *LVC_InProcDispatcher::m_instance;

/*---------------------------------------------------------------------------*/

LVC_InProcDispatcher &LVC_InProcDispatcher::Instance()
{
  if (!m_instance) {
    RTESync_LockedScope lck(LVC_InProcDispatcherInstanceLock);
    if (!m_instance) {
      m_instance = new(LVC_InProcDispatcherInstanceSpace)
        LVC_InProcDispatcher(LVCMem_Allocator::Instance());
    }
  }
  return *m_instance;
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispDispatchDBProcWrapper(tsp_dcom_dispatch_parms *params)
{
  tsp00_C4          session;
  CIliveCacheSink*  pIliveCacheSink;
  tsp1_packet*      SQLBuffer_ptr;

  dispGetSink(REINTERPRET_CAST(tak_all_command_glob*, params->session_context),
    *REINTERPRET_CAST(tgg91_SessionNo*, &session),
    *REINTERPRET_CAST(tsp00_Addr*, &pIliveCacheSink),
    SQLBuffer_ptr );
  pIliveCacheSink->CallbackIsActive = false;
  pIliveCacheSink->sqlErrorFlag     = false;   /* PTS 1110814 FF */


  tsp00_KnlIdentifier *method_name_ascii;
  tsp00_KnlIdentifier asciiBuffer;
  if (g01unicode) {
    // convert method name to ASCII
    tsp00_KnlIdentifier *method_name = &params->method_name;
    const tsp81_UCS2Char *convEndUTF16;
    SAPDB_UTF8Basis::UTF8Pointer convEnd;
    SAPDB_UTF8Basis::KernelConvertFromUTF16(reinterpret_cast<const tsp81_UCS2Char*>(method_name), 
      reinterpret_cast<const tsp81_UCS2Char*>(method_name + 1), convEndUTF16, false,
      (const SAPDB_UTF8Basis::UTF8Pointer) &asciiBuffer,
      ((const SAPDB_UTF8Basis::UTF8Pointer) &asciiBuffer) + sizeof(asciiBuffer), convEnd);
    if (convEnd < asciiBuffer + sizeof(asciiBuffer)) {
      *convEnd = 0;
    } else {
      asciiBuffer[sizeof(asciiBuffer) - 1] = 0;
    }
    method_name_ascii = &asciiBuffer;
  } else {
    method_name_ascii = &params->method_name;
  }

  if (!params->run_in_ude_server){
    // inproc execution of the procedure

    tsp92_RteTaskDetailsInfo taskInfo;  // needed for monitor
    params->hresult = 0;

    params->hresult = dispDispatch(
      pIliveCacheSink, (IUnknown*) params->idispatch, params->method_name, *method_name_ascii,
      params->dispid, params->param_stack_len/sizeof(tak_dbproc_stack_param_debug),
      REINTERPRET_CAST(tak_dbproc_stack_param_debug*,params->param_stack),
      params->monitoring != 0, params->monitor_slot_id, params, &taskInfo);

    if (( GEO00D_DCOM_DBPROC_CRASHED == params->hresult ) || ( GEO00D_DCOM_TERMINATED == params->hresult )) {
      dispDisposeInstances(*REINTERPRET_CAST(tsp00_Uint4*, &session),
        params->session_context, params);
      pIliveCacheSink->lpDefaultContext = NULL;
      /* PTS 1120794 */
      if (NULL != pIliveCacheSink->lpSqlSessionContext) {
        MSGD ((ERR_DCOM_DUMP_INFO,"dispDispatchDBProcWrapper: lpSqlSessionContext <> NULL"));
        pIliveCacheSink->lpSqlSessionContext = NULL;
      }
    }
    else 
    {
      if (pIliveCacheSink->sqlErrorFlag)
        params->hresult = -1;   /* PTS 1110814 FF  */
    }
  }
  else {
    // execute procedure in ude-server
    LVC_ProcServerExtender *pProcServerExtender = GetUDEServerExtender(pIliveCacheSink);
    if (!pProcServerExtender){
      // Create new UDE server extender and store address in sink for next access    
      pProcServerExtender = new(LVCMem_Allocator::Instance().Allocate(sizeof(LVC_ProcServerExtender)))
                               LVC_ProcServerExtender(&LVCMem_Allocator::Instance(), pIliveCacheSink);
      if (pProcServerExtender){
        SetUDEServerExtender(pIliveCacheSink, pProcServerExtender);
      }
      else {
        // TODO: error-handling
        return;
      }  
    }

    CLSID coclsid;
    IID   iid;
    memcpy(&coclsid, &params->coclsid[0], sizeof(CLSID));   
    memcpy(&iid,     &params->iid[0],     sizeof(IID));   

    // Call UDE server extender to execute procedure
    short err = 0;
    pProcServerExtender->StartInUDEServer(
      params->param_stack_len/sizeof(tak_dbproc_stack_param_debug),
      REINTERPRET_CAST(tak_dbproc_stack_param_debug*,params->param_stack),
      params->library_name,
      params->MD5_footprint,
      coclsid,
      iid,
      params->dispid,
      params->method_name, 
      *method_name_ascii,
      err);
    if (err == 0){
      params->hresult = 0;
    }
    else {
      params->hresult = -1;
    }
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispTransEndWrapper(tsp_dcom_dispatch_parms *params)
{
  tsp00_Uint4       session;
  CIliveCacheSink*  pIliveCacheSink;
  tsp1_packet*      SQLBuffer_ptr;

  dispGetSink(REINTERPRET_CAST(tak_all_command_glob*, params->session_context),
    *REINTERPRET_CAST(tgg91_SessionNo*, &session),
    *REINTERPRET_CAST(tsp00_Addr*, &pIliveCacheSink),
    SQLBuffer_ptr );

  tsp92_RteTaskDetailsInfo taskInfo;  // needed for monitor
  params->hresult = 0;

  tsp00_KnlIdentifier methodNameAscii;
  methodNameAscii.c2p("Transaction End");

  // convert methodname to unicode if unicode is active
  tsp00_KnlIdentifier methodNameUni;
  const char* name = "Transaction End";
  int len = strlen(name) * 2;
  for (int i = 0; i < sizeof(methodNameUni); i += 2) {
    methodNameUni[i]   = 0;
    methodNameUni[i+1] = (i < len && g01unicode) ? name[i/2] : ' ';
  }

  params->hresult = dispTransEnd(pIliveCacheSink, 
    session, params->session_context,
    reinterpret_cast<LVC_DispatcherTransEnd*>(params->param_stack),
    params->monitoring != 0, params->monitor_slot_id, params, &taskInfo,
    methodNameAscii, methodNameUni);
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispInvalidateWrapper(tsp_dcom_co_create_instance_parms *parms)
{
  dispInvalidate(*reinterpret_cast<const GUID*>(&parms->coclsid[0]),
    *reinterpret_cast<const IID*>(&parms->iid[0]));
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispGetDispAddrWrapper(tsp_dcom_co_create_instance_parms *parms)
{
  parms->ret = RC_DISPADDR_FAILED;
  parms->idispatch = 0;
  parms->ifactory = 0;

  CIliveCacheSink* pSink = reinterpret_cast<CIliveCacheSink*>(parms->ikernel_sink);

  if (parms->force_run_in_kernel || !UseUDEServer(pSink)){
    parms->run_in_ude_server = false;
    parms->ret = dispGetDispAddr(*reinterpret_cast<const GUID*>(&parms->coclsid[0]),
      *reinterpret_cast<const IID*>(&parms->iid[0]), 
      *reinterpret_cast<tsp00_Uint4*>(&parms->session[0]),
      parms->idispatch, parms->ifactory, parms);
  }
  else {
    parms->run_in_ude_server = true;
    parms->ret = RC_DISPADDR_OK;
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispMonitorOpenWrapper(tsp_dcom_monitor_parms * parms)
{
  LVC_DispatcherCOMClassHashPtr         HashPtr;
  LVC_DispatcherHashEntryPtr           currSessEntry, newSessEntry;
  IUnknown*               lpIUnknown;
  HRESULT                 hr;
  tsp00_TaskId            taskid;

  vgetpid(taskid);
  vbegexcl(taskid, g08ComponentEnd );

  parms->session_handle   = NULL;
  parms->retcode          = -1;
  parms->idispatch        = NULL;

  LVC_DispatcherLock cs;                   
  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if (NULL != HashPtr) 
  {
    HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
    currSessEntry = HashPtr->NextEntry();
    while ( currSessEntry )
    {
      /* only Interfaces, no Factories, no IDebug, no IID_IliveCacheSpec Interface */
      if ((LVC_DispatcherHashEntry::ENTRY_IID == currSessEntry->getType()) &&
        (IID_IliveCacheSpec != *(currSessEntry->getGuid())))
      {
        /* get IUnknown for IID_IliveCacheSpec Interface */
        /* PTS 1109279 new call */
        hr = dispQuerySpecInterface( currSessEntry->getIUnknown()
          , &lpIUnknown
          , "eo38VDcomMonOpen:QueryInterface : unhandled exception" ); 
        if (SUCCEEDED(hr))
        {
          newSessEntry = HashPtr->getFreeEntry();
          newSessEntry->init( currSessEntry->getSession(), 
            currSessEntry->getGuid(), 
            lpIUnknown, 
            currSessEntry->getRemoteDbgFlag(),
            currSessEntry->getType());
          newSessEntry->setNextEntry((LVC_DispatcherHashEntryPtr)parms->session_handle);
          parms->session_handle = (tsp00_Addr)newSessEntry;
        }
      }
      currSessEntry = HashPtr->NextEntry();
    }
  }
  cs.Leave();                                     
  if ( NULL == parms->session_handle )
  {
    parms->idispatch  = NULL;
    parms->retcode    = -1;
    vgetpid(taskid);
    vendexcl(taskid, g08ComponentEnd );
  }
  else
  {
    tsp00_Uint4  session = newSessEntry->getSession();
    newSessEntry = REINTERPRET_CAST(LVC_DispatcherHashEntryPtr,parms->session_handle);
    parms->idispatch = REINTERPRET_CAST(char *, newSessEntry->getIUnknown());
    memcpy (&parms->iid[0], newSessEntry->getGuid(), sizeof (parms->iid));
    memcpy(&parms->session[0], &session, sizeof(parms->session)); 
    parms->retcode = S_OK;
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispMonitorNextWrapper(tsp_dcom_monitor_parms * parms)
{
  LVC_DispatcherHashEntryPtr           currSessEntry, newSessEntry;
  LVC_DispatcherCOMClassHashPtr         HashPtr;
  tsp00_TaskId            taskid;

  parms->retcode = S_OK;

  if ( NULL == parms->session_handle )
  {
    parms->idispatch  = NULL;
    parms->retcode    = -1;
    return;
  }

  currSessEntry = REINTERPRET_CAST(LVC_DispatcherHashEntryPtr,parms->session_handle);
  if (parms->idispatch == REINTERPRET_CAST(char *, currSessEntry->getIUnknown()))
  {
    dispRelease( REINTERPRET_CAST(IUnknown *, parms->idispatch)
      , "eo38VDcomMonNext:Release : unhandled exception" );

    newSessEntry  = currSessEntry->getNextEntry();

    /* re-chain to free chain */
    LVC_DispatcherLock cs;                   
    HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
    HashPtr->chainFreeEntry(currSessEntry);
    cs.Leave();                           

    if ( newSessEntry != NULL )
    {
      parms->session_handle = (tsp00_Addr)newSessEntry;
      tsp00_Uint4  session = newSessEntry->getSession();
      newSessEntry = REINTERPRET_CAST(LVC_DispatcherHashEntryPtr,parms->session_handle);
      parms->idispatch = REINTERPRET_CAST(char *, newSessEntry->getIUnknown());
      memcpy (&parms->iid[0], newSessEntry->getGuid(), sizeof (parms->iid));
      memcpy(&parms->session[0], &session, sizeof(parms->session)); 
    }
    else
    {
      parms->session_handle = NULL;
      parms->idispatch  = NULL;
      parms->retcode    = -1;
      vgetpid(taskid);
      vendexcl(taskid, g08ComponentEnd );
      return;
    }
  }
  else
  {
    MSGD ((ERR_DCOM_DUMP_INFO,"eo38VDcomMonNext: parameter incorrect (heap not cleared)"));
    parms->retcode = -1;
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispMonitorCloseWrapper(tsp_dcom_monitor_parms * parms)
{
  LVC_DispatcherHashEntryPtr           currSessEntry, newSessEntry;
  LVC_DispatcherCOMClassHashPtr         HashPtr;
  tsp00_TaskId            taskid;

  parms->retcode = S_OK;

  if (NULL == parms->session_handle ) return;

  currSessEntry = REINTERPRET_CAST(LVC_DispatcherHashEntryPtr,parms->session_handle);
  while (parms->idispatch == REINTERPRET_CAST(char *, currSessEntry->getIUnknown()))
  {
    dispRelease( REINTERPRET_CAST(IUnknown *, parms->idispatch)
      , "eo38VDcomMonClose:Release : unhandled exception" );

    newSessEntry  = currSessEntry->getNextEntry();

    /* re-chain to free chain */
    LVC_DispatcherLock cs;                   
    HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
    HashPtr->chainFreeEntry(currSessEntry);
    cs.Leave();                           
    currSessEntry = newSessEntry;
  }
  parms->session_handle = NULL;
  parms->idispatch = 0; 

  vgetpid(taskid);
  vendexcl(taskid, g08ComponentEnd );
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispCreateInstance(tsp_dcom_co_create_instance_parms *parms)
{
  LPCLASSFACTORY lpIClassFactory;
  IUnknown *lpIUnknown;
  IliveCacheSpec *pILC;
  tsp00_Uint4 session;
  IID   IId;

  memcpy(&IId, &parms->iid[0], sizeof(IId));   /* PTS 1109371 */
  memcpy (&session, &parms->session[0], sizeof (session));
  lpIClassFactory = (IClassFactory *) parms->ifactory;

  parms->ret = dispCreateInstanceLL(&IId, lpIClassFactory, (void **)&(parms->idispatch));

  if (SUCCEEDED (parms->ret))
  {
    lpIUnknown = (IUnknown *) parms->idispatch;
    parms->ret = lpIUnknown->QueryInterface ( IID_IliveCacheSpec,(void **) &pILC );
    if (! SUCCEEDED (parms->ret)) {
      MSGD ((ERR_DCOM_HRESULT, "dispCreateInstance:QI(ILCSpec)", parms->ret ));
    }
    else {
      tsp_dcom_dispatch_parms disp_parms;
      disp_parms.idispatch = parms->idispatch;
      disp_parms.ifactory = parms->ifactory;
      disp_parms.dispid = -2;
      disp_parms.session_context = (char*)((CIliveCacheSink*)parms->ikernel_sink)->lpSessionContext;
      ((CIliveCacheSink*)parms->ikernel_sink)->CallbackIsActive = false;

      try  {
        parms->ret = pILC->Advise ( (IUnknown *) parms->ikernel_sink);
        parms->ret = pILC->sysMonitorInit (); /* +++ Change Request 1000139 +++ */
      }
      catch ( BAD_ALLOC_GEO573&) {  
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
        if (pCBInterface){
          pCBInterface->dbpCaughtBadAlloc();
        }

        pILC->Release();						/* destroy instance */
        pILC->Release();
        parms->ret = GEO00D_DCOM_BAD_ALLOC;                   
        MSGD ((ERR_DCOM_HRESULT, "dispCreateInstance:Advise", parms->ret ));
      }                                               

      if (!parms->ret) {

        /* memcpy (&session, &parms->session[0], sizeof (session));  PTS 1109371 */
        parms->ret = dispComCache(REINTERPRET_CAST(IID *, & (parms->iid)),
          parms->idispatch,
          session,
          LVC_DispatcherHashEntry::ENTRY_IID);
        if ( parms->ret ) {
          MSGD ((ERR_DCOM_HRESULT, "CreateInstance:dispComCache", parms->ret ));
          /* +++      PTS 1109371      +++ */
          LVC_DispatcherHashEntry        currEntry;

          currEntry.init( 0
            , REINTERPRET_CAST(IID *, & (parms->iid))
            , (LPUNKNOWN)parms->idispatch
            , false
            , LVC_DispatcherHashEntry::ENTRY_IID);
          dispReleaseInstanceLL(&currEntry, (char*)&parms->session[0]);
          /* ---      PTS 1109371      --- */
        } 
        else {
          if (memcmp (&parms->iid[0], &IID_IliveCacheSpec, sizeof (parms->iid)))
            parms->ret = dispRegister(pILC, &disp_parms);
          if ( parms->ret ) {
            MSGD ((ERR_DCOM_HRESULT, "CreateInstance:Register", parms->ret ));
          }
        }
        if ( parms->ret ) {
          dispDisposeInstances(session, parms->session_context, &disp_parms);
        }
        IID     loc_iid;                                          /* PTS 1112617 */
        TASK_CTRL pTaskCtrl = CURR_TASK;                          /* PTS 1112617 */
        memcpy (&loc_iid.Data1, &(parms->iid), sizeof (loc_iid)); /* PTS 1112617 */
        MSGCD ((INFO_DCOM_CREATE_INSTANCE,                        /* PTS 1112617 */
          loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,            /* PTS 1112617 */
          loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],   /* PTS 1112617 */
          loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],   /* PTS 1112617 */
          loc_iid.Data4[6], loc_iid.Data4[7],                     /* PTS 1112617 */ 
          LVC_InProcDispatcherThreadIndex(pTaskCtrl)));           /* PTS 1112617 */ 
        pILC->Release ();
      }
    }
  }
}

/*---------------------------------------------------------------------------*/

bool LVC_InProcDispatcher::dispInitComCacheStatic()
{
  return Instance().dispInitComCache();
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispSetupRoutine(IliveCacheSink *sink, 
  const tsp00_KnlIdentifier &methodName,
  const tsp00_KnlIdentifier &methodNameAscii, void *setupParams)
{
  TASK_CTRL pTaskCtrl = CURR_TASK;                  // PTS 1106969
  TASK_STATE(pTaskCtrl) = TSK_DCOM_OBJ_CALLED_EO00; // PTS 1106969 
  /* +++ initialize statistic record                   PTS 1106969  */
  // use ASCII name for task monitoring
  memcpy(pTaskCtrl->TaskOMSStat.ProcedurName, &methodNameAscii, sizeof(tsp00_KnlIdentifier));
  pTaskCtrl->TaskOMSStat.CallbackCount      = 0;
  pTaskCtrl->TaskOMSStat.MethodIdx          = -1;
  pTaskCtrl->TaskOMSStat.OMSCallActive      = 0;
  /* --- initialize statistic record                   PTS 1106969  */
  /* +++ initialize CIliveCacheSink                    PTS 1106969  */
  CIliveCacheSink*  pIliveCacheSink = reinterpret_cast<CIliveCacheSink*>(sink);

  pIliveCacheSink->lpTaskContext = pTaskCtrl;
  tsp92_RteTaskDetailsInfo *taskInfo = reinterpret_cast<tsp92_RteTaskDetailsInfo*>(setupParams);
  vGetRteInfoTaskDetails (pIliveCacheSink->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00, taskInfo);
  pIliveCacheSink->lpCallTaskInfo = taskInfo;
  /* --- initialize CIliveCacheSink                    PTS 1106969  */
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispCleanupRoutine(IliveCacheSink *sink, void *setupParams)
{
  /*--* --- FF Change Request 1000139  --- *---------------------------------*/
  TASK_CTRL pTaskCtrl = CURR_TASK;                  // PTS 1106969
  TASK_STATE(pTaskCtrl) = TSK_RUNNING_EO00; // PTS 1106969
  /* +++ re-initialize statistic record           PTS 1106969  */
  pTaskCtrl->TaskOMSStat.ProcedurName[0]  = '\0';
  pTaskCtrl->TaskOMSStat.CallbackCount    = 0;
  pTaskCtrl->TaskOMSStat.MethodIdx        = -1;
  pTaskCtrl->TaskOMSStat.OMSCallActive    = 0;
  /* --- re-initialize statistic record           PTS 1106969  */
  reinterpret_cast<CIliveCacheSink*>(sink)->lpCallTaskInfo = 0;
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispGetClock(tsp00_Int4 &sec, tsp00_Int4 &msec)
{
  vclock(&sec, &msec);
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispAbort()
{
  vabort(false);
}

/*---------------------------------------------------------------------------*/

#ifdef WIN32
#include "hos38.h" /* nocheck */

void LVC_InProcDispatcher::dispDumpWin32Exception(struct _EXCEPTION_POINTERS *einfo)
{
  eo38_Dump_Exception(einfo);
}
#endif
/*---------------------------------------------------------------------------*/


int LVC_InProcDispatcher::dispEvalException(HRESULT &hresult, void *userData)
{
  tsp_dcom_dispatch_parms *parms = (tsp_dcom_dispatch_parms*) userData;
  parms->hresult = hresult;
  int retval = eo38_Eval_Exception(parms);
  hresult = parms->hresult;
  return retval;
}


void LVC_InProcDispatcher::dispReleaseInstanceLL(LVC_DispatcherHashEntry *currEntry, tsp00_Addr SessionContext)
{
  tsp_dcom_dispatch_parms   disp_parms;
  tsp00_Uint4               cnt;
  IUnknown                  *lpIUnknown;
  HRESULT                   hr;
  IliveCacheSpec            *pILC;
  tsp00_TaskId              taskid;

  disp_parms.idispatch  = REINTERPRET_CAST(char *,(currEntry->getIUnknown()));
  disp_parms.ifactory   = REINTERPRET_CAST(char *,(currEntry->getIUnknown()));
  disp_parms.dispid     = 2;
  disp_parms.session_context = SessionContext;
  try
  {
    IID     loc_iid;                                                  /* PTS 1112617 */
    TASK_CTRL pTaskCtrl = CURR_TASK;                                  /* PTS 1112617 */
    memcpy (&loc_iid.Data1, currEntry->getGuid(), sizeof (loc_iid));  /* PTS 1112617 */
    MSGCD ((INFO_DCOM_RELEASE_INSTANCE,                               /* PTS 1112617 */
      loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,                    /* PTS 1112617 */
      loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],           /* PTS 1112617 */
      loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],           /* PTS 1112617 */
      loc_iid.Data4[6], loc_iid.Data4[7],                             /* PTS 1112617 */ 
      LVC_InProcDispatcherThreadIndex(pTaskCtrl)));                                   /* PTS 1112617 */ 
    lpIUnknown = currEntry->getIUnknown();
    hr = lpIUnknown->QueryInterface ( IID_IliveCacheSpec, (void **) &pILC);
    if (SUCCEEDED (hr))
    {
      hr = pILC->Unadvise();
      if ( FAILED(hr))  
        MSGD ((ERR_DCOM_HRESULT, "dispReleaseInstanceLL:Unadvise", hr ));
      pILC->Release();
    }
    else
      MSGD ((ERR_DCOM_HRESULT, "dispReleaseInstanceLL:QILC", hr ));
    /* enter g08ComponentEnd region to make sure, that no OMS Monitor Iterator */
    /* is using the instance.                                                  */
    /* reason: instances are destructed via reference counting (0=delete this) */
    /* During destruction memory is passed back to a session heap, which is    */
    /* destructed after leaving the actual running routine ! Therefore it      */
    /* must be made sure, that the following Release() provokes destruction    */
    /* of the instance                                                         */
    vgetpid(taskid);
    vbegexcl(taskid, g08ComponentEnd );
    cnt = lpIUnknown->Release ();
    vendexcl(taskid, g08ComponentEnd );
  }
  catch (RTE_DBProcError&) {
    disp_parms.hresult = GEO00D_DCOM_EXCEPTION_RAISED;
    MSGD ((ERR_DCOM_HRESULT, "dispReleaseInstanceLL:Release", disp_parms.hresult ));
  }
  catch (DbpError &e) { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    char    msgBuffer[132];
    sp77sprintf(&msgBuffer[0], sizeof(msgBuffer), "dispReleaseInstanceLL:Release: DbpError:%d (%s)", 
      e.dbpError(), e.m_errorText );
    MSGD ((ERR_DCOM_DUMP_INFO, msgBuffer));
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispDestroySink(IliveCacheSink *sink)
{
  reinterpret_cast<CIliveCacheSink*>(sink)->Destruct();
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::SetUDEServer(IliveCacheSink* pSink, bool inproc)
{
  reinterpret_cast<CIliveCacheSink*>(pSink)->m_inproc = inproc;
}

/*---------------------------------------------------------------------------*/

bool LVC_InProcDispatcher::UseUDEServer(IliveCacheSink* pSink) const
{
  return !reinterpret_cast<CIliveCacheSink*>(pSink)->m_inproc;
}

/*---------------------------------------------------------------------------*/

LVC_ProcServerExtender* LVC_InProcDispatcher::GetUDEServerExtender(IliveCacheSink* pSink) const
{
  return reinterpret_cast<CIliveCacheSink*>(pSink)->m_pProcServerExtender;
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::SetUDEServerExtender(IliveCacheSink* pSink, LVC_ProcServerExtender* pExtender)
{
  reinterpret_cast<CIliveCacheSink*>(pSink)->m_pProcServerExtender = pExtender;
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispCreateSink(void *addr, size_t size, void *acvaddr)
{
  // Read parameter whether routine should be executed inproc or in a UDE-server
  bool inproc;
  RTEConf_Parameter::String value;
  SAPDBErr_MessageList      errList;
  if (!RTEConf_ParameterAccess::Instance()->GetString((RTEConf_Parameter::Name)"OMS_RUN_IN_UDE_SERVER", value, errList)){
    // ERROR
    inproc = true;
  }
  else {
    if (!strcmp((const char*)value, "YES")){
      inproc = false;
    }
    else {
      inproc = true;
    }
  }

  tak_all_command_glob *acv = reinterpret_cast<tak_all_command_glob*>(acvaddr);

  if (sizeof (CIliveCacheSink) > size) {
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "LVC_InProcDispatcher::createLocalSink: "
      "sizeof(CIliveCacheSink) = %d > reserved space for Object %d, "
      "check tak_kernel_sink", sizeof(CIliveCacheSink), size);
    MSGD ((ERR_DCOM_DUMP_INFO, buf));
    vabort(false);   
  } 
  else {
    CIliveCacheSink *lpCliveCacheSink       = new(addr) CIliveCacheSink;
    lpCliveCacheSink->lpSessionContext      = acv;

    SetUDEServer(lpCliveCacheSink, inproc);
    SetUDEServerExtender(lpCliveCacheSink, NULL);
  }
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispReleaseInstanceWrapper(tsp_dcom_release_instance_parms *parms)
{
  dispReleaseInstance(REINTERPRET_CAST(CIliveCacheSink*, 
    &REINTERPRET_CAST(tak_all_command_glob*, parms->session_context)->a_kernel_sink),
    *reinterpret_cast<tsp00_Uint4*>(&parms->session), parms->session_context);
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispReleaseAllWrapper()
{
  dispReleaseAll();
}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispGetParamInfoWrapper(tsp_dcom_paraminfo_parms *parms)
{
  CHRESULT_EO00D     hr(parms->session,"sql37k_paraminfo");
  tsp1_packet_ptr    lpPacket;
  unsigned short     i;
  unsigned short     cArgs;
  LVC_DispParamNames ParmNames;
  BOOLEAN            is_function = 1;
  BOOLEAN            is_last_func = FALSE;
  LVC_DispatcherParams parm_glob(parms);

  /*                                                                   */
  /* Visual Basic uses as ProgID: '<Project>.<Class>'                  */
  /*            as Componentname: '<Class>'                            */
  /*            as Interfacename: '_<Class>'                           */
  /*               as Methodname: '<Method>'                           */
  /*                                                                   */
  /* <Project>.<Class> is used as ProgID to be found in the Registry   */
  /*                   from the ProgID the Components CLSID is found   */
  /*                   from the CLSID the TypeLibID is extracted       */
  /*                                                                   */
  /* Within the TypeLib the CLSID is used to localize the Component    */
  /* description.                                                      */
  /* It is assumed, that the Component has ONLY ONE Interface.         */
  /* Then this Interface is searched for the <Method>.                 */
  /*                                                                   */

  if (NULL == parms->typeinfo_ptr)
    hr = parm_glob.GetInterface (parms, &cArgs);
  else
    hr = parm_glob.GetMethod(parms, &cArgs, is_last_func);

  if (ERROR_SUCCESS == hr.hresult())
  {
    for (i = 1; (i < (cArgs + 1)) && (hr.succeeded()); i++)
    {
      hr = parm_glob.OneParameter(parms, i);
    }
  }
  if (ERROR_SUCCESS == hr.hresult())
  {
    /* +++           PTS 1110591                                          +++ */
    tsp1_part_ptr  partPtr = REINTERPRET_CAST(tsp1_part_ptr,parm_glob.GetPartHeader()); 
    if (( s26partlen(*partPtr) - (int)sizeof (partPtr->sp1p_part_header()) ) > 
      partPtr->sp1p_buf_size() )
    {
      parm_glob.SetError(GEO00D_DCOM_BUFFER_LIMIT);
    }
    else
    {
      /* ---           PTS 1110591                                        --- */
      s26finish_part ( REINTERPRET_CAST(tsp1_packet_ptr,parms->packet),
        *REINTERPRET_CAST(tsp1_part_ptr,parm_glob.GetPartHeader()) );
      parm_glob.DescribeParamNames(ParmNames, cArgs);
      lpPacket = REINTERPRET_CAST(tsp1_packet_ptr,parms->packet);
      lpPacket->sp1_header.sp1h_varpart_len =   sizeof(lpPacket->sp1_header) +
        lpPacket->sp1_segm().sp1s_segm_header().sp1s_segm_len();
    }
  }
  else
    parm_glob.SetError(hr.hresult());

  if (is_last_func || ( ERROR_SUCCESS != hr.hresult()) ||
    (parms->db_obj_type == dbo_dbproc ) ||
    (dbo_dbfunc == parms->db_obj_type) || (dbo_package == parms->db_obj_type))
  {
    if ( NULL != parms->typeinfo_ptr )
    {
      bool ok;
      char errtext[64];
      LVC_FreeLibrary((HANDLE)parms->typeinfo_ptr, &ok, errtext, sizeof(errtext));
      parms->typeinfo_ptr = NULL;
    }
  }

}

/*---------------------------------------------------------------------------*/

void LVC_InProcDispatcher::dispGetClassObjectWrapper(tsp_dcom_co_create_instance_parms *parms)
{
  parms->idispatch = 0;
  parms->ret = dispGetClassObject(parms->library_name, parms->package_id,
    *reinterpret_cast<GUID*>(&parms->coclsid[0]), 
    parms->MD5_footprint, parms->ifactory);
}

/*---------------------------------------------------------------------------*/

bool LVC_InProcDispatcher_dispInitComCache(void)
{
  return LVC_InProcDispatcher::dispInitComCacheStatic();
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/

