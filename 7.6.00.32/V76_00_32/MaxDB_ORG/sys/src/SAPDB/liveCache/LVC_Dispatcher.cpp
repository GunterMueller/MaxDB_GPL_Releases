/*!
 * \file    LVC_Dispatcher.cpp
 * \author  MarkusSi, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher interface.
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

#define INITGUID
#include "liveCache/LVC_Dispatcher.hpp"
#undef INITGUID

#include "liveCache/LVC_LoadComponentLibrary.hpp"
#include "RunTime/System/RTESys_SharedLib.h"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_DbpError.hpp"
#include <string.h>
#include "Oms/OMS_Defines.h"

#include "hsp77.h"
#include "geo00d.h"

#include "heo01.h"
#include "geo007_1.h"  
#include "vak001.h"
#include "geo38.h"
#include "heo38.h"
#include "geo573.h"
#include "hak263.h"
#include "hak341.h"
#include "hgg01_3.h"
#include "liveCache/LVC_ProcServerExtender.hpp"

#if defined (BIT64)
#define   MIN_PARAMETER_SIZE  8
#else
#define   MIN_PARAMETER_SIZE  4
#endif

/*---------------------------------------------------------------------------*/

// definition of VTABLE structure (platform-specific)
#include "liveCache/LVC_DispatcherVTable.hpp"

/// System callback dispatch ID in liveCache sink (e.g., transaction end).
#define LVC_DISP_SYS_ID 5

/*---------------------------------------------------------------------------*/

/* struct describing 'stack' passed to transend */
struct LVC_DispatcherTransEndVariant
{
  union 
  {
    PVOID   byref;
    tsp00_Int4    *plVal;
  };
#if !defined(BIT64)
  tsp00_Int4  reserved1;
#endif
  short         vt;
  unsigned char io_flag;
  unsigned char ptr_flag;
  tsp00_Int4          len;
};

struct LVC_DispatcherTransEndDesc {
  LVC_DispatcherTransEndVariant lpbuflen;
  LVC_DispatcherTransEndVariant lpbuf;
};

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispSysCleanup(
  IliveCacheSpec *pILC, void *parms)
{
  HRESULT hr;
  try {
    hr = pILC->sysCleanUp();
  }
  catch (RTE_DBProcError&) {
    hr = GEO00D_DCOM_EXCEPTION_RAISED;
  }
  catch (DbpError e) { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }


    if (e.DbpProvokedByTestKernel()){
      // Only for malfunction test. Reset error, so that no abap-shortdump occurs.
      reinterpret_cast<tsp_dcom_dispatch_parms*>(parms)->hresult = GEO00D_DCOM_TERMINATED;
      hr = GEO00D_DCOM_TERMINATED;
    }
    else {
      dispEvalException(hr, parms);
      hr = GEO00D_DCOM_DBPROC_CRASHED; 
    }
  }

  return hr;
};

/*---------------------------------------------------------------------------*/

bool LVC_Dispatcher::dispInitComCacheLL()
{
  bool rc = true;
  tsp00_Uint4  lRC;
  LVC_DispatcherCOMClassHashPtr pMem;

  if (LVC_Dispatcher::getDCOMMemory().lphead == 0)
  {
    pMem = new (LVC_DispatcherCOMClassHash);
    if ( NULL == pMem )
    {
      MSGD ((ERR_DCOM_NOMEM, "dispInitComCache" ));
      rc = false;
    }
    else
    {
#if defined(DEBUG_MEM_ALLOC)
      MSGD(( 11999, INFO_TYPE, "new   ", "LVC_Dispatcher.cpp:dispInitComCache - 0x%lx, sizeInBytes %ld",
        pMem, sizeof(LVC_DispatcherCOMClassHash) ));
#endif /* DEBUG_MEM_ALLOC */
      lRC = pMem->HashInit( LVC_COM_HASH_SESS_HDR_SIZE );
      if ( 0 != lRC )
      {
        MSGD ((ERR_DCOM_NOMEM, "dispInitComCache:HashInit" ));
        rc = false;
      }
      else
      {
        LVC_Dispatcher::getDCOMMemory().lphead = pMem;
      }
    }
  }
  return (rc);
}

/*---------------------------------------------------------------------------*/

bool LVC_Dispatcher::dispInitComCacheLocked()
{
  LVC_DispatcherLock cs;
  return dispInitComCacheLL();
}

/*---------------------------------------------------------------------------*/

bool LVC_Dispatcher::dispInitComCache()
{
  bool b_ret;
  try {
    b_ret = dispInitComCacheLocked();
  }
  catch (DbpError &e)
  {
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    b_ret = 0;
  };
  return b_ret;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispComCache(
  GUID *lpGuid,
  void *lpInterface, 
  tsp00_Uint4 session,
  LVC_DispatcherHashEntry::HashEntryType hType)
{
  LVC_DispatcherCOMClassHashPtr             HashPtr;
  LVC_DispatcherHashEntryPtr  currSessPtr;

  LVC_DispatcherLock cs;                   /* PTS 1109279 */
  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if (NULL == HashPtr) {
    dispInitComCacheLL();
    HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
    if (NULL == HashPtr)
      return -1;
  }
  currSessPtr = HashPtr->getFreeEntry();
  if (NULL == currSessPtr)
    return -1;

  currSessPtr->init(session, lpGuid, (LPUNKNOWN)lpInterface, false, hType);
  HashPtr->chainEntry(currSessPtr);
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispGetClassObjectFromReg(
  GUID          &CoClsId,
  tsp00_Addr    &ifactory)
{
  // load COM object from registry (not supported and not needed by default)
  ifactory = NULL;
  return -1;
}

/*---------------------------------------------------------------------------*/
/* PTS 1121178 FF 19-Mar-2003 */
HRESULT LVC_Dispatcher::dispGetClassObject(
  char const      *libname, 
  const tsp00_C8  &packageId,
  GUID            &CoClsId,
  const tsp00_C32 &MD5mark, 
  tsp00_Addr &ifactory)
{
  HRESULT         ret;
  LPCLASSFACTORY  lpIClassFactory;

  ret = LVC_LoadComponentLibrary(libname, packageId, CoClsId, MD5mark, ifactory);
  lpIClassFactory = (IClassFactory *) ifactory;
  if (!ret)
  {
    if (!ret)
      ret = lpIClassFactory->LockServer (TRUE);
    if (ret) 
    {
      MSGD ((ERR_DCOM_HRESULT, "LockServer", ret ));
      MSGD ((ERR_DCOM_GUID, "COCLSID",
        CoClsId.Data1, CoClsId.Data2, CoClsId.Data3,
        CoClsId.Data4[0], CoClsId.Data4[1], CoClsId.Data4[2],
        CoClsId.Data4[3], CoClsId.Data4[4], CoClsId.Data4[5],
        CoClsId.Data4[6], CoClsId.Data4[7] ));
    }
    ret = dispComCache(&CoClsId, ifactory, 0, LVC_DispatcherHashEntry::ENTRY_CLSID);
  }
  else
  {
    char cClsContext[80];    
    memset(cClsContext,'\0',sizeof(cClsContext));
    strcpy(cClsContext, "INPROC");
    MSGD ((ERR_DCOM_COCREATE, "dispGetClassObject", ret, cClsContext ));
    MSGD ((ERR_DCOM_GUID, "COCLSID",
      CoClsId.Data1, CoClsId.Data2, CoClsId.Data3,
      CoClsId.Data4[0], CoClsId.Data4[1], CoClsId.Data4[2],
      CoClsId.Data4[3], CoClsId.Data4[4], CoClsId.Data4[5],
      CoClsId.Data4[6], CoClsId.Data4[7] ));
  }
  return ret;

};

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispSetupRoutine(IliveCacheSink *sink, 
  const tsp00_KnlIdentifier &methodName,
  const tsp00_KnlIdentifier &methodNameAscii, void *setupParms)
{
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispCleanupRoutine(IliveCacheSink *sink, void *setupParms)
{
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispGetDispAddr(const GUID &guid_coclsid, 
  const IID &guid_iid, tsp00_Uint4 session, tsp00_Addr &idispatch,
  tsp00_Addr &ifactory, void *params)
{
  LVC_DispatcherCOMClassHashPtr HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if (NULL == HashPtr) 
    return RC_DISPADDR_FAILED;
  
  LVC_DispatcherLock cs;                    
  LVC_DispatcherHashEntryPtr currPtr = HashPtr->FindHashEntry( &guid_iid, session );
  if (NULL != currPtr)
  {
    idispatch = REINTERPRET_CAST(char *,(currPtr->getIUnknown())); 
    if ( currPtr->getType() == LVC_DispatcherHashEntry::ENTRY_Invalid_IID ) {
      return RC_DISPADDR_FAILED;
    } else {
      return RC_DISPADDR_OK;
    }
  }
  
  /* check if CLSID in cache ( DBPROC Factory-Pointer)   */
  currPtr = HashPtr->FindHashEntry( &guid_coclsid, 0 );
  if ( NULL != currPtr )
  {
    ifactory = REINTERPRET_CAST(char *,(currPtr->getIUnknown()));
    if ( currPtr->getType() == LVC_DispatcherHashEntry::ENTRY_Invalid_CLSID )
    {
      return RC_DISPADDR_INVALID_FOUND;
    }
    else
    {
      return RC_DISPADDR_FACTORY_FOUND;
    }
  }
  return RC_DISPADDR_FAILED;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispCreateInstanceLL(
  IID *interface_id,
  LPCLASSFACTORY factory,
  void **new_instance)
{
  HRESULT ret;
  try {
    ret = factory->CreateInstance(NULL, *interface_id, new_instance);
    if (ret == E_OUTOFMEMORY)
      ret = GEO00D_DCOM_BAD_ALLOC;
  } catch (BAD_ALLOC_GEO573&) { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtBadAlloc();
    }

    ret = GEO00D_DCOM_BAD_ALLOC;                   
    MSGD ((ERR_DCOM_HRESULT, "dispCreateInstanceLL:CI", ret ));
  } 
  return ret;
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispReleaseInstance(
  IliveCacheSink                  *sink, 
  tsp00_Uint4                     session,
  tsp00_Addr                      session_context)
{
  int                       j;
  bool                      EntryFound = true;
  LVC_DispatcherCOMClassHashPtr           HashPtr;
  LVC_DispatcherHashEntryPtr             currEntry;

  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if ( NULL == HashPtr ) return;

  j = 0;
  while (EntryFound)
  {
    EntryFound = false;
    {                                       
      LVC_DispatcherLock cs;                   
      HashPtr->ResetEntrySearch( session, LVC_DispatcherCOMClassHash::SearchEQ );
      currEntry = HashPtr->NextEntry();
    };                                     

    if (NULL != currEntry)
    {
      EntryFound = true;
      LVC_DispatcherHashEntry    localEntry = *currEntry;                        // PTS 1116802 
      // dispReleaseInstanceLL( currEntry, parms->session_context );     PTS 1116802
      LVC_DispatcherLock cs;                  
      HashPtr->ResetEntrySearch( session, LVC_DispatcherCOMClassHash::SearchEQ );
      currEntry = HashPtr->GetRechainFirstEntry(NULL);
      cs.Leave();                                                   // PTS 1116802
      dispReleaseInstanceLL(&localEntry, session_context);     // PTS 1116802
    }
  }

  dispDestroySink(sink);
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispReleaseAll()
{
  LPCLASSFACTORY        lpIClassFactory;
  tsp00_Uint4           cnt;
  LVC_DispatcherHashEntryPtr         currEntry;
  LVC_DispatcherCOMClassHash          *HashPtr;

  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if ( NULL == HashPtr ) 
    return;

  LVC_DispatcherLock cs;                   
  HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
  currEntry = HashPtr->GetRechainFirstEntry(NULL);  
  while ( NULL != currEntry)
  {
    if ( 0 == currEntry->getSession() )
    {
      lpIClassFactory = REINTERPRET_CAST(IClassFactory *,(currEntry->getIUnknown()));
      cnt = lpIClassFactory->Release ();
      cnt = lpIClassFactory->LockServer (FALSE);
    }
    currEntry = HashPtr->GetRechainFirstEntry(NULL);  
  }
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispInvalidate(const GUID &CoClsId, 
  const IID &interface_id)
{
  LVC_DispatcherHashEntryPtr  currEntry;
  LVC_DispatcherCOMClassHash  *HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;

  if ( NULL == HashPtr )
    return;

  LVC_DispatcherLock cs;       

  HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
  currEntry = HashPtr->NextEntry();  
  while ( NULL != currEntry)
  {
    if ((*(currEntry->getGuid())  == interface_id) ||
      (*(currEntry->getGuid())  == CoClsId))  
    {
      switch(currEntry->getType())
      {
      case LVC_DispatcherHashEntry::ENTRY_CLSID: 
        currEntry->setType(LVC_DispatcherHashEntry::ENTRY_Invalid_CLSID); 
        break;
      case LVC_DispatcherHashEntry::ENTRY_IID: 
        currEntry->setType(LVC_DispatcherHashEntry::ENTRY_Invalid_IID); 
        break;
      default:
        break;
      }
    }
    currEntry = HashPtr->NextEntry();  
  }
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispRegisterLL(IliveCacheSpec *pILC)
{
  HRESULT hr;
  try {
    hr = pILC->Register ();
  }
  catch (BAD_ALLOC_GEO573&) {  
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtBadAlloc();
    }

    hr = GEO00D_DCOM_BAD_ALLOC;                   
  }                                               
  catch (RTE_DBProcError) {
    hr = -1;
  }
  catch (DbpError &e) { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    char    msgBuffer[132];
    sp77sprintf(&msgBuffer[0],sizeof(msgBuffer),"dispRegisterLL: DbpError:%d (%s)", 
      e.dbpError(), e.m_errorText);
    MSGD ((ERR_DCOM_DUMP_INFO,msgBuffer));
    hr = ERR_DCOM_COMETHOD_CRASHED;
  }
  return hr;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispRegister(IliveCacheSpec *pILC, void *parms)
{
  HRESULT hr = dispRegisterLL(pILC);

  return hr;
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispDisposeInstances(
  tsp00_Uint4                     session,
  tsp00_Addr                      session_context,
  void                            *parms) 
{
  int                       j;
  bool                      EntryFound = true;
  LVC_DispatcherCOMClassHashPtr           HashPtr;
  LVC_DispatcherHashEntryPtr             currEntry;
  IUnknown                  *lpIUnknown;
  HRESULT                   hr;
  IliveCacheSpec            *pILC;

  MSGD ((ERR_DCOM_DUMP_INFO, "Drop all component objects active on session"));

  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if ( NULL == HashPtr ) {
    MSGD ((ERR_DCOM_DUMP_INFO, "dispDisposeInstances DCOM_HEAD == NULL"));
    dispAbort();
  };

  j = 0;
  while (EntryFound)
  {
    EntryFound = false;
    {                                       
      LVC_DispatcherLock cs;                   
      HashPtr->ResetEntrySearch( session, LVC_DispatcherCOMClassHash::SearchEQ );
      currEntry = HashPtr->NextEntry();
    };                                      

    if (NULL != currEntry)
    {
      EntryFound = true;

      lpIUnknown = currEntry->getIUnknown();
      /*if (currEntry->getType() != LVC_DispatcherHashEntry::ENTRY_DbgInterface)*/
      {
        hr = lpIUnknown->QueryInterface ( IID_IliveCacheSpec, (void **) &pILC);
        if (SUCCEEDED (hr))
        {
          hr = dispSysCleanup(pILC, parms);   /* PTS 1109279 */
          if ( FAILED(hr)) {  
            IID loc_iid;
            char    msgBuffer[132];

            MSGD ((ERR_DCOM_HRESULT, "dispDisposeInstances:dispSysCleanup", hr ));
            memcpy (&loc_iid.Data1, currEntry->getGuid(), sizeof (loc_iid));
            sp77sprintf(&msgBuffer[0], sizeof(msgBuffer), 
              "IID:{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X} hr:%d",
              loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
              loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
              loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
              loc_iid.Data4[6], loc_iid.Data4[7], hr);
            MSGD ((ERR_DCOM_DUMP_INFO, &msgBuffer[0]));
          }
          pILC->Release ();
        }
        else
          MSGD ((ERR_DCOM_HRESULT, "dispDisposeInstances:QILC", hr ));
      }

      LVC_DispatcherHashEntry    localEntry = *currEntry;                        // PTS 1116802 
      // dispReleaseInstanceLL( currEntry, parms->session_context );     PTS 1116802
      LVC_DispatcherLock cs;                   
      HashPtr->ResetEntrySearch( session, LVC_DispatcherCOMClassHash::SearchEQ );
      currEntry = HashPtr->GetRechainFirstEntry(NULL);
      cs.Leave();                                                   // PTS 1116802
      dispReleaseInstanceLL(&localEntry, session_context);   // PTS 1116802
    }
  }
  MSGD ((ERR_DCOM_DUMP_INFO, "Drop of component objects done"));
};

/*---------------------------------------------------------------------------*/

tsp00_Uint4 LVC_Dispatcher::dispComputeStackSize(tsp00_Uint param_num,
  tak_dbproc_stack_param_debug *param_data)
{
#if defined (HP_IA64)
  return param_num * sizeof(tak_dbproc_stack_param_debug);  // This should resemble what happens in 7.4.03 - hoping it still works in 7.5...
#endif
  tsp00_Uint4 param_stack_size = sizeof(void *);      /* size for this pointer */
  while (param_num > 0)
  {
    if ( '\00' == param_data->dbg_is_ptr )
    {
      /* C++ pushes 1&2 Bytes Types as words   */
      if ( param_data->dbg_length <= MIN_PARAMETER_SIZE ) {
        param_stack_size = param_stack_size + MIN_PARAMETER_SIZE;
      }
      else  
      {
#if defined(PA11) || defined(BIT64)
        /* align to double word */
        param_stack_size = ( param_stack_size + 7 ) & 0xFFFFFFF8;
#endif
#if defined(NMP)
        /* double is passed as 2 Fullwords 8 Byte aligend on NMP */
        if ( param_data->dbg_vt == csp1_vt_r8 ) {
          param_stack_size = (param_stack_size + 7) & 0xFFFFFFF8;
          param_stack_size = param_stack_size + 8;
        }
        else
#endif
          param_stack_size = param_stack_size + param_data->dbg_length;
      }
    }
    else {
#if defined(BIT64)
      param_stack_size = (param_stack_size + 7) & 0xFFFFFFF8;
#endif  // BIT64
      param_stack_size = param_stack_size + sizeof(void *);
    }
    param_num--;
    param_data++;
  }
  return param_stack_size;
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispComputeWin64Stack(tsp00_Uint4 param_num, 
  tak_dbproc_stack_param_debug *param_data,
  tak_dbproc_stack_param *param_stack, tak_dbproc_stack_param *fparam_stack, 
  int &len_ret, int &flen_ret)
{
  tsp00_Uint				len = 0;
  tsp00_Uint				flen = 0;
  while (param_num > 0)
  {
    /*
    * The first 8 floating point parameters go into the floating point
    * parameter stack.  This allows the assembly code to easily determine
    * which parameters are passed in floating point registers.
    */
    if ( param_data->dbg_vt == csp1_vt_r8 ) 
    {
      if (flen < cak260max_fparams)
      {
        /*
        * double precision floats are copied onto the floating point
        * parameter stack in their memory format.
        */
        fparam_stack[flen].r8() = param_data->dbg_value.r8();
        flen++;
      }
      else
      {
        MSGD ((ERR_DCOM_MAX_FPARAM, cak260max_fparams));
        dispAbort();
      }
    } /* end if double */
    else if ( param_data->dbg_vt == csp1_vt_r4 ) 
    {
      if (flen < cak260max_fparams)
      {
        /*
        * Single precision floats are converted to double-precision floats 
        * before placing them on the floating point parameter stack because 
        * the assembly code can't tell the difference between single and 
        * double precision.  This does not affect the target function.  The 
        * target function receives the parameter either as a floating point 
        * register or on the general parameter stack, where it is left 
        * in single precision memory format.
        */
        fparam_stack[flen].r8() = param_data->dbg_value.r4();
        flen++;
      }
      else
      {
        MSGD ((ERR_DCOM_MAX_FPARAM, cak260max_fparams));
        dispAbort();
      }
    } /* end else-if single */

    /*
    * All parameters (including floating point parameters) are placed
    * in order onto the general parameter stack.
    */
    if (len < cak260max_params)
    {
      param_stack[len].ptr() = param_data->dbg_value.ptr();
      len++;
      param_num--;
      param_data++;
    }
    else
    {
      MSGD ((ERR_DCOM_MAX_PARAM, cak260max_params));
      dispAbort();
    }
  } /* end for */

  /*
  * Convert len and flen to bytes.
  */
  len_ret = len << 3;
  flen_ret = flen << 3;
}

/*---------------------------------------------------------------------------*/

/*!
 * \param method_name_ascii method name.
 * \param param_num count of parameters to check.
 * \param param_data data of parameters to check.
 */
bool LVC_Dispatcher::dispCheckStack(const tsp00_KnlIdentifier &method_name_ascii,
  tsp00_Uint4 param_num, tak_dbproc_stack_param_debug *param_data)
{
  int errcnt = 0;
  for (tsp00_Uint4 i = 0; i < param_num; ++i) {
    if (param_data->dbg_is_ptr) {
      // check for not NULL
      if (param_data->dbg_value.ptr() == NULL) {
        // ERROR: found NULL value
        ++errcnt; 
        MSGD ((N(264), ERR_TYPE,_T("DCOM    "),_T("%PARAM #%d of %.64s ERROR: NULL pointer"),
          i, &method_name_ascii));
        continue;
      }
      // check for ABAP table handle with size 0
      if (param_data->dbg_vt == csp1_vt_lc_abap_tab_handle ||
          param_data->dbg_vt == csp1_vt_lc_stream_handle) {
        // is ABAP table handle / stream handle
        ak263_StreamHandle *phandle = (ak263_StreamHandle*) param_data->dbg_value.ptr();
        if (phandle->tabHandle.rowSize <= 0 || phandle->tabHandle.colCount <= 0) {
          // ERROR: invalid handle size
          ++errcnt;
          MSGD ((N(264), ERR_TYPE,_T("DCOM    "),
            _T("PARAM #%d of %.64s ERROR: invalid rowSize %d or colCount %d for ABAP handle"),
            i, &method_name_ascii, phandle->tabHandle.rowSize, phandle->tabHandle.colCount));
        }
      }
    }
    ++param_data;
  }
  if (errcnt) {
    MSGD ((N(264), ERR_TYPE,_T("DCOM    "),
      _T("%d error(s) in DB procedure %.64s parameters, routine NOT started"),
      errcnt, &method_name_ascii));
  }
  return errcnt == 0;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispDispatch(IliveCacheSink               *sink, 
                                     IUnknown                     *disp_addr,
                                     const tsp00_KnlIdentifier    &method_name, 
                                     const tsp00_KnlIdentifier    &method_name_ascii, 
                                     tsp00_Int4                    dispid, 
                                     tsp00_Uint                    param_num,
                                     tak_dbproc_stack_param_debug *param_data, 
                                     bool                          monitor, 
                                     tsp00_Int2                    monitor_slot_id,
                                     void                         *parms, 
                                     void                         *setupParms)
{
  HRESULT         hresult = S_OK;

#if defined(LINUX) 
  //    vdebug_break(10001);
#endif
  dispSetupRoutine(sink, method_name, method_name_ascii, setupParms);

  /* calculate entry address of method to call      */
  IUnknown *lpClassPtr;
  // if dispid < 0, use sysRequest
  t_lpIFunc lp_IFunc = dispid >= 0 ? LVC_DispatcherVTableCompute(dispid, lpClassPtr, disp_addr) : NULL;
#if defined	(_WIN64) && defined (_M_IA64)
  // Get stack for WINDOWS IA64
  //Define verwenden
  tak_dbproc_stack_param	param_stack[cak260max_params];
  //Define verwenden
  tak_dbproc_stack_param	fparam_stack[cak260max_fparams];
  int				len = 0;
  int				flen = 0;
  dispComputeWin64Stack(param_num, param_data, param_stack, fparam_stack, len, flen);
#else
  // Get stack size for all platforms except WINDOWS IA64
  tsp00_Uint4 param_stack_size = dispComputeStackSize(param_num, param_data);
#endif

  /*--* +++ FF Change Request 1000139  +++ *---------------------------------*/
  IliveCacheSpec  *pILC = NULL; 
  hresult = disp_addr->QueryInterface (IID_IliveCacheSpec, (void **)&pILC);
  if (FAILED(hresult))
  {
    MSGD ((ERR_DCOM_HRESULT, "dispDispatch:IID_IliveCacheSpec", hresult));
    return hresult;
  }

  tsp00_Int4      start_time_sec; 
  tsp00_Int4      start_time_msec;
  if (monitor && pILC) {
    try 
    {
      pILC->sysMonitorProlog(monitor_slot_id, 
        reinterpret_cast<unsigned char*>(const_cast<char*>((const char*) &method_name)));
      dispGetClock(start_time_sec, start_time_msec);
    }
    catch (DbpError &e) 
    {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }

      char    msgBuffer[132];
      sp77sprintf(&msgBuffer[0],sizeof(msgBuffer),"dispDispatch:sysMonitorProlog: DbpError:%d (%s)", 
        e.dbpError(), e.m_errorText );
      MSGD ((ERR_DCOM_DUMP_INFO, msgBuffer));           /* PTS 1107168 */
    }
#ifndef WIN32
    // on Dec, sysMonitorProlog might throw bad_alloc as well.
    catch (BAD_ALLOC_GEO573&) {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtBadAlloc();
      }
      MSGD ((ERR_DCOM_DUMP_INFO,"dispDispatch:sysMonitorProlog : unhandled BAD_ALLOC exception"));
    }
    catch (...) {
      MSGD ((ERR_DCOM_DUMP_INFO,"dispDispatch:sysMonitorProlog : unhandled unknown exception"));
    }
#endif
  }

  /*--* --- FF Change Request 1000139  --- *---------------------------------*/
  try             
  {
    if (!lp_IFunc && pILC) {
      /*
      * Use special interface for special dispatcher IDs.
      * This is used to implement COMMIT/ROLLBACK in ProcServer.
      */
      pILC->sysRequest((int*) param_data[0].dbg_value.ptr(),
        (unsigned char*) param_data[1].dbg_value.ptr());
      pILC->Release();
    } 
    else {
      // Check whether session was set to corrupted during the last Transaction-End processing
      // in the liboms. As this Trans-End processing is done after the trans-end in the kernel
      // is finished, a direct reply of the failure is not possible.
      bool corrupt = false;
      int bufLen = 1;
      unsigned char buf[1];
      buf[0] = m_return_error;
      pILC->sysRequest(&bufLen, &buf[0]);
      if (buf[0] != 0){
        corrupt = true;
      }

      if (corrupt){
        MSGD ((ERR_DCOM_DUMP_INFO,"dispDispatch: Session was marked as corrupt by previous trans-end handling"));
        hresult = GEO00D_DCOM_DBPROC_CRASHED;                   
      }
      else if (dispCheckStack(method_name_ascii, param_num, param_data)) {
#if (defined(SUN) || defined(SOLARIS)) && defined(BIT64)
        /* PTS 1109493 */ /* round up to multiple of 16 bytes */
        param_stack_size = ( param_stack_size + 15 ) & (~15);
#endif
        /*
        * following has been coded in Assembler
        * to push the exact length of needed parameters
        */
#if defined (_WIN64) && defined (_M_IA64)
        sql38aVdcomCall (
          lpClassPtr,	       /* class this pointer */ 
          (PROC)lp_IFunc,    /* entry-point */
          param_stack,       /* call-stack */ 
          len,               /* call-stack length */
          fparam_stack,      /* call-stack (floats) */ 
          flen,              /* call-stack length (floats) */
          0);                /* param-stack size, is not needed in this case */
#elif defined(_WIN64) && defined(_M_AMD64)
		    sql38aVdcomCall(
		      (void *)lpClassPtr,
		      (void *)lp_IFunc,
		      (void *)param_data,
		      param_num * sizeof(tak_dbproc_stack_param_debug),
		      param_stack_size);
#else
        sql38aVdcomCall(lpClassPtr,  /* class this pointer*/ 
          (PROC)lp_IFunc,            /* entry-point       */
          param_data,                /* call-stack        */ 
          param_num * sizeof(tak_dbproc_stack_param_debug),    /* call-stack length */
          param_stack_size);         /* param-stack size  */
#endif
      } 
      else {
        // error in stack params
        sink->SetError (e_param_check, 45,
          (const unsigned char*) "Error in DB procedure parameters, see knldiag", csp_ascii);
        hresult = GEO00D_DCOM_EXCEPTION_RAISED;
      }
    }
  }
  catch (RTE_DBProcError e) {
    if (e_terminate == e.error_number) /* 1107692 */
      hresult = GEO00D_DCOM_TERMINATED;
    else
      hresult = GEO00D_DCOM_EXCEPTION_RAISED;
  } 
  catch (DbpError &e) { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    char    msgBuffer[132];
    sp77sprintf(&msgBuffer[0],sizeof(msgBuffer),"dispDispatch: DbpError:%d (%s)", 
      e.dbpError(), e.m_errorText );
    MSGD ((ERR_DCOM_DUMP_INFO, msgBuffer));
    
    dispEvalException(hresult, parms);

    hresult = GEO00D_DCOM_DBPROC_CRASHED;
  } 
  catch ( BAD_ALLOC_GEO573&) {  
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtBadAlloc();
    }

    MSGD ((ERR_DCOM_DUMP_INFO,"dispDispatch: STD::BAD_ALLOC caught"));
    hresult = GEO00D_DCOM_DBPROC_CRASHED;                   
  }                                               

  if (0 == hresult) {
    // make sure COM-routine allocator is empty
    ak341UserAllocatorCtl(OmsCallbackInterface::BlockCOMRoutine, NULL);
  }

  /*--* +++ FF Change Request 1000139  +++ *---------------------------------*/
  if (monitor && (0 == hresult) && pILC) {
    try
    {
      tsp00_Int4      stop_time_sec; 
      tsp00_Int4      stop_time_msec;
      dispGetClock(stop_time_sec, stop_time_msec);
      tsp00_Int4 diff_time_sec  = stop_time_sec - start_time_sec;
      tsp00_Int4 diff_time_msec = stop_time_msec - start_time_msec;
      if (stop_time_msec < start_time_msec)
      {
        diff_time_sec  = diff_time_sec - 1;
        diff_time_msec = diff_time_msec + 1000000 ;
      }
      pILC->sysMonitorEpilog(monitor_slot_id, diff_time_sec * 1000 + diff_time_msec / 1000);
      pILC->Release();
    }
    // BEGIN PTS 1117692
    catch (RTE_DBProcError e) {
      if (e_terminate == e.error_number) /* 1107692 */
        hresult = GEO00D_DCOM_TERMINATED;
      else
        hresult = GEO00D_DCOM_EXCEPTION_RAISED;
    }
    catch (DbpError e) { 
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }

      char    msgBuffer[132];
      sp77sprintf(&msgBuffer[0], sizeof(msgBuffer), "dispDispatch: DbpError:%d (%s)", 
        e.dbpError(), e.m_errorText );
      MSGD ((ERR_DCOM_DUMP_INFO,msgBuffer));

      dispEvalException(hresult, parms);

      hresult = GEO00D_DCOM_DBPROC_CRASHED; 
    }

    if (hresult != 0 && pILC){
      pILC->Release();
    }
    // END PTS 1117692
  }
  else 
  {
    /*
     * PTS 1116425
     * in case of hresult (GEO00D_DCOM_TERMINATED,...),
     * where sysMonitorEpilog isn't executed,
     * correct COM instance reference counting,
     * to make sure instance will be deleted by dispDisposeInstances()
     */
    if (monitor && pILC)
      pILC->Release();
  }

  // call cleanup
  dispCleanupRoutine(sink, setupParms);
  return hresult;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispTransEnd(
  IliveCacheSink *sink, 
  tsp00_Uint4 session,
  tsp00_Addr session_context,
  LVC_DispatcherTransEnd *param_stack,
  bool monitor,
  tsp00_Int2 monitor_slot_id,
  void *parms,
  void *setupParms,
  const tsp00_KnlIdentifier &methodNameAscii,
  const tsp00_KnlIdentifier &methodNameUni)
{

  const short      vt_ptr             = 26;
  bool             DebugSessionLocal  = false;
  bool             DebugSessionRemote = false;
  short            rc                 = 0;
  int                     i;
  short                   arrCnt = 0;
  IUnknown*               arrInterfaces[100];  
  LVC_DispatcherCOMClassHashPtr         HashPtr;
  LVC_DispatcherHashEntryPtr           currEntry;
  tsp00_Int4              l_buflen;
  struct _aligned_buf
  {
    union 
    {
      LVC_DispatcherTransEndBuf   l_buf;
      integer                     align;
    } v;
  } aligned_buf;

#if 1 // defined(_IBMR2) || defined(SUN) || defined(SOLARIS) || defined(PA11) || defined(PA20W) || defined(ALPHA) || defined(NMP) || defined(_WIN64) || (defined(LINUX) && (defined(IA64)||defined(S390X)))
  LVC_DispatcherTransEndDesc   l_param;
  l_param.lpbuflen.plVal       = &l_buflen;  
  l_param.lpbuflen.vt          = vt_ptr;  
  l_param.lpbuflen.io_flag     = 1;  
  l_param.lpbuflen.ptr_flag    = 1;  
  l_param.lpbuflen.len         = 4;  
  l_param.lpbuf.byref          = (void*)&aligned_buf.v.l_buf;
  l_param.lpbuf.vt             = vt_ptr;  
  l_param.lpbuf.io_flag        = 1;  
  l_param.lpbuf.ptr_flag       = 1;  
  l_param.lpbuf.len            = sizeof(LVC_DispatcherTransEndBufMsg);  
#else
  LVC_DispatcherTransEnd l_param;
  l_param.lpbuflen      = &l_buflen;  
  l_param.lpbuf         = &aligned_buf.v.l_buf;
#endif

  HRESULT hresult = 0;
  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if (NULL == HashPtr)
    return S_OK;

  LVC_DispatcherLock cs;                   
  HashPtr->ResetEntrySearch( session, LVC_DispatcherCOMClassHash::SearchEQ );
  currEntry = HashPtr->NextEntry();

  while ( NULL != currEntry)
  {
    if ((currEntry->getType() == LVC_DispatcherHashEntry::ENTRY_IID) ||
      (currEntry->getType() == LVC_DispatcherHashEntry::ENTRY_Invalid_IID))  
    {
      /* Ignore special Interface, which is activated by DDL (CREATE DBPROC) */
      if ( IID_IliveCacheSpec != *(currEntry->getGuid()))
      {
        arrInterfaces[arrCnt] = currEntry->getIUnknown();
        arrCnt++;
        break;       /* PTS 1120585 2003-02-13 FF */
      }
    }
    else
    {
      if (currEntry->getDebugFlag())
      {
        if (currEntry->getRemoteDbgFlag()) 
          DebugSessionRemote = true;
        else
          DebugSessionLocal  = true;
      }
    }
    currEntry = HashPtr->NextEntry();  
  }

  cs.Leave();                           

  HRESULT hr = 0;
  i = 0;                         
  if ( i < arrCnt )              
  {
    aligned_buf.v.l_buf.msg.mtype     = param_stack->lpbuf->msg.mtype;
    aligned_buf.v.l_buf.msg.mtype2    = param_stack->lpbuf->msg.mtype2;
    l_buflen            = sizeof(LVC_DispatcherTransEndBufMsg);
    IUnknown *disp_addr;
    hresult = arrInterfaces[i]->QueryInterface( IID_IliveCacheSpec, 
      REINTERPRET_CAST(void **,(&disp_addr)));
    if (SUCCEEDED(hresult)) {
      hresult = dispDispatch(sink, disp_addr, methodNameUni, methodNameAscii, LVC_DISP_SYS_ID, 2,
        reinterpret_cast<tak_dbproc_stack_param_debug*>(&l_param), monitor, monitor_slot_id,
        parms, setupParms);
      
      rc = aligned_buf.v.l_buf.err; 
      if ((0 != rc) || FAILED(hresult))
      {
        char dmp_buf[132];
        MSGD ((ERR_DCOM_HRESULT, "dispTransEnd: Error calling sysRequest", rc ));
        sp77sprintf(dmp_buf, sizeof(dmp_buf), "mtype: %X  mtype2: %X ", 
          (int)(param_stack->lpbuf->msg.mtype),
          (int)(param_stack->lpbuf->msg.mtype2)); 
        MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));
        hr = GEO00D_DCOM_TRANSEND_FAILED;       /* pts 1105922 FF 10-Mar-2000 */
        /*if ( !hr ) hr = hresult;                 pts 1105922                */
      }
      arrInterfaces[i]->Release();
    }
    else
    {
      MSGD ((ERR_DCOM_HRESULT, "dispTransEnd:QI(ILCSpec)", hresult ));
      hr = GEO00D_DCOM_TRANSEND_FAILED;       /* pts 1105922 FF 10-Mar-2000 */
    }
  }

  /*  if (( 0 != rc ) || FAILED(hr))  pts 1105922 */
  if ( 0 != hr )     /* pts 1105922 FF 10-Mar-2000 */
  {
    dispDisposeInstances(session, session_context, parms );
    hresult = GEO00D_DCOM_TRANSEND_FAILED; 
  }
  return hresult;
}

/*---------------------------------------------------------------------------*/

HRESULT LVC_Dispatcher::dispQuerySpecInterface(
  IUnknown *lpIUnknown, IUnknown **lplpIUnknown,
  const char* msg)
{
  HRESULT hr;
  try { 
    hr = lpIUnknown->QueryInterface(IID_IliveCacheSpec, 
      REINTERPRET_CAST(void **,lplpIUnknown) );
  }
  catch (DbpError &e)
  { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    hr = GEO00D_DCOM_EXCEPTION_RAISED;

    char msgBuffer[132];
    sp77sprintf(msgBuffer, sizeof(msgBuffer), "%s DbpError: %d (%s)", 
      msg, e.dbpError(), e.m_errorText);
    MSGD ((ERR_DCOM_DUMP_INFO, msgBuffer));
  };
  return hr;
}

/*---------------------------------------------------------------------------*/

void LVC_Dispatcher::dispRelease(IUnknown *lpIUnknown, const char *msg)
{
  try { 
    lpIUnknown->Release (); 
  } 
  catch (DbpError &e)
  { 
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    char msgBuffer[132];
    sp77sprintf(msgBuffer, sizeof(msgBuffer), "%s: DbpError: %d (%s)", 
      msg, e.dbpError(), e.m_errorText);
    MSGD ((ERR_DCOM_DUMP_INFO, msgBuffer));
  }
}

/*---------------------------------------------------------------------------*/

LVC_DispatcherDCOMMemory LVC_Dispatcher::m_dcomMemory;

/*---------------------------------------------------------------------------*/

LVC_DispatcherInfo::LVC_DispatcherInfo(RTESys_SharedLib *lib, const char *name,
    RTE_SharedLibProcAddress io, RTE_SharedLibProcAddress im, bool unicode)
  : m_lib(lib), m_introspectObject(io), m_introspectMethod(im), m_unicodeCapable(unicode)
{
  strcpy(m_name, name);
}

/*---------------------------------------------------------------------------*/

LVC_Dispatcher::LVC_Dispatcher(SAPDBMem_IRawAllocator &alloc)
  : m_alloc(alloc), m_instances(alloc)
{
}

/*---------------------------------------------------------------------------*/

const LVC_DispatcherInfo *LVC_Dispatcher::getImplementation(const char *fname)
{
  // look up in hash
  InfoHash::Iterator i(m_instances.Find(fname));
  if (i.IsValid()) {
    // ok, found
    return (*i).value;
  }
  // try to load shared library
  RTESys_SharedLib *lib = reinterpret_cast<RTESys_SharedLib*>(
    m_alloc.Allocate(sizeof(RTESys_SharedLib)));
  if (!lib) {
    return NULL;
  }
  SAPDB_Bool ok = false, unicode = true;
  SAPDBErr_MessageList msgList;
  new(lib) RTESys_SharedLib(fname, ok, msgList);
  if (!ok) {
    RTE_Message(msgList);
    lib->~RTESys_SharedLib();
    m_alloc.Deallocate(lib);
    return NULL;
  }
  // try to look up required introspection routines
  RTE_SharedLibProcAddress introspectObject, introspectMethod;
  ok = lib->GetProcAddress("Co_IntrospectObject", introspectObject, msgList);
  if (ok) {
    ok = lib->GetProcAddress("Co_IntrospectMethod_MO", introspectMethod, msgList);
    if (!ok) {
      // non-unicode capable
      ok = lib->GetProcAddress("Co_IntrospectMethod", introspectMethod, msgList);
      unicode = false;
    }
  }
  if (!ok) {
    RTE_Message(msgList);
    lib->~RTESys_SharedLib();
    m_alloc.Deallocate(lib);
    return NULL;
  }
  // library loaded, allocate and initialize info
  LVC_DispatcherInfo *info = reinterpret_cast<LVC_DispatcherInfo*>(
    m_alloc.Allocate(sizeof(LVC_DispatcherInfo) + strlen(fname)));
  if (!info) {
    lib->~RTESys_SharedLib();
    m_alloc.Deallocate(lib);
    return NULL;
  }
  // create dispatcher info and insert into hashtable
  new(info) LVC_DispatcherInfo(lib, fname, introspectObject, introspectMethod, unicode);
  i = m_instances.Insert(info->m_name, info);
  if (!i.IsValid()) {
    lib->~RTESys_SharedLib();
    m_alloc.Deallocate(lib);
    info->~LVC_DispatcherInfo();
    m_alloc.Deallocate(info);
    return NULL;
  }
  return info;
}

/*---------------------------------------------------------------------------*/
